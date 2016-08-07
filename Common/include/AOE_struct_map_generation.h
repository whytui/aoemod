
#pragma once

#include <AOE_struct_map_def.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// Extern dependencies
	class STRUCT_GAME_MAP_INFO;
	class STRUCT_GAME_MAP_TILE_INFO;

	// Pre-declaration (circular dependencies in objects)
	class STRUCT_MAP_GENERATION_INFO;


	// Size = 0x34
	class STRUCT_MAPGEN_PLAYER_BASE_INFO {
	public:
		unsigned long int unknown_00;
		unsigned long int unknown_04;
		long int posY;
		long int posX;
		// 0x10
		unsigned long int unknown_10; // Could be AGE3 unknown4 in maps/base zone data
		long int nonBaseTerrainSizeBetweenPlayers; // REALLY unsure
		char unknown_18;
		char unused_19[3];
		unsigned long unknown_1C;
		// 0x20
		unsigned long unknown_20;
		unsigned long unknown_24;
		unsigned long mapSizeY;
		unsigned long mapSizeX;
		// 0x30
		char unknown_30;
		char unused_31[3];
	};
	static_assert(sizeof(STRUCT_MAPGEN_PLAYER_BASE_INFO) == 0x34, "STRUCT_MAPGEN_PLAYER_BASE_INFO size");


	// Size = 0xF80. Included in other bigger structs
	class STRUCT_MAPGEN_ELEVATION_DATA {
	public:
		char unknown_000[0x63 * 0x18]; // included array,elemSize=0x18, 0x63 elements
		long int unknown_004_usedElemCount; // +0x948
		unsigned long int unknown_94C; // +94C
		char unknown_950[0xF80 - 0x950]; // +950. Included array, elemSize=0x10, 0x63 elements
	};
	static_assert(sizeof(STRUCT_MAPGEN_ELEVATION_DATA) == 0xF80, "STRUCT_MAPGEN_ELEVATION_DATA size");


	// Size = 0xFB0, constructor=0x46F030
	// TO DO: there are also checksums 00545D04, 00545D10, 00545D14, 00545D4C, 00545CD8 that have the same parent ?
	class STRUCT_MAPGEN_ELEVATION_INFO {
	public:
		unsigned long int checksum; // 00 5D 54 00, parent=C8 5C 54 00 (same parent as STRUCT_MAP_GENERATION_INFO ?)
		STRUCT_MAP_GENERATION_INFO *mapGenInfo; // a "parent" mapgen object ?
		unsigned long int unknown_008;
		STRUCT_GAME_MAP_INFO *gameMapInfo;
		// 0x10
		STRUCT_GAME_MAP_TILE_INFO **pTileInfoRows; // same as gameMapInfo+8D8C
		long int mapSizeY;
		long int mapSizeX;
		float unknown_01C; // Default value = 1.5
		// 0x20
		STRUCT_GAME_MAP_TILE_INFO **unknown_020; // pointer 1 from mapGenInfo
		STRUCT_GAME_MAP_TILE_INFO **unknown_024; // pointer 2 from mapGenInfo
		STRUCT_GAME_MAP_TILE_INFO **unknown_028; // pointer 3 from mapGenInfo
		STRUCT_GAME_MAP_TILE_INFO **unknown_02C; // pointer 4 from mapGenInfo
		// 0x30
		STRUCT_MAPGEN_ELEVATION_DATA elevationData; // Size = 0xF80. Copied from mapGenInfo.unknown_3FD4 ?

		bool IsCheckSumValid() {
			return (this->checksum == 0x00545CC8) || (this->checksum == 0x00545D00);
		}
	};
	static_assert(sizeof(STRUCT_MAPGEN_ELEVATION_INFO) == 0xFB0, "STRUCT_MAPGEN_ELEVATION_INFO size");


	// Size = 8. Used to manage a list of STRUCT_MAPGEN_ELEVATION_INFO for a STRUCT_MAP_GENERATION_INFO object
	// First created is last of the list (we always insert in first position... I believe.)
	class STRUCT_MAPGEN_ELEVATION_LINK {
	public:
		STRUCT_MAPGEN_ELEVATION_INFO *mapGenElevationInfo;
		STRUCT_MAPGEN_ELEVATION_LINK *next; // Last element as a NULL next value.
	};
	static_assert(sizeof(STRUCT_MAPGEN_ELEVATION_LINK) == 8, "STRUCT_MAPGEN_ELEVATION_LINK size");


	// size=0x4F5C, constructor=471D20.
	class STRUCT_MAP_GENERATION_INFO {
	public:
		unsigned long int checksum; // 18 5D 54 00, parent objects are C8 5C 54 00, DC 5C 54 00
		unsigned long int unknown_0004; // a "parent" mapgen object ?
		STRUCT_MAPGEN_ELEVATION_LINK *elevationInfoFirstLinkElem; // represents a list of elevation info objects ?
		STRUCT_GAME_MAP_INFO *gameMapInfo;
		// 0x10
		unsigned long int unknown_0010; // Same as STRUCT_GAME_MAP_INFO+8D8C
		long int mapArraySizeY;
		long int mapArraySizeX;
		unsigned long int unknown_001C;
		// 0x20
		unsigned long int unknown_0020; // pointer 1
		unsigned long int unknown_0024; // pointer 2
		unsigned long int unknown_0028; // pointer 3
		unsigned long int unknown_002C; // pointer 4
		// 0x30
		unsigned long int unknown_0030;
		long int mapType;
		long int playerCount; // NOT counting gaia, only real players.
		unsigned long int unknown_003C;
		// 0x40
		unsigned long int unknown_0040;
		STRUCT_MAPGEN_PLAYER_BASE_INFO playersBaseInfo[8]; // elemSize=0x34. (index=playerId-1?). MAYBE it starts at +3C, not 44
		char unknown_0044[0x1458 - 0x1E4];
		long int totalPlayerBaseZonesCount; // +1458. Corresponds to number of players too.
		long int nonBaseDefaultTerrainId; // +145C
		// 0x1460
		long int cornersBorderPositions[4]; // SO-NO-NE-SE. To precise exact role.
		// 0x1470
		char borderUsagePercentage;
		char unknown_1471[3];
		char unknown_1474[0x270C - 0x1474];
		STRUCT_MAPGEN_OBJECT_TO_PLACE objectsToPlaceOnMap[0x63]; // +270C
		long int numberOfMapGenObjectElems; // +399C.
		// 0x39A0
		char unknown_39A0[0x3FD4 - 0x39A0];
		STRUCT_MAPGEN_ELEVATION_DATA elevationData; // +3FD4. Size=0xF80
		unsigned long int mapTypesCount; // +4F54.
		STRUCT_MAPGEN_MAP_TYPE *mapGenMapTypesInfoArray; // +4F58.

		bool IsCheckSumValid() {
			return (this->checksum == 0x00545D18) || (this->checksum == 0x00545CC8) || (this->checksum == 0x00545CDC);
		}
	};
	static_assert(sizeof(STRUCT_MAP_GENERATION_INFO) == 0x4F5C, "STRUCT_MAP_GENERATION_INFO size");


}
