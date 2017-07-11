"""
THIS STRATEGY WILL ALMOST CERTAINLY LOSE MONEY IN 
LIVE TRADING AND IS MEANT TO BE USED AS A DEMO/TEMPLATE

Automated trading algo based on a 24-tick moving 
avg. and a 9-tick exponential avg. of SPY. This could
easily be extended to include a handful of symbols,
but for simplicity it does not have that functionality.
The initial implementation assumes a 1-minute tick size

Test this algo in a paper trading account -- we are 
not liable for financial losses. Ideally, you can 
adapt this program to suit your needs.

Usage:

1) Have a Linux computer with Python and the gcc compiler
2) Download Interactive Broker's TWS or IB Gateway
3) Change number on TWS or Gateway is 4002
4) Allow Active-X clients (global configuration)
5) cd to TwsApiCpp-master/TwsApiC++/Test/_linux from terminal
6) chmod u+x compile (if running for the first time)
7) ./compile
8) python Momentum.py <account_number> (ex. DU123456)
9) Watch the algo go! (Ideally in TWS to monitor trades)

--

Spread the word and help us lower the barrier to entry in
the finance world!

@author: Justin of Canopy
"""

import sys
import datetime
from subprocess import Popen, PIPE

def Momentum(account_number):
    
    # Only run algo during trading hours (starts at 25 ticks into trading day)
    start = datetime.time(9,55,0)
    end   = datetime.time(16,0,0)
    while True:
        day = datetime.date.weekday(datetime.datetime.now())
        now = datetime.datetime.now().time()
        if now < start or now > end or day > 4:
            continue

        # Initialize the algo by getting current portfolio -- i.e. number of SPY shares
