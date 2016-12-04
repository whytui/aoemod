
#pragma once

#include <assert.h>
#include <string>
#include <AOE_struct_units.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_game_settings.h>
#include "AOEPrimitives_units.h"
#include "AOE_map.h"
#include "unitDefHandling.h"
#include "mainStructuresHandling.h"

/* This file contains primitives about units handling */


namespace AOE_STRUCTURES {

// Securely get an action pointer without having to re-write all checks/gets for intermediate objects.
// Return NULL if one of the objects is NULL/missing
AOE_STRUCTURES::STRUCT_ACTION_BASE *GetUnitAction(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

// Return the total remaining food amount for a farm ("immediatly available" + "action-remaining").
// Returns 0 in error cases (please check it is actually a farm !)
float GetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit);

// Modifies the total remaining food amount for a farm ("immediately available" + "action-remaining").
bool SetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit, float newAmount);

// Returns true if unit is currently fighting against (or moving towards) a MILITARY target (excluding houses, villagers, etc)
// Returns false if target is not visible or out of reach
bool HasVisibleMilitaryTarget(STRUCT_UNIT_BASE *unit);

// Returns current action target IF it is an attack action (including conversion)
STRUCT_UNIT_BASE *GetAttackTarget(STRUCT_UNIT_BASE *unit);

// Return the target position for a unit/action
// The result might be {-1, -1}
std::pair<float, float> GetActionTargetPosition(STRUCT_ACTION_BASE *action);
std::pair<float, float> GetActionTargetPosition(STRUCT_UNIT_BASE *unit);

}
