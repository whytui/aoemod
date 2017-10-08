#include "../include/AOEPrimitives_UI_screens.h"

namespace AOE_METHODS {
namespace UI_BASE {
;

using namespace AOE_STRUCTURES;

// Create a popup from game screen (from Options original model)
// Must be called when game screen is active (no other popup)
STRUCT_UI_EASY_PANEL *CreateGameScreenPopup(STRUCT_UI_EASY_PANEL *parent, long int hSize, long int vSize,
	long int backgroundSlpId) {
	if (!parent) { return NULL; }
	char dlgName[] = "dlg6_3";
	long int arg1;
	long int arg6;
	STRUCT_UI_EASY_PANEL *newObj = 0;
	// Alloc
	_asm {
		PUSH 0x564;
		MOV EAX, 0x0052601D; // alloc
		CALL EAX;
		ADD ESP, 4; // for push 0x564
		MOV newObj, EAX;
	}
	if (newObj == NULL) { return NULL; }

	_asm {
		// Get data
		MOV ESI, parent;
		MOV EAX, DS:[ESI + 0x20];
		MOV arg1, EAX;
		//MOV EAX, DS:[ESI+0x444]
		MOV EAX, backgroundSlpId;
		MOV arg6, EAX;
		// Init
		MOV ECX, newObj;
		MOV EAX, 0x00460730; // DialogObj.genericUIConstructor(ScreenName)
		PUSH 0x00557204; // Config Dialog
		CALL EAX;
		// Create popup + show + focus
		MOV EDX, 0x004607A0; // dialog.setup(renderArea, parentUI, hSize, vSize, dlgName?, resourceFileId, allowShadowArea)
		MOV ECX, newObj;
		PUSH 1; // arg7
		PUSH arg6;
		LEA EAX, dlgName;
		PUSH EAX;
		PUSH vSize;
		PUSH hSize;
		PUSH parent;
		PUSH arg1;
		CALL EDX; //UIObj.createPopupAndFocus(arg1, parentUI, hSize, vSize, dlgName?, arg6, arg7)?
	}
	return newObj;
}



// Return current screen, using UI panel system
STRUCT_UI_EASY_PANEL *GetCurrentScreen() {
	return AOE_STRUCTURES::GetCurrentScreen();
}


// Returns a pointer to a UI object that matches screenName.
// Can return NULL if no matching screen was found
STRUCT_UI_EASY_PANEL *GetScreenFromName(const char *screenName) {
	unsigned long int result = 0;
	_asm {
		MOV EDX, screenName;
		PUSH EDX;
		MOV ECX, 0x5830E8; // A struct directly included in ROR variables section
		MOV EAX, 0x451AE0;
		CALL EAX;
		MOV result, EAX;
	}
	return (AOE_STRUCTURES::STRUCT_UI_EASY_PANEL *)result;
}


// Calls 0x451BE0
void SetCurrentPanel(const char *screenName, unsigned long int makeModal) {
	_asm {
		PUSH makeModal;
		MOV EDX, screenName;
		PUSH EDX;
		MOV ECX, 0x5830E8; // A struct directly included in ROR variables section
		MOV EAX, 0x451BE0;
		CALL EAX;
	}
}

// (cf 0x451DF0)
// ROR code first (not always, sometimes after?) calls SetCurrentScreen on the parent or new screen and then CloseScreenAndDestroy.
bool CloseScreenAndDestroy(const char *screenName) {
	long int res = 0;
	_asm {
		MOV EDX, screenName;
		PUSH EDX; // arg1 = screen name
		MOV ECX, 0x5830E8; // A struct directly included in ROR variables section
		MOV EAX, 0x451DF0;
		CALL EAX; // close screen and destroy (free)
		MOV res, EAX;
	}
	return (res != 0);
}


// Close screen+destroy it, and set main screen = destroyed screen's parent (if any)
// Do not use UIObj after calling this !
void CloseScreenFullTreatment(STRUCT_ANY_UI *UIObj) {
	AOE_STRUCTURES::STRUCT_ANY_UI *parent = UIObj->ptrParentObject;
	CloseScreenAndDestroy(UIObj->screenName);
	if (parent) {
		SetCurrentPanel(parent->screenName, 0);
	}
}


// Open a new modal screen (popup or fullscreen)
// parent can be null: if so current screen is used
// pos/size arguments can be 0 if isFullscreen=true, otherwise sizes must be >0.
// backgroundTheme can be -1
// allowShadowArea : unknown effect. unused ?
// ScreenType: Dialog seems to only work well in game screen or scenario editor ?
STRUCT_UI_EASY_PANEL *CreateModalScreen(const char *screenName, STRUCT_UI_EASY_PANEL *parent, bool isFullscreen, long int hPos, long int vPos, long int hSize, long int vSize,
	bool allowShadowArea, AOE_CONST_DRS::AoeScreenTheme backgroundTheme, ScreenType screenType) {
#ifndef GAMEVERSION_ROR10c
	return NULL; // Not supported in other versions
#endif;
	long int argFullScreen = isFullscreen ? 1 : 0;
	long int argAllowShadowArea = allowShadowArea ? 1 : 0;
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return NULL; }
	STRUCT_UI_EASY_PANEL *parentScreenToUse = parent;
	if (!parent) {
		parentScreenToUse = GetCurrentScreen();
	}
	if (!parentScreenToUse) { return NULL; }
	assert(parentScreenToUse->IsCheckSumValidForAChildClass());
	if (!parentScreenToUse->IsCheckSumValidForAChildClass()) { return NULL; }
	// We refuse to open simultaneously two screens with the same name
	if (GetScreenFromName(screenName) != NULL) {
		return NULL; // or return existing one ? Warning: may not be modal, etc
	}

