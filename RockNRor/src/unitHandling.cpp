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
	GAME_COMMANDS::CreateCmd_RightClick((AOE_STRUCTURES::STRUCT_UNIT_BASE**)actorUnitsList,
		actorUnitsCount, target->unitInstanceId, target->positionX, target->positionY);
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
		unitToMove->currentActivity->orderId = CST_ORDER_NONE; // (-1) force to defend itself when attacked.
	}
}


// Update current action's status to "search" so that it will search for another target (if any)
void ForceUnitChangeTarget(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return; }
	STRUCT_UNIT_ACTIVITY *activity = unit->currentActivity;
	STRUCT_ACTION_BASE *action = GetUnitAction(unit);
	if (!activity || !action) { return; }
	activity->targetUnitId = -1;
	activity->targetUnitType = GLOBAL_UNIT_AI_TYPES::TribeAINone;
	action->targetUnit = NULL;
	action->targetUnitId = -1;
	AOE_METHODS::UNIT::UnitActionSetStatus(action, ACTION_STATUS::CST_AS_03_SEARCH);
}


// Returns true if targetUnitDefId creates renewable resource:
// - farms (immediately available food is limited and increases progressively)
// - docks or trade buildings (trade goods are unlimited, but need time to resplenish)
bool CanGetRenewableResourceFrom(STRUCT_UNIT_BASE *unit, long int targetUnitDefId) {
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
		if ((unitDefCommandable->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_GATHER_NO_ATTACK) &&
			((unitDefCommandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitDefId == targetUnitDef->DAT_ID1) ||
			(unitDefCommandable->ptrUnitCommandHeader->ptrCommandArray[i]->classId == targetUnitDef->unitAIType))) {
			// In theory, we should check if target unit def "produces" food (action 21="make").
			// But anyway, the initial role of ROR method is unclear (mixing farming and trading is weird + it does not take care of actor unit !)
			return true;
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


// Returns the "out" resource Id for trading with target unit, taking into account the correct unitDefCommand.
long int GetTradeOutResourceId(STRUCT_UNIT_BASE *actor, STRUCT_UNIT_BASE *targetUnit) {
	if (!actor || !actor->IsCheckSumValidForAUnitClass() ||
		!targetUnit || !targetUnit->IsCheckSumValidForAUnitClass()) {
		return -1;
	}
#ifdef _DEBUG
	assert(UnitDefOffersTrading(targetUnit->unitDefinition));
#endif
	if (!actor->DerivesFromCommandable()) { return -1; }
	STRUCT_UNITDEF_COMMANDABLE *unitDef = (STRUCT_UNITDEF_COMMANDABLE *)actor->unitDefinition;
	STRUCT_UNITDEF_BASE *targetDef = targetUnit->unitDefinition;
	assert(unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromCommandable());
	assert(targetDef && targetDef->IsCheckSumValidForAUnitClass());
	if (!unitDef || !unitDef->ptrUnitCommandHeader || !targetDef) { return -1; }
	for (int i = 0; i < unitDef->ptrUnitCommandHeader->commandCount; i++) {
		if ((unitDef->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_TRADE) &&
			((targetDef->DAT_ID1 == unitDef->ptrUnitCommandHeader->ptrCommandArray[i]->unitDefId) || (targetDef->unitAIType == unitDef->ptrUnitCommandHeader->ptrCommandArray[i]->classId))
			) {
			return unitDef->ptrUnitCommandHeader->ptrCommandArray[i]->resourceTypeOut;
		}
	}
	return -1;
}


// Returns true if killed unit should keep its owned resource (food for gazelle, etc) when killed = most cases (default)
// Returns false if killed unit should lose its owned resource when killed = for example, gazelle killed by a military unit.
bool ShouldKeepOwnedResourceWhenKilledByClass(STRUCT_UNIT_BASE *killedUnit, GLOBAL_UNIT_AI_TYPES attackerClass) {
	if (!killedUnit || !killedUnit->IsCheckSumValidForAUnitClass()) { return true; }
	if (!killedUnit->unitDefinition || !killedUnit->unitDefinition->IsCheckSumValidForAUnitClass()) { return true; }
	GLOBAL_UNIT_AI_TYPES killedUnitClass = killedUnit->unitDefinition->unitAIType;
	if ((killedUnitClass != TribeAIGroupPreyAnimal) && (killedUnitClass != TribeAIGroupPredatorAnimal)) {
		return true; // Always "preserve resource" for non-animal units.
	}
	// Here killed unit is an animal
	return (
		(attackerClass == TribeAIGroupCivilian) || // standard
		(attackerClass == TribeAIGroupPredatorAnimal) || // added instead of TribeAIGroupOther_Dead_Projectile (bug in original code ?)
		(attackerClass == TribeAIGroupPreyAnimal) || // added (after all, a gazelle killing another one should not destroy underlying food).
		(attackerClass == TribeAIGroupDomesticatedAnimal)); // added (seems logical)
}


// Returns true if a builder can become farmer to gather the farm he just built
// Returns false if he should not (because there already is a farmer, because he already carries a different resource)
// This does not detect pending commands (if some villager has just been told to gather this farm)
bool CanBuilderSwitchToFarmer(STRUCT_UNIT_BASE *builder, STRUCT_UNIT_BASE *farm) {
	if (!builder || !builder->IsCheckSumValidForAUnitClass() || !farm || !farm->IsCheckSumValidForAUnitClass()) { return false; }
	if (!builder->ptrStructPlayer || !builder->ptrStructPlayer->ptrCreatableUnitsListLink) { return false; }
	if (farm->unitStatus != GAME_UNIT_STATUS::GUS_2_READY) { return false; }
	if (!farm->unitDefinition || farm->unitDefinition->DAT_ID1 != CST_UNITID_FARM) {
		return false;
	}

	if ((builder->resourceValue > 0) && (builder->resourceTypeId != CST_RES_ORDER_BERRY_STORAGE)) {
		return false; // builder carries another resource type : first save it into a drop site.
	}

	// This does not detect pending commands !
	STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *curElemVillager = builder->ptrStructPlayer->ptrCreatableUnitsListLink->lastListElement;
	while (curElemVillager) {
		if (curElemVillager->unit && curElemVillager->unit->IsCheckSumValidForAUnitClass()) {
			AOE_STRUCTURES::STRUCT_UNIT_BASE *curUnit = curElemVillager->unit; // a potential farmer on "our" farm
			// Beware repairmen ! (AI bug: their unitDef remains farmers). Here we check action=gather to avoid this.
			if ((curUnit != builder) &&
				(curUnit->unitStatus == GAME_UNIT_STATUS::GUS_2_READY) && curUnit->unitDefinition &&
				curUnit->unitDefinition->IsCheckSumValidForAUnitClass() && (curUnit->unitDefinition->DAT_ID1 == CST_UNITID_FARMER)) {
				STRUCT_ACTION_BASE *action = AOE_STRUCTURES::GetUnitAction(curUnit);
				STRUCT_ACTION_GATHER_WITHOUT_ATTACK *actionGather = NULL;
				if (action && (action->actionTypeID == UNIT_ACTION_ID::CST_IAI_GATHER_NO_ATTACK)) {
					actionGather = (STRUCT_ACTION_GATHER_WITHOUT_ATTACK *)action;
				}
				if (actionGather && (actionGather->targetUnitId == farm->unitInstanceId)) {
					// We found a GATHERER on this farm (note: NOT a repairman or a builder)
					return false; // The farm is already being gathered by someone else
				}
			}
		}
		curElemVillager = curElemVillager->previousElement;
	}

	return true; // Default = other cases
}


// Returns the diplomacy stance regarding "unitOther", from unitMe's point of view.
// ***WARNING*** this returns "neutral" for "self". You might prefer using GetDiplomacyValueForPlayer.
// Returns CST_PDS_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES GetDiplomacyStanceForUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitMe, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitOther) {
	if (!unitMe) {
		return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_UNKNOWN;
	}
	return GetDiplomacyStanceForUnit(unitMe->ptrStructPlayer, unitOther);
}

// Returns the diplomacy stance regarding "unitOther", from playerMe's point of view.
// ***WARNING*** this returns "neutral" for "self". You might prefer using GetDiplomacyValueForPlayer.
// Returns CST_PDS_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES GetDiplomacyStanceForUnit(AOE_STRUCTURES::STRUCT_PLAYER *playerMe, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitOther) {
	if (!unitOther) {
		return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_UNKNOWN;
	}
	return AOE_STRUCTURES::PLAYER::GetDiplomacyStanceForPlayer(playerMe, unitOther->ptrStructPlayer);
}

