#include "../include/UnitGroupAI.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {
;


UnitGroupAI::UnitGroupAI() {
	this->ResetAllInfo();
}


void UnitGroupAI::ResetAllInfo() {
	this->activeGroupsTaskingTempInfo.ResetAllInfo();
	this->lastDebugInfo = "";
	this->gameDiffLevel = GAME_DIFFICULTY_LEVEL::GDL_MEDIUM;
	this->last5TaskingByUGAI.clear();
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (settings && settings->IsCheckSumValid()) {
		this->gameDiffLevel = settings->rgeGameOptions.difficultyLevel;
	}
}


void UnitGroupAI::AddTaskingByUGAITrace(long int unitGroupId, long int taskId, long int targetId, long int gameTime_ms) {
	if (this->last5TaskingByUGAI.size() >= UNITGROUP_AI_LAST_TASK_TRACE_COUNT) {
		this->last5TaskingByUGAI.pop_front(); // Remove oldest
	}
	this->last5TaskingByUGAI.push_back(std::tuple<long int, long int, long int, long int>(
		unitGroupId, taskId, targetId, gameTime_ms));
}


// Sets unitGroup target* fields from provided target unit. Does not set task or run any treatment.
// targetUnit can be NULL.
// Logs an entry in last5TaskingByUGAI.
void UnitGroupAI::SetUnitGroupTarget(STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *targetUnit) {
	this->SetUnitGroupTarget_internal(unitGroup, targetUnit);
	long int gameTime_ms = -1;
#ifdef _DEBUG
	if (targetUnit) {
		STRUCT_PLAYER *player = targetUnit->ptrStructPlayer;
		if (player && player->IsCheckSumValid() && player->ptrGlobalStruct) {
			gameTime_ms = player->ptrGlobalStruct->currentGameTime;
		}
	}
#endif
	if (targetUnit) {
		this->AddTaskingByUGAITrace(unitGroup->unitGroupId, -1, targetUnit->unitInstanceId, gameTime_ms);
	}
}


// Sets unitGroup target* fields from provided target unit. Does not set task or run any treatment.
// targetUnit can be NULL.
// Does not add an entry in last5TaskingByUGAI
// Returns true if successful
bool UnitGroupAI::SetUnitGroupTarget_internal(STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *targetUnit) {
	if (!unitGroup) { return false; }
	if (!targetUnit || !targetUnit->unitDefinition || !targetUnit->ptrStructPlayer) {
		unitGroup->targetDAT_ID = -1;
		unitGroup->targetPlayerId = -1;
		unitGroup->targetUnitId = -1;
		unitGroup->targetPosX = -1;
		unitGroup->targetPosY = -1;
		unitGroup->targetPosZ = -1;
		return true;
	}
	unitGroup->targetDAT_ID = targetUnit->unitDefinition->DAT_ID1;
	unitGroup->targetPlayerId = targetUnit->ptrStructPlayer->playerId;
	unitGroup->targetUnitId = targetUnit->unitInstanceId;
	unitGroup->targetPosX = targetUnit->positionX;
	unitGroup->targetPosY = targetUnit->positionY;
	unitGroup->targetPosZ = targetUnit->positionY;
	return true;
}


// Computes internal variables about military situation
// This method should perform few operations to be quite fast
void UnitGroupAI::EvaluateMilitarySituation(STRUCT_TAC_AI *tacAI) {
	this->activeGroupsTaskingTempInfo.militarySituation = MILITARY_SITUATION::MS_UNKNOWN;
	if (!tacAI) { return; }
	this->activeGroupsTaskingTempInfo.myTotalMilitaryUnitCount = tacAI->landMilitaryUnits.usedElements;
	
	if (!this->activeGroupsTaskingTempInfo.mainCentralUnitIsVital) {
		// Special rules (this could be a scenario without a TC/villagers too: we shouldn't prevent AI from attacking then)

	}

	if (this->activeGroupsTaskingTempInfo.totalPanicModesInPeriod > 0) {
		this->activeGroupsTaskingTempInfo.militarySituation = MILITARY_SITUATION::MS_WEAK;

		if ((this->activeGroupsTaskingTempInfo.myWeaknessScore > 50) ||
			(this->activeGroupsTaskingTempInfo.myTotalMilitaryUnitCount < 4)) {
			this->activeGroupsTaskingTempInfo.militarySituation = MILITARY_SITUATION::MS_CRITICAL;
			return;
		}
		// Under some circumstances, situation is no longer weak
		// Ex: if enemies in my town were killed and I have many defence now, some DM situations (fast army re-training), some unit groups retreated successfully and crushed enemies, etc
		// TODO
		return;
	}

	if (this->activeGroupsTaskingTempInfo.myWeaknessScore >= 70) {
		if (this->activeGroupsTaskingTempInfo.totalLandGroups > 2) {
			this->activeGroupsTaskingTempInfo.militarySituation = MILITARY_SITUATION::MS_NORMAL;
		} else {
			this->activeGroupsTaskingTempInfo.militarySituation = MILITARY_SITUATION::MS_WEAK;
		}
		if ((this->activeGroupsTaskingTempInfo.totalAttacksInTownInPeriod > 0) && (this->activeGroupsTaskingTempInfo.totalLandGroups < 2)) {
			this->activeGroupsTaskingTempInfo.militarySituation = MILITARY_SITUATION::MS_CRITICAL;
		}
		return;
	}

	this->activeGroupsTaskingTempInfo.militarySituation = MILITARY_SITUATION::MS_NORMAL;
	if (this->activeGroupsTaskingTempInfo.myTotalMilitaryUnitCount > AI_CONST::armySizeToConsiderStrong) { // TODO: could depend on max population.
		this->activeGroupsTaskingTempInfo.myTotalMilitaryUnitCount = MILITARY_SITUATION::MS_STRONG;
		return;
	}
	
	if ((this->activeGroupsTaskingTempInfo.myWeaknessScore < 10) && (this->activeGroupsTaskingTempInfo.totalLandGroups > 1)) {
		// Rich and at least 2 land military groups (excluding artefact groups !)
		this->activeGroupsTaskingTempInfo.myTotalMilitaryUnitCount = MILITARY_SITUATION::MS_STRONG;
		return;
	}
	
}


// Set unitGroup.currentTask and call ApplyTask for the group (creates underlying unit commands, etc).
// unitGroup.lastUpdateTime_ms is updated
// unitGroup->lastAttackTaskingTime_ms is updated if taskId is an attack task.
void UnitGroupAI::SetUnitGroupCurrentTask(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup, UNIT_GROUP_TASK_IDS taskId,
	long int resetOrg, bool force) {
	if (!unitGroup) { return; }
	if (unitGroup->unitGroupType > UNIT_GROUP_TYPES::CST_UGT_BOAT_EXPLORE) {
		traceMessageHandler.WriteMessageNoNotification("tasking groupType > 102");
	}
	if ((taskId == UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_15) || (taskId == UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_ROUNDUP_TARGET)) {
		unitGroup->currentTask = UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02;
	} else {
		unitGroup->currentTask = taskId;
	}
	bool rorResult = AOE_METHODS::UNIT_GROUP::ApplyTaskToUnitGroup(unitGroup, tacAI, taskId, resetOrg, force);

	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (global && global->IsCheckSumValid()) {
		// Check on order type / task type / target type. Do not "attack" a friend unit (would block the unit)
		if (unitGroup->targetUnitId >= 0) {
			AOE_STRUCTURES::STRUCT_UNIT_BASE *target = global->GetUnitFromId(unitGroup->targetUnitId);
			if (target == NULL) {
				this->SetUnitGroupTarget(unitGroup, NULL);
			} else {
				assert(target->ptrStructPlayer && tacAI->ptrMainAI);
				PLAYER_DIPLOMACY_VALUES targetDiplValue = tacAI->ptrMainAI->player->diplomacyVSPlayers[target->ptrStructPlayer->playerId];
				bool alliedTarget = (targetDiplValue == PLAYER_DIPLOMACY_VALUES::CST_PDV_ALLY) || (targetDiplValue == PLAYER_DIPLOMACY_VALUES::CST_PDV_SELF);

				switch (unitGroup->currentTask) {
				case UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02:
				case UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_15:
				case UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_ROUNDUP_TARGET:
				case UNIT_GROUP_TASK_IDS::CST_UGT_EXTERMINATE:
					if (alliedTarget) {
						// A 'normal' case is when unit group's current target was just converted
						ROCKNROR::SYSTEM::SaveGameAndStopExecution(_T("SetUnitGroupCurrentTask: target dipl inconsistency"), true, true);
						this->SetUnitGroupTarget(unitGroup, NULL);
					}
					break;
				case UNIT_GROUP_TASK_IDS::CST_UGT_DEFEND_UNIT:
					if (!alliedTarget) {
						ROCKNROR::SYSTEM::SaveGameAndStopExecution(_T("SetUnitGroupCurrentTask: target dipl inconsistency"), true, true);
						this->SetUnitGroupTarget(unitGroup, NULL);
					}
					break;
				}
			}
		}

		unitGroup->lastUpdateTime_ms = global->currentGameTime;
		if (unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02) {
			unitGroup->lastAttackTaskingTime_ms = global->currentGameTime;
		}
	}
	this->AddTaskingByUGAITrace(unitGroup->unitGroupId, unitGroup->currentTask, unitGroup->targetUnitId, unitGroup->lastUpdateTime_ms);
}


