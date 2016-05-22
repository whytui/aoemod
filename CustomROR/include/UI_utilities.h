
#pragma once

#include <AOE_offsets_10c.h>
#include <AOE_const_internal.h>
#include <ROR_structures.h>
#include <ROR_structures_drs.h>
#include <assert.h>
#include "AOE_memory.h"
#include "drsHandler.h"


/*
 * This file provides useful (raw) methods to deal with AOE UI objects.
 * Most of them directly call ROR's methods and are a practical encapsulation to avoid using assembler in our code.
*/

enum AOE_FONTS : long int {
	AOE_FONT_BIG_LABEL = 0,
	AOE_FONT_STANDARD_TEXT = 0x0B
};


static const char scenarioEditorScreenName[] = "Scenario Editor Screen";
static const char menuDialogScreenName[] = "Menu Dialog";
static const char gameScreenName[] = "Game Screen";


// The AOE_Addxxx functions create a UI object and store the new object's pointer in ptrObjToCreate parameter


// Display a small yes/no dialog message (based on game exit popup) + cancel in scenario editor
// Return pointer (address) to new object.
// Compatible with scenario editor screen and in-game screen
// Note that settings->currentUIStatus is unchanged when showing such dialog message.
static ROR_STRUCTURES_10C::STRUCT_ANY_UI *AOE_CreateDialogPopup(const char *text, long int hSize, long int vSize) {
	char *dlgName = (char *)AOE_CONST_INTERNAL::customDialogScreenName;
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = *(ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS **) AOE_OFFSETS_10C::ADDR_VAR_GAME_SETTINGS_STRUCT; // GetGameSettingsPtr();
	if (settings == NULL) { return 0; }
	unsigned long int fct = 0;
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *currentUI = *(ROR_STRUCTURES_10C::STRUCT_ANY_UI **) AOE_OFFSETS_10C::ADDR_VAR_CURRENT_UI_OBJECT;
	if (currentUI == NULL) { return NULL; }
	// Get correct call values according to active screen
	if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) {
		ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *p = (ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *)currentUI;
		assert(p->visible);
		if (!p->IsCheckSumValid()) { return NULL; }
		fct = 0x00457A80;
	}
	if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) {
		if (currentUI->checksum != 0x00547360) { return 0; }
		fct = 0x00457B90;
	}
	if (!fct) { return 0; }
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *obj = NULL;
	// Now call relevant function
	_asm {
		PUSH vSize
		PUSH hSize
		MOV EAX, dlgName
		PUSH EAX
		MOV EAX, text
		PUSH EAX
		MOV ECX, currentUI
		MOV EAX, fct
		CALL EAX // showDialog(ptrText, dlgName, hSize, vSize)
		MOV ECX, currentUI
		MOV EAX, DS:[ECX+0x3C] // Focused object = new popup
		MOV obj, EAX
	}
	return obj;
}


// Create a popup from game screen (from Options original model)
// Must be called when game screen is active (no other popup)
static ROR_STRUCTURES_10C::STRUCT_ANY_UI *AOE_CreateGameScreenPopup(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent, long int hSize, long int vSize) {
	if (!parent) { return NULL; }
	char dlgName[] = "dlg6_3";
	long int arg1;
	long int arg6;
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *newObj = 0;
	// Alloc
	_asm {
		PUSH 0x564
		MOV EAX, 0x0052601D // alloc
		CALL EAX
		ADD ESP, 4 // for push 0x564
		MOV newObj, EAX // test
	}
	if (newObj == NULL) { return NULL; }

	_asm {
		// Get data
		MOV ESI, parent
		MOV EAX, DS:[ESI+0x20]
		MOV arg1, EAX
		MOV EAX, DS:[ESI+0x444]
		MOV arg6, EAX
		// Init
		MOV ECX, newObj
		MOV EAX, 0x00460730 // UIObj.genericUIConstructor(ScreenName) ?
		PUSH 0x00557204 // Config dialog
		CALL EAX
		// Create popup + show + focus
		MOV EDX, 0x004607A0
		MOV ECX, newObj
		PUSH 1 // arg7
		PUSH arg6
		LEA EAX, dlgName
		PUSH EAX
		PUSH vSize
		PUSH hSize
		PUSH parent
		PUSH arg1
		CALL EDX //UIObj.createPopupAndFocus(arg1, parentUI, hSize, vSize, dlgName?, arg6, arg7)?
	}
	return newObj;
}


