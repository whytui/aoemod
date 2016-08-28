#include <assert.h>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_ui_main_info.h>

#pragma once

/*
This file contains primitives mainly to retrieve main structures from global variables, or using links between main structures.
*/


// Returns a pointer to global game struct
// Warning: can sometimes return NULL when called very early (when the game has just been run)
AOE_STRUCTURES::STRUCT_GAME_GLOBAL* GetGameGlobalStructPtr();

// Returns a pointer to game settings struct
// ASSERTS that the pointer is non-NULL
AOE_STRUCTURES::STRUCT_GAME_SETTINGS* GetGameSettingsPtr();

// Returns the UI main information object
AOE_STRUCTURES::STRUCT_UI_MAIN_INFO *GetUIMainInfoStruct();


// Returns a pointer to current active UI (?)
// ASSERTS that the pointer is non-NULL
AOE_STRUCTURES::STRUCT_ANY_UI *GetCurrentUIStruct();

// Returns currently human-controlled player struct according to game settings struct
AOE_STRUCTURES::STRUCT_PLAYER *GetControlledPlayerStruct_Settings();

// Returns player struct for given player id
// returns NULL if incorrect player id
AOE_STRUCTURES::STRUCT_PLAYER *GetPlayerStruct(long int playerId);

AOE_STRUCTURES::STRUCT_CURSOR_SLP_INFO *GetCursorInfo();

