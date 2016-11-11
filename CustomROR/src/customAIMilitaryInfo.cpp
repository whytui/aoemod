#include "../include/customAIMilitaryInfo.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	void CustomAIMilitaryInfo::ResetAllInfo() {
		for (int i = 0; i < 9; i++) {
			this->recentAttacksByPlayer[i].ResetAllInfo();
			this->recentAttacksByPlayer[i].attackerPlayerId = i;
			this->attacksByEnemyPlayersDuringLastPeriod[i] = 0;
			this->panicModeProvokedByEnemyPlayersDuringLastPeriod[i] = 0;
			this->previousAttackCountsByEnemyPlayers[i] = 0;
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

}

