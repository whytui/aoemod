#include "../include/UnitGroupAI.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {
;


UnitGroupAI::UnitGroupAI() {
	this->ResetAllInfo();
}


void UnitGroupAI::ResetAllInfo() {

}


// Set unitGroup.currentTask and call ApplyTask for the group (creates underlying unit commands, etc).
void UnitGroupAI::SetUnitGroupCurrentTask(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup, UNIT_GROUP_TASK_IDS taskId,
	long int resetOrg, bool force) {
	if (!unitGroup) { return; }
	unitGroup->currentTask = taskId;
	unitGroup->unknown_resetOrg = 0; // Not sure if we should do this, and if we should use 0 or the parameter
	AOE_METHODS::UNIT_GROUP::ApplyTaskToUnitGroup(unitGroup, tacAI, taskId, resetOrg, force);
}


// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
// For all non-supported (or without custom treatment) cases, just return false !
bool UnitGroupAI::TaskActiveUnitGroup(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup) {
	if (!tacAI || !tacAI->IsCheckSumValid() || !unitGroup || !unitGroup->IsCheckSumValid()) {
		return false;
	}
	// Not-supported unit group type (no custom treatment)
	if (unitGroup->unitGroupType != UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK) {
		// TODO: support land defend too or even explore (to overload on important situations)
		return false;
	}

	// Get some main objects
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	STRUCT_AI *mainAI = tacAI->ptrMainAI;
	if (!mainAI || !mainAI->IsCheckSumValid()) { return false; }
	STRUCT_PLAYER *player = mainAI->ptrStructPlayer;
	if (!player || !player->IsCheckSumValid()) { return false; }
	if (unitGroup->commanderUnitId < 0) { return false; }
	//AIPlayerTargetingInfo *myAIInfo = playerTargetingHandler.GetPlayerInfo(tacAI->commonAIObject.playerId);
	//if (!myAIInfo) { return false; }

#ifndef _DEBUG
	return false; // feature is still under development
#endif

	if (unitGroup->unknown_resetOrg == 0) {
		// Task has just been set ?
	} else {
		// // Most frequent case
	}

	// myWeaknessScore=100 if I am weak
	int myWeaknessScore = 100 - this->militaryAIInfo->EvaluatePlayerWeakness(player);
	int totalAttacksInPeriod = 0;
	int totalPanicModesInPeriod = 0;
	const long int periodLengthMilliseconds = AI_CONST::delayInWhichEnemyAttacksImpactUnitGroupTasking_ms;
	long int periodStartTime = global->currentGameTime - periodLengthMilliseconds;
	for (int enemyPlayerId = 1; enemyPlayerId < global->playerTotalCount; enemyPlayerId++) {
		bool allyOrGaiaOrSelf = (player->diplomacyVSPlayers[enemyPlayerId] <= PLAYER_DIPLOMACY_VALUES::CST_PDV_ALLY);
		if (!allyOrGaiaOrSelf) {
			totalAttacksInPeriod += this->militaryAIInfo->GetAttacksCountFromPlayerInPeriod(enemyPlayerId, periodStartTime, global->currentGameTime);
			totalPanicModesInPeriod += this->militaryAIInfo->GetPanicModesCountFromPlayerInPeriod(enemyPlayerId, periodStartTime, global->currentGameTime);
		}
	}

	// Get an idea of ALL land military groups and their types
	STRUCT_UNIT_GROUP *loopUnitGroup = &tacAI->fakeFirstUnitGroupElem;
	int landAttackGroupCount = 0;
	int landDefendGroupCount = 0;
	while (loopUnitGroup && loopUnitGroup->IsCheckSumValid() && (loopUnitGroup != &tacAI->fakeFirstUnitGroupElem)) {
		if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK) { landAttackGroupCount++; }
		if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_DEFEND) { landDefendGroupCount++; }
		loopUnitGroup = loopUnitGroup->next;
	}

	// Group (leader) distance from our town
	STRUCT_UNIT_BASE *commander = global->GetUnitFromId(unitGroup->commanderUnitId);
	if (!commander) {
		std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(".group#") + 
			std::to_string(unitGroup->unitGroupId) + std::string(" has an invalid leader ID");
		traceMessageHandler.WriteMessage(msg);
		return false;
	}
	STRUCT_UNIT_BASE *myTC = AOE_MainAI_findUnit(mainAI, CST_UNITID_FORUM);
	float distanceToTC = -1;
	if (myTC && myTC->IsCheckSumValidForAUnitClass()) {
		distanceToTC = GetDistance(commander->positionX, commander->positionY, myTC->positionX, myTC->positionY);
	}

	// Unit group consistution and current activity
	int groupUnitCount = unitGroup->unitCount;
	int index = 0;
	int validUnitsFound = 0;
	int badDefenderUnits = 0; // units that are not very relevant for defense. Typically, catapults.
	int notAttackingUnitsCount = 0; // Number of units that do not currently have an agressive activity.
	int unitsAttackingNonPriorityTarget = 0;
	while ((index < STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT) && (validUnitsFound < groupUnitCount)) {
		long int curUnitId = unitGroup->GetMyUnitId(index);
		if (curUnitId > -1) {
			STRUCT_UNIT_BASE *curUnit = global->GetUnitFromId(curUnitId);
			if (curUnit && curUnit->IsCheckSumValidForAUnitClass()) {
				validUnitsFound++;
				assert(curUnit->unitDefinition && curUnit->unitDefinition->IsCheckSumValidForAUnitClass());
				if (!curUnit->unitDefinition) { return false; } // ERROR
				STRUCT_UNITDEF_BASE *unitDef = curUnit->unitDefinition;
				if (unitDef->DerivesFromAttackable() && curUnit->DerivesFromAttackable()) {
					STRUCT_UNITDEF_ATTACKABLE* unitDefAttackable = (STRUCT_UNITDEF_ATTACKABLE*)curUnit->unitDefinition;
					if (UnitDefProjectileCanDamageOwnUnits(unitDefAttackable)) {
						badDefenderUnits++;
					}
					STRUCT_UNIT_ATTACKABLE *unitAttackable = (STRUCT_UNIT_ATTACKABLE*)curUnit;
					assert(unitAttackable->IsCheckSumValidForAUnitClass());
					if (unitAttackable->currentActivity) {
						bool hasAttackActivity = (unitAttackable->currentActivity->currentActionId == ACTIVITY_TASK_IDS::CST_ATI_ATTACK) ||
							(unitAttackable->currentActivity->currentActionId == ACTIVITY_TASK_IDS::CST_ATI_CONVERT);
						STRUCT_ACTION_BASE *unitAction = AOE_METHODS::GetUnitAction(curUnit);
						bool actionIsAgressive = unitAction && (
							(unitAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_CONVERT) ||
							(unitAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_ATTACK_9) ||
							(unitAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_UNKNOWN_7) // Unsure !!!
							);
						if (!hasAttackActivity && !actionIsAgressive) {
							notAttackingUnitsCount++;
						}
						if (hasAttackActivity && actionIsAgressive && unitAction) {
							STRUCT_UNIT_BASE *targetUnit = unitAction->targetUnit;
							if (!targetUnit) {
								targetUnit = global->GetUnitFromId(unitAction->targetUnitId);
							}
							if (targetUnit && targetUnit->IsCheckSumValidForAUnitClass() && targetUnit->unitDefinition && targetUnit->unitDefinition->IsCheckSumValidForAUnitClass()) {
								GLOBAL_UNIT_AI_TYPES targetAIType = targetUnit->unitDefinition->unitAIType;
								if (((targetAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) ||
									(targetAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupWall)) &&
									(!IsTower(targetUnit->unitDefinition))) {
									unitsAttackingNonPriorityTarget++;
								}
							}
						}
					}
				}
			}
		}
		index++;
	}
	int consideredInactiveCount = notAttackingUnitsCount + unitsAttackingNonPriorityTarget;
	int halfGroup = groupUnitCount / 2;
	bool unitGroupIsAlmostIdle = (consideredInactiveCount > 0) && (consideredInactiveCount >= halfGroup);


	// Handle "weak" situations
	// TODO: no group in my town and few resources (myWeaknessScore) : should depend on random...
	// ...and player "personality": AI players still should try to attack early in both RM/DM, for example. But not always ? If "I sent" a group to attack, make sure I don't recall it here just after
	// Also, take into account strategy progress: if I am attacking for 10th time in "weak" situation and didn't make any progress in strategy (in RM), then stop it ?
	if (myWeaknessScore > 70) {
		// unitGroupIsAlmostIdle: What do we do if units are busy ?
		if (unitGroupIsAlmostIdle && (distanceToTC > AI_CONST::townSize + 5) && 
			(unitGroup->currentTask != UNIT_GROUP_TASK_IDS::CST_UGT_RETREAT)) {
			// Randomly choose a zone to "defend" : TC or last (in-town) attack position
			int randomValue = randomizer.GetRandomPercentageValue();
			if (randomValue > 60) {
				unitGroup->retreatPosX = militaryAIInfo->recentAttacksByPlayer->lastAttackInMyTownPosX;
				unitGroup->retreatPosY = militaryAIInfo->recentAttacksByPlayer->lastAttackInMyTownPosY;
			}
			if ((randomValue <= 40) || (unitGroup->retreatPosX < 0) || (unitGroup->retreatPosX < 0)) {
				unitGroup->retreatPosX = myTC->positionX;
				unitGroup->retreatPosY = myTC->positionY;
			}
			// 40-60: let previous retreat value
#ifdef _DEBUG
			CallWriteText("Ordered a retreat due to weakness");
#endif
			unitGroup->currentTask = UNIT_GROUP_TASK_IDS::CST_UGT_RETREAT;
			this->SetUnitGroupCurrentTask(tacAI, unitGroup, UNIT_GROUP_TASK_IDS::CST_UGT_RETREAT, 1, true);
		}
	}


	/*if (unitGroupIsAlmostIdle) {
	// TEST
	unitGroup->unitGroupType = UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE;
	unitGroup->currentTask = UNIT_GROUP_TASK_IDS::CST_UGT_EXPLORE;
	unitGroup->unknown_resetOrg = 0;
	AOE_METHODS::UNIT_GROUP::ApplyTaskToUnitGroup(unitGroup, tacAI, UNIT_GROUP_TASK_IDS::CST_UGT_EXPLORE, 1, true);
	}*/
	return false;

	//this->priorityLocation
	//playerTargetingHandler.GetPlayerInfo(tacAI->commonAIObject.playerId)->panicModeProvokedByEnemyPlayersDuringLastPeriod

	//if (global->currentGameTime - tacAI->lastPanicModeStrategyUpdateTime < 20000) { // TODO: constant
	// Could use mainAI->structInfAI.attacksHistory too
	// Retreat (test)
	// If group is away from my town and/or partially inactive ?
	/*if (myTC && myTC->IsCheckSumValidForAUnitClass() && myTC->unitDefinition && myTC->unitDefinition->IsCheckSumValidForAUnitClass()) {
		// TEST2 : defend
		if (unitGroup->currentTask != UNIT_GROUP_TASK_IDS::CST_UGT_DEFEND_UNIT) {
			unitGroup->targetPosX = myTC->positionX;
			unitGroup->targetPosY = myTC->positionY;
			unitGroup->targetDAT_ID = myTC->unitDefinition->DAT_ID1;
			unitGroup->targetUnitId = myTC->unitInstanceId;
			unitGroup->currentTask = UNIT_GROUP_TASK_IDS::CST_UGT_DEFEND_UNIT;
			unitGroup->unknown_resetOrg = 0;
			unitGroup->targetPlayerId = player->playerId;
			AOE_METHODS::UNIT_GROUP::ApplyTaskToUnitGroup(unitGroup, tacAI, UNIT_GROUP_TASK_IDS::CST_UGT_DEFEND_UNIT, 1, true);
		}
	}*/
	//}
	/*int panicModeCount = 0;
	for (int i = 1; i < global->playerTotalCount; i++) {
	panicModeCount += myAIInfo->panicModeProvokedByEnemyPlayersDuringLastPeriod[i];

	//TARGETING_CONST::updateDetailedDislikeInfoMaxDelay;
	//myAIInfo->lastUpdateGameTime
	}*/

	return false; //TO DO
}


}
