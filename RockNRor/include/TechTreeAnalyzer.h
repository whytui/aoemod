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
#include "TTDetailedResearchDef.h"

#pragma once

namespace ROCKNROR {
namespace STRATEGY {
;

namespace STRATEGY_CONST {
	const long int MIN_COST_FOR_SUPER_UNIT_UPGRADE = 1000;
}



const int AGES_COUNT = 4;
// Store statistics about tech tree
class TechTreeStatistics {
public:
	TechTreeStatistics() {
		this->Reset();
	}
	int validNonShadowResearchesCount; // Number of non-shadow researches the player can manually develop
	int validRootNonShadowResearchesCount; // Number of "root" researches = number of research series. tool/bronze/iron are 1 series of researches, axeman is 1, short+broad+longsw.+legion is 1 too, etc.
	int inaccessibleResearchesCount; // Number of researches that could not be reached during analysis. Should be 0 if empires.dat is correctly done. Does not include "empty" researches as they have no requirement !

	int allTrainableUnitsCount; // all BUT heroes
	int allBuildingUnitsCount; // all BUT heroes
	int rootTrainableUnitsCount; // excludes heroes
	int rootBuildingUnitsCount; // excludes heroes
	int heroesCount; // all heroes count (including towers)
	int allTrainablesCountByAge[AGES_COUNT]; // age index in 0-3
	int allBuildingsCountByAge[AGES_COUNT]; // age index in 0-3
	int rootTrainablesCountByAge[AGES_COUNT]; // age index in 0-3
	int rootBuildingsCountByAge[AGES_COUNT]; // age index in 0-3
	int villagerCount; // each villager (civilian) unit (builder, farmer...) is a root unit: this has a strong impact on totals !

	void Reset() {
		this->validNonShadowResearchesCount = 0;
		this->validRootNonShadowResearchesCount = 0;
		this->inaccessibleResearchesCount = 0;

		for (int i = 0; i < AGES_COUNT; i++) {
			this->allTrainablesCountByAge[i] = 0;
			this->allBuildingsCountByAge[i] = 0;
			this->rootTrainablesCountByAge[i] = 0;
			this->rootBuildingsCountByAge[i] = 0;
		}
		this->allTrainableUnitsCount = 0;
		this->allBuildingUnitsCount = 0;
		this->rootBuildingUnitsCount = 0;
		this->rootTrainableUnitsCount = 0;
		this->heroesCount = 0;
		this->villagerCount = 0;
	}
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

	TechTreeStatistics statistics;
	std::string myLog;

	// Methods

	// Returns true if analysis has been run and if this object can be used
	bool IsReady() const { return this->analyzeComplete; }

	// Get research count, 0 if not loaded yet
	long int GetResearchCount() const {
		return this->researchCount;
	}
	// Get unitDef count, 0 if not loaded yet
	long int GetUnitDefCount() const {
		return this->unitDefCount;
	}

	// Safely get a DetailedResearchDefobject
	TTDetailedResearchDef *GetDetailedResearchDef(int researchId) const {
		if ((researchId < 0) || (researchId >= this->researchCount) || (this->detailedResearches == NULL)) { return NULL; }
		return this->detailedResearches[researchId];
	}

	// Safely get a DetailedResearchDefobject
	TTDetailedBuildingDef *GetDetailedBuildingDef(int unitDefId) const {
		if ((unitDefId < 0) || (unitDefId >= this->unitDefCount) || (this->detailedBuildings == NULL)) { return NULL; }
		return this->detailedBuildings[unitDefId];
	}

	// Adds (if not already there) building unit definition in internal data. Required to use the corresponding TTDetailedBuildingDef.
	TTDetailedBuildingDef *InitGetDetailedBuildingDef(STRUCT_UNITDEF_BUILDING *unitDef) {
		if (!unitDef) { return NULL; }
		TTDetailedBuildingDef *result = this->GetDetailedBuildingDef(unitDef->DAT_ID1);
		if (result) { return result; }
		if ((unitDef->DAT_ID1 < 0) || (unitDef->DAT_ID1 >= this->unitDefCount)) { return NULL; } // Should not happen
		result = new TTDetailedBuildingDef(unitDef);
		this->detailedBuildings[unitDef->DAT_ID1] = result;
		return result;
	}

	// Safely get a DetailedResearchDefobject
	TTDetailedTrainableUnitDef *GetDetailedTrainableUnitDef(int unitDefId) const {
		if ((unitDefId < 0) || (unitDefId >= this->unitDefCount) || (this->detailedTrainables == NULL)) { return NULL; }
		return this->detailedTrainables[unitDefId];
	}

