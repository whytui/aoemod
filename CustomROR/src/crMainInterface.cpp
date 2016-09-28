#include "../include/crMainInterface.h"


namespace CUSTOMROR {


// Static global object
CustomRORMainInterface CUSTOMROR::crMainInterface;


CustomRORMainInterface::CustomRORMainInterface() {
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
// Warning: UI components may catch and handle events before so this method is NOT called. See crCommand.OnGameCommandButtonClick.
bool CustomRORMainInterface::GameAndEditor_OnKeyPress(long int pressedKey, bool CTRL, bool SHIFT, bool ALT) {
	AOE_STRUCTURES::STRUCT_ANY_UI **pCurrentUI = (AOE_STRUCTURES::STRUCT_ANY_UI **)AOE_OFFSETS::ADDR_VAR_ACTIVE_UI_STRUCT;
	assert(pCurrentUI != NULL);
	AOE_STRUCTURES::STRUCT_ANY_UI *currentUI = *pCurrentUI;
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings != NULL);
	if (!settings) { return false; }
	bool isInEditor = (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR);
	if (isInEditor) {
		// Consistency check.
		assert((currentUI->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MAIN) ||
			(currentUI->checksum == CHECKSUM_UI_SAVE_AS) ||
			(currentUI->checksum == CHECKSUM_UI_LOAD));
		if (currentUI->checksum != CHECKSUM_UI_SCENARIO_EDITOR_MAIN) { return false; }
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

	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return false; } // May occur when a key is pressed and the game just opened

	// Disable ALT-F4 when it would conflict with custom dialog
	// (isInGame) condition is optional. If removed, ALT-F4 will be disabled in scenario editor when our dialog is on.
	// WARNING: we can't prevent this to occur if user clicks on the top-right corner cross :(
	if ((isInGame) && (ALT) && (pressedKey == VK_F4) && (CUSTOMROR::crInfo.customYesNoDialogVar != NULL)) {
		// Disable ALT-F4 when our custom dialog is ON (based on the same dialog, would provoke conflicts and our dialog would stay ON forever)
		return true;
	}

	// Handle more Units shortcuts using numpad keys 0-9
	if ((isInGame) && (!isMenuOpen) && (pressedKey >= VK_NUMPAD0) && (pressedKey <= VK_NUMPAD9)) {
		long int shortcutNumber = GetShortcutInternalValueFromKey(pressedKey);
		if (CTRL) {
			AssignShortcutToSelectedUnits(GetControlledPlayerStruct_Settings(), shortcutNumber);
		} else {
			SelectUnitsUsingShortcut(GetControlledPlayerStruct_Settings(), shortcutNumber, SHIFT);
		}
	}

	// If Key=F10 (0x79) and "current UI"=scenario editor then open menu. 
	// Do NOT do this from other screens or when a popup is already opened !!! Never ! Or game will crash afterwards.
	if ((pressedKey == VK_F10) && isInEditor && !isMenuOpen && (!CUSTOMROR::crInfo.HasOpenedCustomGamePopup())) {
		// Additional check
		assert(GetGameSettingsPtr()->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR);
		if (pCurrentUI) {
			AOE_EditorOpenMenu((AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)*pCurrentUI);
		}
	}

	// ":"(FR) or "," (US) in game: select next idle military unit
	if ((isInGame) && (!isMenuOpen) && (pressedKey == VK_OEM_2)) {
		CUSTOMROR::crCommand.SelectNextIdleMilitaryUnit();
	}

	// F9 in game: bring idle military units to current location (screen position).
	// Requires ManageAI !
	if ((pressedKey == VK_F9) && isInGame && !isMenuOpen) {
		if (CUSTOMROR::crInfo.configInfo.enableCallNearbyIdleMilitaryUnits) {
			CUSTOMROR::crCommand.CallNearbyIdleMilitaryUnits();
		}
	}

	// F2 in game: unit properties
	if ((isInGame) && (!isMenuOpen) && (pressedKey == VK_F2)) {
		this->OpenInGameUnitPropertiesPopup();
	}

	// ESC (1B) - close custom dialog if opened
	if ((isInEditor || isInGame) && ((pressedKey == VK_ESCAPE) /*|| (pressedKey == VK_RETURN)*/) && (CUSTOMROR::crInfo.HasOpenedCustomGamePopup())) {
		this->CloseCustomGamePopup(true);
	}

	// F1 in editor : switch to unit selection
	if (!isMenuOpen && (isInEditor) && !CTRL && (pressedKey == VK_F1) && (!CUSTOMROR::crInfo.HasOpenedCustomGamePopup())) {
		if (!SHIFT) {
			settings->mouseActionType = AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_NORMAL;
		} else {
			STRUCT_POSITION_INFO mousePosInfo = GetMousePosition();
			AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitAtMousePosition(mousePosInfo.posX, mousePosInfo.posY, INTERACTION_MODES::CST_IM_LIVING_UNITS, true);
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
			if (player && player->IsCheckSumValid() && unit && unit->IsCheckSumValidForAUnitClass()) {
				SelectOneUnit(player, unit, false);
			}
		}
	}
	// F2 in editor: edit selected unit or show game coordinates at mouse position
	if (!isMenuOpen && (isInEditor) && (pressedKey == VK_F2) && (!CUSTOMROR::crInfo.HasOpenedCustomGamePopup())) {
		if (CUSTOMROR::crInfo.GetMainSelectedUnit(GetControlledPlayerStruct_Settings()) == NULL) {
			float posX, posY;
			GetGamePositionUnderMouse(&posX, &posY);
			if ((posX > 0) && (posY > 0)) {
				char buffer[100];
				const char *text = localizationHandler.GetTranslation(CRLANG_ID_MOUSE_POSITION, "Mouse position");
				sprintf_s(buffer, "%.70s: X=%4.2f, y=%4.2f", text, posX, posY);
				this->OpenCustomTextEditPopup(text, buffer, 280, 110, sizeof(buffer), NULL, true, false);
			}
		} else {
			this->OpenCustomEditorEditUnitPopup();
		}
	}
	// F3 in editor: scenario information
	if (!isMenuOpen && (isInEditor) && (pressedKey == VK_F3) && (!CUSTOMROR::crInfo.HasOpenedCustomGamePopup())) {
		this->OpenCustomEditorScenarioInfoPopup();
	}

	// CTRL-F1 : display messages
	if (!isMenuOpen && CTRL && (pressedKey == VK_F1)) {
		this->OpenTraceMessagePopup();
	}

	// F4 in editor: copy map
	if (!isMenuOpen && isInEditor && !CTRL && !ALT && (pressedKey == VK_F4) && (!CUSTOMROR::crInfo.HasOpenedCustomGamePopup())) {
		this->OpenCustomGamePopup<MapCopyPopup>(600, 450, false);
	}

#ifdef _DEBUG
	// TEST - F7 - debug only
	if (!isMenuOpen && (isInEditor || isInGame) && (pressedKey == VK_F7)) {
		unsigned long int *marray = (unsigned long int *)AOE_OFFSETS::UNKNOWN_ARRAY_6A18C0;
		AOE_STRUCTURES::STRUCT_UNKNOWN_MAP_DATA_F04C *md1 = (AOE_STRUCTURES::STRUCT_UNKNOWN_MAP_DATA_F04C *) 0x583BC8;
		AOE_STRUCTURES::STRUCT_UNKNOWN_MAP_DATA_F04C *md2 = (AOE_STRUCTURES::STRUCT_UNKNOWN_MAP_DATA_F04C *) 0x6A1CC0;
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
		
		//TEST exp
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		//CR_DEBUG::exportInfAIExplorationToBitmap(player);
		//CR_DEBUG::exportVisibilityToBitmap(true, -1);
		//CR_DEBUG::exportVisibilityToBitmap(false, 1);
		CR_DEBUG::exportGameTerrainRestrictionValuesToBitmap();
		/*_BITMAP::BitmapExporter::ExportDataAsBitmapGreyShades("D:\\test1.bmp", global->gameMapInfo->mapArraySizeX, 
			global->gameMapInfo->mapArraySizeY, global->gameMapInfo->unknown_8DB0, 0, 255, false);*/
		if (player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid()) {
			AOE_STRUCTURES::STRUCT_UNIT_GROUP_ELEM *fakeGroup = &player->ptrAIStruct->structTacAI.fakeFirstUnitGroupElem;
			AOE_STRUCTURES::STRUCT_UNIT_GROUP_ELEM *curGroup = fakeGroup->next;
			while (curGroup && (curGroup != fakeGroup)) {
				if ((curGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_LAND_ATTACK) && (curGroup->commanderUnitId >= 0)) {
					//AOE_STRUCTURES::STRUCT_UNIT_BASE *unitWithShortcut1 = FindUnitWithShortcutNumberForPlayer(player, 1);
					AOE_STRUCTURES::STRUCT_UNIT_BASE *unitWithShortcut1 = CUSTOMROR::crInfo.GetMainSelectedUnit(player);
					//curGroup->currentTask = UNIT_GROUP_TASK_IDS::CST_UGT_EXTERMINATE;
					if (unitWithShortcut1 && unitWithShortcut1->IsCheckSumValidForAUnitClass()) {
						for (int i = 0; i < curGroup->targetUnitIdArrayUsedElemCount; i++) {
							long int targetUnitId = curGroup->targetUnitIdArray[i];
							AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = global->GetUnitFromId(targetUnitId);
							if (targetUnit && targetUnit->IsCheckSumValidForAUnitClass()) {
								AOE_STRUCTURES::STRUCT_PLAYER *targetPlayer = targetUnit->ptrStructPlayer;
								int tpid = targetPlayer->playerId;
							}
							curGroup->targetUnitIdArray[i] = -1;
						}
						curGroup->targetUnitIdArrayUsedElemCount = 0;

						curGroup->taskSubTypeId = -1;
						curGroup->currentTask = UNIT_GROUP_TASK_IDS::CST_UGT_ATTACK_ROUNDUP_TARGET;
						curGroup->targetPlayerId = player->playerId;
						curGroup->targetUnitId = unitWithShortcut1->unitInstanceId;
						curGroup->targetPosX = unitWithShortcut1->positionX;
						curGroup->targetPosY = unitWithShortcut1->positionY;
						AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unitWithShortcut1->unitDefinition;
						curGroup->targetDAT_ID = -1;
						if (unitDef && unitDef->IsCheckSumValidForAUnitClass()) {
							curGroup->targetDAT_ID = unitDef->DAT_ID1;
						}
						curGroup->unknown_resetOrg = 1;
						unsigned long int addr = 0x4CD2D0;
						AOE_STRUCTURES::STRUCT_AI *mainAI = player->ptrAIStruct;
						AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = &player->ptrAIStruct->structTacAI;
						long int taskId = (long int)curGroup->currentTask;
						_asm {
							MOV ECX, curGroup;
							PUSH 0;
							PUSH 0;
							PUSH taskId;
							PUSH mainAI;
							PUSH tacAI;
							CALL addr;
						}
					}
				}
				curGroup = curGroup->next;
			}
			player->ptrAIStruct->structTacAI.targetInfo.targetUnitId = -1;
			player->ptrAIStruct->structTacAI.targetInfo.infAIUnitElemListIndex++;
			
		}
	}

	// TEST - F8 - show dialog
	if (!isMenuOpen && (isInEditor || isInGame) && (pressedKey == VK_F8) && (!CUSTOMROR::crInfo.HasOpenedCustomGamePopup())) {
		std::string sTechTreeInfo = "";
		int techToShowCount = 0;
		static char buffer[1024] = "\0";
		char *posInBuf = buffer;
		AOE_STRUCTURES::STRUCT_PLAYER *player = NULL;
		AOE_STRUCTURES::STRUCT_UNIT_BASE *selectedUnit = NULL;
		if (global) {
			player = GetPlayerStruct(global->humanPlayerId);
		}
		AOE_STRUCTURES::STRUCT_UNIT_BASE **unitArray = CUSTOMROR::crInfo.GetRelevantSelectedUnitsPointer(player);
		if (unitArray) {
			selectedUnit = unitArray[0];
		}
		AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *tmp = (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *)selectedUnit;
		/*if (tmp->IsCheckSumValid()) {
			if ((tmp->resourceValue > 0) && (tmp->unitDefinition->resourceDecay == 0)) {
				tmp->resourceValue = 0;
				if (tmp->hasDedicatedUnitDef) {
					tmp->unitDefinition->resourceDecay = 5;
				}
				return false;
			}
		}*/

		if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) {
			// Editor
			if (player && (player->selectedUnitCount > 0)) {
				if (selectedUnit) {
					sprintf_s(posInBuf, 200, "unitId=%ld/0x%X posX=%f posY=%f status=%d\n",
						selectedUnit->unitInstanceId, selectedUnit->unitInstanceId, selectedUnit->positionX, selectedUnit->positionY, selectedUnit->unitStatus);
					posInBuf = posInBuf + strlen(posInBuf);
					if (selectedUnit->unitDefinition) {
						sprintf_s(posInBuf, 200, "unitDefId=%d / %d", selectedUnit->unitDefinition->DAT_ID1, selectedUnit->unitDefinition->DAT_ID2);
						posInBuf = posInBuf + strlen(posInBuf);
					}
				}
			}
			// TO DO write buffer
			AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scinfo = global->scenarioInformation;
			AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *gmapinfo = global->gameMapInfo;
			AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO *tile = gmapinfo->GetTileInfo(1, 1);
			int a = tile->terrainData.GetTerrainId();
			bool b = tile->terrainData.SetAltitude(3);
			bool c = tile->terrainData.SetTerrainId(4);
			a = a + 1;

			AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *se = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_GetScreenFromName(scenarioEditorScreenName);
			if (se != NULL) {
				int a = se->diamondMap->checksum;
				AOE_STRUCTURES::STRUCT_TERRAIN_DEF *t = &se->diamondMap->gameMapInfo->terrainDefinitions[0];
				se->diamondMap->gameMapInfo->terrainDefinitions;
				se->unknown_580->checksum;//
				se->unknown_59C->checksum;//
				se->trn_lst_terrainList->checksum;
				se->unknown_5AC->sizeX += 50;//
				//char buffer[111];
				//sprintf_s(buffer, "%08X, %08X", se, se->diamondMap);
				//this->OpenCustomDialogMessage(buffer, 100, 100);
				//bool b = se->gamePlayUIZone->IsCheckSumValid();
				//AOE_STRUCTURES::STRUCT_UI_BUTTON *btn = se->btnTabs[0];
				//se->gamePlayUIZone;
				//se->map_btn_random->visible; // map btn randm

				int aa = 1 + 1;
			}
		}
		AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *selectedUnitT50 = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)selectedUnit;
		AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameScreen = GetGameSettingsPtr()->ptrGameUIStruct;
		if (IsGameRunning()) {
			// During Game

			selectedUnit = gameScreen->panelSelectedUnit;
			AOE_STRUCTURES::STRUCT_PLAYER *player = NULL;
			if (selectedUnit) { player = selectedUnit->ptrStructPlayer; }
			if (selectedUnit) {
				long int actionTargetUnitId = -1;
				unsigned long int addraction = -1;
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = selectedUnit->unitDefinition;
				AOE_STRUCTURES::STRUCT_UNIT_ACTION_INFO *ainfo = NULL;
				if (selectedUnitT50 && selectedUnitT50->DerivesFromAttackable()) {
					ainfo = selectedUnitT50->ptrActionInformation;
				}
				if (ainfo && (ainfo->ptrActionLink) && (ainfo->ptrActionLink->actionStruct)) {
					actionTargetUnitId = ainfo->ptrActionLink->actionStruct->targetUnitId;
					addraction = (unsigned long int)ainfo->ptrActionLink->actionStruct;
				}
				AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *unitActivity = selectedUnit->currentActivity;
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
				AOE_STRUCTURES::STRUCT_AI *ai = player->ptrAIStruct;
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
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = selectedUnit->unitDefinition;
				assert(unitDef && unitDef->IsCheckSumValidForAUnitClass());
				if (unitDef) {
					char nameBuffer[50];
					short int researchCount = player->ptrResearchesStruct->researchCount;
					AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS *rs = player->ptrResearchesStruct->researchStatusesArray; // ->currentStatus
					sTechTreeInfo = "";
					for (int rid = 0; rid < researchCount; rid++) {
						if (rs[rid].currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) {
							if (player->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef(rid)->researchLocation == unitDef->DAT_ID1) {
								techToShowCount++;
								if (techToShowCount > 0) { sTechTreeInfo += "\n"; }
								sTechTreeInfo += "techId ";
								sTechTreeInfo += std::to_string(player->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef(rid)->technologyId);
								sTechTreeInfo += " = ";
								*nameBuffer = 0; // Reset string
								GetLanguageDllText(player->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef(rid)->languageDLLName, nameBuffer, sizeof(nameBuffer) - 1,
									player->ptrResearchesStruct->ptrResearchDefInfo->GetResearchDef(rid)->researchName);
								sTechTreeInfo += nameBuffer;
							}
						}
					}
				}
			}
			//TMP
			std::string ss = "";
			AOE_STRUCTURES::STRUCT_PLAYER *humanPlayer = GetControlledPlayerStruct_Settings();
			assert(humanPlayer && humanPlayer->IsCheckSumValid());
			if (selectedUnitT50 && selectedUnitT50->DerivesFromAttackable() && (player->playerId == global->humanPlayerId)) {
				if (selectedUnitT50->ptrActionInformation && selectedUnitT50->ptrActionInformation->ptrActionLink &&
					selectedUnitT50->ptrActionInformation->ptrActionLink->actionStruct &&
					selectedUnitT50->ptrActionInformation->ptrActionLink->actionStruct->unsure_resourceValue) {
				}
				long int u = selectedUnitT50->unitInstanceId;
				long int t = -1;
				//CreateCmd_RightClick(u, t, 40, 20);
				//return false;
				//CreateCmd_Build(u, 110, 1, 1);
			}
		}


