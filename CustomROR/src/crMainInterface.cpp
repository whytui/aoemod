#include "../include/crMainInterface.h"

CustomRORMainInterface::CustomRORMainInterface() {
	this->crCommand = NULL;
	this->currentCustomPopup = NULL;
}

CustomRORMainInterface::CustomRORMainInterface(CustomRORCommand *crCommand) {
	this->crCommand = crCommand;
	this->currentCustomPopup = NULL;
}

CustomRORMainInterface::~CustomRORMainInterface() {
	if (this->currentCustomPopup) {
		delete this->currentCustomPopup;
		this->currentCustomPopup = NULL;
	}
}




// Manage key press in game screen and editor.
// Returns true if event has been handled AND we don't want original code to try to handle it.
bool CustomRORMainInterface::GameAndEditor_OnKeyPress(long int pressedKey, bool CTRL, bool SHIFT, bool ALT) {
	ROR_STRUCTURES_10C::STRUCT_ANY_UI **pCurrentUI = (ROR_STRUCTURES_10C::STRUCT_ANY_UI **)AOE_OFFSETS_10C::ADDR_VAR_ACTIVE_UI_STRUCT;
	assert(pCurrentUI != NULL);
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *currentUI = *pCurrentUI;
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings != NULL);
	if (!settings) { return false; }
	bool isInEditor = (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR);
	if (isInEditor) {
		// Consistency check.
		assert(currentUI->checksum == 0x00547360);
		if (currentUI->checksum != 0x00547360) { return false; }
	}
	bool isInGame = (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) && (currentUI->checksum == CHECKSUM_UI_IN_GAME_MAIN); // True if game is running (and has focus)
	bool isInGame_ScenarioScreen = (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) && (currentUI->checksum == 0x00547B94); // 0x00547B94=scenario info screen
	bool isMenuOpen = false; // Menu or diplomacy dialog
	if (isInGame) {
		isMenuOpen = (currentUI->focusedComponent && (currentUI->focusedComponent->checksum == CHECKSUM_UI_IN_GAME_MENU)) ||
			(currentUI->focusedComponent && (currentUI->focusedComponent->checksum == CHECKSUM_UI_DIPLOMACY_DIALOG));
	}
	if (isInEditor) {
		isMenuOpen = (currentUI->focusedComponent && (currentUI->focusedComponent->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MENU));
	}

	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return false; } // May occur when a key is pressed and the game just opened

	// Disable ALT-F4 when it would conflict with custom dialog
	// (isInGame) condition is optional. If removed, ALT-F4 will be disabled in scenario editor when our dialog is on.
	// WARNING: we can't prevent this to occur if user clicks on the top-right corner cross :(
	if ((isInGame) && (ALT) && (pressedKey == VK_F4) && (this->crCommand->crInfo->customYesNoDialogVar != NULL)) {
		// Disable ALT-F4 when our custom dialog is ON (based on the same dialog, would provoke conflicts and our dialog would stay ON forever)
		return true;
	}

	// Handle more Units shortcuts using numpad keys 0-9
	if ((isInGame) && (!isMenuOpen) && (pressedKey >= VK_NUMPAD0) && (pressedKey <= VK_NUMPAD9)) {
		long int shortcutNumber = (pressedKey - VK_NUMPAD0); // 0-9
		shortcutNumber = shortcutNumber - 11; // Use values [-11,-2]. Do not use those: -1=<joker for groups>. 0=<no shortcut>. 1-10=<standard shortcuts>. 11-127=<group_ids>
		if (CTRL) {
			AssignShortcutToSelectedUnits(GetControlledPlayerStruct_Settings(), shortcutNumber);
		} else {
			SelectUnitsUsingShortcut(GetControlledPlayerStruct_Settings(), shortcutNumber, SHIFT);
		}
	}

	// If Key=F10 (0x79) and "current UI"=scenario editor then open menu. 
	// Do NOT do this from other screens or when a popup is already opened !!! Never ! Or game will crash afterwards.
	if ((pressedKey == VK_F10) && isInEditor && !isMenuOpen && (!this->crCommand->crInfo->HasOpenedCustomGamePopup())) {
		// Additional check
		assert(GetGameSettingsPtr()->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR);
		_asm {
			MOV ECX, pCurrentUI
			MOV ECX, DS:[ECX] // We already checked it is not NULL
			MOV EAX, 0x00494A90 // editor.OpenMenu()
			CALL EAX
		}
	}


	// F9 in game: bring idle military units to current location (screen position).
	// Requires ManageAI !
	if ((pressedKey == VK_F9) && isInGame && !isMenuOpen) {
		if (this->crCommand->crInfo->configInfo.enableCallNearbyIdleMilitaryUnits) {
			this->crCommand->CallNearbyIdleMilitaryUnits();
		}
	}

	// F2 in game: unit properties
	if ((isInGame) && (!isMenuOpen) && (pressedKey == VK_F2)) {
		this->OpenInGameUnitPropertiesPopup();
	}

	// ESC (1B) - close custom dialog if opened
	if ((isInEditor || isInGame) && ((pressedKey == VK_ESCAPE) /*|| (pressedKey == VK_RETURN)*/) && (this->crCommand->crInfo->HasOpenedCustomGamePopup())) {
		this->CloseCustomGamePopup(true);
	}

	// F1 in editor : switch to unit selection
	if (!isMenuOpen && (isInEditor) && (pressedKey == VK_F1) && (!this->crCommand->crInfo->HasOpenedCustomGamePopup())) {
		settings->mouseActionType = AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_NORMAL;
	}
	// F2 in editor: edit selected unit
	if (!isMenuOpen && (isInEditor) && (pressedKey == VK_F2) && (!this->crCommand->crInfo->HasOpenedCustomGamePopup())) {
		this->OpenCustomEditorEditUnitPopup();
	}
	// F3 in editor: scenario information
	if (!isMenuOpen && (isInEditor) && (pressedKey == VK_F3) && (!this->crCommand->crInfo->HasOpenedCustomGamePopup())) {
		this->OpenCustomEditorScenarioInfoPopup();
	}

	// CTRL-F1 : display messages
	if (!isMenuOpen && CTRL && (pressedKey == VK_F1)) {
		// Double spaces are intented (to compensate weird-looking font)
		this->OpenTraceMessagePopup();
	}


