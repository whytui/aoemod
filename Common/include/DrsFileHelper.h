#pragma once

#include <string>
#include <assert.h>
#include "basicFilesHandling.h"
#include "AOE_structures_drs.h"

// This files provides methods to read DRS files


using namespace std;



class DrsFileHelper {
public:
	DrsFileHelper();

	const int maxAllowedTableCount = 20; // Higher value will raise an exception

	string GetDrsMainObjectsList(string filename);

	// Get string representation of errors that happened in last operation
	string GetLastErrors() const { return this->errorLog; }

private:
	size_t fileTotalSize;
	FILE *myFile;
	string errorLog;

	// Open a file. Throw an exception if failed. Fails if a file is already open.
	void FileOpen(string filename);

	// Close current file, if any. Returns true if a file has actually been closed.
	bool FileClose();

	// Read data from file, throws an exception if an error occurs (end of file reached, etc)
	void ReadCheck(size_t offset, void *dest, size_t destSize, size_t elementSize, size_t count);

};
