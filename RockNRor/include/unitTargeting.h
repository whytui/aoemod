#pragma once

#include <assert.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_global.h>
#include <AOE_struct_player.h>
#include <AOE_struct_tac_ai.h>
#include <AOE_struct_inf_ai.h>
#include <AOE_struct_main_ai.h>
#include "mainStructuresHandling.h"
#include "randomizer.h"
#include "AOE_time.h"
#include "RockNRorCommon.h"
#include "unitDefHandling.h"
#include "AOEPrimitives_units.h"
#include "playerTargeting.h"
#include "AOEPrimitives_lists.h"
#include "AOEPrimitives_unitGroup.h"
#include "AOEPrimitives_units.h"
#include "customAIMilitaryInfo.h"
#include "combatAI.h"
#include "UnitExtensionHandler.h"

using namespace std;
using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	// Configuration for targeting evaluation
	namespace TARGETING_CONST {
		const long int neverRunTargetSelectionProcessBeforeThisDelay = 1000; // Minimum millisecond before a target search can be run again (when there already is a valid target)
		const float evaluationForPriorityTargetWonder = 1000.0f;
		// If current target is further than this (square) distance, force try to find another (better) target.
		const long int squareDistanceAboveWhichRecomputeTargetOnAllArtefacts = 6*6;
		// A delay in milliseconds after which "squareDistanceAboveWhichRecomputeTargetOnAllArtefacts" takes effect
		// Before this delay, current target is preserved even if target player has all relics/ruins
		const long int timeAboveWhichFarTargetFromArtefactsIsRemoved = 5000;
		const long int maxDistToSearchUnitsNearTargetLocation = 5; // Horizontal/vertical dist in "tiles"
	}


	class UnitTargeting {
	public:
		// Reset all underlying info (useful at game start, etc)
		void ResetAllInfo();

		// Initializes internal information for a new search for a given player
		void InitNewSearch(long int playerId);

		// Reset all values. InProgress is set to false (0)
		void ResetTargetInfo(STRUCT_TAC_AI_TARGET_INFO *targetInfo);

		// Returns the enemy wonder with higher attack priority. Does not return wonders from allied/neutral players
		// playerId can be a joker (if -1)
		// Returns NULL if not found
		STRUCT_UNIT_MEMORY *FindWonderToAttackUnitMemoryPtr(STRUCT_INF_AI *infAI, long int playerId);

		// If current target is still a valid target - and its position is known -, return its pointer in InfAI elem list.
		// Returns NULL otherwise
		STRUCT_UNIT_MEMORY *GetUnitMemoryForCurrentTargetIfStillEligible(STRUCT_INF_AI *infAI, long int targetPlayerId,
			STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);


		STRUCT_UNIT_MEMORY *ContinueFindGroupMainTargetInProgress(STRUCT_INF_AI *infAI, long int targetPlayerId,
			STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);

		// (TEST) find a target for an active unit group, called from "task active groups" method.
		// This method is used for "attack" phases, not really for defensive situations.
		STRUCT_UNIT_MEMORY *TestFindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
			STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);
		
	private:
		long int lastChosenTargetUnitId[9];
		long int lastTargetChangeGameTime[9]; // Only updated when target changes
		long int lastTargetSelectionGameTime[9]; // Updated each time a target is selected, even if same as before
		STRUCT_POSITION_INFO priorityLocation[9]; // If other than (-1,-1), determines a location (for each AI player) to attack with high priority (all relics, etc)
		bool groupIsInMyTown[9]; // True to indicate unit group is currently n town (defensive position or idle)

		// Set target in unitGroup, in internal data, in targetInfo.
		void SetTarget(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo,
			STRUCT_UNIT_MEMORY *targetUnitMemory, long int leaderTerrainZoneId, float targetEvaluation);

		// If target player has all relics/ruins, finds the location to attack to capture one.
		bool SetPriorityTargetLocation(STRUCT_INF_AI *infAI, long int targetPlayerId,
			STRUCT_UNIT_BASE *groupLeader, STRUCT_TAC_AI_TARGET_INFO *targetInfo);

		STRUCT_UNIT_MEMORY* FindTargetUnitNearPriorityLocation(STRUCT_INF_AI *infAI, long int targetPlayerId);

	};


	// A unique global object for everyone
	extern UnitTargeting unitTargetingHandler;






	// baseTimeGetTimeValue = TimeGetTime() value (ms) for total AI treatment process time calculation. We must not go over global->tmp_allowedTimeForAITreatment
	// If successful, returns a pointer to the selected STRUCT_INF_AI_UNIT_LIST_ELEM element.
	// If successful, unitGroup targets array contains only 1 target (in most cases... to check in case when units block the way to main target)
	// Returns false if found nothing OR if search could not be completed in time.
	STRUCT_UNIT_MEMORY *FindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
		STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);



	// NOTE: this corresponds to original AOE method, kept for backup/information/debugging. Do not use this method
	// baseTimeGetTimeValue = TimeGetTime() value (ms) for total AI treatment process time calculation. We must not go over global->tmp_allowedTimeForAITreatment
	// If successful, returns a pointer to the selected STRUCT_INF_AI_UNIT_LIST_ELEM element.
	// If successful, unitGroup targets array contains only 1 target (in most cases... to check in case when units block the way to main target)
	// Returns false if found nothing OR if search could not be completed in time.
#ifdef _DEBUG
	STRUCT_UNIT_MEMORY *LEGACY_FindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
		STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);
#endif

}
