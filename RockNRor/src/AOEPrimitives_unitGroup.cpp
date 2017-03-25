#include "../include/AOEPrimitives_unitGroup.h"


namespace AOE_METHODS {
namespace UNIT_GROUP {
;


// Create a unit group with 1 unit (leader)
STRUCT_UNIT_GROUP *CreateUnitGroup(STRUCT_AI *mainAI, AOE_CONST_INTERNAL::UNIT_GROUP_TYPES groupType, STRUCT_UNIT_BASE *leader) {
	if (!mainAI || !mainAI->IsCheckSumValid() || !leader || !leader->IsCheckSumValidForAUnitClass()) { return NULL; }
	STRUCT_TAC_AI *tacAI = &mainAI->structTacAI;
	assert(tacAI->IsCheckSumValid());
	STRUCT_UNIT_GROUP *unitGroup = NULL;
	unsigned long int addrCreateGrp = 0x4E0400;
	_asm {
		PUSH 1; // use sequence for ID
		MOV ECX, tacAI;
		CALL addrCreateGrp;
		MOV unitGroup, EAX;
	}
	if (!unitGroup) { return unitGroup; }
	unitGroup->unitGroupType = groupType;
	AddUnitToGroup(unitGroup, mainAI, leader->unitInstanceId);
	SetNewGroupLeader(unitGroup, mainAI, leader->unitInstanceId);
	unitGroup->posX = leader->positionX;
	unitGroup->posY = leader->positionY;
	unitGroup->posZ = leader->positionZ;
	unitGroup->unknown_174_posX = leader->positionX;
	unitGroup->unknown_170_posY = leader->positionY;
	unitGroup->unknown_178_posZ = leader->positionZ;
	unitGroup->retreatPosX = leader->positionX;
	unitGroup->retreatPosY = leader->positionY;
	unitGroup->retreatPosZ = leader->positionZ;
	return unitGroup;
}


bool RemoveUnitFromGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_AI *mainAI, long int unitId) {
	assert(unitGroup && unitGroup->IsCheckSumValid());
	assert(mainAI && mainAI->IsCheckSumValid());
	if (!unitGroup || !unitGroup->IsCheckSumValid()) { return false; }
	if (!mainAI || !mainAI->IsCheckSumValid()) { return false; }
	unsigned long int addrRemoveFromGroup = 0x4CCD10;
	long int result;
	_asm {
		MOV ECX, unitGroup;
		PUSH mainAI;
		PUSH unitId;
		CALL addrRemoveFromGroup;
		MOV result, EAX;
	}
	return (result != 0);
}


bool AddUnitToGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_AI *mainAI, long int unitId) {
	assert(unitGroup && unitGroup->IsCheckSumValid());
	assert(mainAI && mainAI->IsCheckSumValid());
	if (!unitGroup || !unitGroup->IsCheckSumValid()) { return false; }
	if (!mainAI || !mainAI->IsCheckSumValid()) { return false; }
	unsigned long int addrAddToGroup = 0x4CCC90;
	long int result;
	_asm {
		MOV ECX, unitGroup;
		PUSH mainAI;
		PUSH unitId;
		CALL addrAddToGroup;
		MOV result, EAX;
	}
	return (result != 0);
}


// Sets a unit group's new leader.
bool SetNewGroupLeader(STRUCT_UNIT_GROUP *unitGroup, STRUCT_AI *mainAI, long int unitId) {
	assert(unitGroup && unitGroup->IsCheckSumValid());
	assert(mainAI && mainAI->IsCheckSumValid());
	if (!unitGroup || !unitGroup->IsCheckSumValid()) { return false; }
	if (!mainAI || !mainAI->IsCheckSumValid()) { return false; }
	unsigned long int addrSetGroupLeader = 0x4CCFE0;
	_asm {
		MOV ECX, unitGroup;
		PUSH mainAI;
		PUSH unitId;
		CALL addrSetGroupLeader;
	}
	return true;
}


// Find unit's unit group, if any. NULL if not found
STRUCT_UNIT_GROUP *FindUnitGroup(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->ptrStructPlayer || !unit->ptrStructPlayer->IsCheckSumValid()) {
		return NULL;
	}
	STRUCT_PLAYER *player = unit->ptrStructPlayer;
	if (!player->ptrAIStruct || !player->ptrAIStruct->IsCheckSumValid() || (player->ptrAIStruct->structTacAI.unitGroupsCount <= 0)) {
		return NULL;
	}
	long int leaderUnitId = unit->groupLeaderUnitId;
	if (leaderUnitId < 0) {
		leaderUnitId = unit->unitInstanceId;
	}
	STRUCT_UNIT_GROUP *fakeFirstGroup = &player->ptrAIStruct->structTacAI.fakeFirstUnitGroupElem;
	STRUCT_UNIT_GROUP *curGrp = fakeFirstGroup->next;
	while (curGrp && (curGrp != fakeFirstGroup)) {
		if (curGrp->commanderUnitId == leaderUnitId) {
			return curGrp;
		}
		curGrp = curGrp->next;
	}
	return NULL;
}


// Apply a task a unit group (will probably give commands to units)
// Warning: this does NOT set unitGroup.currentTask/resetOrg, you have to set it before (and set target, ..., if necessary)
// If force=true, give the order even for units that are busy with a non-wall attack
bool ApplyTaskToUnitGroup(STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI *tacAI, AOE_CONST_INTERNAL::UNIT_GROUP_TASK_IDS taskId,
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

