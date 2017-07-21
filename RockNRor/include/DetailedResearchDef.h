#include <assert.h>
#include <set>
#include <AOE_offsets.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_research_def.h>
#include <AOE_struct_tech_def.h>


#pragma once

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace STRATEGY {
;


// Represents detailed information about a building definition (unit definition), including information related to tech tree.
class DetailedBuildingDef {
public:
	DetailedBuildingDef(STRUCT_UNITDEF_BUILDING *unitDef) {
		this->internalName = "";
		this->isAvailableInCurrentState = false;
		if (unitDef == NULL) {
			this->unitDefId = -1;
			this->unitDef = NULL;
			this->isAvailableImmediately = false;
			this->initiatesResearch = -1;
			this->baseUnitId = -1;
			return;
		}
		this->unitDefId = unitDef->DAT_ID1;
		this->unitDef = unitDef;
		if (unitDef->ptrUnitName) { this->internalName = unitDef->ptrUnitName; }
		this->isAvailableImmediately = (unitDef->availableForPlayer != 0);
		this->initiatesResearch = unitDef->initiatesResearch;
		this->baseUnitId = this->unitDefId;
	}

	long int unitDefId;
	std::string internalName;
	bool isAvailableImmediately; // True if building is available at game start.
	bool isAvailableInCurrentState; // Reserved for analysis phase: set to true when a "researchIdsThatEnableMe" is ready (building can be built at this stage).
	STRUCT_UNITDEF_BUILDING *unitDef;
	short int initiatesResearch; // ID of research that is triggered when "this" building is constructed. Can be -1 (none).
	std::set<long int> researchIdsThatEnableMe; // It is expected to have 0 (if available at start) or 1 value here, no more.
	std::set<long int> possibleUpgradedUnitIDs; // UnitDefIds of buildings that are "upgrades" of me.
	std::set<long int> possibleAncestorUnitIDs; // UnitDefIds of buildings that can be upgraded into "me". See "baseUnitId" to find THE root unitDefId.

	long int baseUnitId; // UnitDefId of the (root) base unitdef I am a descendent of. =this->unitDefId if I have no ancestor.
};


// Represents detailed information about a research definition, including information related to tech tree.
class DetailedResearchDef {
public:
	DetailedResearchDef(long int researchDefId) {
		this->active = false;
		this->researchDefId = researchDefId;
		this->internalName = "";
		this->requiredAge = (AOE_CONST_FUNC::TECHNOLOGIES) - 1;
		this->researchDef = NULL;
		this->techDef = NULL;
		this->hasOptionalRequirements = false;
		this->allRequirementsAreKnown = false;
		this->hasValidEffect = false;
	}

private:
	long int researchDefId; // Id of "this" research definition. Do not modify.
public:
	std::string internalName; // Research internal name, read from research definition.
	bool active; // True if initialized and if researchDef is valid.
	bool hasValidEffect; // True if the research has a valid effect. Warning, an "empty" research is still useful to handle dependencies !
	short int requiredAge; // -1 or an "age" research ID
	AOE_STRUCTURES::STRUCT_RESEARCH_DEF *researchDef; // Pointer to ROR research definition.
	AOE_STRUCTURES::STRUCT_TECH_DEF *techDef; // A research can only have 0 (useless!) or 1 technology.
	bool hasOptionalRequirements; // true if minimum required research count is < required researches count. Only AGE research should have this.
	bool allRequirementsAreKnown; // true when allRequirementsExcludingAges is fully filled with required researches (research is considered "done" in tech tree analysis)

	std::set<DetailedResearchDef*> directRequirements; // Direct required researches, INCLUDING the research that enables the building that "initiates" me, if any.
	std::set<DetailedResearchDef*> allRequirements; // all required researches, which can make a lot especially for iron-age researches.
	std::set<DetailedResearchDef*> allRequirementsExcludingAges; // all required from same age, direct or indirect
	std::set<DetailedBuildingDef*> triggeredByBuilding; // Detailed info for all buildings that triggers the research

	std::set<DetailedResearchDef*> allChildResearches; // all researches that have a dependency on me (reverse info from allRequirementsExcludingAges)
	std::set<DetailedBuildingDef*> enableBuildings; // The buildings that are enabled thanks to "this" research.
	std::set<long int> enableUnitDefId; // Non-building unitDef IDs that are enabled by this research
	std::set<std::pair<long int, long int>> upgradedUnitDefId; // Non-building unitDef IDs that are upgraded by this research (pair: first=from, second=to)

	// Methods

	// Get "my" research definition ID
	long int GetResearchDefId() const { return this->researchDefId; }
	// Copy all direct/indirect requirements from "other" to this->allRequirementsExcludingAges
	void AddAllRequirementsFrom(DetailedResearchDef *other);

};


}
}
