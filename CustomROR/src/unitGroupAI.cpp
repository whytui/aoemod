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
	AOE_METHODS::UNIT_GROUP::ApplyTaskToUnitGroup(unitGroup, tacAI, taskId, resetOrg, force);
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (global && global->IsCheckSumValid()) {
		unitGroup->lastTaskingTime_ms = global->currentGameTime;
	}
}


// Attack a target or use retreat to approach a zone to defend/attack
// If target is not found and no default retreat position if provided (-1), the group is NOT tasked
// Returns the used task id.
UNIT_GROUP_TASK_IDS UnitGroupAI::AttackOrRetreat(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_INF_AI_UNIT_LIST_ELEM *targetInfo,
	float defaultRetreatPosX, float defaultRetreatPosY, bool forceTasking) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET; }
	STRUCT_UNIT_BASE *targetUnit = NULL;
	if (targetInfo) {
		targetUnit = global->GetUnitFromId(targetInfo->unitId);
	}

	bool isVisible = targetUnit && IsFogVisibleForPlayer(tacAI->commonAIObject.playerId, (long int)targetUnit->positionX, (long int)targetUnit->positionY);
	float targetPosX = -1.f;
	float targetPosY = -1.f;
	if (targetInfo) {
		if (isVisible) {
			targetPosX = targetUnit->positionX;
			targetPosY = targetUnit->positionY;
			// By the way, update info (not always correct)
			targetInfo->posX = (unsigned char)targetUnit->positionX;
			targetInfo->posY = (unsigned char)targetUnit->positionY;
			targetInfo->HP = (short)targetUnit->remainingHitPoints;
		} else {
			// Use last known enemy's position as retreat position (
			targetPosX = targetInfo->posX;
			targetPosY = targetInfo->posY;
		}
	} else {
		targetPosX = defaultRetreatPosX;
		targetPosY = defaultRetreatPosY;
	}
	if ((targetPosX < 0) || (targetPosY < 0)) { return UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET; }
	unitGroup->targetPosX = targetPosX;
	unitGroup->targetPosY = targetPosY;
	unitGroup->unitGroupType = UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK;
	unitGroup->isTasked = 0;
	if (targetUnit && isVisible) {
		if (targetUnit->ptrStructPlayer) { // should always be true !
			unitGroup->targetPlayerId = targetUnit->ptrStructPlayer->playerId;
		}
		unitGroup->targetDAT_ID = targetUnit->unitDefinition->DAT_ID1;
		this->SetUnitGroupCurrentTask(tacAI, unitGroup, UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02, 1, forceTasking);
		unitGroup->lastTaskingTime_ms = global->currentGameTime;
		return UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02;
	}
	// No (visible) target: use retreat. Maybe we'll find the enemy on our way
	unitGroup->targetPlayerId = -1;
	unitGroup->targetDAT_ID = -1;
	unitGroup->retreatPosX = targetPosX;
	unitGroup->retreatPosY = targetPosY;
	this->SetUnitGroupCurrentTask(tacAI, unitGroup, UNIT_GROUP_TASK_IDS::CST_UGT_RETREAT, 1, forceTasking);
	unitGroup->lastTaskingTime_ms = global->currentGameTime;
	return UNIT_GROUP_TASK_IDS::CST_UGT_RETREAT;
}


