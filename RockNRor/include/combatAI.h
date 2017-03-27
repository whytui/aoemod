#pragma once

#include <assert.h>
#include <AOE_const_game_events.h>
#include <AOE_const_internal.h>
#include <AOE_struct_units.h>
#include "unitDefHandling.h"
#include "RockNRorInfo.h"
#include "AOEPrimitives_UI_gameMain.h"
#include "AOEPrimitives_units.h"
#include "unitHandling.h"


using namespace AOE_STRUCTURES;

namespace AOE_METHODS {

	// Estimates the total time to kill a group's units at target position, considering enemy units known from infAI elem list
	float GetTimeToKillGroupUnitsAtTargetPosition(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *targetUnit);

}

namespace COMBAT {

	namespace COMBAT_CONST {
		const float distanceToConsiderVeryClose = 1.0; // Distance (added to range) under which a target is considered "within range". Please leave this > 0.
		const long int distanceAlwaysTaskIdleMilitaryUnitsOnAttack = 6; // Max distance *idle* nearby units will always be tasked when "I" am attacked
		const float afterShootRetreatRangeForHunterSlowAnimal = 1;
		const float afterShootRetreatRangeForHunterFastAnimal = 3;

		// Speed factor to apply on actor unit speed that runs after a civilian. If fixed speed is slower than villager, let him run (if another target is out there).
		// E.g. 1.1 = 110% (real speed+10%). If actor is more than 10% faster than villager, he will continue to chase the villager. Otherwise, actor may change target.
		const float minimumSpeedProportionToFollowFleeingVillager = 1.1f;
	}

	// Custom treatment to decide if a potential target unit should be ignored
	// Overload standard rules for catapults(ignores building if HP=1) and "target=wall" cases.
	// Default result=false (on error cases)
	bool AutoSearchTargetShouldIgnoreUnit(STRUCT_UNIT_ACTIVITY *activity,
		AOE_STRUCTURES::STRUCT_UNIT_BASE *potentialTargetUnit);


	// Returns true if a unit should change target to new one, false if it should keep attacking current one.
	// This choice is very important, unit must NOT change its mind on each call because it is called too often and unit just attacks none of its "attackers"
	// Which is a common issue in original game (example: 2-priest trick to safely convert an enemy)
	bool ShouldChangeTarget(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity, long int targetUnitId);


	// Returns false if we should prevent unit from moving back (to maxrange) after shooting. Default result=true
	bool ShouldRetreatAfterShooting(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity);


	// Computes the damage dealt by a group on a unit, cf 0x4C62F0.
	float GetGroupDamageOnUnit(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *targetUnit);


	// Handles "retreat after shooting" feature for hunters (non-native feature !)
	bool HunterMoveBackAfterShooting(STRUCT_UNIT_ACTIVITY *unitActivity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notify);


	// Triggered when a unit sees another unit around (cf EVENT_PLAYER_SEE_UNIT), even if actor unit is not idle
	// Remark: feeding infAI list is handled in parent calls.
	// This method is called only if IsImproveAIEnabled(...) is true.
	void OnSeeNearbyUnit(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *actorUnit, STRUCT_UNIT_BASE *seenUnit);

	// Triggered when a priest sees another unit around (cf EVENT_PLAYER_SEE_UNIT), even if actor unit is not idle
	void OnPriestSeeNearbyUnit(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *actorUnit, STRUCT_UNIT_BASE *seenUnit);

}
