#include "../include/RockNRorGameObjects.h"



UnitCustomInfo::UnitCustomInfo() {
	this->unitId = -1;
	this->ResetSpawnAutoTargetInfo();
	this->autoAttackPolicyIsSet = false;
	//autoAttackPolicy : default constructor will initialize it correctly.
	this->ResetProtectInfo();
}

// Returns true if object contains no relevant information and can be removed. (all values are set to "none" or default)
bool UnitCustomInfo::CanBeRemoved() const {
	if (this->unitId < 0) { return true; } // invalid unit id = can remove
	// Is the spawn auto target/auto move defined ?
	if ((this->spawnTargetUnitId >= 0) || (this->spawnUnitMoveToPosX >= 0) || (this->spawnUnitMoveToPosY >= 0)) {
		return false;
	}
	// Is auto attack policy set to a custom value ?
	if (!this->autoAttackPolicyIsSet) {
		return false;
	}
	// Is "protect" policy set ?
	if ((this->protectUnitId >= 0) || (this->protectPosX >= 0) || (this->protectPosY >= 0)) {
		return false;
	}
	// We checked all values, and none is set to a "useful" value.
	return true;
}

// Returns true if one of (protect unit, protect location) is defined.
bool UnitCustomInfo::HasValidProtectInfo() const {
	bool protectUnit = (this->protectUnitId >= 0);
	bool protectPos = (this->protectPosX >= 0) && (this->protectPosX >= 0);
	return (protectUnit && !protectPos) || (!protectUnit && protectPos);
}

void UnitCustomInfo::ResetSpawnAutoTargetInfo() {
	this->spawnUnitMoveToPosX = -1;
	this->spawnUnitMoveToPosY = -1;
	this->spawnTargetUnitId = -1;
}

void UnitCustomInfo::ResetProtectInfo() {
	this->protectPosX = -1;
	this->protectPosY = -1;
	this->protectUnitId = -1;
}


FarmRebuildInfo::FarmRebuildInfo() {
	this->villagerUnitId = -1;
	this->gameTime = 0;
	this->playerId = -1;
	this->posX = -1;
	this->posY = -1;
	this->forceNotRebuild = false;
	this->forceRebuild = false;
}



RockNRorGameObjects::RockNRorGameObjects() {

}

RockNRorGameObjects::~RockNRorGameObjects() {

}


void RockNRorGameObjects::ResetObjects() {
	this->FreeAllFarmRebuildInfoList();
	this->FreeAllUnitCustomInfoList();
}


void RockNRorGameObjects::FreeAllUnitCustomInfoList() {
	std::for_each(this->unitCustomInfoList.begin(), this->unitCustomInfoList.end(),
		[](UnitCustomInfo *i) { delete i; }
	);
	this->unitCustomInfoList.clear();
}

void RockNRorGameObjects::FreeAllFarmRebuildInfoList() {
	std::for_each(this->farmRebuildInfoList.begin(), this->farmRebuildInfoList.end(),
		[](FarmRebuildInfo *f) { delete f; }
	);
	this->farmRebuildInfoList.clear();
}



// Remove all information concerning a specific unit
bool RockNRorGameObjects::RemoveAllInfoForUnit(long int unitId, float posX, float posY) {
	bool result = false;
	if ((posX >= 0) && (posY >= 0)) {
		result |= this->RemoveFarmRebuildInfo(posX, posX);
	}
	if (unitId != -1) {
		result |= this->RemoveUnitCustomInfo(unitId);
		result |= this->RemoveProtectedUnit(unitId);
	}
	return result;
}


// Returns a UnitCustomInfo pointer to matching element for given unitId.
// Returns NULL if not found.
// Asserts unitId > 0
UnitCustomInfo *RockNRorGameObjects::FindUnitCustomInfo(long int unitId) {
	assert(unitId >= 0);
	auto it = std::find_if(this->unitCustomInfoList.begin(), this->unitCustomInfoList.end(),
		[unitId](UnitCustomInfo *unitInfo) {return unitInfo->unitId == unitId; }
	);

	if (it != this->unitCustomInfoList.end()) {
		return *it;
	}
	return NULL;
}

