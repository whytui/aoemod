#include "EmpiresX_API.h"


EmpiresX_API::EmpiresX_API()
{
}


EmpiresX_API::~EmpiresX_API()
{
}


// Automatically gets the target executable file with registry keys
// Returns true if the file exists
bool EmpiresX_API::SetFileNameFromRegistry() {
	return this->aoeFileSelector.ReadRegistry();
}


// Asks the user to select manually the target executable file
// Returns true if the file exists
bool EmpiresX_API::SetFileNameManually() {
	return this->aoeFileSelector.AskFileName();
}


// Directly supplies the target filename
// Returns true if the file exists
bool EmpiresX_API::SetFileName(std::wstring fileName) {
	return this->aoeFileSelector.SetFileName(fileName);
}


// Returns _T("") if no valid file has been selected
std::wstring EmpiresX_API::GetFileName() {
	return this->aoeFileSelector.getFileName();
}


// Returns the wstring representation of current (exe) file version
std::wstring EmpiresX_API::GetFileVersion() {
	return AOEFileVersionLabel[this->aoeManager.GetFileVersion()];	
}


// Returns true if the EXE file is open (might be read, write...)
bool EmpiresX_API::IsFileOpen() {
	return this->aoeManager.GetFileStatus() != FS_NOT_OPEN;
}

// Returns true if the game was opened successfully
// Returns false if the file is not valid or if the version is not supported
bool EmpiresX_API::OpenEmpiresXFile() {
	if (this->aoeManager.OpenCheckExe(this->aoeFileSelector.getFileName()) == AOE_INVALID_FILE) {
		return false;
	};
	return true;
}


// Returns true if the game was close successfully
// Returns false if there is an error.
bool EmpiresX_API::CloseEmpiresXFile() {
	return (this->aoeManager.CloseGameFile() == 0);
}


// Cancel user selection for all sequences for a category.
// onlyInvalids: if true, only "invalid" changes will be reverted (where file content is not compatible)
bool EmpiresX_API::CancelPendingChanges(BINSEQ_CATEGORIES category, bool onlyInvalids) {
	try {
		for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(category); seqDefIndex++) {
			if (!onlyInvalids || (this->aoeManager.GetFileSelection(category, seqDefIndex) == -1 )) {
				this->aoeManager.SetUserSelection(category, seqDefIndex, -1);
			}
		}
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return true;
}


// Cancel user selection for all sequences for all categories.
// onlyInvalids: if true, only "invalid" changes will be reverted (where file content is not compatible)
bool EmpiresX_API::CancelPendingChanges(bool onlyInvalids) {
	try {
		for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
			this->CancelPendingChanges(category, onlyInvalids);
		}
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return true;
}


// Returns true if at least 1 change is pending (for any category)
bool EmpiresX_API::HasPendingChanges() {
	try {
		for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
			for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(category); seqDefIndex++) {
				if (this->aoeManager.GetUserSelection(category, seqDefIndex) > -1) { return true; }
			}
		}
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return false;
}


// Gets current file resolution if possible. Returns true if successful.
// Updates x and y parameters when successful.
// If resolution values are inconsistent, returns false (out x and y values are arbitrary)
// Result is only based on HSize and VSize sequences.
bool EmpiresX_API::GetCurrentResolution(unsigned short int *x, unsigned short int *y) {
	short int var_2b_x; // 2-bytes var
	short int var_2b_y; // 2-bytes var
	*x = 0;
	*y = 0;
	std::wstring seqDefName;
	int fileSelection;
	for (int i = 1; i <= 21; i++) {
		seqDefName = _T("HSize") + to_wstring(i);
		fileSelection = this->aoeManager.GetFileSelection(BC_RESOLUTION, seqDefName);
		if (fileSelection < 0) { return false; }
		this->aoeManager.GetFileVarValue(BC_RESOLUTION, seqDefName, &var_2b_x);
		if (*x == 0) { // first element: initialize
			*x = var_2b_x;
		} else {
			if (*x != var_2b_x) { return false; } // inconsistent sequences
		}
	}
	for (int i = 1; i <= 11; i++) {
		seqDefName = _T("VSize") + to_wstring(i);
		fileSelection = this->aoeManager.GetFileSelection(BC_RESOLUTION, seqDefName);
		if (fileSelection < 0) { return false; }
		this->aoeManager.GetFileVarValue(BC_RESOLUTION, seqDefName, &var_2b_y);
		if (*y == 0) { // first element: initialize
			*y = var_2b_y;
		} else {
			if (*y != var_2b_y) { return false; } // inconsistent sequences
		}
	}
	return true;
}


