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
	try {
		this->ReadCheck(0, &myDrsObj, drsHeaderSizeToRead, drsHeaderSizeToRead, 1);

		int drsTabCount = myDrsObj.includedTableCount;
		char cmpBuffer[20];
		memset(cmpBuffer, 0, sizeof(cmpBuffer));
		strncpy_s(cmpBuffer, myDrsObj.version, 4);
		if (strncmp(cmpBuffer, "1.00", 4) != 0) {
			this->errorLog += "Unexpected version info: ";
			this->errorLog += cmpBuffer;
			this->errorLog += "\r\n";
		}
		memset(cmpBuffer, 0, sizeof(cmpBuffer));
		strncpy_s(cmpBuffer, myDrsObj.fileType, 12);
		if (strncmp(cmpBuffer, "tribe", 5) != 0) {
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
			std::string msg = "";
			if (i > 0) {
				msg += "\r\n";
			}
			msg = "DRS.table#";
			msg += std::to_string(i);
			msg += " type=";
			msg += drsTablesArray[i].typeName[3];
			msg += drsTablesArray[i].typeName[2];
			msg += drsTablesArray[i].typeName[1];
			msg += drsTablesArray[i].typeName[0];
			msg += " info_offset=";
			msg += std::to_string(drsTablesArray[i].fileInfoOffsetInDrsFile);
			msg += " file_count=";
			msg += std::to_string(drsTablesArray[i].filesCount);
			msg += "\r\n";

			int drsTablesDataSize = sizeof(STRUCT_DRS_TABLE_DATA);
			drsTablesData = (STRUCT_DRS_TABLE_DATA *)malloc(drsTablesDataSize * drsTablesArray[i].filesCount);

			this->ReadCheck(drsTablesArray[i].fileInfoOffsetInDrsFile, drsTablesData, 
				drsTablesDataSize * drsTablesArray[i].filesCount, drsTablesDataSize, drsTablesArray[i].filesCount);

			for (int j = 0; j < drsTablesArray[i].filesCount; j++) {
				unsigned long int thisOffset = drsTablesData[j].offsetInDrsFile;
				if ((thisOffset < lowestFileOffset) || (lowestFileOffset == 0)) {
					lowestFileOffset = thisOffset;
				}
				if (j > 0) { msg += "\r\n"; }
				msg += std::to_string(j);
				msg += " objId=";
				msg += std::to_string(drsTablesData[j].objectId);
				msg += " objOffset=";
				msg += std::to_string(thisOffset);
				msg += " objSize=";
				msg += std::to_string(drsTablesData[j].objectSize);
			}
			msg += "\r\n";
			result += msg;
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
	}
	this->FileClose();
	return result;
}

