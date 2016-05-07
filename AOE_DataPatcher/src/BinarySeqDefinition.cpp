#include "../include/BinarySeqDefinition.h"


BinarySeqDefinition::BinarySeqDefinition()
{
	this->Init(0, 0);
}


BinarySeqDefinition::BinarySeqDefinition(long size, int totalSequencesCount) {
	ConstructorInit(size, totalSequencesCount, -1);
}


BinarySeqDefinition::BinarySeqDefinition(long size, int totalSequencesCount, long seqOffset) {
	ConstructorInit(size, totalSequencesCount, seqOffset);
}


void BinarySeqDefinition::ConstructorInit(long size, int totalSequencesCount, long seqOffset) {
	if ((size < 1) || (totalSequencesCount < 1)) {
		throw std::exception("Bad parameters. Both size and total sequences count should be > 0");
	}
	this->seqName = _T("");
	this->seqDescription = _T("");
	this->seqOffset = seqOffset;
	this->seqSize = size;
	this->totalSeqCount = totalSequencesCount;
	this->seqBufferList = NULL;
	this->seqVarRelativeOffset = NULL;
	this->seqVarType = NULL;
	this->seqFuncMeaning = NULL;
	this->AllocateSeqBuffers();
}


void BinarySeqDefinition::Init(long size, int totalSequencesCount) {
	if ((size == 0) && (totalSeqCount == 0)) {
		this->FreeSeqBuffers();
		this->seqName = _T("");
		this->seqDescription = _T("");
		this->seqOffset = -1;
		this->seqSize = 0;
		this->seqVarRelativeOffset = NULL;
		this->seqVarType = NULL;
		this->seqFuncMeaning = NULL;
		this->totalSeqCount = 0;
		this->seqBufferList = NULL;
		return;
	}
	if ((this->totalSeqCount) || (this->seqBufferList) ) { throw std::exception("Init cannot be called if the object is already initialised with a non-null size/sequence count"); }
	this->ConstructorInit(size, totalSequencesCount, -1);
}


void BinarySeqDefinition::Init(long size, int totalSequencesCount, long seqOffset) {
	if ((this->totalSeqCount) || (this->seqBufferList)) { throw std::exception("Init cannot be called if the object is already initialised with a non-null size/sequence count"); }
	this->ConstructorInit(size, totalSequencesCount, seqOffset);
}



BinarySeqDefinition::~BinarySeqDefinition()
{
	this->FreeSeqBuffers();
}


// Getters / Setters
long BinarySeqDefinition::GetSeqOffset() {
	return this->seqOffset;
}

long BinarySeqDefinition::GetSeqSize() {
	return this->seqSize;
}


// Returns a pointer to the buffer that contains "seqIndex"th sequence
unsigned char *BinarySeqDefinition::GetSequence(int seqIndex) {
	if (!this->IsSeqIndexValid(seqIndex)) { throw std::exception("Bad index value"); }
	return this->seqBufferList[seqIndex];
}

// Returns a pointer to the buffer that contains "seqIndex"th sequence, starting at position "bytesShift"
unsigned char *BinarySeqDefinition::GetSequenceShifted(int seqIndex, long bytesShift) {
	if (!this->IsSeqIndexValid(seqIndex)) { throw std::exception("Bad index value"); }
	return (this->seqBufferList[seqIndex]) + bytesShift;
}


int BinarySeqDefinition::GetTotalSeqCount() {
	return this->totalSeqCount;
}

long BinarySeqDefinition::GetVarRelativeOffset(int seqIndex) {
	if (!this->IsSeqIndexValid(seqIndex)) { throw std::exception("Bad index value"); }
	return this->seqVarRelativeOffset[seqIndex];
}

long BinarySeqDefinition::GetVarSize(int seqIndex) {
	return GET_VAR_TYPE_SIZE(this->GetVarType(seqIndex));
}

SEQ_VAR_TYPES BinarySeqDefinition::GetVarType(int seqIndex) {
	if (!this->IsSeqIndexValid(seqIndex)) { throw std::exception("Bad index value"); }
	return this->seqVarType[seqIndex];
}

