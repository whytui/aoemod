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
#include "EconomyAI.h"
#include "AOEPrimitives_lists.h"

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

	// Returns a unit extension IF it has already been initialized, NULL otherwise.
	// This may return NULL even if the unit exists in game !
	UnitExtension *GetUnitExtension(long int unitId);

	// Add/initialize extension data for the unit specified
	// If the unit extension is already initialized, this does nothing (does NOT overwrite previous information)
	// Returns true if successful (including the case when unit is already initialized)
	bool AddUnitExtension(STRUCT_UNIT_BASE *unit);

	// Add/initialize extension data for the unitId specified
	// If the unit extension is already initialized, this does nothing (does NOT overwrite previous information)
	// If the unit does NOT exist AND 0 <= unitId < global.seqUnitId, the extension IS created with NULL pointer and marked as "dead" unit.
	// Returns true if successful (including the case when unit is already initialized)
	// You can call this at any time with no restriction (unitId better be valid though)
	bool AddUnitExtension(long int unitId);

	// Add/update specified unit in infAI lists (we consider it is visible for the player specified)
	// DO NOT USE this if unit is not visible !!!
	// Remark: this method is called only if improveAI is true.
	bool AddUpdateUnitInInfAILists(STRUCT_UNIT_BASE *unit, long int infAIPlayerId);

	// Get the index in "InfAI" unit memory list for provided unit ID.
	// Returns -1 if not found
	// This method is faster than basic search because it uses 'cached' index (if possible)
	long int GetIndexForUnitInInfAIUnitMemory(long int unitIdToSearch, long int infAIPlayerId);

	// Get the pointer to "InfAI" unit mmory object for provided unit ID.
	// Returns NULL if not found
	// This method is faster than basic search because it uses 'cached' index (if possible)
	STRUCT_UNIT_MEMORY *UnitExtensionHandler::GetInfAIUnitMemory(long int unitIdToSearch, STRUCT_INF_AI *infAI);

	
	// Returns NULL if not found
	// This method is faster than basic search because it uses 'cached' index (if possible)
	STRUCT_UNIT_MEMORY *GetInfAIUnitMemory(long int unitIdToSearch, long int infAIPlayerId);

	// Remove element for provided unitId in specified player's InfAI unit memory list.
	// This does NOT use optimization from unitExtensions
	// For security, if unitExtension contains a pointer for the unit specified, the pointer is set to NULL and unitExtension invalidated.
	// To be used when improveAI is false.
	// Returns false in error cases only
	bool RemoveInfAIUnitMemoryWithoutOptimization(long int unitId, long int infAIPlayerId);

	// Remove element for provided unitId in specified player's InfAI unit memory list.
	// This uses optimization from unitExtensions (if info is available)
	// You can use this even if improveAI is false
	// Returns false in error cases only
	bool RemoveInfAIUnitMemoryForUnit(long int unitId, long int infAIPlayerId);

	// Remove element for provided unitId in all players' InfAI unit memory list.
	// This uses optimization from unitExtensions (if info is available)
	// You can use this even if improveAI is false
	// Returns true if successful for all players
	bool RemoveAllInfAIUnitMemoryForUnit(long int unitId);


private:
	// Adds (if necessary) unit in infAI lists (other than "spotted units detailed info)
	// This is optimized (using unit extension) so that "adding" operation is done only once.
	// Returns false if an error occurred
	bool AddUnitInOtherInfAILists(STRUCT_UNIT_BASE *unit, STRUCT_INF_AI *infAI);

	// Add/update infAI unit memory for current unit (we consider it is visible for the player specified)
	// Remark: this method is called only if improveAI is true.
	bool AddUpdateInfAIUnitMemory(STRUCT_UNIT_BASE *unit, STRUCT_INF_AI *infAI);

};


// Global object
extern UnitExtensionHandler unitExtensionHandler;

}
