
#include "../include/buttonBar.h"

namespace BUTTONBAR {
;


// Hide a button from buttonbar. cmdButtonId should be in [0,11]
void HideCommandButton(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, int cmdButtonId) {
	assert((cmdButtonId >= 0) && (cmdButtonId < 12));
	if ((cmdButtonId < 0) || (cmdButtonId >= 12)) { return; }
	AOE_METHODS::UI_BASE::ShowUIObject(gameMainUI->unitCommandButtons[cmdButtonId], false);
	gameMainUI->unitCommandButtons[cmdButtonId]->buttonInfoValue[0] = -1;
	gameMainUI->unitCommandButtons[cmdButtonId]->commandIDs[0] = 0;
}


// Force refresh of game button bar
void ForceRefresh(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI) {
	// Note: this call uses recursivity in ROR_API (this ROR methods calls RockNRor...)
	_asm {
		MOV ECX, gameMainUI;
		CALL BUTTONBAR_CONST::MAIN_GAME_ZONE_COMMAND_BAR_RESET_PAGE;
	}
}
// Force refresh of game button bar
void ForceRefresh() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return; }
	BUTTONBAR::ForceRefresh(settings->ptrGameUIStruct);
}


// Refresh status for custom auto-attack policy buttons
void RefreshCustomAutoAttackButtons(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
	const AutoAttackPolicy *attackPolicy) {
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		return;
	}
	if (!attackPolicy) {
		attackPolicy = &ROCKNROR::crInfo.configInfo.autoAttackOptionDefaultValues;
	}
	int value = attackPolicy->attackVillagers ? 0 : 1;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_VILLAGERS]->showNumber = value;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_VILLAGERS]->numberToDisplay = value;
	AOE_METHODS::UI_BASE::RefreshUIObject(gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_VILLAGERS]);
	value = attackPolicy->attackNonTowerBuildings ? 0 : 1;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_BUILDINGS]->showNumber = value;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_BUILDINGS]->numberToDisplay = value;
	AOE_METHODS::UI_BASE::RefreshUIObject(gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_BUILDINGS]);
	value = (!attackPolicy->attackMilitary && !attackPolicy->attackNonTowerBuildings && !attackPolicy->attackTowers && !attackPolicy->attackVillagers);
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_DISABLED]->showNumber = value;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_DISABLED]->numberToDisplay = value;
	AOE_METHODS::UI_BASE::RefreshUIObject(gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_DISABLED]);
	value = (attackPolicy->Equals(ROCKNROR::crInfo.configInfo.autoAttackOptionDefaultValues)) ? 1 : 0;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_SET_DEFAULT]->showNumber = value;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_SET_DEFAULT]->numberToDisplay = value;
	AOE_METHODS::UI_BASE::RefreshUIObject(gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_SET_DEFAULT]);
}



// Called at the end of showUnitCommandButtons. Dispatches to specific command buttons handler (living unit, building.. or none !)
void AfterShowUnitCommandButtons(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI) {
	assert(gameMainUI && gameMainUI->IsCheckSumValid());
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		return;
	}
	if (gameMainUI->panelSelectedUnit == NULL) {
		return;
	}

	// Collect info
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	if (!controlledPlayer || !controlledPlayer->IsCheckSumValid()) {
		return;
	}

	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) gameMainUI->panelSelectedUnit;
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->unitDefinition || !unit->unitDefinition->IsCheckSumValidForAUnitClass()) {
		return;
	}
	if (controlledPlayer != unit->ptrStructPlayer) { return; } // unit does not belong to "me"
	if (unit->unitStatus != 2) {
		return;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
	if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) { return; }
	if (unitDef->commandAttribute == COMMAND_ATTRIBUTES::CST_CA_NONE) { return; } // Corresponds to unselectable, like eye candy

	// For living units (trainable)
	if (unitDef->unitType == GLOBAL_UNIT_TYPES::GUT_TRAINABLE) {
		AddButtonsForLivingUnit(gameMainUI, (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE*)unit);
		return;
	}
	// Buildings
	if (unitDef->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) {
		AddButtonsForBuildingUnit(gameMainUI, (AOE_STRUCTURES::STRUCT_UNIT_BUILDING*)unit);
		return;
	}
}


