
#pragma once

#include <assert.h>
#include <string>
#include <AOE_struct_managed_array.h>
#include <AOE_struct_units.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_tac_ai.h>
#include <AOE_struct_inf_ai.h>
#include <AOE_struct_game_settings.h>
#include "AOE_memory.h"
#include "mainStructuresHandling.h"

/* This file contains AOE primitives about units handling */

using namespace AOE_STRUCTURES;

namespace AOE_METHODS {
namespace UNIT {
;

// Exact role to confirm.
// MAYBE this method allows finding path with enemy units blocking the way. Such units are added to path finding struct's unitid array (unknown_11DCE4) ?
// arg6: seen 0x1B (hardcoded)
// tempList should be empty in input, filled by this method. Please free the array (if non-NULL) afterwards !
// Call [unit+0x1A0], for example 0x44E130.
bool MoveToTarget_1A0(STRUCT_UNIT_BASE *unit, long int targetUnitId, float range, long int arg3,
	long int arg4, long int targetPlayerId, long int arg6, STRUCT_MANAGED_ARRAY *tempList);


// Add a unit to all relevant infAI lists. Cf 0x4BE0D0
bool AddUnitInInfAILists(STRUCT_INF_AI *infAI, long int unitId);

// Returns true if a unit can convert another (actually, for priests)
bool CanConvert(STRUCT_UNIT_BASE *unit, long int targetUnitId);

// Gets the damage from an attacked over a target (defender)
float CalcDamage(STRUCT_UNIT_BASE *attacker, STRUCT_UNIT_BASE *defender);

// All unit-classes-compatible getter for reload time.
float GetReloadTime1(STRUCT_UNIT_BASE *unit);

// All unit-classes-compatible getter for speed.
float GetSpeed(STRUCT_UNIT_BASE *unit);

bool IsUnitIdle(STRUCT_UNIT_BASE *unit);

// Make progress in building construction according to timeToAdd (in seconds)
void BuildingUnitDoConstruct(STRUCT_UNIT_BUILDING *building, float timeToAdd_seconds);

// Create unit activity using derived class that corresponds to supplied checksum (calls the correct constructor)
// Any unit (even base type) can have an activity, even if ROR originally only does that for living&building.
// Returns true if successful.
bool CreateUnitActivity(STRUCT_UNIT_BASE *unit, unsigned long int activityClassChecksum);

// Add visibility for given unit to specified player (might NOT be unit owner)
// If useUnitSizeRadius, do not use distance but unit.unitDef.sizeRadius instead.
bool AddVisibility(STRUCT_UNIT_BASE *unit, STRUCT_PLAYER *playerToUpdate, bool useUnitSizeRadius, long int distance);

// Remove visibility for given unit from specified player (might NOT be unit owner)
// If useUnitSizeRadius, do not use distance but unit.unitDef.sizeRadius instead.
bool RemoveVisibility(STRUCT_UNIT_BASE *unit, STRUCT_PLAYER *playerToUpdate, bool useUnitSizeRadius, long int distance);

}

}
