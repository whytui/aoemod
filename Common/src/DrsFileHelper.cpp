#include "../include/DrsFileHelper.h"

// This files provides methods to read DRS files


using namespace AOE_STRUCTURES;


DrsFileHelper::DrsFileHelper() {
	this->myFile = NULL;
	this->fileTotalSize = 0;
	this->errorLog = "";
	this->infoLog = "";
}

DrsFileHelper::~DrsFileHelper() {
	this->ResetCurrentWorkingSet();
}


void DrsFileHelper::ResetCurrentWorkingSet() {
	for each (DrsSetOfIncludedFiles *fs in this->currentDrsWorkingSet)
	{
		delete fs;
	}
	this->currentDrsWorkingSet.clear();
}

// Add file type, if not already present
DrsSetOfIncludedFiles *DrsFileHelper::AddFileType(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType) {
	DrsSetOfIncludedFiles *existing = this->GetFileTypeInfo(fileType);
	if (existing != NULL) { return existing; }
	DrsSetOfIncludedFiles *s = new DrsSetOfIncludedFiles();
	s->fileType = fileType;
	long int maxExistingIndex = -1;
	for (auto it = this->currentDrsWorkingSet.begin(); it != this->currentDrsWorkingSet.end(); it++) {
		if ((maxExistingIndex < 0) || (maxExistingIndex < (*it)->myOrderIndex)) {
			maxExistingIndex = (*it)->myOrderIndex;
		}
	}
	s->myOrderIndex = maxExistingIndex + 1;
	this->currentDrsWorkingSet.push_back(s);
	return s;
}


DrsSetOfIncludedFiles *DrsFileHelper::GetFileTypeInfo(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType) {
	auto it = std::find_if(this->currentDrsWorkingSet.begin(), this->currentDrsWorkingSet.end(),
		[fileType](DrsSetOfIncludedFiles *s){ return s->fileType.GetAsDword() == fileType.GetAsDword(); }
	);
	if (it != this->currentDrsWorkingSet.end()) {
		return *it; // found in list
	}
	return NULL;
}


// Returns the first file whose ID matches specified one
DrsIncludedFile *DrsFileHelper::FindFileWithId(long int fileId) {
	DrsSetOfIncludedFiles *firstFound = NULL;
	for (auto it = this->currentDrsWorkingSet.begin(); it != this->currentDrsWorkingSet.end(); it++) {
		DrsSetOfIncludedFiles *cur = *it;
		for (auto it_f = cur->myFiles.begin(); it_f != cur->myFiles.end(); it_f++) {
			if ((*it_f)->fileId == fileId) {
				return *it_f;
			}
		}
	}
	return NULL;
}


// Add a file (and file type if necessary)
// Does not add if the ID is already used (returns NULL)
// buffer is copied into a new buffer (you probably should free it after the call)
DrsIncludedFile *DrsFileHelper::AddFile(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType, long int fileId, void *buffer, long int size) {
	if (!buffer || (size <= 0)) {
		return NULL;
	}
	DrsSetOfIncludedFiles *s = this->AddFileType(fileType);
	auto it = std::find_if(s->myFiles.begin(), s->myFiles.end(),
		[fileId](DrsIncludedFile *s){ return s->fileId == fileId; }
	);
	if (it != s->myFiles.end()) {
		return NULL; // already in list - do not add
	}

	DrsIncludedFile *f = new DrsIncludedFile(fileType);
	f->fileId = fileId;
	f->localFileName = "";
	long int maxExistingIndex = -1;
	for (auto it = s->myFiles.begin(); it != s->myFiles.end(); it++) {
		if ((maxExistingIndex < 0) || (maxExistingIndex < (*it)->myOrderIndex)) {
			maxExistingIndex = (*it)->myOrderIndex;
		}
	}
	f->myOrderIndex = maxExistingIndex + 1;
	f->dataSize = size;
	f->rawData = (unsigned char *)malloc(size);
	memcpy_s(f->rawData, size, buffer, size);
	s->myFiles.push_back(f);
	return f;
}