FUNC_MEANING BinarySeqDefinition::GetFuncMeaning(int seqIndex) {
	if (!this->IsSeqIndexValid(seqIndex)) { throw std::exception("Bad index value"); }
	return this->seqFuncMeaning[seqIndex];
}

void BinarySeqDefinition::SetSeqOffset(long value) {
	if (value < 0) { throw std::exception("Sequence offset must be positive"); }
	this->seqOffset = value;
}

void BinarySeqDefinition::SetVarRelativeOffset(int seqIndex, long value) {
	if (!this->IsSeqIndexValid(seqIndex)) { throw std::exception("Bad index value"); }
	if ((value < 0) || (value >= this->GetSeqSize())) {
		throw std::exception("Relative offset out of bounds");
	}
	this->seqVarRelativeOffset[seqIndex] = value;
}

void BinarySeqDefinition::SetVarType(int seqIndex, SEQ_VAR_TYPES value) {
	if (!this->IsSeqIndexValid(seqIndex)) { throw std::exception("Bad index value"); }
	this->seqVarType[seqIndex] = value;
}

void BinarySeqDefinition::SetFuncMeaning(int seqIndex, FUNC_MEANING value) {
	if (!this->IsSeqIndexValid(seqIndex)) { throw std::exception("Bad index value"); }
	this->seqFuncMeaning[seqIndex] = value;
}


// Public function to ask if a sequence definition is valid (all necessary attributes are defined/consistent)
bool BinarySeqDefinition::IsValid() {
	if (!(this->GetSeqSize() > 0) && (this->GetTotalSeqCount() > 0) && (this->GetSeqOffset() >= 0) && (this->seqBufferList)) { return false; }
	
	for (int seqIndex = 0; seqIndex < this->GetTotalSeqCount(); seqIndex++) {
		if (!(this->GetVarType(seqIndex) == SVT_NO_VARIABLE || (this->GetVarRelativeOffset(seqIndex) >= 0 && this->GetVarRelativeOffset(seqIndex) < this->GetSeqSize()))) {
			return false;
		}
	}
	return true;
}


// Given a functional meaning, returns the corresponding sequence index
// Returns -1 if no sequence has a corresponding "func meaning".
// Returns -2 if the object is not valid.
// Returns >= 0 if OK: returns the lower index that corresponds (it is possible - but stupid? - to define several sequences with a same meaning)
int BinarySeqDefinition::GetSeqIndexFromFuncMeaning(FUNC_MEANING meaning) {
	if (!this->IsValid()) { return -1; }
	for (int seqIndex = 0; seqIndex < this->GetTotalSeqCount(); seqIndex++) {
		if (this->GetFuncMeaning(seqIndex) == meaning) { return seqIndex;}
	}
	return -1;
}


// Returns a SEQ_VAR_TYPES value if ALL sequences of the seq definition have a variable with a UNIQUE common type.
// If at least 1 possible sequence has no variable, the returned value is SVT_NO_VARIABLE.
// If all sequences do not all have a variable with a common type, the returned value is SVT_NO_VARIABLE.
SEQ_VAR_TYPES BinarySeqDefinition::HasVariable() {
	if (!this->IsValid()) { return SVT_NO_VARIABLE; }
	SEQ_VAR_TYPES res = this->GetVarType(0);
	for (int seqIndex = 0; seqIndex < this->GetTotalSeqCount(); seqIndex++) {
		if (this->GetVarType(seqIndex) == SVT_NO_VARIABLE) { return SVT_NO_VARIABLE; } // a sequence has no variable
		if (this->GetVarType(seqIndex) != res) { return SVT_NO_VARIABLE; } // found inconsistent types
	}
	return res; // All sequences have the same type
}


