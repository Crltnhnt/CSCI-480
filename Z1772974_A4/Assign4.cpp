/********************************************
* CSCI 480 Section 2
* Name: Carlton Hunt
* Program: Assignment 4
* Due Date: March 24th 2020
*
*********************************************/
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <stack>
#include <queue>


using namespace std;


struct Entry {
	int pageNum;
	int frameNum = -1;
	bool modified;
};

void printTable(Entry[]);

int searchOldPage(Entry[], int);

int findIfAlreadyUsed(Entry[], int);

const int FRAME_NUMBER = 15;
const int PAGE_NUMBER = 25;
const int HOW_OFTEN = 5;
const int PRINT_LINES = 5;
const int PAGE_SIZE = 200;

int main(int argc, char* argv[]) {
    int inputLines = 0;
    int pageFaultTotal = 0;
    string firstArg = argv[1];
    string secondArg = argv[2];
    int logicalAddress;
    int counter = 0;
    string letter;

    stack<int> lRU;
    stack<int> lRU2;
    queue<int> fIFO;

    fstream infile;
    infile.open("data4.txt", ios::in);

    Entry pageTable[PAGE_NUMBER];
    const int frames[FRAME_NUMBER] = {101, 102, 103, 104, 105, 201, 202, 203, 204, 205, 301, 302, 303, 304, 305};

    if(argc > 3) {
        cerr << "Over max arguments" << endl;
        exit(-1);
    } else if (argc < 3) {
        cerr << "Need more arguments" << endl;
        exit(-1);
    }

    if (firstArg != "F" && firstArg != "L") {
        cerr << "Enter F or L for the first argument" << endl;
        exit(-2);
    } else if (secondArg != "P" && secondArg != "D") {
        cerr << "Enter P or D for the second argument" << endl;
        exit(-2);
    }

    if (secondArg == "P") {
        for (int i = 0; i < FRAME_NUMBER; i++) {
            pageTable[i].frameNum = frames[i];
            if (firstArg == "F") {
                fIFO.push(frames[i]); // queue
            } else {
                lRU.push(frames[i]); // stack
            }
        }
    }

    // Displays which algorithm we are doing and what kind of paging
    if (firstArg == "L") {
        cerr << "--------------Starting program with LRU";
    } else {
        cerr << "-------------Starting program with FIFO";
    }

    if (secondArg == "P") {
        cerr << " and Pre Paging--------------" << endl;
    } else {
        cerr << " and Demand Paging-----------" << endl;
    }

    while(infile >> logicalAddress >> letter) {
        inputLines++;

        int pageNum = 0;

        pageNum = logicalAddress / PAGE_SIZE;

/******************************************************************
* place all frames into bottom of the stack
*******************************************************************/
        if (pageTable[pageNum].frameNum != -1 && (firstArg == "L")) {
            int findFrame = pageTable[pageNum].frameNum;
            int stack1size = lRU.size();
            for (int i = 0; i < stack1size; i++){
                int top = lRU.top();
                if (top == findFrame) {
                    lRU.pop();
                } else {
                    lRU2.push(top);
                    lRU.pop();
                }
            }
            int stackSize = lRU2.size();
            for (int i = 0; i < stackSize; i++){
                int top = lRU2.top();
                lRU.push(top);
                lRU2.pop();
            }
            lRU.push(findFrame);
        }
/*******************************************************************************
* Look for new frame if all are being used
*********************************************************************************/
        if(pageTable[pageNum].frameNum == -1) {
            pageFaultTotal++;
            if ((secondArg == "P") || counter == (FRAME_NUMBER)) {
		int oldPage = 0;
		int nextFrame = 0;
                if (firstArg == "F") {
                    nextFrame = fIFO.front();
                    oldPage = searchOldPage(pageTable, nextFrame);
                    fIFO.pop();
                    pageTable[pageNum].frameNum = nextFrame;
                    pageTable[oldPage].frameNum = -1;
                    fIFO.push(nextFrame);
                } else {
                    int size = lRU.size();

                    for (int i = 0; i < size; i++) {
                        int top = lRU.top();
                        lRU.pop();
                        lRU2.push(top);
                    }

                    nextFrame = lRU2.top();
                    oldPage = searchOldPage(pageTable, nextFrame);
                    lRU2.pop();
                    pageTable[pageNum].frameNum = nextFrame;
                    pageTable[oldPage].frameNum = -1;
                    int size2 = lRU2.size();

                    for (int i = 0; i < size2; i++) {
                        int top = lRU2.top();
                        lRU2.pop();
                        lRU.push(top);
                    }
                    lRU.push(nextFrame);
                }

                if (pageTable[oldPage].modified == true) {
                    cerr << "Write page " << oldPage << " from " << nextFrame << " to the disk\n";
                    pageTable[oldPage].modified = false;
                }
              cerr << "Read page " << pageNum << " from the disk into frame " << pageTable[pageNum].frameNum << endl;
            } else {
		pageTable[pageNum].frameNum = frames[counter];
		cerr << "Read page " << pageNum << " from the disk into frame " << pageTable[pageNum].frameNum << endl;
                if (firstArg == "F") {
                    fIFO.push(frames[counter]);

                } else {
                    lRU.push(frames[counter]);

                }
                counter++;
            }
        }

        if (inputLines % PRINT_LINES == 0) {
            printTable(pageTable);
        }
    }

    infile.close();
    cout << "Total page faults: " << pageFaultTotal << endl;

    return 0;
}

/******************************************************************************
* prints the page table
*******************************************************************************/
void printTable(Entry array[]){
    cout <<setw(16)<< "Page Number" << setw(16) << "Frame Number" << setw(16) << "Modified?" << endl;

    for (int i = 0; i < PAGE_NUMBER; i++) {
        if (array[i].modified == true) {
            cout <<setw(16)<< i << setw(16)<< array[i].frameNum << setw(16) <<  "Yes" << endl;
        } else {
            cout <<setw(16)<< i <<setw(16)<< array[i].frameNum << setw(16)<< "No" << endl;
        }
    }
}
/*********************************************************************************************************
* looks for old page and takes the frame 
**********************************************************************************************************/
int searchOldPage(Entry pageTable[], int newFrame) {
    for (int i = 0; i < PAGE_NUMBER; i++) {
        if (newFrame == pageTable[i].frameNum){
            return i;
        }
    }
   return 0;
}
                                                                                                                                                                                                                                                                                                              