// Attack a target or approaches a zone to defend/attack. Updates unitGroup->lastUpdateTime_ms if tasked.
// Updates unitGroup->lastAttackTaskingTime_ms is an attack task is assigned.
// If target is not found and no default retreat position if provided (-1), the group is NOT tasked
// Returns the used task id.
UNIT_GROUP_TASK_IDS UnitGroupAI::AttackOrRetreat(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_MEMORY *targetInfo,
	float defaultRetreatPosX, float defaultRetreatPosY, bool forceTasking) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET; }
	STRUCT_UNIT_BASE *targetUnit = NULL;
	if (targetInfo) {
		targetUnit = global->GetUnitFromId(targetInfo->unitId);
	}
	if (!tacAI->ptrMainAI->player) {
		assert(false && "NULL tacAI->ptrMainAI->player");
		return UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET;
	}

	bool isVisible = targetUnit && PLAYER::IsFogVisibleForPlayer(tacAI->ptrMainAI->player, (long int)targetUnit->positionX, (long int)targetUnit->positionY);
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
			if (targetUnit->ptrStructPlayer) { // should always be true !
				assert(targetInfo->playerId == (char)targetUnit->ptrStructPlayer->playerId); // debug / we are supposed to always fix it after conversion
				targetInfo->playerId = (char) targetUnit->ptrStructPlayer->playerId;
			}
		} else {
			// Use last known enemy's position as retreat position (
			targetPosX = targetInfo->posX;
			targetPosY = targetInfo->posY;
		}
	} else {
		targetPosX = defaultRetreatPosX;
		targetPosY = defaultRetreatPosY;
	}
	
	if (targetInfo && (targetInfo->playerId > 0) &&
		AOE_STRUCTURES::PLAYER::GetDiplomacyValueForPlayer(tacAI->ptrMainAI->player, targetInfo->playerId) < CST_PDV_NEUTRAL) {
		// Unexpected diplomacy
		return UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET;
	}

	if ((targetPosX < 0) || (targetPosY < 0)) { return UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET; }
	unitGroup->targetPosX = targetPosX;
	unitGroup->targetPosY = targetPosY;
	unitGroup->unitGroupType = UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK;
	unitGroup->isTasked = 1; // As we set a precise task with precise target (or at least position), consider the group as "task initialized"
	UNIT_GROUP_TASK_IDS result = UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET;
	if (targetUnit && isVisible) {
		if (forceTasking && (this->gameDiffLevel <= GAME_DIFFICULTY_LEVEL::GDL_MEDIUM)) {
			result = UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_ROUNDUP_TARGET; // units will avoid being distracted by other targets
		} else {
			if (global->currentGameTime - unitGroup->lastUpdateTime_ms < AI_CONST::minimumDelayBetweenBasicUnitGroupAttackTasking_ms) {
				// Tasking "CST_UGT_ATTACK_02" many times in a row is dangerous if it sees other targets (unit constantly changes action and is stuck)
				return UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET;
			}
			result = UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02; // units may change target (and very quickly) if they are challenged or see targets
		}
		if (targetUnit->ptrStructPlayer) { // should always be true !
			unitGroup->targetPlayerId = targetUnit->ptrStructPlayer->playerId;
		}
		unitGroup->targetDAT_ID = targetUnit->unitDefinition->DAT_ID1;
		unitGroup->targetUnitId = targetUnit->unitInstanceId;
		this->SetUnitGroupCurrentTask(tacAI, unitGroup, result, 1, forceTasking);
		return result;
	}
	// No (visible) target. Maybe we'll find the enemy on our way
	this->SetUnitGroupTarget_internal(unitGroup, NULL);
	unitGroup->retreatPosX = targetPosX;
	unitGroup->retreatPosY = targetPosY;
	unitGroup->targetPosX = targetPosX; // useful if using CST_UGT_ATTACK_02
	unitGroup->targetPosY = targetPosY; // useful if using CST_UGT_ATTACK_02
	// CST_UGT_ATTACK_02: needs to set target pos, will record an attack in tacAI history (which is unfortunate/irrelevant)
	//result = UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02; // attack with no target still works: operate a troop movement. Do not use CST_UGT_RETREAT.
	// CST_UGT_RETREAT: Why not ? Will move to exact target pos (which is not desired). I think there was another drawback ?
	//this->SetUnitGroupCurrentTask(tacAI, unitGroup, result, 1, forceTasking);
	//result = UNIT_GROUP_TASK_IDS::CST_UGT_RETREAT;
	// CST_UGT_REGROUP: Good point is that it handles spacing (if using 9, not 0xE). But needs to update grp.posX/Y. But it isn't a problem ?
	unitGroup->regroupPosX = targetPosX; // useful if using regroup
	unitGroup->regroupPosY = targetPosY; // useful if using regroup
	result = UNIT_GROUP_TASK_IDS::CST_UGT_REGROUP;
	this->SetUnitGroupCurrentTask(tacAI, unitGroup, result, 1, forceTasking);
	return result;
}


// Called once before the "task active groups loop on unit groups". Can be used for initializations, common treatments, etc.
// This method is always called (once) just before the TaskActiveUnitGroup calling series.
// Returns true if the "loop" on unit group can be processed (normal case)
// Returns false to disable all "Task active soldiers" treatments (for this occurrence only)
bool UnitGroupAI::OnTaskActiveGroupsBegin(STRUCT_TAC_AI *tacAI, long int processStartTimeGetTimeValue, long int allowedTime) {
	this->activeGroupsTaskingTempInfo.ResetAllInfo();
	
	// TODO: do not recompute (except pointers) when tacAI->currentlyProcessedUnitGroupId is >=0 ? (for perf)

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
	if (periodStartTime < 0) { periodStartTime = 0; }
	for (int enemyPlayerId = 1; enemyPlayerId < global->playerTotalCount; enemyPlayerId++) {
		bool allyOrGaiaOrSelf = (player->diplomacyVSPlayers[enemyPlayerId] <= PLAYER_DIPLOMACY_VALUES::CST_PDV_ALLY);
		if (!allyOrGaiaOrSelf) {
			this->activeGroupsTaskingTempInfo.totalAttacksInPeriod += this->militaryAIInfo->GetAttacksCountFromPlayerInPeriod(enemyPlayerId, periodStartTime, global->currentGameTime);
			this->activeGroupsTaskingTempInfo.totalAttacksInTownInPeriod += this->militaryAIInfo->GetAttacksInTownCountFromPlayerInPeriod(enemyPlayerId, periodStartTime, global->currentGameTime);
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
			if (this->activeGroupsTaskingTempInfo.myMainCentralUnit != NULL) {
				float diffX = this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionX - loopUnitCommander->positionX;
				float diffY = this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionY - loopUnitCommander->positionY;
				float loopGrpSquareDist = (diffX*diffX) + (diffY*diffY);
				if (loopGrpSquareDist < AI_CONST::townSizeSquare) {
					if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK) { this->activeGroupsTaskingTempInfo.townLandAttackGroupCount++; }
					if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_DEFEND) { this->activeGroupsTaskingTempInfo.townLandDefendGroupCount++; }
					if (loopUnitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE) { this->activeGroupsTaskingTempInfo.townLandExploreGroupCount++; }
				}
			}
		}
		loopUnitGroup = loopUnitGroup->next;
	}
	this->activeGroupsTaskingTempInfo.totalLandGroups = this->activeGroupsTaskingTempInfo.totalLandAttackGroupCount + 
		this->activeGroupsTaskingTempInfo.totalLandDefendGroupCount + this->activeGroupsTaskingTempInfo.totalLandExploreGroupCount;

	this->activeGroupsTaskingTempInfo.mainCentralUnitIsVital = false;
	if (this->activeGroupsTaskingTempInfo.myMainCentralUnit && this->activeGroupsTaskingTempInfo.myMainCentralUnit->unitDefinition) {
		STRUCT_UNITDEF_BASE *myMainCentralUnitDef = this->activeGroupsTaskingTempInfo.myMainCentralUnit->unitDefinition;
		this->activeGroupsTaskingTempInfo.mainCentralUnitIsVital = ((myMainCentralUnitDef->unitAIType == TribeAIGroupCivilian) ||
			(myMainCentralUnitDef->unitAIType == TribeAIGroupPriest) ||
			(myMainCentralUnitDef->DAT_ID1 == CST_UNITID_FORUM));
	}

	this->EvaluateMilitarySituation(tacAI);
	// Save the military situation evaluation in military AI object.
	this->militaryAIInfo->SetLastKnownMilitarySituation(this->activeGroupsTaskingTempInfo.militarySituation, global->currentGameTime);
	this->CollectEnemyUnitsNearMyMainUnit(player);

	this->activeGroupsTaskingTempInfo.mainUnitSearchZoneRadius = 5;
	this->activeGroupsTaskingTempInfo.mainUnitProtectionRadius = 5;
	if (this->activeGroupsTaskingTempInfo.myMainCentralUnit && this->activeGroupsTaskingTempInfo.myMainCentralUnit->unitDefinition &&
		(this->activeGroupsTaskingTempInfo.myMainCentralUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM)) {
		this->activeGroupsTaskingTempInfo.mainUnitSearchZoneRadius = AI_CONST::townSize + 5;
		this->activeGroupsTaskingTempInfo.mainUnitProtectionRadius = 7;
	}

