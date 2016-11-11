#include "../include/playerTargeting.h"


namespace CUSTOM_AI {

// Global static objects
PlayerTargeting playerTargetingHandler;



AIPlayerTargetingInfo::AIPlayerTargetingInfo() {
	this->ResetInfo();
}

// Reset all underlying info (useful at game start, etc)
void AIPlayerTargetingInfo::ResetInfo() {
	this->militaryAIInfo = NULL;
	for (int i = 0; i < 9; i++) {
		this->lastComputedDislikeSubScore[i] = 0;
	}
	this->lastUpdateGameTime = 0;
	this->nextUpdateGameTime = 0;
	this->lastTargetPlayerChangeGameTime = 0;
}


// Recompute information (only) if refresh delay has been reached (cf updateDetailedDislikeInfoMaxDelay)
// Returns true if information have been recomputed (false is not necessarily an error)
bool AIPlayerTargetingInfo::RecomputeInfo(STRUCT_PLAYER *player) {
	assert(player && player->IsCheckSumValid());
	assert(player->playerId == this->myPlayerId);
	if (!player || !player->IsCheckSumValid() || (player->playerId != this->myPlayerId)) { return false; }
	STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	assert(global && global->IsCheckSumValid());
	assert(global->currentGameTime >= 0);
	if (this->lastUpdateGameTime > 0) {
		if ((global->currentGameTime <= this->lastUpdateGameTime + (1000 * TARGETING_CONST::updateDetailedDislikeInfoMaxDelay)) &&
			(global->currentGameTime <= this->nextUpdateGameTime)
			) {
			return false; // Next update time AND Max delay (which is no more but a security) have both not been reached
		}
	}
	assert(player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid());
	if (!player->ptrAIStruct || !player->ptrAIStruct->IsCheckSumValid()) { return false; }

	// HERE: it is time for info update.

	this->lastUpdateGameTime = global->currentGameTime;
	this->nextUpdateGameTime = global->currentGameTime +
		(1000 * randomizer.GetRandomValue_normal_moreFlat(TARGETING_CONST::updateDetailedDislikeInfoMinDelay, TARGETING_CONST::updateDetailedDislikeInfoMaxDelay));

	bool ignoreThisPlayer[9];
	for (int i = 1; i < 9; i++) {
		ignoreThisPlayer[i] = false;
	}
	ignoreThisPlayer[0] = true; // always ignore gaia
	long int totalAttacksDuringPeriod = 0;
	int numberOfValidEnemyPlayers = 0;

	// Reset values for all players (even unused ones, just in case)
	for (int i = 0; i < 9; i++) {
		this->lastComputedDislikeSubScore[i] = 0;
	}

	int randomImpact = TARGETING_CONST::persistenceDelayOfIndividualEnemyAttacksInTargetingHistoryRandomImpact;
	int maxDelayToSearchForEnemyAttacks = TARGETING_CONST::persistenceDelayOfIndividualEnemyAttacksInTargetingHistory +
		randomizer.GetRandomValue(-randomImpact, randomImpact);
	if (maxDelayToSearchForEnemyAttacks < 1) { maxDelayToSearchForEnemyAttacks = 1; }
	long int periodBeginTime = global->currentGameTime - maxDelayToSearchForEnemyAttacks;
	long int periodEndTime = global->currentGameTime;

	// Collect individual information
	for (int targetPlayerId = 1; targetPlayerId < global->playerTotalCount; targetPlayerId++) {
		STRUCT_PLAYER *targetPlayer = global->GetPlayerStruct(targetPlayerId);
		assert(targetPlayer && targetPlayer->IsCheckSumValid());
		if (!targetPlayer || !targetPlayer->IsCheckSumValid()) { continue; }
		if ((targetPlayer->aliveStatus > 0) || (targetPlayer->playerId == this->myPlayerId) ||
			(player->ptrDiplomacyStances[targetPlayerId] == 0)) { // 0 means allied
			ignoreThisPlayer[targetPlayerId] = true;
			continue;
		}
		numberOfValidEnemyPlayers++;

		if (this->militaryAIInfo) {
			totalAttacksDuringPeriod += this->militaryAIInfo->GetAttacksCountFromPlayerInPeriod(targetPlayerId, periodBeginTime, periodEndTime);
		}
	}

	int averageAttackCountByPlayerDuringPeriod = (numberOfValidEnemyPlayers > 0) ? totalAttacksDuringPeriod / numberOfValidEnemyPlayers : 0;

	// Processing part
	for (int targetPlayerId = 1; targetPlayerId < global->playerTotalCount; targetPlayerId++) {
		if (ignoreThisPlayer[targetPlayerId]) {
			this->lastComputedDislikeSubScore[targetPlayerId] = 0;
			continue;
		}
		STRUCT_PLAYER *targetPlayer = global->GetPlayerStruct(targetPlayerId);
		// targetPlayer pointer was already checked previously, cf ignoreThisPlayer[targetPlayerId]


		// TODO: compute each lastComputedDislikeSubScore
		// + other rules (current target, attacking me, enemy towers near my town, buildings IN my town? etc)
		// player with lots of towers ?
		// Attacking a allied player ?

		if (this->militaryAIInfo) {
			int attacksCountByPlayer = this->militaryAIInfo->GetAttacksCountFromPlayerInPeriod(targetPlayerId, periodBeginTime, periodEndTime);
			if (attacksCountByPlayer > averageAttackCountByPlayerDuringPeriod) {
				this->lastComputedDislikeSubScore[targetPlayerId] += TARGETING_CONST::dislikeSubScoreAttackedMeMoreThanAverage; // This player attacked me "more than average" => I don't like him
			}

			if (this->militaryAIInfo->GetPanicModesCountFromPlayerInPeriod(targetPlayerId, periodBeginTime, periodEndTime) > 0) {
				this->lastComputedDislikeSubScore[targetPlayerId] += TARGETING_CONST::dislikeSubScoreAttackedMyTown;
			}
		}

		int randomFactor = randomizer.GetRandomValue_normal_moderate(0, TARGETING_CONST::dislikeSubScoreRandomFactor);
		this->lastComputedDislikeSubScore[targetPlayerId] += randomFactor;


	}


	AOE_STRUCTURES::STRUCT_UNIT_BASE *myTC = AOE_MainAI_findUnit(player->ptrAIStruct, CST_UNITID_FORUM);
	if (myTC && !myTC->IsCheckSumValidForAUnitClass()) { myTC = NULL; }

	long int spottedEnemyUnits[9];
	for (int i = 0; i < 9; i++) {
		spottedEnemyUnits[i] = 0;
	}

	// Analyze known enemy units ?
	// TODO: this should be done along the game when "I" receive alerts from attacks, and reset it at each re-computation
	for (int i = 0; i < player->ptrAIStruct->structInfAI.unitElemListSize; i++) {
		long int unitPlayerId = player->ptrAIStruct->structInfAI.unitElemList[i].playerId;
		bool ignoreThisUnit = ignoreThisPlayer[unitPlayerId];
		long int unitPosX = player->ptrAIStruct->structInfAI.unitElemList[i].posX;
		long int unitPosY = player->ptrAIStruct->structInfAI.unitElemList[i].posY;
		if (!ignoreThisUnit) {
			bool isVisible = IsFogVisibleForPlayer(player->playerId, unitPosX, unitPosY); // fast operation: ok with performance
			if (isVisible) {
				STRUCT_UNIT_BASE *unit = global->GetUnitFromId(player->ptrAIStruct->structInfAI.unitElemList[i].unitId);
				if (!unit) {
					// Remove from list ? (no cheating: position is fog-visible)
					// Do not remove during the loop, do it afterwards
				} else {
					// By the way, update unit position in infAI list (it is often NOT up to date)
					long int unitPosX = (long int)unit->positionX;
					long int unitPosY = (long int)unit->positionY;
					player->ptrAIStruct->structInfAI.unitElemList[i].posX = (char)unitPosX;
					player->ptrAIStruct->structInfAI.unitElemList[i].posY = (char)unitPosY;
				}
			} else {
				// Remove from list (can be added back later) ? Warning, might impact severely AI as it cheats a bit in attack phases. Fix/improve AI attack first ?
				// Do not remove during the loop, do it afterwards
			}
				
			STRUCT_PLAYER *targetPlayer = global->GetPlayerStruct(unitPlayerId);
			bool isAggressive = IsNonTowerMilitaryUnit(player->ptrAIStruct->structInfAI.unitElemList[i].unitClass);
			if (targetPlayer && targetPlayer->IsCheckSumValid()) {
				// Using the "unitDef*" overload is better than "unitDefId" overload (which is hardcoded).
				isAggressive |= IsTower(targetPlayer->GetUnitDefBase(player->ptrAIStruct->structInfAI.unitElemList[i].unitDATID));
			}
				
			ignoreThisUnit = ignoreThisUnit || !isVisible || !isAggressive;
		}

		if (!ignoreThisUnit && myTC) {
			long int sqDistToMyTC = ((unitPosX - (long int)myTC->positionX) * (unitPosX - (long int)myTC->positionX)) +
				((unitPosY - (long int)myTC->positionY) * (unitPosY - (long int)myTC->positionY)); // distance^2
			bool isInMyTown = sqDistToMyTC < CUSTOM_AI::AI_CONST::townSizeSquare;
			bool isNearMyTown = !isInMyTown && (sqDistToMyTC < CUSTOM_AI::AI_CONST::townNeighborhoodSizeSquare); // between town limit & neighborhood max distance

			if (isInMyTown) {
				spottedEnemyUnits[unitPlayerId] += 3;
			}
			if (isNearMyTown)
			{
				spottedEnemyUnits[unitPlayerId]++;
			}
		}
	}
	long int maxSpottedUnitCount = 0;
	for (int i = 0; i < global->playerTotalCount; i++) {
		if (spottedEnemyUnits[i] > maxSpottedUnitCount) {
			maxSpottedUnitCount = spottedEnemyUnits[i];
		}
	}
	for (int i = 0; i < global->playerTotalCount; i++) {
		if (spottedEnemyUnits[i] == maxSpottedUnitCount) {
			// Add score to all players (possibly more than 1) with the most (military) units in my town
			this->lastComputedDislikeSubScore[i] += TARGETING_CONST::dislikeSubScoreMostSpottedUnitsMyTown;
		}
	}
		
	// To ensure some continuity in targeting, current target is given a small boost (not forever, this is limited to "msAfterWhichCurrentTargetLosesExtraValue")
	if ((player->ptrAIStruct->structTacAI.targetPlayers.usedElements > 0) &&
		(this->lastTargetPlayerChangeGameTime - global->currentGameTime > TARGETING_CONST::msAfterWhichCurrentTargetLosesExtraValue))
	{
		int currentTargetPlayerId = player->ptrAIStruct->structTacAI.targetPlayers.unitIdArray[0];
		this->lastComputedDislikeSubScore[currentTargetPlayerId] += TARGETING_CONST::extraValueForCurrentTarget;
	}

	for (int i = 0; i < 9; i++) {
		if (this->lastComputedDislikeSubScore[i] < 0) {
			this->lastComputedDislikeSubScore[i] = 0; // Make sure to avoid negative values.
		}
	}

	return true;
}



PlayerTargeting::PlayerTargeting() {
}

// Reset all underlying info (useful at game start, etc)
void PlayerTargeting::ResetAllInfo() {
	for (int i = 0; i < 9; i++) {
		this->GetPlayerInfo(i)->ResetInfo();
		this->GetPlayerInfo(i)->myPlayerId = i;
	}
}

// Safely get a player's info
AIPlayerTargetingInfo *PlayerTargeting::GetPlayerInfo(long int playerId) {
	if ((playerId < 0) || (playerId > 8)) { return 0; }
	return &this->playersAIInfo[playerId];
}


// Returns a 0-100 factor according to a delay (in game "years") corresponding to a victory condition timer (up to 2000 "years")
// yearsDelay should be >=0
long int PlayerTargeting::GetPercentFactorFromVictoryConditionDelay(long int yearsDelay) {
	assert(yearsDelay >= 0);
	if (yearsDelay < 0) { yearsDelay = 0; }
	int delayProportion = TARGETING_CONST::yearsCountInArtefactsVictoryDelay - yearsDelay;
	if (delayProportion < 0) { delayProportion = 0; } // now delayProportion is 0<=x<=2000
	delayProportion = delayProportion / (TARGETING_CONST::yearsCountInArtefactsVictoryDelay / 100); // get a 0-100 value, ~100 representing a low remaining delay !
	return delayProportion;
}


// Returns the most disliked playerIdn, impacting which player "I" will attack.
// Note: standard ROR code (0x40ACDA) is total crap : applies "score/factor" instead of "score*factor/100", and when attackWinningPlayerFactor=false, the score factor is substracted (instead of ignored)
long int PlayerTargeting::GetMostDislikedPlayer(STRUCT_PLAYER *player, STRUCT_DIPLOMACY_AI *diplAI,
	long int askTributeAmount, long int askTributePlayerId, bool attackWinningPlayer, long int attackWinningPlayerFactor) {
	assert(player && player->IsCheckSumValid());
	assert(diplAI && diplAI->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid() || !diplAI || !diplAI->IsCheckSumValid()) { return -1; }
	STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return -1; }
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return -1; }

	if (player->aliveStatus > 0) { return -1; }
	if ((player->playerId < 1) || (player->playerId >= global->playerTotalCount)) { return -1; }
	AIPlayerTargetingInfo *playerTargetInfo = this->GetPlayerInfo(player->playerId);
	if (playerTargetInfo) {
		// Update internal info for current player (info is not updated on each call, for both performance and to avoid changing target on each call here !)
		playerTargetInfo->RecomputeInfo(player);
	}
	assert(attackWinningPlayerFactor <= 100);
	if (attackWinningPlayerFactor > 100) { attackWinningPlayerFactor = 100; }
	if (attackWinningPlayerFactor < 0) { attackWinningPlayerFactor = 0; }

	// TEST: disable targeting when military population is very low ?
	int currentVillagerCount = (int)player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_CIVILIAN_POPULATION);
	int totalPopulation = (int)player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_CURRENT_POPULATION);
	if (totalPopulation - currentVillagerCount < 3) {
		return -1; // Less than 3 military units: forget about targeting enemies
	}

	bool hasStandardVictoryCondition = (global->generalVictoryCondition == GAME_GLOBAL_GENERAL_VICTORY_CONDITION::GGVC_STANDARD);

	long int mostDislikedPlayerId = -1;
	long int bestDislikeValue = -1;

	bool hasBuiltWonder[9];
	bool hasInProgressWonder[9];
	long int allRelicsOrRuinsCounter[9]; // 0=normal, 1=all relics OR all ruins, 2=all relics AND all ruins
	for (int i = 0; i < 9; i++) {
		hasBuiltWonder[i] = false;
		hasInProgressWonder[i] = false;
		allRelicsOrRuinsCounter[i] = 0;
	}

	// Count wonders/relics/ruins : they are public-visible: there is no cheating in counting them this way
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		STRUCT_PLAYER *loopPlayer = global->GetPlayerStruct(loopPlayerId);
		if (loopPlayer && loopPlayer->IsCheckSumValid() && (loopPlayerId != player->playerId) && (loopPlayer->aliveStatus != 2) &&
			(player->ptrDiplomacyStances[loopPlayerId] != 0) && loopPlayer->ptrBuildingsListHeader) {
			// Getting those 3 specific resources is no cheating (whereas getting food amount would be !)
			if (loopPlayer->GetResourceValue(CST_RES_ORDER_ALL_RUINS) > 0) {
				allRelicsOrRuinsCounter[loopPlayerId]++;
			}
			if (loopPlayer->GetResourceValue(CST_RES_ORDER_ALL_RELICS) > 0) {
				allRelicsOrRuinsCounter[loopPlayerId]++;
			}
			if (loopPlayer->GetResourceValue(CST_RES_ORDER_STANDING_WONDERS) > 0) {
				hasBuiltWonder[loopPlayerId] = true;
			} else {
				// Optimization: loop in player's buildings only if no standing wonder found in resources
				// Cons: ignores being-built wonders when a standing one exists.
				// Pros: avoids looping on the whole list in most cases !
				for (int i = 0; i < loopPlayer->ptrBuildingsListHeader->buildingsArrayElemCount; i++) {
					if (loopPlayer->ptrBuildingsListHeader->ptrBuildingsArray[i] && loopPlayer->ptrBuildingsListHeader->ptrBuildingsArray[i]->unitDefinition &&
						(loopPlayer->ptrBuildingsListHeader->ptrBuildingsArray[i]->unitDefinition->DAT_ID1 == CST_UNITID_WONDER)) {
						switch (loopPlayer->ptrBuildingsListHeader->ptrBuildingsArray[i]->unitStatus) {
						case GAME_UNIT_STATUS::GUS_0_NOT_BUILT:
						case GAME_UNIT_STATUS::GUS_1_UNKNOWN_1:
							hasInProgressWonder[loopPlayerId] = true;
							break;
						case GAME_UNIT_STATUS::GUS_2_READY:
							// This loop is not supposed to detect built wonders (already found in resource).
							assert(false && "Inconsistentcy: a wonder was not detected in resources");
							hasBuiltWonder[loopPlayerId] = true;
							break;
						default:
							// >= 3 : dying, ignore it
							break;
						}
					}
					if (hasInProgressWonder[loopPlayerId]) {
						break; // We don't need to continue looping on all buildings. This will continue with next playerId loop.
					}
				}
			}
		}
	}


	// Loop on all non-gaia players
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		STRUCT_PLAYER *loopPlayer = global->GetPlayerStruct(loopPlayerId);
		if (loopPlayer && loopPlayer->IsCheckSumValid() && (loopPlayerId != player->playerId) &&
			loopPlayer->ptrScoreInformation && loopPlayer->ptrScoreInformation->IsCheckSumValid() &&
			(loopPlayer->aliveStatus != 2)) { // Cf 0x40ACFA: exclude defeated players
			assert((loopPlayerId >= 1) && (loopPlayerId < global->playerTotalCount) && (loopPlayerId <= 8));
			if (player->ptrDiplomacyStances[loopPlayerId] != 0) { // Not allied, cf call in 0x40AD14)
				long int playerScoreFactor = 0; // default: no impact
				if (attackWinningPlayer && (attackWinningPlayerFactor > 0)) {
					// Note: in game code, the formula is completely erroneous (DIVIDES by factor !)
					playerScoreFactor = loopPlayer->ptrScoreInformation->currentTotalScore * attackWinningPlayerFactor / 100;
					// Note: In game code, there is a "else" that does the opposite effect if attackWinningPlayer is false (factor is substracted !)
				}

				// Handle some priority rules (NOT in standard game)

				long int otherDislikeAmount = 0;
				if (hasStandardVictoryCondition) {
					// Wonders/relics/ruins can trigger victory in standard conditions
					if (hasInProgressWonder[loopPlayerId]) {
						// Building a wonder: victory timer is not launched yet, but this is critical though.
						otherDislikeAmount += TARGETING_CONST::dislikeAmountWinningWonderInConstruction;
					}
					if (hasBuiltWonder[loopPlayerId]) {
						// Player has a standing wonder (which leads to victory)
						int wonderVictoryDelay = settings->playerWondersVictoryDelays[loopPlayerId];
						assert(wonderVictoryDelay > -1); // standard victory condition + player has a wonder = delay should be set
							
						int wonderDelayProportion = PlayerTargeting::GetPercentFactorFromVictoryConditionDelay(wonderVictoryDelay);
						otherDislikeAmount += TARGETING_CONST::dislikeAmountWinningWonderBuilt + wonderDelayProportion; // Very top priority ! However other criteria can still make the decision between 2 players having a wonder.
					}

					if (player->remainingTimeToAllRelicsVictory > -1) {
						// Player has all relics (which leads to victory)
						int relicsDelayProportion = PlayerTargeting::GetPercentFactorFromVictoryConditionDelay((int)player->remainingTimeToAllRelicsVictory);
						otherDislikeAmount += TARGETING_CONST::dislikeAmountWinningAllArtefacts + relicsDelayProportion;
					}
					if (player->remainingTimeToAllRuinsVictory > -1) {
						// Player has all ruins (which leads to victory)
						int ruinsDelayProportion = PlayerTargeting::GetPercentFactorFromVictoryConditionDelay((int)player->remainingTimeToAllRuinsVictory);
						otherDislikeAmount += TARGETING_CONST::dislikeAmountWinningAllArtefacts + ruinsDelayProportion;
					}
				} else {
					// Wonders/relics/ruins are not (direct) victory conditions. Impact is reduced.
					if (hasInProgressWonder[loopPlayerId]) {
						otherDislikeAmount += TARGETING_CONST::dislikeAmountNoWinningWonderInConstruction;
					}
					if (hasBuiltWonder[loopPlayerId]) {
						otherDislikeAmount += TARGETING_CONST::dislikeAmountNoWinningWonderBuilt;
					}
					otherDislikeAmount += allRelicsOrRuinsCounter[loopPlayerId] * TARGETING_CONST::dislikeAmountNoWinningAllArtefacts;
				}

				long int thisDislikeValue = diplAI->dislikeTable[loopPlayerId] + playerScoreFactor + otherDislikeAmount;
				if (playerTargetInfo) {
					thisDislikeValue += playerTargetInfo->lastComputedDislikeSubScore[loopPlayerId];
				}
				if (thisDislikeValue > bestDislikeValue) {
					bestDislikeValue = thisDislikeValue;
					mostDislikedPlayerId = loopPlayerId;
				}
			}
		}
	}

	if (player->ptrAIStruct) {
		if ((player->ptrAIStruct->structTacAI.targetPlayers.usedElements == 0) ||
			(mostDislikedPlayerId != player->ptrAIStruct->structTacAI.targetPlayers.unitIdArray[0])) {
			// Target player changed: save game time
			playerTargetInfo->lastTargetPlayerChangeGameTime = global->currentGameTime;
		}
	}
