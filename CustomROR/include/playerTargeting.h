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
#include "unitDefHandling.h"
#include "CustomRORInfo.h"
#include "customAIMilitaryInfo.h"

using namespace std;
using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	// Configuration for targeting evaluation
	namespace TARGETING_CONST {
		const long int yearsCountInArtefactsVictoryDelay = 2000; // And wonder delay too
		const long int updateDetailedDislikeInfoMinDelay = 10; // Minimum delay between full recomputing of custom dislike information. In seconds.
		const long int updateDetailedDislikeInfoMaxDelay = 40; // Maximum delay between full recomputing of custom dislike information. In seconds.
		const long int persistenceDelayOfIndividualEnemyAttacksInTargetingHistory = 60; // Delay (seconds) after which enemy attacks are no longer taken into account in targeting
		const long int persistenceDelayOfIndividualEnemyAttacksInTargetingHistoryRandomImpact = 10; // Maximum random time in seconds added/subtracted from persistenceDelayOfIndividualEnemyAttacksInTargetingHistory
		// Dislike values
		const long int dislikeAmountWinningAllArtefacts = 100; // Triggering a victory condition after a 2000 years delay
		const long int dislikeAmountWinningWonderBuilt = 100; // Triggering a victory condition after a 2000 years delay
		const long int dislikeAmountWinningWonderInConstruction = 80; // Triggering a victory condition after a 2000 years delay
		const long int dislikeAmountNoWinningAllArtefacts = 20; // Not triggering a victory condition
		const long int dislikeAmountNoWinningWonderBuilt = 30; // Not triggering a victory condition
		const long int dislikeAmountNoWinningWonderInConstruction = 15; // Not triggering a victory condition
		const long int extraValueForCurrentTarget = 16; // (fake) maximum dislike value added to current target. This base value is decreased as time passes since last target player change.
		const long int extraValueForCurrentTargetDecayBy100SecondsPeriod = 10; // (fake) dislike value added to current target that is decreased (decay) during 100 seconds.

		// Dislike score "sub" values + parameters
		const long int dislikeSubScoreRandomFactor = 10;
		const long int dislikeSubScoreAttackedMeMoreThanAverage = 5;
		const long int dislikeSubScoreAttackedMyTown = 12;
		const long int dislikeSubScoreMostSpottedUnitsMyTown = 3;
		const long int dislikeSubScoreHasTowerInMyTown = 20;

	}

	class AIPlayerTargetingInfo {
	public:
		AIPlayerTargetingInfo();

		long int myPlayerId;
		CustomAIMilitaryInfo *militaryAIInfo; // CustomROR military AI info class for this player
		long int lastUpdateGameTime; // milliseconds since last update of AIPlayer targeting info.
		long int nextUpdateGameTime; // milliseconds
		long int lastTargetPlayerChangeGameTime; // milliseconds
		long int lastComputedDislikeSubScore[9];

		// Reset all underlying info (useful at game start, etc)
		void ResetInfo();

		void GameStartInit();

		// Recompute information (only) if refresh delay has been reached
		// Returns true if information have been recomputed (false is not necessarily an error)
		bool RecomputeInfo(STRUCT_PLAYER *player);

		// Returns the current target player ID from TacAI information. Returns -1 if invalid/not set.
		long int GetCurrentTacAITargetPlayerId(STRUCT_PLAYER *player);
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

		// Compute AI dislike values, taking care of customROR config (dislike human, all ruins, etc)
		void ComputeDislikeValues();


	private:
		AIPlayerTargetingInfo playersAIInfo[9];
	};

	// A unique global object for everyone
	extern PlayerTargeting playerTargetingHandler;
}
