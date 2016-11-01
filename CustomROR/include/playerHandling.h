#include <assert.h>
#include <list>
#include <algorithm>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include "traceMessage.h"
#include "mainStructuresHandling.h"
#include "researches.h"

#pragma once

/*
This file contains primitives to manipulate unit definition objects.
*/

using namespace AOE_STRUCTURES;

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


}
