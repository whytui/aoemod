#include "../include/EconomyAI.h"

namespace CUSTOM_AI {
;

void EconomyAI::ResetAllInfo() {
	this->lastVillagersFix_ms = 0;
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
						// Experimental: not sure the conditions match the case that causes problems in game ! But first tests seem ok !
#ifdef _DEBUG
						GetGameSettingsPtr()->AddEventInHistory((long int)unit->positionX, (long int)unit->positionY);
						AOE_METHODS::CallWriteCenteredText("Unblocked a repairman", 2);
#endif
						GAME_COMMANDS::CreateCmd_Stop(unit->unitInstanceId);
					}
#ifdef _DEBUG
					// TMP
					else {
						if (actionStatus == ACTION_STATUS::CST_AS_0A_DELAY) {
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


// Override the ROR method that executes current tacAI task update (cf AI_UPDATE_TYPES)
// Returns true if some specific treatments have been executed here = do NOT execute ROR standard treatment for current task
// Returns false otherwise (default) = let ROR execute normally its treatments for current task
// Warning *** this is supposed to take care of processing time, be carful about performance ***
bool EconomyAI::RunOneTacAIUpdateTask(STRUCT_TAC_AI *tacAI) {
	// Remark : tacAI.Update() in 0x4D1B70 calls alternately the various AI updates, including this one cf AI_UPDATE_TYPES::CST_AUT_EVAL_CIVILIAN_DISTRIB
	assert(tacAI && tacAI->IsCheckSumValid() && tacAI->ptrMainAI);
	if (!tacAI || !tacAI->ptrMainAI || !tacAI->ptrMainAI->ptrStructPlayer) { return false; }
	bool doNotRunRorUpdate = false;

	switch (tacAI->currentAIUpdateType) {
	case AI_UPDATE_TYPES::CST_AUT_EVAL_CIVILIAN_DISTRIB:
		// If needed resources have never been computed yet, let standard code execute. Next time the flag will be set.
		if ((tacAI->neededResourcesAreInitialized != 0) && (tacAI->allVillagers.usedElements > 0)) {
			doNotRunRorUpdate = EconomyAI::CalcVillagerCountByTask(tacAI);
		}
		break;
		/*case AI_UPDATE_TYPES::CST_AUT_SETUP_SOLDIER_GROUPS:
		case AI_UPDATE_TYPES::CST_AUT_TASK_CIVILIAN:
		case AI_UPDATE_TYPES::CST_AUT_SET_BOAT_GROUPS:
		case AI_UPDATE_TYPES::CST_AUT_FILL_BOAT_GROUPS:
		case AI_UPDATE_TYPES::CST_AUT_TASK_BOATS:
		case AI_UPDATE_TYPES::CST_AUT_FILL_SOLDIER_GROUPS:
		case AI_UPDATE_TYPES::CST_AUT_TASK_IDLE_SOLDIER:
		case AI_UPDATE_TYPES::CST_AUT_TASK_ACTIVE_SOLDIER:
		case AI_UPDATE_TYPES::CST_AUT_PLAYTASKING:
		case AI_UPDATE_TYPES::CST_AUT_TASK_UNGRP_SOLDIER:
		case AI_UPDATE_TYPES::CST_AUT_RESEARCH:
		case AI_UPDATE_TYPES::CST_AUT_TRAIN:
		case AI_UPDATE_TYPES::CST_AUT_BUILD_LIST:
		case AI_UPDATE_TYPES::CST_AUT_HELP_BUILD:
		case AI_UPDATE_TYPES::CST_AUT_REPAIR_BUILDING:
		case AI_UPDATE_TYPES::CST_AUT_REPAIR_WALL:
		case AI_UPDATE_TYPES::CST_AUT_BUILD:
		case AI_UPDATE_TYPES::CST_AUT_OPEN_BUILDS:
		case AI_UPDATE_TYPES::CST_AUT_OPEN_TASKS:
		case AI_UPDATE_TYPES::CST_AUT_FOOD_DROPSITE:
		case AI_UPDATE_TYPES::CST_AUT_BUILD_LIST_INSERTIONS:
		break;*/
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
	if ((totalVillagerCount < 3) && diffHardOrHardest && global && (global->currentGameTime > 60000)) {
		// This situation is not well handled by standard AI
		// TODO: do I have a TC ? Do I have food ?
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
	return doNotRunRorUpdate;
}


}
