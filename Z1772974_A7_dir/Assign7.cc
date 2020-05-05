/*****************************************************************
Program: Assignment 7(extra credit)
Programmer: Carlton Hunt
zID: Z1772974
Due Date: 4/24/20
Class: CSCI 480 Section 2

Purpose: Simulates a FAT-12 using various types of transactions.
*****************************************************************/
#include<iostream>
#include<sstream>
#include<string>
#include<fstream>
#include<list>
#include<iomanip>

#define HOWOFTEN 6
#define MAXENTRIES 12
#define BLOCK 512
#define TSIZE 4096

using namespace std;

struct Entry {				//Struct directory entry
   string fileName;			//Struct file name
   int fileSize;			//Struct file size
   short fatLoc;			//Struct fat location
};

void printFAT();			//Function prototypes
void printDirectory();
Entry newEntry(string, int, short);
bool search(string);
void newFile(string, int, bool);
void deleteFile(string, bool);
void renameFile(string, string, bool);
void modFile(string, int);
void copyFile(string, string);
short getBlock(short);
short allocate(int);

list<Entry> directory;				//List of entries
list<Entry>::iterator it;			//Iterator for directory list

string line;					//Line to hold data
short FAT[TSIZE] = {-1};			//FAT Table first entry is -1
int printCount = 0;				//Print count

int main() {
   string fileName,
	  trans,
	  secondFileName,
	  newFileName;
   int fileSize;

   cout << "Beginning of the FAT Simulation" << endl << endl;

   directory.push_back(newEntry(".", BLOCK, 0));					//Push current directory
   directory.push_back(newEntry("..", 0, -2));						//Push parent into directory

   ifstream infile("data7.txt");							//Read in the data file
   while(getline(infile, line)) {							//While there is data
      if(printCount % HOWOFTEN == 0) {							//If print count is reached
         printDirectory();								//Print directory
	 printFAT();									//Print FAT
      }
      istringstream str(line);								//String for each line
      switch(line[0]) {									//Different transactions
         case 'C':									//Copy File
	    str >> trans >> fileName >> newFileName;
	    copyFile(fileName, newFileName);						//Call copy
	    break;
	 case 'D':									//Delete File
	    str >> trans >> fileName;							//Get line variables
	    deleteFile(fileName, false);						//Call delete
	    break;
	 case 'N':									//New File
            str >> trans >> fileName >> fileSize;					//Get line variables
	    newFile(fileName, fileSize, false);						//Call newFile
	    break;
	 case 'M':									//Modify File
	    str >> trans >> fileName >> fileSize;
	    modFile(fileName, fileSize);						//Call modifyFile
	    break;
	 case 'R':									//Rename File
   	    str >> trans >> fileName >> secondFileName;					//Get line variables
	    renameFile(fileName, secondFileName, false);				//Call renameFile
	    break;
	 case '?':									//Delimiter
	    break;
      }
   printCount++;									//Increment printCount
   }
   cout << endl << "End of the FAT simulation" << endl << endl;
   printDirectory();
   printFAT();

return 0;
}

/*********************************************************
newEntry()

	Creates a new entry and stores in the directory.
**********************************************************/

Entry newEntry(string name, int size, short fatLoc) {
   Entry* entry = new Entry();								//Create a new Entry
   entry->fileName = name;								//fileName
   entry->fileSize = size;								//fileSize
   entry->fatLoc = fatLoc;								//fatLoc

   return *entry;									//Return entry
}

/********************************************************
newFile()

	Creates a new file in the directory.
*********************************************************/

void newFile(string fileName, int fileSize, bool mod) {
   int blocksNeeded = 0,
       fatCount = 1,
       comp = 1;

   if(mod != true)
      cout << "Transaction:  Add a new file" << endl;					//Show transaction

   bool found = search(fileName);							//Search for the file name
   if(found == true) {									//File name is in the directory
      cout << "Error, file name, " << fileName
	   << "  already exists" << endl;
      return;
   }
   else {										//Name is not in the directory
      short fatLoc = allocate(fileSize);						//Get the fat index from the FAT
      directory.push_back(newEntry(fileName, fileSize, fatLoc));			//Push the entry
      if(mod != true) {									//Show transaction
         cout << "Successfully added a new file, " << fileName
	      << ", of size " << fileSize << endl;
      }
   }
}

/***************************************************************
allocate()

	Allocates the FAT to enter in the blocks for the file.
***************************************************************/

short allocate(int fileSize) {
   short previous = -1,									//Previous block
   blocks = 0;										//Number of blocks
   unsigned short start = getBlock(0);							//Starting index
   short current = start;								//Current is the starting index

   while(blocks * BLOCK < fileSize) {							//Need of more blocks
      current = getBlock(current);							//Current is the first unused block
      FAT[current] = -1;								//Set FAT to -1

      if(previous != -1) {								//Previous block is used
         FAT[previous] = current;							//FAT location at previous is current
      }
      previous = current;								//Set previous to current
      blocks++;										//Increment blocks
   }
return start;
}

/*************************************************************
getBlock()

	This method gets the first block in the FAT with 0.
*************************************************************/

short getBlock(short diff) {
   for(short block = diff; block < TSIZE; block++) {
      if(FAT[block] == 0)
         return block;
   }
}

/*************************************************************
copyFile()

	Copies the file.
*************************************************************/

