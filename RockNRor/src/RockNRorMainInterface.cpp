#include "../include/RockNRorMainInterface.h"


namespace ROCKNROR {


// Static global object
RockNRorMainInterface ROCKNROR::crMainInterface;


RockNRorMainInterface::RockNRorMainInterface() {
	windowTitleChanged = false;
}

RockNRorMainInterface::~RockNRorMainInterface() {
}




// Manage key press in game screen and editor.
// Returns true if event has been handled AND we don't want original code to try to handle it.
// Warning: UI components may catch and handle events before so this method is NOT called. See crCommand.OnGameCommandButtonClick.
bool RockNRorMainInterface::GameAndEditor_OnKeyPress(long int pressedKey, bool CTRL, bool SHIFT, bool ALT) {
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
	if ((isInGame) && (ALT) && (pressedKey == VK_F4) && (ROCKNROR::crInfo.customYesNoDialogVar != NULL)) {
		// Disable ALT-F4 when our custom dialog is ON (based on the same dialog, would provoke conflicts and our dialog would stay ON forever)
		return true;
	}

	// Handle more Units shortcuts using numpad keys 0-9
	if ((isInGame) && (!isMenuOpen) && (pressedKey >= VK_NUMPAD0) && (pressedKey <= VK_NUMPAD9)) {
		long int shortcutNumber = GetShortcutInternalValueFromKey(pressedKey);
		if (CTRL) {
			AOE_METHODS::UNIT::AssignShortcutToSelectedUnits(GetControlledPlayerStruct_Settings(), shortcutNumber);
		} else {
			AOE_METHODS::UNIT::SelectUnitsUsingShortcut(GetControlledPlayerStruct_Settings(), shortcutNumber, SHIFT);
		}
	}

	// If Key=F10 (0x79) and "current UI"=scenario editor then open menu. 
	// Do NOT do this from other screens or when a popup is already opened !!! Never ! Or game will crash afterwards.
	if ((pressedKey == VK_F10) && isInEditor && !isMenuOpen && (!ROCKNROR::crInfo.HasOpenedCustomGamePopup())) {
		// Additional check
		assert(GetGameSettingsPtr()->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR);
		if (pCurrentUI) {
			AOE_METHODS::AOE_EditorOpenMenu((AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)*pCurrentUI);
		}
	}

	// ":"(FR) or "," (US) in game: select next idle military unit
	if ((isInGame) && (!isMenuOpen) && (pressedKey == VK_OEM_2)) {
		ROCKNROR::UNIT::SelectNextIdleMilitaryUnit();
	}

	// F9 in game: bring idle military units to current location (screen position).
	// Requires ManageAI !
	if ((pressedKey == VK_F9) && isInGame && !isMenuOpen) {
		if (ROCKNROR::crInfo.configInfo.enableCallNearbyIdleMilitaryUnits) {
			ROCKNROR::UNIT::CallNearbyIdleMilitaryUnits();
		}
	}

	// F2 in game: unit properties
	if ((isInGame) && (!isMenuOpen) && (pressedKey == VK_F2)) {
		InGameUnitPropertiesPopup::OpenInGameUnitPropertiesPopup();
	}

	// ESC (1B) - close custom dialog if opened
	if ((isInEditor || isInGame) && ((pressedKey == VK_ESCAPE) /*|| (pressedKey == VK_RETURN)*/) && (ROCKNROR::crInfo.HasOpenedCustomGamePopup())) {
		ROCKNROR::customPopupSystem.CloseCustomGamePopup(true);
	}

	// F1 in editor : switch to unit selection
	if (!isMenuOpen && (isInEditor) && !CTRL && (pressedKey == VK_F1) && (!ROCKNROR::crInfo.HasOpenedCustomGamePopup())) {
		if (!SHIFT) {
			settings->mouseActionType = AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_NORMAL;
		} else {
			STRUCT_POSITION_INFO mousePosInfo = GetMousePosition();
			AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)AOE_METHODS::GetUnitAtMousePosition(mousePosInfo.posX, mousePosInfo.posY, INTERACTION_MODES::CST_IM_LIVING_UNITS, true);
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
			if (player && player->IsCheckSumValid() && unit && unit->IsCheckSumValidForAUnitClass()) {
				SelectOneUnit(player, unit, false);
			}
		}
	}
	// F2 in editor: edit selected unit or show game coordinates at mouse position
	if (!isMenuOpen && (isInEditor) && (pressedKey == VK_F2) && (!ROCKNROR::crInfo.HasOpenedCustomGamePopup())) {
		if (ROCKNROR::crInfo.GetMainSelectedUnit(GetControlledPlayerStruct_Settings()) == NULL) {
			float posX, posY;
			GetGamePositionUnderMouse(&posX, &posY);
			if ((posX > 0) && (posY > 0)) {
				char buffer[100];
				const char *text = localizationHandler.GetTranslation(CRLANG_ID_MOUSE_POSITION, "Mouse position");
				sprintf_s(buffer, "%.70s: X=%4.2f, y=%4.2f", text, posX, posY);
				SimpleEditTextPopup::OpenCustomTextEditPopup(text, buffer, 280, 110, sizeof(buffer), NULL, true, false);
			}
		} else {
			EditorEditUnitInfoPopup::OpenCustomEditorEditUnitPopup();
		}
	}
	// F3 in editor: scenario information
	if (!isMenuOpen && (isInEditor) && (pressedKey == VK_F3) && (!ROCKNROR::crInfo.HasOpenedCustomGamePopup())) {
		EditorScenarioInfoPopup::OpenCustomEditorScenarioInfoPopup();
	}

