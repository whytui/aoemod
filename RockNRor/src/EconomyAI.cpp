#include "../include/EconomyAI.h"

namespace CUSTOM_AI {
;

void EconomyAI::ResetAllInfo() {
	this->lastVillagersFix_ms = 0;
}


long int EconomyAI::Serialize(FILE *outputFile) const {
	long int result = 0;
	result += this->WriteBytes(outputFile, &this->lastVillagersFix_ms, sizeof(this->lastVillagersFix_ms));
	return result;
}

bool EconomyAI::Deserialize(FILE *inputFile) {
	this->ReadBytes(inputFile, &this->lastVillagersFix_ms, sizeof(this->lastVillagersFix_ms));
	return true;
}

// Run various fixes on villager actions, provided that a sufficient delay has passed since last execution
void EconomyAI::RunFixesOnVillagerActions(AOE_STRUCTURES::STRUCT_PLAYER *player, long int currentGameTime) {
	if (!player || !player->IsCheckSumValid() || !player->ptrGlobalStruct || !player->ptrGlobalStruct->IsCheckSumValid()) {
		return;
	}
	if (currentGameTime - this->lastVillagersFix_ms < AI_CONST::delayForVillagerFixes_ms) {
		// We've not waited enough.
		return;
	}
	this->lastVillagersFix_ms = currentGameTime; // update last execution time.
	this->CheckForDuplicateFarmers(player);
	this->FixStuckRepairmen(player);
}


// Remove farmers when more than 1 are assigned to the same farm.
void EconomyAI::CheckForDuplicateFarmers(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid() || !player->isComputerControlled || !player->ptrAIStruct ||
		!player->ptrAIStruct->IsCheckSumValid() || !player->ptrCreatableUnitsListLink ||
		!player->ptrCreatableUnitsListLink->IsCheckSumValid()) {
		return;
	}
	std::list<std::pair<AOE_STRUCTURES::STRUCT_UNIT_BASE *, long int>>farmersAndTheirTargetFarm;
	std::list<long int>usedFarmIds;

	STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *curElem = player->ptrCreatableUnitsListLink->lastListElement;
	while (curElem) {
		if (curElem->unit && curElem->unit->IsCheckSumValidForAUnitClass()) {
			AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = curElem->unit;
			// Beware repairmen ! (AI bug: their unitDef remains farmers). Here we check action=gather to avoid this.
			if ((unit->unitStatus == GAME_UNIT_STATUS::GUS_2_READY) && unit->unitDefinition &&
				unit->unitDefinition->IsCheckSumValidForAUnitClass() && (unit->unitDefinition->DAT_ID1 == CST_UNITID_FARMER)) {
				STRUCT_ACTION_BASE *action = AOE_STRUCTURES::GetUnitAction(unit);
				STRUCT_ACTION_GATHER_WITHOUT_ATTACK *actionGather = NULL;
				if (action && (action->actionTypeID == UNIT_ACTION_ID::CST_IAI_GATHER_NO_ATTACK)) {
					actionGather = (STRUCT_ACTION_GATHER_WITHOUT_ATTACK *)action;
				}
				if (actionGather && (actionGather->targetUnitDefId == CST_UNITID_FARM)) {
					std::pair<AOE_STRUCTURES::STRUCT_UNIT_BASE *, long int> p;
					p.first = unit;
					p.second = action->targetUnitId;
					farmersAndTheirTargetFarm.push_back(p);
					usedFarmIds.push_back(action->targetUnitId);
				}
			}
		}
		curElem = curElem->previousElement;
	}

	for each (long int farmId in usedFarmIds)
	{
		int gathererCount = 0;
		for each (std::pair<AOE_STRUCTURES::STRUCT_UNIT_BASE *, long int> p in farmersAndTheirTargetFarm)
		{
			AOE_STRUCTURES::STRUCT_UNIT_BASE *curGatherer = p.first;
			long int targetFarmId = p.second;
			if (targetFarmId == farmId) {
				gathererCount++;
				if (curGatherer && (gathererCount > 1)) {
					// More than 1 farmer on a given farm. Stop action (compatible with MP games). 
					// AI will automatically (and immediately) tell the unit to go to TC to drop the food it carries (if any)
					// And then re-task it.
					GAME_COMMANDS::CreateCmd_Stop(curGatherer->unitInstanceId);
				}
			}
		}
	}
}