// Add relevant buttons in command bar for "trainable" unit (type=70). Excludes all other unit types, even child classes (buildings)
void AddButtonsForLivingUnit(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit) {
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) { return; }
	if (!unit || !unit->IsCheckSumValid()) { return; } // MUST be trainable
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return; }

	AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
	assert(unitDef && unitDef->IsCheckSumValid()); // Trainable !

	if (settings->mouseActionType == MOUSE_ACTION_TYPES::CST_MAT_NORMAL) {
		if ((unitDef->blastLevel != BLAST_LEVELS::CST_BL_DAMAGE_TARGET_ONLY) && (unitDef->blastRadius > 0)) {
			UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(unit->unitInstanceId);
			AddInGameCommandButton(CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_VILLAGERS, INGAME_UI_COMMAND_ID::CST_IUC_CROR_DONT_ATTACK_VILLAGERS, 0, false,
				localizationHandler.GetTranslation(CRLANG_ID_BTN_DONT_ATTACK_VILLAGER, "Click to prevent unit from attacking villagers automatically"),
				&ROCKNROR::crInfo.rockNRorIcons, true);
			AddInGameCommandButton(CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_BUILDINGS, INGAME_UI_COMMAND_ID::CST_IUC_CROR_DONT_ATTACK_BUILDINGS, 0, false,
				localizationHandler.GetTranslation(CRLANG_ID_BTN_DONT_ATTACK_BUILDINGS, "Click to prevent unit from attacking buildings automatically"),
				&ROCKNROR::crInfo.rockNRorIcons, false);
			AddInGameCommandButton(CST_CUSTOM_BUTTONID_AUTO_ATTACK_DISABLED, INGAME_UI_COMMAND_ID::CST_IUC_CROR_NO_AUTO_ATTACK, 0, false, 
				localizationHandler.GetTranslation(CRLANG_ID_BTN_DONT_ATTACK_OTHER, "Click to prevent unit from attacking other units automatically"),
				&ROCKNROR::crInfo.rockNRorIcons, false);
			AddInGameCommandButton(CST_CUSTOM_BUTTONID_AUTO_ATTACK_SET_DEFAULT, INGAME_UI_COMMAND_ID::CST_IUC_CROR_RESET_AUTO_ATTACK, 0, false,
				localizationHandler.GetTranslation(CRLANG_ID_BTN_RESTORE_ATTACK_BEHAVIOUR, "Click to restore normal auto-attack behaviour"),
				&ROCKNROR::crInfo.rockNRorIcons, false);
			const AutoAttackPolicy *aap = (unitInfo && unitInfo->autoAttackPolicyIsSet) ? &unitInfo->autoAttackPolicy : &ROCKNROR::crInfo.configInfo.autoAttackOptionDefaultValues;
			RefreshCustomAutoAttackButtons(gameMainUI, aap);
		}
		const char *protectHotkey = localizationHandler.GetTranslation(CRLANG_ID_BTN_UNIT_PROTECT_HOTKEY, "P");
		std::string protectText = localizationHandler.GetTranslation(CRLANG_ID_BTN_UNIT_PROTECT_TEXT, "Protect a position/unit");
		protectText += " ";
		protectText += protectHotkey[0]; // protectHotkey can't be NULL. Worst case, protectHotkey[0] = \0, but this will not cause any error.
		AddInGameCommandButton(CST_CUSTOM_BUTTONID_DEFEND_ZONE_OR_UNIT, INGAME_UI_COMMAND_ID::CST_IUC_CROR_DEFEND, 0, false, 
			protectText.c_str(), NULL /*ROCKNROR::crInfo.rockNRorIcons*/, false);
		gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_DEFEND_ZONE_OR_UNIT]->hotkey = protectHotkey[0]; // shortcut key for "defend/protect"
		gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_DEFEND_ZONE_OR_UNIT]->unknown_29C = 0;
	}

	if (settings->mouseActionType == MOUSE_ACTION_TYPES::CST_MAT_CR_PROTECT_UNIT_OR_ZONE) {
		SetButtonBarForDefendUnitOrZone(gameMainUI, unit);
	}
}


