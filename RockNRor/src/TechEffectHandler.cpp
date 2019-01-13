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

	// The "apply set effect" method in ROR is completely bugged for attributes ARMOR/ATTACK.
	// Not only it does not work when armor does not already exist in list (same limitation as "apply add effect"),
	// But also it contains a confusion between "armor (or attack) index" and "armor (or attack) class" (this bug is NOT present for "apply add effect")
	// So we completely override this case..
	switch (attribute) {
	case TECH_UNIT_ATTRIBUTES::TUA_ARMOR:
		if (unitDefAttackable) {
			AOE_STRUCTURES::SetArmorInList(unitDefAttackable, STRUCT_TECH_DEF_EFFECT::GetAttackClassFromFloatValue(value),
				STRUCT_TECH_DEF_EFFECT::GetAttackOrArmorValueFromFloatValue(value), true);
		}
		return true; // Do not run ROR original code, we just overrode it.
	case TECH_UNIT_ATTRIBUTES::TUA_ATTACK:
		if (unitDefAttackable) {
			AOE_STRUCTURES::SetAttackInList(unitDefAttackable, STRUCT_TECH_DEF_EFFECT::GetAttackClassFromFloatValue(value),
				STRUCT_TECH_DEF_EFFECT::GetAttackOrArmorValueFromFloatValue(value), true);
		}
		return true; // Do not run ROR original code, we just overrode it.
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


// Handles "Resource Modifier (Multiply)" effect, that is natively UNSUPPORTED by ROR
// Returns true if successful
bool PlayerApplyMultiplyResourceEffect(STRUCT_PLAYER *player, float value, AOE_CONST_FUNC::RESOURCE_TYPES resourceType) {
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid() || !player->ptrResourceValues) { return false; }
	if (resourceType < 0) { return false; }

	float v = player->GetResourceValue(resourceType);
	v *= value;
	player->SetResourceValue(resourceType, v);

	return true;
}


// Apply unsupported technology effects to provided player.
// Standard/supported effects are ignored here. Use other overloads if you need to customize/fix something in "standard" supported effects.
void ApplyUnsupportedEffects(STRUCT_PLAYER *player, STRUCT_TECH_DEF_INFO *techDefInfo, short int techId) {
	assert(player && player->IsCheckSumValid());
	assert(techDefInfo && techDefInfo->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return; }
	if (!techDefInfo || !techDefInfo->IsCheckSumValid()) { return; }
	STRUCT_TECH_DEF *techDef = techDefInfo->GetTechDef(techId);
	if (!techDef) { return; }
	for (int i = 0; i < techDef->effectCount; i++) {
		switch (techDef->ptrEffects[i].effectType) {
		case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_MULT:
		{
			// This effect type is unimplemented in ROR !
			AOE_CONST_FUNC::RESOURCE_TYPES resType = (AOE_CONST_FUNC::RESOURCE_TYPES)techDef->ptrEffects[i].effectUnit;
			ROCKNROR::TECHEFFECT::PlayerApplyMultiplyResourceEffect(player, techDef->ptrEffects[i].GetValue(), resType);
		}
			break;
		case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESEARCH_COST_MODIFIER:
		{
			// This effect type is unimplemented in ROR
			short int researchId = techDef->ptrEffects[i].effectUnit;
			AOE_CONST_FUNC::RESOURCE_TYPES resType = (AOE_CONST_FUNC::RESOURCE_TYPES)techDef->ptrEffects[i].effectClass;
			short int mode = techDef->ptrEffects[i].effectAttribute; // 0=set, 1=add
			float value = techDef->ptrEffects[i].GetValue();
			// WARNING: do not implement this because STRUCT_RESEARCH_DEF is common to all players = the "empires.dat reference".
			// We must not modify it, it would impact all players and even other games !
		}
			break;
		case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESEARCH_TIME_MODIFIER:
		{
			// This effect type is unimplemented in ROR
			short int researchId = techDef->ptrEffects[i].effectUnit;
			// effectClass is unused here
			short int mode = techDef->ptrEffects[i].effectAttribute; // 0=set, 1=add
			float value = techDef->ptrEffects[i].GetValue();
			// WARNING: do not implement this because STRUCT_RESEARCH_DEF is common to all players = the "empires.dat reference".
			// We must not modify it, it would impact all players and even other games !
			/*STRUCT_RESEARCH_DEF *resDef = player->GetResearchDef(researchId);
			if (resDef) {
				if (mode) {
					resDef->researchTime += (short int)value;
				} else {
					resDef->researchTime = (short int)value;
				}
			}*/
		}
			break;
		default:
			break;
		}
	}
}


}
}
