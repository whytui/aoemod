#include "../include/crCommon.h"

static char _intDistanceValues[CST_INT_DISTANCE_VALUES_MAX_COORDINATE + 1][CST_INT_DISTANCE_VALUES_MAX_COORDINATE + 1] = { '\0' };


CustomRORInfo::CustomRORInfo() {
	this->triggerSet = NULL;
	this->hasFixForBuildingStratElemUnitId = false;
	this->hasCustomSelectedUnitsMemory = false;
	this->hasManageAIFeatureON = false;
	this->hasRemovePlayerInitialAgeInScenarioInit = false;
	this->ResetVariables();

	for (int x = 0; x <= CST_INT_DISTANCE_VALUES_MAX_COORDINATE; x++) {
		for (int y = 0; y <= CST_INT_DISTANCE_VALUES_MAX_COORDINATE; y++) {
			int v = (int)sqrtf((float)(x*x + y*y)); // Intermediate values can be higher than 127...
			assert(v < 127); // make sure there is no overflow !
			_intDistanceValues[x][y] = v;
		}
	}	
}

// Fast-computes the integer distance for X and Y delta values (sqrt(X^2 + Y^2) as an integer).
// Returns -1 if invalid (diffX and diffY values are capped at CST_INT_DISTANCE_VALUES_MAX_COORDINATE)
char CustomRORInfo::GetIntDistance(char diffX, char diffY) {
	if (diffX < 0) {
		diffX = -diffX;
	}
	if (diffY < 0) {
		diffY = -diffY;
	}
	if ((diffX < 0) || (diffX > CST_INT_DISTANCE_VALUES_MAX_COORDINATE) || (diffY < 0) || (diffY > CST_INT_DISTANCE_VALUES_MAX_COORDINATE)) {
		return -1;
	}
	return _intDistanceValues[diffX][diffY];
}

CustomRORInfo::~CustomRORInfo() {
	if (this->triggerSet) {
		this->triggerSet->Reset();
		delete this->triggerSet;
		this->triggerSet = NULL;
	}
	this->FreeGarbagePopupComponents();
}


// Reset variables that are used during game.
// Only reset internal variables that are game-dependent !
// This is called on each game start/load
void CustomRORInfo::ResetVariables() {
	if (this->triggerSet) {
		this->triggerSet->Reset();
		delete this->triggerSet;
	}
	this->triggerSet = NULL;
	this->triggersLastCheckTime_s = 0;
	this->lastCustomRORTimeExecution_gameTime_s = 0;
	this->customYesNoDialogVar = 0;
	this->customGameMenuOptionsBtnVar = NULL;
	this->customGamePopupButtonVar = NULL;
	this->customGamePopupCancelBtnVar = NULL;
	this->customGamePopupVar = 0;
	this->gameTimerSlowDownCounter = 0;
	this->CollectedTimerIntervalsIndex = 0;
	this->LastGameTimerAutoFix_second = 0;
	for (int i = 0; i < 9; i++) {
		this->activeConversionAttemptsCount[i] = 0;
		this->activeConversionSuccessfulAttemptsCount[i] = 0;
		this->passiveConversionAttemptsCount[i] = 0;
		this->passiveConversionSuccessfulAttemptsCount[i] = 0;
	}
	for (int i = 0; i < CST_TIMER_STATS_ARRAY_SIZE; i++) { this->CollectedTimerIntervals_ms[i] = -1; }
	this->LastDislikeValuesComputationTime_second = 0;

	for (int civId = 0; civId < CST_MAX_TOTAL_CIV_COUNT; civId++) {
		for (int playerNameIndex = 0; playerNameIndex < 9; playerNameIndex++) {
			nameIndexIsUsedByPlayer[civId][playerNameIndex] = -1;
		}
	}
	this->myGameObjects.ResetObjects();
	// Reset list of unit definitions/researches to disable at game start (for scenarios)
	for (int i = 0; i < 9; i++) {
		this->unitDefToDisable[i].clear();
		this->researchesToDisable[i].clear();
	}
}


// Private method: fill a vector from a list of numeric values (short int)
// Separator is , or ;
// Does not support spaces.
void CustomRORInfo::FillIDVectorFromString(vector<short int> &v, long int playerId, const char *text) {
	if (!text || (*text == 0)) { return; }
	if ((playerId < 0) || (playerId > 8)) { return; }
	char currentValue[10];
	int currentLength = 0;
	v.clear();
	while (*text != 0) {
		if ((currentLength + 1) >= sizeof(currentValue)) { // current (string) value is too long: invalid
			currentLength = 1;
			currentValue[0] = '_'; // make sure it will be invalid when read
		} else {
			currentValue[currentLength] = *text;
			currentLength++;
		}
		text++; // next char
		if ((*text == ',') || (*text == ';') || (*text == 0)) {
			currentValue[currentLength] = 0; // end of string
			short int currentDATID = StrToInt(currentValue, -1);
			if (currentDATID >= 0) {
				v.push_back(currentDATID);
			}
			if (*text != 0) {
				text++; // skip separator
			}
			currentLength = 0; // reset position in work buffer
		}
	}
}

// Resets and fills the list of unitDefID to disable for a player from a comma-separated list.
void CustomRORInfo::FillUnitDefToDisableFromString(long int playerId, const char *text) {
	if (!text || (*text == 0)) { return; }
	if ((playerId < 0) || (playerId > 8)) { return; }
	this->FillIDVectorFromString(this->unitDefToDisable[playerId], playerId, text);
}
// Resets and fills the list of researchID to disable for a player from a comma-separated list.
void CustomRORInfo::FillResearchesToDisableFromString(long int playerId, const char *text) {
	if (!text || (*text == 0)) { return; }
	if ((playerId < 0) || (playerId > 8)) { return; }
	this->FillIDVectorFromString(this->researchesToDisable[playerId], playerId, text);
}


// Function to calculate conversion resistance for a giver unit from a given civ.
// This replaces game's algorithm.
float CustomRORInfo::GetConversionResistance(char civId, short int unitClass) {
	// Standard resistances (original game)
	// Macedonian
	if (civId == CST_CIVID_MACEDONIAN) {
		return this->configInfo.conversionResistance_Macedonian;
	}
	// Special unit classes
	if (
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupFishingBoat) || // TribeAIGroupFishingBoat
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTradeBoat) || // trade boat
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTransportBoat) || // TribeAIGroupTransportBoat
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupWarBoat)) // TribeAIGroupWarBoat including fire galley, juggernaught
	{
		return this->configInfo.conversionResistance_Boats;
	}
	if (unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) { // TribeAIGroupPriest
		return this->configInfo.conversionResistance_Priests;
	}
	if (
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot) || // Chariot
		(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher) // Chariot archer
		) {
		return this->configInfo.conversionResistance_Chariots;
	}

	// Custom resistances (only for single player games)
	if ((!IsMultiplayer()) && (unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider)) { // War elephant, armored elephant, including Hannibal
		return (civId == CST_CIVID_PERSIAN) ? this->configInfo.conversionResistance_WarElephants_Persian : this->configInfo.conversionResistance_WarElephants;
	}
	return 1; // Default value
}


