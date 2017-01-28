
#pragma once
#include <assert.h>
#include <string.h>
#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_button.h>
#include <UI_components\AOE_struct_ui_label.h>
#include <UI_components\AOE_struct_ui_textbox.h>
#include <UI_components\AOE_struct_ui_combobox.h>
#include <UI\AOE_struct_ui_player_resource_values.h>

/*
* This file provides useful (raw) methods to deal with AOE UI components.
* Most of them directly call ROR's methods and are a practical encapsulation to avoid using assembler in our code.
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/


enum AOE_FONTS : long int {
	AOE_FONT_BIG_LABEL = 0,
	AOE_FONT_VERY_BIG_LABEL_1 = 1,
	AOE_FONT_BIG_HEIGHT_2 = 2,
	AOE_FONT_BIG_HEIGHT_3 = 3,
	AOE_FONT_BIG_LABEL_4 = 4,
	AOE_FONT_BIG_HEIGHT_5 = 5,
	AOE_FONT_SMALL_TEXT_6 = 6, // Small text. Used in About popup.
	AOE_FONT_SMALL_TEXT_7 = 7, // Small text. Used in About popup.
	AOE_FONT_STANDARD_STRIKETHROUGH = 8, // Standard font with strikethrough formatting
	AOE_FONT_VERY_SMALL = 9, // Smaller than standard font
	AOE_FONT_SMALL_TEXT_10 = 10, // Small text.
	AOE_FONT_STANDARD_TEXT = 11 // Standard font, mostly used. (0x0B)
	// 12+ invalids
};

namespace AOE_METHODS {

namespace UI_BASE {
;

// Set parent's focus to child object.
// child CAN be NULL (set focus to parent itself).
static void SetFocus(AOE_STRUCTURES::STRUCT_ANY_UI *parent, AOE_STRUCTURES::STRUCT_ANY_UI *child) {
	if (!parent) { return; }
	unsigned long int addr = 0x453EB0; // parentObj.setFocus(childObj)
	_asm {
		PUSH child;
		MOV ECX, parent;
		CALL addr;
	}
}


// Show/Hide a UI object (cf CALL DS:[EDX+0x14])
static void ShowUIObject(AOE_STRUCTURES::STRUCT_ANY_UI *object, bool show) {
	if (!object) { return; }
	long int arg = show ? 1 : 0;
	_asm {
		MOV ECX, object;
		MOV EAX, DS:[ECX];
		PUSH arg;
		CALL DS:[EAX + 0x14];
	}
}

// Refresh a UI object (cf CALL DS:[EDX+0x20])
static void RefreshUIObject(AOE_STRUCTURES::STRUCT_ANY_UI *object) {
	if (!object) { return; }
	_asm {
		MOV ECX, object;
		MOV EAX, DS:[ECX];
		PUSH 1;
		CALL DS:[EAX + 0x20];
	}
}

// Refresh a UI object. forceLevel is not very well known !
static void RefreshUIObject(AOE_STRUCTURES::STRUCT_ANY_UI *object, long int forceLevel) {
	if (!object) { return; }
	_asm {
		MOV ECX, object;
		MOV EAX, DS:[ECX];
		PUSH forceLevel;
		CALL DS:[EAX + 0x20];
	}
}



// The AOE_Addxxx functions create a UI object and store the new object's pointer in ptrObjToCreate parameter


// Note: basic size (eg OK button) is 0x?? / 0x1E
static bool AddButton(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_BUTTON **ptrObjToCreate, unsigned long int DLL_STRING_ID,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	long int buttonId = 0, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT) {
	if (!parent) { return false; }
	unsigned long int result;
	_asm {
		MOV ECX, parent;
		PUSH buttonId;
		PUSH 0; // arg10?
		PUSH font;
		PUSH vSize;
		PUSH hSize;
		PUSH vPos;
		PUSH hPos;
		PUSH 0x00; // arg4
		PUSH DLL_STRING_ID;
		PUSH ptrObjToCreate;
		PUSH ECX;
		MOV EAX, 0x00455D10; // addbutton
		CALL EAX;
		MOV result, EAX;
	}
	return result != 0;
}


// Note: basic size (eg OK button) is 0x?? / 0x1E
static bool AddButton(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_BUTTON **ptrObjToCreate, const char *caption,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	long int buttonId = 0, AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT) {
	if (!parent) { return false; }
	unsigned long int result;
	_asm {
		MOV ECX, parent;
		PUSH buttonId;
		PUSH 0; // arg10?
		PUSH font;
		PUSH vSize;
		PUSH hSize;
		PUSH vPos;
		PUSH hPos;
		PUSH 0x00; // arg4 = text2. Unused?
		PUSH caption;
		PUSH ptrObjToCreate;
		PUSH ECX;
		MOV EAX, DS:[ECX];
		CALL DS:[EAX + 0xF0]; // addbutton (using text)
		MOV result, EAX;
	}
	return result != 0;
}


// For fonts, see AOE_FONTS enum
static bool AddLabel(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_LABEL **ptrObjToCreate, const char *label,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize,
	AOE_FONTS font = AOE_FONTS::AOE_FONT_STANDARD_TEXT
	) {
	if (parent == NULL) { return false; }
	long int result;
	_asm {
		MOV ECX, parent;
		PUSH 0; // arg11
		PUSH 1; // arg10
		PUSH 0; // arg9
		PUSH font;
		PUSH vSize;
		PUSH hSize;
		PUSH vPos;
		PUSH hPos;
		PUSH label;
		PUSH ptrObjToCreate;
		PUSH ECX; // Parent
		MOV EDX, DS:[ECX];
		CALL[EDX + 0x104]; // or 0x456530
		MOV result, EAX;
	}
	return result != 0;
}


// Create a textbox using ROR methods.
// If maxTextLength==0, it is replaced by initialText's length.
// Note: The font used seems to be 14 pixels high (?)
static bool AddTextBox(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX **ptrObjToCreate, const char *initialText, long int maxTextLength,
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
		MOV ECX, parent;
		PUSH 0;
		PUSH 0; // arg11
		PUSH font;
		PUSH vSize;
		PUSH hSize;
		PUSH vPos;
		PUSH hPos;
		PUSH inputType; // arg5
		PUSH maxTextLength;
		MOV EAX, initialText;
		PUSH EAX; // initial value
		PUSH ptrObjToCreate;
		PUSH ECX; // parent
		MOV EAX, 0x00456A30;
		CALL EAX;
		MOV result, EAX;
	}
	if (!readOnly && result) {
		_asm {
			// Make writeable
			MOV EAX, ptrObjToCreate;
			MOV EAX, DS:[EAX];
			MOV DS : [EAX + 0x074], 1;
		}
	}
	return result != 0;
}


// Create a "AOE" checkbox (same type as buttons)
// You need to create a label if you want some text aside the checkbox
static bool AddCheckBox(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_BUTTON **ptrObjToCreate,
	unsigned long int hPos, unsigned long int vPos, unsigned long int hSize, unsigned long int vSize) {
	if (!parent) { return false; }
	long int result;
	_asm {
		MOV ECX, parent;
		PUSH 0; // arg8
		PUSH 0; // arg7
		PUSH vSize;
		PUSH hSize;
		PUSH vPos;
		PUSH hPos;
		PUSH ptrObjToCreate;
		PUSH ECX; // parent
		MOV EAX, 0x00456240; // create checkbox
		CALL EAX;
		MOV result, EAX;
	}
	return result != 0;
}


static void CheckBox_SetChecked(AOE_STRUCTURES::STRUCT_UI_BUTTON *checkBox, bool checked) {
	if (!checkBox) { return; }
	checkBox->checked = checked ? 1 : 0;
	int arg = checkBox->checked;
	_asm {
		MOV ECX, checkBox;
		PUSH arg;
		MOV EAX, 0x0045F2F0;
		CALL EAX;
	}
}


// ptrCombo = address of ROR combobox object
// entryId = id to affect to new entry in combobox (generally=index of the entry)
// DLLID = ID in language(x).dll of string to use for entry.
static void AddEntryInComboUsingDLLID(AOE_STRUCTURES::STRUCT_ANY_UI *ptrCombo, long int entryId, long int DLLID) {
	assert(ptrCombo != NULL);
	if (!ptrCombo) { return; }
	_asm {
		MOV ECX, ptrCombo;
		PUSH entryId;
		PUSH DLLID;
		MOV EAX, 0x461DD0;
		CALL EAX; // Add entry in combo
	}
}


// Creates a combobox with 1 "empty" entry. Use AddEntryInCombo afterwards.
// TO DO: this is unfinished. See example in 0x4881A4...
// Unstable and not correctly supported !
static bool AddComboBox(AOE_STRUCTURES::STRUCT_ANY_UI *parent,
	AOE_STRUCTURES::STRUCT_UI_COMBOBOX **ptrObjToCreate,
	long int posX, long int posY, long int listSizeX, long int listSizeY, long int lblSizeX, long int lblSizeY, AOE_FONTS fontId) {
	if (!parent) { return false; }
	long int result;
	_asm {
		MOV ECX, parent;
		PUSH fontId;
		PUSH lblSizeY;
		PUSH lblSizeX;
		PUSH posY;
		PUSH posX;
		PUSH listSizeY;
		PUSH listSizeX;
		PUSH ptrObjToCreate;
		PUSH ECX; // parent
		MOV EAX, 0x00456BF0; // create combobox
		CALL EAX;
		MOV result, EAX;
	}
	return (result != 0);
}



static void AddEntryInCombo(AOE_STRUCTURES::STRUCT_ANY_UI *ptrCombo, long int entryId, const char *text) {
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


// Returns an edit object's text
static char *GetEditText(AOE_STRUCTURES::STRUCT_UI_TEXTBOX *ptrEdit) {
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


// Set an label object's text
static void SetLabelText(AOE_STRUCTURES::STRUCT_UI_LABEL *ptrLabel, const char *text) {
	assert(ptrLabel != NULL);
	if (!ptrLabel || !ptrLabel->IsCheckSumValid()) { return; }
	_asm {
		PUSH text;
		MOV ECX, ptrLabel;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX+0xEC];
	}
}


static void ListBox_clear(AOE_STRUCTURES::STRUCT_UI_LISTBOX *listBox) {
	if (!listBox) { return; }
	_asm {
		MOV ECX, listBox
		MOV EDX, 0x46A6C0
		CALL EDX
	}
}


// Adds an item at position for a listbox / combobox
static bool Listbox_addItem(AOE_STRUCTURES::STRUCT_UI_LISTBOX *obj, long int position, const char *text, long int optionalId) {
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


// Set text color for a label
static void SetLabelTextColor(AOE_STRUCTURES::STRUCT_UI_LABEL *label, unsigned long int textColorRGB, unsigned long int textShadowColorRGB) {
	if (!label) { return; }
	unsigned long int addr = 0x469070;
	_asm {
		PUSH textShadowColorRGB;
		PUSH textColorRGB;
		MOV ECX, label;
		CALL addr;
	}
}


// Set text color for player resource values label
static void SetPlayerResValuesTextColor(AOE_STRUCTURES::STRUCT_UI_PLAYER_RESOURCE_VALUES *resValues, unsigned long int textColorRGB, unsigned long int textShadowColorRGB) {
	if (!resValues) { return; }
	unsigned long int addr = 0x4F7CF0;
	_asm {
		PUSH textShadowColorRGB;
		PUSH textColorRGB;
		MOV ECX, resValues;
		CALL addr;
	}
}


// Set text color for a button, for a given "info index"
static void SetButtonTextColor(AOE_STRUCTURES::STRUCT_UI_BUTTON *btn, unsigned long int textColorRGB, 
	unsigned long int textShadowColorRGB, long int btnInfoIndex = 0) {
	if (!btn) { return; }
	unsigned long int addr = 0x45F210;
	_asm {
		PUSH textShadowColorRGB;
		PUSH textColorRGB;
		PUSH btnInfoIndex;
		MOV ECX, btn;
		CALL addr;
	}
}

}
}