#ifdef _DEBUG
	// TEST - F7 - debug only
	if (!isMenuOpen && (isInEditor || isInGame) && (pressedKey == VK_F7)) {
		unsigned long int *marray = (unsigned long int *)AOE_OFFSETS_10C::UNKNOWN_ARRAY_6A18C0;
		ROR_STRUCTURES_10C::STRUCT_UNKNOWN_MAP_DATA_F04C *md1 = (ROR_STRUCTURES_10C::STRUCT_UNKNOWN_MAP_DATA_F04C *) 0x583BC8;
		ROR_STRUCTURES_10C::STRUCT_UNKNOWN_MAP_DATA_F04C *md2 = (ROR_STRUCTURES_10C::STRUCT_UNKNOWN_MAP_DATA_F04C *) 0x6A1CC0;
		char txt[1000];
		memset(txt, 0, 900);
		long int v = md1->unknown_0CE624_mapUnitOccupiedGrid[0];
		for (int i = 0; i < 64; i++) {
			int bit = v & (1 << i);
			txt[i] = bit ? '1' : '0';
		}
		v = md1->unknown_0CE624_mapUnitOccupiedGrid[0x40];
		for (int i = 0; i < 64; i++) {
			int bit = v & (1 << i);
			txt[64 + i] = bit ? '1' : '0';
		}
		v = md1->unknown_0CE624_mapUnitOccupiedGrid[0x80];
		for (int i = 0; i < 64; i++) {
			int bit = v & (1 << i);
			txt[128 + i] = bit ? '1' : '0';
		}
		v = md1->unknown_0CE624_mapUnitOccupiedGrid[0xC0];
		for (int i = 0; i < 64; i++) {
			int bit = v & (1 << i);
			txt[128 + 64 + i] = bit ? '1' : '0';
		}
		bool c1 = md1->IsCheckSumValid();
		c1 = md2->IsCheckSumValid();
	}

	// TEST - F8 - show dialog
	if (!isMenuOpen && (isInEditor || isInGame) && (pressedKey == VK_F8) && (!this->crCommand->crInfo->HasOpenedCustomGamePopup())) {
		std::string sTechTreeInfo = "";
		int techToShowCount = 0;
		static char buffer[1024] = "\0";
		char *posInBuf = buffer;
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = NULL;
		ROR_STRUCTURES_10C::STRUCT_UNIT *selectedUnit = NULL;
		if (global) {
			player = GetPlayerStruct(global->humanPlayerId);
		}
		ROR_STRUCTURES_10C::STRUCT_UNIT **unitArray = this->crCommand->crInfo->GetRelevantSelectedUnitsPointer(player);
		if (unitArray) {
			selectedUnit = unitArray[0];
		}

		if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) {
			// Editor
			if (player && (player->selectedUnitCount > 0)) {
				if (selectedUnit) {
					sprintf_s(posInBuf, 200, "unitId=%ld/0x%X posX=%f posY=%f status=%d\n",
						selectedUnit->unitInstanceId, selectedUnit->unitInstanceId, selectedUnit->positionX, selectedUnit->positionY, selectedUnit->unitStatus);
					posInBuf = posInBuf + strlen(posInBuf);
					if (selectedUnit->ptrStructDefUnit) {
						sprintf_s(posInBuf, 200, "unitDefId=%d / %d", selectedUnit->ptrStructDefUnit->DAT_ID1, selectedUnit->ptrStructDefUnit->DAT_ID2);
						posInBuf = posInBuf + strlen(posInBuf);
					}
				}
			}
			// TO DO write buffer
			ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *scinfo = global->scenarioInformation;
			ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *gmapinfo = global->gameMapInfo;
			ROR_STRUCTURES_10C::STRUCT_GAME_MAP_TILE_INFO *tile = gmapinfo->GetTileInfo(1, 1);
			int a = tile->GetTerrainId();
			bool b = tile->SetAltitude(3);
			bool c = tile->SetTerrainId(4);
			a = a + 1;

			ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *se = (ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_GetScreenFromName(scenarioEditorScreenName);
			if (se != NULL) {
				int a = se->diamondMap->checksum;
				ROR_STRUCTURES_10C::STRUCT_TERRAIN_DEF *t = &se->diamondMap->gameMapInfo->terrainDefinitions[0];
				se->diamondMap->gameMapInfo->terrainDefinitions;
				se->unknown_580->checksum;//
				se->unknown_59C->checksum;//
				se->trn_lst_terrainList->checksum;
				se->unknown_5AC->sizeX += 50;//
				//char buffer[111];
				//sprintf_s(buffer, "%08X, %08X", se, se->diamondMap);
				//this->OpenCustomDialogMessage(buffer, 100, 100);
				//bool b = se->gamePlayUIZone->IsCheckSumValid();
				//ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *btn = se->btnTabs[0];
				//se->gamePlayUIZone;
				//se->map_btn_random->visible; // map btn randm

				int aa = 1 + 1;
			}

		}
		ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *gameScreen = GetGameSettingsPtr()->ptrGameUIStruct;
		if (IsGameRunning()) {
			// During Game

			selectedUnit = gameScreen->panelSelectedUnit;
			ROR_STRUCTURES_10C::STRUCT_PLAYER *player = NULL;
			if (selectedUnit) { player = selectedUnit->ptrStructPlayer; }
			if (selectedUnit) {
				long int actionTargetUnitId = -1;
				unsigned long int addraction = -1;
				ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDefBase = selectedUnit->GetUnitDefBase();
				ROR_STRUCTURES_10C::STRUCT_UNIT_ACTION_INFO *ainfo = NULL;
				if ((unitDefBase->unitType >= GLOBAL_UNIT_TYPES::GUT_TYPE50) && (unitDefBase->unitType <= GLOBAL_UNIT_TYPES::GUT_BUILDING)) {
					ainfo = selectedUnit->ptrActionInformation;
				}
				if (ainfo && (ainfo->ptrActionLink) && (ainfo->ptrActionLink->actionStruct)) {
					actionTargetUnitId = ainfo->ptrActionLink->actionStruct->targetUnitId;
					addraction = (unsigned long int)ainfo->ptrActionLink->actionStruct;
				}
				ROR_STRUCTURES_10C::STRUCT_UNIT_ACTIVITY *unitActivity = selectedUnit->currentActivity;
				assert(unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass());
				if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) { return false; }
				if (unitActivity) {
					sprintf_s(posInBuf, 200, "unitId=%ld/0x%X DATID=%d/%d  posX=%f posY=%f  status=%d\nActivity=0x%08X - Activity+30=0x%X +28=0x%X\n" \
						"target=%ld\nAction=%08X ActionTargetUnitId=%ld\n",
						selectedUnit->unitInstanceId, selectedUnit->unitInstanceId, unitDefBase->DAT_ID1, unitDefBase->DAT_ID2, 
						selectedUnit->positionX, selectedUnit->positionY, selectedUnit->unitStatus,
						(long int)unitActivity, unitActivity->currentActionId, unitActivity->internalId_whenAttacked, unitActivity->targetUnitId,
						addraction, actionTargetUnitId);
				} else {
					sprintf_s(posInBuf, 200, "unitId=%ld/0x%X DATID=%d/%d  posX=%f posY=%f  status=%d\nActivityChecksum=None\nnActionTargetUnitId=%ld\n",
						selectedUnit->unitInstanceId, selectedUnit->unitInstanceId, unitDefBase->DAT_ID1, unitDefBase->DAT_ID2,
						selectedUnit->positionX, selectedUnit->positionY, selectedUnit->unitStatus, actionTargetUnitId);
				}
				posInBuf = posInBuf + strlen(posInBuf);
			}
			if (player) {
				ROR_STRUCTURES_10C::STRUCT_AI *ai = player->GetAIStruct();
				if (ai) {
					//ai->structTacAI.plannedResourceNeeds.resourceAmount[0];
					sprintf_s(posInBuf, 200, "Player %i:\nresId priority order = %ld  %ld  %ld  %ld\nnb gatherers         = %ld  %ld  %ld  %ld\nResources = %ld  %ld  %ld  %ld\n",
						player->playerId,
						ai->structTacAI.extraResourceTypeNeededByPriority[0],
						ai->structTacAI.extraResourceTypeNeededByPriority[1],
						ai->structTacAI.extraResourceTypeNeededByPriority[2],
						ai->structTacAI.extraResourceTypeNeededByPriority[3],
						ai->structTacAI.gathererCount_desired[0],
						ai->structTacAI.gathererCount_desired[1],
						ai->structTacAI.gathererCount_desired[2],
						ai->structTacAI.gathererCount_desired[3],
						ai->structTacAI.extraResourceAmount[0],
						ai->structTacAI.extraResourceAmount[1],
						ai->structTacAI.extraResourceAmount[2],
						ai->structTacAI.extraResourceAmount[3]
						);
					;
				}
				posInBuf = posInBuf + strlen(posInBuf);
			}
			if (player && selectedUnit && (player->playerId == global->humanPlayerId)) {
				ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = selectedUnit->ptrStructDefUnit;
				assert(unitDef && unitDef->IsCheckSumValid());
				if (unitDef) {
					char nameBuffer[50];
					short int researchCount = player->ptrResearchesStruct->researchCount;
					ROR_STRUCTURES_10C::STRUCT_PLAYER_RESEARCH_STATUS *rs = player->ptrResearchesStruct->researchStatusesArray; // ->currentStatus
					sTechTreeInfo = "";
					for (int rid = 0; rid < researchCount; rid++) {
						if (rs[rid].currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) {
							if (player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[rid].researchLocation == unitDef->DAT_ID1) {
								techToShowCount++;
								if (techToShowCount > 0) { sTechTreeInfo += "\n"; }
								sTechTreeInfo += "techId ";
								sTechTreeInfo += std::to_string(player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[rid].technologyId);
								sTechTreeInfo += " = ";
								*nameBuffer = 0; // Reset string
								GetLanguageDllText(player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[rid].languageDLLName, nameBuffer, sizeof(nameBuffer) - 1,
									player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[rid].researchName);
								sTechTreeInfo += nameBuffer;
							}
						}
					}
				}
			}
			//TMP
			std::string ss = "";
			ROR_STRUCTURES_10C::STRUCT_PLAYER *humanPlayer = GetControlledPlayerStruct_Settings();
			assert(humanPlayer && humanPlayer->IsCheckSumValid());
			if (selectedUnit && selectedUnit->IsCheckSumValid() && (player->playerId == global->humanPlayerId)) {
				if (selectedUnit->ptrActionInformation && selectedUnit->ptrActionInformation->ptrActionLink &&
					selectedUnit->ptrActionInformation->ptrActionLink->actionStruct &&
					selectedUnit->ptrActionInformation->ptrActionLink->actionStruct->unsure_resourceValue) {
				}
				long int u = selectedUnit->unitInstanceId;
				long int t = -1;
				//CreateCmd_RightClick(u, t, 40, 20);
				//return false;
				//CreateCmd_Build(u, 110, 1, 1);
			}
		}


		if (this->OpenCustomGamePopup<CustomPopup>(580, 460, false)) {
			unsigned long int *unused;
			AOE_AddLabel(this->crCommand->crInfo->GetCustomGamePopup(), (ROR_STRUCTURES_10C::STRUCT_UI_LABEL**)&unused, buffer, 60, 20, 520, 160);
			if (!sTechTreeInfo.empty() && (techToShowCount > 0)) {
				AOE_AddLabel(this->crCommand->crInfo->GetCustomGamePopup(), (ROR_STRUCTURES_10C::STRUCT_UI_LABEL**)&unused, "(future) Tech tree available items", 60, 180, 200, 20);
				AOE_AddLabel(this->crCommand->crInfo->GetCustomGamePopup(), (ROR_STRUCTURES_10C::STRUCT_UI_LABEL**)&unused, (char*)sTechTreeInfo.c_str(), 60, 200, 240, 16 * (techToShowCount+1));
			}
		}
	}
