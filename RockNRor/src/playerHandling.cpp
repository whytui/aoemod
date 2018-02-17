#include "../include/playerHandling.h"



namespace AOE_METHODS {
namespace PLAYER {
;


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


// Return the matching score element
// Warning: May return NULL.
AOE_STRUCTURES::STRUCT_SCORE_ELEM *FindScoreElement(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_CONST_FUNC::SCORE_CATEGORIES category, AOE_CONST_FUNC::RESOURCE_TYPES resourceId) {
	if (!player) { return NULL; }
	AOE_STRUCTURES::STRUCT_VICTORY_CONDITIONS *header = player->ptrVictoryConditions;
	assert(header != NULL); // Should never happen (or maybe for gaia?)
	if (!header) { return NULL; }
	assert(header->IsCheckSumValid());
	if (!header->IsCheckSumValid()) { return NULL; }
	AOE_STRUCTURES::STRUCT_SCORE_ELEM *currentElem = header->firstScoreElement;
	//if (currentElem) {
	//currentElem = currentElem->next;
	//}
	int security = 0;
	while (currentElem && (security < 200)) {
		if ((currentElem->category == category) && (currentElem->resourceId == resourceId)) {
			return currentElem;
		}
		security++;
		currentElem = currentElem->next;
	}
	assert(security < 199); // to alert on infinite loops ! (debug)
	return NULL;
}


// Call this when changing a player from "AI control" disabled to enabled
// This will run various actions to fix strategy, etc (example: do not build buildings human already built).
// The method will do nothing if player is NULL or if its AI structure is NULL.
void CheckAIWhenEnablingAIControl(int playerId) {
	CheckAIWhenEnablingAIControl(GetPlayerStruct(playerId));
}

// Call this when changing a player from "AI control" disabled to enabled
// This will run various actions to fix strategy, etc (example: do not build buildings human already built).
// The method will do nothing if player is NULL or if its AI structure is NULL.
void CheckAIWhenEnablingAIControl(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	ROCKNROR::STRATEGY::UpdateStrategyWithUnreferencedExistingUnits(player);
}


// Disable AI flags for human players, based on game initial settings (to be used at game startup)
void DisableAIFlagsForHuman() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }

	bool isSinglePlayer = !IsMultiplayer();

	AOE_STRUCTURES::STRUCT_MP_COMMUNICATION *mpComm = *(AOE_STRUCTURES::STRUCT_MP_COMMUNICATION **)AOE_OFFSETS::ADDR_MP_COMM_STRUCT;
	assert(mpComm != NULL);
	int aValidAIPlayer = -1;
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStructPtrTable()[loopPlayerId];
		bool thisPlayerIsHuman = (isSinglePlayer && (loopPlayerId == global->humanPlayerId)) || // SP: use global struct's human playerId: it is always reliable even in loaded games
			(!isSinglePlayer && (mpComm->playerTypes[loopPlayerId] == 2)); // MP: use initial setting from "common MP struct". This is not reliable for saved games but OK in MP games.
		if (thisPlayerIsHuman) {
			player->isComputerControlled = 0;
			player->SetCustomAIFlag(0);
		}
		if ((!thisPlayerIsHuman) && (aValidAIPlayer == -1)) {
			// Need to save one arbitrary AI player ID to set "currently managed AI player" or everything will be stuck
			aValidAIPlayer = loopPlayerId;
		}
	}
	global->currentlyManagedAIPlayer = aValidAIPlayer; // Required to avoid stuck AI
}


// Restore AI flags based on human-controlled playerID (to be used in SP games only)
void RestoreAllAIFlags() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }

	int aValidAIPlayer = -1;
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStructPtrTable()[loopPlayerId];
		if (loopPlayerId == global->humanPlayerId) {
			player->isComputerControlled = 0;
			player->SetCustomAIFlag(0);
		} else {
			if (player->isComputerControlled == 0) {
				// Player was NOT AI-controlled. We need to update its strategy with (potentially) human-created units
				CheckAIWhenEnablingAIControl(player);
			}
			player->isComputerControlled = 1;
			player->SetCustomAIFlag(1);
			// Need to save an arbitrary AI player ID to set "currently managed AI player" or everything will be stuck
			if (aValidAIPlayer == -1) {
				aValidAIPlayer = loopPlayerId;
			}
		}

	}
	global->currentlyManagedAIPlayer = aValidAIPlayer; // Required to avoid stuck AI
}


