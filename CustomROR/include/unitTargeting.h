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
#include "crCommon.h"
#include "unitDefHandling.h"
#include "AOEPrimitives_units.h"
#include "playerTargeting.h"
#include "AOEPrimitives_unitGroup.h"
#include "AOEPrimitives_units.h"

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

		// Computes the damage dealt by a group on a unit, cf 0x4C62F0.
		float GetGroupDamageOnUnit(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *targetUnit);

		// Estimates the total time to kill a group's units at target position, considering enemy units known from infAI elem list
		float GetTimeToKillGroupUnitsAtTargetPosition(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *targetUnit);

		// Returns the enemy wonder with higher attack priority. Does not return wonders from allied/neutral players
		// playerId can be a joker (if -1)
		// Returns NULL if not found
		STRUCT_INF_AI_UNIT_LIST_ELEM *FindWonderToAttackInfAIElemPtr(STRUCT_INF_AI *infAI, long int playerId);

		// If current target is still a valid target, return its pointer in InfAI elem list.
		// Returns NULL otherwise
		STRUCT_INF_AI_UNIT_LIST_ELEM *GetInfAIElemForCurrentTargetIfStillEligible(STRUCT_INF_AI *infAI, long int targetPlayerId,
			STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);


		STRUCT_INF_AI_UNIT_LIST_ELEM *ContinueFindGroupMainTargetInProgress(STRUCT_INF_AI *infAI, long int targetPlayerId,
			STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);

		STRUCT_INF_AI_UNIT_LIST_ELEM *TestFindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
			STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);
		
		// Returns true if group has been tasked, and standard treatments must be skipped. Default=false (let standard ROR code be executed)
		// For all non-supported (or without custom treatment) cases, just return false !
		bool TaskActiveUnitGroup(STRUCT_TAC_AI *tacAI, STRUCT_UNIT_GROUP *unitGroup);

	private:
		long int lastChosenTargetUnitId[9];
		long int lastTargetChangeGameTime[9]; // Only updated when target changes
		long int lastTargetSelectionGameTime[9]; // Updated each time a target is selected, even if same as before
		STRUCT_POSITION_INFO priorityLocation[9]; // If other than (-1,-1), determines a location (for each AI player) to attack with high priority (all relics, etc)
		

		// Set target in unitGroup, in internal data, in targetInfo.
		void SetTarget(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo,
			STRUCT_INF_AI_UNIT_LIST_ELEM *targetInfAIElem, long int leaderTerrainZoneId, float targetEvaluation);

		// If target player has all relics/ruins, finds the location to attack to capture one.
		bool SetPriorityTargetLocation(STRUCT_INF_AI *infAI, long int targetPlayerId,
			STRUCT_UNIT_BASE *groupLeader, STRUCT_TAC_AI_TARGET_INFO *targetInfo);

		STRUCT_INF_AI_UNIT_LIST_ELEM* FindTargetUnitNearPriorityLocation(STRUCT_INF_AI *infAI, long int targetPlayerId);

	};


	// A unique global object for everyone
	extern UnitTargeting unitTargetingHandler;






	// baseTimeGetTimeValue = TimeGetTime() value (ms) for total AI treatment process time calculation. We must not go over global->tmp_allowedTimeForAITreatment
	// If successful, returns a pointer to the selected STRUCT_INF_AI_UNIT_LIST_ELEM element.
	// If successful, unitGroup targets array contains only 1 target (in most cases... to check in case when units block the way to main target)
	// Returns false if found nothing OR if search could not be completed in time.
	STRUCT_INF_AI_UNIT_LIST_ELEM *FindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
		STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);



	// NOTE: this corresponds to original AOE method, kept for backup/information/debugging. Do not use this method
	// baseTimeGetTimeValue = TimeGetTime() value (ms) for total AI treatment process time calculation. We must not go over global->tmp_allowedTimeForAITreatment
	// If successful, returns a pointer to the selected STRUCT_INF_AI_UNIT_LIST_ELEM element.
	// If successful, unitGroup targets array contains only 1 target (in most cases... to check in case when units block the way to main target)
	// Returns false if found nothing OR if search could not be completed in time.
#ifdef _DEBUG
	STRUCT_INF_AI_UNIT_LIST_ELEM *LEGACY_FindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
		STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);
#endif

}
