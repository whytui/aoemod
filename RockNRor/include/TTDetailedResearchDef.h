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


// Generic parent to describe Tech tree detailed info for a unit. See child classes for strong typing.
class TTDetailedUnitDef {
public:
	TTDetailedUnitDef() {
		this->unitDefId = -1;
		this->internalName = "";
		this->isAvailableImmediately = false;
		this->isAvailableAfterAnalysis = false;
		this->baseUnitId = -1;
		this->requiredAge = -1;
	}
	long int unitDefId;
	std::string internalName;
	bool isAvailableImmediately; // True if building is available at game start.
	bool isAvailableAfterAnalysis; // Used in analysis phase: set to true when a "researchIdsThatEnableMe" is ready (building can be built at this stage).
	short int requiredAge; // -1 or an "age" research ID
	std::set<long int> researchIdsThatEnableMe; // It is expected to have 0 (if available at start) or 1 value here, no more.
	std::set<long int> possibleUpgradedUnitIDs; // UnitDefIds of buildings that are "upgrades" of me.
	std::set<long int> possibleAncestorUnitIDs; // UnitDefIds of buildings that can be upgraded into "me". See "baseUnitId" to find THE root unitDefId.

	long int baseUnitId; // UnitDefId of the (root) base unitdef I am a descendent of. =this->unitDefId if I have no ancestor.

	// True if unitdef is valid and underlying data can be used safely
	virtual bool IsValid() const { return this->unitDefId >= 0; }

	// Safely sets internal name from unit definition.
	void SetNameFromDefinition(STRUCT_UNITDEF_BASE *unitDef) {
		if (unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->ptrUnitName) {
			this->internalName = unitDef->ptrUnitName; 
		} else { 
			this->internalName = "";
		}
	}

	// Returns true if unit is a hero or scenario unit: heroes, cheat code units, unused units, etc
	bool IsHeroOrScenarioUnit() const {
		return (!this->isAvailableAfterAnalysis && !this->isAvailableImmediately);
	}
};


class TTDetailedTrainableUnitDef : public TTDetailedUnitDef {
public:
	TTDetailedTrainableUnitDef(STRUCT_UNITDEF_TRAINABLE *unitDef) {
		__super::TTDetailedUnitDef();
		this->unitDef = unitDef;
		if (!unitDef || !unitDef->IsCheckSumValid()) { return; }
		// Init for valid case
		this->unitDefId = unitDef->DAT_ID1;
		this->unitDef = unitDef;
		this->SetNameFromDefinition(this->unitDef);
		this->isAvailableImmediately = (unitDef->availableForPlayer != 0);
		this->baseUnitId = this->unitDefId;
	}
	STRUCT_UNITDEF_TRAINABLE *unitDef;

	bool IsValid() const override { return __super::IsValid() && (this->unitDef != NULL); }

	long int GetTrainLocation() const {
		if (!this->unitDef) { return -1; }
		return this->unitDef->trainLocation;
	}
};


// Represents Tech tree detailed information about a building definition (unit definition).
class TTDetailedBuildingDef : public TTDetailedUnitDef {
public:
	TTDetailedBuildingDef(STRUCT_UNITDEF_BUILDING *unitDef) {
		__super::TTDetailedUnitDef();
		this->unitDef = unitDef;
		this->initiatesResearch = -1;
		if (!unitDef || !unitDef->IsCheckSumValid()) { return; }
		// Init for valid case
		this->unitDefId = unitDef->DAT_ID1;
		this->unitDef = unitDef;
		this->SetNameFromDefinition(this->unitDef);
		this->isAvailableImmediately = (unitDef->availableForPlayer != 0);
		this->initiatesResearch = unitDef->initiatesResearch;
		this->baseUnitId = this->unitDefId;
	}

	STRUCT_UNITDEF_BUILDING *unitDef;
	short int initiatesResearch; // ID of research that is triggered when "this" building is constructed. Can be -1 (none).

	bool IsValid() const override { return __super::IsValid() && (this->unitDef != NULL); }
};


// Represents Tech tree detailed information about a research definition.
class TTDetailedResearchDef {
public:
	TTDetailedResearchDef(long int researchDefId) {
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

	std::set<TTDetailedResearchDef*> directRequirements; // Direct required researches, INCLUDING the research that enables the building that "initiates" me, if any.
	std::set<TTDetailedResearchDef*> allRequirements; // all required researches, which can make a lot especially for iron-age researches.
	std::set<TTDetailedResearchDef*> allRequirementsExcludingAges; // all required from same age, direct or indirect
	std::set<TTDetailedBuildingDef*> triggeredByBuilding; // Detailed info for all buildings that triggers the research

	std::set<TTDetailedResearchDef*> allChildResearches; // all researches that have a dependency on me (reverse info from allRequirementsExcludingAges)
	std::set<TTDetailedBuildingDef*> enableBuildings; // The buildings that are enabled thanks to "this" research.
	std::set<TTDetailedTrainableUnitDef*> enableTrainables; // The Non-building unitDefs that are enabled thanks to "this" research.
	std::set<std::pair<long int, long int>> upgradedUnitDefId; // Non-building unitDef IDs that are upgraded by this research (pair: first=from, second=to)

	// Methods

	// Get "my" research definition ID
	long int GetResearchDefId() const { return this->researchDefId; }
	
	// Returns true if the research is a shadow research = research NOT triggered 'manually' by player
	bool IsShadowResearch() const;

	// Copy all direct/indirect requirements from "other" to this->allRequirementsExcludingAges
	void AddAllRequirementsFrom(TTDetailedResearchDef *other);

};


}
}