#ifdef _DEBUG
	// TEST
	/*std::string msg = "p#";
	msg += std::to_string(player->playerId);
	msg += ": target=";
	msg += std::to_string(mostDislikedPlayerId);
	CallWriteText(msg.c_str());*/
	/*std::string msg = "p#";
	msg += std::to_string(player->playerId);
	msg += " target=";
	msg += std::to_string(mostDislikedPlayerId);
	traceMessageHandler.WriteMessageNoNotification(msg);*/
#endif
	return mostDislikedPlayerId;
}



void PlayerTargeting::ComputeDislikeValues() {
	if ((CUSTOMROR::crInfo.configInfo.dislike_allArtefacts <= 0) || (CUSTOMROR::crInfo.configInfo.dislike_humanPlayer <= 0)) { return; }

	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	assert(globalStruct->GetPlayerStructPtrTable() != NULL);
	unsigned long int newDislikeValues[9];
	for (int i = 0; i < 9; i++) { newDislikeValues[i] = CST_DISLIKE_INITIAL_VALUE; }

	// Calculate dislike "penalty" for each player.
	for (int iPlayerId = 1; iPlayerId < globalStruct->playerTotalCount; iPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = globalStruct->GetPlayerStructPtrTable()[iPlayerId];
		if (player && player->ptrAIStruct) {
			float *resources = (float *)player->ptrResourceValues;
			if (resources[CST_RES_ORDER_STANDING_WONDERS]) { newDislikeValues[iPlayerId] += CUSTOMROR::crInfo.configInfo.dislike_allArtefacts; }
			if (resources[CST_RES_ORDER_ALL_RUINS]) { newDislikeValues[iPlayerId] += CUSTOMROR::crInfo.configInfo.dislike_allArtefacts; }
			if (resources[CST_RES_ORDER_ALL_RELICS]) { newDislikeValues[iPlayerId] += CUSTOMROR::crInfo.configInfo.dislike_allArtefacts; }
			if (player->isComputerControlled == 0) { newDislikeValues[iPlayerId] += CUSTOMROR::crInfo.configInfo.dislike_humanPlayer; }
		}
	}

	for (int iPlayerId = 1; iPlayerId < globalStruct->playerTotalCount; iPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = globalStruct->GetPlayerStructPtrTable()[iPlayerId];
		if (player) {
			for (int iTargetPlayerId = 1; iTargetPlayerId < globalStruct->playerTotalCount; iTargetPlayerId++) {
				AOE_STRUCTURES::STRUCT_PLAYER *targetPlayer = globalStruct->GetPlayerStructPtrTable()[iTargetPlayerId];
				if ((iPlayerId != iTargetPlayerId) && (player->diplomacyVSPlayers[iTargetPlayerId] > 2) && (player->ptrAIStruct)) {
					// Set the new calculated dislike value against player #iTargetPlayerId
					player->ptrAIStruct->structDiplAI.dislikeTable[iTargetPlayerId] = newDislikeValues[iTargetPlayerId];
				}
			}
		}
	}
}

}
