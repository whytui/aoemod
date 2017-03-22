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
	if ((unitId < 0) || (unitId >= this->currentAllocatedElemCount)) {
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


// Add/update infAI element for current unit (we consider it is visible for the player specified)
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
		return false; // Error case
	};
	// Now we can safely use this->allUnitExtensions[unitId]
	if (this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] >= 0) {
		// InfAI index is already known for that player
		assert(this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] < infAI->unitElemListSize);
		STRUCT_INF_AI_UNIT_LIST_ELEM *elem = &infAI->unitElemList[this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId]];
		if (this->allUnitExtensions[unitId].UpdateInfAIElemInfo(elem)) {
			return true;
		}
		// Failed: maybe the index was wrong ? Reset it.
		this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] = -1;
	}

	// Here index in infAI elem list is unknown. Search for it
	
	long int indexOfAFreeSlot = -1;
	for (int curIndex = 0; curIndex < infAI->unitElemListSize; curIndex++) {
		if (infAI->unitElemList[curIndex].unitId == unitId) {
			// Found: save index, update info and return
			this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] = curIndex;
			return this->allUnitExtensions[unitId].UpdateInfAIElemInfo(&infAI->unitElemList[curIndex]);
		} else {
			if ((indexOfAFreeSlot < 0) && (infAI->unitElemList[curIndex].unitId < 0)) {
				indexOfAFreeSlot = curIndex; // Save index of a free slot for later (may be useful)
			}
		}
	}
	// Here: not found in infAI list: add it
	if (indexOfAFreeSlot < 0) {
		// May append at the end of the list... or reuse an existing slot (should not as we already tried)
		if (!AddUpdateInfAIElemList(infAI, unit)) {
			return false;
		}
		// Find index of new elem.
		STRUCT_INF_AI_UNIT_LIST_ELEM *elem = FindInfAIUnitElemInList(infAI, unitId);
		long int elemIndex = (((unsigned long int)elem) - (unsigned long int)(infAI->unitElemList)) / sizeof(STRUCT_INF_AI_UNIT_LIST_ELEM);
		this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] = elemIndex;
		return true;
	}

	// Here: use the free slot we found
	this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIList[infAIPlayerId] = indexOfAFreeSlot;
	STRUCT_INF_AI_UNIT_LIST_ELEM *elem = &infAI->unitElemList[indexOfAFreeSlot];
	return this->allUnitExtensions[unitId].WriteAllInfAIElemInfo(elem);
}

}
