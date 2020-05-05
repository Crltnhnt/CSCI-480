/**************************************************************************
Program: Assignment 6 (Extra Credit)
Programmer: Carlton Hunt
ZID: z1772974
Due Date: 4/13/2020
Class: Section 2

Purpose: This program simulates memory management. When a process is
	 loaded into memory or a process requests a block of memory
	 dynamically, the system must allocate memory, and when a
	 process terminates or frees memory previously requested,
	 the system must deallocate memory.
**************************************************************************/
#include<iostream>
#include<cstring>
#include<cstdlib>
#include<list>
#include<fstream>
#include<vector>
#include<sstream>
#include<string>

#define HOWOFTEN 6

using namespace std;

struct Block {
int sAddress,			//Starting address of block
    size,			//Size of block
    pid;			//Process ID of block

string bid;			//Block ID
};

void load(string);			//load transaction
void allocate(string);			//allocate transaction
void deallocate(string);		//deallocate transaction
void terminated(string);		//terminate transaction
void printBlocks();			//printBlocks method
Block bestFitBlock(int);
Block newBlock(int, int, int, string);	//newBlock method
list<Block> Avail;			//Blocks not in use
list<Block> InUse;			//Blocks in use
list<Block>::iterator it;		//print
list<Block>::iterator it2;
const int KB = 1024,
          MB = 1024 * 1024;

string algorithm,
       line;

int count = 0;

bool firstFit = false;
bool bestFit = false;

int main(int argc, char* argv[]) {
   Block block1 = newBlock((MB * 3), MB, 0, "");					//Block of size 1 MB
   Avail.push_front(block1);								//Push

   Block block2 = newBlock((MB * 3) + (MB * 2), MB, 0, "");				//Block of size 1 MB
   Avail.push_back(block2);								//Push

   Block block3 = newBlock((MB * 3) + (MB * 3), MB, 0, "");				//Block of size 1 MB
   Avail.push_back(block3);								//Push

   Block block4 = newBlock((MB * 3) + (MB * 3), MB * 2, 0, "");				//Block of size 2 MB
   Avail.push_back(block4);								//Push

   Block block5 = newBlock((MB * 3) + (MB * 3) + (MB * 2), MB * 8, 0, "");		//Block of size 8 MB
   Avail.push_back(block5);								//Push



   if(argv[1] != NULL) {							//If command line argument is present
      if(strcmp(argv[1], "B") == 0 || strcmp(argv[1], "b") == 0) {		//Check if its a b
         bestFit = true;
	 algorithm = "Best-Fit";						//If it is call best fit
      }
      else if(strcmp(argv[1], "F") == 0 || strcmp(argv[1], "f") == 0) {		//Check if its a f
	 firstFit = true;
	 algorithm = "First-Fit";						//If it is call first fit
      }
     else {
         cout << "Invalid argument" << endl;
	 exit(-1);								//Exit
      }
   }
   else {
      cout << "This program needs an argument" << endl;
      exit(-1);									//Exit
   }

   cout << "Simulation of Memory Management using the " << algorithm <<  " algorithm" << endl << endl;
   cout << "Beginning of the run" << endl << endl;

   ifstream infile("data6.txt");						//Read the data file
   while(getline(infile, line)) {						//Get each line
      if(count % HOWOFTEN == 0)							//If mod HOWOFTEN is 0
         printBlocks();								//Print block info

      switch(line[0]) {								//Switch for first letter of each line
         case 'L':								//If trans is load
	    load(line);								//Call load method
	    break;
	 case 'A':								//If trans is allocate
	    allocate(line);							//Call allocate method
	    break;
	 case 'T':								//If trans is terminate
	    terminated(line);							//Call terminated method
	    break;
	 case 'D':								//If trans is deallocate
	    deallocate(line);							//Call deallocate method
	    break;
	 case '?':								//Delimeter
	    cout << "End of Run" << endl << endl;
	    printBlocks();
	    break;
      }
   count++;									//Increment count
   }

return 0;
}

/************************************************************************
load()

	Loads the process into a block depending on transaction data
************************************************************************/

