# PROGRAM:    Assign 4
# PROGRAMMER: Carlton HUnt
# LOGON ID:   Z1772974
# DATE DUE:   03/24/20
#

# Compiler variables
CCFLAGS = -ggdb -Wall -std=c++11

# Rule to link object code files to create executable file
Assign3: Assign4.o
	g++ $(CCFLAGS) -o Assign4 Assign4.o

# Rule to compile source code file to object code
Assign3.o: Assign4.cpp
	g++ $(CCFLAGS) -c Assign4.cpp

# Pseudo-target to remove object code and executable files
clean:
	-rm *.o Assign4