// Fix repairment that are stuck in idle status because they got blocked at some point in their movement.
void EconomyAI::FixStuckRepairmen(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid() || !player->isComputerControlled || !player->ptrAIStruct ||
		!player->ptrAIStruct->IsCheckSumValid() || !player->ptrCreatableUnitsListLink ||
		!player->ptrCreatableUnitsListLink->IsCheckSumValid()) {
		return;
	}
	STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *curElem = player->ptrCreatableUnitsListLink->lastListElement;
	while (curElem) {
		if (curElem->unit && curElem->unit->IsCheckSumValidForAUnitClass()) {
			AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = curElem->unit;
			if (unit && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition && unit->unitDefinition->IsCheckSumValidForAUnitClass() &&
				(unit->unitStatus == GAME_UNIT_STATUS::GUS_2_READY)) {
				STRUCT_ACTION_BASE *action = AOE_STRUCTURES::GetUnitAction(unit);
				if (action && (action->actionTypeID == UNIT_ACTION_ID::CST_IAI_REPAIR)) {
					int actionStatus = action->actionStatus;
					bool foundMoveAction = false;
					STRUCT_UNIT_ACTION_INFO *uai = action->requiredActionInfo;
					if (uai && uai->ptrActionLink) {
						STRUCT_ACTION_BASE *reqAction = uai->ptrActionLink->actionStruct;
						if (reqAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_MOVE_TO) {
							foundMoveAction = true;
						}
					}
					if (!foundMoveAction && (actionStatus == ACTION_STATUS::CST_AS_0A_DELAY)) {
						GAME_COMMANDS::CreateCmd_Stop(unit->unitInstanceId);
					}
#ifdef _DEBUG
					// TMP
					else {
						if (actionStatus == ACTION_STATUS::CST_AS_0A_DELAY) {
							// Does not seem to ever happen
							GetGameSettingsPtr()->AddEventInHistory((long int)unit->positionX, (long int)unit->positionY);
							AOE_METHODS::CallWriteCenteredText("Found action status 0x0A (stuck unit?)", 2);
						}
					}
#endif
				}
			}
		}
		curElem = curElem->previousElement;
	}
}


// Called when "player" kills a gaia animal
void EconomyAI::OnGaiaAnimalKilled(STRUCT_PLAYER *player, STRUCT_UNIT_ATTACKABLE *killedAnimal) {
	if (!player || !player->IsCheckSumValid() || !killedAnimal || !killedAnimal->IsCheckSumValidForAUnitClass()) { return; }
	if (!player->ptrAIStruct || !player->ptrAIStruct->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = &player->ptrAIStruct->structInfAI;
	assert(infAI->IsCheckSumValid());
	if (killedAnimal->unitDefinition->unitAIType == TribeAIGroupPreyAnimal) {
		return; // Gazelles are already handled correctly. The "add" method below checks unicity but it would be bad for performance.
	}
	if ((killedAnimal->resourceTypeId >= 0) && (killedAnimal->resourceValue > 0)) {
		// This is useful for animals that are not added to infAI lists (natively, predator animals)
		unsigned long int addr = 0x4C49C0; // add gatherable target in list
		_asm {
			MOV ECX, infAI;
			PUSH killedAnimal; // parameter is gatherable unit pointer
			CALL addr;
		}
	}
}


// Returns true if unit can be targeted as a resource by AI players
// At infAI level, role = "is important resource"
bool EconomyAI::IsAITargetableResource(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->unitDefinition || !unit->unitDefinition->IsCheckSumValidForAUnitClass()) { return false; }
	GLOBAL_UNIT_AI_TYPES unitClass = unit->unitDefinition->unitAIType;
	// Standard classes, cf 0x4BE1C0
	if (
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupUnknownFish) || // 1F
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSeaFish) || // 5
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupShoreFish) || // 21
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBerryBush) || // 7
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPreyAnimal) || // 9
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupStoneMine) || // 8
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupGoldMine) || // 20
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTree) // 0F
		) {
		return true;
	}
	if (!unit->ptrStructPlayer || !IsImproveAIEnabled(unit->ptrStructPlayer->playerId)) {
		return false; // Standard case when AI improvements are OFF
	}

	// Custom cases (by default, here we should always return false)
	if (unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPredatorAnimal) {
		// Allow slow predator animals (can be hunted - almost - safely)

		STRUCT_UNITDEF_ATTACKABLE *unitDef = (STRUCT_UNITDEF_ATTACKABLE *)unit->unitDefinition;
		// Animals that can't attack (not type50+ / weird for predators) and no-food animals (birds) = NOT targetable as resource
		if (!unitDef->DerivesFromAttackable() || (unitDef->resourceCapacity <= 0)) {
			return false;
		}
		float baseSpeed = unitDef->speed;
		float maxSpeed = unitDef->speed;
		if (unitDef->ptrUnitCommandHeader && unitDef->ptrUnitCommandHeader->IsCheckSumValid()) {
			for (int i = 0; i < unitDef->ptrUnitCommandHeader->commandCount; i++) {
				if (unitDef->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_UNKNOWN_7) {
					STRUCT_UNIT_COMMAND_DEF *cmd = unitDef->ptrUnitCommandHeader->ptrCommandArray[i];
					if (cmd->ptrMovingGraphic) {
						float tmpSpeed = baseSpeed * cmd->ptrMovingGraphic->speedMultiplier;
						if (tmpSpeed > maxSpeed) {
							maxSpeed = tmpSpeed;
						}
					}
				}
			}
		}

		return (maxSpeed < AI_CONST::maxAllowedPredatorAnimalSpeedForHunting) && (unitDef->resourceCapacity > 0); // ignore animals without food, like birds
	}
	return false;
}