// Add relevant buttons in command bar for "building" unit (type=80). Excludes all other unit types.
void AddButtonsForBuildingUnit(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, AOE_STRUCTURES::STRUCT_UNIT_BUILDING *unit) {
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	if (!controlledPlayer || !controlledPlayer->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING*)unit->unitDefinition;
	if (!unitDef || !unitDef->IsCheckSumValid()) { return; } // MUST be building type

	bool isWall = (unitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupWall);
	if (isWall) {
		AddButtonsForWall(gameMainUI, unit);
		return;
	}

	bool multiQueueing = ROCKNROR::crInfo.configInfo.allowMultiQueueing; // If true, a building can have >1 unit "def" in queue.
	bool buttonIsVisible[12]; // Is button visible after standard buttons display.
	bool forceRefresh[12]; // Used to force refresh of buttons : text or read-only status can be wrong when switching between 2 similar units that have different current actions.
	bool currentButtonDoesNotBelongToThisPage[12];
	long int bestElemTotalCost[12];
	long int bestElemDATID[12];
	bool bestElemIsResearch[12];
	bool bestElemIsAvailable[12]; // Used to disable items (visible, but not clickable)
	short int bestElemLangNameId[12];
	for (int i = 0; i < 12; i++) {
		buttonIsVisible[i] = IsInGameUnitCommandButtonVisible(gameMainUI, i);
		bestElemTotalCost[i] = -1;
		bestElemDATID[i] = -1;
		bestElemIsResearch[i] = false;
		bestElemLangNameId[i] = -1;
		currentButtonDoesNotBelongToThisPage[i] = false;
		bestElemIsAvailable[i] = false;
		forceRefresh[i] = (buttonIsVisible[i]); // Always force refresh for "already displayed" buttons (useful when previous selection had a greyed out button, and new selection has same button - not disabled.
		// TODO: it would be better to force refresh another way to gain performance. But we would need to do this BEFORE buttons are updated (_Before_ShowUnitCommandButtons)
	}
	long int currentActionDATID = -1;
	bool currentActionIsResearch = false;
	long int currentActionButtonIndex = -1;
	short int currentActionLangNameId = -1;
	char *currentActionName = NULL; // useful when currentActionLangNameId is invalid.
	std::list<long int> activableUnitDefIDs = PLAYER::GetActivableUnitDefIDs(controlledPlayer); // A list of unitDef IDs that can be enabled thanks to (available) researches.
	// To support >1 page
	long int minButtonId = gameMainUI->panelButtonIdPageOffset;
	long int minButtonIdNextPage = gameMainUI->panelButtonIdPageOffset + 10 + 1; // 10=pageSize, +1 for next button after "this" last !
	long int maxFoundButtonId = -1;

	// TO DO: manage isBusy for living unit ? (not required as long as we don't customize buttons for living units...)
	AOE_STRUCTURES::STRUCT_UNIT_BUILDING *unitAsBuilding = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)unit;
	bool isBusy = false;
	// Fix queue numbers (otherwise, previous selected building's numbers will still display - and are irrelevant here)
	int buttonWithQueueNumber = -1;
	int queueNumberToDisplay = 0;
	if (unitAsBuilding && unitAsBuilding->IsCheckSumValid()) {
		isBusy = (unitAsBuilding->trainUnitQueueCurrentElemCount > 0);
		if (unitAsBuilding->ptrHumanTrainQueueInformation) {
			queueNumberToDisplay = unitAsBuilding->ptrHumanTrainQueueInformation->unitCount;
			if (queueNumberToDisplay > 0) {
				AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *queuedUnitDefBase = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *) controlledPlayer->GetUnitDefBase(unitAsBuilding->ptrHumanTrainQueueInformation->DATID);
				if (queuedUnitDefBase && queuedUnitDefBase->IsCheckSumValidForAUnitClass()) {
					buttonWithQueueNumber = GetButtonInternalIndexFromDatBtnId(queuedUnitDefBase->trainButton);
				}
			}
		}
	}
	for (int currentBtnId = 0; currentBtnId < 12; currentBtnId++) {
		AOE_STRUCTURES::STRUCT_UI_BUTTON_WITH_NUMBER *curBtn = gameMainUI->unitCommandButtons[currentBtnId];
		if (curBtn && curBtn->IsCheckSumValid()) {
			if ((curBtn->commandIDs[0] == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_ADD_TO_QUEUE) ||
				(AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN)) {
				long int n = AOE_METHODS::UNIT::GetTotalQueueNumberForUnit(unitAsBuilding, (short int)curBtn->buttonInfoValue[0]);
				curBtn->showNumber = (n > 0) ? 1 : 0;
				curBtn->numberToDisplay = n;
				forceRefresh[currentBtnId] = true;
			}
		}

	}
	AOE_STRUCTURES::STRUCT_ACTION_BASE *currentAction = AOE_STRUCTURES::GetUnitAction(unit);
	// For buildings, currentAction is Non-NULL when researching tech, when AI-triggered "train unit", but NULL for human-triggered "train unit"
	AOE_STRUCTURES::STRUCT_ACTION_MAKE_OBJECT *currentActionAsMakeObject = (AOE_STRUCTURES::STRUCT_ACTION_MAKE_OBJECT *)currentAction;
	AOE_STRUCTURES::STRUCT_ACTION_MAKE_TECH *currentActionAsMakeTech = (AOE_STRUCTURES::STRUCT_ACTION_MAKE_TECH *)currentAction;
	if (currentAction != NULL) {
		isBusy = true;
		if (currentActionAsMakeObject && currentActionAsMakeObject->IsCheckSumValid() &&
			(currentActionAsMakeObject->actionTypeID == currentActionAsMakeObject->GetExpectedInternalActionId())) {
			// This only handles AI-triggered MakeObject, not human-triggered (see below).
			currentActionDATID = currentActionAsMakeObject->targetUnitDAT_ID;
			currentActionIsResearch = false;
		}
		if (currentActionAsMakeTech && currentActionAsMakeTech->IsCheckSumValid() &&
			(currentActionAsMakeTech->actionTypeID == currentActionAsMakeTech->GetExpectedInternalActionId())) {
			// This handles ALL MakeTech actions (both AI or human triggered)
			currentActionDATID = currentActionAsMakeObject->targetUnitDAT_ID;
			currentActionIsResearch = true;
			AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = AOE_METHODS::PLAYER::GetResearchDef(controlledPlayer, currentActionAsMakeObject->targetUnitDAT_ID);
			if (resDef) {
				currentActionLangNameId = resDef->languageDLLName;
				currentActionName = resDef->researchName;
			}
		}

	}
	if ((unitAsBuilding->trainUnitQueueCurrentElemCount > 0) && (unitAsBuilding->ptrHumanTrainQueueInformation != NULL)) {
		// This only handles human-triggered MakeObject (not AI-triggered).
		currentActionDATID = unitAsBuilding->ptrHumanTrainQueueInformation->DATID;
		currentActionIsResearch = false;
	}
	// Get names for current action unit (when training a unit).
	if ((currentActionDATID != -1) && (!currentActionIsResearch)) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *curActionUnitDefBase = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)GetUnitDefStruct(controlledPlayer, (short int)currentActionDATID);
		if (curActionUnitDefBase && curActionUnitDefBase->IsCheckSumValidForAUnitClass()) {
			if (currentActionName == NULL) {
				currentActionName = curActionUnitDefBase->ptrUnitName;
			}
			if (currentActionLangNameId == -1) {
				currentActionLangNameId = curActionUnitDefBase->languageDLLID_Name;
			}
		}
	}


	// Handle more than 1 page : ROR code doesn't, so we have to hide the buttons it added if current page is not 1st one.
	// For the moment, just collect info.
	for (int currentBtnId = 0; currentBtnId < 12; currentBtnId++) {
		AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID curBtnCmdId = (AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID)gameMainUI->unitCommandButtons[currentBtnId]->commandIDs[0];
		if (curBtnCmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN) {
			AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *tmpUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)
				GetUnitDefStruct(controlledPlayer, (short int)gameMainUI->unitCommandButtons[currentBtnId]->buttonInfoValue[0]);

			if (tmpUnitDef && tmpUnitDef->IsCheckSumValidForAUnitClass() && tmpUnitDef->IsTypeValid() &&
				((tmpUnitDef->trainButton >= minButtonIdNextPage) || (tmpUnitDef->trainButton < minButtonId))) {
				currentButtonDoesNotBelongToThisPage[currentBtnId] = true;
			}
		}
		if (curBtnCmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH) {
			AOE_STRUCTURES::STRUCT_RESEARCH_DEF *tmpResearchDef = AOE_METHODS::PLAYER::GetResearchDef(controlledPlayer,
				(short int)gameMainUI->unitCommandButtons[currentBtnId]->buttonInfoValue[0]);
			if (tmpResearchDef && ((tmpResearchDef->buttonId >= minButtonIdNextPage) || (tmpResearchDef->buttonId < minButtonId))) {
				currentButtonDoesNotBelongToThisPage[currentBtnId] = true;
			}
		}
	}


	// Add custom buttons : not-yet available units/techs

	// RESEARCHES
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *playerResInfo = controlledPlayer->ptrResearchesStruct;
	AOE_STRUCTURES::STRUCT_RESEARCH_DEF_INFO *resDefInfo = NULL;
	if (playerResInfo) {
		resDefInfo = playerResInfo->ptrResearchDefInfo;
	}
	if (playerResInfo && resDefInfo) {
		for (int researchId = 0; (researchId < playerResInfo->researchCount) && (researchId < resDefInfo->researchCount); researchId++) {
			AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS *status = &playerResInfo->researchStatusesArray[researchId];
			AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = resDefInfo->GetResearchDef(researchId);
			assert(resDef);
			int rawButtonId = resDef->buttonId;
			if ((resDef->researchLocation == unitDef->DAT_ID1) &&
				(rawButtonId > maxFoundButtonId)) {
				maxFoundButtonId = rawButtonId;
			}
			long int buttonIndex = -1;
			if ((rawButtonId >= minButtonId) && (rawButtonId < minButtonIdNextPage)) {
				buttonIndex = GetButtonInternalIndexFromDatBtnId(rawButtonId);
			}
			assert(buttonIndex <= 12); // But can be -1
			// rawButtonId=0 for non-visible researches, we get buttonIndex=-1 in this case. Exclude it.
			bool researchIsAvailable = (status->currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE);
			if ((buttonIndex > -1) &&
				((!buttonIsVisible[buttonIndex]) || (currentButtonDoesNotBelongToThisPage[buttonIndex])) &&
				((status->currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) ||
				(researchIsAvailable && isBusy)
				)
				) {
				// Sorting strategy:
				// We suppose higher cost=further research, so lower cost should be first (next) available research.
				long int thisCost = 0;
				if (resDef->costUsed1) {
					thisCost += resDef->costAmount1;
				}
				if (resDef->costUsed2) {
					thisCost += resDef->costAmount2;
				}
				if (resDef->costUsed3) {
					thisCost += resDef->costAmount3;
				}
				bool costIsBetter = (bestElemTotalCost[buttonIndex] == -1) || (thisCost < bestElemTotalCost[buttonIndex]);

				if (((isBusy && researchIsAvailable) || costIsBetter) && // When we choose to show available researches, they have best priority.
					(resDef->researchLocation == unitDef->DAT_ID1) &&
					((resDef->researchTime > 0) || researchIsAvailable) // Hide unavailable immediate researches
					) {
					bestElemDATID[buttonIndex] = researchId;
					bestElemIsResearch[buttonIndex] = true;
					bestElemTotalCost[buttonIndex] = thisCost;
					bestElemLangNameId[buttonIndex] = resDef->languageDLLName;
					bestElemIsAvailable[buttonIndex] = researchIsAvailable && !isBusy;
				}
			}
		}
	}

	// TRAIN UNITS
	if (controlledPlayer->ptrStructDefUnitTable) {
		for (int loopUnitDefId = 0; loopUnitDefId < controlledPlayer->structDefUnitArraySize; loopUnitDefId++) {
			AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *loopUnitDef =
				(AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)controlledPlayer->ptrStructDefUnitTable[loopUnitDefId];
			if (loopUnitDef && loopUnitDef->IsCheckSumValidForAUnitClass() && loopUnitDef->IsTypeValid()) { // Only for living units
				int rawButtonId = loopUnitDef->trainButton;
				if ((loopUnitDef->trainLocation == unitDef->DAT_ID1) && (rawButtonId > maxFoundButtonId)) {
					maxFoundButtonId = rawButtonId;
				}
				long int buttonIndex = -1;
				if ((rawButtonId >= minButtonId) && (rawButtonId < minButtonIdNextPage)) {
					buttonIndex = GetButtonInternalIndexFromDatBtnId(rawButtonId);
				}
				assert(buttonIndex <= 12); // But can be -1
				if ((buttonIndex >= 0) && (buttonIndex <= 12) &&
					((!buttonIsVisible[buttonIndex]) || (currentButtonDoesNotBelongToThisPage[buttonIndex])) &&
					(loopUnitDef->trainLocation == unitDef->DAT_ID1)) {
					long int thisCost = 0;
					if (loopUnitDef->costs[0].costPaid) {
						thisCost += loopUnitDef->costs[0].costAmount;
					}
					if (loopUnitDef->costs[1].costPaid) {
						thisCost += loopUnitDef->costs[1].costAmount;
					}
					if (loopUnitDef->costs[2].costPaid) {
						thisCost += loopUnitDef->costs[2].costAmount;
					}
					bool preferThisUnit = false;
					bool costIsBetter = (bestElemTotalCost[buttonIndex] == -1) || (thisCost < bestElemTotalCost[buttonIndex]);
					if (loopUnitDef->availableForPlayer) { preferThisUnit = true; } // if unit is currently available, it has best priority !
					if (costIsBetter && !loopUnitDef->availableForPlayer) { preferThisUnit = true; }
					if (preferThisUnit && (!bestElemIsResearch) && (bestElemDATID[buttonIndex] >= 0)) { // only if we already selected a valid DATID (unit, ont research)
						AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *previousUnit =
							(AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)controlledPlayer->ptrStructDefUnitTable[bestElemDATID[buttonIndex]];
						// Cancel "prefer" if new one is stronger (stronger = less priority, because further in tech tree)
						if ((previousUnit->totalHitPoints < loopUnitDef->totalHitPoints) || // Use only attributes that ALWAYS increase ! Not speed (risky)
							(previousUnit->displayedAttack < loopUnitDef->displayedAttack)
							) {
							preferThisUnit = false; // "new" unit seems stronger, keep previous one.
						}
					}
					bool unitAvailableInTechTree = (loopUnitDef->availableForPlayer != 0);
					if (!loopUnitDef->availableForPlayer) {
						auto it = std::find_if(activableUnitDefIDs.begin(), activableUnitDefIDs.end(),
							[loopUnitDefId](long int tmpUnitDefId) { return (tmpUnitDefId == loopUnitDefId); });
						if (it != activableUnitDefIDs.end()) { // Found: this unit CAN be enabled in tech tree.
							unitAvailableInTechTree = true;
						}
					}

					if (preferThisUnit && unitAvailableInTechTree) {
						bestElemDATID[buttonIndex] = loopUnitDefId;
						bestElemIsResearch[buttonIndex] = false;
						bestElemTotalCost[buttonIndex] = thisCost;
						bestElemLangNameId[buttonIndex] = loopUnitDef->languageDLLID_Name;
						bestElemIsAvailable[buttonIndex] = (loopUnitDef->availableForPlayer != 0);
					}
				}
			}
		}
	}

	// Hide buttons added by ROR and that do not belong to current page
	for (int currentBtnId = 0; currentBtnId < 12; currentBtnId++) {
		if (currentButtonDoesNotBelongToThisPage[currentBtnId]) {
			bool forceShowAnyway = false;
			// There is only 1 exception : currently-being trained unit should always be displayed
			if (currentActionDATID > -1) {
				AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID curBtnCmdId = (AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID)gameMainUI->unitCommandButtons[currentBtnId]->commandIDs[0];
				long int btnDATID = gameMainUI->unitCommandButtons[currentBtnId]->buttonInfoValue[0];
				if ((curBtnCmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN) &&
					!currentActionIsResearch && (btnDATID == currentActionDATID)) {
					forceShowAnyway = true;
				}
				if ((curBtnCmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH) &&
					currentActionIsResearch && (btnDATID == currentActionDATID)) {
					forceShowAnyway = true;
				}
			}
			if (!forceShowAnyway) {
				HideCommandButton(gameMainUI, currentBtnId);
			}
		}
	}

	if (minButtonId > 0) {
		for (int buttonIndex = 0; buttonIndex < 12; buttonIndex++) {
			forceRefresh[buttonIndex] = true; // Force text refresh on ROR-added buttons on page 2+ (not well supported natively, no text is displayed)
		}
	}
	for (int buttonIndex = 0; buttonIndex < 12; buttonIndex++) {
		if (!bestElemIsResearch[buttonIndex] && multiQueueing) {
			forceRefresh[buttonIndex] = true; // Force text refresh all "train unit" buttons when multi queueing is enabled, to show correct queue number/enabled status
		}
	}
	for (int buttonIndex = 0; buttonIndex < 12; buttonIndex++) {
		AOE_STRUCTURES::STRUCT_UI_BUTTON_WITH_NUMBER *sb = gameMainUI->unitCommandButtons[buttonIndex];
		if (buttonIsVisible[buttonIndex] && forceRefresh[buttonIndex]) {
			long int correctButtonInfoValue = sb->buttonInfoValue[0];
			long int correctButtonCmdId = sb->commandIDs[0];
			if (correctButtonCmdId > 0) {
				bool disabled = (sb->readOnly != 0);
				AddInGameCommandButton(buttonIndex, (INGAME_UI_COMMAND_ID)correctButtonCmdId, correctButtonInfoValue,
					disabled, NULL, NULL, true);
			}
		}
	}

	// Now display our custom buttons (... on free slots)
	char nameBuffer[200];
	for (int buttonIndex = 0; buttonIndex < 12; buttonIndex++) {
		std::string elementInfo;
		if (bestElemDATID[buttonIndex] != -1) {
			GetLanguageDllText(bestElemLangNameId[buttonIndex], nameBuffer, sizeof(nameBuffer), "");
			if (bestElemIsResearch[buttonIndex]) {
				AddInGameCommandButton(buttonIndex, INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH, bestElemDATID[buttonIndex], !bestElemIsAvailable[buttonIndex], NULL /*elementInfo.c_str()*/, NULL, true);
				AOE_STRUCTURES::STRUCT_UI_BUTTON_WITH_NUMBER *sb = gameMainUI->unitCommandButtons[buttonIndex];
				sb->unknown_2C4_btnBoard;
				sb->helpDllId = sb->helpDllId;
				sb->winHelpDataDllId;
				sb->buttonInfoValue;
			} else {
				AddInGameCommandButton(buttonIndex, INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN, bestElemDATID[buttonIndex],
					!bestElemIsAvailable[buttonIndex] || !multiQueueing, NULL /*elementInfo.c_str()*/, NULL, true);
			}
		}
	}

	// Add custom buttons : if top-right button is free, show current action
	const int buttonIdForInProgress = 5;
	if ((currentActionDATID != -1) && !IsInGameUnitCommandButtonVisible(buttonIdForInProgress)) {
		GetLanguageDllText(currentActionLangNameId, nameBuffer, sizeof(nameBuffer), "");
		if ((nameBuffer[0] == 0) && (currentActionName != NULL)) {
			strcpy_s(nameBuffer, currentActionName);
		}
		std::string inProgressInfo;
		if (currentActionIsResearch) {
			inProgressInfo = localizationHandler.GetTranslation(CRLANG_ID_BEING_RESEARCHED, "Being researched:");
			inProgressInfo += " ";
			inProgressInfo += nameBuffer;
			AddInGameCommandButton(buttonIdForInProgress, INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH, currentActionDATID, true, inProgressInfo.c_str(), NULL, true);
		} else {
			inProgressInfo = localizationHandler.GetTranslation(CRLANG_ID_BEING_TRAINED, "Being trained:");
			inProgressInfo += " ";
			inProgressInfo += nameBuffer;
			AddInGameCommandButton(buttonIdForInProgress, INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN, currentActionDATID, true, inProgressInfo.c_str(), NULL, true);
		}
	}

	// When controlled player is an AI player, AI-triggered "make objects" (train unit) can't be stopped. Fix it.
	// Note: for human player-triggered "train unit", a STOP button is already visible (command=STOP), leave it unchanged.
	// WARNING: clicking on STOP for AI-triggered "train unit" does not update strategy and units will never be trained again ! Needs a fix !
	const int buttonIdForStop = 6;
	if ((currentAction != NULL) && (currentAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_MAKE_OBJECT) &&
		(gameMainUI->unitCommandButtons[buttonIdForStop]->commandIDs[0] != (long int)INGAME_UI_COMMAND_ID::CST_IUC_STOP)) {
		GetLanguageDllText(LANG_ID_STOP_CURRENT_ACTION, nameBuffer, sizeof(nameBuffer), "Stop current action");
		AddInGameCommandButton(buttonIdForStop, INGAME_UI_COMMAND_ID::CST_IUC_STOP, 0, false, nameBuffer, NULL, true);
	}

	// Show Next page button (not if busy, because we already have current action there)
	bool hasNextPage = (maxFoundButtonId >= minButtonIdNextPage); // has more buttons (Next actually goes to next page, NOT to first one)
	if (!isBusy && (hasNextPage || (minButtonId > 0))) {
		// There are buttons to display on next page: show button
		AddInGameCommandButton(5, INGAME_UI_COMMAND_ID::CST_IUC_NEXT_PAGE, 0, false, NULL, NULL, true);
		// We store the information "this is not last page" in button's infoValue.
		gameMainUI->unitCommandButtons[5]->buttonInfoValue[0] = hasNextPage;
	}
}