// Returns a UnitCustomInfo pointer to matching element for given unitId.
// Asserts unitId >= 0
UnitCustomInfo *RockNRorGameObjects::FindOrAddUnitCustomInfo(long int unitId) {
	assert(unitId >= 0);
	auto it = std::find_if(this->unitCustomInfoList.begin(), this->unitCustomInfoList.end(),
		[unitId](UnitCustomInfo *unitInfo) {return unitInfo->unitId == unitId; }
	);

	if (it != this->unitCustomInfoList.end()) {
		return *it; // it did not reach end: element was found...
	}
	// If not found, add it.
	UnitCustomInfo *newElem = new UnitCustomInfo(); // Inits all values to NULL / -1...
	newElem->unitId = unitId;
	this->unitCustomInfoList.push_back(newElem);
	return newElem;
}


// Remove custom information for a specific unit
// Returns true if an element was found (and removed)
bool RockNRorGameObjects::RemoveUnitCustomInfo(long int unitId) {
	auto it = std::remove_if(this->unitCustomInfoList.begin(), this->unitCustomInfoList.end(),
		[unitId](UnitCustomInfo *unitInfo) { return unitInfo->unitId == unitId; }
	);
	bool found = (it != this->unitCustomInfoList.end());
	this->unitCustomInfoList.erase(it, this->unitCustomInfoList.end());
	return found;
}

// Remove custom information for a specific unit, ONLY if it contains no relevant value (all parameters are not applicable/default)
// Returns true if an element was removed
bool RockNRorGameObjects::RemoveUnitCustomInfoIfEmpty(long int unitId) {
	UnitCustomInfo *u = this->FindUnitCustomInfo(unitId);
	if (!u) { return false; }
	if (u->CanBeRemoved()) {
		return this->RemoveUnitCustomInfo(unitId);
	}
	return false;
}


// Returns a FarmRebuildInfo pointer to matching element for given position
// Returns NULL if not found.
FarmRebuildInfo *RockNRorGameObjects::FindFarmRebuildInfo(float posX, float posY) {
	assert(posX >= 0);
	assert(posY >= 0);
	auto it = std::find_if(this->farmRebuildInfoList.begin(), this->farmRebuildInfoList.end(),
		[posX, posY](FarmRebuildInfo *f) {
			return (abs(f->posX - posX) <= REBUILD_FARMS_POSITION_THRESHOLD) && (abs(f->posY - posY) <= REBUILD_FARMS_POSITION_THRESHOLD);
		}
	);
	if (it != this->farmRebuildInfoList.end()) {
		return *it;
	}
	return NULL; // not found
}

// Returns (and adds if not existing) a FarmRebuildInfo pointer for given position.
FarmRebuildInfo *RockNRorGameObjects::FindOrAddFarmRebuildInfo(float posX, float posY) {
	FarmRebuildInfo *elem = this->FindFarmRebuildInfo(posX, posY);
	if (elem != NULL) { return elem; }
	elem = new FarmRebuildInfo(); // Inits all values to NULL / -1...
	elem->posX = posX;
	elem->posY = posY;
	this->farmRebuildInfoList.push_back(elem);
	return elem;
}

// Remove a FarmRebuildInfo element from list
// Returns true if an element was found (and removed)
bool RockNRorGameObjects::RemoveFarmRebuildInfo(float posX, float posY) {
	auto it = std::remove_if(this->farmRebuildInfoList.begin(), this->farmRebuildInfoList.end(),
		[posX, posY](FarmRebuildInfo *f) {
			return (abs(f->posX - posX) <= REBUILD_FARMS_POSITION_THRESHOLD) && (abs(f->posY - posY) <= REBUILD_FARMS_POSITION_THRESHOLD);
		}
	);
	bool found = (it != this->farmRebuildInfoList.end());
	this->farmRebuildInfoList.erase(it, this->farmRebuildInfoList.end());
	return found;
}


// Remove "protect" info from all "unit info" objects that tell to protect a specific unit id
bool RockNRorGameObjects::RemoveProtectedUnit(long int protectedUnitId) {
	if (protectedUnitId < 0) { return false; }
	bool result = false;
	std::vector<long int> modifiedUnitIdList;
	for each(UnitCustomInfo *unitInfo in this->unitCustomInfoList) {
		if (unitInfo->protectUnitId == protectedUnitId) {
			unitInfo->ResetProtectInfo();
			result = true;
			modifiedUnitIdList.push_back(unitInfo->unitId);
		}
	}
	// Clean empty unitCustomInfo objects, if any
	for each(long int unitIdToCheck in modifiedUnitIdList) {
		this->RemoveUnitCustomInfoIfEmpty(unitIdToCheck);
	}
	return result;
}