// Returns true if unit is a flag/artefact (for AI). Original method=0x4BE200
// At infAI level, role = "is important misc"
bool EconomyAI::IsArtefactOrFlag(GLOBAL_UNIT_AI_TYPES unitClass) {
	return (unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact) ||
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupFlag);
}


// Returns true if unit is one of (artefact OR targetable resource OR creatable unit). For AI.
// Original method= 0x4BE100, only depends on unit class.
// Game role = "is important object" at infAI level
bool EconomyAI::IsArtefactOrTargetableResourceOrCreatable(STRUCT_UNIT_BASE *unit) {
	long int rorBool = 0;
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
	if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) { return false; }
	unsigned long int addrIsPlayerCreatable = 0x4BE140;
	long int unitClassLong = (long int)unitDef->unitAIType;
	_asm {
		PUSH unitClassLong;
		CALL addrIsPlayerCreatable;
		MOV rorBool, EAX;
	}
	// 1st and 3rd conditions in original method
	if ((rorBool != 0) || (EconomyAI::IsArtefactOrFlag(unitDef->unitAIType))) {
		return true;
	}
	// More complex (adds specific treatments) for 2nd condition (targetable as resource)
	return EconomyAI::IsAITargetableResource(unit);
}


// Override the ROR method (0x4D1BB5) that executes current tacAI task update (cf AI_UPDATE_TYPES)
// Returns true if some specific treatments have been executed here = do NOT execute ROR standard treatment for current task
// Returns false otherwise (default) = let ROR execute normally its treatments for current task
// Warning *** this is supposed to take care of processing time, be carful about performance ***
bool EconomyAI::RunOneTacAIUpdateTask(STRUCT_TAC_AI *tacAI, long int startProcessingTime, long int allowedProcessingTime) {
	// Remark : tacAI.Update() in 0x4D1B70 calls alternately the various AI updates, including this one cf AI_UPDATE_TYPES::CST_AUT_EVAL_CIVILIAN_DISTRIB
	assert(tacAI && tacAI->IsCheckSumValid() && tacAI->ptrMainAI);
	if (!tacAI || !tacAI->ptrMainAI || !tacAI->ptrMainAI->ptrStructPlayer) { return false; }
	bool doNotRunRorUpdate = false;
	bool isAIImproved = ROCKNROR::IsImproveAIEnabled(tacAI->commonAIObject.playerId);
	if (!isAIImproved) { return false; }

	switch (tacAI->currentAIUpdateType) {
	//case AI_UPDATE_TYPES::CST_AUT_SETUP_SOLDIER_GROUPS:
	case AI_UPDATE_TYPES::CST_AUT_EVAL_CIVILIAN_DISTRIB:
		// If needed resources have never been computed yet, let standard code execute. Next time the flag will be set.
		if ((tacAI->neededResourcesAreInitialized != 0) && (tacAI->allVillagers.usedElements > 0)) {
			doNotRunRorUpdate = EconomyAI::CalcVillagerCountByTask(tacAI);
		}
		break;
		/*case AI_UPDATE_TYPES::CST_AUT_TASK_CIVILIAN:
		case AI_UPDATE_TYPES::CST_AUT_SET_BOAT_GROUPS:
		case AI_UPDATE_TYPES::CST_AUT_FILL_BOAT_GROUPS:
		case AI_UPDATE_TYPES::CST_AUT_TASK_BOATS:
		case AI_UPDATE_TYPES::CST_AUT_FILL_SOLDIER_GROUPS:*/
		case AI_UPDATE_TYPES::CST_AUT_TASK_IDLE_SOLDIER: // Calls 0x4D3700 if I own land military units or boats or relics
			ROCKNROR::COMBAT::OnAIUpdateIdleSoldiers(tacAI,startProcessingTime, allowedProcessingTime);
			break;
		/*case AI_UPDATE_TYPES::CST_AUT_TASK_ACTIVE_SOLDIER:
		case AI_UPDATE_TYPES::CST_AUT_PLAYTASKING:
		case AI_UPDATE_TYPES::CST_AUT_ATTACK_ENEMY_TOWER_IN_MY_TOWN:
		case AI_UPDATE_TYPES::CST_AUT_RESEARCH:
		case AI_UPDATE_TYPES::CST_AUT_TRAIN:
		case AI_UPDATE_TYPES::CST_AUT_BUILD_LIST:
		case AI_UPDATE_TYPES::CST_AUT_HELP_BUILD:*/
	case AI_UPDATE_TYPES::CST_AUT_REPAIR_BUILDING:
		doNotRunRorUpdate = EconomyAI::HandleNewRepairmen(tacAI);
		break;
		/*case AI_UPDATE_TYPES::CST_AUT_REPAIR_WALL:
		case AI_UPDATE_TYPES::CST_AUT_BUILD:
		case AI_UPDATE_TYPES::CST_AUT_OPEN_BUILDS:
		case AI_UPDATE_TYPES::CST_AUT_OPEN_TASKS:
		break;*/
	case AI_UPDATE_TYPES::CST_AUT_FOOD_DROPSITE:
	{
		EconomyAI::AiUpdateFoodDropSite(tacAI);
	}
		break;
	case AI_UPDATE_TYPES::CST_AUT_BUILD_LIST_INSERTIONS:
		doNotRunRorUpdate = EconomyAI::UpdateStrategyAutoBuildInsertions(tacAI);
		break;
	default:
		break;
	}

	return doNotRunRorUpdate;
}