#ifdef _DEBUG
	this->lastDebugInfo = "";
	/*long int t = AOE_METHODS::TimeGetTime();
	long int spent = t - processStartTimeGetTimeValue;
	if (spent >= allowedTime) {
		CallWriteCenteredText("Spent too much time");
	} else {
		CallWriteCenteredText("timing ok");
	}*/
#endif

	return true; // Normal case: allow processing the groups
}


// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
// For all non-supported (or without custom treatment) cases, just return false !
// Note: OnTaskActiveGroupsBegin has been called once before calling this (n' times)
bool UnitGroupAI::TaskActiveUnitGroup(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup) {
	if (!tacAI || !tacAI->IsCheckSumValid() || !unitGroup || !unitGroup->IsCheckSumValid()) {
		return false;
	}
	// Restrict to supported unit group type
	if ((unitGroup->unitGroupType != UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK) &&
		(unitGroup->unitGroupType != UNIT_GROUP_TYPES::CST_UGT_LAND_DEFEND) &&
		(unitGroup->unitGroupType != UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE)) {
		return false;
	}

#ifdef _DEBUG
	if ((unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_15) ||
		(unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_ROUNDUP_TARGET) || // never used as a "current task" ?
		(unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_UNKNOWN_05) || 
		(unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_UNKNOWN_06) || 
		(unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_UNKNOWN_11) || 
		(unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_UNKNOWN_12) || 
		(unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_EXPLORE) || 
		(unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_UNKNOWN_16) ) {
		assert(false && "Found an group task that requires debugging !");
	}
#endif

	// To prevent "stuck groups": incomplete groups sometimes remain stuck in "regroup" task and are not reorganized using idle units.
	// Actually, the issue might rather be in group handling TacAI phases, not here...
	if ((unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_REGROUP) ||
		(unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_REGROUP_NO_SPACING)) {
		long int totalSoldiersCount = tacAI->landMilitaryUnits.usedElements;
		STRUCT_UNIT_GROUP *fake = &tacAI->fakeFirstUnitGroupElem;
		STRUCT_UNIT_GROUP *cur = fake->next;
		long int curCount = 0;
		while (cur && (cur->next != fake)) {
			if ((cur->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK) ||
				(cur->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_DEFEND) ||
				(cur->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE)) {
				curCount += cur->unitCount;
			}
			cur = cur->next;
		}
		if ((unitGroup->unitCount < unitGroup->desiredUnitCount) && (curCount < totalSoldiersCount)) {
			// There are idle military units, and this group is incomplete
			// "kill" current group to force a reorganization
			// TODO Use a delay ? And also, improve the conditions if possible...
			unitGroup->unitCount = 0;
			return true;
		}
	}

	// Restrict to supported tasks
	if ((unitGroup->currentTask != UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02) &&
		(unitGroup->currentTask != UNIT_GROUP_TASK_IDS::CST_UGT_EXTERMINATE) &&
		(unitGroup->currentTask != UNIT_GROUP_TASK_IDS::CST_UGT_EXPLORE) &&
		(unitGroup->currentTask != UNIT_GROUP_TASK_IDS::CST_UGT_RETREAT) &&
		(unitGroup->currentTask != UNIT_GROUP_TASK_IDS::CST_UGT_DEFEND_UNIT)) {
		return false;
	}

	// Get some main objects
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	STRUCT_AI *mainAI = tacAI->ptrMainAI;
	if (!mainAI || !mainAI->IsCheckSumValid()) { return false; }
	STRUCT_PLAYER *player = mainAI->ptrStructPlayer; // Current AI player
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
		// Most frequent case (cf 04D4722)
		// Manage transports/being transported/waiting...
		// Retreat: check if "arrived" ?
		// exterminate: search a target (whatever current activity is ?) and switch to attack(2)
		// attack: if valid target, call task to attack roundup target(20) but doesn't switch to it? switch to explore or retreat if target destroyed? switch to idle otherwise
		// defend unit
	}

	if ((unitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE) &&
		(this->activeGroupsTaskingTempInfo.militarySituation != MILITARY_SITUATION::MS_CRITICAL) &&
		(this->activeGroupsTaskingTempInfo.militarySituation != MILITARY_SITUATION::MS_WEAK)) {
		// For explore groups, let ROR code handle all other situations than "critical".
		// TODO: explore groups do not respond to attacks, we could do something here ? Or rather in tacAI.react to event maybe
		return false;
	}

	STRUCT_UNIT_BASE *myMainCentralUnit = this->activeGroupsTaskingTempInfo.myMainCentralUnit;
	assert(myMainCentralUnit && myMainCentralUnit->IsCheckSumValidForAUnitClass());

	// TODO: when unitGroup->isTasked == 1 and no recent attacks in my town, use a delay ? (be careful, more than 1 group may be concerned)
	/*if ((unitGroup->isTasked == 1) && (this->activeGroupsTaskingTempInfo.totalPanicModesInPeriod == 0)) {
		return false; // Let ROR code handle this case
	}*/

	STRUCT_UNIT_BASE *commander = global->GetUnitFromId(unitGroup->commanderUnitId); // Group leader. Guaranteed non-NULL
	if (!commander) {
		std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(".group#") +
			std::to_string(unitGroup->unitGroupId) + std::string(" has an invalid leader ID");
		traceMessageHandler.WriteMessage(msg);
		return false;
	}
	if (commander->ptrStructPlayer != player) {
		std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(".group#") +
			std::to_string(unitGroup->unitGroupId) + std::string(" has an non-owned leader ID");
		traceMessageHandler.WriteMessage(msg);
		return false;
	}
	// Variables depending on unit group location
	this->curGroupDistanceToMainUnit = -1;
	this->curUnitGroupOrientationXFromMainUnit = 0; // -1 if group is "left" of main unit, 1 if on the "right" (X axis)
	this->curUnitGroupOrientationYFromMainUnit = 0; // -1 if group is "down" of main unit, 1 if "upper" (Y axis)
	if (myMainCentralUnit && myMainCentralUnit->IsCheckSumValidForAUnitClass()) {
		this->curGroupDistanceToMainUnit = GetDistance(commander->positionX, commander->positionY, myMainCentralUnit->positionX, myMainCentralUnit->positionY);
		if (commander->positionX < myMainCentralUnit->positionX) {
			this->curUnitGroupOrientationXFromMainUnit = -1;
		}
		if (commander->positionX > myMainCentralUnit->positionX) {
			this->curUnitGroupOrientationXFromMainUnit = 1;
		}
		if (commander->positionY < myMainCentralUnit->positionY) {
			this->curUnitGroupOrientationYFromMainUnit = -1;
		}
		if (commander->positionY > myMainCentralUnit->positionY) {
			this->curUnitGroupOrientationYFromMainUnit = 1;
		}
	}