// Add a file (and file type if necessary)
// Does not add if the ID is already used (returns NULL)
DrsIncludedFile *DrsFileHelper::AddFile(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType, long int fileId, string filename) {
	DrsSetOfIncludedFiles *s = this->AddFileType(fileType);
	auto it = std::find_if(s->myFiles.begin(), s->myFiles.end(),
		[fileId](DrsIncludedFile *s){ return s->fileId == fileId; }
	);
	if (it != s->myFiles.end()) {
		return NULL; // already in list - do not add.
	}
	
	if (!CheckFileExistence(filename.c_str())) {
		return NULL;
	}
	FILE *tmpFile;
	errno_t e = fopen_s(&tmpFile, filename.c_str(), "rb");
	if (e) {
		this->errorLog += "Could not open input file";
		return NULL;
	}
	fseek(tmpFile, 0, SEEK_END);
	long int tmpFileSize = ftell(tmpFile);
	fseek(tmpFile, 0, SEEK_SET);
	if (tmpFileSize <= 0) {
		this->errorLog += "Can't add an empty file";
		return NULL;
	}

	DrsIncludedFile *f = new DrsIncludedFile(fileType);
	f->fileId = fileId;
	f->localFileName = filename;
	long int maxExistingIndex = -1;
	for (auto it = s->myFiles.begin(); it != s->myFiles.end(); it++) {
		if ((maxExistingIndex < 0) || (maxExistingIndex < (*it)->myOrderIndex)) {
			maxExistingIndex = (*it)->myOrderIndex;
		}
	}
	f->myOrderIndex = maxExistingIndex + 1;
	f->dataSize = tmpFileSize;
	f->rawData = (unsigned char *) malloc(tmpFileSize);
	size_t readElemCount = fread_s(f->rawData, tmpFileSize, 1, tmpFileSize, tmpFile);
	fclose(tmpFile);
	s->myFiles.push_back(f);
	return f;
}


// Remove a file type *if it is unused* (no file in it)
// Returns true if a file type was removed.
bool DrsFileHelper::RemoveFileType(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType) {
	int matchCount = 0;
	//bool foundOneContainingFiles = false;
	DrsSetOfIncludedFiles *firstFound = NULL;
	for (auto it = this->currentDrsWorkingSet.begin(); it != this->currentDrsWorkingSet.end(); it++) {
		DrsSetOfIncludedFiles *cur = *it;
		if (cur->fileType.GetAsDword() == fileType.GetAsDword()) {
			matchCount++;
			if (firstFound == NULL) {
				firstFound = cur;
			}
			if (cur->myFiles.size() > 0) {
				//foundOneContainingFiles = true;
				return false; // We don't even need to go further. Forbidden: can't delete a "set" containing files
			}
		}
	}

	if ((matchCount == 0) || (firstFound == NULL)) {
		return false; // Not found = nothing to delete
	}
	if (firstFound->myFiles.size() > 0) { // Redundant check
		return false; // Forbidden: can't delete a "set" containing files
	}

	// Note: remove_if deletes all matches, but we checked there is only 1.
	auto it = std::remove_if(this->currentDrsWorkingSet.begin(), this->currentDrsWorkingSet.end(),
		[fileType](DrsSetOfIncludedFiles *s){ return s->fileType.GetAsDword() == fileType.GetAsDword(); }
	);
	bool found = (it != this->currentDrsWorkingSet.end());
	this->currentDrsWorkingSet.erase(it, this->currentDrsWorkingSet.end());
	return found;
}


// Remove the all files that corresponds to type and ID.
// Returns true if one file (or more) was removed.
bool DrsFileHelper::RemoveFile(AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE fileType, long int fileId) {
	DrsSetOfIncludedFiles *s = this->GetFileTypeInfo(fileType);
	if (s == NULL) { return false; }
	std::list<DrsIncludedFile*> objectsToDelete;
	auto it = std::remove_if(s->myFiles.begin(), s->myFiles.end(),
		[&objectsToDelete, fileId](DrsIncludedFile *s){
		if (s->fileId == fileId) {
			objectsToDelete.push_back(s); // save pointer !
			return true; // can delete
		}
		return false;
	}
	);
	bool found = (it != s->myFiles.end());
	s->myFiles.erase(it, s->myFiles.end());
	// Now: delete all the orphan objects (files that were referenced by deleted list pointers)
	for each (auto s in objectsToDelete)
	{
		delete s;
	}
	objectsToDelete.clear();
	return found;
}