// Applies an "auto-attack policy" on all player's selected units (only for owned units !)
// flagsToApply is used to determine which flags have to be updated using values from autoAttackPolicyValues.
// If flagsToApply.xxx is true, then update unit's auto_attack_policy.xxx to "autoAttackPolicyValues.xxx" value.
void CustomRORInfo::ApplyAutoAttackPolicyToPlayerSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player,
	const AutoAttackPolicy &autoAttackPolicyValues, const AutoAttackPolicy &flagsToApply) {
	if (!flagsToApply.attackMilitary &&
		!flagsToApply.attackNonTowerBuildings &&
		!flagsToApply.attackTowers &&
		!flagsToApply.attackVillagers &&
		!flagsToApply.attackWalls) { return; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = this->GetRelevantSelectedUnitsBasePointer(player);
	assert(selectedUnits != NULL);
	for (int i = 0; i < player->selectedUnitCount; i++) {
		if (selectedUnits[i] && selectedUnits[i]->IsCheckSumValidForAUnitClass() &&
			(selectedUnits[i]->ptrStructPlayer == player)) { // Make sure selected unit is mine !
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *currentUnitDefBase = selectedUnits[i]->GetUnitDefinition();
			if (IsTower(currentUnitDefBase->DAT_ID1) || IsNonTowerMilitaryUnit(currentUnitDefBase->unitAIType)) {
				UnitCustomInfo *currentUnitInfo = this->myGameObjects.FindOrAddUnitCustomInfo(selectedUnits[i]->unitInstanceId);
				currentUnitInfo->autoAttackPolicyIsSet = true;
				if (flagsToApply.attackMilitary) {
					currentUnitInfo->autoAttackPolicy.attackMilitary = autoAttackPolicyValues.attackMilitary;
				}
				if (flagsToApply.attackNonTowerBuildings) {
					currentUnitInfo->autoAttackPolicy.attackNonTowerBuildings = autoAttackPolicyValues.attackNonTowerBuildings;
				}
				if (flagsToApply.attackTowers) {
					currentUnitInfo->autoAttackPolicy.attackTowers = autoAttackPolicyValues.attackTowers;
				}
				if (flagsToApply.attackVillagers) {
					currentUnitInfo->autoAttackPolicy.attackVillagers = autoAttackPolicyValues.attackVillagers;
				}
				if (flagsToApply.attackWalls) {
					currentUnitInfo->autoAttackPolicy.attackWalls = autoAttackPolicyValues.attackWalls;
				}
			}
		}
	}
}


// Returns true if a CUSTOM game popup is opened (only for popups, not dialogs !)
bool CustomRORInfo::HasOpenedCustomGamePopup() {
	return (this->customGamePopupButtonVar != NULL);
}

// Returns true if a custom dialog is opened (only for dialogs, not popups !)
bool CustomRORInfo::HasOpenedCustomDialog() {
	return (this->customYesNoDialogVar != NULL);
}



// Opens a custom popup window in game screen/editor. The created popup window only contains a "OK" button.
// You have to add UI elements afterwards (use GetCustomGamePopup() to get parent object=popup).
// Return popup UI object if OK, NULL if failed
// Fails if another game popup (including options) is already open. Fails if dimensions are too small.
// Pauses the game if running (only if a popup is successfully opened)
// Technically, the created (AOE) popup object is based on game options popup.
AOE_STRUCTURES::STRUCT_ANY_UI *CustomRORInfo::OpenCustomGamePopup(long int hSize, long int vSize, bool hasCancelBtn) {
	if (this->HasOpenedCustomGamePopup()) { return false; }
	if ((hSize < 0xB0) || (vSize < 30)) { return false; }
	if (!ROR_gameSettings) { return false; }

	if ((GetGameSettingsPtr()->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) &&
		(GetGameSettingsPtr()->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING)) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_ANY_UI *currentScreen = GetCurrentUIStruct();
	if (!currentScreen) { return false; }
	// Make sure provided dimensions fit in screen
	if (currentScreen->sizeX < hSize) { hSize = currentScreen->sizeX - 10; }
	if (currentScreen->sizeY < vSize) { vSize = currentScreen->sizeY - 10; }

	SetGamePause(true);
	this->customGamePopupVar = AOE_CreateGameScreenPopup(currentScreen, hSize, vSize);

	// OK button. It is this->customGamePopupButtonVar that identifies the popup (+ the fact it is open)
	long int btnhPos = (hSize - 0xAC) / 2;
	long int btnvPos = vSize - 0x26;
	long int btnHSize = 0xAC;
	if (hasCancelBtn) {
		btnhPos = (hSize - 0xAC - 20) / 2;
		btnHSize = 0x50;
	}

	AOE_AddButton(this->customGamePopupVar, &this->customGamePopupButtonVar, LANG_ID_OK, btnhPos, btnvPos, btnHSize, 0x1E);
	if (hasCancelBtn) {
		AOE_AddButton(this->customGamePopupVar, &this->customGamePopupCancelBtnVar, LANG_ID_CANCEL, btnhPos + btnHSize + 0x20, btnvPos, btnHSize, 0x1E);
	}
	assert(this->objectsToFree.empty()); // there shouldn't be remaining components from a previous popup.

	return this->customGamePopupVar;
}


// Use it to list all UI components (labels, buttons...) that are created(added) to popup content, so they are automatically freed when popup is closed.
// The method is protected against duplicates, you can safely call it more than once.
// Returns true if obj has actually been added to list.
bool CustomRORInfo::AddObjectInPopupContentList(AOE_STRUCTURES::STRUCT_ANY_UI *obj) {
	if (obj == NULL) {
		return false;
	}
	for (std::vector<AOE_STRUCTURES::STRUCT_ANY_UI *>::iterator it = this->objectsToFree.begin(); it != this->objectsToFree.end(); it++) {
		AOE_STRUCTURES::STRUCT_ANY_UI *o = *it;
		if (o == obj) {
			return false; // duplicate: do not insert again
		}
	}
	this->objectsToFree.push_back(obj);
	return true;
}


// Frees (destroys using AOE destructor) all popup's components
void CustomRORInfo::FreePopupAddedObjects() {
	// at this point, our "garbage" collector contains objects from a previous popup
	// which means we are now certain all of these components are NOT related to the "in progress" event (onclick, etc), if any.
	// So we can free them. This way, we don't accumulate old components, but only keep those from "last" popup.
	this->FreeGarbagePopupComponents();

	for (std::vector<AOE_STRUCTURES::STRUCT_ANY_UI *>::iterator it = this->objectsToFree.begin(); it != this->objectsToFree.end(); it++) {
		AOE_STRUCTURES::STRUCT_ANY_UI *curObj = *it;
		curObj->ptrParentObject = NULL; // do not try to free other related objects, just this one ! Remove link. Otherwise some objects would be destroyed more than once => crash
		assert(isAValidRORChecksum(curObj->checksum)); // if failed, this means the object had already been freed, and memory been re-used for something else
		if (static_cast<AOE_STRUCTURES::STRUCT_UI_BUTTON*>(curObj) != NULL) {
			// buttons might be event's sender, do not free them now... Our garbage system will manage them correctly.
			this->garbageComponentsToFree.push_back(curObj);
		} else {
			CallAOEDestructor((unsigned long **)&curObj);
		}
	}
	this->objectsToFree.clear();
}

// Free remaining UI components that could not be freed yet
// The reason we don't free those immediatly is because they might be used in current event (button onclick, etc).
// Warning: this must NOT be called at any moment, make sure none of the concerned object is currently being used (specially for onclick events...)
void CustomRORInfo::FreeGarbagePopupComponents() {
	for (std::vector<AOE_STRUCTURES::STRUCT_ANY_UI *>::iterator it = this->garbageComponentsToFree.begin(); it != this->garbageComponentsToFree.end(); it++) {
		AOE_STRUCTURES::STRUCT_ANY_UI *curObj = *it;
		curObj->ptrParentObject = NULL; // do not try to free other related objects, just this one ! Remove link. Otherwise some objects would be destroyed more than once => crash
		assert(isAValidRORChecksum(curObj->checksum)); // if failed, this means the object had already been freed, and memory been re-used for something else
		if (isAValidRORChecksum(curObj->checksum)) {
			CallAOEDestructor((unsigned long **)&curObj);
		} else {
			traceMessageHandler.WriteMessageNoNotification("FreeGarbagePopupComponents: Invalid checksum !");
		}
	}
	this->garbageComponentsToFree.clear();
}


// Use this to force values for "current custom popup". PLEASE AVOID using it !
// Returns true if successful. Fails if a popup is already open.
bool CustomRORInfo::ForceSetCurrentGamePopup(AOE_STRUCTURES::STRUCT_ANY_UI *customGamePopup, AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK, AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCancel) {
	if (this->HasOpenedCustomGamePopup()) { return false; }
	this->customGamePopupVar = customGamePopup;
	this->customGamePopupButtonVar = btnOK;
	this->customGamePopupCancelBtnVar = btnCancel;
	return true;
}


// To be called when game menu is closed to free custom button
void CustomRORInfo::ForceClearCustomMenuObjects() {
	this->FreePopupAddedObjects();
}


// Closes currently opened custom popup window in game screen.
void CustomRORInfo::CloseCustomGamePopup() {
	AOE_STRUCTURES::STRUCT_ANY_UI *UIObjButton = this->customGamePopupButtonVar;
	if (UIObjButton == NULL) { return; }
	AOE_STRUCTURES::STRUCT_ANY_UI *popupUIObj = UIObjButton->ptrParentObject;

	if (this->customGamePopupCancelBtnVar) { // If a cancel button had been created, free it.
		CallAOEDestructor((unsigned long **)&this->customGamePopupCancelBtnVar); // also sets var to NULL
	}

	CallAOEDestructor((unsigned long **)&this->customGamePopupButtonVar); // also sets var to NULL
	UIObjButton = NULL; // DO NOT use UIObj anymore !
	// Free user-created popup content (underlying UI components)
	this->FreePopupAddedObjects();

	assert(popupUIObj != NULL);
	if (popupUIObj != NULL) {
		AOE_CloseScreenFullTreatment(popupUIObj);
	}
	SetGamePause(false);
}


// Return true if provided memory address is our custom game popup (excluding custom options)
bool CustomRORInfo::IsCustomGamePopupOKButton(unsigned long int UIObjectAddress) {
	return UIObjectAddress == (unsigned long)this->customGamePopupButtonVar;
}

// Return true if provided memory address is our custom game popup Cancel button (excluding custom options)
bool CustomRORInfo::IsCustomGamePopupCancelButton(unsigned long int UIObjectAddress) {
	return UIObjectAddress == (unsigned long)this->customGamePopupCancelBtnVar;
}


// Returns custom popup window in game screen (excluding customROR options popup).
// Returns NULL if this popup is not open.
AOE_STRUCTURES::STRUCT_ANY_UI *CustomRORInfo::GetCustomGamePopup() {
	return this->customGamePopupVar;
}


// Get main (first) selected unit, or NULL if none is selected.
// Works in-game and in editor.
AOE_STRUCTURES::STRUCT_UNIT_BASE *CustomRORInfo::GetMainSelectedUnit(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	assert(player != NULL);
	if (!player) { return NULL; }
	AOE_STRUCTURES::STRUCT_UNIT **selectedUnits = this->GetRelevantSelectedUnitsPointer(player);
	if (!selectedUnits) {
		return NULL;
	}
	return (AOE_STRUCTURES::STRUCT_UNIT_BASE *)*selectedUnits; // not sure it always corresponds to selected unit in bottom left panel ? But this works in editor too
}


// Get relevant "selected units" array pointer according to game EXE status (using custom memory or not ?)
// Please use this instead of playerStruct->selectedStructUnitTable
AOE_STRUCTURES::STRUCT_UNIT **CustomRORInfo::GetRelevantSelectedUnitsPointer(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	assert(player != NULL);
	if (!player) { return NULL; }
	AOE_STRUCTURES::STRUCT_UNIT **selectedUnits;
	if (this->hasCustomSelectedUnitsMemory) {
		selectedUnits = player->custom_selectedUnits;
	} else {
		selectedUnits = player->selectedStructUnitTable;
	}
	assert(selectedUnits != NULL);
	return selectedUnits;
}
AOE_STRUCTURES::STRUCT_UNIT_BASE **CustomRORInfo::GetRelevantSelectedUnitsBasePointer(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	return (AOE_STRUCTURES::STRUCT_UNIT_BASE **)this->GetRelevantSelectedUnitsPointer(player);
}








// Other methods



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


// Get strategy element type for a unit
// Returns AIUCNone if result could not be determined.
AOE_CONST_FUNC::TAIUnitClass GetUnitStrategyElemClass(AOE_STRUCTURES::STRUCT_DEF_UNIT *unitDef) {
	TAIUnitClass unitStrategyClass = AOE_CONST_FUNC::TAIUnitClass::AIUCNone;
	if (!unitDef) { return unitStrategyClass; }
	if (unitDef->unitType == GUT_BUILDING) { unitStrategyClass = AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding; }
	if (unitDef->unitType == GUT_LIVING_UNIT) { unitStrategyClass = AOE_CONST_FUNC::TAIUnitClass::AIUCLivingUnit; }
	return unitStrategyClass;
}


// Calculate distance
float GetDistance(float x1, float y1, float x2, float y2) {
	float dx = x1 - x2;
	float dy = y1 - y2;
	dx = dx * dx;
	dy = dy * dy;
	return sqrt(dx + dy);
}


short int GetDAT_ID2(AOE_STRUCTURES::STRUCT_DEF_UNIT **defUnitTable, short int DAT_ID1) {
	return defUnitTable[DAT_ID1]->DAT_ID2;
}


// Returns a weight for a military unit. Scale is totally subjective ! Just a tool for algorithms...
// Super units have a high weight.
// Weak unit have a low weight
int GetUnitWeight(short int DAT_ID) {
	switch (DAT_ID) {
		// Super units
	case CST_UNITID_ARMORED_ELEPHANT:
	case CST_UNITID_CATAPHRACT:
	case CST_UNITID_CENTURION:
	case CST_UNITID_HEAVY_CATAPULT:
	case CST_UNITID_HEAVY_CHARIOT:
	case CST_UNITID_HEAVY_HORSE_ARCHER:
	case CST_UNITID_HELEPOLIS:
	case CST_UNITID_LEGION:
		return 1000;
		// Naval best units
	case CST_UNITID_JUGGERNAUGHT:
	case CST_UNITID_TRIREME:
		return 900;
		// Intermediate "almost-super" units
	case CST_UNITID_PHALANX:
	case CST_UNITID_CATAPULT:
	case CST_UNITID_HEAVY_CAVALRY:
		return 600;
	case CST_UNITID_PRIEST:
		return 600; // ? Depends on researches !
	case CST_UNITID_ELEPHANT_ARCHER:
		return 400;

	case CST_UNITID_HORSE_ARCHER:
		return 300;
	case CST_UNITID_LONG_SWORDSMAN:
	case CST_UNITID_BALLISTA:
	case CST_UNITID_HOPLITE:
	case CST_UNITID_STONE_THROWER:
	case CST_UNITID_WAR_ELEPHANT:
	case CST_UNITID_WAR_GALLEY:
	case CST_UNITID_CATAPULT_TRIREME:
		return 200;
	case CST_UNITID_CAVALRY:
	case CST_UNITID_CHARIOT:
		return 100;
		// Units under 100 do NOT have upgrades to super units except CST_UNITID_SHORT_SWORDSMAN and CST_UNITID_SCOUT_SHIP
		// (this is nothing but a choice from method writer ;)
	case CST_UNITID_COMPOSITE_BOWMAN:
		return 90;
	case CST_UNITID_CAMEL:
		return 50;

		// Weak units...
	case CST_UNITID_CHARIOT_ARCHER:
	case CST_UNITID_SCOUT_SHIP:
		return 25;
	case CST_UNITID_IMPROVED_BOWMAN:
	case CST_UNITID_BROAD_SWORDSMAN:
		return 10;
	case CST_UNITID_SHORT_SWORDSMAN:
		return 5;
	case CST_UNITID_SLINGER:
	case CST_UNITID_SCOUT:
	case CST_UNITID_BOWMAN:
		return 2;
	case CST_UNITID_CLUBMAN:
		return 1;
	default:
		return 0;
		break;
	}
}


// Pause/unpause the game
void SetGamePause(bool pauseOn) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (settings == NULL) { return; }
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) { return; } // Call should be robust enough, but we still check this
	long int argPause = pauseOn ? 1 : 0;
	_asm {
		PUSH 0 // arg2 - what is this ?
		PUSH argPause
		MOV EAX, 0x0419A60
		MOV ECX, settings
		CALL EAX
	}
}


// DO NOT call this if current UI is not "in-game" screen
void CallWriteText(const char *txt) {
	assert(GetGameSettingsPtr() != NULL);
	assert(GetGameSettingsPtr()->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING);
	long int addr = (long int)txt;
	long int writeFct = 0x480760; // DisplayChatTextOnNextLine
	_asm {
		push addr
		mov ECX, ds:[0x580E38]
		mov ECX, ds:[ECX + 0xC24]
		call writeFct
	}
}


// This will display a text in the game screen (like yellow/orange error messages)
// Note: such a message disappears when the user clicks somewhere.
void CallWriteCenteredText(const char *txt, long int numLine, long int color, long int background) {
	long int addr = (long int)txt;
	long int writeFct = 0x0482260;
	_asm {
		PUSH background
		PUSH color
		PUSH addr
		PUSH numLine
		mov ECX, ds:[0x580E38]
		mov ECX, ds:[ECX + 0xC24]
		call writeFct
	}
}


// If resourceTable has enough resources, returns true and deduces the cost from resourceTable.
// If not, returns false and does not modify any value.
bool ApplyCostIfPossible(float costTable[], float resourceTable[]) {
	if ((costTable[CST_RES_ORDER_FOOD] > 0 && (costTable[CST_RES_ORDER_FOOD] > resourceTable[CST_RES_ORDER_FOOD])) ||
		(costTable[CST_RES_ORDER_WOOD] > 0 && (costTable[CST_RES_ORDER_WOOD] > resourceTable[CST_RES_ORDER_WOOD])) ||
		(costTable[CST_RES_ORDER_STONE] > 0 && (costTable[CST_RES_ORDER_STONE] > resourceTable[CST_RES_ORDER_STONE])) ||
		(costTable[CST_RES_ORDER_GOLD] > 0 && (costTable[CST_RES_ORDER_GOLD] > resourceTable[CST_RES_ORDER_GOLD]))
		) {
		return false;
	}
	resourceTable[CST_RES_ORDER_FOOD] -= costTable[CST_RES_ORDER_FOOD];
	resourceTable[CST_RES_ORDER_WOOD] -= costTable[CST_RES_ORDER_WOOD];
	resourceTable[CST_RES_ORDER_STONE] -= costTable[CST_RES_ORDER_STONE];
	resourceTable[CST_RES_ORDER_GOLD] -= costTable[CST_RES_ORDER_GOLD];
	return true;
}


// Returns true if unit class corresponds to one of
// - Artefact/flag
// - Gatherable unit (mine, tree, gazelle - but not other animals, bushes...)
// - Units that can be created by players: buildings, living units
bool IsClassArtefactOrGatherableOrCreatable(GLOBAL_UNIT_AI_TYPES unitClass) {
	AOE_STRUCTURES::STRUCT_INF_AI fakeInfAI; // unused, ROR method must be static (not using members)
	const unsigned long int addr = 0x4BE100;
	long int res;
	long int dword_unitClass = unitClass;
	_asm {
		LEA ECX, fakeInfAI;
		PUSH dword_unitClass;
		CALL addr;
		MOV res, EAX;
	}
	return (res != 0);
}


// Returns true if unit class corresponds to units that can be created by players: buildings, living units
bool IsClassPlayerCreatable(GLOBAL_UNIT_AI_TYPES unitClass) {
	AOE_STRUCTURES::STRUCT_INF_AI fakeInfAI; // unused, ROR method must be static (not using members)
	const unsigned long int addr = 0x4BE140;
	long int res;
	long int dword_unitClass = unitClass;
	_asm {
		LEA ECX, fakeInfAI;
		PUSH dword_unitClass;
		CALL addr;
		MOV res, EAX;
	}
	return (res != 0);
}


bool GetUnitCost(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID, float costTable[]) {
	if ((DAT_ID < 0) || (DAT_ID > CST_UNIT_NAMES_MAX_ID)) { return false; }
	AOE_STRUCTURES::STRUCT_DEF_UNIT *defUnit = player->ptrStructDefUnitTable[DAT_ID];
	if (!defUnit) { return false; }
	for (int i = 0; i < 4; i++) { costTable[i] = 0; }
	if (defUnit->costs[0].costPaid) {
		if (defUnit->costs[0].costType > MAX_RESOURCE_TYPE_ID) { return false; }
		costTable[defUnit->costs[0].costType] = (float)defUnit->costs[0].costAmount;
	}
	if (defUnit->costs[1].costPaid) {
		if (defUnit->costs[1].costType > MAX_RESOURCE_TYPE_ID) { return false; }
		costTable[defUnit->costs[1].costType] = (float)defUnit->costs[1].costAmount;
	}
	if (defUnit->costs[2].costPaid) {
		if (defUnit->costs[2].costType > MAX_RESOURCE_TYPE_ID) { return false; }
		costTable[defUnit->costs[2].costType] = (float)defUnit->costs[2].costAmount;
	}
	return true;
}


// Fill resourceTypesOrder with ordered resource types: lower value in resourceAmounts = first position in (out) resourceTypesOrder
void SortResourceTypes(const int resourceAmounts[], int resourceTypesOrder[]) {
	int costsOrder_current[4] = { 0, 0, 0, 0 };
	bool valueUsed[4] = { false, false, false, false };
	for (int orderedArrayIndex = 0; orderedArrayIndex < 4; orderedArrayIndex++) {
		int lowestCost;
		bool noValue = true;
		int currentSelection;
		for (int i = 0; i < 4; i++) {
			if (!valueUsed[i] && (noValue || (resourceAmounts[i] < lowestCost))) {
				lowestCost = resourceAmounts[i];
				currentSelection = i;
				noValue = false;
			}
		}
		resourceTypesOrder[orderedArrayIndex] = currentSelection;
		valueUsed[currentSelection] = true;
	}
}



