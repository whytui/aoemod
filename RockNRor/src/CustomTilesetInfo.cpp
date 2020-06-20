#include "../include/CustomTilesetInfo.h"
#include "../include/drsHandler.h"

namespace TILESET {

using namespace AOE_CONST_DRS;
using namespace AOE_STRUCTURES;


// Static common object
TilesetsInfo allTilesetsInfo;


CustomTilesetInfo::CustomTilesetInfo() {
	this->iconsForBuildings = NULL;
}


CustomTilesetInfo::~CustomTilesetInfo() {
	if (this->iconsForBuildings != NULL) {
		AOE_METHODS::FreeSlpInfo(this->iconsForBuildings);
		this->iconsForBuildings = NULL;
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


// Get the SLPInfo for building icons for this tileset. Automatically handles SLP object loading/freeing.
AOE_STRUCTURES::STRUCT_SLP_INFO *CustomTilesetInfo::GetIconsForBuildings() {
	if (this->iconsForBuildings == NULL) {
		this->InitBuildingIcons();
	}
	return this->iconsForBuildings;
}


TilesetsInfo::TilesetsInfo() {
	this->usesCustomCivs = false;
	this->allowCustomizeStandardTilesets = true;
	this->tilesetCount = TILESET_COUNT_STANDARD_ROR;
	// Initialize standard tilesets data
	this->InitStandardTilesetsData();
}


// Securely gets tileset info. Returns NULL if tileset is invalid
CustomTilesetInfo *TilesetsInfo::GetTilesetInfo(long int tileset) {
	assert(tileset >= 0);
	assert(tileset < MAX_TILESET_TOTAL_SUPPORTED_COUNT);
	if ((tileset < 0) || (tileset >= MAX_TILESET_TOTAL_SUPPORTED_COUNT)) { return NULL; }
	return &this->tilesetsInfo[tileset];
}


// Returns true if the specified tileset (ID) is customized by configuration.
bool TilesetsInfo::IsCustomized(long int tileset) {
	assert(tileset >= 0);
	assert(tileset < MAX_TILESET_TOTAL_SUPPORTED_COUNT);
	CustomTilesetInfo *info = this->GetTilesetInfo(tileset);
	return (info && info->isCustomized);
}


// Get the slpinfo for building icons for a custom tileset
// Return NULL if not found (maybe tileset is invalid)
STRUCT_SLP_INFO *TilesetsInfo::GetBuildingIconsSlpInfoForTileSet(long int tileset) {
	CustomTilesetInfo *info = this->GetTilesetInfo(tileset);
	if (!info || (info->GetTilesetId() != tileset)) {
		return NULL;
	}
	return info->GetIconsForBuildings();
}


// Init ROR hardcoded configuration in our internal configuration
void TilesetsInfo::InitStandardTilesetsData() {
	for (int tsid = 0; tsid <= MAX_TILESET_ID_WITH_STANDARD_SLP_IDS; tsid++) {
		this->tilesetsInfo[tsid].slpIdBuildingIcons = SLPID_TILESET_BLD_ICONS_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdButtonBoardHigh = SLPID_TILESET_BUTTON_BOARD_HIGH_RESOLUTION_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdButtonBoardMedium = SLPID_TILESET_BUTTON_BOARD_MEDIUM_RESOLUTION_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdButtonBoardLow = SLPID_TILESET_BUTTON_BOARD_LOW_RESOLUTION_BASE_STD + tsid;
		this->tilesetsInfo[tsid].slpIdCheckboxes = SLPID_TILESET_CHECKBOXES_BASE_STD + tsid;
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
