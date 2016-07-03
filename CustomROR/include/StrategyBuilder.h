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
		this->unitDefId = -1;
		this->enabledByResearchId = -1;
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
	}
	short int unitDefId;
	short int baseUnitDefId; // non-upgraded unit that can be upgraded to "this" one, if any.
	std::list<short int> upgradesUnitDefId; // List of all available unitDefId this unit can be upgraded to
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
};


class StrategyBuilder {
public:
	StrategyBuilder(CustomRORInfo *crInfo) {
		this->crInfo = crInfo;
	}
	CustomRORInfo *crInfo;

	// Create a brand new dynamic strategy for player.
	void CreateStrategyFromScratch(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

	// Fills unitInfos with all available military units from tech tree.
	// Towers are ignored (not added to list). Boats are ignored on non-water maps.
	// *** Make sure to delete all PotentialUnitInfo from list when you're finished with the list ***
	void CollectPotentialUnitsInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, std::list<PotentialUnitInfo*> &unitInfos);

	std::shared_ptr<StrategyGenerationInfo> GetStrategyGenerationInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);


private:
	void CreateBasicStrategyElements(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);
};

