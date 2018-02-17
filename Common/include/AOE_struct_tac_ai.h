
#pragma once

#include <assert.h>
#include <AOE_empires_dat.h>
#include <AOE_const_internal.h>
#include <AOE_struct_common_ai_object.h>
#include <AOE_struct_managed_array.h>
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
	// For gatherers tasks (not builder/repairman/explorer?)
	class STRUCT_VILLAGER_TASKS_ELEM {
	public:
		long int unitId; // Villager unit id
		long int resourceType; // 0-3
		long int maxResourceAmount; // resourceAmount is compared to resourceAmount to known is gathering is "finished" (needs to deposit)
		long int resourceAmount; // +0C. Gathered amount at this point (compared to +08)
		// 0x10
		long int targetUnitId; // building to build, gazelle to hunt, etc
		unsigned long int timeGetTimeValue; // Last evaluation system time in milliseconds. Task changes are evaluated if > 5 seconds (real time) cf 0x4DA38A

		AOE_CONST_FUNC::RESOURCE_TYPES GetResourceType() const { return (AOE_CONST_FUNC::RESOURCE_TYPES)this->resourceType; };
	};


#define CHECKSUM_PLANNED_RESOURCE_NEEDS 0x00542C54
	// Size = 0x70
	// This struct is included in TacAI+0xDA8
	// 0x40EC20 = plannedResourceNeeds.setResourceAmount(resourceId, amount)
	// 0x4B8960 = buildAI.computeCostsForNextStratElem(ptrPlannedResourceNeeds, stratElemCount) for next strategy element not already done/not in progress
	class STRUCT_PLANNED_RESOURCE_NEEDS {
	public:
		unsigned long int checksum; // 54 2C 54 00
		unsigned long int unknown_04; // a dword
		unsigned long int unknown_08; // a dword
		long int resourceAmount[8]; // +0C. Only 4 are used, but there's room for 8 resource types. Index=resourceTypeId(0-3, actually)
		long int unknown_2C[8]; // Only 4 are used, but there's room for 8 resource types. Init to -1 (a resource type?)
		long int unknown_4C[8]; // Only 4 are used, but there's room for 8 resource types. Init to -1 (a resource type?)
		long int resourceTypesCount; // +0x6C ; is always 4

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_PLANNED_RESOURCE_NEEDS; }
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

