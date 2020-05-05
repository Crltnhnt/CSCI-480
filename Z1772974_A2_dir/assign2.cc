/****************************************************************
PROGRAM:   Assignment 2
STUDENT:   Carlton Hunt
Z-ID:	   Z1772974
DUE DATE:  Friday February 7th
FUNCTION: We will have three processes which communicate with
	each other using pipes.
*****************************************************************/
#include<iostream>
#include<string>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
using namespace std;

/***************************************************
*Function: Parent work (PWork) function starts the
*	communication and converts it into nubmers,
*	calculates, converts it to a string, sends it
*	back over the pipe.
*Input: R/W for pipes
*Output: no output
*****************************************************/
void PWork(int write_ID, int read_ID){
	string buffer = "1@";
	string value = "1";
	char ch;
	int M = 1;

	cerr << "The parent process is ready to proceed." <<endl;
	cerr << "Parent        Value: " << M <<endl;
	write(write_ID, buffer.c_str(), buffer.length());
	buffer.clear();	//Clear buffer
	value.clear();  //Clear value

	while(true){
		while(read(read_ID, &ch, 1) > 0){
			if(ch == '@')	//If EOF stop
				break;
			value.push_back(ch);
		}

		try{
			M = stoi(value);
		}catch(...){
			break;
		}

		if (-999999999 <= M && M >= 999999999){
			write(write_ID, "*@", 2);	//Exit other processes
			break;	//return to main
		}

		else{
			M = 200 - 3 * M;	//Calculate

			buffer = to_string(M);	//Convert to string
			cerr << "Parent        Value: " << M <<endl;	//Show m in Parent
			write(write_ID, buffer.c_str(), buffer.length());
			write(write_ID, "@", 1);
		}

		value.clear();	//Clear value
		buffer.clear(); //Clear buffer
	}
}

/************************************************************
*Function: Child work(CWork) function that recives
*	a string over the pipe and converts
*	it into nubmers, calculates, converts it to a string,
*	sends it back over the pipe.
*Input: R/W for pipes
*Output: no output
*************************************************************/
void CWork(int write_ID, int read_ID){
	string buffer;
	string value;
	char ch;
	int M = 1;

	cerr << "The child process is ready to proceed." <<endl;
	while(true){
		while(read(read_ID, &ch, 1) > 0){	//Read chars
			if(ch == '@')
				break;
			value.push_back(ch);
		}

		try{
			M = stoi(value);	//Convert value to m
		}catch(...){
			break;
		}

		if (-999999999 <= M && M >= 999999999){	//If number is outside the limits
			write(write_ID, "*@", 2);
			break;
		}

		else{
			M = 7 * M - 6;	//Calculate

			buffer = to_string(M);	//Convert to string
			cerr << "Child         Value: " << M <<endl;	//Show number in Child
			write(write_ID, buffer.c_str(), buffer.length());
			write(write_ID, "@", 1);
		}

		value.clear();	//Clear value
		buffer.clear(); //Clear buffer
	}

}

/***************************************************
*Function: GrandChild work function that recives
*	a string over the pipe and converts
*	it into nubmers, calculates, converts it to a string,
*	sends it back over the pipe.
*Input: R/W for pipes
*Output: no output
*****************************************************/
void GWork(int write_ID, int read_ID){
	string buffer;
	string value;
	char ch;
	int M;

	cerr << "The Grandchild process is ready to proceed." <<endl;

	while(true){
		while(read(read_ID, &ch, 1) > 0){	//Read chars
			if(ch == '@')
				break;
			value.push_back(ch);
		}

		try{
			M = stoi(value);	//Convert to value
		}
		catch(...){
			break;
		}

		if (-999999999 <= M && M >= 999999999){	//If the number is outside the limits
			write(write_ID, "*@", 2);	//Stop processes
			break;
		}

		else{
			M = 30 - 4 * M;	//math for number

			buffer = to_string(M);	//Convert to string
			cerr << "GrandChild    Value: " << M <<endl;	//Show m in Grandchild
			write(write_ID, buffer.c_str(), buffer.length());	//Send
			write(write_ID, "@", 1);	//Send
		}

		value.clear();	//Clear value
		buffer.clear();	//Clear buffer
	}
}


//Main program

int main()
{
	int pipeA[2];
	int pipeB[2];
	int pipeC[2];

	pid_t pid;	//save the pid

	if(pipe(pipeA)){	//pipeA
		cerr << "Pipe A error." <<endl;
		exit(-5);
	}

	if(pipe(pipeB)){	//pipeB
		cerr << "Pipe B error." <<endl;
		exit(-5);
	}

	if(pipe(pipeC)){	//pipeC
		cerr << "Pipe C error." <<endl;
		exit(-5);
	}





	pid = fork();	//Fork into child process
	if(pid < 0){	//fork failed
		cerr << "fork for child failed." <<endl;
		exit(-5);
	}

	else if(pid == 0){
		pid_t grand_pid;
		grand_pid = fork();	//grandchild fork

		if(grand_pid < 0){	//fork failed
			cerr << "fork for grandchild failed." <<endl;
			exit(-5);
		}

		else if (grand_pid == 0){ //Grandchild process
			close(pipeC[0]);//Close unused ends
			close(pipeB[1]);
			GWork(pipeC[1], pipeB[0]);	//function for grandchild
			close(pipeC[1]);	//Close remaining ends
			close(pipeB[0]);
			exit(0);
		}

		else{
			close(pipeB[0]);	//close unused ends
			close(pipeA[1]);
			CWork(pipeB[1], pipeA[0]);	//function for child
			close(pipeB[1]);	//close the used ends
			close(pipeA[0]);
			exit(0);
		}

	}

	else{
		close(pipeA[0]); //Close ends
		close(pipeC[1]);
		PWork(pipeA[1], pipeC[0]);
		close(pipeA[1]);	//Close used pipes
		close(pipeC[0]);
		wait(0);	//wait
		exit(0);	//exit
	}

	return 0;	//Not needed but good practice
}
