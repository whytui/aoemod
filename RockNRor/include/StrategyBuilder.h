#pragma once
#include <assert.h>
#include <string.h>
#include <vector>
#include <set>
#include <memory>
#include <macroUtil.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_def.h>
#include <AOE_const_functional.h>
#include <basicFilesHandling.h>
#include "RockNRorCommon.h"
#include "RockNRorInfo.h"
#include "researches.h"
#include "randomizer.h"
#include "AOE_strategy.h"
#include "StrategyPotentialElemInfo.h"
#include "customFuncRules.h"
#include "PlayerTechTree.h"


using namespace AOE_CONST_FUNC;

namespace ROCKNROR {
namespace STRATEGY {

	// Class that handles the selection of military units for strategy creation.
	class StrategyBuilder {
	public:
		StrategyBuilder(RockNRorInfo *crInfo, AOE_STRUCTURES::STRUCT_PLAYER *player) {
			this->crInfo = crInfo;
			this->ai = NULL;
			this->buildAI = NULL;
			this->player = NULL;
			this->global = NULL;
			this->playerTechTreeInfo = NULL;
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
#ifdef _DEBUG
			std::string filename = "D:\\Dév\\CPP\\AOE\\Debug\\crstrategy";
			if (this->player) {
				filename += std::to_string(this->player->playerId);
			}
			filename += ".log";
			if (WriteToFile(this->log, filename, false)) {
				traceMessageHandler.WriteMessageNoNotification(std::string("Strategy: find logs in ") + filename);
			}
#endif
		}
		RockNRorInfo *crInfo;

		int GetRandomFactor() { return this->randomPercentFactor; }
		void SetRandomFactor(int value) { if ((value >= 0) && (value <= 100)) { this->randomPercentFactor = value; } }

		const std::string GetLog() const { return this->log; }

		// Create a brand new dynamic strategy for player.
		void CreateStrategyFromScratch();

		// Searches researches that impact a specific living unit and add them to internal list of potential researches.
		// Searches recursively required researches EXCEPT for "optional" requirements. No decision is made here.
		// allUpgrades: if true, all related upgrades will be added. Otherwise, only requirements will be added.
		// Returns a list of research IDs that were actually added to list (some others might already have been in list before)
		// You may need to call UpdateRequiredBuildingsFromValidatedResearches, UpdateMissingResearchRequirements and AddMissingBuildings() afterwards
		void CollectResearchInfoForUnit(PotentialUnitInfo *unitInfo, bool allUpgrades, bool markForSelection);
		
		// Searches researches that impact a specific building and add them to internal list of potential researches.
		// Searches recursively required researches EXCEPT for "optional" requirements. No decision is made here.
		// allUpgrades: if true, all related upgrades will be added. Otherwise, only requirements will be added.
		// Returns a list of research IDs that were actually added to list (some others might already have been in list before)
		// You may need to call UpdateRequiredBuildingsFromValidatedResearches, UpdateMissingResearchRequirements and AddMissingBuildings() afterwards
		void CollectResearchInfoForUnit(PotentialBuildingInfo *bldInfo, bool allUpgrades, bool markForSelection);

		// Adds (and marks for add) all necessary buildings for "validated" researches to buildings ID list.
		// Returns number of building IDs added to internal list
		int UpdateRequiredBuildingsFromValidatedResearches();

		// Update all researches requirement statuses and set directRequirementsAreSatisfied if OK.
		// Takes into account "confirmed" researches from list and also buildings ("initiates research").
		// This does not add anything to our lists.
		void UpdateMissingResearchRequirementsInResearchInfo();

		// Set resInfo->directRequirementsAreSatisfied=true and resInfo->missingRequiredResearches[...]=-1 if (minimum) requirements are satisfied
		void UpdatePotentialResearchStatusFromMissingRequirements(PotentialResearchInfo *resInfo);

		// Add missing buildings - if any - that block some research requirements, also mark them for add
		// Potential research info "requirements" flags are updated to take into account the new building (in case it unlocks some dependencies).
		void AddMissingBuildings();

		// Get the strategy element that correspond to an Age Id. researchId must correspond to an age upgrade !
		// For stone age, returns the "fake" first strategy element (beginning of strategy)
		// For other researches, returns NULL
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *GetAgeStrategyElement(short int researchId);

