#pragma once
#include <string>
#include <AOE_const_drs.h>
#include <AOE_structures_drs.h>


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

// Describes the order of values in button border colors array (arguments for call 0x455950, etc). This is a very technical information.
enum BTN_BORDER_COLOR_ORDER {
	OUT_TOP_RIGHT = 0,
	MID_TOP_RIGHT = 1,
	IN_TOP_RIGHT = 2,
	IN_BOTTOM_LEFT = 3,
	MID_BOTTOM_LEFT = 4,
	OUT_BOTTOM_LEFT = 5
};

class TilesetDefinition{
public:
	TilesetDefinition();

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
	unsigned long int textColorRGB;
	unsigned long int textShadowColorRGB;

	// Sets "my" tileset ID and does some underlying processing (init).
	// Call this before using other members/methods
	void SetTilesetId(long int tileset);
	inline long int GetTilesetId() const { return this->tilesetId; }

	// Set (unsigned int) text color value from RGB 0-255 values
	void SetTextColor(unsigned char red, unsigned char green, unsigned char blue);
	// Set (unsigned int) text shadow color value from RGB 0-255 values
	void SetTextShadowColor(unsigned char red, unsigned char green, unsigned char blue);

protected:
	long int tilesetId;

private:
	// Initialized button border colors using hardcoded values from ROR code.
	// Requires this->tilesetId to be set correctly
	void InitHardcodedBtnBorderColors();

};

}
