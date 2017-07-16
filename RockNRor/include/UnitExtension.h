#pragma once

#include <AOE_struct_units.h>
#include <AOE_struct_inf_ai.h>
#include <AOE_struct_player.h>
#include "AOEPrimitives_units.h"
#include "traceMessage.h"

/*
This file contains additional fields for UNIT structures
Of course, those fieds are not saved in savegame files nor scenario files.
*/

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
;

// Represents additional information related to a specific game unit
class UnitExtension {
public:
	UnitExtension();

	bool isInitialized; // True if this unitExtension has been initialized with correct unit data == unit is "known" by extension system.
	bool unitDied; // True if unit existed at some point (in this game), and is no longer. Do NOT set this when unit is converted ! Be careful with temp units too (negative IDs)
	long int unitId; // same as pUnit->unitInstanceId, -1 if not set/not known.
	STRUCT_UNIT_BASE *pUnit; // pointer to actual unit structure

	// Index of "infAI list" element in other players' infAI.
	// -1 if index is still unknown: unit may not be known in infAI elem list OR this index has not been retrieved yet
	long int myIndexInOtherPlayerInfAIList[9];

	// Flag to indicate when "this" unit has been added to other players infAI lists, so that this treatment is run only once.
	bool hasBeenAddedToOtherPlayersInfAILists[9];

	// Reset/cleans up all data, including unit id/ptr. Call this once to initialize a new object.
	void Reset();

	// Init this object using provided unit.
	// Does nothing if "this" was already initialized (asserts that unit pointer IS the same as the one already known)
	void Init(STRUCT_UNIT_BASE *unit);

	// Init this object, considering the unit no longer exists (DEAD unit).
	// If "this" extension is already initialized, this will set the flag "unitDied" to true.
	void InitForDeadUnit(long int unitId);

	// Updates the specified infAI element with "this" unit information.
	// Returns true if successful
	bool WriteAllInfAIElemInfo(STRUCT_INF_AI_DETAILED_UNIT_INFO *elemToUpdate);

	// Updates some infAI element information with "this" unit information: position, owner playerId...
	// Returns true if successful
	bool UpdateInfAIElemInfo(STRUCT_INF_AI_DETAILED_UNIT_INFO *elemToUpdate);

};



}
