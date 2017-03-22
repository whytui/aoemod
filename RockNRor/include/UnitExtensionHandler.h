#pragma once

#include <AOE_struct_game_global.h>
#include <AOE_struct_units.h>
#include <AOE_struct_main_ai.h>
#include <AOE_struct_inf_ai.h>
#include <AOE_struct_player.h>
#include "AOE_memory.h"
#include "mainStructuresHandling.h"
#include "UnitExtension.h"
#include "RockNRorCommon.h"

/*
This file defines the class that handles units extension data for current game
*/

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
;

class UnitExtensionHandler {
public:
	UnitExtensionHandler();
	~UnitExtensionHandler();

	UnitExtension *allUnitExtensions;
	long int currentAllocatedElemCount; // valid unitIds are [0 to currentAllocatedElemCount-1]

	// Reset all underlying info, typically on new game (including load saved game, etc)
	void ResetAllInfo();

	// Create a new array, sized according to global structure's unit pointers size.
	// Returns true if successful
	bool ReallocArrayUsingGameGlobal();

	// Add/initialize extension data for the unit specified
	// If the unit extension is already initialized, this does nothing (does NOT overwrite previous information)
	// Returns true if successful (including the case when unit is already initialized)
	bool AddUnitExtension(STRUCT_UNIT_BASE *unit);

	// Add/update infAI element for current unit (we consider it is visible for the player specified)
	bool AddUpdateInfAIElem(STRUCT_UNIT_BASE *unit, long int infAIPlayerId);

};


// Global object
extern UnitExtensionHandler unitExtensionHandler;

}
