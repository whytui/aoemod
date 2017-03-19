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


	// Notify event (RockNRor) handler for civilians
	// Returns -1 if standard ROR treatments can be continued.
	// Returns >= 0 to return a specific value and bypass ROR treatments.
	long int VillagerActivityNotify(STRUCT_UNIT_ACTIVITY *unitActivity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notify);

	// Triggered when a unit sees another unit around (cf EVENT_PLAYER_SEE_UNIT), even if actor unit is not idle
	void OnSeeNearbyUnit(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *actorUnit, STRUCT_UNIT_BASE *seenUnit);

}