// Set a new file category
// Returns true if successful
bool DrsFileHelper::ChangeFileCategory(long int fileId, AOE_STRUCTURES::STRUCT_DRS_FILE_TYPE newFileType) {
	DrsIncludedFile *oldFile = this->FindFileWithId(fileId);
	if (!oldFile) {
		return false;
	}
	if (oldFile->GetFileType().GetAsDword() == newFileType.GetAsDword()) {
		return true; // Nothing to do
	}

	void *buffer = oldFile->rawData;
	long int bufferSize = oldFile->dataSize;
	// Trick to preserve buffer while old file object is destroyed: detach file from the object (and keep pointer)
	oldFile->dataSize = 0;
	oldFile->rawData = NULL;
	this->RemoveFile(oldFile->GetFileType(), fileId);
	oldFile = NULL;
	this->AddFile(newFileType, fileId, buffer, bufferSize);
	free(buffer);
	buffer = NULL;
	return true;
}


// Open a file. Throw an exception if failed. Fails if a file is already open.
void DrsFileHelper::FileOpen(string filename, bool readOnly) {
	if (this->myFile != NULL) {
		throw exception("A file is already open");
	}
	this->fileTotalSize = 0;
	errno_t e = fopen_s(&this->myFile, filename.c_str(), readOnly ? "rb" : "wb");
	if (e) {
		throw exception("Could not open file for read");
	}
	fseek(this->myFile, 0, SEEK_END);
	this->fileTotalSize = ftell(this->myFile);
	fseek(this->myFile, 0, SEEK_SET); // SEEK_SET=from begin of file
}


// Close current file, if any. Returns true if a file has actually been closed.
bool DrsFileHelper::FileClose() {
	this->fileTotalSize = 0;
	if (this->myFile != NULL) {
		fclose(this->myFile);
		this->myFile = NULL;
		return true;
	}
	return false;
}


// Read data from file, throws an exception if an error occurs (end of file reached, etc)
void DrsFileHelper::ReadCheck(size_t offset, void *dest, size_t destSize, size_t elementSize, size_t count) {
	if (!dest) {
		throw exception("dest ptr is null in ReadCheck");
	}
	if ((destSize <= 0) || (elementSize <= 0) || (count <= 0)) {
		throw exception("bad parameters in ReadCheck");
	}
	if (this->myFile == NULL) {
		throw exception("no file to read in ReadCheck");
	}
	if (offset + elementSize * count > this->fileTotalSize) {
		throw exception("trying to read beyond file size");
	}
	fseek(this->myFile, offset, SEEK_SET);
	size_t readElemCount = fread_s(dest, destSize, elementSize, count, this->myFile);
	if (readElemCount < count) {
		throw exception("could not read all elements");
	}
}


