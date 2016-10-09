
#pragma once

#include <assert.h>
#include <string>
#include <AOE_struct_units.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_tac_ai.h>
#include <AOE_struct_inf_ai.h>
#include <AOE_struct_game_settings.h>
#include "mainStructuresHandling.h"

/* This file contains AOE primitives about units handling */

using namespace AOE_STRUCTURES;

namespace AOE_METHODS {

// Exact role to confirm.
// MAYBE this method allows finding path with enemy units blocking the way. Such units are added to path finding struct's unitid array (unknown_11DCE4) ?
// arg6: seen 0x1B (hardcoded)
// tempList should be empty in input, filled by this method. Please free the array (if non-NULL) afterwards !
// Call [unit+0x1A0], for example 0x44E130.
bool MoveToTarget_1A0(STRUCT_UNIT_BASE *unit, long int targetUnitId, float range, long int arg3,
	long int arg4, long int targetPlayerId, long int arg6, STRUCT_AI_UNIT_LIST_INFO *tempList);


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

// Securely get an action pointer without having to re-write all checks/gets for intermediate objects.
// Return NULL if one of the objects is NULL/missing
AOE_STRUCTURES::STRUCT_ACTION_BASE *GetUnitAction(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

// Return the total remaining food amount for a farm ("immediatly available" + "action-remaining").
// Returns 0 in error cases (please check it is actually a farm !)
float GetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit);

// Modifies the total remaining food amount for a farm ("immediately available" + "action-remaining").
bool SetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit, float newAmount);

}