// Called once before the "task active groups loop on unit groups". Can be used for initializations, common treatments, etc.
// This method is always called (once) just before the TaskActiveUnitGroup calling series.
// Returns true if the "loop" on unit group can be processed (normal case)
// Returns false to disable all "Task active soldiers" treatments (for this occurrence only)
bool UnitGroupAI::OnTaskActiveGroupsBegin(STRUCT_TAC_AI *tacAI, long int processStartTimeGetTimeValue, long int allowedTime) {
	this->activeGroupsTaskingTempInfo.ResetAllInfo();

	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	STRUCT_AI *mainAI = tacAI->ptrMainAI;
	if (!mainAI || !mainAI->IsCheckSumValid()) { return false; }
	STRUCT_PLAYER *player = mainAI->ptrStructPlayer;
	if (!player || !player->IsCheckSumValid()) { return false; }

	if (tacAI->currentlyProcessedUnitGroupId != -1) {
		// Do something different when a loop is in progress ? (last execution hadn't enough time to complete)
	}

	// Collect and save "static" information (that will be valid for "this loop" only)
	this->activeGroupsTaskingTempInfo.allowedTime = allowedTime;
	this->activeGroupsTaskingTempInfo.processingStartTimeGetTime = processStartTimeGetTimeValue;
	this->activeGroupsTaskingTempInfo.myMainCentralUnit = CUSTOM_AI::CustomAIMilitaryInfo::GetTownCenterOrUnitToDefend(player);
	this->activeGroupsTaskingTempInfo.myWeaknessScore = 100 - this->militaryAIInfo->EvaluatePlayerWeakness(player);

	const long int periodLengthMilliseconds = AI_CONST::delayInWhichEnemyAttacksImpactUnitGroupTasking_ms;
	long int periodStartTime = global->currentGameTime - periodLengthMilliseconds;
	for (int enemyPlayerId = 1; enemyPlayerId < global->playerTotalCount; enemyPlayerId++) {
		bool allyOrGaiaOrSelf = (player->diplomacyVSPlayers[enemyPlayerId] <= PLAYER_DIPLOMACY_VALUES::CST_PDV_ALLY);
		if (!allyOrGaiaOrSelf) {
			this->activeGroupsTaskingTempInfo.totalAttacksInPeriod += this->militaryAIInfo->GetAttacksCountFromPlayerInPeriod(enemyPlayerId, periodStartTime, global->currentGameTime);
			this->activeGroupsTaskingTempInfo.totalPanicModesInPeriod += this->militaryAIInfo->GetPanicModesCountFromPlayerInPeriod(enemyPlayerId, periodStartTime, global->currentGameTime);
		}
	}

	// Get an idea of ALL land military groups and their types
	STRUCT_UNIT_GROUP *loopUnitGroup = tacAI->fakeFirstUnitGroupElem.next;

	while (loopUnitGroup && loopUnitGroup->IsCheckSumValid() && (loopUnitGroup != &tacAI->fakeFirstUnitGroupElem)) {
		STRUCT_UNIT_BASE *loopUnitCommander = NULL;
		if (loopUnitGroup->commanderUnitId >= 0) {
			loopUnitCommander = global->GetUnitFromId(loopUnitGroup->commanderUnitId);
		}
		if (loopUnitCommander) {
			if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK) { this->activeGroupsTaskingTempInfo.totalLandAttackGroupCount++; }
			if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_DEFEND) { this->activeGroupsTaskingTempInfo.totalLandDefendGroupCount++; }
			if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE) { this->activeGroupsTaskingTempInfo.totalLandExploreGroupCount++; }
			float diffX = this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionX - loopUnitCommander->positionX;
			float diffY = this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionY - loopUnitCommander->positionY;
			float loopGrpSquareDist = (diffX*diffX) + (diffY*diffY);
			if (loopGrpSquareDist < AI_CONST::townSizeSquare) {
				if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK) { this->activeGroupsTaskingTempInfo.townLandAttackGroupCount++; }
				if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_DEFEND) { this->activeGroupsTaskingTempInfo.townLandDefendGroupCount++; }
				if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE) { this->activeGroupsTaskingTempInfo.townLandExploreGroupCount++; }
			}
		}
		loopUnitGroup = loopUnitGroup->next;
	}

	this->activeGroupsTaskingTempInfo.mainCentralUnitIsVital = false;
	if (this->activeGroupsTaskingTempInfo.myMainCentralUnit && this->activeGroupsTaskingTempInfo.myMainCentralUnit->unitDefinition) {
		STRUCT_UNITDEF_BASE *myMainCentralUnitDef = this->activeGroupsTaskingTempInfo.myMainCentralUnit->unitDefinition;
		this->activeGroupsTaskingTempInfo.mainCentralUnitIsVital = ((myMainCentralUnitDef->unitAIType == TribeAIGroupCivilian) ||
			(myMainCentralUnitDef->unitAIType == TribeAIGroupPriest) ||
			(myMainCentralUnitDef->DAT_ID1 == CST_UNITID_FORUM));
	}

	return true; // Normal case: allow processing the groups
}


// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
// For all non-supported (or without custom treatment) cases, just return false !
// Note: OnTaskActiveGroupsBegin has been called once before calling this (n' times)
bool UnitGroupAI::TaskActiveUnitGroup(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup) {
	if (!tacAI || !tacAI->IsCheckSumValid() || !unitGroup || !unitGroup->IsCheckSumValid()) {
		return false;
	}
	// Not-supported unit group type (no custom treatment)
	if ((unitGroup->unitGroupType != UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK) &&
		(unitGroup->unitGroupType != UNIT_GROUP_TYPES::CST_UGT_LAND_DEFEND) &&
		(unitGroup->unitGroupType != UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE)) {
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
	long int myPlayerId = tacAI->commonAIObject.playerId;

#ifndef _DEBUG
	return false; // feature is still under development
#endif

	// TODO: get a clear idea of what is being done in each case in standard code
	if (unitGroup->isTasked == 0) {
		// Task has just been set ?
		// Artefacts: if 60s have passed, try to return to TC (possibly require a transport)
		// Other than attack/defend: do nothing ? (re-task with same taskId)
		// Defend grp: see 0x4D3E45
		// Attack grp: if tacAI.attackEnabled=0, do nothing. Check lastAttackTime_ms, SNAttackSeparationTime, find target (targeting), attack (some complex treatments)
	} else {
		// // Most frequent case (cf 04D4722)
	}

	STRUCT_UNIT_BASE *myMainCentralUnit = this->activeGroupsTaskingTempInfo.myMainCentralUnit;
	assert(myMainCentralUnit && myMainCentralUnit->IsCheckSumValidForAUnitClass());

	if ((unitGroup->isTasked == 1) && (this->activeGroupsTaskingTempInfo.totalPanicModesInPeriod == 0)) {
		return false; // Let ROR code handle this case
	}

	int totalGroupsCount = this->activeGroupsTaskingTempInfo.totalLandAttackGroupCount + this->activeGroupsTaskingTempInfo.totalLandDefendGroupCount + this->activeGroupsTaskingTempInfo.totalLandExploreGroupCount;
	int totalGroupsInTown = this->activeGroupsTaskingTempInfo.townLandAttackGroupCount + this->activeGroupsTaskingTempInfo.townLandDefendGroupCount + this->activeGroupsTaskingTempInfo.townLandExploreGroupCount;

	// Group (leader) distance from our town (or unit to defend)
	
	STRUCT_UNIT_BASE *commander = global->GetUnitFromId(unitGroup->commanderUnitId);
	if (!commander) {
		std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(".group#") +
			std::to_string(unitGroup->unitGroupId) + std::string(" has an invalid leader ID");
		traceMessageHandler.WriteMessage(msg);
		return false;
	}
	float distanceToMyMainUnit = -1;
	float unitGroupOrientationXFromMainUnit = 0; // -1 if group is "left" of main unit, 1 if on the "right" (X axis)
	float unitGroupOrientationYFromMainUnit = 0; // -1 if group is "down" of main unit, 1 if "upper" (Y axis)
	if (myMainCentralUnit && myMainCentralUnit->IsCheckSumValidForAUnitClass()) {
		distanceToMyMainUnit = GetDistance(commander->positionX, commander->positionY, myMainCentralUnit->positionX, myMainCentralUnit->positionY);
		if (commander->positionX < myMainCentralUnit->positionX) {
			unitGroupOrientationXFromMainUnit = -1;
		}
		if (commander->positionX > myMainCentralUnit->positionX) {
			unitGroupOrientationXFromMainUnit = 1;
		}
		if (commander->positionY < myMainCentralUnit->positionY) {
			unitGroupOrientationYFromMainUnit = -1;
		}
		if (commander->positionY > myMainCentralUnit->positionY) {
			unitGroupOrientationYFromMainUnit = 1;
		}
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

	STRUCT_UNIT_BASE *enemyUnitNearMyMainUnit = NULL;
	STRUCT_INF_AI_UNIT_LIST_ELEM *enemyUnitNearMyMainUnitInfAIElem = NULL;
	bool enemyUnitNearMyMainUnitIsCurrentlyVisible = false;
	if (myMainCentralUnit) {
		long int searchDistance = 7;
		if (myMainCentralUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) {
			searchDistance = CUSTOM_AI::AI_CONST::townSize + 5;
		}
		// TODO: we could search in several steps: start with part of the town (or zone) which is close to me, search opposite side last
		
		long int bestIndexEnemy = -1;
		STRUCT_UNIT_BASE *bestEnemy = NULL;
		float bestSquareDistance = 999999;
		bool bestEnemyIsVisible = false;
		long int currentIndex = 0;
		while (currentIndex != -1) {
			currentIndex = CUSTOM_AI::CustomAIMilitaryInfo::FindInfAiIndexOfEnemyMilitaryUnitNearPosition(player,
				(long int)myMainCentralUnit->positionX, (long int)myMainCentralUnit->positionY, searchDistance, true, currentIndex);
			STRUCT_UNIT_BASE *curUnit = NULL;
			if (currentIndex >= 0) {
				long int tmpUnitId =mainAI->structInfAI.unitElemList[currentIndex].unitId;
				curUnit = global->GetUnitFromId(tmpUnitId);
				if (curUnit && curUnit->IsCheckSumValidForAUnitClass()) {
					// Calc distance & check visibility
					bool thisUnitIsVisible = IsFogVisibleForPlayer(myPlayerId, (long int)curUnit->positionX, (long int)curUnit->positionY);
					if (thisUnitIsVisible || !bestEnemyIsVisible) { // being visible is top priority criterion
						float diffX = curUnit->positionX - commander->positionX;
						float diffY = curUnit->positionY - commander->positionY;
						float thisUnitSquareDistance = (diffX*diffX) + (diffY*diffY);
						if (thisUnitSquareDistance < bestSquareDistance) {
							bestIndexEnemy = currentIndex;
							bestEnemy = curUnit;
							bestSquareDistance = thisUnitSquareDistance;
							bestEnemyIsVisible = thisUnitIsVisible;
						}
					}
				}
			}
			if (currentIndex > -1) { currentIndex++; } // So next search starts at "next" element.
		}
		if (bestIndexEnemy > -1) {
			enemyUnitNearMyMainUnitInfAIElem = &mainAI->structInfAI.unitElemList[bestIndexEnemy];
			enemyUnitNearMyMainUnit = bestEnemy;
		}

		if (enemyUnitNearMyMainUnitInfAIElem) {
			enemyUnitNearMyMainUnit = global->GetUnitFromId(enemyUnitNearMyMainUnitInfAIElem->unitId);
		}
		if (enemyUnitNearMyMainUnit && !enemyUnitNearMyMainUnit->IsCheckSumValidForAUnitClass()) {
			enemyUnitNearMyMainUnit = NULL; // Security check only.
		}
		if (enemyUnitNearMyMainUnit) {
			enemyUnitNearMyMainUnitIsCurrentlyVisible = IsFogVisibleForPlayer(myPlayerId,
				(long int)enemyUnitNearMyMainUnit->positionX, (long int)enemyUnitNearMyMainUnit->positionY);
		}
	}

	bool beenAttackedRecently = (this->activeGroupsTaskingTempInfo.totalAttacksInPeriod > 0) || (this->activeGroupsTaskingTempInfo.totalPanicModesInPeriod > 0);
	long int mainUnitSearchZoneRadius = 5;
	long int mainUnitProtectionRadius = 5;
	if (myMainCentralUnit && myMainCentralUnit->unitDefinition && (myMainCentralUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM)) {
		mainUnitSearchZoneRadius = AI_CONST::townSize + 5;
		mainUnitProtectionRadius = 7;
	}

	// Handle "weak" situations

	// Special: discard explore groups if situation is critical
	if ((this->activeGroupsTaskingTempInfo.myWeaknessScore > 50) && this->activeGroupsTaskingTempInfo.mainCentralUnitIsVital && 
		beenAttackedRecently && (unitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE)) {
		float targetPosX = -1;
		float targetPosY = -1;
		targetPosX = militaryAIInfo->recentAttacksByPlayer->lastAttackInMyTownPosX; // may be -1
		targetPosY = militaryAIInfo->recentAttacksByPlayer->lastAttackInMyTownPosY; // may be -1
		if (myMainCentralUnit && ((targetPosX < 0) || (targetPosY < 0))) {
			targetPosX = myMainCentralUnit->positionX + (mainUnitProtectionRadius * unitGroupOrientationXFromMainUnit);
			targetPosY = myMainCentralUnit->positionY + (mainUnitProtectionRadius * unitGroupOrientationYFromMainUnit);
		}
		UNIT_GROUP_TASK_IDS result = this->AttackOrRetreat(tacAI, unitGroup, enemyUnitNearMyMainUnitInfAIElem,
			targetPosX, targetPosY, true);
		return (result != UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET); // True if unit group has been tasked
	}

	if (unitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE) {
		// For explore groups, let ROR code handle all other situations than "I am very weak".
		// TODO: explore groups do not respond to attacks, we could do something here ? Or rather in tacAI.react to event maybe
		return false;
	}

	// TODO: no group in my town and few resources (myWeaknessScore) : should depend on random...
	// ...and player "personality": AI players still should try to attack early in both RM/DM, for example. But not always ? If "I sent" a group to attack, make sure I don't recall it here just after
	// Also, take into account strategy progress: if I am attacking for 10th time in "weak" situation and didn't make any progress in strategy (in RM), then stop it ?
	if ((this->activeGroupsTaskingTempInfo.mainCentralUnitIsVital && (this->activeGroupsTaskingTempInfo.totalPanicModesInPeriod > 0) &&
		(this->activeGroupsTaskingTempInfo.myWeaknessScore > 70)) || (totalGroupsInTown <= 1)) { // TEMP TEST
		// TODO: we should count the groups IN my town
		if ((unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_RETREAT) && !AOE_METHODS::IsUnitIdle(commander)) {
			// TODO: check if leader has reached retreat pos (idle or at target pos)
			return true; // If still moving, let the group finish retreating ?
		}
		if (distanceToMyMainUnit > AI_CONST::townSize + 5) {
			// Randomly choose a zone to "defend" : TC or last (in-town) attack position
			int randomValue = randomizer.GetRandomPercentageValue();
			float targetPosX = -1;
			float targetPosY = -1;
			//if (randomValue > 60) {
				targetPosX = militaryAIInfo->recentAttacksByPlayer->lastAttackInMyTownPosX; // may be -1
				targetPosY = militaryAIInfo->recentAttacksByPlayer->lastAttackInMyTownPosY; // may be -1
			//}
			/*if (randomValue <= 40) {
				targetPosX = myMainCentralUnit->positionX + (mainUnitProtectionRadius * unitGroupOrientationXFromMainUnit);
				targetPosY = myMainCentralUnit->positionY + (mainUnitProtectionRadius * unitGroupOrientationYFromMainUnit);
			}*/
			// 40-60: do nothing (do not set targetPosX/targetPosY)

			UNIT_GROUP_TASK_IDS result = this->AttackOrRetreat(tacAI, unitGroup, enemyUnitNearMyMainUnitInfAIElem,
				targetPosX, targetPosY, true);
			
			if (result != UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET) {
#ifdef _DEBUG
				std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(" Ordered taskId=") +
					std::to_string(result) + std::string(" due to weakness, wkn=");
				msg += std::to_string(this->activeGroupsTaskingTempInfo.myWeaknessScore) + std::string(" grpCnt=") + std::to_string(totalGroupsInTown);
				CallWriteText(msg.c_str());
#endif
				return true; // Unit group has been tasked
			}
		} else {
			// Group is in/near town
			if (global->currentGameTime - unitGroup->lastTaskingTime_ms > (1000 * tacAI->SNNumber[SNTacticalUpdateFrequency])) {
				// Attack enemy if visible/if we know a recent location. Otherwise, do nothing
				UNIT_GROUP_TASK_IDS result = this->AttackOrRetreat(tacAI, unitGroup, enemyUnitNearMyMainUnitInfAIElem, -1, -1, false);
				if (result != UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET) {
#ifdef _DEBUG
					std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(" Ordered taskId=") +
						std::to_string(result) + std::string(" due to weakness, wkn=");
					msg += std::to_string(this->activeGroupsTaskingTempInfo.myWeaknessScore) + std::string(" grpCnt=") + std::to_string(totalGroupsInTown);
					CallWriteText(msg.c_str());
#endif
					return true; // Unit group has been tasked
				}
				return true; // no target, do nothing special
			}

#ifdef _DEBUG
			//CallWriteText("Ordered exterminate due to weakness");
#endif
			//unitGroup->isTasked = 0;
			//this->SetUnitGroupCurrentTask(tacAI, unitGroup, UNIT_GROUP_TASK_IDS::CST_UGT_EXTERMINATE, 1, false);
		}
	}



	return false;

	//this->priorityLocation

	// Could use mainAI->structInfAI.attacksHistory too
	// Retreat (test)
	// If group is away from my town and/or partially inactive ?

	// TODO: set tacAI->lastAttackTime_ms if we launch an attack (on a specific target?)

	return false; //TO DO
}


}