// Common function for panic mode unit searching.
// Returns true if it is possible to train the unit. In such case, cost is decreased from remainingResources and actorCounter is decreased too.
// Returns false and does not change values if it is not possible (warning: tempCost can be modified in all cases)
bool PrepareUnitToAddIfPossible(AOE_STRUCTURES::STRUCT_PLAYER *player, short int unitId_toAdd, short int unitId_actor,
	long int *actorCounter, short int *lastCostDAT_ID, float remainingResources[], float tempCost[]) {
	// Check costs. If last loop already calculated the cost for the same DAT_ID, re-use tempCost. Otherwise, recompute it.
	bool getCostIsOK = (*lastCostDAT_ID == unitId_toAdd);
	if (!getCostIsOK) {
		getCostIsOK = GetUnitCost(player, unitId_toAdd, tempCost);
		if (!getCostIsOK) { *lastCostDAT_ID = -1; } // tempCost may have been modified, do NOT try to reuse it later !!!
		else { *lastCostDAT_ID = unitId_toAdd; }
	}
	if (getCostIsOK && ApplyCostIfPossible(tempCost, remainingResources)) {
		(*actorCounter)--; // Consider 1 as "used"
		*lastCostDAT_ID = unitId_toAdd;
		return true;
	}
	return false;
}


// Returns true if a research has been researched for a given player
bool IsTechResearched(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id) {
	assert(player != NULL);
	assert(player->ptrResearchesStruct != NULL);
	if (player->ptrResearchesStruct == NULL) { return false; }
	if (player->ptrResearchesStruct->researchCount <= research_id) { return false; }
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS status = player->ptrResearchesStruct->researchStatusesArray[research_id];
	return (status.currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID);
}

// Returns current status of a research for given player.
// Returns CST_RESEARCH_STATUS_DONE_OR_INVALID if research_id is invalid (not in bounds)
AOE_CONST_FUNC::RESEARCH_STATUSES GetResearchStatus(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id) {
	assert(player != NULL);
	assert(player->ptrResearchesStruct != NULL);
	if (player->ptrResearchesStruct == NULL) { return AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID; }
	if ((research_id < 0) || (player->ptrResearchesStruct->researchCount <= research_id)) { return AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID; }
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS status = player->ptrResearchesStruct->researchStatusesArray[research_id];
	return status.currentStatus;
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
			AOE_STRUCTURES::STRUCT_RESEARCH_DEF *researchDef = &resDefInfo->researchDefArray[researchId];
			if (researchDef && status && (status->currentStatus > AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED)) {
				int techId = researchDef->technologyId;
				if ((techId >= 0) && (techId < techCount)) {
					AOE_STRUCTURES::STRUCT_TECH_DEF *techdef = &techDefInfo->ptrTechDefArray[techId];
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


// Useful to get structure from a unit id. May return NULL !
// Only works for creatable (unitId >= 0). This is just a choice to avoid writing same bugs as ROR
// (some functions use -1 as <No unit> but get an irrevant unit struct then because -1 is not tested before calling getUnitStruct(...))
AOE_STRUCTURES::STRUCT_UNIT *GetUnitStruct(long int unitId) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || (unitId < 0)) { return NULL; }
	AOE_STRUCTURES::STRUCT_UNIT *result;
	_asm {
		MOV ECX, global
		PUSH unitId
		MOV EAX, 0x005206D0
		CALL EAX
		MOV result, EAX
	}
	return result;
}


// Returns a unit definition if valid, NULL otherwise.
AOE_STRUCTURES::STRUCT_DEF_UNIT *GetUnitDefStruct(AOE_STRUCTURES::STRUCT_PLAYER *player, short int unitDefId) {
	if (!player || !player->IsCheckSumValid() || (unitDefId < 0)) { return NULL; }
	if (unitDefId >= player->structDefUnitArraySize) { return NULL; }
	return player->ptrStructDefUnitTable[unitDefId];
}


// Securely get an action pointer without having to re-write all checks/gets for intermediate objects.
// Return NULL if one of the objects is NULL/missing
// WARNING: this overload is risky (does not check actual unit structure type / might access wrong pointers !)
AOE_STRUCTURES::STRUCT_ACTION_BASE *GetUnitAction(AOE_STRUCTURES::STRUCT_UNIT *unit) {
	if (!unit || (unit->ptrActionInformation == NULL) || (unit->ptrActionInformation->ptrActionLink == NULL)) {
		return NULL;
	}
	assert(unit->ptrActionInformation->IsCheckSumValid()); // This should never occur. Whereas having NULL pointers in action info/link structures is possible (and valid)
	if (!unit->ptrActionInformation->IsCheckSumValid()) {return NULL; }
	if (unit->ptrActionInformation->ptrActionLink == NULL) {
		return NULL;
	}
	return unit->ptrActionInformation->ptrActionLink->actionStruct;
}

// Securely get an action pointer without having to re-write all checks/gets for intermediate objects.
// Return NULL if one of the objects is NULL/missing
// Please use THIS overload.
AOE_STRUCTURES::STRUCT_ACTION_BASE *GetUnitAction(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	if (!unit->DerivesFromBird()) {
		return NULL;
	}
	return GetUnitAction((AOE_STRUCTURES::STRUCT_UNIT*)unit);
}


// Returns true if unit definition is a tower (using unit type and the fact it has attacks or not)
// See also IsTower(datid) in AOE_empires_dat.h, which uses a hardcoded list.
bool IsTower(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
	if (!unitDef->DerivesFromType50()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_TYPE50 *unitDef50 = (AOE_STRUCTURES::STRUCT_UNITDEF_TYPE50 *)unitDef;
	if (!unitDef50->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	// A tower is generally a building with attack, BUT be careful, some towers are defined as living units with speed=0
	// We try to use the most generic criteria: speed=0 & attack>0.
	return (unitDef50->speed == 0) && (unitDef50->attacksCount > 0);
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
	return &ri->ptrResearchDefInfo->researchDefArray[researchId];
}


bool IsMultiplayer() {
#pragma message("TODO: bugs in IsMultiplayer when running MP and then SP games")
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = (AOE_STRUCTURES::STRUCT_GAME_SETTINGS *)AOE_OFFSETS::ADDR_VAR_GAME_SETTINGS_STRUCT;
	if (settings == NULL) { return false; }
	return (settings->isMultiplayer != 0);
}


// Returns true if the game is currently running
bool IsGameRunning() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) {
		return false;
	}
	if (global->gameRunStatus != 0) {
		return false;
	}

	return (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING);
}


// Returns true for maps where AI does build a dock and boats. Warning: for unknown map type (custom), this returns true.
bool IsDockRelevantForMap(MAP_TYPE_INDEX mti) {
	MAP_WATER_TYPE m = GetMapWaterType(mti);
	return (m != MAP_WATER_TYPE::MWT_ALL_LAND_MAP) && (m != MAP_WATER_TYPE::MWT_MOSTLY_LAND_MAP);
}


// Return the matching score element
// Warning: May return NULL.
AOE_STRUCTURES::STRUCT_SCORE_ELEM *FindScoreElement(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_CONST_FUNC::SCORE_CATEGORIES category, AOE_CONST_FUNC::RESOURCE_TYPES resourceId) {
	if (!player) { return NULL; }
	AOE_STRUCTURES::STRUCT_SCORE_HEADER *header = player->ptrScoreInformation;
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


// Returns true if a unit is idle
bool IsUnitIdle(AOE_STRUCTURES::STRUCT_UNIT *unit) {
	int result;
	_asm {
		MOV ECX, unit
		MOV EAX, 0x00406610
		CALL EAX
		MOV result, EAX
	}
	return (result != 0);
}


// Calls AOE's code mainAI.findUnit(DAT_Id)
AOE_STRUCTURES::STRUCT_UNIT_BASE *AOE_MainAI_findUnit(AOE_STRUCTURES::STRUCT_AI *mainAI, long int DAT_ID) {
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit;
	_asm {
		// Search for a unit (given its def id) in MainAI list
		PUSH - 1
		PUSH - 1
		PUSH - 1
		PUSH - 1
		PUSH - 1
		PUSH - 1
		PUSH - 1
		PUSH - 1
		PUSH DAT_ID
		PUSH - 1
		MOV EAX, 0x0040BAD0
		MOV ECX, mainAI
		CALL EAX // search for a unit in MainAI list
		MOV unit, EAX // Save the result in our variable. Can be NULL (not found)
	}
	return unit;
}


// Calls ROR's method to change a unit's action so it will move to supplied unit/position
// target can be NULL (only position will matter)
// unitToMove->ptrActionInformation is required to be NON-NULL ! Or the method will return without doing anything.
// Compatible with MP, but in MP unit will not defend itself if attacked.
void MoveUnitToTargetOrPosition(AOE_STRUCTURES::STRUCT_UNIT *unitToMove, AOE_STRUCTURES::STRUCT_UNIT *target, float posX, float posY) {
	if (!unitToMove) { return; }

	if (IsMultiplayer()) {
		// Manage MP games : use a command (but unit will not defend itself if attacked)
		if (!unitToMove || !unitToMove->IsCheckSumValid()) {
			return;
		}
		long int targetId = -1;
		if (target && target->IsCheckSumValid()) { targetId = target->unitInstanceId; }
		CreateCmd_RightClick(unitToMove->unitInstanceId, targetId, posX, posY);
		return;
	}

	// unit.createMoveToAction(targetUnitStruct,pos,pos,arg4) does NOT support if action info struct does not exist.
	if (!unitToMove->ptrActionInformation) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = unitToMove->ptrStructPlayer;

	_asm {
		MOV ECX, unitToMove
		MOV EAX, 0x405DB0 // unit.createMoveToAction(targetUnitStruct,pos,pos,arg4)
		PUSH 0 // posZ?, 0 in game call when using mouse click, so we do the same
		PUSH posX
		PUSH posY
		PUSH target
		CALL EAX
	}
	if (unitToMove->currentActivity) {
		unitToMove->currentActivity->internalId_whenAttacked = CST_ATI_NONE; // (-1) force to defend itself when attacked.
	}
}


// Tells a unit to (move and) attack another unit
bool MoveAndAttackTarget(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT *actor, AOE_STRUCTURES::STRUCT_UNIT *target) {
	if (!tacAI || !actor || !target) { return false; }
	assert(tacAI->IsCheckSumValid());
	assert(actor->IsCheckSumValid());
	assert(target->IsCheckSumValid());
	assert(actor->ptrStructDefUnit);
	assert(target->ptrStructPlayer);
	if (!tacAI->IsCheckSumValid() || !actor->IsCheckSumValid() || !target->IsCheckSumValid() || 
		!actor->ptrStructDefUnit || !target->ptrStructPlayer) { return false; }
	long int posX = (long int)target->positionX;
	long int posY = (long int)target->positionY;
	long int posZ = (long int)target->positionZ;
	float maxRange = actor->ptrStructDefUnit->maxRange;
	long int actorUnitId = actor->unitInstanceId;
	long int targetUnitId = target->unitInstanceId;
	long int targetPlayerId = target->ptrStructPlayer->playerId;
	unsigned long int actorChecksum = actor->checksum;
	long int callResult = 0;
	// Call MoveTo?
	_asm {
		PUSH -1
		PUSH -1
		PUSH 1
		PUSH 0
		PUSH targetUnitId
		PUSH maxRange
		PUSH posZ
		PUSH posX
		PUSH posY
		MOV ECX, actor
		MOV EAX, actorChecksum
		CALL [EAX + 0x198]
		MOV callResult, EAX
	}
	if (callResult == 0) {
		return false; // failure
	}
	// Call set attack task
	_asm {
		PUSH 0
		PUSH -1
		PUSH 0
		PUSH 0
		PUSH targetPlayerId
		PUSH targetUnitId
		PUSH posX
		PUSH posY
		PUSH actorUnitId
		MOV ECX, tacAI
		MOV EAX, 0x4DBE10
		CALL EAX // TacAI.taskUnit
	}
	return true;
}


// Given a list of (actor) units, tell them to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitsToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT **actorUnitsList, long int actorUnitsCount, AOE_STRUCTURES::STRUCT_UNIT *target) {
	if (!actorUnitsList || (actorUnitsCount <= 0) || !target || !target->IsCheckSumValid()) { return false; }
	assert(actorUnitsCount < 255); // More would be... more than huge !
	if (actorUnitsCount >= 255) { return false; } // This can't be normal
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_COMMANDS_INFO *commandsInfo = global->commandsInfo;
	assert(commandsInfo && commandsInfo->IsCheckSumValid());
	if (!commandsInfo || !commandsInfo->IsCheckSumValid()) { return false; }
	unsigned long int executeCommandForUnitArrayOnTarget = 0x042B730; // gameCommandsInfo.executeCommandForUnitArrayOnTarget(pUnitArray, (word)unitCount, targetUnit, f_pos, f_pos)
	float posX = target->positionX;
	float posY = target->positionY;
	CreateCmd_RightClick(actorUnitsList, actorUnitsCount, target->unitInstanceId, target->positionX, target->positionY);
	return true;
}


// Tell a unit to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT *actorUnit, AOE_STRUCTURES::STRUCT_UNIT *target) {
	AOE_STRUCTURES::STRUCT_UNIT *local_actorUnit = actorUnit;
	return TellUnitsToInteractWithTarget(&actorUnit, 1, target);
}


