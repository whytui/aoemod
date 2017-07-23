#include "../include/techDefHandling.h"



namespace AOE_STRUCTURES {
	
	// Re-alloc array of technology definitions in Global structure to add some techs.
	// Requirement: Global structure must have been initialized and DAT file must have been read
	// Added technology definition pointers will be NULL (this does not create any structure)
	// This updates global->technologiesInfo->{ptrTechDefArray and technologyCount}
	bool ReallocTechDefArray(long int newTechDefCount) {
		STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (!global || !global->IsCheckSumValid() || !global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid()) {
			return false; // global no created yet
		}
		if (global->technologiesInfo->technologyCount == 0) { return false; }
		if (global->technologiesInfo->technologyCount >= newTechDefCount) {
			return false; // Remove some techs: not supported yet
		}

		STRUCT_TECH_DEF *oldArray = global->technologiesInfo->ptrTechDefArray;
		if (!oldArray) { return false; }

		STRUCT_TECH_DEF *newArray = (STRUCT_TECH_DEF*)AOEAllocZeroMemory(newTechDefCount, sizeof(STRUCT_TECH_DEF));
		if (!newArray) { return false; }

		// Copy old into new array
		size_t bytesCount = sizeof(STRUCT_TECH_DEF) * global->technologiesInfo->technologyCount;
		memcpy_s(newArray, bytesCount, oldArray, bytesCount);

		// Initialized added ones (in practice it should not be necessary because of ZeroMemory, as we're only setting 0/NULL values)
		for (int i = global->technologiesInfo->technologyCount; i < newTechDefCount; i++) {
			STRUCT_TECH_DEF *curTech = &newArray[i];
			curTech->effectCount = 0;
			curTech->ptrEffects = NULL; // OK when count is 0
			curTech->techName[0] = 0;
		}

		global->technologiesInfo->ptrTechDefArray = newArray;
		global->technologiesInfo->technologyCount = newTechDefCount;
		AOEFree(oldArray);
		return true;
	}


	// Removes all effects for a technology definition
	bool CleanEffectsForTechDef(STRUCT_TECH_DEF *techDef) {
		if (!techDef) { return false; }
		if (techDef->ptrEffects) {
			// STRUCT_TECH_DEF_EFFECT does not contain pointers: we can free it directly (no need for more "destructor" actions)
			AOEFree(techDef->ptrEffects);
		}
		techDef->effectCount = 0;
		techDef->ptrEffects = NULL;
		return true;
	}

}
