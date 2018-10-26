#https://www.cs.bu.edu/teaching/cpp/writing-makefiles/
# Makefile for Writing Make Files Example

# *****************************************************
# Variables to control Makefile operation

CXX = g++
CXXFLAGS = -Wall -g

# ****************************************************
# Targets needed to bring the executable up to date

escalona: escalona.o Esc.o EvTx.o Graph.o Log.o Tx.o TxIdx.o  
	$(CXX) $(CXXFLAGS) -o escalona escalona.o Esc.o EvTx.o Graph.o Log.o Tx.o TxIdx.o  

# The main.o target can be written more simply

escalona.o: escalona.cpp Esc.h EvTx.h Graph.h Log.h Tx.h TxIdx.h  
	$(CXX) $(CXXFLAGS) -c escalona.cpp

Esc.o: Esc.h Log.h
EvTx.o: EvTx.h 
Graph.o: Graph.h
Log.o: Log.h
Tx.o: Tx.h
TxIdx.o: TxIdx.h