	// CTRL-F1 : display messages
	if (!isMenuOpen && CTRL && (pressedKey == VK_F1)) {
		SimpleEditTextPopup::OpenTraceMessagePopup();
	}

	// F4 in editor: copy map
	if (!isMenuOpen && isInEditor && !CTRL && !ALT && (pressedKey == VK_F4) && (!ROCKNROR::crInfo.HasOpenedCustomGamePopup())) {
		ROCKNROR::customPopupSystem.OpenCustomGamePopup<MapCopyPopup>(600, 450, false);
	}

	// F5 in game: set debug info level
	if (!isMenuOpen && isInGame && (pressedKey == VK_F5)) {
		ROCKNROR::crCommand.SetNextInGameDebugInfoLevel();
	}

#ifdef _DEBUG
	// TEST - F7 - debug only
	if (!isMenuOpen && (isInEditor || isInGame) && (pressedKey == VK_F7)) {
#ifdef GAMEVERSION_ROR10c
		unsigned long int *marray = (unsigned long int *)AOE_OFFSETS::UNKNOWN_ARRAY_6A18C0;
#endif
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
		//CR_DEBUG::exportVisibilityToBitmap(false, 2);
		//CR_DEBUG::exportGameTerrainRestrictionValuesToBitmap();
		/*_BITMAP::BitmapExporter::ExportDataAsBitmapGreyShades("D:\\test1.bmp", global->gameMapInfo->mapArraySizeX, 
			global->gameMapInfo->mapArraySizeY, global->gameMapInfo->unknown_8DB0, 0, 255, false);*/
	}

