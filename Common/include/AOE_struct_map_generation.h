
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
	//class STRUCT_RMM_DB_CONTROLLER;


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


#define CHECKSUM_MAPGEN_BASE 0x00545CC8 // Common parent.
#define CHECKSUM_RMM_DB_CONTROLLER 0x00545CDC // Please use TRIBE_RMM_DB_CONTROLLER. This intermediate class corresponds to RGE_Map level
#define CHECKSUM_TRIBE_RMM_DB_CONTROLLER 0x00545D18 // Child class, corresponds to Tribe_Map (the one to use)
#define CHECKSUM_RMM_CLIFF_GENERATOR 0x00545CD8
#define CHECKSUM_RMM_TERRAIN_GENERATOR 0x00545D4C
#define CHECKSUM_RMM_LAND_CONTROLLER 0x00545D04
#define CHECKSUM_RMM_OBJECT_GENERATOR 0x00545D10
#define CHECKSUM_RMM_ELEVATION_GENERATOR 0x00545D00
#define CHECKSUM_RMM_SHALLOWS_GENERATOR 0x00545D14


	// "Random_Map_Module". Size = 0x30. Constructor=0x46BD00.
	class STRUCT_MAPGEN_BASE {
	public:
		unsigned long int checksum; // C8 5C 54 00
		STRUCT_MAPGEN_BASE *parentMapGenObject; // +04
		STRUCT_MAPGEN_BASE *unknown_08; // +08. represents a list of elevation info objects ? Old name = elevationInfoFirstLinkElem
		STRUCT_GAME_MAP_INFO *gameMapInfo;
		STRUCT_GAME_MAP_TILE_INFO **pTileInfoRows; // +10. same as gameMapInfo+8D8C
		long int mapSizeY;
		long int mapSizeX;
		float schedule; // Default value = 1.5
		STRUCT_GAME_MAP_TILE_INFO **unknown_020; // pointer 1 from mapGenInfo
		STRUCT_GAME_MAP_TILE_INFO **unknown_024; // pointer 2 from mapGenInfo
		STRUCT_GAME_MAP_TILE_INFO **unknown_028; // pointer 3 from mapGenInfo
		STRUCT_GAME_MAP_TILE_INFO **unknown_02C; // pointer 4 from mapGenInfo

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_MAPGEN_BASE; }
	};
	static_assert(sizeof(STRUCT_MAPGEN_BASE) == 0x30, "STRUCT_MAPGEN_BASE size");


	// Size = 0xFB0, constructor=0x46F030
	class STRUCT_RMM_ELEVATION_GENERATOR : public STRUCT_MAPGEN_BASE {
	public:
		// unsigned long int checksum; // 00 5D 54 00
		STRUCT_MAPGEN_ELEVATION_DATA elevationData; // +30. Size = 0xF80. Copied from mapGenInfo.unknown_3FD4 ?

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_RMM_ELEVATION_GENERATOR); }
	};
	static_assert(sizeof(STRUCT_RMM_ELEVATION_GENERATOR) == 0xFB0, "STRUCT_RMM_ELEVATION_GENERATOR size");


	// Size = 8. Used to manage a list of STRUCT_MAPGEN_ELEVATION_INFO for a STRUCT_RMM_DB_CONTROLLER object
	// First created is last of the list (we always insert in first position... I believe.)
	class STRUCT_MAPGEN_ELEVATION_LINK {
	public:
		STRUCT_RMM_ELEVATION_GENERATOR *mapGenElevationInfo;
		STRUCT_MAPGEN_ELEVATION_LINK *next; // Last element as a NULL next value.
	};
	static_assert(sizeof(STRUCT_MAPGEN_ELEVATION_LINK) == 8, "STRUCT_MAPGEN_ELEVATION_LINK size");


	// size=0x508, constructor=0x46CDB0.
	// "RMM_Database_Controller" (old names Base="ControlerOfLandscape", child="TribeLandMaker" ?). RMM=RandomMapModule
	class STRUCT_RMM_CLIFF_GENERATOR : public STRUCT_MAPGEN_BASE {
	public:
		char unknown_30[0x508 - 0x30];

		bool IsCheckSumValid() const {
			return (this->checksum == 0x00545CD8);
		}
	};
	static_assert(sizeof(STRUCT_RMM_CLIFF_GENERATOR) == 0x508, "STRUCT_RMM_CLIFF_GENERATOR size");


	// size=0x12CC, constructor=0x472DF0.
	class STRUCT_RMM_TERRAIN_GENERATOR : public STRUCT_MAPGEN_BASE {
	public:
		char unknown_30[0x12CC - 0x30];

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_RMM_TERRAIN_GENERATOR);}
	};
	static_assert(sizeof(STRUCT_RMM_TERRAIN_GENERATOR) == 0x12CC, "STRUCT_RMM_TERRAIN_GENERATOR size");


	// size=0x1468, constructor=0x46FAE0.
	class STRUCT_RMM_LAND_CONTROLLER : public STRUCT_MAPGEN_BASE {
	public:
		char unknown_30[0x1468 - 0x30];

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_RMM_LAND_CONTROLLER); }
	};
	static_assert(sizeof(STRUCT_RMM_LAND_CONTROLLER) == 0x1468, "STRUCT_RMM_LAND_CONTROLLER size");


	// size=0x1920, constructor=0x4704D0.
	class STRUCT_RMM_OBJECT_GENERATOR : public STRUCT_MAPGEN_BASE {
	public:
		char unknown_30[0x1920 - 0x30];

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_RMM_OBJECT_GENERATOR); }
	};
	static_assert(sizeof(STRUCT_RMM_OBJECT_GENERATOR) == 0x1920, "STRUCT_RMM_OBJECT_GENERATOR size");


	// size=0x34C, constructor=0x4716D0.
	class STRUCT_RMM_SHALLOWS_GENERATOR : public STRUCT_MAPGEN_BASE {
	public:
		char unknown_30[0x34C - 0x30];

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_RMM_SHALLOWS_GENERATOR); }
	};
	static_assert(sizeof(STRUCT_RMM_SHALLOWS_GENERATOR) == 0x34C, "STRUCT_RMM_SHALLOWS_GENERATOR size");




	// size=0x4F5C (both classes), constructors=0x471D20+0x46DA10 for intermediate class, 0x471CF0+0x471D20 for TRIBE_RMM_DB_CONTROLLER.
	// "RMM_Database_Controller/TribeRMMDatabaseController". RMM=RandomMapModule
	// Main class for random map generation.
	// 0x471D40=RMMDBController.init(gameMapInfo, globalStruct, mapType, playerCount, fixedPositions)
	class STRUCT_RMM_DB_CONTROLLER : public STRUCT_MAPGEN_BASE {
	public:
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
		STRUCT_MAPGEN_OBJECT_PLACER objectsToPlaceOnMap[0x63]; // +270C
		long int numberOfMapGenObjectElems; // +399C.
		// 0x39A0
		char unknown_39A0[0x3FD0 - 0x39A0];
		unsigned long int unknown_3FD0;
		STRUCT_MAPGEN_ELEVATION_DATA elevationData; // +3FD4. Size=0xF80
		unsigned long int mapTypesCount; // +4F54.
		STRUCT_MAPGEN_MAP_TYPE *mapGenMapTypesInfoArray; // +4F58.

		bool IsCheckSumValid() const {
			return (this->checksum == CHECKSUM_TRIBE_RMM_DB_CONTROLLER) || //(this->checksum == CHECKSUM_MAPGEN_BASE) ||
				(this->checksum == CHECKSUM_RMM_DB_CONTROLLER);
		}
	};
	static_assert(sizeof(STRUCT_RMM_DB_CONTROLLER) == 0x4F5C, "STRUCT_RMM_DB_CONTROLLER size");


}
