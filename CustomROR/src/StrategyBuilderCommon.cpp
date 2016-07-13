#include "../include/StrategyBuilderCommon.h"

using namespace STRATEGY;


// Returns true if the two classes are equals or very similar
// Infantry - phalanx - war elephant.
// Cavalry (includes camel) - chariot
// All archers together + slinger (?)
bool STRATEGY::AreSimilarClasses(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2) {
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
void STRATEGY::ShareCommonTechs(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2, bool &attack, bool &defense, bool &hitPoints) {
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

