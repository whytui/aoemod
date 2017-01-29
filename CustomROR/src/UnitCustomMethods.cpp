#include "../include/UnitCustomMethods.h"

namespace CUSTOMROR {
namespace UNIT {
;


// Returns how many units were told to move.
// Returns <0 if there is an error
int MoveIdleMilitaryUnitsToMousePosition(STRUCT_PLAYER *player, float maxDistance) {
	if (!player) { return -1; }
	STRUCT_AI *ai = player->ptrAIStruct;
	if (!ai || !ai->allMyUnits.unitIdArray) { return -1; }
	if (ai->structTacAI.landMilitaryUnits.usedElements == 0) { return 0; }

	float posY;
	float posX;
	if (!GetGamePositionUnderMouse(&posX, &posY)) { return 0; }
	if ((posX < 0) || (posY < 0)) { return 0; }
	int result = 0;

	long int unitCount = ai->allMyUnits.usedElements;
	if (ai->allMyUnits.arraySize < unitCount) {
		unitCount = ai->allMyUnits.arraySize;
		traceMessageHandler.WriteMessage("Invalid element count in AI.allMyUnits !");
	}
	long int *myUnitsList = ai->allMyUnits.unitIdArray;
	for (long int index = 0; index < unitCount; index++) {
		long int unitId = myUnitsList[index];
		STRUCT_UNIT_COMMANDABLE *unit = NULL;
		if (unitId > 0) {
			unit = (STRUCT_UNIT_COMMANDABLE *)GetUnitStruct(unitId);
		}
		if (unit != NULL) {
			STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
			assert(unitDef != NULL);
			if ((unit->DerivesFromCommandable()) &&
				(unitDef->unitAIType != TribeAIGroupCivilian) &&
				(unitDef->unitAIType != TribeAIGroupFishingBoat) &&
				(unitDef->unitAIType != TribeAIGroupTradeBoat) &&
				(unitDef->unitAIType != TribeAIGroupTransportBoat) &&
				(unitDef->unitAIType != TribeAIGroupWarBoat) &&
				(unitDef->unitAIType != TribeAIGroupArtefact)
				) {
				if (AOE_METHODS::UNIT::IsUnitIdle(unit)) {
					// maxDistance <= 0 : argument is ignored (=> always true). Otherwise: check distance condition
					if ((maxDistance <= 0) || (GetDistance(posX, posY, unit->positionX, unit->positionY) <= maxDistance)) {
						GAME_COMMANDS::CreateCmd_RightClick(unit->unitInstanceId, -1, posX, posY);
						//MoveUnitToTargetOrPosition(unit, NULL, posX, posY); // not MP-compatible at this point
						result++;
					}
				}
			}
		}
	}

	return result;
}


// Searches all idle units in a specified range (see config) and order them to come at screen location
// Requires ManageAI !
// Disabled in MP games.
void CallNearbyIdleMilitaryUnits() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	short int playerId = globalStruct->humanPlayerId;
	if ((playerId < 0) || (playerId > 8)) { return; }

	if (IsMultiplayer()) { return; } // MP protection
	AOE_STRUCTURES::STRUCT_PLAYER **playerTable = globalStruct->GetPlayerStructPtrTable();
	if (!playerTable || !playerTable[playerId]) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = playerTable[playerId];
	MoveIdleMilitaryUnitsToMousePosition(player, (float)CUSTOMROR::crInfo.configInfo.distanceToCallNearbyIdleMilitaryUnits);
}


// Select next idle military unit for current player
void SelectNextIdleMilitaryUnit() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	short int playerId = globalStruct->humanPlayerId;
	if ((playerId < 0) || (playerId > 8)) { return; }

	AOE_STRUCTURES::STRUCT_PLAYER **playerTable = globalStruct->GetPlayerStructPtrTable();
	if (!playerTable || !playerTable[playerId]) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = playerTable[playerId];
	if (!player || !player->ptrCreatableUnitsListLink || (player->ptrCreatableUnitsListLink->listElemCount <= 0)) {
		return;
	}
	assert(player->ptrCreatableUnitsListLink->IsCheckSumValid());

	AOE_STRUCTURES::STRUCT_UNIT_BASE *mainSelectedUnit = CUSTOMROR::crInfo.GetMainSelectedUnit(player);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *firstIgnoredUnit = NULL; // in case seelcted unit = last from list, keep in memory first one to loop back
	bool ignoreUntilSelectedUnitIsMet = false;
	if (mainSelectedUnit && mainSelectedUnit->IsCheckSumValidForAUnitClass()) {
		ignoreUntilSelectedUnitIsMet = true;
	}

	AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *currentUnitElem = player->ptrCreatableUnitsListLink->lastListElement;
	while (currentUnitElem) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)currentUnitElem->unit;
		if (unitBase && unitBase->IsCheckSumValidForAUnitClass() && unitBase->unitDefinition && unitBase->unitDefinition->IsCheckSumValidForAUnitClass()) {
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unitBase->unitDefinition;
			char result;
			if ((unitBase->transporterUnit == NULL) && IsNonTowerMilitaryUnit(unitDefBase->unitAIType)) { // Excludes towers
				_asm {
					MOV ECX, unitBase;
					MOV EDX, DS:[ECX];
					CALL DS : [EDX + 0x210]; // unit.IsIdle() (this just test action status : not expensive)
					MOV result, AL
				}
				if (result) {
					if (!ignoreUntilSelectedUnitIsMet) {
						SelectOneUnit(player, unitBase, true);
						return;
					}
					if (firstIgnoredUnit == NULL) {
						firstIgnoredUnit = unitBase;
					}
				}
			}
			if (unitBase == mainSelectedUnit) {
				ignoreUntilSelectedUnitIsMet = false;
			}
		}
		currentUnitElem = currentUnitElem->previousElement;
	}
	if (firstIgnoredUnit) {
		SelectOneUnit(player, firstIgnoredUnit, true);
	}
}


// Handles the event "farm is depleted". NOT called when a farm is destroyed/killed.
// Warning: this event occurs before the farm unit is actually "killed"
void OnFarmDepleted(long int farmUnitId) {
	AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farm = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)GetUnitStruct(farmUnitId);
	if (!farm || !farm->IsCheckSumValid()) { return; } // (test BUILDING checksum)
	AOE_STRUCTURES::STRUCT_PLAYER *player = farm->ptrStructPlayer;
	if (!player || !player->IsCheckSumValid()) { return; }
	if (player != GetControlledPlayerStruct_Settings()) { return; } // Only for human-controlled player
	if (!player->ptrCreatableUnitsListLink || !player->ptrCreatableUnitsListLink->IsCheckSumValid()) { return; }
	if (!IsGameRunning()) {
		assert(false);
		traceMessageHandler.WriteMessage("OnFarmDepleted called but game not running");
		return;
	}

	// Is feature enabled ?
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	CUSTOMROR::CONFIG::AutoRebuildFarmConfig *autoRebuildFarmConfig = CUSTOMROR::crInfo.configInfo.GetAutoRebuildFarmConfig(settings->rgeGameOptions.isScenario || settings->isCampaign, settings->isDeathMatch);
	if (!autoRebuildFarmConfig->enableAutoRebuildFarms) { return; }

	// Check auto-rebuild farms conditions (parameters)
	if (player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_FOOD) > autoRebuildFarmConfig->autoRebuildFarms_maxFood) { return; }
	if (player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_WOOD) < autoRebuildFarmConfig->autoRebuildFarms_minWood) { return; }
	// Remark : currentFarmCount includes current farm (that is going to be deleted)
	long int currentFarmCount = PLAYER::GetPlayerUnitCount(player, CST_UNITID_FARM, GLOBAL_UNIT_AI_TYPES::TribeAINone, 0, 2); // Include being-built farms
	bool farmCountConditionIsOK = (currentFarmCount <= autoRebuildFarmConfig->autoRebuildFarms_maxFarms);

	AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *farmerUnit = NULL;
	// Search for the farmer that was working on this farm (first -arbitrary- one if there are many)
	AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *curElem = player->ptrCreatableUnitsListLink->lastListElement;
	while ((curElem != NULL) && (farmerUnit == NULL)) {
		if (curElem->unit && curElem->unit->IsCheckSumValidForAUnitClass() && curElem->unit->unitDefinition &&
			curElem->unit->unitDefinition->IsCheckSumValidForAUnitClass() && (curElem->unit->unitDefinition->DAT_ID1 == CST_UNITID_FARMER)) {
			AOE_STRUCTURES::STRUCT_ACTION_BASE *curUnitAction = AOE_STRUCTURES::GetUnitAction(curElem->unit);
			// There is 1 special case when farmer's resourceType is NOT berryBush: when AI player repairs a farm (bug: villager type is farmer instead of repairman)
			// Also, if more than 1 villager is gathering the same farm, it is possible than some of them have a resourceId = -1.
			if (curUnitAction && (curUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_GATHER_NO_ATTACK)) {
				if ((curUnitAction->targetUnitId == farmUnitId) && (curElem->unit->resourceTypeId == AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_BERRY_STORAGE)) {
					farmerUnit = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)curElem->unit;
				}
			}
		}
		curElem = curElem->previousElement;
	}
	if (farmerUnit) {
		if (!farmCountConditionIsOK) {
			FarmRebuildInfo *ftmp = CUSTOMROR::crInfo.myGameObjects.FindFarmRebuildInfo(farm->positionX, farm->positionY);
			if (ftmp && ftmp->forceRebuild) {
				farmCountConditionIsOK = true; // "Force rebuild" flag is stronger than farm number limitation.
			}
		}

		if (farmCountConditionIsOK) {
			// Add/Update farm rebuild info to trigger construction of a new farm when "this" one is actually removed.
			FarmRebuildInfo *f = CUSTOMROR::crInfo.myGameObjects.FindOrAddFarmRebuildInfo(farm->positionX, farm->positionY);
			f->villagerUnitId = farmerUnit->unitInstanceId;
			f->playerId = player->playerId;
			f->posX = farm->positionX; // Only useful if just added (otherwise, unchanged)
			f->posY = farm->positionY; // Only useful if just added (otherwise, unchanged)
			f->gameTime = (player->ptrGlobalStruct != NULL) ? player->ptrGlobalStruct->currentGameTime : 0;
		}
	}
}


