#pragma once

#include <assert.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_global.h>
#include <AOE_struct_player.h>
#include <AOE_struct_diplomacy_ai.h>
#include <AOE_struct_main_ai.h>
#include "mainStructuresHandling.h"
#include "randomizer.h"

using namespace std;
using namespace AOE_STRUCTURES;

namespace CUSTOMROR {

	const long int updateDetailedDislikeInfoMinDelay = 10; // Minimum delay between full recomputing of custom dislike information. In seconds.
	const long int updateDetailedDislikeInfoMaxDelay = 40; // Maximum delay between full recomputing of custom dislike information. In seconds.

	class AIPlayerTargetingInfo {
	public:
		AIPlayerTargetingInfo();

		long int myPlayerId;
		long int lastUpdateGameTime;
		long int nextUpdateGameTime;
		long int lastComputedDislikeSubScore[9];
		long int previousAttackCountsByEnemyPlayers[9]; // Number of enemy attacks (from each player) when last update was run
		// Number of enemy attacks (from each player) during last period of time
		// Warning: as this also counts towers shooting my explorers (for example), this information must be used carefully
		long int attacksByEnemyPlayersDuringLastPeriod[9];

		// Reset all underlying info (useful at game start, etc)
		void ResetInfo();

		// Recompute information (only) if refresh delay has been reached
		// Returns true if information have been recomputed (false is not necessarily an error)
		bool RecomputeInfo(STRUCT_PLAYER *player);
	};

	class PlayerTargeting {
	public:
		PlayerTargeting();

		// Reset all underlying info (useful at game start, etc)
		void ResetAllInfo();
		// Safely get a player's info
		AIPlayerTargetingInfo *GetPlayerInfo(long int playerId);

		// Returns a 0-100 factor according to a delay (in game "years") corresponding to a victory condition timer (up to 2000 "years")
		// yearsDelay should be >=0
		static long int GetPercentFactorFromVictoryConditionDelay(long int yearsDelay);

		// Returns the most disliked playerIdn, impacting which player "I" will attack.
		long int GetMostDislikedPlayer(STRUCT_PLAYER *player, STRUCT_DIPLOMACY_AI *diplAI,
			long int askTributeAmount, long int askTributePlayerId, bool attackWinningPlayer, long int attackWinningPlayerFactor);


	private:
		AIPlayerTargetingInfo playersAIInfo[9];
	};

	// A unique global object for everyone
	extern PlayerTargeting playerTargetingHandler;
}