#endif

	// F1: do not open help. This sucks.
	if (pressedKey == VK_F1) {
		return true;
	}

	return false; // Standard case: leave ROR code execute normally.
}


// Create in-game customROR options screen. Returns false if failed and if return address must be changed.
bool CustomRORMainInterface::CreateGameCustomRorOptionsPopup(ROR_STRUCTURES_10C::STRUCT_ANY_UI *previousPopup) {
	assert(previousPopup != NULL);
	if (!previousPopup) { return false; }

	assert(!this->crCommand->crInfo->HasOpenedCustomGamePopup()); // Custom popup is not already open
	if (this->crCommand->crInfo->HasOpenedCustomGamePopup()) { return true; } // do nothing, but not treated as alloc error
	
	InGameCustomRorOptionsPopup *popupUIObject = this->CreateCustomPopupObject<InGameCustomRorOptionsPopup>();
	if (popupUIObject) {
		popupUIObject->CloseMenuAndOpenPopup(previousPopup);
	}

	if (!popupUIObject) {
		return false; // need to change return address
	}
	// Dirty workaround because custom options popup is not created using standard procedures :(
	this->crCommand->crInfo->ForceSetCurrentGamePopup(popupUIObject->GetAOEPopupObject(), popupUIObject->customOptionButtonVar, NULL);
	
	return true;
}


