//========================================================================================================================================
// TwsApi Test
//========================================================================================================================================
#include "TwsApiL0.h"
#include "TwsApiDefs.h"
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
	
using namespace TwsApi;
using namespace std;

bool ErrorForRequest		= false;
bool continueRequest        = true;
IBString account_str;
struct PortfolioPosition
{
	IBString symbol;
    int expiry;
	double strike;
	int quantity;
};

vector <PortfolioPosition> AllPositions;

//---------------------------------------------------------------------------------------------------------------------------------------
//                                                        MyEWrapper
//---------------------------------------------------------------------------------------------------------------------------------------
class MyEWrapper: public EWrapperL0
{
	public:

		// main EWrapper functino
		MyEWrapper( bool CalledFromThread = true ) : EWrapperL0( CalledFromThread ) {}

		virtual void position( const IBString& account, const Contract& contract, int position, double avgCost)
		{
            PortfolioPosition p;

			if (contract.secType == "STK" and account == account_str){
			
                p.symbol    = contract.symbol;
				p.expiry    = atoi(contract.expiry);
				p.strike    = contract.strike;
				p.quantity  = position;

				AllPositions.push_back(p);
            }
		}

		virtual void positionEnd()
		{
			continueRequest = false;
		}

		virtual void winError( const IBString& str, int lastError )
		{
			fprintf( stderr, "WinError: %d = %s\n", lastError, (const char*)str );
			ErrorForRequest = true;
		}

		virtual void error( const int id, const int errorCode, const IBString errorString )
		{
			fprintf( stderr, "Error for id=%d: %d = %s\n", id, errorCode, (const char*)errorString );
			ErrorForRequest = (id > 0);
			// id == -1 are 'system' messages, not for user requests
			// as a test, set year to 2010 in the reqHistoricalData
		}
};


//---------------------------------------------------------------------------------------------------------------------------------------
//																 MAIN
//---------------------------------------------------------------------------------------------------------------------------------------
int main( int argc, const char* argv[] )
{
	// stuff needed to connect to IB
	int socketNumber      = 4002;
	MyEWrapper	MW( false );
	EClientL0*	EC = EClientL0::New( &MW );
    account_str = argv[1];

	// make sure EC is able to connect and then get the data for all the strikes we care about
    while ( !EC->eConnect( "", socketNumber, 100 ) );

    EC->reqPositions();
    while(continueRequest) EC->checkMessages();
    EC->eDisconnect();
    delete EC;

    ofstream file;
    //file.open("portfolio.csv");
    // put all the portfolio info in the .csv file
    for (unsigned int i = 0; i < AllPositions.size(); i++)
        fprintf(stdout, "%s %d ", (const char*) AllPositions[i].symbol, AllPositions[i].quantity);
/*        file << AllPositions[i].symbol 
             << " , " << AllPositions[i].strike
             << " , " << AllPositions[i].quantity
             << " , " << AllPositions[i].expiry 
             << endl;

    file.close();
*/
    return ErrorForRequest;
}
