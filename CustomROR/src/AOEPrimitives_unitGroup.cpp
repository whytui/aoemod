#include "../include/AOEPrimitives_unitGroup.h"


namespace AOE_METHODS {
namespace UNIT_GROUP {
;

bool RemoveUnitFromGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_AI *mainAI, STRUCT_UNIT_BASE *unit) {
	assert(unitGroup && unitGroup->IsCheckSumValid());
	assert(mainAI && mainAI->IsCheckSumValid());
	assert(unit && unit->IsCheckSumValidForAUnitClass());
	if (!unitGroup || !unitGroup->IsCheckSumValid()) { return false; }
	if (!mainAI || !mainAI->IsCheckSumValid()) { return false; }
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	unsigned long int addrRemoveFromGroup = 0x4CCD10;
	long int result;
	_asm {
		MOV ECX, unitGroup;
		PUSH mainAI;
		PUSH unit;
		CALL addrRemoveFromGroup;
		MOV result, EAX;
	}
	return (result != 0);
}


bool AddUnitToGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_AI *mainAI, STRUCT_UNIT_BASE *unit) {
	assert(unitGroup && unitGroup->IsCheckSumValid());
	assert(mainAI && mainAI->IsCheckSumValid());
	assert(unit && unit->IsCheckSumValidForAUnitClass());
	if (!unitGroup || !unitGroup->IsCheckSumValid()) { return false; }
	if (!mainAI || !mainAI->IsCheckSumValid()) { return false; }
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	unsigned long int addrAddToGroup = 0x4CCC90;
	long int result;
	_asm {
		MOV ECX, unitGroup;
		PUSH mainAI;
		PUSH unit;
		CALL addrAddToGroup;
		MOV result, EAX;
	}
	return (result != 0);
}


// Task a unit group (will probably give commands to units)
bool TaskUnitGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI *tacAI, AOE_CONST_INTERNAL::UNIT_GROUP_TASK_IDS taskId, 
	long int resetOrg, bool force) {
	assert(unitGroup && unitGroup->IsCheckSumValid());
	assert(tacAI && tacAI->IsCheckSumValid() && tacAI->ptrMainAI && tacAI->ptrMainAI->IsCheckSumValid());
	if (!unitGroup || !unitGroup->IsCheckSumValid()) { return false; }
	if (!tacAI || !tacAI->IsCheckSumValid()) { return false; }
	STRUCT_AI *mainAI = tacAI->ptrMainAI;
	if (!mainAI || !mainAI->IsCheckSumValid()) { return false; }
	unsigned long int addrTaskGroup = 0x4CD2D0;
	long int longIntForce = force ? 1 : 0;
	long int result;
	_asm {
		PUSH longIntForce;
		PUSH resetOrg;
		PUSH taskId;
		PUSH mainAI;
		PUSH tacAI;
		MOV ECX, unitGroup;
		CALL addrTaskGroup;
		MOV result, EAX;
	}
	return (result != 0);
}

}
}

