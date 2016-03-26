#include <Windows.h>
#include <tchar.h>
#include <string>
#include "AOE_const_global.h"
#include "FileEditor.h"
#include "AOE_binData.h"

#pragma once

using namespace std;

enum FILESTATUS { FS_NOT_OPEN, FS_READ_ONLY, FS_WRITE_ONLY, FS_READ_WRITE };

class EmpiresXFileManager
{
public:
	EmpiresXFileManager();
	~EmpiresXFileManager();

	int CloseGameFile();
	wstring GetFileName() { return this->gameFileName; };
	long GetFileSize() { return this->fileSize; };
	AOE_FILE_VERSION GetFileVersion() { return this->gameFileVersion; }
	AOE_FILE_VERSION OpenCheckExe(wstring argFileName);
	bool SwitchToReadMode();
	bool SwitchToWriteMode();
	FILESTATUS GetFileStatus() { return this->fileStatus; };

	// Get a sequence definition index from its name
	int GetSeqDefIndex(BINSEQ_CATEGORIES category, std::wstring seqDefName);
	// Get a sequence name from its index
	std::wstring GetSequenceDefName(BINSEQ_CATEGORIES category, int seqDefIndex);
	// Get a sequence name from its index
	std::wstring GetSequenceDefDescription(BINSEQ_CATEGORIES category, int seqDefIndex);
	// Get a sequence base address. Returns -1 if invalid.
	long int GetSequenceOffset(BINSEQ_CATEGORIES category, int seqDefIndex);

	// Find the sequence index that corresponds to a fonctional meaning (feature ON, OFF, etc)
	int GetSeqIndexFromFuncMeaning(BINSEQ_CATEGORIES category, int seqDefIndex, FUNC_MEANING meaning);
	// Get the functional meaning of a "user choice"
	FUNC_MEANING GetFuncMeaningFromSeqIndex(BINSEQ_CATEGORIES category, int seqDefIndex, int seqIndex);

	// Returns a SEQ_VAR_TYPES value if ALL sequences of the seq definition have a variable with a UNIQUE common type.
	// If at least 1 possible sequence has no variable, the returned value is SVT_NO_VARIABLE.
	// If all sequences do not all have a variable with a common type, the returned value is SVT_NO_VARIABLE.
	SEQ_VAR_TYPES SeqDefHasVariable(BINSEQ_CATEGORIES category, int seqDefIndex);
	SEQ_VAR_TYPES SeqDefHasVariable(BINSEQ_CATEGORIES category, std::wstring seqDefName);

