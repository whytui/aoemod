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
	std::set<ROCKNROR::STRATEGY::TTDetailedTrainableUnitDef*>enabledTrainableUnitInfos;
	std::set<ROCKNROR::STRATEGY::TTDetailedTrainableUnitDef*>disabledTrainableUnitInfos; // All disabled trainable units, including "intermediate" upgrades
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

	// Fill eligibleDisableXxx, disabledResearchIds, disabledUnitDefIds collections (etc) according to player's tech tree (myTechTreeId).
	void CollectInfoFromExistingTechTree();

	// Returns true if the living unit is disabled in player tech tree.
	// Returns false if the unit is NOT a living unit (returns false for buildings !)
	bool IsTrainableUnitDisabled(long int unitDefId) const;
	// Returns true if the building is disabled in player tech tree.
	// Returns false if the unit is NOT a building
	bool IsBuildingUnitDisabled(long int unitDefId) const;
	// Returns true if the unit (living unit or building) is disabled in player tech tree
	bool IsUnitDisabled(long int unitDefId) const;

	// Returns true if the research is disabled in player tech tree
	bool IsResearchDisabled(long int resDefId) const;

	// Adds an asterisk to unit names, for unit that benefit from a civ bonus
	void MarkCivBonusUnitNames();
};


// Research-information for tech tree generator
class TTCreatorResearchInfo {
public:
	TTCreatorResearchInfo() {
		this->researchDetail = NULL;
		this->researchId = -1;
		this->rawDisableProbability = -1;
		this->disableWeight = 0;
		this->updatedArmorClass = ATTACK_CLASS::CST_AC_NONE;
		this->updatedAttackClass = ATTACK_CLASS::CST_AC_NONE;
		this->techImpactsReligion = false;
		this->hasBeenDisabled = false;
		this->isFakeForDisableUnit = false;
		this->internalName = "";
		this->langName = "";
	}
	long int researchId;
	std::string internalName;
	std::string langName;
	double rawDisableProbability; // 0-1 score. -1 means "not set"
	double disableWeight; // 0-1 score. The weight (impact) if this research gets disabled
	TTDetailedResearchDef *researchDetail;
	long int researchLocation;
	AOE_CONST_FUNC::ATTACK_CLASS updatedArmorClass; // Armor class the tech upgrades. None (-1) in most cases.
	AOE_CONST_FUNC::ATTACK_CLASS updatedAttackClass; // Attack class the tech upgrades. None (-1) in most cases.
	bool techImpactsReligion;
	bool hasBeenDisabled;
	bool isFakeForDisableUnit;

	// Returns the final "disable probability" for this research.
	// Returns <=0 if the unit must not be disable OR must not be disabled directly (handled by "unit line")
	double GetDisableProbability() const;
};


// Unit-information for tech tree generator
class TTCreatorUnitInfo {
public:
	TTCreatorUnitInfo() {
		this->unitDefId = -1;
		this->unitDetail = NULL;
		this->rawDisableProbability = -1;
		this->disableWeight = 0;
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
	double rawDisableProbability; // 0-1 score. -1 means "not set"
	double disableWeight; // 0-1 score. The weight (impact) if this research gets disabled
	TTDetailedUnitDef *unitDetail;
	long int trainLocation;
	long int sourceResearchId; // ID of the research that enables me, if it exists and is unique
	bool hasBeenDisabled;
	bool hasBeenDisabledDirectly; // True when the unit was disabled by direct choice (not just a consequence of another research being disabled)
	bool hasADisabledChild; // True when a child unit (an upgrade) was disabled by direct choice. WARNING: this is only set for root units.
	TTCreatorBaseUnitDisableBehavior rootUnitDisablePolicy;

	// Returns the final "disable probability" for this unit.
	// Returns <=0 if the unit must not be disable OR must not be disabled directly (handled by "unit line")
	double GetDisableProbability() const;
};


// Configuration for tech tree generation
namespace TT_CONFIG {
	static const double RES_PROBA_MIN = 0;
	static const double RES_PROBA_MAX = 1;
	static const double RES_WEIGHT_MIN = 0;
	static const double RES_WEIGHT_MAX = 1;
	static const double RES_PROBA_STANDARD_RESEARCH = 0.5; // "disable probability" for a standard research
	static const double RES_WEIGHT_STANDARD_RESEARCH = 0.1; // "disable weight" for a standard research
	static const double RES_PROBA_SPECIALIZED_RESEARCH = 0.6; // "disable probability" for "specialized" research: availability=a bit more rare than standard. E.g. shields.
	static const double RES_PROBA_OFTEN_DISABLED = 0.7; // "disable probability" for researches that are disabled in many cases
	static const double RES_PROBA_COMMON_USEFUL_RESEARCH = 0.25; // "disable probability" for a very common/useful research like architecture
	static const double RES_WEIGHT_COMMON_USEFUL_RESEARCH = 0.75; // "disable weight" for a very common/useful research like architecture
	static const double RES_PROBA_WHEEL = 0.015; // "disable probability" for Wheel research
	static const double RES_WEIGHT_WHEEL = 0.9; // "disable weight" for Wheel research
	static const double RES_PROBA_STANDARD_UNIT = 0.5; // "disable probability" for a standard unit
	static const double RES_WEIGHT_STANDARD_UNIT = 0.2; // "disable weight" for a standard unit
	static const double RES_PROBA_SPECIALIZED_UNIT = 0.6; // "disable probability" for a unit which is slighly more rare than standard, like ballista (quite often disabled).
	static const double RES_PROBA_VERY_RARE = 0.05; // Disable probability for unit that must be disabled on very rare occasions: priests, stone thrower, hoplite...
	static const double RES_WEIGHT_HIGH_IMPACT_UNIT = 0.7; // "Disable weight" for unit whose absence has a strong impact on civilization: priest, stone thrower
	static const double RES_WEIGHT_CHARIOTS = 0.4; // "Disable weight" for chariots/chariot archer
	static const double RES_ADDITIONTAL_WEIGHT_SUPER_UNIT = 0.08; // Additional "Disable weight" for super units