void load(string line) {
   int id,
       size;
   bool found;							//Found flag
   string blockId;
   string trans;

   istringstream str(line);
   str >> trans >> id >> size >> blockId;			//Load in the rest of the line

   cout << "Transaction:   request to load process " 		//Process is being loaded
	<< id << ", block ID " << blockId << " using "
	<< size << " bytes" << endl;

   if(firstFit) {							//If first fit was chosen
      for(it = Avail.begin(); it != Avail.end(); it++) {		//Loop through available list
         if(it->size >= size) {						//If block size is greater than or equal to size of trans block
	    found = true;
	    cout << "Found a block of size " << it->size << endl;	//Show user block has been found
   	    Block block1 = newBlock(it->sAddress, size, id, blockId);	//Create a new block
	    it->size -= size;						//Decrement size of block by size of trans block
	    it->sAddress += size;					//Increment starting address of block
	    InUse.push_front(block1);					//Push the new block into InUse list
	    cout << "Success in allocating a block" << endl;
	    break;
	 }
      }
      if(found == false)
         cout << "No block of adequate size is available" << endl;
   }

   else {								//Best fit was chosen
      Block bBlock = bestFitBlock(size);				//Get the best fit block
      for(it = Avail.begin(); it != Avail.end(); it++) {		//Loop through Avail list
         if(bBlock.sAddress == it->sAddress) {				//If bBlock start address is same in list
	    found = true;						//Found best block in list
	    cout << "Found a block of size " << it->size << endl;	//Show user block has been found
   	    Block block1 = newBlock(it->sAddress, size, id, blockId);	//Create a new block
	    it->size -= size;						//Decrement size of block by size of trans block
	    it->sAddress += size;					//Increment starting address of block
	    InUse.push_front(block1);					//Push the new block into InUse list
	    cout << "Success in allocating a block" << endl;
	    break;
	 }
      }
      if(found == false)
         cout << "No block of adequate size is available" << endl;
   }
}

/************************************************************************
Allocate()

	Allocates a block of memory for a specific process
*************************************************************************/

void allocate(string line) {
   int id,
       size;
   bool found;						//Found flag
   string blockId;
   string trans;

   istringstream str(line);				//Convert line to use >> operator
   str >> trans >> id >> size >> blockId;		//Load in the rest of the line

   cout << "Transaction:   request to allocate " << 	//Show the user the process is being allocated
        size << " bytes for process " << id <<
	", block id: " << blockId << endl;

   if(firstFit) {							//If first fit was chosen
      for(it = Avail.begin(); it != Avail.end(); it++) {		//Loop through available list
         if(it->size >= size) {						//If block size is greater than or equal to size of trans block
	    found = true;						//Found is true
	    cout << "Found a block of size " << it->size << endl;	//Show user block has been found
   	    Block block1 = newBlock(it->sAddress, size, id, blockId);	//Create a new block
	    it->size -= size;						//Decrement size of block
	    it->sAddress += size;					//Increment starting address of block
	    InUse.push_front(block1);					//Push block to InUse list
	    cout << "Success in allocating a block" << endl;
	    break;
	 }
      }
      if(found == false)
         cout << "No block of adequate size is available" << endl;
   }
   else {								//Best fit was chosen
      Block bBlock = bestFitBlock(size);				//Get the best fit block
      for(it = Avail.begin(); it != Avail.end(); it++) {		//Loop through Avail list
         if(bBlock.sAddress == it->sAddress) {				//If start address of best fit matches list entry
	    found = true;						//Found the best fit block in list
	    cout << "Found a block of size " << it->size << endl;	//Show user block has been found
   	    Block block1 = newBlock(it->sAddress, size, id, blockId);	//Create a new block
	    it->size -= size;						//Decrement size of block by size of trans block
	    it->sAddress += size;					//Increment starting address of block
	    InUse.push_front(block1);					//Push the new block into InUse list
	    cout << "Success in allocating a block" << endl;
	    break;
	 }
      }
      if(found == false)
         cout << "No block of adequate size is available" << endl;
   }
}

/************************************************************************
deallocate()

	Deallocates block of memory
*************************************************************************/

void deallocate(string line) {
   list<Block>::iterator nextBlock;
   int id;
   bool found;
   string blockId;
   string trans;

   istringstream str(line);
   str >> trans >> id >> blockId;						//Load in the rest of the line

   cout << "Transaction:   request to deallocate block ID "
	<< blockId << " for process " << id << endl;

   for(it = InUse.begin(); it != InUse.end(); it++) {				//Loop through InUse list
      if(it->pid == id && it->bid == blockId) {					//If block pid and block bid equal trans data
         found = true;								//Mark found as true
	 Block block1 = newBlock(it->sAddress, it->size, it->pid, it->bid);	//Create a copy of block
	 for(it2 = Avail.begin(); it2 != Avail.end(); it2++) {			//Loop through Avail list to find where it goes depending on starting address
	    if(it2->sAddress > it->sAddress) {					//If starting address of Avail block is greater than starting address of InUse block
	       Avail.insert(it2, block1);					//Insert the block at that iteration
	       break;
	    }
         }
         for(it2 = Avail.begin(); it2 != --Avail.end(); it2++) {		//Loop through Avail looking for consecutive blocks where end address is starting address of next
            nextBlock = it2;							//Copy the iterator in Avail
	    nextBlock++;							//Increment iterator to get next block
	    if((it2->sAddress + it2->size) == nextBlock->sAddress) {		//If the end address of first block is equal to start address of second block
	       if((it2->size + nextBlock->size) < 4 * MB) {			//If the size of the combined block isnt more than 4 MB
	    	  cout << "Merging two blocks at " << it2->sAddress
		       << " and " << nextBlock->sAddress << endl;
	    	  it2->size = nextBlock->size + it2->size;			//Set the size of the merged block
                  Avail.erase(nextBlock);					//Erase the second block in the merger
	          it2--;							//Decrement iterator since block has been erased
	       }
	    }
         }
      }
   }
   if(found == false)
      cout << "The indicated block cannot be found" << endl << endl;
   else {
      cout << "Success in deallocating a block" << endl << endl;
   }
}