// Add relevant buttons in command bar for walls (type=80, class=27)
void AddButtonsForWall(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, AOE_STRUCTURES::STRUCT_UNIT_BUILDING *unit) {
	if (!ROCKNROR::crInfo.configInfo.useGatesFeature) {
		return;
	}
	if (!gameMainUI || !unit || !unit->IsCheckSumValidForAUnitClass() || !unit->unitDefinition) { return; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)unit->unitDefinition;
	assert(unitDef->IsCheckSumValid());
	if (!unitDef->IsCheckSumValid()) { return; }
	// Gate = wall with a "task swap group"
	if (unitDef->unitDefinitionSwitchGroupId <= 0) { return; }

	const char *gateHotkey = localizationHandler.GetTranslation(CRLANG_ID_BTN_OPEN_CLOSE_GATE_HOTKEY, "G");
	std::string gateText = localizationHandler.GetTranslation(CRLANG_ID_BTN_OPEN_CLOSE_GATE, "Open/Close gate");
	gateText += " (";
	gateText += gateHotkey[0]; // gateHotkey can't be NULL. Worst case, protectHotkey[0] = \0, but this will not cause any error.
	gateText += ")";

	AddInGameCommandButton(CST_CUSTOM_BUTTONID_OPEN_CLOSE_GATE, AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_GATE_OPEN_CLOSE, 0,
		false, gateText.c_str(), NULL, true);
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_OPEN_CLOSE_GATE]->hotkey = gateHotkey[0]; // shortcut key
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_OPEN_CLOSE_GATE]->unknown_29C = 0;
}


