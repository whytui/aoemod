#include "../include//UnitExtensionHandler.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
;

// Common global object
UnitExtensionHandler unitExtensionHandler;


UnitExtensionHandler::UnitExtensionHandler() {
	this->allUnitExtensions = NULL;
	this->currentAllocatedElemCount = 0;
}


UnitExtensionHandler::~UnitExtensionHandler() {
	this->ResetAllInfo();
}


// Reset all underlying info, typically on new game (including load saved game, etc)
void UnitExtensionHandler::ResetAllInfo() {
	this->currentAllocatedElemCount = 0;
	if (this->allUnitExtensions) {
		AOEFree(this->allUnitExtensions);
	}
	this->allUnitExtensions = NULL;
}


// Create a new array, sized according to global structure's unit pointers size.
// Returns true if successful
bool UnitExtensionHandler::ReallocArrayUsingGameGlobal() {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) {
		return false;
	}
	long int newElemCount = global->unitPointersListSize;
	assert(newElemCount > this->currentAllocatedElemCount);
	if (newElemCount <= this->currentAllocatedElemCount) { return false; }
	UnitExtension *newArray = (UnitExtension*)AOEAllocZeroMemory(newElemCount, sizeof(UnitExtension));
	
	if ((this->currentAllocatedElemCount > 0) && (this->allUnitExtensions != NULL)) {
		// Copy old array to new one. New one is bigger so we are safe here.
		// Warning: constructor is not called here !
		memcpy_s(newArray, newElemCount * sizeof(UnitExtension), this->allUnitExtensions, this->currentAllocatedElemCount);
	}
	// Call constructor (reset, actually) on *new* slots (only)
	for (int i = this->currentAllocatedElemCount; i < newElemCount; i++) {
		newArray[i].Reset();
	}

	this->currentAllocatedElemCount = newElemCount; // set new size (elem count)
	if (this->allUnitExtensions != NULL) {
		AOEFree(this->allUnitExtensions); // Free old array
	}
	this->allUnitExtensions = newArray;
	return true;
}


// Add/initialize extension data for the unit specified
// If the unit extension is already initialized, this does nothing (does NOT overwrite previous information)
// Returns true if successful (including the case when unit is already initialized)
bool UnitExtensionHandler::AddUnitExtension(STRUCT_UNIT_BASE *unit) {
	if (!unit) { return false; }
	long int unitId = unit->unitInstanceId;
	if (unitId < 0) { return false; }
	if (unitId >= this->currentAllocatedElemCount) {
		if (!this->ReallocArrayUsingGameGlobal()) {
			return false;
		}
	}
	if (this->allUnitExtensions[unitId].isInitialized) {
		// This unit is already known/initialized. Considered successful.
		return true;
	}
	this->allUnitExtensions[unitId].Init(unit);
	return true;
}


// Add/initialize extension data for the unitId specified
// If the unit extension is already initialized, this does nothing (does NOT overwrite previous information)
// If the unit does NOT exist AND 0 <= unitId < global.seqUnitId, the extension IS created with NULL pointer and marked as "dead" unit.
// Returns true if successful (including the case when unit is already initialized)
bool UnitExtensionHandler::AddUnitExtension(long int unitId) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return false; }
	assert(global->IsCheckSumValid());
	STRUCT_UNIT_BASE *unit = global->GetUnitFromId(unitId);
	if (unit) {
		return this->AddUnitExtension(unit);
	} else {
		if ((unitId < 0) || (unitId >= global->seqUnitId)) {
			return false; // Unknown unit id regarding possible IDs. This unit can't be a dead unit ! Error case.
		}
		// Here we have a valid unitId but no matching unit: consider it existED but died
		if (unitId >= this->currentAllocatedElemCount) {
			// Make sure the array can handle this new unitId
			if (!this->ReallocArrayUsingGameGlobal()) {
				return false;
			}
		}
		if (this->allUnitExtensions[unitId].isInitialized) {
			// This unit is already known/initialized. Considered successful.
			return true;
		}
		this->allUnitExtensions[unitId].InitForDeadUnit(unitId);
		return true;
	}
}


