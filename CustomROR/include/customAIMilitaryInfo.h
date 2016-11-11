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

		TimeIntervalAttacksRecordForPlayer recentAttacksByPlayer[9];

		void ResetAllInfo() {
			for (int i = 0; i < 9; i++) {
				this->recentAttacksByPlayer[i].ResetAllInfo();
				this->recentAttacksByPlayer[i].attackerPlayerId = i;
				this->attacksByEnemyPlayersDuringLastPeriod[i] = 0;
				this->panicModeProvokedByEnemyPlayersDuringLastPeriod[i] = 0;
				this->previousAttackCountsByEnemyPlayers[i] = 0;
			}
		}

	};
	
}
