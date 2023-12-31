#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_unit_group.h>
#include "AOE_time.h"
#include "AOEPrimitives_units.h"
#include "AOEPrimitives_unitGroup.h"
#include "randomizer.h"
#include "RockNRorInfo.h"
#include "customAIMilitaryInfo.h"
#include "unitHandling.h"
#include "combatAI.h"
#include "UnitExtensionHandler.h"
#include "Serializable.h"


using namespace AOE_STRUCTURES;

#define UNITGROUP_AI_LAST_TASK_TRACE_COUNT 5

namespace CUSTOM_AI {


	// A temporary structure to store current detailed info about a unit group. Not stored in "static" object.
	class UnitGroupDetailedInfo {
	public:
		UnitGroupDetailedInfo() {
			this->ResetAllInfo();
		}
		int validUnitsCount; // Number of units in group that could actually be retrieved
		int badDefenderUnitsCount; // Number of units that are not relevant for defending
		int notAttackingUnitsCount; // Number of units that are not currently attacking an enemy
		int unitsAttackingNonPriorityTarget; // Number of units that are attacking a non-priority target (=units that can't attack/defend itself)
		bool unitGroupIsAlmostIdle;
		long int unitIdAttackingMyLeader; // Unit ID (or -1) of the unit that is attacking the group's leader
		std::list<long int> unitsAttackingMyGroup; // List of unit IDs that are attacking some members of the group. Does NOT include ALL units attacking my group !

		// Number of units (in group) that are under attack. This information is not 100% reliable: if my units is attacking a (nontower) building, we can't know if it is being attacked itself.
		int GetNumberOfUnitsUnderAttack() { return this->unitsAttackingNonPriorityTarget; }

		void ResetAllInfo() {
			this->badDefenderUnitsCount = 0;
			this->notAttackingUnitsCount = 0;
			this->unitGroupIsAlmostIdle = false;
			this->unitsAttackingNonPriorityTarget = 0;
			this->validUnitsCount = 0;
			this->unitIdAttackingMyLeader = -1;
			this->unitsAttackingMyGroup.clear();
		}
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
		// Being true means that myMainCentralUnit is non-NULL
		bool mainCentralUnitIsVital;
		long int mainUnitSearchZoneRadius;
		long int mainUnitProtectionRadius;
		std::list<STRUCT_UNIT_MEMORY *> enemiesNearMyMainUnit; // A list of enemies close to my main unit, valued from InfAI list.
		STRUCT_UNIT_BASE *enemyUnitNearMyMainUnit = NULL; // Enemy unit that is located near my 'main central unit'. Can be NULL.
		STRUCT_UNIT_MEMORY *enemyUnitNearMyMainUnitInfAIMemoryElem = NULL; // infAI elem of enemy unit near my main central unit. Can be NULL
		bool enemyUnitNearMyMainUnitIsCurrentlyVisible; // True if enemyUnitNearMyMainUnit is currently visible

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
			this->enemyUnitNearMyMainUnit = NULL;
			this->enemyUnitNearMyMainUnitInfAIMemoryElem = NULL;
			this->enemyUnitNearMyMainUnitIsCurrentlyVisible = false;
			this->mainUnitProtectionRadius = 5;
			this->mainUnitSearchZoneRadius = 5;
			this->enemiesNearMyMainUnit.clear();
		}

