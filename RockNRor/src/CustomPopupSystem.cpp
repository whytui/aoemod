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



}
