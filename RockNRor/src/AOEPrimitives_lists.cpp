#include "../include/AOEPrimitives_lists.h"


using namespace AOE_STRUCTURES;

namespace AOE_METHODS {
namespace LISTS {
;


// Reset an element in infAI.unitMemory list. The slot will be re-used later by ROR. cf 0x4BA401.
// Return true if the element was updated (reset).
// Please DO NOT USE directly. See unitExtensionHandler
bool ResetInfAIUnitMemoryListElem(AOE_STRUCTURES::STRUCT_UNIT_MEMORY *elem) {
	if (!elem) { return false; }
	elem->unitId = -1;
	elem->unitDefId = -1;
	elem->unitClass = (AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES) - 1;
	elem->posX = 0;
	elem->posY = 0;
	elem->posZ = 0;
	elem->playerId = 0;
	elem->HP = 0;
	elem->attackAttempts = 0;
	elem->unknown_14 = 0;
	elem->attack = 0;
	elem->reloadTime1 = 0;
	elem->maxRange = 0;
	return true;
}


// Find a unitElem in infAI list, returns NULL if not found. Similar to 0x4BD710.
// Please DO NOT USE directly. See unitExtensionHandler
AOE_STRUCTURES::STRUCT_UNIT_MEMORY *FindInfAIUnitMemoryElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int unitId) {
	if (!infAI || !infAI->IsCheckSumValid() || (unitId < 0)) { return NULL; }
	for (int i = 0; i < infAI->unitMemoryListSize; i++) {
		if (infAI->unitMemoryList[i].unitId == unitId) {
			return &infAI->unitMemoryList[i];
		}
	}
	return NULL;
}


// Add a unit in infAI elem list, or update if existing. Warning: this uses unit structure's info, even if it is not visible !
// Please check for visibility to avoid "cheating"
// Returns true if successful
// This executes ROR's code
// Please DO NOT USE directly. See unitExtensionHandler
bool AddUpdateInfAIMemoryList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	if (!infAI || !infAI->IsCheckSumValid() || !unit || !unit->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	unsigned long int addr = 0x4BD750;
	// Use ROR method that does all the stuff...
	long int result;
	_asm{
		MOV ECX, infAI;
		PUSH unit;
		CALL addr;
		MOV result, EAX;
	}
	return (result != 0);
}


// Remove a unitId from infAI "info" lists (creatable, gatherable, "defendable", artefact units).
// This does NOT manage "all my units" and "my buildings" lists. This does NOT manage unitElemList neither.
// DATID and unitAIType (unitClass) are used for optimisation. You can provide -1 if you don't have the information.
// Returns false if failed.
// Please DO NOT USE directly. See unitExtensionHandler
bool RemoveFromInfAIMemoryList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int unitId, short int DATID, AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType) {
	if (!infAI || !infAI->IsCheckSumValid() || (unitId < 0)) { return false; }

	if ((unitAIType == -1) || IsClassArtefactOrGatherableOrCreatable(unitAIType)) {
		infAI->artefactsCreatableGatherableUnits.Remove(unitId);
	}
	if ((unitAIType == -1) || IsClassPlayerCreatable(unitAIType)) {
		infAI->playerCreatableUnits.Remove(unitId);
	}
	if ((unitAIType == -1) || (unitAIType == TribeAIGroupArtefact) || (unitAIType == TribeAIGroupFlag)) {
		infAI->artefactsAndFlags.Remove(unitId);
	}
	// Hardcoded list of units: cf 0x4C1730 = infAI.AddUnitToDefend(unitStruct)
	if ((DATID == -1) || (unitAIType == -1) ||
		(DATID == CST_UNITID_FORUM) || (DATID == CST_UNITID_DOCK) ||
		(DATID == CST_UNITID_RELIC) ||
		(DATID == CST_UNITID_RUIN) || (DATID == CST_UNITID_RUIN2) ||
		(unitAIType == TribeAIGroupStoneMine) || (unitAIType == TribeAIGroupGoldMine) ||
		(unitAIType == TribeAIGroupBerryBush)) {
		// Note: gaia elements to defend are never removed when deleted (depleted)... (because not belonging to the same player !)
		infAI->elementsToDefend.Remove(unitId);
	}
	return true;
}



// Remove a building from arrays for a player
void PlayerBldHeader_RemoveBldFromArrays(AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	if (buildingsHeader && unit && unit->IsCheckSumValidForAUnitClass()) {
		_asm {
			MOV ECX, buildingsHeader;
			MOV EDX, unit;
			PUSH EDX;
			MOV EAX, 0x00436980; // playerUnitsHeader.removeBuildingFromArrays(ptrUnit)
			CALL EAX;
		}
	}
}


// Set status for an element in infAI "construction history" array.
void InfAIBuildHistory_setStatus(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int posX, long int posY, long int unitDefId,
	AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS status) {
	if (!infAI || !infAI->IsCheckSumValid()) {
		return;
	}
	// Note: bad position values have no impact here (no possible crash), it will just find no item. Same for unitDefId.
	assert((status >= 0) && (status <= AOE_CONST_INTERNAL::CST_BHS_MAX_VALUE));
	long int status_long = status;
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	const unsigned long int RORMethod = 0x4C34C0;
	_asm {
		MOV ECX, infAI;
		PUSH status_long;
		PUSH posX;
		PUSH posY;
		PUSH unitDefId;
		CALL RORMethod;
	}
}


}
}
