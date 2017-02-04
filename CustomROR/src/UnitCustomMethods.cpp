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



// Entry point when creating unit activity structure
// Returns true if ROR code can create unitAI (activity) on its own (default).
// Returns false if we want to skip ROR code (so that it does not create unitAI)
bool OnUnitCreateActivityStruct(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase) {
	bool allowRorToCreateUnitActivity = AllowCreateActivityStructForUnit(unitBase);
	if (!unitBase || !unitBase->IsCheckSumValidForAUnitClass() || !unitBase->ptrStructPlayer ||
		!unitBase->unitDefinition || !unitBase->unitDefinition->IsCheckSumValidForAUnitClass()) {
		return allowRorToCreateUnitActivity;
	}
	if (IsImproveAIEnabled(unitBase->ptrStructPlayer->playerId)) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unitBase->unitDefinition;
		if (unitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupDomesticatedAnimal) {
			// Create unit activity (example: trained_lion)
			AOE_METHODS::UNIT::CreateUnitActivity(unitBase, CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL);
		}
		bool hasTradingAbility = false;
		bool hasAttackAbility = false;
		STRUCT_UNIT_COMMAND_DEF *tradeCommandDef = NULL;
		if (unitDef->DerivesFromAttackable()) {
			AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDefAtt = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDef;
			if (unitDefAtt->ptrUnitCommandHeader && (unitDefAtt->attacksCount <= 0)) {
				for (int i = 0; i < unitDefAtt->ptrUnitCommandHeader->commandCount; i++) {
					if (unitDefAtt->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_TRADE) {
						tradeCommandDef = unitDefAtt->ptrUnitCommandHeader->ptrCommandArray[i];
						hasTradingAbility = true;
					}
					hasAttackAbility |= (unitDefAtt->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_UNKNOWN_7);
				}
			}
		}
		if ((unitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTradeCart) || (hasTradingAbility && !hasAttackAbility)) {
			// Trade activity class is hardcoded for trade boats, we can't use it for "custom" units.
			// Using another class than ACTIVITY_BASE causes at least a bug: unit can't deposit to drop site (using right click) !
			// activity Base causes the unit to auto-attack enemies (even if no attack, no attack graphic, etc). Same bug as archimedes !
			//AOE_METHODS::UNIT::CreateUnitActivity(unitBase, CHECKSUM_UNIT_ACTIVITY_BASE);
			// Set important units = buildings (hardcoded). Should we keep this ? It's ok as long as drop site is a building...
			/*if (unitBase->currentActivity) {
				if (unitBase->currentActivity->listOfImportantUnitAITypes) { // Should not happen
					assert(false && "Unexpected list of important units");
					AOEFree(unitBase->currentActivity->listOfImportantUnitAITypes);
					unitBase->currentActivity->listOfImportantUnitAITypesArraySize = 0;
				}
				unitBase->currentActivity->listOfImportantUnitAITypes = (long int *)AOEAlloc(0x4);
				unitBase->currentActivity->listOfImportantUnitAITypes[0] = GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding;
				unitBase->currentActivity->listOfImportantUnitAITypesArraySize = 1;
			}*/
			allowRorToCreateUnitActivity = false;
		}
		if (unitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTradeBoat) {
			if (unitDef->DerivesFromAttackable()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *unitDefComm = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)unitDef;
				bool tradeTargetIsDock = (tradeCommandDef && (tradeCommandDef->unitDefId == CST_UNITID_DOCK));
				bool tradeDropSiteIsDock = ((unitDefComm->dropSite1 = CST_UNITID_DOCK) || (unitDefComm->dropSite2 = CST_UNITID_DOCK));
				if (!tradeTargetIsDock || !tradeDropSiteIsDock) {
					// NOT standard trade boat case.
					// We have a trade unit that do not trade "dock->dock": do NOT use TRADE activity because it wouldn't work
					// activity Base causes the unit to auto-attack enemies (even if no attack, no attack graphic, etc). Same bug as archimedes !
					//AOE_METHODS::UNIT::CreateUnitActivity(unitBase, CHECKSUM_UNIT_ACTIVITY_BASE);
					//allowRorToCreateUnitActivity = false;
				}
			}
		}
	}

	// A security:
	if (unitBase->currentActivity && allowRorToCreateUnitActivity) {
		assert(false && "Should not allow creating unitAI when there is already one");
		allowRorToCreateUnitActivity = false;
	}
	return allowRorToCreateUnitActivity;
}


// Returns true if the unit specified can have a unit activity.
bool AllowCreateActivityStructForUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase) {
	if (!unitBase || !unitBase->IsCheckSumValidForAUnitClass() || !unitBase->unitDefinition) { return false; }
	switch (unitBase->unitDefinition->unitAIType) {
		// List of unit classes that can't have "unit AI" (unit activity)
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupBerryBush:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupFlag:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupGoldMine:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupOther_Dead_Projectile:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSeaFish:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupShoreFish:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupUnknownFish:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupTree:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupTreeStump:
		// Those ones are not correctly handled in 0x4AFBE0=unit.createUnitActivity() (however, there is no bug because this is only called for living units)
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupBird:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupCliff:
		// Any added custom class that shouldn't have unit AI should be added here...
		return false;
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupTradeCart:
		// In original game, it is considered like a military unit ! It shouldn't.
		return false;
	default:
		return true;
	}
}



}
}