	// Check sizes (may cause a game crash)
	if (!isFullscreen && ((hSize <= 0) || (vSize <= 0))) {
		ROCKNROR::SYSTEM::ShowMessageModal(_T("Error"), _T("Bad size arguments in CreateModalScreen"));
		return NULL;
	}

	// Dialog: check no other dialog is open ?

	const unsigned long int ADDR_easyPanelCcor = 0x454430;
	const unsigned long int ADDR_dialogPanelCcor = 0x460730;
	const unsigned long int ADDR_screenPanelCcor = 0x468150;
	const unsigned long int ADDR_easyPanelSetup = 0x4544A0;
	const unsigned long int ADDR_dialogPanelSetup = 0x4607A0;
	const unsigned long int ADDR_screenPanelSetup = 0x468180;
	unsigned long int ccorCallAddress = ADDR_easyPanelCcor;
	unsigned long int setupCallAddress = ADDR_easyPanelSetup;
	// Create screen object
	STRUCT_UI_EASY_PANEL *newScreen = NULL;
	if (screenType == ScreenType::EASY_PANEL) {
		newScreen = (STRUCT_UI_EASY_PANEL *)AOEAlloc(sizeof(STRUCT_UI_EASY_PANEL));
	}
	if (screenType == ScreenType::DIALOG_PANEL) {
		newScreen = (STRUCT_UI_DIALOG_BASE *)AOEAlloc(sizeof(STRUCT_UI_DIALOG_BASE));
		ccorCallAddress = ADDR_dialogPanelCcor;
		setupCallAddress = ADDR_dialogPanelSetup;
	}
	if (screenType == ScreenType::SCREEN_PANEL) {
		newScreen = (STRUCT_UI_SCREEN_PANEL *)AOEAlloc(sizeof(STRUCT_UI_SCREEN_PANEL));
		ccorCallAddress = ADDR_screenPanelCcor;
		setupCallAddress = ADDR_screenPanelSetup;
	}
	if (!newScreen) { return NULL; }
	// Call constructor (same arguments for each class)
	_asm {
		PUSH screenName; // arg1 = new screen name
		MOV ECX, newScreen;
		CALL ccorCallAddress;
	}
	assert(newScreen->IsCheckSumValidForAChildClass());
	// Setup screen object
	STRUCT_UI_DRAW_AREA * renderArea = settings->renderArea;
	if (screenType == ScreenType::EASY_PANEL) {
		_asm {
			MOV ECX, newScreen;
			PUSH argAllowShadowArea;
			PUSH vSize;
			PUSH hSize;
			PUSH vPos;
			PUSH hPos;
			PUSH argFullScreen;
			PUSH backgroundTheme; // slpid background
			PUSH screenName;
			PUSH parentScreenToUse; // parent
			PUSH renderArea;
			call setupCallAddress;
		}
	}
	if (screenType == ScreenType::DIALOG_PANEL) {
		_asm {
			MOV ECX, newScreen;
			PUSH argAllowShadowArea;
			PUSH backgroundTheme; // slpid background
			PUSH screenName;
			PUSH vSize;
			PUSH hSize;
			PUSH parentScreenToUse; // parent
			PUSH renderArea;
			call setupCallAddress;
			// For dialogs, the setup call also "shows" the dialog
		}
	}
	if (screenType == ScreenType::SCREEN_PANEL) {
		_asm {
			MOV ECX, newScreen;
			PUSH argAllowShadowArea;
			PUSH backgroundTheme; // slpid background
			PUSH 0; // info file name TODO
			PUSH renderArea;
			call setupCallAddress;
			// For dialogs, the setup call also "shows" the dialog
		}
	}
	// Important: set new panel as current screen
	if ((screenType == ScreenType::EASY_PANEL) || (screenType == ScreenType::SCREEN_PANEL)) {
		AOE_METHODS::UI_BASE::SetCurrentPanel(screenName, 1);
	}

	return newScreen;
}



}
}
