#include "../include/RockNRorGameObjects.h"


unsigned char integrityCheck = 0xAB;

UnitCustomInfo::UnitCustomInfo() {
	this->unitId = -1;
	this->ResetSpawnAutoTargetInfo();
	this->autoAttackPolicyIsSet = false;
	//autoAttackPolicy : default constructor will initialize it correctly.
	this->ResetProtectInfo();
	this->myMainBuilderId = -1;
}

// Returns true if object contains no relevant information and can be removed. (all values are set to "none" or default)
bool UnitCustomInfo::CanBeRemoved() const {
	if (this->unitId < 0) { return true; } // invalid unit id = can remove
	// Is the spawn auto target/auto move defined ?
	if ((this->spawnTargetUnitId >= 0) || (this->spawnUnitMoveToPosX >= 0) || (this->spawnUnitMoveToPosY >= 0)) {
		return false;
	}
	// Is auto attack policy set to a custom value ?
	if (this->autoAttackPolicyIsSet) {
		return false;
	}
	// Is "protect" policy set ?
	if ((this->protectUnitId >= 0) || (this->protectPosX >= 0) || (this->protectPosY >= 0)) {
		return false;
	}
	// Is a specific builder assigned ? (building in construction)
	if (this->myMainBuilderId >= 0) {
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


// Serialize object to output stream (file). May throw SerializeException
long int UnitCustomInfo::Serialize(FILE *outputFile) const {
	long int result = 0;
	result += this->WriteBytes(outputFile, &this->autoAttackPolicy, sizeof(this->autoAttackPolicy)); // OK as the class only contains bools
	result += this->WriteBytes(outputFile, &this->autoAttackPolicyIsSet, sizeof(this->autoAttackPolicyIsSet));
	result += this->WriteBytes(outputFile, &this->myMainBuilderId, sizeof(this->myMainBuilderId));
	result += this->WriteBytes(outputFile, &this->protectPosX, sizeof(this->protectPosX));
	result += this->WriteBytes(outputFile, &this->protectPosY, sizeof(this->protectPosY));
	result += this->WriteBytes(outputFile, &this->protectUnitId, sizeof(this->protectUnitId));
	result += this->WriteBytes(outputFile, &this->spawnTargetUnitId, sizeof(this->spawnTargetUnitId));
	result += this->WriteBytes(outputFile, &this->spawnUnitMoveToPosX, sizeof(this->spawnUnitMoveToPosX));
	result += this->WriteBytes(outputFile, &this->spawnUnitMoveToPosY, sizeof(this->spawnUnitMoveToPosY));
	result += this->WriteBytes(outputFile, &this->unitId, sizeof(this->unitId));
	return result;
}

// Deserialize object from input stream (file). May throw SerializeException
bool UnitCustomInfo::Deserialize(FILE *inputFile) {
	this->ReadBytes(inputFile, &this->autoAttackPolicy, sizeof(this->autoAttackPolicy));
	this->ReadBytes(inputFile, &this->autoAttackPolicyIsSet, sizeof(this->autoAttackPolicyIsSet));
	this->ReadBytes(inputFile, &this->myMainBuilderId, sizeof(this->myMainBuilderId));
	this->ReadBytes(inputFile, &this->protectPosX, sizeof(this->protectPosX));
	this->ReadBytes(inputFile, &this->protectPosY, sizeof(this->protectPosY));
	this->ReadBytes(inputFile, &this->protectUnitId, sizeof(this->protectUnitId));
	this->ReadBytes(inputFile, &this->spawnTargetUnitId, sizeof(this->spawnTargetUnitId));
	this->ReadBytes(inputFile, &this->spawnUnitMoveToPosX, sizeof(this->spawnUnitMoveToPosX));
	this->ReadBytes(inputFile, &this->spawnUnitMoveToPosY, sizeof(this->spawnUnitMoveToPosY));
	this->ReadBytes(inputFile, &this->unitId, sizeof(this->unitId));
	return true;
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


// Serialize object to output stream (file). May throw SerializeException
long int FarmRebuildInfo::Serialize(FILE *outputFile) const {
	long int result = 0;
	result += this->WriteBytes(outputFile, &this->forceNotRebuild, sizeof(this->forceNotRebuild));
	result += this->WriteBytes(outputFile, &this->forceRebuild, sizeof(this->forceRebuild));
	result += this->WriteBytes(outputFile, &this->gameTime, sizeof(this->gameTime));
	result += this->WriteBytes(outputFile, &this->playerId, sizeof(this->playerId));
	result += this->WriteBytes(outputFile, &this->posX, sizeof(this->posX));
	result += this->WriteBytes(outputFile, &this->posY, sizeof(this->posY));
	result += this->WriteBytes(outputFile, &this->villagerUnitId, sizeof(this->villagerUnitId));
	return result;
}

// Deserialize object from input stream (file). May throw SerializeException
bool FarmRebuildInfo::Deserialize(FILE *inputFile) {
	this->ReadBytes(inputFile, &this->forceNotRebuild, sizeof(this->forceNotRebuild));
	this->ReadBytes(inputFile, &this->forceRebuild, sizeof(this->forceRebuild));
	this->ReadBytes(inputFile, &this->gameTime, sizeof(this->gameTime));
	this->ReadBytes(inputFile, &this->playerId, sizeof(this->playerId));
	this->ReadBytes(inputFile, &this->posX, sizeof(this->posX));
	this->ReadBytes(inputFile, &this->posY, sizeof(this->posY));
	this->ReadBytes(inputFile, &this->villagerUnitId, sizeof(this->villagerUnitId));
	return true;
}




RockNRorGameObjects::RockNRorGameObjects() {

}

RockNRorGameObjects::~RockNRorGameObjects() {

}


void RockNRorGameObjects::ResetObjects() {
	this->FreeAllFarmRebuildInfoList();
	this->FreeAllUnitCustomInfoList();
	this->currentGameHasAllTechs = false;
	this->doNotApplyHardcodedCivBonus = false;
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
// Does not impact unitExtensions though
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


// Remove all "empty" unit custom info objects
void RockNRorGameObjects::RemoveAllEmptyUnitCustomInfos() {
	for (auto it = this->unitCustomInfoList.begin(); it != this->unitCustomInfoList.end(); /*it++ inside the loop code*/) {
		if ((*it)->CanBeRemoved()) {
			it = this->unitCustomInfoList.erase(it);
		}
		else {
			it++;
		}
	}
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


// Serialize object to output stream (file). May throw SerializeException
long int RockNRorGameObjects::Serialize(FILE *outputFile) const {
	long int result = 0;
	result += this->WriteBytes(outputFile, &integrityCheck, sizeof(integrityCheck));
	result += this->WriteBytes(outputFile, &this->currentGameHasAllTechs, sizeof(this->currentGameHasAllTechs));
	result += this->WriteBytes(outputFile, &this->doNotApplyHardcodedCivBonus, sizeof(this->doNotApplyHardcodedCivBonus));

	size_t size = this->unitCustomInfoList.size();
	result += this->WriteBytes(outputFile, &size, sizeof(size));
	for (auto it = this->unitCustomInfoList.cbegin(); it != this->unitCustomInfoList.cend(); it++) {
		UnitCustomInfo *u = *it;
		result += u->Serialize(outputFile);
	}

	size = this->farmRebuildInfoList.size();
	result += this->WriteBytes(outputFile, &size, sizeof(size));
	for (auto it = this->farmRebuildInfoList.cbegin(); it != this->farmRebuildInfoList.cend(); it++) {
		FarmRebuildInfo *f = *it;
		result += f->Serialize(outputFile);
	}
	
	return result;
}

// Deserialize object from input stream (file). May throw SerializeException
bool RockNRorGameObjects::Deserialize(FILE *inputFile) {
	unsigned char tmpCheck = 0;
	this->ReadBytes(inputFile, &tmpCheck, sizeof(tmpCheck));
	if (tmpCheck != integrityCheck) {
		throw ROCKNROR::SYSTEM::SerializeException("Integrity check failed in deserialize (RockNRorGameObjects)");
	}
	this->ReadBytes(inputFile, &this->currentGameHasAllTechs, sizeof(this->currentGameHasAllTechs));
	this->ReadBytes(inputFile, &this->doNotApplyHardcodedCivBonus, sizeof(this->doNotApplyHardcodedCivBonus));
	
	size_t size = 0;
	this->ReadBytes(inputFile, &size, sizeof(size));
	for (size_t i = 0; i < size; i++) {
		UnitCustomInfo *newElem = new UnitCustomInfo(); // Inits all values to NULL / -1...
		if (!newElem->Deserialize(inputFile)) {
			throw ROCKNROR::SYSTEM::SerializeException("Inconsistent data in deserialize (RockNRorGameObjects/UnitCustomInfo)");
		}
		this->unitCustomInfoList.push_back(newElem);
	}

	size = 0;
	this->ReadBytes(inputFile, &size, sizeof(size));
	for (size_t i = 0; i < size; i++) {
		FarmRebuildInfo *newElem = new FarmRebuildInfo();
		if (!newElem->Deserialize(inputFile)) {
			throw ROCKNROR::SYSTEM::SerializeException("Inconsistent data in deserialize (RockNRorGameObjects/FarmRebuildInfo)");
		}
		this->farmRebuildInfoList.push_back(newElem);
	}

	return true;
}

