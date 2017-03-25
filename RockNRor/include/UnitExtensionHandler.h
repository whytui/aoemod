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
#include "RockNRorInfo.h"

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

	// Add/initialize extension data for the unitId specified
	// If the unit extension is already initialized, this does nothing (does NOT overwrite previous information)
	// If the unit does NOT exist AND 0 <= unitId <= global.seqUnitId, the extension IS created with NULL pointer and marked as "dead" unit.
	// Returns true if successful (including the case when unit is already initialized)
	bool AddUnitExtension(long int unitId);

	// Add/update infAI element for current unit (we consider it is visible for the player specified)
	// Remark: this method is called only if improveAI is true.
	bool AddUpdateInfAIElem(STRUCT_UNIT_BASE *unit, long int infAIPlayerId);

	// Remove element for provided unitId in specified player's InfAI elem list.
	// This does NOT use optimization from unitExtensions
	// For security, if unitExtension contains a pointer for the unit specified, the pointer is set to NULL and unitExtension invalidated.
	// To be used when improveAI is false.
	// Returns false in error cases only
	bool RemoveInfAIElemForUnitWithoutOptimization(long int unitId, long int infAIPlayerId);

	// Remove element for provided unitId in specified player's InfAI elem list.
	// This uses optimization from unitExtensions (if info is available)
	// You can use this even if improveAI is false
	// Returns false in error cases only
	bool RemoveInfAIElemForUnit(long int unitId, long int infAIPlayerId);

	// Remove element for provided unitId in all players' InfAI elem list.
	// This uses optimization from unitExtensions (if info is available)
	// You can use this even if improveAI is false
	// Returns true if successful for all players
	bool RemoveAllInfAIElemForUnit(long int unitId);

};


// Global object
extern UnitExtensionHandler unitExtensionHandler;

}
