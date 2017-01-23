
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


// Set the text to show in "F5 debug info" zone.
// Warning: if "debug info zone visibility" has not been set, this won't have any visible effect.
static void GameMainUI_writeF5DebugInfo(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, char *text) {
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		return;
	}
	char *foo = "";
	if (!text) {
		text = foo;
	}
	const unsigned long int addr = 0x482140;
	_asm {
		PUSH text;
		PUSH foo; // arg1 is unused.
		MOV ECX, gameMainUI;
		CALL addr;
	}
}


// Write up to 6 text lines in game zone as colored "chat" text in bottom-left zone.
// Line1 is top line, line6 is bottom line (note: reverse in AOE method call)
static void GameMainUI_writeTextDebugLines(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
	const char *line1, const char *line2, const char *line3, const char *line4, const char *line5, const char *line6) {
	const unsigned long int addr = 0x4821B0;
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		return;
	}
	const char *empty = "";
	if (!line1) { line1 = empty; }
	if (!line2) { line2 = empty; }
	if (!line3) { line3 = empty; }
	if (!line4) { line4 = empty; }
	if (!line5) { line5 = empty; }
	if (!line6) { line6 = empty; }
	_asm {
		MOV ECX, gameMainUI;
		// Note: reversing order of text lines
		PUSH line1;
		PUSH line2;
		PUSH line3;
		PUSH line4;
		PUSH line5;
		PUSH line6;
		call addr;
	}
}


// DO NOT call this if current UI is not "in-game" screen
static void CallWriteText(const char *txt) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings != NULL);
	assert((settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) ||
		(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_GAME_OVER_BUT_STILL_IN_GAME));
	long int addr = (long int)txt;
	long int writeFct = 0x480760; // DisplayChatTextOnNextLine
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMain = settings->ptrGameUIStruct;
	if (!gameMain) { return; }
	_asm {
		push addr;
		MOV ECX, gameMain;
		call writeFct;
	}
}


// This will display a text in the game screen (like yellow/orange error messages)
// Note: such a message disappears when the user clicks somewhere.
static void CallWriteCenteredText(const char *txt, long int numLine = 1, long int color = 0x55, long int background = 0) {
	long int addr = (long int)txt;
	long int writeFct = 0x0482260;
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (settings == NULL) { return; }
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMain = settings->ptrGameUIStruct;
	if (!gameMain) { return; }
	_asm {
		PUSH background;
		PUSH color;
		PUSH addr;
		PUSH numLine;
		MOV ECX, gameMain;
		call writeFct;
	}
}



// Displays the green blinking around a unit (like in right-click)
// Default blinking time in ROR code is 1500 milliseconds.
// To allow more than 1 blinking at once, remove the SUB instruction in 0x50F9B0.
static void DisplayGreenBlinkingOnUnit(AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *gamePlayZone, long int unitId,
	unsigned long int blinkingTime_ms) {
	if (!gamePlayZone || !gamePlayZone->IsCheckSumValid()) {
		return;
	}
	if ((blinkingTime_ms < 0) || (unitId <= 0)) { return; } // Invalid arguments
	unsigned long int addr = 0x50F970;
	_asm {
		MOV ECX, gamePlayZone;
		PUSH 2;
		PUSH 2;
		PUSH blinkingTime_ms;
		PUSH unitId; // must NOT be <0
		CALL addr;
	}
}


}
}