// Returns a unitDefCommand object if actor unit has a valid right-click command on target unit.
// Returns NULL if there no possible interaction
AOE_STRUCTURES::STRUCT_DEF_UNIT_COMMAND *GetUnitDefCommandForTarget(AOE_STRUCTURES::STRUCT_UNIT *actorUnit,
	AOE_STRUCTURES::STRUCT_UNIT *target, bool canSwitchForVillager) {
	if (!actorUnit || !target || !actorUnit->IsCheckSumValid() || !target->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef_base = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)actorUnit->ptrStructDefUnit;
	if (!unitDef_base->DerivesFromBird()) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BIRD *unitDef_asBird = (AOE_STRUCTURES::STRUCT_UNITDEF_BIRD *)unitDef_base;
	if (!unitDef_asBird->ptrUnitCommandHeader || !unitDef_asBird->ptrUnitCommandHeader->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_DEF_UNIT_COMMAND_HEADER *cmdh = unitDef_asBird->ptrUnitCommandHeader;
	AOE_STRUCTURES::STRUCT_DEF_UNIT_COMMAND *result;
	assert(cmdh != NULL);
	if (!cmdh) {
		return NULL;
	}
	_asm {
		MOV ECX, cmdh
		MOV EAX, DS:[ECX]
		PUSH 0
		PUSH 0
		PUSH 0
		PUSH target
		PUSH actorUnit
		CALL DS:[EAX+4]
		MOV result, EAX
	}
	if ((result != NULL) || !canSwitchForVillager) {
		return result; // Not villager mode: return what we just found.
	}

	// Here: not found but we can try other villager types if unit is a villager
	if (!unitDef_asBird->villagerMode) {
		return NULL;
	}
	// Try other villager types
	// We choose to use hardcoded villager IDS for simple/faster treatment
	static const long int villagerDATIDs[] = { 83, 118, 119, 120, 122, 123, 124, 127, 156, 251, 259 };
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = actorUnit->ptrStructPlayer;
	if (!actorPlayer || !actorPlayer->IsCheckSumValid()) {
		return NULL;
	}
	// Ignore 293 (upgraded villager): will be used if needed when testing 83 (base villager)
	for (int i = 0; i < sizeof(villagerDATIDs); i++) {
		if (villagerDATIDs[i] < actorPlayer->structDefUnitArraySize) {
			AOE_STRUCTURES::STRUCT_DEF_UNIT *tmpUnitDef = actorPlayer->ptrStructDefUnitTable[villagerDATIDs[i]];
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *tmpUnitDefBase = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *) tmpUnitDef;
			if (tmpUnitDefBase && tmpUnitDefBase->IsCheckSumValidForAUnitClass() && tmpUnitDefBase->DerivesFromBird()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_BIRD *tmpUnitDefBird = (AOE_STRUCTURES::STRUCT_UNITDEF_BIRD *)tmpUnitDefBase;
				cmdh = tmpUnitDefBird->ptrUnitCommandHeader;
				if (cmdh) {
					assert(cmdh->IsCheckSumValid());
					if (cmdh->IsCheckSumValid()) {
						// Try other villager type's command (with current "actorUnit")
						_asm {
							MOV ECX, cmdh
							MOV EAX, DS:[ECX]
							PUSH 0
							PUSH 0
							PUSH 0
							PUSH target
							PUSH actorUnit
							CALL DS:[EAX+4]
							MOV result, EAX
						}
					} else {
						std::string msg = "Bad commandHeader at ";
						msg += GetHexStringAddress((long)cmdh);
						traceMessageHandler.WriteMessage(msg);
						result = NULL;
					}
				}
				if (result) {
					return result; // If successful, return the command found. Otherwise, continue.
				}
			}
		}
	}
	return NULL;
}


// Returns <> 0 if a unit is (currently) available for a given player. Returns the unit def pointer.
// Does not consider potential researches that could enable the unit afterwards.
// Warning: this only works for non-upgraded unit def IDs. OK for clubman, not for axeman (because axeman's DATID1 is clubman's DATID).
long unsigned int IsUnitAvailableForPlayer(short int DAT_ID, AOE_STRUCTURES::STRUCT_PLAYER *player) {
	long int addrStructPlayer = (long int)player;
	long int fct_isUnitAvailableForPlayer = 0x4F20C0;
	int isUnitAvailableForPlayer;
	long int myDAT_ID = DAT_ID; // do NOT push short int: 2 high bytes would be arbitrary in stack (PUSH word)
	_asm {
		PUSH myDAT_ID
		MOV EAX, fct_isUnitAvailableForPlayer
		MOV ECX, addrStructPlayer
		CALL EAX
		MOV isUnitAvailableForPlayer, EAX
	}
	return isUnitAvailableForPlayer;
}


// Searches (at least) a unit with provided shortcut number for given player.
// The first matching unit is returned (arbitrary), even if there are several.
// Returns NULL if not found
AOE_STRUCTURES::STRUCT_UNIT *FindUnitWithShortcutNumberForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, char shortcutNumber) {
	assert(player != NULL);
	AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_LINK *units = player->ptrCreatableUnitsListLink;
	assert(units != NULL);
	if (units->listElemCount == 0) {
		return NULL;
	}
	AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *currentElem = units->lastListElement;
	int index = 0;
	while ((currentElem != NULL) && (index < units->listElemCount)) {
		AOE_STRUCTURES::STRUCT_UNIT *currentUnit = currentElem->unit;
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
		AOE_STRUCTURES::STRUCT_UNIT *currentUnit = currentElem->unit;
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = currentUnit->GetUnitDefBase();
		if (currentUnit && currentUnit->IsCheckSumValid() && unitDef && unitDef->IsCheckSumValidForAUnitClass()) {
			bool ok_DATID = (DAT_ID == -1) || (currentUnit->ptrStructDefUnit->DAT_ID1 == DAT_ID);
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


// Return the total remaining food amount for a farm ("immediatly available" + "action-remaining").
// Returns 0 in error cases (please check it is actually a farm !)
float GetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit) {
	if (!farmUnit || !farmUnit->IsCheckSumValid()) { return 0; }
	if (farmUnit->resourceTypeId != RESOURCE_TYPES::CST_RES_ORDER_BERRY_STORAGE) { return 0; }
	if (!farmUnit->ptrStructDefUnit || !farmUnit->ptrStructDefUnit) { return 0; }
	if (farmUnit->ptrStructDefUnit->DAT_ID1 != CST_UNITID_FARM) { return 0; }
	// Farms are quite special. Their "resource value" increases slowly, like docks' trade value.
	// This system prevents from gathering faster when assigning >1 villager. (other limitation: each farmer also has a maximum work rate !)
	// Action contains the remaining "locked" (not available yet) amount.
	// Unit resource value contains available amount (immediatly gatherable amount).
	AOE_STRUCTURES::STRUCT_ACTION_BASE *action = GetUnitAction((AOE_STRUCTURES::STRUCT_UNIT*)farmUnit);
	if (!action) { return 0; }
	return action->unsure_resourceValue + farmUnit->resourceValue;
}

// Modifies the total remaining food amount for a farm ("immediately available" + "action-remaining").
bool SetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit, float newAmount) {
	if (!farmUnit || !farmUnit->IsCheckSumValid()) { return false; }
	if (farmUnit->resourceTypeId != RESOURCE_TYPES::CST_RES_ORDER_BERRY_STORAGE) { return false; }
	if (!farmUnit->ptrStructDefUnit || !farmUnit->ptrStructDefUnit) { return false; }
	if (farmUnit->ptrStructDefUnit->DAT_ID1 != CST_UNITID_FARM) { return false; }
	AOE_STRUCTURES::STRUCT_ACTION_BASE *action = GetUnitAction((AOE_STRUCTURES::STRUCT_UNIT*)farmUnit);
	if (!action) { return false; }
	action->unsure_resourceValue = newAmount - farmUnit->resourceValue;
	if (action->unsure_resourceValue < 0) {
		action->unsure_resourceValue = 0;
		farmUnit->resourceValue = newAmount;
	}
	return true;
}


// Returns a pointer to trigger data
char *GetTriggerDataPtr(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo) {
	if (!scInfo) { return NULL; }
	return scInfo->personalityFileContent[UNUSED_PLID_FOR_TRIGGERS - 1];
}
// Set trigger data pointer. Returns false if failed.
bool SetTriggerDataPtr(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo, char *newDataPtr) {
	if (!scInfo) { return false; }
	scInfo->personalityFileContent[UNUSED_PLID_FOR_TRIGGERS - 1] = newDataPtr;
	return true;
}
// Returns a pointer to trigger data size
long int GetTriggerDataSize(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo) {
	if (!scInfo) { return 0; }
	return scInfo->personalityFileSize[UNUSED_PLID_FOR_TRIGGERS - 1];
}
// Set trigger data size. Returns false if failed.
bool SetTriggerDataSize(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo, long int sizeToSet) {
	if (!scInfo) { return false; }
	scInfo->personalityFileSize[UNUSED_PLID_FOR_TRIGGERS - 1] = sizeToSet;
	return true;
}


// Returns true if provided trigger text contains the END tag before any trigger information.
bool TriggerTextContainsENDTagAtBeginning(char *triggerText) {
	if (!triggerText) { return false; }
	char *pos = triggerText;
	char *lineBeginning = triggerText;
	bool isComment = false;
	while (*pos != 0) {
		if ((pos == lineBeginning) && (pos[0] == 'E') && (pos[1] == 'N') && (pos[2] == 'D')) {
			return true; // Found a line starting by END
		}
		if ((pos == lineBeginning) && (pos[0] == '/') && (pos[1] == '/')) {
			isComment = true; // We ignore comments
		}
		if ((pos == lineBeginning) && SpecialKeyword_equals(pos, CR_TRIGGERS::KW_TRIGGER_SECTION)) {
			return false; // Found TRIGGER keyword before END tag.
		}
		if ((*pos == '=') && (!isComment)) {
			return false; // Found "=" in a line that is not a comment
		}
		if (*pos == '\n') { // Note there might be a preceding \r, it has no impact in this function.
			lineBeginning = pos + 1;
			isComment = false;
		}
		pos++;
	}
	return false;
}


// Returns the number of queued units for a given DATID.
long int GetTotalQueueNumberForUnit(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *bld, short int unitDefId) {
	if (!bld || !bld->IsTypeValid() || !bld->ptrHumanTrainQueueInformation) { return 0; }
	if (unitDefId < 0) { return 0; }
	long int result = 0;
	for (int index = 0; index < bld->trainUnitQueueCurrentElemCount; index++) {
		if ((bld->ptrHumanTrainQueueInformation[index].DATID == unitDefId) && (bld->ptrHumanTrainQueueInformation[index].unitCount >= 0)) {
			result += bld->ptrHumanTrainQueueInformation[index].unitCount;
		}
	}
	return result;
}


// Creates a unit at provided location. Does NOT make checks on location, please first make sure GetErrorForUnitCreationAtLocation returns 0.
// Please use CheckAndCreateUnit instead (unless you explicitely do NOT want to check target location)
// You can use 0 as posZ value.
// Returns NULL if it failed
AOE_STRUCTURES::STRUCT_UNIT *CreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID, float posY, float posX, float posZ) {
	if (!player || !player->IsCheckSumValid()) {
		return NULL;
	}
	long int res;
	_asm {
		MOV ECX, player
		PUSH 0 // arg5=unused ?
		PUSH posZ
		PUSH posX
		PUSH posY
		PUSH DAT_ID
		MOV EAX, 0x4F0B50 // does not check if there is free space to accept the new unit nor terrain restriction... (check it first)
		CALL EAX
		MOV res, EAX
	}
	return (AOE_STRUCTURES::STRUCT_UNIT *) res;
}


// Creates a unit at provided location only if GetErrorForUnitCreationAtLocation agrees !
// Returns NULL if it failed
AOE_STRUCTURES::STRUCT_UNIT *CheckAndCreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_DEF_UNIT *unitDef,
	float posX, float posY, bool checkVisibility, bool checkHills, bool checkConflictingUnits) {
	if (GetErrorForUnitCreationAtLocation(player, unitDef, posY, posX, checkVisibility, checkHills, false, true, checkConflictingUnits) != 0) {
		return NULL;
	}
	return CreateUnit(player, unitDef->DAT_ID1, posY, posX, 0);
}


// Will has the same effect as "CTRL-0" or "CTRL-1" etc: assigns a shortcut number to units (and removes this shortcut from old units that had it)
// Do not use numbers > 10 (0x0A), they are used for groups. You can use negative values.
// shortcutNumber must be an "internal" shortcut number ([-11;-2], 0 or [1-10] for shortcuts). This is not checked here.
// shortcutNumber>10 values correspond to groups.
// Other numbers than 1-9 will never be displayed as SLP file contains only 1-9 images (unless using custom DRS/SLPs)
// Returns 1 on success.
long int AssignShortcutToSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player, long int shortcutNumber) {
	if (!player) { return 0; }
	// shortcutNumber is a char, actually.
	if ((shortcutNumber < -128) || (shortcutNumber > 127)) { return 0; }
	char c_shortcut = (char)shortcutNumber;
	long int res = 0;
	//GetPlayerStruct(playerId);
	_asm {
		MOV ECX, player
		MOVSX EAX, c_shortcut
		PUSH EAX
		MOV EDX, 0x0045DE00 // player.assignShortcut(shortcutNumber)
		CALL EDX
		MOV res, EAX
	}
	return res;
}

// Selects units that have a given shortcut number.
// shortcutNumber must be an "internal" shortcut number ([-11;-2], 0 or [1-10] for shortcuts). This is not checked here.
// shortcutNumber>10 values correspond to groups.
// Returns 1 on success.
long int SelectUnitsUsingShortcut(AOE_STRUCTURES::STRUCT_PLAYER *player, long int shortcutNumber, bool addToSelection) {
	if (!player) { return 0; }
	// shortcutNumber is a char, actually.
	//if ((shortcutNumber < 0) || (shortcutNumber > 0xFF)) { return 0; }
	if ((shortcutNumber < -128) || (shortcutNumber > 127)) { return 0; }
	char c_shortcut = (char)shortcutNumber;
	long int res = 0;
	if (!addToSelection) { // Clear selection first
		_asm {
			MOV ECX, player
			MOV EDX, 0x0045DCB0 // player.ClearSelectedUnits
			CALL EDX
		}
	}
	_asm {
		MOV ECX, player
		MOVSX EAX, c_shortcut
		PUSH EAX
		MOV EDX, 0x0045DF40 // player.selectUnitsForShortcut(shortcutNumber)
		CALL EDX
		MOV res, EAX
	}
	return res;
}


// Apply a research for a player (if possible).
// Return true if successful
bool ApplyResearchForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id) {
	if (!player || !player->IsCheckSumValid() || (!player->ptrResearchesStruct)) { return false; }
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *researchInfo = player->ptrResearchesStruct;
	if (!researchInfo) { return false; }
	if ((research_id < 0) || (research_id >= researchInfo->researchCount)) { return false; }
	short int currentStatus = researchInfo->researchStatusesArray[research_id].currentStatus;
	if ((currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) ||
		(currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID)) {
		return false;
	}
	long int result;
	// Apply research if its status is not done and not disabled.
	_asm {
		MOV ECX, researchInfo
		XOR EAX, EAX
		MOV AX, research_id
		PUSH EAX
		MOV EAX, 0x004EAA3D
		CALL EAX
		MOV result, EAX
	}
	return (result != 0);
}
bool ApplyResearchForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int research_id) {
	return ApplyResearchForPlayer(player, (short int)research_id);
}