		if (this->OpenCustomGamePopup<CustomPopupBase>(580, 460, false)) {
			unsigned long int *unused;
			AOE_AddLabel(CUSTOMROR::crInfo.GetCustomGamePopup(), (AOE_STRUCTURES::STRUCT_UI_LABEL**)&unused, buffer, 60, 20, 520, 160);
			if (!sTechTreeInfo.empty() && (techToShowCount > 0)) {
				AOE_AddLabel(CUSTOMROR::crInfo.GetCustomGamePopup(), (AOE_STRUCTURES::STRUCT_UI_LABEL**)&unused, "(future) Tech tree available items", 60, 180, 200, 20);
				AOE_AddLabel(CUSTOMROR::crInfo.GetCustomGamePopup(), (AOE_STRUCTURES::STRUCT_UI_LABEL**)&unused, (char*)sTechTreeInfo.c_str(), 60, 200, 240, 16 * (techToShowCount+1));
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
bool CustomRORMainInterface::CreateGameCustomRorOptionsPopup(AOE_STRUCTURES::STRUCT_ANY_UI *previousPopup) {
	assert(previousPopup != NULL);
	if (!previousPopup) { return false; }

	assert(!CUSTOMROR::crInfo.HasOpenedCustomGamePopup()); // Custom popup is not already open
	if (CUSTOMROR::crInfo.HasOpenedCustomGamePopup()) { return true; } // do nothing, but not treated as alloc error
	
	InGameCustomRorOptionsPopup *popupUIObject = this->CreateCustomPopupObject<InGameCustomRorOptionsPopup>();
	if (popupUIObject) {
		popupUIObject->CloseMenuAndOpenPopup(previousPopup);
	}

	if (!popupUIObject) {
		return false; // need to change return address
	}
	// Dirty workaround because custom options popup is not created using standard procedures :(
	CUSTOMROR::crInfo.ForceSetCurrentGamePopup(popupUIObject->GetAOEPopupObject(), popupUIObject->customOptionButtonVar, NULL);
	
	return true;
}


// Ensure exiting game won't crash due to UI pointer issues related to custom popup (occurs in scenario editor)
// Returns true if we want NOT the game to exit
bool CustomRORMainInterface::FixGamePopupIssuesBeforeGameClose() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (CUSTOMROR::crInfo.HasOpenedCustomGamePopup() && settings &&
		(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR)) {
		// In scenario editor, UI objects are not correctly freed and the game crashes if we have a custom popup opened...
		// Closing the popup solves the issue !
		this->CloseCustomGamePopup(true);
		assert(!CUSTOMROR::crInfo.HasOpenedCustomGamePopup());
		return true; // Do NOT exit game (if possible). We consider the ALT-F4 closed the popup (that's a choice...)
	}
	return false;
}


// Returns NULL if failed (you can't create a custom popup object if previous one is still opened).
// If result is non-NULL, you can call [result]->OpenPopup() to actually open the popup.
template<class popupType> popupType *CustomRORMainInterface::CreateCustomPopupObject() {
	if (CUSTOMROR::crInfo.HasOpenedCustomGamePopup()) {
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
	this->currentCustomPopup = (CustomPopupBase*) new popupType();
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
		CustomPopupBase *next = this->currentCustomPopup->nextPopup;
		delete this->currentCustomPopup;
		this->currentCustomPopup = next; // Generally NULL unless closing the popup triggered a new one.

		if ((next == NULL) && traceMessageHandler.HasUnreadMessages()) {
			this->OpenTraceMessagePopup();
		}
	}
}


// Called for ALL button clicks in the game.
// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool CustomRORMainInterface::Global_OnButtonClick(unsigned long int objAddress) {
	if (CUSTOMROR::crInfo.IsCustomGamePopupOKButton(objAddress)) {
		// Close the custom popup when it is open and we just clicked on OK button.
		this->CloseCustomGamePopup(false);
		return true;
	}
	if (CUSTOMROR::crInfo.IsCustomGamePopupCancelButton(objAddress)) {
		// Close the custom popup when it is open and we just clicked on Cancel button.
		this->CloseCustomGamePopup(true);
		return true;
	}
	AOE_STRUCTURES::STRUCT_ANY_UI *obj = (AOE_STRUCTURES::STRUCT_ANY_UI *)objAddress;

	AOE_STRUCTURES::STRUCT_UI_MAIN_INFO *uiMainInfo = GetUIMainInfoStruct();
	if (uiMainInfo && uiMainInfo->previousFocusedObject) {
		AOE_STRUCTURES::STRUCT_UI_TEXTBOX *prevFocusAsTextbox = (AOE_STRUCTURES::STRUCT_UI_TEXTBOX *)uiMainInfo->previousFocusedObject;
		if (prevFocusAsTextbox && prevFocusAsTextbox->IsCheckSumValid() && ((unsigned long int)prevFocusAsTextbox != objAddress)) {
			// Prevent textbox from automatically receiving focus (after a combobox is closed, for example)
			// This fixes textboxes that sometimes remain active (and not destroyed) when closing popups with ESC or ALT-F4.
			// This fixes scenario editor's edit text boxes that never lose focus :)
			// Note that we don't do this if current event's component IS a checkbox
			uiMainInfo->previousFocusedObject = NULL;
		}
	}

	//Custom checkboxes: check/uncheck + manage custom buttons onclick
	if (CUSTOMROR::crInfo.HasOpenedCustomGamePopup()) {
		AOE_STRUCTURES::STRUCT_UI_BUTTON *objAsButton = (AOE_STRUCTURES::STRUCT_UI_BUTTON *)objAddress;
		if (objAsButton->IsCheckSumValid()) {
			if (objAsButton->IsACheckBox()) {
				// We should NOT do this for standard buttons
				AOE_CheckBox_SetChecked(objAsButton, !objAsButton->IsChecked());
			}
			return this->OnCustomButtonClick(objAsButton); // run for checkboxes also.
		}
		AOE_STRUCTURES::STRUCT_UI_BUTTON_COMBOBOX *objAsComboboxBtn = (AOE_STRUCTURES::STRUCT_UI_BUTTON_COMBOBOX *)objAddress;
		if (objAsComboboxBtn->IsCheckSumValid()) {
			// Is there something to do here to get those comboboxes work properly ?
			
		}
	}

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();

	// Scenario editor
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *se = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_GetScreenFromName(scenarioEditorScreenName);
	if ((se != NULL) && settings && (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR)) {
		// Manage (overload) scenario editor buttons
		if (obj == se->map_btn_generateMap) {
			return this->ScenarioEditor_callMyGenerateMapIfRelevant();
		}
	}

	// Game menu
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MENU *menu = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MENU *)AOE_GetScreenFromName(menuDialogScreenName);
	if (menu && menu->IsCheckSumValid() && settings && (obj->ptrParentObject == menu)) {
		AOE_STRUCTURES::STRUCT_UI_BUTTON *objAsButton = (AOE_STRUCTURES::STRUCT_UI_BUTTON *)objAddress;
		if (objAsButton->IsCheckSumValid()) {
			// True for buttons that do not trigger a call to ManageOptionButtonClickInMenu
			bool btnEventNotCaughtByRORMenuBtnEvent = (objAsButton->commandIDs[0] == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_SAVE) ||
				(objAsButton->commandIDs[0] == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_LOAD) ||
				(objAsButton->commandIDs[0] == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_QUIT) ||
				(objAsButton->commandIDs[0] == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_HELP);
			if (btnEventNotCaughtByRORMenuBtnEvent) {
				this->FreeInGameCustomOptionsButton(); // Free custom button if needed, for buttons not handled in ManageOptionButtonClickInMenu.
				CUSTOMROR::crInfo.ForceClearCustomMenuObjects(); // Here we CAN already free custom button because it's not in use (current event is about another button)
			}
		}
	}

