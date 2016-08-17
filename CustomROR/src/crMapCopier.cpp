#include "../include/crMapCopier.h"

using namespace MAP;

// Define it only once (in .cpp file) so all files will use the same instance.
MapCopier MAP::mapCopierInstance;

MapCopier::MapCopier() {
	this->copiedSizeX = 0;
	this->copiedSizeY = 0;
	this->copiedBufferContainsUnits = false;
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
	this->copiedBufferContainsUnits = false;
}


// Copy map data into internal buffer. Terrain+units
bool MapCopier::CopyMapZone(long int minX, long int minY, long int maxX, long int maxY, bool includeUnits) {
	this->copiedSizeX = 0;
	this->copiedSizeY = 0;
	this->ClearCopiedUnits();
	this->lastError = "Technical error";
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert((global != NULL) && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	assert((global->gameMapInfo != NULL) && (global->gameMapInfo->IsCheckSumValid()));
	if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return false; }
	STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	if ((minX < 0) || (minY < 0)) {
		this->lastError = "Negative coordinates";
		return false; }
	if ((maxX >= mapInfo->mapArraySizeX) || (maxY >= mapInfo->mapArraySizeY)) {
		this->lastError = "Max position is out of map bounds";
		return false;
	}
	if ((maxX < minX) || (maxY < minY)) {
		this->lastError = "Inconsistent minimum and maximum positions";
		return false;
	}

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			STRUCT_GAME_MAP_TILE_INFO *tile = mapInfo->GetTileInfo(x, y);
			if (tile) {
				this->copiedTerrainData[x - minX][y - minY] = tile->terrainData;
				this->copiedBorderData[x - minX][y - minY] = tile->terrainBorderData;
				this->copiedElevationIndexData[x - minX][y - minY] = tile->elevationGraphicsIndex;
				int loopindex;
				STRUCT_UNIT_BASE_LIST *listElem = tile->unitsOnThisTile;
				for (loopindex = 0; loopindex < tile->unitsOnThisTileCount; loopindex++) {
					if (!listElem) { break; } // stop as soon as a NULL element is found (should not happen if unitsOnThisTileCount is correct)
					STRUCT_UNIT_BASE *unit = listElem->unit;
					if (unit && !unit->IsCheckSumValidForAUnitClass()) { unit = NULL; }
					bool isDecorationOrCliff = unit->unitDefinition && (
						(unit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTerrain) ||
						(unit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCliff));
					bool copyUnit = includeUnits || isDecorationOrCliff; // Always copy terrain decoration, cliffs
					if (copyUnit && unit && unit->IsCheckSumValidForAUnitClass()) {
						UNIT_INSTANCE_SERIALIZED_DATA *unitData = new UNIT_INSTANCE_SERIALIZED_DATA();
						// Store relative position, not absolute !
						unitData->posX = unit->positionX - (float)minX;
						unitData->posY = unit->positionY - (float)minY;
						if (unit->unitDefinition) {
							unitData->unitDefId = unit->unitDefinition->DAT_ID1;
						}
						if (unit->ptrStructPlayer && unit->ptrStructPlayer->IsCheckSumValid()) {
							unitData->playerId = unit->ptrStructPlayer->playerId;
						}
						unitData->unitId = unit->unitInstanceId;
						// This dirty trick directly comes from AOE/ROR... 2 different kind of values are stored in 1 field, depending on type.
						if (unit->DerivesFromCommandable()) {
							unitData->orientation = ((AOE_STRUCTURES::STRUCT_UNIT_MOVABLE*)unit)->orientationAngle;
						} else {
							// Types 10/20/30/90: there is no (float) orientation (except for 30), only orientation index, which is stored as is in serialized format.
							unitData->orientation = (float)unit->orientationIndex;
						}
						unitData->status = unit->unitStatus;
						this->copiedUnits.push_back(unitData);
					}
					listElem = listElem->next;
				}
				assert(loopindex == tile->unitsOnThisTileCount);
			} else {
				std::string msg = "Error: could not access tile info (";
				msg += std::to_string(x);
				msg += ", ";
				msg += std::to_string(y);
				msg += ")";
				traceMessageHandler.WriteMessageNoNotification(msg);
				this->lastError = msg;
				return false;
			}
		}
	}
	// Validate copy only once everything is finished without errors
	this->copiedSizeX = maxX - minX + 1;
	this->copiedSizeY = maxY - minY + 1;
	this->copiedBufferContainsUnits = includeUnits;
	this->lastError = "";
	return true;
}

// Paste from internal buffer to actual map
bool MapCopier::PasteMapZone(long int startPosX, long int startPosY) {
	if ((this->copiedSizeX <= 0) || (this->copiedSizeY <= 0)) {
		this->lastError = "No valid buffer to paste.";
		return false;
	}
	this->lastError = "Technical error";
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert((global != NULL) && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	assert((global->gameMapInfo != NULL) && (global->gameMapInfo->IsCheckSumValid()));
	if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return false; }
	STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	if ((startPosX < 0) || (startPosY < 0)) {
		this->lastError = "Negative coordinates";
		return false;
	}
	long int maxX = startPosX + this->copiedSizeX - 1;
	long int maxY = startPosY + this->copiedSizeY - 1;
	if ((maxX >= mapInfo->mapArraySizeX) || (maxY >= mapInfo->mapArraySizeY)) {
		this->lastError = "Could not paste map data, map is not large enough";
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
				STRUCT_UNIT_BASE_LIST *listElem = tile->unitsOnThisTile;
				for (int i = 0; i < tile->unitsOnThisTileCount; i++) {
					if (!listElem) {
						assert(false && "reached end of unit list too early");
						break; // stop as soon as a NULL element is found (should not happen if unitsOnThisTileCount is correct)
					}
					STRUCT_UNIT_BASE *unit = listElem->unit;
					if (unit && unit->IsCheckSumValidForAUnitClass()) {
						unitsToDelete.insert(unit); // I prefer NOT to delete units inside this loop. Seems not wise.
					} else {
						traceMessageHandler.WriteMessage("Encountered invalid unit data :(");
					}
					listElem = listElem->next;
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
				traceMessageHandler.WriteMessageNoNotification(msg);
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
				// unitData->unitId is not preserved during this operation. We could using the force flag in global struct,
				// but here the unit with this id may already exist (here the map is not new nor empty)
				STRUCT_UNIT_BASE *unit = (STRUCT_UNIT_BASE *)CreateUnit(player, unitData->unitDefId,
					newPosY, newPosX, 0, unitData->status, unitData->orientation);
			}
		}
	}
	DiamondMapDrawAllTiles();
	this->lastError = "";
	return true;
}