// Allocates memory for the list of sequences (and the sequence buffers)
void BinarySeqDefinition::AllocateSeqBuffers() {
	if (this->seqBufferList) { throw std::exception("Sequences list is not empty"); }
	if (this->seqVarRelativeOffset) { throw std::exception("Sequences relative offset list is not empty"); }
	if (this->seqVarType) { throw std::exception("Sequences var type list is not empty"); }
	if (this->seqFuncMeaning) { throw std::exception("Sequences functional meaning list is not empty"); }
	// Sequences
	this->seqBufferList = (unsigned char **)malloc(this->totalSeqCount * sizeof(unsigned char *)); // allocates the array of ptr
	if (!this->seqBufferList) { throw std::exception("Error while allocating list buffer. Operation canceled"); } // nothing to free
	memset(this->seqBufferList, 0, this->totalSeqCount * sizeof(unsigned char *));
	for (long i = 0; i < this->totalSeqCount; i++) {
		this->seqBufferList[i] = (unsigned char *)malloc(this->seqSize); // allocates each binary sequence buffer
		if (this->seqBufferList[i]) {
			memset(this->seqBufferList[i], 0, this->seqSize); // initially clears the buffer
		} else {
			// Our status is inconsistent. Free what's been allocated so far and throw an error
			this->FreeSeqBuffers(); // May show error output because of non-allocated sequence buffers, but will do the job
			throw std::exception("Error while allocating buffers. Operation canceled");
		}
	}
	// Var types/offsets/func_meaning
	// We use memset (to 0) => corresponds to SVT_NO_VARIABLE and FM_NONE (we should use a for loop)
	this->seqVarRelativeOffset = (long *)malloc(this->totalSeqCount * sizeof(long));
	memset(this->seqVarRelativeOffset, 0, this->totalSeqCount * sizeof(long));
	this->seqVarType = (SEQ_VAR_TYPES *)malloc(this->totalSeqCount * sizeof(SEQ_VAR_TYPES));
	memset(this->seqVarType, 0, this->totalSeqCount * sizeof(SEQ_VAR_TYPES));
	this->seqFuncMeaning = (FUNC_MEANING *)malloc(this->totalSeqCount * sizeof(FUNC_MEANING));
	memset(this->seqFuncMeaning, 0, this->totalSeqCount * sizeof(FUNC_MEANING));
}


// Free all buffers for the stored binary sequences, including the list of sequences pointers.
void BinarySeqDefinition::FreeSeqBuffers() {
	if (this->totalSeqCount < 1) { return; }
	if (!this->seqBufferList) {
		printf("This really shouldnt happen and is quite bad :-(\n");
		return;
	}
	for (long i = 0; i < this->totalSeqCount; i++) {
		if (!this->seqBufferList[i]) {
			printf("This really shouldnt happen\n"); // May occur if called from AllocateSeqBuffers() exception treatment. do not stop the loop.
		} else {
			free(this->seqBufferList[i]);
			this->seqBufferList[i] = NULL;
		}
	}
	free(this->seqBufferList);
	this->seqBufferList = NULL;

	// Var types/offsets/func_meaning
	if (this->seqVarRelativeOffset) free(this->seqVarRelativeOffset);
	this->seqVarRelativeOffset = NULL;
	if (this->seqVarType) free(this->seqVarType);
	this->seqVarType = NULL;
	if (this->seqFuncMeaning) free(this->seqFuncMeaning);
	this->seqFuncMeaning = NULL;
}


// Provides a human-readable output of the binary sequence of index "seqIndex"
// Warning: index start at 0
std::wstring BinarySeqDefinition::DumpSeqBuffer(int seqIndex, const std::wstring &separator) {
	std::wstring result = _T("");
	char buf[5];
	if (!this->IsSeqIndexValid(seqIndex)) { return _T("Bad index"); }
	for (long i = 0; i < this->seqSize; i++) {
		int value = (int) this->seqBufferList[seqIndex][i];
		sprintf_s(buf, 5, "0x%02X", value); // don't forget it writes a 5th char (\0)
		result += (char) buf[0];
		result += (char) buf[1];
		result += (char) buf[2];
		result += (char) buf[3];
		if (i < this->seqSize - 1) result += separator;
	}
	return result;
}


// Also checks that sequence count is > 0
bool BinarySeqDefinition::IsSeqIndexValid(int seqIndex) {
	return ((this->totalSeqCount > 0) && (seqIndex >= 0) && (seqIndex < this->totalSeqCount));
}

// No size provided in arguments: will use GetSeqSize() as buffer size.
// Make sure srcBuffer is long enough (>= GetSeqSize())
void BinarySeqDefinition::WriteSequence(int seqIndex, const void *srcBuffer) {
	if (!this->IsSeqIndexValid(seqIndex)) { 
		throw std::exception("Bad index value");
	}
	memcpy(this->seqBufferList[seqIndex], srcBuffer, this->GetSeqSize());
}


