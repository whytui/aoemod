#include <assert.h>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_ui_panel_system.h>

#pragma once

/*
This file contains primitives mainly to retrieve main structures from global variables, or using links between main structures.
*/


namespace AOE_STRUCTURES {
;

// Returns a pointer to global game struct
// Warning: can sometimes return NULL when called very early (when the game has just been run)
AOE_STRUCTURES::STRUCT_GAME_GLOBAL* GetGameGlobalStructPtr();

// Returns a pointer to game settings struct
// ASSERTS that the pointer is non-NULL
AOE_STRUCTURES::STRUCT_GAME_SETTINGS* GetGameSettingsPtr();

// Returns the UI main information object
AOE_STRUCTURES::STRUCT_UI_PANEL_SYSTEM *GetUIMainInfoStruct();
// Returns the UI system object - synonym to GetUIMainInfoStruct()
AOE_STRUCTURES::STRUCT_UI_PANEL_SYSTEM *GetUIPanelSystem();

// Return current screen, using UI panel system
STRUCT_UI_EASY_PANEL *GetCurrentScreen();

// Returns currently human-controlled player struct according to game settings struct
AOE_STRUCTURES::STRUCT_PLAYER *GetControlledPlayerStruct_Settings();

// Returns player struct for given player id
// returns NULL if incorrect player id
AOE_STRUCTURES::STRUCT_PLAYER *GetPlayerStruct(long int playerId);

// Useful to get structure from a unit id. May return NULL !
// Only works for creatable (unitId >= 0). This is just a choice to avoid writing same bugs as ROR
// (some functions use -1 as <No unit> but get an irrevant unit struct then because -1 is not tested before calling getUnitStruct(...))
AOE_STRUCTURES::STRUCT_UNIT_BASE *GetUnitStruct(long int unitId);

// Returns a unit definition if valid, NULL otherwise.
AOE_STRUCTURES::STRUCT_UNITDEF_BASE *GetUnitDefStruct(AOE_STRUCTURES::STRUCT_PLAYER *player, short int unitDefId);


AOE_STRUCTURES::STRUCT_CURSOR_SLP_INFO *GetCursorInfo();

// Returns true if current game is multiplayer, false otherwise (including error cases)
bool IsMultiplayer();

// Returns true if the game is currently running
// Warning: this returns true if game is "running" and paused !
bool IsGameRunning();

// Returns true if the game is currently in PAUSED mode.
// Returns false if the game is runnning (not paused), if in editor, in menus, etc.
bool IsGamePaused();

}
