
#pragma once
#include <assert.h>
#include <string.h>
#include <UI_components\AOE_struct_any_ui.h>
#include <UI\AOE_struct_ui_scenario_editor_main.h>
#include <UI\AOE_struct_ui_in_game_main.h>
#include <AOE_const_internal.h>
#include <AOE_offsets.h>
#include <AOE_struct_game_settings.h>

/*
* This file provides useful (raw) basic methods to deal with AOE UI screens.
* Most of them directly call ROR's methods and are a practical encapsulation to avoid using assembler in our code.
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/


namespace AOE_METHODS {
namespace UI_BASE {
;

// Create a popup from game screen (from Options original model)
// Must be called when game screen is active (no other popup)
static AOE_STRUCTURES::STRUCT_ANY_UI *CreateGameScreenPopup(AOE_STRUCTURES::STRUCT_ANY_UI *parent, long int hSize, long int vSize,
	long int backgroundSlpId) {
	if (!parent) { return NULL; }
	char dlgName[] = "dlg6_3";
	long int arg1;
	long int arg6;
	AOE_STRUCTURES::STRUCT_ANY_UI *newObj = 0;
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



// Return current screen, using 0x5830E8 structure info
static AOE_STRUCTURES::STRUCT_ANY_UI *GetCurrentScreen() {
	AOE_STRUCTURES::STRUCT_ANY_UI *res = NULL;
	_asm {
		MOV ECX, 0x5830E8;
		MOV EAX, DS:[ECX + 0xC];
		MOV res, EAX;
	}
	return res;
}


// Returns a pointer to a UI object that matches screenName.
// Can return NULL if no matching screen was found
static AOE_STRUCTURES::STRUCT_ANY_UI *GetScreenFromName(const char *screenName) {
	unsigned long int result = 0;
	_asm {
		MOV EDX, screenName;
		PUSH EDX;
		MOV ECX, 0x5830E8; // A struct directly included in ROR variables section
		MOV EAX, 0x451AE0;
		CALL EAX;
		MOV result, EAX;
	}
	return (AOE_STRUCTURES::STRUCT_ANY_UI *)result;
}


// Calls 0x451BE0
static void RefreshScreen(const char *screenName, unsigned long int arg2) {
	_asm {
		PUSH arg2;
		MOV EDX, screenName;
		PUSH EDX;
		MOV ECX, 0x5830E8; // A struct directly included in ROR variables section
		MOV EAX, 0x451BE0;
		CALL EAX;
	}
}

// (cf 0x451DF0)
// ROR code first (not always, sometimes after?) calls RefreshScreen on the parent and then CloseScreenAndDestroy.
static void CloseScreenAndDestroy(const char *screenName) {
	_asm {
		MOV EDX, screenName
			PUSH EDX // arg1 = screen name
			MOV ECX, 0x5830E8 // A struct directly included in ROR variables section
			MOV EAX, 0x451DF0
			CALL EAX // close screen and destroy (free)
	}
}


// Refresh parent, close screen and destroy it.
static void CloseScreenFullTreatment(AOE_STRUCTURES::STRUCT_ANY_UI *UIObj) {
	AOE_STRUCTURES::STRUCT_ANY_UI *parent = UIObj->ptrParentObject;
	CloseScreenAndDestroy(UIObj->screenName);
	if (parent) {
		RefreshScreen(parent->screenName, 0);
	}
}

}
}