// Note: basic size (eg OK button) is 0x?? / 0x1E
static bool AOE_AddButton(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON **ptrObjToCreate, unsigned long int DLL_STRING_ID,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	long int buttonId = 0, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT) {
	if (!parent) { return false; }
	unsigned long int result;
	_asm {
		MOV ECX, parent
		PUSH buttonId
		PUSH 0 // arg10?
		PUSH font
		PUSH vSize
		PUSH hSize
		PUSH vPos
		PUSH hPos
		PUSH 0x00 // arg4
		PUSH DLL_STRING_ID
		PUSH ptrObjToCreate
		PUSH ECX
		MOV EAX, 0x00455D10 // addbutton
		CALL EAX
		MOV result, EAX
	}
	return result != 0;
}


// Note: basic size (eg OK button) is 0x?? / 0x1E
static bool AOE_AddButton(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON **ptrObjToCreate, char *caption,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	long int buttonId = 0, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT) {
	if (!parent) { return false; }
	unsigned long int result;
	_asm {
		MOV ECX, parent
		PUSH buttonId
		PUSH 0 // arg10?
		PUSH font
		PUSH vSize
		PUSH hSize
		PUSH vPos
		PUSH hPos
		PUSH 0x00 // arg4 = text2. Unused?
		PUSH caption
		PUSH ptrObjToCreate
		PUSH ECX
		MOV EAX, DS:[ECX]
		CALL DS:[EAX+0xF0] // addbutton (using text)
		MOV result, EAX
	}
	return result != 0;
}



// For fonts, see AOE_FONTS enum
static bool AOE_AddLabel(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL **ptrObjToCreate, char *label,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT
) {
	if (parent == NULL) { return false; }
	long int result;
	_asm {
		MOV ECX, parent
		PUSH 0 // arg11
		PUSH 1 // arg10
		PUSH 0 // arg9
		PUSH font
		PUSH vSize
		PUSH hSize
		PUSH vPos
		PUSH hPos
		PUSH label
		PUSH ptrObjToCreate
		PUSH ECX // Parent
		MOV EDX, DS:[ECX]
		CALL[EDX + 0x104] // or 0x456530
		MOV result, EAX
	}
	return result != 0;
}


// Create a textbox using ROR methods.
// If maxTextLength==0, it is replaced by initialText's length.
// Note: The font used seems to be 14 pixels high (?)
static bool AOE_AddTextBox(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_TEXTBOX **ptrObjToCreate, const char *initialText, long int maxTextLength,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize, 
	bool readOnly = false, bool multiline = false, bool onlyNumbers = false, unsigned long int font = AOE_FONTS::AOE_FONT_STANDARD_TEXT) {
	if (!parent) { return false; }
	//long int maskValue = onlyNumbers ? 3 : 0;
	long int inputType = 0;
	if (multiline) { inputType = 7; }
	if (onlyNumbers) { inputType = 3; }
	if (maxTextLength <= 0) {
		maxTextLength = strlen(initialText);
	}
	if (maxTextLength >= 0x8000 - 2) { maxTextLength = 0x8000 - 2; } // AOE does not support longer textarea length (will not be work correctly)
	long int result;
	_asm {
		MOV ECX, parent
		PUSH 0
		PUSH 0 // arg11
		PUSH font
		PUSH vSize
		PUSH hSize
		PUSH vPos
		PUSH hPos
		PUSH inputType // arg5
		PUSH maxTextLength
		MOV EAX, initialText
		PUSH EAX // initial value
		PUSH ptrObjToCreate
		PUSH ECX // parent
		MOV EAX, 0x00456A30
		CALL EAX
		MOV result, EAX
	}
	if (!readOnly && result) {
		_asm {
			// Make writeable
			MOV EAX, ptrObjToCreate
			MOV EAX, DS:[EAX]
			MOV DS:[EAX+0x074], 1
		}
	}
	return result != 0;
}