#ifdef _DEBUG
	this->lastDebugInfo += "group#" + std::to_string(unitGroup->unitGroupId) + ": mstatus=" + std::to_string(this->activeGroupsTaskingTempInfo.militarySituation) + " ";
#endif

	// Manage explore groups here
	if (unitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_EXPLORE) {
		this->TaskActiveExploreGroup(player, unitGroup);
	}
	// Now explore groups have been dealt with

	if (this->activeGroupsTaskingTempInfo.mainCentralUnitIsVital) {
		if (this->activeGroupsTaskingTempInfo.militarySituation == MILITARY_SITUATION::MS_CRITICAL) {
			// Critical with a unit to defend: see dedicated method.
			return this->TaskActiveAttackGroupCriticalWithVitalMainUnit(player, unitGroup);
		}
		if (this->activeGroupsTaskingTempInfo.militarySituation == MILITARY_SITUATION::MS_WEAK) {
			// Weak with a unit to defend: see dedicated method.
			this->TaskActiveAttackGroupWeakWithVitalMainUnit(player, unitGroup);
		}
	} else {
		// No TC/villager/priest to protect. This may be a scenario or a game-end situation where I am probably gonna lose
		// Waiting and remaining idle could make the situation worse here
		// TODO: if I have an allied player, consider running to his town or helping him in battle
		// TODO: check for priority target ?
		if ((this->activeGroupsTaskingTempInfo.militarySituation == MILITARY_SITUATION::MS_CRITICAL) ||
			(this->activeGroupsTaskingTempInfo.militarySituation == MILITARY_SITUATION::MS_WEAK)) {
#ifdef _DEBUG
			this->lastDebugInfo += "No-TC/villager/priest situation: use normal code\n";
#endif
			return false;
		}
	}

	// Remaining: non-critical and non-weak situations
	if (unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02) {
		if (unitGroup->taskSubTypeId != -1) {
#ifdef _DEBUG
			this->lastDebugInfo += "Protect/capture: use normal code\n";
#endif
			return false; // Let ROR handle this...
		}
		return this->TaskActiveAttackGroup(player, unitGroup);
	}


	if (this->activeGroupsTaskingTempInfo.mainCentralUnitIsVital) {
		// If non weak and no target then explore ?
	}


	// TODO: no group in my town and few resources (myWeaknessScore) : should depend on random...
	// ...and player "personality": AI players still should try to attack early in both RM/DM, for example. But not always ? If "I sent" a group to attack, make sure I don't recall it here just after
	// Also, take into account strategy progress: if I am attacking for 10th time in "weak" situation and didn't make any progress in strategy (in RM), then stop it ?

	//this->priorityLocation

	// Could use mainAI->structInfAI.attacksHistory too

	// TODO: attack, lastupdate long time ago, target not visible: force change ?

#ifdef _DEBUG
	this->lastDebugInfo += "Default: use normal code\n";
#endif
	return false; //TO DO (let ROR code be executed)
}


// Triggered when a member of unit group is being attacked. Does nothing if unitGroup==NULL.
void UnitGroupAI::OnUnitGroupAttacked(STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *myUnit, STRUCT_UNIT_BASE *enemyUnit) {
	if (!unitGroup || !unitGroup->IsCheckSumValid() || !myUnit || !myUnit->IsCheckSumValidForAUnitClass() ||
		!enemyUnit || !enemyUnit->IsCheckSumValidForAUnitClass() || !myUnit->ptrStructPlayer) {
		return;
	}
	STRUCT_GAME_GLOBAL *global = myUnit->ptrStructPlayer->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return; }
	if (GetGameSettingsPtr()->rgeGameOptions.difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_MEDIUM) {
		return; // easy/medium: do not use this feature.
	}

	// Make sure unit group has not already been tasked very recently (to avoid accumulation of orders/counter-orders)
	if (unitGroup &&
		(global->currentGameTime - unitGroup->lastUpdateTime_ms >= AI_CONST::minimumDelayBetweenBasicUnitGroupAttackTasking_ms)) {
		// Unit is part of a group. Check if other members can help
		for (int i = 0; i < myUnit->unitIDsInMyGroup.usedElements; i++) {
			long int alliedUnitId = myUnit->unitIDsInMyGroup.unitIdArray[i];
			STRUCT_UNIT_BASE *alliedUnit = global->GetUnitFromId(alliedUnitId);
			if (alliedUnit && alliedUnit->IsCheckSumValidForAUnitClass() && (alliedUnit != myUnit) && alliedUnit->currentActivity &&
				AOE_METHODS::UNIT::CanAttackTarget(alliedUnit, enemyUnit)) {
				STRUCT_UNIT_BASE *alliedUnitCurTarget = global->GetUnitFromId(alliedUnit->currentActivity->targetUnitId);
				bool canChangeTarget = (alliedUnitCurTarget == NULL);
				if (alliedUnitCurTarget && alliedUnitCurTarget->IsCheckSumValidForAUnitClass() && alliedUnitCurTarget->unitDefinition) {
					// Note: do not interrupt allied units that are attacking villagers or wonders
					canChangeTarget = (alliedUnitCurTarget->unitDefinition->unitAIType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian) &&
						!UnitDefCanAttack(alliedUnitCurTarget->unitDefinition) && (alliedUnitCurTarget->unitDefinition->DAT_ID1 != CST_UNITID_WONDER);
				}
				if (canChangeTarget) {
					long int sqrdist = (long int)((alliedUnit->positionX - enemyUnit->positionX) * (alliedUnit->positionX - enemyUnit->positionX) +
						(alliedUnit->positionY - enemyUnit->positionY) * (alliedUnit->positionY - enemyUnit->positionY));
					if (sqrdist < ROCKNROR::COMBAT::COMBAT_CONST::distanceAlwaysTaskIdleMilitaryUnitsOnAttack * ROCKNROR::COMBAT::COMBAT_CONST::distanceAlwaysTaskIdleMilitaryUnitsOnAttack) {
						GAME_COMMANDS::CreateCmd_RightClick(alliedUnit->unitInstanceId, enemyUnit->unitInstanceId,
							enemyUnit->positionX, enemyUnit->positionY);
						unitGroup->lastUpdateTime_ms = global->currentGameTime;
					}
				}
			}
		}
	}
}


