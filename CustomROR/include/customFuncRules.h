
#pragma once

#include <string>
#include <assert.h>
#include "traceMessage.h"
#include <AOE_structures.h>
#include <AOE_struct_unit_def.h>

/* This contains various definitions for "custom" (unofficial) game functional rules
*/

using namespace AOE_STRUCTURES;

namespace AOE_CONST_FUNC {

	enum UNIT_PROJECTILE_TYPE {
		UPT_NONE = 0,
		UPT_ARROW, // Tower/archer arrows (most ranged units) and slingers
		UPT_BALLISTA_BOLT, // ballista-like bolts (helepolis, ballista tower, trireme)
		UPT_CATAPULT_STONE,
		UPT_OTHER
	};


	// Returns the projectile type from a projectile unit (proj is NOT the unit that shoots but the projectile itself)
	static UNIT_PROJECTILE_TYPE GetProjectileType(STRUCT_UNITDEF_PROJECTILE *proj) {
		if (!proj) { return UPT_NONE; }
		// We have 2 important criteria at hand: projectile arc, and speed. (+accuracy ?)
		if (proj->speed >= 6) {
			return UNIT_PROJECTILE_TYPE::UPT_ARROW;
		}
		if (proj->speed >= 3) {
			return UNIT_PROJECTILE_TYPE::UPT_BALLISTA_BOLT;
		}
		if ((proj->speed > 0) && (proj->projectileArc > 0.25f)) {
			return UNIT_PROJECTILE_TYPE::UPT_CATAPULT_STONE;
		}
		return UPT_OTHER;
	}


	enum UNIT_SPEED_CATEGORY {
		USC_NONE = 0,
		USC_SLOW, // speed under 1
		USC_NORMAL, // speed from 1 to 1.5
		USC_FAST // spedd above 1.5
	};

	// Get speed in very simple categories
	static UNIT_SPEED_CATEGORY GetUnitSpeedCategory(float speed) {
		if (speed <= 0) { return UNIT_SPEED_CATEGORY::USC_NONE; }
		if (speed <= 1) { return UNIT_SPEED_CATEGORY::USC_SLOW; }
		if (speed < 1.5f) { return UNIT_SPEED_CATEGORY::USC_NORMAL; }
		return UNIT_SPEED_CATEGORY::USC_FAST;
	}
	static inline UNIT_SPEED_CATEGORY GetUnitSpeedCategory(STRUCT_UNITDEF_FLAG *unit) {
		if (!unit) { return UNIT_SPEED_CATEGORY::USC_NONE; }
		return GetUnitSpeedCategory(unit->speed);
	}


	// Returns true if unit is ranged unit (shoots projectiles)
	// Note: priests are not ranged units
	static inline bool IsRangedUnit(STRUCT_UNITDEF_TYPE50 *unit) {
		return (unit && (unit->projectileUnitId >= 0));
	}

	// You can do comparisons on enum values
	enum RESEARCH_COST_CATEGORY {
		RCC_VERY_CHEAP = 0, // <= 120
		RCC_CHEAP, // <= 220
		RCC_MODERATE, // <= 350
		RCC_EXPENSIVE, // <= 1000
		RCC_VERY_EXPENSIVE // > 1000
	};

	static RESEARCH_COST_CATEGORY GetResearchCostCategory(int totalCosts) {
		if (totalCosts <= 120) { return RESEARCH_COST_CATEGORY::RCC_VERY_CHEAP; }
		if (totalCosts <= 220) { return RESEARCH_COST_CATEGORY::RCC_CHEAP; }
		if (totalCosts <= 350) { return RESEARCH_COST_CATEGORY::RCC_MODERATE; }
		if (totalCosts <= 1000) { return RESEARCH_COST_CATEGORY::RCC_EXPENSIVE; }
		return RESEARCH_COST_CATEGORY::RCC_VERY_EXPENSIVE;
	}


