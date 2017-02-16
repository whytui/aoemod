#include "EmpiresXFileManager.h"


EmpiresXFileManager::EmpiresXFileManager()
{
	this->gameFileName = _T("");
	this->fileSize = 0;
	this->gameFileVersion = AOE_INVALID_FILE;
	this->gameFile = NULL;
	this->fileStatus = FS_NOT_OPEN;
}


EmpiresXFileManager::~EmpiresXFileManager()
{
	this->CloseGameFile();
}


// Set the empiresx.exe file full path.
// This invalidates the current status, the file must be checked again to detect its version and its validity.
// If a file was open, it is closed.
void EmpiresXFileManager::SetFileName(wstring argFileName) {
	this->CloseGameFile();
	this->gameFileName = argFileName;
	this->fileSize = 0;
	this->gameFileVersion = AOE_INVALID_FILE;
}


// Close currently-open file, if any. Returns fclose return value (0 = OK).
int EmpiresXFileManager::CloseGameFile() {
	int res = 0;
	if (this->gameFile) {
		this->fileStatus = FS_NOT_OPEN;
		res = fclose(this->gameFile);
		if (res) { printf("Error while closing file: %d\n", res); }
		this->gameFile = NULL;
	}
	return res;
}


// Return 0 if the file was opened successfully
// Also gets file size.
int EmpiresXFileManager::OpenGameFile(const char *argMode) {
	char str[MAX_PATH];
	sprintf_s(str, MAX_PATH, "%ls", this->gameFileName.c_str());
	if (this->gameFileName == _T("")) { return -1; }
	if (this->gameFile) { return 0; } // file already open
	int res = fopen_s(&this->gameFile, str, argMode);
	if (!res) {
		if (!this->gameFile) { throw std::exception("This really shouldnt happen"); }
		FileEditor *fe = new FileEditor(this->gameFile);
		this->fileSize = fe->GetFileSize();
		delete fe;
		fe = NULL;
		this->fileStatus = this->GetStatusFromFileMode(argMode);
	} else {
		this->fileSize = 0;
		this->fileStatus = FS_NOT_OPEN;
	}
	return res;
}


// Returns true if gameFile has a correct EXE magic number
bool EmpiresXFileManager::CheckExeMagicNumber() {
	if (!this->gameFile) { throw std::exception("Error: no file is open. Cannot check magic number"); }
	FileEditor *fe = new FileEditor(this->gameFile);
	bool res;
	try {
		res = fe->CheckBufferConsistency(EXE_MAGIC_NUMBER, 0, EXE_MAGIC_NUMBER_SIZE);
		delete fe;
	}
	catch (...) { // For example, if file size < 2
		return false;
	}
	
	return res;
}


// Use this to ensure the selected game file is the expected version of the file.
// Returns true if the binary sequence check is OK for the version stored in this->gameFileVersion
// this->gameFileVersion must be valued
bool EmpiresXFileManager::CheckVersionBinarySequence() {
	if (!this->gameFile) { throw std::exception("Error: no file is open. Cannot check version"); }

	BinarySeqDefinition *versionSeq;
	switch (this->gameFileVersion) {
	case AOE_VERSION_AOE1_0A:
		versionSeq = new BinarySeqDefinition(EXE_VERSION_CHECK_AOE10A_SIZE, 1, EXE_VERSION_CHECK_AOE10A_OFFSET);
		versionSeq->WriteSequence(0, EXE_VERSION_CHECK_AOE10A);
		break;
	case AOE_VERSION_AOE1_0B:
		versionSeq = new BinarySeqDefinition(EXE_VERSION_CHECK_AOE10B_SIZE, 1, EXE_VERSION_CHECK_AOE10B_OFFSET);
		versionSeq->WriteSequence(0, EXE_VERSION_CHECK_AOE10B);
		break;
	case AOE_VERSION_AOE1_0C:
		versionSeq = new BinarySeqDefinition(EXE_VERSION_CHECK_AOE10C_SIZE, 1, EXE_VERSION_CHECK_AOE10C_OFFSET);
		versionSeq->WriteSequence(0, EXE_VERSION_CHECK_AOE10C);
		break;
	case AOE_VERSION_ROR1_0B:
		versionSeq = new BinarySeqDefinition(EXE_VERSION_CHECK_10B_SIZE, 1, EXE_VERSION_CHECK_10B_OFFSET);
		versionSeq->WriteSequence(0, EXE_VERSION_CHECK_10B);
		break;
	case AOE_VERSION_ROR1_0C:
		versionSeq = new BinarySeqDefinition(EXE_VERSION_CHECK_10C_SIZE, 1, EXE_VERSION_CHECK_10C_OFFSET);
		versionSeq->WriteSequence(0, EXE_VERSION_CHECK_10C);
		break;
	case AOE_INVALID_FILE:
		return false;
		break;
	default:
		printf("Warning: no binary sequence control for this version\n");
		return true;
	}

	FileEditor *fe = new FileEditor(this->gameFile);
	int res = fe->CheckSeqDefConsistency(versionSeq);
	delete fe;
	fe = NULL;
	delete versionSeq;
	versionSeq = NULL;
	// Interpret the result
	if (res == -1) {
		this->gameFileVersion = AOE_INVALID_FILE;
		return false;
	}
	return true;
}


