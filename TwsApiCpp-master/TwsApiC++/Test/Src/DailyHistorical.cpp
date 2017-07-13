//============================================================================
// This will get the previous 24 ticks (initially minute intervals)
// and print them to stdout to be used by the main python program
//============================================================================
#include "TwsApiL0.h"
#include "TwsApiDefs.h"
#include <iostream>

using namespace TwsApi;
using namespace std;

bool EndOfHistoricalData	= false;
bool ErrorForRequest		= false;
int dataCheck               = 0;

//----------------------------------------------------------------------------
// MyEWrapper
//----------------------------------------------------------------------------
class MyEWrapper: public EWrapperL0
{
	public:

		MyEWrapper( bool CalledFromThread = true ) : EWrapperL0( CalledFromThread ) {}

        virtual void tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute )
        {
            if (field < 10) {
                //printf("%d -- %f\n", field, price);
                dataCheck++; // only counts the basic tick types (for tickPrice)
            }
        }

        virtual void tickSize( TickerId tickerId, TickType field, int size )
        {
            if (field < 10) dataCheck++; // only counts basic tick types (for tickSize)
        }

		virtual void winError( const IBString& str, int lastError )
		{
		    fprintf( stderr, "WinError: %d = %s\n", lastError, (const char*)str );
			ErrorForRequest = true;
		}
		
		virtual void error( const int id, const int errorCode, const IBString errorString )
		{
			if (errorCode < 2000) fprintf( stderr, "Error for id=%d: %d = %s\n", id, errorCode, (const char*)errorString );
			ErrorForRequest = (id > 0);
			// id == -1 are 'system' messages, not for user requests
			// as a test, set year to 2010 in the reqHistoricalData
		}

		virtual void historicalData( TickerId reqId, const IBString& date, 
				double open, double high, double low, double close, int volume, int barCount, double WAP, int hasGaps )
		{
			if( IsEndOfHistoricalData(date) )
			{
				EndOfHistoricalData = true;
				return;
			}

			fprintf( stdout, "%5.3f\n", close );
		}

};

//----------------------------------------------------------------------------
// main
//----------------------------------------------------------------------------
int main( int argc, const char* argv[] )
{
    // parse command line args (1: symbol -- 2: strike (temporary))
    if (argc != 2) fprintf(stderr, "Need symbol and strike");

	Contract			C;
	C.symbol			= argv[1];
	C.secType			= "STK";
	C.currency			= "USD";
	C.exchange			= "SMART";

    time_t rawtime;
    tm* timeinfo;
    char queryTime [80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(queryTime, 80, "%Y%m%d %H:%M:%S", timeinfo);

    IBString DS			= DurationStr(24*60, *DurationHorizon::Seconds);
    IBString BSS		= *BarSizeSetting::_1_min;
    IBString WTS		= *WhatToShow::TRADES;

    int socketNumber   	= 4002;
    MyEWrapper	MW( false ); // threaded option
    EClientL0*	EC = EClientL0::New( &MW );

    if( EC->eConnect( "", socketNumber, 100 ) )
    {
        // reset/initialize variables at top of loop
        EndOfHistoricalData = false;
        dataCheck = 0;

        //////////////// GET TICKS
        EC->reqHistoricalData
            ( 1
              , C
              , queryTime
              , DS	
              , BSS
              , WTS	
              , UseRTH::OnlyRegularTradingData
              , FormatDate::AsDate                  		// AsSecondsSince or AsDate
            );

        while( !EndOfHistoricalData and !ErrorForRequest ) EC->checkMessages();
        //printf("Finished historical data call\n");


    }

    EC->eDisconnect();
    delete EC;
    return ErrorForRequest;
}
