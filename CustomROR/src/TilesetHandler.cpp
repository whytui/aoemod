#include "../include/TileSetHandler.h"

namespace TILESET {

// Static object
TilesetHandler tilesetHandler;

using namespace AOE_CONST_DRS;
using namespace AOE_STRUCTURES;


CustomTilesetInfo::CustomTilesetInfo() {
	this->tilesetId = -1;
	this->tilesetName = "";
	this->slpIdBuildingIcons = -1;
	this->slpIdButtonBoardHigh = -1;
	this->slpIdButtonBoardLow = -1;
	this->slpIdButtonBoardMedium = -1;
	this->slpIdCheckboxes = -1;
	this->slpIdCheckboxes = -1;
	this->slpIdCommonCommandIcons = -1;
	this->slpIdGameScreenHigh = -1;
	this->slpIdGameScreenLow = -1;
	this->slpIdGameScreenMedium = -1;
	this->slpIdThemeInfo = -1;
}


TilesetHandler::TilesetHandler() {
	this->tilesetCount = TILESET_COUNT_STANDARD_ROR;
}


CustomTilesetInfo *TilesetHandler::GetTilesetInfo(long int tileset) {
	assert(tileset >= 0);
	assert(tileset < MAX_TILESET_TOTAL_SUPPORTED_COUNT);
	if ((tileset < 0) || (tileset >= MAX_TILESET_TOTAL_SUPPORTED_COUNT)) { return NULL; }
	return &this->tilesetsInfo[tileset];
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
	long int slpId = SLPID_TILESET_SCREEN_THEME_BASE_STD + tileset; // default (for standard tilesets = 0-3)
	if (tileset == MAX_STANDARD_TILESET_ID) {
		slpId = SLPID_TILESET_SCREEN_THEME_ROR; // Roman
	}
	if (tileset > MAX_STANDARD_TILESET_ID) {
		if (this->tilesetsInfo[tileset].tilesetId == tileset) { // true only if it has been set from config
			slpId = this->tilesetsInfo[tileset].slpIdThemeInfo;
		}
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
	slpId = SLPID_TILESET_CHECKBOXES_BASE_STD + tileset; // default (for standard tilesets = 0-3)
	if (tileset == MAX_STANDARD_TILESET_ID) {
		slpId = SLPID_TILESET_CHECKBOXES_ROR; // Roman
	}
	if (tileset > MAX_STANDARD_TILESET_ID) {
		if (this->tilesetsInfo[tileset].tilesetId == tileset) { // true only if it has been set from config
			slpId = this->tilesetsInfo[tileset].slpIdCheckboxes;
		}
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
	unsigned char argColor[6]; // Actual values are 0-255, but use long ints to push DWORDS
	for (int i = 0; i < _countof(argColor); i++) {
		argColor[i] = 0;
	}
	this->FillColorArgs(argColor, tileset);
	this->SetBorderColors(gameMainUI, argColor);
	
	// 0x4819E6
	if (gameMainUI->buttonBoardResolutionA != NULL) {
		AOE_STRUCTURES::AOE_FreeSlpInfoUnderlyingObjects(gameMainUI->buttonBoardResolutionA); // call 0x49F840 and free
		gameMainUI->buttonBoardResolutionA = NULL;
	}
	std::string btnbrdax_shp = "btnbrda" + tilesetAsString + ".shp";
	gameMainUI->buttonBoardResolutionA = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!gameMainUI->buttonBoardResolutionA) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for buttonBoardResolutionA");
		return;
	}
	slpId = SLPID_TILESET_BUTTON_BOARD_LOW_RESOLUTION_BASE_STD + tileset; // default (for standard tilesets = 0-3)
	if (tileset == MAX_STANDARD_TILESET_ID) {
		slpId = SLPID_TILESET_BUTTON_BOARD_LOW_RESOLUTION_ROR; // Roman
	}
	if (tileset > MAX_STANDARD_TILESET_ID) {
		if (this->tilesetsInfo[tileset].tilesetId == tileset) { // true only if it has been set from config
			slpId = this->tilesetsInfo[tileset].slpIdButtonBoardLow;
		}
	}
	InitSlpInfoFromDrs(gameMainUI->buttonBoardResolutionA, slpId, btnbrdax_shp.c_str());

	// 0x481A7B
	if (gameMainUI->iconsForOtherButtons != NULL) {
		AOE_STRUCTURES::AOE_FreeSlpInfoUnderlyingObjects(gameMainUI->iconsForOtherButtons);
		gameMainUI->iconsForOtherButtons = NULL;
	}
	std::string btnothx_shp = "btnoth" + tilesetAsString + ".shp";
	gameMainUI->iconsForOtherButtons = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!gameMainUI->iconsForOtherButtons) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for iconsForOtherButtons");
		return;
	}
	slpId = SLPID_TILESET_COMMON_CMD_ICONS_BASE_STD + tileset; // default (for standard tilesets = 0-3)
	if (tileset == MAX_STANDARD_TILESET_ID) {
		slpId = SLPID_TILESET_COMMON_CMD_ICONS_ROR; // Roman
	}
	if (tileset > MAX_STANDARD_TILESET_ID) {
		if (this->tilesetsInfo[tileset].tilesetId == tileset) { // true only if it has been set from config
			slpId = this->tilesetsInfo[tileset].slpIdCommonCommandIcons;
		}
	}
	InitSlpInfoFromDrs(gameMainUI->iconsForOtherButtons, slpId, btnbrdax_shp.c_str());

	// 0x481B34
	if (gameMainUI->buttonBoardResolutionB != NULL) {
		AOE_STRUCTURES::AOE_FreeSlpInfoUnderlyingObjects(gameMainUI->buttonBoardResolutionB);
		gameMainUI->buttonBoardResolutionB = NULL;
	}
	std::string btnbrdbx_shp = "btnbrdb" + tilesetAsString + ".shp";
	gameMainUI->buttonBoardResolutionB = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!gameMainUI->buttonBoardResolutionB) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for buttonBoardResolutionB");
		return;
	}
	slpId = SLPID_TILESET_BUTTON_BOARD_MEDIUM_RESOLUTION_BASE_STD + tileset; // default (for standard tilesets = 0-3)
	if (tileset == MAX_STANDARD_TILESET_ID) {
		slpId = SLPID_TILESET_BUTTON_BOARD_MEDIUM_RESOLUTION_ROR; // Roman
	}
	if (tileset > MAX_STANDARD_TILESET_ID) {
		if (this->tilesetsInfo[tileset].tilesetId == tileset) { // true only if it has been set from config
			slpId = this->tilesetsInfo[tileset].slpIdButtonBoardMedium;
		}
	}
	InitSlpInfoFromDrs(gameMainUI->buttonBoardResolutionB, slpId, btnbrdbx_shp.c_str());

	// 0x481BC0
	if (gameMainUI->buttonBoardResolutionC != NULL) {
		AOE_STRUCTURES::AOE_FreeSlpInfoUnderlyingObjects(gameMainUI->buttonBoardResolutionC);
		gameMainUI->buttonBoardResolutionC = NULL;
	}
	std::string btnbrdcx_shp = "btnbrdc" + tilesetAsString + ".shp";
	gameMainUI->buttonBoardResolutionC = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!gameMainUI->buttonBoardResolutionC) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for buttonBoardResolutionC");
		return;
	}
	slpId = SLPID_TILESET_BUTTON_BOARD_HIGH_RESOLUTION_BASE_STD + tileset; // default (for standard tilesets = 0-3)
	if (tileset == MAX_STANDARD_TILESET_ID) {
		slpId = SLPID_TILESET_BUTTON_BOARD_HIGH_RESOLUTION_ROR; // Roman
	}
	if (tileset > MAX_STANDARD_TILESET_ID) {
		if (this->tilesetsInfo[tileset].tilesetId == tileset) { // true only if it has been set from config
			slpId = this->tilesetsInfo[tileset].slpIdButtonBoardHigh;
		}
	}
	InitSlpInfoFromDrs(gameMainUI->buttonBoardResolutionC, slpId, btnbrdcx_shp.c_str());

	// 0x481C61
	if (gameMainUI->gameTopAndBottomFriezes != NULL) {
		AOE_STRUCTURES::AOE_FreeSlpInfoUnderlyingObjects(gameMainUI->gameTopAndBottomFriezes); // call 0x49F840 and free
		gameMainUI->gameTopAndBottomFriezes = NULL;
	}
	std::string gamex_shp = "";
	int resolution = 0;
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
	if (tileset <= MAX_STANDARD_TILESET_ID) {
		// For ROR-supported tilesets
		switch (resolution) {
		case 1:
			if (tileset == MAX_STANDARD_TILESET_ID) {
				slpId = SLPID_TILESET_GAME_FRIEZES_LOW_RESOLUTION_ROR; // Roman
			} else {
				slpId = SLPID_TILESET_GAME_FRIEZES_LOW_RESOLUTION_BASE_STD + tileset; // default (for standard tilesets = 0-3)
			}
			break;
		case 2:
			if (tileset == MAX_STANDARD_TILESET_ID) {
				slpId = SLPID_TILESET_GAME_FRIEZES_MEDIUM_RESOLUTION_ROR; // Roman
			} else {
				slpId = SLPID_TILESET_GAME_FRIEZES_MEDIUM_RESOLUTION_BASE_STD + tileset; // default (for standard tilesets = 0-3)
			}
			break;
		case 3:
			if (tileset == MAX_STANDARD_TILESET_ID) {
				slpId = SLPID_TILESET_GAME_FRIEZES_HIGH_RESOLUTION_ROR; // Roman
			} else {
				slpId = SLPID_TILESET_GAME_FRIEZES_HIGH_RESOLUTION_BASE_STD + tileset; // default (for standard tilesets = 0-3)
			}
			break;
		default:
			break;
		}
	} else {
		if (this->tilesetsInfo[tileset].tilesetId == tileset) { // true only if it has been set from config
			switch (resolution) {
			case 1:
				slpId = this->tilesetsInfo[tileset].slpIdGameScreenLow;
				break;
			case 2:
				slpId = this->tilesetsInfo[tileset].slpIdGameScreenMedium;
				break;
			case 3:
				slpId = this->tilesetsInfo[tileset].slpIdGameScreenHigh;
				break;
			}
			
		}
	}
	gameMainUI->gameTopAndBottomFriezes = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!gameMainUI->gameTopAndBottomFriezes) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for gameTopAndBottomFriezes");
		return;
	}
	InitSlpInfoFromDrs(gameMainUI->gameTopAndBottomFriezes, slpId, gamex_shp.c_str());
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
}


