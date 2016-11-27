#include "../include/EconomyAI.h"

namespace CUSTOM_AI {
;

void EconomyAI::ResetAllInfo() {
	this->lastVillagersFix_ms = 0;
}


// Run various fixes on villager actions, provided that a sufficient delay has passed since last execution
void EconomyAI::RunFixesOnVillagerActions(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid() || !player->ptrGlobalStruct || !player->ptrGlobalStruct->IsCheckSumValid()) {
		return;
	}
	unsigned long int currentGameTime = player->ptrGlobalStruct->currentGameTime;
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
				STRUCT_ACTION_BASE *action = AOE_METHODS::GetUnitAction(unit);
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
				STRUCT_ACTION_BASE *action = AOE_METHODS::GetUnitAction(unit);
				if (action && (action->actionTypeID == UNIT_ACTION_ID::CST_IAI_REPAIR)) {
					int actionStatus = action->actionStatus;
					bool foundMoveAction = false;
					STRUCT_UNIT_ACTION_INFO *uai = action->requiredActionInfo;
					if (uai && uai->ptrActionLink) {
						STRUCT_ACTION_BASE *reqAction = uai->ptrActionLink->actionStruct;
						if (reqAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_MOVE_1) {
							foundMoveAction = true;
						}
					}
					if (!foundMoveAction && (actionStatus == ACTION_STATUS::CST_AS_UNKNOWN_0A_STUCK)) {
						// Experimental: not sure the conditions match the case that causes problems in game ! But first tests seem ok !
#ifdef _DEBUG
						GetGameSettingsPtr()->AddEventInHistory((long int)unit->positionX, (long int)unit->positionY);
						CallWriteCenteredText("Unblocked a repairman", 2);
#endif
						GAME_COMMANDS::CreateCmd_Stop(unit->unitInstanceId);
					}
#ifdef _DEBUG
					// TMP
					else {
						if (actionStatus == ACTION_STATUS::CST_AS_UNKNOWN_0A_STUCK) {
							GetGameSettingsPtr()->AddEventInHistory((long int)unit->positionX, (long int)unit->positionY);
							CallWriteCenteredText("Found action status 0x0A (stuck unit?)", 2);
						}
					}
#endif
				}
			}
		}
		curElem = curElem->previousElement;
	}
}

}