// Add/update infAI element for current unit (we consider it is visible for the player specified)
// Remark: this method is called only if improveAI is true.
bool UnitExtensionHandler::AddUpdateInfAIElem(STRUCT_UNIT_BASE *unit, long int infAIPlayerId) {
	if (!unit || infAIPlayerId < 0) { return false; }
	STRUCT_PLAYER *AIPlayer = GetPlayerStruct(infAIPlayerId);
	if (!AIPlayer) { return false; } // Player does not exist
	assert(AIPlayer->IsCheckSumValid());
	if (AIPlayer->aliveStatus == 2) { return true; } // Ignore defeated players (not an error)
	if (AIPlayer->ptrAIStruct == NULL) { return false; } // No AI for the player specified
	STRUCT_INF_AI *infAI = &AIPlayer->ptrAIStruct->structInfAI;

	long int unitId = unit->unitInstanceId;
	if (unitId < 0) { return false; }
	if (!this->AddUnitExtension(unit)) { // does nothing if already known
		std::string msg = "Error adding unit extension for id=";
		msg += std::to_string(unit->unitInstanceId);
		traceMessageHandler.WriteMessage(msg.c_str());
		return false; // Error case
	}
	// Now we can safely use this->allUnitExtensions[unitId]
	if (this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] >= 0) {
		// InfAI index is already known for that player
		assert(this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] < infAI->detailedSpottedUnitInfoListSize);
		STRUCT_INF_AI_DETAILED_UNIT_INFO *elem = &infAI->detailedSpottedUnitInfoList[this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId]];
		if (this->allUnitExtensions[unitId].UpdateInfAIElemInfo(elem)) {
			return true;
		}
		// Failed: maybe the index was wrong ? Reset it.
		std::string msg = "Incorrect cache index in unit extension/id=";
		msg += std::to_string(unit->unitInstanceId);
		traceMessageHandler.WriteMessage(msg.c_str());
		this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] = -1;
	}

	// Here index in infAI elem list is unknown. Search for it
	
	long int indexOfAFreeSlot = -1;
	for (int curIndex = 0; curIndex < infAI->detailedSpottedUnitInfoListSize; curIndex++) {
		if (infAI->detailedSpottedUnitInfoList[curIndex].unitId == unitId) {
			// Found: save index, update info and return
			this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] = curIndex;
			return this->allUnitExtensions[unitId].UpdateInfAIElemInfo(&infAI->detailedSpottedUnitInfoList[curIndex]);
		} else {
			if ((indexOfAFreeSlot < 0) && (infAI->detailedSpottedUnitInfoList[curIndex].unitId < 0)) {
				indexOfAFreeSlot = curIndex; // Save index of a free slot for later (may be useful)
			}
		}
	}
	// Here: not found in infAI list: add it
	if (indexOfAFreeSlot < 0) {
		// May append at the end of the list... or reuse an existing slot (should not as we already tried)
		if (!AOE_METHODS::LISTS::AddUpdateInfAIElemList(infAI, unit)) {
			return false;
		}
		// Find index of new elem.
		STRUCT_INF_AI_DETAILED_UNIT_INFO *elem = AOE_METHODS::LISTS::FindInfAIUnitElemInList(infAI, unitId);
		long int elemIndex = (((unsigned long int)elem) - (unsigned long int)(infAI->detailedSpottedUnitInfoList)) / sizeof(STRUCT_INF_AI_DETAILED_UNIT_INFO);
		this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] = elemIndex;
		return true;
	}

	// Here: use the free slot we found
	this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] = indexOfAFreeSlot;
	STRUCT_INF_AI_DETAILED_UNIT_INFO *elem = &infAI->detailedSpottedUnitInfoList[indexOfAFreeSlot];
	return this->allUnitExtensions[unitId].WriteAllInfAIElemInfo(elem);
}


