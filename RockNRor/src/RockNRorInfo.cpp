#include "../include/RockNRorInfo.h"

static char _intDistanceValues[CST_INT_DISTANCE_VALUES_MAX_COORDINATE + 1][CST_INT_DISTANCE_VALUES_MAX_COORDINATE + 1] = { '\0' };


// Global static objects
CONFIG::RockNRorConfig RockNRorInfo::configInfo;
RockNRorInfo ROCKNROR::crInfo;



RockNRorInfo::RockNRorInfo() {
	this->triggerSet = NULL;
	this->hasFixForBuildingStratElemUnitId = false;
	this->hasCustomSelectedUnitsMemory = false;
	this->hasManageAIFeatureON = false;
	this->hasRemovePlayerInitialAgeInScenarioInit = false;
	this->ResetVariables();

	// empires.dat variables are NOT game-dependent: do not initialize them in ResetVariables().
	this->empiresDatCivCount = 0;
	this->empiresDatTechDefCount = 0;

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
char RockNRorInfo::GetIntDistance(char diffX, char diffY) {
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

RockNRorInfo::~RockNRorInfo() {
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
void RockNRorInfo::ResetVariables() {
	if (this->triggerSet) {
		this->triggerSet->Reset();
		delete this->triggerSet;
	}
	this->triggerSet = NULL;
	this->triggersLastCheckTime_s = 0;
	this->lastRockNRorTimeExecution_gameTime_s = 0;
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
void RockNRorInfo::FillIDVectorFromString(vector<short int> &v, long int playerId, const char *text) {
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
void RockNRorInfo::FillUnitDefToDisableFromString(long int playerId, const char *text) {
	if (!text || (*text == 0)) { return; }
	if ((playerId < 0) || (playerId > 8)) { return; }
	this->FillIDVectorFromString(this->unitDefToDisable[playerId], playerId, text);
}
// Resets and fills the list of researchID to disable for a player from a comma-separated list.
void RockNRorInfo::FillResearchesToDisableFromString(long int playerId, const char *text) {
	if (!text || (*text == 0)) { return; }
	if ((playerId < 0) || (playerId > 8)) { return; }
	this->FillIDVectorFromString(this->researchesToDisable[playerId], playerId, text);
}


// Function to calculate conversion resistance for a giver unit from a given civ.
// This replaces game's algorithm.
// Use civId=0 (same as "gaia" civ) to exclude any civ bonus/malus from calculation
float RockNRorInfo::GetConversionResistance(char civId, short int unitClass) {
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
void RockNRorInfo::ApplyAutoAttackPolicyToPlayerSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player,
	const AutoAttackPolicy &autoAttackPolicyValues, const AutoAttackPolicy &flagsToApply) {
	if (!flagsToApply.attackMilitary &&
		!flagsToApply.attackNonTowerBuildings &&
		!flagsToApply.attackTowers &&
		!flagsToApply.attackVillagers &&
		!flagsToApply.attackWalls) { return; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = this->GetRelevantSelectedUnitsPointer(player);
	assert(selectedUnits != NULL);
	for (int i = 0; i < player->selectedUnitCount; i++) {
		if (selectedUnits[i] && selectedUnits[i]->IsCheckSumValidForAUnitClass() &&
			(selectedUnits[i]->ptrStructPlayer == player)) { // Make sure selected unit is mine !
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *currentUnitDefBase = selectedUnits[i]->unitDefinition;
			if (IsTower(currentUnitDefBase) || IsNonTowerMilitaryUnit(currentUnitDefBase->unitAIType)) {
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
bool RockNRorInfo::HasOpenedCustomGamePopup() {
	return (this->customGamePopupButtonVar != NULL);
}

// Returns true if a custom dialog is opened (only for dialogs, not popups !)
bool RockNRorInfo::HasOpenedCustomDialog() {
	return (this->customYesNoDialogVar != NULL);
}



// Opens a custom popup window in game screen/editor. The created popup window only contains a "OK" button.
// You have to add UI elements afterwards (use GetCustomGamePopup() to get parent object=popup).
// Return popup UI object if OK, NULL if failed
// Fails if another game popup (including options) is already open. Fails if dimensions are too small.
// Pauses the game if running (only if a popup is successfully opened)
// Technically, the created (AOE) popup object is based on game options popup.
// themeSlpId is a "bina" slpid from interfac.drs with references to colors and slpids to use for buttons, etc. Basically 50051 to 50061.
AOE_STRUCTURES::STRUCT_UI_EASY_PANEL *RockNRorInfo::OpenCustomGamePopup(long int hSize, long int vSize, bool hasCancelBtn, long int themeSlpId) {
	if (this->HasOpenedCustomGamePopup()) { return false; }
	if ((hSize < 0xB0) || (vSize < 30)) { return false; }
	if (!ROR_gameSettings) { return false; }

	if ((GetGameSettingsPtr()->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) &&
		(GetGameSettingsPtr()->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING)) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_UI_EASY_PANEL *currentScreen = (AOE_STRUCTURES::STRUCT_UI_EASY_PANEL *)GetCurrentUIStruct();
	assert(currentScreen && currentScreen->IsCheckSumValidForAChildClass());
	if (!currentScreen || !currentScreen->IsCheckSumValidForAChildClass()) { return false; }
	// Make sure provided dimensions fit in screen
	if (currentScreen->sizeX < hSize) { hSize = currentScreen->sizeX - 10; }
	if (currentScreen->sizeY < vSize) { vSize = currentScreen->sizeY - 10; }

	AOE_METHODS::SetGamePause(true);
	if (themeSlpId < 0) {
		themeSlpId = currentScreen->themeSlpId;
	}
	this->customGamePopupVar = AOE_METHODS::CreateGameScreenPopup(currentScreen, hSize, vSize, themeSlpId);

	// OK button. It is this->customGamePopupButtonVar that identifies the popup (+ the fact it is open)
	long int btnhPos = (hSize - 0xAC) / 2;
	long int btnvPos = vSize - 38;
	long int btnHSize = 0xAC; // 172
	if (hasCancelBtn) {
		btnhPos = (hSize - 0xAC - 20) / 2;
		btnHSize = 0x50;
	}

	AOE_METHODS::UI_BASE::AddButton(this->customGamePopupVar, &this->customGamePopupButtonVar, LANG_ID_OK, btnhPos, btnvPos, btnHSize, 30);
	if (hasCancelBtn) {
		AOE_METHODS::UI_BASE::AddButton(this->customGamePopupVar, &this->customGamePopupCancelBtnVar, LANG_ID_CANCEL, btnhPos + btnHSize + 32, btnvPos, btnHSize, 30);
	}
	assert(this->objectsToFree.empty()); // there shouldn't be remaining components from a previous popup.

	return this->customGamePopupVar;
}


// Use it to list all UI components (labels, buttons...) that are created(added) to popup content, so they are automatically freed when popup is closed.
// The method is protected against duplicates, you can safely call it more than once.
// Returns true if obj has actually been added to list.
bool RockNRorInfo::AddObjectInPopupContentList(AOE_STRUCTURES::STRUCT_ANY_UI *obj) {
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
void RockNRorInfo::FreePopupAddedObjects() {
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
void RockNRorInfo::FreeGarbagePopupComponents() {
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
bool RockNRorInfo::ForceSetCurrentGamePopup(AOE_STRUCTURES::STRUCT_UI_EASY_PANEL *customGamePopup, AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK, AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCancel) {
	if (this->HasOpenedCustomGamePopup()) { return false; }
	this->customGamePopupVar = customGamePopup;
	this->customGamePopupButtonVar = btnOK;
	this->customGamePopupCancelBtnVar = btnCancel;
	return true;
}


// To be called when game menu is closed to free custom button
void RockNRorInfo::ForceClearCustomMenuObjects() {
	this->FreePopupAddedObjects();
}


// Closes currently opened custom popup window in game screen.
void RockNRorInfo::CloseCustomGamePopup() {
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
		AOE_METHODS::CloseScreenFullTreatment(popupUIObj);
	}
	AOE_METHODS::SetGamePause(false);
}


// Return true if provided memory address is our custom game popup (excluding custom options)
bool RockNRorInfo::IsCustomGamePopupOKButton(unsigned long int UIObjectAddress) {
	return UIObjectAddress == (unsigned long)this->customGamePopupButtonVar;
}

// Return true if provided memory address is our custom game popup Cancel button (excluding custom options)
bool RockNRorInfo::IsCustomGamePopupCancelButton(unsigned long int UIObjectAddress) {
	return UIObjectAddress == (unsigned long)this->customGamePopupCancelBtnVar;
}


// Returns custom popup window in game screen (excluding RockNRor options popup).
// Returns NULL if this popup is not open.
AOE_STRUCTURES::STRUCT_UI_EASY_PANEL *RockNRorInfo::GetCustomGamePopup() {
	return this->customGamePopupVar;
}


// Get main (first) selected unit, or NULL if none is selected.
// Works in-game and in editor.
// See SelectOneUnit (in playerHandling) for unit selection + AOE_selectUnit, AOE_clearSelectedUnits
AOE_STRUCTURES::STRUCT_UNIT_BASE *RockNRorInfo::GetMainSelectedUnit(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	assert(player != NULL);
	if (!player) { return NULL; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = this->GetRelevantSelectedUnitsPointer(player);
	if (!selectedUnits) {
		return NULL;
	}
	return *selectedUnits; // not sure it always corresponds to selected unit in bottom left panel ? But this works in editor too
}


// Get relevant "selected units" array pointer according to game EXE status (using custom memory or not ?)
// Please use this instead of playerStruct->selectedStructUnitTable
AOE_STRUCTURES::STRUCT_UNIT_BASE **RockNRorInfo::GetRelevantSelectedUnitsPointer(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	assert(player != NULL);
	if (!player) { return NULL; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits;
	if (this->hasCustomSelectedUnitsMemory) {
		selectedUnits = player->custom_selectedUnits;
	} else {
		selectedUnits = player->selectedStructUnitTable;
	}
	assert(selectedUnits != NULL);
	return selectedUnits;
}



// TEMPORARY
#ifdef _DEBUG
static bool _DEBUG_improveThisPlayer(int playerId) {
	bool applyFilter = false; // TRUE = DEBUG, false = normal

	static bool msgDone = false;
	if (!msgDone) {
		msgDone = true;
		if (applyFilter)
			traceMessageHandler.WriteMessage("Using DEBUG AI improvement filter");
	}
	if (applyFilter) {
		// Filter clause (DEBUG)
		return playerId >= 5;
	}

	// Normal: no filter
	return true;
}
#endif



// Use this API to get "do we improve AI" information. This takes care of RockNRor configuration + possible custom rules.
// Remark: AI can be currently disabled for a player, but "improved AI" will still remain true.
// Useful to add custom rules for debugging
bool ROCKNROR::IsImproveAIEnabled(int playerId) {
	bool useImprovement = (ROCKNROR::crInfo.configInfo.improveAILevel > 0);
	if (ROCKNROR::crInfo.configInfo.enableTestCompareAI) {
		useImprovement &= ((playerId & 1) == 1);
	}
#ifdef _DEBUG
	useImprovement &= _DEBUG_improveThisPlayer(playerId);
#endif
	return useImprovement;
}


// Returns true if RPG mode is active in current game
bool ROCKNROR::IsRpgModeEnabled() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	if (!settings->rgeGameOptions.isSinglePlayer) { return false; } // Disable in MP
	// Note: to work well on saved games, this requires RockNRor's fix on game's IsScenario information.
	bool isScenario = (settings->isCampaign || settings->rgeGameOptions.isScenario);
	if (isScenario) {
		return ROCKNROR::crInfo.configInfo.enableRPGModeInScenario;
	} else {
		return ROCKNROR::crInfo.configInfo.enableRPGModeInRandomGames;
	}
}


// Returns true if we want to use ROR's method to handle panic mode (not recommended): not optimized, and not very good
// Returns false if we want to disable completely ROR's method, and use RockNRor methods for panic mode instead (recommended)
bool ROCKNROR::ShouldUseOriginalPanicModeMethod() {
	return (ROCKNROR::crInfo.configInfo.improveAILevel == 0);
}


// Get a localized string to overload ROR's string localization system (language(x).dll)
// Returns true if a (custom) localized string has been written into buffer.
bool ROCKNROR::GetLocalizedString(long int stringId, char *buffer, long int bufferSize) {
	if (!buffer) { return false; }
	return localizationHandler.ReadTranslation((short int)stringId, buffer, bufferSize);
}


