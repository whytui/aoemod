#include "../include/customAIMilitaryInfo.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	void CustomAIMilitaryInfo::ResetAllInfo() {
		for (int i = 0; i < 9; i++) {
			this->recentAttacksByPlayer[i].ResetAllInfo();
			this->recentAttacksByPlayer[i].attackerPlayerId = i;
		}
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

	bool CustomAIMilitaryInfo::SaveEnemyAttackInHistory(long int attackerPlayerId, long int currentGameTime) {
		if ((attackerPlayerId < 0) || (attackerPlayerId > 8)) { return false; }
		CUSTOM_AI::TimeIntervalAttackRecord *interval = this->GetAttackInfoForCurrentTimeInterval(attackerPlayerId, currentGameTime);
		if (interval) {
			interval->attacksCount++;
		}
		return (interval != NULL);
	}


	// Get the number of attacks from a player during the specified interval (game times in milliseconds)
	int CustomAIMilitaryInfo::GetAttacksCountFromPlayerInPeriod(long int attackerPlayerId, long int startGameTime, long int endGameTime) {
		if ((attackerPlayerId < 0) || (attackerPlayerId > 8)) { return 0; }
		return this->recentAttacksByPlayer[attackerPlayerId].GetAttacksCountInPeriod(startGameTime, endGameTime);
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

	// Get the number of panic mode caused by a player during the specified interval (game times in milliseconds)
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
		float score = (food + wood + stone + gold) * 100 / maxResValueToEvaluatePlayerWeakness;
		// Out of a key resource: negative impact
		if (food < 100) {
			score = score* 0.8f;
		}
		if (wood < 100) {
			score = score* 0.9f;
		}
		if (gold < 100) {
			score = score* 0.9f;
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
		if (villagerProportion > 100) { villagerProportion = 100; } // if more villagers than expected, ignore it (no bonus)
		float villagersCountImpact = 0.15f; // 15%
		score = score * (1 - (score * villagersCountImpact / 100));

		if (score < 0) { score = 0; }
		if (score > 100) { score = 100; }
		return (int)score;
	}

}

