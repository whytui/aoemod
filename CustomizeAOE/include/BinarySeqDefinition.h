#include <tchar.h>
#include <string>

#pragma once


// Always use consecutive values for iterators
// Example: for (FUNC_MEANING fmeaning = FUNC_MEANING(FM_NONE + 1); fmeaning < FM_COUNT; fmeaning = FUNC_MEANING(fmeaning + 1)) {...}
// This enum represents possible functional significations for binary sequences.
// Example: in a definition with 2 possible sequences, seq0=default might be FM_ON (feature is active) and seq1 might be FM_OFF (inactive)
enum FUNC_MEANING { FM_NONE, FM_ON, FM_OFF, FM_NO_CHOICE, FM_COUNT };

const static std::wstring FUNC_MEANING_NAME[FM_COUNT] = { _T("No functional meaning"), _T("ON"), _T("OFF"), _T("Single") };


enum SEQ_VAR_TYPES { SVT_NO_VARIABLE, SVT_INT_1B, SVT_INT_2B, SVT_INT_4B, SVT_FLOAT_4B, SVT_COUNT };
const static int SEQ_VAR_TYPE_SIZE[SVT_COUNT] = { 0, 1, 2, 4, 4 };
const static std::wstring SEQ_VAR_TYPE_NAME[SVT_COUNT] = { _T("No variable"), _T("1-byte int"), _T("2-bytes int"), _T("4-bytes int"), _T("4-bytes float")};


#define GET_VAR_TYPE_SIZE(n) SEQ_VAR_TYPE_SIZE[n]
// The maximum size in bytes of a Binary sequence variable (MAX(SEQ_VAR_TYPE_SIZE[...]))
#define MAX_VAR_SIZE 4

// A MAX_VAR_SIZE-sized variable type
typedef struct T_COMMON_VAR_BUFFER { unsigned char x[MAX_VAR_SIZE]; } COMMON_VAR_BUFFER;


// Calls BinarySeqDefinition->WriteSequence with a prior check on buffer size
// WARNING: This only works if buffer is an array (so that sizeof() really returns the array length - not the pointer size (4)
#define SECURE_WriteSequence(binSeqDef, index, buffer) if (sizeof(buffer) != binSeqDef->GetSeqSize()) \
	 	 { throw std::exception("ERROR: Buffer size doesnt match sequence size !"); }; \
	binSeqDef->WriteSequence(index, buffer);


// Common info for all following macros CST_* (init using constants)
// binSeqDef: the BinarySeqDefinition object
// constRootName_prefix : the prefix part of the normalized constant name
// constRootName : the normalized constant name (without the prefix)
// The BinarySeqDefinition's name will be = constRootName (without the prefix).