// Read a DRS file. All internal data from previous file/previous manipulations is lost
// Returns true if successful
// Use GetLastErrors and GetLastInfos to retrieve errors & info log from this operation 
bool DrsFileHelper::ReadDrsFile(string filename) {
	this->errorLog = "";
	this->infoLog = "";
	this->ResetCurrentWorkingSet();
	if (!CheckFileExistence(filename.c_str())) {
		this->errorLog += "File does not exist\r\n";
		return false;
	}
	
	try {
		this->FileOpen(filename, true);
	}
	catch (exception ex) {
		this->errorLog += ex.what();
		this->errorLog += "\r\n";
		return false;
	}
	
	STRUCT_DRS_FILE myDrsObj;
	memset(&myDrsObj, 0, AOE_STRUCTURES::DrsHeaderSize);
	if (this->fileTotalSize < AOE_STRUCTURES::DrsHeaderSize) {
		// ERROR
		this->FileClose();
		this->errorLog += "File size is too small to read header section\r\n";
		return false;
	}

	std::set<long int> allObjectIDs;
	STRUCT_DRS_TABLE *drsTablesArray = NULL;
	STRUCT_DRS_TABLE_DATA *drsTablesData = NULL;
	STRUCT_SLP_FRAME_HEADER *frameHeaderArray = NULL;
	STRUCT_SLP_FRAME_ROW_EDGE *curEdgesArray = NULL;
	unsigned long int *curCommandOffsetsArray = NULL;
	try {
		// Read DRS Header
		this->ReadCheck(0, &myDrsObj, AOE_STRUCTURES::DrsHeaderSize, AOE_STRUCTURES::DrsHeaderSize, 1);

		int drsTabCount = myDrsObj.includedTableCount;
		char cmpBuffer[20];
		memset(cmpBuffer, 0, sizeof(cmpBuffer));
		strncpy_s(cmpBuffer, myDrsObj.version, 4);
		if (strncmp(cmpBuffer, "1.00", 4) != 0) {
			this->errorLog += "Unexpected DRS version info: ";
			this->errorLog += cmpBuffer;
			this->errorLog += "\r\n";
		}
		memset(cmpBuffer, 0, sizeof(cmpBuffer));
		strncpy_s(cmpBuffer, myDrsObj.fileType, 12);
		if (strncmp(cmpBuffer, AOE_STRUCTURES::DrsTableTribeType, sizeof(AOE_STRUCTURES::DrsTableTribeType)) != 0) {
			this->errorLog += "Unexpected fileType: ";
			this->errorLog += cmpBuffer;
			this->errorLog += "\r\n";
		}
		if (drsTabCount > this->maxAllowedTableCount) {
			throw exception("Table count is really huge, file must be corrupted");
		}
		if (myDrsObj.firstFileDataOffset <= AOE_STRUCTURES::DrsHeaderSize) {
			throw exception("First file offset is below header size, file must be corrupted");
		}
		this->infoLog += "FirstFileOffset=";
		this->infoLog += to_string(myDrsObj.firstFileDataOffset);

		// DRS table is just after header (list of file types)
		drsTablesArray = (STRUCT_DRS_TABLE*)malloc(sizeof(STRUCT_DRS_TABLE) * drsTabCount);
		this->ReadCheck(AOE_STRUCTURES::DrsHeaderSize, drsTablesArray, sizeof(STRUCT_DRS_TABLE) * drsTabCount, sizeof(STRUCT_DRS_TABLE), drsTabCount);

		// Loop on DRS tables (file types details)
		unsigned long int lowestFileOffset = 0;
		long int numberOfAddedFiles = 0;
		for (int i = 0; i < drsTabCount; i++) {
			this->infoLog += "\r\n";
			this->infoLog += "DRS.table#";
			this->infoLog += std::to_string(i);
			this->infoLog += " type=";
			this->infoLog += drsTablesArray[i].typeName.Get4LettersExtension().GetAsCharPtr();
			this->infoLog += " info_offset=";
			this->infoLog += std::to_string(drsTablesArray[i].fileInfoOffsetInDrsFile);
			this->infoLog += " file_count=";
			this->infoLog += std::to_string(drsTablesArray[i].filesCount);
			this->infoLog += "\r\n";
			bool isSlp = drsTablesArray[i].IsSlp();

			if (drsTablesArray[i].filesCount > 0) {
				// If 0, NOT an error (seen in original file), but ignore...

				int drsTablesDataSize = sizeof(STRUCT_DRS_TABLE_DATA);
				drsTablesData = (STRUCT_DRS_TABLE_DATA *)malloc(drsTablesDataSize * drsTablesArray[i].filesCount);

				this->ReadCheck(drsTablesArray[i].fileInfoOffsetInDrsFile, drsTablesData,
					drsTablesDataSize * drsTablesArray[i].filesCount, drsTablesDataSize, drsTablesArray[i].filesCount);

				for (int j = 0; j < drsTablesArray[i].filesCount; j++) {
					unsigned long int curObjOffset = drsTablesData[j].offsetInDrsFile;
					long int curObjectSize = drsTablesData[j].objectSize;
					if ((curObjOffset < lowestFileOffset) || (lowestFileOffset == 0)) {
						lowestFileOffset = curObjOffset;
					}
					if (j > 0) { this->infoLog += "\r\n"; }
					long int objectId = drsTablesData[j].objectId;
					this->infoLog += std::to_string(j);
					this->infoLog += " objId=";
					this->infoLog += std::to_string(objectId);
					this->infoLog += " dataOffset=";
					this->infoLog += std::to_string(curObjOffset);
					this->infoLog += " dataSize=";
					this->infoLog += std::to_string(drsTablesData[j].objectSize);

					if (allObjectIDs.find(objectId) != allObjectIDs.end()) {
						this->errorLog += "ID #";
						this->errorLog += to_string(objectId);
						this->errorLog += " is used by more than 1 object\r\n";
					}
					allObjectIDs.insert(objectId);

					if (drsTablesData[j].objectSize > 0) {
						// TODO optim: add a primitive that "AddFile" directly from input file, without using this temporary buffer.
						void *tempBuffer = malloc(curObjectSize);
						this->ReadCheck(curObjOffset, tempBuffer, curObjectSize, 1, curObjectSize);
						DrsIncludedFile *curFile = this->AddFile(drsTablesArray[i].typeName, objectId, tempBuffer, curObjectSize);
						if (curFile) {
							curFile->myOrderIndex = numberOfAddedFiles;
							numberOfAddedFiles++;
						}
						free(tempBuffer); tempBuffer = NULL;
					}

					if (isSlp) {
						// TO REMOVE/move somewhere else (was for testing purpose)
						// SLP file format...
						STRUCT_SLP_FILE_HEADER slpHeader;
						this->ReadCheck(curObjOffset, &slpHeader, sizeof(STRUCT_SLP_FILE_HEADER), sizeof(STRUCT_SLP_FILE_HEADER), 1);
						memset(cmpBuffer, 0, sizeof(cmpBuffer));
						strncpy_s(cmpBuffer, slpHeader.version, 4);
						if (strncmp(cmpBuffer, "2.0N", 4) != 0) {
							this->errorLog += "Unexpected SLP version info: ";
							this->errorLog += cmpBuffer;
							this->errorLog += "\r\n";
						}
						int shapeCount = slpHeader.shapeCount;
						this->infoLog += " SLP_desc=";
						this->infoLog += slpHeader.description;
						this->infoLog += " shpCount=";
						this->infoLog += to_string(shapeCount);

						// After header: frame header table
						STRUCT_SLP_FRAME_HEADER *frameHeaderArray = (STRUCT_SLP_FRAME_HEADER *)malloc(sizeof(STRUCT_SLP_FRAME_HEADER) * shapeCount);
						long int frHdrOffset = curObjOffset + sizeof(STRUCT_SLP_FILE_HEADER);
						this->ReadCheck(frHdrOffset,
							frameHeaderArray, sizeof(STRUCT_SLP_FILE_HEADER)*shapeCount, sizeof(STRUCT_SLP_FILE_HEADER), shapeCount);

						for (int shpIndex = 0; shpIndex < shapeCount; shpIndex++) {
							int x = frameHeaderArray[shpIndex].xSize;
							int y = frameHeaderArray[shpIndex].ySize;
							long int outlineTableOffset = frameHeaderArray[shpIndex].outlineTableOffset;
							long int cmdTableOffset = frameHeaderArray[shpIndex].commandTableOffset;
							int rowCount = frameHeaderArray[shpIndex].ySize;
							int colCount = frameHeaderArray[shpIndex].xSize;

							// Then: row-by-row transparency info (STRUCT_SLP_FRAME_ROW_EDGE) cf outline info offset
							curEdgesArray = (STRUCT_SLP_FRAME_ROW_EDGE*)malloc(sizeof(STRUCT_SLP_FRAME_ROW_EDGE) * rowCount);
							this->ReadCheck(curObjOffset + outlineTableOffset, curEdgesArray, sizeof(STRUCT_SLP_FRAME_ROW_EDGE) * rowCount, sizeof(STRUCT_SLP_FRAME_ROW_EDGE), rowCount);
							for (int k = 0; k < rowCount; k++) {
								int l = curEdgesArray[k].leftSpace;
								int r = curEdgesArray[k].rightSpace;
								if (l > colCount) {
									this->errorLog += "leftSpace bigger than X size\r\n";
								}
								if (r > colCount) {
									this->errorLog += "rightSpace bigger than X size\r\n";
								}
							}

							// Then: command offsets
							curCommandOffsetsArray = (unsigned long int *)malloc(sizeof(unsigned long int) * rowCount);
							this->ReadCheck(curObjOffset + cmdTableOffset, curCommandOffsetsArray, sizeof(unsigned long int) * rowCount, sizeof(unsigned long int), rowCount);
							// Consistency: first command offset should correspond to "current position", because commands come just after...
							if (cmdTableOffset + rowCount * sizeof(unsigned long int) != curCommandOffsetsArray[0]) {
								this->errorLog += "Inconsistent offset after command offsets table\r\n";
							}
							// Remark: reading commands sequencially should be equivalent as using offsets
							// TODO: SLP commands format ?
							// TODO: could add a private method "read next bytes" without a fseek

							free(curCommandOffsetsArray); curCommandOffsetsArray = NULL;
							free(curEdgesArray); curEdgesArray = NULL;
						}
						free(frameHeaderArray); frameHeaderArray = NULL;
					}
				}
			}
			this->infoLog += "\r\n";
			if (drsTablesData != NULL) { free(drsTablesData); drsTablesData = NULL; }
		}

		if (lowestFileOffset != myDrsObj.firstFileDataOffset) {
			// ERROR
			this->errorLog += "Inconsistent file offset\r\n";
		}

		free(drsTablesArray); drsTablesArray = NULL;
	}
	catch (exception ex) {
		this->FileClose();
		this->errorLog += "ERROR: ";
		this->errorLog += ex.what();
		this->errorLog += "\r\n";
		if (drsTablesArray != NULL) {
			free(drsTablesArray); drsTablesArray = NULL;
		}
		if (drsTablesData != NULL) {
			free(drsTablesData); drsTablesData = NULL;
		}
		if (frameHeaderArray != NULL) {
			free(frameHeaderArray); frameHeaderArray = NULL;
		}
		if (curEdgesArray != NULL) {
			free(curEdgesArray); curEdgesArray = NULL;
		}
		if (curCommandOffsetsArray != NULL) {
			free(curCommandOffsetsArray); curCommandOffsetsArray = NULL;
		}
		return false;
	}
	this->FileClose();
	return true;
}


