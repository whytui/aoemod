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


	// Represents 1 unit ("definition") that can potentially be selected to be added to strategy.
	class PotentialUnitInfo {
	public:
		PotentialUnitInfo() {
			this->baseUnitDefLiving = NULL;
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
			this->speedBase = 0;
			this->speedUpgraded = 0;
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
			this->isOptionalUnit = false;
		}
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *baseUnitDefLiving; // (DATID1) Unit definition of base unit, without upgrade
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
		float speedBase;
		float speedUpgraded;
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
		bool isOptionalUnit; // True for "retrains" units and/or units added specifically against a weakness (ex: chariot vs priest is strategy has elephants). Such units don't need full upgrades
	};

	class PotentialResearchInfo {
	public:
		short int researchId;
		ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *researchDef;
		bool forceUse; // If true, add the research to strategy in all circumstances
		int impactedUnitsCount; // Number of units that have benefits from the research, including villagers, towers.
		std::list<short int> researchesThatMustBePutBeforeMe;
		std::list<short int> researchesThatMustBePutAfterMe;
		std::list<short int> unitsThatMustBePutBeforeMe;
		std::list<short int> unitsThatMustBePutAfterMe;
	};

	// Class that handles the selection of military units for strategy creation.
	class StrategyBuilder {
	public:
		StrategyBuilder(CustomRORInfo *crInfo, ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
			this->crInfo = crInfo;
			this->ai = NULL;
			this->buildAI = NULL;
			this->player = NULL;
			this->selectedLandUnitsCount = 0;
			this->selectedWaterUnitsCount = 0;
			for (int i = 0; i < 4; i++) { this->totalTrainUnitCosts[i] = 0; }
			this->seToolAge = NULL;
			this->seBronzeAge = NULL;
			this->seIronAge = NULL;
			this->seGovCenter = NULL;
			this->seMarket = NULL;
			this->seTownCenter = NULL;
			this->seWheel = NULL;
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
		~StrategyBuilder() {
			this->FreePotentialElementsList();
		}
		CustomRORInfo *crInfo;

		int GetRandomFactor() { return randomPercentFactor; }
		void SetRandomFactor(int value) { if ((value >= 0) && (value <= 100)) { randomPercentFactor = value; } }
		// Create a brand new dynamic strategy for player.
		void CreateStrategyFromScratch(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

	private:
		static int randomPercentFactor;
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player;
		ROR_STRUCTURES_10C::STRUCT_AI *ai; // automatically set from player (SetPlayerAndAIStructs)
		ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI; // automatically set from player (SetPlayerAndAIStructs)
		short int civId;
		bool isWaterMap;
		long int maxPopulation;
		long int villagerCount_alwaysRetrain;
		long int villagerCount_limitedRetrains;
		long int fishingShipsCount_alwaysRetrain;
		long int fishingShipsCount_limitedRetrains;
		long int tradeShipsCount;
		long int transportShipsCount;
		// Variables for units handling
		std::list<PotentialUnitInfo*> potentialUnitsList; // All eligible military units, collected from tech tree and player's unitDef array
		std::list<PotentialUnitInfo*> actuallySelectedUnits; // *The* selection of units that will actually be added to strategy
		int selectedLandUnitsCount;
		int selectedWaterUnitsCount;
		int totalTrainUnitCosts[4];
		// Variables for researches handling
		std::list<PotentialResearchInfo*> potentialResearchesList;
		// Strategy itself
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seTownCenter;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seToolAge;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seBronzeAge;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seIronAge;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seMarket;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seGovCenter;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seWheel; // May be null
		std::list<short int> requiredBuildings; // List of required DATID of buildings (units)

		/*** General methods ***/
		// Initialize player-related information (for constructor)
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

		// Clears potential units list and free each underlying PotentialUnitInfo object.
		void FreePotentialElementsList();

		// Add a unit to selection and updates some internal variables accordingly
		// Return total unit count in selection
		int AddUnitIntoToSelection(PotentialUnitInfo *unitInfo);


		/*** Units selection methods ***/

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

		// Recompute unit Info's score for costs, according to currently (actually) selected units.
		int GetCostScoreRegardingCurrentSelection(PotentialUnitInfo *unitInfo);

		// Get unit info's score for costs, according to currently (actually) selected units.
		// upgradedUnit: if true, consider upgraded unit. if false, consider base unit
		void RecomputeComparisonBonuses(std::list<PotentialUnitInfo*> selectedUnits, bool waterUnit, bool upgradedUnit);

		// Make the selection of units to add to strategy based on a pre-selection list.
		// waterUnits : true=deal with water units only, false=deal with land units only
		void SelectStrategyUnitsFromPreSelection(std::list<PotentialUnitInfo*> preSelectedUnits, bool waterUnits);

		// Select which units are to be added in strategy, based on potentialUnitsList
		// Requires that this->potentialUnitsList has already been filled
		void SelectStrategyUnitsForLandOrWater(bool waterUnits);

		// Search for a unit that can be added to strategy as "optional" (without full upgrades) to fight a detected weakness.
		// waterUnits: if true, manage water units only. If false, manage land units only
		void AddOptionalUnitAgainstWeakness(MILITARY_CATEGORY weaknessCategory, bool waterUnits);

		// Select which units are to be added in strategy, based on potentialUnitsList
		void SelectStrategyUnits();


		/*** Strategy writing methods ***/

		// Get the very global information about strategy generation (number of villagers, etc
		void CollectGlobalStrategyGenerationInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

		// Create all base strategy elements (ages + buildings=barracks,market,govSiege + wheel if available)
		// Does not add villagers
		void CreateBasicStrategyElements();

		// Create villager strategy elements, including fishing ships
		void CreateVillagerStrategyElements();
	};


	// Returns true if the two classes are equals or very similar (infantry with phalanx, archer classes...)
	static bool AreSimilarClasses(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2);
}
