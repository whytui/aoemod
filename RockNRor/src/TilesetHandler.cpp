#include "../include/TileSetHandler.h"

namespace TILESET {

// Static object
TilesetHandler tilesetHandler;

using namespace AOE_CONST_DRS;
using namespace AOE_STRUCTURES;



TilesetHandler::TilesetHandler() {
}



// This corresponds to ROR code in 0x481792
void TilesetHandler::InitGameMainUITilesetDependentGraphics(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, long int tileset) {
	assert(gameMainUI && gameMainUI->IsCheckSumValid());
	assert(tileset >= 0);
	assert(tileset <= 255);
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		traceMessageHandler.WriteMessage("Error: gameMainUI is NULL, cannot init tileset-dependent graphics");
		return;
	}
	if ((tileset < 0) || (tileset > 255)) {
		tileset = 0;
		traceMessageHandler.WriteMessage("Error: tileset should be between 0 and 255. Using tileset 0 instead.");
	}

	std::string tilesetAsString = std::to_string(tileset);
	std::string dlg6_x = "dlg6_" + tilesetAsString;
	long int slpId;
	CustomTilesetInfo *tilesetInfo = allTilesetsInfo.GetTilesetInfo(tileset);
	if (tilesetInfo == NULL) {
		tileset = 0;
		tilesetInfo = allTilesetsInfo.GetTilesetInfo(0);
		traceMessageHandler.WriteMessage("Error: tileset ID is invalid, using 0 instead.");
		if (!tilesetInfo) { return; }
	}
	if (tilesetInfo->GetTilesetId() == tileset) {
		slpId = tilesetInfo->slpIdThemeInfo;
	} else {
		slpId = SLPID_TILESET_SCREEN_THEME_BASE_STD; // Fallback in error cases (we should not end up in this block)
	}
	unsigned long int callAddr1 = 0x4553F0;
	const char *tmpText = dlg6_x.c_str();
	_asm {
		PUSH slpId;
		PUSH tmpText;
		MOV ECX, gameMainUI;
		CALL callAddr1;
	}
	unsigned long int callAddr2 = 0x4559F0;
	_asm {
		PUSH slpId;
		PUSH tmpText;
		MOV ECX, gameMainUI;
		CALL callAddr2;
	}

	// 0x4817E7
	std::string btn6_x = "btn6_" + tilesetAsString;
	if (tilesetInfo->GetTilesetId() == tileset) {
		slpId = tilesetInfo->slpIdCheckboxes;
	} else {
		slpId = SLPID_TILESET_CHECKBOXES_BASE_STD; // Fallback in error cases (we should not end up in this block)
	}
	unsigned long int callAddr3 = 0x455A50;
	tmpText = btn6_x.c_str();
	_asm {
		PUSH slpId;
		PUSH tmpText;
		MOV ECX, gameMainUI;
		CALL callAddr3;
	}

	// 0x481825
	// Set interface colors (for button borders ?)
	this->SetBorderColors(gameMainUI, allTilesetsInfo.GetTilesetInfo(tileset)->btnBorderColors);
	
