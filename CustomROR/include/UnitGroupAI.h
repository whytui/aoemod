#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_unit_group.h>
#include "AOEPrimitives_units.h"
#include "AOEPrimitives_unitGroup.h"
#include "randomizer.h"
#include "CustomRORInfo.h"
#include "customAIMilitaryInfo.h"


using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	class UnitGroupAI {
	public:
		UnitGroupAI();

		CustomAIMilitaryInfo *militaryAIInfo;

		void ResetAllInfo();

		// Set unitGroup.currentTask and call ApplyTask for the group (creates underlying unit commands, etc).
		void SetUnitGroupCurrentTask(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup, UNIT_GROUP_TASK_IDS taskId,
			long int resetOrg, bool force);

		// Attack a target or use retreat to approach a zone to defend/attack
		// If target is not found and no default retreat position if provided (-1), the group is NOT tasked
		// Returns the used task id.
		UNIT_GROUP_TASK_IDS AttackOrRetreat(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_INF_AI_UNIT_LIST_ELEM *targetInfo,
			float defaultRetreatPosX, float defaultRetreatPosY);

		// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
		// For all non-supported (or without custom treatment) cases, just return false !
		bool TaskActiveUnitGroup(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup);


	};

}