// Task an active attack group, when player situation is critical AND there is a vital central unit (like TC, villager).
// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
// The main idea in this case is to retreat at all costs and protect "main" unit
bool UnitGroupAI::TaskActiveAttackGroupCriticalWithVitalMainUnit(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup) {
	STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !player || !player->ptrAIStruct) { return false; }
	STRUCT_UNIT_BASE *commander = global->GetUnitFromId(unitGroup->commanderUnitId); // Group leader. Guaranteed non-NULL
	STRUCT_TAC_AI *tacAI = &player->ptrAIStruct->structTacAI;
	if (!this->activeGroupsTaskingTempInfo.myMainCentralUnit) { return false; } // should NOT be NULL in this case
	
	bool commanderHasMilitaryTarget = HasVisibleMilitaryTarget(commander);
	bool commanderTargetIsInTown = false;
	STRUCT_ACTION_BASE *action = AOE_STRUCTURES::GetUnitAction(commander);
	if (action) {
		std::pair<float, float> targetPosition = GetActionTargetPosition(action);
		float diffX = (targetPosition.first- this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionX);
		float diffY = (targetPosition.second - this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionY);
		if ((-this->activeGroupsTaskingTempInfo.mainUnitProtectionRadius <= diffX) && (diffX <= this->activeGroupsTaskingTempInfo.mainUnitProtectionRadius) &&
			(-this->activeGroupsTaskingTempInfo.mainUnitProtectionRadius <= diffY) && (diffY <= this->activeGroupsTaskingTempInfo.mainUnitProtectionRadius)) {
			commanderTargetIsInTown = true;
		}
	}
	bool doNotRetaskCommander = commanderHasMilitaryTarget || commanderTargetIsInTown;
	if (doNotRetaskCommander && unitGroup->isTasked &&
		(global->currentGameTime - unitGroup->lastUpdateTime_ms < AI_CONST::minimumDelayBetweenBasicUnitGroupAttackTasking_ms)) {
		return false;
	}

	int totalGroupsInTown = this->activeGroupsTaskingTempInfo.townLandAttackGroupCount + this->activeGroupsTaskingTempInfo.townLandDefendGroupCount + this->activeGroupsTaskingTempInfo.townLandExploreGroupCount;

	float distanceToMyMainUnit = -1;
	if (this->activeGroupsTaskingTempInfo.myMainCentralUnit && this->activeGroupsTaskingTempInfo.myMainCentralUnit->IsCheckSumValidForAUnitClass()) {
		distanceToMyMainUnit = GetDistance(commander->positionX, commander->positionY, this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionX, this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionY);
	}
	// Unit group is far from TC: force it to retreat (or attack a unit in my town)
	if (distanceToMyMainUnit > AI_CONST::townSize + 5) {
		// Randomly choose a zone to "defend" : TC or last (in-town) attack position
		int randomValue = randomizer.GetRandomPercentageValue();
		float targetPosX = militaryAIInfo->recentAttacksByPlayer->lastAttackInMyTownPosX; // may be -1
		float targetPosY = militaryAIInfo->recentAttacksByPlayer->lastAttackInMyTownPosY; // may be -1
		/*if (randomValue <= 40) {
		targetPosX = myMainCentralUnit->positionX + (mainUnitProtectionRadius * unitGroupOrientationXFromMainUnit);
		targetPosY = myMainCentralUnit->positionY + (mainUnitProtectionRadius * unitGroupOrientationYFromMainUnit);
		}*/
		STRUCT_UNIT_MEMORY *targetElem = this->GetUnitMemoryElemEnemyUnitCloserToPosition(player, (long int)targetPosX, (long int)targetPosY);
		// Check that targetElem refers to an enemy unit
		bool canAttackTargetElem = (targetElem != nullptr) && (AOE_STRUCTURES::PLAYER::GetDiplomacyValueForPlayer(player, targetElem->playerId) >= CST_PDV_NEUTRAL);
		UNIT_GROUP_TASK_IDS result = UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET;
		if (canAttackTargetElem) {
			result = this->AttackOrRetreat(&player->ptrAIStruct->structTacAI, unitGroup, targetElem, targetPosX, targetPosY, true);
		} else {
			UNIT_GROUP_TASK_IDS result = this->AttackOrRetreat(&player->ptrAIStruct->structTacAI, unitGroup, this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnitInfAIMemoryElem,
				targetPosX, targetPosY, true);
		}
		if (result != UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET) {
#ifdef _DEBUG
			std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(" Ordered+force taskId=") +
				std::to_string(result) + std::string(" due to critical situation, wkn=");
			msg += std::to_string(this->activeGroupsTaskingTempInfo.myWeaknessScore) + std::string(" grpCnt=") + std::to_string(totalGroupsInTown);
			//CallWriteText(msg.c_str());
			this->lastDebugInfo += msg + std::string("\n");
#endif
			return true; // Unit group has been tasked
		}
	} else {
		// Unit group is already in (or near) town
		if (global->currentGameTime - unitGroup->lastUpdateTime_ms > (1000 * tacAI->SNNumber[SNTacticalUpdateFrequency])) {
			STRUCT_UNIT_MEMORY *targetElem = this->GetUnitMemoryElemEnemyUnitCloserToPosition(player, (long int)commander->positionX, (long int)commander->positionY);
			// Check that targetElem refers to an enemy unit
			bool canAttackTargetElem = (targetElem != nullptr) && (AOE_STRUCTURES::PLAYER::GetDiplomacyValueForPlayer(player, targetElem->playerId) >= CST_PDV_NEUTRAL);

			UNIT_GROUP_TASK_IDS result = UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET;
			// Attack enemy if visible/if we know a recent location. Otherwise, do nothing (we don't set a retreat position in AttackOrRetreat call)
			if (canAttackTargetElem) {
				result = this->AttackOrRetreat(tacAI, unitGroup, targetElem, -1, -1, true);
			} else {
				result = this->AttackOrRetreat(tacAI, unitGroup, this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnitInfAIMemoryElem, -1, -1, false);
			}
			if (result != UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET) {
#ifdef _DEBUG
				std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(" Ordered taskId=") +
					std::to_string(result) + std::string(" due to weakness, wkn=");
				msg += std::to_string(this->activeGroupsTaskingTempInfo.myWeaknessScore) + std::string(" grpCnt=") + std::to_string(totalGroupsInTown);
				//CallWriteText(msg.c_str());
				this->lastDebugInfo += msg + std::string("\n");
#endif
				return true; // Unit group has been tasked
			}
#ifdef _DEBUG
			this->lastDebugInfo += "Already in town, enemy not visible, stand by (skip)\n";
#endif
			return true; // no target, do nothing special (stand by - prevent an attack to some target outta town !)
		}
		// TODO: if currently no target AND we know a "good" target in town, force doing something even if delay has not passed ?
#ifdef _DEBUG
		this->lastDebugInfo += "Already in town, tasking delay not passed, stand by (skip)\n";
#endif
		return true; // Wait a bit more time...
	}
#ifdef _DEBUG
	this->lastDebugInfo += "stand by (skip)\n";
#endif
	return true;
}


// Task an active attack group, when player situation is "weak" AND there is a vital central unit (like TC, villager).
// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
bool UnitGroupAI::TaskActiveAttackGroupWeakWithVitalMainUnit(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup) {
	STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !player || !player->ptrAIStruct) { return false; }
	STRUCT_UNIT_BASE *commander = global->GetUnitFromId(unitGroup->commanderUnitId); // Group leader. Guaranteed non-NULL
	STRUCT_TAC_AI *tacAI = &player->ptrAIStruct->structTacAI;
	bool commanderHasMilitaryTarget = HasVisibleMilitaryTarget(commander);
	if (unitGroup->isTasked &&
		// TODO: could use a longer delay than minimumDelayBetweenBasicUnitGroupAttackTasking_ms
		(global->currentGameTime - unitGroup->lastUpdateTime_ms < AI_CONST::minimumDelayBetweenBasicUnitGroupAttackTasking_ms)) {
		return false;
	}

	UnitGroupDetailedInfo detailedInfo;
	this->CollectInfoAboutGroup(player, unitGroup, &detailedInfo);
	float distanceToMyMainUnit = -1;
	if (this->activeGroupsTaskingTempInfo.myMainCentralUnit && this->activeGroupsTaskingTempInfo.myMainCentralUnit->IsCheckSumValidForAUnitClass()) {
		distanceToMyMainUnit = GetDistance(commander->positionX, commander->positionY, this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionX, this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionY);
	}

	// Help leader if under attack
	if (detailedInfo.unitIdAttackingMyLeader > -1) {
		long int newTargetId = detailedInfo.unitIdAttackingMyLeader;
		STRUCT_UNIT_BASE *newTargetUnit = global->GetUnitFromId(newTargetId);
		if (newTargetUnit && newTargetUnit->IsCheckSumValidForAUnitClass()) {
			this->SetUnitGroupTarget_internal(unitGroup, newTargetUnit);
			this->SetUnitGroupCurrentTask(tacAI, unitGroup, UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02, 0, false);
#ifdef _DEBUG
			this->lastDebugInfo += "weak situation: set target to help leader (under attack)\n";
#endif
			return true; // unit group has been tasked
		}
	}

	// Help other units if under attack
	// TODO: Use condition: detailedInfo.unitGroupIsAlmostIdle ? According to personality ? Random ? Difficulty
	if (!detailedInfo.unitsAttackingMyGroup.empty()) {
		long int newTargetId = detailedInfo.unitsAttackingMyGroup.front();
		STRUCT_UNIT_BASE *newTargetUnit = global->GetUnitFromId(newTargetId);
		if (newTargetUnit && newTargetUnit->IsCheckSumValidForAUnitClass()) {
			this->SetUnitGroupTarget_internal(unitGroup, newTargetUnit);
			this->SetUnitGroupCurrentTask(tacAI, unitGroup, UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02, 0, false);
#ifdef _DEBUG
			this->lastDebugInfo += "weak situation: set target from list of units attacking my group\n";
#endif
			return true; // unit group has been tasked
		}
	}

	// Note: GetNumberOfUnitsUnderAttack should be 0 here as we already treated such cases just above (help if under attack)

	// TODO: if group is away from town, choose between retreating and other attack actions
	// TODO: group might be away, but protecting villagers (eg a storage pit near gold that has been attacked)
	// TODO: On the contrary: it might be important to allow defending villagers outside the town
	bool groupIsAway = (distanceToMyMainUnit > AI_CONST::townSize + 5);
	bool forceRetreat = false;
	if (groupIsAway) {
		forceRetreat = false; // TODO: set true according other variables, random... ?
	}

	if (!forceRetreat && detailedInfo.unitGroupIsAlmostIdle) {
		if (this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit && this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnitIsCurrentlyVisible) {
			this->SetUnitGroupTarget_internal(unitGroup, this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit);
			this->SetUnitGroupCurrentTask(tacAI, unitGroup, UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02, 1, false);
#ifdef _DEBUG
			this->lastDebugInfo += "weak situation: set target = target found near leader\n";
#endif
			return true; // unit group has been tasked
		}
	}

	if (detailedInfo.unitGroupIsAlmostIdle) {
		assert(this->activeGroupsTaskingTempInfo.myMainCentralUnit != NULL); // guaranteed by top "if" condition

		// If group is idle and there is some enemy building in my town, attack it (be careful with towers if I am too weak ?)
		AOE_STRUCTURES::STRUCT_UNIT_MEMORY *targetInTown = ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(this->militaryAIInfo->unitIdEnemyTowerInMyTown, player->playerId);
		if (!targetInTown) {
			targetInTown = ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(this->militaryAIInfo->unitIdEnemyBuildingInMyTown, player->playerId);
		}
		if (targetInTown) {
			UNIT_GROUP_TASK_IDS result = this->AttackOrRetreat(tacAI, unitGroup, targetInTown, -1, -1, false);
			if (result != UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET) {
#ifdef _DEBUG
				std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(" attack enemy building in my town, weak case, wkn=");
				msg += std::to_string(this->activeGroupsTaskingTempInfo.myWeaknessScore) + std::string(" grpCntTwn=") + std::to_string(this->activeGroupsTaskingTempInfo.townLandAttackGroupCount + this->activeGroupsTaskingTempInfo.townLandDefendGroupCount + this->activeGroupsTaskingTempInfo.townLandExploreGroupCount);
				this->lastDebugInfo += msg + std::string("\n");
#endif
				return true; // Unit group has been tasked
			}
		}

		// No specific target: just fall back in town
		float retreatPosX = this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionX + (this->activeGroupsTaskingTempInfo.mainUnitProtectionRadius * this->curUnitGroupOrientationXFromMainUnit);
		float retreatPosY = this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionY + (this->activeGroupsTaskingTempInfo.mainUnitProtectionRadius * this->curUnitGroupOrientationYFromMainUnit);
		UNIT_GROUP_TASK_IDS result = this->AttackOrRetreat(tacAI, unitGroup, NULL, retreatPosX, retreatPosY, false);
		if (result != UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET) {
#ifdef _DEBUG
			std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(" Ordered taskId=") +
				std::to_string(result) + std::string(" for idle grp, weak case, wkn=");
			msg += std::to_string(this->activeGroupsTaskingTempInfo.myWeaknessScore) + std::string(" grpCntTwn=") + std::to_string(this->activeGroupsTaskingTempInfo.townLandAttackGroupCount + this->activeGroupsTaskingTempInfo.townLandDefendGroupCount + this->activeGroupsTaskingTempInfo.townLandExploreGroupCount);
			//CallWriteText(msg.c_str());
			this->lastDebugInfo += msg + std::string("\n");
#endif
			return true; // Unit group has been tasked
		}
		// Retreating failed ?
	}
	if (groupIsAway) {
		return false; // Let ROR do something with this group. Being idle out of nowhere is no use !
	}

	// TODO: if I have priority targets from TARGETING classes, consider attacking them
	// ... at least in my town or if related to victory conditions
