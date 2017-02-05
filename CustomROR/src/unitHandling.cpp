#include "../include/unitHandling.h"


namespace AOE_STRUCTURES {
;


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
	if ((commanderAction->actionTypeID != UNIT_ACTION_ID::CST_IAI_ATTACK) && (commanderAction->actionTypeID != UNIT_ACTION_ID::CST_IAI_CONVERT)) {
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
	if (!PLAYER::IsFogVisibleForPlayer(unit->ptrStructPlayer, (long int)targetUnit->positionX, (long int)targetUnit->positionY)) {
		return false;
	}
	return UnitDefCanAttack(targetUnit->unitDefinition);
}


// Return the target position for a unit/action
// The result might be {-1, -1}
std::pair<float, float> GetActionTargetPosition(STRUCT_ACTION_BASE *action) {
	if (!action) { return std::pair<float, float>(-1.f, -1.f); }
	if (action->actionTypeID == UNIT_ACTION_ID::CST_IAI_MOVE_TO) {
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
		return PLAYER::IsExploredForPlayer(player, (long int)targetUnit->positionX, (long int)targetUnit->positionY);
	} else {
		return PLAYER::IsFogVisibleForPlayer(player, (long int)targetUnit->positionX, (long int)targetUnit->positionY);
	}
}


// Change line of sight for a unique unit. Fails if unit does NOT have a dedicated unit definition.
bool ChangeLOSForUniqueUnit(STRUCT_UNIT_TRAINABLE *unit, float newLOS) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->DerivesFromTrainable() || !unit->unitDefinition || !unit->unitDefinition->DerivesFromTrainable()) {
		return false;
	}
	if (!unit->hasDedicatedUnitDef) {
		return false;
	}
	if ((unit->unitDefinition->unitType == GUT_BUILDING) && (unit->unitStatus == AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_0_NOT_BUILT)) {
		AOE_METHODS::UNIT::RemoveVisibility(unit, unit->ptrStructPlayer, 1, -1);
	} else {
		AOE_METHODS::UNIT::RemoveVisibility(unit, unit->ptrStructPlayer, 0, -1);
	}
	unit->unitDefinition->lineOfSight = newLOS;
	if ((unit->unitDefinition->unitType == GUT_BUILDING) && (unit->unitStatus == AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_0_NOT_BUILT)) {
		AOE_METHODS::UNIT::AddVisibility(unit, unit->ptrStructPlayer, 1, -1);
	} else {
		AOE_METHODS::UNIT::AddVisibility(unit, unit->ptrStructPlayer, 0, -1);
	}
	return true;
}


// Given a list of (actor) units, tell them to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitsToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE **actorUnitsList, long int actorUnitsCount,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *target) {
	if (!actorUnitsList || (actorUnitsCount <= 0) || !target || !target->IsCheckSumValidForAUnitClass()) { return false; }
	assert(actorUnitsCount < 255); // More would be... more than huge !
	if (actorUnitsCount >= 255) { return false; } // This can't be normal
	float posX = target->positionX;
	float posY = target->positionY;
	GAME_COMMANDS::CreateCmd_RightClick(actorUnitsList, actorUnitsCount, target->unitInstanceId, target->positionX, target->positionY);
	return true;
}


// Tell a unit to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actorUnit, AOE_STRUCTURES::STRUCT_UNIT_BASE *target) {
	if (!actorUnit || !actorUnit->DerivesFromCommandable()) { return false; }
	return TellUnitsToInteractWithTarget(&actorUnit, 1, target);
}