// This enables AI flags for all players
void SetAllAIFlags(bool enable) {
	if (IsMultiplayer()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStruct(loopPlayerId);
		assert(player);
		if (enable) {
			if (player->isComputerControlled == 0) {
				// Player was NOT AI-controlled. We need to update its strategy with (potentially) human-created units
				CheckAIWhenEnablingAIControl(player);
			}
		}
		player->isComputerControlled = enable ? 1 : 0;
		player->SetCustomAIFlag(enable ? 1 : 0);
	}
}


// Set AI flag for specified player
// enableMode : 0=disable AI, 1=enable AI, -1=switch
void SetAIFlag(int playerId, int enableMode) {
	if (IsMultiplayer()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStruct(playerId);
	if (!player) { return; }
	bool enable = (enableMode == 1);
	if (enableMode == -1) {
		enable = !player->IsAIActive(ROCKNROR::crInfo.hasManageAIFeatureON);
	}
	if (enable) {
		if (player->ptrAIStruct == nullptr) {
			return;
		}
		if (player->isComputerControlled == 0) {
			// Player was NOT AI-controlled. We need to update its strategy with (potentially) human-created units
			CheckAIWhenEnablingAIControl(player);
		}
	}
	player->isComputerControlled = enable ? 1 : 0;
	player->SetCustomAIFlag(enable ? 1 : 0);

	// Check that a valid AI player is set as "currently managed AI player"
	AOE_STRUCTURES::STRUCT_PLAYER *curManagedPlayer = global->GetPlayerStruct(global->currentlyManagedAIPlayer);
	if (!curManagedPlayer || !curManagedPlayer->IsAIActive(ROCKNROR::crInfo.hasManageAIFeatureON)) {
		for (int i = 1; i < global->playerTotalCount; i++) {
			AOE_STRUCTURES::STRUCT_PLAYER *loopPlayer = global->GetPlayerStruct(i);
			if (loopPlayer && loopPlayer->IsAIActive(ROCKNROR::crInfo.hasManageAIFeatureON)) {
				global->currentlyManagedAIPlayer = i;
				break;
			}
		}
	}
}


// Changes destination player id screen position from srcPlayerId's
bool CopyScreenPosition(const AOE_STRUCTURES::STRUCT_PLAYER *srcPlayer, AOE_STRUCTURES::STRUCT_PLAYER *destPlayer)  {
	if (!srcPlayer || !destPlayer || !srcPlayer->IsCheckSumValid() || !destPlayer->IsCheckSumValid()) { return false; }
	destPlayer->screenPositionX = srcPlayer->screenPositionX;
	destPlayer->screenPositionY = srcPlayer->screenPositionY;
	destPlayer->mapLocationPosX = srcPlayer->mapLocationPosX;
	destPlayer->mapLocationPosY = srcPlayer->mapLocationPosY;
	return true;
}
bool CopyScreenPosition(int srcPlayerId, int destPlayerId) {
	if ((srcPlayerId < 0) || (srcPlayerId > 8)) { return false; }
	if ((destPlayerId < 0) || (destPlayerId > 8)) { return false; }
	AOE_STRUCTURES::STRUCT_PLAYER *srcPlayer = GetPlayerStruct(srcPlayerId);
	AOE_STRUCTURES::STRUCT_PLAYER *destPlayer = GetPlayerStruct(destPlayerId);
	return CopyScreenPosition(srcPlayer, destPlayer);
}


// Change human control to another player and set AI flags accordingly (if updateAIFlags is true)
AOE_STRUCTURES::STRUCT_PLAYER *ChangeControlledPlayer(int playerId, bool updateAIFlags) {
	if ((playerId < 0) || (playerId > 8)) { return NULL; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
	// Do not switch to defeated (or invalid) player
	if (!player || !player->IsCheckSumValid() || (player->aliveStatus == 2)) { return NULL; }
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return NULL; }
	STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return NULL; }
	STRUCT_PLAYER *previousPlayer = global->GetPlayerStruct(global->humanPlayerId);
	if (!previousPlayer || !previousPlayer->IsCheckSumValid()) { return NULL; }
	ClearSelectedUnits(previousPlayer);
	if ((settings->mouseActionType == AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_VILLAGER_BUILD_MENU) &&
		(settings->gameModeSub == 1)) {
		_asm {
			MOV ECX, settings;
			PUSH 0;
			PUSH 0;
			MOV EAX, DS:[ECX];
			CALL DWORD PTR DS:[EAX+0x10]; // gameSettings.setGameMode(int, int)
		}
	}
	_asm {
		MOV ECX, settings;
		MOV EDX, DWORD PTR SS:[ECX];
		MOV EAX, playerId;
		PUSH EAX;
		CALL DWORD PTR DS:[EDX+0x14]; // Change controlled player
	}
	if (updateAIFlags) {
		AOE_METHODS::PLAYER::RestoreAllAIFlags();
	}
	return player;
}