#define CHECKSUM_TAC_AI 0x00548CF4

	// Tactical AI. Lots of things are managed here !
	// size 0x100C - Constructor=0x4CEBE0, Deserialize=0x4CEF10. Corresponds to MainAI+0x1146C
	// +0x30 = mainUpdate(arg1 = always 0 ?). "Scaling" update, tributes/dipl/likes, runs tacAIUpdate(strategy, military, gathering... cf AI_UPDATE_TYPES). Called by 0x4C71A0=main AI update
	// +0x40 = tacAI.reactToEvent(unitId, arg2, eventId, arg4, arg5, arg6)
	class STRUCT_TAC_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=?
		// 0xF0
		STRUCT_AI *ptrMainAI;
		STRUCT_MANAGED_ARRAY allVillagers; // +F4. All *my* villagers. Array can contain empty values (-1). Does NOT contain any boat ! 
		STRUCT_MANAGED_ARRAY villagerExplorers; // +104. All my villagers tasked to exploration. Size=desired explorers count.
		STRUCT_MANAGED_ARRAY landMilitaryUnits; // +114. All my land military unitIds (cf AI type - 0x4D277A), EXCLUDING towers, boats
		STRUCT_MANAGED_ARRAY unknown_124; // NOT SURE it is a STRUCT_MANAGED_ARRAY
		STRUCT_MANAGED_ARRAY allBoats; // +134. All my boats, including trade/fishing/transport/war boat AI type units.
		STRUCT_MANAGED_ARRAY warShips; // +144. All my war ships.
		STRUCT_MANAGED_ARRAY fishingShips; // +154. All my fishing ships.
		STRUCT_MANAGED_ARRAY tradeShips; // +164. All my trade ships.
		STRUCT_MANAGED_ARRAY transportShips; // +174. All my transport ships.
		STRUCT_MANAGED_ARRAY relics; // +184. all my relics (only) ? What is sure is that it does NOT include ruins.
		long int SNNumber[0xE2]; // +194. In game, we use this one (not strategyAI's). Total size=0x388 (4*0xE2). index from 0 to E1. (missing 1 SN?)
		// 0x51C
		// Here: a list of small structures size=0x18 (0x32 elems) = gatherers and their tasks (excluding explorers).
		// This is used to restore gathering activity after repairing something, for example.
		// The array can have empty slots, used slots may not be consecutive. To search a specific unitId, you need to loop on all 50 entries. Eg 0x4E1870.
		AOE_STRUCTURES::STRUCT_VILLAGER_TASKS_ELEM gatherersTasks[0x32];
		unsigned long int villagerTasksRelevantElemCount; // +9CC. Number of RELEVANT element count in villagerTasks array (array size is fixed, it's included in this structure !) "villager gatherers".
		long int desiredGathererVillagersCount; // +9D0. Counts all villagers BUT explorers = "desired gatherers count" (also includes repairmen, etc)
		AOE_STRUCTURES::STRUCT_UNIT_GROUP fakeFirstUnitGroupElem; // +9D4. Organized as a circular list (each elem is a group) ? This one = "fake" elem (like in strategy)
		unsigned long int seqUnitGroupId; // +D04. Id = next group's ID.
		unsigned long int unitGroupsCount; // +D08. This does NOT count fakeFirstUnitGroupElem (so it CAN be 0).
		unsigned long int lastAttackTime_ms; // +D0C. Last attack time (not response time, unlesse SNLockAttackAndAttackResponse=1). Set in 0x4D457B.
		long int unknown_D10_ms; // +D10. A time value in ms. Last recomputation of unit groups ?
		unsigned long int lastAttackResponseTime_ms; // +D14. Value in milliseconds. Used to compute +D24 ?
		unsigned long int unknown_D18; // +D18.
		long int lastScalingUpdate; // +D1C. Value in milliseconds. See RockNRor config (tacticalAI/updateDelay) or SNScalingFrequency in standard game.
		long int buildFrequencyCounter; // +D20. incremented until it reaches SNBuildFrequency
		long int currentAttackSeparationTime_seconds; // +D24. Compared to SNAttackResponseSeparationTime,SNAttackSeparationTimeRandomness. See 4E0BC0,4E0C03.
		
		// +D28. "MostDislikedPlayers". Contains target player IDs, ordered by priority (?) SEEMS TO BE WHAT DEFINES WHO TO ATTACK?
		// In practice, there is always only 1 (or 0 at startup) element (hardcoded in several places like 0x4D622C)
		// This is set in 0x4D0980=tacAI.mainUpdate(arg1): set target from 0x40ACB0 call=diplAI.GetMostDislikedPlayerId(SNaskTributeAmount, SNaskTributePlayerId, bAttackWinningPlayer, attackWinningPlayerFactor)
		// This is used ("Get" only) in
		// - 0x4D3AB0=tacAI.TaskActiveSoldiers(timeGetTime, allowedtime).
		// - 0x4D6060=tacAI.doPlayAttacking ? (arg1, playerId).
		// - 0x4D7880=tacAI.reactToEvent(myUnitId, arg2_unitId, eventId, arg4, arg5, arg6) when event=0x201(attacked). (0x4D8309).
		// - 0x4E0A70=tacAI.GetTargetPlayerUnitCountInInfAIList() => from taskIdleSoldiers. 
		// - 0x4E3560=tacAI.attackTowerInMyTown(unitId)
		// If neutral and dislike>74, most disliked player becomes enemy.
		STRUCT_MANAGED_ARRAY mostDislikedPlayers;

		// +D38. Most liked player IDs. "myself" IS in list (0x4D0CA7) ! In practice, always 2 elements? (me + most liked player) ?
		// Contains 1 playerID ("me") when most liked=most disliked (e.g. in 2-players games).
		// Contains 2 playerIDs (me+most liked other player) in other situations
		// Re-filled in 0x4D0BF7. If neutral and like>74, most liked player becomes ally.
		STRUCT_MANAGED_ARRAY mostLikedPlayers;

		STRUCT_MANAGED_ARRAY tmpUnitIdArrayForGroupSetup; // +D48. available military units (idle OR in range from TC?) for temp treatments ? See 4D8960. Includes fishing,trade ships? Units without a group ?
		STRUCT_MANAGED_ARRAY unknownUnits_D58; // TC + villagers ? + others? Builders ? explorers (can be villagers AND military) ? Unit that defend something ? Trade ships also? Used when tasking? Temp ?
		unsigned long int gathererCount_actual[4]; // +D68. Index IS resource ID.
		long int gathererCount_desired[AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT]; // +D78. Villager count we want to assign to each type of resource (4). Index IS resource Type ID
		long int neededResourceTypesByPriority[4]; // +D88 First=most needed in extraResourceAmount. <0 amount = I must collect (needed). Eg 0x4D7760=tacAI.GetResourceNeedAmount(priorityRank)
		long int extraResourceAmount[4]; // +D98. Index is always the same (0=food, 1=food...) <0 amount = I need to collect some. "resDiff"
		STRUCT_PLANNED_RESOURCE_NEEDS plannedResourceNeeds; // +DA8, size=0x70. Related to //SNMostNeededResourceLookAhead (number of stratElem to take into account). "NeededResourceAmount".
		// 0xE18
		long int attackEnabled; // +E18. Starts with 0, Once set to 1, AI can attack. Set in 0x4E15F0.
		AOE_CONST_INTERNAL::AI_UPDATE_TYPES currentAIUpdateType; // +E1C. loop ID for TacAI treatments types.
		long int neededResourcesAreInitialized; // +E20. Related to villagers/gathering. 0 initially, set to 1 once needed resources by priority have been computed once.
		char unknown_E24[0xE44 - 0xE24];
		long int myWonderBeingBuiltFlag; // +E44. true(1) or false(0) value.
		long int myWonderIsBuiltFlag; // +E48. true(1) or false(0) value.
		STRUCT_TAC_AI_BUILD_TO_TRIGGER constructionToTrigger; // E4C : an included temporary struct (=>F78 or F80?) about building currently analyzed for being built. +0=DATID to build. +108=actorUnitId?
		long int currentAIUpdateVillagerId; // +F7C. Currently evaluated villager unitId (from allVillagers array). Set to -1 when I have no villager or out of loop ?
		long int unknown_F80; // +F80. signed dword. A group id ? Last group id that was tasked ?
		long int currentlyProcessedUnitGroupId; // +F84. Id of "next" group to process in active group tasking (just technical, to continue work when has not enough time to finish in 1 execution)
		long int storagePitAddedToStrategy; // +F88. 1 when a SP has been inserted into strategy.
		long int granaryAddedToStrategy; // +F8C. 1 when a granary has been inserted into strategy.
		long int lastConstructionStartOrAddToStrategyTime; // +F90. 4D2245,4D242A... Last game time when a construction was triggered or added to strategy ?
		long int lastPanicModeStrategyUpdateTime; // +F94. In milliseconds. Updated with game time when "panic mode" units are added to strategy.
		long int unknown_F98_timeAboutTributes; // +F98. Related to SNCoopDemandTributeInterval
		unsigned long int unknown_F9C;
		long int lastCoopSharAttackTime_ms; // +FA0. Only used if SNCoopShareAttacking=1 (0x4E2CD7). In milliseconds. Updated when an group is tasked to attack some target?
		unsigned long int unknown_FA4; // flag ? about exploration ? Used in gatherer affectation methods
		unsigned long int unknown_FA8; // +FA8.
		STRUCT_TAC_AI_TARGET_INFO targetInfo; // +FAC. This is only used in "group.FindAttackTarget". Values only make sense when inProgress=1 (unused otherwise).
		long int attacksByPlayerCount[9]; // +FD8. number of times this player attacked me. Updated in 0x4D7AF0 (tacAI.reactToEvent 0x201), never used in standard code ?
		long int lastUAms; // +FFC. "LastUAms" (UpdateAI milliseconds). Time spent in last updateAI execution
		long int averageUpdateAITime_ms; // +1000. Updated each time unknown_1004_time_counter reaches 20 and is reset. "Average UAms" ?
		long int calcAverageUpdateAITime_counter; // Maximum value = 20 = 0x14
		long int lastAIUpdateTime_ms; // +1008. A timeGetTime value. (not a game time)

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_TAC_AI; }

		// GetCivilianCount : allVillagers.usedElements
		// GetCivilianExplorersCount = villagerExplorers.usedElements
		// GetCivilianGatherersCount = villagerTasksRelevantElemCount
		// GetDesiredNumberCivilianExplorersCount = villagerExplorers.maxElementCount (+10C, array+8)
		// NumberSoldiers = landMilitaryUnits.usedElements
		// NumberBoats = allBoats.usedElements
	};
	static_assert(sizeof(STRUCT_TAC_AI) == 0x100C, "STRUCT_TAC_AI size");


}
