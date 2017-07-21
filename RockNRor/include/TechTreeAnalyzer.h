#include <assert.h>
#include <set>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_global.h>
#include <basicFilesHandling.h>
#include "RockNRorConfig.h"
#include "RockNRorInfo.h"
#include "AOE_memory.h"
#include "traceMessage.h"
#include "mainStructuresHandling.h"
#include "techDefHandling.h"
#include "researches.h"

#pragma once

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
			return;
		}
		this->unitDefId = unitDef->DAT_ID1;
		this->unitDef = unitDef;
		if (unitDef->ptrUnitName) { this->internalName = unitDef->ptrUnitName; }
		this->isAvailableImmediately = (unitDef->availableForPlayer != 0);
		this->initiatesResearch = unitDef->initiatesResearch;
	}

	long int unitDefId;
	std::string internalName;
	bool isAvailableImmediately; // True if building is available at game start.
	bool isAvailableInCurrentState; // Reserved for analysis phase: set to true when a "researchIdsThatEnableMe" is ready (building can be built at this stage).
	STRUCT_UNITDEF_BUILDING *unitDef;
	short int initiatesResearch; // ID of research that is triggered when "this" building is constructed. Can be -1 (none).
	std::set<long int> researchIdsThatEnableMe; // It is expected to have 0 (if available at start) or 1 value here, no more.
	std::set<long int> possibleUpgradedUnitIDs; // UnitDefIds of buildings that are "upgrades" of me.
	std::set<long int> possibleAncestorUnitIDs; // UnitDefIds of buildings that can be upgraded into "me".
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
	//std::set<long int> triggeredByBuildingId; // All building unitDefId that triggers the research
	std::set<DetailedBuildingDef*> triggeredByBuilding; // Detailed info for all buildings that triggers the research

	// Methods

	// Get "my" research definition ID
	long int GetResearchDefId() const { return this->researchDefId; }
	// Copy all direct/indirect requirements from "other" to this->allRequirementsExcludingAges
	void AddAllRequirementsFrom(DetailedResearchDef *other);

private:
	long int researchDefId; // Id of "this" research definition. Do not modify.

};



class TechTreeAnalyzer {
public:
	TechTreeAnalyzer() {
		this->researchCount = 0;
		this->unitDefCount = 0;
		this->detailedResearches = NULL;
		this->detailedBuildings = NULL;
		this->analyzeComplete = false;
		this->myLog = "";
		this->myLog.reserve(1000);
	}
	~TechTreeAnalyzer() {
#ifdef _DEBUG
		// Write log to local file
		std::string filename = "D:\\Dév\\CPP\\AOE\\Debug\\TechTreeAnalyze.log";
		if (WriteToFile(this->myLog, filename, false)) {
			traceMessageHandler.WriteMessageNoNotification(std::string("TechTreeAnalyze: find logs in ") + filename);
		}
#endif
		this->FreeArrays();
	}

	std::string myLog;

	// Methods

	// Safely get a DetailedResearchDefobject
	DetailedResearchDef *GetDetailedResearchDef(int researchId) const {
		if ((researchId < 0) || (researchId >= this->researchCount) || (this->detailedResearches == NULL)) { return NULL; }
		return this->detailedResearches[researchId];
	}

	// Safely get a DetailedResearchDefobject
	DetailedBuildingDef *GetDetailedBuildingDef(int unitDefId) const {
		if ((unitDefId < 0) || (unitDefId >= this->unitDefCount) || (this->detailedBuildings == NULL)) { return NULL; }
		return this->detailedBuildings[unitDefId];
	}

	// Adds (if not already there) building unit definition in internal data. Required to use the corresponding DetailedBuildingDef.
	DetailedBuildingDef *InitGetDetailedBuildingDef(STRUCT_UNITDEF_BUILDING *unitDef) {
		if (!unitDef) { return NULL; }
		DetailedBuildingDef *result = this->GetDetailedBuildingDef(unitDef->DAT_ID1);
		if (result) { return result; }
		if ((unitDef->DAT_ID1 < 0) || (unitDef->DAT_ID1 >= this->unitDefCount)) { return NULL; } // Should not happen
		result = new DetailedBuildingDef(unitDef);
		this->detailedBuildings[unitDef->DAT_ID1] = result;
		return result;
	}

	// Analyze tech tree and fill internal data
	bool AnalyzeTechTree();

private:
	DetailedResearchDef **detailedResearches; // Array of all research detailed info, size=researchCount
	DetailedBuildingDef **detailedBuildings; // Array of all research buildings info, size=unitDefCount. May contain (many) NULLs.
	std::set<DetailedResearchDef*> unreachableResearches; // Impossible researched (could not be reached during analysis)
	bool analyzeComplete;
	long int researchCount;
	long int unitDefCount;


	// AllocArray needs researchCount and totalUnitDefCount to be set (>0) to work (needs empires.dat data to be loaded in global struct)
	bool AllocArrays() {
		if (this->detailedResearches) { return false; }
		if ((this->researchCount <= 0) || (this->unitDefCount <= 0)) { return false; }
		this->detailedResearches = (DetailedResearchDef**)malloc(sizeof(DetailedResearchDef*) * this->researchCount);
		for (int i = 0; i < this->researchCount; i++) {
			this->detailedResearches[i] = new DetailedResearchDef(i);
		}
		this->detailedBuildings = (DetailedBuildingDef**)malloc(sizeof(DetailedBuildingDef*) * this->unitDefCount);
		for (int i = 0; i < this->unitDefCount; i++) {
			this->detailedBuildings[i] = NULL;
		}
		return true;
	}
	// Free detailedResearches and detailedBuildings arrays and underlying objects.
	void FreeArrays() {
		if (this->detailedResearches != NULL) {
			for (int i = 0; i < this->researchCount; i++) {
				delete this->detailedResearches[i]; // detailedResearches objects from array ALL always exist
			}
			this->detailedResearches = NULL;
		}
		if (this->detailedBuildings != NULL) {
			for (int i = 0; i < this->unitDefCount; i++) {
				if (this->detailedBuildings[i]) {
					delete this->detailedBuildings[i];
					this->detailedBuildings[i] = NULL;
				}
			}
			this->detailedBuildings = NULL;
		}
	}


	// Fill detailedResearches info from global game structure (from DAT info)
	// Does not evaluate indirect dependencies, etc
	// requiredAge is set only if there is a direct dependency
	bool ReadRawDataFromDat();

	// Update detailedResearches info to find buildings that trigger researches
	// Finds (and create in this->detailedBuildings) all buildings that trigger a research.
	void FindBuildingThatTriggerResearches();

	// Update this->detailedBuildings with researches that enable the buildings (if not available at start)
	void FindResearchesThatEnableKnownBuildings();

	void MarkBuildingAsAvailable(long int unitDefId);

	// Update required researches for researches whose direct dependencies are all satisfied (all required researches are known)
	// Returns the number of researches that are now "satisfied"
	int EvaluateRequiredResearchesSimple();

	void EvaluateRequiredResearches();

	// Fill "unreachableResearches" from detailed research info (requirements must have been computed)
	int CollectUnreachableResearches();

};


}
}
