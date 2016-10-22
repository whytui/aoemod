#pragma once
#include <string>
#include <AOE_const_drs.h>
#include <UI\AOE_struct_ui_in_game_main.h>
#include <AOE_structures_drs.h>
#include "traceMessage.h"
#include "drsHandler.h"
#include "AOE_memory.h"

namespace TILESET {

const long int MAX_TILESET_ID_WITH_STANDARD_SLP_IDS = 3; // The last tileset ID supported by AOE, and using "standard" (consecutive) SLP IDs in DRS files.
const long int MAX_STANDARD_TILESET_ID = 4; // The last tileset ID supported by ROR (=roman tileset).
const long int MAX_TILESET_TOTAL_SUPPORTED_COUNT = 256;

// Standard tileset IDs
enum TILESET_ID {
	TILESET_EGYPT = 0,
	TILESET_GREECE = 1,
	TILESET_BABYLONIAN = 2,
	TILESET_ASIAN = 3,
	TILESET_ROMAN = 4,
	TILESET_COUNT_STANDARD_ROR
};


class CustomTilesetInfo {
public:
	CustomTilesetInfo();

	long int tilesetId;
	std::string tilesetName; // Optional name.
	long int slpIdThemeInfo;
	long int slpIdCheckboxes;
	long int slpIdBuildingIcons;
	long int slpIdCommonCommandIcons;
	long int slpIdButtonBoardLow;
	long int slpIdButtonBoardMedium;
	long int slpIdButtonBoardHigh;
	long int slpIdGameScreenLow;
	long int slpIdGameScreenMedium;
	long int slpIdGameScreenHigh;
};


class TilesetHandler {
public:
	TilesetHandler();

	CustomTilesetInfo *GetTilesetInfo(long int tileset);

	int tilesetCount;

	// Overrides GameMainUI components initialization in 0x4978AF, to allow usage of custom tilesets
	void InitGameMainUITilesetDependentGraphics(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, long int tileset);


private:
	CustomTilesetInfo tilesetsInfo[256];

	// Make sure that argColor's size is 6. (indexes 0-5)
	void FillColorArgs(unsigned char *argColor, long int tileset);
	void SetBorderColors(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, unsigned char *argColor);
};

extern TilesetHandler tilesetHandler;

}
