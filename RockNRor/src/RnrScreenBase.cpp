#include "../include/RnrScreenBase.h"

namespace ROCKNROR {
namespace UI {

RnrScreenBase::RnrScreenBase(const char *screenName) {
	this->screenName = std::string(screenName);
	this->rorScreenStatus = NOT_CREATED;
	this->rorScreenType = AOE_METHODS::UI_BASE::EASY_PANEL;
	this->backgroundSlpTheme = AOE_CONST_DRS::AoeScreenTheme::None_Invalid;
	this->isFullscreen = true;
	this->screenPosX = 0;
	this->screenPosY = 0;
	this->screenSizeX = 0;
	this->screenSizeY = 0;
	ROCKNROR::crInfo.rnrUIHelper->NotifyNewRnrScreen(this);
}


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


// Actually create and show the screen in ROR UI.
// parentScreen can be NULL.
bool RnrScreenBase::CreateScreen(STRUCT_UI_EASY_PANEL *parentScreen) {
	// Note: example of screen close+open other in 0x486dbc
	assert(this->rorScreenStatus == NOT_CREATED);
	if (this->rorScreenStatus != NOT_CREATED) {
		return false;
	}
	STRUCT_UI_EASY_PANEL *screenObject = AOE_METHODS::UI_BASE::CreateModalScreen(this->screenName.c_str(), parentScreen,
		this->isFullscreen, this->screenPosX, this->screenPosY, this->screenSizeX, this->screenSizeY, true, this->backgroundSlpTheme, this->rorScreenType);
	if (!screenObject) {
		return false;
	}
	this->rorScreenStatus = CREATED;
	return true;
}


// Close (and frees) screen in ROR UI.
// Do not close if there are underlying screens !
// You can call this from "this" class.
bool RnrScreenBase::CloseScreen(bool ignoreErrors) {
	if (!ignoreErrors) { assert(this->rorScreenStatus == CREATED); }
	if (this->rorScreenStatus != CREATED) {
		return false;
	}
	this->FreeComponents(); // Crucial to free components BEFORE closing the screen
	bool result = false;
	STRUCT_UI_EASY_PANEL *screen = this->GetAoeScreenObject();
	if (screen && screen->IsCheckSumValidForAChildClass()) {
		STRUCT_ANY_UI *previous = screen->previousPanel;
		if (!previous) { previous = screen->ptrParentObject; }
		result = AOE_METHODS::UI_BASE::CloseScreenAndDestroy(this->screenName.c_str());
		this->rorScreenStatus = CLOSED;
		STRUCT_UI_EASY_PANEL *currentUI = GetCurrentScreen();
		// Make sure a screen is set. If the one we closed was set (focused), then set its parent.
		if ((currentUI == NULL) && (previous != NULL)) {
			AOE_METHODS::UI_BASE::SetCurrentPanel(previous->screenName, 1);
		}
	} else {
		if (!ignoreErrors) {
			SYSTEM::StopExecution(_T("Could not close ROR screen panel: not found or invalid"), true, true);
		}
		result = false;
	}
	return result;
}


// Close "this" screen in ROR UI, and opens "otherScreen" instead.
// "this" must be CREATED and otherScreen must be NOT_CREATED.
bool RnrScreenBase::OpenOtherScreenAndCloseThisOne(RnrScreenBase *otherScreen, STRUCT_UI_EASY_PANEL *parentForNewScreen) {
	if (this->GetScreenStatus() != RnrScreenBase::CREATED) {
		return false;
	}
	if (otherScreen->GetScreenStatus() != RnrScreenBase::NOT_CREATED) {
		return false;
	}

	STRUCT_UI_EASY_PANEL *myScreenObject = this->GetAoeScreenObject();
	if (!myScreenObject) { return false; }

	if (otherScreen->CreateScreen(parentForNewScreen)) {
		STRUCT_UI_EASY_PANEL *newScreenObject = otherScreen->GetAoeScreenObject();
		if (!newScreenObject) { return false; }
		// I don't understand how it works in original game. There is no such affectation ?
		// What is sure is we must make sure previousPanel is a valid pointer... to a valid "previous" UI.
		newScreenObject->previousPanel = myScreenObject->previousPanel;
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
bool RnrScreenBase::OnButtonClick(STRUCT_ANY_UI *button) {
	return false;
}




// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool RnrScreenBaseTest::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	if (keyCode == VK_F3) {
		ROCKNROR::UI::RnrScreenBase *testScreen2 = ROCKNROR::crInfo.rnrUIHelper->FindRnrScreen("testscreen2");
		if (!testScreen2) {
			testScreen2 = new ROCKNROR::UI::RnrScreenBaseTest("testscreen2");
			testScreen2->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::BabylonNeutralScreensGreyBricks);
			testScreen2->SetWindowed(50, 100, 400, 300);
			return this->OpenOtherScreenAndCloseThisOne(testScreen2, NULL);
		}
	}
	return false;
}



}
}
