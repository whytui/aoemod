#pragma once

#include <string>
#include <assert.h>
#include <list>
#include <algorithm>
#include "basicFilesHandling.h"
#include "AOE_structures_drs.h"
#include "mystrings.h"

// This files provides methods to read DRS files


using namespace std;


class DrsIncludedFile {
public:
	DrsIncludedFile(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType) {
		this->fileType = fileType;
		this->fileId = -1;
		this->dataSize = 0;
		this->rawData = NULL;
		this->localFileName = "";
		this->myOrderIndex = -1;
		this->tmpOffsetInFile = 0;
	}
	~DrsIncludedFile() {
		if (this->rawData != NULL) {
			free(this->rawData);
		}
		this->rawData = NULL;
		this->dataSize = 0;
	}
	long int fileId;
	std::string localFileName;
	unsigned char *rawData;
	long int dataSize;
	long int myOrderIndex; // -1=not set
	long int tmpOffsetInFile; // Used during file generation
private:
	AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType;
};


class DrsSetOfIncludedFiles {
public:
	DrsSetOfIncludedFiles() {
		this->fileType.SetFromDword(0);
		this->myOrderIndex = -1;
	}
	~DrsSetOfIncludedFiles() {
		this->ClearFiles();
	}

	AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType;
	std::list<DrsIncludedFile*> myFiles;
	long int myOrderIndex; // -1=not set

	void ClearFiles() {
		for each (DrsIncludedFile *f in this->myFiles)
		{
			delete f;
		}
		this->myFiles.clear();
	}
};

// Comparison for DrsIncludedFile type (using myOrderIndex)
static bool DrsIncludedFileHasLowerRankThan(DrsIncludedFile *first, DrsIncludedFile *second) {
	if (first->myOrderIndex < 0) {
		if (second->myOrderIndex < 0) {
			return first->fileId <= second->fileId;
		}
		return false; // "no_rank">"has_a_rank"
	}
	if (second->myOrderIndex < 0) {
		return true; // "has_a_rank"<"no_rank"
	}
	return (first->myOrderIndex < second->myOrderIndex);
}
// Comparison for DrsSetOfIncludedFiles type (using myOrderIndex)
static bool DrsSetOfFilesHasLowerRankThan(DrsSetOfIncludedFiles *first, DrsSetOfIncludedFiles *second) {
	if (first->myOrderIndex < 0) {
		if (second->myOrderIndex < 0) {
			return &first->myFiles < &second->myFiles; // If no available rank: Arbitrary but not random once pointers are allocated...
		}
		return false; // "no_rank">"has_a_rank"
	}
	if (second->myOrderIndex < 0) {
		return true; // "has_a_rank"<"no_rank"
	}
	return (first->myOrderIndex < second->myOrderIndex);
}


class DrsFileHelper {
public:
	DrsFileHelper();
	~DrsFileHelper();

	const int maxAllowedTableCount = 20; // Higher value will raise an exception
	const bool useDefaultCopyright = true; // Use standard DRS files copyright string. Turtle pack does not support other strings.


	// Removes all files/file types from current working set
	void ResetCurrentWorkingSet();

	// Add file type, if not already present
	DrsSetOfIncludedFiles *AddFileType(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType);

	// Returns tyoe info, or NULL if not found
	DrsSetOfIncludedFiles *GetFileTypeInfo(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType);

	// Add a file (and file type if necessary)
	DrsIncludedFile *AddFile(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType, long int fileId, string filename);

	// Remove a file type *if it is unused* (no file in it)
	// Returns true if a file type was removed.
	bool RemoveFileType(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType);

	// Remove the all files that corresponds to type and ID.
	// Returns true if one file (or more) was removed.
	bool RemoveFile(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType, long int fileId);

	string GetDrsMainObjectsList(string filename);

	// Get string representation of errors that happened in last operation
	string GetLastErrors() const { return this->errorLog; }

	void TestCreateDrs(string filename);

private:
	size_t fileTotalSize;
	FILE *myFile;
	string errorLog;
	std::list<DrsSetOfIncludedFiles*> currentDrsWorkingSet;

	// Open a file. Throw an exception if failed. Fails if a file is already open.
	void FileOpen(string filename, bool readOnly);

	// Close current file, if any. Returns true if a file has actually been closed.
	bool FileClose();

	// Read data from file, throws an exception if an error occurs (end of file reached, etc)
	void ReadCheck(size_t offset, void *dest, size_t destSize, size_t elementSize, size_t count);

};