// Override the ROR method that calculates desired number of explorers, and then desired number of non-explorers (cf call to 0x4D9BE0)
// Returns true if treatments have been run here and if ROR treatments MUST NOT be run (prevent ROR from calculating number of villagers by task)
// Returns false if nothing special has been done = default case (let ROR treatments happen)
// Warning *** this is supposed to take care of processing time, be carful about performance ***
bool EconomyAI::CalcVillagerCountByTask(STRUCT_TAC_AI *tacAI) {
	// Remark : tacAI.Update() in 0x4D1B70 calls alternately the various AI updates, including this one cf AI_UPDATE_TYPES::CST_AUT_EVAL_CIVILIAN_DISTRIB
	assert(tacAI && tacAI->IsCheckSumValid() && tacAI->ptrMainAI);
	if (!tacAI || !tacAI->ptrMainAI || !tacAI->ptrMainAI->ptrStructPlayer) { return false; }
	bool doNotRunRorUpdate = false;
	long int totalVillagerCount = tacAI->allVillagers.usedElements;
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(global && global->IsCheckSumValid());
	assert(settings && settings->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *player = tacAI->ptrMainAI->ptrStructPlayer;
	bool diffHardOrHardest = (settings->rgeGameOptions.difficultyLevel <= GAME_DIFFICULTY_LEVEL::GDL_HARD);
	long int minVillager = settings->isDeathMatch ? AI_CONST::minimumVillagersToAllowExplorersInLateGame_DM : AI_CONST::minimumVillagersToAllowExplorersInLateGame_RM;
	if ((totalVillagerCount < minVillager) && diffHardOrHardest && global && 
		(global->currentGameTime > AI_CONST::delayForExplorationLateGame_ms)) {
		// This situation is not well handled by standard AI
		// TODO: do I have a TC ? Do I have food ? What is my desired number of villagers (beware DM) ?
		// player->ptrAIStruct->structTacAI.SNNumber[SNCapCivilianExplorers]
		doNotRunRorUpdate = true;
		tacAI->villagerExplorers.usedElements = 0;
		tacAI->desiredGathererVillagersCount = totalVillagerCount;
		unsigned long int addrCalcDesiredGathererCounts = 0x4D9BE0;
		_asm {
			MOV ECX, tacAI;
			CALL addrCalcDesiredGathererCounts; // No parameter
		}
	}

	// Check for obsolete / inadequate villager task elements
	// The goal is to reset tacAI's "gatherer task info" on non-essential resource types, because they can prevent 
	// villager tasking method (eg tacAI.highLevelTaskGatherer in 0x4DA330) from gathering another (more useful) resource type
	if (diffHardOrHardest && tacAI->neededResourcesAreInitialized) {
		const int numberOfLowPriorityResTypeToPrevent = 1; // Number of lowest-priority resource types to forbid from being gathered more than desired (gatherer count)
		int resTypePriority[4]; // value 0(low priority) to 3(high priority)
		for (int i = 0; i < 4; i++) {
			long int resType = tacAI->neededResourceTypesByPriority[i];
			resTypePriority[resType] = 3 - i;
		}

		int maxDiff = 0;
		int maxDiffScore = 0;
		int maxDiffIndex = -1;
		for (int i = 0; i < 4; i++) {
			int diff = tacAI->gathererCount_actual[i] - tacAI->gathererCount_desired[i];
			int diffScore = diff * 10 - resTypePriority[i];
			if ((diffScore > maxDiffScore) && (resTypePriority[i] < numberOfLowPriorityResTypeToPrevent)) {
				maxDiff = diff;
				maxDiffScore = diffScore;
				maxDiffIndex = i;
			}
		}

		const int neededResourceComparisonValue = 10;  // Not zero, use a bit more, especially for wood/stone or even gold that may be needed for repairs
		const int minOverdueGathererProportion = 20; // % above which extra gatherers may be forced to stop gathering their current non-priority resource.

		int proportionPercent = (maxDiff * 100) / totalVillagerCount;
		bool topPriorityResourceIsReallyNeeded = tacAI->extraResourceAmount[tacAI->neededResourceTypesByPriority[0]] < neededResourceComparisonValue;
		bool maxDiffResourceIsReallyNeeded = tacAI->extraResourceAmount[maxDiffIndex] < neededResourceComparisonValue;
		if (!topPriorityResourceIsReallyNeeded || maxDiffResourceIsReallyNeeded) {
			// When all resources are needed, even lowest priority one (or when no resource is really needed = DM), let villagers work as they currently do.
			maxDiffIndex = -1;
		}

		if ((maxDiffIndex >= 0) && (proportionPercent > minOverdueGathererProportion)) {
			unsigned long int validEntriesFound = 0; // optim only / no functional role
			for (int i = 0; i < 50; i++) {
				if (tacAI->gatherersTasks[i].unitId >= 0) {
					validEntriesFound++;
					if ((tacAI->gatherersTasks[i].resourceType == maxDiffIndex) &&
						(tacAI->gatherersTasks[i].resourceAmount < tacAI->gatherersTasks[i].maxResourceAmount)) {
						// Mark task info as complete: that will allow the villager to be assigned another gathering task (on another resource type, probably)
						tacAI->gatherersTasks[i].maxResourceAmount = 0;
						break;
					}
					if (validEntriesFound >= tacAI->villagerTasksRelevantElemCount) {
						break; // Found all valid tasks, no need to go further
					}
				}
			}
			
		}
	}

	return doNotRunRorUpdate;
}


// Override the ROR method that handles insertions for first granary, first SP, auto build farms/towers, specific build item from SN numbers
// Returns true if ROR treatments must be skipped, false if ROR treatments must be executed normally
bool EconomyAI::UpdateStrategyAutoBuildInsertions(STRUCT_TAC_AI *tacAI) {
	// Standard treatments: 
	// Add the first storage pit in strategy when enough trees have been found (cf SNAutoBuildDropsites + SNMinimumForestTiles)
	// Add the first granary cf SNAutoBuildDropsites if infAI.bestForageDropSiteDistance >= 0 (-1 while no berry is known?)
	// Add the "specific item" cf SNSpecificBuildItemToBuild + SNSpecificBuildItemTime
	// Add towers if available, if (planned) resources are OK, according to SNMaxTowers + SNAutoBuildTowers
	// Add farms if iron age, according to SNMaxFarms + SNAutoBuildFarms
	// TOWERS+FARMS: one more condition="all strategy buildings have been/are being constructed". This condition has a BUG if "limited retrains" buildings are not alive.
	// It seems farms are only added when strategy file is "finished" (at least buildings),
	// which is a bad criteria and explains why it seems so random/poorly handled in standard games

	if (!tacAI || !tacAI->ptrMainAI) { return false; }
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = &tacAI->ptrMainAI->structInfAI;
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = &tacAI->ptrMainAI->structBuildAI;
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	AOE_STRUCTURES::STRUCT_PLAYER *player = tacAI->ptrMainAI->player;
	assert(global && global->IsCheckSumValid() && player && player->IsCheckSumValid());
	assert(infAI->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid() || !infAI->IsCheckSumValid() || !player || !player->IsCheckSumValid()) { return false; }
	if (!tacAI->granaryAddedToStrategy || !tacAI->storagePitAddedToStrategy) {
		// Game start: let ROR standard code handle this until first SP/granary are added to strategy
		return false;
	}

	// SN numbers specific build item
	EconomyAI::HandleSNSpecificBuildItem(tacAI);

	int currentAge = (int)player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_CURRENT_AGE); // 1=stone, 4=iron
	bool canBuildTower = (AOE_STRUCTURES::PLAYER::IsUnitAvailableForPlayer(CST_UNITID_WATCH_TOWER, player) != 0);
	bool canBuildFarm = (AOE_STRUCTURES::PLAYER::IsUnitAvailableForPlayer(CST_UNITID_FARM, player) != 0); // True if unit is available and autobuildfarms is OK
	canBuildTower &= (tacAI->SNNumber[SNAutoBuildTowers] == 1);
	canBuildFarm &= (tacAI->SNNumber[SNAutoBuildFarms] == 1);
	long int maxTowers = tacAI->SNNumber[SNMaxTowers];
	long int maxFarms = tacAI->SNNumber[SNMaxFarms];
	int actualTowerCount = AOE_STRUCTURES::PLAYER::GetPlayerUnitCount(player, CST_UNITID_WATCH_TOWER, GLOBAL_UNIT_AI_TYPES::TribeAINone, 0, 2);
	int actualFarmCount = AOE_STRUCTURES::PLAYER::GetPlayerUnitCount(player, CST_UNITID_FARM, GLOBAL_UNIT_AI_TYPES::TribeAINone, 0, 2);
	canBuildTower &= PLAYER::CanUnitCostBePaid(player, CST_UNITID_WATCH_TOWER);
	bool canPayFarm = PLAYER::CanUnitCostBePaid(player, CST_UNITID_FARM);
	long int currentStratElemId = buildAI->currentIDInStrategy;

	// Retrieve all information from strategy in only one loop
	int strategyTowerCount = 0;
	int strategyFarmCount = 0;
	int strategyPendingFarmCount = 0; // non-build/non in progress farm count
	int currentEstimatedPopulation = 0;
	int firstTownCenterStatus = -1; // -1=not found, 0=found&destroyed, 1=found&built or in progress
	const int LAST_BLD_NO_FURTHER_THAN_POPULATION = 45;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *tmpElem = buildAI->fakeFirstStrategyElement.next;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *buildAiCurrentStratElem = NULL; // determined by buildAI->currentIDInStrategy
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *lastStandardBuilding = NULL; // last building in strategy, excluding wonder/farm/tower/house. Limitation to search no further than pop=LAST_BLD_NO_FURTHER_THAN_POPULATION
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *ironAge = NULL;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *latestActiveResearch = NULL; // last research that is "active" (done)
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *towerToReuse = NULL;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *farmToReuse = NULL;
	bool strategyContainsDock = false;
	while (tmpElem && (tmpElem != &buildAI->fakeFirstStrategyElement)) {
		if (tmpElem->elemId == currentStratElemId) {
			buildAiCurrentStratElem = tmpElem;
		}
		if ((tmpElem->elementType == TAIUnitClass::AIUCCritical) || (tmpElem->elementType == TAIUnitClass::AIUCTech)) {
			if (tmpElem->unitDAT_ID == CST_RSID_IRON_AGE) {
				ironAge = tmpElem;
			}
			if (tmpElem->aliveCount > 0) {
				latestActiveResearch = tmpElem;
			}
		}
		if (tmpElem->elementType == TAIUnitClass::AIUCBuilding) {
			if (tmpElem->unitDAT_ID == CST_UNITID_WATCH_TOWER) {
				if ((tmpElem->aliveCount > 0) || (tmpElem->inProgressCount > 0) || (tmpElem->retrains < 0) || (tmpElem->totalCount < tmpElem->retrains)) {
					strategyTowerCount++;
				}
				if ((towerToReuse == NULL) && (tmpElem->aliveCount == 0) && (tmpElem->inProgressCount == 0) && (tmpElem->retrains >= 0) && (tmpElem->totalCount >= tmpElem->retrains)) {
					towerToReuse = tmpElem;
				}
			}
			if (tmpElem->unitDAT_ID == CST_UNITID_FARM) {
				if ((tmpElem->aliveCount > 0) || (tmpElem->inProgressCount > 0) || (tmpElem->retrains < 0) || (tmpElem->totalCount < tmpElem->retrains)) {
					strategyFarmCount++;
					if ((tmpElem->aliveCount == 0) && (tmpElem->inProgressCount == 0)) {
						strategyPendingFarmCount++;
					}
				}
				if ((farmToReuse == NULL) && (tmpElem->aliveCount == 0) && (tmpElem->inProgressCount == 0) && (tmpElem->retrains >= 0) && (tmpElem->totalCount >= tmpElem->retrains)) {
					farmToReuse = tmpElem;
				}
			}
			if ((firstTownCenterStatus == -1) && (tmpElem->unitDAT_ID == CST_UNITID_FORUM)) {
				if ((tmpElem->aliveCount > 0) || (tmpElem->inProgressCount > 0) ||
					((tmpElem->retrains >= 0) && (tmpElem->totalCount >= tmpElem->retrains))) {
					firstTownCenterStatus = 1; // Alive or in progress
				} else {
					firstTownCenterStatus = 0; // Need to trigger build
				}
			}
			if ((tmpElem->unitDAT_ID != CST_UNITID_WATCH_TOWER) &&
				(tmpElem->unitDAT_ID != CST_UNITID_FARM) &&
				(tmpElem->unitDAT_ID != CST_UNITID_HOUSE) &&
				(tmpElem->unitDAT_ID != CST_UNITID_WONDER) &&
				(currentEstimatedPopulation < LAST_BLD_NO_FURTHER_THAN_POPULATION)) {
				// Additional condition: take only building with unlimited retrains (except first time, to make sure we find a building)
				if ((tmpElem->retrains < 0) || (lastStandardBuilding == NULL)) {
					lastStandardBuilding = tmpElem;
				}
			}
			if (tmpElem->unitDAT_ID == CST_UNITID_DOCK) {
				strategyContainsDock = true;
			}
		}
		if ((tmpElem->elementType == TAIUnitClass::AIUCLivingUnit) && (tmpElem->retrains < 0)) {
			currentEstimatedPopulation++;
		}
		tmpElem = tmpElem->next;
	}
	if (lastStandardBuilding == NULL) {
		lastStandardBuilding = buildAI->fakeFirstStrategyElement.next; // should never happen
	}
	if (ironAge == NULL) {
		ironAge = lastStandardBuilding; // should never happen
	}
	if (latestActiveResearch == NULL) {
		latestActiveResearch = buildAiCurrentStratElem;
	}


	// Force wait for a minimum exploration before adding towers. Especially when starting at iron age
	const int MIN_EXPLORED_TILES_FOR_TOWERS = 2700; // note: a town with radius=20 represents 40*40=1600 tiles
	// Test also RESOURCE_TYPES::CST_RES_ORDER_MAP_EXPLORATION for small maps ?
	bool enoughExplorationForTowers = (player->myMapInfo->exploredTilesCount > MIN_EXPLORED_TILES_FOR_TOWERS);

	// Auto build towers
	if (canBuildTower && (actualTowerCount < maxTowers) && (strategyTowerCount < maxTowers) && enoughExplorationForTowers) {
		const int MAX_TOWERS_BEFORE_IRON = 4;
		// Allow to add towers in iron, allow to add *some* towers before (but no more than MAX_TOWERS_BEFORE_IRON)
		if ((actualTowerCount < MAX_TOWERS_BEFORE_IRON) || (currentAge >= 4)) {
			if (towerToReuse) {
				towerToReuse->totalCount = 0; // Reset it so that it can be built again
			} else {
				ROCKNROR::STRATEGY::AddUnitInStrategy_before(buildAI, lastStandardBuilding->next, 1, -1, TAIUnitClass::AIUCBuilding, CST_UNITID_WATCH_TOWER, player, "Tower, added");
			}
		}
	}

	// Auto build farms
	int spareFarms = strategyFarmCount - tacAI->gathererCount_desired[CST_RES_ORDER_FOOD];
	if (spareFarms < 0) { spareFarms = 0; }
	// Evaluate various conditions that prevent addition of farms to strategy. Those conditions are non exclusive, it's just a filter
	bool doNotAddFarmToStrategy = (strategyFarmCount >= maxFarms) || (actualFarmCount >= maxFarms);
	if (spareFarms > 0) { doNotAddFarmToStrategy = true; } // Never build additional farms when I already have more than currently "desired"
	if (firstTownCenterStatus == 0) { doNotAddFarmToStrategy = true; } // TC has top priority, do not build other stuff right now
	if ((!canBuildFarm || !canPayFarm) && (strategyPendingFarmCount > 0)) {
		// not enough wood to build. Don't add farm unless there really is no available (spare) farm to gather (and none anticipated in strategy):
		// in this case, we may add a farm so that its cost will be taken into account in gathering so we can actually build it
		doNotAddFarmToStrategy = true;
	}

	if (!doNotAddFarmToStrategy && canBuildFarm) {
		int food = (int)player->GetResourceValue(CST_RES_ORDER_FOOD);
		//int wood = (int)player->GetResourceValue(CST_RES_ORDER_WOOD);

		// a "DM" case: much food, ok if all buildings have been built (no hurry to build farms)
		bool deathmatchCaseOk = (food > 5000) && (lastStandardBuilding->aliveCount != 0);
		bool needFoodCaseOk = (food <= 5000); // Not so much food: build farms, don't wait for all strategy elements to be built !
		//if (strategyContainsDock) { // this also applies to non-water maps, actually
			int i = 0;
			bool foundWoodOrFood = false;
			while ((i < 4) && !foundWoodOrFood) {
				if (tacAI->neededResourceTypesByPriority[i] == CST_RES_ORDER_WOOD) {
					foundWoodOrFood = true;
					needFoodCaseOk = false; // At this point, wood is more needed than food.
				}
				if (tacAI->neededResourceTypesByPriority[i] == CST_RES_ORDER_FOOD) {
					foundWoodOrFood = true;
				}
				i++;
			}
		//}

		// Only add farms when there is an actual need for food. Take into account the fact I have berries near a granary in my town ?
		if ((deathmatchCaseOk || needFoodCaseOk)) {
			if (farmToReuse) {
				farmToReuse->totalCount = 0; // Reset it so that it can be built again
			} else {
				ROCKNROR::STRATEGY::AddUnitInStrategy_before(buildAI, latestActiveResearch, 1, -1, TAIUnitClass::AIUCBuilding, CST_UNITID_FARM, player, "Farm, added");
			}
		}
	}

	// We could handle auto build houses here too

	return true; // Do not run standard treatments
}


