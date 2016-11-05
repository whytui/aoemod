#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include "unitDefHandling.h"
#include "CustomRORInfo.h"
#include "AOEPrimitives_units.h"


using namespace AOE_STRUCTURES;

namespace COMBAT {

	namespace COMBAT_CONST {
		const float distanceToConsiderVeryClose = 1.0; // Distance (added to range) under which a target is considered "within range". Please leave this > 0.
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

}
