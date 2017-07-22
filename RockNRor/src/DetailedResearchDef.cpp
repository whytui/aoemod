#include "../include/DetailedResearchDef.h"

using namespace AOE_STRUCTURES;
using namespace AOE_CONST_FUNC;

namespace ROCKNROR {
namespace STRATEGY {
;


// Returns true if the research is a shadow research = research NOT triggered 'manually' by player
bool DetailedResearchDef::IsShadowResearch() const {
	if (!this->active || !this->researchDef) { return false; }
	return this->researchDef->IsShadowResearch();
}


// Copy all direct/indirect requirements from "other" to this->allRequirementsExcludingAges and this->allRequirements
void DetailedResearchDef::AddAllRequirementsFrom(DetailedResearchDef *other) {
	if (!other) { return; }
	// "all requirements": just copy everything, don't think too much...
	for each (DetailedResearchDef *requirement in other->allRequirements)
	{
		this->allRequirements.insert(requirement);
	}
	for each (DetailedResearchDef *requirement in other->directRequirements)
	{
		this->allRequirements.insert(requirement);
	}

	// allRequirementsExcludingAges: "recursively" ensure at each level not to copy Ages' requirements.
	long int otherResDefId = other->researchDefId;
	bool otherIsAge = (otherResDefId == CST_RSID_STONE_AGE) || (otherResDefId == CST_RSID_TOOL_AGE) ||
		(otherResDefId == CST_RSID_BRONZE_AGE) || (otherResDefId == CST_RSID_IRON_AGE);
	long int maxRequiredAge = CST_RSID_STONE_AGE;
	if (this->requiredAge > maxRequiredAge) { maxRequiredAge = this->requiredAge; }
	for each (DetailedResearchDef *requirement in other->allRequirementsExcludingAges)
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
	for each (DetailedResearchDef *requirement in other->directRequirements)
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


}
}
