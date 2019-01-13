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
	this->rnrUIHelper = new ROCKNROR::UI::RnrUIHelper();

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
	if (this->rnrUIHelper) {
		delete this->rnrUIHelper;
		this->rnrUIHelper = NULL;
	}
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
	this->customGameMenuOptionsBtnVar = NULL;
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
// civId is only used if no conversion resistance is configured for unit, or if "improved conversion resistance mode" is disabled.
float RockNRorInfo::GetConversionResistance(char civId, AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
	assert(unitDef != NULL);
	if (unitDef == NULL) { return 1.f; }
	if (this->configInfo.conversionResistanceAttackClassEnabled) {
		// "Improved conversion resistance mode" is active : get it from empires.dat (entirely configurable)
		if (!unitDef->DerivesFromAttackable()) {
			return 1.f;
		}
		AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE * unitDefAtk = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE*)unitDef;
		for (int i = 0; i < unitDefAtk->armorsCount; i++) {
			if (unitDefAtk->ptrArmorsList[i].classId == this->configInfo.conversionResistanceAttackClass) {
				// empires.dat contains a conversion resistance value for that unit.
				// We store conversion resistance as a percentage (that can be more than 100), because armors are (short) ints
				float value = ((float)(unitDefAtk->ptrArmorsList[i].amount)) / 100;
				if (value < 0) { value = 0; } // 0 means immediate conversion
				return value;
			}
		}
		GLOBAL_UNIT_AI_TYPES unitClass = unitDefAtk->unitAIType;
		// No conversion resistance value is configured for this unit. Apply GAME defaults ; start with special cases
		if (civId == CST_CIVID_MACEDONIAN) {
			return 4.f;
		}
		if (
			(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupFishingBoat) || // TribeAIGroupFishingBoat
			(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTradeBoat) || // trade boat
			(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTransportBoat) || // TribeAIGroupTransportBoat
			(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupWarBoat)) // TribeAIGroupWarBoat including fire galley, juggernaught
		{
			return 2.f;
		}
		if (unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) { // TribeAIGroupPriest
			return 2.f;
		}
		if (
			(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot) || // Chariot
			(unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher) // Chariot archer
			) {
			return 8.f;
		}
		return 1.f; // No configuration and unit is a standard one = standard resistance
	} else {
		return this->GetConversionResistance_hardcodedWithConf(civId, unitDef->unitAIType);
	}
}


// Function to calculate conversion resistance for a giver unit from a given civ.
// This replaces game's algorithm.
// Use civId=0 (same as "gaia" civ) to exclude any civ bonus/malus from calculation
float RockNRorInfo::GetConversionResistance_hardcodedWithConf(char civId, short int unitClass) {
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
	return this->rnrUIHelper->GetCurrentRnrScreen() != NULL;
}


// Get main selected unit, or NULL if none is selected.
// Works in-game and in editor.
// See SelectOneUnit (in playerHandling) for unit selection + AOE_selectUnit, AOE_clearSelectedUnits
AOE_STRUCTURES::STRUCT_UNIT_BASE *RockNRorInfo::GetMainSelectedUnit(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	return this->GetRelevantMainSelectedUnitPointer(player);
}


// Get relevant "main selected unit" pointer according to game EXE status (using custom memory or not ?)
AOE_STRUCTURES::STRUCT_UNIT_BASE *RockNRorInfo::GetRelevantMainSelectedUnitPointer(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player) { return NULL; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *selectedUnit;
	if (this->hasCustomSelectedUnitsMemory) {
		selectedUnit = player->custom_mainSelectedUnit;
	} else {
		selectedUnit = player->mainSelectedUnit;
	}
	return selectedUnit;
}


// Get relevant "selected units" array pointer according to game EXE status (using custom memory or not ?)
// Please use this instead of playerStruct->selectedUnitsTable
AOE_STRUCTURES::STRUCT_UNIT_BASE **RockNRorInfo::GetRelevantSelectedUnitsPointer(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	assert(player != NULL);
	if (!player) { return NULL; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits;
	if (this->hasCustomSelectedUnitsMemory) {
		selectedUnits = player->custom_selectedUnits;
	} else {
		selectedUnits = player->selectedUnitsTable;
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


