#  Carlton Hunt
#  Assignment 2 makefile

.DEFAULT_GOAL := assign2.exe

shuffle.o: assign2.cc
	g++ -c assign2.cc -Wall -std=c++11

assign2.exe: assign2.o
	g++ -o assign2.exe assign2.o -Wall -std=c++11

clean:
	rm *.o assign2.exe