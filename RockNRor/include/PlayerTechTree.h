#include <assert.h>
#include <set>
#include <AOE_empires_dat_concept_names.h>
#include "randomizer.h"
#include "TTDetailedResearchDef.h"
#include "TechTreeAnalyzer.h"
#include "RockNRorInfo.h"

#pragma once

namespace ROCKNROR {
namespace STRATEGY {
;


enum TTCreatorBaseUnitDisableBehavior {
	TTBUDisableNever = 0, // Never disable root unit of a given unit line
	TTBUDisableVeryRare, // Almost never disable root unit of a given unit line
	TTBUDisableNormal, // Root unit of unit line might be disabled just like any other one
	TTBUDisableMostlyExclusiveSameClassIron // In most cases, only 1 *iron age* root unit of same class is left available. E.g. fireGalley vs catTrireme
};


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

	// Fill eligibleDisableXxx collections according to player's tech tree (myTechTreeId).
	void CollectInfoFromExistingTechTree();
};


// Research-information for tech tree generator
class TTCreatorResearchInfo {
public:
	TTCreatorResearchInfo() {
		this->researchDetail = NULL;
		this->researchId = -1;
		this->disableProbability = -1;
		this->disableWeight = 0;
		this->updatedArmorClass = ATTACK_CLASS::CST_AC_NONE;
		this->updatedAttackClass = ATTACK_CLASS::CST_AC_NONE;
		this->techImpactsReligion = false;
		this->disableScore = 0;
		this->hasBeenDisabled = false;
		this->isFakeForDisableUnit = false;
		this->internalName = "";
		this->langName = "";
	}
	long int researchId;
	std::string internalName;
	std::string langName;
	double disableProbability; // 0-1 score. -1 means "not set"
	double disableWeight; // 0-1 score. The weight (impact) if this research gets disabled
	double disableScore; // Used to decide which research to disable in tech tree
	TTDetailedResearchDef *researchDetail;
	long int researchLocation;
	AOE_CONST_FUNC::ATTACK_CLASS updatedArmorClass; // Armor class the tech upgrades. None (-1) in most cases.
	AOE_CONST_FUNC::ATTACK_CLASS updatedAttackClass; // Attack class the tech upgrades. None (-1) in most cases.
	bool techImpactsReligion;
	bool hasBeenDisabled;
	bool isFakeForDisableUnit;
};


// Unit-information for tech tree generator
class TTCreatorUnitInfo {
public:
	TTCreatorUnitInfo() {
		this->unitDefId = -1;
		this->unitDetail = NULL;
		this->disableProbability = -1;
		this->disableWeight = 0;
		this->disableScore = 0;
		this->trainLocation = -1;
		this->sourceResearchId = -1;
		this->hasBeenDisabled = false;
		this->hasBeenDisabledDirectly = false;
		this->hasADisabledChild = false;
		this->internalName = "";
		this->langName = "";
		this->rootUnitDisablePolicy = TTCreatorBaseUnitDisableBehavior::TTBUDisableNormal;
	}
	long int unitDefId;
	std::string internalName;
	std::string langName;
	double disableProbability; // 0-1 score. -1 means "not set"
	double disableWeight; // 0-1 score. The weight (impact) if this research gets disabled
	double disableScore; // Used to decide which research to disable in tech tree
	TTDetailedUnitDef *unitDetail;
	long int trainLocation;
	long int sourceResearchId; // ID of the research that enables me, if it exists and is unique
	bool hasBeenDisabled;
	bool hasBeenDisabledDirectly; // True when the unit was disabled by direct choice (not just a consequence of another research being disabled)
	bool hasADisabledChild; // True when a child unit (an upgrade) was disabled by direct choice. WARNING: this is only set for root units.
	TTCreatorBaseUnitDisableBehavior rootUnitDisablePolicy;
};


// Configuration for tech tree generation
namespace TT_CONFIG {
	static const double RES_PROBA_MIN = 0;
	static const double RES_PROBA_MAX = 1;
	static const double RES_WEIGHT_MIN = 0;
	static const double RES_WEIGHT_MAX = 1;
	static const double RES_PROBA_STANDARD_RESEARCH = 0.5; // "disable probability" for a standard research
	static const double RES_WEIGHT_STANDARD_RESEARCH = 0.1; // "disable weight" for a standard research
	static const double RES_PROBA_SPECIALIZED_RESEARCH = 0.6; // "disable probability" for "specialized" research: a bit more rare than standard. E.g. shields.
	static const double RES_PROBA_COMMON_USEFUL_RESEARCH = 0.35; // "disable probability" for a very common/useful research like architecture
	static const double RES_WEIGHT_COMMON_USEFUL_RESEARCH = 0.75; // "disable weight" for a very common/useful research like architecture
	static const double RES_PROBA_WHEEL = 0.05; // "disable probability" for Wheel research
	static const double RES_WEIGHT_WHEEL = 0.9; // "disable weight" for Wheel research
	static const double RES_PROBA_STANDARD_UNIT = 0.5; // "disable probability" for a standard unit
	static const double RES_WEIGHT_STANDARD_UNIT = 0.1; // "disable weight" for a standard unit
	static const double RES_PROBA_SPECIALIZED_UNIT = 0.6; // "disable probability" for a unit which is slighly more rare than standard, like ballista (quite often disabled).
	static const double RES_PROBA_VERY_RARE = 0.05; // Disable probability for unit that must be disabled on very rare occasions: priests, stone thrower, hoplite...
	static const double RES_WEIGHT_HIGH_IMPACT_UNIT = 0.8; // "Disable weight" for unit whose absence has a strong impact on civilization: priest, stone thrower