// Ensure exiting game won't crash due to UI pointer issues related to custom popup (occurs in scenario editor)
// Returns true if we want NOT the game to exit
bool CustomRORMainInterface::FixGamePopupIssuesBeforeGameClose() {
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (this->crCommand->crInfo->HasOpenedCustomGamePopup() && settings &&
		(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR)) {
		// In scenario editor, UI objects are not correctly freed and the game crashes if we have a custom popup opened...
		// Closing the popup solves the issue !
		this->CloseCustomGamePopup(true);
		assert(!this->crCommand->crInfo->HasOpenedCustomGamePopup());
		return true; // Do NOT exit game (if possible). We consider the ALT-F4 closed the popup (that's a choice...)
	}
	return false;
}


// Returns NULL if failed (you can't create a custom popup object if previous one is still opened).
// If result is non-NULL, you can call [result]->OpenPopup() to actually open the popup.
template<class popupType> popupType *CustomRORMainInterface::CreateCustomPopupObject() {
	if (this->crCommand->crInfo->HasOpenedCustomGamePopup()) {
		return NULL;
	}
	if (this->currentCustomPopup) {
		// If previous popup object still exists but is marked as "closed", we can delete it.
		if (this->currentCustomPopup->IsClosed()) {
			delete this->currentCustomPopup;
			this->currentCustomPopup = NULL;
		} else {
			return NULL;
		}
	}
	this->currentCustomPopup = (CustomPopup*) new popupType();
	this->currentCustomPopup->SetCRCommand(this->crCommand);
	return (popupType*)this->currentCustomPopup;
}