// Create a "AOE" checkbox (same type as buttons)
// You need to create a label if you want some text aside the checkbox
static bool AOE_AddCheckBox(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_BUTTON **ptrObjToCreate,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize) {
	if (!parent) { return false; }
	long int result;
	_asm {
		MOV ECX, parent
		PUSH 0 // arg8
		PUSH 0 // arg7
		PUSH vSize
		PUSH hSize
		PUSH vPos
		PUSH hPos
		PUSH ptrObjToCreate
		PUSH ECX // parent
		MOV EAX, 0x00456240 // create checkbox
		CALL EAX
		MOV result, EAX
	}
	return result != 0;
}


static void AOE_CheckBox_SetChecked(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *checkBox, bool checked) {
	if (!checkBox) { return; }
	checkBox->checked = checked ? 1 : 0;
	int arg = checkBox->checked;
	_asm {
		MOV ECX, checkBox
		PUSH arg
		MOV EAX, 0x0045F2F0
		CALL EAX
	}
}


// ptrCombo = address of ROR combobox object
// entryId = id to affect to new entry in combobox (generally=index of the entry)
// DLLID = ID in language(x).dll of string to use for entry.
static void AOE_AddEntryInComboUsingDLLID(ROR_STRUCTURES_10C::STRUCT_ANY_UI *ptrCombo, long int entryId, long int DLLID) {
	assert(ptrCombo != NULL);
	if (!ptrCombo) { return; }
	_asm {
		MOV ECX, ptrCombo
		PUSH entryId
		PUSH DLLID
		MOV EAX, 0x461DD0
		CALL EAX // Add entry in combo
	}
}


// Creates an empty combobox. Use AddEntryInCombo aftewards.
// TO DO: this is unfinished
// Unstable and not correctly supported !
static bool AOE_AddComboBox(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent,
	ROR_STRUCTURES_10C::STRUCT_UI_COMBOBOX **ptrObjToCreate,
	long int posX, long int posY, long int listSizeX, long int listSizeY, long int lblSizeX, long int lblSizeY, AOE_FONTS fontId) {
	if (!parent) { return false; }
	long int result;
	_asm {
		MOV ECX, parent
		PUSH fontId
		PUSH lblSizeY
		PUSH lblSizeX
		PUSH posY
		PUSH posX
		PUSH listSizeY
		PUSH listSizeX
		PUSH ptrObjToCreate
		PUSH ECX // parent
		MOV EAX, 0x00456BF0 // create combobox
		CALL EAX
		MOV result, EAX
	}
	return (result != 0);
}


static void AOE_AddEntryInCombo(ROR_STRUCTURES_10C::STRUCT_ANY_UI *ptrCombo, long int entryId, const char *text) {
	assert(ptrCombo != NULL);
	if (!ptrCombo) { return; }
	_asm {
		MOV ECX, ptrCombo
		MOV ECX, DS:[ECX+0xFC]
		MOVSX EAX, WORD PTR DS:[ECX+0xF8]
		PUSH entryId
		PUSH text
		PUSH EAX
		MOV EAX, 0x46A190
		CALL EAX // Add entry in combo
	}
}


// Returns selected index for a combobox object. Returns -1 in error cases.
static long int AOE_GetComboSelectedIndex(ROR_STRUCTURES_10C::STRUCT_ANY_UI *ptrCombo) {
	assert(ptrCombo != NULL);
	if (!ptrCombo) { return -1; }
	long int result = -1;
	_asm {
		MOV EDX, 0x461E50
		MOV ECX, ptrCombo
		CALL EDX
		MOV result, EAX
	}
	return result;
}