// Identifies game version. returns true if found, false if invalid
// This will update gameFileVersion variable
bool EmpiresXFileManager::FindGameVersion() {
	switch (this->GetFileSize()) {
	//case EXE_FILE_SIZE_AOE10A:
	case EXE_FILE_SIZE_AOE10B:
		// Special: AOE 1.0a and 1.0b have the same size.
		this->gameFileVersion = AOE_VERSION_AOE1_0A;
		if (this->CheckVersionBinarySequence()) {
			return true;
		}
		this->gameFileVersion = AOE_VERSION_AOE1_0B;
		break;
	case EXE_FILE_SIZE_AOE10C:
		this->gameFileVersion = AOE_VERSION_AOE1_0C;
		break;
	case EXE_FILE_SIZE_10B:
		this->gameFileVersion = AOE_VERSION_ROR1_0B;
		break;
	case EXE_FILE_SIZE_10C:
		this->gameFileVersion = AOE_VERSION_ROR1_0C;
		break;
	case EXE_FILE_SIZE_UPATCH_BETA:
		this->gameFileVersion = AOE_VERSION_UPATCH_BETA;
		break;
	case EXE_FILE_SIZE_UPATCH_1_1_HD:
		this->gameFileVersion = AOE_VERSION_UPATCH_1_1_HD;
		break;
	default: 
		this->gameFileVersion = AOE_INVALID_FILE; 
		return false;
	}

	// Control that version info corresponds to the game version we assumed thanks to file size
	// This will set gameFileVersion to invalid if needed
	return this->CheckVersionBinarySequence();
}


// Public entry point to open a file and find its version
// Returns the detected version.
// Immediatly closes the file if it is not valid. Otherwise the file is kept open.
AOE_FILE_VERSION EmpiresXFileManager::OpenCheckExe(wstring argFileName) {
	this->SetFileName(argFileName);
	if (this->OpenGameFile()) {
		return AOE_INVALID_FILE;
	}
	if (!this->CheckExeMagicNumber()) { return AOE_INVALID_FILE; }
	FindGameVersion();
	/*if (this->gameFileVersion == AOE_VERSION_UPATCH) {
		printf("UPatch version is not compatible with standard versions\n");
		this->gameFileVersion = AOE_INVALID_FILE;
	}*/
	if (this->gameFileVersion == AOE_INVALID_FILE) {
		this->CloseGameFile();
		return AOE_INVALID_FILE;
	}
	return this->gameFileVersion;
}


// Used to go back to read-only mode, if write mode has been activated.
// Warning: the file is Closed, so this->OpenGameFile's value changes (FILE)
// Returns true if switch is OK
bool EmpiresXFileManager::SwitchToReadMode() {
	if (!this->gameFile) {
		throw std::exception("No file is open, cannot switch to write mode");
	}
	this->CloseGameFile();
	return (this->OpenGameFile("rb") == 0);
}


// Only authorized when file already has been opened... and checked !
// Warning: the file is Closed, so this->OpenGameFile's value changes (FILE)
// Returns true if switch is OK
// If the result is false, be careful, the file has been closed.
// Throws exceptions
bool EmpiresXFileManager::SwitchToWriteMode() {
	if (!this->gameFile) {
		throw std::exception("No file is open, cannot switch to write mode");
	}
	this->CloseGameFile();
	int res = this->OpenGameFile("r+b");
	if (res == 13) {
		printf("Cannot open file for write.\n");
	}
	return (res == 0);
}