// Please use this API to open a custom game popup. Do not call crInfo's method directly.
// Returns NULL if failed.
// You can get AOE popup object with xxx->GetAOEPopupObject()
template<class popupType> popupType *CustomRORMainInterface::OpenCustomGamePopup(long int hSize, long int vSize, bool withCancelBtn) {
	popupType *result = this->CreateCustomPopupObject<popupType>();
	if (result != NULL) {
		if (!this->currentCustomPopup) { return result; } // Just a security
		if (this->currentCustomPopup->OpenPopup(hSize, vSize, withCancelBtn) == NULL) {
			// Open popup failed. Don't return an invalid/incomplete object: free it and return NULL.
			delete result;
			result = NULL;
		}
	}
	return result;
}


// Please always use this API to close a custom game popup. Do not call crInfo's method directly.
// set isCancel to true to indicate we don't want to save/apply changes.
// this->currentCustomPopup is cleared/freed in this method.
void CustomRORMainInterface::CloseCustomGamePopup(bool isCancel) {
	if (this->currentCustomPopup) {
		this->currentCustomPopup->ClosePopup(isCancel);
		CustomPopup *next = this->currentCustomPopup->nextPopup;
		delete this->currentCustomPopup;
		this->currentCustomPopup = next; // Generally NULL unless closing the popup triggered a new one.
	}
}


