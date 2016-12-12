#include "../include/unitHandling.h"


namespace AOE_STRUCTURES {

// Securely get an action pointer without having to re-write all checks/gets for intermediate objects.
// Return NULL if one of the objects is NULL/missing
AOE_STRUCTURES::STRUCT_ACTION_BASE *GetUnitAction(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->DerivesFromCommandable()) { return NULL; }
	AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *unitAsBird = (AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *)unit;
	if ((unitAsBird->ptrActionInformation == NULL) || (unitAsBird->ptrActionInformation->ptrActionLink == NULL)) {
		return NULL;
	}
	assert(unitAsBird->ptrActionInformation->IsCheckSumValid()); // This should never occur. Whereas having NULL pointers in action info/link structures is possible (and valid)
	if (!unitAsBird->ptrActionInformation->IsCheckSumValid()) { return NULL; }
	if (unitAsBird->ptrActionInformation->ptrActionLink == NULL) {
		return NULL;
	}
	return unitAsBird->ptrActionInformation->ptrActionLink->actionStruct;
}


// Return the total remaining food amount for a farm ("immediatly available" + "action-remaining").
// Returns 0 in error cases (please check it is actually a farm !)
float GetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit) {
	if (!farmUnit || !farmUnit->IsCheckSumValid()) { return 0; } // test BUILDING checksum
	if (farmUnit->resourceTypeId != RESOURCE_TYPES::CST_RES_ORDER_BERRY_STORAGE) { return 0; }
	if (!farmUnit->unitDefinition || !farmUnit->unitDefinition) { return 0; }
	if (farmUnit->unitDefinition->DAT_ID1 != CST_UNITID_FARM) { return 0; }
	// Farms are quite special. Their "resource value" increases slowly, like docks' trade value.
	// This system prevents from gathering faster when assigning >1 villager. (other limitation: each farmer also has a maximum work rate !)
	// Action contains the remaining "locked" (not available yet) amount.
	// Unit resource value contains available amount (immediatly gatherable amount).
	AOE_STRUCTURES::STRUCT_ACTION_BASE *action = GetUnitAction(farmUnit);
	if (!action) { return 0; }
	return action->timer + farmUnit->resourceValue;
}

// Modifies the total remaining food amount for a farm ("immediately available" + "action-remaining").
bool SetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit, float newAmount) {
	if (!farmUnit || !farmUnit->IsCheckSumValid()) { return false; } // test BUILDING checksum
	if (farmUnit->resourceTypeId != RESOURCE_TYPES::CST_RES_ORDER_BERRY_STORAGE) { return false; }
	if (!farmUnit->unitDefinition || !farmUnit->unitDefinition) { return false; }
	if (farmUnit->unitDefinition->DAT_ID1 != CST_UNITID_FARM) { return false; }
	AOE_STRUCTURES::STRUCT_ACTION_BASE *action = GetUnitAction(farmUnit);
	if (!action) { return false; }
	action->timer = newAmount - farmUnit->resourceValue;
	if (action->timer < 0) {
		action->timer = 0;
		farmUnit->resourceValue = newAmount;
	}
	return true;
}

// Returns current action target IF it is an attack action (including conversion)
STRUCT_UNIT_BASE *GetAttackTarget(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->ptrStructPlayer || !unit->ptrStructPlayer->ptrGlobalStruct) { return false; }
	STRUCT_ACTION_BASE *commanderAction = AOE_STRUCTURES::GetUnitAction(unit);
	if (!commanderAction) {
		return NULL;
	}
	if ((commanderAction->actionTypeID != UNIT_ACTION_ID::CST_IAI_ATTACK_9) && (commanderAction->actionTypeID != UNIT_ACTION_ID::CST_IAI_CONVERT)) {
		return NULL;
	}
	STRUCT_UNIT_BASE *targetUnit = commanderAction->targetUnit;
	if (!targetUnit) {
		targetUnit = unit->ptrStructPlayer->ptrGlobalStruct->GetUnitFromId(commanderAction->targetUnitId);
	}
	return targetUnit;
}


// Returns true if unit is currently fighting against (or moving towards) a MILITARY target (excluding houses, villagers, etc)
// Returns false if target is not visible or out of reach
bool HasVisibleMilitaryTarget(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->ptrStructPlayer || !unit->ptrStructPlayer->ptrGlobalStruct) { return false; }
	STRUCT_UNIT_BASE *targetUnit = GetAttackTarget(unit);
	if (!targetUnit) { return false; }
	if (!IsFogVisibleForPlayer(unit->ptrStructPlayer->playerId, (long int)targetUnit->positionX, (long int)targetUnit->positionY)) {
		return false;
	}
	return UnitDefCanAttack(targetUnit->unitDefinition);
}


// Return the target position for a unit/action
// The result might be {-1, -1}
std::pair<float, float> GetActionTargetPosition(STRUCT_ACTION_BASE *action) {
	if (!action) { return std::pair<float, float>(-1.f, -1.f); }
	if (action->actionTypeID == UNIT_ACTION_ID::CST_IAI_MOVE_1) {
		return std::pair<float, float>(action->targetUnitPositionX, action->targetUnitPositionY);
	}
	std::pair<float, float> result;
	// Recursively search for an underlying MOVE "required" action
	if (action->requiredActionInfo && action->requiredActionInfo->ptrActionLink && action->requiredActionInfo->ptrActionLink->actionStruct) {
		result = GetActionTargetPosition(action->requiredActionInfo->ptrActionLink->actionStruct);
		if ((result.first > -1) && (result.second > -1)) { return result; }
	}
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return result; }
	STRUCT_UNIT_BASE *targetUnit = action->targetUnit;
	if (!targetUnit) {
		targetUnit = global->GetUnitFromId(action->targetUnitId);
	}
	if (targetUnit && targetUnit->IsCheckSumValidForAUnitClass()) {
		result.first = targetUnit->positionX;
		result.second = targetUnit->positionY;
	}
	return result;
}

// Return the target position for a unit/action
// The result might be {-1, -1}
std::pair<float, float> GetActionTargetPosition(STRUCT_UNIT_BASE *unit) {
	STRUCT_ACTION_BASE *action = AOE_STRUCTURES::GetUnitAction(unit);
	return GetActionTargetPosition(action);
}


// Returns true if "player" knows where targetUnit is located: unit is visible OR its location is explored and its speed=0
bool IsUnitPositionKnown(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *targetUnit) {
	if (!player || !player->IsCheckSumValid()) { return false; }
	if (!targetUnit || !targetUnit->IsCheckSumValidForAUnitClass()) { return false; }
	if (GetUnitDefSpeed(targetUnit->unitDefinition) <= 0) {
		return IsExploredForPlayer(player->playerId, (long int)targetUnit->positionX, (long int)targetUnit->positionY);
	} else {
		return IsFogVisibleForPlayer(player->playerId, (long int)targetUnit->positionX, (long int)targetUnit->positionY);
	}
}

}

