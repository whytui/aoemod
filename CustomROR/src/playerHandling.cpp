#include "../include/playerHandling.h"



namespace AOE_METHODS {

void ClearSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) {
		return;
	}
	_asm {
		MOV ECX, player;
		MOV EDX, 0x0045DCB0; // player.ClearSelectedUnits
		CALL EDX;
	}
}

// select: if true, add unit to selection. If false, remove from selection.
bool SelectUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, bool select) {
	if (!player || !player->IsCheckSumValid() || !unit || !unit->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	long int arg2 = select ? 1 : 0;
	long int result = 0;
	_asm {
		MOV ECX, player;
		PUSH arg2;
		PUSH unit;
		MOV EDX, 0x0045DC10; // player.selectUnit(pUnit, bool)
		CALL EDX;
		MOV result, EAX;
	}
	return (result != 0);
}

}

namespace PLAYER {



	// Call this to make sure "currently managed AI player" is valid, so that AI is not stuck.
	void SetAValidCurrentAIPlayerId() {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (!global) { return; }
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct((short int)global->currentlyManagedAIPlayer);
		if (player && (player->isComputerControlled)) { return; } // Currently manager playerId is OK (player is actually computer-controlled).
		for (int playerId = 1; playerId < global->playerTotalCount; playerId++) {
			player = GetPlayerStruct(playerId);
			if (player && player->isComputerControlled) {
				global->currentlyManagedAIPlayer = playerId;
				return; // Found (and set) one. Exit.
			}
		}
	}


	// Remove all AI-controlled flags for currently controlled player (cf game settings structure).
	// Only for single player games.
	void RemoveAIFlagsForCurrentlyControlledPlayer() {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		if (!player) { return; }
		player->isComputerControlled = 0;
		player->SetCustomAIFlag(0);
		// Make sure to set current AI player to a valid player
		SetAValidCurrentAIPlayerId();
	}



	// Return a list of all unitDefIds that are/can be enabled in player's tech tree.
	std::list<long int> GetActivableUnitDefIDs(AOE_STRUCTURES::STRUCT_PLAYER *player) {
		std::list<long int> result;
		if (!player || !player->IsCheckSumValid() || !player->ptrGlobalStruct || !player->ptrGlobalStruct->IsCheckSumValid()) {
			return result;
		}
		AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *playerResInfo = player->ptrResearchesStruct;
		if (!playerResInfo) {
			return result;
		}
		AOE_STRUCTURES::STRUCT_RESEARCH_DEF_INFO *resDefInfo = playerResInfo->ptrResearchDefInfo;
		if (!resDefInfo) {
			return result;
		}
		AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *techDefInfo = player->ptrGlobalStruct->technologiesInfo;
		if (!techDefInfo || !techDefInfo->IsCheckSumValid()) {
			return result;
		}
		long int techCount = techDefInfo->technologyCount;
		if (playerResInfo && resDefInfo && techDefInfo) {
			for (int researchId = 0; (researchId < playerResInfo->researchCount) && (researchId < resDefInfo->researchCount); researchId++) {
				AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS *status = &playerResInfo->researchStatusesArray[researchId];
				AOE_STRUCTURES::STRUCT_RESEARCH_DEF *researchDef = resDefInfo->GetResearchDef(researchId);
				if (researchDef && status && (status->currentStatus > AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED)) {
					int techId = researchDef->technologyId;
					if ((techId >= 0) && (techId < techCount)) {
						AOE_STRUCTURES::STRUCT_TECH_DEF *techdef = techDefInfo->GetTechDef(techId);
						for (int effectId = 0; effectId < techdef->effectCount; effectId++) {
							if ((techdef->ptrEffects[effectId].effectType == TDE_ENABLE_DISABLE_UNIT) &&
								(techdef->ptrEffects[effectId].effectClass)) { // effectClass is "Mode" for "enable disable unit"
								short int unitDefIdToAdd = techdef->ptrEffects[effectId].effectUnit;
								auto it = std::find_if(result.begin(), result.end(),
									[unitDefIdToAdd](long int tmpUnitDefId) { return (tmpUnitDefId == unitDefIdToAdd); });
								if (it == result.end()) { // Not found: add
									result.push_back(unitDefIdToAdd);
								}
							}
						}
					}
				}
			}
		}
		return result;
	}


