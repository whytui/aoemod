
#pragma once

#include <assert.h>
#include <AOE_empires_dat.h>
#include <AOE_const_internal.h>
#include <AOE_struct_common_ai_object.h>
#include <AOE_struct_ai_unit_list_info.h>
#include <AOE_struct_strategy_element.h>
#include <AOE_struct_unit_group.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {
	// External dependencies
	class STRUCT_AI;



	// Size = 0x18
	// For gatheres tasks (not builder/repairman/explorer?)
	class STRUCT_VILLAGER_TASKS_ELEM {
	public:
		long int unitId; // Villager unit id
		long int resourceType; // 0-3
		long int maxResourceAmount; // resourceAmount is compared to resourceAmount to known is gathering is "finished" (needs to deposit)
		long int resourceAmount; // +0C. Gathered amount at this point (compared to +08)
		// 0x10
		long int targetUnitId; // building to build, gazelle to hunt, etc
		unsigned long int timeGetTimeValue; // Last evaluation system time in milliseconds. Task changes are evaluated if > 5 seconds ?

		AOE_CONST_FUNC::RESOURCE_TYPES GetResourceType() { return (AOE_CONST_FUNC::RESOURCE_TYPES)this->resourceType; };
	};


	// Size = 0x70
	// This struct is included in TacAI
	class STRUCT_PLANNED_RESOURCE_NEEDS {
	public:
		unsigned long int checksum; // 54 2C 54 00
		unsigned long int unknown_04; // a dword
		unsigned long int unknown_08; // a dword
		long int resourceAmount[8]; // +0C. Only 4 are used, but there's room for 8 resource types. Index is resourceId.
		long int unknown_2C[8]; // Only 4 are used, but there's room for 8 resource types. Init to -1 (a resource type?)
		long int unknown_4C[8]; // Only 4 are used, but there's room for 8 resource types. Init to -1 (a resource type?)
		long int resourceTypesCount; // +0x6C ; is always 4

		bool IsCheckSumValid() { return this->checksum == 0x00542C54; }
	};
	static_assert(sizeof(STRUCT_PLANNED_RESOURCE_NEEDS) == 0x70, "STRUCT_PLANNED_RESOURCE_NEEDS size");


	// Included in TacAI (+E4C). Temporary info about building currently analyzed for being built.
	// Size=0x130
	class STRUCT_TAC_AI_BUILD_TO_TRIGGER {
	public:
		long int unitDefId; // DAT_ID of currently analyzed building
		float unknown_04; // SizeRadius1+SizeRadius2 ?
		char bldName_ToBuild[0x100]; // +8. Set (copied) in 0x4BE9C5.
		long int actorUnitId; // +108.
		unsigned long int minDistanceToTC; // +10C.
		unsigned long int maxDistanceToTC; // +110.
		long int infoIsValid; // +114. Flag 0/1. See 4BE350. 1 when information is valid (saved for a next AI treatment loop because it could not be completed last time)
		long int buildSitePosY; // +118.
		long int buildSitePosX; // +11C
		long int buildSiteScore; // +120. Score from "temp map like" values
		long int unknown_124; // minPosY ?????
		long int randomPlacementFactor; // To confirm
		STRUCT_STRATEGY_ELEMENT *strategyElement;
	};
	static_assert(sizeof(STRUCT_TAC_AI_BUILD_TO_TRIGGER) == 0x130, "STRUCT_TAC_AI_BUILD_TO_TRIGGER size");


	// Included in tacAI structure (+FAC). Size=0x2C.
	class STRUCT_TAC_AI_TARGET_INFO {
	public:
		long int targetUnitId;
		float targetEvaluation; // +04. According to SNTargetEvaluationxxx numbers
		long int targetInfAIUnitElemListIndex; // +08. infAIUnitElemListIndex corresponding to (main) target ?
		long int currentSearchInfAIUnitElemListIndex; // +0C. Index of current search element in infAI.unitListElem ? "Previous" elements in infAI.unitListElem are ignored ?
		// 0x10
		long int unknown_10; // default -1
		long int unknown_14; // default -1
		char currentSearchIsBuildings; // +18. default 0. Search building targets ? See 4C0ABC, 4C0E1A (search buildings if no living found?)
		char unused_19[3];
		long int buildingTargetUnitId; // +1C. Target buiding id (secondary target ?)
		// 0x20
		float buildingTargetEvaluation; // +20. Target evaluation for building target.
		long int buildingTargetInfAIUnitElemListIndex; // +24. Target building infAI list index ?
		long int targetSearchInProgress; // +28.
	};
	static_assert(sizeof(STRUCT_TAC_AI_TARGET_INFO) == 0x2C, "STRUCT_TAC_AI_TARGET_INFO size");


	// Tactical AI. Lots of things are managed here !
	// size 0x100C - Constructor=0x4CEBE0, Deserialize=0x4CEF10. Corresponds to MainAI+0x1146C
	// [+0x30] mainUpdate(arg1 = always 0 ?). "Scaling" update, tributes/dipl/likes, runs tacAIUpdate(strategy, military, gathering... cf AI_UPDATE_TYPES). Called by 0x4C71A0=main AI update
	class STRUCT_TAC_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=?
		// 0xF0
		STRUCT_AI *ptrMainAI;
		STRUCT_AI_UNIT_LIST_INFO allVillagers; // +F4. Array can contain empty values (-1). Does NOT contain any boat ! 
		//long int *ptrAllVillagersUnitIdArray; // Array of unitIDs (all villagers). May contain empty values (-1) ?
		//long int myVillagerCount; // +F8. exact count of villagers in ptrAllVillagersArray
		//unsigned long int unknown_0FC;
		// 0x100
		//long int allVillagersArraySize; // Size for ptrAllVillagersArray. Can be higher than exact count (if empty spaces).
		STRUCT_AI_UNIT_LIST_INFO villagerExplorers; // +104.
		STRUCT_AI_UNIT_LIST_INFO landMilitaryUnits; // +114. military unitIds that do NOT belong to a unitGroup. Contains military unit (cf AI type - 0x4D277A), EXCLUDING towers, boats
		STRUCT_AI_UNIT_LIST_INFO unknown_124; // NOT SURE it is a STRUCT_AI_UNIT_LIST_INFO
		STRUCT_AI_UNIT_LIST_INFO allBoats; // +134. Includes trade/fishing/transport/war boat AI type units.
		STRUCT_AI_UNIT_LIST_INFO warShips; // +144
		STRUCT_AI_UNIT_LIST_INFO fishingShips; // +154
		STRUCT_AI_UNIT_LIST_INFO tradeShips; // +164
		STRUCT_AI_UNIT_LIST_INFO transportShips; // +174
		STRUCT_AI_UNIT_LIST_INFO relics; // +184. Relics only ? What is sure is that is does NOT include ruins.
		long int SNNumber[0xE2]; // +194. In game, we use this one (not strategyAI's). Total size=0x388 (4*0xE2). index from 0 to E1. (missing 1 SN?)
		// 0x51C
		// Here: a list of small structures size=0x18 (0x32 elems?) = gatherers and their tasks (excluding explorers). builders/repairmen NOT in this array?
		AOE_STRUCTURES::STRUCT_VILLAGER_TASKS_ELEM gatherersTasks[0x32];
		unsigned long int villagerTasksRelevantElemCount; // +9CC. Number of RELEVANT element count in villagerTasks array (array size is fixed, it's included in this structure !)
		// 0x9D0
		long int nonExplorerVillagersCount; // +9D0. Counts all villagers BUT explorers
		AOE_STRUCTURES::STRUCT_UNIT_GROUP fakeFirstUnitGroupElem; // +9D4. Organized as a circular list (each elem is a group) ? This one = "fake" elem (like in strategy)
		unsigned long int seqUnitGroupId; // +D04. Is = next group's ID.
		unsigned long int unitGroupsCount; // +D08. This does NOT count fakeFirstUnitGroupElem (so it CAN be 0).
		unsigned long int lastAttackTime_ms; // +D0C. Last attack time (not response time, unlesse SNLockAttackAndAttackResponse=1). Set in 0x4D457B.
		// 0xD10
		long int unknown_D10_ms; // +D10. A time value in ms
		unsigned long int lastAttackResponseTime_ms; // +D14. Value in milliseconds. Used to compute +D24 ?
		unsigned long int unknown_D18;
		long int lastScalingUpdate; // +D1C. Value in milliseconds. See customROR config (tacticalAI/updateDelay) or SNScalingFrequency in standard game.
		// 0xD20
		long int buildFrequencyCounter; // incremented until it reaches SNBuildFrequency
		long int currentAttackSeparationTime_seconds; // +D24. Compared to SNAttackResponseSeparationTime,SNAttackSeparationTimeRandomness. See 4E0BC0,4E0C03.
		STRUCT_AI_UNIT_LIST_INFO targetPlayers; // +D28. Contains target player IDs, ordered by priority (?) SEEMS TO BE WHAT DEFINES WHO TO ATTACK? Always only 1 element (sometimes hardcoded?4D622C)
		STRUCT_AI_UNIT_LIST_INFO likedPlayers; // +D38. allied playerIds ? "myself" IS in list ! In practise, always 2 elements? (me + most liked player) ?
		STRUCT_AI_UNIT_LIST_INFO IdleOrInRangeMilitaryUnits; // +D48. available military units (idle OR in range from TC?) for temp treatments ? See 4D8960
		STRUCT_AI_UNIT_LIST_INFO unknownUnits_D58; // TC + villagers ? + others? Builders ? explorers (can be villagers AND military) ? Unit that defend something ?
		unsigned long int gathererCount_actual[4]; // +D68. Index IS resource ID.
		long int gathererCount_desired[AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT]; // +D78. Villager count we want to assign to each type of resource (4). Index IS resource Type ID
		long int extraResourceTypeNeededByPriority[4]; // +D88 First=most needed. Use extraResourceAmount with this index to get resource amount. <0 amount = I must collect
		long int extraResourceAmount[4]; // +D98. Index is always the same (0=food, 1=food...) <0 amount = I need to collect some. "resDiff"
		STRUCT_PLANNED_RESOURCE_NEEDS plannedResourceNeeds; // +DA8, size=0x70. Related to //SNMostNeededResourceLookAhead (number of stratElem to take into account)
		// 0xE18
		long int attackEnabled; // +E18. Starts with 0, Once set to 1, AI can attack.
		AOE_CONST_INTERNAL::AI_UPDATE_TYPES currentAIUpdateType; // +E1C. loop ID for TacAI treatments types.
		// 0xE20
		long int unknown_E20; // a dword, a flag - related to villagers/gathering ?
		char unknown_E24[0xE44 - 0xE24];
		long int myWonderBeingBuiltFlag; // +E44. true(1) or false(0) value.
		long int myWonderIsBuiltFlag; // +E48. true(1) or false(0) value.
		STRUCT_TAC_AI_BUILD_TO_TRIGGER constructionToTrigger; // E4C : an included temporary struct (=>F78 or F80?) about building currently analyzed for being built. +0=DATID to build. +108=actorUnitId?
		long int currentAIUpdateVillagerId; // Currently evaluated villager unitId (from allVillagers array). Set to -1 when I have no villager or out of loop ?
		long int unknown_F80; // +F80. signed dword. A group id ? Last group id that was tasked ?
		long int lastProcessedUnitGroupId; // +F84. a unit group ID. 4D1CF7,4D3ACB. Current group for treatments(just technical, to continue work when has no time to finish?)
		long int storagePitAddedToStrategy; // +F88. 1 when a SP has been inserted into strategy.
		long int granaryAddedToStrategy; // +F8C. 1 when a granary has been inserted into strategy.
		// 0xF90
		long int unknown_F90_someTimeValue; // 4D2245,4D242A...
		long int lastPanicModeStrategyUpdateTime; // +F94. In milliseconds. Updated with game time when "panic mode" units are added to strategy.
		long int unknown_F98_timeAboutTributes; // Related to SNCoopDemandTributeInterval
		unsigned long int unknown_F9C;
		// 0xFA0
		long int lastCoopSharAttackTime_ms; // Only used if SNCoopShareAttacking=1 (0x4E2CD7). In milliseconds. Updated when an group is tasked to attack some target?
		unsigned long int unknown_FA4; // flag ? about exploration ? Used in gatherer affectation methods
		unsigned long int unknown_FA8;
		STRUCT_TAC_AI_TARGET_INFO targetInfo; // +FAC. This is only used in "group.FindAttackTarget". Values only make sense when inProgress=1 (unused otherwise).
		long int attacksByPlayerCount[9]; // +FD8. number of times this player attacked me. Updated in 0x4D7AF0 (tacAI.reactToEvent 0x201), never used in standard code ?
		long int lastUAms; // +FFC. "LastUAms" (UpdateAI). Time spent in last updateAI execution
		long int averageUpdateAITime_ms; // +1000. Updated each time unknown_1004_time_counter reaches 20 and is reset. "Average UAms" ?
		long int calcAverageUpdateAITime_counter; // Maximum value = 20 = 0x14
		long int lastAIUpdateTime_ms; // +1008. A timeGetTime value. (not a game time)

		bool IsCheckSumValid() { return this->checksum == 0x00548CF4; }
	};
	static_assert(sizeof(STRUCT_TAC_AI) == 0x100C, "STRUCT_TAC_AI size");


}
