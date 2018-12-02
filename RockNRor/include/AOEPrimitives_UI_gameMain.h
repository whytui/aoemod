
#pragma once
#include <assert.h>
#include <string.h>
#include <UI_components\AOE_struct_any_ui.h>
#include <UI\AOE_struct_ui_scenario_editor_main.h>
#include <UI\AOE_struct_ui_in_game_main.h>
#include <UI\AOE_struct_ui_unit_info_zone.h>
#include <AOE_const_internal.h>
#include <AOE_offsets.h>
#include <AOE_struct_game_settings.h>
#include "AOEPrimitives_UI_components.h"
#include "AOEPrimitives_global.h"

/*
* This file provides useful (raw) basic methods to deal with AOE UI screens.
* Most of them directly call ROR's methods and are a practical encapsulation to avoid using assembler in our code.
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/


namespace AOE_STRUCTURES {
	extern AOE_STRUCTURES::STRUCT_GAME_SETTINGS* GetGameSettingsPtr();
}


namespace AOE_METHODS {
namespace UI_BASE {
;


// Set the text to show in "F5 debug info" zone.
// Warning: if "debug info zone visibility" has not been set, this won't have any visible effect.
static void GameMainUI_writeF5DebugInfo(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, const char *text) {
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


static void ShowF11_zone() {
	// Show "F11" information in top part of game zone
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid() || !settings->ptrGameUIStruct || !settings->ptrGameUIStruct->IsCheckSumValid()) {
		return;
	}
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI = settings->ptrGameUIStruct;
	AOE_STRUCTURES::STRUCT_ANY_UI *topleft = gameMainUI->topLeftInfoLabel;
	AOE_STRUCTURES::STRUCT_ANY_UI *popInfo = gameMainUI->populationInfoPanel;
	AOE_STRUCTURES::STRUCT_ANY_UI *playZone = gameMainUI->gamePlayUIZone;
	assert(topleft && popInfo && playZone);
	if (!topleft || !popInfo || !playZone) { return; }
	const unsigned long int addr1 = 0x004FAF80;
	const unsigned long int addr2 = 0x004839A0;
	_asm {
		MOV ECX, topleft;
		PUSH 0;
		PUSH 2;
		CALL addr1;
	}
	AOE_METHODS::UI_BASE::ShowUIObject(popInfo, true);
	_asm {
		MOV ECX, gameMainUI;
		CALL addr2;
	}
	AOE_METHODS::UI_BASE::RefreshUIObject(playZone, 1);
}


// DO NOT call this if current UI is not "in-game" screen
static void CallWriteText(const char *txt) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = AOE_STRUCTURES::GetGameSettingsPtr();
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
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = AOE_STRUCTURES::GetGameSettingsPtr();
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


// Show "F5" debug infos for communications (original - hidden - ROR behaviour)
static bool ShowHiddenDebugComm(AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings) {
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	_asm {
		MOV ECX, settings;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x13C]; // gameSettings.showComm()
	}
}

// Show "F5" debug infos for currently controlled AI player (original - hidden - ROR behaviour)
static bool ShowHiddenDebugAIInfo(AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings) {
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	_asm {
		MOV ECX, settings;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x140]; // gameSettings.showAI()
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


// Show a in-game sign: movement red cross or step flag, or even other stuff !
// displayMode: 0=no display? 1=loop (repeat), 2=show once and stop (recommended). Don't know how to stop a 'repeat' mode !
// drawInterval: milliseconds between 2 SLP frames. 90 for moving red cross, 125 for attack zone red cross, 250 for move step...
static AOE_STRUCTURES::STRUCT_UI_OVERLAY_SPRITE *DisplayInGameSign(AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *UIGameZone,
	AOE_STRUCTURES::STRUCT_SLP_INFO *slp, long int mousePosX, long int mousePosY, long int displayMode,
	long int drawInterval) {
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	assert((displayMode >= 0) && (displayMode <= 2));
	assert(UIGameZone && UIGameZone->IsCheckSumValid());
	const unsigned long int addr = 0x516050;
	if (!UIGameZone || !UIGameZone->IsCheckSumValid() || !slp) { return NULL; }
	long int mx = UIGameZone->unknown_130_mousePosX + mousePosX;
	long int my = UIGameZone->unknown_134_mousePosY + mousePosY;
	AOE_STRUCTURES::STRUCT_UI_OVERLAY_SPRITE *result = NULL;
	_asm {
		MOV ECX, UIGameZone;
		PUSH drawInterval; // Display interval (ms between 2 SLP frames)
		PUSH displayMode; // arg8 = mode? 1=persist/replay, 2=play once
		PUSH 0; // color table
		PUSH 0x0F; // drawLevel
		PUSH 0; // flags
		PUSH my;
		PUSH mx;
		PUSH 0; // facet
		PUSH slp;
		CALL addr;
		MOV result, EAX;
	}
	AOE_METHODS::UI_BASE::RefreshUIObject(UIGameZone);
	return result;
}



// Add a line with an attribute icon/value in game unit info zone (bottom left)
// If a line is added, lineIndex is incremented.
// displayType: 
// - 0 = standard number "7"
// - 1 = number with upgrade: "7+1"
static void UnitInfoZoneAddAttributeLine(AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone,
	long int iconId, long int displayType, long int displayedValue, long int totalValue, long int &lineIndex) {
	if (!unitInfoZone || !unitInfoZone->IsCheckSumValid()) {
		return;
	}
	if (iconId < 0) { return; } // and zero ?
	// Make sure we use correct stack data
	totalValue = totalValue & 0xFFFF;
	displayedValue = displayedValue & 0xFFFF;
	long int line = lineIndex; // PUSH lineIndex would not push the VALUE !
	_asm {
		PUSH totalValue;
		PUSH displayedValue;
		PUSH displayType;
		PUSH iconId;
		PUSH line;
		MOV ECX, unitInfoZone;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0xE4]; // Add info line
	}
	lineIndex++;
}


// Generate full creation text for a button (research, train) with costs and everything.
// buffer size must be at least 0x200.
// unitButtonInfo is allowed to be NULL.
// Cost info come from unitButtonInfo.
// elemLanguageCreationDllId is unit (or research) creationDllId (it includes shortcut key, if any)
static void GetUIButtonCreationText(char *buffer, AOE_STRUCTURES::STRUCT_UI_UNIT_BUTTON_INFO *unitButtonInfo,
	AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID uiCmdId, long int elemLanguageCreationDllId) {
	if (!buffer) {
		return;
	}
	buffer[0] = 0;
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	unsigned long int addr = 0x4834F0;
	_asm {
		PUSH elemLanguageCreationDllId;
		PUSH uiCmdId;
		PUSH unitButtonInfo;
		PUSH buffer;
		CALL addr;
	}
}



}
}
