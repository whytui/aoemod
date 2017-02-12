#include "../include/CustomPopupSystem.h"

namespace CUSTOMROR {
;
// A common global object
CustomPopupSystem customPopupSystem;


CustomPopupSystem::CustomPopupSystem() {
	this->currentCustomPopup = NULL;
}

CustomPopupSystem::~CustomPopupSystem() {
	if (this->currentCustomPopup) {
		delete this->currentCustomPopup;
		this->currentCustomPopup = NULL;
	}
}


// Ensure exiting game won't crash due to UI pointer issues related to custom popup (occurs in scenario editor)
// Returns true if we want NOT the game to exit
bool CustomPopupSystem::FixGamePopupIssuesBeforeGameClose() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (CUSTOMROR::crInfo.HasOpenedCustomGamePopup() && settings &&
		(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR)) {
		// In scenario editor, UI objects are not correctly freed and the game crashes if we have a custom popup opened...
		// Closing the popup solves the issue !
		CUSTOMROR::customPopupSystem.CloseCustomGamePopup(true);
		assert(!CUSTOMROR::crInfo.HasOpenedCustomGamePopup());
		return true; // Do NOT exit game (if possible). We consider the ALT-F4 closed the popup (that's a choice...)
	}
	return false;
}


// Please always use this API to close a custom game popup. Do not call crInfo's method directly.
// set isCancel to true to indicate we don't want to save/apply changes.
// this->currentCustomPopup is cleared/freed in this method.
void CustomPopupSystem::CloseCustomGamePopup(bool isCancel) {
	if (this->currentCustomPopup) {
		this->currentCustomPopup->ClosePopup(isCancel);
		CustomPopupBase *next = this->currentCustomPopup->nextPopup;
		delete this->currentCustomPopup;
		this->currentCustomPopup = next; // Generally NULL unless closing the popup triggered a new one.

		if ((next == NULL) && traceMessageHandler.HasUnreadMessages()) {
			//this->OpenTraceMessagePopup(); // TODO
		}
	}
}


// Use this to properly free custom button from options menu. This should (always) be called when ingame menu is closed.
void CustomPopupSystem::FreeInGameCustomOptionsButton() {
	if (CUSTOMROR::crInfo.configInfo.showCustomRORMenu) {
		// Add "custom options" button (from game menu) to our "garbage collector" so that it is freed later.
		if (CUSTOMROR::crInfo.customGameMenuOptionsBtnVar) {
			CUSTOMROR::crInfo.AddObjectInPopupContentList(CUSTOMROR::crInfo.customGameMenuOptionsBtnVar);
			CUSTOMROR::crInfo.customGameMenuOptionsBtnVar = NULL;
		}
	}
}


// Create in-game customROR options screen. Returns false if failed and if return address must be changed.
bool CustomPopupSystem::CreateGameCustomRorOptionsPopup(AOE_STRUCTURES::STRUCT_ANY_UI *previousPopup) {
	assert(previousPopup != NULL);
	if (!previousPopup) { return false; }

	assert(!CUSTOMROR::crInfo.HasOpenedCustomGamePopup()); // Custom popup is not already open
	if (CUSTOMROR::crInfo.HasOpenedCustomGamePopup()) { return true; } // do nothing, but not treated as alloc error

	InGameCustomRorOptionsPopup *popupUIObject = this->CreateCustomPopupObject<InGameCustomRorOptionsPopup>();
	if (popupUIObject) {
		popupUIObject->CloseMenuAndOpenPopup(previousPopup);
	}

	if (!popupUIObject) {
		return false; // need to change return address
	}
	// Dirty workaround because custom options popup is not created using standard procedures :(
	CUSTOMROR::crInfo.ForceSetCurrentGamePopup(popupUIObject->GetAOEPopupObject(), popupUIObject->customOptionButtonVar, NULL);

	return true;
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool CustomPopupSystem::OnCustomButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) {
	if (!sender || !sender->IsCheckSumValidForAChildClass()) { return false; }
	bool doCloseCustomPopup = false;
	if (!GetGameGlobalStructPtr()) { return false; }
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = GetGameGlobalStructPtr()->scenarioInformation;

	if (CUSTOMROR::customPopupSystem.currentCustomPopup) {
		bool eventHandled = CUSTOMROR::customPopupSystem.currentCustomPopup->OnButtonClick(sender);
		if (CUSTOMROR::customPopupSystem.currentCustomPopup->IsClosed()) {
			CustomPopupBase *next = CUSTOMROR::customPopupSystem.currentCustomPopup->nextPopup;
			delete CUSTOMROR::customPopupSystem.currentCustomPopup;
			CUSTOMROR::customPopupSystem.currentCustomPopup = next; // Generally NULL unless closing the popup triggered a new one.
		}
		return eventHandled;
	}
	return false;
}