#ifdef _DEBUG
	this->lastDebugInfo += "stand by to avoid suicidal attacking (skip)\n";
#endif
	return true; // Stand by = prevent ROR from sending this group attacking some non-priority target
}


// Task an active attack group, when player situation is not critical/weak.
// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
bool UnitGroupAI::TaskActiveAttackGroup(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup) {
	// Is current target valid ? Visible ?
	// If no (valid) current target then attack nearby units or units attacking my leader/my group
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid() || !player || !player->ptrAIStruct) { return false; }
	STRUCT_UNIT_BASE *currentTargetUnit = global->GetUnitFromId(unitGroup->targetUnitId);
	STRUCT_UNIT_BASE *commanderUnit = global->GetUnitFromId(unitGroup->commanderUnitId);
	if (!commanderUnit || !commanderUnit->IsCheckSumValidForAUnitClass()) { return false; }
	STRUCT_TAC_AI *tacAI = &player->ptrAIStruct->structTacAI;
	STRUCT_INF_AI *infAI = &player->ptrAIStruct->structInfAI;
	assert(tacAI && tacAI->IsCheckSumValid() && infAI && infAI->IsCheckSumValid());
	if (!tacAI || !tacAI->IsCheckSumValid() || !infAI || !infAI->IsCheckSumValid()) { return false; }

	float distanceToMyMainUnit = -1;
	if (this->activeGroupsTaskingTempInfo.myMainCentralUnit && this->activeGroupsTaskingTempInfo.myMainCentralUnit->IsCheckSumValidForAUnitClass()) {
		distanceToMyMainUnit = GetDistance(commanderUnit->positionX, commanderUnit->positionY, this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionX, this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionY);
	}
	bool groupIsAway = (distanceToMyMainUnit > AI_CONST::townSize + 5);

	STRUCT_PLAYER *currentTargetPlayer = NULL;
	if (currentTargetUnit) {
		currentTargetPlayer = currentTargetUnit->ptrStructPlayer;
	}
	bool allyOrGaiaOrSelf = true;
	if (currentTargetPlayer) {
		allyOrGaiaOrSelf = (player->diplomacyVSPlayers[currentTargetPlayer->playerId] <= PLAYER_DIPLOMACY_VALUES::CST_PDV_ALLY);
	}
	if (allyOrGaiaOrSelf) {
		currentTargetUnit = NULL;
		currentTargetPlayer = NULL;
	}
	bool targetPosIsKnown = currentTargetUnit && currentTargetUnit->unitDefinition && currentTargetUnit->unitDefinition->visibleInFog;
	if (currentTargetUnit && !targetPosIsKnown) {
		targetPosIsKnown = IsUnitPositionKnown(player, currentTargetUnit);
	}
	if (!currentTargetUnit) {
		this->SetUnitGroupTarget_internal(unitGroup, NULL);
	}
	// Current target is not visible
	if (currentTargetUnit && !targetPosIsKnown) {
		// Search if there is some other target at reach
		STRUCT_ACTION_BASE *commanderAction = AOE_STRUCTURES::GetUnitAction(commanderUnit);
		if (commanderAction && (commanderAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_ATTACK)) {
			STRUCT_UNIT_BASE *newTargetUnit = commanderAction->targetUnit;
			if (!newTargetUnit) {
				newTargetUnit = global->GetUnitFromId(commanderAction->targetUnitId);
			}
			bool newTargetIsVisible = newTargetUnit && PLAYER::IsFogVisibleForPlayer(player, (long int)newTargetUnit->positionX, (long int)newTargetUnit->positionY);
			if (newTargetUnit && newTargetUnit->unitDefinition && newTargetUnit->ptrStructPlayer && newTargetIsVisible) {
#ifdef _DEBUG
				std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string("Target #") + 
					std::to_string(unitGroup->targetUnitId) + (" can't be located. Set new target = leader's target (") +
					std::to_string(commanderAction->targetUnitId) + std::string(")\n");
				this->lastDebugInfo += msg;
#endif
				this->SetUnitGroupTarget_internal(unitGroup, newTargetUnit);
				// Set unit group task, but do not force (if units already have an activity, let them)
				// Remark: there is no risk this "tasking" occurs too many times in a row, because here the case is "I have a target that is not visible" and we set a visible target.
				this->SetUnitGroupCurrentTask(&player->ptrAIStruct->structTacAI, unitGroup, UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_02, 1, false);
				return true;
			}
			if (global->currentGameTime - unitGroup->lastAttackTaskingTime_ms < AI_CONST::delayBetweenUnitGroupAttackTasking_ms) {
				return true;
			}
			// Commander has no target. Continue attacking "non-visible" one = go to last known position using infAI elem.
			// Find infAI elem for currentTargetUnit
			AOE_STRUCTURES::STRUCT_UNIT_MEMORY *elem = ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(currentTargetUnit->unitInstanceId, &tacAI->ptrMainAI->structInfAI);
			if (elem) {
				this->AttackOrRetreat(tacAI, unitGroup, NULL, elem->posX, elem->posY, false);
				return true;
			} else {
				// The target is unknown in infAI list. Prevent the game code from possibly cheating here. We'll have to find a new valid target.
				this->SetUnitGroupTarget_internal(unitGroup, NULL);
			}
		}
		// Is some unit attacking the leader or some unit of the group (or some of my nearby units ?)
		// TODO
	}

	if (global->currentGameTime - unitGroup->lastAttackTaskingTime_ms < AI_CONST::minimumDelayBetweenBasicUnitGroupAttackTasking_ms) {
		return false;
	}

	// No target, group in town, commander is idle
	if (!currentTargetUnit && !groupIsAway && AOE_METHODS::UNIT::IsUnitIdle(commanderUnit)) {
		// If group is idle and there is some enemy building in my town, attack it (be careful with towers if I am too weak ?)
		AOE_STRUCTURES::STRUCT_UNIT_MEMORY *targetInTown = ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(this->militaryAIInfo->unitIdEnemyTowerInMyTown, player->playerId);
		if (!targetInTown) {
			targetInTown = ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(this->militaryAIInfo->unitIdEnemyBuildingInMyTown, player->playerId);
		}
		if (targetInTown) {
			UNIT_GROUP_TASK_IDS result = this->AttackOrRetreat(tacAI, unitGroup, targetInTown, -1, -1, false);
			if (result != UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET) {
#ifdef _DEBUG
				std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(" attack enemy building in my town, std case, wkn=");
				msg += std::to_string(this->activeGroupsTaskingTempInfo.myWeaknessScore) + std::string(" grpCntTwn=") + std::to_string(this->activeGroupsTaskingTempInfo.townLandAttackGroupCount + this->activeGroupsTaskingTempInfo.townLandDefendGroupCount + this->activeGroupsTaskingTempInfo.townLandExploreGroupCount);
				this->lastDebugInfo += msg + std::string("\n");
#endif
				return true; // Unit group has been tasked
			}
		}
	}

	if (this->gameDiffLevel > GAME_DIFFICULTY_LEVEL::GDL_HARD) {
		return false;
	}

	// Idle group: assist active units, if any (attacking something OR being attacked)
	// Otherwise, enter "exterminate" mode (explore to find a target)
	for (int curIndex = 0; curIndex < STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT; curIndex++) {
		long int curUnitId = unitGroup->GetMyUnitId(curIndex);
		STRUCT_UNIT_BASE *curUnit = global->GetUnitFromId(curUnitId);
		if (!curUnit || !curUnit->IsCheckSumValidForAUnitClass() || !curUnit->currentActivity) { continue; }
		long int unitIdToTarget = -1;
		// Is this group unit being attacked ?
		int attackingMeCount = curUnit->currentActivity->unitIDsThatAttackMe.usedElements;
		if (attackingMeCount > 0) {
			unitIdToTarget = curUnit->currentActivity->unitIDsThatAttackMe.unitIdArray[0];
		}
		// Is this group unit attacking some target ?
		if ((unitIdToTarget == -1) && (curUnit->currentActivity->targetUnitId >= 0)) {
			STRUCT_UNIT_BASE *curTarget = global->GetUnitFromId(curUnit->currentActivity->targetUnitId);
			if (curTarget && curTarget->IsCheckSumValidForAUnitClass() && (curTarget->unitStatus <= GAME_UNIT_STATUS::GUS_2_READY) &&
				(player->ptrDiplomacyStances[curTarget->ptrStructPlayer->playerId] == PLAYER_DIPLOMACY_STANCES::CST_PDS_ENEMY)) {
				unitIdToTarget = curUnit->currentActivity->targetUnitId;
			}
		}
		// If we found a target, assign it to other group units
		if (unitIdToTarget >= 0) {
			AOE_STRUCTURES::STRUCT_UNIT_MEMORY *elem = ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(unitIdToTarget, infAI);
			bool canAttackTargetElem = (elem != nullptr) && (AOE_STRUCTURES::PLAYER::GetDiplomacyValueForPlayer(player, elem->playerId) >= CST_PDV_NEUTRAL);
			if (canAttackTargetElem) {
				UNIT_GROUP_TASK_IDS result = this->AttackOrRetreat(tacAI, unitGroup, elem, -1, -1, false);
				if (result != UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET) {
#ifdef _DEBUG
					std::string msg = std::string("p#") + std::to_string(player->playerId) + std::string(" : idle => assist other group unit, wkn=");
					msg += std::to_string(this->activeGroupsTaskingTempInfo.myWeaknessScore) + std::string(" grpCntTwn=") + std::to_string(this->activeGroupsTaskingTempInfo.townLandAttackGroupCount + this->activeGroupsTaskingTempInfo.townLandDefendGroupCount + this->activeGroupsTaskingTempInfo.townLandExploreGroupCount);
					this->lastDebugInfo += msg + std::string("\n");
#endif
					return true; // Unit group has been tasked
				}
			}
		}
	}
	
	this->SetUnitGroupCurrentTask(tacAI, unitGroup, UNIT_GROUP_TASK_IDS::CST_UGT_EXTERMINATE, 0, 0);

	return false;
}


