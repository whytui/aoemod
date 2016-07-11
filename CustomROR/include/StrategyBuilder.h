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
	// Used for military units (not buildings, not villagers, not towers)
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
			this->bonusForTechsSimilarity = 0;
			this->isSelected = false;
			this->isOptionalUnit = false;
			this->earlyAgeUnit = false;
			this->desiredCount = 0;
			this->addedCount = 0;
			this->scoreForEarlyAge = 0;
			this->firstStratElem = NULL;
		}
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *baseUnitDefLiving; // (DATID1) Unit definition of base unit, without upgrade
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *upgradedUnitDefLiving; // (DATID2) Unit definition of best upgraded unit (this is not base unit = could differ from unitDefId)
		short int unitDefId; // DATID1 (base unit ID = without upgrades)
		short int baseUnitDefId; // non-upgraded unit that can be upgraded to "this" one, if any. -1 in most cases. PLEASE use unitDefId instead.
		std::list<short int> upgradesUnitDefId; // List of all available unitDefId this unit can be upgraded to
		short int strongestUpgradeUnitDefId; // Unit definition ID of best available upgraded unit
		short int enabledByResearchId; // research ID that enables the unit, if any. -1 if no such research (unit is always available)
		bool hasUnavailableUpgrade; // Some unit upgrade are unavailable in my tech tree (which means I'm missing super unit, or at least full upgraded unit)
		long int unavailableRelatedResearchesCount;
		long int availableRelatedResearchesCount;
		short int ageResearchId; // ResearchId of the age (stone/tool/bronze/iron) where the unit can be enabled first. -1=always available
		std::set<short int> requiredResearchesForBaseUnit;
		bool isBoat; // true for water units, false for land units
		float speedBase;
		float speedUpgraded;
		float range;
		bool isMelee; // Might be simpler that manipulating range everywhere... We consider range<2 means melee (normally, range=0 for melee, but beware exceptions like fire galley)
		short int hitPoints;
		short int displayedAttack; // displayedAttack from unitDef is reliable (always matches default attack without bonuses, never 0 for units with an attack). displayedAttack=number before the "+" (constant even after upgrade)
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
		int bonusForTechsSimilarity; // 0-100
		bool isSelected;
		bool isOptionalUnit; // True for "retrains" units and/or units added specifically against a weakness (ex: chariot vs priest is strategy has elephants). Such units don't need full upgrades
		bool earlyAgeUnit; // If true, this unit is just for early ages. It is NOT a main unit (not even optional from main choice) and must have limited retrains.
		float desiredCount;
		float addedCount;
		float scoreForUnitCount; // Used in temporary treatments to compute number of units to add in strategy
		float scoreForEarlyAge; // A temporary score for early age unit choices
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *firstStratElem; // First strategy element that trains such a unit
	};

	class PotentialResearchInfo {
	public:
		PotentialResearchInfo() {
			this->researchId = -1;
			this->researchDef = NULL;
			this->requiredAge = -1;
			this->techDef = NULL;
			this->forceUse = false;
			this->hasOptionalRequirements = false;
			this->directRequirementsAreSatisfied; // True when enough direct requirements are satisfied to "enable" the research.
			for (int i = 0; i < 4; i++) { missingRequiredResearches[i] = -1; }
			this->isInStrategy = false;
			this->mustBeAfterThisElem = NULL;
			this->mustBeBeforeThisElem = NULL;
			this->requiredAgeResearch = NULL;
			this->totalCosts = 0;
			this->forcePlaceForFirstImpactedUnit = false;
		}
		short int researchId;
		short int requiredAge;
		ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *researchDef;
		ROR_STRUCTURES_10C::STRUCT_TECH_DEF *techDef;
		bool hasOptionalRequirements;
		bool directRequirementsAreSatisfied;
		short int missingRequiredResearches[4]; // -1 = empty slot. Other value = a required research ID that is not satisfied yet
		bool forceUse; // If true, add the research to strategy in all circumstances
		std::set<short int> researchesThatMustBePutBeforeMe;
		std::set<short int> researchesThatMustBePutAfterMe;
		std::set<short int> unitsThatMustBePutBeforeMe;
		std::set<short int> unitsThatMustBePutAfterMe;
		std::set<short int> impactedUnitDefIds; // Units that have benefits from the research, including villagers, towers.
		int unitInstanceScoreForOptionalResearch; // internal value for optional researches computing
		bool forcePlaceForFirstImpactedUnit; // If true, research will be put close to the first impacted unit "train" elements. Default=false. If set, other constraints may be ignored.
		bool isInStrategy;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *mustBeAfterThisElem; // Do not modify outside of ComputeStratElemPositionConstraints
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *mustBeBeforeThisElem; // Do not modify outside of ComputeStratElemPositionConstraints
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *requiredAgeResearch; // Age research strategy element of my required age.
		int totalCosts; // Sum of all raw costs (not weighted)

		// Updates this->mustBeBeforeThisElem and this->mustBeAfterThisElem according to known dependencies on other unit/researches
		// Previous values of mustBeBeforeThisElem  and mustBeAfterThisElem are reset (lost)
		void ComputeStratElemPositionConstraints(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);
	};

	class PotentialBuildingInfo {
	public:
		PotentialBuildingInfo() {
			this->unitDef = NULL;
			this->unitDefId = -1;
			this->enabledByResearchId = -1;
			this->enabledInAge = -1; // ResearchId of age where the building becomes available
			this->addedInStrategyCount = 0;
		}
		short int unitDefId;
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDef;
		short int enabledByResearchId;
		short int enabledInAge;
		int addedInStrategyCount;
	};

	// Class that handles the selection of military units for strategy creation.
	class StrategyBuilder {
	public:
		StrategyBuilder(CustomRORInfo *crInfo, ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
			this->crInfo = crInfo;
			this->ai = NULL;
			this->buildAI = NULL;
			this->player = NULL;
			this->global = NULL;
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

		const std::string GetLog() const { return this->log; }

		// Create a brand new dynamic strategy for player.
		void CreateStrategyFromScratch(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

		// Searches researches that impact a specific unit and add them to internal list of potential researches.
		// Searches recursively required researches EXCEPT for "optional" requirements. No decision is made here.
		// allUpgrades: if true, all related upgrades will be added. Otherwise, only requirements will be added.
		// Returns the number of actually added elements to list (some might already be in list before)
		// You may need to call UpdateRequiredBuildingsFromValidatedResearches, UpdateMissingResearchRequirements and AddMissingBuildings() afterwards
		int CollectResearchInfoForUnit(short int unitDefId, bool allUpgrades);

		// Adds all necessary buildings for "validated" researches to buildings ID list.
		// Returns number of building IDs added to internal list
		int UpdateRequiredBuildingsFromValidatedResearches();

		// Update all researches requirement statuses and set directRequirementsAreSatisfied if OK.
		// Takes into account "confirmed" researches from list and also buildings ("initiates research").
		// This does not add anything to our lists.
		void UpdateMissingResearchRequirements();

		// Set resInfo->directRequirementsAreSatisfied=true and resInfo->missingRequiredResearches[...]=-1 if (minimum) requirements are satisfied
		void UpdatePotentialResearchStatusFromMissingRequirements(PotentialResearchInfo *resInfo);

		// Add missing buildings - if any - that block some research requirements
		void AddMissingBuildings();

	private:
		std::string log;
		static int randomPercentFactor;
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player;
		ROR_STRUCTURES_10C::STRUCT_AI *ai; // automatically set from player (SetPlayerAndAIStructs)
		ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI; // automatically set from player (SetPlayerAndAIStructs)
		ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global;
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
		std::list<PotentialUnitInfo*> actuallySelectedUnits; // *The* selection of units that will actually be added to strategy, including "temporary" units (limited retrains)
		int selectedLandUnitsCount;
		int selectedWaterUnitsCount;
		int totalTrainUnitCosts[4];
		// Variables for researches handling
		std::list<PotentialResearchInfo*> potentialResearchesList;
		std::set<PotentialBuildingInfo*> potentialBuildingsList; // Info about needed buildings (units)
		// Strategy itself
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seTownCenter;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seToolAge;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seBronzeAge;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seIronAge;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seMarket;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seGovCenter;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *seWheel; // May be null

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
			this->global = player->ptrGlobalStruct;
			if (this->global && !this->global->IsCheckSumValid()) { this->global = NULL; }
			this->maxPopulation = (int)player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_POULATION_LIMIT);
		}

		// Clears potential units list and free each underlying PotentialUnitInfo object.
		void FreePotentialElementsList();

		// Add a unit to selection and updates some internal variables accordingly
		// Return total unit count in selection
		int AddUnitIntoToSelection(PotentialUnitInfo *unitInfo);

		// Add a research to strategy just before supplied insertion point. Updates "isInStrategy" field.
		// Returns actual number of element that were added
		int AddResearchToStrategy(PotentialResearchInfo *resInfo, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *insertionPoint);

		// Returns a pointer to the PotentialResearchInfo object for a research, or NULL if not found.
		PotentialResearchInfo *GetResearchInfo(short int researchId);
		// Returns a pointer to the PotentialResearchInfo object for a research, or NULL if not found.
		PotentialResearchInfo *GetResearchInfo(ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef);
		// Add building to potential buildings list and initializes underlying info.
		// Returns true if actually added
		bool AddPotentialResearchInfoToList(short int researchId);

		// Returns a pointer to the PotentialBuildingInfo object for a research, or NULL if not found.
		PotentialBuildingInfo *GetBuildingInfo(short int unitDefId);
		// Returns a pointer to the PotentialBuildingInfo object for a research, or NULL if not found.
		PotentialBuildingInfo *GetBuildingInfo(ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDef);
		// Add building to potential buildings list and initializes underlying info.
		// Returns true if actually added
		bool AddPotentialBuildingInfoToList(short int unitDefId);
		bool AddPotentialBuildingInfoToList(ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDef);


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

		// Recompute unit info bonuses that depend on comparison with other units: rare strengths, balanced cost
		// Does not recompute unit strengths (but computes it using the correct strength values)
		// upgradedUnit: if true, consider upgraded unit. if false, consider base unit
		void RecomputeComparisonBonuses(std::list<PotentialUnitInfo*> selectedUnits, bool waterUnit, bool upgradedUnit);
		void RecomputeComparisonBonuses(std::list<PotentialUnitInfo*> selectedUnits, bool waterUnit, bool upgradedUnit, short int maxAgeResearchId);

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

		// age = age to take care (ignore units that are only available in later ages)
		// hasAlreadyUnits = true if provided age already has some military units
		// Returns number of added units (0/1)
		int AddOneMilitaryUnitForEarlyAge(short int age, bool hasAlreadyUnits);

		// Add some military to selection, to fille gaps in strategy in early ages (if any)
		void AddMilitaryUnitsForEarlyAges();

		// Selects optional researches to add to strategy
		// All villagers and military units must have already been added to strategy.
		void ChooseOptionalResearches();

		/*** Strategy writing methods ***/

		// Get the very global information about strategy generation (number of villagers, etc)
		void CollectGlobalStrategyGenerationInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

		// Creates base strategy elements (ages + initial Town center)
		// Does not add villagers or any other item
		void CreateTCAndAgesStrategyElements();

		// Create villager strategy elements, including fishing ships
		void CreateVillagerStrategyElements();

		// Create early ages military units strategy elements (limited retrains units)
		void CreateEarlyMilitaryUnitsElements();

		// Create main military units strategy elements
		void CreateMainMilitaryUnitsElements();

		// Add strategy elements for required researches for "main units"
		void CreateMilitaryRequiredResearchesStrategyElements();

		// Add remaining (missing) buildings to strategy. Returns number of added buildings
		// Always add in "parent" age strategy zone.
		int CreateBuildingsStrategyElements();
	};


	// Returns true if the two classes are equals or very similar
	// Infantry - phalanx - war elephant
	// Cavalry (includes camel) - chariot
	// All archers together
	static bool AreSimilarClasses(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2);

	// Updates provided bools to true/false according to the 2 classes. true=the classes use comme techs for this domain.
	static void ShareCommonTechs(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2, bool &attack, bool &defense, bool &hitPoints);

}