// This writes the variable value from "default" sequence (from sequence binary definition itself) to the provided buffer.
// Warning: caller must ensure the buffer is large enough.
// Normally, caller should always use a buffer that matches the correct variable type
void BinarySeqDefinition::WriteDefVarValueToBuffer(int seqIndex, void *dstBuffer) {
	if (!this->IsSeqIndexValid(seqIndex)) {
		throw std::exception("Bad index value");
	}
	memcpy(dstBuffer, (this->seqBufferList[seqIndex]) + this->GetVarRelativeOffset(seqIndex), this->GetVarSize(seqIndex));
}


// Returns true if (this) has common offsets with the provided BinarySeqDefinition
bool BinarySeqDefinition::OffsetConflictsWith(BinarySeqDefinition &otherBsd) {
	if (this->GetSeqOffset() > otherBsd.GetSeqOffset()) {
		return otherBsd.OffsetConflictsWith(*this);
	}
	// From now on, we know that "my" offset is smaller (so "my" sequence starts first).
	// There is NO conflict <=> "my" final (last) sequence offset is strictly smaller than "other" sequence's first offset
	return (this->GetSeqOffset() + this->GetSeqSize() >= otherBsd.GetSeqOffset());
}


std::wstring BinarySeqDefinition::ExportToBinaryPatcherIni() {
	if (!this->IsValid()) { return _T(""); }
	if (this->GetTotalSeqCount() < 1) { return _T(""); }
	if (this->GetTotalSeqCount() > 2) { return _T(""); }
	std::wstring result = _T("[BinChange?]\nName=");
	int indexON, indexOFF;
	result += this->GetSeqName();
	result += _T("\nDescription=");
	std::wstring tmp = this->GetSeqDescription();
	int pos = tmp.find(_T("\n"));
	while (pos >= 0) {
		tmp = tmp.replace(pos, 1, _T("\\n"));
		pos = tmp.find(_T("\n"));
	}
	result += tmp;
	result += _T("\nOffset=");
	result += std::to_wstring(this->GetSeqOffset());
	result += _T("\nSize=");
	result += std::to_wstring(this->GetSeqSize());
	indexOFF = indexON = 0;
	if (this->GetTotalSeqCount() > 1) {
		if (this->GetFuncMeaning(0) == FM_OFF) { indexON = 1; }
		else { indexOFF = 1; }
	}
	result += _T("\nBuffer_OFF=");
	result += this->DumpSeqBuffer(indexOFF, _T(" "));
	result += _T("\nBuffer_ON=");
	result += this->DumpSeqBuffer(indexON, _T(" "));
	result += _T("\nDefaultIsOff=");
	result += ((this->GetFuncMeaning(0) == FM_OFF) || (this->GetFuncMeaning(0) == FM_NO_CHOICE)) ? _T("1") : _T("0");
	if (this->HasVariable()) {
		result += _T("\nIntParamOffset=");
		result += std::to_wstring(this->GetVarRelativeOffset(0));
		result += _T("\nIntParamSize=");
		result += std::to_wstring(this->GetVarSize(0));
	}
	result += _T("\n");
	return result;
}



/*    BinarySeqDefSet    */


BinarySeqDefSet::BinarySeqDefSet() {
	this->seqDefListCount = 0;
	this->seqDefList = NULL;
	this->userSelection = NULL;
}

BinarySeqDefSet::BinarySeqDefSet(int count) {
	if (count < 1) {
		throw std::exception("count must be > 0");
	}
	if (!this->SetCount(count)) {
		throw std::exception("Could not initialise correctly: memory allocation error");
	}
}


BinarySeqDefSet::~BinarySeqDefSet() {
	for (int i = 0; i < this->seqDefListCount; i++) {
		if (this->seqDefList[i]) {
			delete this->seqDefList[i];
			this->seqDefList[i] = NULL;
		} else {
			printf("This shouldnt happen\n");
		}
	}
	free(this->seqDefList);
	this->seqDefList = NULL;
	free(this->userSelection);
	this->userSelection = NULL;
	free(this->userVarValue);
	this->userVarValue = NULL;
}


