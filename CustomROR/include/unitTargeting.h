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

using namespace std;
using namespace AOE_STRUCTURES;

namespace CUSTOMROR {

	// Configuration for targeting evaluation
	namespace TARGETING_CONST {

	}


	STRUCT_INF_AI_UNIT_LIST_ELEM *TestFindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
		STRUCT_UNIT_GROUP_ELEM *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);


	STRUCT_INF_AI_UNIT_LIST_ELEM *ContinueFindGroupMainTargetInProgress(STRUCT_INF_AI *infAI, long int targetPlayerId,
		STRUCT_UNIT_GROUP_ELEM *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);

	// If current target is still a valid target, return its pointer in InfAI elem list.
	// Returns NULL otherwise
	STRUCT_INF_AI_UNIT_LIST_ELEM *GetInfAIElemForCurrentTargetIfStillEligible(STRUCT_INF_AI *infAI, long int targetPlayerId,
		STRUCT_UNIT_GROUP_ELEM *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);


	// baseTimeGetTimeValue = TimeGetTime() value (ms) for total AI treatment process time calculation. We must not go over global->tmp_allowedTimeForAITreatment
	// If successful, returns a pointer to the selected STRUCT_INF_AI_UNIT_LIST_ELEM element.
	// If successful, unitGroup targets array contains only 1 target (in most cases... to check in case when units block the way to main target)
	// Returns false if found nothing OR if search could not be completed in time.
	STRUCT_INF_AI_UNIT_LIST_ELEM *FindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
		STRUCT_UNIT_GROUP_ELEM *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);


	// Returns the enemy wonder with higher attack priority. Does not return wonders from allied/neutral players
	// playerId can be a joker (if -1)
	// Returns NULL if not found
	STRUCT_INF_AI_UNIT_LIST_ELEM *FindWonderToAttackInfAIElemPtr(STRUCT_INF_AI *infAI, long int playerId);

	// Computes the damage dealt by a group on a unit, cf 0x4C62F0.
	float GetGroupDamageOnUnit(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP_ELEM *unitGroup, STRUCT_UNIT_BASE *targetUnit);

	// Estimates the total time to kill a group's units at target position, considering enemy units known from infAI elem list
	float GetTimeToKillGroupUnitsAtTargetPosition(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP_ELEM *unitGroup, STRUCT_UNIT_BASE *targetUnit);


	// NOTE: this corresponds to original AOE method, kept for backup/information/debugging. Do not use this method
	// baseTimeGetTimeValue = TimeGetTime() value (ms) for total AI treatment process time calculation. We must not go over global->tmp_allowedTimeForAITreatment
	// If successful, returns a pointer to the selected STRUCT_INF_AI_UNIT_LIST_ELEM element.
	// If successful, unitGroup targets array contains only 1 target (in most cases... to check in case when units block the way to main target)
	// Returns false if found nothing OR if search could not be completed in time.
#ifdef _DEBUG
	STRUCT_INF_AI_UNIT_LIST_ELEM *LEGACY_FindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
		STRUCT_UNIT_GROUP_ELEM *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue);
#endif

}