// Task an active explore group
// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
bool UnitGroupAI::TaskActiveExploreGroup(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid() || !player || !player->ptrAIStruct) { return false; }
	STRUCT_TAC_AI *tacAI = &player->ptrAIStruct->structTacAI;

	bool discardExploreAndRetreat = (this->activeGroupsTaskingTempInfo.militarySituation == MILITARY_SITUATION::MS_CRITICAL);
	if (this->activeGroupsTaskingTempInfo.mainCentralUnitIsVital) {
		// If we have a town to defend, take less risks
		if (this->activeGroupsTaskingTempInfo.militarySituation == MILITARY_SITUATION::MS_WEAK) {
			// TODO: use some AI personality criteria (a random value that is not recomputed each time) to decide if AI takes the risk to explore when being weak (beginning of game...)
			discardExploreAndRetreat = true;
		}
	}
	if (discardExploreAndRetreat && (this->curGroupDistanceToMainUnit > AI_CONST::townSize + 5)) {
		this->SetUnitGroupTarget_internal(unitGroup, NULL);
		this->SetUnitGroupCurrentTask(tacAI, unitGroup, UNIT_GROUP_TASK_IDS::CST_UGT_IDLE, 0, 1);
#ifdef _DEBUG
		this->lastDebugInfo += "explore switched to idle\n";
#endif
		return true; // Unit group has been tasked
	}
	if (discardExploreAndRetreat) {
		float targetPosX = -1;
		float targetPosY = -1;
		targetPosX = militaryAIInfo->recentAttacksByPlayer->lastAttackInMyTownPosX; // may be -1
		targetPosY = militaryAIInfo->recentAttacksByPlayer->lastAttackInMyTownPosY; // may be -1
		if (this->activeGroupsTaskingTempInfo.myMainCentralUnit && ((targetPosX < 0) || (targetPosY < 0))) {
			targetPosX = this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionX + (this->activeGroupsTaskingTempInfo.mainUnitProtectionRadius * this->curUnitGroupOrientationXFromMainUnit);
			targetPosY = this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionY + (this->activeGroupsTaskingTempInfo.mainUnitProtectionRadius * this->curUnitGroupOrientationYFromMainUnit);
		}
		STRUCT_UNIT_MEMORY *targetElem = this->GetUnitMemoryElemEnemyUnitCloserToPosition(player, (long int)targetPosX, (long int)targetPosY);
		UNIT_GROUP_TASK_IDS result = UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET;
		if (targetElem) {
			result = this->AttackOrRetreat(tacAI, unitGroup, targetElem, targetPosX, targetPosY, true);
		} else {
			result = this->AttackOrRetreat(tacAI, unitGroup, this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnitInfAIMemoryElem,
				targetPosX, targetPosY, true);
		}
		if (result != UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET) {
#ifdef _DEBUG
			this->lastDebugInfo += "explore switched to task " + std::to_string(result) + "\n";
#endif
			return true; // True if unit group has been tasked
		}
#ifdef _DEBUG
		this->lastDebugInfo += "explore group tasking failed. Use ROR code.\n";
#endif
		return false;
	}
#ifdef _DEBUG
	this->lastDebugInfo += "explore group: use ROR code (normal)\n";
#endif
	return false; // let standard code for explore groups, in normal situations
}