// Called for ALL button clicks in the game.
// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool CustomRORMainInterface::Global_OnButtonClick(unsigned long int objAddress) {
	if (this->crCommand->crInfo->IsCustomGamePopupOKButton(objAddress)) {
		// Close the custom popup when it is open and we just clicked on OK button.
		this->CloseCustomGamePopup(false);
		return true;
	}
	if (this->crCommand->crInfo->IsCustomGamePopupCancelButton(objAddress)) {
		// Close the custom popup when it is open and we just clicked on Cancel button.
		this->CloseCustomGamePopup(true);
		return true;
	}
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *obj = (ROR_STRUCTURES_10C::STRUCT_ANY_UI *)objAddress;

	//Custom checkboxes: check/uncheck + manage custom buttons onclick
	if (this->crCommand->crInfo->HasOpenedCustomGamePopup()) {
		ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *objAsCheckBox = (ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *)objAddress;
		if (objAsCheckBox->IsCheckSumValid()) {
			AOE_CheckBox_SetChecked(objAsCheckBox, !objAsCheckBox->IsChecked());
		}
		ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *objAsButton = (ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *)objAddress;
		if (objAsButton->IsCheckSumValid()) {
			return this->OnCustomButtonClick(objAsButton);
		}
	}

	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();

	// Scenario editor
	ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *se = (ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_GetScreenFromName(scenarioEditorScreenName);
	if ((se != NULL) && settings && (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR)) {
		// Manage (overload) scenario editor buttons
		if (obj == se->map_btn_generateMap) {
			return this->ScenarioEditor_callMyGenerateMapIfRelevant();
		}
	}

	// Game menu
	ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MENU *menu = (ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MENU *)AOE_GetScreenFromName(menuDialogScreenName);
	if (menu && menu->IsCheckSumValid() && settings && (obj->ptrParentObject == menu)) {
		ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *objAsButton = (ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *)objAddress;
		if (objAsButton->IsCheckSumValid()) {
			// True for buttons that do not trigger a call to ManageOptionButtonClickInMenu
			bool btnEventNotCaughtByRORMenuBtnEvent = (objAsButton->unsure_buttonId == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_SAVE) ||
				(objAsButton->unsure_buttonId == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_LOAD) ||
				(objAsButton->unsure_buttonId == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_QUIT) ||
				(objAsButton->unsure_buttonId == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_HELP);
			if (btnEventNotCaughtByRORMenuBtnEvent) {
				this->FreeInGameCustomOptionsButton(); // Free custom button if needed, for buttons not handled in ManageOptionButtonClickInMenu.
				this->crCommand->crInfo->ForceClearCustomMenuObjects(); // Here we CAN already free custom button because it's not in use (current event is about another button)
			}
		}
	}

	return false; // Normal case: let game handle event normally.
}


// Use this to properly free custom button from options menu. This should (always) be called when ingame menu is closed.
void CustomRORMainInterface::FreeInGameCustomOptionsButton() {
	if (this->crCommand->crInfo->configInfo.showCustomRORMenu) {
		// Add "custom options" button (from game menu) to our "garbage collector" so that it is freed later.
		if (this->crCommand->crInfo->customGameMenuOptionsBtnVar) {
			this->crCommand->crInfo->AddObjectInPopupContentList(this->crCommand->crInfo->customGameMenuOptionsBtnVar);
			this->crCommand->crInfo->customGameMenuOptionsBtnVar = NULL;
		}
	}
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool CustomRORMainInterface::OnCustomButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_BUTTON *sender) {
	if (!sender || !sender->IsCheckSumValid()) { return false; }
	bool doCloseCustomPopup = false;
	if (!GetGameGlobalStructPtr()) { return false; }
	ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *scInfo = GetGameGlobalStructPtr()->scenarioInformation;

	if (this->currentCustomPopup) {
		bool eventHandled = this->currentCustomPopup->OnButtonClick(sender);
		if (this->currentCustomPopup->IsClosed()) {
			CustomPopup *next = this->currentCustomPopup->nextPopup;
			delete this->currentCustomPopup;
			this->currentCustomPopup = next; // Generally NULL unless closing the popup triggered a new one.
		}
		return eventHandled;
	}
	return false;
}


// Opens the custom "edit unit" in editor
// Returns true if OK.
bool CustomRORMainInterface::OpenCustomEditorEditUnitPopup() {
	return (this->OpenCustomGamePopup<EditorEditUnitInfoPopup>(500, 400, true) != NULL);
}


// Opens the custom "simple edit text" popup in editor
// outputBuffer is a pointer where the typed text will be copied when the popup is closed. Make sure its allocated size is >=maxLength
// Returns true if OK.
bool CustomRORMainInterface::OpenCustomLargeTextEditPopup(char *title, char *initialValue, long int maxLength, char *outputBuffer, bool readOnly, bool withCancelBtn) {
	SimpleEditTextPopup *p = this->OpenCustomGamePopup<SimpleEditTextPopup>(780, 590, withCancelBtn);
	if (p) {
		p->AddPopupContent(title, initialValue, maxLength, outputBuffer, readOnly);
	}
	return (p != NULL);
}


// Open a popup with CustomROR (debug) messages
bool CustomRORMainInterface::OpenTraceMessagePopup() {
	bool reverseOrder = this->crCommand->crInfo->configInfo.showLogsInReverseOrder;
	// Double spaces are intented (to compensate weird-looking font)
	char *title = "Show  debug  messages";
	if (reverseOrder) {
		title = "Show  debug  messages (reverse order)";
	}
	return this->OpenCustomLargeTextEditPopup(title,
		(char*)traceMessageHandler.GetAllMessages(reverseOrder).c_str(),
		0, NULL, false /* NOT read only so that scrollbar is active*/, false);
}