// Enable or disable a research for a player
void AOE_enableResearch(AOE_STRUCTURES::STRUCT_PLAYER *player, long int research_id, bool enable) {
	if (!player || !player->IsCheckSumValid()) { return; }
	if (!player->ptrResearchesStruct) { return; }
	if ((research_id < 0) || (research_id >= player->ptrResearchesStruct->researchCount)) { return; }

	const long int f = 0x4F1870;
	long int argResearch_id = research_id & 0xFF; // It is a byte only
	long int argEnable = enable ? 1 : 0;
	_asm {
		MOV ECX, player
		PUSH argEnable
		PUSH argResearch_id
		CALL f
	}
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
	AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *techDefInfo= global->technologiesInfo;
	if (!techDefInfo || (!techDefInfo->IsCheckSumValid())) { return false; }
	if (techDefInfo->ptrTechDefArray == NULL) { return false; }

	// Search for the technology that enables that unit
	long int techId = -1;
	long int effectsCount = 0;
	for (int i = 0; (i < techDefInfo->technologyCount) && (techId == -1); i++) {
		effectsCount = 0;
		for (int effectId = 0; effectId < techDefInfo->ptrTechDefArray[i].effectCount; effectId++) {
			assert(techDefInfo->ptrTechDefArray[i].ptrEffects != NULL); // should never happen because effectCount would be 0
			// Count number of effects for this tech, excluding the "increase researches count" thing.
			if ((techDefInfo->ptrTechDefArray[i].ptrEffects[effectId].effectType != AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET) || // resource modifier
				(techDefInfo->ptrTechDefArray[i].ptrEffects[effectId].effectUnit != RESOURCE_TYPES::CST_RES_ORDER_TECHNOLOGIES)
				) {
				effectsCount++;
			}
			if ((techDefInfo->ptrTechDefArray[i].ptrEffects[effectId].effectType == AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) &&
				(techDefInfo->ptrTechDefArray[i].ptrEffects[effectId].effectUnit == DAT_ID)
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
			assert(player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray != NULL); // should never happen when researchCount >0
			if (player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[i].technologyId == techId) {
				researchId = i;
			}
		}
	}

	// And disable it : only if no side effects (some techs have more than 1 effect, to disable the research would remove other effects !)
	// Are concerned: tech0(fishingship+trade), tech26(farm), tech94(stoneAge:villager+house)
	// Here: we could think about forcing update for farms (tech=26) because the 2nd effect is useless ? (seems obsolete and invalid)
	if ((researchId != -1) && (effectsCount == 1)) {
		AOE_enableResearch(player, researchId, false);
		return true;
	}
	return false;
}


// Apply a technology for a player. To be used only when there is no research for given technology (like cheat techs).
// In most cases, please use ApplyResearchForPlayer instead.
bool ApplyTechnologyForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int tech_id) {
	if (!player || !player->IsCheckSumValid() || (!player->ptrGlobalStruct) || (!player->ptrGlobalStruct->IsCheckSumValid())) { return false; }
	if (tech_id < 0) { return false; } // The call we do will check that tech_id is valid.
	AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *techDefInfo = player->ptrGlobalStruct->technologiesInfo;
	if (!techDefInfo || !techDefInfo->IsCheckSumValid()) { return false; }
	_asm {
		MOV ECX, techDefInfo
		XOR EAX, EAX
		MOV AX, tech_id
		PUSH player
		PUSH EAX
		MOV EAX, 0x004EBB10
		CALL EAX
		// No return value
	}
	return true;
}
bool ApplyTechnologyForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int tech_id) {
	return ApplyTechnologyForPlayer(player, (short int)tech_id);
}


// Calls AOE's method to change a unit owner. Warning, this has bugs, see crCommand.OnUnitChangeOwner_fixes to call missing treatments.
void AOE_ChangeUnitOwner(AOE_STRUCTURES::STRUCT_UNIT *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer) {
	if (!targetUnit || !actorPlayer || !targetUnit->IsCheckSumValid() || !actorPlayer->IsCheckSumValid()) {
		return;
	}
	_asm {
		MOV ECX, targetUnit
		MOV EAX, actorPlayer
		MOV EDX, DS:[ECX]
		PUSH EAX
		CALL DS:[EDX+0x44] // unit.convertToPlayer(ptrPlayer)
	}
}


// Call AOE's Notify event method. Warning, the parameters can have different types.
// Use the overload with pointers to make sure you don't have cast issues.
void AOE_callNotifyEvent(long int eventId, long int playerId, long int variant_arg3, long int variant_arg4, long int variant_arg5){
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	_asm {
		MOV ECX, settings
		PUSH variant_arg5
		PUSH variant_arg4
		PUSH variant_arg3
		PUSH playerId
		PUSH eventId
		MOV EAX, 0x501980
		CALL EAX
	}
}
// Call AOE's Notify event method. Warning, the parameters can have different types. Here pointers here so there is no undesired cast !
void AOE_callNotifyEvent(long int eventId, long int playerId, void *variant_arg3, void *variant_arg4, void *variant_arg5) {
	AOE_callNotifyEvent(eventId, playerId, *(long int*)variant_arg3, *(long int*)variant_arg4, *(long int*)variant_arg5);
}


void AOE_clearSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player) {
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
bool AOE_selectUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT *unit, bool select) {
	if (!player || !player->IsCheckSumValid() || !unit || !unit->IsCheckSumValid()) {
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


// Calls AOE's path finding method.
// allArgs indices are 1-15 (do NOT use 0). Warning, allArgs[6] is a float, not an int.
// Arguments (1-15) are:
// srcPosY, srcPosX, destPosY, destPosX, ptrActorUnit, f_range?, targetUnitId, updateUnitPathInfo?(!OnlyCheck),
// arg9, arg10, arg11, arg12, arg13(int_distance?), arg14, arg15
long int callFindPathForUnit(long int allArgs[15 + 1]) {
	const long int myecx = 0x583BC8;
	const long int callAddr = 0x458930;
	long int result;
	_asm {
		MOV ECX, myecx
		MOV EAX, allArgs
		ADD EAX, 0x3C
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		MOV EAX, callAddr
		CALL EAX
		MOV result, EAX
	}
#ifdef _DEBUG
	bool doTrace = false; // change this to enable/disable debug on call result.
	if (doTrace) {
		char buffer[800];
		// fix float arg
		float *pf = (float*)(&allArgs[6]);
		double f = (double)*pf;
		std::string s5 = GetHexStringAddress(allArgs[5]);
		std::string s7 = GetHexStringAddress(allArgs[7]);
		sprintf_s(buffer, 800, "%ld %ld %ld %ld %s %7.1f %s %d %d %d %d %d %d %d %d ; res=%d", allArgs[1], allArgs[2], allArgs[3], allArgs[4],
			s5.c_str(), f, s7.c_str(),
			allArgs[8], allArgs[9], allArgs[10], allArgs[11], allArgs[12], allArgs[13], allArgs[14], allArgs[15], result);
		traceMessageHandler.WriteMessageNoNotification(buffer);
	}
#endif
	return result;
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
		MOV ECX, global
		MOV EAX, 0x520C60
		CALL EAX
	}
}


// Set "shared exploration" flag for a given player to true or false. This version should be compatible with MP games (uses ROR command system)
void SetPlayerSharedExploration_safe(long int playerId) {
	CreateCmd_SetSharedExploration((short int)playerId);
}


// Reset an element in infAI.unitElemList. The slot will be re-used later by ROR. cf 0x4BA401.
// Return true if the element was updated.
bool ResetInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *elem) {
	if (!elem) { return false; }
	elem->unitId = -1;
	elem->unitDATID = -1;
	elem->unitClass = (AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES) - 1;
	elem->posX = 0;
	elem->posY = 0;
	elem->posZ = 0;
	elem->playerId = 0;
	elem->HP = 0;
	elem->unknown_10 = 0;
	elem->unknown_14 = 0;
	elem->attack = 0;
	elem->reloadTime1 = 0;
	elem->maxRange = 0;
	return true;
}


// Update an element in infAI.unitElemList if the unit is visible.
// Reset the element otherwise.
// If the element is reset, it is ALSO REMOVED from infAI lists.
// Return true if the element was updated/reset.
bool UpdateOrResetInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *elem) {
	if (!infAI || !infAI->IsCheckSumValid() || !elem) { return false; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(elem->unitId);
	assert((unitBase == NULL) || (unitBase->IsCheckSumValidForAUnitClass()));
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = NULL;
	long int actorPlayerId = infAI->commonAIObject.playerId;
	long int unitPlayerId = -1;
	if (unitBase) {
		unitDefBase = unitBase->GetUnitDefinition();
		assert(unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass());
		if (unitBase->ptrStructPlayer && unitBase->ptrStructPlayer->IsCheckSumValid()) {
			unitPlayerId = unitBase->ptrStructPlayer->playerId;
		}
	}
	bool unitItVisible = false;
	if (unitBase && unitDefBase) {
		if (unitDefBase->visibleInFog) {
			// The unit still exists, and is visible through fog: only remove it from the list if its position is not EXPLORED.
			unitItVisible = IsExploredForPlayer(actorPlayerId, (long int)unitBase->positionX, (long int)unitBase->positionY);
		} else {
			// Standard case, check fog visibility.
			unitItVisible = IsFogVisibleForPlayer(actorPlayerId, (long int)unitBase->positionX, (long int)unitBase->positionY);
		}
	}
	if (unitItVisible) {
		// Update the element because I can see it (no cheating)
		elem->playerId = (char)unitPlayerId; // the most important update
		if (unitBase) { // should always be true here
			elem->posX = (unsigned char)unitBase->positionX;
			elem->posY = (unsigned char)unitBase->positionY;
			elem->HP = (short)unitBase->remainingHitPoints;
		}
		if (unitDefBase && unitDefBase->DerivesFromType50()) {
			// By the way... This part is not really necessary.
			AOE_STRUCTURES::STRUCT_UNITDEF_TYPE50 *unitDef50 = (AOE_STRUCTURES::STRUCT_UNITDEF_TYPE50 *)unitDefBase;
			elem->maxRange = unitDef50->maxRange;
			elem->reloadTime1 = unitDef50->reloadTime1;
			//elem->attack = unitDef50->displayedAttack;
		}
		return true;
	} else { // No longer visible
		// First remove from infAI lists, if needed. (do it while elem has not been reset... yet)
		RemoveFromInfAIInfoList(infAI, elem->unitId, elem->unitDATID, elem->unitClass);

		// Reset the element
		return ResetInfAIUnitListElem(elem);
	}
	return false;
}


// Remove a unitId from infAI "info" lists (creatable, gatherable, "defendable", artefact units).
// This does NOT manage "all my units" and "my buildings" lists. This does NOT manage unitElemList neither.
// DATID and unitAIType (unitClass) are used for optimisation. You can provide -1 if you don't have the information.
// Returns false if failed.
bool RemoveFromInfAIInfoList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int unitId, short int DATID, AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType) {
	if (!infAI || !infAI->IsCheckSumValid() || (unitId < 0)) { return false; }

	if ((unitAIType == -1) || IsClassArtefactOrGatherableOrCreatable(unitAIType)) {
		infAI->creatableAndGatherableUnits.Remove(unitId);
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


// Find a unitElem in infAI list, returns NULL if not found.
AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *FindInfAIUnitElemInList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int unitId) {
	if (!infAI || !infAI->IsCheckSumValid() || (unitId < 0)) { return NULL; }
	for (int i = 0; i < infAI->unitElemListSize; i++) {
		if (infAI->unitElemList[i].unitId == unitId) {
			return &infAI->unitElemList[i];
		}
	}
	return NULL;
}


// Change unit owner in InfAI unitListElem according to unit visibility
// Return true if updated.
bool UpdateUnitOwnerInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit,
	long int newPlayerId) {
	if (!infAI || !infAI->IsCheckSumValid()) { return false; }
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	if (newPlayerId < 0) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unit->GetUnitDefinition();
	if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) { return false; }
	if (unit->unitInstanceId < 0) { return false; } // Ignore temp units.
	bool isVisible = false;
	if (unitDefBase->visibleInFog) {
		isVisible = IsExploredForPlayer(infAI->commonAIObject.playerId, (long int)unit->positionX, (long int)unit->positionY);
	} else {
		isVisible = IsFogVisibleForPlayer(infAI->commonAIObject.playerId, (long int)unit->positionX, (long int)unit->positionY);
	}
	// Search for unit in infAI unit elem list
	long int size = infAI->unitElemListSize;
	for (long int i = 0; i < size; i++) {
		AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *curElem = &infAI->unitElemList[i];
		if (curElem->unitId == unit->unitInstanceId) {
			if (isVisible) {
				curElem->playerId = (char)newPlayerId;
				return true;
			} else {
				return false; // Not visible=no update. Exploring should do the update.
			}
		}
	}
	return false;
}


// Set status for an element in infAI "construction history" array.
void AOE_InfAIBuildHistory_setStatus(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int posX, long int posY, long int unitDefId,
	AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS status) {
	if (!infAI || !infAI->IsCheckSumValid()) {
		return;
	}
	// Note: bad position values have no impact here (no possible crash), it will just find no item. Same for unitDefId.
	assert((status >= 0) && (status <= AOE_CONST_INTERNAL::CST_BHS_MAX_VALUE));
	long int status_long = status;
	const unsigned long int RORMethod = 0x4C34C0;
	_asm {
		MOV ECX, infAI
		PUSH status_long
		PUSH posX
		PUSH posY
		PUSH unitDefId
		CALL RORMethod
	}
}


// Remove a building from arrays for a player
void AOE_playerBldHeader_RemoveBldFromArrays(AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader, AOE_STRUCTURES::STRUCT_UNIT *unit) {
	if (buildingsHeader && unit && unit->IsCheckSumValid()) {
		_asm {
			MOV ECX, buildingsHeader
			MOV EDX, unit
			PUSH EDX
			MOV EAX, 0x00436980 // playerUnitsHeader.removeBuildingFromArrays(ptrUnit)
			CALL EAX
		}
	}
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
	AOE_clearSelectedUnits(player);
	AOE_selectUnit(player, (AOE_STRUCTURES::STRUCT_UNIT*) unitBase, true);
	if (centerScreen) {
		player->screenPositionX = unitBase->positionX;
		player->screenPositionY = unitBase->positionY;
		player->unknown_122_posX = (short int)unitBase->positionX;
		player->unknown_120_posY = (short int)unitBase->positionY;
	}
	if (!settings || !settings->IsCheckSumValid() || !settings->ptrGameUIStruct ||
		!settings->ptrGameUIStruct->IsCheckSumValid() || !settings->ptrGameUIStruct->gamePlayUIZone ||
		!settings->ptrGameUIStruct->gamePlayUIZone->IsCheckSumValid()) {
		return;
	}
	AOE_RefreshUIObject(settings->ptrGameUIStruct->gamePlayUIZone);
}