	// Returns true if the two classes are equals or very similar
	// Infantry - phalanx - war elephant.
	// Cavalry (includes camel) - chariot
	// All archers together + slinger (?)
	static bool AreSimilarClasses(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2) {
		if (class1 == class2) { return true; }
		int id1 = 0;
		switch (class1) {
			// Archer types
		case AOE_CONST_FUNC::TribeAIGroupArcher:
		case AOE_CONST_FUNC::TribeAIGroupChariotArcher:
		case AOE_CONST_FUNC::TribeAIGroupElephantArcher:
		case AOE_CONST_FUNC::TribeAIGroupHorseArcher:
		case AOE_CONST_FUNC::TribeAIGroupSlinger:
			id1 = 1;
			break;
			// slow/middle-speed infantry
		case AOE_CONST_FUNC::TribeAIGroupFootSoldier:
		case AOE_CONST_FUNC::TribeAIGroupHeavyFootSoldier:
		case AOE_CONST_FUNC::TribeAIGroupPhalanx:
		case AOE_CONST_FUNC::TribeAIGroupElephantRider:
			id1 = 2;
			break;
			// Cavalry (fast melee)
		case AOE_CONST_FUNC::TribeAIGroupMountedSoldier:
		case AOE_CONST_FUNC::TribeAIGroupChariot:
			id1 = 3;
			break;
			// Other types : can't be regouped
		case AOE_CONST_FUNC::TribeAIGroupSiegeWeapon:
		case AOE_CONST_FUNC::TribeAIGroupPriest:
		case AOE_CONST_FUNC::TribeAIGroupWarBoat:
		default:
			return false;
		}
		switch (class2) {
			// Archer types
		case AOE_CONST_FUNC::TribeAIGroupArcher:
		case AOE_CONST_FUNC::TribeAIGroupChariotArcher:
		case AOE_CONST_FUNC::TribeAIGroupElephantArcher:
		case AOE_CONST_FUNC::TribeAIGroupHorseArcher:
			return (id1 == 1);
			break;
			// slow/middle-speed infantry
		case AOE_CONST_FUNC::TribeAIGroupFootSoldier:
		case AOE_CONST_FUNC::TribeAIGroupHeavyFootSoldier:
		case AOE_CONST_FUNC::TribeAIGroupPhalanx:
		case AOE_CONST_FUNC::TribeAIGroupElephantRider:
			return (id1 == 2);
			break;
			// Cavalry (fast melee)
		case AOE_CONST_FUNC::TribeAIGroupMountedSoldier:
		case AOE_CONST_FUNC::TribeAIGroupChariot:
			return (id1 == 3);
			break;
		case AOE_CONST_FUNC::TribeAIGroupSiegeWeapon:
		case AOE_CONST_FUNC::TribeAIGroupPriest:
		case AOE_CONST_FUNC::TribeAIGroupWarBoat:
		case AOE_CONST_FUNC::TribeAIGroupSlinger:
		default:
			return false;
		}
	}

	// Updates provided bools to true/false according to the 2 classes. true=the classes use comme techs for this domain.
	static void ShareCommonTechs(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2, bool &attack, bool &defense, bool &hitPoints) {
		attack = AreSimilarClasses(class1, class2);
		defense = attack;
		hitPoints = false;
		// Handle specific cases
		if ((class1 == AOE_CONST_FUNC::TribeAIGroupElephantRider) || (class2 == AOE_CONST_FUNC::TribeAIGroupElephantRider)) {
			attack = false; // war elephants don't have attack increase from storage pit techs
		}
		bool cavalry1 = (class1 == AOE_CONST_FUNC::TribeAIGroupChariot) ||
			(class1 == AOE_CONST_FUNC::TribeAIGroupChariotArcher) ||
			(class1 == AOE_CONST_FUNC::TribeAIGroupMountedSoldier) ||
			(class1 == AOE_CONST_FUNC::TribeAIGroupHorseArcher);
		bool cavalry2 = (class2 == AOE_CONST_FUNC::TribeAIGroupChariot) ||
			(class2 == AOE_CONST_FUNC::TribeAIGroupChariotArcher) ||
			(class2 == AOE_CONST_FUNC::TribeAIGroupMountedSoldier) ||
			(class2 == AOE_CONST_FUNC::TribeAIGroupHorseArcher);
		if (cavalry1 && cavalry2) {
			hitPoints = true;
		}
	}

}
