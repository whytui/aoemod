
#pragma once

#include <AOE_const_internal.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {
	typedef char char_array_128[0x80];


#pragma pack(push, 1) // Prevent compiler from aligning on dwords (or other alignment)
	// Size=0xA8. Depends on game version (size is specified in communications?)
	// See 0x504FD0 = MPComm.copyHostOptionsFrom(tempOptionsStruct, size)
	class STRUCT_RGE_GAME_OPTIONS {
	public:
		float gameSpeed;
		char isScenario; // +04
		char scenarioName[0x80]; // +05. Scenario name, if relevant (or "").
		char isSinglePlayer; // +85
		char isMultiPlayer; // +86
		unsigned char mapSizeX; // +87
		unsigned char mapSizeY; // +88
		char unknown_89_maxPlayerCount; // +89. Maximum allowed player count for map ?
		char enableCheatMode; // +8A
		char pathFindingMPChoice; // +8B. To confirm
		char unknown_8C; // +8C. Cf unknown_984.
		char hasRevealMapAtStartup; // +8D
		char hasNoFogAtStartup; // +8E
		char enableColoredChat; // +8F. To confirm
		char playerCountWithoutGaia; // +90
		char unknown_91_enableDeveloperOptions; // +91. Not sure
		char playerGameVersion[9]; // +92 to +9A included. 0="", 1="1.0", 2="1.0a", etc. Index is playerId
		AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL difficultyLevel; // +9B. 1-byte.
		char teamNumber[9]; // +9C. Index is playerId
		char unknown_A5[3]; // unused ?		
	};
	static_assert(sizeof(STRUCT_RGE_GAME_OPTIONS) == 0xA8, "STRUCT_RGE_GAME_OPTIONS size");

	// This structure is the additional part to add to RGE_Game_Options.
	// 0x505170=gameSettings.setTribeSpecificGameOptions(&tribeSpecificGameOptions)
	struct STRUCT_TRIBE_SPECIFIC_GAME_OPTIONS {
		long int mapSizeChoice; // +A8 (total offset)
		long int mapTypeChoice; // +AC
		long int unknown_B0; // +B0. gameSettings+A84
		long int unknown_B4; // +B4. gameSettings+A88
		long int victoryConditionChoice; // +B8
		unsigned long int victoryConditionChoice_parameter; // +BC. or float ?
		char chosenCiv[9]; // +C0 (relative +18) to +C8
		char unknown_C9[3]; // +C9
		long int unknown_CC[9]; // +CC. Cf unknown_AA0. Limits player # ? 8 Players: [0,1,2,3,4,5,6,7] 4 Players: [0,1,2,3,0,1,2,3] 2 Players: [0,1,0,1,0,1,0,1]
		char chosenPlayerNum[9]; // +F0 (relative +48).  Corresponds to the colored number in game settings screen
		char playerNameCivDLLID_Offset[9]; // +F9
		char unknown_102; // +102. Cf unknown_AD6
		char unknown_103; // +103. Cf unknown_AD7
		char fixedPositions; // +104
		char allTechnologies; // +105
		char unknown_106[2];
		long int initialResourcesChoice; // +108
		long int initialAgeChoice; //+10C
		char unknown_110; // +110. cf unknown_AE4
		char isDeathMatch; // +111
		char populationLimit; // +112.
		char unused_113; // +113
	};

	// Size=0x114. Depends on game version (size is specified in communications?). Latest overload/version of game options communication message ?
	// 0x505070 = xxx.SetOptions(tribeOptions*)
	struct STRUCT_TRIBE_GAME_OPTIONS : STRUCT_RGE_GAME_OPTIONS {
	public:
		STRUCT_TRIBE_SPECIFIC_GAME_OPTIONS tribeSpecificOptions;
	};
	static_assert(sizeof(STRUCT_TRIBE_GAME_OPTIONS) == 0x114, "STRUCT_TRIBE_GAME_OPTIONS size");