// Sets user selection for resolution sequences.
// This does not update the target file, only user selection
bool EmpiresX_API::SetResolution(unsigned short int x, unsigned short int y, bool preserveMapPos) {
	short int var_2b_x = x; // 2-bytes var
	short int var_2b_y = y; // 2-bytes var
	std::wstring seqDefName;
	try {
		for (int i = 1; i <= 21; i++) {
			seqDefName = _T("HSize") + to_wstring(i);
			this->aoeManager.SetUserSelection(BC_RESOLUTION, seqDefName, 0);
			this->aoeManager.SetUserVarValue(BC_RESOLUTION, seqDefName, &var_2b_x);
		}
		for (int i = 1; i <= 11; i++) {
			seqDefName = _T("VSize") + to_wstring(i);
			this->aoeManager.SetUserSelection(BC_RESOLUTION, seqDefName, 0);
			this->aoeManager.SetUserVarValue(BC_RESOLUTION, seqDefName, &var_2b_y);
		}

		var_2b_y = y - 146;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("VPos_GameZoneBottom"), 0, &var_2b_y);
		var_2b_y = y - 120;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("VPos_Map"), 0, &var_2b_y);
		var_2b_y = y - 119;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("VPos_UnitInfoZone"), 0, &var_2b_y);
		var_2b_y = y - 118;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("VPos_ScoreInformation"), 0, &var_2b_y);
		var_2b_y = y - 36;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("VPos_HelpButton"), 0, &var_2b_y);
		var_2b_y = y - 118;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("VPos_ScoreButton"), 0, &var_2b_y);
		var_2b_y = y - 126;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("VPos_BottomPanel"), 0, &var_2b_y);
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("VPos_PanelImage"), 0, &var_2b_y);
		var_2b_x = x - 35;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("HPos_ScoreButton"), 0, &var_2b_x);
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("HPos_HelpButton"), 0, &var_2b_x);
		var_2b_x = (preserveMapPos ? 1024 : x) - 229;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("HPos_Map"), 0, &var_2b_x);
		var_2b_x = x - 72;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("HPos_MenuButton"), 0, &var_2b_x);
		var_2b_x = x - 180;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("HPos_DiplButton"), 0, &var_2b_x);
		var_2b_x = x - 252;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("HPos_ButtonsZone"), 0, &var_2b_x);
		var_2b_y = y - 176;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("VSize_GameZoneMinus30"), 0, &var_2b_y);
		var_2b_x = x - 532;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("HSize_AgePanel"), 0, &var_2b_x);

		var_2b_x = x / 2 - 80;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("HPos_PopLimit"), 0, &var_2b_x);
		var_2b_x = x - 6;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("SI_HPos_TopLabel"), 0, &var_2b_x);
		//SI_VSize_TextArea
		var_2b_x = x - 80;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("SI_HSize_TextArea"), 0, &var_2b_x);
		//SI_VPos_TextArea // 1 bytes /!\
		//SI_HPos_TextArea// 1 bytes /!\
		//SI_VSize_HistoryButton // 1 bytes /!\
		//SI_VSize_InstrButton // 1 bytes /!\
		//SI_VSize_HintsButton // 1 bytes /!\
		//SI_VSize_OkButton // 1 bytes /!\
		//SI_HSize_HistButton
		//SI_HSize_InstrButton
		//SI_HSize_HintsButton
		//SI_HSize_OkButton
		var_2b_y = y - 78;
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("SI_VPos_HistButton"), 0, &var_2b_y);
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("SI_VPos_InstrButton"), 0, &var_2b_y);
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("SI_VPos_HintsButton"), 0, &var_2b_y);
		this->aoeManager.SetUserSelectionWithVarValue(BC_RESOLUTION, _T("SI_VPos_OkButton"), 0, &var_2b_y);
		//SI_HPos_HistButton // 1 bytes /!\
		//SI_HPos_InstrButton
		//SI_HPos_HintsButton
		//SI_HPos_OkButton
		//SI_HPos_Map
		//SI_VPos_Map
		//UnknownSize1
		//UnknownSize2
		//UnknownSize3
		//UnknownSize4
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return true;
}