// Collects info on group and sets UnitGroupDetailedInfo fields
void UnitGroupAI::CollectInfoAboutGroup(STRUCT_PLAYER *player, STRUCT_UNIT_GROUP *unitGroup, UnitGroupDetailedInfo *outputInfos) {
	if (!outputInfos) { return; }
	outputInfos->ResetAllInfo();
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }
	// Unit group constitution and current activity
	int groupUnitCount = unitGroup->unitCount;
	int index = 0;
	while ((index < STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT) && (outputInfos->validUnitsCount < groupUnitCount)) {
		long int curUnitId = unitGroup->GetMyUnitId(index);
		if (curUnitId > -1) {
			STRUCT_UNIT_BASE *curUnit = global->GetUnitFromId(curUnitId);
			if (curUnit && curUnit->IsCheckSumValidForAUnitClass()) {
				outputInfos->validUnitsCount++;
				assert(curUnit->unitDefinition && curUnit->unitDefinition->IsCheckSumValidForAUnitClass());
				if (!curUnit->unitDefinition) { return; } // ERROR
				STRUCT_UNITDEF_BASE *unitDef = curUnit->unitDefinition;
				if (unitDef->DerivesFromAttackable() && curUnit->DerivesFromAttackable()) {
					STRUCT_UNITDEF_ATTACKABLE* unitDefAttackable = (STRUCT_UNITDEF_ATTACKABLE*)curUnit->unitDefinition;
					if (UnitDefProjectileCanDamageOwnUnits(unitDefAttackable)) {
						outputInfos->badDefenderUnitsCount++;
					}
					STRUCT_UNIT_ATTACKABLE *unitAttackable = (STRUCT_UNIT_ATTACKABLE*)curUnit;
					assert(unitAttackable->IsCheckSumValidForAUnitClass());
					if (unitAttackable->currentActivity) {
						// Handle the list of units attacking my group
						for (int i = 0; i < unitAttackable->currentActivity->unitIDsThatAttackMe.usedElements; i++) {
							// Attacker might be allied/mine (typically just after a conversion).
							long int theAttackerId = unitAttackable->currentActivity->unitIDsThatAttackMe.unitIdArray[i];
							STRUCT_UNIT_BASE *theAttacker = global->GetUnitFromId(theAttackerId);
							PLAYER_DIPLOMACY_VALUES dipl = AOE_STRUCTURES::GetDiplomacyValueForUnit(player, theAttacker);
							if ((dipl >= PLAYER_DIPLOMACY_VALUES::CST_PDV_NEUTRAL) || (dipl == PLAYER_DIPLOMACY_VALUES::CST_PDV_GAIA_SPECIAL)) {
								outputInfos->unitsAttackingMyGroup.push_back(theAttackerId);
								if (curUnit->unitInstanceId == unitGroup->commanderUnitId) {
									// TODO: There might be more than one of such units !
									outputInfos->unitIdAttackingMyLeader = theAttackerId;
								}
							}
						}

						bool hasAttackActivity = (unitAttackable->currentActivity->currentTaskId == ACTIVITY_TASK_ID::CST_ATI_TASK_ATTACK) ||
							(unitAttackable->currentActivity->currentTaskId == ACTIVITY_TASK_ID::CST_ATI_TASK_CONVERT);
						STRUCT_ACTION_BASE *unitAction = AOE_STRUCTURES::GetUnitAction(curUnit);
						bool actionIsAgressive = unitAction && (
							(unitAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_CONVERT) ||
							(unitAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_ATTACK) ||
							(unitAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_UNKNOWN_7) // Unsure !!!
							);
						if (!hasAttackActivity && !actionIsAgressive) {
							outputInfos->notAttackingUnitsCount++;
						}
					}
				}
			}
		}
		index++;
	}
	int consideredInactiveCount = outputInfos->notAttackingUnitsCount + outputInfos->unitsAttackingNonPriorityTarget;
	int halfGroup = groupUnitCount / 2;
	outputInfos->unitGroupIsAlmostIdle = (consideredInactiveCount > 0) && (consideredInactiveCount >= halfGroup);
}


// Collects info on enemy units near "main central unit"
// Computes "building in my town" fields in military AI
void UnitGroupAI::CollectEnemyUnitsNearMyMainUnit(STRUCT_PLAYER *player) {
	STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }
	STRUCT_AI *mainAI = player->ptrAIStruct;
	if (!mainAI || !mainAI->IsCheckSumValid()) { return; }

	this->militaryAIInfo->unitIdEnemyBuildingInMyTown = -1;
	this->militaryAIInfo->unitIdEnemyTowerInMyTown = -1;
	STRUCT_UNIT_MEMORY *curElem = NULL;

	this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit = NULL;
	this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnitInfAIMemoryElem = NULL;
	this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnitIsCurrentlyVisible = false;
	if (this->activeGroupsTaskingTempInfo.myMainCentralUnit) {
		long int searchDistance = 7;
		if (this->activeGroupsTaskingTempInfo.myMainCentralUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) {
			searchDistance = CUSTOM_AI::AI_CONST::townSize + 5;
		}

		long int bestIndexEnemy = -1;
		STRUCT_UNIT_BASE *bestEnemy = NULL;
		bool bestEnemyIsVisible = false;
		long int currentIndex = 0;
		while (currentIndex != -1) {
			currentIndex = CUSTOM_AI::CustomAIMilitaryInfo::FindInfAiIndexOfEnemyMilitaryUnitNearPosition(player,
				(long int)this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionX, (long int)this->activeGroupsTaskingTempInfo.myMainCentralUnit->positionY, searchDistance, true, currentIndex);
			STRUCT_UNIT_BASE *curUnit = NULL;
			if (currentIndex >= 0) { // found
				curElem = &mainAI->structInfAI.unitMemoryList[currentIndex];
				this->activeGroupsTaskingTempInfo.enemiesNearMyMainUnit.push_back(curElem);
				// By the way, store (first) enemy building/tower in my town (militaryAI)
				if (curElem->unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) {
					// How about walls ?
					STRUCT_PLAYER *elemPlayer = global->GetPlayerStruct(curElem->playerId);
					if (elemPlayer) {
						if (UnitDefCanAttack(player->GetUnitDefBase(curElem->unitDefId))) {
							if (this->militaryAIInfo->unitIdEnemyTowerInMyTown == -1) {
								this->militaryAIInfo->unitIdEnemyTowerInMyTown = curElem->unitId;
							}
						} else {
							if (this->militaryAIInfo->unitIdEnemyBuildingInMyTown == -1) {
								this->militaryAIInfo->unitIdEnemyBuildingInMyTown = curElem->unitId;
							}
						}
					}
				}
			}
			if (currentIndex > -1) { currentIndex++; } // So next search starts at "next" element.
		}
		if (bestIndexEnemy > -1) {
			this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnitInfAIMemoryElem = &mainAI->structInfAI.unitMemoryList[bestIndexEnemy];
			this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit = bestEnemy;
		}

		if (this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnitInfAIMemoryElem) {
			this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit = global->GetUnitFromId(this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnitInfAIMemoryElem->unitId);
		}
		if (this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit && !this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit->IsCheckSumValidForAUnitClass()) {
			this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit = NULL; // Security check only.
		}
	}
	if (this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit) {
		this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnitIsCurrentlyVisible = PLAYER::IsFogVisibleForPlayer(player,
			(long int)this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit->positionX, (long int)this->activeGroupsTaskingTempInfo.enemyUnitNearMyMainUnit->positionY);
	}
}


// Returns the element from "list of enemies near my main unit" that is closest to a specified position.
STRUCT_UNIT_MEMORY *UnitGroupAI::GetUnitMemoryElemEnemyUnitCloserToPosition(STRUCT_PLAYER *player, long int posX, long int posY) {
	long int bestSquareDist = 999999;
	STRUCT_UNIT_MEMORY *bestElem = NULL;
	bool bestElemIsVisible = false;
	for each (STRUCT_UNIT_MEMORY *e in this->activeGroupsTaskingTempInfo.enemiesNearMyMainUnit) {
		if (!bestElem) {
			bestElem = e;
			bestSquareDist = (e->posX - posX) * (e->posX - posX) + (e->posX - posY) * (e->posX - posY);
			bestElemIsVisible = PLAYER::IsFogVisibleForPlayer(player, e->posX, e->posY);
		} else {
			bool thisUnitIsVisible = PLAYER::IsFogVisibleForPlayer(player, e->posX, e->posY);
			// Visibility is top criterion.
			if (!bestElemIsVisible || thisUnitIsVisible) {
				long int curSquareDist = (e->posX - posX) * (e->posX - posX) + (e->posX - posY) * (e->posX - posY);
				if (curSquareDist < bestSquareDist) {
					bestElem = e;
					bestSquareDist = curSquareDist;
					bestElemIsVisible = thisUnitIsVisible;
				}
			}
		}
	}
	return bestElem;
}


}
