
#pragma once

#include <assert.h>
#include <AOE_offsets.h>
#include <AOE_const_internal.h>
#include <AOE_structures_drs.h>
#include <AOE_struct_game_settings.h>
#include <UI_components\AOE_struct_any_ui.h>
#include <UI\AOE_struct_ui_scenario_editor_main.h>
#include <UI\AOE_struct_ui_in_game_main.h>
#include "AOEPrimitives_UI_components.h"
#include "AOEPrimitives_UI_screens.h"
#include "AOE_memory.h"
#include "drsHandler.h"
#include "mainStructuresHandling.h"
#include "traceMessage.h"
#include "CustomTilesetInfo.h"

/*
 * This file provides useful (raw) methods to deal with AOE UI objects.
 * Most of them directly call ROR's methods and are a practical encapsulation to avoid using assembler in our code.
*/


static const char scenarioEditorScreenName[] = "Scenario Editor Screen";
static const char menuDialogScreenName[] = "Menu Dialog";
static const char gameScreenName[] = "Game Screen";

namespace AOE_METHODS {
;

using namespace UI_BASE;


// Display a small yes/no dialog message (based on game exit popup) + cancel in scenario editor
// Return pointer (address) to new object.
// Compatible with scenario editor screen and in-game screen
// Note that settings->currentUIStatus is unchanged when showing such dialog message.
static AOE_STRUCTURES::STRUCT_UI_EASY_PANEL *AOE_CreateDialogPopup(const char *text, long int hSize, long int vSize) {
	char *dlgName = (char *)AOE_CONST_INTERNAL::customDialogScreenName;
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = AOE_STRUCTURES::GetGameSettingsPtr();
	if (settings == NULL) { return 0; }
	unsigned long int fct = 0;
	AOE_STRUCTURES::STRUCT_UI_EASY_PANEL *currentUI = AOE_STRUCTURES::GetCurrentScreen();
	if (currentUI == NULL) { return NULL; }
	// Get correct call values according to active screen
	if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) {
		AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *p = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *)currentUI;
		assert(p->visible);
		if (!p->IsCheckSumValid()) { return NULL; }
		fct = 0x00457A80; // yesno
	}
	if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) {
		if (currentUI->checksum != CHECKSUM_UI_SCENARIO_EDITOR_MAIN) { return 0; }
		fct = 0x00457B90; // yesnocancel
	}
	if (!fct) { return 0; }
	AOE_STRUCTURES::STRUCT_UI_EASY_PANEL *obj = NULL;
	// Now call relevant function
	_asm {
		PUSH vSize;
		PUSH hSize;
		MOV EAX, dlgName;
		PUSH EAX;
		MOV EAX, text;
		PUSH EAX;
		MOV ECX, currentUI;
		MOV EAX, fct; // creates an easyPanel (even a messageDialog, actually)
		CALL EAX; // showDialog(ptrText, dlgName, hSize, vSize)
		MOV ECX, currentUI;
		MOV EAX, DS:[ECX + 0x3C]; // Focused object = new popup
		MOV obj, EAX;
	}
	return obj;
}


static long int AOE_GetGamePosFromMousePos(AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *gameZone,
	AOE_STRUCTURES::STRUCT_TEMP_MAP_POSITION_INFO *pPosInfo, long int mousePosX, long int mousePosY) {
	if (!gameZone || !pPosInfo || !gameZone->IsCheckSumValid()) { return 0; }
	const unsigned long int callAddr = 0x511430;
	long int res;
	_asm {
		MOV ECX, gameZone;
		PUSH 0;
		PUSH pPosInfo;
		PUSH mousePosY;
		PUSH mousePosX;
		PUSH 0;
		PUSH 0x28; // "click release" MOUSE_BUTTON_ACTION::CST_MBA_RELEASE_CLICK
		CALL callAddr;
		MOV res, EAX;
	}
	return res;
}


