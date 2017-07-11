"""
THIS STRATEGY WILL ALMOST CERTAINLY LOSE MONEY IN 
LIVE TRADING AND IS MEANT TO BE USED AS A DEMO/TEMPLATE

Automated trading algo based on a 24-tick moving 
avg. and a 9-tick exponential avg. of SPY. This could
easily be extended to include more symbols but for
simplicity it does not have that functionality. The 
initial implementation assumes a 1-minute tick size.

Test this algo in a paper trading account -- we are 
not liable for financial losses. Ideally, you can 
adapt this program to suit your needs.

Usage:

1) Have a Linux computer (written on Ubuntu 16.04)
    with Python and the gcc compiler
2) Download Interactive Broker's TWS or IB Gateway
3) Change number on TWS or Gateway to 4002
4) Allow Active-X clients (global configuration)
5) cd to TwsApiCpp-master/TwsApiC++/Test/_linux from terminal
6) chmod u+x compile (if running for the first time)
7) ./compile
8) python Momentum.py <account_number> (ex. DU123456)
9) Watch the algo go! (Ideally in TWS to monitor trades
    while you get the hang of it)

--

Spread the word and help us lower the barrier to entry in
the finance world!

@author: Justin
"""

import sys
import os
from time import sleep
import datetime
import numpy as np
from subprocess import Popen, PIPE

def Momentum(account_number):
    
    # Only run algo during trading hours (starts at 25 ticks into trading day)
    start = datetime.time(9,55,0)
    end   = datetime.time(16,0,0)
    
    # define the ticker list and momentum criteria
    # algo: if the short-term moving avg. is less than the long-term + ticker_criteria.val, ideal size = -100
    #       if the short-term moving avg. is more than the long-term + ticker_criteria.val, ideal size = 100
    #       else the ideal position size is zero
    ticker_criteria = {'GS': 0.1, 'AAPL': 0.1, 'SPY': 0.1, 'GOOG': 0.1, 'AMZN': 0.1, 'TSLA': 0.1}

    while True:
        day = datetime.date.weekday(datetime.datetime.now())
        now = datetime.datetime.now().time()
        if now < start or now > end or day > 4: continue

        # Initialize the algo by getting current portfolio -- i.e. number of SPY shares
        # Popen will call a C++ program and gather data through standard output
        p = Popen('./debug/getPortfolio ' + account_number, stdout=PIPE, stderr=PIPE, shell=True)
        while p.poll() != None: continue

        # create portfolio dict {symbol, quantity} and add to dict if IB showed zero quantity
        portfolio = p.communicate()[0].split() # will look like AAPL 100 GOOG -100 AMZN 100 ....
        portfolio = {portfolio[i-1]: int(portfolio[i]) for i in xrange(1,len(portfolio),2)}
        portfolio.update({i: 0 for i in ticker_criteria if i not in portfolio})
        try: p.kill()
        except OSError: pass
        print portfolio

        # Get data for previous 24 ticks to compute the 2 moving averages
        # Place order to make actual position match ideal position (-100, 0, or 100)
        for symbol in ticker_criteria:
            print '\n\n--------------'
            print symbol
            command = './debug/DailyHistorical ' + symbol
            #print command
            p = Popen(command, stdout=PIPE, stderr=PIPE, shell=True)
            while p.poll() != None: continue

            ticks_24 = p.communicate()[0].split()
            ticks_24 = np.array([float(x) for x in ticks_24])
            ticks_9  = ticks_24[15:24]
            #print ticks_24
            #print ticks_9

            long_ma  = np.average(ticks_24)
            try: short_ma = np.ma.average(ticks_9, weights=[1,2,3,4,5,6,7,8,9])
            except: continue
            print long_ma
            print short_ma

            try: p.kill()
            except OSError: pass
        
            # figure out how much position needs to change by, and place order for that quantity
            if short_ma > long_ma + ticker_criteria[symbol]:
                ideal_quantity = 100
            elif short_ma < long_ma - ticker_criteria[symbol]:
                ideal_quantity = -100
            else:
                ideal_quantity = 0

            diff = ideal_quantity - portfolio[symbol]
            if diff != 0:
                print 'sending order for ' + symbol
                command = './debug/SendStockAdaptiveOrder ' + symbol + ' ' + str(diff) + ' ' + account_number
                print command
                os.system(command)
                #p = Popen(command, stdout=PIPE, stderr=PIPE, shell=True)
                #while p.poll() != None: continue
                #try: p.kill()
                #except OSError: print 'failed kill process'
                
            sleep(10)

if __name__=='__main__':
    if len(sys.argv) == 2:
        Momentum(sys.argv[1])
    else:
        sys.stderr.write('ERROR: Command Line: Need one argument -- IB account number\n')