// Sets "user selection" so that the maximum number of selected units = <number>
// This does not update the target file, only user selection
// Returns false if an error occured
// @param number : maximum number of units that can be selected at once. Default value is 25.
bool EmpiresX_API::SetMaxSelectedUnits(unsigned char number) {
	if (number < 1) { return false; } // any lower value than 25 would be stupid, but allow them...
	long int var_4b = number; // 4-bytes variable
	std::wstring seqDefName;
	try {
		// Alloc: 4-bytes variable, = 4*(number+1) + 2140 (0x85C)
		// Yes, number+1, because the code uses 1 more room that actual number of selected units (first=last).
		var_4b = 4 * (number+1) + 0x85C + 4;
		for (int i = 1; i <= 7; i++) {
			seqDefName = _T("Alloc") + to_wstring(i);
			this->aoeManager.SetUserSelection(BC_SELECTED_UNITS, seqDefName, 0);
			this->aoeManager.SetUserVarValue(BC_SELECTED_UNITS, seqDefName, &var_4b); // alloc: 4 bytes variable
		}
		// Init buffer: no variable. Just select sequence 1
		this->aoeManager.SetUserSelection(BC_SELECTED_UNITS, _T("InitBuffer1"), 1);
		this->aoeManager.SetUserSelection(BC_SELECTED_UNITS, _T("InitBuffer2"), 1);

		// MaxNumber: 1 byte variable = the max number (no calculation).
		for (int i = 1; i <= 18; i++) {
			seqDefName = _T("MaxNumber") + to_wstring(i);
			this->aoeManager.SetUserSelection(BC_SELECTED_UNITS, seqDefName, 0);
			this->aoeManager.SetUserVarValue(BC_SELECTED_UNITS, seqDefName, &number); // MaxNumber: 1 byte variable
		}

		// BufferOffset sequences: no variable, just select seq 1
		for (int i = 1; i <= 58; i++) {
			seqDefName = _T("BufferOffset") + to_wstring(i);
			this->aoeManager.SetUserSelection(BC_SELECTED_UNITS, seqDefName, 1);
		}
		
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return true;
}


// Sets "user selection" to activate/deactivate windowed mode
// Warning: activating this requires wndmode.dll. See also SetROR_API(...).
// Returns true if successful
bool EmpiresX_API::SetWindowedMode(bool activate) {
	if (this->aoeManager.GetSeqDefCount(BC_WINDOWED_MODE) == 0) { return false; }
	try {
		for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(BC_WINDOWED_MODE); seqDefIndex++) {
			int choice = activate ? 1 : 0;
			this->aoeManager.SetUserSelection(BC_WINDOWED_MODE, seqDefIndex, choice);
		}
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return true;
}


// Sets "user selection" to activate/deactivate "Manage AI" features/fixes
// Requires ROR_API.dll
// Returns true if OK
bool EmpiresX_API::SetManageAI(bool activate) {
	if (this->aoeManager.GetSeqDefCount(BC_MANAGE_AI) == 0) { return false; }
	try {
		int choice = activate ? 1 : 0;
		for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(BC_MANAGE_AI); seqDefIndex++) {
			// All options in BC_MANAGE_AI are 0=default=OFF, 1=ON
			this->aoeManager.SetUserSelection(BC_MANAGE_AI, seqDefIndex, choice);
		}
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return true;
}


// Sets "user selection" to activate/deactivate ROR_API
// This only impacts REQUIRED changes for ROR_API, not optionals.
// If you disable ROR_API support, make sure you also disable ROR_API optional changes (see SetAllROR_API_optionals(...))
// Requires ROR_API.dll
// Returns true if OK
bool EmpiresX_API::SetROR_API(bool activate) {
	try {
		int choice = activate ? 1 : 0;
		this->aoeManager.SetUserSelection(BC_ROR_API, _T("ROR_API_DLL_initialization"), choice);
		this->aoeManager.SetUserSelection(BC_ROR_API, _T("ROR_API_DLL_API_methods"), choice);
		this->aoeManager.SetUserSelection(BC_ROR_API, _T("ROR_API_VarDataUpdate"), choice);
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return true;
}


// Sets "user selection" to activate/deactivate ROR_API optional changes.
// This only impacts optional changes for ROR_API, not REQUIRED changes (see SetROR_API).
// If you enable these options, make sure you also enable REQUIRED changes with SetROR_API(...)
// Requires ROR_API.dll
// Returns true if OK
bool EmpiresX_API::SetAllROR_API_optionals(bool activate) {
	try {
		for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(BC_ROR_API); seqDefIndex++) {
			int choice = activate ? 1 : 0;
			std::wstring seqDefName;
			seqDefName = this->aoeManager.GetSequenceDefName(BC_ROR_API, seqDefIndex);
			if ((seqDefName != _T("DLL_initialization")) && (seqDefName != _T("ROR_API_DLL_API_methods")) &&
				(seqDefName != _T("ROR_API_VarDataUpdate"))) {
				this->aoeManager.SetUserSelection(BC_ROR_API, seqDefIndex, choice);
			}
		}
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return true;
}


// Sets "user selection" so that all tech fixes are selected
// The OLD "GameSaveBug" fixes will NOT be installed (reset to default).
// Collectors edition CD crack must NOT be installed (not standard - reset to default)
// Requirement: all tech fixes have 2 sequences 0 (default) and 1 (with fix)
// Tip: Save the file immediatly after calling this so you can work on a clean file 
// and avoid confusion between these "pending changes" and user's selections.
bool EmpiresX_API::SetAllTechFixes() {
	try {
		for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(BC_TECH_FIXES); seqDefIndex++) {
			std::wstring seqDefName = this->aoeManager.GetSequenceDefName(BC_TECH_FIXES, seqDefIndex);
			bool obsolete = this->aoeManager.IsObsolete(BC_TECH_FIXES, seqDefName);
			int selectionToSet = obsolete ? 0 : 1;
			int curSeqSelection = this->aoeManager.GetFileSelection(BC_TECH_FIXES, seqDefIndex);

			// Obsolete sequence definitions: ignore the error if any
			if ((curSeqSelection < 0) && (!obsolete)) { throw std::exception("Non-matching sequence for a required tech fix."); }
			
			// If the sequence is valid AND the current selection is not the good one...
			if ((curSeqSelection >= 0) && (curSeqSelection != selectionToSet)) {
				// This will work as long as all tech fixes have (at least) 2 sequences: 0 (default) and 1 (with fix)
				this->aoeManager.SetUserSelection(BC_TECH_FIXES, seqDefIndex, selectionToSet);
			}
		}
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return true;
}


// Sets ("user selection") all sequences (and their variables, if any) to default values.
// i.e. this sets user selections to 0 (+ var to its default).
// Warning: this is different from setting user selections to -1 (see CancelPendingChanges for that)
// Obsolete sequences will be ignored (will NOT be modified)
// Returns true if successful
bool EmpiresX_API::ResetToDefault(BINSEQ_CATEGORIES category) {
	try {
		for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(category); seqDefIndex++) {
			if (!this->aoeManager.IsObsolete(category, seqDefIndex)) {
				this->aoeManager.SetUserSelectionToDefault(category, seqDefIndex); // Also updates variable, if any
			}
		}
	}
	catch (std::exception &e) {
		printf("ERROR: %s\n", e.what());
		return false;
	}
	return true;
}


// Sets "user selection" so that all obsolete sequences are disabled (removed).
// See also HasObsoleteSequencesInstalled()
// This method IGNORES all tech fixes (see SetAllTechFixes() for that)
// Tip: Save the file immediatly after calling this so you can work on a clean file 
// and avoid confusion between these "pending changes" and user's selections.
bool EmpiresX_API::RemoveObsoletes() {
	for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
		// Ignore tech fixes
		if (category != BC_TECH_FIXES) {
			for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(category); seqDefIndex++) {
				// Work on obsoletes only
				if (this->aoeManager.IsObsolete(category, seqDefIndex)) {
					int curSeqSelection = this->aoeManager.GetFileSelection(category, seqDefIndex);
					if (curSeqSelection > 0) {
						this->aoeManager.SetUserSelection(category, seqDefIndex, 0);
					}
					// If curSeqSelection == 0 : nothing to do
					// If curSeqSelection == -1 : sequence does not match. It is possible for an obsolete sequence, just ignore it (some non-obsolete sequence has probably overwritten this sequence).
				}
			}
		}
	}
	return true;
}


// Returns 0 if tech fixes are present in target file.
// Returns >0 if some tech fixes are missing (not selected)
// Returns <0 if an error occurs (some non-obsolete tech fix has a non-matching sequence, no file selected...)
// The OLD "GameSaveBug" fixes should NOT be installed.
// Collectors edition CD crack must NOT be installed (not standard)
// If the result is true, then you should run SetAllTechFixes() before doing anything else.
int EmpiresX_API::HasMissingTechFixes() {
	if (this->aoeManager.GetFileStatus() == FS_NOT_OPEN) { return -2; }
	for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(BC_TECH_FIXES); seqDefIndex++) {
		int curSeqSelection = this->aoeManager.GetFileSelection(BC_TECH_FIXES, seqDefIndex);
		std::wstring seqDefName = this->aoeManager.GetSequenceDefName(BC_TECH_FIXES, seqDefIndex);
		// Use default values for these elements
		bool obsolete = this->aoeManager.IsObsolete(BC_TECH_FIXES, seqDefName);

		if ((curSeqSelection == -1) && (!obsolete)) { return -1; } // no sequence matches (ok if obsolete) - this is an error case
		
		// Dirty hack: always ignore CD_CHECK (for 2015 october ROR_API migration)
		// Don't raise an error for CD_CHECK, so that ROR_API obsoletes can be removed.
#pragma message("[HasMissingTechFixes] Hack to ignore CD_CHECK for 2015-october ROR_API technical migration.")
		if (seqDefName != _T("CD_CHECK")) {
			if ((curSeqSelection == 0) && !obsolete) { return 2; } // default sequence (ok if obsolete)
			if ((curSeqSelection > 0) && obsolete) { return 3; } // non-default sequence (ok if non-obsolete)
		}
	}
	return 0;
}