// Only r and r+ are supported ('b' is ignored)
FILESTATUS EmpiresXFileManager::GetStatusFromFileMode(const char *argMode) {
	bool end = false;
	bool read = false;
	bool plus = false;
	int i = 0;
	while (!end) {
		switch (argMode[i++]) {
		case 0: end = true; break;
		case 'r': read = true; break;
		case '+': plus = true; break;
		case 'a':
		case 'w': throw std::exception("This file open mode is not supported");
		case 'b': break; // ignored
		default:
			break;
		}
	}
	if (read && plus) return FS_READ_WRITE;
	if (read) return FS_READ_ONLY;
	throw std::exception("Could not guess file open mode");
}



int EmpiresXFileManager::GetSeqDefIndex(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	return sds->FindSeqDefinitionIndex(seqDefName);
}


int EmpiresXFileManager::GetUserSelection(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	return sds->GetUserSelection(seqDefIndex);
}

int EmpiresXFileManager::GetUserSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	return sds->GetUserSelection(sds->FindSeqDefinitionIndex(seqDefName));
}


void EmpiresXFileManager::SetUserVarValueFromDefault(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	sds->SetUserVarValue_default(seqDefIndex);
}

void EmpiresXFileManager::SetUserVarValueFromDefault(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	sds->SetUserVarValue_default(sds->FindSeqDefinitionIndex(seqDefName));
}


int EmpiresXFileManager::GetFileSelection(BINSEQ_CATEGORIES category, int seqDefIndex) {
	if ((this->fileStatus != FS_READ_ONLY) && (this->fileStatus != FS_READ_WRITE)) {
		throw std::exception("Invalid file status");
	}
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	FileEditor *fe = new FileEditor(this->gameFile);
	int res = fe->CheckSeqDefConsistency(sds->GetBinSeqDefinition(seqDefIndex));
	delete fe;
	fe = NULL;
	return res;
}

int EmpiresXFileManager::GetFileSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	return this->GetFileSelection(category, sds->FindSeqDefinitionIndex(seqDefName));
}


void EmpiresXFileManager::SetUserSelection(BINSEQ_CATEGORIES category, int seqDefIndex, int value) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	sds->SetUserSelection(seqDefIndex, value);
}

void EmpiresXFileManager::SetUserSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName, int value) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	sds->SetUserSelection(sds->FindSeqDefinitionIndex(seqDefName), value);
}


void EmpiresXFileManager::SetUserSelectionToDefault(BINSEQ_CATEGORIES category, int seqDefIndex) {
	this->SetUserSelection(category, seqDefIndex, 0);
	this->SetUserVarValueFromDefault(category, seqDefIndex);
}

void EmpiresXFileManager::SetUserSelectionToDefault(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	int seqDefIndex = sds->FindSeqDefinitionIndex(seqDefName);
	this->SetUserSelectionToDefault(category, seqDefIndex);
}


int EmpiresXFileManager::SetFileSelection(BINSEQ_CATEGORIES category, int seqDefIndex) {
	int res = 0;
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	FileEditor *fe = new FileEditor(this->gameFile);
	int userChoice = sds->GetUserSelection(seqDefIndex);
	if (userChoice == -1) { return -1; }; // No user choice: leave the file as it is...
	if (sds->GetBinSeqDefinition(seqDefIndex)->GetVarType(userChoice) == SVT_NO_VARIABLE) {
		res = fe->WriteFromSequence(sds->GetBinSeqDefinition(seqDefIndex), userChoice); // Returns >0 if ok
	} else {
		res = this->SetUserSelectionUsingValue(fe, sds, seqDefIndex, userChoice); // Returns >0 if ok
	}

	delete fe;
	fe = NULL;
	return res;
}


int EmpiresXFileManager::SetFileSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	return this->SetFileSelection(category, sds->FindSeqDefinitionIndex(seqDefName));
}