// Make sure that argColor's size is 6. (indexes 0-5)
void TilesetHandler::FillColorArgs(unsigned char *argColor, long int tileset) {
	switch (tileset) {
	case TILESET_EGYPT:
		argColor[0] = 0x7E;
		argColor[1] = 0x6D;
		argColor[2] = 0x6F;
		argColor[3] = 0x6F;
		argColor[4] = 0xEE;
		argColor[5] = 0x38;
		return;
	case TILESET_GREECE:
		argColor[0] = 0x72;
		argColor[1] = 0x73;
		argColor[2] = 0x74;
		argColor[3] = 0x74;
		argColor[4] = 0xB7;
		argColor[5] = 0xB8;
		return;
	case TILESET_BABYLONIAN:
		argColor[0] = 0xB9;
		argColor[1] = 0x77;
		argColor[2] = 0x78;
		argColor[3] = 0x78;
		argColor[4] = 0x78;
		argColor[5] = 0x79;
		return;
	case TILESET_ASIAN:
		argColor[0] = 0x8A;
		argColor[1] = 0xED;
		argColor[2] = 0xEE;
		argColor[3] = 0xEE;
		argColor[4] = 0x38;
		argColor[5] = 0x95;
		return;
	case TILESET_ROMAN:
		argColor[0] = 0x73;
		argColor[1] = 0x74;
		argColor[2] = 0x75;
		argColor[3] = 0x75;
		argColor[4] = 0x76;
		argColor[5] = 0x77;
		return;
	default:
		break;
	}
	argColor[0] = 0x7E;
	argColor[1] = 0x6D;
	argColor[2] = 0x6F;
	argColor[3] = 0x6F;
	argColor[4] = 0xEE;
	argColor[5] = 0x38;
	// TODO: custom tilesets: add a config for this.
}