	// TEST - F8 - show dialog
	if (!isMenuOpen && (isInEditor || isInGame) && (pressedKey == VK_F8) && (!ROCKNROR::crInfo.HasOpenedCustomGamePopup())) {
		std::string sTechTreeInfo = "";
		int techToShowCount = 0;
		static char buffer[1024] = "\0";
		char *posInBuf = buffer;
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(global->humanPlayerId);
		AOE_STRUCTURES::STRUCT_UNIT_BASE *selectedUnit = NULL;
		AOE_STRUCTURES::STRUCT_UNIT_BASE **unitArray = ROCKNROR::crInfo.GetRelevantSelectedUnitsPointer(player);
		if (unitArray) {
			selectedUnit = unitArray[0];
		}
		float x, y;
		GetGamePositionUnderMouse(&x, &y);
		bool expl = AOE_STRUCTURES::PLAYER::IsExploredForPlayer(player, (long)x, (long)y);
		bool fogVis = AOE_STRUCTURES::PLAYER::IsFogVisibleForPlayer(player, (long)x, (long)y);

		STRUCT_UNKNOWN_MAP_INFO_7D2058 *m = *ROR_unknown_mapInfo_7D2058;
		STRUCT_VISIBLE_UNIT_INFO_SET *vs = m->GetUnknown_00_elem(player->playerId, GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian);
		if (vs) {
			for (int i = 0; i < vs->numberUsed; i++) {
				int otherPid = vs->nearbyUnitInfoArray[i].playerId;
				long int curUnitId = vs->nearbyUnitInfoArray[i].unitId;
				STRUCT_UNIT_BASE *curUnit = global->GetUnitFromId(curUnitId);
				long int seerPosX = vs->nearbyUnitInfoArray[i].posX;
				long int seerPosY = vs->nearbyUnitInfoArray[i].posY;
				long int initialDist = vs->nearbyUnitInfoArray[i].distance;
				
			}
		}

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
						(long int)unitActivity, unitActivity->currentActionId, unitActivity->orderTaskId, unitActivity->targetUnitId,
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
			}
		}

		if (ROCKNROR::customPopupSystem.OpenCustomGamePopup<CustomPopupBase>(580, 460, false)) {
			unsigned long int *unused;
			AOE_METHODS::UI_BASE::AddLabel(ROCKNROR::crInfo.GetCustomGamePopup(), (AOE_STRUCTURES::STRUCT_UI_LABEL**)&unused, buffer, 60, 20, 520, 160);
			if (!sTechTreeInfo.empty() && (techToShowCount > 0)) {
				AOE_METHODS::UI_BASE::AddLabel(ROCKNROR::crInfo.GetCustomGamePopup(), (AOE_STRUCTURES::STRUCT_UI_LABEL**)&unused, "(future) Tech tree available items", 60, 180, 200, 20);
				AOE_METHODS::UI_BASE::AddLabel(ROCKNROR::crInfo.GetCustomGamePopup(), (AOE_STRUCTURES::STRUCT_UI_LABEL**)&unused, (char*)sTechTreeInfo.c_str(), 60, 200, 240, 16 * (techToShowCount + 1));
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


// Called for ALL button clicks in the game.
// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool RockNRorMainInterface::Global_OnButtonClick(unsigned long int objAddress) {
	if (ROCKNROR::customPopupSystem.OnButtonClickClosePopupIfNecessary(objAddress)) {
		return true;
	}
	AOE_STRUCTURES::STRUCT_ANY_UI *obj = (AOE_STRUCTURES::STRUCT_ANY_UI *)objAddress;

	AOE_STRUCTURES::STRUCT_UI_PANEL_SYSTEM *uiMainInfo = GetUIMainInfoStruct();
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

	// Custom checkboxes: check/uncheck + manage custom buttons onclick
	if (ROCKNROR::crInfo.HasOpenedCustomGamePopup()) {
		AOE_STRUCTURES::STRUCT_UI_BUTTON *objAsButton = (AOE_STRUCTURES::STRUCT_UI_BUTTON *)objAddress;
		if (objAsButton->IsCheckSumValid()) {
			if (objAsButton->IsACheckBox()) {
				// We should NOT do this for standard buttons
				AOE_METHODS::UI_BASE::CheckBox_SetChecked(objAsButton, !objAsButton->IsChecked());
			}
			return ROCKNROR::customPopupSystem.OnCustomButtonClick(objAsButton); // run for checkboxes also.
		}
		AOE_STRUCTURES::STRUCT_UI_BUTTON_COMBOBOX *objAsComboboxBtn = (AOE_STRUCTURES::STRUCT_UI_BUTTON_COMBOBOX *)objAddress;
		if (objAsComboboxBtn->IsCheckSumValid()) {
			// Is there something to do here to get those comboboxes work properly ?
			
		}
	}

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();

	// Scenario editor
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *se = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_METHODS::GetScreenFromName(scenarioEditorScreenName);
	if ((se != NULL) && settings && (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR)) {
		// Manage (overload) scenario editor buttons
		if (obj == se->map_btn_generateMap) {
			return this->ScenarioEditor_callMyGenerateMapIfRelevant();
		}
	}

	// Game menu
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MENU *menu = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MENU *)AOE_METHODS::GetScreenFromName(menuDialogScreenName);
	if (menu && menu->IsCheckSumValid() && settings && (obj->ptrParentObject == menu)) {
		AOE_STRUCTURES::STRUCT_UI_BUTTON *objAsButton = (AOE_STRUCTURES::STRUCT_UI_BUTTON *)objAddress;
		if (objAsButton->IsCheckSumValid()) {
			// True for buttons that do not trigger a call to ManageOptionButtonClickInMenu
			bool btnEventNotCaughtByRORMenuBtnEvent = (objAsButton->commandIDs[0] == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_SAVE) ||
				(objAsButton->commandIDs[0] == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_LOAD) ||
				(objAsButton->commandIDs[0] == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_QUIT) ||
				(objAsButton->commandIDs[0] == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_HELP);
			if (btnEventNotCaughtByRORMenuBtnEvent) {
				ROCKNROR::customPopupSystem.FreeInGameCustomOptionsButton(); // Free custom button if needed, for buttons not handled in ManageOptionButtonClickInMenu.
				ROCKNROR::crInfo.ForceClearCustomMenuObjects(); // Here we CAN already free custom button because it's not in use (current event is about another button)
			}
		}
	}

	return false; // Normal case: let game handle event normally.
}