// Returns true if some obsolete sequence is installed (enabled) in target file
// This method IGNORES all tech fixes (see AreTechFixesInstalled() for that)
// If the result is true, then you should run RemoveObsoletes() before doing anything else.
bool EmpiresX_API::HasObsoleteSequencesInstalled() {
	if (this->aoeManager.GetFileStatus() == FS_NOT_OPEN) { return false; }
	for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
		// Ignore tech fixes
		if (category != BC_TECH_FIXES) {
			for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(category); seqDefIndex++) {
				// Work on obsoletes only
				if (this->aoeManager.IsObsolete(category, seqDefIndex)) {
					int curSeqSelection = this->aoeManager.GetFileSelection(category, seqDefIndex);
					if (curSeqSelection > 0) { return true; }
					// if curSeqSelection == -1 : sequence does not match. It is possible for an obsolete sequence, just ignore it.
				}
			}
		}
	}
	return false;
}



// Shortcut to call SetSeqSelection for BC_OPTIONS, without variable
bool EmpiresX_API::SetOptionSelection(std::wstring seqDefName, int userSelection) {
	return this->SetSeqSelection(BC_OPTIONS, seqDefName, userSelection, _T(""));
}

// Sets the user selection for an option that has a 1-byte int variable
// Returns false if an error occurred.
bool EmpiresX_API::SetOptionSelectionWithInt1(std::wstring seqDefName, int userSelection, char value) {
	return this->SetSeqSelectionWithVariable(BC_OPTIONS, seqDefName, userSelection, value);
}

// Sets the user selection for an option that has a 2-bytes int variable
// Returns false if an error occurred.
bool EmpiresX_API::SetOptionSelectionWithInt2(std::wstring seqDefName, int userSelection, short int value) {
	return this->SetSeqSelectionWithVariable(BC_OPTIONS, seqDefName, userSelection, value);
}

// Sets the user selection for an option that has a 4-bytes int variable
// Returns false if an error occurred.
bool EmpiresX_API::SetOptionSelectionWithInt4(std::wstring seqDefName, int userSelection, long int value) {
	return this->SetSeqSelectionWithVariable(BC_OPTIONS, seqDefName, userSelection, value);
}

// Sets the user selection for an option that has a 4-bytes float variable
// Returns false if an error occurred.
bool EmpiresX_API::SetOptionSelectionWithFloat(std::wstring seqDefName, int userSelection, float value) {
	return this->SetSeqSelectionWithVariable(BC_OPTIONS, seqDefName, userSelection, value);
}



bool EmpiresX_API::SetSeqSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName, int userSelection) {
	return this->SetSeqSelection(category, seqDefName, userSelection, _T(""));
}

// A very generic method to make a user selection (user choice) and set a variable value (if any)
// s_value is unused if the sequence has no variable (you can provide _T(""))
// Returns false if an error occured (including bad conversion from wstring to the relevant variable type)
// The caller must make sure he's calling with consistent arguments (valid userSelection, correct format for s_value...) !
bool EmpiresX_API::SetSeqSelection(BINSEQ_CATEGORIES category, std::wstring seqDefName, int userSelection, std::wstring s_value) {
	SEQ_VAR_TYPES vtype = this->OptionHasVariable(category, seqDefName);
	std::wstring str;
	float f;
	char int1;
	short int int2;
	long int int4;
	try {
		switch (vtype) {
		case SVT_NO_VARIABLE:
			this->SetSeqSelectionWithoutVariable(category, seqDefName, userSelection);
			return true;
		case SVT_INT_1B:
			int1 = stoi(s_value);
			SetSeqSelectionWithVariable(category, seqDefName, userSelection, int1);
			return true;
		case SVT_INT_2B:
			int2 = stoi(s_value);
			SetSeqSelectionWithVariable(category, seqDefName, userSelection, int2);
			return true;
		case SVT_INT_4B:
			int4 = stoi(s_value);
			SetSeqSelectionWithVariable(category, seqDefName, userSelection, int4);
			return true;
		case SVT_FLOAT_4B:
			f = stof(s_value);
			SetSeqSelectionWithVariable(category, seqDefName, userSelection, f);
			return true;
		default: return false;
		}
	}
	catch (std::invalid_argument) {
		return false;
	}
	catch (std::out_of_range) {
		return false;
	}
	catch (std::exception &e) {
		printf("Unexpected error: %s", e.what());
		return false;
	}
}


// Returns a SEQ_VAR_TYPES value if ALL sequences of the seq definition have a variable with a UNIQUE common type.
// If at least 1 possible sequence has no variable, the returned value is SVT_NO_VARIABLE.
// If all sequences do not all have a variable with a common type, the returned value is SVT_NO_VARIABLE.
SEQ_VAR_TYPES EmpiresX_API::OptionHasVariable(BINSEQ_CATEGORIES category, std::wstring seqDefName) {
	return this->aoeManager.SeqDefHasVariable(category, seqDefName);
}


// Given a functional meaning, returns the corresponding sequence index
// Returns -1 if no sequence has a corresponding "func meaning".
// Returns -2 if there is a technical error.
// Returns >= 0 if OK: returns the lower index that corresponds (it is possible - but stupid? - to define several sequences with a same meaning)
int EmpiresX_API::GetSeqIndexFromFuncMeaning(BINSEQ_CATEGORIES category, std::wstring seqDefName, FUNC_MEANING meaning) {
	return this->aoeManager.GetSeqIndexFromFuncMeaning(category, this->aoeManager.GetSeqDefIndex(category, seqDefName), meaning);
}