	static const double RES_PROBA_IMPACT_ON_RANDOM = 0.9; // computed probability counts as much as xxx% vs random. 1=100%=apply probability normally. Do not set to 0.
	static int CALC_MIN_DISABLE_UNITLINE_COUNT_PERCENT = 40;
	static int CALC_MAX_DISABLE_UNITLINE_COUNT_PERCENT = 66;
	static int CALC_MIN_DISABLE_RESEARCH_COUNT_PERCENT = 20;
	static int CALC_MAX_DISABLE_RESEARCH_COUNT_PERCENT = 45;
	static int MIN_DISABLE_UNITLINE_COUNT = 6; // default value, recomputed according to CALC_*
	static int MAX_DISABLE_UNITLINE_COUNT = 12; // default value, recomputed according to CALC_*
	static int MIN_DISABLE_RESEARCH_COUNT = 6; // default value, recomputed according to CALC_*
	static int MAX_DISABLE_RESEARCH_COUNT = 13; // default value, recomputed according to CALC_*

	static const int GEN_BONUS_MIN_RATE_BASE = 10; // "Base" minimum % improvement for generated bonus (can be adjusted)
	static const int GEN_BONUS_MAX_RATE_BASE = 25; // "Base" maximum % improvement for generated bonus (can be adjusted)
	static const int GEN_BONUS_MIN_RATE_BETTER = 15; // Min % improvement for bonus that needs a decent value to be useful: e.g. HP (buildings)
	static const int GEN_BONUS_MAX_RATE_BETTER = 30; // Max % improvement for bonus that needs a decent value to be useful: e.g. HP (buildings)
	static const int GEN_BONUS_MIN_RATE_GOOD = 25; // Min % improvement for bonus that needs a decent value to be useful: e.g. speed, HP
	static const int GEN_BONUS_MAX_RATE_GOOD = 40; // Max % improvement for bonus that needs a decent value to be useful: e.g. speed, HP
	static const int GEN_BONUS_MIN_RATE_FARM_FOOD_AMOUNT = 30; // Min % improvement for farm food amount bonus
	static const int GEN_BONUS_MAX_RATE_FARM_FOOD_AMOUNT = 80; // Max % improvement for farm food amount bonus
}


struct BonusGenProperties {
	double weight;
	double probabilityCoeff; // Coefficient 0-1 that affects probability of choosing this class for a bonus. Default=1. Use values like 1.20 or 0.80 to add/drop 20% chances, etc
};
typedef std::pair<const GLOBAL_UNIT_AI_TYPES, BonusGenProperties> BonusGenUCPPair; // For unitClass-genProperties pair
typedef std::pair<const TECH_UNIT_ATTRIBUTES, BonusGenProperties> BonusGenAttrPPair; // For unitAttribute-genProperties pair


// Class that allows creating dynamic tech tree, based on tech tree analyzer
class TechTreeCreator {
public:
	TechTreeCreator(bool simulationMode) {
		this->Reset();
		this->simulationMode = simulationMode;
	}
	TechTreeCreator(double expectedAverageDisableWeight) {
		this->Reset();
		this->expectedAverageDisableWeight = expectedAverageDisableWeight;
	}
	long int arrayUnitDefCount;
	long int arrayResearchCount;
	STRUCT_TECH_DEF *techDefForTechTree;
	int religionLevel; // 0=low(high disable proba), 3=high(low disable temple research proba)
	int economyLevel; // 0=low(high disable proba), 3=high(low disable economy techs proba)
	double totalDisableWeight; // Evaluated "weight" of disabled units/researches (sum of disabled elements weight)
	double disabledWeightProportion; // "disabledWeight/average". If <1, my tech tree is quite rich. If >1, many techs are disabled
	double bonusProportion; // "bonusWeight/average". If <1, I have few bonus. If <disabledWeightProportion, I have too few bonus to compensate the amount of disabled stuff in tech tree.
	std::set<long int> eligibleDisableResearchIdBronze;
	std::set<long int> eligibleDisableResearchIdIron;
	std::set<long int> eligibleDisableUnitDefIdBronze;
	std::set<long int> eligibleDisableUnitDefIdIron;
	std::list<TTCreatorResearchInfo*> allCreatorResearchInfo; // All *eligible* (that may be disabled) research info. May exclude researches that enable units (see allCreatorUnitInfo instead)
	std::list<TTCreatorUnitInfo*> allCreatorUnitInfo; // All *eligible* units info (units that may be disabled).
	std::list<STRUCT_TECH_DEF_EFFECT> techTreeEffects;
	std::set<GLOBAL_UNIT_AI_TYPES> classesWithBonus; // Unit classes that benefit from a civ bonus
	std::set<long int> trainLocationsWithBonus; // All buildings where at least 1 trainable unit benefits from some civ bonus