// Returns true if custom map generation has been executed. False if we want to let original game code apply.
bool RockNRorMainInterface::ScenarioEditor_callMyGenerateMapIfRelevant() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) { return false; }
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_METHODS::GetScreenFromName(scenarioEditorScreenName);
	assert(scEditor && scEditor->IsCheckSumValid());
	if (!scEditor || !scEditor->IsCheckSumValid()) { return false; }
	assert(scEditor->map_cbb_mapSize);
	if (!scEditor->map_cbb_mapSize) { return false; }
	long int mapSizeIndex = scEditor->map_cbb_mapSize->GetSelectedIndex();
	if (mapSizeIndex != 6) { return false; }
	// Here: custom map size
	if (!ROCKNROR::crInfo.configInfo.useCustomMapDimensions) { return false; }
	// If result is true, WE generated the map ourselves => Disable standard generation code.
	EditMapSizeXYPopup::OpenEditMapSizePopup();
	return true;
}


// Manage right button release action on selected units for given player
// Returns true if ROR code should be skipped -----WRONG if a red cross sign should be displayed (a relevant action occurred)
bool RockNRorMainInterface::ApplyRightClickReleaseOnSelectedUnits(AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *UIGameZone,
	AOE_STRUCTURES::STRUCT_PLAYER *player, long int mousePosX, long int mousePosY) {
	if (!UIGameZone || !UIGameZone->IsCheckSumValid()) { return false; }
	assert(UIGameZone != NULL);
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = UIGameZone->controlledPlayer;
	assert(player == controlledPlayer);
	if (controlledPlayer == NULL) { return false; } // it shouldn't happen
	long int unitCount = controlledPlayer->selectedUnitCount;
	if (unitCount == 0) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return false; }

	// Get relevant selected units array
	bool skipRORTreatments = false;
	bool showRedCrossSign = false;
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = ROCKNROR::crInfo.GetRelevantSelectedUnitsPointer(controlledPlayer);

	AOE_STRUCTURES::STRUCT_TEMP_MAP_POSITION_INFO posInfos;
	AOE_METHODS::GetGameInfoUnderMouse(4, mousePosX, mousePosY, &posInfos);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = AOE_METHODS::GetUnitAtMousePosition(mousePosX, mousePosY, INTERACTION_MODES::CST_IM_LIVING_UNITS, false);

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

	// Fix for right-clicking on deposit building (all hardcoded in ROR)
	AOE_STRUCTURES::STRUCT_UNIT_BASE *mainSelectedUnit = selectedUnits[0];
	if (mainSelectedUnit && mainSelectedUnit->IsCheckSumValidForAUnitClass() && 
		targetUnit && targetUnit->IsCheckSumValidForAUnitClass() &&
		(mainSelectedUnit->resourceValue > 0) && mainSelectedUnit->DerivesFromCommandable() &&
		(targetUnit->ptrStructPlayer == player) && (controlledPlayer == player)) {
		AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *mainSelectedUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)mainSelectedUnit->unitDefinition;
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *targetUnitDef = targetUnit->unitDefinition;

		bool isStandardHardcodedDropSite = ((targetUnitDef->DAT_ID1 == CST_UNITID_FORUM) || (targetUnitDef->DAT_ID1 == CST_UNITID_STORAGE_PIT) ||
			(targetUnitDef->DAT_ID1 == CST_UNITID_GRANARY));
		bool letRorCodeHandleThis = (isStandardHardcodedDropSite && (mainSelectedUnit->resourceTypeId >= 0) &&
			(mainSelectedUnit->resourceTypeId <= CST_RES_ORDER_GOLD));
		// Same for dock and fishing ships ?

		// TODO: exclude standard cases, already handled by ROR ? Filter on resource type cf unitDefCommand ?
		// Check there is a unitDefCmd CST_IAI_GATHER_NO_ATTACK, trade or gather_attack ? Beware villager mode !
		if (!letRorCodeHandleThis && (targetUnit->unitDefinition->DAT_ID1 == mainSelectedUnitDef->dropSite1) ||
			(targetUnit->unitDefinition->DAT_ID1 == mainSelectedUnitDef->dropSite2)) {
			if (GAME_COMMANDS::CreateCmd_RightClick(mainSelectedUnit->unitInstanceId, targetUnit->unitInstanceId, targetUnit->positionX, targetUnit->positionY)) {
				AOE_METHODS::UI_BASE::DisplayGreenBlinkingOnUnit(UIGameZone, targetUnit->unitInstanceId, 1000);
				skipRORTreatments = true;
			}
		}
	}

	// TODO : move this
	if (ROCKNROR::crInfo.configInfo.enableSpawnUnitsMoveToLocation) {
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
							UnitCustomInfo *u = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(unit->unitInstanceId);
							if (u) {
								u->ResetSpawnAutoTargetInfo();
								ROCKNROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(unit->unitInstanceId);
								AOE_METHODS::CallWriteText("Disabled auto-move for new units for selected building");
							}
						} else {
							// For "valid clicks" (not on building itself), add building+click info (target unit or position) to list

							// Is there a unit to interact with at this position ?
							long int targetUnitId = -1;
							AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
							assert(global && global->IsCheckSumValid());
							if (!global || !global->IsCheckSumValid()) { return false; }

							AOE_STRUCTURES::STRUCT_UNIT_BASE *unitUnderMouse = AOE_METHODS::GetUnitAtMousePosition(mousePosX, mousePosY, INTERACTION_MODES::CST_IM_LIVING_UNITS, false);
							if (unitUnderMouse && unitUnderMouse->IsCheckSumValidForAUnitClass()) {
								AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefUnderMouse = unitUnderMouse->unitDefinition;
								if (unitDefUnderMouse && unitDefUnderMouse->IsCheckSumValidForAUnitClass() &&
									(unitDefUnderMouse->interactionMode >= AOE_CONST_FUNC::INTERACTION_MODES::CST_IM_RESOURCES)
									) {
									targetUnitId = unitUnderMouse->unitInstanceId;
								}
							}

							// Add to list (or update) to set new units target position/unit
							UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(unit->unitInstanceId);
							if (unitInfo) {
								unitInfo->spawnTargetUnitId = targetUnitId;
								unitInfo->spawnUnitMoveToPosX = posInfos.posX;
								unitInfo->spawnUnitMoveToPosY = posInfos.posY;
								showRedCrossSign = true;
							}
						}
					}
				}
			}
		}
	}
	if (showRedCrossSign) {
		long int mx = UIGameZone->unknown_130_mousePosX + mousePosX;
		long int my = UIGameZone->unknown_134_mousePosY + mousePosY;
		AOE_STRUCTURES::STRUCT_SLP_INFO *slp = settings->ptrInfosSLP[AOE_CONST_DRS::AoeInGameFlagsIconId::IGF_MOVETO_RED_CROSS];
		AOE_METHODS::UI_BASE::DisplayInGameSign(UIGameZone, slp, mousePosX, mousePosY, 2);
	}
	return skipRORTreatments;
}


// Change ROR's window title to add our mod name.
bool RockNRorMainInterface::ChangeWindowTitle() {
	if (this->windowTitleChanged) { return false; }
	HWND hWndTmp = FindWindow(NULL, _T("Age of Empires: The Rise of Rome"));
	if (hWndTmp == NULL) {
		hWndTmp = FindWindow(NULL, _T("Age of Empires Expansion"));
		if (hWndTmp == NULL) {
			return false;
		}
	}
	wchar_t bufTitle[200];
	if (GetWindowText(hWndTmp, bufTitle, sizeof(bufTitle)) == 0) {
		return false;
	}
	std::wstring title = std::wstring(bufTitle) + std::wstring(_T(" - ") _T(MOD_NAME));
	if (SetWindowText(hWndTmp, title.c_str()) != 0) {
		this->windowTitleChanged = true;
		return true;
	}
	return false;
}


}