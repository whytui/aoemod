#include <assert.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include "traceMessage.h"

#pragma once

/*
This file contains primitives to manipulate unit definition objects.
*/

using namespace AOE_STRUCTURES;

namespace PLAYER {

	// Transforms specified unit to "main" unit when a quick selection (via keyboard shortcut) is requested.
	// For example, for key 'H', the "main" TC is selected. This method allows to set the "main" TC to select with 'H'.
	// Actually, this moves the unit in player's "creatable" units list
	// This only has a visible effect for units that can be selected with a shortcut key.
	bool MakeMainUnitForShortcutSelection(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *unit);

}