// Save internal data as a DRS file
void DrsFileHelper::ExportToDrsFile(string filename) {
	this->errorLog = "";
	//if (CheckFileExistence(filename.c_str())) {
		//this->errorLog += "File already exists\r\n";
		//return;
	//}

	try {
		this->FileOpen(filename, false);
	}
	catch (exception ex) {
		this->errorLog += ex.what();
	}
	string result = "";

	// Pre-compute some sizes/offsets
	int drsTableSizeOf = sizeof(STRUCT_DRS_TABLE); // sizeof(STRUCT_DRS_TABLE)
	int drsTablesInfoSizeOf = sizeof(STRUCT_DRS_TABLE_DATA);
	int drsTabCount = this->currentDrsWorkingSet.size(); // Nb of file types
	long int drsTablesSizeInBytes = drsTableSizeOf * drsTabCount;
	long int totalNumberOfFiles = 0; // Sum of fileCount for each type
	for each (auto fs in this->currentDrsWorkingSet)
	{
		totalNumberOfFiles += fs->myFiles.size();
	}

	long int drsFileInfoSizeInBytes = drsTablesInfoSizeOf * totalNumberOfFiles;
	long int offsetOfFirstFileInfo = AOE_STRUCTURES::DrsHeaderSize + drsTablesSizeInBytes;
	long int offsetOfFirstFileData = AOE_STRUCTURES::DrsHeaderSize + drsTablesSizeInBytes + drsFileInfoSizeInBytes;

	STRUCT_DRS_TABLE *drsTablesArray = (STRUCT_DRS_TABLE*)malloc(drsTablesSizeInBytes);
	STRUCT_DRS_TABLE_DATA *drsTablesInfo = (STRUCT_DRS_TABLE_DATA *)malloc(drsTablesInfoSizeOf * drsFileInfoSizeInBytes); // file info array for ALL types
	memset(drsTablesArray, 0, drsTablesSizeInBytes);
	memset(drsTablesInfo, 0, drsTablesInfoSizeOf * drsFileInfoSizeInBytes);

	try {
		if (drsTabCount <= 0) {
			throw exception("drsTabCount <= 0");
		}

		STRUCT_DRS_FILE myDrsObj;
		if (useDefaultCopyright) {
			memcpy_s(myDrsObj.copyrightHeader, sizeof(myDrsObj.copyrightHeader), DrsFileDefaultCopyright, sizeof(DrsFileDefaultCopyright));
		} else {
			memcpy_s(myDrsObj.copyrightHeader, sizeof(myDrsObj.copyrightHeader), "RockNRor", 9);
		}
		memcpy_s(myDrsObj.fileType, sizeof(myDrsObj.fileType), AOE_STRUCTURES::DrsTableTribeType, sizeof(AOE_STRUCTURES::DrsTableTribeType));
		myDrsObj.version[0] = '1';
		myDrsObj.version[1] = '.';
		myDrsObj.version[2] = '0';
		myDrsObj.version[3] = '0';
		myDrsObj.includedTableCount = drsTabCount;
		myDrsObj.firstFileDataOffset = offsetOfFirstFileData;


		// Loop to fill drsTablesArray and drsTablesInfo (in memory)
		long int drsTabIndex = 0;
		long int curFileIndexInGlobalArray = 0; // index, including ALL files from all types
		long int curOffsetForFileRawData = offsetOfFirstFileData;
		for (auto it_fs = this->currentDrsWorkingSet.begin(); it_fs != this->currentDrsWorkingSet.end(); it_fs++) {
			DrsSetOfIncludedFiles *fileSet = *it_fs;
			drsTablesArray[drsTabIndex].filesCount = fileSet->myFiles.size();
			drsTablesArray[drsTabIndex].fileInfoOffsetInDrsFile = offsetOfFirstFileInfo + sizeof(STRUCT_DRS_TABLE_DATA)*drsTabIndex;
			drsTablesArray[drsTabIndex].typeName = fileSet->fileType;

			// Included files for current type...
			for (auto it_f = fileSet->myFiles.begin(); it_f != fileSet->myFiles.end(); it_f++) {
				DrsIncludedFile *curFileInfo = *it_f;
				drsTablesInfo[curFileIndexInGlobalArray].objectId = curFileInfo->fileId;
				drsTablesInfo[curFileIndexInGlobalArray].objectSize = curFileInfo->dataSize;
				drsTablesInfo[curFileIndexInGlobalArray].offsetInDrsFile = curOffsetForFileRawData;
				curFileInfo->tmpOffsetInFile = curOffsetForFileRawData;
				curOffsetForFileRawData += curFileInfo->dataSize;
				curFileIndexInGlobalArray++;
			}
			drsTabIndex++;
		}


		// Write header, drsTablesArray and drsTablesInfo content to output file
		fseek(this->myFile, 0, SEEK_SET);
		fwrite(&myDrsObj, AOE_STRUCTURES::DrsHeaderSize, 1, this->myFile);
		fwrite(drsTablesArray, drsTableSizeOf, drsTabCount, this->myFile);
		fwrite(drsTablesInfo, drsTablesInfoSizeOf, totalNumberOfFiles, this->myFile);

		// Loop to write files (in correct order !)
		drsTabIndex = 0;
		curFileIndexInGlobalArray = 0; // index, including ALL files from all types
		long int writtenBytesForFileData = 0;
		curOffsetForFileRawData = offsetOfFirstFileData;
		for (auto it_fs = this->currentDrsWorkingSet.begin(); it_fs != this->currentDrsWorkingSet.end(); it_fs++) {
			DrsSetOfIncludedFiles *fileSet = *it_fs;

			// Included files for current type...
			for (auto it_f = fileSet->myFiles.begin(); it_f != fileSet->myFiles.end(); it_f++) {
				DrsIncludedFile *curFileInfo = *it_f;

				if (curFileInfo->tmpOffsetInFile != curOffsetForFileRawData) {
					this->errorLog += "Error with calculated offsets in file writing";
				}

				fwrite(curFileInfo->rawData, 1, curFileInfo->dataSize, this->myFile);
				writtenBytesForFileData += curFileInfo->dataSize;
				curOffsetForFileRawData += curFileInfo->dataSize;
			}
			drsTabIndex++;
		}
	}
	catch (exception ex) {
		this->errorLog += ex.what();
		this->errorLog += "\r\n";
	}

	if (drsTablesArray) { free(drsTablesArray); drsTablesArray = NULL; }
	if (drsTablesInfo) { free(drsTablesInfo); drsTablesInfo = NULL; }
	this->FileClose();
}


