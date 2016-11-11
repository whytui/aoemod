#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_main_ai.h>
#include "CustomRORInfo.h"
#include "StrategyUpdater.h"
#include "EnemyAttacksHistory.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	namespace AI_CONST {
		const long int townSize = 20; // In tiles
		const long int townSizeSquare = townSize * townSize;
		const long int townNeighborhoodSize = 30;
		const long int townNeighborhoodSizeSquare = townNeighborhoodSize * townNeighborhoodSize;
	}


	// Stores information about military topcis for custom AI (for a specific player)
	class CustomAIMilitaryInfo {
	public:
		CustomAIMilitaryInfo() {
			this->ResetAllInfo();
		}

		long int previousAttackCountsByEnemyPlayers[9]; // Number of enemy attacks (from each player) when last update was run
		// Number of enemy attacks (from each player) during last period of time
		// Warning: as this also counts towers shooting my explorers (for example), this information must be used carefully
		long int attacksByEnemyPlayersDuringLastPeriod[9];
		long int panicModeProvokedByEnemyPlayersDuringLastPeriod[9]; // Counter of "panic modes" triggered by each player during last period of time.

		TimeIntervalAttacksRecordForPlayer<TimeIntervalAttackRecord> recentAttacksByPlayer[9];

		void ResetAllInfo();

		// Securely get the attacks information for the player specified. Returns NULL if playerId is invalid.
		TimeIntervalAttacksRecordForPlayer<TimeIntervalAttackRecord> *GetAttackInfoForPlayer(long int attackerPlayerId);

		TimeIntervalAttackRecord *GetAttackInfoForCurrentTimeInterval(long int attackerPlayerId, long int currentGameTime);

		// Returns true if successful
		bool SaveEnemyAttackInHistory(long int attackerPlayerId, long int currentGameTime);

		// Get the number of attacks from a player during the specified interval (game times in milliseconds)
		int GetAttacksCountFromPlayerInPeriod(long int attackerPlayerId, long int startGameTime, long int endGameTime);
	};
	
}
