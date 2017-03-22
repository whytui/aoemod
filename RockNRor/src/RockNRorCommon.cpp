#include "../include/RockNRorCommon.h"


// Calculate distance between 2 points (without optimization)
float GetDistance(float x1, float y1, float x2, float y2) {
	float dx = x1 - x2;
	float dy = y1 - y2;
	dx = dx * dx;
	dy = dy * dy;
	return sqrt(dx + dy);
}


// If resourceTable has enough resources, returns true and deduces the cost from resourceTable.
// If not, returns false and does not modify any value.
bool ApplyCostIfPossible(float costTable[], float resourceTable[]) {
	if ((costTable[CST_RES_ORDER_FOOD] > 0 && (costTable[CST_RES_ORDER_FOOD] > resourceTable[CST_RES_ORDER_FOOD])) ||
		(costTable[CST_RES_ORDER_WOOD] > 0 && (costTable[CST_RES_ORDER_WOOD] > resourceTable[CST_RES_ORDER_WOOD])) ||
		(costTable[CST_RES_ORDER_STONE] > 0 && (costTable[CST_RES_ORDER_STONE] > resourceTable[CST_RES_ORDER_STONE])) ||
		(costTable[CST_RES_ORDER_GOLD] > 0 && (costTable[CST_RES_ORDER_GOLD] > resourceTable[CST_RES_ORDER_GOLD]))
		) {
		return false;
	}
	resourceTable[CST_RES_ORDER_FOOD] -= costTable[CST_RES_ORDER_FOOD];
	resourceTable[CST_RES_ORDER_WOOD] -= costTable[CST_RES_ORDER_WOOD];
	resourceTable[CST_RES_ORDER_STONE] -= costTable[CST_RES_ORDER_STONE];
	resourceTable[CST_RES_ORDER_GOLD] -= costTable[CST_RES_ORDER_GOLD];
	return true;
}


bool GetUnitCost(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID, float costTable[]) {
	if (!player || !player->IsCheckSumValid()) { return false; }
	if ((DAT_ID < 0) || (DAT_ID >= player->structDefUnitArraySize)) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *defUnit = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE*)player->GetUnitDefBase(DAT_ID);
	if (!defUnit || !defUnit->DerivesFromTrainable()) { return false; }
	for (int i = 0; i < 4; i++) { costTable[i] = 0; }
	if (defUnit->costs[0].costPaid) {
		if (defUnit->costs[0].costType > MAX_RESOURCE_TYPE_ID) { return false; }
		costTable[defUnit->costs[0].costType] = (float)defUnit->costs[0].costAmount;
	}
	if (defUnit->costs[1].costPaid) {
		if (defUnit->costs[1].costType > MAX_RESOURCE_TYPE_ID) { return false; }
		costTable[defUnit->costs[1].costType] = (float)defUnit->costs[1].costAmount;
	}
	if (defUnit->costs[2].costPaid) {
		if (defUnit->costs[2].costType > MAX_RESOURCE_TYPE_ID) { return false; }
		costTable[defUnit->costs[2].costType] = (float)defUnit->costs[2].costAmount;
	}
	return true;
}


// Fill resourceTypesOrder with ordered resource types: lower value in resourceAmounts = first position in (out) resourceTypesOrder
void SortResourceTypes(const int resourceAmounts[], int resourceTypesOrder[]) {
	int costsOrder_current[4] = { 0, 0, 0, 0 };
	bool valueUsed[4] = { false, false, false, false };
	for (int orderedArrayIndex = 0; orderedArrayIndex < 4; orderedArrayIndex++) {
		int lowestCost;
		bool noValue = true;
		int currentSelection;
		for (int i = 0; i < 4; i++) {
			if (!valueUsed[i] && (noValue || (resourceAmounts[i] < lowestCost))) {
				lowestCost = resourceAmounts[i];
				currentSelection = i;
				noValue = false;
			}
		}
		resourceTypesOrder[orderedArrayIndex] = currentSelection;
		valueUsed[currentSelection] = true;
	}
}


// Returns true for maps where AI does build a dock and boats. Warning: for unknown map type (custom), this returns true.
bool IsDockRelevantForMap(MAP_TYPE_INDEX mti) {
	MAP_WATER_TYPE m = GetMapWaterType(mti);
	return (m != MAP_WATER_TYPE::MWT_ALL_LAND_MAP) && (m != MAP_WATER_TYPE::MWT_MOSTLY_LAND_MAP);
}



