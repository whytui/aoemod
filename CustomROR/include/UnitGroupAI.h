#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_unit_group.h>
#include "AOE_time.h"
#include "AOEPrimitives_units.h"
#include "AOEPrimitives_unitGroup.h"
#include "randomizer.h"
#include "CustomRORInfo.h"
#include "customAIMilitaryInfo.h"


using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	enum MILITARY_SITUATION { MS_UNKNOWN = -1,
		MS_CRITICAL, // {few resources and few military and recent panic modes} or {very few resources and recently attacked}
		MS_WEAK, // {recent panic modes} or {very few resources}
		MS_NORMAL, 
		MS_STRONG // Large military numbers, with at least 2 land (military) unit groups
	};

	// Represents common (temporary) information about active unit group tasking
	// Such data is only valid during the "task active unit groups" loop.
	class ActiveUnitGroupTaskingContextInfo {
	public:
		ActiveUnitGroupTaskingContextInfo() {
			this->ResetAllInfo();
		}
		long int processingStartTimeGetTime; // TimeGetTime value of processing start time (provided by ROR code)
		long int allowedTime; // Time in millisecond we are allowed to spend from processingStartTimeGetTime (provided by ROR code)
		MILITARY_SITUATION militarySituation;
		// My town center, or another "main" unit to consider as "main town unit" if no TC was found
		STRUCT_UNIT_BASE *myMainCentralUnit;
		int myTotalMilitaryUnitCount;
		int myWeaknessScore; // myWeaknessScore = 100 if I am weak
		int totalAttacksInPeriod; // Number of enemy attacks in last AI_CONST::delayInWhichEnemyAttacksImpactUnitGroupTasking milliseconds
		int totalAttacksInTownInPeriod; // Number of enemy attacks in my town in last AI_CONST::delayInWhichEnemyAttacksImpactUnitGroupTasking milliseconds
		int totalPanicModesInPeriod; // Number of triggered panic modes in last AI_CONST::delayInWhichEnemyAttacksImpactUnitGroupTasking milliseconds
		int totalLandAttackGroupCount;
		int totalLandDefendGroupCount;
		int totalLandExploreGroupCount;
		int townLandAttackGroupCount;
		int townLandDefendGroupCount;
		int townLandExploreGroupCount;
		int totalLandGroups; // Sum of land military groups, excluding artefacts
		// mainCentralUnitIsVital determines if "central" unit is TC or a unit that allows survival (villager, priest).
		// If false, player has no villager, TC, etc, and may adopt a more aggressive attitude (nothing to lose) ; especially in scenarios
		bool mainCentralUnitIsVital;

		void ResetAllInfo() {
			this->processingStartTimeGetTime = 0;
			this->allowedTime = 0;
			this->militarySituation = MILITARY_SITUATION::MS_UNKNOWN;
			this->myMainCentralUnit = NULL;
			this->myTotalMilitaryUnitCount = 0;
			this->myWeaknessScore = 0;
			this->totalAttacksInPeriod = 0;
			this->totalAttacksInTownInPeriod = 0;
			this->totalPanicModesInPeriod = 0;
			this->totalLandAttackGroupCount = 0;
			this->totalLandDefendGroupCount = 0;
			this->totalLandExploreGroupCount = 0;
			this->totalLandGroups = 0;
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
		GAME_DIFFICULTY_LEVEL gameDiffLevel; // Game difficulty level (copied from game settings at game init)
		std::string lastDebugInfo;

		void ResetAllInfo();

		// Computes internal variables about military situation
		// This method should perform few operations to be quite fast
		void EvaluateMilitarySituation(STRUCT_TAC_AI *tacAI);

		// Set unitGroup.currentTask and call ApplyTask for the group (creates underlying unit commands, etc).
		// unitGroup.lastTaskingTime_ms is updated
		void SetUnitGroupCurrentTask(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup, UNIT_GROUP_TASK_IDS taskId,
			long int resetOrg, bool force);

		// Attack a target or use retreat to approach a zone to defend/attack. Updates unitGroup->lastTaskingTime_ms if tasked.
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

		// Task an active attack group, when player situation is not critical/weak.
		// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
		bool TaskActiveAttackGroup(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup);
	};

}