// Save an included file as...
bool DrsFileHelper::ExportIncludedFile(long int fileId, string filename) {
	DrsIncludedFile *fileObj = this->FindFileWithId(fileId);
	if (!fileObj) { return false; }
	if ((fileObj->dataSize <= 0) || (fileObj->rawData == NULL)) { return false; }
	if (filename.empty()) { return false; }
	FILE *file;
	if (fopen_s(&file, filename.c_str(), "wb") != 0) {
		return false;
	}
	size_t writtenBytes = fwrite(fileObj->rawData, 1, fileObj->dataSize, file);
	fclose(file);
	return (writtenBytes == fileObj->dataSize);
}

// Sort all files using IDs
void DrsFileHelper::SortFilesUsingId() {
	// Collect pointers to all files
	list<DrsIncludedFile*> allFiles;
	for each (DrsSetOfIncludedFiles *ws in this->currentDrsWorkingSet)
	{
		for each (DrsIncludedFile *f in ws->myFiles)
		{
			allFiles.push_back(f);
			f->myOrderIndex = f->fileId;
		}
	}
	allFiles.sort(DrsIncludedFileHasLowerRankThan);
	// Recompute consecutive ranks 1->n
	int curIndex = 1;
	for (auto it = allFiles.begin(); it != allFiles.end(); it++) {
		(*it)->myOrderIndex = curIndex;
		curIndex++;
	}
	// Now sort (update) each list (in each file category)
	for each (DrsSetOfIncludedFiles *ws in this->currentDrsWorkingSet)
	{
		ws->myFiles.sort(DrsIncludedFileHasLowerRankThan);
	}
}


