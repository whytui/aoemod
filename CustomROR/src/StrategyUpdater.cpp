#include "../include/StrategyUpdater.h"

using namespace STRATEGY;


// Analyze strategy and fixes what's necessary. Called every <crInfo.configInfo.tacticalAIUpdateDelay> seconds.
void STRATEGY::AnalyzeStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI) {
	// Exit if AI improvement is not enabled.
	if (!CUSTOMROR::IsImproveAIEnabled(buildAI->commonAIObject.playerId)) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	bool difficultyIsEasy = (settings->difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASY); // True for easy / easiest
	if (difficultyIsEasy) { return; } // Do not improve strategy in easy levels.

	assert(buildAI != NULL);
	assert(buildAI->IsCheckSumValid());

	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeFirstStratElem = &buildAI->fakeFirstStrategyElement;
	if (fakeFirstStratElem == NULL) { return; } // Just a security
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeFirstStratElem->next;
	assert(buildAI->mainAI != NULL);
	AOE_STRUCTURES::STRUCT_PLAYER *player = buildAI->mainAI->ptrStructPlayer;
	assert(player != NULL);

	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *elemMainTownCenter = NULL; // Main town center
	int townCentersCount = 0; // Total number of town centers.
	bool foundCounter_one = false; // set to true if we passed the "counter=1" element. Allow us to identify "panic units" from normal strategy units
	int currentPopulation = 0; // Does not count limited-retrains units

	int toolAgeResearched = IsTechResearched(player, CST_RSID_TOOL_AGE);
	int bronzeAgeResearched = IsTechResearched(player, CST_RSID_BRONZE_AGE);
	int ironAgeResearched = IsTechResearched(player, CST_RSID_IRON_AGE);
	bool canGoToBronzeAge = (bronzeAgeResearched != 0); // Useful to detect strategies that do NOT have bronze (scenarios ?)
	bool canGoToIronAge = (ironAgeResearched != 0); // Useful to detect strategies that do NOT have iron (scenarios ?)
	bool foundWheel = false;
	bool foundArchitecture = false;
	bool foundWoodWorking = false, foundArtisanShip = false, foundCraftManShip = false;
	bool foundToolWorking = false;
	bool foundDomestication = false;
	//bool foundWatchTower = false, foundSentryTower = false, foundGuardTower = false, foundBallistaTower = false;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *resWatchTower = NULL, *resSentryTower = NULL, *resGuardTower = NULL, *resBallistaTower = NULL;
	bool foundBallistics = false;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *wonder = NULL;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *lastNonTowerElement = fakeFirstStratElem;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *goodDevelopmentPointElement = NULL; // Represents the location in strategy where we can assume player is strong enough to insert optional researches. Be careful, AI can skip a certain number of items (10 according to SN numbers)
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem_bronzeAge = NULL;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *customRorMaxPopBegin = GetCustomRorMaxPopulationBeginStratElem(buildAI);
	float *resources = (float *)player->ptrResourceValues;

	// Do only 1 loop on strategy and collect all necessary information.
	while (currentStratElem && currentStratElem != fakeFirstStratElem) {
		assert(currentStratElem != NULL);
		// Save some useful elements/information as we're passing on it
		if (currentStratElem->unitDAT_ID == CST_UNITID_FORUM) {
			townCentersCount++;
			if (!elemMainTownCenter) {
				elemMainTownCenter = currentStratElem;
			}
		}
		if (currentStratElem->elemId == 1) { foundCounter_one = true; }
		if ((currentStratElem->unitDAT_ID != CST_UNITID_WATCH_TOWER) || (currentStratElem->elementType != AIUCBuilding)) {
			lastNonTowerElement = currentStratElem;
		}
		if ((currentStratElem->elementType == AIUCLivingUnit) && (currentStratElem->retrains == -1)) {
			currentPopulation++;
			// minPopulationBeforeBuildOptionalItems+1 : take next one and insert BEFORE (inserting BEFORE an given element preserves insert order consistency. After reverses all).
			if (!goodDevelopmentPointElement && ironAgeResearched && (currentPopulation >= CUSTOMROR::crInfo.configInfo.minPopulationBeforeBuildOptionalItems + 1)) {
				// We store the location where player theoritically has a population=[minPopulationBeforeBuildOptionalItems] (we also ask for iron age).
				// BUT AI can skip items so this strategy element is triggered BEFORE the [minPopulationBeforeBuildOptionalItems] units are really trained (alive).
				// SO: Use a HIGHER value for the tested population amount than the value we really would like to use.
				// For example: add ~5 or ~10 to the population number we want AI to train before triggering optionals.
				goodDevelopmentPointElement = currentStratElem;
			}
		}

		// On iron age, disable limited-retrains weak units (except panic units, if any). Make sure NOT to disable villagers.
		if (ironAgeResearched && foundCounter_one && (currentStratElem->elementType == AIUCLivingUnit) && (currentStratElem->retrains > -1) &&
			(currentStratElem->inProgressCount == 0) && (!settings->isScenario)) {
			long int DATID = currentStratElem->unitDAT_ID;
			if ((DATID == CST_UNITID_SLINGER) ||
				(DATID == CST_UNITID_BOWMAN) ||
				(DATID == CST_UNITID_CLUBMAN) ||
				(DATID == CST_UNITID_SCOUT)
				) {
				currentStratElem->totalCount = 2;
				currentStratElem->retrains = 1;
				currentStratElem->aliveCount = 0; // Not really necessary...
			}
		}

		// Unblock "stuck buildings". We intentionally set unitInstanceId to -1 but keep "inprogress" when an unfinished building is destroyed.
		// The goal is to wait (for THIS update) before trying to build it again.
		// Warning: The 2 other "fixes" about destroyed buildings are absolutely required or AI will build, build and rebuild the same thing forever.
		if (CUSTOMROR::crInfo.hasFixForBuildingStratElemUnitId && (currentStratElem->inProgressCount == 1) &&
			(currentStratElem->elementType == AIUCBuilding) && (currentStratElem->unitInstanceId == -1)) {
			currentStratElem->inProgressCount = 0;
		}

		// Collect info on existing elements...
		// True if current strategy element is a research (including "critical")
		bool isTechOrCritical = (currentStratElem->elementType == AIUCTech) || (currentStratElem->elementType == AIUCCritical);

		if ((currentStratElem->unitDAT_ID == CST_RSID_WHEEL) && isTechOrCritical) {
			foundWheel = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_ARCHITECTURE) && (isTechOrCritical)) {
			foundArchitecture = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_WOOD_WORKING) && (isTechOrCritical)) {
			foundWoodWorking = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_ARTISANSHIP) && (isTechOrCritical)) {
			foundArtisanShip = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_CRAFTMANSHIP) && (isTechOrCritical)) {
			foundCraftManShip = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_TOOL_WORKING) && (isTechOrCritical)) {
			foundToolWorking = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_DOMESTICATION) && (isTechOrCritical)) {
			foundDomestication = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_WATCH_TOWER) && (isTechOrCritical)) {
			resWatchTower = currentStratElem;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_SENTRY_TOWER) && (isTechOrCritical)) {
			resSentryTower = currentStratElem;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_GUARD_TOWER) && (isTechOrCritical)) {
			resGuardTower = currentStratElem;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_BALLISTA_TOWER) && (isTechOrCritical)) {
			resBallistaTower = currentStratElem;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_BALLISTICS) && (isTechOrCritical)) {
			foundBallistics = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_BRONZE_AGE) && (isTechOrCritical)) {
			stratElem_bronzeAge = currentStratElem;
			canGoToBronzeAge = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_IRON_AGE) && (isTechOrCritical)) {
			canGoToIronAge = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_UNITID_WONDER) && (
			(currentStratElem->elementType == AIUCBuilding))
			) {
			wonder = currentStratElem;
		}

		currentStratElem = currentStratElem->next;
	}

	//AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *endElement; // the element before which to insert, for elements we want to add at the end.
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *optionalsLocation; // the element before which to insert, for optionals elements.
	if (goodDevelopmentPointElement) {
		optionalsLocation = goodDevelopmentPointElement;
	} else {
		if (customRorMaxPopBegin) { optionalsLocation = customRorMaxPopBegin; } else {
			if (wonder) { optionalsLocation = wonder; } else {
				optionalsLocation = lastNonTowerElement; // fakeFirstStratElem->previous if it could not be set during loop, so it's never NULL.
			}
		}
	}

	// Improve farms management (can be dynamically added: do not do it in initialization ! They will not exist yet)
	// TO DO

	// Add useful (available) researches when they are missing, under some conditions.
	// Note: testing if the relevant age is researched is useful for performance (stored in a boolean variable, avoids some GetResearchStatus calls)
	int numberOfAddedResearches = 0;
	bool isVeryRich = (resources[CST_RES_ORDER_FOOD] > 6000) &&
		(resources[CST_RES_ORDER_WOOD] > 6000) &&
		(resources[CST_RES_ORDER_GOLD] > 3000) &&
		(resources[CST_RES_ORDER_STONE] > 1000); // Generally corresponds to deathmatch situation.
	bool isStrong = (resources[CST_RES_ORDER_CURRENT_POPULATION] > 40) && ironAgeResearched &&
		(resources[CST_RES_ORDER_FOOD] > 800) &&
		(resources[CST_RES_ORDER_WOOD] > 700) &&
		(resources[CST_RES_ORDER_GOLD] > 500); // Generally corresponds to a random map player that is well developed, with very high population and a minimum resources amount.

	// TO DO: get count of "customROR-added" researches that are NOT done, to limit the number of pending additions ?

	// Wheel & architecture: very important researches !
	// We always add wheel if available in tech tree (and if strategy goes to bronze, at least).
	if (!foundWheel && canGoToBronzeAge && IsResearchRelevantForStrategy(player, CST_RSID_WHEEL)) {
		//AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_WHEEL, player, "customROR-Wheel");
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *tmpElemForWheel = stratElem_bronzeAge;
		// get element AFTER bronze to insert before it
		// Indeed, to add researches, we must use "AddUnitInStrategy_before" method, no "AddUnitInStrategy".
		if (tmpElemForWheel) { tmpElemForWheel = tmpElemForWheel->next; }
		if (!tmpElemForWheel) { // if not found, use our good old "optionals" location.
			tmpElemForWheel = optionalsLocation;
		}
		AddUnitInStrategy_before(buildAI, tmpElemForWheel, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_WHEEL, player, "customROR-Wheel");
		numberOfAddedResearches++;
		std::string msg = "Added wheel in strategy for player #";
		msg += to_string(player->playerId);
		traceMessageHandler.WriteMessageNoNotification(msg);
		// Make sure we added this after market
		//this->MoveStrategyElement_after_ifWrongOrder(buildAI, market, optionalsLocation->previous);
	}
	if (!foundArchitecture && /*canGoToBronzeAge &&*/ bronzeAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_ARCHITECTURE)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GOVERNMENT_CENTER, AIUCTech, CST_RSID_ARCHITECTURE, player, "customROR-Architecture");
		numberOfAddedResearches++;
		//this->MoveStrategyElement_after_ifWrongOrder(buildAI, gov_siege, optionalsLocation->previous);
	}
	// Towers: note: only one of them can be "available to be researched" at once, so this can only add one of them during this tactical update.
	if (!resWatchTower && toolAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_WATCH_TOWER)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GRANARY, AIUCTech, CST_RSID_WATCH_TOWER, player, "customROR-WatchTower");
		numberOfAddedResearches++;
	}
	if (!resSentryTower && bronzeAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_SENTRY_TOWER)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GRANARY, AIUCTech, CST_RSID_SENTRY_TOWER, player, "customROR-SentryTower");
		numberOfAddedResearches++;
		// Make sure new item is after watch tower tech
		MoveStrategyElement_after_ifWrongOrder(buildAI, resWatchTower, optionalsLocation->previous);
	}
	// if (CUSTOMROR::crInfo.configInfo.improveAILevel <= ...) or IsImproveAIEnabled(player->playerId)
	// then add only at a certain level of AI improvement ? (if implemented some day)
	if (!resGuardTower && ironAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_GUARD_TOWER)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GRANARY, AIUCTech, CST_RSID_GUARD_TOWER, player, "customROR-GuardTower");
		numberOfAddedResearches++;
		// Make sure new item is after sentry tower tech
		MoveStrategyElement_after_ifWrongOrder(buildAI, resSentryTower, optionalsLocation->previous);
	}
	// Ballista tower: only add in deathmatch situation
	if (!resBallistaTower && ironAgeResearched && isVeryRich && IsResearchRelevantForStrategy(player, CST_RSID_BALLISTA_TOWER) &&
		(resources[CST_RES_ORDER_STONE] > 2500)) {
		bool canAdd = true;
		// Make sure requirement is ok - add it if needed.
		if (!foundBallistics) {
			AOE_CONST_FUNC::RESEARCH_STATUSES rs = GetResearchStatus(player, CST_RSID_BALLISTICS);
			if ((rs == RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) ||
				(rs == RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE)) {
				AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GOVERNMENT_CENTER, AIUCTech, CST_RSID_BALLISTICS, player, "customROR-Ballistics");
				numberOfAddedResearches++;
			}
			if (rs == RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) { canAdd = false; }
		}
		if (canAdd) {
			AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GRANARY, AIUCTech, CST_RSID_BALLISTA_TOWER, player, "customROR-BallistaTower");
			numberOfAddedResearches++;
			// Make sure new item it after guard tower tech
			MoveStrategyElement_after_ifWrongOrder(buildAI, resGuardTower, optionalsLocation->previous);
		}
	}

	// The +1 range bonuses: note: only one of them can be "available to be researched" at once, so this can only add one of them during this tactical update.
	// Of course, next time this method is called, there's a chance next one can be added too.
	if (!foundWoodWorking && toolAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_WOOD_WORKING)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_WOOD_WORKING, player, "customROR-WoodWorking");
		numberOfAddedResearches++;
	}
	if (!foundArtisanShip && bronzeAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_ARTISANSHIP)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_ARTISANSHIP, player, "customROR-Artisanship");
		numberOfAddedResearches++;
	}
	if (!foundCraftManShip && ironAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_CRAFTMANSHIP)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_CRAFTMANSHIP, player, "customROR-Craftmanship");
		numberOfAddedResearches++;
	}
	// Some low-level cheap researches that are almost always useful: 1st farm improvement, 1st +2 attack bonus.
	// Limit to 2 added researches max (total) if we are using the condition "isStrong".
	if (!foundToolWorking && toolAgeResearched && IsResearchRelevantForStrategy(player, CST_RSID_TOOL_WORKING) &&
		(((numberOfAddedResearches < 2) && isStrong) || isVeryRich)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_STORAGE_PIT, AIUCTech, CST_RSID_TOOL_WORKING, player, "customROR-ToolWorking");
		numberOfAddedResearches++;
	}
	if (!foundDomestication && toolAgeResearched && IsResearchRelevantForStrategy(player, CST_RSID_DOMESTICATION) &&
		(((numberOfAddedResearches < 2) && isStrong) || isVeryRich)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_DOMESTICATION, player, "customROR-Domestication");
		numberOfAddedResearches++;
	}
	// Add a secondary town center when there is only one. It's a good security.
	if ((townCentersCount == 1) && (settings->difficultyLevel <= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_HARD)) { // Hard/hardest only
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, -1, AIUCBuilding, CST_UNITID_FORUM, player, "customROR-BackupTC");
		townCentersCount++;
	}
}
