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

		// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
		// For all non-supported (or without custom treatment) cases, just return false !
		bool TaskActiveUnitGroup(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup);


	};

}