// Returns an edit object's text
static char *AOE_GetEditText(ROR_STRUCTURES_10C::STRUCT_ANY_UI *ptrEdit) {
	assert(ptrEdit != NULL);
	if (!ptrEdit) { return NULL; }
	char *result = NULL;
	_asm {
		MOV EDX, 0x462560
		MOV ECX, ptrEdit
		CALL EDX
		MOV result, EAX
	}
	return result;
}

// Set an edit object's text
static void AOE_SetEditText(ROR_STRUCTURES_10C::STRUCT_ANY_UI *ptrEdit, char *text) {
	assert(ptrEdit != NULL);
	if (!ptrEdit) { return; }
	_asm {
		PUSH text
		MOV ECX, ptrEdit
		MOV EDX, DS:[ECX]
		CALL DS:[EDX+0xEC]
	}
}

static void AOE_listBox_clear(ROR_STRUCTURES_10C::STRUCT_UI_LISTBOX *listBox) {
	if (!listBox) { return; }
	_asm {
		MOV ECX, listBox
		MOV EDX, 0x46A6C0
		CALL EDX
	}
}

static void AOE_comboBox_clear(ROR_STRUCTURES_10C::STRUCT_UI_LISTBOX *comboBox) {
	if (!comboBox) { return; }
	_asm {
		MOV ECX, comboBox
		MOV EDX, 0x461F10
		CALL EDX
	}
}


// Adds an item at position for a listbox / combobox
static bool AOE_listbox_addItem(ROR_STRUCTURES_10C::STRUCT_UI_LISTBOX *obj, long int position, char *text, long int optionalId) {
	if (!obj) { return false; }
	long int res;
	_asm {
		PUSH optionalId
		PUSH text
		PUSH position
		MOV ECX, obj
		MOV EDX, 0x46A190
		CALL EDX
		MOV EAX, res
	}
	return (res != 0);
}


// Return current screen, using 0x5830E8 structure info
static ROR_STRUCTURES_10C::STRUCT_ANY_UI *AOE_GetCurrentScreen() {
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *res = NULL;
	_asm {
		MOV ECX, 0x5830E8
		MOV EAX, DS:[ECX+0xC]
		MOV res, EAX
	}
	return res;
}


// Returns a pointer to a UI object that matches screenName.
// Can return NULL if no matching screen was found
static ROR_STRUCTURES_10C::STRUCT_ANY_UI *AOE_GetScreenFromName(const char *screenName) {
	unsigned long int result = 0;
	_asm {
		MOV EDX, screenName
		PUSH EDX
		MOV ECX, 0x5830E8 // A struct directly included in ROR variables section
		MOV EAX, 0x451AE0
		CALL EAX
		MOV result, EAX
	}
	return (ROR_STRUCTURES_10C::STRUCT_ANY_UI *)result;
}


// Calls 0x451BE0
static void AOE_RefreshScreen(const char *screenName, unsigned long int arg2) {
	_asm {
		PUSH arg2
		MOV EDX, screenName
		PUSH EDX
		MOV ECX, 0x5830E8 // A struct directly included in ROR variables section
		MOV EAX, 0x451BE0
		CALL EAX
	}
}

// (cf 0x451DF0)
// ROR code first (not always, sometimes after?) calls RefreshScreen on the parent and then CloseScreenAndDestroy.
static void AOE_CloseScreenAndDestroy(const char *screenName) {
	_asm {
		MOV EDX, screenName
		PUSH EDX // arg1 = screen name
		MOV ECX, 0x5830E8 // A struct directly included in ROR variables section
		MOV EAX, 0x451DF0
		CALL EAX // close screen and destroy (free)
	}
}


// Refresh parent, close screen and destroy it.
static void AOE_CloseScreenFullTreatment(ROR_STRUCTURES_10C::STRUCT_ANY_UI *UIObj) {
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent = UIObj->ptrParentObject;
	AOE_CloseScreenAndDestroy(UIObj->screenName);
	if (parent) {
		AOE_RefreshScreen(parent->screenName, 0);
	}
}


