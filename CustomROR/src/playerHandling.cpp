#include "../include/playerHandling.h"

namespace PLAYER {


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

