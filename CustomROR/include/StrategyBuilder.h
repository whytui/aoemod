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

namespace STRATEGY {

	// Represents various military categories used to help strategy building decisions
	enum MILITARY_CATEGORY {
		MC_NONE = -1, // ignore in computations
		MC_PRIEST = 0,
		MC_FAST_RANGED,
		MC_SIEGE,
		MC_MELEE,
		MC_TOWER,
		MC_COUNT
	};


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


	// Represents 1 unit ("definition") that can potentially be selected to be added to strategy.
	class PotentialUnitInfo {
	public:
		PotentialUnitInfo() {
			this->upgradedUnitDefLiving = NULL;
			this->unitDefId = -1;
			this->baseUnitDefId = -1;
			this->enabledByResearchId = -1;
			this->strongestUpgradeUnitDefId = -1;
			this->hasUnavailableUpgrade = false;
			this->hasCivBonus = false;
			this->hasSpecificAttackBonus = false;
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
			this->isSuperUnit = false;
			this->damageScore = 0;
			this->globalScore = 0;
			for (int i = 0; i < 4; i++) { this->trainCosts[i] = 0; }
			this->weightedCost = 0;
			this->bonusForRareStrength = 0;
			this->bonusForUsedResourceTypes = 0;
			this->isSelected = false;
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
		bool isBoat; // true for water units, false for land units
		float speed;
		float range;
		bool isMelee; // Might be simpler that manipulating range everywhere... We consider range<2 means melee (normally, range=0 for melee, but beware exceptions like fire galley)
		short int hitPoints;
		short int displayedAttack; // displayedAttack from unitDef is reliable (always matches default attack without bonuses, never 0 for units with an attack)
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType;
		char *unitName; // For debug
		bool hasCivBonus;
		bool hasSpecificAttackBonus; // true if unit has some special specific bonus, like camel, slinger, chariot
		float conversionResistance;
		long int totalResourceCost;
		bool costsGold;
		int trainCosts[4]; // Cost for training 1 unit (only food/wood/stone/gold costs)
		int weightedCost; // A (subjective) weighted cost value, where rare / precious resources are more valued.
		bool isSuperUnit;
		int damageScore; // An index 1-4 indicating unit attack. 1 is weak, 4 is strong.
		// Scores
		int strengthVs[MC_COUNT];
		int weaknessVs[MC_COUNT]; // Negative values mean good resistance
		int globalScore;
		int bonusForRareStrength; // 0-100
		int bonusForUsedResourceTypes; // 0-100
		bool isSelected;
	};

	// Class that handles the selection of military units for strategy creation.
	class StrategyBuilder {
	public:
		StrategyBuilder(CustomRORInfo *crInfo) {
			this->crInfo = crInfo;
			this->player = NULL;
			this->selectedLandUnitsCount = 0;
			this->selectedWaterUnitsCount = 0;
			for (int i = 0; i < 4; i++) { this->totalTrainUnitCosts[i] = 0; }
		}
		~StrategyBuilder() {
			this->FreePotentialElementsList();
		}
		CustomRORInfo *crInfo;

		// Create a brand new dynamic strategy for player.
		void CreateStrategyFromScratch(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

		std::shared_ptr<StrategyGenerationInfo> GetStrategyGenerationInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);


	private:
		std::list<PotentialUnitInfo*> potentialUnitsList; // All eligible military units, collected from tech tree and player's unitDef array
		std::list<PotentialUnitInfo*> actuallySelectedUnits; // *The* selection of units that will actually be added to strategy
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player;
		int selectedLandUnitsCount;
		int selectedWaterUnitsCount;
		int totalTrainUnitCosts[4];

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
		void ComputeScoresVsPriests(PotentialUnitInfo *unitInfo);
		// Compute score fields for towers in unitInfo object.
		void ComputeScoresVsTower(PotentialUnitInfo *unitInfo);

		// Compute global scores using many criteria (unit cost, damage, civ bonus, super unit, (un/)available techs...), using a random part.
		// Does not compare units/unit scores to each other at this point (all scores are independent)
		void ComputeGlobalScores();

		// Recompute unit info bonuses that depend on comparison with other units
		void RecomputeComparisonBonuses(std::list<PotentialUnitInfo*> selectedUnits, bool waterUnit);

		void SelectStrategyUnitsFromPreSelection(std::list<PotentialUnitInfo*> preSelectedUnits, bool waterUnits);

		// Select which units are to be added in strategy, based on potentialUnitsList
		// Requires that this->potentialUnitsList has already been filled
		void SelectStrategyUnitsForCategory(bool waterUnits);

		// Select which units are to be added in strategy, based on potentialUnitsList
		void SelectStrategyUnits();

		// Add a unit to selection and updates some internal variables accordingly
		// Return total unit count in selection
		int AddUnitIntoToSelection(PotentialUnitInfo *unitInfo);
	};


	// Returns true if the two classes are equals or very similar (infantry with phalanx, archer classes...)
	static bool AreSimilarClasses(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2);
}
