#include "../include/RockNRorUIHelper.h"


namespace ROCKNROR {
namespace UI {
;


RnrUIHelper::RnrUIHelper() {
	this->internalDeletionInProgress = false;
}

RnrUIHelper::~RnrUIHelper() {
	this->DestroyAllScreens();
	assert(this->rnrScreens.empty());
}


// Find a RockNRor screen from its name. NULL if not found.
// Result may not be currently-focused UI, or may even not be "opened" in ROR process
RnrScreenBase *RnrUIHelper::FindRnrScreen(const std::string &screenName) const {
	for (auto it = this->rnrScreens.cbegin(); it != this->rnrScreens.cend(); it++) {
		ROCKNROR::UI::RnrScreenBase *curScreen = *it;
		if (curScreen->GetScreenName() == screenName) {
			return curScreen;
		}
	}
	return NULL;
}


// Returns currently displayed RockNRor screen, if any
RnrScreenBase *RnrUIHelper::GetCurrentRnrScreen() const {
	STRUCT_UI_PANEL_SYSTEM *uiMainInfo = GetUIMainInfoStruct();
	if (!uiMainInfo) { return NULL; }
	// Priority: modal screen
	STRUCT_UI_EASY_PANEL *currentModal = uiMainInfo->modalPanel;
	if (currentModal && currentModal->IsCheckSumValidForAChildClass()) {
		return this->FindRnrScreen(currentModal->screenName);
	}
	// Then, screens
	if (uiMainInfo->currentScreen && uiMainInfo->currentScreen->IsCheckSumValidForAChildClass()) {
		return this->FindRnrScreen(uiMainInfo->currentScreen->screenName);
	}
	return NULL;
}


// Closes and destroy a specific screen, if found
bool RnrUIHelper::DestroyOneScreen(RnrScreenBase *screenToDestroy) {
	if (!screenToDestroy) { return false; }
	delete screenToDestroy; // This will automatically remove the screen from rnrScreens
	return true;
}


// Closes and destroys all remaining RockNRor screens
void RnrUIHelper::DestroyAllScreens() {
	this->internalDeletionInProgress = true; // set rnrScreens to "read only" mode
	for (auto it = this->rnrScreens.cbegin(); it != this->rnrScreens.cend(); it++) {
		ROCKNROR::UI::RnrScreenBase *curScreen = *it;
		delete curScreen; // Destructor will automatically handle underlying treatments
	}
	this->rnrScreens.clear();
	this->internalDeletionInProgress = false;
}


// Call this to indicate that a new RnrScreen has been created with operator "new"
// This safely updates internal collection
void RnrUIHelper::NotifyNewRnrScreen(RnrScreenBase *newScreen) {
	auto it = std::find(this->rnrScreens.cbegin(), this->rnrScreens.cend(), newScreen);
	if (it != this->rnrScreens.cend()) {
		return; // Already known
	}

	// Check for duplicate ?
	this->rnrScreens.push_back(newScreen);
}


// Call this to indicate that instruction "delete screenToDestroy" is gonna be executed
// This safely updates internal collection
// Do not call this if you don't actually delete the object !
void RnrUIHelper::NotifyDeleteRnrScreen(RnrScreenBase *screenToDestroy) {
	if (!this->internalDeletionInProgress) {
		// This is called from RnrScreenBase destructor, and not driven by this class: we can (need to) remove the entry from the list
		auto it = std::remove(this->rnrScreens.begin(), this->rnrScreens.end(), screenToDestroy);
		this->rnrScreens.erase(it, this->rnrScreens.end());
	}
	// else: the collection is currently in "read only" mode, do not modify it
}


}
}