	// Adds (if not already there) trainable unit definition in internal data. Required to use the corresponding TTDetailedTrainableUnitDef.
	TTDetailedTrainableUnitDef *InitGetDetailedTrainableUnitDef(STRUCT_UNITDEF_TRAINABLE *unitDef) {
		if (!unitDef) { return NULL; }
		TTDetailedTrainableUnitDef *result = this->GetDetailedTrainableUnitDef(unitDef->DAT_ID1);
		if (result) { return result; }
		if ((unitDef->DAT_ID1 < 0) || (unitDef->DAT_ID1 >= this->unitDefCount)) { return NULL; } // Should not happen
		result = new TTDetailedTrainableUnitDef(unitDef);
		this->detailedTrainables[unitDef->DAT_ID1] = result;
		return result;
	}

	// Analyze tech tree and fill internal data
	bool AnalyzeTechTree();

	// Please, please call this if TechDef pointers have changed in global->technologyInfos (when array has been re-allocated)
	void RefreshTechDefPointers();

private:
	TTDetailedResearchDef **detailedResearches; // Array of all research detailed info, size=researchCount
	TTDetailedBuildingDef **detailedBuildings; // Array of all (tech tree) buildings info, size=unitDefCount. May contain (many) NULLs.
	TTDetailedTrainableUnitDef **detailedTrainables; // Array of all (tech tree) trainable units info, size=unitDefCount. May contain (many) NULLs.
	std::set<TTDetailedResearchDef*> unreachableResearches; // Impossible researched (could not be reached during analysis)
	bool analyzeComplete;
	long int researchCount;
	long int unitDefCount;


	// AllocArray needs researchCount and totalUnitDefCount to be set (>0) to work (needs empires.dat data to be loaded in global struct)
	bool AllocArrays() {
		if (this->detailedResearches) { return false; }
		if ((this->researchCount <= 0) || (this->unitDefCount <= 0)) { return false; }
		this->detailedResearches = (TTDetailedResearchDef**)malloc(sizeof(TTDetailedResearchDef*) * this->researchCount);
		for (int i = 0; i < this->researchCount; i++) {
			this->detailedResearches[i] = new TTDetailedResearchDef(i);
		}
		this->detailedBuildings = (TTDetailedBuildingDef**)malloc(sizeof(TTDetailedBuildingDef*) * this->unitDefCount);
		for (int i = 0; i < this->unitDefCount; i++) {
			this->detailedBuildings[i] = NULL;
		}
		this->detailedTrainables = (TTDetailedTrainableUnitDef**)malloc(sizeof(TTDetailedTrainableUnitDef*) * this->unitDefCount);
		for (int i = 0; i < this->unitDefCount; i++) {
			this->detailedTrainables[i] = NULL;
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
		if (this->detailedTrainables != NULL) {
			for (int i = 0; i < this->unitDefCount; i++) {
				if (this->detailedTrainables[i]) {
					delete this->detailedTrainables[i];
					this->detailedTrainables[i] = NULL;
				}
			}
			this->detailedTrainables = NULL;
		}
	}


	// Fill detailedResearches info from global game structure (from DAT info)
	// Does not evaluate indirect dependencies, etc
	// requiredAge is set only if there is a direct dependency
	bool ReadRawDataFromDat();

	// Update detailedResearches info to find buildings that trigger researches
	// Finds (and create in this->detailedBuildings) all buildings that trigger a research.
	// Also feeds this->detailedTrainables
	void FindBuildingThatTriggerResearches();

	// Update this->detailedBuildings with researches that enable the buildings (if not available at start)
	void FindResearchesThatEnableUnits();

	// this->detailedBuildings with units (trainable) that are trained there
	void CollectTrainLocations();

	void MarkBuildingAsAvailable(long int unitDefId);
	void MarkTrainableUnitAsAvailable(long int unitDefId);

	// Update required researches for researches whose direct dependencies are all satisfied (all required researches are known)
	// Returns the number of researches that are now "satisfied"
	int EvaluateRequiredResearchesSimple();

	void EvaluateRequiredResearches();

	// Fill "unreachableResearches" from detailed research info (requirements must have been computed)
	int CollectUnreachableResearches();

	// Update allChildResearches for each research info
	// Also updates "researchLine" predecessors/successors
	void UpdateChildResearchDependencies();

	// Update baseUnitDefId for all known buildings/trainable units to identify root units
	void UpdateUnitsBaseId();

	// Find out which trainable units are "super units" (cataphracts, armored elephants, massive catapults, etc)
	// Priest canNOT be super units here.
	void DetectSuperUnits();

	// Calculate additional statistics, one tech tree has been analysed
	void CalculateStatistics();

};


}
}