// Open the relevant "view/edit unit" popup for currently selected unit.
// Returns true if successful.
bool CustomRORMainInterface::OpenInGameUnitPropertiesPopup() {
	ROR_STRUCTURES_10C::STRUCT_PLAYER *humanPlayer = GetControlledPlayerStruct_Settings();
	if (!humanPlayer || !humanPlayer->IsCheckSumValid()) {
		return false;
	}
	if (humanPlayer->selectedUnitCount <= 0) {
		return false;
	}
	ROR_STRUCTURES_10C::STRUCT_UNIT **selectedUnits = this->crCommand->crInfo->GetRelevantSelectedUnitsPointer(humanPlayer);
	assert(selectedUnits != NULL);
	ROR_STRUCTURES_10C::STRUCT_UNIT *selectedUnit = selectedUnits[0];
	if (!selectedUnit || !selectedUnit->IsCheckSumValid()) {
		return false;
	}
	if (!this->OpenInGameUnitPropertiesPopup(selectedUnit)) {
		return false;
	}
	
	if (!IsMultiplayer()) {
		SetGamePause(true);
	}
	return true;
}


// Open the relevant "view/edit unit" popup for provided unit.
// Returns true if successful.
bool CustomRORMainInterface::OpenInGameUnitPropertiesPopup(ROR_STRUCTURES_10C::STRUCT_UNIT *unit) {
	if (!unit || !unit->IsCheckSumValid()) {
		return false;
	}
	/*ROR_STRUCTURES_10C::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	if (!controlledPlayer || !controlledPlayer->IsCheckSumValid()) {
		return false;
	}
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDefBase = unit->GetUnitDefBase();
	if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	if (!unit->ptrStructPlayer || !unit->ptrStructPlayer->IsCheckSumValid()) {
		return false;
	}
	short int unitPlayerId = unit->ptrStructPlayer->playerId;
	bool unitIsMine = (unitPlayerId == controlledPlayer->playerId);

	if (unit->unitInstanceId < 0) {
		return false; // Exclude temp units
	}
	*/
	InGameUnitPropertiesPopup *popup = this->OpenCustomGamePopup<InGameUnitPropertiesPopup>(600, 500, true);
	popup->AddPopupContent(unit->unitInstanceId);
	return (popup != NULL);
}


// Opens the custom "edit unit" in editor
// Returns true if OK.
bool CustomRORMainInterface::OpenCustomEditorScenarioInfoPopup() {
	EditorScenarioInfoPopup *popup = this->OpenCustomGamePopup<EditorScenarioInfoPopup>(500, 400, false);
	bool result = (popup != NULL);
	if (popup) {
		popup->SetVarToUpdate_allowUnitOverlapping(&this->crCommand->crInfo->configInfo.editor_allowUnitOverlapping);
		popup->SetVarToUpdate_disableHillModeChecks(&this->crCommand->crInfo->configInfo.editor_disableHillModeChecks);
		popup->SetVarToUpdate_disableTerrainRestrictionChecks(&this->crCommand->crInfo->configInfo.editor_disableTerrainRestrictions);
		popup->SetVarToUpdate_lengthenCombatMode(NULL); // Default
		ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (global && global->IsCheckSumValid()) {
			ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *scInfo = global->scenarioInformation;
			if (scInfo && scInfo->IsCheckSumValid()) {
				popup->SetVarToUpdate_lengthenCombatMode(&scInfo->enableLengthenCombatMode);
			}
		}
	}
	return result;
}


// Opens the custom "edit map size" popup in editor
// Returns true if OK.
bool CustomRORMainInterface::OpenEditMapSizePopup() {
	return (this->OpenCustomGamePopup<EditMapSizeXYPopup>(320, 130, true) != NULL);
}


// Opens the custom "edit terrain" popup in editor
// Returns true if OK.
bool CustomRORMainInterface::OpenCustomTerrainEditPopup() {
	return (this->OpenCustomGamePopup<EditTerrainPopup>(400, 300, true) != NULL);
}


// Returns true if custom map generation has been executed. False if we want to let original game code apply.
bool CustomRORMainInterface::ScenarioEditor_callMyGenerateMapIfRelevant() {
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) { return false; }
	ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_GetScreenFromName(scenarioEditorScreenName);
	assert(scEditor && scEditor->IsCheckSumValid());
	if (!scEditor || !scEditor->IsCheckSumValid()) { return false; }
	assert(scEditor->map_cbb_mapSize);
	if (!scEditor->map_cbb_mapSize) { return false; }
	long int mapSizeIndex = AOE_GetComboSelectedIndex(scEditor->map_cbb_mapSize);
	if (mapSizeIndex != 6) { return false; }
	// Here: custom map size
	if (!this->crCommand->crInfo->configInfo.useCustomMapDimensions) { return false; }
	// If result is true, WE generated the map ourselves => Disable standard generation code.
	this->OpenEditMapSizePopup();
	return true;
}