	private:
		std::string log;
		static int randomPercentFactor;
		AOE_STRUCTURES::STRUCT_PLAYER *player;
		AOE_STRUCTURES::STRUCT_AI *ai; // automatically set from player (SetPlayerAndAIStructs)
		AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI; // automatically set from player (SetPlayerAndAIStructs)
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global;
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings;
		ROCKNROR::STRATEGY::CustomPlayerInfo *playerTechTreeInfo; // contains tech tree information for current player
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
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *seTownCenter;
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *seToolAge;
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *seBronzeAge;
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *seIronAge;
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *seMarket;
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *seGovCenter;
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *seWheel; // May be null

		/*** General methods ***/
		// Initialize player-related information (for constructor). Please set this->crInfo first.
		void SetPlayerInfo(AOE_STRUCTURES::STRUCT_PLAYER *player) {
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
			this->settings = GetGameSettingsPtr();
			if (this->settings && !this->settings->IsCheckSumValid()) { this->settings = NULL; }
			assert(settings != NULL);
			this->maxPopulation = (int)player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_POPULATION_LIMIT);
			// Set max population from config. Not necessary if resources have already been valued (which is the case with current implementation)
			assert(this->crInfo != NULL);
			if (this->crInfo && this->settings && (this->settings->rgeGameOptions.isSinglePlayer)) {
				this->maxPopulation = this->crInfo->configInfo.singlePlayerMaxPopulation;
			}
			this->playerTechTreeInfo = new CustomPlayerInfo();
			this->playerTechTreeInfo->ResetAndInit(player->playerId, player->civilizationId, player->techTreeId);
			this->playerTechTreeInfo->isActive = true;
			this->playerTechTreeInfo->CollectInfoFromExistingTechTree();
		}

		// Clears potential units list and free each underlying PotentialUnitInfo object.
		void FreePotentialElementsList();

		// Add a unit to selection (actuallySelectedUnits) and updates some internal variables accordingly
		// Return total unit count in selection
		int AddUnitIntoToSelection(PotentialUnitInfo *unitInfo);

		// Add a research to strategy just before supplied insertion point. Updates "isInStrategy" field.
		// Returns actual number of element that were added
		int AddResearchToStrategy(PotentialResearchInfo *resInfo, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *insertionPoint);

		// Add a building to strategy just before supplied insertion point. Updates underlying fields (added count...)
		// Returns actual number of element that were added
		int AddBuildingToStrategy(PotentialBuildingInfo *bldInfo, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *insertionPoint);

		// Update a building information object when 1 building has been added to strategy
		void UpdateBuildingInfoAfterAddInStrategy(PotentialBuildingInfo *bldInfo, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *newElem);

		// Returns a pointer to the PotentialResearchInfo object for a research, or NULL if not found.
		PotentialResearchInfo *GetResearchInfo(short int researchId) const;
		// Returns a pointer to the PotentialResearchInfo object for a research, or NULL if not found.
		PotentialResearchInfo *GetResearchInfo(AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef) const;
		// Add research to potential researches list and initializes underlying info.
		// Returns NULL if not added (error), or pointer to object if successful OR ALREADY existing
		// Adds all requirements too (researches, buildings)
		PotentialResearchInfo *AddPotentialResearchInfoToList(short int researchId, bool markForAdd);

		// Returns a pointer to the PotentialBuildingInfo object for a research, or NULL if not found.
		PotentialBuildingInfo *GetBuildingInfo(short int unitDefId) const;
		// Returns a pointer to the PotentialBuildingInfo object for a research, or NULL if not found.
		PotentialBuildingInfo *GetBuildingInfo(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef) const;
		// Add building to potential buildings list and initializes underlying info.
		// Returns true if actually added
		bool AddPotentialBuildingInfoToList(short int unitDefId, bool markForAdd);
		bool AddPotentialBuildingInfoToList(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef, bool markForAdd);

		// Returns true if a research is available in tech tree
		bool IsResearchInTechTree(short int researchId);


		/*** Units selection methods ***/

		// Fills unitInfos with ALL available military units from tech tree (only "root" units are listed).
		// Towers are ignored (not added to list). Boats are ignored on non-water maps.
		// *** Make sure to delete all PotentialUnitInfo from list when you're finished with the list ***
		void CollectPotentialUnitsInfo(AOE_STRUCTURES::STRUCT_PLAYER *player);