int EmpiresXFileManager::CheckAndSetFileSelection(BINSEQ_CATEGORIES category, int seqDefIndex) {
	int currentSelection = this->GetFileSelection(category, seqDefIndex);
	if (currentSelection < 0) { return -2; } // File is inconsistent with definitions
	return this->SetFileSelection(category, seqDefIndex); // will return -1 if no user choice has been made.
}

int EmpiresXFileManager::CheckAndSetFileSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); };
	return this->CheckAndSetFileSelection(category, sds->FindSeqDefinitionIndex(seqDefName));
}


// Write default variable value to a wstring
// Returns an empty wstring if there is no variable.
// Throws exceptions if setup or arguments are incorrect.
std::wstring EmpiresXFileManager::DumpDefaultVarValue(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); }
	BinarySeqDefinition *seqDef = sds->GetBinSeqDefinition(seqDefIndex);
	if (seqDef == NULL) { throw std::exception("Error getting sequence definition"); }
	char int1;
	short int int2;
	long int int4;
	float f;
	std::wstring res;
	switch (seqDef->HasVariable()) {
	case SVT_INT_1B:
		seqDef->GetDefVarValue(0, &int1);
		res = to_wstring(int1);
		break;
	case SVT_INT_2B:
		seqDef->GetDefVarValue(0, &int2);
		res = to_wstring(int2);
		break;
	case SVT_INT_4B:
		seqDef->GetDefVarValue(0, &int4);
		res = to_wstring(int4);
		break;
	case SVT_FLOAT_4B:
		seqDef->GetDefVarValue(0, &f);
		res = to_wstring(f);
		break;
	default:
		res = _T("");
		break;
	}
	return res;
}


// Write current user-defined variable value to a wstring
// Returns an empty wstring if there is no variable.
// Throws exceptions if setup or arguments are incorrect.
std::wstring EmpiresXFileManager::DumpUserVarValue(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); }
	char int1;
	short int int2;
	long int int4;
	float f;
	std::wstring res;
	switch (this->SeqDefHasVariable(category, seqDefIndex)) {
	case SVT_INT_1B:
		sds->GetUserVarValue(seqDefIndex, &int1);
		res = to_wstring(int1);
		break;
	case SVT_INT_2B:
		sds->GetUserVarValue(seqDefIndex, &int2);
		res = to_wstring(int2);
		break;
	case SVT_INT_4B:
		sds->GetUserVarValue(seqDefIndex, &int4);
		res = to_wstring(int4);
		break;
	case SVT_FLOAT_4B:
		sds->GetUserVarValue(seqDefIndex, &f);
		res = to_wstring(f);
		break;
	default:
		res = _T("");
		break;
	}
	return res;
}


// Write current file variable value to a wstring
// Returns an empty wstring if there is no variable.
// Throws exceptions if setup or arguments are incorrect.
std::wstring EmpiresXFileManager::DumpFileVarValue(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); }
	char int1;
	short int int2;
	long int int4;
	float f;
	std::wstring res;
	switch (this->SeqDefHasVariable(category, seqDefIndex)) {
	case SVT_INT_1B:
		this->GetFileVarValue(category, seqDefIndex, &int1);
		res = to_wstring(int1);
		break;
	case SVT_INT_2B:
		this->GetFileVarValue(category, seqDefIndex, &int2);
		res = to_wstring(int2);
		break;
	case SVT_INT_4B:
		this->GetFileVarValue(category, seqDefIndex, &int4);
		res = to_wstring(int4);
		break;
	case SVT_FLOAT_4B:
		this->GetFileVarValue(category, seqDefIndex, &f);
		res = to_wstring(f);
		break;
	default:
		res = _T("");
		break;
	}
	return res;
}


std::wstring EmpiresXFileManager::DumpFileSeqContent(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); }
	FileEditor *fe = new FileEditor(this->gameFile);
	BinarySeqDefinition *binSeq = sds->GetBinSeqDefinition(seqDefIndex);

	char *dataBuffer = (char *) malloc(binSeq->GetSeqSize());
	fe->ReadBuffer(binSeq->GetSeqOffset(), dataBuffer, binSeq->GetSeqSize());
	std::wstring result = _T("");
	wchar_t buf[10];
	for (int pos = 0; pos < binSeq->GetSeqSize(); pos++) {
		if (pos > 0) {
			result = result + _T(" ");
		}
		swprintf_s(buf, 5, _T("0x%02X"), (unsigned char) dataBuffer[pos]); // don't forget it writes a 5th char (\0)
		result = result + buf;
	}
	free(dataBuffer);
	dataBuffer = NULL;
	return result;
}