// Return NULL if one of the objects is NULL/missing
AOE_STRUCTURES::STRUCT_RESEARCH_DEF *GetResearchDef(const AOE_STRUCTURES::STRUCT_PLAYER *player, short int researchId) {
	if (!player || !player->IsCheckSumValid() || (researchId < 0)) {
		return NULL;
	}
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *ri = player->ptrResearchesStruct;
	if (!ri || (researchId >= ri->researchCount)) {
		return false;
	}
	return ri->ptrResearchDefInfo->GetResearchDef(researchId);
}


// Calls AOE's code mainAI.findUnit(DAT_Id)
AOE_STRUCTURES::STRUCT_UNIT_BASE *AOE_MainAI_findUnit(AOE_STRUCTURES::STRUCT_AI *mainAI, long int DAT_ID) {
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit;
	_asm {
		// Search for a unit (given its def id) in MainAI list
		PUSH -1;
		PUSH -1;
		PUSH -1;
		PUSH -1;
		PUSH -1;
		PUSH -1;
		PUSH -1;
		PUSH -1;
		PUSH DAT_ID;
		PUSH -1;
		MOV EAX, 0x0040BAD0;
		MOV ECX, mainAI;
		CALL EAX; // search for a unit in MainAI list
		MOV unit, EAX; // Save the result in our variable. Can be NULL (not found)
	}
	return unit;
}


void GlobalSetNextManagedAIPlayer() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return; }
	unsigned long int addr = 0x5204D0;
	_asm {
		MOV ECX, global;
		PUSH 3;
		CALL addr;
	}
}


}
}


namespace AOE_STRUCTURES {
namespace PLAYER {
;

// Returns true if a position is fog-visible for a player (or a player with shared exploration).
// This is quite fast: directly accesses (optimized) memory, no underlying calls.
// Warning: posX/posY values are not controlled regarding map size !
bool IsFogVisibleForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int posX, long int posY) {
	if (!player || !player->IsCheckSumValid()) { return false; }
	STRUCT_MAP_VISIBILITY_MASK myMask = player->myFogVisibilityMask;
	return IsFogVisibleForMask(myMask, posX, posY);
}

// Returns true if a position has been explored by a player (or a player with shared exploration).
// This is quite fast: directly accesses (optimized) memory, no underlying calls.
// Warning: posX/posY values are not controlled regarding map size !
bool IsExploredForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int posX, long int posY) {
	if (!player || !player->IsCheckSumValid()) { return false; }
	STRUCT_MAP_VISIBILITY_MASK myMask = player->myExplorationVisibilityMask;
	return IsExploredForMask(myMask, posX, posY);
}


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


// Returns true if the specified research is enabled for given player.
// True if the research is waiting requirement, available, in progress, done.
// False if the research is disabled (or invalid ID).
bool IsResearchEnabledForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int researchId) {
	if (!player || !player->IsCheckSumValid()) { return false; }
	if (player->GetResearchDef(researchId) == NULL) { return false; }
	return GetResearchStatus(player, researchId) != RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED;
}


