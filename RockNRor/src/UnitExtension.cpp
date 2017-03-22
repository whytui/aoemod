#include "../include/UnitExtension.h"

namespace ROCKNROR {
;

UnitExtension::UnitExtension() {
	this->Reset();
}


void UnitExtension::Reset() {
	this->pUnit = NULL;
	this->unitId = -1;
	this->isInitialized = false;
	this->unitDied = false;
	for (int i = 0; i < 9; i++) {
		this->myIndexInOtherPlayerInfAIList[i] = -1;
		this->hasBeenAddedToOtherPlayersInfAILists[i] = false;
	}
}


// Init this object using provided unit.
// Does nothing if "this" was already initialized (asserts that unit pointer IS the same as the one already known)
void UnitExtension::Init(STRUCT_UNIT_BASE *unit) {
	if (!unit) { return; }
	if (this->isInitialized) {
		assert(unit == this->pUnit);
		return;
	}
	this->isInitialized = true;
	this->pUnit = unit;
	this->unitId = unit->unitInstanceId;
}


// Init this object, considering the unit no longer exists (DEAD unit).
// If "this" extension is already initialized, this will set the flag "unitDied" to true.
void UnitExtension::InitForDeadUnit(long int unitId) {
	this->isInitialized = true;
	this->pUnit = NULL;
	this->unitDied = true;
	this->unitId = unitId;
}


// Updates the specified infAI element with "this" unit information.
// Returns true if successful
bool UnitExtension::WriteAllInfAIElemInfo(STRUCT_INF_AI_UNIT_LIST_ELEM *elemToUpdate) {
	if (!this->pUnit || !elemToUpdate) { return false; }
	elemToUpdate->unitId = this->pUnit->unitInstanceId;
	elemToUpdate->posX = (unsigned char)this->pUnit->positionX;
	elemToUpdate->posY = (unsigned char)this->pUnit->positionY;
	elemToUpdate->posZ = (unsigned char)this->pUnit->positionZ;
	elemToUpdate->playerId = (char)this->pUnit->ptrStructPlayer->playerId;
	elemToUpdate->attackAttempts = 0;
	elemToUpdate->HP = (short int)this->pUnit->remainingHitPoints;
	STRUCT_UNITDEF_BASE *unitDef = this->pUnit->unitDefinition;
	elemToUpdate->unitDATID = unitDef->DAT_ID1;
	elemToUpdate->unitClass = unitDef->unitAIType;
	elemToUpdate->maxRange = AOE_METHODS::UNIT::GetMaxRange(this->pUnit);
	elemToUpdate->reloadTime1 = AOE_METHODS::UNIT::GetReloadTime1(this->pUnit);
	elemToUpdate->attack = (float)AOE_METHODS::UNIT::GetAttacksAmount(this->pUnit);
	elemToUpdate->unknown_14 = 0;
	return true;
}

// Updates some infAI element information with "this" unit information: position, owner playerId...
// Returns true if successful
bool UnitExtension::UpdateInfAIElemInfo(STRUCT_INF_AI_UNIT_LIST_ELEM *elemToUpdate) {
	if (!this->pUnit || !elemToUpdate) { return false; }
#ifdef _DEBUG
	if (elemToUpdate->unitId != this->pUnit->unitInstanceId) {
		std::string msg = "UpdateInfAIElemInfo: bad id, elem.id=";
		msg += std::to_string(elemToUpdate->unitId);
		msg += ", this.unitId=";
		msg += std::to_string(this->pUnit->unitInstanceId);
		msg += " thisp=";
		msg += GetHexStringAddress(this->pUnit);
		traceMessageHandler.WriteMessage(msg.c_str());
		return false; // failed
	}
#endif
	assert(elemToUpdate->unitId == this->pUnit->unitInstanceId);
	elemToUpdate->unitId = this->pUnit->unitInstanceId; // security
	elemToUpdate->posX = (unsigned char)this->pUnit->positionX;
	elemToUpdate->posY = (unsigned char)this->pUnit->positionY;
	elemToUpdate->posZ = (unsigned char)this->pUnit->positionZ;
	elemToUpdate->playerId = (char) this->pUnit->ptrStructPlayer->playerId;
	elemToUpdate->HP = (short int)this->pUnit->remainingHitPoints;
	// Attack/maxRange/reloadTime almost never change... Ignore them
	return true;
}

}