// Move up a file in files list (updates index and position in list)
bool DrsFileHelper::FileOrderMoveUp(long int fileId) {
	DrsIncludedFile *f = this->FindFileWithId(fileId);
	if (!f) { return false; }
	DrsSetOfIncludedFiles *drsSet = this->GetFileTypeInfo(f->GetFileType());
	if (!drsSet) { return false; }
	DrsIncludedFile *previous = NULL;
	for (auto it = drsSet->myFiles.begin(); it != drsSet->myFiles.end(); it++) {
		if ((*it)->fileId == fileId) {
			DrsIncludedFile *curToExchange = *it;
			if (previous == NULL) {
				return false; // Can't move up, it is already first
			}
			// Exchange positions (and indexes)
			long int tmpIndex = previous->myOrderIndex;
			previous->myOrderIndex = curToExchange->myOrderIndex;
			curToExchange->myOrderIndex = tmpIndex;
			// exchange positions: in fact, keep pointers, exchange data so we don't have to modify the list
			DrsIncludedFile tmpSwap = DrsIncludedFile(*previous);
			previous->dataSize = curToExchange->dataSize;
			previous->fileId = curToExchange->fileId;
			previous->localFileName = curToExchange->localFileName;
			previous->myOrderIndex = curToExchange->myOrderIndex;
			previous->rawData = curToExchange->rawData;
			previous->tmpOffsetInFile = curToExchange->tmpOffsetInFile;
			curToExchange->dataSize = tmpSwap.dataSize;
			curToExchange->fileId = tmpSwap.fileId;
			curToExchange->localFileName = tmpSwap.localFileName;
			curToExchange->myOrderIndex = tmpSwap.myOrderIndex;
			curToExchange->rawData = tmpSwap.rawData;
			curToExchange->tmpOffsetInFile = tmpSwap.tmpOffsetInFile;
			tmpSwap.rawData = NULL; // Make sure destructor won't free the memory !!! We only used it as a temporary container.
			tmpSwap.dataSize = 0;
			tmpSwap.fileId = -1;
			return true;
		}
		previous = *it;
	}
	return false;
}