	static const double RES_PROBA_IMPACT_ON_RANDOM = 0.4; // computed probability counts as much as xxx% vs random.
	static const int CALC_MIN_DISABLE_UNITLINE_COUNT_PERCENT = 40;
	static const int CALC_MAX_DISABLE_UNITLINE_COUNT_PERCENT = 70;
	static const int CALC_MIN_DISABLE_RESEARCH_COUNT_PERCENT = 20;
	static const int CALC_MAX_DISABLE_RESEARCH_COUNT_PERCENT = 45;
	static int MIN_DISABLE_UNITLINE_COUNT = 6; // default value, recomputed according to CALC_*
	static int MAX_DISABLE_UNITLINE_COUNT = 12; // default value, recomputed according to CALC_*
	static int MIN_DISABLE_RESEARCH_COUNT = 6; // default value, recomputed according to CALC_*
	static int MAX_DISABLE_RESEARCH_COUNT = 13; // default value, recomputed according to CALC_*
}


// Class that allows creating dynamic tech tree, based on tech tree analyzer
class TechTreeCreator {
public:
	TechTreeCreator() {
		this->Reset();
	}
	long int arrayUnitDefCount;
	long int arrayResearchCount;
	STRUCT_TECH_DEF *techDefForTechTree;
	int religionLevel; // 0=low(high disable proba), 3=high(low disable temple research proba)
	int economyLevel; // 0=low(high disable proba), 3=high(low disable economy techs proba)
	double totalDisableWeight; // Evaluated "weight" of disabled units/researches
	double relativeDisableWeight; // If >0 then civ is unlucky regarding disabled techs/units and better civ bonus would be welcome
	double computedMeanWeight;
	std::set<long int> eligibleDisableResearchIdBronze;
	std::set<long int> eligibleDisableResearchIdIron;
	std::set<long int> eligibleDisableUnitDefIdBronze;
	std::set<long int> eligibleDisableUnitDefIdIron;
	std::list<TTCreatorResearchInfo*> allCreatorResearchInfo;
	std::list<TTCreatorUnitInfo*> allCreatorUnitInfo;
	std::list<STRUCT_TECH_DEF_EFFECT> techTreeEffects;
	std::set<GLOBAL_UNIT_AI_TYPES> classesWithBonus; // Unit classes that benefit from a civ bonus

