#include <assert.h>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <ROR_structures.h>

#pragma once

/*
This file contains primitives mainly to retrieve main structures from global variables, or using links between main structures.
*/


// Returns a pointer to global game struct
// Warning: can sometimes return NULL when called very early (when the game has just been run)
ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL* GetGameGlobalStructPtr();

// Returns a pointer to game settings struct
// ASSERTS that the pointer is non-NULL
ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS* GetGameSettingsPtr();


// Returns a pointer to current active UI (?)
// ASSERTS that the pointer is non-NULL
ROR_STRUCTURES_10C::STRUCT_ANY_UI *GetCurrentUIStruct();

// Returns currently human-controlled player struct according to game settings struct
ROR_STRUCTURES_10C::STRUCT_PLAYER *GetControlledPlayerStruct_Settings();

// Returns player struct for given player id
// returns NULL if incorrect player id
ROR_STRUCTURES_10C::STRUCT_PLAYER *GetPlayerStruct(long int playerId);