// Returns true if a shortcut has been added/modified
bool AutoAssignShortcutToUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	// Only care about living units or building
	if ((unit->unitType != GUT_BUILDING) && (unit->unitType != GUT_TRAINABLE)) {
		return false;
	}
	// We choose not to modify existing shortcut
	if (unit->shortcutNumber != 0) { return false; }

	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
	assert(unitDef != NULL);
	assert(unitDef->IsCheckSumValidForAUnitClass());

	int shortcutId = CUSTOMROR::crInfo.configInfo.unitShortcutsPriorityReverseOrder ? 9 : 1;
	for (int currentIndex = 1; currentIndex < CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS; currentIndex++) {
		if (CUSTOMROR::crInfo.configInfo.unitShortcutsPriorityReverseOrder) {
			shortcutId = CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS - currentIndex;
		}
		assert(shortcutId < CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS);
		assert(shortcutId >= 0);
		CONFIG::UnitSpawnShortcutInfo *shortcutInfo = &CUSTOMROR::crInfo.configInfo.unitShortcutsInformation[shortcutId];
		if (shortcutInfo->DAT_ID == unitDef->DAT_ID1) {
			// This rule matches our unit DAT ID.
			// If "only 1 unit" is NOT enabled, always assign. Otherwise, check the shortcut number is still unused.
			if (!shortcutInfo->onlyOneUnit || PLAYER::FindUnitWithShortcutNumberForPlayer(unit->ptrStructPlayer, shortcutId) == NULL) {
				unit->shortcutNumber = shortcutId;
				return true;
			}
		}
	}
	return false;
}


// Occurs when a unit is killed by an attack (EXCLUDES suicide with DEL, transported units whose transport is destroyed, conversion)
void OnAttackableUnitKilled(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *killedUnit, AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit) {
	if (!actorUnit || !actorUnit->IsCheckSumValidForAUnitClass()) {
		return;
	}
	if (!killedUnit || !killedUnit->IsCheckSumValidForAUnitClass()) {
		return;
	}
	assert(killedUnit->DerivesFromAttackable());
	if (!killedUnit->DerivesFromAttackable()) { return; }

	if (CUSTOMROR::IsRpgModeEnabled() && actorUnit->DerivesFromTrainable() && (killedUnit->ptrStructPlayer->playerId > 0)) {
		RPG_MODE::OnUnitKill(killedUnit, (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE*)actorUnit);
	}

	// Handle internal objects
	CUSTOMROR::crInfo.myGameObjects.RemoveAllInfoForUnit(killedUnit->unitInstanceId, killedUnit->positionX, killedUnit->positionY);
	AOE_STRUCTURES::STRUCT_UNIT_BUILDING *killedUnitAsBld = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *) killedUnit;
	if (killedUnitAsBld->IsCheckSumValid()) {
		// When destroyed, buildings do not trigger "tacAI.reactToEvent" (they only do it when attacked, but still having HP left)
		if (killedUnit->ptrStructPlayer && killedUnit->ptrStructPlayer->ptrAIStruct &&
			IsImproveAIEnabled(killedUnit->ptrStructPlayer->playerId)) {
			CUSTOM_AI::customAIHandler.OnUnitAttacked(&killedUnit->ptrStructPlayer->ptrAIStruct->structTacAI, killedUnit, actorUnit, false);
		}
	}
}


}
}