// Handles the specific build item from SN numbers (SNSpecificBuildItemToBuild)
void EconomyAI::HandleSNSpecificBuildItem(STRUCT_TAC_AI *tacAI) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = &tacAI->ptrMainAI->structBuildAI;
	long int specificItemId = tacAI->SNNumber[SNSpecificBuildItemToBuild];
	if (specificItemId >= 0) {
		int triggerTime = tacAI->SNNumber[SNSpecificBuildItemTime];
		triggerTime = triggerTime * 60000; // milliseconds
		if (triggerTime <= global->currentGameTime) {
			tacAI->SNNumber[SNSpecificBuildItemToBuild] = -1; // unset / disable specific build item for the rest of the game
			//ROCKNROR::STRATEGY::AddUnitInStrategy(buildAI, 0, 1, -1, TAIUnitClass::AIUCBuilding, specificItemId, tacAI->ptrMainAI->player);
			// Note: buildAI.addElementInStrategy only handles standard units (and not all, slingers, camels, etc don't seem to be supported)
			unsigned long int addrAddElemInStrategy = 0x4B9050;
			_asm {
				PUSH 0; // position
				PUSH 1; // retrains
				PUSH specificItemId;
				MOV ECX, buildAI;
				CALL addrAddElemInStrategy; // buildAI.addElementInStrategy(DATID, retrains, position)
			}
		}
	}
}


