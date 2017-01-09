#include "../include/AOEPrimitives_units.h"


namespace AOE_METHODS {

// Exact role to confirm.
// MAYBE this method allows finding path with enemy units blocking the way. Such units are added to path finding struct's unitid array (unknown_11DCE4) ?
// arg6: seen -1 or 0x1B (hardcoded). Could be a AI unit class (walls) ?
// tempList should be empty in input, filled by this method. Please free the array (if non-NULL) afterwards !
// Call [unit+0x1A0], for example 0x44E130.
bool MoveToTarget_1A0(STRUCT_UNIT_BASE *unit, long int targetUnitId, float range, long int arg3,
	long int arg4, long int targetPlayerId, long int arg6, STRUCT_MANAGED_ARRAY *tempList) {
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


// Make progress in building construction according to timeToAdd (in seconds)
void BuildingUnitDoConstruct(STRUCT_UNIT_BUILDING *building, float timeToAdd_seconds) {
	if (!building || !building->IsCheckSumValid()) {
		return;
	}
	_asm {
		PUSH timeToAdd_seconds;
		MOV ECX, building;
		MOV EAX, 0x4AD7B0; // For ROR 1.0c
		CALL EAX;
	}
}


// Create unit activity using derived class that corresponds to supplied checksum (calls the correct constructor)
// Any unit (even base type) can have an activity, even if ROR originally only does that for living&building.
// Returns true if successful.
bool CreateUnitActivity(STRUCT_UNIT_BASE *unit, unsigned long int activityClassChecksum) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !activityClassChecksum) { return false; }
	if (!isAValidRORChecksum(activityClassChecksum)) { return false; }
	unsigned long int ccorAddr = STRUCT_UNIT_ACTIVITY::ConstructorAddress(activityClassChecksum);
	if (!ccorAddr) { return false; }
	STRUCT_UNIT_ACTIVITY *activity = (STRUCT_UNIT_ACTIVITY*)AOEAllocZeroMemory(1, sizeof(STRUCT_UNIT_ACTIVITY));
	if (!activity) { return false; }
	STRUCT_UNIT_ACTIVITY *result_check = NULL;
	long int arg2 = 0x0A; // Note: arg2=0x0A in all cases but artefacts(5)
	if (unit->unitDefinition && (unit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact)) {
		arg2 = 5;
	}
	_asm {
		MOV ECX, activity;
		PUSH arg2;
		PUSH unit;
		CALL ccorAddr;
		MOV result_check, EAX;
	}
	assert(result_check == activity);
	unit->currentActivity = activity;
	return true;
}


}

