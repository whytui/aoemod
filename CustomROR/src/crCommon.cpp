#include "../include/crCommon.h"


// Calculate distance (without optimization)
float GetDistance(float x1, float y1, float x2, float y2) {
	float dx = x1 - x2;
	float dy = y1 - y2;
	dx = dx * dx;
	dy = dy * dy;
	return sqrt(dx + dy);
}



// Pause/unpause the game
void SetGamePause(bool pauseOn) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (settings == NULL) { return; }
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) { return; } // Call should be robust enough, but we still check this
	long int argPause = pauseOn ? 1 : 0;
	_asm {
		PUSH 0; // arg2 - what is this ?
		PUSH argPause;
		MOV EAX, 0x0419A60;
		MOV ECX, settings;
		CALL EAX;
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


bool GetUnitCost(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID, float costTable[]) {
	if (!player || !player->IsCheckSumValid()) { return false; }
	if ((DAT_ID < 0) || (DAT_ID >= player->structDefUnitArraySize)) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *defUnit = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE*)player->GetUnitDefBase(DAT_ID);
	if (!defUnit || !defUnit->DerivesFromTrainable()) { return false; }
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


// Returns true for maps where AI does build a dock and boats. Warning: for unknown map type (custom), this returns true.
bool IsDockRelevantForMap(MAP_TYPE_INDEX mti) {
	MAP_WATER_TYPE m = GetMapWaterType(mti);
	return (m != MAP_WATER_TYPE::MWT_ALL_LAND_MAP) && (m != MAP_WATER_TYPE::MWT_MOSTLY_LAND_MAP);
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
	elem->attackAttempts = 0;
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
	if (!infAI || !infAI->IsCheckSumValid() || !elem || !infAI->ptrMainAI) { return false; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(elem->unitId);
	assert((unitBase == NULL) || (unitBase->IsCheckSumValidForAUnitClass()));
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = NULL;
	long int actorPlayerId = infAI->commonAIObject.playerId;
	long int unitPlayerId = -1;
	if (unitBase) {
		unitDefBase = unitBase->unitDefinition;
		assert(unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass());
		if (unitBase->ptrStructPlayer && unitBase->ptrStructPlayer->IsCheckSumValid()) {
			unitPlayerId = unitBase->ptrStructPlayer->playerId;
		}
	}
	bool unitItVisible = false;
	if (unitBase && unitDefBase) {
		if (unitDefBase->visibleInFog) {
			// The unit still exists, and is visible through fog: only remove it from the list if its position is not EXPLORED.
			unitItVisible = PLAYER::IsExploredForPlayer(infAI->ptrMainAI->player, (long int)unitBase->positionX, (long int)unitBase->positionY);
		} else {
			// Standard case, check fog visibility.
			unitItVisible = PLAYER::IsFogVisibleForPlayer(infAI->ptrMainAI->player, (long int)unitBase->positionX, (long int)unitBase->positionY);
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
		if (unitDefBase && unitDefBase->DerivesFromAttackable()) {
			// By the way... This part is not really necessary.
			AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDef50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDefBase;
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


// Find a unitElem in infAI list, returns NULL if not found. Similar to 0x4BD710.
AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *FindInfAIUnitElemInList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int unitId) {
	if (!infAI || !infAI->IsCheckSumValid() || (unitId < 0)) { return NULL; }
	for (int i = 0; i < infAI->unitElemListSize; i++) {
		if (infAI->unitElemList[i].unitId == unitId) {
			return &infAI->unitElemList[i];
		}
	}
	return NULL;
}


// Add a unit in infAI elem list, or update if existing. Warning: this uses unit structure's info, even if it is not visible !
// Please check for visibility to avoid "cheating"
// Returns true if successful
bool AddUpdateInfAIElemList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
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


// Change unit owner in InfAI unitListElem according to unit visibility
// Return true if updated.
bool UpdateUnitOwnerInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit,
	long int newPlayerId) {
	if (!infAI || !infAI->IsCheckSumValid() || !infAI->ptrMainAI) { return false; }
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	if (newPlayerId < 0) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unit->unitDefinition;
	if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) { return false; }
	if (unit->unitInstanceId < 0) { return false; } // Ignore temp units.
	bool isVisible = false;
	if (unitDefBase->visibleInFog) {
		isVisible = PLAYER::IsExploredForPlayer(infAI->ptrMainAI->player, (long int)unit->positionX, (long int)unit->positionY);
	} else {
		isVisible = PLAYER::IsFogVisibleForPlayer(infAI->ptrMainAI->player, (long int)unit->positionX, (long int)unit->positionY);
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
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
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
void AOE_playerBldHeader_RemoveBldFromArrays(AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	if (buildingsHeader && unit && unit->IsCheckSumValidForAUnitClass()) {
		_asm {
			MOV ECX, buildingsHeader
			MOV EDX, unit
			PUSH EDX
			MOV EAX, 0x00436980 // playerUnitsHeader.removeBuildingFromArrays(ptrUnit)
			CALL EAX
		}
	}
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
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
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
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	unsigned long int addr = 0x4834F0;
	_asm {
		PUSH elemLanguageCreationDllId;
		PUSH uiCmdId;
		PUSH unitButtonInfo;
		PUSH buffer;
		CALL addr;
	}
}



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
		STRUCT_RESEARCH_DEF *resDef = global->researchDefInfo->GetResearchDef(researchId);
		if (!resDef) {
			msg = "ERROR: Research #";
			msg += std::to_string(researchId);
			msg += " is NULL";
			traceMessageHandler.WriteMessage(msg);
			continue;
		}
		int remainingRequirementsCount = resDef->minRequiredResearchesCount;
		if ((remainingRequirementsCount < 0) || (remainingRequirementsCount > 4)) {
			msg = "Research #";
			msg += std::to_string(researchId);
			msg += " has an invalid requirements count (";
			msg += std::to_string(remainingRequirementsCount);
			msg += ")";
			traceMessageHandler.WriteMessage(msg);
			remainingRequirementsCount = 0;
		}
		int validReqResearchCount = 0;
		for (int index = 0; index < 4; index++) {
			short int reqResearchId = resDef->requiredResearchId[index];
			if (reqResearchId >= 0) {
				validReqResearchCount++; // found a valid required research id
				if (reqResearchId >= global->researchDefInfo->researchCount) {
					msg = "Research #";
					msg += std::to_string(researchId);
					msg += " has an invalid required research (id=";
					msg += std::to_string(reqResearchId);
					msg += ")";
					traceMessageHandler.WriteMessage(msg);
				}
			}
		}
		if (validReqResearchCount > remainingRequirementsCount) {
			msg = "Research #";
			msg += std::to_string(researchId);
			msg += " has optional requirements (more required researches than minRequiredResearch). This should be avoided (used in standard game for 'age' researches). You can set required research IDs to -1 to disable them.";
			traceMessageHandler.WriteMessage(msg);
		}
		bool hasDuplicateRequirement = false;
		for (int index = 0; index < 4; index++) {
			short int reqResearchId = resDef->requiredResearchId[index];
			if (reqResearchId >= 0) {
				for (int index2 = index + 1; index2 < 4; index2++) {
					if (reqResearchId == resDef->requiredResearchId[index2]) {
						hasDuplicateRequirement = true;
					}
				}
			}
		}
		if (hasDuplicateRequirement) {
			msg = "Research #";
			msg += std::to_string(researchId);
			msg += " has duplicate requirements (other than -1).";
			traceMessageHandler.WriteMessage(msg);
		}
		if (resDef->researchLocation >= 0) {
			// If research is a shadow research, error
			if ((resDef->researchTime <= 0) || (resDef->buttonId <= 0)) {
				msg = "Research #";
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
	std::set<pair<short int, short int>> unitDefCommandAlreadyReported;
	for (int civId = 0; civId < global->civCount; civId++) {
		for (int unitDefId = 0; unitDefId < global->civilizationDefinitions[civId]->civUnitDefCount; unitDefId++) {
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = global->civilizationDefinitions[civId]->GetUnitDef(unitDefId);
			if (unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass()) {
				if (unitDefBase->unitAIType < 0) {
					msg = "UnitDef ";
					msg += std::to_string(unitDefId);
					msg += " is missing an AI type (class = -1)";
					traceMessageHandler.WriteMessage(msg);
				}
			}
			// Blast type / level consistency (tons of errors)
			if (unitDefBase && unitDefBase->DerivesFromAttackable()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *type50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDefBase;
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
			// Gather Commands using unitId instead of class (miners) => adding custom stone/gold mines wouldn't work
			if (unitDefBase && unitDefBase->DerivesFromCommandable()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *commandable = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)unitDefBase;
				if (commandable->ptrUnitCommandHeader && commandable->ptrUnitCommandHeader->IsCheckSumValid()) {
					for (int i = 0; i < commandable->ptrUnitCommandHeader->commandCount; i++) {
						if ((commandable->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_GATHER_NO_ATTACK) ||
							(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_GATHER_NO_ATTACK)) {
							if ((commandable->ptrUnitCommandHeader->ptrCommandArray[i]->classId == -1) && 
								(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitDefId >= 0) &&
								(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitDefId != CST_UNITID_FARM) // Allow farm because they don't have a dedicated AI type (class)
								) {
								pair<short int, short int> p;
								p.first = unitDefId;
								p.second = i;
								if (unitDefCommandAlreadyReported.find(p) == unitDefCommandAlreadyReported.end()) {
									msg = "UnitDef ";
									msg += std::to_string(unitDefId);
									msg += ": command #";
									msg += std::to_string(i);
									msg += " should not use unitDefId (";
									msg += std::to_string(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitDefId);
									msg += "). Please use unit class instead (civ #";
									msg += std::to_string(civId);
									msg += " - the message won't be repeated for other civs)";
									traceMessageHandler.WriteMessage(msg);
									unitDefCommandAlreadyReported.insert(p);
								}
							}
						}
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