// Return true if successful. Only works if current screen is game screen !
// Adds a button in unit-commands panel (bottom, beside select unit info).
// player is used to find correct color.
// ButtonIndex is 0-5 (first row), 6-11 (2nd row), 5 and 11 are special (on the right, eg. next, cancel).
// helpDllId can be 0 ? creationDllId can be -1. 
// Name can be left NULL. Don't know exactly when this is being used (in help popup ?)
// If description == NULL, then creationDllId will be used for displayed context text.
// IconId and DATID are related to the type of action, be careful to provide consistent values.
// You can set DATId to 0 if not relevant.
static bool AOE_InGameAddCommandButton(AOE_STRUCTURES::STRUCT_PLAYER *player, long int buttonIndex, long int iconId,
	AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID UICmdId, long int DATID,
	long int helpDllId, long int creationDllId, long int shortcutDllId, const char *name, const char *description,
	bool isDisabled, AOE_STRUCTURES::STRUCT_SLP_INFO *iconSlpInfo) {
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *inGameMain = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *) GetScreenFromName(gameScreenName);
	if (!inGameMain || !inGameMain->IsCheckSumValid() || !inGameMain->visible) {
		return false;
	}
	if (inGameMain != (void*)GetCurrentScreen()) {
		return false;
	}

	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) {
		return false;
	}
	unsigned long int unknown_colorPtr = NULL;

	const unsigned long int calladdr = 0x483760;
	AOE_STRUCTURES::STRUCT_SLP_INFO *iconsSLP = iconSlpInfo;
	if ((iconSlpInfo == NULL) || (iconSlpInfo->slpSize == 0)) {
		iconsSLP = inGameMain->iconsForUnitCommands; // Default (for most cases)
	}
	
	// Guess automatically in which SLP we should search the icon.
	if (UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN) {
		unknown_colorPtr = player->ptrPlayerColorStruct + 0x28;
		iconsSLP = inGameMain->iconsForTrainUnits;
	}
	if (UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_BUILD) {
		unknown_colorPtr = player->ptrPlayerColorStruct + 0x28;
		assert(player->tileSet >= 0);
		assert(player->tileSet <= TILESET::allTilesetsInfo.tilesetCount);
		int tileSet = player->tileSet;
		TILESET::CustomTilesetInfo *tilesetInfo = TILESET::allTilesetsInfo.GetTilesetInfo(tileSet);
		if (tilesetInfo) {
			iconsSLP = tilesetInfo->GetIconsForBuildings();
		} else {
			if ((tileSet < 0) || (tileSet > TILESET::allTilesetsInfo.tilesetCount)) { tileSet = 0; } // in theory, this is not necessary.
			iconsSLP = inGameMain->iconsForBuildings[player->tileSet];
		}
	}
	if ((UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH) ||
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_TRADE_FOOD_FOR_GOLD) || 
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_TRADE_STONE_FOR_GOLD) || 
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_TRADE_WOOD_FOR_GOLD)) {
		iconsSLP = inGameMain->iconsForResearches;
		unknown_colorPtr = player->ptrPlayerColorStruct + 0x28;
	}
	if ((UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CANCEL_BUILD) ||
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CANCEL_OR_BACK) || 
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CANCEL_SELECTION) || 
		(UICmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_NEXT_PAGE)) {
		iconsSLP = inGameMain->iconsNextCancelForCurrentResolution;
	}

	// Check that iconId is valid in SLP : otherwise, the whole game display will encounter serious issues !
	if (!IsObjectIndexValidInSlp(iconsSLP, iconId)) {
		std::string msg = "iconId " + std::to_string(iconId) + " does not exist in current SLP file.";
		traceMessageHandler.WriteMessageNoNotification(msg);
		assert(!"iconId does not exist in this SLP file. Maybe a DRS file is missing.");
		return false;
	}

	AOE_STRUCTURES::STRUCT_UI_BUTTON_WITH_NUMBER *btn = inGameMain->unitCommandButtons[buttonIndex];
	bool showNumber = (btn->showNumber != 0);

	long int long_UICmdId = (long int)UICmdId;
	long int long_isDisabled = isDisabled;
	_asm {
		PUSH long_isDisabled;
		PUSH description;
		PUSH name;
		PUSH unknown_colorPtr; // arg9
		PUSH shortcutDllId; // shortcut DLLID (hotkey)
		PUSH creationDllId; // arg7 = creation text (or hotkey text)
		PUSH helpDllId; // help dll id (language DLL creation + 100000)
		PUSH DATID; // DATID
		PUSH long_UICmdId;
		PUSH iconId; // iconid
		PUSH buttonIndex; // button index
		PUSH iconsSLP;
		MOV ECX, inGameMain;
		CALL calladdr;
	}
	if (btn && btn->IsCheckSumValid() && (btn->numberToDisplay > 0) && showNumber) {
		const unsigned long int enableShowBtnCount = 0x4F7BC0;
		const unsigned long int setBtnCount = 0x4F7BE0;
		long int count = btn->numberToDisplay;
		_asm {
			PUSH 1;
			MOV ECX, btn;
			CALL enableShowBtnCount;
			PUSH count;
			MOV ECX, btn;
			CALL setBtnCount;
		}
	}

	return true;
}