void copyFile(string fileName, string newFileName) {
   Entry* entry;
   int fileSize;

   cout << "Transaction:   Copy a file" << endl;
   bool found = search(fileName);							//Search the filename
   if(found != true) {
      cout << "Error!   The old file name, " << fileName
	   << ", was not found." << endl;
      return;
   }
   else {
      bool found2 = search(newFileName);						//Search the new file name
      if(found2 == true) {
         cout << "Error!   The new file name, " << newFileName
	      << ", already exists." << endl;
	 return;
      }
      else {										//File names will be entered
	 for(it = directory.begin(); it != directory.end(); it++) {
	    if(it->fileName == fileName)
	       fileSize = it->fileSize;
         }
	 newFile(newFileName, fileSize, true);
      }
   }
   cout << "Successfully copied an existing file, " << fileName
	<< " to a new file, " << newFileName << endl;
}

/*****************************************************************
renameFile()

	Renames the file in the directory.
******************************************************************/

void renameFile(string fileName, string secondFileName, bool mod) {
   if(mod != true)
      cout << "Transaction:  Rename a file" << endl;

   bool found = search(fileName);							//Search the directory for the file name
   if(found != true) {
      cout << "Error!   The old file name, " << fileName
	   << ", was not found." << endl;
      return;
   }
   else {
      bool found2 = search(secondFileName);						//Search for the new file name
      if(found2 == true) {								//Name is already in the directory
         cout << "Error!   The new file name, " << secondFileName
	      << ", already exists." << endl;
	 return;
      }
      else {										//Rename the file
         for(it = directory.begin(); it != directory.end(); it++) {
	    if(it->fileName == fileName)						//File name matches
	    it->fileName = secondFileName;						//Rename the file
	 }
      if(mod != true) {
         cout << "Successfully changed the file name " << fileName
	      << " to " << secondFileName << endl;
      }
      }
   }
}

/****************************************************************
modFile()

	Modifies the file in the directory.
****************************************************************/

void modFile(string fileName, int fileSize) {
   cout << "Transaction:   Modify a file" << endl;

   bool found = search(fileName);							//Search for filename
   if(found != true) {
      cout << "Error!    The file name, " << fileName
	   << ", does not exist." << endl;
      return;
   }
   else {										//File was found
      newFile("temp", fileSize, true);							//Create a new file
      deleteFile(fileName, true);							//Delete the file
      renameFile("temp", fileName, true);   						//Rename the file
   }
   cout << "Successfully modified a file, " << fileName << endl;
}

/****************************************************************
deleteFile()

	Deletes the file from the directory and clears up FAT.
****************************************************************/

void deleteFile(string fileName, bool mod) {
   short fatLocation;
   int fatNum;
   if(mod != true)
      cout << "Transaction:  Delete a file" << endl;

   bool found = search(fileName);							//Search for the file name
   if(!found == true) {									//File name is already in the directory
      cout << "Error!   The file name, " << fileName
	   << ", was not found." << endl;
      return;
   }
   else {										//Filename isnt in directory
      for(it = directory.begin(); it != directory.end(); it++) {			//Loop through directory
         if(it->fileName == fileName) {							//Name matches
	    fatLocation = it->fatLoc;
	    directory.erase(it);							//Remove from directory
	    break;
         }
      }
      short i = fatLocation;								//Set the fatLocation
      while(i != -1) {									//While not eof
         short j = FAT[i];								//Table entry is j
         FAT[i] = 0;									//Set table entry to 0
         i = j;										//Next spot in table
      }
   }
   if(mod != true)
      cout << "Successfully deleted a file, " << fileName << endl;
}

/**************************************************************
search()

	Searches the directory for the same file name.
**************************************************************/

bool search(string name) {
   for(it = directory.begin(); it != directory.end(); it++) {
      if(name == it->fileName) {
         return true;
      }
   }
}

/************************************************************
printFAT()

	Prints the contents of the FAT.
************************************************************/

void printFAT() {
   cout << "Contents of the File Allocation Table" << endl;
   for(int i = 0; i < 240; i++) {
      if(i % MAXENTRIES == 0 && i != 0)
         cout << endl;
      cout << setw(6) << FAT[i];
   }
   cout << endl;
}

/**********************************************************
printDirectory()

	Prints the contents of the directory.
**********************************************************/

void printDirectory() {
   short index;
   int fileCount = 0,
       sizeTotal = 0;
   cout << "Directory Listing" << endl;
   for(it = directory.begin(); it != directory.end(); it++) {
      if(&it != nullptr)
         fileCount++;										//Increment counter
      sizeTotal += it->fileSize;								//Total the size
      cout << "File name: " << setw(20) << left << it->fileName 				//File name
	   << "File Size: " << setw(20) << right << it->fileSize 				//File size
	   << endl;
      cout << "Cluster(s) in use: ";
      if(it->fatLoc == -2)
	 cout << "(none)" << endl;
      else {											//Print clusters
         index = it->fatLoc;									//Get the fat start
	 if(index != -1) {									//If eof
	    cout << index << "   ";								//Print out the index
	 }
	 while(FAT[index] != -1) {								//While its not eof
	    index = FAT[index];									//Set index to the FAT location at previous index
	    cout << index << "   ";								//Print out index
	 }
         cout << endl;
      }
   }
   cout << "Files:   " << fileCount << "   " << "Total Size:   "
	<< sizeTotal << " bytes" << endl;
}
