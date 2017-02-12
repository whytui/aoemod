#include "../include/StrategyUpdater.h"



// Common function for panic mode unit searching.
// Returns true if it is possible to train the unit. In such case, cost is decreased from remainingResources and actorCounter is decreased too.
// Returns false and does not change values if it is not possible (warning: tempCost can be modified in all cases)
bool PrepareUnitToAddIfPossible(AOE_STRUCTURES::STRUCT_PLAYER *player, short int unitId_toAdd, short int unitId_actor,
	long int *actorCounter, short int *lastCostDAT_ID, float remainingResources[], float tempCost[]) {
	// Check costs. If last loop already calculated the cost for the same DAT_ID, re-use tempCost. Otherwise, recompute it.
	bool getCostIsOK = (*lastCostDAT_ID == unitId_toAdd);
	if (!getCostIsOK) {
		getCostIsOK = GetUnitCost(player, unitId_toAdd, tempCost);
		if (!getCostIsOK) { *lastCostDAT_ID = -1; } // tempCost may have been modified, do NOT try to reuse it later !!!
		else { *lastCostDAT_ID = unitId_toAdd; }
	}
	if (getCostIsOK && ApplyCostIfPossible(tempCost, remainingResources)) {
		(*actorCounter)--; // Consider 1 as "used"
		*lastCostDAT_ID = unitId_toAdd;
		return true;
	}
	return false;
}



namespace STRATEGY {
;

// Static common object. Index is playerId
StrategyUpdater strategyUpdater[9];



// This is called from buildAI.resetStratElemForUnitId, when testing is "elem.alive" field.
// The fixes in this method are "technical" and are always applied (even if improve AI is disabled)
void CheckStratElemAliveForReset(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *currentElement) {
	if (!buildAI || !buildAI->IsCheckSumValid() || !currentElement || !currentElement->IsCheckSumValid()) {
		return;
	}
	// Manage the case when "in-progress" building is destroyed: do not reset it immediately because it would be re-triggered, destroyed again by same enemy (while HP=1) and so on
	// We just reset unitInstanceId to -1 to show that the unit no longer exists. Next strategy update will detect it and reset it "asynchronously".
	if ((currentElement->inProgressCount > 0) && (currentElement->elementType == AIUCBuilding)) {
		assert(buildAI->mainAI && buildAI->mainAI->IsCheckSumValid() && buildAI->mainAI->player && buildAI->mainAI->player->IsCheckSumValid());
		if (!buildAI->mainAI || !buildAI->mainAI->player) { return; } // error case
		STRUCT_GAME_GLOBAL *global = buildAI->mainAI->player->ptrGlobalStruct;
		strategyUpdater[buildAI->commonAIObject.playerId].AddLastInProgressFailureInfoForStratElem(currentElement->elemId, global->currentGameTime);
		currentElement->unitInstanceId = -1;
		if (currentElement->unitDAT_ID == CST_UNITID_WONDER) { // Fix a bug with wonders, by the way
			assert(buildAI->mainAI && buildAI->mainAI->IsCheckSumValid());
			if ((buildAI->mainAI) && (buildAI->mainAI->IsCheckSumValid())) {
				// Make sure "wonder is being built flag" is reset;
				buildAI->mainAI->structTacAI.myWonderBeingBuiltFlag = 0;
			}
		}
	}
}


// Analyze strategy and fixes what's necessary. Called every <crInfo.configInfo.tacticalAIUpdateDelay> seconds.
void AnalyzeStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI) {
	// Exit if AI improvement is not enabled.
	if (!CUSTOMROR::IsImproveAIEnabled(buildAI->commonAIObject.playerId)) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	bool difficultyIsEasy = (settings->rgeGameOptions.difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASY); // True for easy / easiest
	if (difficultyIsEasy) { return; } // Do not improve strategy in easy levels.

	assert(buildAI != NULL);
	assert(buildAI->IsCheckSumValid());

	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeFirstStratElem = &buildAI->fakeFirstStrategyElement;
	if (fakeFirstStratElem == NULL) { return; } // Just a security
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeFirstStratElem->next;
	assert(buildAI->mainAI != NULL);
	AOE_STRUCTURES::STRUCT_PLAYER *player = buildAI->mainAI->ptrStructPlayer;
	assert(player != NULL);
	assert((player->playerId >= 0) && (player->playerId < 9));
	if (!player || (player < 0) || (player->playerId >= 9)) { return; }

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
			(currentStratElem->inProgressCount == 0) && (!settings->rgeGameOptions.isScenario)) {
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
			long int lastDestructionTime = strategyUpdater[player->playerId].GetLastInProgressFailureGameTimeForBldStratElem(currentStratElem->elemId);
			if ((lastDestructionTime < 0) ||
				(player->ptrGlobalStruct->currentGameTime - lastDestructionTime > CUSTOM_AI::AI_CONST::delayToRetryFailedConstruction)) {
				currentStratElem->inProgressCount = 0;
			} // else: not waited enough
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
	if ((townCentersCount == 1) && (settings->rgeGameOptions.difficultyLevel <= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_HARD)) { // Hard/hardest only
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, -1, AIUCBuilding, CST_UNITID_FORUM, player, "customROR-BackupTC");
		townCentersCount++;
	}
}



