#include "../include/DrsFileHelper.h"

// This files provides methods to read DRS files


using namespace AOE_STRUCTURES;


DrsFileHelper::DrsFileHelper() {
	this->myFile = NULL;
	this->fileTotalSize = 0;
	this->errorLog = "";
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
DrsSetOfIncludedFiles *DrsFileHelper::AddFileType(AOE_STRUCTURES::DRS_FILE_TYPE_DWORD fileType) {
	DrsSetOfIncludedFiles *existing = this->GetFileTypeInfo(fileType);
	if (existing != NULL) { return existing; }
	DrsSetOfIncludedFiles *s = new DrsSetOfIncludedFiles();
	s->fileTypeAsDword = fileType;
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


DrsSetOfIncludedFiles *DrsFileHelper::GetFileTypeInfo(AOE_STRUCTURES::DRS_FILE_TYPE_DWORD fileType) {
	auto it = std::find_if(this->currentDrsWorkingSet.begin(), this->currentDrsWorkingSet.end(),
		[fileType](DrsSetOfIncludedFiles *s){ return s->fileTypeAsDword == fileType; }
	);
	if (it != this->currentDrsWorkingSet.end()) {
		return *it; // found in list
	}
	return NULL;
}


// Add a file (and file type if necessary)
DrsIncludedFile *DrsFileHelper::AddFile(AOE_STRUCTURES::DRS_FILE_TYPE_DWORD fileType, long int fileId, string filename) {
	DrsSetOfIncludedFiles *s = this->AddFileType(fileType);
	auto it = std::find_if(s->myFiles.begin(), s->myFiles.end(),
		[fileId](DrsIncludedFile *s){ return s->fileId == fileId; }
	);
	if (it != s->myFiles.end()) {
		return *it; // already in list
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


string DrsFileHelper::GetDrsMainObjectsList(string filename) {
	this->errorLog = "";
	if (!CheckFileExistence(filename.c_str())) {
		this->errorLog += "File does not exist\r\n";
		return "";
	}
	
	try {
		this->FileOpen(filename, true);
	}
	catch (exception ex) {
		this->errorLog += ex.what();
		this->errorLog += "\r\n";
		return ex.what();
	}
	string result = "";
	
	STRUCT_DRS_FILE myDrsObj;
	memset(&myDrsObj, 0, AOE_STRUCTURES::DrsHeaderSize);
	if (this->fileTotalSize < AOE_STRUCTURES::DrsHeaderSize) {
		// ERROR
		this->FileClose();
		this->errorLog += "File size is too small to read header section\r\n";
		return "";
	}

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
		result += "FirstFileOffset=";
		result += to_string(myDrsObj.firstFileDataOffset);

		// DRS table is just after header (list of file types)
		drsTablesArray = (STRUCT_DRS_TABLE*)malloc(sizeof(STRUCT_DRS_TABLE) * drsTabCount);
		this->ReadCheck(AOE_STRUCTURES::DrsHeaderSize, drsTablesArray, sizeof(STRUCT_DRS_TABLE) * drsTabCount, sizeof(STRUCT_DRS_TABLE), drsTabCount);

		// Loop on DRS tables (file types details)
		unsigned long int lowestFileOffset = 0;
		for (int i = 0; i < drsTabCount; i++) {
			result += "\r\n";
			result += "DRS.table#";
			result += std::to_string(i);
			result += " type=";
			result += drsTablesArray[i].typeName[3];
			result += drsTablesArray[i].typeName[2];
			result += drsTablesArray[i].typeName[1];
			result += drsTablesArray[i].typeName[0];
			result += " info_offset=";
			result += std::to_string(drsTablesArray[i].fileInfoOffsetInDrsFile);
			result += " file_count=";
			result += std::to_string(drsTablesArray[i].filesCount);
			result += "\r\n";
			bool isSlp = drsTablesArray[i].IsSlp();

			if (drsTablesArray[i].filesCount > 0) {
				// If 0, NOT an error (seen in original file), but ignore...

				int drsTablesDataSize = sizeof(STRUCT_DRS_TABLE_DATA);
				drsTablesData = (STRUCT_DRS_TABLE_DATA *)malloc(drsTablesDataSize * drsTablesArray[i].filesCount);

				this->ReadCheck(drsTablesArray[i].fileInfoOffsetInDrsFile, drsTablesData,
					drsTablesDataSize * drsTablesArray[i].filesCount, drsTablesDataSize, drsTablesArray[i].filesCount);

				for (int j = 0; j < drsTablesArray[i].filesCount; j++) {
					unsigned long int curObjOffset = drsTablesData[j].offsetInDrsFile;
					if ((curObjOffset < lowestFileOffset) || (lowestFileOffset == 0)) {
						lowestFileOffset = curObjOffset;
					}
					if (j > 0) { result += "\r\n"; }
					result += std::to_string(j);
					result += " objId=";
					result += std::to_string(drsTablesData[j].objectId);
					result += " dataOffset=";
					result += std::to_string(curObjOffset);
					result += " dataSize=";
					result += std::to_string(drsTablesData[j].objectSize);

					if (isSlp) {
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
						result += " SLP_desc=";
						result += slpHeader.description;
						result += " shpCount=";
						result += to_string(shapeCount);

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
			result += "\r\n";
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
	}
	this->FileClose();
	return result;
}


void DrsFileHelper::TestCreateDrs(string filename) {
	this->errorLog = "";
	if (CheckFileExistence(filename.c_str())) {
		this->errorLog += "File already exist\r\n";
		return;
	}


	//TEST data
	this->ResetCurrentWorkingSet();
	//this->AddFile(DRS_FILE_TYPE_DWORD::DFT_SLP, 42, "F:\\tmpaoe\\output\\CROR_NB.SLP");
	//this->AddFile(DRS_FILE_TYPE_DWORD::DFT_SLP, 43, "F:\\tmpaoe\\output\\CROR_NB2.SLP");
	this->AddFile(DRS_FILE_TYPE_DWORD::DFT_SLP, 15000, "F:\\tmpaoe\\output\\_\\15000.slp");
	this->AddFile(DRS_FILE_TYPE_DWORD::DFT_SLP, 15001, "F:\\tmpaoe\\output\\_\\15001.slp");
	this->AddFile(DRS_FILE_TYPE_DWORD::DFT_SLP, 15002, "F:\\tmpaoe\\output\\_\\15002.slp");
	this->AddFile(DRS_FILE_TYPE_DWORD::DFT_SLP, 15003, "F:\\tmpaoe\\output\\_\\15003.slp");
	this->AddFile(DRS_FILE_TYPE_DWORD::DFT_SLP, 12345, "F:\\tmpaoe\\output\\_\\12345.slp");

	//DrsSetOfIncludedFiles *slpFilesSet = this->GetFileTypeInfo(DRS_FILE_TYPE_DWORD::DFT_SLP);

	// TEST sort
	//this->currentDrsWorkingSet.sort(DrsSetOfFilesHasLowerRankThan);


	try {
		this->FileOpen(filename, false);
	}
	catch (exception ex) {
		this->errorLog += ex.what();
	}
	string result = "";

	/* Pre-compute some sizes/offsets */
	int drsTableSizeOf = sizeof(STRUCT_DRS_TABLE);
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
			// ERROR
			throw exception("drsTabCount <= 0");
		}

		STRUCT_DRS_FILE myDrsObj;
		//memcpy_s(myDrsObj.copyrightHeader, sizeof(myDrsObj.copyrightHeader), "RockNRor", 9);
		memcpy_s(myDrsObj.copyrightHeader, sizeof(myDrsObj.copyrightHeader), DrsFileDefaultCopyright, sizeof(DrsFileDefaultCopyright));
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
			drsTablesArray[drsTabIndex].SetTypeNameFromDword(fileSet->fileTypeAsDword);

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
					this->errorLog += "Error with offsets";
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
	//STRUCT_SLP_FRAME_HEADER *frameHeaderArray = NULL;
	//STRUCT_SLP_FRAME_ROW_EDGE *curEdgesArray = NULL;

	if (drsTablesArray) { free(drsTablesArray); drsTablesArray = NULL; }
	if (drsTablesInfo) { free(drsTablesInfo); drsTablesInfo = NULL; }
	this->FileClose();
}

