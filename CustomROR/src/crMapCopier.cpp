#include "../include/crMapCopier.h"

using namespace MAP;

// Define it only once (in .cpp file) so all files will use the same instance.
MapCopier MAP::mapCopierInstance;

MapCopier::MapCopier() {
	this->copiedSizeX = 0;
	this->copiedSizeY = 0;
}

MapCopier::~MapCopier() {
	this->ClearCopiedUnits();
}

void MapCopier::ClearCopiedUnits() {
	for each (UNIT_INSTANCE_SERIALIZED_DATA *unitInfo in this->copiedUnits)
	{
		delete unitInfo;
	}
	this->copiedUnits.clear();
}


// Copy map data into internal buffer. Terrain+units
bool MapCopier::CopyMapZone(long int minX, long int minY, long int maxX, long int maxY, bool includeUnits) {
	this->copiedSizeX = 0;
	this->copiedSizeY = 0;
	this->ClearCopiedUnits();
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert((global != NULL) && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	assert((global->gameMapInfo != NULL) && (global->gameMapInfo->IsCheckSumValid()));
	if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return false; }
	STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	if ((minX < 0) || (minY < 0)) { return false; }
	if ((maxX >= mapInfo->mapArraySizeX) || (maxY >= mapInfo->mapArraySizeY)) { return false; }
	if ((maxX < minX) || (maxY < minY)) { return false; }

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			STRUCT_GAME_MAP_TILE_INFO *tile = mapInfo->GetTileInfo(x, y);
			if (tile) {
				this->copiedTerrainData[x - minX][y - minY] = tile->terrainData;
				this->copiedBorderData[x - minX][y - minY] = tile->terrainBorderData;
				this->copiedElevationIndexData[x - minX][y - minY] = tile->elevationGraphicsIndex;
				for (int i = 0; i < tile->unitsOnThisTileCount; i++) {
					STRUCT_UNIT_BASE_LIST *listElem = tile->unitsOnThisTile;
					if (!listElem) { break; } // stop as soon as a NULL element is found (should not happen if unitsOnThisTileCount is correct)
					STRUCT_UNIT_BASE *unit = listElem->unit;
					if (unit && !unit->IsCheckSumValidForAUnitClass()) { unit = NULL; }
					bool isDecorationOrCliff = unit->ptrStructDefUnit && (
						(unit->ptrStructDefUnit->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTerrain) ||
						(unit->ptrStructDefUnit->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCliff));
					bool copyUnit = includeUnits || isDecorationOrCliff; // Always copy terrain decoration, cliffs
					if (copyUnit && unit && unit->IsCheckSumValidForAUnitClass()) {
						UNIT_INSTANCE_SERIALIZED_DATA *unitData = new UNIT_INSTANCE_SERIALIZED_DATA();
						// Store relative position, not absolute !
						unitData->posX = unit->positionX - (float)minX;
						unitData->posY = unit->positionY - (float)minY;
						if (unit->ptrStructDefUnit) {
							unitData->unitDefId = unit->ptrStructDefUnit->DAT_ID1;
						}
						if (unit->ptrStructPlayer && unit->ptrStructPlayer->IsCheckSumValid()) {
							unitData->playerId = unit->ptrStructPlayer->playerId;
						}
						unitData->unitId = unit->unitInstanceId;
						unitData->orientation = unit->orientation; // unsure ?
						unitData->status = unit->unitStatus;
						this->copiedUnits.push_back(unitData);
					}
				}
				//tile->unitsOnThisTileCount...
			} else {
				std::string msg = "Error: could not access tile info (";
				msg += std::to_string(x);
				msg += ", ";
				msg += std::to_string(y);
				msg += ")";
				traceMessageHandler.WriteMessage(msg);
				return false;
			}
		}
	}
	// Validate copy only once everything is finished without errors
	this->copiedSizeX = maxX - minX;
	this->copiedSizeY = maxY - minY;
	return true;
}

