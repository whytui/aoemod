#include "../include/TileSetHandler.h"

namespace TILESET {

// Static object
TilesetHandler tilesetHandler;

using namespace AOE_CONST_DRS;
using namespace AOE_STRUCTURES;


CustomTilesetInfo::CustomTilesetInfo() {
	this->isCustomized = false;
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
	this->iconsForBuildings = NULL;
	// Initialize button border colors (to 0 here as tilesetId is still undetermined)
	this->InitHardcodedBtnBorderColors();
}


CustomTilesetInfo::~CustomTilesetInfo() {
	if (this->iconsForBuildings != NULL) {
		AOEFree(this->iconsForBuildings);
	}
}


// Set SLP info object for building icons for this tileset. It needs to be freed when "this" is destroyed.
void CustomTilesetInfo::InitBuildingIcons() {
	assert(this->iconsForBuildings == NULL);
	this->iconsForBuildings = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	if (!this->iconsForBuildings) {
		traceMessageHandler.WriteMessage("Error with AOEAlloc for iconsForBuildings");
		return;
	}
	AOE_METHODS::InitSlpInfoFromDrs(this->iconsForBuildings, this->slpIdBuildingIcons, "");
}


// Initialized button border colors using hardcoded values from ROR code.
// Requires this->tilesetId to be set correctly
void CustomTilesetInfo::InitHardcodedBtnBorderColors() {
	switch (this->tilesetId) {
	case TILESET_EGYPT:
		this->btnBorderColors[0] = 0x7E;
		this->btnBorderColors[1] = 0x6D;
		this->btnBorderColors[2] = 0x6F;
		this->btnBorderColors[3] = 0x6F;
		this->btnBorderColors[4] = 0xEE;
		this->btnBorderColors[5] = 0x38;
		return;
	case TILESET_GREECE:
		this->btnBorderColors[0] = 0x72;
		this->btnBorderColors[1] = 0x73;
		this->btnBorderColors[2] = 0x74;
		this->btnBorderColors[3] = 0x74;
		this->btnBorderColors[4] = 0xB7;
		this->btnBorderColors[5] = 0xB8;
		return;
	case TILESET_BABYLONIAN:
		this->btnBorderColors[0] = 0xB9;
		this->btnBorderColors[1] = 0x77;
		this->btnBorderColors[2] = 0x78;
		this->btnBorderColors[3] = 0x78;
		this->btnBorderColors[4] = 0x78;
		this->btnBorderColors[5] = 0x79;
		return;
	case TILESET_ASIAN:
		this->btnBorderColors[0] = 0x8A;
		this->btnBorderColors[1] = 0xED;
		this->btnBorderColors[2] = 0xEE;
		this->btnBorderColors[3] = 0xEE;
		this->btnBorderColors[4] = 0x38;
		this->btnBorderColors[5] = 0x95;
		return;
	case TILESET_ROMAN:
		this->btnBorderColors[0] = 0x73;
		this->btnBorderColors[1] = 0x74;
		this->btnBorderColors[2] = 0x75;
		this->btnBorderColors[3] = 0x75;
		this->btnBorderColors[4] = 0x76;
		this->btnBorderColors[5] = 0x77;
		return;
	default:
		this->btnBorderColors[0] = 0;
		this->btnBorderColors[1] = 0;
		this->btnBorderColors[2] = 0;
		this->btnBorderColors[3] = 0;
		this->btnBorderColors[4] = 0;
		this->btnBorderColors[5] = 0;
	}
}

// Sets "my" tileset ID and does some underlying processing.
void CustomTilesetInfo::SetTilesetId(long int tileset) {
	this->tilesetId = tileset;
	this->InitHardcodedBtnBorderColors();
}


// Get the SLPInfo for building icons for this tileset. Automatically handles SLP object loading/freeing.
AOE_STRUCTURES::STRUCT_SLP_INFO *CustomTilesetInfo::GetIconsForBuildings() {
	if (this->iconsForBuildings == NULL) {
		this->InitBuildingIcons();
	}
	return this->iconsForBuildings;
}




TilesetHandler::TilesetHandler() {
	this->usesCustomCivs = false;
	this->allowCustomizeStandardTilesets = true;
	this->tilesetCount = TILESET_COUNT_STANDARD_ROR;
	// Initialize standard tilesets data
	this->InitStandardTilesetsData();
}


CustomTilesetInfo *TilesetHandler::GetTilesetInfo(long int tileset) {
	assert(tileset >= 0);
	assert(tileset < MAX_TILESET_TOTAL_SUPPORTED_COUNT);
	if ((tileset < 0) || (tileset >= MAX_TILESET_TOTAL_SUPPORTED_COUNT)) { return NULL; }
	return &this->tilesetsInfo[tileset];
}


// Returns true if the specified tileset (ID) is customized by configuration.
bool TilesetHandler::IsCustomized(long int tileset) {
	assert(tileset >= 0);
	assert(tileset < MAX_TILESET_TOTAL_SUPPORTED_COUNT);
	CustomTilesetInfo *info = this->GetTilesetInfo(tileset);
	return (info && info->isCustomized);
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
	if (this->tilesetsInfo[tileset].GetTilesetId() == tileset) {
		slpId = this->tilesetsInfo[tileset].slpIdThemeInfo;
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
	if (this->tilesetsInfo[tileset].GetTilesetId() == tileset) {
		slpId = this->tilesetsInfo[tileset].slpIdCheckboxes;
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
	this->SetBorderColors(gameMainUI, this->GetTilesetInfo(tileset)->btnBorderColors);
	
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
	if (this->tilesetsInfo[tileset].GetTilesetId() == tileset) {
		slpId = this->tilesetsInfo[tileset].slpIdButtonBoardLow;
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
	if (this->tilesetsInfo[tileset].GetTilesetId() == tileset) {
		slpId = this->tilesetsInfo[tileset].slpIdCommonCommandIcons;
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
	if (this->tilesetsInfo[tileset].GetTilesetId() == tileset) {
		slpId = this->tilesetsInfo[tileset].slpIdButtonBoardMedium;
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
	if (this->tilesetsInfo[tileset].GetTilesetId() == tileset) {
		slpId = this->tilesetsInfo[tileset].slpIdButtonBoardHigh;
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
		if (this->tilesetsInfo[tileset].GetTilesetId() == tileset) {
			slpId = this->tilesetsInfo[tileset].slpIdGameScreenLow;
		} else {
			slpId = SLPID_TILESET_GAME_FRIEZES_LOW_RESOLUTION_BASE_STD; // Fallback in error cases (we should not end up in this block)
		}
		break;
	case 2:
		if (this->tilesetsInfo[tileset].GetTilesetId() == tileset) {
			slpId = this->tilesetsInfo[tileset].slpIdGameScreenMedium;
		} else {
			slpId = SLPID_TILESET_GAME_FRIEZES_MEDIUM_RESOLUTION_BASE_STD; // Fallback in error cases (we should not end up in this block)
		}
		break;
	case 3:
		if (this->tilesetsInfo[tileset].GetTilesetId() == tileset) {
			slpId = this->tilesetsInfo[tileset].slpIdGameScreenHigh;
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
}


// Get the slpinfo for building icons for a custom tileset
STRUCT_SLP_INFO *TilesetHandler::GetBuildingIconsSlpInfoForTileSet(long int tileset) {
	CustomTilesetInfo *info = this->GetTilesetInfo(tileset);
	if (!info || (info->GetTilesetId() != tileset)) {
		return NULL;
	}
	return info->GetIconsForBuildings();
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


void TilesetHandler::InitStandardTilesetsData() {
	for (int tsid = 0; tsid <= MAX_TILESET_ID_WITH_STANDARD_SLP_IDS; tsid++) {
		this->tilesetsInfo[tsid].slpIdBuildingIcons = SLPID_TILESET_BLD_ICONS_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdButtonBoardHigh = SLPID_TILESET_BUTTON_BOARD_HIGH_RESOLUTION_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdButtonBoardMedium = SLPID_TILESET_BUTTON_BOARD_MEDIUM_RESOLUTION_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdButtonBoardLow = SLPID_TILESET_BUTTON_BOARD_LOW_RESOLUTION_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdCheckboxes= SLPID_TILESET_CHECKBOXES_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdCommonCommandIcons = SLPID_TILESET_COMMON_CMD_ICONS_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdGameScreenHigh = SLPID_TILESET_GAME_FRIEZES_HIGH_RESOLUTION_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdGameScreenMedium = SLPID_TILESET_GAME_FRIEZES_MEDIUM_RESOLUTION_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdGameScreenLow = SLPID_TILESET_GAME_FRIEZES_LOW_RESOLUTION_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdThemeInfo = SLPID_TILESET_SCREEN_THEME_BASE_STD + tsid;
		this->tilesetsInfo[tsid].isCustomized = false;
		this->tilesetsInfo[tsid].SetTilesetId(tsid);
		this->tilesetsInfo[tsid].tilesetName = std::string("Standard tileset #") + std::to_string(tsid);
	}
	// ROR tileset
	this->tilesetsInfo[TILESET_ROMAN].slpIdBuildingIcons = SLPID_TILESET_BLD_ICONS_ROR;
	this->tilesetsInfo[TILESET_ROMAN].slpIdButtonBoardHigh = SLPID_TILESET_BUTTON_BOARD_HIGH_RESOLUTION_ROR;
	this->tilesetsInfo[TILESET_ROMAN].slpIdButtonBoardMedium = SLPID_TILESET_BUTTON_BOARD_MEDIUM_RESOLUTION_ROR;
	this->tilesetsInfo[TILESET_ROMAN].slpIdButtonBoardLow = SLPID_TILESET_BUTTON_BOARD_LOW_RESOLUTION_ROR;
	this->tilesetsInfo[TILESET_ROMAN].slpIdCheckboxes = SLPID_TILESET_CHECKBOXES_ROR;
	this->tilesetsInfo[TILESET_ROMAN].slpIdCommonCommandIcons = SLPID_TILESET_COMMON_CMD_ICONS_ROR;
	this->tilesetsInfo[TILESET_ROMAN].slpIdGameScreenHigh = SLPID_TILESET_GAME_FRIEZES_HIGH_RESOLUTION_ROR;
	this->tilesetsInfo[TILESET_ROMAN].slpIdGameScreenMedium = SLPID_TILESET_GAME_FRIEZES_MEDIUM_RESOLUTION_ROR;
	this->tilesetsInfo[TILESET_ROMAN].slpIdGameScreenLow = SLPID_TILESET_GAME_FRIEZES_LOW_RESOLUTION_ROR;
	this->tilesetsInfo[TILESET_ROMAN].slpIdThemeInfo = SLPID_TILESET_SCREEN_THEME_ROR;
	this->tilesetsInfo[TILESET_ROMAN].isCustomized = false;
	this->tilesetsInfo[TILESET_ROMAN].SetTilesetId(TILESET_ROMAN); // Also initializes button border colors
	this->tilesetsInfo[TILESET_ROMAN].tilesetName = std::string("Standard tileset #") + std::to_string(TILESET_ROMAN);
}

}
