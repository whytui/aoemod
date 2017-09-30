#pragma once

#include <assert.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_global.h>
#include <AOE_struct_player.h>
#include <AOE_struct_diplomacy_ai.h>
#include <AOE_struct_main_ai.h>
#include "mainStructuresHandling.h"
#include "randomizer.h"
#include "RockNRorCommon.h"
#include "unitDefHandling.h"
#include "RockNRorInfo.h"
#include "customAIMilitaryInfo.h"
#include "UnitExtensionHandler.h"

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
		const long int extraValueForCurrentTarget = 20; // (fake) maximum dislike value added to current target. This base value is decreased as time passes since last target player change.
		const long int extraValueForCurrentTargetDecayBy100SecondsPeriod = 10; // (fake) dislike value added to current target that is decreased (decay) during 100 seconds.

		// Dislike score "sub" values + parameters
		const bool useRandomInDislikeSubScore = true;
		const long int dislikeSubScoreRandomFactor = 10;
		const long int dislikeSubScoreAttackedMeMoreThanAverage = 5;
		const long int dislikeSubScoreAttackedMyTown = 12;
		const long int dislikeSubScoreMostSpottedUnitsMyTown = 3;
		const long int dislikeSubScoreHasTowerInMyTown = 20;
		const long int dislikeSubScorePlayerScoreFinalImpact = 50; // 0-100 impact of "player score" on global "dislike score" calculation. Note: attackWinningPlayerFactor percentage is alreday applied before (which reduces the impact)
		const long int dislikeSubScorePriorityRulesFinalImpact = 75; // 0-100 impact of "priority rules" (winning conditions...) on global "dislike score" calculation
		const long int dislikeSubScoreCustomRulesFinalImpact = 30; // 0-100 impact of "custom rules" on global "dislike score" calculation

		// For debugging
		const bool alwaysForceUseStandardRorPlayerTargetSelection = false; // When true, never use RockNRor rules for 'target player' selection
	}

	class AIPlayerTargetingInfo {
	public:
		AIPlayerTargetingInfo();

		long int myPlayerId;
		CustomAIMilitaryInfo *militaryAIInfo; // RockNRor military AI info class for this player
		long int lastUpdateGameTime; // milliseconds since last update of AIPlayer targeting info.
		long int nextUpdateGameTime; // milliseconds
		long int lastTargetPlayerChangeGameTime; // milliseconds
		
		// A Dislike score for each player, taking into account complex rules (players that attacked me, etc) + a random part.
		// Expected values in 0-100.
		long int lastComputedDislikeSubScore[9];

		// Reset all underlying info (useful at game start, etc)
		void ResetInfo();

		void GameStartInit();

		// Recompute Custom information (only) if refresh delay has been reached
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

		// Returns true if we should use ROR's standard treatments for target player selection: e.g. in scenario
		bool ForceUseStandardRorTargetPlayerSelection();

		// Returns the most disliked playerId for TacAI, impacting which player "I" will attack.
		// Uses playerTargetInfo->lastComputedDislikeSubScore[...] values (they are not computed each time)
		// Note: standard ROR code (0x40ACDA) is total crap : applies "score/factor" instead of "score*factor/100", and when attackWinningPlayerFactor=false, the score factor is substracted (instead of ignored)
		long int GetMostDislikedPlayer(STRUCT_PLAYER *player, STRUCT_DIPLOMACY_AI *diplAI,
			long int askTributeAmount, long int askTributePlayerId, bool attackWinningPlayer, long int attackWinningPlayerFactor);

		// Compute AI dislike values, taking care of RockNRor config (dislike human, all relics/ruins, has wonder)
		// In standard game, dislike update is run when a unit is attacked (see 0x4D7B0F), which is disabled by "disable_dislike_human_player" change in AOE_binData.
		void ComputeDislikeValues();


	private:
		AIPlayerTargetingInfo playersAIInfo[9];
	};

	// A unique global object for everyone
	extern PlayerTargeting playerTargetingHandler;
}
