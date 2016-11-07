
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

bool RemoveUnitFromGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_AI *mainAI, STRUCT_UNIT_BASE *unit);

bool AddUnitToGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_AI *mainAI, STRUCT_UNIT_BASE *unit);

// Task a unit group (will probably give commands to units)
bool TaskUnitGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI *tacAI, AOE_CONST_INTERNAL::UNIT_GROUP_TASK_IDS taskId, long int resetOrg, bool force);

}
}
