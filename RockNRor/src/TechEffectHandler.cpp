#include "../include/TechEffectHandler.h"

// *** IMPORTANT REMARK ***
// See header file for detailed information about technology effects handling in ROR and how to overload it.


using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace TECHEFFECT {
;

// Overload unitDef.applyAddEffect(...)
// Returns true if the original ROR code must NOT be executed
// Returns false (default) if original ROR code must be executed after this method.
bool UnitDefApplyAddEffect(STRUCT_UNITDEF_BASE *unitDef, float value, AOE_CONST_FUNC::TECH_UNIT_ATTRIBUTES attribute) {
	assert(unitDef && unitDef->IsCheckSumValidForAUnitClass());

	STRUCT_UNITDEF_ATTACKABLE *unitDefAttackable = nullptr;
	if (unitDef->DerivesFromAttackable()) {
		unitDefAttackable = (STRUCT_UNITDEF_ATTACKABLE *)unitDef;
	}


	// There is a limitation in tech effects in ROR for armors/attack updates:
	// If updating armor/attack value for an armor/attack that does not exist yet, then it does nothing
	// We fix this by adding the armor/attack in the list with default value (0)
	// This limitation concerns for add/set/multiply effects
	// Note: we only fix for add/set (multiply N/A or 0 has no effect, so it's useless)
	switch (attribute) {
	case TECH_UNIT_ATTRIBUTES::TUA_ARMOR:
		if (unitDefAttackable) {
			AOE_STRUCTURES::AddArmorIfMissing(unitDefAttackable, STRUCT_TECH_DEF_EFFECT::GetAttackClassFromFloatValue(value), 0);
		}
		break;
	case TECH_UNIT_ATTRIBUTES::TUA_ATTACK:
		if (unitDefAttackable) {
			AOE_STRUCTURES::AddAttackIfMissing(unitDefAttackable, STRUCT_TECH_DEF_EFFECT::GetAttackClassFromFloatValue(value), 0);
		}
		break;
	}

	return false;
}


// Overload unitDef.applySetEffect(...)
// Returns true if the original ROR code must NOT be executed
// Returns false (default) if original ROR code must be executed after this method.
bool UnitDefApplySetEffect(STRUCT_UNITDEF_BASE *unitDef, float value, AOE_CONST_FUNC::TECH_UNIT_ATTRIBUTES attribute) {
	STRUCT_UNITDEF_ATTACKABLE *unitDefAttackable = nullptr;
	if (unitDef->DerivesFromAttackable()) {
		unitDefAttackable = (STRUCT_UNITDEF_ATTACKABLE *)unitDef;
	}
	// There is a limitation in tech effects in ROR for armors/attack updates:
	// If updating armor/attack value for an armor/attack that does not exist yet, then it does nothing
	// We fix this by adding the armor/attack in the list with default value (0)
	// This limitation concerns for add/set/multiply effects
	// Note: we only fix for add/set (multiply N/A or 0 has no effect, so it's useless)
	switch (attribute) {
	case TECH_UNIT_ATTRIBUTES::TUA_ARMOR:
		if (unitDefAttackable) {
			AOE_STRUCTURES::AddArmorIfMissing(unitDefAttackable, STRUCT_TECH_DEF_EFFECT::GetAttackClassFromFloatValue(value), 0);
		}
		break;
	case TECH_UNIT_ATTRIBUTES::TUA_ATTACK:
		if (unitDefAttackable) {
			AOE_STRUCTURES::AddAttackIfMissing(unitDefAttackable, STRUCT_TECH_DEF_EFFECT::GetAttackClassFromFloatValue(value), 0);
		}
		break;
	}

	return false;
}


// Overload unitDef.applyMultEffect(...)
// Returns true if the original ROR code must NOT be executed
// Returns false (default) if original ROR code must be executed after this method.
bool UnitDefApplyMultiplyEffect(STRUCT_UNITDEF_BASE *unitDef, float value, AOE_CONST_FUNC::TECH_UNIT_ATTRIBUTES attribute) {
	// Unused at this point (no specific treatments)
	return false;
}



}
}