		// Compute all strength/weakness fields for all units in potential units list.
		void ComputeStrengthsForPotentialUnits();
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
		void RecomputeComparisonBonuses(std::list<PotentialUnitInfo*> selectedUnits, bool waterUnit, bool upgradedUnit, short int maxAgeResearchId);

		// Get the additional farm production amount for a research and underlying required researches
		float GetFarmProductionBonusRecursive(PotentialResearchInfo *resInfo, float supposedCurrentProduction) const;
		// Get the additional farm production amount for a research (not recursive)
		float GetFarmProductionBonus(short int researchId, float supposedCurrentProduction) const;
		// Get the additional farm production amount for a technology (not recursive)
		float GetFarmProductionBonusForTech(short int techId, float supposedCurrentProduction) const;

		// Compute unitInstanceScoreForOptionalResearch for remaining (not selected) optional researches
		void ComputeScoresForRemainingOptionalResearches();

		// Make the selection of units to add to strategy based on a pre-selection list.
		// waterUnits : true=deal with water units only, false=deal with land units only
		void SelectStrategyUnitsFromPreSelection(std::list<PotentialUnitInfo*> preSelectedUnits, bool waterUnits);

		// Select which units are to be added in strategy, based on potentialUnitsList
		// Requires that this->potentialUnitsList has already been filled
		// Only consider water units (waterUnits=true) or only consider land units (waterUnits=false)
		void SelectStrategyUnitsForLandOrWater(bool waterUnits);

		// Search for a unit that can be added to strategy as "optional" (without full upgrades) to fight a detected weakness.
		// waterUnits: if true, manage water units only. If false, manage land units only
		void AddOptionalUnitAgainstWeakness(MILITARY_CATEGORY weaknessCategory, bool waterUnits);

		// Select the main (military) units are to be added in strategy, chosen in potentialUnitsList
		// This only interacts with potentialUnitsList (may set isSelected=true) and feeds actuallySelectedUnits with "main" non-tower military units
		void SelectStrategyUnits();

		// age = age to take care (ignore units that are only available in later ages)
		// hasAlreadyUnits = true if provided age already has some military units
		// Returns number of added units (0/1). Added units are marked for add (and WILL actually be written in strategy).
		int AddOneMilitaryUnitForEarlyAge(short int age, bool hasAlreadyUnits);

		// Add (and mark for add) some military to selection, to fill gaps in strategy in early ages (if any)
		void AddMilitaryUnitsForEarlyAges();

		// Selects optional researches to add to strategy
		// All villagers and military units must have already been added to strategy.
		// This method only updates flags, does not actually add anything to strategy
		void ChooseOptionalResearches();

		// Collect all researches info for villagers/economy (does NOT mark them for add : optional researches)
		void AddResearchesForEconomy();

		// Add user-defined "forced" researches (always develop... if possible)
		void AddUserDefinedForcedResearches();
		// Add one user-defined "forced" researches (always develop... if possible)
		void AddUserDefinedForcedResearches(short int oneResearchId);

		// Add tower upgrades to internal objects (and mark them as priority items)
		// Does not add upgrades that slow projectiles down (ballista tower)
		// Only adds unit upgrades (sentry, watch tower) + "enable unit" (watch tower) researches, not others researches.
		// Added researches (unit upgrades) are automatically marked for add (will actually be included in strategy)
		void AddTowerResearches();

		// Handle farm building/research info/desired count
		void HandleFarmsInfo();

		/*** Strategy writing methods ***/

		// Get the very global information about strategy generation (number of villagers, etc)
		void CollectGlobalStrategyGenerationInfo(AOE_STRUCTURES::STRUCT_PLAYER *player);

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

		// Add strategy elements for towers (buildings - not researches)
		void CreateTowerBuildingsStrategyElements();

		// Add the first building of each building kind to strategy. Returns number of added buildings
		// Does not add an additional building if there is already one in strategy
		// Always add in "parent" age strategy zone.
		int CreateFirstBuildingsStrategyElements();
		// Create secondary (optional) occurrences of buildings. E.g. 2nd TC, or additional military buildings to train army faster and do researches while another building is training units
		int CreateSecondaryBuildingStrategyElements();

		// Returns a vector of all non-disabled researches that affect the provided unit definition (ID)
		// useFilter: if true, exclude researches with drawbacks for current game type (DM/RM)
		std::vector<short int> GetAllResearchesThatAffectUnit(short int unitDefId, bool useFilter);

	};

}
}
