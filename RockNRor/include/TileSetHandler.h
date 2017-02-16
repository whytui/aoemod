#pragma once
#include <string>
#include <AOE_const_drs.h>
#include <UI\AOE_struct_ui_in_game_main.h>
#include <AOE_structures_drs.h>
#include <TileSetDefinition.h>
#include "CustomTilesetInfo.h"
#include "traceMessage.h"
#include "drsHandler.h"
#include "AOE_memory.h"
#include "UI_utilities.h"

namespace TILESET {


// Handles custom treatments related to tilesets
class TilesetHandler {
public:
	TilesetHandler();

	// Overrides GameMainUI components initialization in 0x4978AF, to allow usage of custom tilesets
	void InitGameMainUITilesetDependentGraphics(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, long int tileset);

private:
	// Make sure that argColor's size is 6. (indexes 0-5)
	void SetBorderColors(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, unsigned char *argColor);
};

extern TilesetHandler tilesetHandler;

}