// Update the whole button bar when user clicks "defend unit/zone"
void SetButtonBarForDefendUnitOrZone(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit) {
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) { return; }
	// Hide all previous buttons
	for (int btnId = 0; btnId < 12; btnId++) {
		BUTTONBAR::HideCommandButton(gameMainUI, btnId);
	}
	if (!unit || !unit->IsCheckSumValid()) { return; } // MUST be trainable, not building

	AddInGameCommandButton(0, INGAME_UI_COMMAND_ID::CST_IUC_CANCEL_OR_BACK, 0, false, 
		localizationHandler.GetTranslation(CRLANG_ID_BTN_BACK, "Back"),
		NULL, false);

	UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(unit->unitInstanceId);
	if (unitInfo && unitInfo->HasValidProtectInfo()) {
		AddInGameCommandButton(1, INGAME_UI_COMMAND_ID::CST_IUC_CROR_DEFEND_STOP, 0, false,
			localizationHandler.GetTranslation(CRLANG_ID_BTN_UNIT_STOP_PROTECTING, "Stop defending current position/unit"),
			NULL, false);
	}
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
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *inGameMain = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *) AOE_METHODS::GetScreenFromName(gameScreenName);
	if (!inGameMain || !inGameMain->IsCheckSumValid()) { return false; }
	long int iconId = BUTTONBAR::BUTTONBAR_CONST::GuessIconIdFromUICommandId(UICmdId);
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
			AOE_METHODS::ReadLanguageTextForCategory(INTERNAL_MAIN_CATEGORIES::CST_IMC_UI_COMMANDS, UICmdId, 0, creationTextBuffer, sizeof(creationTextBuffer));
		}
		creationTextIfMissing = std::string(creationTextBuffer) + std::string(" ");
		creationTextBuffer[0] = 0; // reset buffer
	}

	if (UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN) {
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)GetUnitDefStruct(player, (short int)DATID);
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
		AOE_STRUCTURES::STRUCT_RESEARCH_DEF *researchDef = AOE_METHODS::PLAYER::GetResearchDef(player, (short int)DATID);
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
		AOE_METHODS::UI_BASE::GetUIButtonCreationText(creationTextBuffer, &btnInfoForMissingText, UICmdId, creationDllId_original);
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

	if (!BUTTONBAR::IsInGameUnitCommandButtonVisible(buttonIndex)) { // If this button was not used
		inGameMain->panelDisplayedButtonCount++; // Update number of used buttons. Not sure it has any impact
	}
	const char *creationTextToUse = (creationText == NULL) ? creationTextIfMissing.c_str() : creationText;
	if (forceUpdate) { // Dirty trick to force refresh
		AOE_STRUCTURES::STRUCT_UI_BUTTON_WITH_NUMBER *button = inGameMain->unitCommandButtons[buttonIndex];
		button->buttonInfoValue[0] = -1;
		button->commandIDs[0] = -1;
		button->hotkey = hotkeyDllId;
	}
	return AOE_METHODS::AOE_InGameAddCommandButton(player, buttonIndex, iconId, UICmdId, DATID,
		helpDllId, creationDllId, hotkeyDllId,
		pName, creationTextToUse, isDisabled, iconSlpInfo);
}


}
