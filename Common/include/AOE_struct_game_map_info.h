
#pragma once

#include <AOE_struct_terrain_def.h>
#include <AOE_struct_border_def.h>
#include <AOE_struct_map_tile_info.h>
#include <AOE_struct_map_generation.h>
#include <AOE_struct_map_global_objects.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// External dependencies
	class STRUCT_GAME_GLOBAL;

#pragma pack(push, 1) // Prevent compiler from aligning on dwords (or other alignment). Very important here as there are some weird structures.
	
	// Size=0x06. For tile position/size display
	class STRUCT_ELEVATION_TILE_SIZE_INFO {
	public:
		short int tile_sizeY; // +00. "width"
		short int tile_sizeX; // +02. "height"
		short int yDelta; // +04. A signed value.
	};
	static_assert(sizeof(STRUCT_ELEVATION_TILE_SIZE_INFO) == 0x06, "STRUCT_ELEVATION_TILE_SIZE_INFO size");


#define CHECKSUM_RGE_MAP 0x005445A0 // Base class
#define CHECKSUM_GAME_MAP 0x005499F4
	// Parent size=0x88D0. Constructor=0x443CE0(fromFile), 0x443D80
	// Size = 0xB5F8. Init in 0x4445E0. Constructor=0x4ED4B0(fromFile), 0x4ED800.
	// "RGE_Map" / "Tribe_Map"
	// 0x447DE0 = gameMapInfo.getTerrain(col=y, row=x)
	// +00 = destructor(do_free)
	// +04 = gameMapInfo.init(internalFileId?)?
	// +14 = gameMapInfo.setRawAltitudeValueNoCalc(posYstart, posXstart, size?; altitude)
	// +18 = gameMapInfo.setAltitude(fromPosY, fromPosX, toPosY, toPosX, pencilSize, altitude)
	// +28 = gameMapInfo.generateMap(globalStruct, mapSize, mapSize, mapType, playerCount)
	// +2C = RGE_map.save(internalFileId) [Last for base class]
	// +30 = RGE_map.xxx(...) [Last]
	class STRUCT_GAME_MAP_INFO {
	public:
		unsigned long int checksum; // F4 99 54 00 (base class 0A 45 54 00)
		STRUCT_GAME_MAP_TILE_INFO *pTilesArray; // +04. Array base of all map tiles
		long int mapArraySizeY; // +08.
		long int mapArraySizeX; // +0C.
		// 0x10
		unsigned long int unknown_0010; // = mapSizeY * [+8D96]
		unsigned long int unknown_0014; // = mapSizeX * [+8D98]
		STRUCT_ELEVATION_TILE_SIZE_INFO tileSizeInfoForElevations[ELEVATION_GRAPHICS_TYPE::EGT_COUNT];// +18. Array (19) of tile size info for various elevation graphics. Init (hardcoded) in 0x444240.
		short int unknown_008A;
		STRUCT_TERRAIN_DEF terrainDefinitions[0x20]; // +8C. Count is unsure
		STRUCT_BORDER_DEF borderDefinitions[0x10]; // +338C. Array of 0x10 elements.
		STRUCT_GAME_MAP_TILE_INFO **pTileInfoCols; // 0x8D8C. Please use GetTileInfo(...)
		short int terrainCount; // +8D90. Default = 0x17 = 23 different terrains in empires.dat
		short int unknown_8D92; // +8D92. Borders count ?
		unsigned short int unknown_8D94; // Max terrains ????
		unsigned short int tileSizeY; // +8D96. init 0x40. for Y tile size ?
		unsigned short int tileSizeX; // +8D98. init 0x20. for X tile size ?
		unsigned short int tileHalfSizeY_unsure; // +8D9A. init 0x10. (posX-posY)*unknown_8D9A - (altitude*[unknown_8D9E or unknown_8D9E-1]) = tile.displayY
		unsigned short int tileHalfSizeX_unsure; // +8D9C. init 0x20. (posX+posY)*unknown_8D9C = tile.displayX
		unsigned short int tileSizeZ; // +8D9E. init 0x10. Use "unknown_8D9E+1" for tile.elevationGraphicIndex in (2,6,8,A,E,F,10), see 0x444B7B.
		short int unknown_8DA0_curPosY; // +8DA0
		short int unknown_8DA2_curPosX; // +8DA2
		short int unknown_8DA4; // block0 Y ?
		short int unknown_8DA6; // block1 Y ?
		short int unknown_8DA8; // block0 X ?
		short int unknown_8DAA; // block1 X ?
		short int unknown_8DAC; // searchMapY ??
		short int unknown_8DAE; // searchMapX ??
		char *unknown_8DB0; // ptr, map data also. Size= sizeX*sizeY*1 (x*y bytes)
		unsigned long int unknown_8DB4; // ptr, map data also ? Size=(sizeX+1)*4 bytes (ptrs ?) = rows for +8DB0
		char revealMap; // 0x8DB8
		char hasFog; // 0x8DB9
		char unknown_8DBA;
		char unknown_8DBB;
		STRUCT_MAP_GENERATION_INFO *mapgenInfos; // +8DBC.
		STRUCT_GAME_GLOBAL *globalStruct; // +8DC0
		STRUCT_MAP_TERRAIN_ZONES_INFO_LINK *terrainZonesInfoLink; // +8DC4.
		STRUCT_MAP_VISIBILITY_LINK *mapVisibilityLink; // +8DC8. Link to an array with visibility masks tile by tile. Data array is the same as 0x7D205C
		STRUCT_VISIBLE_UNITS_HELPER *visibleUnitsHelper; // +8DCC. Used to search for visible units from a specific player perspective
		// Parent class ends here
		char unknown_8DD0[0x8DE4 - 0x8DD0];
		unsigned long int unknown_8DE4; // init -1
		unsigned long int unknown_8DE8; // init -1
		unsigned long int unknown_8DEC; // init -1
		// 0x8DF0
		unsigned long int unknown_8DF0; // init -1
		unsigned long int unknown_8DF4; //?
		// 0x8DF8
		char unknown_8DF8_array[0x100 * 0x28]; // Array: 0x100 elements, size=0x28. All are dwords.

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_GAME_MAP; }
		bool IsPositionInBounds(long int x, long int y) const {
			return (x >= 0) && (y >= 0) && (x < this->mapArraySizeX) && (y < this->mapArraySizeY);
		}
		void FixPositionToGetInMapBounds(long int *posX, long int *posY) const {
			if (*posX < 0) { *posX = 0; }
			if (*posY < 0) { *posY = 0; }
			if (*posX >= this->mapArraySizeX) { *posX = this->mapArraySizeX - 1; }
			if (*posY >= this->mapArraySizeY) { *posY = this->mapArraySizeY - 1; }
		}
		// Safely gets Tile info for coordinates. Returns NULL if invalid coordinates.
		STRUCT_GAME_MAP_TILE_INFO *GetTileInfo(long int x, long int y) const {
			if ((x < 0) || (x >= this->mapArraySizeX) || (y < 0) || (y >= this->mapArraySizeY)) { return NULL; }
			return &this->pTileInfoCols[x][y];
		}
		// Updates a tile's displayX/Y fields according to its elevation and position.
		// Necessary to fix tile "height" display when altitude/elevationGraphicIndex changed.
		bool RecomputeTileDisplayPos(short int x, short int y) {
			if ((x < 0) || (x >= this->mapArraySizeX) || (y < 0) || (y >= this->mapArraySizeY)) { return false; }
			STRUCT_GAME_MAP_TILE_INFO *tile = this->GetTileInfo(x, y);
			if (!tile) { return false; }
			tile->displayX = (x + y) * this->tileHalfSizeX_unsure;
			tile->displayY = (x - y) * this->tileHalfSizeY_unsure;
			tile->displayY -= tile->terrainData.GetAltitude() * this->tileSizeZ;
			switch (tile->elevationGraphicsIndex) {
			case 2:
			case 6:
			case 8:
			case 0xA:
			case 0xE:
			case 0xF:
			case 0x10:
				tile->displayY -= this->tileSizeZ;
				break;
			default:
				break;
			}
			return true;
		}
	};
	static_assert(sizeof(STRUCT_GAME_MAP_INFO) == 0xB5F8, "STRUCT_GAME_MAP_INFO size");


#pragma pack(pop)
}
