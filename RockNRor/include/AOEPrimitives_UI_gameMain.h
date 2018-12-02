
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
void GameMainUI_writeF5DebugInfo(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, const char *text);


// Write up to 6 text lines in game zone as colored "chat" text in bottom-left zone.
// Line1 is top line, line6 is bottom line (note: reverse in AOE method call)
void GameMainUI_writeTextDebugLines(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
	const char *line1, const char *line2, const char *line3, const char *line4, const char *line5, const char *line6);


void ShowF11_zone();


// DO NOT call this if current UI is not "in-game" screen
void CallWriteText(const char *txt);


// This will display a text in the game screen (like yellow/orange error messages)
// Note: such a message disappears when the user clicks somewhere.
void CallWriteCenteredText(const char *txt, long int numLine = 1, long int color = 0x55, long int background = 0);


// Show "F5" debug infos for communications (original - hidden - ROR behaviour)
bool ShowHiddenDebugComm(AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings);


// Show "F5" debug infos for currently controlled AI player (original - hidden - ROR behaviour)
bool ShowHiddenDebugAIInfo(AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings);


// Displays the green blinking around a unit (like in right-click)
// Default blinking time in ROR code is 1500 milliseconds.
// To allow more than 1 blinking at once, remove the SUB instruction in 0x50F9B0.
void DisplayGreenBlinkingOnUnit(AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *gamePlayZone, long int unitId,
	unsigned long int blinkingTime_ms);


// Show a in-game sign: movement red cross or step flag, or even other stuff !
// displayMode: 0=no display? 1=loop (repeat), 2=show once and stop (recommended). Don't know how to stop a 'repeat' mode !
// drawInterval: milliseconds between 2 SLP frames. 90 for moving red cross, 125 for attack zone red cross, 250 for move step...
AOE_STRUCTURES::STRUCT_UI_OVERLAY_SPRITE *DisplayInGameSign(AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *UIGameZone,
	AOE_STRUCTURES::STRUCT_SLP_INFO *slp, long int mousePosX, long int mousePosY, long int displayMode,
	long int drawInterval);


// Get screen coordinates (for UI) for a specific unit. All pointers must be non-null.
// Returns true if successful
bool GetScreenPosForUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, long int *pOutScreenX, long int *pOutScreenY);


// Get screen coordinates (for UI) for a specific game position. Pointers must be non-null.
// Returns true if successful
bool GetScreenPosFromGamePos(long int posX, long int posY, long int *pOutScreenX, long int *pOutScreenY);
// Get screen coordinates (for UI) for a specific game position. Pointers must be non-null.
// Returns true if successful
bool GetScreenPosFromGamePos(float posX, float posY, long int *pOutScreenX, long int *pOutScreenY);


// Add a line with an attribute icon/value in game unit info zone (bottom left)
// If a line is added, lineIndex is incremented.
// displayType: 
// - 0 = standard number "7"
// - 1 = number with upgrade: "7+1"
void UnitInfoZoneAddAttributeLine(AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone,
	long int iconId, long int displayType, long int displayedValue, long int totalValue, long int &lineIndex);


// Generate full creation text for a button (research, train) with costs and everything.
// buffer size must be at least 0x200.
// unitButtonInfo is allowed to be NULL.
// Cost info come from unitButtonInfo.
// elemLanguageCreationDllId is unit (or research) creationDllId (it includes shortcut key, if any)
void GetUIButtonCreationText(char *buffer, AOE_STRUCTURES::STRUCT_UI_UNIT_BUTTON_INFO *unitButtonInfo,
	AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID uiCmdId, long int elemLanguageCreationDllId);


}
}
