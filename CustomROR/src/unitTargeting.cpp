#include "../include/unitTargeting.h"


using namespace AOE_CONST_FUNC;
using namespace AOE_STRUCTURES;

namespace CUSTOMROR {


// baseTimeGetTimeValue = TimeGetTime() value (ms) for total AI treatment process time calculation. We must not go over global->tmp_allowedTimeForAITreatment
// If successful, returns a pointer to the selected STRUCT_INF_AI_UNIT_LIST_ELEM element.
// If successful, unitGroup targets array contains only 1 target (in most cases... to check in case when units block the way to main target)
// Returns false if found nothing OR if search could not be completed in time.
STRUCT_INF_AI_UNIT_LIST_ELEM *FindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
	STRUCT_UNIT_GROUP_ELEM *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue) {
	// Collect info / Check parameters
#pragma message("TODO: set inprogress=0 when exiting in error")
	if (!targetInfo) { return NULL; }
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	if ((targetPlayerId <= 0) || (targetPlayerId >= global->playerTotalCount)) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_AI *mainAI = infAI->ptrMainAI;
	assert(mainAI && mainAI->IsCheckSumValid());
	if (!mainAI || !mainAI->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_PLAYER *player = mainAI->player;
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_UNIT_BASE *groupLeader = global->GetUnitFromId(unitGroup->commanderUnitId);
	if (!groupLeader || !groupLeader->IsCheckSumValidForAUnitClass()) { targetInfo->targetSearchInProgress = 0; return NULL; } // 0x4BFFE2
	if (!groupLeader->currentActivity) { targetInfo->targetSearchInProgress = 0; return NULL; } // 0x4BFFF1
	STRUCT_TAC_AI *tacAI = &infAI->ptrMainAI->structTacAI;
	assert(tacAI && tacAI->IsCheckSumValid());
	if (!tacAI || !tacAI->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }

	char leaderTerrainZoneId = groupLeader->GetMyTerrainZoneId(); // 0x4BFFF9
	STRUCT_UNITDEF_BASE *groupLeaderDef = groupLeader->unitDefinition;
	assert(groupLeaderDef && groupLeaderDef->IsCheckSumValidForAUnitClass());
	bool isPriest = groupLeaderDef->unitAIType == TribeAIGroupPriest;
	bool isBoat = IsWaterUnit(groupLeaderDef->unitAIType); // 0x4C001F

	// 0x4C003F
	if (!targetInfo->targetSearchInProgress) {
		// Initialize search ?
		if ((targetPlayerId == unitGroup->targetPlayerId) && // 0x4C0054
			(unitGroup->targetUnitIdArrayUsedElemCount > 1) && // 0x4C0062
			((char)unitGroup->terrainZoneId) == leaderTerrainZoneId) { // 0x4C007A
			long int lastTargetUnitId = unitGroup->GetTargetUnitIdFromArray(unitGroup->targetUnitIdArrayUsedElemCount - 1);
			// There is a bug here in original code with unitId -1 !
			STRUCT_UNIT_BASE *targetUnit = NULL;
			if (lastTargetUnitId >= 0) {
				targetUnit = global->GetUnitFromId(lastTargetUnitId);
			}
			// 0x4C0095
			if (targetUnit && targetUnit->IsCheckSumValidForAUnitClass()) {
				STRUCT_PLAYER *targetPlayer = targetUnit->ptrStructPlayer;
				assert(targetPlayer && targetPlayer->IsCheckSumValid());
				long int targetUnitPlayerId = targetPlayer->playerId;
				bool isEnemy = (targetUnitPlayerId > 0) &&
					(player->ptrDiplomacyStances[targetPlayerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ENEMY);
				if (isEnemy && (targetUnit->unitStatus <= AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY)) {
					// 0x4C00C7-0x4C00FF
					long int canMoveToTarget = groupLeader->CanMoveTo(targetUnit->unitInstanceId, groupLeader->GetMaxRange(),
						0, 1, -1, -1);
					if (canMoveToTarget) {
						// 0x4C010A
						unitGroup->targetUnitIdArrayUsedElemCount = 0; // Reset (no need to erase values from array)
						unitGroup->AddTargetUnitIdToArray(targetUnit->unitInstanceId);
						return FindInfAIUnitElemInList(infAI, targetUnit->unitInstanceId); // 0x4C0135
					}
				}
			}
			// 0x4C013A. Strange conception: why not just do a (reverse) loop instead of duplicate code with code above ?
			for (int i = 0; i < unitGroup->targetUnitIdArrayUsedElemCount; i++) {
				long int loopTargetId = unitGroup->GetTargetUnitIdFromArray(i);
				STRUCT_UNIT_BASE *loopTargetUnit = NULL;
				if (loopTargetId >= 0) { // Missing check in original code
					loopTargetUnit = global->GetUnitFromId(loopTargetId); // 0x4C0164
				}
				if (loopTargetUnit && loopTargetUnit->IsCheckSumValidForAUnitClass() && loopTargetUnit->ptrStructPlayer &&
					loopTargetUnit->ptrStructPlayer->IsCheckSumValid()) {
					// 0x4C0171
					long int loopTargetPlayerId = loopTargetUnit->ptrStructPlayer->playerId;
					long int isLoopUnitEnemy = (loopTargetPlayerId > 0) &&
						(player->ptrDiplomacyStances[loopTargetPlayerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ENEMY);
					if (isLoopUnitEnemy && (loopTargetUnit->unitStatus <= AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY)) {
						// 0x4C0199
						long int canMoveToTarget = groupLeader->CanMoveTo(loopTargetId, groupLeader->GetMaxRange(),
							0, 1, -1, -1);
						if (canMoveToTarget) {
							return FindInfAIUnitElemInList(infAI, targetUnit->unitInstanceId); // 0x4C0232 to 0x4C0246
						}
					}
				}
			}
		}
		// 0x4C01E8
		if (!targetInfo->targetSearchInProgress) { // always true: same condition as parent if
			STRUCT_INF_AI_UNIT_LIST_ELEM *enemyWonderInfo = FindWonderToAttackInfAIElemPtr(infAI, -1);
			if (enemyWonderInfo) {
				// 0x4C0208
				char wonderTerrainZoneId = groupLeader->GetTerrainZoneIdAtPos(enemyWonderInfo->posX, enemyWonderInfo->posY);
				if ((wonderTerrainZoneId == leaderTerrainZoneId) || isBoat) {
					// 0x4C024B
					long int canMoveToTarget = groupLeader->CanMoveTo(enemyWonderInfo->unitId, groupLeader->GetMaxRange(),
						0, 1, -1, -1); // 0x4C0272
					if (canMoveToTarget) {
						// 0x4C027D
						unitGroup->AddTargetUnitIdToArray(enemyWonderInfo->unitId);
						return enemyWonderInfo;
					}
					// 0x4C02B6
					STRUCT_AI_UNIT_LIST_INFO tempList;
					// This call searches a path, even if enemy units block the way ? Unsure
					bool success = AOE_METHODS::MoveToTarget_1A0(groupLeader, enemyWonderInfo->unitId, groupLeader->GetMaxRange(),
						0, 1, targetPlayerId, 0x1B, &tempList);
					if (success) {
						// 0x4C030F
						if (tempList.usedElements > 0) {
							for (int i = 0; i < tempList.usedElements; i++) { // 0x4C0323
								if (i > tempList.arraySize) {
									// 0x4C032C. Dead code & unused ? How could usedElemCount be > allocCount ???
									assert(false);
									// I did not rewrote this portion of code, as I don't see the point...
								}
								// 0x4C0380
								long int currentUnitId = tempList.unitIdArray[i];
								STRUCT_UNIT_BASE *currentUnit = NULL;
								// ADDED: check unitId is not -1
								if (currentUnitId > -1) {
									currentUnit = global->GetUnitFromId(currentUnitId);
								}
								if (currentUnit && currentUnit->IsCheckSumValidForAUnitClass()) {
									if (i > tempList.arraySize - 1) {
										// 0x4C03A4. Dead code & unused ? How could usedElemCount be > allocCount ???
										assert(false);
										// I did not rewrote this portion of code, as I don't see the point...
									}
									// 0x4C03F8
									// Add units that block access to wonder to targets list ??
									// Note: this is cheating, currentUnit may not be visible !
									AOE_METHODS::AddUnitInInfAILists(infAI, currentUnitId);
									if (i > tempList.arraySize - 1) {
										// 0x4C0410. Dead code & unused ? How could usedElemCount be > allocCount ???
										assert(false);
										// I did not rewrote this portion of code, as I don't see the point...
									}
									// 0x4C0464
									unitGroup->AddTargetUnitIdToArray(currentUnitId);
								}
								// 0x4C0473: end of loop => back to 0x4C0323
							}
						}
						// 0x4C0480
						unitGroup->AddTargetUnitIdToArray(enemyWonderInfo->unitId);
						unitGroup->targetPlayerId = enemyWonderInfo->playerId; // 0x4C049E
						unitGroup->terrainZoneId = leaderTerrainZoneId;
						if (tempList.unitIdArray) {
							AOEFree(tempList.unitIdArray);
							tempList.unitIdArray = NULL; // ADDED
							tempList.usedElements = tempList.arraySize = 0; // ADDED
						}
						return enemyWonderInfo; // 0x4C04C6
					} else {
						// Move failed, even ignoring enemy units (not sure about this part)
						// 0x4C04CB
						if (tempList.unitIdArray) {
							AOEFree(tempList.unitIdArray);
							tempList.unitIdArray = NULL; // ADDED
							tempList.usedElements = tempList.arraySize = 0; // ADDED
						}
					}
				} else {
					// 0x4C0229
					// Not sure about this. Seems illogical ?
					return enemyWonderInfo;
				}
			}
			// 0x4C04E6 / 0x4C04E9 (same, 0x4C04E6 just sets EAX=-1 if not already done)
			// Search is not in progress (we are still in "prepare" phase), no wonder was found.
			// Init search (as no priority target was selected)
			targetInfo->targetUnitId = -1;
			targetInfo->targetEvaluation = -1.f;
			targetInfo->targetInfAIUnitElemListIndex = -1;
			targetInfo->currentSearchInfAIUnitElemListIndex = 0;
			targetInfo->unknown_10 = -1;
			targetInfo->unknown_14 = -1;
			targetInfo->currentSearchIsBuildings = 0; // start with living units
			targetInfo->buildingTargetUnitId = -1;
			targetInfo->buildingTargetEvaluation = -1.f;
			targetInfo->buildingTargetInfAIUnitElemListIndex = -1;
			targetInfo->targetSearchInProgress = 1;
		}
	}

	// 0x4C0517 : main part
	if (targetInfo->currentSearchInfAIUnitElemListIndex < infAI->unitElemListSize) {
		// Loop from 0x4C053C to 0x4C0DBD. Loop on infAI.unitElems...
		for (int curIndex = targetInfo->currentSearchInfAIUnitElemListIndex; curIndex < infAI->unitElemListSize; curIndex++) {
			if (curIndex > targetInfo->currentSearchInfAIUnitElemListIndex) {
				// For all case EXCEPT first loop: check the time spent
				long int t = AOE_METHODS::TimeGetTime();
				if (t - baseTimeGetTimeValue >= global->tmp_allowedTimeForAITreatment) {
					// 0x4C0E07
					targetInfo->currentSearchInfAIUnitElemListIndex = curIndex;
					return NULL; // Unfinished business. Note that "in progress" is still 1, to finish (continue) next time.
				}
			}
			// 0x4C0576
			STRUCT_INF_AI_UNIT_LIST_ELEM *curElem = &infAI->unitElemList[curIndex];
#pragma message("We should check that playerId is correct: may have changed due to a conversion. Beware fog-visibility too")
			if (curElem->playerId != targetPlayerId) {
				continue;
			}
			if (curElem->unitId == -1) { continue; } // ADDED
			STRUCT_UNIT_BASE *curUnit = global->GetUnitFromId(curElem->unitId);
			if (!curUnit || !curUnit->IsCheckSumValidForAUnitClass() ||
				(curUnit->unitStatus > GAME_UNIT_STATUS::GUS_2_READY) || (curUnit->remainingHitPoints <= 0) ||
				(!curUnit->unitDefinition || !curUnit->unitDefinition->IsCheckSumValidForAUnitClass())) { // ADDED
				continue;
			}
			assert(curUnit->unitDefinition && curUnit->unitDefinition->IsCheckSumValidForAUnitClass());
			// 0x4C05D1
			if (!isBoat && IsWaterUnit(curUnit->unitDefinition->unitAIType)) {
				continue; // do not attack boats with non-water unit groups
			}
			// 0x4C05EA
			// CHEATING : unit may not be visible
			long int curUnitTerrainZoneId = groupLeader->GetTerrainZoneIdAtPos((long int)curUnit->positionX, (long int)curUnit->positionY);
			// 0x4C060D. Is the !isBoat condition correct here ?
			if (!isBoat && (curElem->unitDATID == CST_UNITID_DOCK) && (leaderTerrainZoneId != curUnitTerrainZoneId)) {
				continue;
			}
			// 0x4C062B
			// if (!targetInfo->currentSearchIsBuildings) {} // Duplicate if/else code !
			if (curElem->unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupWall) {
				continue;
			}
			// 0x4C0669
			long int diffX = curElem->posX - (long int)curUnit->positionX;
			long int diffY = curElem->posY - (long int)curUnit->positionY;
			if (diffX < 0) { diffX = -diffX; }
			if (diffY < 0) { diffY = -diffY; }
			bool consideredVisible = false;
			if ((diffX <= 5) && (diffY <= 5)) {
				consideredVisible = true; // ah!!! cheating
			} else {
				consideredVisible = IsFogVisibleForPlayer(player->playerId, (long int)curUnit->positionX, (long int)curUnit->positionY);
			}
			if (!consideredVisible) {
				continue;
			}
			// 0x4C06D6
			if (isPriest && !AOE_METHODS::CanConvert(groupLeader, curElem->unitId)) {
				continue;
			}
			// 0x4C0705 : begin calculations
			float groupDamage = GetGroupDamageOnUnit(infAI, unitGroup, curUnit); // Damage per second, actually
			if (groupDamage <= 0) { // ADDED. What happens in AOE code? e.g. group has only priests?
				continue;
			}
			float timeToKillEnemy = curUnit->remainingHitPoints / groupDamage;
			if (timeToKillEnemy <= 0) {
				continue; // 0x4C0740
			}
			// 0x4C0728
			float timeToKillMyUnitsAtTargetPos = GetTimeToKillGroupUnitsAtTargetPosition(infAI, unitGroup, curUnit);
			if (timeToKillMyUnitsAtTargetPos <= 0) {
				continue; // 0x4C0755
			}
			float timeKillProportion = timeToKillMyUnitsAtTargetPos / timeToKillEnemy; // 0x4C075F->77C (local_50). High value = good
			float posDiffX = (((float)curElem->posX) - groupLeader->positionX);
			float posDiffY = (((float)curElem->posY) - groupLeader->positionY);
			float distanceCommanderToCurEnemy = sqrtf((posDiffX * posDiffX) + (posDiffY * posDiffY)); // local_30
			if (distanceCommanderToCurEnemy < 1) {
				distanceCommanderToCurEnemy = 1; // 0x4C07C6
			}
			float unknown_4C = (float)curElem->unknown_14; // TODO !!! 0x4C07E9 (note: ESP+50 because a PUSH 0x53 is pending)
			long int targetEvaluationRandomness = tacAI->SNNumber[SNTargetEvaluationRandomness]; // local40
			float randomnessEval = 0;
			if (targetEvaluationRandomness > 0) {
				// 0x4C07F6
				// We could use AOE's pseudo-randomizer to be respect original code?
				randomnessEval = (float)(randomizer.GetRandomValue(0, 0xFFFF) % targetEvaluationRandomness);
			}
			long int targetEvaluationAttackAttempts = tacAI->SNNumber[SNTargetEvaluationAttackAttempts];
			// 0x4C0821
			float attemptsEval = (float)(targetEvaluationAttackAttempts * curElem->attackAttempts); // cf local_54
			float targetEvaluationContinent = 0;
			if (leaderTerrainZoneId == curUnitTerrainZoneId) { // Always true because of previous "if"s ?
				targetEvaluationContinent = (float)tacAI->SNNumber[SNTargetEvaluationContinent];
			} // 0x4C0881
			long int targetEvaluationInProgress = tacAI->SNNumber[SNTargetEvaluationInProgress];
			float targetInProgressEvaluation = 0; // local_38.
			if (targetEvaluationInProgress > 0) {
				long int alreadyTargetCount = 0; // cf local_2C
				for (int i = 0; i < unitGroup->unitCount; i++) {
					// 0x4C08C7
					long int myGrpUnitId = unitGroup->GetMyUnitId(i);
					STRUCT_UNIT_BASE * myGrpUnit = NULL;
					if (myGrpUnitId >= 0) { // ADDED
						myGrpUnit = global->GetUnitFromId(myGrpUnitId);
					}
					if (myGrpUnit && myGrpUnit->IsCheckSumValidForAUnitClass() && myGrpUnit->currentActivity) {
						if (myGrpUnit->currentActivity->targetUnitId == curElem->unitId) {
							alreadyTargetCount++;
						}
					}
				}
				// 0x4C0919
				targetInProgressEvaluation = (float)(targetEvaluationInProgress * alreadyTargetCount);
			} // 0x4C093F

			float evalDistanceBetweenLoopElemAndUnknown24Elem = 0; // local_1C
			if ((targetInfo->currentSearchIsBuildings) && (targetInfo->buildingTargetUnitId != -1)) {
				// 0x4C095A
				long int unknown24_posX = infAI->unitElemList[targetInfo->buildingTargetInfAIUnitElemListIndex].posX;
				long int unknown24_posY = infAI->unitElemList[targetInfo->buildingTargetInfAIUnitElemListIndex].posY;
				long int elemsDiffX = curElem->posX - unknown24_posX;
				long int elemsDiffY = curElem->posY - unknown24_posY;
				float elemsDistance = (float)((elemsDiffX*elemsDiffX) + (elemsDiffY*elemsDiffY));
				elemsDistance = sqrtf(elemsDistance); // 0x4C09A2
				if (elemsDistance < 1) {
					evalDistanceBetweenLoopElemAndUnknown24Elem = 5.f;
				} else {
					evalDistanceBetweenLoopElemAndUnknown24Elem = 25000.f / elemsDistance;
				}
			}
			// 0x4C09CB
			bool canTrainUnits = (curUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) && (
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_BARRACKS) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_RANGE) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_SIEGE_WORKSHOP) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_DOCK) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_ACADEMY) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_STABLE) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_TEMPLE)
				); // Cf call to 0x4BE250
			if (canTrainUnits) {
				if ((curUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) ||
					(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_DOCK)) {
					evalDistanceBetweenLoopElemAndUnknown24Elem += 200.f; // 0x4C09FF
				} else {
					evalDistanceBetweenLoopElemAndUnknown24Elem += 300.f; // 0x4C09F3
				}
			} // 0x4C0A09
			long int targetEvaluationKills = tacAI->SNNumber[SNTargetEvaluationKills];
			long int targetEvaluationDistance = tacAI->SNNumber[SNTargetEvaluationDistance];
			long int targetEvaluationTimeKillRatio = tacAI->SNNumber[SNTargetEvaluationTimeKillRatio];
			float unknown_killRating = targetEvaluationKills * unknown_4C; // 0x4C0A32. in local_48_tmp
			assert(distanceCommanderToCurEnemy > 0);
			float distanceEval = ((float)targetEvaluationDistance) / ((float)distanceCommanderToCurEnemy);
			float killAndDistanceRating = distanceEval + unknown_killRating; // 0x4C0A5C. local_48_final

			float totalEvaluation = ((float)targetEvaluationTimeKillRatio) * timeKillProportion; // 0x4C0A70->A77
			totalEvaluation += killAndDistanceRating; // 0x4C0A7B
			totalEvaluation += evalDistanceBetweenLoopElemAndUnknown24Elem;
			totalEvaluation += targetInProgressEvaluation; // local_38
			totalEvaluation += targetEvaluationContinent; // local_3C
			totalEvaluation += attemptsEval; // local_54
			totalEvaluation += randomnessEval; // local_40
			if ((targetInfo->targetUnitId != -1) && (targetInfo->targetEvaluation >= totalEvaluation)) { // 0x4C0A99 : compare with previous best score
				continue;
			} // 0x4C0ABA
			if (!targetInfo->currentSearchIsBuildings && (AOE_METHODS::GetSpeed(curUnit) <= 0)) {
				// A non-building search found a building (or non-moving) unit. Save it, but in "building" slot
				// Improvement: is it a tower ? Can it attack (attack OR convert) ?
				// Note: not checking terrainZone, movement...
				targetInfo->buildingTargetEvaluation = totalEvaluation;
				targetInfo->buildingTargetUnitId = curElem->unitId;
				targetInfo->buildingTargetInfAIUnitElemListIndex = targetInfo->currentSearchInfAIUnitElemListIndex; // NOT using curIndex. Not to skip some buildings in search ?
			} // 0x4C0AF8
			long int unknown_EBP = targetInfo->currentSearchIsBuildings ? -1 : 0x1B;
			STRUCT_AI_UNIT_LIST_INFO tempListForMove;
			tempListForMove.arraySize = 0;
			tempListForMove.maxElementCount = 0;
			tempListForMove.unitIdArray = NULL;
			tempListForMove.usedElements = 0;
			if (leaderTerrainZoneId == curUnitTerrainZoneId) {
				bool canMoveToTarget = (groupLeader->CanMoveTo(curElem->unitId, groupLeader->GetMaxRange(), 0, 1, -1, -1) != 0);
				if (!canMoveToTarget) {
					// 0x4C0B62
					bool canMoveToTargetIfKillingEnemies = AOE_METHODS::MoveToTarget_1A0(groupLeader, curElem->unitId,
						groupLeader->GetMaxRange(), 0, 1, targetPlayerId, unknown_EBP, &tempListForMove);
				}
			}
			// 0x4C0BA2
			unitGroup->targetUnitIdArrayUsedElemCount = 0; // reset targets array.
			if (tempListForMove.usedElements > 0) {
				// Only if we used the "force" mode: add found units (that block the way?) in infAI lists & as group targets
				// TODO 0x4C0BBD
#pragma message("UNFINISHED 0x4C0BBD")
			} // 0x4C0D16
			unitGroup->AddTargetUnitIdToArray(curElem->unitId);
			unitGroup->targetPlayerId = curElem->playerId;
			unitGroup->terrainZoneId = leaderTerrainZoneId;
			targetInfo->targetUnitId = curElem->unitId;
			targetInfo->targetEvaluation = totalEvaluation;
			targetInfo->targetInfAIUnitElemListIndex = curIndex;
			if (tempListForMove.unitIdArray != NULL) {
				AOEFree(tempListForMove.unitIdArray); // 0x4C0D89
			}
			tempListForMove.arraySize = 0;
			tempListForMove.maxElementCount = 0;
			tempListForMove.unitIdArray = NULL;
			tempListForMove.usedElements = 0;
			// 0x4C0DA1 : end of one loop (next element)
		}
		// 0x4C0DC3
	}
	// 0x4C0DD1
	if (targetInfo->targetUnitId == -1) { // not found
		// 0x4C0E13
		if (!targetInfo->currentSearchIsBuildings) {
			targetInfo->currentSearchIsBuildings = 1; // 0x4C0E1A
			targetInfo->currentSearchInfAIUnitElemListIndex = 0; // restart from beginning (search remains in progress). Exit this method, but next call will go on with buildings.
		} else {
			targetInfo->targetSearchInProgress = 0; // 0x4C0E27. Search completed but found nothing.
		}
		return NULL; // Not found
	}
	// 0x4C0DD6
	assert(targetInfo->targetInfAIUnitElemListIndex >= 0);
	assert(targetInfo->targetInfAIUnitElemListIndex < infAI->unitElemListSize);
	targetInfo->targetSearchInProgress = 0; // 0x4C0DF1
	infAI->unitElemList[targetInfo->targetInfAIUnitElemListIndex].attackAttempts++; // 0x4C0DEF
	// 0x4C0E00
	// Extremely NOT optimized (lol) ! Doing a whole loop to find something we already have = &infAI->unitElemList[targetInfo->targetInfAIUnitElemListIndex]
	STRUCT_INF_AI_UNIT_LIST_ELEM *result = FindInfAIUnitElemInList(infAI, unitGroup->GetTargetUnitIdFromArray(0));
	assert(&infAI->unitElemList[targetInfo->targetInfAIUnitElemListIndex] == result); // TEST for optimization (TODO)
	return result;
}


