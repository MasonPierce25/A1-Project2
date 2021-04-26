all: main.o Board.o
	g++ -std=c++11 main.o Board.o

main.o: main.cpp Board.h
	g++ -std=c++11 -c main.cpp

Board.o: Board.h Board.cpp
	g++ -std=c++11 -c Board.cpp
