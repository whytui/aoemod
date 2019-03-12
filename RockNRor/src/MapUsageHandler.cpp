#include "../include/MapUsageHandler.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
;



// Iterates to a "next" position in a square of radius "r". Positions are "offsets" from -radius to +radius.
// Does NOT iterate to "inside" positions. Example if radius=1, only "border" tiles of a 3*3 square are looped into:
// XXX
// X X
// XXX
bool MapUsageHandler::GetNextPosOffsetSquareAround(long int &curX, long int &curY, long int radius) {
	bool isOutOfBounds = (abs(curX) > radius) || (abs(curY) > radius);
	bool isXOnBorder = (abs(curX) == radius);
	bool isYOnBorder = (abs(curY) == radius);

	if (((curX == 0) && (curY == 0)) || isOutOfBounds || (!isXOnBorder && !isYOnBorder)) {
		// Init step (start at top-left position)
		curX = -radius;
		curY = -radius;
		return true;
	}
	if (curX == radius) {
		// Go to next "line"
		curX = -radius;
		curY++;
		return (curY <= radius);
	}
	if (isYOnBorder) { // on first/last "line", loop on all X positions
		curX++;
		return true;
	}
	// Inside lines : do NOT loop on inside X positions ; only go to first/last ones
	assert(curX == -radius);
	curX = radius; // (we already treated "x=radius" case, so here x is the left position (-radius)
	return true;
}


// Internal function
// Hardcoded part : gatherer must be a civilian, building must be granry, storage pit or TC.
// Detected resources are bushes (granary, TC), stone/gold mines, trees (SP, TC) ; based on unit AI type.
// Prey animals ignored, in case the player prefers to hunt manually
STRUCT_UNIT_BASE *MapUsageHandler::SearchGatherableTarget(long int posX, long int posY, long int searchRadius,
	const STRUCT_GAME_MAP_INFO *mapInfo, const STRUCT_PLAYER *gaia, bool granary, bool storagePit) {
	for (long int currRadius = 1; currRadius <= searchRadius; currRadius++) {
		long int offsetX = 0;
		long int offsetY = 0;
		while (GetNextPosOffsetSquareAround(offsetX, offsetY, currRadius)) {
			char cDist = RockNRorInfo::GetIntDistance((char)offsetX, (char)offsetY);
			if ((cDist < 0) || (cDist > searchRadius)) {
				continue; // skip (out of allowed search radius)
			}
			long int x = posX + offsetX;
			long int y = posY + offsetY;
			STRUCT_GAME_MAP_TILE_INFO *tile = mapInfo->GetTileInfo(x, y);
			if (tile) {
				STRUCT_UNIT_BASE_LIST *list = tile->unitsOnThisTile;
				while (list) {
					STRUCT_UNIT_BASE *tileUnit = list->unit;
					if (tileUnit && (tileUnit->ptrStructPlayer == gaia) && tileUnit->unitDefinition) {
						if (storagePit) {
							// Could add "resource" info in TTDetailedUnitDef so we can know a resource's possible deposit buildings ?
							//ROCKNROR::STRATEGY::TTDetailedUnitDef *detail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedUnitDef(tileUnit->unitInstanceId);
							if ((tileUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupGoldMine) ||
								(tileUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupStoneMine) ||
								//(tileUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPreyAnimal) ||
								(tileUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTree)) {
								return tileUnit;
							}
						}
						if (granary) {
							// Could add "resource" info in TTDetailedUnitDef so we can know a resource's possible deposit buildings ?
							//ROCKNROR::STRATEGY::TTDetailedUnitDef *detail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedUnitDef(tileUnit->unitInstanceId);
							if (tileUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORAGE_FRUIT) {
								return tileUnit;
							}
						}
					}
					list = list->next;
				}
			}
		}
	}
	return NULL;
}


// Search a gatherable target near a deposit building
// Hardcoded part : gatherer must be a civilian, building must be granry, storage pit or TC.
// Detected resources are bushes (granary, TC), stone/gold mines, trees (SP, TC) ; based on unit AI type.
// Prey animals ignored, in case the player prefers to hunt manually
STRUCT_UNIT_BASE *MapUsageHandler::SearchAutoGatherTargetNearDeposit(STRUCT_UNIT_TRAINABLE *gatherer, STRUCT_UNIT_BUILDING *depositBuilding) {
	if (!gatherer || !depositBuilding || !gatherer->DerivesFromTrainable() || !depositBuilding->IsCheckSumValid() ||
		!gatherer->ptrStructPlayer || !depositBuilding->unitDefinition || !gatherer->unitDefinition) {
		return NULL;
	}
	if (gatherer->ptrStructPlayer != depositBuilding->ptrStructPlayer) {
		return NULL;
	}

	STRUCT_UNITDEF_BUILDING *depositDef = (STRUCT_UNITDEF_BUILDING *)depositBuilding->unitDefinition;
	STRUCT_UNITDEF_TRAINABLE *gathererDef = (STRUCT_UNITDEF_TRAINABLE *)gatherer->unitDefinition;
	if (!depositDef->IsCheckSumValid() || !gathererDef->DerivesFromTrainable() ||
		(gathererDef->unitAIType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian)) { // gatherer must be civilian
		return NULL;
	}

	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid() || !global->gameMapInfo) {
		return NULL;
	}
	STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	STRUCT_PLAYER *gaia = global->GetPlayerStruct(0);
	if (!gaia) { return NULL; }

	bool granary = depositDef->DAT_ID1 == CST_UNITID_GRANARY;
	bool storagePit = depositDef->DAT_ID1 == CST_UNITID_STORAGE_PIT;
	if (depositDef->DAT_ID1 == CST_UNITID_FORUM) {
		granary = true;
		storagePit = true;
	}
	int depositPosX = (int)depositBuilding->positionX;
	int depositPosY = (int)depositBuilding->positionY;
	int gathererPosX = (int)gatherer->positionX;
	int gathererPosY = (int)gatherer->positionY;
	int searchSizeGatherer = (int)gathererDef->searchRadius;
	int searchSizeDeposit = (int)depositDef->searchRadius;

	const int maxSquareDiff = 3 * 3;
	int squareDiff = (depositPosX - gathererPosX) *  (depositPosX - gathererPosX) + (depositPosY - gathererPosY) * (depositPosY - gathererPosY);
	bool villagerIsNearDeposit = (squareDiff < maxSquareDiff);

	STRUCT_UNIT_BASE *result = NULL;
	// If the villager is far from deposit building, do NOT search a gatherable target near villager.
	if (villagerIsNearDeposit) {
		// Search around gatherer, going more and more far if necessary
		result = MapUsageHandler::SearchGatherableTarget(gathererPosX, gathererPosY, searchSizeGatherer, mapInfo, gaia, granary, storagePit);
		if (result != NULL) {
			return result;
		}
	}

	// Search around deposit building, going more and more far if necessary
	result = MapUsageHandler::SearchGatherableTarget(depositPosX, depositPosY, searchSizeDeposit, mapInfo, gaia, granary, storagePit);

	return result;
}



}
