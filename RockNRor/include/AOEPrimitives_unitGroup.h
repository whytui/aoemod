
#pragma once

#include <assert.h>
#include <string>
#include <AOE_struct_units.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_tac_ai.h>
#include <AOE_struct_inf_ai.h>
#include <AOE_struct_main_ai.h>
#include <AOE_struct_unit_group.h>
#include <AOE_struct_game_settings.h>
#include "mainStructuresHandling.h"

/* This file contains AOE primitives about unit groups handling */

using namespace AOE_STRUCTURES;

namespace AOE_METHODS {
	namespace UNIT_GROUP {
;


// Create a unit group with 1 unit (leader)
STRUCT_UNIT_GROUP *CreateUnitGroup(STRUCT_AI *mainAI, AOE_CONST_INTERNAL::UNIT_GROUP_TYPES groupType, STRUCT_UNIT_BASE *leader);

bool RemoveUnitFromGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_AI *mainAI, long int unitId);

bool AddUnitToGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_AI *mainAI, long int unitId);

// Sets a unit group's new leader.
bool SetNewGroupLeader(STRUCT_UNIT_GROUP *unitGroup, STRUCT_AI *mainAI, long int unitId);

// Find unit's unit group, if any. NULL if not found
STRUCT_UNIT_GROUP *FindUnitGroup(STRUCT_UNIT_BASE *unit);


// Apply a task a unit group (will probably give commands to units)
// Warning: this does NOT set unitGroup.currentTask/resetOrg, you have to set it before (and set target, ..., if necessary)
// If force=true, give the order even for units that are busy with a non-wall attack
bool ApplyTaskToUnitGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI *tacAI, AOE_CONST_INTERNAL::UNIT_GROUP_TASK_IDS taskId,
	long int resetOrg, bool force);

}
}