std::wstring EmpiresXFileManager::DumpAllDefSequencesContent(BINSEQ_CATEGORIES category, int seqDefIndex, wchar_t seqIndexSeparator) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("Invalid file version/category"); }
	BinarySeqDefinition *binSeq = sds->GetBinSeqDefinition(seqDefIndex);
	std::wstring result = _T("");
	for (int seqIndex = 0; seqIndex < binSeq->GetTotalSeqCount(); seqIndex++) {
		if (seqIndex > 0) {
			result = result + seqIndexSeparator;
		}
		result = result + _T("[");
		result = result + to_wstring(seqIndex);
		result = result + _T("]");
		result = result + binSeq->DumpSeqBuffer(seqIndex, _T(" "));
	}
	return result;
}


// Returns the number of sequence definitions for a given category
int EmpiresXFileManager::GetSeqDefCount(BINSEQ_CATEGORIES category) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return 0; };
	return sds->GetCount();
}


// Returns the name of a sequence definition
std::wstring EmpiresXFileManager::GetSequenceDefName(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return _T(""); }
	return sds->GetBinSeqDefinition(seqDefIndex)->GetSeqName();
}


// Returns the name of a sequence definition
std::wstring EmpiresXFileManager::GetSequenceDefDescription(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return _T(""); }
	return sds->GetBinSeqDefinition(seqDefIndex)->GetSeqDescription();
}

long int EmpiresXFileManager::GetSequenceOffset(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return -1; }
	try {
		return sds->GetBinSeqDefinition(seqDefIndex)->GetSeqOffset();
	}
	catch (std::exception) {
		return -1;
	}
}


// Given a functional meaning, returns the corresponding sequence index
// Returns -1 if no sequence has a corresponding "func meaning".
// Returns -2 if there is a technical error.
// Returns >= 0 if OK: returns the lower index that corresponds (it is possible - but stupid? - to define several sequences with a same meaning)
int EmpiresXFileManager::GetSeqIndexFromFuncMeaning(BINSEQ_CATEGORIES category, int seqDefIndex, FUNC_MEANING meaning) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return -2; }
	return sds->GetBinSeqDefinition(seqDefIndex)->GetSeqIndexFromFuncMeaning(meaning); // Can also return errors (-1 / -2)
}


// Returns the functional meaning of a given sequence (user choice) from a sequence definition.
// Returns FM_NONE if an error occurs (no exception)
FUNC_MEANING EmpiresXFileManager::GetFuncMeaningFromSeqIndex(BINSEQ_CATEGORIES category, int seqDefIndex, int seqIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return FM_NONE; }
	return sds->GetBinSeqDefinition(seqDefIndex)->GetFuncMeaning(seqIndex);
}


// Returns a SEQ_VAR_TYPES value if ALL sequences of the seq definition have a variable with a UNIQUE common type.
// If at least 1 possible sequence has no variable, the returned value is SVT_NO_VARIABLE.
// If all sequences do not all have a variable with a common type, the returned value is SVT_NO_VARIABLE.
SEQ_VAR_TYPES EmpiresXFileManager::SeqDefHasVariable(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return SVT_NO_VARIABLE; }
	return sds->GetBinSeqDefinition(seqDefIndex)->HasVariable();
}

SEQ_VAR_TYPES EmpiresXFileManager::SeqDefHasVariable(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return SVT_NO_VARIABLE; }
	return this->SeqDefHasVariable(category, sds->FindSeqDefinitionIndex(seqDefName));
}