	return false; // Normal case: let game handle event normally.
}


// Use this to properly free custom button from options menu. This should (always) be called when ingame menu is closed.
void CustomRORMainInterface::FreeInGameCustomOptionsButton() {
	if (CUSTOMROR::crInfo.configInfo.showCustomRORMenu) {
		// Add "custom options" button (from game menu) to our "garbage collector" so that it is freed later.
		if (CUSTOMROR::crInfo.customGameMenuOptionsBtnVar) {
			CUSTOMROR::crInfo.AddObjectInPopupContentList(CUSTOMROR::crInfo.customGameMenuOptionsBtnVar);
			CUSTOMROR::crInfo.customGameMenuOptionsBtnVar = NULL;
		}
	}
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool CustomRORMainInterface::OnCustomButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) {
	if (!sender || !sender->IsCheckSumValidForAChildClass()) { return false; }
	bool doCloseCustomPopup = false;
	if (!GetGameGlobalStructPtr()) { return false; }
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = GetGameGlobalStructPtr()->scenarioInformation;

	if (this->currentCustomPopup) {
		bool eventHandled = this->currentCustomPopup->OnButtonClick(sender);
		if (this->currentCustomPopup->IsClosed()) {
			CustomPopupBase *next = this->currentCustomPopup->nextPopup;
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
bool CustomRORMainInterface::OpenCustomTextEditPopup(const char *title, char *initialValue, long int sizeX, long int sizeY, 
	long int maxLength, char *outputBuffer, bool readOnly, bool withCancelBtn) {
	SimpleEditTextPopup *p = this->OpenCustomGamePopup<SimpleEditTextPopup>(sizeX, sizeY, withCancelBtn);
	if (p) {
		p->AddPopupContent(title, initialValue, maxLength, outputBuffer, readOnly);
	}
	return (p != NULL);
}


// Open a popup with CustomROR (debug) messages
bool CustomRORMainInterface::OpenTraceMessagePopup() {
	bool reverseOrder = CUSTOMROR::crInfo.configInfo.showLogsInReverseOrder;
	// Double spaces are intented (to compensate weird-looking font)
	char *title = "Show  debug  messages";
	if (reverseOrder) {
		title = "Show  debug  messages (reverse order)";
	}
	return this->OpenCustomTextEditPopup(title, (char*)traceMessageHandler.GetAllMessages(reverseOrder).c_str(),
		780, 590,
		0, NULL, false /* NOT read only so that scrollbar is active*/, false);
}


// Open the relevant "view/edit unit" popup for currently selected unit.
// Returns true if successful.
bool CustomRORMainInterface::OpenInGameUnitPropertiesPopup() {
	AOE_STRUCTURES::STRUCT_PLAYER *humanPlayer = GetControlledPlayerStruct_Settings();
	if (!humanPlayer || !humanPlayer->IsCheckSumValid()) {
		return false;
	}
	if (humanPlayer->selectedUnitCount <= 0) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = CUSTOMROR::crInfo.GetRelevantSelectedUnitsPointer(humanPlayer);
	assert(selectedUnits != NULL);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *selectedUnit = selectedUnits[0];
	if (!selectedUnit || !selectedUnit->IsCheckSumValidForAUnitClass()) {
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
bool CustomRORMainInterface::OpenInGameUnitPropertiesPopup(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	InGameUnitPropertiesPopup *popup = this->OpenCustomGamePopup<InGameUnitPropertiesPopup>(600, 500, true);
	if (popup == NULL) { return false; }
	popup->AddPopupContent(unit->unitInstanceId);
	return true;
}


// Opens the custom "edit unit" in editor
// Returns true if OK.
bool CustomRORMainInterface::OpenCustomEditorScenarioInfoPopup() {
	EditorScenarioInfoPopup *popup = this->OpenCustomGamePopup<EditorScenarioInfoPopup>(500, 400, false);
	bool result = (popup != NULL);
	if (popup) {
		popup->SetVarToUpdate_allowUnitOverlapping(&CUSTOMROR::crInfo.configInfo.editor_allowUnitOverlapping);
		popup->SetVarToUpdate_disableHillModeChecks(&CUSTOMROR::crInfo.configInfo.editor_disableHillModeChecks);
		popup->SetVarToUpdate_disableTerrainRestrictionChecks(&CUSTOMROR::crInfo.configInfo.editor_disableTerrainRestrictions);
		popup->SetVarToUpdate_lengthenCombatMode(NULL); // Default
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (global && global->IsCheckSumValid()) {
			AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = global->scenarioInformation;
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
	return (this->OpenCustomGamePopup<EditTerrainPopup>(500, 300, true) != NULL);
}


// Returns true if custom map generation has been executed. False if we want to let original game code apply.
bool CustomRORMainInterface::ScenarioEditor_callMyGenerateMapIfRelevant() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) { return false; }
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_GetScreenFromName(scenarioEditorScreenName);
	assert(scEditor && scEditor->IsCheckSumValid());
	if (!scEditor || !scEditor->IsCheckSumValid()) { return false; }
	assert(scEditor->map_cbb_mapSize);
	if (!scEditor->map_cbb_mapSize) { return false; }
	long int mapSizeIndex = scEditor->map_cbb_mapSize->GetSelectedIndex();
	if (mapSizeIndex != 6) { return false; }
	// Here: custom map size
	if (!CUSTOMROR::crInfo.configInfo.useCustomMapDimensions) { return false; }
	// If result is true, WE generated the map ourselves => Disable standard generation code.
	this->OpenEditMapSizePopup();
	return true;
}


// Manage right button release action on selected units for given player
// Returns true if a red cross sign should be displayed (a relevant action occurred)
bool CustomRORMainInterface::ApplyRightClickReleaseOnSelectedUnits(AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *UIGameMain,
	AOE_STRUCTURES::STRUCT_PLAYER *player, long int mousePosX, long int mousePosY) {
	if (!UIGameMain || !UIGameMain->IsCheckSumValid()) { return false; }
	assert(UIGameMain != NULL);
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = UIGameMain->controlledPlayer;
	if (controlledPlayer == NULL) { return false; } // I suppose it shouldn't happen but I don't know all behaviours for this variable
	long int unitCount = controlledPlayer->selectedUnitCount;
	if (unitCount == 0) { return false; }

	if (!CUSTOMROR::crInfo.configInfo.enableSpawnUnitsMoveToLocation) { return false; }
	// Get relevant selected units array
	bool result = false;
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = CUSTOMROR::crInfo.GetRelevantSelectedUnitsPointer(controlledPlayer);

	AOE_STRUCTURES::STRUCT_TEMP_MAP_POSITION_INFO posInfos;
	long int callResult;
	// TODO use AOE_GetGameInfoUnderMouse ?
	_asm {
		LEA EAX, posInfos
		PUSH 1
		PUSH 0
		PUSH EAX // STRUCT_TEMP_MAP_POSITION_INFO
		PUSH mousePosY
		PUSH mousePosX
		PUSH 0
		PUSH CST_MBA_RELEASE_CLICK //PUSH 0x28 // "click release"
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
	for (long int index = 0; index < unitCount; index++) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = selectedUnits[index];
		if (unit && unit->IsCheckSumValidForAUnitClass() && (unit->ptrStructPlayer == controlledPlayer)) {
			hasSelectedBuildings = hasSelectedBuildings || (unit->unitType == GUT_BUILDING);
			hasSelectedLivings = hasSelectedLivings || (unit->unitType == GUT_TRAINABLE);
		}
	}

	for (long int index = 0; index < unitCount; index++) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = selectedUnits[index];
		// Make sure unit is valid, from MY player
		if (unit && unit->IsCheckSumValidForAUnitClass() && (unit->ptrStructPlayer == controlledPlayer)) {
			if (unit->unitType == GUT_BUILDING) {
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
				assert(unitDef != NULL);
				if (unitDef && (unitDef->unitType == GUT_BUILDING) && (unit->unitStatus == 2) &&
					(!hasSelectedLivings) && // Do not apply auto-move when both livings and buildings are selected (building selection is probably unintentional)
					(DoesBuildingTrainUnits(unitDef->DAT_ID1))) {
					// Our right-click treatments are limited to buildings that can train units (not towers, houses...)

					// If the player clicked ON the building itself, cancel auto-move.
					if ((abs(posInfos.posX - unit->positionX) < 2) && (abs(posInfos.posY - unit->positionY) < 2)) {
						UnitCustomInfo *u = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(unit->unitInstanceId);
						if (u) {
							u->ResetSpawnAutoTargetInfo();
							CUSTOMROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(unit->unitInstanceId);
							CallWriteText("Disabled auto-move for new units for selected building");
						}
					} else {
						// For "valid clicks" (not on building itself), add building+click info (target unit or position) to list

						// Is there a unit to interact with at this position ?
						long int targetUnitId = -1;
						AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
						assert(global && global->IsCheckSumValid());
						if (!global || !global->IsCheckSumValid()) { return false; }

						AOE_STRUCTURES::STRUCT_UNIT_BASE *unitUnderMouse = GetUnitAtMousePosition(mousePosX, mousePosY, INTERACTION_MODES::CST_IM_LIVING_UNITS, false);
						if (unitUnderMouse && unitUnderMouse->IsCheckSumValidForAUnitClass()) {
							AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefUnderMouse = unitUnderMouse->unitDefinition;
							if (unitDefUnderMouse && unitDefUnderMouse->IsCheckSumValidForAUnitClass() &&
								(unitDefUnderMouse->interactionMode >= AOE_CONST_FUNC::INTERACTION_MODES::CST_IM_RESOURCES)
								) {
								targetUnitId = unitUnderMouse->unitInstanceId;
							}
						}

						// Add to list (or update) to set new units target position/unit
						UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(unit->unitInstanceId);
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

}