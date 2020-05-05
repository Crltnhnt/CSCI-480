# Carlton Hunt
# Z1772974
# makefile for assign 5

CCFLAGS = -Wall

# Rule to link object code files to create executable file
Assign5: Assign5.cpp
	g++ -pthread -Wall -g -std=c++11 Assign5.cpp -o Assign5

# Rule to compile source code file to object code
Assign5.o: Assign5.cpp
	g++ $(CCFLAGS) -c Assign5.cpp

# Pseudo-target to remove object code and executable files
clean:
	-rm *.o Assign5