// Display shortcut number below unit in game zone. Can be used to display other SLP bitmaps !
static void DisplayUnitShortcutSymbol(AOE_STRUCTURES::STRUCT_SLP_FILE_HEADER *slpHeader,
	AOE_STRUCTURES::STRUCT_SLP_FRAME_HEADER *slpFrameHeader, long int posX, long int posY, long int unknown_arg3) {
	unsigned long int showSlp = 0x516390;
	_asm {
		MOV ECX, DWORD PTR DS:[0x7C0444];
		PUSH ECX; // ds:[7C0444]
		PUSH 0x1E; // arg9 = zindex level (sort of)
		PUSH 0;
		PUSH 0;
		PUSH 0; // arg6
		PUSH posY; // arg5 = posY
		PUSH posX; // arg4 = posX
		PUSH unknown_arg3; // arg3
		PUSH slpFrameHeader; // arg2
		PUSH slpHeader; // arg1
		MOV ECX, DWORD PTR DS:[0x7C0460];
		CALL showSlp;
	}
}


// Refresh diamond map (draw all tiles)
static void AOE_DiamondMapDrawAllTiles(AOE_STRUCTURES::STRUCT_UI_DIAMOND_MAP *diamMap) {
	if (!diamMap || !diamMap->IsCheckSumValid()) { return; }
	const unsigned long int addr = 0x42D0C0;
	_asm {
		MOV ECX, diamMap;
		CALL addr;
	}
}

// Refresh diamond map (draw all tiles) in editor or in-game (automatically finds the correct structure)
static bool DiamondMapDrawAllTiles() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = AOE_STRUCTURES::GetGameSettingsPtr();
	if (settings && settings->IsCheckSumValid()) {
		AOE_STRUCTURES::STRUCT_UI_DIAMOND_MAP *diamMap = NULL;
		if ((settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) ||
			(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_GAME_OVER_BUT_STILL_IN_GAME)) {
			if (settings->ptrGameUIStruct && settings->ptrGameUIStruct->IsCheckSumValid()) {
				diamMap = settings->ptrGameUIStruct->diamondMap;
			}
		}
		if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) {
			AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)GetScreenFromName(scenarioEditorScreenName);
			if (scEditor && scEditor->IsCheckSumValid()) {
				diamMap = scEditor->diamondMap;
			}
		}
		if (diamMap && diamMap->IsCheckSumValid()) {
			AOE_DiamondMapDrawAllTiles(diamMap);
			return true;
		}
	}
	return false;
}

// Opens scenario editor menu
static void AOE_EditorOpenMenu(AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *editorUI) {
	if (!editorUI || !editorUI->IsCheckSumValid()) { return; }
	_asm {
		MOV ECX, editorUI;
		MOV EAX, 0x00494A90; // editor.OpenMenu()
		CALL EAX;
	}
}

// Returns game zone UI object for both in-game or scenario editor. Returns NULL if not found.
static AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *GetGameZone() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = AOE_STRUCTURES::GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_ANY_UI *currentUI = GetCurrentScreen();
	assert(currentUI);
	if (!settings || !currentUI || !settings->IsCheckSumValid()) { return NULL; }

	if ((settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) &&
		(settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) &&
		(settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_GAME_OVER_BUT_STILL_IN_GAME)) {
		return NULL;
	}

	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *)currentUI;
	if (gameMainUI->IsCheckSumValid()) {
		assert((settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) ||
			(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_GAME_OVER_BUT_STILL_IN_GAME));
		if (gameMainUI->gamePlayUIZone && gameMainUI->gamePlayUIZone->IsCheckSumValid()) {
			return gameMainUI->gamePlayUIZone;
		} else {
			return NULL;
		}
	}

	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditorMainUI = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)currentUI;

	if (scEditorMainUI && scEditorMainUI->IsCheckSumValid()) {
		assert(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR);
		if (scEditorMainUI->gamePlayUIZone && scEditorMainUI->gamePlayUIZone->IsCheckSumValid()) {
			return scEditorMainUI->gamePlayUIZone;
		} else {
			return NULL;
		}
	}
	return NULL;
}

// Change mouse cursor. Warning, the game may change it back rapidly (eg. in game when hovering units).
static void SetGameCursor(AOE_CONST_INTERNAL::GAME_CURSOR cursorId) {
	const unsigned long int pCursorsStruct = AOE_OFFSETS::AOE_VAR_CURSORS_INFO;
	const unsigned long int addr = 0x44A6C0;
	_asm {
		MOV ECX, pCursorsStruct;
		MOV ECX, DS:[ECX];
		PUSH cursorId;
		CALL addr;
	}
}


// Change screen resolution.
// WARNING: this can be unstable
static void SetScreenResolution(unsigned long int x, unsigned long int y) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) {
		return;
	}
	assert(x > 100);
	assert(y > 100);
	if ((x == 0) || (y == 0)) { return; } // A security.
	unsigned long int callAddr = 0x41BE20;
	_asm {
		MOV ECX, settings;
		PUSH y;
		PUSH x;
		CALL callAddr;
	}
}

}
