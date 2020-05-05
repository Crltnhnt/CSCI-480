/**********************************************************************
PROGRAM: Assignment 1
AUTHOR:  Carlton Hunt
ZID:  Z1772974
DUE DATE:  Friday, January 24
FUNCTION:  This program involves using system functions such as fork(),
			getpid(), getppid(), sleep(), wait() and system().
***********************************************************************/
#include<iostream>
#include<unistd.h>
#include <sys/wait.h>
using namespace std;

int main()
{
	pid_t pid; //save the child pid

	cerr << "I am the original process. My PID is " << getpid() << " and my parent's PID is " << getppid() <<endl;
	cerr << "Now we have the first fork." <<endl;

	pid = fork();  //fork the parent to go to child
	if(pid < 0)  //If failure, exit
	{
		cerr << "The First fork has failed!" <<endl;
		exit(-1);
	}

	else if(pid == 0)  //If the fork workeed, cool
	{
		pid_t g_pid; //grand child pid

		cerr << "I am the child. My PID is " << getpid() << " and my parent's PID is " << getppid() <<endl;
		cerr << "Now we have the second fork." <<endl;

                system("ps"); //ps command
		g_pid = fork(); //grand child fork

		if(g_pid < 0)	//If failure, exit
		{
			cerr << "The second fork has failed!" <<endl;
			exit(-1);
		}

		else if (g_pid == 0)//successful fork for grand child
		{
			cerr << "I am the grandchild. My PID is " << getpid() << " and my parent's PID is " << getppid() <<endl;
                        cerr << "I will now call ps" << endl;
                        system("ps"); //system ps command
                        wait(0); //wait until finish
			cerr << "I am the grandchild. I am going to exit" <<endl;
			exit(0);
		}
		else //child process
		{
			cerr << "I am the child. My PID is " << getpid() << " and my parent's PID is " << getppid() <<endl;
			wait(0); //wait
			cerr << "I am the child process. I am about to exit." <<endl;
			exit(0); //exit child process
		}
	}

	else //parent process after fork
	{
		cerr << "I am the parent process. My PID is " << getpid() << " and my parend's PID is " << getppid() <<endl;
		sleep(2); //3 seconds
		cerr << "I am the parent, about to call 'ps'." <<endl;
		system("ps"); //system ps command
		wait(0); //wait until finish
		cerr << "I am the parent. I am about to exit." <<endl;
		exit(0); //done
	}

	return 0;
}