// Returns true if at least 1 sequence definition (from any category) has conflicting offsets.
// If a conflict is found, foundCategory and foundSeqDefIndex are updated with conflicting sequence definition values
// If no conflict is found, foundCategory and foundSeqDefIndex are updated with BC_NONE and -1.
bool EmpiresXFileManager::FindConflictingSequenceDef(BINSEQ_CATEGORIES searchedCategory, int searchedSeqDefIndex, BINSEQ_CATEGORIES &foundCategory, int &foundSeqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), searchedCategory);
	if (sds == NULL) { return _T(""); };
	BinarySeqDefinition *searched_bsd = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), searchedCategory)->GetBinSeqDefinition(searchedSeqDefIndex);

	for (BINSEQ_CATEGORIES curCategory = BINSEQ_CATEGORIES(BC_NONE + 1); curCategory < BC_COUNT; curCategory = BINSEQ_CATEGORIES(curCategory + 1)) {
		for (int curSeqDefIndex = 0; curSeqDefIndex < this->GetSeqDefCount(curCategory); curSeqDefIndex++) {
			BinarySeqDefinition *cur_bsd = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), curCategory)->GetBinSeqDefinition(curSeqDefIndex);
			if (searched_bsd->OffsetConflictsWith(*cur_bsd)) {
				foundCategory = curCategory;
				foundSeqDefIndex = curSeqDefIndex;
				return true;
			}
		}
	}

	foundCategory = BC_NONE;
	foundSeqDefIndex = -1;
	return false; // not found
}

bool EmpiresXFileManager::FindConflictingSequenceDef(BINSEQ_CATEGORIES searchedCategory, std::wstring searchedSeqDefName, BINSEQ_CATEGORIES &foundCategory, int &foundSeqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), searchedCategory);
	if (sds == NULL) { return _T(""); };
	return this->FindConflictingSequenceDef(searchedCategory, sds->FindSeqDefinitionIndex(searchedSeqDefName), foundCategory, foundSeqDefIndex);
}


bool EmpiresXFileManager::IsObsolete(BINSEQ_CATEGORIES category, int seqDefIndex) {
	return this->IsObsolete(category, this->GetSequenceDefName(category, seqDefIndex));
}

bool EmpiresXFileManager::IsObsolete(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	if (category == BC_VEG_WINDOWED_MODE) { 
		return (seqDefName != _T("VEG_ScreenToClient1")) &&
			(seqDefName != _T("VEG_ScreenToClient2")) &&
			(seqDefName != _T("VEG_ScreenToClient3"));
	}
	if (category == BC_OBSOLETES) { return true; }
	if (category == BC_TECH_FIXES) {
		// CD_CHECK is no longer considered obsolete: from 2015-10, it is now compatible with ROR_API
		if (/*(seqDefName == _T("CD_CHECK")) ||*/ (seqDefName == _T("CD_CHECK_COLLECTORS")) ||
			(seqDefName == _T("OLD_GameSaveBug_FixA_compatibility")) || (seqDefName == _T("OLD_GameSaveBug_FixB_10c_only")) ) {
			return true;
		}
	}
	return false;
}


int EmpiresXFileManager::IteratorFirst(BINSEQ_CATEGORIES category) {
	this->it_category = category;
	if ((category == BC_NONE) || (category == BC_COUNT)) { return -1; }
	if (this->GetSeqDefCount(category) < 1) { return -1; }
	this->it_current_index = 0;
	return this->it_current_index;
}


int EmpiresXFileManager::IteratorNext() {
	if ((this->it_category == BC_NONE) || (this->it_category == BC_COUNT)) { return -1; }
	this->it_current_index++;
	if (this->it_current_index >= this->GetSeqDefCount(this->it_category)) { return -1; }
	return this->it_current_index;
}


std::wstring EmpiresXFileManager::DumpSequenceDefNames(BINSEQ_CATEGORIES category, std::wstring separator, bool only_pending) {
	std::wstring result = _T("");
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return result; };
	for (int seqDefIndex = 0; seqDefIndex < sds->GetCount(); seqDefIndex++) {
		BinarySeqDefinition *bsd = sds->GetBinSeqDefinition(seqDefIndex);
		if (!only_pending || (this->GetUserSelection(category, seqDefIndex) > -1)) {
			if (seqDefIndex > 0) { result = result + separator; }
			result = result + bsd->GetSeqName();
		}
	}
	return result;
}


