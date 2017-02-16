#include "CustomizeAOE_base.h"


CustomizeAOE_base::CustomizeAOE_base()
{
	this->interactive = true;
}


CustomizeAOE_base::~CustomizeAOE_base()
{
}


// Returns true if successful
bool CustomizeAOE_base::OpenEmpiresX(bool autoFixFile, bool noPromptBeforeFix) {
	if (!e_api.OpenEmpiresXFile()) {
		return false;
	}
	std::wstring s = _T("File version=");
	s.append(e_api.GetFileVersion());
	s.append(_T("\n"));
	this->DisplayMessage(s);
	// In batch mode, do not apply fixes (there is no way to ask the user for a choice)
	if (!autoFixFile || !this->interactive) { return true; }

	return CheckAndRunAutoFixes(noPromptBeforeFix);
}


// Automatically install technical fixes and remove obsolete changes.
// Returns true if successful
bool CustomizeAOE_base::CheckAndRunAutoFixes(bool noPromptBeforeFix) {
	e_api.CancelPendingChanges();
	int techFixesStatus = e_api.HasMissingTechFixes();
	bool hasObsolete = e_api.HasObsoleteSequencesInstalled();
	if (techFixesStatus < 0) {
		// Tech fixes status is invalid. We try a workaround here: remove obsoletes first, in case it fixes the issue
		// This can happen when user is migrating from an old version of RockNRor/RockNRorAdmin.
		this->DisplayMessage(_T("Some technical fixes have an invalid status...\n"));
		if (hasObsolete) {
			if (this->AskForConfirmation(_T("\nRockNRorAdmin can try to fix incompatibility by removing obsolete changes. Let RockNRorAdmin fix obsolete changes (Y/N) ? "))) {
				e_api.RemoveObsoletes();
				if (e_api.WriteChangesToFile() < 0) {
					this->DisplayMessage(_T("An error occurred. Maybe the file is not writable.\n"));
					e_api.OpenEmpiresXFile();
					return false;
				} else {
					this->DisplayMessage(_T("Removed obsolete changes from file.\n"));
				}

				// Update information
				techFixesStatus = e_api.HasMissingTechFixes();
				hasObsolete = e_api.HasObsoleteSequencesInstalled();
			} else {
				this->DisplayMessage(_T("Aborting due to inconsistent status. Try again and answer yes to let RockNRorAdmin try to fix EXE file...\n"));
				return false;
			}
		} else {
			// We just have an invalid status...
			if (!this->AskForConfirmation(_T("\nEXE file is invalid or corrupted. Click yes to force opening file (not recommended)"))) {
				return false;
			}
		}
	}
	std::wstring s;
	s = _T("Tech fixes are all installed: ");
	s += (techFixesStatus == 0) ? _T("true") : _T("false");
	s += _T("\n");
	this->DisplayMessage(s);
	
	s = _T("Is some obsolete sequence installed: ");
	s += hasObsolete ? _T("true") : _T("false");
	s += _T("\n");
	this->DisplayMessage(s);

	if (hasObsolete || (techFixesStatus > 0)) {
		if (noPromptBeforeFix || 
			(this->AskForConfirmation(_T("\nNot fixed bugs and obsolete changes may provoke errors and incompatibility issues.\n")
			_T("Do you want to fix the bugs and remove obsolete changes (Y/N) ? ")))) {
			if (techFixesStatus > 0) {
				e_api.SetAllTechFixes();
			}
			if (hasObsolete) {
				e_api.RemoveObsoletes();
			}
			if (e_api.HasPendingChanges()) {
				if (e_api.WriteChangesToFile() < 0) {
					this->DisplayMessage(_T("An error occurred. Maybe the file is not writable.\n"));
					e_api.OpenEmpiresXFile();
					return false;
				} else {
					this->DisplayMessage(_T("Fixes have been written to file.\n"));
				}
			}
		}
	}

	return true;
}