// Complete set-up of a BinarySeqDefinition that has 1 sequence and no variable
// Requires appropriate normalized constants : xxx_OFFSET, xxx_SEQ0
#define CST_INITSEQ_1_NOVAR(binSeqDef, constRootName_prefix, constRootName) \
	binSeqDef->Init(sizeof(constRootName_prefix##constRootName##_SEQ0), 1, constRootName_prefix##constRootName##_OFFSET); \
	binSeqDef->SetSeqName(_T(#constRootName)); \
	SECURE_WriteSequence(binSeqDef, 0, constRootName_prefix##constRootName##_SEQ0);


// Complete set-up of a BinarySeqDefinition that has 2 sequences and no variable
// Requires appropriate normalized constants : xxx_OFFSET, xxx_SEQ0, xxx_SEQ1
#define CST_INITSEQ_2_NOVAR(binSeqDef, constRootName_prefix, constRootName) \
	binSeqDef->Init(sizeof(constRootName_prefix##constRootName##_SEQ0), 2, constRootName_prefix##constRootName##_OFFSET); \
	binSeqDef->SetSeqName(_T(#constRootName)); \
	SECURE_WriteSequence(binSeqDef, 0, constRootName_prefix##constRootName##_SEQ0); \
	SECURE_WriteSequence(binSeqDef, 1, constRootName_prefix##constRootName##_SEQ1);


// Complete set-up of a BinarySeqDefinition that has 2 sequences and no variable
// Requires appropriate normalized constants : xxx_OFFSET, xxx_SEQ0, xxx_SEQ1, xxx_SEQ2
#define CST_INITSEQ_3_NOVAR(binSeqDef, constRootName_prefix, constRootName) \
	binSeqDef->Init(sizeof(constRootName_prefix##constRootName##_SEQ0), 2, constRootName_prefix##constRootName##_OFFSET); \
	binSeqDef->SetSeqName(_T(#constRootName)); \
	SECURE_WriteSequence(binSeqDef, 0, constRootName_prefix##constRootName##_SEQ0); \
	SECURE_WriteSequence(binSeqDef, 1, constRootName_prefix##constRootName##_SEQ1); \
	SECURE_WriteSequence(binSeqDef, 2, constRootName_prefix##constRootName##_SEQ2);

// Complete set-up of a BinarySeqDefinition that has 1 sequence, containing a variable
// Requires appropriate normalized constants : xxx_OFFSET, xxx_SEQ0, xxx_VARTYPE0, xxx_VAROFFSET0
#define CST_INITSEQ_1_VAR(binSeqDef, constRootName_prefix, constRootName) \
	binSeqDef->Init(sizeof(constRootName_prefix##constRootName##_SEQ0), 1, constRootName_prefix##constRootName##_OFFSET); \
	binSeqDef->SetSeqName(_T(#constRootName)); \
	binSeqDef->SetVarType(0, constRootName_prefix##constRootName##_VARTYPE0); \
	binSeqDef->SetVarRelativeOffset(0, constRootName_prefix##constRootName##_VAROFFSET0); \
	SECURE_WriteSequence(binSeqDef, 0, constRootName_prefix##constRootName##_SEQ0);

// Complete set-up of a BinarySeqDefinition that has 2 sequences, containing a variable
// Requires appropriate normalized constants : xxx_OFFSET, xxx_SEQ0, xxx_SEQ1, xxx_VARTYPE0, xxx_VARTYPE1, xxx_VAROFFSET0, xxx_VAROFFSET1
#define CST_INITSEQ_2_VAR(binSeqDef, constRootName_prefix, constRootName) \
	binSeqDef->Init(sizeof(constRootName_prefix##constRootName##_SEQ0), 2, constRootName_prefix##constRootName##_OFFSET); \
	binSeqDef->SetSeqName(_T(#constRootName)); \
	binSeqDef->SetVarType(0, constRootName_prefix##constRootName##_VARTYPE0); \
	binSeqDef->SetVarType(1, constRootName_prefix##constRootName##_VARTYPE1); \
	binSeqDef->SetVarRelativeOffset(0, constRootName_prefix##constRootName##_VAROFFSET0); \
	binSeqDef->SetVarRelativeOffset(1, constRootName_prefix##constRootName##_VAROFFSET1); \
	SECURE_WriteSequence(binSeqDef, 0, constRootName_prefix##constRootName##_SEQ0); \
	SECURE_WriteSequence(binSeqDef, 1, constRootName_prefix##constRootName##_SEQ1);


// Used in other macros
#define NOPAREN(...) __VA_ARGS__


// Complete set-up of a BinarySeqDefinition that has 1 sequence, without variable
// name must be unique in the given scope and must only contain alphanumerical chars.
#define INITSEQ_1_NOVAR(binSeqDef, name, desc, offset, seq0) \
	const static unsigned char __MACRO_BUF_##name##0[] = {NOPAREN seq0}; \
	binSeqDef->Init(sizeof(__MACRO_BUF_##name##0), 1, offset); \
	binSeqDef->SetSeqName(_T(#name)); \
	binSeqDef->SetSeqDescription(_T(desc)); \
	binSeqDef->SetFuncMeaning(0, FM_NO_CHOICE); \
	SECURE_WriteSequence(binSeqDef, 0, __MACRO_BUF_##name##0);

// Complete set-up of a BinarySeqDefinition that has 1 sequence, without variable
// name must be unique in the given scope and must only contain alphanumerical chars.
#define INITSEQ_2_NOVAR(binSeqDef, name, desc, offset, seq0, seq1, fmeaning0, fmeaning1) \
	const static unsigned char __MACRO_BUF_##name##0[] = {NOPAREN seq0}; \
	const static unsigned char __MACRO_BUF_##name##1[] = {NOPAREN seq1}; \
	binSeqDef->Init(sizeof(__MACRO_BUF_##name##0), 2, offset); \
	binSeqDef->SetSeqName(_T(#name)); \
	binSeqDef->SetSeqDescription(_T(desc)); \
	binSeqDef->SetFuncMeaning(0, fmeaning0); \
	binSeqDef->SetFuncMeaning(1, fmeaning1); \
	SECURE_WriteSequence(binSeqDef, 0, __MACRO_BUF_##name##0); \
	SECURE_WriteSequence(binSeqDef, 1, __MACRO_BUF_##name##1);

// Complete set-up of a BinarySeqDefinition that has 1 sequence, without variable
// name must be unique in the given scope and must only contain alphanumerical chars.
#define INITSEQ_3_NOVAR(binSeqDef, name, desc, offset, seq0, seq1, seq2, fmeaning0, fmeaning1, fmeaning2) \
	const static unsigned char __MACRO_BUF_##name##0[] = {NOPAREN seq0}; \
	const static unsigned char __MACRO_BUF_##name##1[] = {NOPAREN seq1}; \
	const static unsigned char __MACRO_BUF_##name##2[] = {NOPAREN seq2}; \
	binSeqDef->Init(sizeof(__MACRO_BUF_##name##0), 3, offset); \
	binSeqDef->SetSeqName(_T(#name)); \
	binSeqDef->SetSeqDescription(_T(desc)); \
	binSeqDef->SetFuncMeaning(0, fmeaning0); \
	binSeqDef->SetFuncMeaning(1, fmeaning1); \
	binSeqDef->SetFuncMeaning(2, fmeaning2); \
	SECURE_WriteSequence(binSeqDef, 0, __MACRO_BUF_##name##0); \
	SECURE_WriteSequence(binSeqDef, 1, __MACRO_BUF_##name##1); \
	SECURE_WriteSequence(binSeqDef, 2, __MACRO_BUF_##name##2);


// Complete set-up of a BinarySeqDefinition that has 1 sequence, containing a variable
// name must be unique in the given scope and must only contain alphanumerical chars.
#define INITSEQ_1_VAR(binSeqDef, name, desc, offset, seq0, vartype0, varoffset0) \
	const static unsigned char __MACRO_BUF_##name##0[] = {NOPAREN seq0}; \
	binSeqDef->Init(sizeof(__MACRO_BUF_##name##0), 1, offset); \
	binSeqDef->SetSeqName(_T(#name)); \
	binSeqDef->SetSeqDescription(_T(desc)); \
	binSeqDef->SetVarType(0, vartype0); \
	binSeqDef->SetVarRelativeOffset(0, varoffset0); \
	binSeqDef->SetFuncMeaning(0, FM_NO_CHOICE); \
	SECURE_WriteSequence(binSeqDef, 0, __MACRO_BUF_##name##0);


// Complete set-up of a BinarySeqDefinition that has 1 sequence, containing a variable
// name must be unique in the given scope and must only contain alphanumerical chars.
#define INITSEQ_2_VAR(binSeqDef, name, desc, offset, seq0, seq1, fmeaning0, fmeaning1, vartype0, vartype1, varoffset0, varoffset1) \
	const static unsigned char __MACRO_BUF_##name##0[] = {NOPAREN seq0}; \
	const static unsigned char __MACRO_BUF_##name##1[] = {NOPAREN seq1}; \
	binSeqDef->Init(sizeof(__MACRO_BUF_##name##0), 2, offset); \
	binSeqDef->SetSeqName(_T(#name)); \
	binSeqDef->SetSeqDescription(_T(desc)); \
	binSeqDef->SetFuncMeaning(0, fmeaning0); \
	binSeqDef->SetFuncMeaning(1, fmeaning1); \
	binSeqDef->SetVarType(0, vartype0); \
	binSeqDef->SetVarType(1, vartype1); \
	binSeqDef->SetVarRelativeOffset(0, varoffset0); \
	binSeqDef->SetVarRelativeOffset(1, varoffset1); \
	SECURE_WriteSequence(binSeqDef, 0, __MACRO_BUF_##name##0); \
	SECURE_WriteSequence(binSeqDef, 1, __MACRO_BUF_##name##1);



/* This class defines fixed-length binary sequences
 * That may have 1 or several possible values : standard sequence (index=0) + [0-n] custom sequences
 * Each sequence may own a variable with its own type and its own offset. All sequence's variables are independent
*/
class BinarySeqDefinition
{
public:
	// Avoid using this constructor.
	// Or immediately call Init(...) after.
	BinarySeqDefinition();

	/* size: length (number of bytes) of the binary sequence
	   totalSequencesCount: number of possible difference binary values sequences
	*/
	BinarySeqDefinition(long size, int totalSequencesCount);

	BinarySeqDefinition(long size, int totalSequencesCount, long seqOffset);

	BinarySeqDefinition(long size, int totalSequencesCount, long seqOffset, long seqVarRelativeOffset, SEQ_VAR_TYPES seqVarType);

	~BinarySeqDefinition();

	// Call this after using the default constructor (with no argument) or the object will be unuseable.
	// Calling this is forbidden if size and sequence count has already been set.
	// Calling Init(0, 0) completely resets the object (that will required a new Init(...)).
	void Init(long size, int totalSequencesCount);
	void Init(long size, int totalSequencesCount, long seqOffset);
	void Init(long size, int totalSequencesCount, long seqOffset, long seqVarRelativeOffset, SEQ_VAR_TYPES seqVarType);


	std::wstring DumpSeqBuffer(int seqIndex, const std::wstring &separator = _T(" "));
	std::wstring GetSeqDescription() { return this->seqDescription; }
	std::wstring GetSeqName() { return this->seqName; }
	long GetSeqOffset();
	long GetSeqSize();
	unsigned char *GetSequence(int seqIndex);
	unsigned char *GetSequenceShifted(int seqIndex, long bytesShift);
	int GetTotalSeqCount();
	long GetVarRelativeOffset(int seqIndex);
	long GetVarSize(int seqIndex);
	SEQ_VAR_TYPES GetVarType(int seqIndex);
	FUNC_MEANING GetFuncMeaning(int seqIndex);
	bool IsValid();
	int GetSeqIndexFromFuncMeaning(FUNC_MEANING meaning);
	SEQ_VAR_TYPES HasVariable();

	void SetSeqDescription(std::wstring value) { this->seqDescription = value; }
	void SetSeqName(std::wstring value) { this->seqName = value; }
	void SetSeqOffset(long value);
	void SetVarRelativeOffset(int seqIndex, long value);
	void SetVarType(int seqIndex, SEQ_VAR_TYPES value);
	void SetFuncMeaning(int seqIndex, FUNC_MEANING value);
	void WriteSequence(int seqIndex, const void *srcBuffer);

	// Template functions MUST be entirely defined in header file (to avoid link error)
	// GetDefVarValue: get the variable value from "default" sequence (from sequence definition)
	template<typename varType> void GetDefVarValue(int seqIndex, varType *var) {
		if (!this->IsSeqIndexValid(seqIndex)) {
			throw std::exception("Bad index value");
		}
		if (sizeof(varType) != this->GetVarSize(seqIndex)) {
			throw std::exception("Variable type does not correspond to the size");
		}
		// Get a pointer to the location of the variable (use unsigned char* type because its size is 1)
		unsigned char *ptr = this->seqBufferList[seqIndex] + this->GetVarRelativeOffset(seqIndex);
		*var = *((varType *)ptr); // Get the value in the relevant type : cast the pointer, not the value, to preserve the original binary value
	}

	void WriteDefVarValueToBuffer(int seqIndex, void *dstBuffer);
	bool OffsetConflictsWith(BinarySeqDefinition &otherBsd);
	std::wstring ExportToBinaryPatcherIni();

private:
	std::wstring seqName;
	std::wstring seqDescription;
	long seqOffset;
	long seqSize; // Must remain consistent with seqBufferListallocation
	long *seqVarRelativeOffset; // Array (dynamically allocated) to store variable relative offset for all sequences
	SEQ_VAR_TYPES *seqVarType; // Array (dynamically allocated) to store variable types for all sequences
	FUNC_MEANING *seqFuncMeaning; // Array (dynamically allocated) to store functional meaning for all sequences
	int totalSeqCount; // Number of possible sequences (including default sequence + custom ones). Must remain consistent with seqBufferListallocation
	unsigned char **seqBufferList; // Array (dynamically allocated) of binary sequences

	void ConstructorInit(long size, int totalSequencesCount, long seqOffset, long seqVarRelativeOffset, SEQ_VAR_TYPES seqVarType);
	void AllocateSeqBuffers();
	void FreeSeqBuffers();
	bool IsSeqIndexValid(int seqIndex);
};


/* This class represents a set of BinarySeqDefinition.
 * Just provide the number of BinarySeqDefinition with SetCount, then every memory allocation is managed automatically (freed in destructor)
 * Each element is an independant BinarySeqDefinition : with its own offset, its own number of possible sequences, etc
 */
class BinarySeqDefSet
{
public:
	// Default constructor. Call SetCount() to allocate memory for the number of sequences you need.
	BinarySeqDefSet();
	// Preferred constructor: directly allocates memory for "count" sequences
	BinarySeqDefSet(int count);

	~BinarySeqDefSet();

	// Get the number of binary sequence definitions.
	int GetCount();

	// Set the number of binary sequence definitions. Only allowed if nothing is allocated yet (if no-argument constructor was used)
	// Returns true if the operation completed successfully.
	bool SetCount(int count);

	// Get the user choice value for the given sequence definition in the list (referred to by index)
	// -1 = no choice. if <> -1, value should correspond to a sequence index in seqDefList[index].
	// Throws an exception if index is invalid
	int GetUserSelection(int seqDefIndex);

	// Set the user choice value for the given sequence definition in the list (referred to by index)
	// -1 = no choice. if <> -1, value should correspond to a sequence index in seqDefList[index].
	// Throws an exception if index is invalid
	// If the selected sequence contains a variable, you should call SetUserVarValue(...) after this call.
	void SetUserSelection(int seqDefIndex, int value);

	// Template functions MUST be entirely defined in header file (to avoid link error)
	// GetUserVarValue: get the user-set variable value
	// data type must match the SELECTED bin sequence in the BinarySeqDefinition of index=seqIndex.
	// This means you must call SetUserSelection(...) first to choose the sequence and then get the value
	template<typename varType> void GetUserVarValue(int seqDefIndex, varType *var) {
		if (!this->IsIndexValid(seqDefIndex)) {
			throw std::exception("Bad index value");
		}
		int chosenBinSeq = this->GetUserSelection(seqDefIndex);
		if (chosenBinSeq == -1) { throw std::exception("No sequence has been selected, cannot get a var value."); }
		if (sizeof(varType) != this->GetBinSeqDefinition(seqDefIndex)->GetVarSize(chosenBinSeq)) {
			throw std::exception("Variable type does not correspond to the size");
		}
		// Get a pointer to the location of the variable (use unsigned char* type because its size is 1)
		unsigned char *ptr = (unsigned char *)(&this->userVarValue[seqDefIndex]);
		*var = *((varType *)ptr); // Get the value in the relevant type : cast the pointer, not the value, to preserve the original binary value
	}

	// Template functions MUST be entirely defined in header file (to avoid link error)
	// SetUserVarValue: set the user-selected variable value
	// data type must match the SELECTED bin sequence in the BinarySeqDefinition of index=seqIndex.
	// This means you must call SetUserSelection(...) first to choose the sequence and then set the value
	template<typename varType> void SetUserVarValue(int seqDefIndex, varType *var) {
		if (!this->IsIndexValid(seqDefIndex)) {
			throw std::exception("Bad index value");
		}
		int chosenBinSeq = this->GetUserSelection(seqDefIndex);
		if (chosenBinSeq == -1) {
			throw std::exception("No selection for this sequence definition");
		}
		if (sizeof(varType) != this->GetBinSeqDefinition(seqDefIndex)->GetVarSize(chosenBinSeq)) {
			throw std::exception("Variable type does not correspond to the size");
		}

		// Get a pointer to the location of the variable (use unsigned char* type because its size is 1)
		unsigned char *ptr = (unsigned char *)(&this->userVarValue[seqDefIndex]);
		*((varType *)ptr) = *var;
	}

	// Set the user-selected variable value using definition's default value (value in binary sequence)
	// Does nothing if the sequence does not contain a variable
	void SetUserVarValue_default(int seqDefIndex);

	// Returns true if the index is valid regarding list bounds
	bool IsIndexValid(int seqDefIndex);

	// Returns a BinarySeqDefinition object.
	// Throws an exception if index is invalid
	BinarySeqDefinition *GetBinSeqDefinition(int seqDefIndex);

	int FindSeqDefinitionIndex(std::wstring seqDefName);

private:
	BinarySeqDefinition **seqDefList;
	int seqDefListCount;
	int *userSelection; // Allows storing user's choice for each sequence. Use -1 as 'not set'
	T_COMMON_VAR_BUFFER *userVarValue; // Allows storing user's variable value whatever (listed) type it is. WARNING: indices are "seqDefIndex" (not "userChoice")
};
