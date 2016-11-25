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

// Describes the order of values in button border colors array (arguments for call 0x455950, etc)
enum BTN_BORDER_COLOR_ORDER {
	OUT_TOP_RIGHT = 0,
	MID_TOP_RIGHT = 1,
	IN_TOP_RIGHT = 2,
	IN_BOTTOM_LEFT = 3,
	MID_BOTTOM_LEFT = 4,
	OUT_BOTTOM_LEFT = 5
};

class CustomTilesetInfo {
public:
	CustomTilesetInfo();
	~CustomTilesetInfo();

	std::string tilesetName; // Optional name.
	bool isCustomized; // mostly useful for standard tilesets.
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
	unsigned char btnBorderColors[6]; // Index: cf BTN_BORDER_COLOR_ORDER

	// Sets "my" tileset ID and does some underlying processing.
	void SetTilesetId(long int tileset);
	inline long int GetTilesetId() const { return this->tilesetId; }

	// Get the SLPInfo for building icons for this tileset. Automatically handles SLP object loading/freeing.
	AOE_STRUCTURES::STRUCT_SLP_INFO *GetIconsForBuildings();

private:
	long int tilesetId;
	AOE_STRUCTURES::STRUCT_SLP_INFO *iconsForBuildings; // SLPInfo object for building icons. Loaded only when necessary, freed when "this" is destroyed.

	// Set SLP info object for building icons for this tileset. It needs to be freed when "this" is destroyed.
	void InitBuildingIcons();

	// Initialized button border colors using hardcoded values from ROR code.
	// Requires this->tilesetId to be set correctly
	void InitHardcodedBtnBorderColors();

};


class TilesetHandler {
public:
	TilesetHandler();

	// Securely gets tileset info. Returns NULL if tileset is invalid
	CustomTilesetInfo *GetTilesetInfo(long int tileset);
	// Returns true if the specified tileset (ID) is customized by configuration.
	bool IsCustomized(long int tileset);

	int tilesetCount;
	bool usesCustomCivs; // Set to true if specific configuration for tilesets is being used.
	bool allowCustomizeStandardTilesets; // If true, it is allowed to modify standard tilesets.

	// Overrides GameMainUI components initialization in 0x4978AF, to allow usage of custom tilesets
	void InitGameMainUITilesetDependentGraphics(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, long int tileset);

	// Get the slpinfo for building icons for a custom tileset
	// Return NULL if not found (maybe tileset is invalid)
	AOE_STRUCTURES::STRUCT_SLP_INFO *GetBuildingIconsSlpInfoForTileSet(long int tileset);

private:
	CustomTilesetInfo tilesetsInfo[256];

	// Make sure that argColor's size is 6. (indexes 0-5)
	void SetBorderColors(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, unsigned char *argColor);

	void InitStandardTilesetsData();
};

extern TilesetHandler tilesetHandler;

}