// Returns true if player resources are sufficient to pay the specified unit's cost (building or living unit)
// This only checks the 4 basic resources food/wood/stone/gold. For example, this does not control population headroom, etc.
bool CanUnitCostBePaid(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID) {
	if (!player || !player->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = player->GetUnitDefBase(DAT_ID);
	if (!unitDefBase || !unitDefBase->DerivesFromTrainable()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unitDefBase;
	assert(unitDef->IsCheckSumValidForAUnitClass());
	for (int i = 0; i < 3; i++) {
		if ((unitDef->costs[i].costPaid != 0) && (unitDef->costs[i].costType >= 0) &&
			(unitDef->costs[i].costType < RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT)) {
			if ((unitDef->costs[i].costAmount > 0) && (unitDef->costs[i].costAmount > player->GetResourceValue(unitDef->costs[i].costType))) {
				return false;
			}
		}
	}
	return true;
}


// Searches (at least) a unit with provided shortcut number for given player.
// The first matching unit is returned (arbitrary), even if there are several.
// Returns NULL if not found
AOE_STRUCTURES::STRUCT_UNIT_BASE *FindUnitWithShortcutNumberForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, char shortcutNumber) {
	assert(player != NULL);
	AOE_STRUCTURES::STRUCT_OBJECT_LIST *units = player->ptrCreatableUnitsListLink;
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
	AOE_STRUCTURES::STRUCT_OBJECT_LIST *units = player->ptrCreatableUnitsListLink;
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


// Clear player selection then select provided unit. Compatible with editor too.
// If centerScreen is true, player's screen will be centered to unit.
void SelectOneUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase, bool centerScreen) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	if ((settings->currentUIStatus != GAME_SETTINGS_UI_STATUS::GSUS_GAME_OVER_BUT_STILL_IN_GAME) &&
		(settings->currentUIStatus != GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) &&
		(settings->currentUIStatus != GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR)) {
		return;
	}
	if (!player || !player->IsCheckSumValid()) { return; }
	AOE_METHODS::PLAYER::ClearSelectedUnits(player);
	AOE_METHODS::PLAYER::SelectUnit(player, unitBase, true);
	if (centerScreen) {
		player->screenPositionX = unitBase->positionX;
		player->screenPositionY = unitBase->positionY;
		player->mapLocationPosX = (short int)unitBase->positionX;
		player->mapLocationPosY = (short int)unitBase->positionY;
	}
	if (!settings || !settings->IsCheckSumValid() || !settings->ptrGameUIStruct ||
		!settings->ptrGameUIStruct->IsCheckSumValid() || !settings->ptrGameUIStruct->gamePlayUIZone ||
		!settings->ptrGameUIStruct->gamePlayUIZone->IsCheckSumValid()) {
		return;
	}
	AOE_METHODS::UI_BASE::RefreshUIObject(settings->ptrGameUIStruct->gamePlayUIZone);
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
	// Could use ptrBuildingsListHeader ?
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
		STRUCT_UNIT_BASE *otherUnit = firstFound->unit;
		// Exchange position with "main" unit
		thisUnitElem->unit = otherUnit;
		otherUnit->ptrElemInPlayerUnitList = thisUnitElem;
		firstFound->unit = unit;
		unit->ptrElemInPlayerUnitList = firstFound;
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


// OBSOLETE: avoid using it
// Duplicates an existing unit definition (srcDAT_ID) into a new unit definition for given player.
// New unitDef will be available in player's list. Everything is managed so that the game will automatically free everything correctly.
// You can send "" as name (in that case original unit's name will be used). Max length = 0x2F.
// Returns the new DAT_ID if successful, -1 otherwise.
// You can change any value for the new unitDef (but avoid messing with pointers?), but make sure you do it BEFORE you create units of this kind !
// It is not recommended to call this too much ! It would add a lot of unit definition and would impact seriously game performance.
// The maximum number of DAT_ID we allow here is 0x7FFF (to take no risk regarding signed short int values)
// The method supports all unit (def) types
// Warning: there might be issues when such units are converted (bug in destructor in AOE, a unitDef for a given DATID should exist for all civs ?)
// Maybe it's better not to use it for living/buildings units (because of conversion)
// Note: see also AOE unitDef constructors that copy an existing one, e.g. 4ED1B0 for living (type 70).
short int DuplicateUnitDefinitionForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int srcDAT_ID, char *name) {
	if (!player) { return -1; }

	short int newDAT_ID = player->structDefUnitArraySize;
	if (newDAT_ID >= 0x7FFF) {
		traceMessageHandler.WriteMessage("Error: tried to add too many unit definitions.");
		return -1;
	}
	short int nbDef = player->structDefUnitArraySize + 1; // new number of unit definitions
	long int newSizeInBytes = nbDef * 4;
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE **oldArray = player->ptrStructDefUnitTable;
	// Run some checks
	assert(oldArray != NULL);
	if (oldArray == NULL) { return -1; }
	assert(player->structDefUnitArraySize > srcDAT_ID);
	assert(0 <= srcDAT_ID);
	if ((srcDAT_ID < 0) || (srcDAT_ID >= player->structDefUnitArraySize)) {
		std::string msg = std::string("ERROR: tried to copy a unit definition that does not exist, ID=") + std::to_string(srcDAT_ID);
		traceMessageHandler.WriteMessage(msg);
		return -1;
	}

	// Create new array
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE **newArray = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE **)AOEAlloc(newSizeInBytes);
	if (newArray == NULL) { return -1; } // nothing allocated: return an error

	// Copy old array into new (for all existing unitDefs => copy pointers)
	memcpy(newArray, oldArray, newSizeInBytes);
	AOEFree(oldArray); // old array is freed, we replace it by new (larger) one
	player->ptrStructDefUnitTable = newArray;
	player->structDefUnitArraySize = nbDef;
	player->ptrStructDefUnitTable[newDAT_ID] = NULL; // until we set a valid pointer.

	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *srcDef_base = player->ptrStructDefUnitTable[srcDAT_ID];
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *srcDef_type50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)srcDef_base;
	assert(srcDef_base != NULL);
	if (srcDef_base == NULL) { return -1; }
	if ((srcDef_base->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) || (srcDef_base->unitType == GLOBAL_UNIT_TYPES::GUT_TRAINABLE)) {
		traceMessageHandler.WriteMessage("WARNING: adding unitdef for living/buildings is disabled due to conversion bug.");
		// To solve the limitation, a solution could be to add it for all players !
		return -1;
	}

	long int objectSize = 0;
	bool hasAttacksAndArmors = srcDef_base->DerivesFromAttackable();
	bool hasCommandsHeader = srcDef_base->DerivesFromCommandable();
	switch (srcDef_base->unitType) {
	case GUT_EYE_CANDY:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BASE);
		break;
	case GUT_FLAGS:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_FLAG);
		break;
	case GUT_DOPPLEGANGER:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_DOPPLEGANGER);
		break;
	case GUT_MOVABLE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE);
		break;
	case GUT_COMMANDABLE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE);
		break;
	case GUT_ATTACKABLE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE);
		break;
	case GUT_PROJECTILE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE);
		break;
	case GUT_TRAINABLE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE);
		break;
	case GUT_BUILDING:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING);
		break;
	case GUT_TREE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_TREE);
		break;
	default:
		// Not supported
		assert(0 && "Trying to create a non-supported unit type.");
		traceMessageHandler.WriteMessage("ERROR: Tried to create a non-supported unit type.");
		return -1;
	}
	if (objectSize <= 0) { return -1; }

	// Create new defUnit in the free room we just created
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *newUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *) AOEAlloc(objectSize);
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *newUnitDefType50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)newUnitDef;
	if (!newUnitDef) { return -1; } // nothing allocated: return an error

	player->ptrStructDefUnitTable[newDAT_ID] = newUnitDef;

	memcpy(newUnitDef, srcDef_base, objectSize);

	// Very important: update new unitDef's DATIDs or some features will not work
	newUnitDef->DAT_ID1 = newDAT_ID;
	newUnitDef->DAT_ID2 = newDAT_ID;

	// Manage damage graphics array (allocated for each unitDef)
	// It belongs to base class => do this for all unit types, even if it seems this array is only used for buildings
	if (newUnitDef->damageGraphicCount > 0) {
		newUnitDef->damageGraphicsArray = (STRUCT_DAMAGE_GRAPHIC *)AOEAlloc(sizeof(STRUCT_DAMAGE_GRAPHIC) * newUnitDef->damageGraphicCount);
		memcpy(newUnitDef->damageGraphicsArray, srcDef_base->damageGraphicsArray, sizeof(AOE_STRUCTURES::STRUCT_DAMAGE_GRAPHIC) * newUnitDef->damageGraphicCount);
	} else {
		assert(newUnitDef->damageGraphicsArray == NULL);
		newUnitDef->damageGraphicsArray = NULL; // Should already be NULL in this case
		newUnitDef->damageGraphicCount = 0;// Should not be <0
	}

	if (hasAttacksAndArmors && srcDef_type50->DerivesFromAttackable()) {
		// Attacks and armors are allocated for each unitDef. We need to duplicate them (or ROR would free them twice)
		newUnitDefType50->ptrArmorsList = NULL; // Default value, will be overwritten if necessary
		newUnitDefType50->ptrAttacksList = NULL; // Default value, will be overwritten if necessary
		if (newUnitDefType50->armorsCount > 0) {
			newUnitDefType50->ptrArmorsList = (AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK *)AOEAlloc(sizeof(AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK) * newUnitDefType50->armorsCount);
			memcpy(newUnitDefType50->ptrArmorsList, srcDef_type50->ptrArmorsList, sizeof(AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK) * newUnitDefType50->armorsCount);
		}
		if (newUnitDefType50->attacksCount > 0) {
			newUnitDefType50->ptrAttacksList = (AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK *)AOEAlloc(sizeof(AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK) * newUnitDefType50->attacksCount);
			memcpy(newUnitDefType50->ptrAttacksList, srcDef_type50->ptrAttacksList, sizeof(AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK) * newUnitDefType50->attacksCount);
		}
	}

	// Graphics structures are COMMON (not freed at unitDef level): we can keep them, they won't be freed twice

	if (hasCommandsHeader && srcDef_type50->DerivesFromAttackable()) {
		// Commands: we need to duplicate them (or ROR would free them twice)
		assert(srcDef_type50->ptrUnitCommandHeader != NULL);
		if (srcDef_type50->ptrUnitCommandHeader != NULL) {
			AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF_HEADER *newCmdHeader = (AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF_HEADER *) AOEAlloc(sizeof(AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF_HEADER));
			newUnitDefType50->ptrUnitCommandHeader = newCmdHeader;
			newCmdHeader->checksum = srcDef_type50->ptrUnitCommandHeader->checksum;
			newCmdHeader->commandCount = srcDef_type50->ptrUnitCommandHeader->commandCount;
			if (newCmdHeader->commandCount > 0) {
				newCmdHeader->ptrCommandArray = (AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF**) AOEAlloc(newCmdHeader->commandCount * 4);
			} else {
				newCmdHeader->ptrCommandArray = NULL;
			}

			for (long int i = 0; i < newCmdHeader->commandCount; i++) {
				newCmdHeader->ptrCommandArray[i] = (AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF*) AOEAlloc(sizeof(AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF)); // Alloc
				memcpy(newCmdHeader->ptrCommandArray[i], srcDef_type50->ptrUnitCommandHeader->ptrCommandArray[i], sizeof(AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF)); // Copy from source
			}
		} else {
			newUnitDefType50->ptrUnitCommandHeader = NULL; // This should NOT happen
		}
	}

	// Note: If we keep ptrLanguageDLLName, source unit name will appear in game. If we set to NULL, it will use ptrUnitName instead.
	if (!name || (*name == 0)) {
		return newDAT_ID;
	}
	newUnitDef->languageDLLID_Name = 0;
	newUnitDef->languageDLLID_Creation = 0;
	char *newName = (char *)AOEAlloc(0x30);
	strcpy_s(newName, 0x30, name);
	newUnitDef->ptrUnitName = newName; // issues with the name ?

	return newDAT_ID;
}


