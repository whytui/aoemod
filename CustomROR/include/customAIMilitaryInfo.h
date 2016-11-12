#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include "CustomRORInfo.h"
#include "EnemyAttacksHistory.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	namespace AI_CONST {
		const long int townSize = 22; // In tiles. See also SNMinimumTownSize, SNMaximumTownSize, etc. Actual AI towns can be >20 tiles.
		const long int townSizeSquare = townSize * townSize;
		const long int townNeighborhoodSize = 30;
		const long int townNeighborhoodSizeSquare = townNeighborhoodSize * townNeighborhoodSize;
		const long int townDefendSizeIfWeak = 16; // A limited territory in defense considerations when player has few resources.
		const long int delayInWhichEnemyAttacksImpactUnitGroupTasking_ms = 15000; // The time in milliseconds we go back in history to count enemy attacks when tasking unit groups
	}


	// Stores information about military topics for custom AI (for a specific player)
	class CustomAIMilitaryInfo {
	public:
		CustomAIMilitaryInfo() {
			this->ResetAllInfo();
		}

		TimeIntervalAttacksRecordForPlayer<TimeIntervalAttackRecord> recentAttacksByPlayer[9];

		void ResetAllInfo();

		// Securely get the attacks information for the player specified. Returns NULL if playerId is invalid.
		TimeIntervalAttacksRecordForPlayer<TimeIntervalAttackRecord> *GetAttackInfoForPlayer(long int attackerPlayerId);

		TimeIntervalAttackRecord *GetAttackInfoForCurrentTimeInterval(long int attackerPlayerId, long int currentGameTime);

		// Returns true if successful
		// myTownCenter is used to evaluate my town position, it is NOT the target of the attack
		bool SaveEnemyAttackInHistory(long int attackerPlayerId, long int currentGameTime, 
			STRUCT_UNIT_BASE *enemyUnit, STRUCT_UNIT_BASE *myTownCenter);

		// Get the number of attacks from a player during the specified interval (game times in milliseconds)
		int GetAttacksCountFromPlayerInPeriod(long int attackerPlayerId, long int startGameTime, long int endGameTime);

		// Returns true if successful
		bool SavePanicModeOccurrenceInHistory(long int attackerPlayerId, long int currentGameTime);

		// Get the number of panic mode caused by a player during the specified interval (game times in milliseconds)
		int GetPanicModesCountFromPlayerInPeriod(long int attackerPlayerId, long int startGameTime, long int endGameTime);

		// Returns a value 0-100 to evaluate player weakness. 0 means player is very weak (no more resources), 100=player is rich.
		// Criteria are resources and civilian population
		// TODO: this could go somewhere else
		int EvaluatePlayerWeakness(STRUCT_PLAYER *player);

		// Returns true if unit positions (my unit, TC and enemy unit) are located in such way that panic mode can be triggered.
		// This is an approximative evaluation, not a complete analysis.
		static bool IsPanicModeEligible(STRUCT_UNIT_BASE *myTC, STRUCT_UNIT_BASE *myUnit, STRUCT_UNIT_BASE *enemyUnit);
	};
	
}