// Set parent's focus to child object.
// child CAN be NULL (set focus to parent itself).
static void AOE_SetFocus(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent, ROR_STRUCTURES_10C::STRUCT_ANY_UI *child) {
	if (!parent) { return; }
	_asm {
		PUSH child
		MOV ECX, parent
		MOV EAX, 0x453EB0 // parentObj.setFocus(childObj)
		CALL EAX
	}
}


// Show/Hide a UI object
static void AOE_ShowUIObject(ROR_STRUCTURES_10C::STRUCT_ANY_UI *object, bool show) {
	if (!object) { return; }
	long int arg = show ? 1 : 0;
	_asm {
		MOV ECX, object
		MOV EAX, DS:[ECX]
		PUSH arg
		CALL DS:[EAX + 0x14]
	}
}

static void AOE_RefreshUIObject(ROR_STRUCTURES_10C::STRUCT_ANY_UI *object) {
	if (!object) { return; }
	_asm {
		MOV ECX, object
		MOV EAX, DS:[ECX]
		PUSH 1
		CALL DS:[EAX + 0x20]
	}
}


// Create a popup (from Options original model) and returns the new UI object's address as an unsigned long int
// This must be called when menu is opened
static ROR_STRUCTURES_10C::STRUCT_ANY_UI *AOE_CreateCustomOptionsPopupFromMenu(ROR_STRUCTURES_10C::STRUCT_ANY_UI *parent) {
	if (!parent) { return NULL; }

	// Analog to 0x43424D
	AOE_CloseScreenAndDestroy("Menu Dialog");
	AOE_RefreshScreen("Game Screen", 0);
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *newPtr = (ROR_STRUCTURES_10C::STRUCT_ANY_UI *)AOEAlloc(0x564);
	if (!newPtr) { return NULL; }

	_asm {
		// see 0x4300D0
		MOV ECX, newPtr // Now ECX is the new popup object (parent for other new objects)
		MOV ESI, ECX // Now and for all the code below ESI is the popup object (parent for other new objects)
		PUSH 0x00557204
		MOV EAX, 0x00460730 // UIObj.genericUIConstructor(ScreenName) ?
		CALL EAX
		MOV EAX, 0
		LEA EDX,DWORD PTR DS:[ESI+0x498]
		LEA EBX,DWORD PTR DS:[ESI+0x494]
		XOR ECX,ECX
		MOV DWORD PTR DS:[ESI+0x490],EAX
		MOV DWORD PTR DS:[ESI],0x005436E0
		MOV DWORD PTR DS:[EBX],0
		MOV DWORD PTR DS:[EDX],ECX
		LEA EAX,DWORD PTR DS:[ESI+0x4A8]
		PUSH 1
		MOV DWORD PTR DS:[EDX+4],ECX
		MOV DWORD PTR DS:[EDX+8],ECX
		MOV DWORD PTR DS:[EDX+0x0C],ECX
		MOV DWORD PTR DS:[EAX],ECX
		XOR EDX,EDX
		MOV DWORD PTR DS:[EAX+4],ECX
		MOV DWORD PTR DS:[EAX+8],ECX
		LEA EAX,DWORD PTR DS:[ESI+0x4B4]
		MOV DWORD PTR DS:[ESI+0x4B4],EDX
		MOV DWORD PTR DS:[EAX+4],EDX
		MOV DWORD PTR DS:[EAX+8],EDX
		LEA EAX,DWORD PTR DS:[ESI+0x4C4]
		MOV DWORD PTR DS:[ESI+0x4C0],EDX
		MOV DWORD PTR DS:[EAX],ECX
		MOV DWORD PTR DS:[EAX+4],ECX
		MOV DWORD PTR DS:[EAX+8],ECX
		LEA EAX,DWORD PTR DS:[ESI+0x4D4]
		MOV DWORD PTR DS:[ESI+0x4D0],EDX
		MOV DWORD PTR DS:[EAX],EDX
		MOV DWORD PTR DS:[EAX+4],EDX
		MOV DWORD PTR DS:[EAX+8],EDX
		LEA EAX,DWORD PTR DS:[ESI+0x4E4]
		MOV DWORD PTR DS:[ESI+0x4E0],EDX
		MOV DWORD PTR DS:[EAX],ECX
		MOV DWORD PTR DS:[EAX+4],ECX
		MOV DWORD PTR DS:[EAX+8],ECX
		MOV DWORD PTR DS:[EAX+0x0C],ECX
		LEA EAX,DWORD PTR DS:[ESI+0x4F4]
		MOV DWORD PTR DS:[ESI+0x4F4],EDX
		MOV DWORD PTR DS:[EAX+4],EDX
		MOV DWORD PTR DS:[EAX+8],EDX
		LEA EAX,DWORD PTR DS:[ESI+0x500]
		MOV DWORD PTR DS:[ESI+0x500],ECX
		MOV DWORD PTR DS:[EAX+4],ECX
		MOV DWORD PTR DS:[EAX+8],ECX
		LEA EAX,DWORD PTR DS:[ESI+0x50C]
		MOV DWORD PTR DS:[ESI+0x50C],EDX
		MOV DWORD PTR DS:[EAX+4],EDX
		LEA EAX,DWORD PTR DS:[ESI+0x514]
		MOV DWORD PTR DS:[ESI+0x514],ECX
		MOV DWORD PTR DS:[EAX+4],ECX
		MOV DWORD PTR DS:[EAX+8],ECX
		LEA EAX,DWORD PTR DS:[ESI+0x520]
		MOV DWORD PTR DS:[ESI+0x520],EDX
		MOV DWORD PTR DS:[EAX+4],EDX
		LEA EAX,DWORD PTR DS:[ESI+0x528]
		MOV DWORD PTR DS:[ESI+0x528],ECX
		MOV DWORD PTR DS:[EAX+4],ECX
		MOV DWORD PTR DS:[EAX+8],ECX
		MOV DWORD PTR DS:[EAX+0x0C],ECX
		LEA EAX,DWORD PTR DS:[ESI+0x538]
		MOV EDI, parent // test
		MOV ECX,EDI // ECX is (again) the popup (parent) object
		MOV DWORD PTR DS:[EAX],EDX
		MOV DWORD PTR DS:[EAX+4],EDX
		MOV DWORD PTR DS:[EAX+8],EDX
		MOV DWORD PTR DS:[ESI+0x544],EDX
		MOV DWORD PTR DS:[ESI+0x548],EDX
		MOV EAX, 0x00455D00
		CALL EAX
		PUSH EAX
		MOV ECX,EDI
		MOV EAX, 0x00455CF0
		CALL EAX
		PUSH EAX
		PUSH 0x186
		PUSH 0x258
		PUSH EDI
		MOV ECX,EDI
		MOV EAX, 0x00454260
		CALL EAX
		PUSH EAX
		MOV ECX, ESI
		MOV EAX, 0x004607A0 // UIObj.createPopupAndFocus(arg1, parentUI, hSize, vSize, dlgName?, arg6, arg7)?
		CALL EAX
		TEST EAX,EAX
		JE failed
	}
	return newPtr;
failed:
	return 0;
}