// Set "shared exploration" flag for a given player to true or false.
// Old version: Not compatible with MP games (to verify)
void SetPlayerSharedExploration_hard(long int playerId, bool enable) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) {
		return;
	}
	if ((playerId < 0) || (playerId > global->playerTotalCount)) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStruct(playerId);
	if (!player || !player->IsCheckSumValid()) { return; }
	long int value = enable ? 1 : 0;
	player->sharedExploration = enable;
	player->SetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_SHARED_EXPLORATION, enable);
	_asm {
		// Cf 0x4F6CAF
		MOV ECX, global;
		MOV EAX, 0x520C60;
		CALL EAX;
	}
}


// Set "shared exploration" flag for a given player to true or false. This version should be compatible with MP games (uses ROR command system)
void SetPlayerSharedExploration_safe(long int playerId) {
	GAME_COMMANDS::CreateCmd_SetSharedExploration((short int)playerId);
}


// Returns the most damaged unit, NULL if not found
// unitDefIdFilter = -1 in most cases (joker). Otherwise, THE only unit def to take into account
STRUCT_UNIT_BASE *GetPlayerMostDamagedUnit(STRUCT_AI *mainAI, GLOBAL_UNIT_AI_TYPES aiType, long int unitDefIdFilter) {
	if (!mainAI || !mainAI->IsCheckSumValid()) { return NULL; }
	unsigned long int addr = 0x40BE70;
	STRUCT_UNIT_BASE *res = NULL;
	_asm {
		MOV ECX, mainAI;
		PUSH unitDefIdFilter;
		PUSH aiType;
		CALL addr;
		MOV res, EAX;
	}
	return res;
}