void TilesetHandler::SetBorderColors(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, unsigned char *argColor) {
	assert(argColor != NULL);
	// Cf code in 0x481890 (and pushes just above)
	gameMainUI->unknown_320 = argColor[0]; // cf call 0x455950
	gameMainUI->unknown_321 = argColor[1];
	gameMainUI->unknown_322 = argColor[2];
	gameMainUI->unknown_323 = argColor[3];
	gameMainUI->unknown_324 = argColor[4];
	gameMainUI->unknown_325 = argColor[5];
	assert(gameMainUI->btnChat != NULL);
	gameMainUI->btnChat->unknown_2A0 = 3;
	gameMainUI->btnChat->unknown_2A4 = argColor[0];
	gameMainUI->btnChat->unknown_2A5 = argColor[1];
	gameMainUI->btnChat->unknown_2A6 = argColor[2];
	gameMainUI->btnChat->unknown_2A7 = argColor[3];
	gameMainUI->btnChat->unknown_2A8 = argColor[4];
	gameMainUI->btnChat->unknown_2A9 = argColor[5];
	assert(gameMainUI->btnDiplomacy != NULL);
	gameMainUI->btnDiplomacy->unknown_2A0 = 3;
	gameMainUI->btnDiplomacy->unknown_2A4 = argColor[0];
	gameMainUI->btnDiplomacy->unknown_2A5 = argColor[1];
	gameMainUI->btnDiplomacy->unknown_2A6 = argColor[2];
	gameMainUI->btnDiplomacy->unknown_2A7 = argColor[3];
	gameMainUI->btnDiplomacy->unknown_2A8 = argColor[4];
	gameMainUI->btnDiplomacy->unknown_2A9 = argColor[5];
	assert(gameMainUI->btnMenu != NULL);
	gameMainUI->btnMenu->unknown_2A0 = 3;
	gameMainUI->btnMenu->unknown_2A4 = argColor[0];
	gameMainUI->btnMenu->unknown_2A5 = argColor[1];
	gameMainUI->btnMenu->unknown_2A6 = argColor[2];
	gameMainUI->btnMenu->unknown_2A7 = argColor[3];
	gameMainUI->btnMenu->unknown_2A8 = argColor[4];
	gameMainUI->btnMenu->unknown_2A9 = argColor[5];
	assert(gameMainUI->btnHelp != NULL);
	gameMainUI->btnHelp->unknown_2A0 = 4;
	gameMainUI->btnHelp->unknown_2A4 = argColor[0];
	gameMainUI->btnHelp->unknown_2A5 = argColor[1];
	gameMainUI->btnHelp->unknown_2A6 = argColor[2];
	gameMainUI->btnHelp->unknown_2A7 = argColor[3];
	gameMainUI->btnHelp->unknown_2A8 = argColor[4];
	gameMainUI->btnHelp->unknown_2A9 = argColor[5];
	assert(gameMainUI->btnShowScores != NULL);
	gameMainUI->btnShowScores->unknown_2A0 = 4;
	gameMainUI->btnShowScores->unknown_2A4 = argColor[0];
	gameMainUI->btnShowScores->unknown_2A5 = argColor[1];
	gameMainUI->btnShowScores->unknown_2A6 = argColor[2];
	gameMainUI->btnShowScores->unknown_2A7 = argColor[3];
	gameMainUI->btnShowScores->unknown_2A8 = argColor[4];
	gameMainUI->btnShowScores->unknown_2A9 = argColor[5];
}

}