// Triggered on a "onclick" on a UI component at provided address (objAddress)
// This just detects if the component is the popup's OK or Cancel button, and handles popup closing event if so.
// Returns true if the popup has been closed (and the event processed)
bool CustomPopupSystem::OnButtonClickClosePopupIfNecessary(unsigned long int objAddress) {
	if (CUSTOMROR::crInfo.IsCustomGamePopupOKButton(objAddress)) {
		// Close the custom popup when it is open and we just clicked on OK button.
		this->CloseCustomGamePopup(false);
		return true;
	}
	if (CUSTOMROR::crInfo.IsCustomGamePopupCancelButton(objAddress)) {
		// Close the custom popup when it is open and we just clicked on Cancel button.
		this->CloseCustomGamePopup(true);
		return true;
	}
	return false; // Standard case: the event is not related to OK and Cancel buttons.
}




// Opens the custom "edit unit" in editor
// Returns true if OK.
bool CustomPopupSystem::OpenCustomEditorEditUnitPopup() {
	return (this->OpenCustomGamePopup<EditorEditUnitInfoPopup>(500, 400, true) != NULL);
}


// Opens the custom "simple edit text" popup in editor
// outputBuffer is a pointer where the typed text will be copied when the popup is closed. Make sure its allocated size is >=maxLength
// Returns true if OK.
bool CustomPopupSystem::OpenCustomTextEditPopup(const char *title, char *initialValue, long int sizeX, long int sizeY,
	long int maxLength, char *outputBuffer, bool readOnly, bool withCancelBtn) {
	SimpleEditTextPopup *p = CUSTOMROR::customPopupSystem.OpenCustomGamePopup<SimpleEditTextPopup>(sizeX, sizeY, withCancelBtn);
	if (p) {
		p->AddPopupContent(title, initialValue, maxLength, outputBuffer, readOnly);
	}
	return (p != NULL);
}


// Opens the custom "edit unit" in editor
// Returns true if OK.
bool CustomPopupSystem::OpenCustomEditorScenarioInfoPopup() {
	EditorScenarioInfoPopup *popup = this->OpenCustomGamePopup<EditorScenarioInfoPopup>(500, 400, false);
	bool result = (popup != NULL);
	if (popup) {
		popup->SetVarToUpdate_allowUnitOverlapping(&CUSTOMROR::crInfo.configInfo.editor_allowUnitOverlapping);
		popup->SetVarToUpdate_disableHillModeChecks(&CUSTOMROR::crInfo.configInfo.editor_disableHillModeChecks);
		popup->SetVarToUpdate_disableTerrainRestrictionChecks(&CUSTOMROR::crInfo.configInfo.editor_disableTerrainRestrictions);
		popup->SetVarToUpdate_lengthenCombatMode(NULL); // Default
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (global && global->IsCheckSumValid()) {
			AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = global->scenarioInformation;
			if (scInfo && scInfo->IsCheckSumValid()) {
				popup->SetVarToUpdate_lengthenCombatMode(&scInfo->enableLengthenCombatMode);
			}
		}
	}
	return result;
}


// Opens the custom "edit map size" popup in editor
// Returns true if OK.
bool CustomPopupSystem::OpenEditMapSizePopup() {
	return (this->OpenCustomGamePopup<EditMapSizeXYPopup>(320, 130, true) != NULL);
}


// Opens the custom "edit terrain" popup in editor
// Returns true if OK.
bool CustomPopupSystem::OpenCustomTerrainEditPopup() {
	return (this->OpenCustomGamePopup<EditTerrainPopup>(500, 300, true) != NULL);
}


// Open a popup with CustomROR (debug) messages
bool CustomPopupSystem::OpenTraceMessagePopup() {
	bool reverseOrder = CUSTOMROR::crInfo.configInfo.showLogsInReverseOrder;
	// Double spaces are intented (to compensate weird-looking font)
	char *title = "Show  debug  messages";
	if (reverseOrder) {
		title = "Show  debug  messages (reverse order)";
	}
	return CUSTOMROR::customPopupSystem.OpenCustomTextEditPopup(title, (char*)traceMessageHandler.GetAllMessages(reverseOrder).c_str(),
		780, 590,
		0, NULL, false /* NOT read only so that scrollbar is active*/, false);
}


// Open the relevant "view/edit unit" popup for currently selected unit.
// Returns true if successful.
bool CustomPopupSystem::OpenInGameUnitPropertiesPopup() {
	AOE_STRUCTURES::STRUCT_PLAYER *humanPlayer = GetControlledPlayerStruct_Settings();
	if (!humanPlayer || !humanPlayer->IsCheckSumValid()) {
		return false;
	}
	if (humanPlayer->selectedUnitCount <= 0) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = CUSTOMROR::crInfo.GetRelevantSelectedUnitsPointer(humanPlayer);
	assert(selectedUnits != NULL);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *selectedUnit = selectedUnits[0];
	if (!selectedUnit || !selectedUnit->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	if (!this->OpenInGameUnitPropertiesPopup(selectedUnit)) {
		return false;
	}

	if (!IsMultiplayer()) {
		AOE_METHODS::SetGamePause(true);
	}
	return true;
}


// Open the relevant "view/edit unit" popup for provided unit.
// Returns true if successful.
bool CustomPopupSystem::OpenInGameUnitPropertiesPopup(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	InGameUnitPropertiesPopup *popup = CUSTOMROR::customPopupSystem.OpenCustomGamePopup<InGameUnitPropertiesPopup>(600, 500, true);
	if (popup == NULL) { return false; }
	popup->AddPopupContent(unit->unitInstanceId);
	return true;
}



}
