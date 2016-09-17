
#pragma once

#include <AOE_struct_map_base_common.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// External dependencies
	class STRUCT_UNIT_BASE;
	class STRUCT_GAME_GLOBAL;
	class STRUCT_GAME_MAP_INFO;


	/*
	Objects from this file are not very well known...
	*/


	// Size = 8
	// Contains information about nearby units for temp treatments.
	// Each player has its own object
	class STRUCT_UNKNOWN_MAP_INFO_7D2058_PLAYER_NEARBY_UNITS_INFO {
	public:
		STRUCT_NEARBY_UNIT_INFO *nearbyUnitInfoArray;
		short int arraySize; // +4
		short int numberUsed; // +6
	};

	// Referenced in global variable 0x7D2058 and also from STRUCT_GAME_MAP_INFO+8DCC
	// Size = 0x24. Constructor = 0x516800
	class STRUCT_UNKNOWN_MAP_INFO_7D2058 {
	public:
		STRUCT_UNKNOWN_MAP_INFO_7D2058_PLAYER_NEARBY_UNITS_INFO **unknown_00; // Array[playerId] => pointer to array of STRUCT_UNKNOWN_MAP_INFO_7D2058_UNITINFOELEM
		char *unknown_04; // Array size = 0x800 bytes.
		char *unknown_08; // Array size = 0x800 bytes.
		char *unknown_0C; // Array size = 0x800 bytes.
		// 0x10
		char *unknown_10; // Array size = 0x800 bytes.
		char *unknown_14; // Array size = 0x800 bytes.
		long int totalPlayerCount; // Including gaia. It is also the size (number of dwords) of unknown_00 array.
		long int unknown_1C_size; // unknown_1C_size * 8 = sizeof(unknown_00[x] array)
		// 0x20
		char *sqrtTable_distanceFromDiffXY; // ptr to array of 0x10*0x10 bytes. to get a distance for diffX/Y: array[diffX*0x10 + diffY]. X,Y < 0x10.
		// Returns distance(int, int) as an integer result. Returns -1 if failed or result>0x10. Works only for x,y <= 15 (0x0F)
		char GetIntDistance(char x, char y) {
			if ((x < 0) || (x >= 16) || (y < 0) || (y >= 16)) {
				return -1;
			}
			return this->sqrtTable_distanceFromDiffXY[x * 0x10 + y];
		}
	};


	// Included structure in addresses 0x583BC8 and 0x6A1CC0
	// Constuctor 00458570
	// This structure is for path finding.
	// Note: it seems map is split into tiles (1 unit of position = 1 tile) but tiles are split into 4 "quarters".
	// In this struct, arrays indices are "quarter tiles". (use posX*4 for example).
	// Arrays value are binary (1 bit of one elementary position), which adds more precision under tile/quarter tile level ?
	class STRUCT_UNKNOWN_MAP_DATA_F04C {
	public:
		unsigned long int checksum; // F0 4C 54 00
		long int mapArraySizeY;
		long int mapArraySizeX;
		char unknown_00000C[0xFF * 0xFF]; // Access to data: 0xFF*y + x ?
		long int unknown_00FE10_dist16[0xFF * 0xFF]; // +FE10. Array of distance*16 values (?). index=y*0xFF+x. Each tile's distance to src??
		//char unknown_00FE10[0x4F614 - 0xFE10]; // size ?
		STRUCT_PATH_FINDING_UNKNOWN_POS_INFO unknown_4F614[0xFF * 0xFF]; // +4F614. Size 7F008. NOT indexed by coordinates ? Sort of list? Steps for short path finding??
		unsigned long int unknown_0CE61C;
		// 0x4F620
		unsigned long int unknown_0CE620;
		// 458870: set "has unit" at position?
		long int unknown_0CE624_mapUnitOccupiedGrid[0xFF * 0xFF]; // index posY*4*0xFF+x. Its value is a bits mask ??
		// 0x10DE28 ?
		char unknown_010DE28[0x11DC2C - 0x10DE28];
		long int unknown_011DC2C_index; // an index to access +4F614 + index*8 (byte)?
		char unknown_011DC30_posY; // current step posY ?
		char unknown_011DC31_posX; // current step posX ?
		char unknown_011DC32; // ?
		char unknown_011DC33; // ?
		long int unknown_11DC34_distanceOffset; // a distance*16 value. Current step dist*16 ? or best dist*16 ?
		long int unknown_11DC38_counter; // a counter
		unsigned long int uknown_11DC3C_ptr; // ptr to F4 99 54 00
		// 0x11DC40
		STRUCT_GAME_GLOBAL *globalStruct; // +11DC40: in old debugging times, was used as a global var to "game global", was not exactly the case.
		STRUCT_UNIT_BASE *currentUnit; // 11DC44.
		unsigned long int unknown_11DC48;
		long int destinationUnitId; // +11DC4C.
		// 0x11DC50
		float *pCurrentTerrainRestriction;
		float unknown_11DC54; // sizeRadiusY ?
		float unknown_11DC58; // sizeRadiusX ?
		long int unknown_11DC5C; // a terrainID ??? or -1 ?
		// 0x11DC60
		long int unknown_11DC60; // a terrainID ??? or -1 ?
		char unknown_11DC64[0x11DC7C - 0x11DC64];
		long int srcPosY; // +11DC7C
		// 0x11DC80
		long int srcPosX; // +11DC80
		long int destMinPosY_offset; // +11DC84. (destPosY - sizeRadius)*4 ?
		long int destMinPosX_offset; // +11DC88. (destPosX - sizeRadius2)*4 ?
		long int destMaxPosY_offset; // +11DC8C. (destPosY + sizeRadius)*4 ? then modified...
		long int destMaxPosX_offset; // +11DC90. (destPosX + sizeRadius)*4 then modified...
		long int actorMinPosY_offset; // +11DC94. (actorPosY - sizeRadius)*4 ?
		long int actorMinPosX_offset; // +11DC98. (actorPosX - sizeRadius)*4 ?
		long int actorMaxPosY_offset; // +11DC9C. (actorPosY + sizeRadius)*4 ?
		long int actorMaxPosX_offset; // +11DCA0. (actorPosX + sizeRadius)*4 ?
		char unknown_11DCA4[0x11DCC8 - 0x11DCA4];
		long int unknown_11DCC8_distance_offset; // Init=-1. Distance*4 between ? Total distance*4 (in straight line) between src and dest ?
		char unknown_11DCCC[0x11DCD8 - 0x11DCCC];
		char unknown_11DCD8_resetCounter; // A counter, +=8 at each pathFinding. When 0xF0 (30 iterations), reset and resets mapData. Value is similar to +C array
		char unknown_11DCD9[3];
		long int unknown_11DCDC; // arg15 of 00458930 call
		// 0x11DCE0
		long int unknown_11DCE0; // arg14 of 00458930 call
		long int unknown_11DCE4;
		long int unknown_11DCE8;
		long int unknown_11DCEC;
		// 0x11DCF0
		long int unknown_11DCF0;
		long int unknown_11DCF4; // seen 1
		// ...
		bool IsCheckSumValid() { return this->checksum == 0x00544CF0; }
	};


	// Size = 0x51C. Constructor = 0x521060 (gameMapInfo, terrainRestrictionValues, layer)
	class STRUCT_GAME_TERRAIN_RESTRICTION_INFO {
	public:
		long int unknown_00_array[0xFF]; // Values are (almost) always zero ?
		unsigned long int unknown_3FC; // unused ?
		unsigned long int unknown_400; // unused ?
		char *terrainAccessibilityValues; // +404: array of mapSizeX*mapSizeY bytes. Default values = -1. Value = 0 when terrain accessibility (restriction) is same as next (y+1) ?
		char **terrainAccessibilityValuesCols; // +408: pointers to cols from +404 array. Array size = mapSizeX*4 (dwords). the array[x] = position in bytesarray
		char unknown_40C[0xFF]; // +40C. Default values=-1
		char unknown_50B; // unused ?
		unsigned long int unknown_50C; // unused ?
		float *terrainRestrictionValues; // +510. Index = terrainId. count = +514 ("layer") ?
		long int terrainsCountInTerrainRestrictions; // +514. Determines the element count in terrainRestrictionValues.
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +518

		char GetTerrainAccessibilityValue(long int posX, long int posY) const {
			if ((posX < 0) || (posY < 0)) { return -1; }
			if ((posX > 255) || (posY > 255)) { return -1; }
			assert(this->gameMapInfo);
			//if ((posX >= this->gameMapInfo->mapArraySizeX) || (posY >= this->gameMapInfo->mapArraySizeY)) { return -1; }
			assert(this->terrainAccessibilityValues);
			assert(this->terrainAccessibilityValuesCols);
			if (!this->terrainAccessibilityValuesCols) { return -1; }
			char *pCol = this->terrainAccessibilityValuesCols[posX];
			assert(pCol != NULL);
			if (pCol == NULL) { return -1; }
			return pCol[posY];
		}
	};


	// Size = 0x0C. Constructor = 0x5219C0
	// 0x521C60 = getItem(i)
	class STRUCT_MAP_TERRAIN_RESTRICTION_INFO_LINK {
	public:
		STRUCT_GAME_TERRAIN_RESTRICTION_INFO **ptrArray; // +00. Element count is "+4" dword.
		long int arrayElemCount; // +04: number of items in array (allocated size = elemCount*4)
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +08
	};

}
