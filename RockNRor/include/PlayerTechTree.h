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



class TTCreatorResearchInfo {
public:
	TTCreatorResearchInfo() {
		this->researchDetail = NULL;
		this->researchId = -1;
		this->disableProbability = -1;
		this->disableWeight = 0;
		this->affinityGroupId = -1;
		this->updatedArmorClass = ATTACK_CLASS::CST_AC_NONE;
		this->updatedAttackClass = ATTACK_CLASS::CST_AC_NONE;
		this->techImpactsReligion = false;
		this->disableScore = 0;
	}
	long int researchId;
	double disableProbability; // 0-1 score. -1 means "not set"
	double disableWeight; // 0-1 score. The weight (impact) if this research gets disabled
	double disableScore; // Used to decide which research to disable in tech tree
	TTDetailedResearchDef *researchDetail;
	long int affinityGroupId;
	long int researchLocation;
	AOE_CONST_FUNC::ATTACK_CLASS updatedArmorClass; // Armor class the tech upgrades. None (-1) in most cases.
	AOE_CONST_FUNC::ATTACK_CLASS updatedAttackClass; // Attack class the tech upgrades. None (-1) in most cases.
	bool techImpactsReligion;
};


static const double RES_PROBA_MIN = 0;
static const double RES_PROBA_MAX = 1;
static const double RES_WEIGHT_MIN = 0;
static const double RES_WEIGHT_MAX = 1;
static const double RES_PROBA_STANDARD_RESEARCH = 0.5; // "disable probability" for a standard research
static const double RES_WEIGHT_STANDARD_RESEARCH = 0.1; // "disable weight" for a standard research
static const double RES_PROBA_WHEEL = 0.05; // "disable probability" for Wheel research
static const double RES_WEIGHT_WHEEL = 0.9; // "disable weight" for Wheel research
static const double RES_PROBA_IMPACT_ON_RANDOM = 0.3; // computed probability counts as much as xxx% vs random.


class TechTreeCreator {
public:
	TechTreeCreator() {
		this->Reset();
	}
	long int arrayUnitDefCount;
	long int arrayResearchCount;
	int religionLevel; // 0=low(high disable proba), 3=high(low disable temple research proba)
	int economyLevel; // 0=low(high disable proba), 3=high(low disable economy techs proba)
	std::set<long int> eligibleDisableResearchIdBronze;
	std::set<long int> eligibleDisableResearchIdIron;
	std::set<long int> eligibleDisableUnitDefIdBronze;
	std::set<long int> eligibleDisableUnitDefIdIron;
	std::list<TTCreatorResearchInfo*> allCreatorResearchInfo;


	void Reset() {
		this->arrayResearchCount = 0;
		this->arrayUnitDefCount = 0;
		this->eligibleDisableResearchIdBronze.clear();
		this->eligibleDisableResearchIdIron.clear();
		this->eligibleDisableUnitDefIdBronze.clear();
		this->eligibleDisableUnitDefIdIron.clear();
		this->economyLevel = 0;
		this->religionLevel = 0;
	}

	// Create a random tech tree (list of effects) on provided TechDef (should be initially empty).
	void CreateRandomTechTree(STRUCT_TECH_DEF *techDef);

private:
	void CollectResearchInfo();

	void CollectUnitInfo();

	// Set initial (base) probabilities for all researches. Does not take care of dependencies/impact on other researches/units.
	void SetResearchBaseProbabilities();
	void UpdateResearchProbabilitiesWithImpacts();

	void CalcResearchesDisableScore();

};




}
}