int BinarySeqDefSet::GetCount() {
	return this->seqDefListCount;
}


bool BinarySeqDefSet::SetCount(int count) {
	if ((this->seqDefListCount != 0) || (this->seqDefList)) { return false; }
	this->seqDefListCount = count;
	// Allocate list
	this->seqDefList = (BinarySeqDefinition**)malloc(this->seqDefListCount * sizeof(BinarySeqDefinition *));
	if (!this->seqDefList) {
		this->seqDefListCount = 0;
		return false;
	}
	// Clear list
	memset(this->seqDefList, 0, this->seqDefListCount * sizeof(BinarySeqDefinition *));

	// Create each object
	for (int i = 0; i < this->seqDefListCount; i++) {
		this->seqDefList[i] = new BinarySeqDefinition();
	}

	// User selection list of values + var values
	this->userSelection = (int *)malloc(this->seqDefListCount * sizeof(int *));
	if (!this->userSelection) {
		free(this->seqDefList);
		this->seqDefList = NULL;
		this->seqDefListCount = 0;
		return false;
	}
	this->userVarValue = (T_COMMON_VAR_BUFFER*)malloc(this->seqDefListCount * sizeof(T_COMMON_VAR_BUFFER));
	if (!this->userVarValue) {
		free(this->seqDefList);
		free(this->userSelection);
		this->seqDefList = NULL;
		this->userSelection = NULL;
		this->seqDefListCount = 0;
		return false;
	}
	for (int i = 0; i < this->seqDefListCount; i++) {
		this->userSelection[i] = -1; // Initial value
		memset(userVarValue, 0, sizeof(T_COMMON_VAR_BUFFER)); // Initial var value (just empty the buffer)
	}
	return true;
}


bool BinarySeqDefSet::IsIndexValid(int seqDefIndex) {
	return ((seqDefIndex >= 0) && (seqDefIndex < this->seqDefListCount));
}

int BinarySeqDefSet::GetUserSelection(int seqDefIndex) {
	if (!this->IsIndexValid(seqDefIndex)) { throw std::exception("Bad index"); }
	return this->userSelection[seqDefIndex];
}


void BinarySeqDefSet::SetUserSelection(int seqDefIndex, int value) {
	if (!this->IsIndexValid(seqDefIndex)) { throw std::exception("Bad index"); }
	// -1 is accepted because it means "no user selection"
	if ((value < -1) || (value > this->GetBinSeqDefinition(seqDefIndex)->GetTotalSeqCount())) {
		throw std::exception("Invalid user selection: the sequence index does not exist");
	}
	this->userSelection[seqDefIndex] = value;
}


BinarySeqDefinition *BinarySeqDefSet::GetBinSeqDefinition(int seqDefIndex) {
	if (!this->IsIndexValid(seqDefIndex)) { throw std::exception("Bad index"); }
	if (!this->seqDefList) { throw std::exception("This really should not occur :("); }
	if (this->seqDefList[seqDefIndex] == NULL) { throw std::exception("Technical error: the seq definition is NULL"); };
	return this->seqDefList[seqDefIndex];
}


// Throws an exception if the sequence is not found
int BinarySeqDefSet::FindSeqDefinitionIndex(std::wstring seqDefName) {
	bool found = false;
	int i = -1;
	while ((!found) && (++i < this->GetCount())) {
		found = (this->GetBinSeqDefinition(i)->GetSeqName() == seqDefName);
	}
	if (!found) {
		throw std::exception("Sequence definition was not found");
	}
	return i;
}


void BinarySeqDefSet::SetUserVarValue_default(int seqDefIndex) {
	if (!this->IsIndexValid(seqDefIndex)) {
		throw std::exception("Bad index value");
	}
	int chosenBinSeq = this->GetUserSelection(seqDefIndex);
	if (this->GetBinSeqDefinition(seqDefIndex)->GetVarType(chosenBinSeq) == SVT_NO_VARIABLE) { return; }
	this->GetBinSeqDefinition(seqDefIndex)->WriteDefVarValueToBuffer(chosenBinSeq, &this->userVarValue[seqDefIndex]);
}