// Move down a file in files list (updates index and position in list)
bool DrsFileHelper::FileOrderMoveDown(long int fileId) {
	DrsIncludedFile *f = this->FindFileWithId(fileId);
	if (!f) { return false; }
	DrsSetOfIncludedFiles *drsSet = this->GetFileTypeInfo(f->GetFileType());
	if (!drsSet) { return false; }
	DrsIncludedFile *previousIfMatching = NULL;
	for (auto it = drsSet->myFiles.begin(); it != drsSet->myFiles.end(); it++) {
		// If previous "matching" one is set, then current file is "next" file, the one to exchange with "previousIfMatching"
		if (previousIfMatching != NULL) {
			DrsIncludedFile *previous = previousIfMatching;
			DrsIncludedFile *curToExchange = *it;
			// Exchange positions (and indexes)
			long int tmpIndex = previous->myOrderIndex;
			previous->myOrderIndex = curToExchange->myOrderIndex;
			curToExchange->myOrderIndex = tmpIndex;
			// exchange positions: in fact, keep pointers, exchange data so we don't have to modify the list
			DrsIncludedFile tmpSwap = DrsIncludedFile(*previous);
			previous->dataSize = curToExchange->dataSize;
			previous->fileId = curToExchange->fileId;
			previous->localFileName = curToExchange->localFileName;
			previous->myOrderIndex = curToExchange->myOrderIndex;
			previous->rawData = curToExchange->rawData;
			previous->tmpOffsetInFile = curToExchange->tmpOffsetInFile;
			curToExchange->dataSize = tmpSwap.dataSize;
			curToExchange->fileId = tmpSwap.fileId;
			curToExchange->localFileName = tmpSwap.localFileName;
			curToExchange->myOrderIndex = tmpSwap.myOrderIndex;
			curToExchange->rawData = tmpSwap.rawData;
			curToExchange->tmpOffsetInFile = tmpSwap.tmpOffsetInFile;
			tmpSwap.rawData = NULL; // Make sure destructor won't free the memory !!! We only used it as a temporary container.
			tmpSwap.dataSize = 0;
			tmpSwap.fileId = -1;
			return true;
		}
		if ((*it)->fileId == fileId) {
			previousIfMatching = *it;
		}
	}
	return false;
}