	// Get the user choice value for the given sequence definition in the list (referred to by index)
	// -1 = no choice. if <> -1, value should correspond to a sequence index in seqDefList[index].
	// Throws an exception if index is invalid
	int GetUserSelection(BINSEQ_CATEGORIES category, int seqDefIndex);
	int GetUserSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName);

	// Finds out which binary sequence currently matches game file's content
	// Throws exception if the file is not accessible
	// Returns -1 if the file does not match any sequence from the definition (file is not valid/not consistent with definition)
	// Returns the index of the matching sequence, if any (>=0)
	int GetFileSelection(BINSEQ_CATEGORIES category, int seqDefIndex);
	int GetFileSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName);

	// Set the user choice value for the given sequence definition in the list (referred to by index)
	// -1 = no choice. if <> -1, value should correspond to a sequence index in seqDefList[index].
	// Throws an exception if index is invalid
	// If the selected sequence contains a variable, you should call SetUserVarValue(...) after this call.
	void SetUserSelection(BINSEQ_CATEGORIES category, int seqDefIndex, int value);
	void SetUserSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName, int value);

	// Set the user choice value = default sequence (index 0), also sets the variable value (if any) to its default value
	void SetUserSelectionToDefault(BINSEQ_CATEGORIES category, int seqDefIndex);
	void SetUserSelectionToDefault(BINSEQ_CATEGORIES category, std::wstring seqDefName);

	// Write current "user selection" to file. First call SetUserSelection(...) and SetUserVarValue(...) if the sequence owns a variable
	// This method DOES NOT CHECK the sequence context (check that current file sequence matches a valid sequence def) before it writes to the file.
	// See CheckAndSetFileSelection(...) for secure writing to file.
	// Returns >0 if OK, 0 if a technical error occurred (file is read-only, for example)
	// Returns -1 if no "user selection" has been set
	int SetFileSelection(BINSEQ_CATEGORIES category, int seqDefIndex);
	int SetFileSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName);

	// Write current "user selection" to file. First call SetUserSelection(...) and SetUserVarValue(...) if the sequence owns a variable
	// This method checks the sequence context (check that current file sequence matches a valid sequence def) before it writes to the file.
	// Returns >0 if OK, 0 if a technical error occurred (file is read-only, for example)
	// Returns -1 if no "user selection" has been set
	// Returns -2 if the existing sequence in the file does not match a valid sequence (nothing is written)
	int CheckAndSetFileSelection(BINSEQ_CATEGORIES category, int seqDefIndex);
	int CheckAndSetFileSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName);


	// Template functions MUST be entirely defined in header file (to avoid link error)
	// GetUserVarValue: get the user-set variable value
	// data type must match the SELECTED bin sequence in the BinarySeqDefinition of index=seqIndex.
	// This means you must call SetUserSelection(...) first to choose the sequence and then get the value
	template<typename varType> void GetUserVarValue(BINSEQ_CATEGORIES category, int seqDefIndex, varType *var) {
		BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
		if (sds == NULL) { throw std::exception("Invalid file version/category"); }
		sds->GetUserVarValue(seqDefIndex, var);
	}
	template<typename varType> void GetUserVarValue(BINSEQ_CATEGORIES category, std::wstring seqDefName, varType *var) {
		BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
		if (sds == NULL) { throw std::exception("Invalid file version/category"); }
		sds->GetUserVarValue(sds->FindSeqDefinitionIndex(seqDefName), var);
	}

	// Write current user-defined variable value to a wstring
	std::wstring DumpUserVarValue(BINSEQ_CATEGORIES category, int seqDefIndex);
	// Write current file variable value to a wstring
	std::wstring DumpFileVarValue(BINSEQ_CATEGORIES category, int seqDefIndex);
	// Write default variable value to a wstring
	std::wstring DumpDefaultVarValue(BINSEQ_CATEGORIES category, int seqDefIndex);

	// Write current file content for sequence
	std::wstring DumpFileSeqContent(BINSEQ_CATEGORIES category, int seqDefIndex);
	// Write all sequences content for a BinarySeqDefinition
	std::wstring DumpAllDefSequencesContent(BINSEQ_CATEGORIES category, int seqDefIndex, wchar_t seqIndexSeparator = _T('\n'));

	template<typename varType> void GetFileVarValue(BINSEQ_CATEGORIES category, int seqDefIndex, varType *var) {
		if ((this->fileStatus != FS_READ_ONLY) && (this->fileStatus != FS_READ_WRITE)) {
			throw std::exception("Invalid file status");
		}
		BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
		if (sds == NULL) { throw std::exception("Invalid file version/category"); };
		FileEditor *fe = new FileEditor(this->gameFile);
		fe->GetVarValue(sds->GetBinSeqDefinition(seqDefIndex), this->GetFileSelection(category, seqDefIndex), var);
		delete fe;
		fe = NULL;
	}
	template<typename varType> void GetFileVarValue(BINSEQ_CATEGORIES category, std::wstring seqDefName, varType *var) {
		BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
		if (sds == NULL) { throw std::exception("Invalid file version/category"); };
		this->GetFileVarValue(category, sds->FindSeqDefinitionIndex(seqDefName), var);
	}


	// Template functions MUST be entirely defined in header file (to avoid link error)
	// SetUserVarValue: get the user-set variable value
	// data type must match the SELECTED bin sequence in the BinarySeqDefinition of index=seqIndex.
	// This means you must call SetUserSelection(...) first to choose the sequence and then set the value
	template<typename varType> void SetUserVarValue(BINSEQ_CATEGORIES category, int seqDefIndex, varType *var) {
		BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
		if (sds == NULL) { throw std::exception("Invalid file version/category"); };
		sds->SetUserVarValue(seqDefIndex, var);
	}
	template<typename varType> void SetUserVarValue(BINSEQ_CATEGORIES category, std::wstring seqDefName, varType *var) {
		BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
		if (sds == NULL) { throw std::exception("Invalid file version/category"); };
		sds->SetUserVarValue(sds->FindSeqDefinitionIndex(seqDefName), var);
	}

	// Shortcut method equivalent to SetUserSelection(...) + SetUserVarValue(...)
	template<typename varType> void SetUserSelectionWithVarValue(BINSEQ_CATEGORIES category, int seqDefIndex, int userChoice, varType *var) {
		this->SetUserSelection(category, seqDefIndex, userChoice);
		this->SetUserVarValue(category, seqDefIndex, var);
	}
	template<typename varType> void SetUserSelectionWithVarValue(BINSEQ_CATEGORIES category, std::wstring seqDefName, int userChoice, varType *var) {
		BinarySeqDefSet *sds = this->gameDataDefinitions.GetSeqDefSet(this->GetFileVersion(), category);
		if (sds == NULL) { throw std::exception("Invalid file version/category"); };
		int seqDefIndex = sds->FindSeqDefinitionIndex(seqDefName);
		this->SetUserSelectionWithVarValue(category, seqDefIndex, userChoice, var);
	}

	// Set user value from sequence definition default value (from binary sequence itself)
	void SetUserVarValueFromDefault(BINSEQ_CATEGORIES category, int seqDefIndex);
	// Set user value from sequence definition default value (from binary sequence itself)
	void SetUserVarValueFromDefault(BINSEQ_CATEGORIES category, std::wstring seqDefName);

	template<typename varType> bool CompareFileAndSequenceValues(BINSEQ_CATEGORIES category, int seqDefIndex, int seqIndex, BinarySeqDefinition *bsd) {
		varType val1;
		varType val2;
		this->GetFileVarValue(category, seqDefIndex, &val1);
		bsd->GetDefVarValue(seqIndex, &val2);
		return val1 == val2;
	}

	bool IsFileSelectionDefault(BINSEQ_CATEGORIES category, int seqDefIndex);

	// Returns a string that lists all sequence definition names for a given category
	// Returns an empty string if an error occurs
	std::wstring DumpSequenceDefNames(BINSEQ_CATEGORIES category, std::wstring separator = _T(", "), bool only_pending = false);
	std::wstring DumpSequenceDefInfo(BINSEQ_CATEGORIES category, std::wstring separator = _T(", "), bool only_pending = false, bool only_file_non_default = false, bool with_obsoletes = false);

	std::wstring ExportToBinaryPatcherIni(BINSEQ_CATEGORIES category, int seqDefIndex);
	std::wstring ExportToBinaryPatcherIni(BINSEQ_CATEGORIES category, std::wstring seqDefName);

	int EmpiresXFileManager::GetSeqDefCount(BINSEQ_CATEGORIES category);

	// Searches in all sequences (all categories) a conflicting sequence definition.
	// Two sequence definitions are conflicting if their offset intervals have a common part.
	// Returns true if at least 1 sequence definition (from any category) has conflicting offsets.
	// If a conflict is found, foundCategory and foundSeqDefIndex are updated with conflicting sequence definition values
	// If no conflict is found, foundCategory and foundSeqDefIndex are updated with BC_NONE and -1.
	bool FindConflictingSequenceDef(BINSEQ_CATEGORIES searchedCategory, int searchedSeqDefIndex, BINSEQ_CATEGORIES &foundCategory, int &foundSeqDefIndex);
	bool FindConflictingSequenceDef(BINSEQ_CATEGORIES searchedCategory, std::wstring searchedSeqDefName, BINSEQ_CATEGORIES &foundCategory, int &foundSeqDefIndex);

	// Returns true if the referenced sequence definition is obsolete (should be set back to its default value)
	bool IsObsolete(BINSEQ_CATEGORIES category, int seqDefIndex);
	bool IsObsolete(BINSEQ_CATEGORIES category, std::wstring seqDefName);

	// Required call to iterate on a BINSEQ_CATEGORIES.
	// Returns the sequence definition index.
	// Returns -1 if iterating is not possible
	// Warning: 0 is a valid result
	int IteratorFirst(BINSEQ_CATEGORIES category);
	// Returns the next sequence definition index.
	// Returns -1 if iterating is not possible (EOF)
	// Warning: 0 is a valid result
	int IteratorNext();


