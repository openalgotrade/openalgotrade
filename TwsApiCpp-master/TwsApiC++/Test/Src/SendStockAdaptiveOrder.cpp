/* 
This program will send a stock order to the specified account using IBs built in
Adaptive algo. The default is setting your worst case senario to be the market order,
but will try to fill at better prices.

The program will not return until the order placed was cancelled (through TWS) or filled.
This could be modified to wait a certain amount of time and then cancel (or other more sophisticated functions).

@author: Justin Clark of Canopy
*/

#include "TwsApiL0.h"
#include "TwsApiDefs.h"
	
using namespace TwsApi;
using namespace std;

bool error_for_request = false;
OrderId next_id        = 0;
bool continue_order    = true;

void Wait(int seconds)
{
    time_t t1, t2;
    time(&t1);
    while (time(&t2), t2-t1 < seconds);
}

//---------------------------------------------------------------------------------------------------------------------------------------
//                                                        MyEWrapper
//---------------------------------------------------------------------------------------------------------------------------------------
class MyEWrapper: public EWrapperL0
{
	public:

		MyEWrapper( bool CalledFromThread = true ) : EWrapperL0( CalledFromThread ) {}

		virtual void orderStatus( OrderId orderId, const IBString& status, int filled, int remaining, 
				double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId, const IBString& whyHeld )
		{
			if (orderId == next_id and (status == *OrderStatus::Cancelled or status == *OrderStatus::Filled)) continue_order = false;
		}

		virtual void nextValidId( OrderId orderId )
		{
			if (next_id == 0) next_id = orderId;
		}

		virtual void winError( const IBString& str, int lastError )
		{
			fprintf( stderr, "WinError: %d = %s\n", lastError, (const char*)str );
			error_for_request = true;
		}

		virtual void error( const int id, const int errorCode, const IBString errorString )
		{
			fprintf( stderr, "Error for id=%d: %d = %s\n", id, errorCode, (const char*)errorString );
			error_for_request = (id > 0);
			// id == -1 are 'system' messages, not for user requests
			// as a test, set year to 2010 in the reqHistoricalData
		}

};


//---------------------------------------------------------------------------------------------------------------------------------------
//																 MAIN
//---------------------------------------------------------------------------------------------------------------------------------------
int main( int argc, const char* argv[] )
{
    if (argc != 4){
        fprintf(stderr, "Argument error: 1) symbol | 2) quantity ( sell(-) | buy(+)) | 3) account num\n");
        return -1;
    }
    
	double   quantity;
	quantity = atoi(argv[2]);
	Order    O;
	Contract C;

	// ------------------------ DEFINE CONTRACT --------------------------//
	C.symbol   = argv[1];
	C.secType  = *SecType::STK;
	C.currency = "USD";
	C.exchange = *Exchange::IB_SMART;

	// ------------------------- CONNECT TO IB ---------------------------//
	int socket_number = 4002;
	MyEWrapper	MW( true );
	EClientL0*	EC = EClientL0::New( &MW );
    while ( !EC->eConnect( "", socket_number, 100 ) );
    Wait(1);    

    // ------------------ SET ADAPTIVE ALGO PARAMETERS -------------------//
    if (quantity > 0)      O.action = "BUY";  // positive quantity means buy
    else if (quantity < 0) O.action = "SELL"; // negative quantity means sell
    O.algoStrategy                  = "Adaptive";
    O.algoParams.reset(new TagValueList());
    TagValueSPtr tag1(new TagValue("adaptivePriority", "Urgent"));
    O.algoParams->push_back(tag1);
    O.totalQuantity                 = abs(quantity);
    O.orderType                     = "MKT";
    O.account                       = argv[3];

    // -------------------------- PLACE ORDER ----------------------------//
    EC->placeOrder(next_id , C , O);
    while (continue_order);

    EC->eDisconnect();
    delete EC;
    return error_for_request;
}
