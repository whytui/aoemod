
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
	// External dependencies
	class STRUCT_GAME_GLOBAL;



	// Size = 0x14
	// Constructor : 4EBBF0 (readFromFile)
	class STRUCT_UNKNOWN_GAME_TIME {
	public:
		unsigned long int checksum; // 2C 99 54 00
		STRUCT_GAME_GLOBAL *global;
		unsigned long int unknown_08;
		short int unknown_0C; // Count of... ? (+08 ptr ?)
		short int unknown_0E;
		// 0x10
		float gameTime;

		bool IsCheckSumValid() { return (this->checksum == 0x0054992C); }
	};


	// Size = 0x514C. Constructor=0x0506900
	// Warning: all arrays[0x10] are indexed by playerId-1, which means tribeNames[0] is player1's name.
	class STRUCT_SCENARIO_INFO {
	public:
		unsigned long int checksum; // C8 A3 54 00 ; parent struct = 3C 5E 54 00
		STRUCT_UNKNOWN_GAME_TIME *timeInfo; // ptr to 2C 99 54 00
		STRUCT_GAME_GLOBAL *globalStruct;
		char unknown_000C_generalVictory_conquest; // Init = 1 (default)
		char tribeNames[0x10][0x100]; // +D. Index=(playerId-1)
		char unknown_100E; // unused?
		char unknown_100F; // unused?
		// 0x10
		char *scenarioFileName; // +1010. Actually reset to "" when loading a non-scenario saved game. Loading a scenario saved game correctly sets this name, very useful !
		char *scenarioInstructions; // +1014
		char *scenarioTips;
		char *scenarioVictoryText;
		// 0x1020
		char *scenarioDefeatedText;
		char *scenarioHistoryText;
		//char unknown_1028[0x12C8 - 0x1028]; // 475C55
		char fullPath_tmpBuildList[0xE0];
		char unknown_1108[0x12C8 - 0x1108];
		char *strategyFileNames[0x10]; // +12C8. Index=(playerId-1). Size = 0xE0.
		char *cityPlanFileNames[0x10]; // +1308. Index=(playerId-1). Size = 0xE0.
		char *personalityFileNames[0x10]; // +1348. Index=(playerId-1). Size = 0xE0.
		char *strategyFileContent[0x10]; // +1388. pointer to AI file content. See in 408948 (read from a temp file, not from THIS). Is size limited to ~32ko ? Not sure
		char *cityPlanFileContent[0x10]; // +13C8. Pointer to an array of bytes.
		char *personalityFileContent[0x10]; // +1408. Pointer to an array of bytes. Note: The "END" keyword actually stops reading the file.
		long int strategyFileSize[0x10]; // +1448.
		long int cityPlanFileSize[0x10]; // +1488.
		long int personalityFileSize[0x10]; // +14C8.
		char scenarioBeginningAnimationFileName[0xE0]; // +1508
		char scenarioVictoryAnimationFileName[0xE0]; // +15E8
		char scenarioFailureAnimationFileName[0xE0]; // +16C8
		char scenarioBitmapFileName[0xE0]; // +17A8
		float scenarioVersion; // +1888. Corresponds to .scn/.scx version, like 1.02, 1.03, 1.04, 1.05, 1.13, 1.15(=ROR 1.0a). Not always reset (when loading a non-scenario game)
		long int player_unknown_188C[0x10]; // +188C. Often referred to as +18CC-40. Default = 4 ?
		long int playerIsHumanPlayable[0x10]; // +18CC. 0 if computer player only (cannot be played as human)
		long int playerCivilizationId[0x10]; // +190C. Often referred to as +18CC+40.
		long int playerIsActive[0x10]; // +194C. 1 if player is active. Often referred to as +18CC+80.
		long int *unknown_198C; // +198C. ptr to struct size=0x20, constructor 45AF60. TO DO.
		long int startingResources[0x10][4]; // +1990. WARNING: resource order = gold/wood/food/stone here !!!
		// 0x1A90 - probably a sub-structure of size=0x3160 (last 4 bytes are unused) for victory conditions settings.
		// Those generalVictory_* are only relevant if generalVictory_mainCondition==4 (custom)
		long int generalVictory_conquest; // 1 = Standard, Conquest, Score, Time Limit, Custom if Conquest is enabled. ; 0=others
		long int generalVictory_ruinsCount; // Number of ruins to collect to win the game (for all players)
		long int generalVictory_relicsCount; // Number of relics to collect to win the game (for all players)
		long int generalVictory_discoveriesCount; // Number of discoveries to find to win the game (for all players)
		// 0x1AA0
		long int generalVictory_explorationAmount; // % of exploration to reach to win the game (for all players)
		long int generalVictory_goldAmount; // Amount of gold to collect to win the game (for all players). Hidden in scenario editor (value is never modified by editor).
		char unknown_1AA8_individual_victory_conditions[0x10][0x2D0]; // Internal structure to define
		long int diplomacySettings[0x10][0x10]; // +47A8. Refers to PLAYER_DIPLOMACY_STANCES but as DWORDs. 0=allied,1=neutral,3=enemy.
		long int hasAlliedVictory[0x10]; // +4BA8. 1=option is checked for that player
		long int artefactsRequireAll; // +4BE8. 0="at least one", 1="all". Applies to relics, ruins, discoveries ?
		unsigned long int unknown_4BEC; // Unused, it seems.
		// 0x4BF0
		unsigned long int unknown_4BF0;
		long int disableTechnologyFlags[0x10][AOE_CONST_INTERNAL::SC_ED_DISABLE_TECHNOLOGY_INDICES::CST_DTI_COUNT]; // +4BF4. Total Size =0x500 bytes. Used in 0x507E50=scenarioInfo.applyDisabledResearches(player)
		long int enableLengthenCombatMode; // +50F4. A flag 0/1. If 1, lengthen combat mode is ON (most units get *3 HP, it is technology 0x64)
		long int unknown_50F8; // +50F8 ? Unused ?
		long int fullTechTree; // +50FC
		// 0x5100
		long int playersStartingAge[0x10]; // 1=Tool, 4=post iron. Warning, this does not correspond to CST_AGE_TOOL, etc. Index is playerId-1.
		// 0x5140
		AOE_CONST_INTERNAL::GENERAL_VICTORY_CONDITION generalVictory_mainCondition; // 0=Standard, 1=Conquest, 2=Score, 3=Time Limit, 4=Custom
		long int generalVictory_scoreAmount; // Default 900
		long int generalVictory_timeAmount; // Default 9000 "years"

		bool IsCheckSumValid() { return this->checksum == 0x0054A3C8; }
	};


}