private:
	wstring gameFileName;
	AOE_FILE_VERSION gameFileVersion;
	AOE_binData gameDataDefinitions;
	long fileSize;
	FILE *gameFile;
	FILESTATUS fileStatus;
	BINSEQ_CATEGORIES it_category;
	int it_current_index;

	bool CheckExeMagicNumber();
	bool CheckVersionBinarySequence();
	bool FindGameVersion();
	int OpenGameFile(const char *argMode = "rb");
	void SetFileName(wstring argFileName);
	FILESTATUS GetStatusFromFileMode(const char *argMode);

	
	// Internal API to call WriteFromSequenceUsingValue with the relevant variable type
	// Returns 0 if a tech error occured.
	// Returns >0 if OK
	int SetUserSelectionUsingValue(FileEditor *fe, BinarySeqDefSet *sds, int seqDefIndex, int userChoice) {
		switch (sds->GetBinSeqDefinition(seqDefIndex)->GetVarType(userChoice)) {
		case SVT_INT_1B: return _SetUserSelectionUsingValue<char>(fe, sds, seqDefIndex, userChoice); break;
		case SVT_INT_2B: return _SetUserSelectionUsingValue<short int>(fe, sds, seqDefIndex, userChoice); break;
		case SVT_INT_4B: return _SetUserSelectionUsingValue<long int>(fe, sds, seqDefIndex, userChoice); break;
		case SVT_FLOAT_4B: return _SetUserSelectionUsingValue<float>(fe, sds, seqDefIndex, userChoice); break;
		default: throw std::exception("Invalid variable type");
			break;
		}
	}

	// Template functions MUST be entirely defined in header file (to avoid link error)
	// Internal method
	// Returns >0 if OK.
	// Returns 0 if a tech error occured
	template<typename varType> int _SetUserSelectionUsingValue(FileEditor *fe, BinarySeqDefSet *sds, int seqDefIndex, int userChoice) {
		varType var;
		sds->GetUserVarValue(seqDefIndex, &var);
		return fe->WriteFromSequenceUsingValue(sds->GetBinSeqDefinition(seqDefIndex), userChoice, &var);
	}
};