// Returns the enemy wonder with higher attack priority. Does not return wonders from allied/neutral players
// playerId can be a joker (if -1)
// Returns NULL if not found
STRUCT_INF_AI_UNIT_LIST_ELEM *FindWonderToAttackInfAIElemPtr(STRUCT_INF_AI *infAI, long int playerId) {
	assert(infAI && infAI->IsCheckSumValid());
	if (!infAI || !infAI->IsCheckSumValid()) { return NULL; }
	assert((playerId >= -1) && (playerId <= 8));
	STRUCT_INF_AI_UNIT_LIST_ELEM *result;
	unsigned long int addr = 0x4C5980;
	_asm{
		MOV ECX, infAI;
		PUSH playerId;
		CALL addr;
		MOV result, EAX;
	}
	return result;
}


// Computes the damage dealt by a group on a unit, cf 0x4C62F0.
float GetGroupDamageOnUnit(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP_ELEM *unitGroup, STRUCT_UNIT_BASE *targetUnit) {
	assert(infAI && infAI->IsCheckSumValid() && unitGroup && unitGroup->IsCheckSumValid() &&
		targetUnit && targetUnit->IsCheckSumValidForAUnitClass());
	if (!infAI || !infAI->IsCheckSumValid() || !unitGroup || !unitGroup->IsCheckSumValid() ||
		!targetUnit || !targetUnit->IsCheckSumValidForAUnitClass()) {
		return 0;
	}
	if (unitGroup->unitCount <= 0) {
		return 0;
	}
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) {
		return 0;
	}
	assert(unitGroup->unitCount <= 40);
	float totalDamagePerSecond = 0;
	for (int i = 0; i < unitGroup->unitCount; i++) {
		long int myUnitId = unitGroup->GetMyUnitId(i);
		STRUCT_UNIT_BASE *myUnit = NULL;
		if (myUnitId > -1) {
			myUnit = global->GetUnitFromId(myUnitId);
		}
		if (myUnit && myUnit->IsCheckSumValidForAUnitClass()) {
			float myDamage = AOE_METHODS::CalcDamage(myUnit, targetUnit);
			float myReloadTime = AOE_METHODS::GetReloadTime1(myUnit);
			totalDamagePerSecond += (myDamage / myReloadTime);
		}
	}
	return totalDamagePerSecond;
}


// Estimates the total time to kill a group's units at target position, considering enemy units known from infAI elem list
float GetTimeToKillGroupUnitsAtTargetPosition(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP_ELEM *unitGroup, STRUCT_UNIT_BASE *targetUnit) {
	if (!infAI || !infAI->IsCheckSumValid()) { return 0; }
	if (!unitGroup || !unitGroup->IsCheckSumValid()) { return 0; }
	if (!targetUnit || !targetUnit->IsCheckSumValidForAUnitClass()) { return 0; }
	unsigned long int callAddr = 0x4C6400;
	float result;
	_asm {
		MOV ECX, infAI;
		PUSH targetUnit;
		PUSH unitGroup;
		CALL callAddr;
		FSTP DS : [result]; // REQUIRED to compensate the FLD from called method (for float stack consistency)
	}
	return result;
}


}
