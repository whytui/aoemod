#include <assert.h>
#include <set>
#include "randomizer.h"
#include "TTDetailedResearchDef.h"
#include "TechTreeAnalyzer.h"
#include "RockNRorInfo.h"

#pragma once

namespace ROCKNROR {
namespace STRATEGY {
;

class CustomPlayerInfo {
public:
	CustomPlayerInfo() {
		this->ResetAndInit(-1, -1, -1);
	}
	~CustomPlayerInfo() {}

	long int myPlayerId;
	long int myCivId;
	long int myTechTreeId;
	bool isActive; // True if player is active/valid for current game
	std::set<ROCKNROR::STRATEGY::TTDetailedBuildingDef*>enabledBuildingsInfo;
	std::set<ROCKNROR::STRATEGY::TTDetailedBuildingDef*>disabledBuildingsInfo; // techtree BuildingDef info for non-available buildings. Most commonly disabled buildings are walls, sometimes temple, academy, ballista tower... (+hero buildings)
	std::set<ROCKNROR::STRATEGY::TTDetailedUnitDef*>enabledTrainableUnitInfos;
	std::set<ROCKNROR::STRATEGY::TTDetailedUnitDef*>disabledTrainableUnitInfos;
	std::set<ROCKNROR::STRATEGY::TTDetailedResearchDef*>enabledResearchesInfoAll; // All non-disabled (by tech tree) researches, including shadow researches
	std::set<ROCKNROR::STRATEGY::TTDetailedResearchDef*>enabledResearchesInfo; // All non-disabled (by tech tree) researches, excluding shadow researches
	std::set<ROCKNROR::STRATEGY::TTDetailedResearchDef*>disabledResearchesInfo;

	// Reset internal data and re-init.
	void ResetAndInit(long int playerId, long int civId, long int techTreeId) {
		this->myCivId = civId;
		this->myTechTreeId = techTreeId;
		this->myPlayerId = playerId;
		this->isActive = false;
		this->enabledBuildingsInfo.clear();
		this->enabledTrainableUnitInfos.clear();
		this->disabledBuildingsInfo.clear();
		this->disabledTrainableUnitInfos.clear();
		this->enabledResearchesInfo.clear();
		this->disabledResearchesInfo.clear();
	}

	// Fill enabledXxx/disabledXxx collections according to player's tech tree (myTechTreeId).
	void CollectInfoFromExistingTechTree();
};



// Create a random tech tree (list of effects) on provided TechDef (should be initially empty).
void CreateRandomTechTree(STRUCT_TECH_DEF *techDef);


}
}