// Remove element for provided unitId in specified player's InfAI elem list.
// This does NOT use optimization from unitExtensions
// For security, if unitExtension contains a pointer for the unit specified, the pointer is set to NULL and unitExtension invalidated.
// To be used when improveAI is false.
// Returns false in error cases only
bool UnitExtensionHandler::RemoveInfAIElemForUnitWithoutOptimization(long int unitId, long int infAIPlayerId) {
	// This mode is NOT optimized ! Uses classical search.
	if (this->allUnitExtensions[unitId].isInitialized) {
		this->allUnitExtensions[unitId].isInitialized = false;
		this->allUnitExtensions[unitId].pUnit = NULL; // To make sure we don't keep an obsolete pointer
	}
	STRUCT_PLAYER *playerToUpdate = GetPlayerStruct(infAIPlayerId);
	STRUCT_INF_AI *infAI = NULL;
	if (playerToUpdate && playerToUpdate->IsCheckSumValid() && playerToUpdate->ptrAIStruct && playerToUpdate->ptrAIStruct->IsCheckSumValid()) {
		infAI = &playerToUpdate->ptrAIStruct->structInfAI;
	}

	if (!infAI) { return true; }
	STRUCT_INF_AI_DETAILED_UNIT_INFO *elem = AOE_METHODS::LISTS::FindInfAIUnitElemInList(infAI, unitId);
	if (!elem) { return true; } // no elem to update: not an error case
	return AOE_METHODS::LISTS::ResetInfAIUnitListElem(elem);
}


// Remove element for provided unitId in specified player's InfAI elem list.
// This uses optimization from unitExtensions (if info is available)
// You can use this even if improveAI is false
// Returns false in error cases only
bool UnitExtensionHandler::RemoveInfAIElemForUnit(long int unitId, long int infAIPlayerId) {
	if ((unitId < 0) || (unitId >= this->currentAllocatedElemCount)) { return false; }
	if (infAIPlayerId < 0) { return false; }

	if (!ROCKNROR::IsImproveAIEnabled(infAIPlayerId)) {
		// Restricted mode: only remove from infAI list, do not use unit extensions (just remove unit pointer if existing, in case this corresponds to unit death)
		return this->RemoveInfAIElemForUnitWithoutOptimization(unitId, infAIPlayerId);
	}

	if (!this->allUnitExtensions[unitId].isInitialized) {
		if (!this->AddUnitExtension(unitId)) {
			// Error case (this overload should create the extension even if unit is dead and no longer exists)
			return false;
		}
	}
	
	STRUCT_PLAYER *playerToUpdate = GetPlayerStruct(infAIPlayerId);
	STRUCT_INF_AI *infAI = NULL;
	if (playerToUpdate && playerToUpdate->IsCheckSumValid() && playerToUpdate->ptrAIStruct && playerToUpdate->ptrAIStruct->IsCheckSumValid()) {
		infAI = &playerToUpdate->ptrAIStruct->structInfAI;
	}
	if (infAI) {
		int index = this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId];
		if ((index >= 0) && (index < infAI->detailedSpottedUnitInfoListSize)) {
			// don't forget to remove the "cached" index because ROR will re-use the slot for another unit
			this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] = -1;
			return AOE_METHODS::LISTS::ResetInfAIUnitListElem(&infAI->detailedSpottedUnitInfoList[index]);
		}
	}
	return true; // nothing to do here
}


// Remove element for provided unitId in all players' InfAI elem list.
// This uses optimization from unitExtensions (if info is available)
// You can use this even if improveAI is false
// Returns true if successful for all players
bool UnitExtensionHandler::RemoveAllInfAIElemForUnit(long int unitId) {
	bool result = true;
	for (int i = 0; i < 9; i++) {
		result &= this->RemoveInfAIElemForUnit(unitId, i);
	}
	return result;
}


}