#pragma pack(pop)


	// This structure does not have a checksum. It is referenced by 0x580DA8
	// It is probably this structure that collects all 'common' information to be multiplayer-compatible (player names, etc).
	// pointed by GameSettings+18C ?
	// Size=0x1774
	class STRUCT_MP_COMMUNICATION {
	public:
		unsigned long int unknown_0000;
		STRUCT_TRIBE_GAME_OPTIONS *MPGameOptions; // +04. See size in +171C field. Contains MP game settings info. Freed in 0x4258A0.
		unsigned long int unknown_0008;
		unsigned long int unknown_000C;
		// 0x10
		unsigned long int unknown_0010;
		unsigned long int unknown_0014;
		unsigned long int *unknown_0018_ptr; // Ptr to ? Player types ?
		long int unknown_001C; // flag ?
		// 0x20
		unsigned long int unknown_0020; // Dword
		char unknown_0024[0x9D0 - 0x24];
		unsigned long int unknown_09D0[8];
		char unknown_09F0[0x1000 - 0x9F0];
		// 0x1000
		char unknown_1000[0x10E0 - 0x1000];
		long int localControlPlayerId; // +10E0. "local" playerId for MP.
		char unknown_10E4[0x1100 - 0x10E4];
		// +1179= byte
		char unknown_1100[0x12A0 - 0x1100];
		unsigned long int unknown_12A0;
		unsigned long int unknown_12A4;
		unsigned long int unknown_12A8;
		unsigned long int unknown_12AC;
		// 0x12B0
		unsigned long int unknown_12B0[4];
		// 0x12C0
		unsigned long int unknown_12C0;
		unsigned long int unknown_12C4;
		char_array_128 *playerNames; // +12C8. Player names, each string size=0x80
		char_array_128 *unknownNames; // +12CC. xxx names, each string size=0x80
		// 0x12D0
		unsigned long int unknown_12D0;
		AOE_CONST_INTERNAL::COMM_LINK_LEVEL commLinkLevel; // +12D4. "Link Level".
		short int maximumPlayerCount; // +12D8. Is 8
		short int unknown_12DA;
		//char unknown_12DC[0x13C0 - 0x12DC];
		char unknown_12DC[0x136C - 0x12DC];
		unsigned long int unknown_136C[10]; // +136C. 1 per (COMM) playerId (0 and 9 unused). TO VERIFY
		char unknown_1394[0x13C0 - 0x1394];
		unsigned long int unknown_13C0[10]; // +13C0. 1 per (COMM) playerId (0 and 9 unused)
		char unknown_13E8[0x1484 - 0x13E8];
		unsigned long int unknown_1484;
		unsigned long int unknown_1488;
		unsigned long int unknown_148C;
		unsigned long int unknown_1490;
		unsigned long int unknown_1494;
		unsigned long int unknown_1498;
		char unknown_149C[0x14C4 - 0x149C];
		long int *unknown_14C4; // Related to players communication. +114=ptr (player queue)?
		char unknown_14C8[0x14D4 - 0x14C8];
		unsigned long int *unknown_14D4_ptr; // Ptr to struct size=0x240, constructor=0x428990.
		unsigned long int *unknown_14D8_ptr; // Ptr to struct size=0x7EC, constructor=0x429330. +0=backptr
		char unknown_14DC[0x14E0 - 0x14DC];
		long int *unknown_14E0; // Related to players communication
		char unknown_14E4[0x1528 - 0x14E4];
		unsigned long int unknown_1528[10]; // +1528. 1 per (COMM) playerId (0 and 9 unused)
		char unknown_1550;
		char unknown_1551[3];
		unsigned long int unknown_1554;
		unsigned long int unknown_1558[3]; // ?
		unsigned long int unknown_1564[10]; // +1564. 1 per (COMM) playerId (0 and 9 unused). Some DPlay info ?
		unsigned long int unknown_158C;
		// 0x1590
		char unknown_1590;
		char unknown_1591[3]; // Probably unused
		long int unknown_1594_isComputerControlled[10]; // 1 per (COMM) playerId (0 and 9 unused). 1 if AI ? Or playerId of host, or smthing else ?
		unsigned long int unknown_15BC[10]; // +15BC. 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_15E4[10]; // 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_160C[10]; // 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_1634[10]; // 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_165C[10]; // 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_1684[10]; // 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_16AC[10]; // +16AC. 1 per (COMM) playerId (0 and 9 unused)
		AOE_CONST_INTERNAL::GAME_PLAYER_TYPES playerTypes[10]; // 0x16D4.  1 per (COMM) playerId. Values: 0=gaia/other, 2=human, 4=computer
		char unknown_16FC[10];
		char unknown_1706[2]; // unused ?
		// 0x1700
		char unknown_1708[0x1718 - 0x1708];
		unsigned long int unknown_1718;
		long int MPGameOptionsStructSize; // +171C. Game options struct size (cf +04). Set in 0x425848. Is 0x114 in ROR 1.0a version.
		unsigned long int unknown_1720[10]; // +1720. 1 per (COMM) playerId (0 and 9 unused)
		unsigned long int unknown_1748; // ?
		unsigned long int unknown_174C[10]; // +174C ?. 1 per (COMM) playerId (0 and 9 unused)
	};
	static_assert(sizeof(STRUCT_MP_COMMUNICATION) == 0x1774, "STRUCT_UNKNOWN_P580DA8 Size");

}
