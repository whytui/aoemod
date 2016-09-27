
#pragma once

#include <AOE_struct_terrain_def.h>
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


	// Size = 0xB5F8 to confirm. Init in 0x4445E0.
	class STRUCT_GAME_MAP_INFO {
	public:
		unsigned long int checksum; // F4 99 54 00 (parent class 0A 45 54 00)
		STRUCT_GAME_MAP_TILE_INFO *pTilesArray; // +04. Array base of all map tiles
		long int mapArraySizeY;
		long int mapArraySizeX;
		// 0x10
		unsigned long int unknown_0010; // = mapSizeY * [+8D96]
		unsigned long int unknown_0014; // = mapSizeX * [+8D98]
		char unknown_0018_array[0x6 * 0x13];// 0x18: array of elems size=0x6, 3 words. 0x13 elems ? Init in 444240
		short int unknown_008A;
		STRUCT_TERRAIN_DEF terrainDefinitions[0x20]; // Count is unsure
		char unknown_338C[0x8D8C - 0x338C];
		// 8808-1C... 0x10 elems and size=5A0 => start at 2E08 ? WTF. about shp
		STRUCT_GAME_MAP_TILE_INFO **pTileInfoCols; // 0x8D8C. Please use GetTileInfo(...)
		// 0x8D90
		short int terrainCount; // +8D90. Default = 0x17 = 23 different terrains in empires.dat
		short int unknown_8D92;
		unsigned short int unknown_8D94;
		unsigned short int unknown_8D96; // init 0x40 . for Y ?
		unsigned short int unknown_8D98; // init 0x20 . for X ?
		unsigned short int unknown_8D9A; // init 0x10. (posX-posY)*unknown_8D9A - (altitude*[unknown_8D9E or unknown_8D9E-1]) = tile.displayY
		unsigned short int unknown_8D9C; // init 0x20. (posX+posY)*unknown_8D9C = tile.displayX
		unsigned short int unknown_8D9E; // init 0x10. Use "unknown_8D9E+1" for tile.elevationGraphicIndex in (2,6,8,A,E,F,10), see 0x444B7B.
		char unknown_8DA0[0x8DB0 - 0x8DA0];
		char *unknown_8DB0; // ptr, map data also. Size= sizeX*sizeY*1 (x*y bytes)
		unsigned long int unknown_8DB4; // ptr, map data also ? Size=(sizeX+1)*4 bytes (ptrs ?) = rows for +8DB0
		char revealMap; // 0x8DB8
		char hasFog; // 0x8DB9
		char unknown_8DBA;
		char unknown_8DBB;
		STRUCT_MAP_GENERATION_INFO *unknown_8DBC_mapgenInfos;
		// 0x8DC0
		STRUCT_GAME_GLOBAL *globalStruct;
		STRUCT_MAP_TERRAIN_ZONES_INFO_LINK *terrainZonesInfoLink; // +8DC4.
		unsigned long int unknown_8DC8; // +8DC8. ptr, size=0x0C. +0/+4=mapSizeYX.+8=ptr_array(sizeX*sizeY*4). Constructor 517E30. Related to 7D205C
		STRUCT_UNKNOWN_MAP_INFO_7D2058 *unknown_8DCC;
		char unknown_8DD0[0x8DE4 - 0x8DD0];
		unsigned long int unknown_8DE4; // init -1
		unsigned long int unknown_8DE8; // init -1
		unsigned long int unknown_8DEC; // init -1
		// 0x8DF0
		unsigned long int unknown_8DF0; // init -1
		unsigned long int unknown_8DF4; //?
		// 0x8DF8
		char unknown_8DF8_array[0x100 * 0x28]; // Array: 0x100 elements, size=0x28. All are dwords.

		bool IsCheckSumValid() const { return this->checksum == 0x005499F4; }
		bool IsPositionInBounds(long int x, long int y) const {
			return (x >= 0) && (y >= 0) && (x < this->mapArraySizeX) && (y < this->mapArraySizeY);
		}
		STRUCT_GAME_MAP_TILE_INFO *GetTileInfo(short int x, short int y) const {
			if ((x < 0) || (x >= this->mapArraySizeX) || (y < 0) || (y >= this->mapArraySizeY)) { return NULL; }
			return &this->pTileInfoCols[x][y];
		}
		// Updates a tile's displayX/Y fields according to its elevation and position.
		// Necessary to fix tile "height" display when altitude/elevationGraphicIndex changed.
		bool RecomputeTileDisplayPos(short int x, short int y) {
			if ((x < 0) || (x >= this->mapArraySizeX) || (y < 0) || (y >= this->mapArraySizeY)) { return false; }
			STRUCT_GAME_MAP_TILE_INFO *tile = this->GetTileInfo(x, y);
			if (!tile) { return false; }
			tile->displayX = (x + y) * this->unknown_8D9C;
			tile->displayY = (x - y) * this->unknown_8D9A;
			tile->displayY -= tile->terrainData.GetAltitude() * this->unknown_8D9E;
			switch (tile->elevationGraphicsIndex) {
			case 2:
			case 6:
			case 8:
			case 0xA:
			case 0xE:
			case 0xF:
			case 0x10:
				tile->displayY -= this->unknown_8D9E;
				break;
			default:
				break;
			}
			return true;
		}
	};
	static_assert(sizeof(STRUCT_GAME_MAP_INFO) == 0xB5F8, "STRUCT_GAME_MAP_INFO size");


}