// Add a line with an attribute icon/value in game unit info zone (bottom left)
// If a line is added, lineIndex is incremented.
void UnitInfoZoneAddAttributeLine(AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone,
	long int iconId, long int displayType, long int displayedValue, long int totalValue, long int &lineIndex) {
	if (!unitInfoZone || !unitInfoZone->IsCheckSumValid()) {
		return;
	}
	if (iconId < 0) { return; } // and zero ?
	// Make sure we use correct stack data
	totalValue = totalValue & 0xFFFF;
	displayedValue = displayedValue & 0xFFFF;
	long int line = lineIndex; // PUSH lineIndex would not push the VALUE !
	_asm {
		PUSH totalValue;
		PUSH displayedValue;
		PUSH displayType;
		PUSH iconId;
		PUSH line;
		MOV ECX, unitInfoZone;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0xE4]; // Add info line
	}
	lineIndex++;
}


// commandStruct must have been allocated with a "AOE" alloc method like AOEAlloc(...)
// It is freed by game code, don't use it / free it afterwards !
// Returns false if failed
bool AddCommandToGameCmdQueue(void *commandStruct, long int structSize) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_GAME_COMMANDS_INFO *cmdInfo = global->commandsInfo;
	assert(cmdInfo && cmdInfo->IsCheckSumValid());
	if (!cmdInfo || !cmdInfo->IsCheckSumValid()) {
		return false;
	}
	const unsigned long int addr = 0x42B6E0; // commandsInfo.addCommandToList(ptrCommand, structSize)
	_asm {
		PUSH structSize
		PUSH commandStruct
		MOV EAX, addr
		MOV ECX, cmdInfo
		CALL EAX // returns void
	}
	return true;
}


// Create a "ROR" command struct (right-click). Returns false if failed.
// targetUnitId is optional. If -1 or non valid, then unit will just move to posX/posY.
// This method is compatible with MP games (does NOT causes sync issue)
bool CreateCmd_RightClick(long int actorUnitId, long int targetUnitId, float posX, float posY) {
	AOE_STRUCTURES::STRUCT_UNIT *unit = GetUnitStruct(actorUnitId);
	if (!unit || !unit->IsCheckSumValid()) {
		return false;
	}
	//AOE_STRUCTURES::STRUCT_UNIT *actorUnit = NULL;
	return CreateCmd_RightClick(&unit, 1, targetUnitId, posX, posY);
}

bool CreateCmd_RightClick(AOE_STRUCTURES::STRUCT_UNIT **actorUnitsList, long int actorUnitsCount, long int targetUnitId, float posX, float posY) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) {
		return false;
	}
	assert(actorUnitsCount > 0);
	if (actorUnitsCount <= 0) { return false; }
	const long int structSize = 0x1C + (4 * actorUnitsCount);
	AOE_STRUCTURES::COMMAND_RIGHT_CLICK *cmd = (AOE_STRUCTURES::COMMAND_RIGHT_CLICK *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_RIGHT_CLICK;
	assert(cmd->IsCmdIdValid());
	cmd->actorCount = (char) actorUnitsCount;
	for (int i = 0; i < actorUnitsCount; i++) {
		if (actorUnitsList[i]) {
			cmd->actorIdList[i] = actorUnitsList[i]->unitInstanceId;
		}
		else {
			cmd->actorIdList[i] = -1;
		}
	}
	cmd->targetUnitId = targetUnitId;
	cmd->posX = posX;
	cmd->posY = posY;
	cmd->humanPlayerId = 0; // See below, overwritten if MP game.
	cmd->targetVisibleInFog = 0; // default
	AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(targetUnitId);
	if (targetUnit && targetUnit->IsCheckSumValidForAUnitClass()) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *targetDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *) targetUnit->ptrStructDefUnit;
		if (targetDef && targetDef->IsCheckSumValidForAUnitClass()) {
			cmd->targetVisibleInFog = targetDef->visibleInFog;
		}
	}
	if (IsMultiplayer()) {
		cmd->humanPlayerId = (char)global->humanPlayerId;
#pragma message("MP Not fully supported at this point")
		// Not handled at this point
		// See 0x42B7ED...
		cmd->fogVisibilityMask = 0; // TO DO
		cmd->explorationVisibilityMask = 0; // TO DO
		cmd->sharedExploration = 0; // TO DO
		cmd->unknown_13 = 0; // TO DO
		cmd->unknown_14 = 0; // TO DO
	}
	// Our command is ready. Call API to add it in queue.
	return AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (build). Returns false if failed.
bool CreateCmd_Build(long int actorUnitId, short int DATID, float posX, float posY) {
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(actorUnitId);
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->ptrStructPlayer || !unit->ptrStructPlayer->IsCheckSumValid()) {
		return false;
	}
	const long int structSize = 0x18; // 0x14 + 4*1 (1 actor unit)
	AOE_STRUCTURES::COMMAND_MAKE_OBJECT *cmd = (AOE_STRUCTURES::COMMAND_MAKE_OBJECT *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_BUILD;
	assert(cmd->IsCmdIdValid());
	cmd->builderIdList[0] = actorUnitId;
	cmd->builderCount = 1;
	cmd->playerId = (char) unit->ptrStructPlayer->playerId;
	cmd->unitDefIdToBuild = DATID;
	cmd->stratElemId = -1;
	cmd->posX = posX;
	cmd->posY = posY;
	return AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (change diplomacy). Returns false if failed.
bool CreateCmd_ChangeDiplomacy(short int playerId, short int targetPlayerId, PLAYER_DIPLOMACY_STANCES diplomacyStance) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_CHANGE_DIPLOMACY);
	AOE_STRUCTURES::COMMAND_CHANGE_DIPLOMACY *cmd = (AOE_STRUCTURES::COMMAND_CHANGE_DIPLOMACY *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_CHANGE_DIPLOMACY;
	assert(cmd->IsCmdIdValid());
	cmd->actorPlayerId = playerId;
	cmd->targetPlayerId = targetPlayerId;
	cmd->diplomacyStance = (short int)diplomacyStance;
	cmd->floatDiplomacyStance = (float)diplomacyStance;
	cmd->unknown_6 = 0;
	cmd->unused_E = 0;
	return AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (set ally victory flag). Returns false if failed.
bool CreateCmd_SetAllyVictory(short int playerId, bool enable) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_SET_ALLY_VICTORY);
	AOE_STRUCTURES::COMMAND_SET_ALLY_VICTORY *cmd = (AOE_STRUCTURES::COMMAND_SET_ALLY_VICTORY *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_SET_ALLY_VICTORY;
	assert(cmd->IsCmdIdValid());
	cmd->actorPlayerId = playerId;
	cmd->allyVictoryFlag = (short int)enable;
	cmd->unknown_6 = 0;
	cmd->unused_8 = 0;
	cmd->unknown_C = 0;
	cmd->unused_E = 0;
	return AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (give writing - set shared exploration). Returns false if failed.
bool CreateCmd_SetSharedExploration(short int playerId) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_APPLY_WRITING);
	AOE_STRUCTURES::COMMAND_APPLY_WRITING *cmd = (AOE_STRUCTURES::COMMAND_APPLY_WRITING *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_APPLY_WRITING_TECH;
	assert(cmd->IsCmdIdValid());
	cmd->playerId = playerId;
	cmd->unused_4 = 0;
	cmd->unused_6 = 0;
	cmd->unused_8 = 0;
	cmd->unused_C = 0;
	cmd->unused_E = 0;
	return AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (change game speed). Returns false if failed.
bool CreateCmd_ChangeGameSpeed(float newSpeed) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_CHANGE_GAME_SPEED);
	AOE_STRUCTURES::COMMAND_CHANGE_GAME_SPEED *cmd = (AOE_STRUCTURES::COMMAND_CHANGE_GAME_SPEED *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_CHANGE_GAME_SPEED;
	assert(cmd->IsCmdIdValid());
	cmd->newSpeed = newSpeed;
	cmd->unused_2 = 0;
	cmd->unused_4 = 0;
	cmd->unused_6 = 0;
	cmd->unused_C = 0;
	cmd->unused_E = 0;
	return AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (add a resource amount to a player). Returns false if failed.
bool CreateCmd_AddResource(short int playerId, short int resourceId, float value) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_ADD_RESOURCE);
	AOE_STRUCTURES::COMMAND_ADD_RESOURCE *cmd = (AOE_STRUCTURES::COMMAND_ADD_RESOURCE *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_ADD_RESOURCE;
	assert(cmd->IsCmdIdValid());
	cmd->actorPlayerId = playerId;
	cmd->resourceId = resourceId;
	cmd->resourceAmount = value;
	cmd->unused_6 = 0;
	cmd->unused_C = 0;
	cmd->unused_E = 0;
	return AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (set ally victory flag). Returns false if failed.
bool CreateCmd_SetSteroids(bool enable) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_SET_STEROIDS_MODE);
	AOE_STRUCTURES::COMMAND_SET_STEROIDS_MODE *cmd = (AOE_STRUCTURES::COMMAND_SET_STEROIDS_MODE *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_SET_STEROIDS_MODE;
	assert(cmd->IsCmdIdValid());
	cmd->steroidsModeFlag = (short int)enable;
	cmd->unused_4 = 0;
	cmd->unused_6 = 0;
	cmd->unused_8 = 0;
	cmd->unused_C = 0;
	cmd->unused_E = 0;
	return AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (pay tribute). Returns false if failed.
bool CreateCmd_PayTribute(long int actorPlayerId, long int targetPlayerId, AOE_CONST_FUNC::RESOURCE_TYPES resourceType, float amount, float tributeInefficiency) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_PAY_TRIBUTE);
	AOE_STRUCTURES::COMMAND_PAY_TRIBUTE *cmd = (AOE_STRUCTURES::COMMAND_PAY_TRIBUTE *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_PAY_TRIBUTE;
	assert(cmd->IsCmdIdValid());
	cmd->actorPlayerId = (char)actorPlayerId;
	cmd->targetPlayerId = (char)targetPlayerId;
	cmd->resourceId = (char)resourceType;
	cmd->amount = amount;
	cmd->tributeTaxProportion = tributeInefficiency;
	return AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a new unitDef, copied for provided one, using actual derived class.
// Returns a STRUCT_UNITDEF_BASE, but it can be any derived class (living, building, etc)
AOE_STRUCTURES::STRUCT_UNITDEF_BASE *CopyUnitDefToNewUsingGoodClass(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *existingUnitDef) {
	if (!existingUnitDef || !existingUnitDef->IsCheckSumValidForAUnitClass()) {
		return NULL;
	}
	switch (existingUnitDef->unitType) {
	case GLOBAL_UNIT_TYPES::GUT_BIRD:
		return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_BIRD>((AOE_STRUCTURES::STRUCT_UNITDEF_BIRD*)existingUnitDef);
		break;
	case GLOBAL_UNIT_TYPES::GUT_BUILDING:
		return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING>((AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING*)existingUnitDef);
		break;
	case GLOBAL_UNIT_TYPES::GUT_DEAD_UNITS:
		return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_DEAD_FISH>((AOE_STRUCTURES::STRUCT_UNITDEF_DEAD_FISH*)existingUnitDef);
		break;
	case GLOBAL_UNIT_TYPES::GUT_DOPPLEGANGER:
		return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_DOPPLEGANGER>((AOE_STRUCTURES::STRUCT_UNITDEF_DOPPLEGANGER*)existingUnitDef);
		break;
	case GLOBAL_UNIT_TYPES::GUT_EYE_CANDY:
		return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_BASE>((AOE_STRUCTURES::STRUCT_UNITDEF_BASE*)existingUnitDef);
		break;
	case GLOBAL_UNIT_TYPES::GUT_FLAGS:
		return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_FLAG>((AOE_STRUCTURES::STRUCT_UNITDEF_FLAG*)existingUnitDef);
		break;
	case GLOBAL_UNIT_TYPES::GUT_LIVING_UNIT:
		return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_LIVING>((AOE_STRUCTURES::STRUCT_UNITDEF_LIVING*)existingUnitDef);
		break;
	case GLOBAL_UNIT_TYPES::GUT_PROJECTILE:
		return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE>((AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE*)existingUnitDef);
		break;
	case GLOBAL_UNIT_TYPES::GUT_TREE:
		return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_TREE>((AOE_STRUCTURES::STRUCT_UNITDEF_TREE*)existingUnitDef);
		break;
	case GLOBAL_UNIT_TYPES::GUT_TYPE50:
		return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_TYPE50>((AOE_STRUCTURES::STRUCT_UNITDEF_TYPE50*)existingUnitDef);
		break;
	default:
		traceMessageHandler.WriteMessage("Unhandled unit definition class.");
		return NULL;
	}
}


// Extends a player's unitDef table to add a new one (unitDef).
// unitDef's DAT_ID1 and DAT_ID2 are modified with new ID.
// Returns -1 on failure, new unitDefId on success.
short int AddUnitDefToPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
	if (!player || !player->IsCheckSumValid() || !unitDef || !unitDef->IsCheckSumValidForAUnitClass()) {
		return -1;
	}

	short int nbDef = player->structDefUnitArraySize + 1; // new number of unit definitions
	short int newDATID = player->structDefUnitArraySize;
	long int newSizeInBytes = nbDef * 4;
	AOE_STRUCTURES::STRUCT_DEF_UNIT **oldArray = player->ptrStructDefUnitTable;
	// Run some checks
	assert(oldArray != NULL);
	if (oldArray == NULL) { return -1; }
	assert(0 <= newDATID);

	// Create new array
	AOE_STRUCTURES::STRUCT_DEF_UNIT **newArray = (AOE_STRUCTURES::STRUCT_DEF_UNIT **)AOEAlloc(newSizeInBytes);
	if (newArray == NULL) { return -1; } // nothing allocated: return an error

	// Copy old array into new (for all existing unitDefs => copy pointers)
	memcpy(newArray, oldArray, newSizeInBytes);
	AOEFree(oldArray); // old array is freed, we replace it by new (larger) one
	player->ptrStructDefUnitTable = newArray;
	player->structDefUnitArraySize = nbDef;
	player->ptrStructDefUnitTable[newDATID] = (AOE_STRUCTURES::STRUCT_DEF_UNIT *)unitDef;
	unitDef->DAT_ID1 = newDATID;
	unitDef->DAT_ID2 = newDATID;
	return newDATID;
}


