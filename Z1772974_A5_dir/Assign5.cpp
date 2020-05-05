/*****************************************************************
Program: Assignment 5
Programmer: Carlton Hunt
zID: Z1772974
Due Date: 4/06/2020
Class: Section 2

Purpose: use POSIX threads, semaphores and a mutex to illustrate
		the Producer-Consumer Problem.

*****************************************************************/

#include<iostream>
#include<iomanip>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include<utility>

#define BUFFER_SIZE 35
#define P_NUMBER 7
#define C_NUMBER 5
#define P_STEPS 5
#define C_STEPS 7

using namespace std;

void insert(int);
void remove(int);
void* produce(void*);
void* consume(void*);

int counter = 0; 				//Counter
int num = -1;					//Value of widget

pair<int,int> widget;			//Widget is a pair (P & W)

queue<pair<int,int>> buffer;	//Buffer for widgets
queue<pair<int,int>> temp;		//Queue for printing widgets

pthread_mutex_t mCounter;	//Mutex

sem_t NotFull;
sem_t NotEmpty;

pthread_t cThreads[C_NUMBER];	//Consumer threads
pthread_t pThreads[P_NUMBER];	//Producer threads

int main() {
   cout << "Simulation of Producers and Consumers" << endl << endl;

   if(sem_init(&NotFull, 0, BUFFER_SIZE) == -1) {						//NotFull = buffer size,check for error, show user if error
      cerr << "Error initializing NotFull semaphore";
      exit(-1);
   }

   if(sem_init(&NotEmpty, 0, 0) == -1) {								//NotEmpty = 0,check for error, show user if error
      cerr << "Error initializing NotEmpty semaphore";
      exit(-1);
   }

   if(pthread_mutex_init(&mCounter, NULL) != 0) {						//Counter = default, check for error, show user if error
      cerr << "Error initializing mCounter mutex";
      exit(-1);
   }

   cout << "The semaphores and mutex have been initialized." << endl;	//Semaphores and mutex have been initialized

   for(long threadID = 0; threadID < C_NUMBER; threadID++) {							//Loop until CNUMBER
      if(pthread_create(&cThreads[threadID], NULL, consume, (void*) threadID) != 0) {	//Create consumer threads with threadID number and call consume function, check for error
         cerr << "Error creating a consumer thread";
         exit(-1);
      }
   }

   for(long threadID = 0; threadID < P_NUMBER; threadID++) {					//Loop until PNUMBER
      if(pthread_create(&pThreads[threadID], NULL, produce, (void*) threadID) != 0) {	//Create producer threads with threadID number and call produce function, check for error
         cerr << "Error creating a producer thread";
         exit(-1);
      }
   }

   for(int i = 0; i < C_NUMBER; i++) {						//Loop through cThreads
      if(pthread_join(cThreads[i], NULL) != 0) {			//Terminate all consumer threads, check for error
         cerr << "Error terminating consumer thread";
         exit(-1);
      }
   }

   for(int i = 0; i < P_NUMBER; i++) {						//Loop through pThreads
      if(pthread_join(pThreads[i], NULL) != 0) {			//Terminate all producer threads, check for error
         cerr << "Error terminating producer thread";
         exit(-1);
      }
   }

   cout << endl <<"All the producer and consumer threads have been closed" << endl;	//All threads have been terminated

   sem_destroy(&NotFull); 								//Destroy NotFull semaphore

   sem_destroy(&NotEmpty);								//Destory NotEmpty semaphore


   pthread_mutex_destroy(&mCounter);					//Destroy mCounter mutex

   cout << "The semaphores and mutex have been deleted" << endl;		//Semaphores and mutex have been destroyed

   pthread_exit(NULL);									//Terminate thread

   return 0;
}

/****************************************************
insert() Method
	Lock the mutex
	Add the Widget to the buffer
	Increment Counter
	Unlock the mutex
****************************************************/

void insert(int threadID) {
   pthread_mutex_lock(&mCounter);						// Lock the mutex

   if(threadID == temp.front().first)					//If thread has already created a widget
      num++;											//Increment value
   buffer.push(make_pair(threadID, num));				//Add widget
   counter++;											//Increment buffer
   queue<pair<int,int>> temp = buffer;					//Copy buffer to queue to print


   cout << left << setw(9) << "Producer " << threadID << " inserted one item." << setw(15) << right
	<< "Total is now " << counter << ".   Buffer now contains:" << endl << "\t";

   while(!temp.empty()) {								//Loop through temp queue
      	cout << "P" << temp.front().first;				//Show the producer threadID in buffer
	cout << "W" << temp.front().second;					//Show the widget value in buffer
	temp.pop();											//Pop the queue
	cout << " ";
   }

   cout << endl;

   pthread_mutex_unlock(&mCounter);						//Unlock the mutex

}

/****************************************************
remove() Method

   Lock the mutex
   Remove a Widget
   Decrement Counter
   Unlock the mutex
****************************************************/

void remove(int threadID) {

   pthread_mutex_lock(&mCounter);						//Lock the mutex

   buffer.pop();										//Pop widget
   counter--;											//Decrement counter
   queue<pair<int,int>> temp = buffer;					//Copy buffer to a temp queue to print

   cout << left << setw(9) << "Consumer " << threadID << " removed one item." << setw(16) << right
	<< "Total is now " << counter << ".   Buffer now contains:" << endl << "\t";

   while(!temp.empty()) {								//Loop through temp queue
      	cout << "P" << temp.front().first;				//Show the producer threadID in buffer
	cout << "W" << temp.front().second;					//Show the widget value in buffer
	temp.pop();											//Pop
	cout << " ";
   }

   cout << endl;

   pthread_mutex_unlock(&mCounter);						//Unlock the mutex
}

/****************************************************
produce() Method

   wait
   Insert
   post
****************************************************/

void* produce(void* threadID) {
   for(int i = 0; i < P_STEPS; i++) {					//Loop until P_STEPS
      sem_wait(&NotFull);								//Wait for space
      insert((long)threadID);							//Insert a widget
      sleep(1);											//Sleep
      sem_post(&NotEmpty);	     						//Post the semaphore
   }
   pthread_exit(0);										//Terminate thread
}

/****************************************************************************
consume() Method

   wait
   Remove
   post
*****************************************************************************/

void* consume(void* threadID) {
   for(int i = 0; i < C_STEPS; i++) {					//Loop until C_STEPS
      sem_wait(&NotEmpty);								//Wait for buffer
      remove((long)threadID);							//Remove a widget
      sleep(1);											//Sleep
      sem_post(&NotFull);								//Post the semaphore
   }
   pthread_exit(0);										//Terminate thread
}