// Paste from internal buffer to actual map
bool MapCopier::PasteMapZone(long int startPosX, long int startPosY) {
	if ((this->copiedSizeX <= 0) || (this->copiedSizeY <= 0)) { return false; }
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert((global != NULL) && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	assert((global->gameMapInfo != NULL) && (global->gameMapInfo->IsCheckSumValid()));
	if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return false; }
	STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	if ((startPosX < 0) || (startPosY < 0)) { return false; }
	long int maxX = startPosX + this->copiedSizeX;
	long int maxY = startPosY + this->copiedSizeY;
	if ((maxX >= mapInfo->mapArraySizeX) || (maxY >= mapInfo->mapArraySizeY)) {
		traceMessageHandler.WriteMessage("Could not paste map data, map is not large enough");
		return false;
	}
	assert(maxX < this->maxArraySize);
	assert(maxY < this->maxArraySize);
	// Clear target map zone first
	for (int x = startPosX; x <= maxX; x++) {
		for (int y = startPosY; y <= maxY; y++) {
			STRUCT_GAME_MAP_TILE_INFO *tile = mapInfo->GetTileInfo(x, y);
			if (tile) {
				std::set<STRUCT_UNIT_BASE *>unitsToDelete;
				for (int i = 0; i < tile->unitsOnThisTileCount; i++) {
					STRUCT_UNIT_BASE_LIST *listElem = tile->unitsOnThisTile;
					if (!listElem) { break; } // stop as soon as a NULL element is found (should not happen if unitsOnThisTileCount is correct)
					STRUCT_UNIT_BASE *unit = listElem->unit;
					if (unit && unit->IsCheckSumValidForAUnitClass()) {
						unitsToDelete.insert(unit); // I prefer NOT to delete units inside this loop. Seems not wise.
					} else {
						traceMessageHandler.WriteMessage("Encountered invalid unit data :(");
					}
				}
				for each (STRUCT_UNIT_BASE *unit in unitsToDelete)
				{
					assert(unit->IsCheckSumValidForAUnitClass());
					unit->AOE_destructor(true);
				}
				unitsToDelete.clear();
			} else {
				std::string msg = "Error: could not access tile info (";
				msg += std::to_string(x);
				msg += ", ";
				msg += std::to_string(y);
				msg += ")";
				traceMessageHandler.WriteMessage(msg);
				return false;
			}
		}
	}
	// Proceed to paste operation
	for (int x = startPosX; x <= maxX; x++) {
		for (int y = startPosY; y <= maxY; y++) {
			STRUCT_GAME_MAP_TILE_INFO *tile = mapInfo->GetTileInfo(x, y);
			if (tile) {
				tile->terrainData = this->copiedTerrainData[x - startPosX][y - startPosY];
				tile->terrainBorderData = this->copiedBorderData[x - startPosX][y - startPosY];
				tile->elevationGraphicsIndex = this->copiedElevationIndexData[x - startPosX][y - startPosY];
				mapInfo->RecomputeTileDisplayPos(x, y); // Necessary when elevation data has been modified
			} else {
				std::string msg = "Error: could not access tile info (";
				msg += std::to_string(x);
				msg += ", ";
				msg += std::to_string(y);
				msg += ")";
				traceMessageHandler.WriteMessage(msg);
				return false;
			}
		}
	}
	// Units
	for each (UNIT_INSTANCE_SERIALIZED_DATA *unitData in this->copiedUnits)
	{
		if (unitData) {
			// Stored position is relative one
			float newPosX = unitData->posX + startPosX;
			float newPosY = unitData->posY + startPosY;
			assert(newPosX >= 0);
			assert(newPosY >= 0);
			//GetErrorForUnitCreationAtLocation(, , unitData->posY, unitData->posX, false, false, true, false, false);
			STRUCT_PLAYER *player = global->GetPlayerStruct(unitData->playerId);
			if (player && player->IsCheckSumValid()) {
				STRUCT_UNIT_BASE *unit = (STRUCT_UNIT_BASE *)CreateUnit(player, unitData->unitDefId,
					newPosY, newPosX, 0);
				if (unit) {
					unit->orientation = unitData->orientation;
					unit->unitStatus = unitData->status;
					// unitData->unitId is not preserved during this operation.
				}
			}
		}
	}
	DiamondMapDrawAllTiles();
	return true;
}