// Update an element in infAI.unitElemList if the unit is visible.
// Reset the element otherwise.
// If the element is reset, it is ALSO REMOVED from infAI lists.
// Return true if the element was updated/reset.
#pragma message("UpdateOrResetInfAIUnitListElem should no longer be useful for unit 50+ if unitExtension works ?")
bool UpdateOrResetInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *elem) {
	if (!infAI || !infAI->IsCheckSumValid() || !elem || !infAI->ptrMainAI) { return false; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(elem->unitId);
	assert((unitBase == NULL) || (unitBase->IsCheckSumValidForAUnitClass()));
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = NULL;
	long int actorPlayerId = infAI->commonAIObject.playerId;
	long int unitPlayerId = -1;
	if (unitBase) {
		unitDefBase = unitBase->unitDefinition;
		assert(unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass());
		if (unitBase->ptrStructPlayer && unitBase->ptrStructPlayer->IsCheckSumValid()) {
			unitPlayerId = unitBase->ptrStructPlayer->playerId;
		}
	}
	bool unitItVisible = false;
	if (unitBase && unitDefBase) {
		if (unitDefBase->visibleInFog) {
			// The unit still exists, and is visible through fog: only remove it from the list if its position is not EXPLORED.
			unitItVisible = PLAYER::IsExploredForPlayer(infAI->ptrMainAI->player, (long int)unitBase->positionX, (long int)unitBase->positionY);
		} else {
			// Standard case, check fog visibility.
			unitItVisible = PLAYER::IsFogVisibleForPlayer(infAI->ptrMainAI->player, (long int)unitBase->positionX, (long int)unitBase->positionY);
		}
	}
	if (unitItVisible) {
		// Update the element because I can see it (no cheating)
		elem->playerId = (char)unitPlayerId; // the most important update
		if (unitBase) { // should always be true here
			elem->posX = (unsigned char)unitBase->positionX;
			elem->posY = (unsigned char)unitBase->positionY;
			elem->HP = (short)unitBase->remainingHitPoints;
		}
		if (unitDefBase && unitDefBase->DerivesFromAttackable()) {
			// By the way... This part is not really necessary.
			AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDef50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDefBase;
			elem->maxRange = unitDef50->maxRange;
			elem->reloadTime1 = unitDef50->reloadTime1;
			//elem->attack = unitDef50->displayedAttack;
		}
		return true;
	} else { // No longer visible
		// First remove from infAI lists, if needed. (do it while elem has not been reset... yet)
		AOE_METHODS::LISTS::RemoveFromInfAIInfoList(infAI, elem->unitId, elem->unitDATID, elem->unitClass);

		// Reset the element
		return AOE_METHODS::LISTS::ResetInfAIUnitListElem(elem);
	}
	return false;
}


// Change unit owner in InfAI unitListElem according to unit visibility
// Return true if updated.
bool UpdateUnitOwnerInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit,
	long int newPlayerId) {
	if (!infAI || !infAI->IsCheckSumValid() || !infAI->ptrMainAI) { return false; }
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	if (newPlayerId < 0) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unit->unitDefinition;
	if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) { return false; }
	if (unit->unitInstanceId < 0) { return false; } // Ignore temp units.
	bool isVisible = false;
	if (unitDefBase->visibleInFog) {
		isVisible = PLAYER::IsExploredForPlayer(infAI->ptrMainAI->player, (long int)unit->positionX, (long int)unit->positionY);
	} else {
		isVisible = PLAYER::IsFogVisibleForPlayer(infAI->ptrMainAI->player, (long int)unit->positionX, (long int)unit->positionY);
	}
	// Search for unit in infAI unit elem list
	long int size = infAI->unitElemListSize;
	for (long int i = 0; i < size; i++) {
		AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *curElem = &infAI->unitElemList[i];
		if (curElem->unitId == unit->unitInstanceId) {
			if (isVisible) {
				curElem->playerId = (char)newPlayerId;
				return true;
			} else {
				return false; // Not visible=no update. Exploring should do the update.
			}
		}
	}
	return false;
}


