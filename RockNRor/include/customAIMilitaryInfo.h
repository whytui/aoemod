#pragma once

#include <assert.h>
#include <macroUtil.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include <AOE_struct_game_map_info.h>
#include <AOE_struct_inf_ai.h>
#include "RockNRorInfo.h"
#include "EnemyAttacksHistory.h"
#include "unitHandling.h"
#include "playerHandling.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	namespace AI_CONST {
		const long int townSize = 22; // In tiles. See also SNMinimumTownSize, SNMaximumTownSize, etc. Actual AI towns can be >20 tiles.
		const long int townSizeSquare = townSize * townSize;
		const long int townNeighborhoodSize = 30;
		const long int townNeighborhoodSizeSquare = townNeighborhoodSize * townNeighborhoodSize;
		const long int townDefendSizeIfWeak = 16; // A limited territory in defense considerations when player has few resources.
		const long int delayInWhichEnemyAttacksImpactUnitGroupTasking_ms = 15000; // The time in milliseconds we go back in history to count enemy attacks when tasking unit groups
		const long int minimumDelayBetweenBasicUnitGroupAttackTasking_ms = 1500; // Time to wait between 2 "attack" tasking operations for unit groups (all situations, including defensive).
		const long int delayBetweenUnitGroupAttackTasking_ms = 10000; // Time to wait between 2 (non-defensive) "attack" tasking operations for unit groups.
		const long int armySizeToConsiderStrong = 20; // Number of military units above which I consider myself strong in a military way
	}


	enum MILITARY_SITUATION {
		MS_UNKNOWN = -1,
		MS_CRITICAL, // {few resources and few military and recent panic modes} or {very few resources and recently attacked}
		MS_WEAK, // {recent panic modes} or {very few resources}
		MS_NORMAL,
		MS_STRONG // Large military numbers, with at least 2 land (military) unit groups
	};


	// Stores information about military topics for custom AI (for a specific player)
	class CustomAIMilitaryInfo {
	public:
		CustomAIMilitaryInfo() {
			this->ResetAllInfo();
		}

		MILITARY_SITUATION lastKnownMilitarySituation;
		unsigned long int lastKnownMilitarySituationComputationGameTime;

		// Stores information about recent history of being attacked by other players
		TimeIntervalAttacksRecordForPlayer<TimeIntervalAttackRecord> recentAttacksByPlayer[9];
		
		// Refers to an enemy building spotted in my town (potential target for idle units or in-town groups) (unit id)
		long int unitIdEnemyBuildingInMyTown;
		// Refers to an enemy tower spotted in my town (unit id)
		long int unitIdEnemyTowerInMyTown;

		void ResetAllInfo();

		// Securely get the attacks information for the player specified. Returns NULL if playerId is invalid.
		TimeIntervalAttacksRecordForPlayer<TimeIntervalAttackRecord> *GetAttackInfoForPlayer(long int attackerPlayerId);

		TimeIntervalAttackRecord *GetAttackInfoForCurrentTimeInterval(long int attackerPlayerId, long int currentGameTime);

		// Sets the "last known military situation" variables
		void SetLastKnownMilitarySituation(MILITARY_SITUATION situation, unsigned long int gameTime_ms);

		// Returns true if successful
		// myTownCenter is used to evaluate my town position, it is NOT the target of the attack. myTownCenter may be NULL !
		bool SaveEnemyAttackInHistory(long int attackerPlayerId, long int currentGameTime, 
			STRUCT_UNIT_BASE *enemyUnit, STRUCT_UNIT_BASE *myTownCenter);

		// Get the number of attacks from a player during the specified interval (game times in milliseconds)
		int GetAttacksCountFromPlayerInPeriod(long int attackerPlayerId, long int startGameTime, long int endGameTime);

		// Get the number of attacks *in my town* from a player during the specified interval (game times in milliseconds)
		int GetAttacksInTownCountFromPlayerInPeriod(long int attackerPlayerId, long int startGameTime, long int endGameTime);

		// Returns true if successful
		bool SavePanicModeOccurrenceInHistory(long int attackerPlayerId, long int currentGameTime);

		// Get the number of strategy panic mode caused by a player during the specified interval (game times in milliseconds)
		int GetPanicModesCountFromPlayerInPeriod(long int attackerPlayerId, long int startGameTime, long int endGameTime);

		// Returns a value 0-100 to evaluate player weakness. 0 means player is very weak (no more resources), 100=player is rich.
		// Criteria are resources and civilian population
		// TODO: this could go somewhere else
		int EvaluatePlayerWeakness(STRUCT_PLAYER *player);

		// Returns true if unit positions (my unit, TC and enemy unit) are located in such way that panic mode can be triggered.
		// This is an approximative evaluation, not a complete analysis.
		static bool IsPanicModeEligible(STRUCT_UNIT_BASE *myTC, STRUCT_UNIT_BASE *myUnit, STRUCT_UNIT_BASE *enemyUnit);

		// Get main town center, or the "main" unit to defend if no TC is found
		// This is not supposed to return NULL for a valid player (alive) because we should always find at least 1 valid unit
		static STRUCT_UNIT_BASE *GetTownCenterOrUnitToDefend(STRUCT_PLAYER *player);

		// Returns infAI elem list entry for the first found enemy or neutral military unit in specified zone
		// startIndex = index of first element to search in infAI elem list. Use 0 or -1 to start from beginning.
		// Returns -1 if not found
		// Excludes units that cannot attack (villagers, fishing boats, transports...)
		static long int FindInfAiIndexOfEnemyMilitaryUnitNearPosition(STRUCT_PLAYER *player, long int posX, long int posY,
			long int distanceFromCenter, bool landOnly, long int startIndex);
	};
	
}
