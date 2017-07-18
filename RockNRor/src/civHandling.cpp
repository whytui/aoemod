#include "../include/civHandling.h"


using namespace AOE_STRUCTURES;

namespace AOE_METHODS {
	namespace CIVILIZATION {

		// Call destructor on a STRUCT_CIVILIZATION_DEF object
		void CivilizationDestroy(STRUCT_CIVILIZATION_DEF *civDef) {
			if (!civDef) { return; }
			unsigned long int addrDestructor = 0x4EF4D0;
			_asm {
				MOV ECX, civDef;
				PUSH 1; // do_free=1
				CALL addrDestructor;
			}
		}

	}
}

namespace AOE_STRUCTURES {
;

// Re-alloc array of civilizations in Global structure to add some civs.
// newCivCount must be > "initial civ count" (from DAT) and < 255
// Requirement: Global structure must have been initialized and DAT file must have been read
// Added civilization pointers will be NULL (this does not create any structure)
// This updates global->civCount
bool ReallocCivArray(long int newCivCount) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) {
		return false; // global no created yet
	}
	STRUCT_CIVILIZATION_DEF **oldArray = global->civilizationDefinitions; 
	if (!oldArray || (global->civCount == 0)) {
		return false;
	}
	if (newCivCount > 255) { return false; }

	// If new count is lower, some civilizations must be dropped (and freed)
	char minCivCount = (char) ROCKNROR::crInfo.configInfo.civCount;
	if (newCivCount < minCivCount) { return false; }
	if (newCivCount < global->civCount) {
		// Drop civs from "newCount" to "currentCount-1", so only 0->newCount remains
		for (int i = newCivCount; i < global->civCount; i++) {
			STRUCT_CIVILIZATION_DEF *curCiv = global->civilizationDefinitions[i];
			AOE_METHODS::CIVILIZATION::CivilizationDestroy(curCiv);
			global->civilizationDefinitions[i] = NULL;
		}
	}

	STRUCT_CIVILIZATION_DEF **newArray = (STRUCT_CIVILIZATION_DEF**)AOEAllocZeroMemory(newCivCount, sizeof(STRUCT_CIVILIZATION_DEF*));
	if (!newArray) { return false; }
	// Copy old into new array
	size_t bytesCount = sizeof(newArray) * global->civCount;
	memcpy_s(newArray, bytesCount, oldArray, bytesCount);
	global->civilizationDefinitions = newArray;
	global->civCount = (short int) newCivCount;
	AOEFree(oldArray);
	return true;
}


// Create a new civilization definition object, copy of source object.
// Returns NULL on failure
STRUCT_CIVILIZATION_DEF *CreateCivCopy(STRUCT_CIVILIZATION_DEF *source) {
	if (!source) { return NULL; }
	STRUCT_CIVILIZATION_DEF *newObj = (STRUCT_CIVILIZATION_DEF*)AOEAlloc(sizeof(STRUCT_CIVILIZATION_DEF));
	if (!newObj) { return NULL; }
	memcpy_s(newObj, sizeof(STRUCT_CIVILIZATION_DEF), source, sizeof(STRUCT_CIVILIZATION_DEF));

	// Do NOT re-use the same pointer for resources !
	newObj->ptrResourceValues = (float *)AOEAllocZeroMemory(source->civResourcesCount, sizeof(float));
	if (newObj->ptrResourceValues) {
		size_t bytesCount = sizeof(float) * source->civResourcesCount;
		memcpy_s(newObj->ptrResourceValues, bytesCount, source->ptrResourceValues, bytesCount);
	}
	// Do NOT re-use the same unitDef objects !
	newObj->ptrUnitDefArray = (STRUCT_UNITDEF_BASE**)AOEAllocZeroMemory(source->civUnitDefCount, sizeof(STRUCT_UNITDEF_BASE*));
	if (newObj->ptrUnitDefArray) {
		for (int i = 0; i < source->civUnitDefCount; i++) {
			STRUCT_UNITDEF_BASE *copiedUnitDef = CopyUnitDefToNewUsingGoodClass(source->GetUnitDef(i));
			newObj->ptrUnitDefArray[i] = copiedUnitDef;
		}
	}

	return newObj;
}


}