/*************************************************************************
terminate()

	Finds and deallocates all blocks belonging to the process.
*************************************************************************/

void terminated(string line) {
   list<Block>::iterator nextBlock;
   int id;
   bool found;
   string trans;

   istringstream str(line);
   str >> trans >> id;								//Load in the rest of the line

   cout << "Transaction:   request to terminate process " << id << endl;

   for(it = InUse.begin(); it != InUse.end(); it++) {				//Loop through list
      if(it->pid == id) {							//If pid from trans equals InUse block pid
         found = true;								//Found = true
	 Block block1 = newBlock(it->sAddress, it->size, it->pid, it->bid);	//Copy block
	 for(it2 = Avail.begin(); it2 != Avail.end(); it2++) {			//Loop through Avail list
	    if(it2->sAddress > it->sAddress) {					//If starting address of Avail block is greater than starting address of InUse block
	       Avail.insert(it2, block1);					//Insert the block
	       break;
	    }
         }
         for(it2 = Avail.begin(); it2 != --Avail.end(); it2++) {		//Loop through Avail
            nextBlock = it2;							//Copy the iterator
	    nextBlock++;							//Increment iterator to get next block
	    if((it2->sAddress + it2->size) >= nextBlock->sAddress) {		//If the end address of first block is eqial to start address of second block
	       if((it2->size + nextBlock->size) <= 4 * MB) {			//If the size of the combined block isnt more than 4 MB
	          cout << "Merging two blocks at " << it2->sAddress
		       << " and " << nextBlock->sAddress << endl;
	          it2->size += nextBlock->size;					//Set the size of the merged block
                  Avail.erase(nextBlock);					//Erase the second block in the merger
	          it2--;							//Decrement iterator
	       }
	    }
         }
      }
   }
   if(found == false)
      cout << "The indicated block cannot be found" << endl << endl;
   else {
      cout << "Success in terminating a process" << endl << endl;
   }
}

/*********************************************************************
bestFitBlock()

	Finds the best-fit block in the list.
*********************************************************************/

Block bestFitBlock(int size) {
      Block bBlock = newBlock(0, 0, 0, "");					//Init the bBlock to empty
      for(it = Avail.begin(); it != Avail.end(); it++) {			//Loop through list
	 if(size <= it->size && (bBlock.sAddress == 0 || it->size < bBlock.size)) {	//If the block in list is big enough and is lesser than previous best blocks
	    bBlock.sAddress = it->sAddress;					//New best block set start address
	    bBlock.size = it->size;						//size
	    bBlock.pid = it->pid;						//process ID
	    bBlock.bid = it->bid;						//block ID
	 }
      }
   return bBlock;								//Return the best fit block
}

/*********************************************************************
newBlock()

	Creates a new block with starting address,size, pid, bid.
*********************************************************************/

Block newBlock(int sAddress, int size, int pid, string bid) {
   Block* block = new Block();							//Create a new Block

   block->sAddress = sAddress;							//starting address
   block->size = size;								//size
   block->pid = pid;								//process ID
   block->bid = bid;								//block ID

   return *block;								//Return created block
}

/***************************************************************************
printBlocks()

	Prints the list
****************************************************************************/
void printBlocks() {
   int sum = 0;

   cout << "List of available blocks" << endl;

   for(it = Avail.begin(); it != Avail.end(); it++) {
      cout << "Start Address =  " << it->sAddress << " Size = " << it->size << endl;
      sum += it->size;
   }

   cout << "Total size of the list = " << sum << endl << endl;

   cout << "List of blocks in use" << endl;

   sum = 0;
   if(InUse.size() == 0) {
      cout << "(none)" << endl;
   }
   else {
      for(it = InUse.begin(); it != InUse.end(); it++) {
	 sum += it->size;
         cout << "Start Address = " << it->sAddress << " Size = " << it->size
	      << " Process ID = " << it->pid << " Block ID = " << it->bid << endl;
      }
   }
   cout << "Total size of the list = " << sum << endl << endl;
}

