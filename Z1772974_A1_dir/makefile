#  Carlton Hunt
#  Assignment 1 makefile

.DEFAULT_GOAL := assign1.exe

shuffle.o: assign1.cc
	g++ -c assign1.cc -Wall -std=c++11

assign1.exe: assign1.o
	g++ -o assign1.exe assign1.o -Wall -std=c++11

clean:
	rm *.o assign1.exe