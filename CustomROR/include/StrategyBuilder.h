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
#include "AOE_strategy.h"
#include "StrategyPotentialElemInfo.h"


using namespace AOE_CONST_FUNC;

namespace STRATEGY {

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
		void CreateStrategyFromScratch();

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

		// Get the strategy element that correspond to an Age Id. researchId must correspond to an age upgrade !
		// For stone age, returns the "fake" first strategy element (beginning of strategy)
		// For other researches, returns NULL
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *GetAgeStrategyElement(short int researchId);

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

		// Add a building to strategy just before supplied insertion point. Updates underlying fields (added count...)
		// Returns actual number of element that were added
		int AddBuildingToStrategy(PotentialBuildingInfo *bldInfo, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *insertionPoint);

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

		// Adds non-military researches that should always be included, for example wheel - if available in tech tree.
		void AddMandatoryNonMilitaryResearches();

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

		// Add strategy elements other than "main units" required researches.
		void CreateOtherResearchesStrategyElements();

		// Add strategy elements for farms
		void CreateFarmStrategyElements();

		// Add the first building of each building kind to strategy. Returns number of added buildings
		// Does not add an additional building if there is already one in strategy
		// Always add in "parent" age strategy zone.
		int CreateFirstBuildingsStrategyElements();
		// Create secondary (optional) occurrences of buildings. E.g. 2nd TC, or additional military buildings to train army faster and do researches while another building is training units
		int CreateSecondaryBuildingStrategyElements();
	};

}
