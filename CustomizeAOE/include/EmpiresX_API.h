#include <tchar.h>
#include <string>
#include "FileSelector.h"
#include "AOE_const_global.h"
#include "EmpiresXFileManager.h"


#pragma once
class EmpiresX_API
{
public:
	EmpiresX_API();
	~EmpiresX_API();
	bool SetFileNameFromRegistry();
	bool SetFileNameManually();
	bool SetFileName(std::wstring fileName);
	std::wstring GetFileName();
	std::wstring GetFileVersion();
	bool IsFileOpen();
	bool OpenEmpiresXFile();
	bool CloseEmpiresXFile();

	bool CancelPendingChanges(bool onlyInvalids = false);
	bool CancelPendingChanges(BINSEQ_CATEGORIES category, bool onlyInvalids = false);
	bool CancelInvalidPendingChanges();
	bool HasPendingChanges();
	SEQ_VAR_TYPES OptionHasVariable(BINSEQ_CATEGORIES category, std::wstring seqDefName);
	int GetSeqIndexFromFuncMeaning(BINSEQ_CATEGORIES category, std::wstring seqDefName, FUNC_MEANING meaning);
	BINSEQ_CATEGORIES FindCategory(std::wstring seqDefName);
	EmpiresXFileManager *GetAOEManager() { return &this->aoeManager; } // Gives access to more low-level methods. Can not return NULL.

	bool GetCurrentResolution(unsigned short int *x, unsigned short int *y);
	bool SetResolution(unsigned short int x, unsigned short int y, bool preserveMapPos);
	bool SetMaxSelectedUnits(unsigned char number);
	bool SetWindowedMode(bool activate);
	bool SetManageAI(bool activate);
	bool SetROR_API(bool activate);
	bool SetAllROR_API_optionals(bool activate);
	int HasMissingTechFixes();
	bool SetAllTechFixes();
	bool HasObsoleteSequencesInstalled();
	bool RemoveObsoletes();
	bool ResetToDefault(BINSEQ_CATEGORIES category);
	
	bool SetOptionSelection(std::wstring seqDefName, int userSelection);
	bool SetOptionSelectionWithInt1(std::wstring seqDefName, int userSelection, char value);
	bool SetOptionSelectionWithInt2(std::wstring seqDefName, int userSelection, short int value);
	bool SetOptionSelectionWithInt4(std::wstring seqDefName, int userSelection, long int value);
	bool SetOptionSelectionWithFloat(std::wstring seqDefName, int userSelection, float value);
	
	bool SetSeqSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName, int userSelection); // To be used if there is no variable
	bool SetSeqSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName, int userSelection, std::wstring s_value);

	bool CheckPendingChangeSequences();
	bool CheckPendingChangeSequences(BINSEQ_CATEGORIES category, bool ignoreObsolete);
	int WriteChangesToFile();
	int WriteChangesToFile(BINSEQ_CATEGORIES category);

	// Make a user choice for a sequence that has no variable
	bool SetSeqSelectionWithoutVariable(BINSEQ_CATEGORIES category, std::wstring seqDefName, int userSelection) {
		try {
			int seqDefIndex = this->aoeManager.GetSeqDefIndex(category, seqDefName);
			this->aoeManager.SetUserSelection(category, seqDefIndex, userSelection);
		}
		catch (std::exception) {
			return false;
		}
		return true;
	}

	// Template functions MUST be entirely defined in header file (to avoid link error)
	// Make a user choice for a sequence that has a variable
	template<typename varType> bool SetSeqSelectionWithVariable(BINSEQ_CATEGORIES category, std::wstring seqDefName, int userSelection, varType value) {
		try {
			int seqDefIndex = this->aoeManager.GetSeqDefIndex(category, seqDefName);
			this->aoeManager.SetUserSelectionWithVarValue(category, seqDefIndex, userSelection, &value);
		}
		catch (std::exception) {
			return false;
		}
		return true;
	}


	// Non-critical/low priority functions
	bool SetSuggestedOptions();
	bool SetBasicCustomROROptions();
	bool SetOtherCustomROROptions();

	// More or less debug methods
	void DumpAllNames(std::wstring separator = _T(", "));
	void DumpAllInfo(std::wstring separator = _T(", "), bool withObsoletes = false);
	void DumpPendings(std::wstring separator = _T(", "));
	std::wstring DumpPendingsToWString(std::wstring separator = _T(", "));
	void DumpNotDefault(std::wstring separator = _T(", "));

private:
	EmpiresXFileManager aoeManager;
	FileSelector aoeFileSelector;

	bool CheckPendingChangeSequences(bool ignoreObsolete);
	int WriteChangesToFile_internal(BINSEQ_CATEGORIES category);

};