// Find the category of a sequence definition from its name.
// The result is uncertain if several sequence definitions (from different categories) have the same name.
// Returns BC_NONE if not found
BINSEQ_CATEGORIES EmpiresX_API::FindCategory(std::wstring seqDefName) {
	for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
		for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(category); seqDefIndex++) {
			if (this->aoeManager.GetSequenceDefName(category, seqDefIndex) == seqDefName) { return category; }
		}
	}
	return BC_NONE;
}


// For all pending changes (user selection is <> -1), checks in target file if the current sequence is consistent with definitions
// Returns true if all pending changes are compatible with current file content
// Returns false if at least one pending change has compatibility issues
bool EmpiresX_API::CheckPendingChangeSequences(){
	return this->CheckPendingChangeSequences(false);
}


// For all pending changes (user selection is <> -1), checks in target file if the current sequence is consistent with definitions
// The check is made for all sequence definitions for the provided category
// @Param: ignoreObsolete = if true, obsolete sequence definitions are ignored (check is not run on them)
// Returns true if all pending changes are compatible with current file content
// Returns false if at least one pending change has compatibility issues
bool EmpiresX_API::CheckPendingChangeSequences(BINSEQ_CATEGORIES category, bool ignoreObsolete) {
	if (this->aoeManager.GetFileStatus() == FS_NOT_OPEN) { return false; } // exception?
	for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(category); seqDefIndex++) {
		// Only check where a user selection has been made
		if ((this->aoeManager.GetUserSelection(category, seqDefIndex) > -1) && 
			(!ignoreObsolete || !this->aoeManager.IsObsolete(category, seqDefIndex))) {
			int checkResult = this->aoeManager.GetFileSelection(category, seqDefIndex);
			if (checkResult < 0) {
				std::wstring seqDefName = this->aoeManager.GetSequenceDefName(category, seqDefIndex);
				wprintf(_T("ERROR while writing sequence to file, sequence context is not valid. Abort. Category=%d, index=%s\n"), int(category), seqDefName.c_str());
				return false;
			}
		}
	}
	return true;
}


// For all pending changes (user selection is <> -1), checks in target file if the current sequence is consistent with definitions
// Returns true if all pending changes are compatible with current file content
// Returns false if at least one pending change has compatibility issues
// @Param: ignoreObsolete = if true, obsolete sequence definitions are ignored (incompatible file content will not raise an error)
bool EmpiresX_API::CheckPendingChangeSequences(bool ignoreObsolete) {
	for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
		if (!CheckPendingChangeSequences(category, ignoreObsolete)) {
			return false;
		}
	}
	return true;
}


// Saves all user selections to file (and resets user selections to -1 when done)
// For sequences that have no user selection (-1), does not write anything: the file sequence is preserved as is.
// Returns < 0 if an error occurred. WARNING: in that case, game file is closed in this->aoeManager. You'll have to open it again.
// Returns the number of written sequences (>=0) if successful.
int EmpiresX_API::WriteChangesToFile() {
	if (this->aoeManager.GetFileStatus() == FS_NOT_OPEN) { return -4; }
	int writtenCount = 0;
	try {
		// First check in file all sequences we need to write. Allow tech fixes'CD_CHECK* to be wrong (because of other changes that can overwrite this zone)
		if (!this->CheckPendingChangeSequences(true)) {
			this->aoeManager.CloseGameFile(); // Method consistency: make sure the game file is always closed when returning an "error" status
			return -2;
		}

		if (!this->aoeManager.SwitchToWriteMode()) { // throws exceptions
			// Note: the file has been closed in this->aoeManager
			return -3;
		}
		for (BINSEQ_CATEGORIES category = BINSEQ_CATEGORIES(BC_NONE + 1); category < BC_COUNT; category = BINSEQ_CATEGORIES(category + 1)) {
			int writtenTmp = this->WriteChangesToFile_internal(category);
			if (writtenTmp < 0) {
				this->aoeManager.CloseGameFile(); // Method consistency: make sure the game file is always closed when returning an "error" status
				return -1;
			}
			writtenCount += writtenTmp;
		}
		this->aoeManager.SwitchToReadMode();
		return writtenCount;
	}
	catch (...) {
		return -5;
	}
}


// Saves all user selections to file
// For sequences that have no user selection (-1), does not write anything: the file sequence is preserved as is.
// Returns < 0 if an error occurred. WARNING: in that case, game file is closed in this->aoeManager. You'll have to open it again.
// Returns the number of written sequences (>=0) if successful.
// If at least one "pending change" is not compatible with current file content, the operation aborts and nothing is written to the file.
int EmpiresX_API::WriteChangesToFile(BINSEQ_CATEGORIES category) {
	if (this->aoeManager.GetFileStatus() == FS_NOT_OPEN) { return -4; }
	int writtenCount = 0;
	// First check in file all sequences we need to write.
	if (!this->CheckPendingChangeSequences(category, false)) {
		this->aoeManager.CloseGameFile(); // Method consistency: make sure the game file is always closed when returning an "error" status
		return -2;
	}

	if (!this->aoeManager.SwitchToWriteMode()) {
		// Note: the file has been closed in this->aoeManager
		return -3;
	}

	writtenCount = this->WriteChangesToFile_internal(category);
	if (writtenCount < 0) {
		this->aoeManager.CloseGameFile(); // Method consistency: make sure the game file is always closed when returning an "error" status
		return writtenCount;
	}
	
	this->aoeManager.SwitchToReadMode();
	return writtenCount;
}