static long int AOE_GetGamePosFromMousePos(ROR_STRUCTURES_10C::STRUCT_UI_PLAYING_ZONE *gameZone,
	ROR_STRUCTURES_10C::STRUCT_TEMP_MAP_POSITION_INFO *pPosInfo, long int mousePosX, long int mousePosY) {
	if (!gameZone || !pPosInfo || !gameZone->IsCheckSumValid()) { return 0; }
	const unsigned long int callAddr = 0x511430;
	long int res;
	_asm {
		MOV ECX, gameZone;
		PUSH 0;
		PUSH pPosInfo;
		PUSH mousePosY;
		PUSH mousePosX;
		PUSH 0;
		PUSH 0x28;
		CALL callAddr;
		MOV res, EAX;
	}
	return res;
}


// Return true if successful. Only works if current screen is game screen !
// Adds a button in unit-commands panel (bottom, beside select unit info).
// player is used to find correct color.
// ButtonIndex is 0-5 (first row), 6-11 (2nd row), 5 and 11 are special (on the right, eg. next, cancel).
// helpDllId can be 0 ? creationDllId can be -1. 
// Name can be left NULL. Don't know exactly when this is being used (in help popup ?)
// If description == NULL, then creationDllId will be used for displayed context text.
// IconId and DATID are related to the type of action, be careful to provide consistent values.
// You can set DATId to 0 if not relevant.
static bool AOE_InGameAddCommandButton(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, long int buttonIndex, long int iconId,
	AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID UICmdId, long int DATID,
	long int helpDllId, long int creationDllId, long int shortcutDllId, const char *name, const char *description,
	bool isDisabled, ROR_STRUCTURES_10C::STRUCT_SLP_INFO *iconSlpInfo) {
	ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *inGameMain = (ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *) AOE_GetScreenFromName(gameScreenName);
	if (!inGameMain || !inGameMain->IsCheckSumValid() || !inGameMain->visible) {
		return false;
	}
	if (inGameMain != (void*)AOE_GetCurrentScreen()) {
		return false;
	}

	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) {
		return false;
	}
	unsigned long int unknown_colorPtr = NULL;

	const unsigned long int calladdr = 0x483760;
	ROR_STRUCTURES_10C::STRUCT_SLP_INFO *iconsSLP = iconSlpInfo;
	if ((iconSlpInfo == NULL) || (iconSlpInfo->slpSize == 0)) {
		iconsSLP = inGameMain->iconsForUnitCommands; // Default (for most cases)
	}
	
	// Guess automatically in which SLP we should search the icon.
	if (UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN) {
		unknown_colorPtr = player->ptrPlayerColorStruct + 0x28;
		iconsSLP = inGameMain->iconsForTrainUnits;
	}
	if (UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_BUILD) {
		unknown_colorPtr = player->ptrPlayerColorStruct + 0x28;
		assert(player->tileSet >= 0);
		assert(player->tileSet <= 4);
		int tileSet = player->tileSet;
		if ((tileSet < 0) || (tileSet > 4)) { tileSet = 0; } // in theory, this is not necessary.
		iconsSLP = inGameMain->iconsForBuildings[player->tileSet];
	}
	if ((UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH) ||
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_TRADE_FOOD_FOR_GOLD) || 
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_TRADE_STONE_FOR_GOLD) || 
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_TRADE_WOOD_FOR_GOLD)) {
		iconsSLP = inGameMain->iconsForResearches;
		unknown_colorPtr = player->ptrPlayerColorStruct + 0x28;
	}
	if ((UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CANCEL_BUILD) ||
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CANCEL_OR_BACK) || 
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CANCEL_SELECTION) || 
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_NEXT_PAGE)) {
		iconsSLP = inGameMain->unknown_4AC_icons;
	}

	// Check that iconId is valid in SLP : otherwise, the whole game display will encounter serious issues !
	if (!IsObjectIndexValidInSlp(iconsSLP, iconId)) {
		assert(!"iconId does not exist in this SLP file. Maybe a DRS file is missing.");
		return false;
	}

	long int long_UICmdId = (long int)UICmdId;
	long int long_isDisabled = isDisabled;
	_asm {
		PUSH long_isDisabled;
		PUSH description;
		PUSH name;
		PUSH unknown_colorPtr; // arg9
		PUSH shortcutDllId; // shortcut DLLID (hotkey)
		PUSH creationDllId; // arg7 = creation text (or hotkey text)
		PUSH helpDllId; // help dll id (language DLL creation + 100000)
		PUSH DATID; // DATID
		PUSH long_UICmdId;
		PUSH iconId; // iconid
		PUSH buttonIndex; // button index
		PUSH iconsSLP;
		MOV ECX, inGameMain;
		CALL calladdr;
	}
	return true;
}
