#include "../include/customAIMilitaryInfo.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	void CustomAIMilitaryInfo::ResetAllInfo() {
		for (int i = 0; i < 9; i++) {
			this->recentAttacksByPlayer[i].ResetAllInfo();
			this->recentAttacksByPlayer[i].attackerPlayerId = i;
		}
		this->enemyBuildingInMyTown = NULL;
		this->enemyTowerInMyTown = NULL;
		this->recentAttacksByPlayer->ResetAllInfo();
		this->lastKnownMilitarySituation = MILITARY_SITUATION::MS_UNKNOWN;
		this->lastKnownMilitarySituationComputationGameTime = 0;
	}


	// Securely get the attacks information for the player specified. Returns NULL if playerId is invalid.
	TimeIntervalAttacksRecordForPlayer<TimeIntervalAttackRecord> *CustomAIMilitaryInfo::GetAttackInfoForPlayer(long int attackerPlayerId) {
		if ((attackerPlayerId < 0) || (attackerPlayerId > 8)) { return NULL; }
		return &this->recentAttacksByPlayer[attackerPlayerId];
	}

	// Returns current interval of attacks info for the player specified
	TimeIntervalAttackRecord *CustomAIMilitaryInfo::GetAttackInfoForCurrentTimeInterval(long int attackerPlayerId, long int currentGameTime) {
		TimeIntervalAttacksRecordForPlayer<TimeIntervalAttackRecord> *intervalsForPlayer = this->GetAttackInfoForPlayer(attackerPlayerId);
		if (intervalsForPlayer == NULL) { return NULL; }
		return intervalsForPlayer->GetIntervalForGameTime(currentGameTime);
	}


	// Sets the "last known military situation" variables
	void CustomAIMilitaryInfo::SetLastKnownMilitarySituation(MILITARY_SITUATION situation, unsigned long int gameTime_ms) {
		assert(gameTime_ms >= 0);
		this->lastKnownMilitarySituation = situation;
		this->lastKnownMilitarySituationComputationGameTime = gameTime_ms;
	}

	// Returns true if successful
	// myTownCenter is used to evaluate my town position, it is NOT the target of the attack. myTownCenter may be NULL !
	bool CustomAIMilitaryInfo::SaveEnemyAttackInHistory(long int attackerPlayerId, long int currentGameTime, 
		STRUCT_UNIT_BASE *enemyUnit, STRUCT_UNIT_BASE *myTownCenter) {
		if ((attackerPlayerId < 0) || (attackerPlayerId > 8)) { return false; }

		bool inTown = false;
		if (enemyUnit && myTownCenter) {
			float diff = (enemyUnit->positionX - myTownCenter->positionX);
			if (diff < 0) { diff = -diff; }
			if (diff <= AI_CONST::townSize) { // X position fits my town's positions
				diff = (enemyUnit->positionY - myTownCenter->positionY);
				if (diff < 0) { diff = -diff; }
				if (diff <= AI_CONST::townSize) { // Y position too fits my town's positions => it is IN my town's (square) territory
					this->recentAttacksByPlayer->lastAttackInMyTownPosX = enemyUnit->positionX;
					this->recentAttacksByPlayer->lastAttackInMyTownPosY = enemyUnit->positionY;
					inTown = true;
				}
			}
		}

		CUSTOM_AI::TimeIntervalAttackRecord *interval = this->GetAttackInfoForCurrentTimeInterval(attackerPlayerId, currentGameTime);
		if (interval) {
			GLOBAL_UNIT_AI_TYPES enemyClass = TribeAINone;
			if (enemyUnit && enemyUnit->unitDefinition) {
				enemyClass = enemyUnit->unitDefinition->unitAIType;
			}
			interval->AddAttackRecord(enemyClass, inTown);
		}
		return (interval != NULL);
	}


	// Get the number of attacks from a player during the specified interval (game times in milliseconds)
	int CustomAIMilitaryInfo::GetAttacksCountFromPlayerInPeriod(long int attackerPlayerId, long int startGameTime, long int endGameTime) {
		if ((attackerPlayerId < 0) || (attackerPlayerId > 8)) { return 0; }
		return this->recentAttacksByPlayer[attackerPlayerId].GetAttacksCountInPeriod(startGameTime, endGameTime);
	}

	// Get the number of attacks *in my town* from a player during the specified interval (game times in milliseconds)
	int CustomAIMilitaryInfo::GetAttacksInTownCountFromPlayerInPeriod(long int attackerPlayerId, long int startGameTime, long int endGameTime) {
		if ((attackerPlayerId < 0) || (attackerPlayerId > 8)) { return 0; }
		return this->recentAttacksByPlayer[attackerPlayerId].GetAttacksInMyTownCountInPeriod(startGameTime, endGameTime);
	}


	// Returns true if successful
	bool CustomAIMilitaryInfo::SavePanicModeOccurrenceInHistory(long int attackerPlayerId, long int currentGameTime) {
		if ((attackerPlayerId < 0) || (attackerPlayerId > 8)) { return false; }
		CUSTOM_AI::TimeIntervalAttackRecord *interval = this->GetAttackInfoForCurrentTimeInterval(attackerPlayerId, currentGameTime);
		if (interval) {
			interval->triggeredPanicModesCount++;
		}
		return (interval != NULL);
	}

	// Get the number of strategy panic mode caused by a player during the specified interval (game times in milliseconds)
	int CustomAIMilitaryInfo::GetPanicModesCountFromPlayerInPeriod(long int attackerPlayerId, long int startGameTime, long int endGameTime) {
		if ((attackerPlayerId < 0) || (attackerPlayerId > 8)) { return 0; }
		return this->recentAttacksByPlayer[attackerPlayerId].GetPanicModesCountInPeriod(startGameTime, endGameTime);
	}


	// Returns a value 0-100 to evaluate player weakness. 0 means player is very weak (no more resources), 100=player is rich.
	// Criteria are resources and civilian population
	// TODO: this could go somewhere else
	int CustomAIMilitaryInfo::EvaluatePlayerWeakness(STRUCT_PLAYER *player) {
		if (!player || !player->IsCheckSumValid() || !player->ptrResourceValues) { return 0; }
		const long int maxResValueToEvaluatePlayerWeakness = 500;
		// Get all resources. "Extras" above "maxResValueToEvaluatePlayerWeakness" has no importance (having 9999 wood and 0 or all other resources isn't useful)
		float food = min(maxResValueToEvaluatePlayerWeakness, player->GetResourceValue(CST_RES_ORDER_FOOD));
		float wood = min(maxResValueToEvaluatePlayerWeakness, player->GetResourceValue(CST_RES_ORDER_WOOD));
		float stone = min(maxResValueToEvaluatePlayerWeakness, player->GetResourceValue(CST_RES_ORDER_STONE));
		float gold = min(maxResValueToEvaluatePlayerWeakness, player->GetResourceValue(CST_RES_ORDER_GOLD));
		float score = (food + wood + stone + gold) * 100 / (maxResValueToEvaluatePlayerWeakness * 4);
		// Out of a key resource: negative impact
		if (food < 100) {
			if (food < 60) {
				score = score* 0.60f;
			} else {
				score = score* 0.70f;
			}
		}
		if (wood < 100) {
			score = score* 0.75f;
		}
		if (gold < 100) {
			score = score* 0.85f;
		}
		// Consider current population too
		float villagerCount = player->GetResourceValue(CST_RES_ORDER_CIVILIAN_POPULATION);
		float villagerMinimumExpected = 5;
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		if (settings && settings->IsCheckSumValid() && !settings->isDeathMatch) {
			// Not DM: expected villager count is at least 15.
			villagerMinimumExpected = 15;
		}
		float villagerProportion = villagerCount / villagerMinimumExpected;
		if (villagerProportion < 0) { villagerProportion = 0; }
		if (villagerProportion > 1) { villagerProportion = 1; } // if more villagers than expected, ignore it (no bonus)
		float villagersCountImpact = 0.90f * (1 - villagerProportion); // Weight = x%
		score = score * (1 - (score * villagersCountImpact / 100));

		if (score < 0) { score = 0; }
		if (score > 100) { score = 100; }
		return (int)score;
	}


	// Returns true if unit positions (my unit, TC and enemy unit) are located in such way that panic mode can be triggered.
	// This is an approximative evaluation, not a complete analysis.
	bool CustomAIMilitaryInfo::IsPanicModeEligible(STRUCT_UNIT_BASE *myTC, STRUCT_UNIT_BASE *myUnit, STRUCT_UNIT_BASE *enemyUnit) {
		if (!myUnit || !enemyUnit) {
			return false; // Error case
		}
		if (!myTC) {
			return true; // No town center. All attacks are critical and must be dealt with
		}
		const long int townSizePlusMargin = AI_CONST::townSize + 5; // add some margin to make sure we don't exclude too many cases

		// TC and my unit
		float diff = (myTC->positionX - myUnit->positionX); // Diff can be negative
		if ((diff >= -townSizePlusMargin) && (diff <= townSizePlusMargin)) {
			diff = (myTC->positionY - myUnit->positionY);
			if ((diff >= -townSizePlusMargin) && (diff <= townSizePlusMargin)) {
				return true; // my unit is close enough to TC, panic mode can be triggered
			}
		}
		// TC and enemy unit
		diff = (myTC->positionX - enemyUnit->positionX);
		if ((diff >= -townSizePlusMargin) && (diff <= townSizePlusMargin)) {
			diff = (myTC->positionY - enemyUnit->positionY);
			if ((diff >= -townSizePlusMargin) && (diff <= townSizePlusMargin)) {
				return true; // enemy unit is close enough to my TC, panic mode can be triggered
			}
		}
		return false; // Both units seem far from my town
	}

	// Get main town center, or the "main" unit to defend if no TC is found
	// This is not supposed to return NULL for a valid player (alive) because we should always find at least 1 valid unit
	STRUCT_UNIT_BASE *CustomAIMilitaryInfo::GetTownCenterOrUnitToDefend(STRUCT_PLAYER *player) {
		if (!player || !player->IsCheckSumValid()) { return NULL; }
		if (!player->ptrCreatableUnitsListLink || !player->ptrCreatableUnitsListLink->IsCheckSumValid()) { return NULL; }
		STRUCT_UNIT_BASE *unfinishedTC = NULL;
		STRUCT_UNIT_BASE *finishedTC = NULL;
		STRUCT_UNIT_BASE *priestOrCivilian = NULL;
		STRUCT_UNIT_BASE *house = NULL;
		STRUCT_UNIT_BASE *building = NULL;
		STRUCT_UNIT_BASE *anyUnit = NULL;
		bool hadVillager = false;
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *curElem = player->ptrCreatableUnitsListLink->lastListElement;
		while (curElem != NULL) {
			STRUCT_UNIT_BASE *curUnit = curElem->unit;
			if (curUnit && curUnit->IsCheckSumValidForAUnitClass() && (curUnit->unitStatus <= 2)) {
				STRUCT_UNITDEF_BASE *curUnitDef = curUnit->unitDefinition;
				if (curUnitDef && curUnitDef->IsCheckSumValidForAUnitClass()) {
					if (curUnitDef->DAT_ID1 == CST_UNITID_FORUM) {
						if (curUnit->unitStatus == 0) {
							unfinishedTC = curUnit;
						} else {
							finishedTC = curUnit; // Continue ; "main" TC is last one from the list
						}
					} else {
						if (curUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian) {
							priestOrCivilian = curUnit;
							hadVillager = true;
						} else {
							if (curUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
								priestOrCivilian = curUnit;
							} else {
								if (curUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) {
									if (curUnitDef->DAT_ID1 == CST_UNITID_HOUSE) {
										house = curUnit;
									} else {
										building = curUnit;
									}
								} else {
									anyUnit = curUnit;
								}
							}
						}
					}
				}
			}
			curElem = curElem->previousElement;
		}
		if (finishedTC) {
			return finishedTC;
		}
		if (hadVillager && unfinishedTC) {
			return unfinishedTC;
		}
		if (priestOrCivilian) {
			return priestOrCivilian;
		}
		if (house) {
			return house;
		}
		if (building) {
			return building;
		}
		return anyUnit;
	}


	// Returns infAI elem list entry for the first found enemy or neutral military unit in specified zone
	// startIndex = index of first element to search in infAI elem list. Use 0 or -1 to start from beginning.
	// Returns -1 if not found
	// Excludes units that cannot attack (villagers, fishing boats, transports...)
	long int CustomAIMilitaryInfo::FindInfAiIndexOfEnemyMilitaryUnitNearPosition(STRUCT_PLAYER *player, long int posX, long int posY,
		long int distanceFromCenter, bool landOnly, long int startIndex) {
		long int resultNotFound = -1;
		if (!player || !player->IsCheckSumValid() || !player->ptrAIStruct || !player->ptrAIStruct->IsCheckSumValid()) { return resultNotFound; }
		STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
		assert(global && global->IsCheckSumValid());
		if (!global || !global->IsCheckSumValid()) { return resultNotFound; }
		assert(global->gameMapInfo && global->gameMapInfo->IsCheckSumValid());
		global->gameMapInfo->FixPositionToGetInMapBounds(&posX, &posY);
		assert(distanceFromCenter > 0);
		if (distanceFromCenter <= 0) { return resultNotFound; }
		long int minPosX = posX - distanceFromCenter;
		long int minPosY = posY - distanceFromCenter;
		long int maxPosX = posX + distanceFromCenter;
		long int maxPosY = posY + distanceFromCenter;
		global->gameMapInfo->FixPositionToGetInMapBounds(&minPosX, &minPosY);
		global->gameMapInfo->FixPositionToGetInMapBounds(&maxPosX, &maxPosY);
		
		if (startIndex < 0) { startIndex = 0; }
		for (int i = startIndex; i < player->ptrAIStruct->structInfAI.detailedSpottedUnitInfoListSize; i++) {
			STRUCT_INF_AI_DETAILED_UNIT_INFO *curElem = &player->ptrAIStruct->structInfAI.detailedSpottedUnitInfoList[i];
			bool enemyOrNeutral = (player->diplomacyVSPlayers[curElem->playerId] > PLAYER_DIPLOMACY_VALUES::CST_PDV_ALLY);
			enemyOrNeutral &= (curElem->unitId > -1);
			STRUCT_UNIT_BASE *curUnit = global->GetUnitFromId(curElem->unitId);
			enemyOrNeutral &= (curUnit && curUnit->IsCheckSumValidForAUnitClass() && curUnit->unitDefinition && curUnit->unitDefinition->IsCheckSumValidForAUnitClass());
			if (enemyOrNeutral && landOnly && IsWaterUnit(curUnit->unitDefinition->unitAIType)) {
				enemyOrNeutral = false;
			}
			if (enemyOrNeutral) {
				// Villagers are excluded here
				enemyOrNeutral = (curUnit->unitDefinition->unitAIType != TribeAIGroupCivilian) &&
					(curUnit->unitDefinition->unitAIType != TribeAIGroupFishingBoat) &&
					UnitDefCanAttack(curUnit->unitDefinition);
			}
			if (enemyOrNeutral) {
				// Check position
				if ((curUnit->positionX >= minPosX) && (curUnit->positionX <= maxPosX) &&
					(curUnit->positionY >= minPosY) && (curUnit->positionY <= maxPosY)) {
					return i;
				}
			}
		}
		return resultNotFound; // Not found
	}
}