// Get a localized string using ROR method.
// Returns true on success.
bool AOE_ReadLanguageTextForCategory(INTERNAL_MAIN_CATEGORIES category, long int commandId, long int subParam, char *buffer, long int bufferSize) {
	if (!buffer || (bufferSize <= 0)) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) {
		return false;
	}
	long int result = 0;
	const unsigned long int addr = 0x4FF580;
	_asm {
		PUSH bufferSize;
		PUSH buffer;
		PUSH subParam;
		PUSH commandId;
		PUSH category;
		MOV ECX, settings;
		CALL addr;
		MOV result, EAX;
	}
	return (result != 0);
}


// Generate full creation text for a button (research, train) with costs and everything.
// buffer size must be at least 0x200.
// unitButtonInfo is allowed to be NULL.
// Cost info come from unitButtonInfo.
// elemLanguageCreationDllId is unit (or research) creationDllId (it includes shortcut key, if any)
void AOE_GetUIButtonCreationText(char *buffer, AOE_STRUCTURES::STRUCT_UI_UNIT_BUTTON_INFO *unitButtonInfo,
	AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID uiCmdId, long int elemLanguageCreationDllId) {
	if (!buffer) {
		return;
	}
	buffer[0] = 0;
	unsigned long int addr = 0x4834F0;
	_asm {
		PUSH elemLanguageCreationDllId;
		PUSH uiCmdId;
		PUSH unitButtonInfo;
		PUSH buffer;
		CALL addr;
	}
}


// Returns the icon id relevant for provided UI command id, if found.
// Returns -1 if not found. (WARNING: check this case in caller, -1 is not an acceptable value)
long int GuessIconIdFromUICommandId(AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID UICmdId) {
	// Note : icon 6 in actions is a nice flag but is unused.
	switch (UICmdId) {
	case AOE_CONST_INTERNAL::CST_IUC_WORK:
		return -1; // does not have an icon ?
	case AOE_CONST_INTERNAL::CST_IUC_MOVE:
		return 1; // Just a supposition as it is not implemented ! (actually, from early versions then removed, probably)
	case AOE_CONST_INTERNAL::CST_IUC_BUILD:
		return 2; // cf 482598, 483273
	case AOE_CONST_INTERNAL::CST_IUC_EXCHANGE:
		return -1; // unused/not implemented?
	case AOE_CONST_INTERNAL::CST_IUC_STOP:
		return 3; // hand icon
	case AOE_CONST_INTERNAL::CST_IUC_CANCEL_SELECTION:
		return 0x0A;
	case AOE_CONST_INTERNAL::CST_IUC_UNLOAD_TRANSPORT:
		return 5;
	case AOE_CONST_INTERNAL::CST_IUC_REGROUP:
		return 7;
	case AOE_CONST_INTERNAL::CST_IUC_UNGROUP:
		return 8;
	case AOE_CONST_INTERNAL::CST_IUC_FORMATION:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_CANCEL_OR_BACK:
		return 0xA; // To confirm it is a constant. Cf 482526.
	case AOE_CONST_INTERNAL::CST_IUC_NEXT_PAGE:
		return 0xB; // To confirm it is a constant. Cf 4824F9.
	case AOE_CONST_INTERNAL::CST_IUC_CHAT:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_DIPLOMACY:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_MENU:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_TRADE_WITH:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_CANCEL_BUILD:
		return 3; // cf 0x482C30
	case AOE_CONST_INTERNAL::CST_IUC_SHOW_HELP:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_HOLD_POSITION:
		return 4; // cf 4827AD
	case AOE_CONST_INTERNAL::CST_IUC_ATTACK_POSITION:
		return 0xC; // cf 4827DD
	case AOE_CONST_INTERNAL::CST_IUC_SHOW_SCORES:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_TRADE_FOOD_FOR_GOLD:
		return 0x57; // cf 482AB7
	case AOE_CONST_INTERNAL::CST_IUC_TRADE_WOOD_FOR_GOLD:
		return 0x56;
	case AOE_CONST_INTERNAL::CST_IUC_TRADE_STONE_FOR_GOLD:
		return 0x58;
	case AOE_CONST_INTERNAL::CST_IUC_HEAL:
		return 0x0D; // cf 4828E9
	case AOE_CONST_INTERNAL::CST_IUC_CONVERT:
		return 0x0E;
	case AOE_CONST_INTERNAL::CST_IUC_ATTACK:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_REPAIR:
		return 0;
	case AOE_CONST_INTERNAL::CST_IUC_ADD_TO_QUEUE:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_DROP_FROM_QUEUE:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_DO_RESEARCH:
	case AOE_CONST_INTERNAL::CST_IUC_DO_TRAIN:
	case AOE_CONST_INTERNAL::CST_IUC_DO_BUILD:
		// Those ones do not have a specific icon id, it depends on unit/research...
	case AOE_CONST_INTERNAL::CST_IUC_CROR_DONT_ATTACK_VILLAGERS:
		return CST_CUSTOMROR_ICON_ID_DONT_ATTACK_VILLAGERS;
	case AOE_CONST_INTERNAL::CST_IUC_CROR_DONT_ATTACK_BUILDINGS:
		return CST_CUSTOMROR_ICON_ID_DONT_ATTACK_BUILDINGS;
	case AOE_CONST_INTERNAL::CST_IUC_CROR_NO_AUTO_ATTACK:
		return CST_CUSTOMROR_ICON_ID_DONT_AUTO_ATTACK;
	case AOE_CONST_INTERNAL::CST_IUC_CROR_RESET_AUTO_ATTACK:
		return CST_CUSTOMROR_ICON_ID_RESTORE_AUTO_ATTACK;
	default:
		break;
	}
	return -1; // unknown or not applicable
}

// Returns true if the button is visible. Use this overload for performance if you already have STRUCT_UI_IN_GAME_MAIN pointer.
// Returns false if the button is hidden, or if an error occurs.
bool IsInGameUnitCommandButtonVisible(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, long int buttonIndex) {
	assert(gameMainUI && gameMainUI->IsCheckSumValid());
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		return false;
	}
	if ((buttonIndex < 0) || (buttonIndex >= 12)) {
		assert(false); // Should never happen
		return false;
	}
	return (gameMainUI->unitCommandButtons[buttonIndex] != NULL) &&
		(gameMainUI->unitCommandButtons[buttonIndex]->visible != 0);
}
// Returns true if the button is visible
// Returns false if the button is hidden, or if an error occurs.
bool IsInGameUnitCommandButtonVisible(long int buttonIndex) {
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *inGameMain = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *) AOE_GetScreenFromName(gameScreenName);
	if (!inGameMain || !inGameMain->IsCheckSumValid() || !inGameMain->visible) {
		return false;
	}
	if (inGameMain != (void*)AOE_GetCurrentScreen()) {
		return false;
	}
	return IsInGameUnitCommandButtonVisible(inGameMain, buttonIndex);
}



// To be used with button IDs from unit defintion/researches to get a buttonIndex for game main UI structure (command buttons)
// WARNING: returns -1 if DATButtonId is 0 or negative (invalid)
// Valid results are 0-4 or 6-10.
// See also 0x483710 (only works for 2 pages)
long int GetButtonInternalIndexFromDatBtnId(char DATButtonId) {
	if (DATButtonId <= 0) { return -1; } // Invalid (including 0).
	long int tmp = ((DATButtonId - 1) % 10) + 1; // 1-5 is same as 11-15 / 21-25, etc. 6-10 = same as 16-20, etc
	if (tmp <= 5) {
		return tmp - 1; // Source 1-5 = index 0-4
	}
	if (tmp <= 10) {
		return tmp; // Source 6-10 = index 6-10
	}
	return -1; // Should never happen !
}


// Add a command button in unit-commands zone (under game zone).
// buttonIndex: 0-4 = first row, 6-10=second row, 5 and 11 are "special" right buttons (11=unselect/cancel, generally)
// DATID can be a unitDefId (train), researchId (do_research)... Set it to 0 when not relevant.
// isDisabled : set it to true to get a read only button (no possible click)
// creationText can be left NULL to display a text using unit/research/etc's LanguageDLLID.
// iconSlpInfo can be left NULL unless we want to use specific SLP resource for icon.
// Technically, this just updates the button (no button object is created).
// Refresh may NOT be performed if underlying info (command id, DATID) are unchanged, which may lead to wrong updates
bool AddInGameCommandButton(long int buttonIndex, AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID UICmdId,
	long int DATID, bool isDisabled, const char *creationText, AOE_STRUCTURES::STRUCT_SLP_INFO *iconSlpInfo,
	bool forceUpdate) {
	if (buttonIndex < 0) {
		return false;
	}
	if (!IsGameRunning()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
	if (!player || !player->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *inGameMain = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *) AOE_GetScreenFromName(gameScreenName);
	if (!inGameMain || !inGameMain->IsCheckSumValid()) { return false; }
	long int iconId = GuessIconIdFromUICommandId(UICmdId);
	long int helpDllId = 0;
	long int creationDllId = 0;
	long int creationDllId_original = 0; // unchanged creationDllId (without the +100000, for units)
	long int hotkeyDllId = 0;
	char pName[200];
	char disabledPrefix[50];
	char creationTextBuffer[0x200];
	pName[0] = 0;
	creationTextBuffer[0] = 0;
	disabledPrefix[0] = 0;
	std::string creationTextIfMissing = "";
	AOE_STRUCTURES::STRUCT_UI_UNIT_BUTTON_INFO btnInfoForMissingText;
	btnInfoForMissingText.DATID = -1; // not required here
	btnInfoForMissingText.iconId = 0; // not required here
	btnInfoForMissingText.languageDllHotkeyText = -1; // not required here
	btnInfoForMissingText.name = ""; // not required here

	if (creationText == NULL) {
		if (isDisabled) {
			if (UICmdId == INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH) {
				strcpy_s(disabledPrefix, localizationHandler.GetTranslation(CRLANG_ID_CANT_RESEARCH, "Can't be researched right now: "));
			} else if (UICmdId == INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN) {
				strcpy_s(disabledPrefix, localizationHandler.GetTranslation(CRLANG_ID_CANT_TRAIN, "Can't be trained right now: "));
			} else {
				//strcpy_s(disabledPrefix, "Disabled: ");
			}
		} else {
			AOE_ReadLanguageTextForCategory(INTERNAL_MAIN_CATEGORIES::CST_IMC_UI_COMMANDS, UICmdId, 0, creationTextBuffer, sizeof(creationTextBuffer));
		}
		creationTextIfMissing = std::string(creationTextBuffer) + std::string(" ");
		creationTextBuffer[0] = 0; // reset buffer
	}

	if (UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN) {
		AOE_STRUCTURES::STRUCT_UNITDEF_LIVING *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_LIVING *)GetUnitDefStruct(player, (short int)DATID);
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) {
			return false;
		}
		helpDllId = unitDef->languageDLLHelp;
		creationDllId_original = unitDef->languageDLLID_Creation;
		creationDllId = creationDllId_original + 100000; // cf 48248B
		hotkeyDllId = unitDef->hotKey;
		iconId = unitDef->iconId;
		GetLanguageDllText(unitDef->languageDLLID_Name, pName, sizeof(pName), "");
		if (creationText == NULL) {
			btnInfoForMissingText.languageDllHelp = helpDllId;
			btnInfoForMissingText.costAmount1 = unitDef->costs[0].costAmount;
			btnInfoForMissingText.costAmount2 = unitDef->costs[1].costAmount;
			btnInfoForMissingText.costAmount3 = unitDef->costs[2].costAmount;
			btnInfoForMissingText.costType1 = unitDef->costs[0].costType;
			btnInfoForMissingText.costType2 = unitDef->costs[1].costType;
			btnInfoForMissingText.costType3 = unitDef->costs[2].costType;
		}
	}
	if (UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH) {
		AOE_STRUCTURES::STRUCT_RESEARCH_DEF *researchDef = GetResearchDef(player, (short int)DATID);
		if (researchDef == NULL) { return false; }
		iconId = researchDef->iconId;
		creationDllId_original = researchDef->languageDLLDescription; // languageDLLCreation;
		creationDllId = creationDllId_original;
		helpDllId = researchDef->languageDLLHelp;
		GetLanguageDllText(researchDef->languageDLLName, pName, sizeof(pName), researchDef->researchName);
		btnInfoForMissingText.languageDllHelp = helpDllId;
		btnInfoForMissingText.costAmount1 = researchDef->costAmount1;
		btnInfoForMissingText.costAmount2 = researchDef->costAmount2;
		btnInfoForMissingText.costAmount3 = researchDef->costAmount3;
		btnInfoForMissingText.costType1 = researchDef->costType1;
		btnInfoForMissingText.costType2 = researchDef->costType2;
		btnInfoForMissingText.costType3 = researchDef->costType3;
	}
	// Train or research: if no text is available, try to build one
	if ((creationText == NULL) && ((UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN) ||
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH))) {
		// Note: some units have bad creationDllId, like some heroes.
		AOE_GetUIButtonCreationText(creationTextBuffer, &btnInfoForMissingText, UICmdId, creationDllId_original);
		creationTextIfMissing = disabledPrefix;
		if (creationTextBuffer[0] == 0) {
			creationTextIfMissing += pName; // Rock'n'roll build a custom string as dllId is invalid/missing
		} else {
			creationTextIfMissing += creationTextBuffer; // this is standard behaviour
		}
	}
	if (UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_NEXT_PAGE) { // cf 0x4824FF
		creationDllId = -1;
		helpDllId = LANG_ID_NEXT_PAGE;
		GetLanguageDllText(LANG_ID_NEXT_PAGE, pName, sizeof(pName), "Next page");
	}

	// Manage do_build too ?
	// TODO : for many commands, we need to set name/description ?
	if (iconId < 0) {
		traceMessageHandler.WriteMessageNoNotification("No icon for command button, using 0");
		iconId = 0;
	}
	
	if (!IsInGameUnitCommandButtonVisible(buttonIndex)) { // If this button was not used
		inGameMain->panelDisplayedButtonCount++; // Update number of used buttons. Not sure it has any impact
	}
	const char *creationTextToUse = (creationText == NULL) ? creationTextIfMissing.c_str() : creationText;
	if (forceUpdate) { // Dirty trick to force refresh
		AOE_STRUCTURES::STRUCT_UI_BUTTON_WITH_NUMBER *button = inGameMain->unitCommandButtons[buttonIndex];
		button->buttonInfoValue[0] = -1;
		button->commandIDs[0] = -1;
		button->hotkey = hotkeyDllId;
	}
	return AOE_InGameAddCommandButton(player, buttonIndex, iconId, UICmdId, DATID,
		helpDllId, creationDllId, hotkeyDllId,
		pName, creationTextToUse, isDisabled, iconSlpInfo);
}


