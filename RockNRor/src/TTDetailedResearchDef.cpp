#include "../include/TTDetailedResearchDef.h"

using namespace AOE_STRUCTURES;
using namespace AOE_CONST_FUNC;

namespace ROCKNROR {
namespace STRATEGY {
;


// Returns true if the research is a shadow research = research NOT triggered 'manually' by player
bool TTDetailedResearchDef::IsShadowResearch() const {
	if (!this->active || !this->researchDef) { return false; }
	return this->researchDef->IsShadowResearch();
}


// Copy all direct/indirect requirements from "other" to this->allRequirementsExcludingAges and this->allRequirements
void TTDetailedResearchDef::AddAllRequirementsFrom(TTDetailedResearchDef *other) {
	if (!other) { return; }
	// "all requirements": just copy everything, don't think too much...
	this->allRequirements.insert(other);
	for each (TTDetailedResearchDef *requirement in other->allRequirements)
	{
		this->allRequirements.insert(requirement);
	}
	for each (TTDetailedResearchDef *requirement in other->directRequirements)
	{
		this->allRequirements.insert(requirement);
	}

	// allRequirementsExcludingAges: "recursively" ensure at each level not to copy Ages' requirements.
	long int otherResDefId = other->researchDefId;
	bool otherIsAge = (otherResDefId == CST_RSID_STONE_AGE) || (otherResDefId == CST_RSID_TOOL_AGE) ||
		(otherResDefId == CST_RSID_BRONZE_AGE) || (otherResDefId == CST_RSID_IRON_AGE);
	long int maxRequiredAge = CST_RSID_STONE_AGE;
	if (this->requiredAge > maxRequiredAge) { maxRequiredAge = this->requiredAge; }
	for each (TTDetailedResearchDef *requirement in other->allRequirementsExcludingAges)
	{
		long int resDefId = requirement->researchDefId;
		if ((resDefId > maxRequiredAge) && (resDefId <= CST_RSID_IRON_AGE)) { // Modify this if you want to support a republic age
			maxRequiredAge = resDefId;
		}
		if (requirement->requiredAge > maxRequiredAge) {
			maxRequiredAge = requirement->requiredAge;
		}
		if (!otherIsAge && (resDefId != CST_RSID_STONE_AGE) && (resDefId != CST_RSID_TOOL_AGE) &&
			(resDefId != CST_RSID_BRONZE_AGE) && (resDefId != CST_RSID_IRON_AGE)) {
			this->allRequirementsExcludingAges.insert(requirement);
		}
	}
	for each (TTDetailedResearchDef *requirement in other->directRequirements)
	{
		long int resDefId = requirement->researchDefId;
		if ((resDefId > maxRequiredAge) && (resDefId <= CST_RSID_IRON_AGE)) { // Modify this if you want to support a republic age
			maxRequiredAge = resDefId;
		}
		if (requirement->requiredAge > maxRequiredAge) {
			maxRequiredAge = requirement->requiredAge;
		}
		if (!otherIsAge && (resDefId != CST_RSID_STONE_AGE) && (resDefId != CST_RSID_TOOL_AGE) &&
			(resDefId != CST_RSID_BRONZE_AGE) && (resDefId != CST_RSID_IRON_AGE)) {
			this->allRequirementsExcludingAges.insert(requirement);
		}
	}
	this->requiredAge = (short int)maxRequiredAge;
}


// Returns true if this research is wheel. We try to be as much generic as possible.
bool TTDetailedResearchDef::IsWheel() const {
	if (!this->active) { return false; }
	if (this->requiredAge != AOE_CONST_FUNC::CST_RSID_BRONZE_AGE) { return false; } // should exclude jihad
	if (!AOE_STRUCTURES::RESEARCH::DoesTechAffectCivilianSpeed(this->techDef)) { return false; }
	// Ensure to exclude jihad
	if (!this->techDef) { return false; }
	for (int effectIndex = 0; effectIndex < this->techDef->effectCount; effectIndex++) {
		// Jihad increases villager's attack
		if (this->techDef->ptrEffects[effectIndex].GetAttackOrArmorType() != ATTACK_CLASS::CST_AC_NONE) { return false; }
		// We could also exclude if "decrease carry capacity"
	}
	return true;
}


// Returns true if this research is architecture. We try to be as much generic as possible.
bool TTDetailedResearchDef::IsArchitecture() const {
	if (!this->active) { return false; }
	if (this->requiredAge != AOE_CONST_FUNC::CST_RSID_BRONZE_AGE) { return false; }
	if (!this->techDef) { return false; }
	
	return AOE_STRUCTURES::RESEARCH::HasArchitectureEffects(this->techDef);
}


// Returns true if the research is a "wood working"-like tech: improves lumberjack carry capacity+work rate + improves range/LOS for almost all range units.
bool TTDetailedResearchDef::IsWoodWorkingAndRange() const {
	if (!this->active) { return false; }
	if (!this->techDef) { return false; }
	return AOE_STRUCTURES::RESEARCH::HasWoodWorkingAndImproveRangeEffects(this->techDef);
}


}
}
