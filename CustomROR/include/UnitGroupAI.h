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


	// Represents common (temporary) information about active unit group tasking
	// Such data is only valid during the "task active unit groups" loop.
	class ActiveUnitGroupTaskingContextInfo {
	public:
		ActiveUnitGroupTaskingContextInfo() {
			this->ResetAllInfo();
		}
		long int processingStartTimeGetTime; // TimeGetTime value of processing start time (provided by ROR code)
		long int allowedTime; // Time in millisecond we are allowed to spend from processingStartTimeGetTime (provided by ROR code)
		// My town center, or another "main" unit to consider as "main town unit" if no TC was found
		STRUCT_UNIT_BASE *myMainCentralUnit;
		int myWeaknessScore; // myWeaknessScore = 100 if I am weak
		int totalAttacksInPeriod;
		int totalPanicModesInPeriod;
		int totalLandAttackGroupCount;
		int totalLandDefendGroupCount;
		int totalLandExploreGroupCount;
		int townLandAttackGroupCount;
		int townLandDefendGroupCount;
		int townLandExploreGroupCount;
		// mainCentralUnitIsVital determines if "central" unit is TC or a unit that allows survival (villager, priest).
		// This information can be useful to handle well scenario situations (AI should be able to attack in scenarios when it doesn't have villagers, etc)
		bool mainCentralUnitIsVital;

		void ResetAllInfo() {
			this->processingStartTimeGetTime = 0;
			this->allowedTime = 0;
			this->myMainCentralUnit = NULL;
			this->myWeaknessScore = 0;
			this->totalAttacksInPeriod = 0;
			this->totalPanicModesInPeriod = 0;
			this->totalLandAttackGroupCount = 0;
			this->totalLandDefendGroupCount = 0;
			this->totalLandExploreGroupCount = 0;
			this->townLandAttackGroupCount = 0;
			this->townLandDefendGroupCount = 0;
			this->townLandExploreGroupCount = 0;
			this->mainCentralUnitIsVital = false;
		}
	};

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
			float defaultRetreatPosX, float defaultRetreatPosY, bool forceTasking);

		// Called once before the "task active groups loop on unit groups". Can be used for initializations, common treatments, etc.
		// This method is always called (once) just before the TaskActiveUnitGroup calling series.
		// Returns true if the "loop" on unit group can be processed (normal case)
		// Returns false to disable all "Task active soldiers" treatments (for this occurrence only)
		bool OnTaskActiveGroupsBegin(STRUCT_TAC_AI *tacAI, long int processStartTimeGetTimeValue, long int allowedTime);

		// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
		// For all non-supported (or without custom treatment) cases, just return false !
		// Note: OnTaskActiveGroupsBegin has been called once before calling this (n' times)
		bool TaskActiveUnitGroup(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup);

	private:
		// "static" temporary information, only valid during the task active soliders loop.
		// DO NOT access it from other methods.
		ActiveUnitGroupTaskingContextInfo activeGroupsTaskingTempInfo;

	};

}