// only_file_non_default: if true, will dump only changes that are currently non-default in file.
// Throws exceptions on error
std::wstring EmpiresXFileManager::DumpSequenceDefInfo(BINSEQ_CATEGORIES category, std::wstring separator, bool only_pending, bool only_file_non_default, bool with_obsoletes) {
	std::wstring result = _T("");
	bool first = true;
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return result; };
	for (int seqDefIndex = 0; seqDefIndex < sds->GetCount(); seqDefIndex++) {
		BinarySeqDefinition *bsd = sds->GetBinSeqDefinition(seqDefIndex);
		if (bsd == NULL) { throw std::exception("bsd should not be NULL"); }
		
		if ((with_obsoletes || !this->IsObsolete(category, seqDefIndex)) &&
			(!only_file_non_default || !this->IsFileSelectionDefault(category, seqDefIndex)) &&
			(!only_pending || (this->GetUserSelection(category, seqDefIndex) > -1)) ) {
			if (!first) { result = result + separator; }
			first = false;
			int totalseqCount = bsd->GetTotalSeqCount();
			int userChoice = this->GetUserSelection(category, seqDefIndex);
			result += bsd->GetSeqName().c_str();
			//result += _T(", seqCount=");
			//result += to_wstring(totalseqCount);
			result += _T(", choices=");
			for (int i = 0; i < bsd->GetTotalSeqCount(); i++) {
				if (i > 0) { result += _T(","); }
				result += FUNC_MEANING_NAME[this->GetFuncMeaningFromSeqIndex(category, seqDefIndex, i)];
			}
			result += _T(", pending=[sel=");
			result += to_wstring(userChoice);
			result += _T(",var=");
			SEQ_VAR_TYPES varType = this->SeqDefHasVariable(category, seqDefIndex);
			if (varType != SVT_NO_VARIABLE) {
				if (userChoice > -1) {
					result += this->DumpUserVarValue(category, seqDefIndex);
				} else {
					result += _T("no selection");
				}
			} else {
				result += _T("none");
			}
			if (this->GetFileStatus() != FS_NOT_OPEN) {
				result += _T("] file=[sel=");
				int fileSelection = this->GetFileSelection(category, seqDefIndex);
				result += to_wstring(fileSelection);
				result += _T(",var=");
				if (varType != SVT_NO_VARIABLE) {
					if (fileSelection > -1) {
						result += this->DumpFileVarValue(category, seqDefIndex);
					} else {
						result += _T("no valid selection");
					}
				} else {
					result += _T("none");
				}
				result += _T("]");
			} else {
				result += _T("] [no file is open]");
			}
		}
	}
	return result;
}


std::wstring EmpiresXFileManager::ExportToBinaryPatcherIni(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return _T(""); }
	BinarySeqDefinition *bsd = sds->GetBinSeqDefinition(seqDefIndex);
	if (bsd == NULL) { return _T(""); }
	return bsd->ExportToBinaryPatcherIni();
}

std::wstring EmpiresXFileManager::ExportToBinaryPatcherIni(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { return _T(""); }
	return this->ExportToBinaryPatcherIni(category, sds->FindSeqDefinitionIndex(seqDefName));
}


// Returns true if current file sequence corresponds exactly to the default sequence, including variable value (if any)
// Also returns false if file sequence does not correspond to a valid sequence from the definitions.
bool EmpiresXFileManager::IsFileSelectionDefault(BINSEQ_CATEGORIES category, int seqDefIndex) {
	BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
	if (sds == NULL) { throw std::exception("sds should not be NULL"); }
	BinarySeqDefinition *bsd = sds->GetBinSeqDefinition(seqDefIndex);
	if (bsd == NULL) { throw std::exception("bsd should not be NULL"); }
	int curFileSelection = this->GetFileSelection(category, seqDefIndex); // -1=no match
	if (curFileSelection != 0) { return false; }

	// Note: we're using curFileSelection but in fact it is 0 here.
	switch (bsd->GetVarType(curFileSelection)) {
	case SVT_NO_VARIABLE: return true;
	case SVT_FLOAT_4B: return this->CompareFileAndSequenceValues<float>(category, seqDefIndex, curFileSelection, bsd);
	case SVT_INT_1B: return this->CompareFileAndSequenceValues<unsigned char>(category, seqDefIndex, curFileSelection, bsd);
	case SVT_INT_2B: return this->CompareFileAndSequenceValues<short int>(category, seqDefIndex, curFileSelection, bsd);
	case SVT_INT_4B: return this->CompareFileAndSequenceValues<long int>(category, seqDefIndex, curFileSelection, bsd);
	default: throw std::exception("Bad variable type");
	}
}