// Returns the diplomacy value regarding "unitOther", from unitMe's point of view.
// Returns CST_PDV_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES GetDiplomacyValueForUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitMe, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitOther) {
	if (!unitMe) {
		return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES::CST_PDV_UNKNOWN;
	}
	return GetDiplomacyValueForUnit(unitMe->ptrStructPlayer, unitOther);
}

// Returns the diplomacy value regarding "unitOther", from playerMe's point of view.
// Returns CST_PDV_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES GetDiplomacyValueForUnit(AOE_STRUCTURES::STRUCT_PLAYER *playerMe, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitOther) {
	if (!unitOther) {
		return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES::CST_PDV_UNKNOWN;
	}
	return AOE_STRUCTURES::PLAYER::GetDiplomacyValueForPlayer(playerMe, unitOther->ptrStructPlayer);
}


// Delete all units whose "definition ID" is not a vanilla-AOE one.
// To be used in scenario editor
bool DeleteAllNonVanillaAoeUnits() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) {
		return false;
	}
	for (int i = 0; i < global->seqUnitId; i++) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = global->GetUnitFromId(i);
		if (unit && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition) {
			if (!AOE_CONST_FUNC::IsVanillaAoeUnit(unit->unitDefinition->DAT_ID1)) {
				AOE_METHODS::UNIT::CallUnitDestructor(unit, true);
			}
		}
	}
	return true;
}


}
