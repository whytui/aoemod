#include "../include/TileSetDefinition.h"

namespace TILESET {
;


TilesetDefinition::TilesetDefinition() {
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
	// Initialize button border colors (to 0 here as tilesetId is still undetermined)
	this->InitHardcodedBtnBorderColors();
}



// Initialized button border colors using hardcoded values from ROR code.
// Requires this->tilesetId to be set correctly
void TilesetDefinition::InitHardcodedBtnBorderColors() {
	switch (this->tilesetId) {
	case TILESET_EGYPT:
		this->btnBorderColors[0] = 0x7E;
		this->btnBorderColors[1] = 0x6D;
		this->btnBorderColors[2] = 0x6F;
		this->btnBorderColors[3] = 0x6F;
		this->btnBorderColors[4] = 0xEE;
		this->btnBorderColors[5] = 0x38;
		this->textColorRGB = 0;
		this->textShadowColorRGB = 0xFFFFFF;
		return;
	case TILESET_GREECE:
		this->btnBorderColors[0] = 0x72;
		this->btnBorderColors[1] = 0x73;
		this->btnBorderColors[2] = 0x74;
		this->btnBorderColors[3] = 0x74;
		this->btnBorderColors[4] = 0xB7;
		this->btnBorderColors[5] = 0xB8;
		this->textColorRGB = 0;
		this->textShadowColorRGB = 0xFFFFFF;
		return;
	case TILESET_BABYLONIAN:
		this->btnBorderColors[0] = 0xB9;
		this->btnBorderColors[1] = 0x77;
		this->btnBorderColors[2] = 0x78;
		this->btnBorderColors[3] = 0x78;
		this->btnBorderColors[4] = 0x78;
		this->btnBorderColors[5] = 0x79;
		this->textColorRGB = 0xFFFFFF; // ROR default = white
		this->textShadowColorRGB = 0x000000; // ROR default = black
		return;
	case TILESET_ASIAN:
		this->btnBorderColors[0] = 0x8A;
		this->btnBorderColors[1] = 0xED;
		this->btnBorderColors[2] = 0xEE;
		this->btnBorderColors[3] = 0xEE;
		this->btnBorderColors[4] = 0x38;
		this->btnBorderColors[5] = 0x95;
		this->textColorRGB = 0xFFFFFF; // ROR default = white
		this->textShadowColorRGB = 0x000000; // ROR default = black
		return;
	case TILESET_ROMAN:
		this->btnBorderColors[0] = 0x73;
		this->btnBorderColors[1] = 0x74;
		this->btnBorderColors[2] = 0x75;
		this->btnBorderColors[3] = 0x75;
		this->btnBorderColors[4] = 0x76;
		this->btnBorderColors[5] = 0x77;
		this->textColorRGB = 0xFFFFFF; // ROR default = white
		this->textShadowColorRGB = 0x000000; // ROR default = black
		return;
	default:
		// Applies to all non-stndard tilesets (those are just default values)
		this->btnBorderColors[0] = 0;
		this->btnBorderColors[1] = 0;
		this->btnBorderColors[2] = 0;
		this->btnBorderColors[3] = 0;
		this->btnBorderColors[4] = 0;
		this->btnBorderColors[5] = 0;
		this->textColorRGB = 0xFFFFFF; // ROR default = white
		this->textShadowColorRGB = 0x000000; // ROR default = black
	}
}

// Sets "my" tileset ID and does some underlying processing (init).
// Call this before using other members/methods
void TilesetDefinition::SetTilesetId(long int tileset) {
	this->tilesetId = tileset;
	this->InitHardcodedBtnBorderColors();
}


// Set (unsigned int) text color value from RGB 0-255 values
void TilesetDefinition::SetTextColor(unsigned char red, unsigned char green, unsigned char blue) {
	this->textColorRGB = (blue << 16) + (green << 8) + red;
}
// Set (unsigned int) text shadow color value from RGB 0-255 values
void TilesetDefinition::SetTextShadowColor(unsigned char red, unsigned char green, unsigned char blue) {
	this->textShadowColorRGB = (blue << 16) + (green << 8) + red;
}


}