// This is an internal common method, do not use it directly.
// This method does not manage file open/close operations and does not run initial checks.
// This method resets user selection to -1 if it has been written successfully
// Returns < 0 if an error occurred.
// Returns the number of written sequences (>=0) if successful.
// May throw exceptions
int EmpiresX_API::WriteChangesToFile_internal(BINSEQ_CATEGORIES category) {
	int writtenCount = 0;
	for (int seqDefIndex = 0; seqDefIndex < this->aoeManager.GetSeqDefCount(category); seqDefIndex++) {
		// Only update where a user selection has been made
		int currentUserSelection = this->aoeManager.GetUserSelection(category, seqDefIndex);
		if (currentUserSelection > -1) {
			bool go_on = true;
			// DEBUG
			wprintf(_T("Writing to file: Category=%d, name=%s, selection=%d\n"), int(category), this->aoeManager.GetSequenceDefName(category, seqDefIndex).c_str(), currentUserSelection);
			// Use the secured write interface: only updates file if the sequence context is valid.
			// Even if we already checked that, this may still happen if definitions overwrite a common part of the file
			int writeResult = this->aoeManager.CheckAndSetFileSelection(category, seqDefIndex);
			writtenCount++;
			// 0=tech error, -2=invalid context, -1=no user choice (not possible here thanks to the "if")
			if (writeResult <= 0) {
				go_on = false;
				std::wstring seqDefName = this->aoeManager.GetSequenceDefName(category, seqDefIndex);
				if (writeResult == -2) {
					wprintf(_T("ERROR while write sequence to file, sequence context is not valid. Abort. Category=%d, index=%s\n"), int(category), seqDefName.c_str());
				} else {
					wprintf(_T("ERROR while write sequence to file. Technical error. Abort. Category=%d, index=%s\n"), int(category), seqDefName.c_str());
				}
				if (!go_on) {
					return -1; // Error
				}
			}
			this->aoeManager.SetUserSelection(category, seqDefIndex, -1); // reset user selection once it has been successfully written
		}
	}
	return writtenCount;
}


// Automatically sets "user selection" for recommended options (my preferences, haha ! Developer's privilege)
// Returns true if everything is OK
bool EmpiresX_API::SetSuggestedOptions() {
	bool result;
	result = this->SetROR_API(true);
	result = result && this->SetAllROR_API_optionals(true);
	result = result && 	this->SetMaxSelectedUnits(100);
	result = result && this->SetWindowedMode(true);
	result = result && this->SetResolution(1920, 1200, true);
	result = result && this->SetOptionSelection(_T("AOEDebugToFile"), 0); // 0=default=no
	result = result && this->SetOptionSelectionWithInt1(_T("InitSN161_RequiredFirstBuilding"), 0, 1);
	result = result && this->SetOptionSelection(_T("InitSN188_SNAttackWinningPlayer"), 0); // // 0=default=yes
	result = result && this->SetOptionSelection(_T("InitSN194_CoopShareInformation"), 1); // 0=default=yes
	result = result && this->SetOptionSelection(_T("InitSN201_TrackPlayerHistory"), 0); // 0=default=yes
	result = result && this->SetOptionSelection(_T("InitSN215_AllowDiplomacyChangeOnAllyAttack"), 1); // default val=0=yes
	result = result && this->SetOptionSelectionWithInt4(_T("LVL5_BONUS_AMOUNT"), 0, 200);
	result = result && this->SetOptionSelection(_T("lvl5DisableGoldBonus"), 0); // 0=default=no
	result = result && this->SetOptionSelection(_T("lvl5DisableStoneBonus"), 1); // 0=default=no
	result = result && this->SetOptionSelection(_T("lvl5DisableWoodBonus"), 0); // 0=default=no
	result = result && this->SetOptionSelection(_T("PopulationPoints"), 1); // 0=default=no
	result = result && this->SetOptionSelectionWithInt1(_T("PointsPerTemple"), 0, 5); // default value=5
	result = result && this->SetOptionSelection(_T("disable_dislike_human_player"), 1); // 0=default=no
	result = result && this->SetOptionSelection(_T("FixAI_initialDiplomacy"), 1); // 0=default=no
	result = result && this->SetOptionSelection(_T("gaiaX_cheat_code1"), 1); // 0=default=no
	result = result && this->SetOptionSelection(_T("gaiaX_cheat_code2"), 1); // 0=default=no
	result = result && this->SetOptionSelection(_T("gaiaX_cheat_code3"), 1); // 0=default=no
	result = result && this->SetOptionSelection(_T("play_cheat_code"), 0); // 0=default=no
	result = result && this->SetOptionSelection(_T("FULL_Computer_player_1"), 1); // default=no
	result = result && this->SetOptionSelection(_T("GenerateNameForHuman"), 1); // default=no
	result = result && this->SetOptionSelectionWithInt1(_T("SetSinglePlayerMaxPop"), 0, 50); // default val=50
	result = result && this->SetOptionSelectionWithInt1(_T("SetAIHousesMaxPop"), 0, 50); // default val=50
	result = result && this->SetOptionSelection(_T("MoreGameSpeeds1"), 1); // default=no
	result = result && this->SetOptionSelection(_T("MoreGameSpeeds2"), 1); // default=no
	result = result && this->SetOptionSelection(_T("PostIronAge_Item"), 1); // default=no
	result = result && this->SetOptionSelection(_T("PostIronAge_Setup"), 1); // default=no
	result = result && this->SetOptionSelection(_T("AdditionalMapType"), 1); // default=no
	result = result && this->SetOptionSelectionWithInt4(_T("AdditionalMapTypeChangeLabel"), 0, 9286); // default value=10610
	result = result && this->SetOptionSelection(_T("AdditionalMapType_FixLabelS"), 1); // default=no
	result = result && this->SetOptionSelectionWithInt4(_T("AdditionalMapType_FixLabelS_ChangeLabel"), 0, 9286); // default value=10610
	result = result && this->SetOptionSelection(_T("AdditionalMapType_FixLabelInstr"), 1); // default=no
	result = result && this->SetOptionSelectionWithInt4(_T("AdditionalMapType_FixLabelInstr_ChangeLabel"), 0, 9286); //default value=10610
	result = result && this->SetOptionSelection(_T("SE_AdditionalMapType"), 1); // default=no
	result = result && this->SetOptionSelectionWithInt4(_T("SE_AdditionalMapTypeChangeLabel"), 0, 9286); //default value=10610
	result = result && this->SetOptionSelection(_T("AdditionalResourcesChoice"), 1); // default=no
	result = result && this->SetOptionSelectionWithInt4(_T("PathFindingDefaultValue"), 0, 7620); // default value=2500
	result = result && this->SetOptionSelectionWithFloat(_T("DeathMatchInitialFoodWood"), 0, 20000); // default value=20000
	result = result && this->SetOptionSelectionWithFloat(_T("DeathMatchInitialGold"), 0, 10000); // default value=10000
	result = result && this->SetOptionSelectionWithFloat(_T("DeathMatchInitialStone"), 0, 4500); // default value=5000
	result = result && this->SetOptionSelectionWithFloat(_T("InitialResources_High_FoodWood"), 0, 1200); // default value=1000
	result = result && this->SetOptionSelectionWithFloat(_T("InitialResources_High_Stone"), 0, 500); // default value=750
	result = result && this->SetOptionSelection(_T("InitialResources_High_Gold_as_Stone"), 1); // 0=default=no
	result = result && this->SetOptionSelectionWithFloat(_T("InitialResources_Medium_FoodWood"), 0, 500); // default value=500
	result = result && this->SetOptionSelectionWithFloat(_T("InitialResources_Medium_Stone"), 0, 250); // default value=250
	result = result && this->SetOptionSelection(_T("InitialResources_Medium_Gold_as_Stone"), 0); // 0=default=no
	result = result && this->SetOptionSelectionWithFloat(_T("InitialResources_Low_FoodWood"), 0, 200); // default value=200
	result = result && this->SetOptionSelectionWithFloat(_T("InitialResources_Low_Stone"), 0, 50); // default value=100
	result = result && this->SetOptionSelection(_T("InitialResources_Low_Gold_as_Stone"), 0); // 0=default=no
	result = result && this->SetOptionSelectionWithFloat(_T("ShangInitialFoodDecrease"), 0, 40); // default value=40
	return result;
}


