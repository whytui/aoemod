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
		memcpy_s(newArray, newElemCount * sizeof(UnitExtension), this->allUnitExtensions, this->currentAllocatedElemCount * sizeof(UnitExtension));
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


// Returns a unit extension IF it has already been initialized, NULL otherwise.
// This may return NULL even if the unit exists in game !
UnitExtension *UnitExtensionHandler::GetUnitExtension(long int unitId) {
	if (unitId < 0) { return NULL; }
	if (unitId >= this->currentAllocatedElemCount) {
		return NULL;
	}
	if (this->allUnitExtensions[unitId].isInitialized) {
		return &this->allUnitExtensions[unitId];
	}
	return NULL;
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
// You can call this at any time with no restriction (unitId better be valid though)
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


// Add/update specified unit in infAI lists (we consider it is visible for the player specified)
// DO NOT USE this if unit is not visible !!!
// Remark: this method is called only if improveAI is true.
bool UnitExtensionHandler::AddUpdateUnitInInfAILists(STRUCT_UNIT_BASE *unit, long int infAIPlayerId) {
	if (!unit || infAIPlayerId < 0) { return false; }
	STRUCT_PLAYER *AIPlayer = GetPlayerStruct(infAIPlayerId);
	if (!AIPlayer) { return false; } // Player does not exist
	assert(AIPlayer->IsCheckSumValid());
	if (AIPlayer->aliveStatus == 2) { return true; } // Ignore defeated players (not an error)
	if (AIPlayer->ptrAIStruct == NULL) { return false; } // No AI for the player specified

	bool result = this->AddUpdateInfAIUnitMemory(unit, &AIPlayer->ptrAIStruct->structInfAI);
	result &= this->AddUnitInOtherInfAILists(unit, &AIPlayer->ptrAIStruct->structInfAI);
	return result;
}


// Handle adding a unit memory entry for a specific player (infAI) when internal cache is already initialized
// Returns true if cache has been updated. False is not necessarily an error
bool UnitExtensionHandler::UpdateUnitMemoryCacheForPlayer(STRUCT_UNIT_MEMORY *updatedUnitMemory, STRUCT_INF_AI *infAI) {
	if (!updatedUnitMemory || !infAI) { return false; }
	long int infAIPlayerId = infAI->commonAIObject.playerId;
	if (infAIPlayerId < 0) { return false; }
	ROCKNROR::UnitExtension *e = this->GetUnitExtension(updatedUnitMemory->unitId);
	// This is useful when cache is iniitalized, by NOT for current player (because he didn't knew that unit when cache was initialized)
	if (e && e->isInitialized && (e->myIndexInOtherPlayerInfAIMemory[infAIPlayerId] < 0)) {
		unsigned long int pElem = (unsigned long int)updatedUnitMemory;
		unsigned long int pBase = (unsigned long int)infAI->unitMemoryList;
		long int index = (long int)(pElem - pBase) / sizeof(AOE_STRUCTURES::STRUCT_UNIT_MEMORY);
#ifdef _DEBUG
		assert((pElem - pBase) % sizeof(AOE_STRUCTURES::STRUCT_UNIT_MEMORY) == 0);
#endif
		if ((index >= 0) && (index < infAI->unitMemoryListSize)) {
			e->myIndexInOtherPlayerInfAIMemory[infAIPlayerId] = index;
			e->wasNotFoundInOtherPlayerInfAIMemory[infAIPlayerId] = false;
			return true;
		}
	}
	return false;
}


// Get the index in "InfAI" unit memory list for provided unit ID.
// Returns -1 if not found
// This method is faster than basic search because it uses 'cached' index (if possible)
long int UnitExtensionHandler::GetIndexForUnitInInfAIUnitMemory(long int unitIdToSearch, long int infAIPlayerId) {
	if ((unitIdToSearch < 0) || (unitIdToSearch >= this->currentAllocatedElemCount)) { return -1; }
	if (infAIPlayerId < 0) { return -1; }

	if (!this->allUnitExtensions[unitIdToSearch].isInitialized) {
		if (!this->AddUnitExtension(unitIdToSearch)) {
			// Error case (this overload should create the extension even if unit is dead and no longer exists)
			return -1;
		}
	}
	long result = this->allUnitExtensions[unitIdToSearch].myIndexInOtherPlayerInfAIMemory[infAIPlayerId];
	if (result >= 0) {
		return result;
	}
	// Search in infAI list (only once, thanks to wasNotFoundInOtherPlayerInfAIMemory flag)
	if (!this->allUnitExtensions[unitIdToSearch].wasNotFoundInOtherPlayerInfAIMemory[infAIPlayerId]) {
		STRUCT_PLAYER *player = GetPlayerStruct(infAIPlayerId);
		if (!player || !player->IsCheckSumValid() || !player->ptrAIStruct) {
			return result;
		}
		long int size = player->ptrAIStruct->structInfAI.unitMemoryListSize;
		for (int i = 0; i < size; i++) {
			if (player->ptrAIStruct->structInfAI.unitMemoryList[i].unitId == unitIdToSearch) {
				// We found the index: save it in cache
				this->allUnitExtensions[unitIdToSearch].myIndexInOtherPlayerInfAIMemory[infAIPlayerId] = i;
				return i;
			}
		}
		// Not found: remember it for next time. We'll never search again in infAI unit memory list,
		// BUT we have an entry point in "addupdate" ROR method that will automatically save the index in unit extension when the element is added
		this->allUnitExtensions[unitIdToSearch].wasNotFoundInOtherPlayerInfAIMemory[infAIPlayerId] = true;
	}
	return -1;
}


// Returns NULL if not found
// This method is faster than basic search because it uses 'cached' index (if possible)
STRUCT_UNIT_MEMORY *UnitExtensionHandler::GetInfAIUnitMemory(long int unitIdToSearch, long int infAIPlayerId) {
	STRUCT_UNIT_MEMORY *elem = NULL;
	long int index = this->GetIndexForUnitInInfAIUnitMemory(unitIdToSearch, infAIPlayerId);
	if (index < 0) { return NULL; }
	STRUCT_PLAYER *player = GetPlayerStruct(infAIPlayerId);
	if (!player || !player->IsCheckSumValid() || !player->ptrAIStruct) {
		return NULL;
	}
#ifdef _DEBUG
	assert(index < player->ptrAIStruct->structInfAI.unitMemoryListSize);
	assert(player->ptrAIStruct->structInfAI.unitMemoryList[index].unitId == unitIdToSearch);
#endif
	return &player->ptrAIStruct->structInfAI.unitMemoryList[index];
}

// Get the pointer to "InfAI" unit memory object for provided unit ID.
// Returns NULL if not found
// This method is faster than basic search because it uses 'cached' index (if possible)
STRUCT_UNIT_MEMORY *UnitExtensionHandler::GetInfAIUnitMemory(long int unitIdToSearch, STRUCT_INF_AI *infAI) {
	if (!infAI || !infAI->IsCheckSumValid()) { return NULL; }
	STRUCT_UNIT_MEMORY *elem = NULL;
	long int index = this->GetIndexForUnitInInfAIUnitMemory(unitIdToSearch, infAI->commonAIObject.playerId);
	if (index < 0) { return NULL; }
#ifdef _DEBUG
	assert(index < infAI->unitMemoryListSize);
#endif
	return &infAI->unitMemoryList[index];
}


// Remove element for provided unitId in specified player's InfAI unit memory list.
// This does NOT use optimization from unitExtensions
// For security, if unitExtension contains a pointer for the unit specified, the pointer is set to NULL and unitExtension invalidated.
// To be used when improveAI is false.
// Returns false in error cases only
bool UnitExtensionHandler::RemoveInfAIUnitMemoryWithoutOptimization(long int unitId, long int infAIPlayerId) {
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
	STRUCT_UNIT_MEMORY *elem = AOE_METHODS::LISTS::FindInfAIUnitMemoryElem(infAI, unitId);
	if (!elem) { return true; } // no elem to update: not an error case
	return AOE_METHODS::LISTS::ResetInfAIUnitMemoryListElem(elem);
}


// Remove element for provided unitId in all players' InfAI unit memory list.
// This uses optimization from unitExtensions (if info is available)
// You can use this even if improveAI is false
// Returns false in error cases only
bool UnitExtensionHandler::RemoveInfAIUnitMemoryForUnit(long int unitId, long int infAIPlayerId) {
	if ((unitId < 0) || (unitId >= this->currentAllocatedElemCount)) { return false; }
	if (infAIPlayerId < 0) { return false; }

	if (!ROCKNROR::IsImproveAIEnabled(infAIPlayerId)) {
		// Restricted mode: only remove from infAI list, do not use unit extensions (just remove unit pointer if existing, in case this corresponds to unit death)
		return this->RemoveInfAIUnitMemoryWithoutOptimization(unitId, infAIPlayerId);
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
		int index = this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIMemory[infAIPlayerId];
		if ((index >= 0) && (index < infAI->unitMemoryListSize)) {
			// don't forget to remove the "cached" index because ROR will re-use the slot for another unit
			this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIMemory[infAIPlayerId] = -1;
			this->allUnitExtensions[unitId].wasNotFoundInOtherPlayerInfAIMemory[infAIPlayerId] = false; // Reset this flag
			return AOE_METHODS::LISTS::ResetInfAIUnitMemoryListElem(&infAI->unitMemoryList[index]);
		}
	}
	return true; // nothing to do here
}


// Remove element for provided unitId in all players' InfAI unit memory list.
// This uses optimization from unitExtensions (if info is available)
// You can use this even if improveAI is false
// Returns true if successful for all players
bool UnitExtensionHandler::RemoveAllInfAIUnitMemoryForUnit(long int unitId) {
	bool result = true;
	for (int i = 0; i < 9; i++) {
		result &= this->RemoveInfAIUnitMemoryForUnit(unitId, i);
	}
	return result;
}


// Add/update infAI unit memory for current unit (we consider it is visible for the player specified)
// Remark: this method is called only if improveAI is true.
bool UnitExtensionHandler::AddUpdateInfAIUnitMemory(STRUCT_UNIT_BASE *unit, STRUCT_INF_AI *infAI) {
	if (!unit || !infAI) { return false; }
	long int myPlayerId = infAI->commonAIObject.playerId;
	long int unitId = unit->unitInstanceId;
	if (unitId < 0) { return false; }
	if (!this->AddUnitExtension(unit)) { // does nothing if already known
		std::string msg = "Error adding unit extension for id=";
		msg += std::to_string(unit->unitInstanceId);
		traceMessageHandler.WriteMessage(msg.c_str());
		return false; // Error case
	}
	// Now we can safely use this->allUnitExtensions[unitId]
	if (this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIMemory[myPlayerId] >= 0) {
		// InfAI index is already known for that player
		assert(this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIMemory[myPlayerId] < infAI->unitMemoryListSize);
		STRUCT_UNIT_MEMORY *elem = &infAI->unitMemoryList[this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIMemory[myPlayerId]];
		if (this->allUnitExtensions[unitId].UpdateInfAIUnitMemoryElem(elem)) {
			return true;
		}
		// Failed: maybe the index was wrong ? Reset it.
		std::string msg = "Incorrect cache index in unit extension/id=";
		msg += std::to_string(unit->unitInstanceId);
		traceMessageHandler.WriteMessage(msg.c_str());
		this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIMemory[myPlayerId] = -1;
		this->allUnitExtensions[unitId].wasNotFoundInOtherPlayerInfAIMemory[myPlayerId] = false; // Reset this flag
	}

	// Here index in infAI elem list is unknown. Search for it

	long int indexOfAFreeSlot = -1;
	for (int curIndex = 0; curIndex < infAI->unitMemoryListSize; curIndex++) {
		if (infAI->unitMemoryList[curIndex].unitId == unitId) {
			// Found: save index, update info and return
			this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIMemory[myPlayerId] = curIndex;
			this->allUnitExtensions[unitId].wasNotFoundInOtherPlayerInfAIMemory[myPlayerId] = false;
			return this->allUnitExtensions[unitId].UpdateInfAIUnitMemoryElem(&infAI->unitMemoryList[curIndex]);
		} else {
			if ((indexOfAFreeSlot < 0) && (infAI->unitMemoryList[curIndex].unitId < 0)) {
				indexOfAFreeSlot = curIndex; // Save index of a free slot for later (may be useful)
			}
		}
	}
	// Here: not found in infAI list: add it
	if (indexOfAFreeSlot < 0) {
		// May append at the end of the list... or reuse an existing slot (should not as we already tried)
		if (!AOE_METHODS::LISTS::AddUpdateInfAIMemoryList(infAI, unit)) {
			return false;
		}
		// Find index of new elem.
		STRUCT_UNIT_MEMORY *elem = AOE_METHODS::LISTS::FindInfAIUnitMemoryElem(infAI, unitId);
		long int elemIndex = (((unsigned long int)elem) - (unsigned long int)(infAI->unitMemoryList)) / sizeof(STRUCT_UNIT_MEMORY);
		this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIMemory[myPlayerId] = elemIndex;
		this->allUnitExtensions[unitId].wasNotFoundInOtherPlayerInfAIMemory[myPlayerId] = false;
		return true;
	}

	// Here: use the free slot we found
	this->allUnitExtensions[unitId].myIndexInOtherPlayerInfAIMemory[myPlayerId] = indexOfAFreeSlot;
	this->allUnitExtensions[unitId].wasNotFoundInOtherPlayerInfAIMemory[myPlayerId] = false;
	STRUCT_UNIT_MEMORY *elem = &infAI->unitMemoryList[indexOfAFreeSlot];
	return this->allUnitExtensions[unitId].WriteAllInfAIUnitMemory(elem);
}


// Adds (if necessary) unit in infAI lists (other than "spotted units detailed info)
// This is optimized (using unit extension) so that "adding" operation is done only once.
// Returns false if an error occurred
bool UnitExtensionHandler::AddUnitInOtherInfAILists(STRUCT_UNIT_BASE *unit, STRUCT_INF_AI *infAI) {
	if (!unit || !infAI) { return false; }
	long int myPlayerId = infAI->commonAIObject.playerId;
	long int unitId = unit->unitInstanceId;
	if (unitId < 0) { return true; } // Ignore temp units
	if (!this->AddUnitExtension(unit)) { // does nothing if already known
		std::string msg = "Error adding unit extension for id=";
		msg += std::to_string(unit->unitInstanceId);
		traceMessageHandler.WriteMessage(msg.c_str());
		return false; // Error case
	}
	// Now we can safely use this->allUnitExtensions[unitId]
	if (this->allUnitExtensions[unitId].hasBeenAddedToOtherPlayersInfAILists[myPlayerId]) {
		return true; // Nothing to do / but not an error ;)
	}
	
	// Here this is the first time we request adding this unit in infAI info.
	this->allUnitExtensions[unitId].hasBeenAddedToOtherPlayersInfAILists[myPlayerId] = true;

	// Remark: this code looks like 0x4BDD10 (removing some parts)
	// TODO: how about coopShareInfo...?

	bool isArtefactOrTargetableOrCreatable = CUSTOM_AI::EconomyAI::IsArtefactOrTargetableResourceOrCreatable(unit);
	if (!isArtefactOrTargetableOrCreatable || !unit->unitDefinition) {
		return true; // Nothing to do for such a unit
	}

	// addrHandleAddToElemsToDefend: Adds a unit to infAI resources arrays. Does nothing and returns 1 if already in list.
	// Returns 0 if not added. Does NOT add to defend list if corresponding SNxxxDefendPriority is 0.
	unsigned long int addrHandleAddToElemsToDefend = 0x4C1730; // cf call in 0x4BDE32
	long int longIsUnitToDefend = 0;
	// This handles adding (if relevant) into infAI->elementsToDefend. This checks for duplicates (ok).
	bool isUnitToDefend = false;
	_asm {
		MOV ECX, infAI;
		PUSH unit;
		CALL addrHandleAddToElemsToDefend;
		MOV longIsUnitToDefend, EAX;
	}
	isUnitToDefend = (longIsUnitToDefend != 0);

	// Here we know that isArtefactOrTargetableOrCreatable is true: always add in artefactsCreatableGatherableUnits
	bool addedInArtefactsCreatableGatherable = infAI->artefactsCreatableGatherableUnits.Add(unitId);

	// Possible optimization: if !addedInArtefactsCreatableGatherable, return because it must have already been added in all lists (where relevant)

	GLOBAL_UNIT_AI_TYPES unitClass = unit->unitDefinition->unitAIType;
	bool isPlayerCreatable = AOE_STRUCTURES::IsClassPlayerCreatable(unitClass);
	if (isPlayerCreatable) {
		infAI->playerCreatableUnits.Add(unitId);
	} else {
		// Not player-creatable
		if (CUSTOM_AI::EconomyAI::IsAITargetableResource(unit)) {
			// Add in infAI resource lists
			const unsigned long int addrAddGatherable = 0x4C49C0;
			_asm {
				MOV ECX, infAI;
				PUSH unit;
				CALL addrAddGatherable;
			}
		} else {
			if (CUSTOM_AI::EconomyAI::IsArtefactOrFlag(unitClass)) {
				infAI->artefactsAndFlags.Add(unitId);
			}
		}
	}
	return true;
}


}
