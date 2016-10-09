#include "../include/AOEPrimitives_units.h"


namespace AOE_METHODS {

// Exact role to confirm.
// MAYBE this method allows finding path with enemy units blocking the way. Such units are added to path finding struct's unitid array (unknown_11DCE4) ?
// arg6: seen -1 or 0x1B (hardcoded). Could be a AI unit class (walls) ?
// tempList should be empty in input, filled by this method. Please free the array (if non-NULL) afterwards !
// Call [unit+0x1A0], for example 0x44E130.
bool MoveToTarget_1A0(STRUCT_UNIT_BASE *unit, long int targetUnitId, float range, long int arg3,
	long int arg4, long int targetPlayerId, long int arg6, STRUCT_AI_UNIT_LIST_INFO *tempList) {
	assert(unit && unit->IsCheckSumValidForAUnitClass());
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	long int callResult;
	_asm {
		MOV ECX, unit;
		PUSH tempList;
		PUSH arg6;
		PUSH targetPlayerId;
		PUSH arg4;
		PUSH arg3;
		PUSH range;
		PUSH unit;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX+0x1A0];
		MOV callResult, EAX;
	}
	return (callResult != 0);
}


// Add a unit to all relevant infAI lists. Cf 0x4BE0D0
bool AddUnitInInfAILists(STRUCT_INF_AI *infAI, long int unitId) {
	assert(infAI && infAI->IsCheckSumValid());
	if (!infAI || !infAI->IsCheckSumValid()) { return false; }
	unsigned long int callAddr = 0x4BE0D0;
	_asm {
		MOV ECX, infAI;
		PUSH unitId;
		CALL callAddr;
	}
	return true;
}


// Returns true if a unit can convert another (actually, for priests)
bool CanConvert(STRUCT_UNIT_BASE *unit, long int targetUnitId) {
	assert(unit && unit->IsCheckSumValidForAUnitClass());
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || (targetUnitId < 0)) {
		return false;
	}
	STRUCT_UNIT_ACTIVITY *activity = unit->currentActivity;
	if (!activity) { return false; }
	long int result;
	_asm {
		MOV ECX, activity;
		PUSH targetUnitId;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0xC4];
		MOV result, EAX;
	}
	return (result != 0);
}


// Gets the damage from an attacked over a target (defender)
float CalcDamage(STRUCT_UNIT_BASE *attacker, STRUCT_UNIT_BASE *defender) {
	if (!attacker || !attacker->IsCheckSumValidForAUnitClass()) { return 0; }
	if (!defender || !defender->IsCheckSumValidForAUnitClass()) { return 0; }
	float result;
	_asm {
		MOV ECX, defender;
		PUSH attacker;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x104]; // returns 0 for classes with no attack
		FSTP DS:[result]; // REQUIRED to compensate the FLD from called method (for float stack consistency)
	}
	return result;
}

// All-unit classes compatible getter for reload time.
float GetReloadTime1(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return 0; }
	float result;
	_asm {
		MOV ECX, unit;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x100]; // returns 0 for classes with no reload attribute
		FSTP DS:[result]; // REQUIRED to compensate the FLD from called method (for float stack consistency)
	}
	return result;
}

// All unit-classes-compatible getter for speed.
float GetSpeed(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return 0; }
	float result;
	_asm {
		MOV ECX, unit;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0xFC]; // returns 0 for classes with no speed attribute
		FSTP DS:[result]; // REQUIRED to compensate the FLD from called method (for float stack consistency)
	}
	return result;
}


// Returns true if a unit is idle
bool IsUnitIdle(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	int result;
	_asm {
		MOV ECX, unit;
		MOV EDX, DS:[ECX]; //0x00406610
		CALL DS:[EDX + 0x210];
		MOV result, EAX;
	}
	return (result != 0);
}



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
	return action->unsure_resourceValue + farmUnit->resourceValue;
}

// Modifies the total remaining food amount for a farm ("immediately available" + "action-remaining").
bool SetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit, float newAmount) {
	if (!farmUnit || !farmUnit->IsCheckSumValid()) { return false; } // test BUILDING checksum
	if (farmUnit->resourceTypeId != RESOURCE_TYPES::CST_RES_ORDER_BERRY_STORAGE) { return false; }
	if (!farmUnit->unitDefinition || !farmUnit->unitDefinition) { return false; }
	if (farmUnit->unitDefinition->DAT_ID1 != CST_UNITID_FARM) { return false; }
	AOE_STRUCTURES::STRUCT_ACTION_BASE *action = GetUnitAction(farmUnit);
	if (!action) { return false; }
	action->unsure_resourceValue = newAmount - farmUnit->resourceValue;
	if (action->unsure_resourceValue < 0) {
		action->unsure_resourceValue = 0;
		farmUnit->resourceValue = newAmount;
	}
	return true;
}



}

