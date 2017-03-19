
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


	// Size = 8
	// Contains information about visible units.
	class STRUCT_VISIBLE_UNIT_INFO_SET {
	public:
		STRUCT_NEARBY_UNIT_INFO *nearbyUnitInfoArray; // +00. Represents a list of visible units
		short int arraySize; // +4
		short int numberUsed; // +6
	};
	static_assert(sizeof(STRUCT_VISIBLE_UNIT_INFO_SET) == 8, "STRUCT_VISIBLE_UNIT_INFO_SET size");


	// Referenced in global variable 0x7D2058 and also from STRUCT_GAME_MAP_INFO+8DCC
	// Size = 0x24. Constructor = 0x516800 = [7D2058].constructor(totalPlayerCount, maxAITypeIndex)
	// Supplies an up-to-date list of visible units for each player perspective (separated by unit AI type)
	// Contains arrays used to store temporary results for specific searches
	// Warning: search results are limited to 255 results and to a distance of 15 (0xF).
	class STRUCT_VISIBLE_UNITS_HELPER {
	public:
		STRUCT_VISIBLE_UNIT_INFO_SET **visibleUnitsByPlayerAndUnitAIType; // 2-dims array. Index are playerId (0-n) then unitAIType. List of visible units (including mine) by unit class.
		// +04. Index1=PLAYER_DIPLOMACY_VALUES(0-4), index2=elemIndex. Each results array=0x800 bytes=0x100 entries (sizeof(STRUCT_NEARBY_UNIT_INFO)=8)
		// Note: Those 5 pointers are the same as ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE (you better use the global variable directly)
		// Note: The "actual elem count" are in corresponding global array ADDR_ELEMCOUNT_TEMP_NEARBY_UNITS_PER_DIPLVALUE (indexed by PLAYER_DIPLOMACY_VALUES)
		// See also GetElemCountInUnitListTempSearchResult and GetUnitListTempSearchResult.
		STRUCT_NEARBY_UNIT_INFO *tempSearchResultsByDiplValue[5];
		long int totalPlayerCount; // +18. Including gaia. It is also the size (number of dwords) of visibleUnitsByPlayerAndUnitAIType array.
		long int unitAITypesCountForVisibleUnitInfoSetArray; // +1C. Number of elements in visibleUnitsByPlayerAndUnitAIType[x].
		// 0x20
		unsigned char *sqrtTable_distanceFromDiffXY; // ptr to array of 0x10*0x10 bytes. to get a distance for diffX/Y: array[diffX*0x10 + diffY]. X,Y < 0x10.
		// Returns distance(int, int) as an integer result. Returns -1 if failed or result>0x10. Works only for x,y <= 15 (0x0F)
		unsigned char GetIntDistance(unsigned char x, unsigned char y) const {
			if (x < 0) { x = -x; }
			if (y < 0) { y = -y; }
			if ((x >= 16) || (y >= 16)) {
				return -1;
			}
			return this->sqrtTable_distanceFromDiffXY[x * 0x10 + y];
		}
		// Get the "collection" of units seen by "playerId" and whose type is unitAIType
		// Returns NULL if some argument is invalid.
		STRUCT_VISIBLE_UNIT_INFO_SET *GetVisibleUnitInfoSet(long int playerId, long int unitAIType) const {
			if ((playerId < 0) || (playerId > this->totalPlayerCount) || (unitAIType < 0) || (unitAIType >= this->unitAITypesCountForVisibleUnitInfoSetArray)) {
				return NULL;
			}
			return &this->visibleUnitsByPlayerAndUnitAIType[playerId][unitAIType];
		}
	};
	static_assert(sizeof(STRUCT_VISIBLE_UNITS_HELPER) == 0x24, "STRUCT_VISIBLE_UNITS_HELPER size");



	// Included structure in addresses 0x583BC8 and 0x6A1CC0 (open paths and traversed paths ?)
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
		long int unknown_0CE624_mapUnitOccupiedGrid[0xFF * 0xFF]; // index posY*4*0xFF+x. Its value is a bits mask ?? STRUCT_MAP_VISIBILITY_MASK?
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
		STRUCT_UNIT_BASE *currentUnit; // 11DC44. The unit for which a path finding is being run
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
		char unknown_11DCA4[0x11DCC4 - 0x11DCA4];
		long int unknown_11DCC4_counter; // +11DCC4. INC in 0x45A4E0
		long int unknown_11DCC8_distance_offset; // Init=-1. Distance*4 between ? Total distance*4 (in straight line) between src and dest ?
		char unknown_11DCCC[0x11DCD8 - 0x11DCCC];
		char unknown_11DCD8_resetCounter; // +11DCD8. A counter, +=8 at each pathFinding. When 0xF0 (30 iterations), reset and resets mapData. Value is similar to +C array
		char unknown_11DCD9[3];
		long int unknown_11DCDC_unitClass; // +11DCDC. arg15 of 00458930 call. Seen 0x1B (walls????). Used in 459AD4
		// 0x11DCE0
		long int unknown_11DCE0; // arg14 of 00458930 call. A playerId ?
		STRUCT_MANAGED_ARRAY unknown_11DCE4; // +11DCE4 = array of unit IDs
		/*long int *unknown_11DCE4; // +11DCE4 = array of unit IDs
		long int unknown_11DCE8; // +11DCE8 = used count in unknown_11DCE4
		long int unknown_11DCEC;
		// 0x11DCF0
		long int unknown_11DCF0; // +11DCF0 = allocated number of elems in unknown_11DCE4*/
		long int unknown_11DCF4; // seen 1
		// ...
		bool IsCheckSumValid() { return this->checksum == 0x00544CF0; }
	};


	// Size = 0x51C. Constructor = 0x521060 (gameMapInfo, terrainRestrictionValues, layer)
	// Contains information about terrain zones, for a specific set of terrain restriction accessibility
	// The map terrain zones repartition depends on terrain restriction sets (ex: shallows can split 2 islands from lion/gazelle perspective (terrRest=1), but not for a villager (terrRest=7))
	// What identifies this object is 1) number of terrain per restriction
	// 2) the set of terrain restriction values, taken as boolean (just comparing the fact that terrain is accessible or not).
	class STRUCT_GAME_TERRAIN_ZONES_INFO {
	public:
		long int unknown_00_array[0xFF]; // Values are (almost) always zero ?
		unsigned long int unused_3FC; // +3FC. Always BAADFOOD ?
		unsigned long int unused_400; // +400. Always BAADFOOD ?
		char *landAndWaterIdentifiersArray; // +404: array of mapSizeX*mapSizeY bytes. -1=not initialized. Value=continent/island/lake/sea identifier for current tile. Each ID represents a zone of the map.
		char **landAndWaterIdentifiersCols; // +408: pointers to cols from +404 array. Array size = mapSizeX*4 (dwords). the array[x] = position in bytesarray
		char unknown_40C[0xFF]; // +40C. Default values=-1
		char unknown_50B; // unused ?
		unsigned long int unknown_50C; // +50C. Always BAADFOOD ?
		float *terrainRestrictionValues; // +510. Index = terrainId.
		long int terrainsCountInTerrainRestrictions; // +514. Determines the element count in terrainRestrictionValues.
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +518

		// Returns the terrain zone ID at given position, from this terrainZoneInfo's perspective
		char GetTerrainZoneId(long int posX, long int posY) const {
			if ((posX < 0) || (posY < 0)) { return -1; }
			if ((posX > 255) || (posY > 255)) { return -1; }
			assert(this->gameMapInfo);
			//if ((posX >= this->gameMapInfo->mapArraySizeX) || (posY >= this->gameMapInfo->mapArraySizeY)) { return -1; }
			assert(this->landAndWaterIdentifiersArray);
			assert(this->landAndWaterIdentifiersCols);
			if (!this->landAndWaterIdentifiersCols) { return -1; }
			char *pCol = this->landAndWaterIdentifiersCols[posX];
			assert(pCol != NULL);
			if (pCol == NULL) { return -1; }
			return pCol[posY];
		}

		// Returns true if target position belong to the same zone as source position (accepting a maxDistance margin)
		// TO CONFIRM
		bool IsSameZone(long int posX, long int posY, long int targetPosX, long int targetPosY, float maxDistance) const {
			unsigned long int addr = 0x521640;
			long int result = 0;
			_asm {
				MOV ECX, this;
				PUSH maxDistance;
				PUSH targetPosX;
				PUSH targetPosY;
				PUSH posX;
				PUSH posY;
				CALL addr; // returns a dword (containing 0 or 1)
				MOV EAX, result;
			}
		}
	};


	// Size = 0x0C. Constructor = 0x5219C0
	// Provides the terrain zones info for each set of "terrain restriction accessibility values" (taken as bool).
	// An entry in ptrTerrainZoneInfoArray is identified by 1) number of terrains (in terrain restriction values array)
	// and 2) set of "bool" accessibility values in terrain restriction array.
	// 0x521B50 = GetTerrainZoneInfo(pTerrainRestrictionValues, nbTerrainPerTerrainRestr, pOutFoundIndex), returns pointer to STRUCT_GAME_TERRAIN_ZONES_INFO
	class STRUCT_MAP_TERRAIN_ZONES_INFO_LINK {
	public:
		STRUCT_GAME_TERRAIN_ZONES_INFO **ptrTerrainZoneInfoArray; // +00. Element count is "+4" dword.
		long int arrayElemCount; // +04: number of items in array (allocated size = elemCount*4)
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +08

		// Securely retrieve a terrain zone info from its index (cf 0x521C60)
		STRUCT_GAME_TERRAIN_ZONES_INFO *GetTerrainZoneInfo(long int index) const {
			assert((index >= 0) && (index < this->arrayElemCount));
			if ((index < 0) || (index >= this->arrayElemCount)) { return NULL; }
			return this->ptrTerrainZoneInfoArray[index];
		}
	};

}
