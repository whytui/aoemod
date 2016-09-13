#pragma once

#include <assert.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_global.h>
#include <AOE_struct_player.h>
#include <AOE_struct_diplomacy_ai.h>
#include <AOE_struct_main_ai.h>
#include "mainStructuresHandling.h"
#include "randomizer.h"
#include "crCommon.h"

using namespace std;
using namespace AOE_STRUCTURES;

namespace CUSTOMROR {

	// Configuration for targeting evaluation
	namespace TARGETING_CONST {
		const long int yearsCountInArtefactsVictoryDelay = 2000; // And wonder delay too
		const long int updateDetailedDislikeInfoMinDelay = 10; // Minimum delay between full recomputing of custom dislike information. In seconds.
		const long int updateDetailedDislikeInfoMaxDelay = 40; // Maximum delay between full recomputing of custom dislike information. In seconds.
		// Dislike values
		const long int dislikeAmountWinningAllArtefacts = 100; // Triggering a victory condition after a 2000 years delay
		const long int dislikeAmountWinningWonderBuilt = 100; // Triggering a victory condition after a 2000 years delay
		const long int dislikeAmountWinningWonderInConstruction = 80; // Triggering a victory condition after a 2000 years delay
		const long int dislikeAmountNoWinningAllArtefacts = 20; // Not triggering a victory condition
		const long int dislikeAmountNoWinningWonderBuilt = 30; // Not triggering a victory condition
		const long int dislikeAmountNoWinningWonderInConstruction = 15; // Not triggering a victory condition
		const long int extraValueForCurrentTarget = 3; // (fake) dislike value added to current target
		const long int msAfterWhichCurrentTargetLosesExtraValue = 180 * 1000; // After x milliseconds with the same target player, current target no longer receives "extraValueForCurrentTarget" (more chances to change target)
		// Dislike score "sub" values + parameters
		const long int townSize = 20; // In tiles
		const long int townSizeSquare = townSize * townSize;
		const long int townNeighborhoodSize = 30;
		const long int townNeighborhoodSizeSquare = townNeighborhoodSize * townNeighborhoodSize;
		const long int dislikeSubScoreRandomFactor = 10;
		const long int dislikeSubScoreAttackedMeMoreThanAverage = 5;

	}

	class AIPlayerTargetingInfo {
	public:
		AIPlayerTargetingInfo();

		long int myPlayerId;
		long int lastUpdateGameTime; // milliseconds
		long int nextUpdateGameTime; // milliseconds
		long int lastTargetPlayerChangeGameTime; // milliseconds
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