// To customize for debug purpose...
void DebugDumpAllUnits() {
#ifndef _DEBUG
	return;
#endif
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) return;
	for (int i = 0; (i < GetGameGlobalStructPtr()->seqUnitId - 1) && (i < GetGameGlobalStructPtr()->unitPointersListSize); i++) {
		AOE_STRUCTURES::STRUCT_UNIT *unit = GetGameGlobalStructPtr()->ptrUnitPointersList[i];
		if (unit && unit->IsCheckSumValid() && unit->ptrStructDefUnit && unit->ptrStructDefUnit->IsCheckSumValid()) {
			char buf[200];
			char buf2[10];
			buf2[0] = '-';
			buf2[1] = 0;
			if ((unit->ptrStructDefUnit->unitType >= 70) && (unit->ptrStructDefUnit->unitType != 90)) {
				sprintf_s(buf2, "%d", unit->stillToBeDiscoveredByHuman);
			}
			sprintf_s(buf, "unit %ld\tType=%ld\tDATID=%ld\t1B8=%s\tname=%s\t%d\t%d\t%d\t%d\n", unit->unitInstanceId, unit->unitType,
				unit->ptrStructDefUnit->DAT_ID1, buf2, unit->ptrStructDefUnit->ptrUnitName,
				unit->unitStatus, unit->isNotCreatable, unit->isDoppleGanger, unit->unknown_04B);
			WriteToFile(buf, "D:\\AOEUnits.txt", true);
		}
	}
}


// For testing purpose
#ifdef _DEBUG
#define thisbufSize 20000
static char buffer[thisbufSize];
#endif
char *DumpPosToTextBuffer(AOE_STRUCTURES::STRUCT_TEMP_MAP_BUILD_LIKE_INFOS *mapInfosStruct, long int posX, long int posY, int radius) {
#ifdef _DEBUG
	if (((radius * 2 + 1) * (radius * 2 + 1) * 3) >= thisbufSize) { return "Buffer is not large enough for this radius."; }
	int pos = 0;
	for (long int i = -radius; i <= radius; i++) {
		for (long int j = -radius; j <= radius; j++) {
			unsigned char value = (unsigned char)mapInfosStruct->GetMapLikeValue(posX + i, posY + j);
			if (value == 0xFF) {
				sprintf_s(&buffer[pos], 4, "-1 ");
			} else {
				sprintf_s(&buffer[pos], 4, "%02X ", value);
			}
			pos += 3; // we will overwrite sprintf_s' ending \n
		}
		buffer[pos] = '\n';
		pos++;
	}
	buffer[pos] = '\n'; pos++; // Better with a (heavy) chariot return at end ;)
	buffer[pos] = '\0'; pos++;
	assert(pos < thisbufSize);
	return buffer;
#undef thisbufSize
#else
	return "";
#endif
}

#ifdef _DEBUG
bool debugSerialization = false;
// Write deserialization data into a buffer, then to a log file.
// This affects greatly performance !!! Debug only.
void WriteDebugLogForDeserializedData(unsigned long int callAddr, unsigned char *buffer, long int bufferSize) {
	static std::string test;
	static int writtenLines = 0;
	if (!buffer || (bufferSize <= 0)) {
		// Flush
		if (writtenLines > 0) {
			writtenLines = 0;
			WriteToFile(test, serializationLogFilename, true);
			test.clear();
		}
		return;
	}
	static long int totalBufferSize = 0; // Gives (next) offset
	static long int previousSerzOffset = 0;
	static long int previousSerzEndOffset = 0; // last "end position, =this begin position (unless object changed)
	long int serzOffset = *AOE_OFFSETS::AOE_CURRENT_OFFSET_IN_FILE_DESERIALIZATION;
	long int tmpEndOffset = serzOffset;
	serzOffset -= bufferSize;
	totalBufferSize += bufferSize;
	if (previousSerzOffset + bufferSize > 0x10000) {
		//test += GetHexStringAddress(totalBufferSize - bufferSize);
		//test += " (reload buffer)\n";
		serzOffset = 0;
		previousSerzEndOffset = previousSerzOffset + bufferSize - 0x10000;
		previousSerzOffset = serzOffset;
	} else {
bool objectChanged = (serzOffset <= previousSerzOffset);
if (objectChanged) {
	test += "*****\n"; // indicate we now deserialize another file (?)
	totalBufferSize = bufferSize;
}
previousSerzOffset = serzOffset;
previousSerzEndOffset = tmpEndOffset;
	}


	test += "{";
	test += GetHexStringAddress(callAddr);
	test += "}\t";
	test += "[";
	test += GetHexStringAddress((unsigned long int)buffer);
	test += "]\t[+";
	//test += GetHexStringAddress(serzOffset + totalOffsetCountingResets, 5);
	test += GetHexStringAddress(totalBufferSize - bufferSize, 5);
	test += "]\tsz=";
	test += to_string(bufferSize);
	test += "\t";
	for (int i = 0; i < bufferSize; i++) {
		test += GetHexStringAddress(buffer[i], 2);
		test += " ";
	}
	test += "\t\"";
	for (int i = 0; i < bufferSize; i++) {
		char c = (char)buffer[i];
		if ((c < 32) || (c > 126)) { c = ' '; }
		test += c;
	}
	test += "\"";
	if (bufferSize == 4) {
		float f = 0;
		long int *fakePtr = (long int*)&f;
		*fakePtr = *((long int*)buffer);
		test += "\tfloat=";
		test += std::to_string(f);
	}
	test += "\n";
	writtenLines++;
	if (writtenLines >= 500) {
		writtenLines = 0;
		WriteToFile(test, serializationLogFilename, true);
		test.clear();
	}
}
#endif

// Automatically detects issues in empires.dat (requires to have been loaded already) and writes logs about errors in trace message handler.
bool AnalyzeEmpiresDatQuality() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return false; }
	if (!global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid()) {
		traceMessageHandler.WriteMessage("ERROR: missing or invalid techInfo structure");
	}
	if (!global->researchDefInfo) {
		traceMessageHandler.WriteMessage("ERROR: missing or invalid researchInfo structure");
	}
	std::string msg;
	int civResCount = -1;
	int civUnitDefCount = -1;
	std::set<short int> techTreeIds; // stores all technology tree IDs
	// CivDef
#if 1
	for (int civId = 0; civId < global->civCount; civId++) {
		if (civResCount == -1) {
			civResCount = global->civilizationDefinitions[civId]->civResourcesCount;
		}
		if (global->civilizationDefinitions[civId]->civResourcesCount != civResCount) {
			msg = "Civilization ";
			msg += std::to_string(civId);
			msg += " has not the same resources count (";
			msg += std::to_string(global->civilizationDefinitions[civId]->civResourcesCount);
			msg += ") as previous civilizations (";
			msg += std::to_string(civResCount);
			msg += ")";
			traceMessageHandler.WriteMessage(msg);
		}
		if (civUnitDefCount == -1) {
			civUnitDefCount = global->civilizationDefinitions[civId]->civUnitDefCount;
		}
		if (global->civilizationDefinitions[civId]->civUnitDefCount != civUnitDefCount) {
			msg = "Civilization ";
			msg += std::to_string(civId);
			msg += " has not the same unit definitions count (";
			msg += std::to_string(global->civilizationDefinitions[civId]->civUnitDefCount);
			msg += ") as previous civilizations (";
			msg += std::to_string(civUnitDefCount);
			msg += "). This can provoke crashes.";
			traceMessageHandler.WriteMessage(msg);
		}
		if (global->civilizationDefinitions[civId]->techTreeId >= global->technologiesInfo->technologyCount) {
			msg = "Civilization ";
			msg += std::to_string(civId);
			msg += " has an invalid tech tree ID (";
			msg += std::to_string(global->civilizationDefinitions[civId]->techTreeId);
			msg += ")";
			traceMessageHandler.WriteMessage(msg);
		}
		if (global->civilizationDefinitions[civId]->techTreeId < 0) {
			msg = "Civilization ";
			msg += std::to_string(civId);
			msg += " does not have a tech tree (tech tree id=-1). This is not an error and is acceptable.";
			traceMessageHandler.WriteMessage(msg);
		} else {
			techTreeIds.insert(global->civilizationDefinitions[civId]->techTreeId);
		}
		if ((global->civilizationDefinitions[civId]->graphicSetId > 4) || (global->civilizationDefinitions[civId]->graphicSetId < 0)) {
			msg = "Civilization ";
			msg += std::to_string(civId);
			msg += " has an invalid graphic set ID (";
			msg += std::to_string(global->civilizationDefinitions[civId]->graphicSetId);
			msg += ")";
			traceMessageHandler.WriteMessage(msg);
		}
	}
#endif
	// Technologies
#if 1
	for (int techId = 0; techId < global->technologiesInfo->technologyCount; techId++) {
		AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = global->technologiesInfo->GetTechDef(techId);
		assert(techDef != NULL);
		if (techDef) {
			for (int effectId = 0; effectId < techDef->effectCount; effectId++) {
				if (!techDef->ptrEffects[effectId].HasValidEffect()) {
					msg = "Technology ";
					msg += std::to_string(techId);
					msg += "/effect #";
					msg += std::to_string(effectId);
					msg += " does not have an effective impact";
					traceMessageHandler.WriteMessage(msg);
				}
				if (techTreeIds.find(techId) == techTreeIds.end()) {
					if (techDef->ptrEffects[effectId].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) {
						msg = "Technology ";
						msg += std::to_string(techId);
						msg += "/effect #";
						msg += std::to_string(effectId);
						msg += " has 'set attribute' type, which should not be used. Try to use add or multiply instead";
						traceMessageHandler.WriteMessage(msg);
					}
					if ((techDef->ptrEffects[effectId].effectType == TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET) &&
						(techDef->ptrEffects[effectId].effectClass == 0)) { // mode = 0 = set
						msg = "Technology ";
						msg += std::to_string(techId);
						msg += "/effect #";
						msg += std::to_string(effectId);
						msg += " has 'set resource' type, which should not be used. Try to use add or multiply instead";
						traceMessageHandler.WriteMessage(msg);
					}
				}
				// TODO: Negative tech effects should be avoided?
			}
			
		}
	}
#endif
	// Researches
#if 1
	for (int researchId = 0; researchId < global->researchDefInfo->researchCount; researchId++) {
		int remainingRequirementsCount = global->researchDefInfo->researchDefArray[researchId].minRequiredResearchesCount;
		if ((remainingRequirementsCount < 0) || (remainingRequirementsCount > 4)) {
			msg = "Research ";
			msg += std::to_string(researchId);
			msg += " has an invalid requirements count (";
			msg += std::to_string(remainingRequirementsCount);
			msg += ")";
			traceMessageHandler.WriteMessage(msg);
			remainingRequirementsCount = 0;
		}
		int validReqResearchCount = 0;
		for (int index = 0; index < 4; index++) {
			short int reqResearchId = global->researchDefInfo->researchDefArray[researchId].requiredResearchId[index];
			if (reqResearchId >= 0) {
				validReqResearchCount++; // found a valid required research id
				if (reqResearchId >= global->researchDefInfo->researchCount) {
					msg = "Research ";
					msg += std::to_string(researchId);
					msg += " has an invalid required research (id=";
					msg += std::to_string(reqResearchId);
					msg += ")";
					traceMessageHandler.WriteMessage(msg);
				}
			}
		}
		if (validReqResearchCount > remainingRequirementsCount) {
			msg = "Research ";
			msg += std::to_string(researchId);
			msg += " has optional requirements (more required researches than minRequiredResearch). This should be avoided (used in standard game for 'age' researches). You can set required research IDs to -1 to disable them.";
			traceMessageHandler.WriteMessage(msg);
		}
		if (global->researchDefInfo->researchDefArray[researchId].researchLocation >= 0) {
			// If research is a shadow research, error
			if ((global->researchDefInfo->researchDefArray[researchId].researchTime <= 0) ||
				(global->researchDefInfo->researchDefArray[researchId].buttonId <= 0)) {
				msg = "Research ";
				msg += std::to_string(researchId);
				msg += " is a shadow research but has a research location. If this really is a shadow research, you should set research location to -1.";
				traceMessageHandler.WriteMessage(msg);
			}
		}
	}
#endif
	// Units
#if 1
	std::set<short int> badBlastLevelUnits;
	for (int civId = 0; civId < global->civCount; civId++) {
		for (int unitDefId = 0; unitDefId < global->civilizationDefinitions[civId]->civUnitDefCount; unitDefId++) {
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = global->civilizationDefinitions[civId]->GetUnitDefBase(unitDefId);
			if (unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass()) {
				if (unitDefBase->unitAIType < 0) {
					msg = "UnitDef ";
					msg += std::to_string(unitDefId);
					msg += " is missing an AI type (class = -1)";
					traceMessageHandler.WriteMessage(msg);
				}
			}
			if (unitDefBase && unitDefBase->DerivesFromType50()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_TYPE50 *type50 = (AOE_STRUCTURES::STRUCT_UNITDEF_TYPE50 *)unitDefBase;
				if ((type50->blastLevel != CST_BL_DAMAGE_TARGET_ONLY) && (type50->blastRadius == 0)) {
					badBlastLevelUnits.insert(unitDefId);
					if (false) { // generates too many errors
						msg = "UnitDef ";
						msg += std::to_string(unitDefId);
						msg += " has an inactive blast level set (";
						msg += std::to_string((int)type50->blastLevel);
						msg += ") because blast radius is 0 (no blast damage). You should set blast level to 3 when no blast damage is used.";
						traceMessageHandler.WriteMessage(msg);
					}
				}
			}
		}
	}
	if (badBlastLevelUnits.size() > 0) {
		msg = "Those units have an inactive blast level set because blast radius is 0 (no blast damage). You should set blast level to 3 when no blast damage is used. ";
		for each (short int unitDefId in badBlastLevelUnits)
		{
			msg += std::to_string(unitDefId);
			msg += ", ";
		}
		traceMessageHandler.WriteMessage(msg);
	}
	
#endif
	// Search for techtree-unavailable researches
	for (int civId = 0; civId < global->civCount; civId++)
	{
		DetectDatImpossibleResearches(global, civId);
	}

	return true;
}
