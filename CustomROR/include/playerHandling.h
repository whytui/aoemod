#include <assert.h>
#include <list>
#include <algorithm>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include "AOE_memory.h"
#include "traceMessage.h"
#include "mainStructuresHandling.h"
#include "researches.h"

#pragma once

/*
This file contains primitives to manipulate unit definition objects.
*/

using namespace AOE_STRUCTURES;

namespace AOE_METHODS {

	void ClearSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player);
	// select: if true, add unit to selection. If false, remove from selection.
	bool SelectUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, bool select);


}

namespace PLAYER {

	// Call this to make sure "currently managed AI player" is valid, so that AI is not stuck.
	void SetAValidCurrentAIPlayerId();

	// Remove all AI-controlled flags for currently controlled player (cf game settings structure).
	// Only for single player games.
	void RemoveAIFlagsForCurrentlyControlledPlayer();


	// Return a list of all unitDefIds that are/can be enabled in player's tech tree.
	std::list<long int> GetActivableUnitDefIDs(AOE_STRUCTURES::STRUCT_PLAYER *player);

	// Returns <> 0 if a unit is (currently) available for a given player. Returns the unit def pointer.
	// Does not consider potential researches that could enable the unit afterwards.
	// Warning: this only works for non-upgraded unit def IDs. OK for clubman, not for axeman (because axeman's DATID1 is clubman's DATID).
	long unsigned int IsUnitAvailableForPlayer(short int DAT_ID, AOE_STRUCTURES::STRUCT_PLAYER *player);

	// Searches (at least) a unit with provided shortcut number for given player.
	// The first matching unit is returned (arbitrary), even if there are several.
	AOE_STRUCTURES::STRUCT_UNIT_BASE *FindUnitWithShortcutNumberForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, char shortcutNumber);

	// Get number of matching units (for a unitDef ID) for given player. Restricted to "creatable" units
	// -1 are jokers for DAT_ID, unitAIType, unitStatus
	long int GetPlayerUnitCount(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID,
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType, char minUnitStatus, char maxUnitStatus);

	// Disable the research that enables a given unit, so that it can't be trained.
	// For units that are enabled by a "visible" research, like short sword, the research won't be visible (disabled).
	// To be called before game starts
	// Returns true if a research was found and disabled.
	bool DisableUnitForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID);


	// Transforms specified unit to "main" unit when a quick selection (via keyboard shortcut) is requested.
	// For example, for key 'H', the "main" TC is selected. This method allows to set the "main" TC to select with 'H'.
	// Actually, this moves the unit in player's "creatable" units list
	// This only has a visible effect for units that can be selected with a shortcut key.
	bool MakeMainUnitForShortcutSelection(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *unit);

	// Transforms specified unit to "main" unit when a quick selection (via keyboard shortcut) is requested.
	// For example, for key 'H', the "main" TC is selected. This method allows to set the "main" TC to select with 'H'.
	// Actually, this moves the unit in player's "creatable" units list
	// This only has a visible effect for units that can be selected with a shortcut key.
	// This is only possible when unitId belongs to human-controlled player.
	bool MakeMainUnitForShortcutSelection(long int unitId);


	// OBSOLETE: avoid using it
	// Duplicates an existing unit definition (srcDAT_ID) into a new unit definition for given player.
	// New unitDef will be available in player's list. Everything is managed so that the game will automatically free everything correctly.
	// You can send "" as name (in that case original unit's name will be used). Max length = 0x2F.
	// Returns the new DAT_ID if successful, -1 otherwise.
	// You can change any value for the new unitDef (but avoid messing with pointers?), but make sure you do it BEFORE you create units of this kind !
	// It is not recommended to call this too much ! It would add a lot of unit definition and would impact seriously game performance.
	// The maximum number of DAT_ID we allow here is 0x7FFF (to take no risk regarding signed short int values)
	// The method supports all unit (def) types
	// Warning: there might be issues when such units are converted (bug in destructor in AOE, a unitDef for a given DATID should exist for all civs ?)
	// Maybe it's better not to use it for living/buildings units (because of conversion)
	// Note: see also AOE unitDef constructors that copy an existing one, e.g. 4ED1B0 for living (type 70).
	short int DuplicateUnitDefinitionForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int srcDAT_ID, char *name);


}