// Set "customROR-recommended" changes ON (does NOT include changes from OPTIONS category. See also SetOtherCustomROROptions.
// Includes ROR_API (mandatory + optionals), MaxSelectedUnits, WindowedMode
// Returns true if everything is OK
bool EmpiresX_API::SetBasicCustomROROptions() {
	bool result = true;
	result = this->SetROR_API(true);
	result = result && this->SetAllROR_API_optionals(true);
	result = result && this->SetMaxSelectedUnits(100);
	result = result && this->SetWindowedMode(true);
	//result = result && this->SetResolution(1920, 1200, true);
	//result = result && this->SetManageAI(true);
	return result;
}


// Set recommended "options" changes for customROR
// Includes some additional options like additional map/resource/starting age choices
// Returns true if everything is OK
bool EmpiresX_API::SetOtherCustomROROptions() {
	bool result = true;
	result = result && this->SetOptionSelection(_T("disable_dislike_human_player"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("disable_dislike_human_player"), FM_ON));
	result = result && this->SetOptionSelection(_T("gaiaX_cheat_code1"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("gaiaX_cheat_code1"), FM_ON));
	result = result && this->SetOptionSelection(_T("gaiaX_cheat_code2"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("gaiaX_cheat_code2"), FM_ON));
	result = result && this->SetOptionSelection(_T("gaiaX_cheat_code3"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("gaiaX_cheat_code3"), FM_ON));
	//result = result && this->SetOptionSelection(_T("FULL_Computer_player_1"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("FULL_Computer_player_1"), FM_ON));
	result = result && this->SetOptionSelection(_T("PostIronAge_Item"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("PostIronAge_Item"), FM_ON));
	result = result && this->SetOptionSelection(_T("PostIronAge_Setup"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("PostIronAge_Setup"), FM_ON));
	result = result && this->SetOptionSelection(_T("AdditionalMapType"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("AdditionalMapType"), FM_ON)); // default=no
	result = result && this->SetOptionSelectionWithInt4(_T("AdditionalMapTypeChangeLabel"), 0, 9286); // default value=10610
	result = result && this->SetOptionSelection(_T("AdditionalMapType_FixLabelS"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("AdditionalMapType_FixLabelS"), FM_ON)); // default=no
	result = result && this->SetOptionSelectionWithInt4(_T("AdditionalMapType_FixLabelS_ChangeLabel"), 0, 9286); // default value=10610
	result = result && this->SetOptionSelection(_T("AdditionalMapType_FixLabelInstr"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("AdditionalMapType_FixLabelInstr"), FM_ON)); // default=no
	result = result && this->SetOptionSelectionWithInt4(_T("AdditionalMapType_FixLabelInstr_ChangeLabel"), 0, 9286); //default value=10610
	result = result && this->SetOptionSelection(_T("SE_AdditionalMapType"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("SE_AdditionalMapType"), FM_ON)); // default=no
	result = result && this->SetOptionSelectionWithInt4(_T("SE_AdditionalMapTypeChangeLabel"), 0, 9286); //default value=10610
	result = result && this->SetOptionSelection(_T("AdditionalResourcesChoice"), this->GetSeqIndexFromFuncMeaning(BC_OPTIONS, _T("AdditionalResourcesChoice"), FM_ON)); // default=no
	return true;
}