		// Returns true if I have been attacked recently
		inline bool BeenAttackedRecently() const { return (this->totalAttacksInPeriod > 0) || (this->totalPanicModesInPeriod > 0); }
	};


	// A "static" (has "current game" life span) object to handle unit groups AI.
	class UnitGroupAI : public Serializable {
	public:
		UnitGroupAI();

		CustomAIMilitaryInfo *militaryAIInfo;
		GAME_DIFFICULTY_LEVEL gameDiffLevel; // Game difficulty level (copied from game settings at game init)
		std::string lastDebugInfo;
		// The last 5 "unit group tasking" operation executed by RockNRor's unit group AI.
		// Tuple: 1=unitGroupId, 2=taskId (-1 if not tasked, just target change), 3=targetId, 4=gameTime_ms
		std::list<std::tuple<long int, long int, long int, long int>> last5TaskingByUGAI;

		void ResetAllInfo();

		long int Serialize(FILE *outputFile) const override;
		bool Deserialize(FILE *inputFile) override;

		void AddTaskingByUGAITrace(long int unitGroupId, long int taskId, long int targetId, long int gameTime_ms);

		// Sets unitGroup target* fields from provided target unit. Does not set task or run any treatment.
		// targetUnit can be NULL.
		// Logs an entry in last5TaskingByUGAI.
		void SetUnitGroupTarget(STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *targetUnit);

		// Computes internal variables about military situation
		// This method should perform few operations to be quite fast
		void EvaluateMilitarySituation(STRUCT_TAC_AI *tacAI);

		// Set unitGroup.currentTask and call ApplyTask for the group (creates underlying unit commands, etc).
		// unitGroup.lastUpdateTime_ms is updated
		// unitGroup->lastAttackTaskingTime_ms is updated if taskId is an attack task.
		void SetUnitGroupCurrentTask(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup, UNIT_GROUP_TASK_IDS taskId,
			long int resetOrg, bool force);

		// Attack a target or approaches a zone to defend/attack. Updates unitGroup->lastTaskingTime_ms if tasked.
		// Updates unitGroup->lastAttackTaskingTime_ms is an attack task is assigned.
		// If target is not found and no default retreat position if provided (-1), the group is NOT tasked
		// Returns the used task id.
		UNIT_GROUP_TASK_IDS AttackOrRetreat(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_MEMORY *targetInfo,
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

		// Triggered when a member of unit group is being attacked. Does nothing if unitGroup==NULL.
		void OnUnitGroupAttacked(STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *myUnit, STRUCT_UNIT_BASE *enemyUnit);

	private:
		// Sets unitGroup target* fields from provided target unit. Does not set task or run any treatment.
		// targetUnit can be NULL.
		// Does not add an entry in last5TaskingByUGAI
		// Returns true if successful
		bool SetUnitGroupTarget_internal(STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *targetUnit);


		// "static" temporary information, only valid during the task active soldiers loop.
		// DO NOT access it from other methods.
		ActiveUnitGroupTaskingContextInfo activeGroupsTaskingTempInfo;
		float curGroupDistanceToMainUnit; // Distance between group and "my main" unit. Only relevant in "task active solider" methods
		float curUnitGroupOrientationXFromMainUnit = 0; // -1 if group is "left" of main unit, 1 if on the "right" (X axis)
		float curUnitGroupOrientationYFromMainUnit = 0; // -1 if group is "down" of main unit, 1 if "upper" (Y axis)

		// Task an active explore group
		// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
		bool TaskActiveExploreGroup(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup);

		// Task an active attack group, when player situation is not critical/weak.
		// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
		bool TaskActiveAttackGroup(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup);

		// Task an active attack group, when player situation is critical AND there is a vital central unit (like TC, villager).
		// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
		// The main idea in this case is to retreat at all costs and protect "main" unit
		bool TaskActiveAttackGroupCriticalWithVitalMainUnit(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup);

		// Task an active attack group, when player situation is "weak" AND there is a vital central unit (like TC, villager).
		// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
		bool TaskActiveAttackGroupWeakWithVitalMainUnit(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup);

		// Collects info on group and sets UnitGroupDetailedInfo fields
		void CollectInfoAboutGroup(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup, UnitGroupDetailedInfo *outputInfos);

		// Collects info on enemy units near "main central unit"
		// Computes "building in my town" fields in military AI
		void CollectEnemyUnitsNearMyMainUnit(STRUCT_PLAYER *player);

		// Returns the element from "list of enemies near my main unit" that is closest to a specified position.
		STRUCT_UNIT_MEMORY *GetUnitMemoryElemEnemyUnitCloserToPosition(STRUCT_PLAYER *player, long int posX, long int posY);


	};

}