	// Returns <> 0 if a unit is (currently) available for a given player. Returns the unit def pointer.
	// Does not consider potential researches that could enable the unit afterwards.
	// Warning: this only works for non-upgraded unit def IDs. OK for clubman, not for axeman (because axeman's DATID1 is clubman's DATID).
	long unsigned int IsUnitAvailableForPlayer(short int DAT_ID, AOE_STRUCTURES::STRUCT_PLAYER *player) {
		long int addrStructPlayer = (long int)player;
		assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
		long int fct_isUnitAvailableForPlayer = 0x4F20C0;
		int isUnitAvailableForPlayer;
		long int myDAT_ID = DAT_ID; // do NOT push short int: 2 high bytes would be arbitrary in stack (PUSH word)
		_asm {
			PUSH myDAT_ID;
			MOV EAX, fct_isUnitAvailableForPlayer;
			MOV ECX, addrStructPlayer;
			CALL EAX;
			MOV isUnitAvailableForPlayer, EAX;
		}
		return isUnitAvailableForPlayer;
	}


	// Searches (at least) a unit with provided shortcut number for given player.
	// The first matching unit is returned (arbitrary), even if there are several.
	// Returns NULL if not found
	AOE_STRUCTURES::STRUCT_UNIT_BASE *FindUnitWithShortcutNumberForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, char shortcutNumber) {
		assert(player != NULL);
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_LINK *units = player->ptrCreatableUnitsListLink;
		assert(units != NULL);
		if (units->listElemCount == 0) {
			return NULL;
		}
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *currentElem = units->lastListElement;
		int index = 0;
		while ((currentElem != NULL) && (index < units->listElemCount)) {
			AOE_STRUCTURES::STRUCT_UNIT_BASE *currentUnit = currentElem->unit;
			if (currentUnit && (currentUnit->shortcutNumber == shortcutNumber)) {
				return currentUnit; // We found a matching element
			}
			currentElem = currentElem->previousElement; // Warning, looping backwards because we store the last element
			index++;
		}
		return NULL; // Not found
	}


	// Get number of matching units (for a unitDef ID) for given player. Restricted to "creatable" units
	// -1 are jokers for DAT_ID, unitAIType, unitStatus
	long int GetPlayerUnitCount(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID,
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType, char minUnitStatus, char maxUnitStatus) {
		assert(player != NULL);
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_LINK *units = player->ptrCreatableUnitsListLink;
		assert(units != NULL);
		if (units->listElemCount == 0) {
			return 0;
		}
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *currentElem = units->lastListElement;
		int index = 0;
		long int result = 0;

		while ((currentElem != NULL) && (index < units->listElemCount)) {
			AOE_STRUCTURES::STRUCT_UNIT_BASE *currentUnit = currentElem->unit;
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = currentUnit->unitDefinition;
			if (currentUnit && currentUnit->IsCheckSumValidForAUnitClass() && unitDef && unitDef->IsCheckSumValidForAUnitClass()) {
				bool ok_DATID = (DAT_ID == -1) || (currentUnit->unitDefinition->DAT_ID1 == DAT_ID);
				bool ok_AIType = (unitAIType == TribeAINone) || (unitDef->unitAIType == unitAIType);
				bool ok_status = (minUnitStatus == -1) || (currentUnit->unitStatus >= minUnitStatus) &&
					(maxUnitStatus == -1) || (currentUnit->unitStatus <= maxUnitStatus);
				if (ok_DATID && ok_AIType && ok_status) {
					result++;
				}
			}
			currentElem = currentElem->previousElement; // Warning, looping backwards because we store the last element
			index++;
		}
		return result;
	}


	// Disable the research that enables a given unit, so that it can't be trained.
	// For units that are enabled by a "visible" research, like short sword, the research won't be visible (disabled).
	// To be called before game starts
	// Returns true if a research was found and disabled.
	// See in function code: due to technical limitations, it doesn't work for some units (villager, house, farm, fishing ship, trade ship)
	bool DisableUnitForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID) {
		if (!player || !player->IsCheckSumValid()) { return false; }
		if ((DAT_ID < 0) || (DAT_ID >= player->structDefUnitArraySize)) { return false; }
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (!global || !global->IsCheckSumValid()) { return false; }
		AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *techDefInfo = global->technologiesInfo;
		if (!techDefInfo || (!techDefInfo->IsCheckSumValid())) { return false; }

		// Search for the technology that enables that unit
		long int techId = -1;
		long int effectsCount = 0;
		for (int i = 0; (i < techDefInfo->technologyCount) && (techId == -1); i++) {
			effectsCount = 0;
			AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = techDefInfo->GetTechDef(i);
			for (int effectId = 0; techDef && (effectId < techDef->effectCount); effectId++) {
				assert(techDef->ptrEffects != NULL); // should never happen because effectCount would be 0
				// Count number of effects for this tech, excluding the "increase researches count" thing.
				if ((techDef->ptrEffects[effectId].effectType != AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET) || // resource modifier
					(techDef->ptrEffects[effectId].effectUnit != RESOURCE_TYPES::CST_RES_ORDER_TECHNOLOGIES)
					) {
					effectsCount++;
				}
				if ((techDef->ptrEffects[effectId].effectType == AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) &&
					(techDef->ptrEffects[effectId].effectUnit == DAT_ID)
					) {
					techId = i;
				}
			}
		}

		if (player->ptrResearchesStruct == NULL) { return false; }
		if (player->ptrResearchesStruct->ptrResearchDefInfo == NULL) { return false; }

		// Find the research that triggers that technology...
		long int researchId = -1;
		if (techId >= 0) {
			for (int i = 0; (i < player->ptrResearchesStruct->ptrResearchDefInfo->researchCount) && (researchId == -1); i++) {
				if (player->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef(i)->technologyId == techId) {
					researchId = i;
				}
			}
		}

		// And disable it : only if no side effects (some techs have more than 1 effect, to disable the research would remove other effects !)
		// Are concerned: tech0(fishingship+trade), tech26(farm), tech94(stoneAge:villager+house)
		// Here: we could think about forcing update for farms (tech=26) because the 2nd effect is useless ? (seems obsolete and invalid)
		if ((researchId != -1) && (effectsCount == 1)) {
			AOE_METHODS::AOE_enableResearch(player, researchId, false);
			return true;
		}
		return false;
	}


	// Transforms specified unit to "main" unit when a quick selection (via keyboard shortcut) is requested.
	// For example, for key 'H', the "main" TC is selected. This method allows to set the "main" TC to select with 'H'.
	// Actually, this moves the unit in player's "creatable" units list
	// This only has a visible effect for units that can be selected with a shortcut key.
	bool MakeMainUnitForShortcutSelection(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *unit) {
		assert(player && player->IsCheckSumValid() && unit && unit->IsCheckSumValidForAUnitClass());
		if (!player || !player->IsCheckSumValid() || !unit || !unit->IsCheckSumValidForAUnitClass()) {
			return false;
		}
		assert(unit->unitDefinition && unit->unitDefinition->IsCheckSumValidForAUnitClass());
		if (!unit->unitDefinition || !unit->unitDefinition->IsCheckSumValidForAUnitClass()) {
			return false;
		}
		if (unit->ptrStructPlayer != player) {
			return false;
		}
		short int unitDefId = unit->unitDefinition->DAT_ID1;
		assert(player->ptrCreatableUnitsListLink && player->ptrCreatableUnitsListLink->IsCheckSumValid());
		if (!player->ptrCreatableUnitsListLink || !player->ptrCreatableUnitsListLink->IsCheckSumValid()) {
			return false;
		}
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *curElem = player->ptrCreatableUnitsListLink->lastListElement;
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *firstFound = NULL;
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *thisUnitElem = NULL;
		while (curElem && (!firstFound || !thisUnitElem)) {
			if (curElem->unit == unit) {
				thisUnitElem = curElem;
			} else {
				if (!firstFound && curElem->unit && curElem->unit->IsCheckSumValidForAUnitClass() &&
					curElem->unit->unitDefinition && curElem->unit->unitDefinition->IsCheckSumValidForAUnitClass() &&
					(curElem->unit->unitDefinition->DAT_ID1 == unitDefId)) {
					firstFound = curElem;
				}
			}
			curElem = curElem->previousElement;
		}
		if (firstFound && thisUnitElem && (thisUnitElem != firstFound)) {
			// Exchange position with "main" unit
			thisUnitElem->unit = firstFound->unit;
			firstFound->unit = unit;
			return true;
		}
		return (firstFound == thisUnitElem); // "Success" if was already first in list
	}

	// Transforms specified unit to "main" unit when a quick selection (via keyboard shortcut) is requested.
	// For example, for key 'H', the "main" TC is selected. This method allows to set the "main" TC to select with 'H'.
	// Actually, this moves the unit in player's "creatable" units list
	// This only has a visible effect for units that can be selected with a shortcut key.
	// This is only possible when unitId belongs to human-controlled player.
	bool MakeMainUnitForShortcutSelection(long int unitId) {
		STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		assert(global && global->IsCheckSumValid());
		STRUCT_UNIT_BASE *unitBase = global->GetUnitFromId(unitId);
		if (!unitBase || !unitBase->IsCheckSumValidForAUnitClass()) {
			return false;
		}
		if (unitBase->ptrStructPlayer != GetControlledPlayerStruct_Settings()) {
			return false;
		}
		return MakeMainUnitForShortcutSelection(unitBase->ptrStructPlayer, unitBase);
	}

}

