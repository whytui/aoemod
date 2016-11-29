#include "../include/crCommon.h"


// Calculate distance
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


// Get a unit name from empires.dat data (read from civ 0)
// Returns NULL if not found. This requires that empires.dat file has already been read to global structure.
const char *GetUnitName(short int unitDefId) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return NULL; }
	if (global->civCount <= 0) { return NULL; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = global->civilizationDefinitions[0]->GetUnitDef(unitDefId);
	if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) { return NULL; }
	return unitDef->ptrUnitName;
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
void MoveUnitToTargetOrPosition(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *unitToMove, AOE_STRUCTURES::STRUCT_UNIT_BASE *target, float posX, float posY) {
	if (!unitToMove) { return; }

	if (IsMultiplayer()) {
		// Manage MP games : use a command (but unit will not defend itself if attacked)
		if (!unitToMove || !unitToMove->IsCheckSumValidForAUnitClass()) {
			return;
		}
		long int targetId = -1;
		if (target && target->IsCheckSumValidForAUnitClass()) { targetId = target->unitInstanceId; }
		GAME_COMMANDS::CreateCmd_RightClick(unitToMove->unitInstanceId, targetId, posX, posY);
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
bool MoveAndAttackTarget(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actor, AOE_STRUCTURES::STRUCT_UNIT_BASE *target) {
	if (!tacAI || !actor || !target) { return false; }
	assert(tacAI->IsCheckSumValid());
	assert(actor->IsCheckSumValidForAUnitClass() && actor->DerivesFromCommandable());
	assert(target->IsCheckSumValidForAUnitClass());
	assert(actor->unitDefinition);
	assert(target->ptrStructPlayer);
	if (!tacAI->IsCheckSumValid() || !actor->DerivesFromCommandable() || !target->IsCheckSumValidForAUnitClass() || 
		!actor->unitDefinition || !target->ptrStructPlayer) { return false; }
	long int posX = (long int)target->positionX;
	long int posY = (long int)target->positionY;
	long int posZ = (long int)target->positionZ;
	float maxRange = 1;
	if (actor->DerivesFromAttackable()) {
		maxRange = ((STRUCT_UNITDEF_ATTACKABLE*)actor->unitDefinition)->maxRange;
	}
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
bool TellUnitsToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE **actorUnitsList, long int actorUnitsCount,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *target) {
	if (!actorUnitsList || (actorUnitsCount <= 0) || !target || !target->IsCheckSumValidForAUnitClass()) { return false; }
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
	GAME_COMMANDS::CreateCmd_RightClick(actorUnitsList, actorUnitsCount, target->unitInstanceId, target->positionX, target->positionY);
	return true;
}


// Tell a unit to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actorUnit, AOE_STRUCTURES::STRUCT_UNIT_BASE *target) {
	if (!actorUnit || !actorUnit->DerivesFromCommandable()) { return false; }
	return TellUnitsToInteractWithTarget(&actorUnit, 1, target);
}


// Returns a unitDefCommand object if actor unit has a valid right-click command on target unit.
// Returns NULL if there no possible interaction
AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF *GetUnitDefCommandForTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actorUnit,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *target, bool canSwitchForVillager) {
	if (!actorUnit || !target || !actorUnit->IsCheckSumValidForAUnitClass() || !actorUnit->DerivesFromCommandable() ||
		!target->IsCheckSumValidForAUnitClass()) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef_base = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)actorUnit->unitDefinition;
	if (!unitDef_base->DerivesFromCommandable()) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *unitDef_asBird = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)unitDef_base;
	if (!unitDef_asBird->ptrUnitCommandHeader || !unitDef_asBird->ptrUnitCommandHeader->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF_HEADER *cmdh = unitDef_asBird->ptrUnitCommandHeader;
	AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF *result;
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
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *tmpUnitDefBase = actorPlayer->GetUnitDefBase((short int)villagerDATIDs[i]);
			if (tmpUnitDefBase && tmpUnitDefBase->IsCheckSumValidForAUnitClass() && tmpUnitDefBase->DerivesFromCommandable()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *tmpUnitDefBird = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)tmpUnitDefBase;
				cmdh = tmpUnitDefBird->ptrUnitCommandHeader;
				if (cmdh) {
					assert(cmdh->IsCheckSumValid());
					if (cmdh->IsCheckSumValid()) {
						// Try other villager type's command (with current "actorUnit")
						_asm {
							MOV ECX, cmdh;
							MOV EAX, DS:[ECX];
							PUSH 0;
							PUSH 0;
							PUSH 0;
							PUSH target;
							PUSH actorUnit;
							CALL DS:[EAX+4];
							MOV result, EAX;
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
AOE_STRUCTURES::STRUCT_UNIT_BASE *CreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID, float posY, float posX, float posZ) {
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
	return (AOE_STRUCTURES::STRUCT_UNIT_BASE *) res;
}


// Creates a unit at provided location. Does NOT make checks on location, please first make sure GetErrorForUnitCreationAtLocation returns 0.
// Please use CheckAndCreateUnit instead (unless you explicitely do NOT want to check target location)
// You can use 0 as posZ value.
// Warning: orientation IS unit.orientation for types 30-80. But for types 10/20/90, is corresponds to orientationIndex as float.
// Returns NULL if it failed
// Compatible with both in-game and editor screens. This overload corresponds to deserialize (and create) unit operation.
AOE_STRUCTURES::STRUCT_UNIT_BASE *CreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID, float posY, float posX, float posZ,
	long int status, float orientation) {
	if (!player || !player->IsCheckSumValid()) {
		return NULL;
	}
	long int res;
	_asm {
		PUSH orientation;
		PUSH status;
		PUSH DAT_ID;
		PUSH posZ;
		PUSH posX;
		PUSH posY;
		MOV ECX, player;
		MOV EAX, DS:[ECX];
		CALL DS:[EAX + 0x60];
		MOV res, EAX;
	}
	return (AOE_STRUCTURES::STRUCT_UNIT_BASE *) res;
}


// Creates a unit at provided location only if GetErrorForUnitCreationAtLocation agrees !
// Returns NULL if it failed
AOE_STRUCTURES::STRUCT_UNIT_BASE *CheckAndCreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef,
	float posX, float posY, bool checkVisibility, bool checkHills, bool checkConflictingUnits) {
	if (AOE_METHODS::GetErrorForUnitCreationAtLocation(player, unitDef, posY, posX, checkVisibility, checkHills, false, true, checkConflictingUnits) != 0) {
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
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
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


// Calls AOE's method to change a unit owner. Warning, this has bugs, see crCommand.OnUnitChangeOwner_fixes to call missing treatments.
void AOE_ChangeUnitOwner(AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer) {
	if (!targetUnit || !actorPlayer || !targetUnit->IsCheckSumValidForAUnitClass() || !actorPlayer->IsCheckSumValid()) {
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
	if (!infAI || !infAI->IsCheckSumValid() || !elem) { return false; }
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
	if (!infAI || !infAI->IsCheckSumValid()) { return false; }
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	if (newPlayerId < 0) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unit->unitDefinition;
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
	AOE_METHODS::ClearSelectedUnits(player);
	AOE_METHODS::SelectUnit(player, unitBase, true);
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
	AOE_METHODS::UI_BASE::RefreshUIObject(settings->ptrGameUIStruct->gamePlayUIZone);
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
								(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitId >= 0) &&
								(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitId != CST_UNITID_FARM) // Allow farm because they don't have a dedicated AI type (class)
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
									msg += std::to_string(commandable->ptrUnitCommandHeader->ptrCommandArray[i]->unitId);
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