// Returns the diplomacy stance regarding "playerOther", from playerMe's point of view.
// ***WARNING*** this returns "neutral" for "self". You might prefer using GetDiplomacyValueForPlayer.
// Returns CST_PDS_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES GetDiplomacyStanceForPlayer(STRUCT_PLAYER *playerMe, long int otherPlayerId) {
	if (!playerMe) {
		return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_UNKNOWN;
	}
	STRUCT_GAME_GLOBAL *global = playerMe->ptrGlobalStruct;
	if (!global) { return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_UNKNOWN; }
	if ((otherPlayerId < 0) || (otherPlayerId >= global->playerTotalCount)) {
		return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_UNKNOWN;
	}
	return playerMe->ptrDiplomacyStances[otherPlayerId];
}

// Returns the diplomacy stance regarding "playerOther", from playerMe's point of view.
// ***WARNING*** this returns "neutral" for "self". You might prefer using GetDiplomacyValueForPlayer.
// Returns CST_PDS_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES GetDiplomacyStanceForPlayer(STRUCT_PLAYER *playerMe, STRUCT_PLAYER *playerOther) {
	if (!playerOther) {
		return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_UNKNOWN;
	}
	return GetDiplomacyStanceForPlayer(playerMe, playerOther->playerId);
}



// Returns the diplomacy stance regarding "otherPlayerId", from playerMe's point of view.
// Returns CST_PDV_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES GetDiplomacyValueForPlayer(STRUCT_PLAYER *playerMe, long int otherPlayerId) {
	if (!playerMe) {
		return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES::CST_PDV_UNKNOWN;
	}
	if ((otherPlayerId < 0) || (otherPlayerId > 8)) { // diplomacyVSPlayers size is always 9.
		return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES::CST_PDV_UNKNOWN;
	}
	return playerMe->diplomacyVSPlayers[otherPlayerId];
}

// Returns the diplomacy stance regarding "playerOther", from playerMe's point of view.
// Returns CST_PDV_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES GetDiplomacyValueForPlayer(STRUCT_PLAYER *playerMe, STRUCT_PLAYER *playerOther) {
	if (!playerOther) {
		return AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES::CST_PDV_UNKNOWN;
	}
	return GetDiplomacyValueForPlayer(playerMe, playerOther->playerId);
}


}
}
