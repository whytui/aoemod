#include <assert.h>
#include <set>
#include <list>
#include <AOE_offsets.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_research_def.h>
#include <AOE_struct_tech_def.h>
#include "researches.h"
#include "language.h"


#pragma once

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace STRATEGY {
;


class TTDetailedResearchDef;


// Generic parent to describe Tech tree detailed info for a unit. See child classes for strong typing.
class TTDetailedUnitDef {
public:
	TTDetailedUnitDef() {
		this->unitDefId = -1;
		this->internalName = "";
		this->langName = "";
		this->isAvailableImmediately = false;
		this->isAvailableAfterAnalysis = false;
		this->baseUnitId = -1;
		this->requiredAge = -1;
		this->unitClass = AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAINone;
	}
	long int unitDefId; // This unit's definition ID
	std::string internalName;
	std::string langName;
	bool isAvailableImmediately; // True if unit is available at game start.
	bool isAvailableAfterAnalysis; // Used in analysis phase: set to true when a "researchIdsThatEnableMe" is ready (building can be built at this stage).
	short int requiredAge; // -1 or an "age" research ID
	std::set<long int> researchIdsThatEnableMe; // It is expected to have 0 (if available at start) or 1 value here, no more.
	std::list<long int> possibleUpgradedUnitIDs; // Ordered UnitDefIds of units that are "upgrades" of me. Does NOT include "myself".
	std::list<long int> possibleAncestorUnitIDs; // Ordered UnitDefIds of units that can be upgraded into "me". See "baseUnitId" to find THE root unitDefId. Does NOT include "myself".
	std::set<TTDetailedResearchDef*> affectedByResearches; // Research IDs that affect this unit (add HP, speed, etc, or even indirectly like ballistics for range units...)

	long int baseUnitId; // UnitDefId of the (root) base unitdef I am a descendent of. =this->unitDefId if I have no ancestor.
	AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitClass;

	// True if unitdef is valid and underlying data can be used safely
	virtual bool IsValid() const { return (this->unitDefId >= 0) && (this->GetUnitDef() != NULL); }

	// Safely sets internal name from unit definition.
	void SetNameFromDefinition(STRUCT_UNITDEF_BASE *unitDef) {
		if (unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->ptrUnitName) {
			this->internalName = unitDef->ptrUnitName;
			this->langName = GetLanguageDllText(unitDef->languageDLLID_Name);
		} else { 
			this->internalName = "";
		}
	}

	// Warning: according to child class, it may be trainable or building. See derived methods.
	virtual STRUCT_UNITDEF_TRAINABLE *GetUnitDef() const { return NULL; }

	// Returns true if unit is a hero or scenario unit: heroes, cheat code units, unused units, etc
	// Do not call this before the analysis is complete !
	bool IsHeroOrScenarioUnit() const {
		return (!this->isAvailableAfterAnalysis && !this->isAvailableImmediately);
	}

	// Returns unit's train location. -1 if not set/invalid.
	virtual long int GetTrainLocation() const { return -1; }
	// Returns unit's train button: 1-10 for first page, etc.
	virtual long int GetTrainButton() const { return -1; }
};


class TTDetailedTrainableUnitDef : public TTDetailedUnitDef {
public:
	TTDetailedTrainableUnitDef(STRUCT_UNITDEF_TRAINABLE *unitDef) {
		__super::TTDetailedUnitDef();
		this->unitDef = unitDef;
		this->isSuperUnit = false;
		if (!unitDef || !unitDef->IsCheckSumValid()) { return; }
		// Init for valid case
		this->unitDefId = unitDef->DAT_ID1;
		this->unitDef = unitDef;
		this->SetNameFromDefinition(this->unitDef);
		this->isAvailableImmediately = (unitDef->availableForPlayer != 0);
		this->baseUnitId = this->unitDefId;
		this->unitClass = unitDef->unitAIType;
	}
	STRUCT_UNITDEF_TRAINABLE *unitDef;
	bool isSuperUnit; // True if this is a "super" unit like heavy cat/helepolis/legion/etc: Iron age unit with no more upgrades, and huge cost.

	bool IsValid() const override { return __super::IsValid() && (this->unitDef != NULL); }
	
	STRUCT_UNITDEF_TRAINABLE *GetUnitDef() const override { return this->unitDef; }

	// Returns unit's train location. -1 if not set/invalid.
	long int GetTrainLocation() const override  {
		if (!this->unitDef) { return -1; }
		return this->unitDef->trainLocation;
	}
	// Returns unit's train button: 1-10 for first page, etc.
	long int GetTrainButton() const override {
		if (!this->unitDef) { return -1; }
		return this->unitDef->trainButton;
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
		this->unitClass = unitDef->unitAIType;
	}

	STRUCT_UNITDEF_BUILDING *unitDef;
	short int initiatesResearch; // ID of research that is triggered when "this" building is constructed. Can be -1 (none).
	std::set<STRUCT_UNITDEF_TRAINABLE*> unitsTrainedHere; // List of unit definitions that are trained in this building (excluding heroes)
	std::set<long int> researchesDevelopedHere; // List of researches (IDs) that are developed in this building. Excludes shadow researches.

	bool IsValid() const override { return __super::IsValid() && (this->unitDef != NULL); }

	// Warning: returned object is actually a building
	STRUCT_UNITDEF_TRAINABLE *GetUnitDef() const override { return this->unitDef; }
};


// Represents Tech tree detailed information about a research definition.
class TTDetailedResearchDef {
public:
	TTDetailedResearchDef(long int researchDefId) {
		this->active = false;
		this->researchDefId = researchDefId;
		this->internalName = "";
		this->langName = "";
		this->requiredAge = (AOE_CONST_FUNC::TECHNOLOGIES) - 1;
		this->researchDef = NULL;
		this->techDef = NULL;
		this->hasOptionalRequirements = false;
		this->allRequirementsAreKnown = false;
		this->hasValidEffect = false;
		this->hasNegativeSideEffect = false;
		this->isAiUnsupported = false;
	}

private:
	long int researchDefId; // Id of "this" research definition. Do not modify.
public:
	std::string internalName; // Research internal name, read from research definition.
	std::string langName; // Name from language file
	bool active; // True if initialized and if researchDef is valid.
	bool hasValidEffect; // True if the research has a valid effect. Warning, an "empty" research is still useful to handle dependencies !
	short int requiredAge; // -1 or an "age" research ID
	AOE_STRUCTURES::STRUCT_RESEARCH_DEF *researchDef; // Pointer to ROR research definition.
	AOE_STRUCTURES::STRUCT_TECH_DEF *techDef; // A research can only have 0 (useless!) or 1 technology.
	bool hasOptionalRequirements; // true if minimum required research count is < required researches count. Only AGE research should have this.
	bool allRequirementsAreKnown; // true when allRequirementsExcludingAges is fully filled with required researches (research is considered "done" in tech tree analysis)

	std::set<TTDetailedResearchDef*> directRequirements; // Direct required researches, INCLUDING the research that enables the building that "initiates" me, if any.
	std::set<TTDetailedResearchDef*> allRequirements; // ALL required researches, which can make a lot especially for iron-age researches. Includes researches initiated by buildings (so this gives us required buildings)
	std::set<TTDetailedResearchDef*> allRequirementsExcludingAges; // all required from same age, direct or indirect
	std::set<TTDetailedBuildingDef*> triggeredByBuilding; // Detailed info for all buildings that triggers the research

	std::set<TTDetailedResearchDef*> allChildResearches; // all researches that have a dependency on me (reverse info from allRequirementsExcludingAges)
	std::set<TTDetailedBuildingDef*> enableBuildings; // The buildings that are enabled thanks to "this" research.
	std::set<TTDetailedTrainableUnitDef*> enableTrainables; // The Non-building unitDefs that are enabled thanks to "this" research.
	std::set<std::pair<long int, long int>> upgradedUnitDefId; // Non-building unitDef IDs that are upgraded by this research (pair: first=from, second=to)
	std::set<TTDetailedUnitDef*> affectsUnits; // All units (buildings/trainable) affected by this research

	std::list<TTDetailedResearchDef*> researchLinePredecessors; // All (ordered) researches from same "line" (location+buttonId) and come before me. E.g tool&bronze age if "me"=iron age
	std::list<TTDetailedResearchDef*> researchLineSuccessors; // All (ordered) researches from same "line" (location+buttonId) and come after me. E.g iron age if "me"=bronze age
	
	bool hasNegativeSideEffect; // True for a research that has some negative side effects. E.g. ballista tower upgrade has much slower projectile. Jihad decreases villagers gathering abilities, etc. Warning: includes trireme !
	bool isAiUnsupported; // True for a research that is not supported by AI (no use to AI players), like martyrdom (priest sacrifice), writing (shared exploration).

	// Methods

	// Get "my" research definition ID
	long int GetResearchDefId() const { return this->researchDefId; }
	
	// Returns true if the research is a shadow research = research NOT triggered 'manually' by player
	bool IsShadowResearch() const;

	// Copy all direct/indirect requirements from "other" to this->allRequirementsExcludingAges
	void AddAllRequirementsFrom(TTDetailedResearchDef *other);

	// Returns true if this research is wheel. We try to be as much generic as possible.
	bool IsWheel() const;

	// Returns true if this research is architecture. We try to be as much generic as possible.
	bool IsArchitecture() const;

	// Returns true if the research is a "wood working"-like tech: improves lumberjack carry capacity+work rate + improves range/LOS for almost all range units.
	bool IsWoodWorkingAndRange() const;

	// Returns true if research must be ignored by AI in random map games
	bool CanExcludeInRandomMapAI() const {
		if (this->isAiUnsupported) { return true; }
		if (!this->hasNegativeSideEffect) { return false; }
		// If "this" has negative side effects: return true except for war boats (allow trireme upgrade in all games !)
		for each(auto x in this->affectsUnits) {
			if (x->unitClass == TribeAIGroupWarBoat) {
				return false;
			}
		}
		return true;
	}

	// Returns true if research must be ignored by AI in deathmatch games
	bool CanExcludeInDeathMatchAI() const {
		// In DM, all all techs (even jihad...) except those that AI doesn't/can't use (martyrdom...)
		return (this->isAiUnsupported);
	}

	// Returns true if the research is an age (stone/tool/bronze/iron)
	bool IsAgeResearch() const {
		return (this->researchDefId >= AOE_CONST_FUNC::CST_RSID_STONE_AGE) &&
			(this->researchDefId <= AOE_CONST_FUNC::CST_RSID_IRON_AGE);
	}

};


// Provides a deterministic order for TTDetailedUnitDef elements.
// Warning: requires baseUnitId to be correctly valued
static bool operator< (const TTDetailedUnitDef &left, const TTDetailedUnitDef &right) {
	// 1) Train location
	long int lefttl = left.GetTrainLocation();
	long int righttl = right.GetTrainLocation();
	if (lefttl != righttl) {
		return lefttl < righttl;
	}
	// 2) Train button
	long int leftb = left.GetTrainButton();
	long int rightb = right.GetTrainButton();
	if (leftb != rightb) {
		return leftb < rightb;
	}
	// 3) Same location/button but different base unit id (unexpected case for non-hero units)
	if (left.baseUnitId != right.baseUnitId) {
		return left.baseUnitId < right.baseUnitId;
	}
	// {now we have 2 units of same "unit line" }
	// 4) Required age
	if (left.requiredAge != right.requiredAge) {
		return left.requiredAge < right.requiredAge;
	}
	// 5) Number of ancestors
	int lefta = left.possibleAncestorUnitIDs.size();
	int righta = right.possibleAncestorUnitIDs.size();
	if (lefta != righta) {
		return lefta < righta;
	}
	// 6) ... Unit definition ID. For desperate cases.
	return left.unitDefId < right.unitDefId;
}
static bool operator> (const TTDetailedUnitDef &left, const TTDetailedUnitDef &right) {
	return !(left < right);
}

static bool operator< (const TTDetailedResearchDef &left, const TTDetailedResearchDef &right) {
	// 1) 'Invalid' is 'lower' than anything...
	if (!left.active) { return true; }
	if (!right.active) { return false; }
	// 2) Research location
	if (left.researchDef->researchLocation != right.researchDef->researchLocation) {
		return left.researchDef->researchLocation < right.researchDef->researchLocation;
	}
	// 3) Research button
	if (left.researchDef->buttonId != right.researchDef->buttonId) {
		return left.researchDef->buttonId < right.researchDef->buttonId;
	}
	// 4) Required age
	if (left.requiredAge != right.requiredAge) {
		return left.requiredAge < right.requiredAge;
	}
	// 5) Number of ancestors (both are part of same research line)
	int leftp = left.researchLinePredecessors.size();
	int rightp = right.researchLinePredecessors.size();
	if (leftp != rightp) {
		return leftp < rightp;
	}
	// 6) Desperate case: research definition ID
	return left.GetResearchDefId() < right.GetResearchDefId();
}
static bool operator> (const TTDetailedResearchDef &left, const TTDetailedResearchDef &right) {
	return !(left < right);
}

}
}