	// 0x4819E6
	if (gameMainUI->buttonBoardResolutionA != NULL) {
		AOE_METHODS::FreeSlpInfoUnderlyingObjects(gameMainUI->buttonBoardResolutionA); // call 0x49F840 and free
		gameMainUI->buttonBoardResolutionA = NULL;
	}
	std::string btnbrdax_shp = "btnbrda" + tilesetAsString + ".shp";
	gameMainUI->buttonBoardResolutionA = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!gameMainUI->buttonBoardResolutionA) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for buttonBoardResolutionA");
		return;
	}
	if (tilesetInfo->GetTilesetId() == tileset) {
		slpId = tilesetInfo->slpIdButtonBoardLow;
	} else {
		slpId = SLPID_TILESET_BUTTON_BOARD_LOW_RESOLUTION_BASE_STD; // Fallback in error cases (we should not end up in this block)
	}
	AOE_METHODS::InitSlpInfoFromDrs(gameMainUI->buttonBoardResolutionA, slpId, btnbrdax_shp.c_str());

	// 0x481A7B
	if (gameMainUI->iconsForOtherButtons != NULL) {
		AOE_METHODS::FreeSlpInfoUnderlyingObjects(gameMainUI->iconsForOtherButtons);
		gameMainUI->iconsForOtherButtons = NULL;
	}
	std::string btnothx_shp = "btnoth" + tilesetAsString + ".shp";
	gameMainUI->iconsForOtherButtons = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!gameMainUI->iconsForOtherButtons) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for iconsForOtherButtons");
		return;
	}
	if (tilesetInfo->GetTilesetId() == tileset) {
		slpId = tilesetInfo->slpIdCommonCommandIcons;
	} else {
		slpId = SLPID_TILESET_COMMON_CMD_ICONS_BASE_STD; // Fallback in error cases (we should not end up in this block)
	}
	AOE_METHODS::InitSlpInfoFromDrs(gameMainUI->iconsForOtherButtons, slpId, btnbrdax_shp.c_str());

	// 0x481B34
	if (gameMainUI->buttonBoardResolutionB != NULL) {
		AOE_METHODS::FreeSlpInfoUnderlyingObjects(gameMainUI->buttonBoardResolutionB);
		gameMainUI->buttonBoardResolutionB = NULL;
	}
	std::string btnbrdbx_shp = "btnbrdb" + tilesetAsString + ".shp";
	gameMainUI->buttonBoardResolutionB = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!gameMainUI->buttonBoardResolutionB) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for buttonBoardResolutionB");
		return;
	}
	if (tilesetInfo->GetTilesetId() == tileset) {
		slpId = tilesetInfo->slpIdButtonBoardMedium;
	} else {
		slpId = SLPID_TILESET_BUTTON_BOARD_MEDIUM_RESOLUTION_BASE_STD; // Fallback in error cases (we should not end up in this block)
	}
	AOE_METHODS::InitSlpInfoFromDrs(gameMainUI->buttonBoardResolutionB, slpId, btnbrdbx_shp.c_str());

	// 0x481BC0
	if (gameMainUI->buttonBoardResolutionC != NULL) {
		AOE_METHODS::FreeSlpInfoUnderlyingObjects(gameMainUI->buttonBoardResolutionC);
		gameMainUI->buttonBoardResolutionC = NULL;
	}
	std::string btnbrdcx_shp = "btnbrdc" + tilesetAsString + ".shp";
	gameMainUI->buttonBoardResolutionC = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!gameMainUI->buttonBoardResolutionC) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for buttonBoardResolutionC");
		return;
	}
	if (tilesetInfo->GetTilesetId() == tileset) {
		slpId = tilesetInfo->slpIdButtonBoardHigh;
	} else {
		slpId = SLPID_TILESET_BUTTON_BOARD_HIGH_RESOLUTION_BASE_STD; // Fallback in error cases (we should not end up in this block)
	}
	AOE_METHODS::InitSlpInfoFromDrs(gameMainUI->buttonBoardResolutionC, slpId, btnbrdcx_shp.c_str());

	// 0x481C61
	if (gameMainUI->gameTopAndBottomFriezes != NULL) {
		AOE_METHODS::FreeSlpInfoUnderlyingObjects(gameMainUI->gameTopAndBottomFriezes); // call 0x49F840 and free
		gameMainUI->gameTopAndBottomFriezes = NULL;
	}
	std::string gamex_shp = "";
	int resolution = 0; // 1=low, 2=medium, 3=high resolution
	slpId = -1;
	if (gameMainUI->sizeX < 0x320) {
		// Low resolution
		gamex_shp = "gamea" + tilesetAsString + ".shp";
		resolution = 1;
	} else {
		if(gameMainUI->sizeX < 0x400) {
			// Medium resolution (B) : 800<sizeX<1024
			gamex_shp = "gameb" + tilesetAsString + ".shp";
			resolution = 2;
		} else {
			// High resolution (0x400 = 1024 = high resolution sizeX in standard game. If > 1024, it is a custom (but still highest) resolution
			gamex_shp = "gamec" + tilesetAsString + ".shp";
			resolution = 3;
		}
	}
	switch (resolution) {
	case 1:
		if (tilesetInfo->GetTilesetId() == tileset) {
			slpId = tilesetInfo->slpIdGameScreenLow;
		} else {
			slpId = SLPID_TILESET_GAME_FRIEZES_LOW_RESOLUTION_BASE_STD; // Fallback in error cases (we should not end up in this block)
		}
		break;
	case 2:
		if (tilesetInfo->GetTilesetId() == tileset) {
			slpId = tilesetInfo->slpIdGameScreenMedium;
		} else {
			slpId = SLPID_TILESET_GAME_FRIEZES_MEDIUM_RESOLUTION_BASE_STD; // Fallback in error cases (we should not end up in this block)
		}
		break;
	case 3:
		if (tilesetInfo->GetTilesetId() == tileset) {
			slpId = tilesetInfo->slpIdGameScreenHigh;
		} else {
			slpId = SLPID_TILESET_GAME_FRIEZES_HIGH_RESOLUTION_BASE_STD; // Fallback in error cases (we should not end up in this block)
		}
		break;
	}

	gameMainUI->gameTopAndBottomFriezes = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!gameMainUI->gameTopAndBottomFriezes) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for gameTopAndBottomFriezes");
		return;
	}
	AOE_METHODS::InitSlpInfoFromDrs(gameMainUI->gameTopAndBottomFriezes, slpId, gamex_shp.c_str());
	// End of tileset-related inits : 0x481D51

	for (int i = 0; i < 12; i++) {
		gameMainUI->unitCommandButtons[i]->unknown_2C4_btnBoard = gameMainUI->buttonBoardResolutionA;
	}
	gameMainUI->btnChat->unknown_2C4_btnBoard = gameMainUI->buttonBoardResolutionB;
	gameMainUI->btnDiplomacy->unknown_2C4_btnBoard = gameMainUI->buttonBoardResolutionB;
	gameMainUI->btnMenu->unknown_2C4_btnBoard = gameMainUI->buttonBoardResolutionB;
	gameMainUI->btnHelp->unknown_2C4_btnBoard = gameMainUI->buttonBoardResolutionB;
	gameMainUI->btnShowScores->unknown_2C4_btnBoard = gameMainUI->buttonBoardResolutionB;

	if (resolution > 1) {
		// Medium/high resolutions
		gameMainUI->iconsNextCancelForCurrentResolution = gameMainUI->iconsForUnitCommands;
	} else {
		// Low
		gameMainUI->iconsNextCancelForCurrentResolution = gameMainUI->iconsForOtherButtons; // "next" and "cancel" need to be smaller not to conflict with map
	}
	STRUCT_UI_BUTTON_WITH_NUMBER *nextPageBtn = gameMainUI->unitCommandButtons[5];
	assert(nextPageBtn != NULL);
	long int nextPageIconId = gameMainUI->nextPageIconId;
	STRUCT_SLP_INFO *unknown_4AC_icons = gameMainUI->iconsNextCancelForCurrentResolution;
	unsigned long int callAddr4_seticon = 0x45EF60;
	_asm {
		MOV ECX, nextPageBtn;
		PUSH nextPageIconId;
		PUSH unknown_4AC_icons;
		PUSH 0;
		CALL callAddr4_seticon;
	}
	STRUCT_UI_BUTTON_WITH_NUMBER *cancelBtn = gameMainUI->unitCommandButtons[11]; // last button
	assert(cancelBtn != NULL);
	long int value = cancelBtn->commandIDs[cancelBtn->checked];
	long int arg3_iconid = 0;
	if (value == 6) {
		arg3_iconid = gameMainUI->unknown_4B4;
	} else {
		arg3_iconid = gameMainUI->unknown_4B2_iconId;
	}
	_asm {
		MOV ECX, cancelBtn;
		PUSH arg3_iconid;
		PUSH unknown_4AC_icons;
		PUSH 0;
		CALL callAddr4_seticon; // call in 0x481E05
	}
	// 0x481E21 (tilesets 0,1) or 0x481E9E (tilesets 2,3,4)
	unsigned long int textColor = tilesetInfo->textColorRGB;
	unsigned long int shadowTextColor = tilesetInfo->textShadowColorRGB;
	AOE_METHODS::UI_BASE::SetLabelTextColor(gameMainUI->lblCurrentAge, textColor, shadowTextColor);
	AOE_METHODS::UI_BASE::SetPlayerResValuesTextColor(gameMainUI->resourceValuesIndicator, textColor, shadowTextColor);
	AOE_METHODS::UI_BASE::SetButtonTextColor(gameMainUI->btnChat, textColor, shadowTextColor);
	AOE_METHODS::UI_BASE::SetButtonTextColor(gameMainUI->btnDiplomacy, textColor, shadowTextColor);
	AOE_METHODS::UI_BASE::SetButtonTextColor(gameMainUI->btnMenu, textColor, shadowTextColor);
	AOE_METHODS::UI_BASE::SetButtonTextColor(gameMainUI->btnHelp, textColor, shadowTextColor);
	AOE_METHODS::UI_BASE::SetButtonTextColor(gameMainUI->btnShowScores, textColor, shadowTextColor);
}