	void Reset() {
		this->simulationMode = false;
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
		this->disabledWeightProportion = 0;
		this->bonusProportion = 0;
		this->bonusText = "";
		this->classesWithBonus.clear();
		this->lastAverageBonusWeight = 0;
		this->expectedAverageDisableWeight = 0;
		this->bonusCount = 0;
	}

	// Create a random tech tree (list of effects) on provided TechDef (should be initially empty).
	void CreateRandomTechTree(STRUCT_TECH_DEF *techDef);

	std::string GetDisabledUnitsText() const;

	std::string GetDisabledResearchesText() const;

	std::string GetCivBonusText() const;

	std::list<std::string> GetHumanBonusTextLines() const;

private:
	std::string bonusText;

	// Actual number of bonus created for the civilization
	int bonusCount;

	// If true, do not actually create any bonus
	bool simulationMode;

	// Average bonus weight from last calculation
	double lastAverageBonusWeight;
	// Expected average value for "disable weight" (guessed from statistics)
	double expectedAverageDisableWeight;

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

	// Creates disable research effects according to disable scores that have been computed (on all researches)
	void CreateDisableResearchesEffects();
	// Creates disable unit effects according to disable scores that have been computed (on root units)
	// If a unit(line) is selected for being disabled, then this method chooses at which level (which upgrade) the unitline becomes unavailable.
	void CreateDisableUnitsEffects();

	// Calculates this->totalDisableWeight according to what has been disabled in tech tree (cf internal data)
	void CalcTotalDisabledWeight();

	// Creates random civ bonuses
	void CalcRandomCivBonus();

	// Create one civ bonus
	double CreateOneBonus();

	// Returns the weight to be added. 0 in most cases, !=0 for special cases
	double CreateOneBonusEffect(AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES bonusUnitClass, TECH_UNIT_ATTRIBUTES unitAttr,
		int minBonusRate, int maxBonusRate, int availableAffectedUnitLines);

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

	// Add an effect into internal collection (techTreeEffects) for EACH unitDefId provided in the parameter collection
	// Useful to copy an effect from root unit to its upgrades
	void AddSameEffectForUnitDefIDs(AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT *srcEffect, const std::list<long int> &unitDefIDs);

	// Add an effect into internal collection (techTreeEffects) for EACH unitDefId that belongs to same unit lineage
	// Useful to copy an effect from root unit to its upgrades
	// Unit can be either a "trainable" or a building
	void AddSameEffectForUnitLineage(AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT *srcEffect, short int unitDefId);

	// Randomly selects 1 non-disabled root unit in provided unit class. NULL if not found.
	// Use minimumRequiredAgeResearchId to filter on units >= provided age. E.g. CST_RSID_TOOL_AGE
	TTDetailedUnitDef *PickOneRandomRootUnitIdAmongUnitClass(GLOBAL_UNIT_AI_TYPES unitClass, int minimumRequiredAgeResearchId);

};




}
}
