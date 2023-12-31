#include "../include/RnrScreenBase.h"

namespace ROCKNROR {
namespace UI {
;

// Constructor for a custom screen, that will be automatically added to custom UI helper.
RnrScreenBase::RnrScreenBase(const char *screenName) {
	this->ResetPointers();
	this->screenName = std::string(screenName);
	this->rorScreenStatus = NOT_CREATED;
	this->rorScreenType = AOE_METHODS::UI_BASE::EASY_PANEL;
	this->backgroundSlpTheme = AOE_CONST_DRS::AoeScreenTheme::None_Invalid;
	this->isFullscreen = true;
	this->screenPosX = 0;
	this->screenPosY = 0;
	this->screenSizeX = 0;
	this->screenSizeY = 0;
	this->afterCloseGamePausePolicy = AfterClosePausePolicy::NONE;
	this->parentScreenFocusedComponent = NULL;
	this->backupKeyboardOwner = NULL;
	this->backupModalPanel = NULL;
	this->backupPreviousFocusedObject = NULL;
	ROCKNROR::crInfo.rnrUIHelper->NotifyNewRnrScreen(this);
}


// Destructor. Being virtual is crucial to allow child classes to be deleted correctly even if calling "delete ptr_as_base_class"
RnrScreenBase::~RnrScreenBase() {
	this->FreeComponents();
	this->CloseScreen(true);
	ROCKNROR::crInfo.rnrUIHelper->NotifyDeleteRnrScreen(this);
}


// Set screen type. Only allowed when screen has not been opened yet.
bool RnrScreenBase::SetScreenType(AOE_METHODS::UI_BASE::ScreenType newType) {
	if (this->rorScreenStatus != NOT_CREATED) { return false; }
	this->rorScreenType = newType;
	return true;
}


// Set background theme. Only allowed when screen has not been opened yet.
bool RnrScreenBase::SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme backgroundSlpTheme) {
	if (this->rorScreenStatus != NOT_CREATED) { return false; }
	this->backgroundSlpTheme = backgroundSlpTheme;
	return true;
}



void RnrScreenBase::FreeComponents() {
	for (auto it = this->componentsToFree.cbegin(); it != this->componentsToFree.cend(); it++) {
		STRUCT_ANY_UI *obj = *it;
		CallAOEDeleteDialog((unsigned long int **)&obj);
	}
	this->componentsToFree.clear();
}


// Only allowed when screen has not been opened yet.
bool RnrScreenBase::SetFullscreen() {
	if (this->rorScreenStatus != NOT_CREATED) { return false; }
	this->isFullscreen = true;
	this->screenPosX = 0;
	this->screenPosY = 0;
	this->screenSizeX = 0;
	this->screenSizeY = 0;
	return true;
}

// Only allowed when screen has not been opened yet.
bool RnrScreenBase::SetWindowed(unsigned long int posX, unsigned long int posY, unsigned long int sizeX, unsigned long int sizeY) {
	if (this->rorScreenStatus != NOT_CREATED) { return false; }
	this->isFullscreen = false;
	this->screenPosX = posX;
	this->screenPosY = posY;
	this->screenSizeX = sizeX;
	this->screenSizeY = sizeY;
	return true;
}

// Only allowed when screen has not been opened yet.
bool RnrScreenBase::SetCenteredForSize(unsigned long int sizeX, unsigned long int sizeY) {
	if (this->rorScreenStatus != NOT_CREATED) { return false; }
	if ((sizeX <= 0) || (sizeY <= 0)) { return false; }
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	assert(settings->renderArea);
	if (!settings->renderArea) { return false; }
	this->isFullscreen = false;
	this->screenSizeX = sizeX;
	this->screenSizeY = sizeY;
	long int halfRemainingX = settings->renderArea->screenSizeX - this->screenSizeX;
	long int halfRemainingY = settings->renderArea->screenSizeY - this->screenSizeY;
	if (halfRemainingX < 0) { halfRemainingX = 0; }
	if (halfRemainingY < 0) { halfRemainingY = 0; }
	halfRemainingX = halfRemainingX / 2;
	halfRemainingY = halfRemainingY / 2;
	this->screenPosX = halfRemainingX;
	this->screenPosY = halfRemainingY;
	return true;
}


// Get X position for a centered object of provided size
unsigned long int RnrScreenBase::GetCenteredPositionX(unsigned long int objSizeX) const {
	if ((objSizeX < 0) || (objSizeX >= this->screenSizeX)) { return 0; }
	long int halfRemainingX = this->screenSizeX - objSizeX;
	halfRemainingX = halfRemainingX / 2;
	return halfRemainingX;
}


// Actually create and show the screen in ROR UI.
// parentScreen can be NULL.
bool RnrScreenBase::CreateScreen(STRUCT_UI_EASY_PANEL *parentScreen) {
	// Note: example of screen close+open other in 0x486dbc
	assert(this->rorScreenStatus == NOT_CREATED);
	if (this->rorScreenStatus != NOT_CREATED) {
		return false;
	}
	// Warning: windows dimensions are weird when trying to open fullscreen when current screen is windowed !
	STRUCT_UI_EASY_PANEL *screenObject = AOE_METHODS::UI_BASE::CreateModalScreen(this->screenName.c_str(), parentScreen,
		this->isFullscreen, this->screenPosX, this->screenPosY, this->screenSizeX, this->screenSizeY, true, this->backgroundSlpTheme, this->rorScreenType);
	if (!screenObject) {
		return false;
	}

	this->parentScreenFocusedComponent = NULL;
	if (parentScreen) {
		// Backup the original focused component in parent screen.
		// If our dropdowns mess with that, we can still restore the correct pointer when closing our screen.
		this->parentScreenFocusedComponent = parentScreen->focusedComponent;
	} else {
		if (screenObject->previousPanel) {
			this->parentScreenFocusedComponent = screenObject->previousPanel->focusedComponent;
		}
	}
	// Collect backup pointers to fix combobox bugs (using dropdowns corrupts some panelSystem pointers, this is an issue in AOE code)
	AOE_STRUCTURES::STRUCT_UI_PANEL_SYSTEM *uiMainInfo = GetUIMainInfoStruct();
	this->backupModalPanel = uiMainInfo->modalPanel;
	this->backupKeyboardOwner = uiMainInfo->keyboardOwner;
	this->backupPreviousFocusedObject = uiMainInfo->previousFocusedObject;

	// Adjust real size/position in internal variables
	this->screenSizeX = screenObject->sizeX;
	this->screenSizeY = screenObject->sizeY;
	this->screenPosX = screenObject->posX;
	this->screenPosY = screenObject->posY;
	this->rorScreenStatus = CREATED;
	this->CreateScreenComponents();
	return true;
}


// Close (and frees) screen in ROR UI.
// Do not close if there are children screens !
// You can call this from "this" class.
bool RnrScreenBase::CloseScreen(bool ignoreErrors) {
	if (!ignoreErrors) { assert(this->rorScreenStatus == CREATED); }
	if (this->rorScreenStatus != CREATED) {
		return false;
	}

	AOE_STRUCTURES::STRUCT_UI_PANEL_SYSTEM *uiMainInfo = GetUIMainInfoStruct();
	bool keyboardOwnerChanged = (uiMainInfo->keyboardOwner != this->backupKeyboardOwner);
	bool modalPanelChanged = (uiMainInfo->modalPanel != this->backupModalPanel);
	bool previousFocusedObjectChanged = (uiMainInfo->previousFocusedObject != this->backupPreviousFocusedObject);
	if (keyboardOwnerChanged) {
		uiMainInfo->keyboardOwner = this->backupKeyboardOwner;
	}
	if (modalPanelChanged) {
		uiMainInfo->modalPanel = this->backupModalPanel;
	}
	if (previousFocusedObjectChanged) {
		uiMainInfo->previousFocusedObject = this->backupPreviousFocusedObject;
	}

	this->FreeComponents(); // Crucial to free components BEFORE closing the screen
	bool result = false;
	STRUCT_UI_EASY_PANEL *screen = this->GetAoeScreenObject();
	if (screen && screen->IsCheckSumValidForAChildClass()) {
		STRUCT_ANY_UI *previous = screen->previousPanel;
		if (!previous) { previous = screen->ptrParentObject; }
		if (previous && (previous->focusedComponent != this->parentScreenFocusedComponent)) {
			// Restore the original (correct) focused component in parent screen.
			// Dropdown buttons generally mess with that pointer because dropdowns do not work well in modal screens.
			previous->focusedComponent = this->parentScreenFocusedComponent;
		}
		result = AOE_METHODS::UI_BASE::CloseScreenAndDestroy(this->screenName.c_str());

		this->rorScreenStatus = CLOSED;
		STRUCT_UI_EASY_PANEL *currentUI = GetCurrentScreen();
		// Make sure a screen is set. If the one we closed was set (focused), then set its parent.
		if ((currentUI == NULL) && (previous != NULL)) {
			if (previous->focusedComponent && !isAValidRORChecksum(previous->focusedComponent->checksum)) {
				// This case happens rarely and seems to be caused by dropdowns (in screens that can be closed by ESC ?)
				// Previous panel's focus is on a no-longer-existing object => game crash
				// We have chances to detect this with invalid checksum, however in unlucky cases the checksum is still there but other object data has been overwritten in memory...
				// This is no more but a workaround that saves us from most crashes
				// NOTE: this should not happen anymore thank to the hack above (save&restore previous focused component)
				previous->focusedComponent = NULL;
			}
			AOE_METHODS::UI_BASE::SetCurrentPanel(previous->screenName, 0);
		}
	} else {
		if (!ignoreErrors) {
			ROCKNROR::SYSTEM::SaveGameAndStopExecution(_T("Could not close ROR screen panel: not found or invalid"), true, true);
		}
		result = false;
	}
	switch (this->afterCloseGamePausePolicy) {
	case AfterClosePausePolicy::SET_PAUSE_ON:
		AOE_METHODS::SetGamePause(true);
		break;
	case AfterClosePausePolicy::SET_PAUSE_OFF:
		AOE_METHODS::SetGamePause(false);
		break;
	}
	return result;
}


// Close "this" screen in ROR UI, and opens "otherScreen" instead.
// "this" must be CREATED and otherScreen must be NOT_CREATED.
bool RnrScreenBase::OpenOtherScreenAndCloseThisOne(RnrScreenBase *otherScreen, bool nullParent) {
	if (this->GetScreenStatus() != RnrScreenBase::CREATED) {
		return false;
	}
	if (otherScreen->GetScreenStatus() != RnrScreenBase::NOT_CREATED) {
		return false;
	}

	AfterClosePausePolicy gamePausePolicyToUse = this->afterCloseGamePausePolicy;
	STRUCT_UI_EASY_PANEL *myScreenObject = this->GetAoeScreenObject();
	STRUCT_UI_EASY_PANEL *parentToUse = NULL;
	if (myScreenObject && !nullParent) {
		parentToUse = (STRUCT_UI_EASY_PANEL *)myScreenObject->ptrParentObject;
	}
	if (!myScreenObject) { return false; }
#ifdef _DEBUG
	if (parentToUse && !parentToUse->IsCheckSumValidForAChildClass()) {
		ROCKNROR::SYSTEM::SaveGameAndStopExecution(_T("Bad parent checksum in OpenOtherScreenAndCloseThisOne"), true, true);
	}
#endif

	if (otherScreen->CreateScreen(parentToUse)) {
		STRUCT_UI_EASY_PANEL *newScreenObject = otherScreen->GetAoeScreenObject();
		if (!newScreenObject) { return false; }
		// I don't understand how it works in original game. There is no such affectation ?
		// What is sure is we must make sure previousPanel is a valid pointer... to a valid "previous" UI.
		newScreenObject->previousPanel = myScreenObject->previousPanel;

		// Transfer the "game pause policy to apply on close" to next popup.
		this->afterCloseGamePausePolicy = AfterClosePausePolicy::NONE;
		otherScreen->afterCloseGamePausePolicy = gamePausePolicyToUse;

		this->CloseScreen(false);
	}
	return true;
}


STRUCT_UI_EASY_PANEL *RnrScreenBase::GetAoeScreenObject() const {
	if (this->rorScreenStatus == NOT_CREATED) {
		return NULL;
	}
	return AOE_METHODS::UI_BASE::GetScreenFromName(this->screenName.c_str());
}


// API to add UI components and add them automatically in "objects to free" list.
bool RnrScreenBase::AddLabel(STRUCT_UI_LABEL **ptrObjToCreate, const char *label,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	AOE_FONTS font) {
	STRUCT_UI_EASY_PANEL *parent = this->GetAoeScreenObject();
	assert(parent);
	assert(ptrObjToCreate);
	if (!parent) { return false; }
	bool res;
	res = AOE_METHODS::UI_BASE::AddLabel(parent, ptrObjToCreate, label, hPos, vPos, hSize, vSize, font);
	if (res && ptrObjToCreate) {
		this->AddComponentToFreeList(*ptrObjToCreate);
	}
	return res;
}

bool RnrScreenBase::AddTextBox(STRUCT_UI_TEXTBOX **ptrObjToCreate, const char *initialText, long int maxTextLength,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	bool readOnly, bool multiline, bool onlyNumbers, unsigned long int font) {
	STRUCT_UI_EASY_PANEL *parent = this->GetAoeScreenObject();
	assert(parent);
	assert(ptrObjToCreate);
	if (!parent) { return false; }
	bool res;
	res = AOE_METHODS::UI_BASE::AddTextBox(parent, ptrObjToCreate, initialText, maxTextLength, hPos, vPos, hSize, vSize,
		readOnly, multiline, onlyNumbers, font);
	if (res && ptrObjToCreate) {
		this->AddComponentToFreeList(*ptrObjToCreate);
	}
	return res;
}

bool RnrScreenBase::AddCheckBox(STRUCT_UI_BUTTON **ptrObjToCreate,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize) {
	STRUCT_UI_EASY_PANEL *parent = this->GetAoeScreenObject();
	assert(parent);
	assert(ptrObjToCreate);
	if (!parent) { return false; }
	bool res;
	res = AOE_METHODS::UI_BASE::AddCheckBox(parent, ptrObjToCreate, hPos, vPos, hSize, vSize);
	if (res && ptrObjToCreate) {
		this->AddComponentToFreeList(*ptrObjToCreate);
	}
	return res;
}

bool RnrScreenBase::AddButton(STRUCT_UI_BUTTON **ptrObjToCreate, const char *caption,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	long int buttonId, AOE_FONTS font) {
	STRUCT_UI_EASY_PANEL *parent = this->GetAoeScreenObject();
	assert(parent);
	assert(ptrObjToCreate);
	if (!parent) { return false; }
	bool res;
	res = AOE_METHODS::UI_BASE::AddButton(parent, ptrObjToCreate, caption, hPos, vPos, hSize, vSize, buttonId, font);
	if (res && ptrObjToCreate) {
		this->AddComponentToFreeList(*ptrObjToCreate);
	}
	return res;
}

bool RnrScreenBase::AddButton(STRUCT_UI_BUTTON **ptrObjToCreate, unsigned long int DLL_STRING_ID,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	long int buttonId, AOE_FONTS font) {
	STRUCT_UI_EASY_PANEL *parent = this->GetAoeScreenObject();
	assert(parent);
	assert(ptrObjToCreate);
	if (!parent) { return false; }
	bool res;
	res = AOE_METHODS::UI_BASE::AddButton(parent, ptrObjToCreate, DLL_STRING_ID, hPos, vPos, hSize, vSize, buttonId, font);
	if (res && ptrObjToCreate) {
		this->AddComponentToFreeList(*ptrObjToCreate);
	}
	return res;
}


bool RnrScreenBase::AddComboBox(STRUCT_UI_COMBOBOX **ptrObjToCreate, long int posX, long int posY,
	long int listSizeX, long int listSizeY, long int lblSizeX, long int lblSizeY, AOE_FONTS font) {
	STRUCT_UI_EASY_PANEL *parent = this->GetAoeScreenObject();
	assert(parent);
	assert(ptrObjToCreate);
	if (!parent) { return false; }
	bool res;
	res = AOE_METHODS::UI_BASE::AddComboBox(parent, ptrObjToCreate, posX, posY, listSizeX, listSizeY, lblSizeX, lblSizeY, font);
	if (res && ptrObjToCreate) {
		this->AddComponentToFreeList(*ptrObjToCreate);
	}
	return res;
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool RnrScreenBase::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	return false;
}

// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool RnrScreenBase::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	return false;
}


// Returns left position for a centered element with desired size.
unsigned long int RnrScreenBase::GetLeftCenteredPositionX(long int desiredSize) const {
	if (desiredSize <= 0) { return 0; }
	long int halfRemainingSize = (this->screenSizeX - desiredSize) / 2;
	if (halfRemainingSize + desiredSize > (long int)this->screenSizeX) {
		halfRemainingSize = this->screenSizeX - desiredSize;
	}
	if (halfRemainingSize < 0) { halfRemainingSize = 0; }
	return halfRemainingSize;
}


// Returns true if behavior on "ALT-F4" event is to close this screen only
// Warning: returning false may cause unstability under some circumstances (because this will have the game shut down while a custom screen is open).
bool RnrScreenBase::CloseOnlyThisScreenOnAltF4() const {
	return !this->isFullscreen;
	//return true; // Might be more prudent
}


}
}