void TilesetHandler::SetBorderColors(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, unsigned char *argColor) {
	assert(argColor != NULL);
	// Cf code in 0x481890 (and pushes just above)
	gameMainUI->unknown_colorForBtnBorderOutTopRight = argColor[0]; // cf call 0x455950
	gameMainUI->unknown_colorForBtnBorderMidTopRight = argColor[1];
	gameMainUI->unknown_colorForBtnBorderInTopRight = argColor[2];
	gameMainUI->unknown_colorForBtnBorderInBottomLeft = argColor[3];
	gameMainUI->unknown_colorForBtnBorderMidBottomLeft = argColor[4];
	gameMainUI->unknown_colorForBtnBorderOutBottomLeft = argColor[5];
	assert(gameMainUI->btnChat != NULL);
	gameMainUI->btnChat->unknown_2A0_borderSize = 3;
	gameMainUI->btnChat->borderColorOutTopRight = argColor[0];
	gameMainUI->btnChat->borderColorMidtTopRight = argColor[1];
	gameMainUI->btnChat->borderColorInTopRight = argColor[2];
	gameMainUI->btnChat->borderColorInBottomLeft = argColor[3];
	gameMainUI->btnChat->borderColorMidBottomLeft = argColor[4];
	gameMainUI->btnChat->borderColorOutBottomLeft = argColor[5];
	assert(gameMainUI->btnDiplomacy != NULL);
	gameMainUI->btnDiplomacy->unknown_2A0_borderSize = 3;
	gameMainUI->btnDiplomacy->borderColorOutTopRight = argColor[0];
	gameMainUI->btnDiplomacy->borderColorMidtTopRight = argColor[1];
	gameMainUI->btnDiplomacy->borderColorInTopRight = argColor[2];
	gameMainUI->btnDiplomacy->borderColorInBottomLeft = argColor[3];
	gameMainUI->btnDiplomacy->borderColorMidBottomLeft = argColor[4];
	gameMainUI->btnDiplomacy->borderColorOutBottomLeft = argColor[5];
	assert(gameMainUI->btnMenu != NULL);
	gameMainUI->btnMenu->unknown_2A0_borderSize = 3;
	gameMainUI->btnMenu->borderColorOutTopRight = argColor[0];
	gameMainUI->btnMenu->borderColorMidtTopRight = argColor[1];
	gameMainUI->btnMenu->borderColorInTopRight = argColor[2];
	gameMainUI->btnMenu->borderColorInBottomLeft = argColor[3];
	gameMainUI->btnMenu->borderColorMidBottomLeft = argColor[4];
	gameMainUI->btnMenu->borderColorOutBottomLeft = argColor[5];
	assert(gameMainUI->btnHelp != NULL);
	gameMainUI->btnHelp->unknown_2A0_borderSize = 4;
	gameMainUI->btnHelp->borderColorOutTopRight = argColor[0];
	gameMainUI->btnHelp->borderColorMidtTopRight = argColor[1];
	gameMainUI->btnHelp->borderColorInTopRight = argColor[2];
	gameMainUI->btnHelp->borderColorInBottomLeft = argColor[3];
	gameMainUI->btnHelp->borderColorMidBottomLeft = argColor[4];
	gameMainUI->btnHelp->borderColorOutBottomLeft = argColor[5];
	assert(gameMainUI->btnShowScores != NULL);
	gameMainUI->btnShowScores->unknown_2A0_borderSize = 4;
	gameMainUI->btnShowScores->borderColorOutTopRight = argColor[0];
	gameMainUI->btnShowScores->borderColorMidtTopRight = argColor[1];
	gameMainUI->btnShowScores->borderColorInTopRight = argColor[2];
	gameMainUI->btnShowScores->borderColorInBottomLeft = argColor[3];
	gameMainUI->btnShowScores->borderColorMidBottomLeft = argColor[4];
	gameMainUI->btnShowScores->borderColorOutBottomLeft = argColor[5];
}



}
