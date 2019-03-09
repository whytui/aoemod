
#pragma once

#include <assert.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_game_global.h>
#include <AOE_struct_units.h>
#include <AOE_struct_game_map_info.h>
#include "RockNRorInfo.h"
#include "mainStructuresHandling.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
;

class MapUsageHandler {
public:

	// Search a gatherable target near a deposit building
	// Hardcoded part : gatherer must be a civilian, building must be granry, storage pit or TC.
	// Detected resources are bushes (granary, TC), stone/gold mines, trees (SP, TC) ; based on unit AI type.
	// Prey animals ignored, in case the player prefers to hunt manually
	static STRUCT_UNIT_BASE *SearchAutoGatherTargetNearDeposit(STRUCT_UNIT_TRAINABLE *gatherer, STRUCT_UNIT_BUILDING *depositBuilding);


private:
	// Iterates to a "next" position in a square of radius "r". Positions are "offsets" from -radius to +radius.
	// Does NOT iterate to "inside" positions. Example if radius=1, only "border" tiles of a 3*3 square are looped into:
	// XXX
	// X X
	// XXX
	static bool GetNextPosOffsetSquareAround(long int &curX, long int &curY, long int radius);

	// Internal function
	// Hardcoded part : gatherer must be a civilian, building must be granry, storage pit or TC.
	// Detected resources are bushes (granary, TC), stone/gold mines, trees (SP, TC) ; based on unit AI type.
	// Prey animals ignored, in case the player prefers to hunt manually
	static STRUCT_UNIT_BASE *SearchGatherableTarget(long int posX, long int posY, long int searchRadius,
		const STRUCT_GAME_MAP_INFO *mapInfo, const STRUCT_PLAYER *gaia, bool granary, bool storagePit);
};

}