// Manage right button release action on selected units for given player
// Returns true if a red cross sign should be displayed (a relevant action occurred)
bool CustomRORMainInterface::ApplyRightClickReleaseOnSelectedUnits(ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_SUB1 *UIGameMain,
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player, long int mousePosX, long int mousePosY) {
	if (!UIGameMain || !UIGameMain->IsCheckSumValid()) { return false; }
	assert(UIGameMain != NULL);
	ROR_STRUCTURES_10C::STRUCT_PLAYER *controlledPlayer = UIGameMain->humanControlledPlayer;
	if (controlledPlayer == NULL) { return false; } // I suppose it shouldn't happen but I don't know all behaviours for this variable
	unsigned long int unitCount = controlledPlayer->selectedUnitCount;
	if (unitCount == 0) { return false; }

	if (!this->crCommand->crInfo->configInfo.enableSpawnUnitsMoveToLocation) { return false; }
	// Get relevant selected units array
	bool result = false;
	ROR_STRUCTURES_10C::STRUCT_UNIT **selectedUnits = this->crCommand->crInfo->GetRelevantSelectedUnitsPointer(controlledPlayer);

	ROR_STRUCTURES_10C::STRUCT_TEMP_MAP_POSITION_INFO posInfos;
	long int callResult;
	_asm {
		LEA EAX, posInfos
		PUSH 1
		PUSH 0
		PUSH EAX // posInMap(Y,X)
		PUSH mousePosY
		PUSH mousePosX
		PUSH 0
		PUSH 0x28
		MOV EAX, 0x51A650
		MOV ECX, UIGameMain
		CALL EAX // get map position from mouse pos
		MOV callResult, EAX
		MOV ECX, UIGameMain
		LEA EAX, posInfos
		PUSH EAX // arg1=posInfos
		MOV EAX, 0x51B070
		CALL EAX // Fit position in map bounds
	}
	// callResult== 0x33 if ok ? dont know the meaning

	// First loop to collect info on selected units. Not really necessary at this point.
	bool hasSelectedBuildings = false;
	bool hasSelectedLivings = false;
	for (unsigned long int index = 0; index < unitCount; index++) {
		ROR_STRUCTURES_10C::STRUCT_UNIT *unit = selectedUnits[index];
		if (unit && unit->IsCheckSumValid() && (unit->ptrStructPlayer == controlledPlayer)) {
			hasSelectedBuildings = hasSelectedBuildings || unit->unitType == GUT_BUILDING;
			hasSelectedLivings = hasSelectedLivings || unit->unitType == GUT_LIVING_UNIT;
		}
	}

	for (unsigned long int index = 0; index < unitCount; index++) {
		ROR_STRUCTURES_10C::STRUCT_UNIT *unit = selectedUnits[index];
		// Make sure unit is valid, from MY player
		if (unit && unit->IsCheckSumValid() && (unit->ptrStructPlayer == controlledPlayer)) {
			if (unit->unitType == GUT_BUILDING) {
				ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = unit->ptrStructDefUnit;
				assert(unitDef != NULL);
				if (unitDef && (unitDef->unitType == GUT_BUILDING) && (unit->unitStatus == 2) &&
					(!hasSelectedLivings) && // Do not apply auto-move when both livings and buildings are selected (building selection is probably unintentional)
					(DoesBuildingTrainUnits(unitDef->DAT_ID1))) {
					// Our right-click treatments are limited to buildings that can train units (not towers, houses...)

					// If the player clicked ON the building itself, cancel auto-move.
					if ((abs(posInfos.posX - unit->positionX) < 2) && (abs(posInfos.posY - unit->positionY) < 2)) {
						if (this->crCommand->crInfo->myGameObjects.RemoveUnitCustomInfo(unit->unitInstanceId)) {
							CallWriteText("Disabled auto-move for new units for selected building");
						}
					} else {
						// For "valid clicks" (not on building itself), add building+click info (target unit or position) to list

						// Is there a unit to interact with at this position ?
						long int targetUnitId = -1;
						ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
						assert(global && global->IsCheckSumValid());
						if (!global || !global->IsCheckSumValid()) { return false; }

						ROR_STRUCTURES_10C::STRUCT_UNIT *unitUnderMouse = GetUnitAtMousePosition(mousePosX, mousePosY, false);
						if (unitUnderMouse) {
							targetUnitId = unitUnderMouse->unitInstanceId;
						}

						// Add to list (or update) to set new units target position/unit
						UnitCustomInfo *unitInfo = this->crCommand->crInfo->myGameObjects.FindOrAddUnitCustomInfo(unit->unitInstanceId);
						if (unitInfo) {
							unitInfo->spawnTargetUnitId = targetUnitId;
							unitInfo->spawnUnitMoveToPosX = posInfos.posX;
							unitInfo->spawnUnitMoveToPosY = posInfos.posY;
							result = true;
						}
					}
				}
			}
		}
	}
	return result;
}