	void Reset() {
		this->techDefForTechTree = NULL;
		this->techTreeEffects.clear();
		this->arrayResearchCount = 0;
		this->arrayUnitDefCount = 0;
		this->eligibleDisableResearchIdBronze.clear();
		this->eligibleDisableResearchIdIron.clear();
		this->eligibleDisableUnitDefIdBronze.clear();
		this->eligibleDisableUnitDefIdIron.clear();
		this->allCreatorResearchInfo.clear();
		this->allCreatorUnitInfo.clear();
		this->economyLevel = 0;
		this->religionLevel = 0;
		this->totalDisableWeight = 0;
		this->computedMeanWeight = 0;
		this->bonusText = "";
		this->classesWithBonus.clear();
	}

	// Create a random tech tree (list of effects) on provided TechDef (should be initially empty).
	void CreateRandomTechTree(STRUCT_TECH_DEF *techDef);

	std::string GetDisabledUnitsText() const;

	std::string GetDisabledResearchesText() const;

	std::string GetCivBonusText() const;

private:
	std::string bonusText;

	// Get a TTCreatorResearchInfo from this->allCreatorResearchInfo for given ID. NULL if not found
	TTCreatorResearchInfo *GetCrResearchInfo(long int researchId) const;
	// Get a TTCreatorUnitInfo from this->allCreatorUnitInfo for given ID. NULL if not found
	TTCreatorUnitInfo *GetCrUnitInfo(long int unitDefId) const;

	void SetConfigFromStatistics();

	void CollectResearchInfo();

	void CollectUnitInfo();

	// Set initial (base) probabilities for all researches. Does not take care of dependencies/impact on other researches/units.
	// Creates the TTCreatorResearchInfo objects into allCreatorResearchInfo list.
	void SetResearchBaseProbabilities();
	// Set initial (base) probabilities for all units. Does not take care of dependencies/impact on other researches/units.
	// Creates the TTCreatorUnitInfo objects into allCreatorUnitInfo list.
	void SetUnitBaseProbabilities();

	void UpdateResearchProbabilitiesWithImpacts();

	// Use each research's probability to compute its "disable score"
	void CalcResearchesDisableScore();
	// Use each *root* unit's probability to compute its "disable score".
	void CalcUnitsDisableScore();
	// Creates disable research effects according to disable scores that have been computed (on all researches)
	void CreateDisableResearchesEffects();
	// Creates disable unit effects according to disable scores that have been computed (on root units)
	// If a unit(line) is selected for being disabled, then this method chooses at which level (which upgrade) the unitline becomes unavailable.
	void CreateDisableUnitsEffects();

	// Creates random civ bonuses
	void CalcRandomCivBonus();

	// Create one civ bonus
	double CreateOneBonus();

	// Choose one of the upgrade of rootUnitInfo (or rootUnitInfo) that will be the first unit from the lineage to be unavailable.
	TTCreatorUnitInfo *PickUnitUpgradeToDisableInUnitLine(TTCreatorUnitInfo *rootUnitInfo);

	// Disable (cascade) child researches for parentResearch
	// Returns the *child* disabled researches (not the parent research itself)
	std::list<TTCreatorResearchInfo*> CreateDisableEffectOnResearch(TTCreatorResearchInfo *resInfo);

	// Returns the list of "recursive" TTCreatorResearchInfo that have been disabled, including the research that enables unitInfo.
	std::list<TTCreatorResearchInfo*> CreateDisableEffectOnUnit(TTCreatorUnitInfo *unitInfo);
	std::list<TTCreatorResearchInfo*> CreateDisableEffectOnUnit(TTDetailedUnitDef *unitDetail);

	// Copy (add) effects from internal collection (techTreeEffects) to actual tech tree object (techDef)
	void AddEffectsToTechDef();

};




}
}