// Useful for debug: prints all binary change names
void EmpiresX_API::DumpAllNames(std::wstring separator) {
	wprintf(_T("Tech fixes:\n"));
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_TECH_FIXES, separator).c_str());
	wprintf(_T("\nOptions:\n"));
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_OPTIONS, separator).c_str());
	wprintf(_T("\nResolution:\n"));
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_RESOLUTION, separator).c_str());
	wprintf(_T("\nSelected units:\n"));
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_SELECTED_UNITS, separator).c_str());
	wprintf(_T("\nWindowed mode:\n"));
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_WINDOWED_MODE, separator).c_str());
	wprintf(_T("\nROR API:\n"));
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_ROR_API, separator).c_str());
	wprintf(_T("\nManage AI:\n"));
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_MANAGE_AI, separator).c_str());
	wprintf(_T("\nVEG Windowed mode (obsolete):\n"));
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_VEG_WINDOWED_MODE, separator).c_str());
	wprintf(_T("\nObsolete sequences:\n"));
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_OBSOLETES, separator).c_str());
	wprintf(_T("\n"));
}
// Useful for debug: prints all binary change info
void EmpiresX_API::DumpAllInfo(std::wstring separator, bool withObsoletes) {
	wprintf(_T("* Tech fixes:\n"));
	wprintf(this->aoeManager.DumpSequenceDefInfo(BC_TECH_FIXES, separator, false, false, withObsoletes).c_str());
	wprintf(_T("\n* Options:\n"));
	wprintf(this->aoeManager.DumpSequenceDefInfo(BC_OPTIONS, separator, false, false, withObsoletes).c_str());
	wprintf(_T("\n* Resolution:\n"));
	wprintf(this->aoeManager.DumpSequenceDefInfo(BC_RESOLUTION, separator, false, false, withObsoletes).c_str());
	wprintf(_T("\n* Selected units:\n"));
	wprintf(this->aoeManager.DumpSequenceDefInfo(BC_SELECTED_UNITS, separator, false, false, withObsoletes).c_str());
	wprintf(_T("\n* Windowed mode:\n"));
	wprintf(this->aoeManager.DumpSequenceDefInfo(BC_WINDOWED_MODE, separator, false, false, withObsoletes).c_str());
	wprintf(_T("\n* ROR API:\n"));
	wprintf(this->aoeManager.DumpSequenceDefInfo(BC_ROR_API, separator, false, false, withObsoletes).c_str());
	wprintf(_T("\n* Manage AI:\n"));
	wprintf(this->aoeManager.DumpSequenceDefInfo(BC_MANAGE_AI, separator, false, false, withObsoletes).c_str());
	wprintf(_T("\n* VEG Windowed mode (obsolete):\n"));
	wprintf(this->aoeManager.DumpSequenceDefInfo(BC_VEG_WINDOWED_MODE, separator, false, false, withObsoletes).c_str());
	wprintf(_T("\n* Obsolete sequences:\n"));
	wprintf(this->aoeManager.DumpSequenceDefInfo(BC_OBSOLETES, separator, false, false, withObsoletes).c_str());
	wprintf(_T("\n"));
}
// Useful for debug: prints all pending (modified) binary change names
void EmpiresX_API::DumpPendings(std::wstring separator) {
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_TECH_FIXES, separator, true).c_str());
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_OPTIONS, separator, true).c_str());
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_RESOLUTION, separator, true).c_str());
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_SELECTED_UNITS, separator, true).c_str());
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_WINDOWED_MODE, separator, true).c_str());
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_ROR_API, separator, true).c_str());
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_MANAGE_AI, separator, true).c_str());
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_VEG_WINDOWED_MODE, separator, true).c_str());
	wprintf(this->aoeManager.DumpSequenceDefNames(BC_OBSOLETES, separator, true).c_str());
	wprintf(_T("\n"));
}
std::wstring EmpiresX_API::DumpPendingsToWString(std::wstring separator) {
	std::wstring dest = _T("");
	dest += this->aoeManager.DumpSequenceDefNames(BC_TECH_FIXES, separator, true);
	dest += this->aoeManager.DumpSequenceDefNames(BC_OPTIONS, separator, true);
	dest += this->aoeManager.DumpSequenceDefNames(BC_RESOLUTION, separator, true);
	dest += this->aoeManager.DumpSequenceDefNames(BC_SELECTED_UNITS, separator, true);
	dest += this->aoeManager.DumpSequenceDefNames(BC_WINDOWED_MODE, separator, true);
	dest += this->aoeManager.DumpSequenceDefNames(BC_ROR_API, separator, true);
	dest += this->aoeManager.DumpSequenceDefNames(BC_MANAGE_AI, separator, true);
	dest += this->aoeManager.DumpSequenceDefNames(BC_VEG_WINDOWED_MODE, separator, true);
	dest += this->aoeManager.DumpSequenceDefNames(BC_OBSOLETES, separator, true);
	return dest;
}
// Useful for debug: prints all binary change names
void EmpiresX_API::DumpNotDefault(std::wstring separator) {
	std::wstring str;
	str = this->aoeManager.DumpSequenceDefInfo(BC_TECH_FIXES, separator, false, true);
	if (!str.empty()) wprintf(_T("* Tech fixes:\n%s\n"), str.c_str());
	str = this->aoeManager.DumpSequenceDefInfo(BC_OPTIONS, separator, false, true);
	if (!str.empty()) wprintf(_T("* Options:\n%s\n"), str.c_str());
	str = this->aoeManager.DumpSequenceDefInfo(BC_RESOLUTION, separator, false, true);
	if (!str.empty()) wprintf(_T("* Resolution:\n%s\n"), str.c_str());
	str = this->aoeManager.DumpSequenceDefInfo(BC_SELECTED_UNITS, separator, false, true);
	if (!str.empty()) wprintf(_T("* Selected units:\n%s\n"), str.c_str());
	str = this->aoeManager.DumpSequenceDefInfo(BC_WINDOWED_MODE, separator, false, true);
	if (!str.empty()) wprintf(_T("* Windowed mode:\n%s\n"), str.c_str());
	str = this->aoeManager.DumpSequenceDefInfo(BC_ROR_API, separator, false, true);
	if (!str.empty()) wprintf(_T("* ROR API:\n%s\n"), str.c_str());
	str = this->aoeManager.DumpSequenceDefInfo(BC_MANAGE_AI, separator, false, true);
	if (!str.empty()) wprintf(_T("* Manage AI:\n%s\n"), str.c_str());
	str = this->aoeManager.DumpSequenceDefInfo(BC_VEG_WINDOWED_MODE, separator, false, true);
	if (!str.empty()) wprintf(_T("* VEG windowed mode:\n%s\n"), str.c_str());
	str = this->aoeManager.DumpSequenceDefInfo(BC_OBSOLETES, separator, false, true);
	if (!str.empty()) wprintf(_T("* Obsolete:\n%s\n"), str.c_str());
}
