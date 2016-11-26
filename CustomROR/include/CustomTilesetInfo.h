#pragma once
#include <AOE_structures_drs.h>
#include <TileSetDefinition.h>
#include "AOE_memory.h"
#include "traceMessage.h"
#include "drsHandler.h"

namespace TILESET {


class CustomTilesetInfo : public TilesetDefinition {
public:
	CustomTilesetInfo();
	~CustomTilesetInfo();

	// Get the SLPInfo for building icons for this tileset. Automatically handles SLP object loading/freeing.
	AOE_STRUCTURES::STRUCT_SLP_INFO *GetIconsForBuildings();

private:
	AOE_STRUCTURES::STRUCT_SLP_INFO *iconsForBuildings; // SLPInfo object for building icons. Loaded only when necessary, freed when "this" is destroyed.

	// Set SLP info object for building icons for this tileset. It needs to be freed when "this" is destroyed.
	void InitBuildingIcons();
};


// Container for all tilesets information
class TilesetsInfo {
public:
	TilesetsInfo();

	int tilesetCount;
	bool usesCustomCivs; // Set to true if specific configuration for tilesets is being used.
	bool allowCustomizeStandardTilesets; // If true, it is allowed to modify standard tilesets.
	CustomTilesetInfo tilesetsInfo[256];

	// Securely gets tileset info. Returns NULL if tileset is invalid
	CustomTilesetInfo *GetTilesetInfo(long int tileset);
	// Returns true if the specified tileset (ID) is customized by configuration.
	bool IsCustomized(long int tileset);

	// Get the slpinfo for building icons for a custom tileset
	// Return NULL if not found (maybe tileset is invalid)
	AOE_STRUCTURES::STRUCT_SLP_INFO *GetBuildingIconsSlpInfoForTileSet(long int tileset);

private:
	// Init ROR hardcoded configuration in our internal configuration
	void InitStandardTilesetsData();
};

// Common object to access tilesets information
extern TilesetsInfo allTilesetsInfo;


}
