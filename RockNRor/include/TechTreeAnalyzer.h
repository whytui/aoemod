#include <assert.h>
#include <set>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_global.h>
#include <basicFilesHandling.h>
#include "AOE_memory.h"
#include "traceMessage.h"
#include "mainStructuresHandling.h"
#include "techDefHandling.h"
#include "researches.h"
#include "DetailedResearchDef.h"

#pragma once

namespace ROCKNROR {
namespace STRATEGY {
;


class TechTreeAnalyzer {
public:
	TechTreeAnalyzer() {
		this->researchCount = 0;
		this->unitDefCount = 0;
		this->detailedResearches = NULL;
		this->detailedBuildings = NULL;
		this->analyzeComplete = false;
		this->myLog = "";
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

	// Returns true if analysis has been run and if this object can be used
	bool IsReady() const { return this->analyzeComplete; }

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

	// Update allChildResearches for each research info
	void UpdateChildResearchDependencies();

	// Update baseUnitDefId for all known buildings to identify root buildings
	void UpdateBuildingsBaseId();

};


}
}
