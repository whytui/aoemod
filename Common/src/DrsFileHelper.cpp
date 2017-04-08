#include "../include/DrsFileHelper.h"

// This files provides methods to read DRS files


using namespace AOE_STRUCTURES;


DrsFileHelper::DrsFileHelper() {
	this->myFile = NULL;
	this->fileTotalSize = 0;
	this->errorLog = "";
}


// Open a file. Throw an exception if failed. Fails if a file is already open.
void DrsFileHelper::FileOpen(string filename) {
	if (this->myFile != NULL) {
		throw exception("A file is already open");
	}
	this->fileTotalSize = 0;
	errno_t e = fopen_s(&this->myFile, filename.c_str(), "rb");
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
		this->FileOpen(filename);
	}
	catch (exception ex) {
		return ex.what();
	}
	string result = "";
	const long int drsHeaderSizeToRead = 0x40;
	STRUCT_DRS_FILE myDrsObj;
	memset(&myDrsObj, 0, drsHeaderSizeToRead);
	if (this->fileTotalSize < drsHeaderSizeToRead) {
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
		this->ReadCheck(0, &myDrsObj, drsHeaderSizeToRead, drsHeaderSizeToRead, 1);

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
		if (strncmp(cmpBuffer, AOE_STRUCTURES::DrsTableTribeType, sizeof(DrsTableTribeType)) != 0) {
			this->errorLog += "Unexpected fileType: ";
			this->errorLog += cmpBuffer;
			this->errorLog += "\r\n";
		}
		if (drsTabCount > this->maxAllowedTableCount) {
			throw exception("Table count is really huge, file must be corrupted");
		}
		if (myDrsObj.firstFileDataOffset <= drsHeaderSizeToRead) {
			throw exception("First file offset is below header size, file must be corrupted");
		}

		// DRS table is just after header
		drsTablesArray = (STRUCT_DRS_TABLE*)malloc(sizeof(STRUCT_DRS_TABLE_DATA) * drsTabCount);
		this->ReadCheck(drsHeaderSizeToRead, drsTablesArray, sizeof(STRUCT_DRS_TABLE_DATA) * drsTabCount, sizeof(STRUCT_DRS_TABLE_DATA), drsTabCount);

		// Loop on DRS tables (file types)
		unsigned long int lowestFileOffset = 0;
		for (int i = 0; i < drsTabCount; i++) {
			if (i > 0) {
				result += "\r\n";
			}
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
				result += " objOffset=";
				result += std::to_string(curObjOffset);
				result += " objSize=";
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
			result += "\r\n";
			free(drsTablesData); drsTablesData = NULL;
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