// Checks if some repairations should be triggered
// Returns true if ROR treatments must be skipped, false if ROR treatments must be executed normally
bool EconomyAI::HandleNewRepairmen(STRUCT_TAC_AI *tacAI) {
	if (!tacAI || !tacAI->ptrMainAI || !tacAI->ptrMainAI->player) { return false; }

	bool noMoreWood = tacAI->ptrMainAI->player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_WOOD) < 0.1;
	bool noMoreStone = tacAI->ptrMainAI->player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_STONE) < 0.1;

	if (!noMoreStone && !noMoreWood) {
		// Standard case: we have resources to repair anything. Let ROR code execute normally.
		return false;
	}
	if (noMoreStone && noMoreWood) {
		return true; // prevent ROR from trying to repair. Everything is depleted...
	}

	// Here either wood or stone is missing.

	if (tacAI->myWonderBeingBuiltFlag) {
		return true; // Do nothing. Don't repair other buildings when wonder is being built (~equivalent to original code)
	}
	
#pragma WARNING("Unfinished. Continue using ROR code meanwhile. Need to test ROR code more: in theory, it checks we have the resources to repair !")
	return false;

	if (noMoreStone) {
		// No stone
		
	} else {
		// No wood
		//STRUCT_UNIT_BASE *unitToRepair = AOE_STRUCTURES::PLAYER::GetPlayerMostDamagedUnit(tacAI->ptrMainAI, GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding, CST_UNITID_WATCH_TOWER);

	}

	return false; // Default: let standard ROR treatments occur
}


// Returns true if ROR treatments must be skipped, false if ROR treatments must be executed normally
bool EconomyAI::AiUpdateFoodDropSite(STRUCT_TAC_AI *tacAI) {
	// Try to fix outdated dropsites (when destroyed) ?
	// Find out why AI sometimes chooses gather targets so far
	/*auto infAI = &tacAI->ptrMainAI->structInfAI;
	auto woodInfo = infAI->spottedGatherableUnitsByResourceTypeArrays[1];
	auto woodInfoSize = infAI->spottedGatherableUnitsByResourceTypeArraySizes[1];
	auto woodInfoUsed = infAI->spottedGatherableUnitsCountByResourceType[1];
	for (int i = 0; i < woodInfoSize; i++) {
	auto xx = woodInfo[i];
	if (woodInfo[i].gatherableUnitId == -1) {
	bool foundUnused = true;
	}
	}*/
	return false;
}


}
