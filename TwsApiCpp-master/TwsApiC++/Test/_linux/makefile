# TwsApiC++ Debug Version

CC      = gcc
INCLUDE = -I../../Api -I../../../source/PosixClient/Shared
CFLAGS  = -D_REENTRANT -D_DEBUG -ggdb -Wall
LIBS    = -L../../Api -lTwsApiL0D -lstdc++ -lpthread -lm

all: SendStockAdaptiveOrder DailyHistorical getPortfolio

SendStockAdaptiveOrder: ../Src/SendStockAdaptiveOrder.cpp ../../Api/libTwsApiL0D.a
	$(CC) $(INCLUDE) $(CFLAGS) ../Src/SendStockAdaptiveOrder.cpp $(LIBS) -o ./debug/SendStockAdaptiveOrder

DailyHistorical: ../Src/DailyHistorical.cpp ../../Api/libTwsApiL0D.a
	$(CC) $(INCLUDE) $(CFLAGS) ../Src/DailyHistorical.cpp $(LIBS) -o ./debug/DailyHistorical

getPortfolio: ../Src/getPortfolio.cpp ../../Api/libTwsApiL0D.a
	$(CC) $(INCLUDE) $(CFLAGS) ../Src/getPortfolio.cpp $(LIBS) -o ./debug/getPortfolio


clean:
	rm -f ./debug/*