// Manage strategy updates for panic mode (AI player is being attacked and seems not to have enough defences).
// This method is only called if an attack occurs in (near) "my" town, and if "panic mode delay" has passed since last execution.
// This strategy is not supposed to impact combat or anything else than strategy (at least in standard game !)
void STRATEGY::ManagePanicMode(AOE_STRUCTURES::STRUCT_AI *mainAI, long int enemyPlayerId, CUSTOM_AI::CustomAIMilitaryInfo *militaryAIInfo) {
	// CONFIG
	long int maxPanicModeUnitsInStrategy = CUSTOMROR::crInfo.configInfo.maxPanicUnitsCountToAddInStrategy;
	long int maxPanicModeSeekEnemyDistance = CUSTOM_AI::AI_CONST::townSize; // Hardcoded in original code to 20 (0x14)
	const long int maxPanicModeSeekEnemyDistanceIfFewResources = CUSTOM_AI::AI_CONST::townDefendSizeIfWeak; // Defend a smaller territory if weak
#define PANIC_MODE_ARRAYS_MAX_SIZE 20 // technical limit to the number of panic mode units
	if (maxPanicModeUnitsInStrategy > PANIC_MODE_ARRAYS_MAX_SIZE) { maxPanicModeUnitsInStrategy = PANIC_MODE_ARRAYS_MAX_SIZE; }

	// Collect context information/variables
	assert(mainAI != NULL);
	assert(mainAI->IsCheckSumValid());
	if (!mainAI || (!mainAI->IsCheckSumValid())) { return; }

	// Other variables
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = &mainAI->structBuildAI;
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = &mainAI->structInfAI;
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = &mainAI->structTacAI;
	assert(buildAI->IsCheckSumValid());
	assert(infAI->IsCheckSumValid());
	assert(tacAI->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *player = mainAI->ptrStructPlayer;
	assert(player != NULL);
	assert(player->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	char myCivId = player->civilizationId;
	long int myMilitaryUnitsCount = tacAI->landMilitaryUnits.usedElements; // does not count villagers, nor towers - nor boats

	assert((enemyPlayerId >= 0) && (enemyPlayerId < 9));
	AOE_STRUCTURES::STRUCT_PLAYER *enemyPlayer = GetPlayerStruct((short int)enemyPlayerId);
	assert(enemyPlayer != NULL);
	assert(enemyPlayer->IsCheckSumValid());
	if (!enemyPlayer) { return; }
	char enemyCivId = enemyPlayer->civilizationId;


	AOE_STRUCTURES::STRUCT_UNIT_BASE *forumUnitStruct;
	forumUnitStruct = AOE_METHODS::PLAYER::AOE_MainAI_findUnit(mainAI, CST_UNITID_FORUM);
	if (forumUnitStruct == NULL) { return; } // TO DO: make special treatments if no town center (maybe we still have other buildings... Especially temple!)
	assert(forumUnitStruct->IsCheckSumValidForAUnitClass());
	// Add "fake" enemys "priestVictims" so that we create units to defend what's left and possibly convert a villager ! Use fake position ?
	long int forumPosX = (long int)forumUnitStruct->positionX;
	long int forumPosY = (long int)forumUnitStruct->positionY;

	// My resources
	float remainingResources[MAX_RESOURCE_TYPE_ID];
	remainingResources[CST_RES_ORDER_FOOD] = player->GetResourceValue(CST_RES_ORDER_FOOD); // *(float *)(ptrResources + 4 * CST_RES_ORDER_FOOD);
	remainingResources[CST_RES_ORDER_WOOD] = player->GetResourceValue(CST_RES_ORDER_WOOD); //*(float *)(ptrResources + 4 * CST_RES_ORDER_WOOD);
	remainingResources[CST_RES_ORDER_STONE] = player->GetResourceValue(CST_RES_ORDER_STONE); //*(float *)(ptrResources + 4 * CST_RES_ORDER_STONE);
	remainingResources[CST_RES_ORDER_GOLD] = player->GetResourceValue(CST_RES_ORDER_GOLD); //*(float *)(ptrResources + 4 * CST_RES_ORDER_GOLD);
	remainingResources[CST_RES_ORDER_POPULATION_HEADROOM] = player->GetResourceValue(CST_RES_ORDER_POPULATION_HEADROOM); //*(float *)(ptrResources + 0x10); // free population left in houses
	// Keep some necessary basic resources left to create villagers/forum if needed
	remainingResources[CST_RES_ORDER_FOOD] -= 150;
	remainingResources[CST_RES_ORDER_WOOD] -= 200;
	if (remainingResources[CST_RES_ORDER_POPULATION_HEADROOM] <= 0) { return; } // No room for new units
	// We don't use pop_headroom anymore at this point. We could use it to limit the number of added units.
	if ((remainingResources[CST_RES_ORDER_FOOD] < 40) && (remainingResources[CST_RES_ORDER_WOOD] < 20) &&
		(remainingResources[CST_RES_ORDER_GOLD] < 15)) {
		return;
	} // Out of resources: exit now. Optional check.

	// Evaluate player weakness (0=weak, 100=ok)
	int myWeaknessScore = (militaryAIInfo == NULL) ? 50 : militaryAIInfo->EvaluatePlayerWeakness(player);
	float tempCost[MAX_RESOURCE_TYPE_ID + 1];
	short int lastCostDAT_ID = -1; // DAT ID to which last computed cost corresponds to.
	for (int i = 0; i < 4; i++) { tempCost[i] = 0; }

	long int totalEnemyCount = 0; // Total count of enemy units in my town
	long int enemyPriestCount = 0;
	long int enemyCamelVictimsCount = 0; // Units I can kill with camels: chariots, cavalry, horse archers
	long int enemySlingerVictimsCount = 0; // All archers but elephant (they are slow, they can't make my units run after them)
	long int enemySiegeWeaponsCount = 0;
	long int enemyPriestVictimsCount = 0; // Easily convertible units: slow units like elephants or possibly hoplites
	long int enemyChariotsCount = 0;
	long int enemyTowersCount = 0;
	// TODO: here are hardcoded buildings. Try to make something more generic.
	long int myBarracksCount = 0;
	long int myRangeCount = 0;
	long int myStableCount = 0;
	long int myTempleCount = 0;
	long int mySiegeWorkshopCount = 0;
	long int myAcademyCount = 0;
	long int myDockCount = 0;
	bool hasBoats = false;
	// If SNMaximumTownSize has a reasonable value, use it instead of hardcoded value.
	if ((tacAI->SNNumber[SNMaximumTownSize] > maxPanicModeSeekEnemyDistanceIfFewResources) &&
		(tacAI->SNNumber[SNMaximumTownSize] < maxPanicModeSeekEnemyDistance + 4)) {
		maxPanicModeSeekEnemyDistance = tacAI->SNNumber[SNMaximumTownSize];
	}
	long int square_maxPanicModeSeekEnemyDistance;
	if ((myWeaknessScore < 33) || (settings->rgeGameOptions.difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASY)) {
		// Defend a restricted territory if weak OR if level is easy/easiest.
		square_maxPanicModeSeekEnemyDistance = maxPanicModeSeekEnemyDistanceIfFewResources * maxPanicModeSeekEnemyDistanceIfFewResources;
	} else {
		if (myWeaknessScore < 66) {
			assert(maxPanicModeSeekEnemyDistance > maxPanicModeSeekEnemyDistanceIfFewResources);
			int intermediateValue = maxPanicModeSeekEnemyDistanceIfFewResources + ((maxPanicModeSeekEnemyDistance - maxPanicModeSeekEnemyDistanceIfFewResources) / 2);
			square_maxPanicModeSeekEnemyDistance = intermediateValue * intermediateValue;
		} else {
			square_maxPanicModeSeekEnemyDistance = maxPanicModeSeekEnemyDistance * maxPanicModeSeekEnemyDistance;
		}
	}
	long int unitListElementCount = infAI->unitElemListSize;
	long int unitListBase = (long int)infAI->unitElemList;
	assert((unitListElementCount == 0) || (infAI->unitElemList != NULL));
	// Collect info about ATTACKER's units in my town. Note: the unit list we use may not be up to date depending on "my" exploration and what happened to involved units
	// In standard game, infAI->unitElemList contains many obsolete information (never cleaned up !). This is improved in customROR.
	for (int i = 0; i < unitListElementCount; i++) {
		// We could count "my" buildings from this list but it is incomplete ! Original game searches there but it's bad. We will search directly in player.building_list list.
		if (infAI->unitElemList[i].playerId == enemyPlayerId) {
			// Count enemy units in my town
			long int diffX = (forumPosX - infAI->unitElemList[i].posX);
			long int diffY = (forumPosY - infAI->unitElemList[i].posY);
			if ((diffX * diffX) + (diffY * diffY) <= square_maxPanicModeSeekEnemyDistance) {
				totalEnemyCount++;
				if (infAI->unitElemList[i].unitDATID == CST_UNITID_PRIEST) { enemyPriestCount++; }
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_CAVALRY) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HEAVY_CAVALRY) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CATAPHRACT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HORSE_ARCHER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HEAVY_HORSE_ARCHER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CHARIOT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CHARIOT_ARCHER)) {
					enemyCamelVictimsCount++;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_SCOUT_SHIP) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_WAR_GALLEY) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_TRIREME) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CATAPULT_TRIREME) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_JUGGERNAUGHT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_FIRE_GALLEY)) {
					hasBoats = true;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_BOWMAN) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_IMPROVED_BOWMAN) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_COMPOSITE_BOWMAN) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CHARIOT_ARCHER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HORSE_ARCHER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HEAVY_HORSE_ARCHER) // ||
					// Remove elephant archer ? His slowness makes him quite different for defence strategy (he won't make my slow units run after him)
					//(infAI->unitElemList[i].unitDATID == CST_UNITID_ELEPHANT_ARCHER)
					) {
					enemySlingerVictimsCount++;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_BALLISTA) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HELEPOLIS) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_STONE_THROWER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CATAPULT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HEAVY_CATAPULT)) {
					enemySiegeWeaponsCount++;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_ELEPHANT_ARCHER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_WAR_ELEPHANT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_ARMORED_ELEPHANT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HOPLITE) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_PHALANX) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CENTURION)) {
					enemyPriestVictimsCount++;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_HEAVY_CHARIOT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CHARIOT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CHARIOT_ARCHER)) {
					enemyChariotsCount++;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_WATCH_TOWER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_SENTRY_TOWER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_GUARD_TOWER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_BALLISTA_TOWER)) {
					enemyTowersCount++;
				}
			}
		}
	}
	// Get my buildings list: quite efficient because we have a dedicated list (only buildings) at player level (does not rely on AI structures)
	assert(player->ptrBuildingsListHeader != NULL);
	long int buildingList_count = 0;
	AOE_STRUCTURES::STRUCT_UNIT_BASE **buildingsList = NULL;
	if (player->ptrBuildingsListHeader) {
		buildingList_count = player->ptrBuildingsListHeader->buildingsArrayElemCount;
		buildingsList = player->ptrBuildingsListHeader->ptrBuildingsArray;
	}
	assert((buildingList_count == 0) || (buildingsList != NULL));
	for (int i = 0; i < buildingList_count; i++) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *loopUnit = NULL;
		assert(player->ptrBuildingsListHeader != NULL);
		loopUnit = buildingsList[i];
		if (loopUnit != NULL) {
			// We are running so much asserts because player->ptrBuildingsListHeader sometimes contain invalid data !
			// It happens because of buildings conversion bug
			assert(loopUnit->IsCheckSumValidForAUnitClass());
			assert(GetUnitStruct(player->ptrBuildingsListHeader->ptrBuildingsUnitIDs[i]) == loopUnit);
			assert(GetUnitStruct(loopUnit->unitInstanceId) == loopUnit);
			assert(loopUnit->unitType == GUT_BUILDING);
			assert(loopUnit->unitDefinition != NULL);
			if ((loopUnit->IsCheckSumValidForAUnitClass()) && (GetUnitStruct(player->ptrBuildingsListHeader->ptrBuildingsUnitIDs[i]) == loopUnit) &&
				(loopUnit->unitType == GUT_BUILDING) && (loopUnit->unitDefinition != NULL)) {
				if ((loopUnit->unitStatus == 2) && (loopUnit->unitDefinition != NULL)) {
					short int DAT_ID = loopUnit->unitDefinition->DAT_ID1;
					if (DAT_ID == CST_UNITID_ACADEMY) { myAcademyCount++; }
					if (DAT_ID == CST_UNITID_BARRACKS) { myBarracksCount++; }
					if (DAT_ID == CST_UNITID_DOCK) { myDockCount++; }
					if (DAT_ID == CST_UNITID_RANGE) { myRangeCount++; }
					if (DAT_ID == CST_UNITID_STABLE) { myStableCount++; }
					if (DAT_ID == CST_UNITID_SIEGE_WORKSHOP) { mySiegeWorkshopCount++; }
					if (DAT_ID == CST_UNITID_TEMPLE) { myTempleCount++; }
				}
			} else {
				// For debug - or just in case...
				// THIS CODE is NO LONGER needed since I fixed the building conversion bug.
				AOE_STRUCTURES::STRUCT_UNIT_BASE *invalidUnit = loopUnit;
				player->ptrBuildingsListHeader->ptrBuildingsArray[i] = NULL; // Remove invalid entry from list, it could cause a crash when some code accesses it.
				char buffer[300];
				sprintf_s(buffer, "ERROR: invalid unit list entry for player #%ld. ptr=%08lX. unitId=%ld GUT=%d, index=%d/%ld.\n",
					player->playerId, (long int)invalidUnit, invalidUnit->unitInstanceId, (int)invalidUnit->unitType, i, buildingList_count);
				//AddTraceToFile(buffer);
				traceMessageHandler.WriteMessage(buffer);
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *invalidUnitDef = invalidUnit->unitDefinition;
				long checksum = ((long)invalidUnitDef < 0x10000) ? -1 : invalidUnitDef->checksum; // if invalidUnitDef ptr is low it is probably invalid, don't access it
				int tmp_DAT_ID = -1;
				if (checksum == 0x00549930) { // checksum is correct (building unitDef)
					tmp_DAT_ID = invalidUnitDef->DAT_ID1;
				}
				sprintf_s(buffer, "unitDef=%08lX checksum=%08lX, DAT_ID=%d\n", (long)invalidUnitDef, checksum, tmp_DAT_ID);
				//AddTraceToFile(buffer); // Do it separately in case we have a crash because of unitDef (even if we try to be careful, we can't detect invalid pointers with 100% confidence)
				traceMessageHandler.WriteMessage(buffer);

				//sprintf_s(buffer, "An error occurred. CustomROR could prevent game crash. See error log file on c:\\ (or C:\\Users\\...\\AppData\\Local\\VirtualStore) and report to developer.");
				traceMessageHandler.WriteMessage("An error occurred. CustomROR could prevent game crash. Please report to developer.");
				//this->OpenCustomDialogMessage(buffer, 450, 250);
			}
		}
	}

	if (!myAcademyCount && !myBarracksCount && !myRangeCount && !mySiegeWorkshopCount && !myStableCount && !myTempleCount) {
		return; // No building, no need to continue
	}
	// For large enemy armies, ignore units when their number is very low (example: do not impact choices because of 1 single cavalry).
	if (totalEnemyCount > 6) {
		if (enemyCamelVictimsCount * 100 / totalEnemyCount < 16) { enemyCamelVictimsCount = 0; }
		if (enemySiegeWeaponsCount * 100 / totalEnemyCount < 16) { enemySiegeWeaponsCount = 0; }
		if (enemyChariotsCount * 100 / totalEnemyCount < 20) { enemyChariotsCount = 0; }
		if (enemyPriestVictimsCount * 100 / totalEnemyCount < 16) { enemyPriestVictimsCount = 0; }
		if (enemySlingerVictimsCount * 100 / totalEnemyCount < 16) { enemySlingerVictimsCount = 0; }
		// Do NOT update enemyPriestCount. Always take care of priest, it is too important, even alone
		// Same for towers
	}

	// Basic units (where DATID1=DATID2)
	int canTrainSlinger = IsUnitAvailableForPlayer(CST_UNITID_SLINGER, player);
	int canTrainClubman = IsUnitAvailableForPlayer(CST_UNITID_CLUBMAN, player);
	// + axe
	int canTrainSwordsman = IsUnitAvailableForPlayer(CST_UNITID_SHORT_SWORDSMAN, player); //+others
	// broad, long sword
	int canTrainScout = IsUnitAvailableForPlayer(CST_UNITID_SCOUT, player);
	int canTrainCavalry = IsUnitAvailableForPlayer(CST_UNITID_CAVALRY, player);
	int canTrainChariot = IsUnitAvailableForPlayer(CST_UNITID_CHARIOT, player);
	int canTrainCamel = IsUnitAvailableForPlayer(CST_UNITID_CAMEL, player);
	int canTrainWarElephant = IsUnitAvailableForPlayer(CST_UNITID_WAR_ELEPHANT, player);
	int canTrainBowman = IsUnitAvailableForPlayer(CST_UNITID_BOWMAN, player);
	int canTrainImprovedBowman = IsUnitAvailableForPlayer(CST_UNITID_IMPROVED_BOWMAN, player);
	// + composite
	int canTrainChariotArcher = IsUnitAvailableForPlayer(CST_UNITID_CHARIOT_ARCHER, player);
	int canTrainHorseArcher = IsUnitAvailableForPlayer(CST_UNITID_HORSE_ARCHER, player);
	int canTrainElephantArcher = IsUnitAvailableForPlayer(CST_UNITID_ELEPHANT_ARCHER, player);
	int canTrainPriest = IsUnitAvailableForPlayer(CST_UNITID_PRIEST, player);
	int canTrainHoplite = IsUnitAvailableForPlayer(CST_UNITID_HOPLITE, player);
	int canTrainBallista = IsUnitAvailableForPlayer(CST_UNITID_BALLISTA, player);
	int canTrainStoneThrower = IsUnitAvailableForPlayer(CST_UNITID_STONE_THROWER, player);
	// Fire galley, scout ship...

	// Upgraded units (DATID1 <> DATID2) => do not search in unit_def array but search if tech has been researched
	// The canTrainxxx && clause is useful for performance, we check the prerequisite first as we already have this information
	int canTrainLegion = canTrainSwordsman && IsTechResearched(player, CST_RSID_LEGION);
	int canTrainHeavyCavalry = canTrainCavalry && IsTechResearched(player, CST_RSID_HEAVY_CAVALRY);
	int canTrainCataphract = canTrainHeavyCavalry && IsTechResearched(player, CST_RSID_CATAPHRACT);
	int canTrainHeavyChariot = canTrainChariot && IsTechResearched(player, CST_RSID_HEAVY_CHARIOT);
	int canTrainArmoredElephant = canTrainWarElephant && IsTechResearched(player, CST_RSID_ARMORED_ELEPHANT);
	int canTrainPhalanx = canTrainHoplite && IsTechResearched(player, CST_RSID_PHALANX);
	int canTrainCenturion = canTrainPhalanx && IsTechResearched(player, CST_RSID_CENTURION);
	int canTrainHelepolis = canTrainBallista && IsTechResearched(player, CST_RSID_HELEPOLIS);
	int canTrainCatapult = canTrainStoneThrower && IsTechResearched(player, CST_RSID_CATAPULT);
	int canTrainHeavyCatapult = canTrainCatapult && IsTechResearched(player, CST_RSID_HEAVY_CATAPULT);
	int canTrainHeavyHorseArcher = canTrainHorseArcher && IsTechResearched(player, CST_RSID_HEAVY_HORSE_ARCHER);

	// Other info about researches
	int hasAristocracy = IsTechResearched(player, CST_RSID_ARISTOCRACY);

	// Collect info on current strategy status (panic mode elements, if any).
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *currentStratElem = &buildAI->fakeFirstStrategyElement;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *firstStratElem = currentStratElem;
	bool go_on = true;
	long int inProgressPanicUnitCount = 0;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *panicStratElemToReuse[PANIC_MODE_ARRAYS_MAX_SIZE];
	long int panicStratElemToReuseCount = 0;
	for (int i = 0; i < PANIC_MODE_ARRAYS_MAX_SIZE; i++) { panicStratElemToReuse[i] = NULL; }
	// Loop only on the beginning of strategy. Stop at "counter=1" or when meeting a "setGather..." (panic mode units always are before those).
	while ((currentStratElem != NULL) && (go_on)) {
		currentStratElem = currentStratElem->next;
		// If we get back to fake first element, we looped on all elements (should not happen unless the strategy is empty)
		if ((currentStratElem->elementType == TAIUnitClass::AIUCStrategyCmd) ||
			(currentStratElem->elemId == 1) || (currentStratElem == firstStratElem)) {
			go_on = false;
		}

		bool isPanicModeElement = false;
		if ((currentStratElem->retrains == 1) && // retrainable element
			((currentStratElem->elementType == TAIUnitClass::AIUCLivingUnit) || // We could add a filter to exclude villagers here
			((currentStratElem->elementType == TAIUnitClass::AIUCBuilding) && (currentStratElem->unitDAT_ID == CST_UNITID_WATCH_TOWER)))
			) { // living OR tower
			isPanicModeElement = true;
		}

		// Test if it is an obsolete panic mode element (retrains=1, already trained... and dead).
		if (isPanicModeElement) {
			//if ((elemInProgress == 0) && (elemAliveCount == 0) && (elemTotalCount > 0)) {
			if (currentStratElem->inProgressCount == 0) {
				// Can reuse this one (or update it, if not trained yet).
				if (panicStratElemToReuseCount < PANIC_MODE_ARRAYS_MAX_SIZE) {
					panicStratElemToReuse[panicStratElemToReuseCount] = currentStratElem;
					panicStratElemToReuseCount++;
				}
			} else {
				short int actorUnitDefId = (short int)currentStratElem->actor;
				inProgressPanicUnitCount++; // Do NOT modify this strategy element. It is being trained.
				// This also means that a building is currently used to train the unit: remove it from our available buildings list
				if (actorUnitDefId == CST_UNITID_BARRACKS) { myBarracksCount--; }
				if (actorUnitDefId == CST_UNITID_RANGE) { myRangeCount--; }
				if (actorUnitDefId == CST_UNITID_STABLE) { myStableCount--; }
				if (actorUnitDefId == CST_UNITID_DOCK) { myDockCount--; }
				if (actorUnitDefId == CST_UNITID_TEMPLE) { myTempleCount--; }
				if (actorUnitDefId == CST_UNITID_SIEGE_WORKSHOP) { mySiegeWorkshopCount--; }
				if (actorUnitDefId == CST_UNITID_ACADEMY) { myAcademyCount--; }
			}
		}
	}
	if (inProgressPanicUnitCount >= maxPanicModeUnitsInStrategy) { return; } // panic mode unit count in strategy is already at maximum

	bool strategyUpdated = false; // Set to true if we add something in strategy
	long int totalPanicModeElemCount = inProgressPanicUnitCount;
	long int unitToCreateCount = maxPanicModeUnitsInStrategy; // <= 0 means NO panic mode. Max value is maxPanicModeUnitsInStrategy.
	if (totalEnemyCount - myMilitaryUnitsCount < maxPanicModeUnitsInStrategy) {
		unitToCreateCount = totalEnemyCount - myMilitaryUnitsCount;
	}

	if (unitToCreateCount > 0) {
		if (militaryAIInfo && player->ptrGlobalStruct && player->ptrGlobalStruct->IsCheckSumValid()) {
			militaryAIInfo->SavePanicModeOccurrenceInHistory(enemyPlayerId, player->ptrGlobalStruct->currentGameTime);
		}
	}

	// Main loop to insert panic units one by one
	while (totalPanicModeElemCount < unitToCreateCount) {
		long int reuseIndex = totalPanicModeElemCount - inProgressPanicUnitCount;
		short int unitId_toAdd = -1;
		short int unitId_actor = -1;
		bool found = false;

		// 1 : enemy has priests. Create as much chariots as possible
		if (enemyPriestCount > 0) {
			// 1a (scythe) chariot
			if ((!found) && canTrainChariot && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CHARIOT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// 1b : chariot archer
			if ((!found) && (canTrainChariotArcher) && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_CHARIOT_ARCHER;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
		}

		// TODO insert 2 ? If any kind of tower, siege, armored ele, slinger, infantry... avoid priest.
		// if (!found && (enemyTowersCount > 0))

		// 2 : Super units - but avoid heavy catapults ?
		// Warning: in strategy, use DATAID1 (base unit), not DATID2 (upgraded unit)
		if (!found) {
			if (!found && canTrainCenturion && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainLegion && (myBarracksCount > 0)) {
				unitId_toAdd = CST_UNITID_SHORT_SWORDSMAN;
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// For civs that have both armored elephants and cataphracts (and/or heavy chariots), try elephant before cataphract (if no priest, and not too much range units)
			// Elephant is stronger against infantry
			if (!found && canTrainArmoredElephant && (enemyPriestCount == 0) && (myStableCount > 0) &&
				((enemySlingerVictimsCount + enemySiegeWeaponsCount) * 100 / totalEnemyCount < 30) &&
				((myCivId == CST_CIVID_PERSIAN) || (myCivId == CST_CIVID_PALMYRA) || (myCivId == CST_CIVID_HITTITE) || (myCivId == CST_CIVID_MACEDONIAN))) {
				unitId_toAdd = CST_UNITID_WAR_ELEPHANT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainCataphract && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CAVALRY;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainHeavyChariot && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CHARIOT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainHelepolis && (mySiegeWorkshopCount > 0)) {
				unitId_toAdd = CST_UNITID_BALLISTA;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Avoid training armored elephants when enemy priests are up there. We better train anything else, even a scout
			if (!found && canTrainArmoredElephant && (enemyPriestCount == 0) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_WAR_ELEPHANT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainHeavyHorseArcher && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_HORSE_ARCHER;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// If I have a temple, I may suppose I am a "good-priest" civ, at least I have priests in my normal strategy
			// This is not perfectly correct: Yamato just builds a temple in DM as requirement to upgrade to iron.
			// Also, in RM, priest researches may not be available/researched yet
			// That's why we must NOT place this in top priority in "super units" section
			// Also, avoid this if there is too much chariots in enemy's army
			if (!found && canTrainPriest && (myTempleCount > 0) && (enemyCivId != CST_CIVID_MACEDONIAN)) {
				if (!((enemyChariotsCount > 3) || // More than 3 chariots: always consider it is too much
					((enemyChariotsCount > 0) && (totalEnemyCount < 5)) || // only a few enemy units, including chariot: do NOT train priests
					((enemyChariotsCount * 100) / totalEnemyCount > 20) // More than 20% of enemy units are chariots: do NOT train priests
					)) {
					unitId_toAdd = CST_UNITID_PRIEST;
					unitId_actor = CST_UNITID_TEMPLE;
					found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myTempleCount, &lastCostDAT_ID, remainingResources, tempCost);
				}
			}

			// Heavy Catapults: very destructive in my own town, not on top priority in such a situation !
			// TODO. Put it in lower priority ?
		}

		// 3 : Specific counter-attack units
		// Priests already have been added in "super" unit section with necessary criteria
		// Slingers: only do them if enemy has much bowmen and if I can't do better than clubmen
		// Chariot (against priests) have been managed in 1st priority rules
		if (!found) {
			// 3a : Camel (if we can do cataphracts or armored elephants, it is has already been done to its maximum number in super units section)
			// TODO : improve this. If I can train (heavy) cavalry and enemy has more "other" units than chariots/cavalry, train cavalry instead
			if (!found && canTrainCamel && (enemyCamelVictimsCount > 0) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CAMEL;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Against siege weapons, try to create fast/strong units, avoid archers
			if (!found && canTrainCavalry && (enemySiegeWeaponsCount > 0) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CAVALRY; // cavalry or heavy cav (cataphract: already managed)
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainChariot && (enemySiegeWeaponsCount > 0) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CHARIOT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Catapult (not stone thrower). Only if several enemy siege weapons
			if (!found && canTrainCatapult && (enemySiegeWeaponsCount > 2) && (mySiegeWorkshopCount > 0)) {
				unitId_toAdd = CST_UNITID_CATAPULT;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// If greek or macedonian (they have bonuses): hoplite BEFORE standard infantry
			if (!found && (enemySiegeWeaponsCount > 2) && ((myCivId == CST_CIVID_MACEDONIAN) || (myCivId == CST_CIVID_GREEK)) && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainSwordsman && (enemySiegeWeaponsCount > 0) && (myBarracksCount > 0)) {
				unitId_toAdd = CST_UNITID_SHORT_SWORDSMAN;
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainHoplite && (enemySiegeWeaponsCount > 0) && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Scout. TODO Try more complex rules (only if many siege weapons)
			if (!found && canTrainScout && (enemySiegeWeaponsCount > 1) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_SCOUT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Clubman: only if there are a lot of siege weapon (otherwise, try others like archers first)
			if (!found && canTrainClubman && (enemySiegeWeaponsCount > 5) && (myBarracksCount > 0)) {
				unitId_toAdd = CST_UNITID_CLUBMAN; // or axeman
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
		}

		// 4 : Other units
		if (!found) {
			if (!found && canTrainPhalanx && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Only if greek or macedonian (they have bonuses) + if aristocracy is ON (faster hoplites): try hoplite BEFORE some other units.
			// NOT for roman: try standard infantry first because it has a bonus
			if (!found && ((myCivId == CST_CIVID_GREEK) || (myCivId == CST_CIVID_MACEDONIAN) || (hasAristocracy)) && (myCivId != CST_CIVID_ROMAN) && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Heavy cavalry
			if (!found && (canTrainHeavyCavalry) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CAVALRY; // heavy cav (cataphract: already managed)
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Horse archer
			if (!found && (canTrainHorseArcher) && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_HORSE_ARCHER;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// All swordsmen but legion (already managed). Fast to train. In addition, I have only 1 barracks (or this means I have good infantry)
			if (!found && canTrainSwordsman && (myBarracksCount > 0)) {
				unitId_toAdd = CST_UNITID_SHORT_SWORDSMAN; // or axeman
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// basic chariot
			if (!found && (canTrainChariot) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CHARIOT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Basic hoplite.
			if (!found && canTrainHoplite && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Elephant archer (if no priest)
			if (!found && (enemyPriestCount == 0) && (canTrainElephantArcher) && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_ELEPHANT_ARCHER;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Catapult against archers
			if (!found && canTrainCatapult && (mySiegeWorkshopCount > 0) && (enemySlingerVictimsCount * 100 / totalEnemyCount > 30)) {
				unitId_toAdd = CST_UNITID_STONE_THROWER;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Ballista (better than basic stone thrower but not as good as catapult)
			if (!found && canTrainBallista && (mySiegeWorkshopCount > 0)) {
				unitId_toAdd = CST_UNITID_BALLISTA;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Stone thrower against archers (after ballista)
			if (!found && canTrainStoneThrower && (mySiegeWorkshopCount > 0) && (enemySlingerVictimsCount * 100 / totalEnemyCount > 30)) {
				unitId_toAdd = CST_UNITID_STONE_THROWER;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Improved/composite bowman
			if (!found && canTrainImprovedBowman && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_IMPROVED_BOWMAN;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Scout
			if (!found && canTrainScout && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_SCOUT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Slinger => only if there are much bowmen
			if (!found && canTrainSlinger && (myBarracksCount > 0) && (enemySlingerVictimsCount * 100 / totalEnemyCount > 30)) {
				unitId_toAdd = CST_UNITID_SLINGER; // or axeman
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Basic bowman
			if (!found && canTrainBowman && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_BOWMAN;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Clubman (or axe)
			if (!found && canTrainClubman && (myBarracksCount > 0)) {
				unitId_toAdd = CST_UNITID_CLUBMAN; // or axeman
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// stone thrower: last call? (can be useful when out of food)
			if (!found && canTrainStoneThrower && (mySiegeWorkshopCount > 0)) {
				unitId_toAdd = CST_UNITID_STONE_THROWER;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
		}

		// Add the unit in strategy
		//if (found || (unitId_toAdd > -1)) {
		if (found) {
			strategyUpdated = true;
			if (panicStratElemToReuse[reuseIndex] != NULL) {
				// update strategy element
				long int *p = (long int*)panicStratElemToReuse[reuseIndex];
				panicStratElemToReuse[reuseIndex]->unitDAT_ID = unitId_toAdd;
				panicStratElemToReuse[reuseIndex]->unitInstanceId = -1;
				panicStratElemToReuse[reuseIndex]->elementType = TAIUnitClass::AIUCLivingUnit;
				panicStratElemToReuse[reuseIndex]->inProgressCount = 0;
				panicStratElemToReuse[reuseIndex]->aliveCount = 0;
				panicStratElemToReuse[reuseIndex]->buildAttempts = 0;
				panicStratElemToReuse[reuseIndex]->actor = unitId_actor;
				panicStratElemToReuse[reuseIndex]->totalCount = 0;
				panicStratElemToReuse[reuseIndex]->retrains = 1;
				panicStratElemToReuse[reuseIndex]->unitName[0] = 0;
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBaseToAdd = player->GetUnitDefBase(unitId_toAdd);
				if (unitDefBaseToAdd && unitDefBaseToAdd->IsCheckSumValidForAUnitClass()) {
					// Using internal name is sufficient here
					strcpy_s(panicStratElemToReuse[reuseIndex]->unitName, 0x3C, unitDefBaseToAdd->ptrUnitName);
				}
			} else {
				// Insert new
				STRATEGY::AddUnitInStrategy((AOE_STRUCTURES::STRUCT_BUILD_AI *) buildAI, 0, 1, unitId_actor, AIUCLivingUnit, unitId_toAdd, player);
			}

			totalPanicModeElemCount++;
		} else {
			totalPanicModeElemCount = 999; // stop !
		}
	}

	if (strategyUpdated && GetGameGlobalStructPtr()) {
		tacAI->lastPanicModeStrategyUpdateTime = GetGameGlobalStructPtr()->currentGameTime;
	}

	// If NOT panic mode, cancel panic mode units that are not being trained (no need to train them anymore). Too late for "in progress" ones...
	// Limitation: this "clean" can only be executed if I am attacked but out of my town :(
	if ((unitToCreateCount < 0) && (panicStratElemToReuse[0] != 0)) {
		for (int i = 0; i < PANIC_MODE_ARRAYS_MAX_SIZE; i++) {
			long int *p = (long int*)panicStratElemToReuse[i];
			if ((p != NULL) && (*(p + 0x20) == 0)) { // Not in progress
				*(p + 0x28) += 2; // +0xA0 total count. Make sure it is >=1 (original game often uses 2 and not 1, why ? Maybe just a mistake)
			}
		}
	}
}
// End of panic mode management



// Returns true if a construction should NOT be triggered.
// Default result is false (standard ROR behavior), this returns true only for specific (custom) cases.
// Warning: it could be wise for this method to be kept simple... and fast. It's called quite often.
bool STRATEGY::ShouldNotTriggerConstruction(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem) {
	if (!tacAI || !tacAI->IsCheckSumValid()) { return false; } // error: use default
	if (!stratElem || !stratElem->IsCheckSumValid()) { return false; } // error: use default

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; } // error: use default

	// AI improvements are not enabled => return default value (like in normal ROR code)
	if (!CUSTOMROR::IsImproveAIEnabled(tacAI->commonAIObject.playerId)) {
		return false; // error: use default
	}

	// Easy difficulty levels / MP games: default behavior too
	if (settings->rgeGameOptions.isMultiPlayer || (settings->rgeGameOptions.difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASY)) { return false; } // use default

	assert(stratElem->elementType == TAIUnitClass::AIUCBuilding);
	long int villagerTotalCount = tacAI->allVillagers.usedElements;
	if (villagerTotalCount <= 0) {
		return true; // No villager: don't build, of course ! (yes, ROR tries to start construction without villagers ;)
	}

	AOE_STRUCTURES::STRUCT_AI *ai = tacAI->ptrMainAI;
	if (!ai || !ai->IsCheckSumValid()) { return false; } // error: use default
	AOE_STRUCTURES::STRUCT_PLAYER *player = ai->ptrStructPlayer;
	if (!player || !player->IsCheckSumValid()) { return false; } // error: use default

	// Situation 1 : I'm going to build a wonder
	if (stratElem->unitDAT_ID == CST_UNITID_WONDER) {
		// *****
		// Make sure all researches from my strategy are already done before starting a wonder
		if (!STRATEGY::IsStrategyCompleteForWonder(ai)) {
			return true;
		}

		// *****
		// Make sure I have extra resources before starting a wonder

		// Collect desired extra amounts
		float myVirtualResources[RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT];
		// Start with a resource "debt" = extra amount I want to keep on hand
		myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_FOOD] = (float)-tacAI->SNNumber[SN_NUMBERS::SNMinimumFood];
		myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_WOOD] = (float)-tacAI->SNNumber[SN_NUMBERS::SNMinimumWood];
		myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_STONE] = (float)-tacAI->SNNumber[SN_NUMBERS::SNMinimumStone];
		myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_GOLD] = (float)-tacAI->SNNumber[SN_NUMBERS::SNMinimumGold];
		// Apply some hardcoded minimum requirements (for hard levels - excludes "medium")
		// Do not use hardcoded values for campaign / scenario : as we're using SN numbers, if the scenario if correctly designed, it will still work normally.
		if ((settings->rgeGameOptions.difficultyLevel < AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_MEDIUM) && (!settings->isCampaign) && (!settings->rgeGameOptions.isScenario)) {
			// I know that wonders don't require food, be let's remain generic... 
			// Moreover, that can prevent from starting a wonder when food is very low, which is a healthy restriction.
			if (myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_FOOD] > -300) {
				myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_FOOD] = -300;
			}
			if (myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_WOOD] > -300) {
				myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_WOOD] = -300;
			}
			if (myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_STONE] > -200) {
				myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_STONE] = -200;
			}
			if (myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_GOLD] > -250) {
				myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_GOLD] = -250;
			}
		}

		long int unitDefCount = player->structDefUnitArraySize;
		if (!player->ptrStructDefUnitTable) { unitDefCount = 0; }
		if ((stratElem->unitDAT_ID < 0) || (stratElem->unitDAT_ID >= unitDefCount)) {
			traceMessageHandler.WriteMessage("Error: Could not find unit definition for wonder");
			return true; // Unit definition does not exist !
		}
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *) player->ptrStructDefUnitTable[stratElem->unitDAT_ID];
		assert(unitDef && unitDef->DerivesFromTrainable());
		if (!unitDef || !unitDef->DerivesFromTrainable()) { return true; } // Unit definition does not exist !
		AOE_STRUCTURES::STRUCT_COST *costs = unitDef->costs;
		for (int i = 0; i < 3; i++) {
			if (costs[i].costPaid && (costs[i].costType >= 0) &&
				(costs[i].costType < RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT)) {
				// (valid) cost used: substract amount from my resources. We only car about food/wood/stone/gold here.
				myVirtualResources[costs[i].costType] -= (float)costs[i].costAmount;
			}
		}
		// Add player's actual owned resources and check the total
		for (int resourceType = 0; resourceType < RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT; resourceType++) {
			myVirtualResources[resourceType] += player->GetResourceValue((RESOURCE_TYPES)resourceType);
			if (myVirtualResources[resourceType] <= 0) { return true; } // A resource is missing. Cancel construction !
		}

		// *****
		// Another player already has a wonder
		// TO DO. To define what are the rules we want to apply.

		// [Wonder] We found no reason to cancel construction, so... let's proceed !
		return false;
	}

	// Situation 2 : I already have a lot of pending constructions
	if (stratElem->unitDAT_ID != CST_UNITID_FORUM) { // Allow building a TC even if I have many pending constructions*
		// First get the number of pending constructions
		long int unfinishedBuildings = 1; // We count the potential new building in the total.
		for (int i = 0; i < ai->structInfAI.buildingUnits.arraySize; i++) {
			long int bldId = ai->structInfAI.buildingUnits.unitIdArray[i];
			AOE_STRUCTURES::STRUCT_UNIT_BUILDING *unit = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)GetUnitStruct(bldId);
			if (unit && unit->IsCheckSumValid()) { // check IS building type
				if (unit->unitStatus < 2) {
					unfinishedBuildings++;
				}
			}
		}
		long int allowedExtraConstructionCount = 0;
		// Is total number of construction (including new potential one) too large ?
		if (villagerTotalCount + allowedExtraConstructionCount < unfinishedBuildings) {
#ifdef _DEBUG
			std::string msg = "Cancel construction launch: too many pending constructions / p#";
			msg += to_string(player->playerId);
			traceMessageHandler.WriteMessageNoNotification(msg); // Temporary message for debug
#endif
			return true; // There are too many constructions, more than my villagers count. Do NOT add a new construction !
		}
	}

	// [General] We found no reason to cancel construction, so... let's proceed !
	return false;
}


}
