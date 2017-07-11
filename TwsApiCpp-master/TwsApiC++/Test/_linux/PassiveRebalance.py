"""
*** NOTE: Before proceeding, understand that you are 100% responsible
          for understanding this program completely. We are not liable
          for any financial losses you take by using this program. You
          should absolutely test this extensively in a paper trading
          account.

This program drives a passive investing strategy based on
popular ETFs of 11 major asset classes:

US Stocks               VTI
Foreign Stocks          VEA
Emerging Markets        VWO
Dividend Stocks         VIG
Natural Resources       XLE
TIPS                    SCHP
US Government Bonds     SHY
Municipal Bonds         MUB
Corporate Bonds         LQD
Emerging Market Bonds   VWO
Real Estate             VNQ

*** NOTE: Do your own research on the ETFs being used, they don't
          necessarily have the same tax benefits and are often used
          for different purposes. The breakdown of these ETFs should
          be chosen based on your own needs and research.

          ETFs can also be changed in the initialization dictionary.


Based on the user's defined allocation of each asset class, the purpose
of this program is to constantly monitor and rebalance when necessary.

*** NOTE: This implementation does not optimize for taxes, and only rebalances
          based on the asset allocation specified. For the initial
          implementation, this program also does not optimize for transaction
          fees and could be done in a better way.


@author: Justin of White Oak Financial Analytics
"""

import numpy as np
from subprocess import Popen, PIPE

def Rebalance():
    # define rebalance parameters -- thresholds, allocations, etc.
    # single dictionary will be used to define parameters:
    # 'assetClass' (str): (symbol, idealAllocation, threshhold, currentAllocation) (tuple)

    rebalance_dict = {'USStocks':         ('VTI', 0.35, 0.02, None),
                      'ForeignStocks':    ('VEA', 0.20, 0.02, None),
                      'EmergingMarkets':  ('VWO', 0.15, 0.02, None),
                      'DividendStocks':   ('VIG', 0.07, 0.02, None),
                      'NaturalResources': ('XLE', 0.05, 0.02, None),
                      'MunicipleBonds':   ('MUB', 0.18, 0.02, None) }

    portfolio, portfolio_value = getPortfolio()
    
    # if portfolio is currently empty, place trades corresponding to ideal allocation of each asset
    if not portfolio:
        for _, val in rebalance_dict.iteritems():
            print val
            price = getCurrentPrice(val[0])
            quantity = (portfolio_value * val[1]) / price
            print quantity
            placeOrder(val[0], quantity)

    # rebalance

def placeOrder(symbol, quantity):
    pass

def getCurrentPrice(symbol):
    return 100

def getPortfolio():
    return [], 100000

if __name__=='__main__':
    Rebalance()
