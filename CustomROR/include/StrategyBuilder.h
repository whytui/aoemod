#pragma once
#include <assert.h>
#include <string.h>
#include <vector>
#include <set>
#include <memory>
#include <ROR_structures.h>
#include <AOE_const_functional.h>
#include "crCommon.h"
#include "researches.h"
#include "randomizer.h"
#include "strategy.h"


using namespace AOE_CONST_FUNC;


// This class collects useful information for strategy generation
class StrategyGenerationInfo {
public:
	StrategyGenerationInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
		this->civId = -1;
		this->fishingShipsCount_alwaysRetrain = 0;
		this->fishingShipsCount_limitedRetrains = 0;
		this->maxPopulation = 50;
		this->tradeShipsCount = 0;
		this->transportShipsCount = 0;
		this->villagerCount_alwaysRetrain = 0;
		this->villagerCount_limitedRetrains = 0;
		this->SetPlayerInfo(player);
	}
	const ROR_STRUCTURES_10C::STRUCT_PLAYER *player;
	const ROR_STRUCTURES_10C::STRUCT_AI *ai; // automatically set from player (SetPlayerAndAIStructs)
	const ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI; // automatically set from player (SetPlayerAndAIStructs)
	short int civId;
	bool isWaterMap;
	long int maxPopulation;
	long int villagerCount_alwaysRetrain;
	long int villagerCount_limitedRetrains;
	long int fishingShipsCount_alwaysRetrain;
	long int fishingShipsCount_limitedRetrains;
	long int tradeShipsCount;
	long int transportShipsCount;
	//long int warShipsCount;
	//long int militaryUnitsCount;

private:
	void SetPlayerInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
		this->ai = NULL;
		this->buildAI = NULL;
		this->player = player;
		if (!player || !player->IsCheckSumValid()) { return; }
		this->civId = player->civilizationId;
		this->ai = player->ptrAIStruct;
		if (!this->ai || !this->ai->IsCheckSumValid()) { return; }
		this->buildAI = &this->ai->structBuildAI;
	}
};


class PotentialUnitInfo {
public:
	PotentialUnitInfo() {
		this->upgradedUnitDefLiving = NULL;
		this->unitDefId = -1;
		this->enabledByResearchId = -1;
		this->strongestUpgradeUnitDefId = -1;
		this->hasUnavailableUpgrade = false;
		this->hasCivBonus = false;
		this->hitPoints = 0;
		this->range = 0;
		this->isMelee = false;
		this->speed = 0;
		this->displayedAttack = 0;
		this->unitAIType = GLOBAL_UNIT_AI_TYPES::TribeAINone;
		this->unavailableRelatedResearchesCount = 0;
		this->availableRelatedResearchesCount = 0;
		this->ageResearchId = -1;
		this->conversionResistance = 1; // Default value
		this->totalResourceCost = 0;
		this->costsGold = false;
	}
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *upgradedUnitDefLiving; // (DATID2) Unit definition of best upgraded unit (this is not base unit = could differ from unitDefId)
	short int unitDefId; // DATID1 (base unit ID = without upgrades)
	short int baseUnitDefId; // non-upgraded unit that can be upgraded to "this" one, if any.
	std::list<short int> upgradesUnitDefId; // List of all available unitDefId this unit can be upgraded to
	short int strongestUpgradeUnitDefId; // Unit definition ID of best available upgraded unit
	short int enabledByResearchId; // research ID that enables the unit, if any. -1 if no such research (unit is always available)
	bool hasUnavailableUpgrade; // Some unit upgrade are unavailable in my tech tree (which means I'm missing super unit, or at least full upgraded unit)
	long int unavailableRelatedResearchesCount;
	long int availableRelatedResearchesCount;
	short int ageResearchId; // ResearchId of the age (stone/tool/bronze/iron) where the unit can be enabled first. -1=always available
	float speed;
	float range;
	bool isMelee; // Might be simpler that manipulating range everywhere... We consider range<2 means melee (normally, range=0 for melee, but beware exceptions like fire galley)
	short int hitPoints;
	short int displayedAttack; // displayedAttack from unitDef is reliable (always matches default attack without bonuses, never 0 for units with an attack)
	AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType;
	char *unitName; // For debug
	bool hasCivBonus;
	float conversionResistance;
	long int totalResourceCost;
	bool costsGold;
	// Scores
	int strengthVsPriests;
	int strengthVsSiege;
	int strengthVsFastRanged;
	int strengthVsMelee;
	int strengthVsTowers;
	int weaknessVsPriests; // Negative value means good resistance
	int weaknessVsSiege; // Negative value means good resistance
	int weaknessVsFastRanged; // Negative value means good resistance
	int weaknessVsMelee; // Negative value means good resistance
};


class StrategyBuilder {
public:
	StrategyBuilder(CustomRORInfo *crInfo) {
		this->crInfo = crInfo;
		this->player = NULL;
	}
	~StrategyBuilder() {
		this->FreePotentialElementsList();
	}
	CustomRORInfo *crInfo;

	// Create a brand new dynamic strategy for player.
	void CreateStrategyFromScratch(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

	std::shared_ptr<StrategyGenerationInfo> GetStrategyGenerationInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);


private:
	std::list<PotentialUnitInfo*> potentialUnitsList;
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player;

	void CreateBasicStrategyElements(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

	// Clears potential units list and free each underlying PotentialUnitInfo object.
	void FreePotentialElementsList();

	// Fills unitInfos with all available military units from tech tree.
	// Towers are ignored (not added to list). Boats are ignored on non-water maps.
	// *** Make sure to delete all PotentialUnitInfo from list when you're finished with the list ***
	void CollectPotentialUnitsInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

	// Compute all score fields for all units in potential units list.
	void ComputeScoresForPotentialUnits();
	// Compute score fields for priests in unitInfo object.
	void ComputeScoresVsPriests(PotentialUnitInfo *unitInfo, int damageScore);
	// Compute score fields for towers in unitInfo object.
	void ComputeScoresVsTower(PotentialUnitInfo *unitInfo);

	void SelectStrategyUnits();
};