// Calls ROR's method to change a unit's action so it will move to supplied unit/position
// target can be NULL (only position will matter)
// unitToMove->ptrActionInformation is required to be NON-NULL ! Or the method will return without doing anything.
// Compatible with MP, but in MP unit will not defend itself if attacked.
void MoveUnitToTargetOrPosition(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *unitToMove, AOE_STRUCTURES::STRUCT_UNIT_BASE *target, float posX, float posY) {
	if (!unitToMove) { return; }

	if (IsMultiplayer()) {
		// Manage MP games : use a command (but unit will not defend itself if attacked)
		if (!unitToMove || !unitToMove->IsCheckSumValidForAUnitClass()) {
			return;
		}
		long int targetId = -1;
		if (target && target->IsCheckSumValidForAUnitClass()) { targetId = target->unitInstanceId; }
		GAME_COMMANDS::CreateCmd_RightClick(unitToMove->unitInstanceId, targetId, posX, posY);
		return;
	}

	// unit.createMoveToAction(targetUnitStruct,pos,pos,arg4) does NOT support if action info struct does not exist.
	if (!unitToMove->ptrActionInformation) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = unitToMove->ptrStructPlayer;

	_asm {
		MOV ECX, unitToMove;
		MOV EAX, 0x405DB0; // unit.createMoveToAction(targetUnitStruct,pos,pos,arg4)
		PUSH 0; // posZ?, 0 in game call when using mouse click, so we do the same
		PUSH posX;
		PUSH posY;
		PUSH target;
		CALL EAX;
	}
	if (unitToMove->currentActivity) {
		unitToMove->currentActivity->orderTaskId = CST_ATI_NONE; // (-1) force to defend itself when attacked.
	}
}


// Returns true if unit (typically, a trade boat) can trade with target unit (typically, a building like dock)
bool CanTradeWithUnitDef(STRUCT_UNIT_BASE *unit, long int targetUnitDefId) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->DerivesFromTrainable()) {
		// Note: in standard game, only trainable and building units can trade. For other unit types, DS:[EDX+0x138] points to a "return 0" function.
		return false;
	}
	if (!unit->ptrStructPlayer || !unit->ptrStructPlayer->IsCheckSumValid() || !unit->ptrStructPlayer->ptrStructDefUnitTable ||
		!unit->unitDefinition || !unit->unitDefinition->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	
	AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *unitDefCommandable = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)unit->unitDefinition;
	if (!unitDefCommandable->ptrUnitCommandHeader) { return false; }

	STRUCT_PLAYER *player = unit->ptrStructPlayer;
	// This requires unitDef to be the same for all players (which is a strong AOE requirement anyway)
	STRUCT_UNITDEF_BASE *targetUnitDef = player->GetUnitDefBase((short int)targetUnitDefId);
	if (!targetUnitDef || !targetUnitDef->IsCheckSumValidForAUnitClass()) {
		return false;
	}

	// Does unit (def) have a command to trade with target unit (def) ?
	for (int i = 0; i < unitDefCommandable->ptrUnitCommandHeader->commandCount; i++) {
		if ((unitDefCommandable->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_TRADE) &&
			((unitDefCommandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitDefId == targetUnitDef->DAT_ID1) ||
			(unitDefCommandable->ptrUnitCommandHeader->ptrCommandArray[i]->classId == targetUnitDef->unitAIType))) {
			return UnitDefOffersTrading(targetUnitDef);
		}
	}
	return false;
}


// Returns true if it is possible to trade with this unit (it has trade goods in its resource storage)
bool UnitOffersTrading(STRUCT_UNIT_BASE *unit) {
	// We could also check resource storage in unitDef.
	// It may only differ if at some point during the game we change unit's resourceTypeId into CST_RES_ORDER_TRADE_GOODS.
	return unit && unit->unitDefinition && UnitDefOffersTrading(unit->unitDefinition);
}


// Returns true if it is possible to trade with this unit (it has trade goods in its resource storage, typically with enableMode=0)
bool UnitDefOffersTrading(STRUCT_UNITDEF_BASE *unitDef) {
	if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	return (unitDef->resourceStorageType_1 == CST_RES_ORDER_TRADE_GOODS) ||
		(unitDef->resourceStorageType_2 == CST_RES_ORDER_TRADE_GOODS) ||
		(unitDef->resourceStorageType_3 == CST_RES_ORDER_TRADE_GOODS);
}


}
