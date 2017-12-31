
#pragma once

#include <AOE_empires_dat.h>
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



#define CHECKSUM_GAME_TIMELINE 0x0054992C
	// "RGE_Timeline". Size=0x14
	// Constructor : 0x4EBBF0 (readFromFile), 0x4EBDC0
	class STRUCT_GAME_TIMELINE {
	public:
		unsigned long int checksum; // 2C 99 54 00
		STRUCT_GAME_GLOBAL *global;
		unsigned long int unknown_08;
		short int unknown_0C; // Count of... ? (+08 ptr ?)
		short int unknown_0E;
		// 0x10
		float gameTime;

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_GAME_TIMELINE); }
	};


	// Size 0x3C. An array of 0x10 elements starts at scenarioInfo+0x1AA8.
	// As there are 0x0C slots, the size for 1 player is 0x2D0(=3C*C). For 0x10 players, it is 0x2D00.
	class STRUCT_SCENARIO_IND_VICTORY_CONDITION {
	public:
		long int objectUnitDefId; // +00 (+1AA8). UnitDefId for create/destroy object.
		long int allFlag; // +04 (+1AAC). If set, then the condition applies on ALL objects. Only used for "destroy ALL objects" (of a kind) ?
		long int targetPlayerId; // +08 (+1AB0). Target playerId (1-8) of the individual victory condition.
		long int unknown_0C;
		float targetZoneMinPosY; // +10 (+1AB8)
		float targetZoneMinPosX;
		float targetZoneMaxPosY;
		float targetZoneMaxPosX; // +1C (+1AC4)
		AOE_CONST_INTERNAL::INDIVIDUAL_VICTORY_CONDITION victoryTypeId; // +20 (+1AC8). Represents the type of condition the slot represents.
		long int amount; // +24 (+1ACC). Number of units to create, of razings, resource amount to reach (type4) (can be ageId for "age", stone=1).
		long int resourceType; // +28 (+1AD0). Nature depends on victoryTypeId. Type4=>SCENARIO_INFO_RESOURCE_TYPE. Type6=>RESOURCE_TYPES. Type7=>researchID.
		long int actionUnitId; // +2C (+1AD4). Target unit ID for "Kill 1 object", "capture"
		long int targetUnitId; // +30 (+1AD8).
		unsigned long int unknown_34;
		unsigned long int unknown_38;
		// Returns WRT_NONE if victory condition type is NOT relevant
		AOE_CONST_INTERNAL::SCENARIO_INFO_RESOURCE_TYPE GetSpecificResourceTypeIfRelevant() const {
			if (this->victoryTypeId != AOE_CONST_INTERNAL::INDIVIDUAL_VICTORY_CONDITION::IVC_SPECIFIC_RESOURCE) {
				return AOE_CONST_INTERNAL::SCENARIO_INFO_RESOURCE_TYPE::WRT_NONE;
			}
			return (AOE_CONST_INTERNAL::SCENARIO_INFO_RESOURCE_TYPE)this->resourceType;
		}
		// Returns -1 if victory condition type is NOT relevant
		AOE_CONST_FUNC::RESOURCE_TYPES GetStandardResourceTypeIfRelevant() const {
			if (this->victoryTypeId != AOE_CONST_INTERNAL::INDIVIDUAL_VICTORY_CONDITION::IVC_STANDARD_RESOURCES) {
				return (AOE_CONST_FUNC::RESOURCE_TYPES)-1;
			}
			return (AOE_CONST_FUNC::RESOURCE_TYPES)this->resourceType;
		}
	};
	static_assert(sizeof(STRUCT_SCENARIO_IND_VICTORY_CONDITION) == 0x3C, "STRUCT_SCENARIO_IND_VICTORY_CONDITION size");


#define CHECKSUM_SCENARIO_INFO_BASE 0x00545E3C
#define CHECKSUM_SCENARIO_INFO 0x0054A3C8
	// Size = 0x1990. Constructor=0x474C90, 0x474D40. "RGE_Scenario". Please use child class.
	class STRUCT_SCENARIO_INFO_BASE {
	public:
		unsigned long int checksum; // C8 A3 54 00 ; parent struct = 3C 5E 54 00
		STRUCT_GAME_TIMELINE *timeInfo; // +04.
		STRUCT_GAME_GLOBAL *globalStruct;
		char generalVictory_conquestBase; // +0C. Init = 1 (default). Duplicate with child class ?
		char tribeNames[0x10][0x100]; // +D. Index=(playerId-1)
		char unknown_100E; // unused?
		char unknown_100F; // unused?
		// 0x10
		char *scenarioFileName; // +1010. Actually reset to "" when loading a non-scenario saved game. Loading a scenario saved game correctly sets this name, very useful !
		char *scenarioInstructions; // +1014. "description"
		char *scenarioTips;
		char *scenarioVictoryText;
		// 0x1020
		char *scenarioDefeatedText;
		char *scenarioHistoryText;
		//char unknown_1028[0x12C8 - 0x1028]; // 475C55
		char fullPath_tmpBuildList[0xE0];
		char unknown_1108[0x12C8 - 0x1108];
		char *strategyFileNames[0x10]; // +12C8. Index=(playerId-1). Size = 0xE0. Warning: file name is NOT empty when no explicit strategy has been set, because this name is "Random" or "Aléatoire"... Refer to content size==0.
		char *cityPlanFileNames[0x10]; // +1308. Index=(playerId-1). Size = 0xE0.
		char *personalityFileNames[0x10]; // +1348. Index=(playerId-1). Size = 0xE0.
		char *strategyFileContent[0x10]; // +1388. pointer to AI file content. See in 408948 (read from a temp file, not from THIS). Is size limited to ~32ko ? Not sure
		char *cityPlanFileContent[0x10]; // +13C8. Pointer to an array of bytes.
		char *personalityFileContent[0x10]; // +1408. Pointer to an array of bytes. Note: The "END" keyword actually stops reading the file.
		long int strategyFileSize[0x10]; // +1448. If 0, then no "explicit" strategy has been set. The game will choose one (for RM) randomly at game start.
		long int cityPlanFileSize[0x10]; // +1488.
		long int personalityFileSize[0x10]; // +14C8.
		char scenarioBeginningAnimationFileName[0xE0]; // +1508
		char scenarioVictoryAnimationFileName[0xE0]; // +15E8
		char scenarioFailureAnimationFileName[0xE0]; // +16C8
		char scenarioBitmapFileName[0xE0]; // +17A8
		float scenarioVersion; // +1888. Corresponds to .scn/.scx version, like 1.02, 1.03, 1.04, 1.05, 1.13, 1.15(=ROR 1.0a). Not always reset (when loading a non-scenario game)
		long int player_unknown_188C_posture[0x10]; // +188C. Often referred to as +18CC-40. Default = 4 ? Player posture ?
		long int playerIsHumanPlayable[0x10]; // +18CC. 0 if computer player only (cannot be played as human). "Player type"
		long int playerCivilizationId[0x10]; // +190C. Often referred to as +18CC+40.
		long int playerIsActive[0x10]; // +194C. 1 if player is active. Often referred to as +18CC+80.
		long int *unknown_198C; // +198C. ptr to struct size=0x20, constructor 45AF60. TO DO.

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_SCENARIO_INFO_BASE); }
		bool IsCheckSumValidForAScenarioInfoClass() const { return (this->checksum == CHECKSUM_SCENARIO_INFO) || (this->checksum == CHECKSUM_SCENARIO_INFO_BASE); }
	};
	static_assert(sizeof(STRUCT_SCENARIO_INFO_BASE) == 0x1990, "STRUCT_SCENARIO_INFO_BASE size");


	// Size = 0x514C. Constructor=0x0506900, 0x506830. "T_Scenario".
	// Warning: all arrays[0x10] are indexed by playerId-1, which means tribeNames[0] is player1's name.
	class STRUCT_SCENARIO_INFO : public STRUCT_SCENARIO_INFO_BASE {
	public:
		long int startingResources[0x10][4]; // +1990. Index=(playerId-1). WARNING: resource order = gold/wood/food/stone here !!! Use SCENARIO_INFO_RESOURCE_TYPE as index.
		// 0x1A90 - probably a sub-structure of size=0x3160 (last 4 bytes are unused) for victory conditions settings.
		// Those generalVictory_* are only relevant if generalVictory_mainCondition==4 (custom)
		long int generalVictory_conquest; // 1 = Standard, Conquest, Score, Time Limit, Custom if Conquest is enabled. ; 0=others. Duplicate with base class ?
		long int generalVictory_ruinsCount; // +1A94. Number of ruins to collect to win the game (for all players)
		long int generalVictory_relicsCount; // +1A98. Number of relics to collect to win the game (for all players)
		long int generalVictory_discoveriesCount; // +1A9C. Number of discoveries to find to win the game (for all players)
		// 0x1AA0
		long int generalVictory_explorationAmount; // % of exploration to reach to win the game (for all players)
		long int generalVictory_goldAmount; // +1AA4. Amount of gold to collect to win the game (for all players). Hidden in scenario editor (value is never modified by editor).
		STRUCT_SCENARIO_IND_VICTORY_CONDITION individualVictoryConditions[0x10][0x0C]; // +1AA8. See 507140 and following
		long int diplomacySettings[0x10][0x10]; // +47A8. Refers to PLAYER_DIPLOMACY_STANCES but as DWORDs. 0=allied,1=neutral,3=enemy.
		long int hasAlliedVictory[0x10]; // +4BA8. 1=option is checked for that player
		long int artefactsRequireAll; // +4BE8. 0="at least one", 1="all". Applies to relics, ruins, discoveries ?
		unsigned long int unknown_4BEC; // +4BEC0. Unused, it seems.
		long int currentPlayerIdMinusOne; // +4BF0. PlayerId-1 being selected in scenario editor (player tab, etc).
		long int disableTechnologyFlags[0x10][AOE_CONST_INTERNAL::SC_ED_DISABLE_TECHNOLOGY_INDICES::CST_DTI_COUNT]; // +4BF4. Total Size =0x500 bytes. Used in 0x507E50=scenarioInfo.applyDisabledResearches(player)
		// +50F4 is an array of 3 long int. Index 0=lengthenCombat, 1=?, 2=allTechs. Cf Getter in 0x5076A0 (getter is only used with index 2, beware direct accesses too).
		long int enableLengthenCombatMode; // +50F4. A flag 0/1. If 1, lengthen combat mode is ON (most units get *3 HP, it is technology 0x64)
		long int unusedGameMode_50F8; // +50F8. Supposedly another "game mode" option, unused (sure at 99%)
		long int fullTechTree; // +50FC. Stored in savegame files, but always 0 (fixed by RockNRor) in RM/DM games. Initially supposed to indicate if scenario uses all techs. We extend this to all games in RockNRor. Set to 1 when using "custom tech trees".
		// 0x5100
		long int playersStartingAge[0x10]; // 1=Tool, 4=post iron (SCENARIO_INFO_AGE_ID). Warning, this does not correspond to CST_AGE_TOOL, etc. Index is playerId-1. Not initialized in RM/DM games ! (fixed by RockNRor)
		// 0x5140
		AOE_CONST_INTERNAL::GENERAL_VICTORY_CONDITION generalVictory_mainCondition; // 0=Standard, 1=Conquest, 2=Score, 3=Time Limit, 4=Custom
		long int generalVictory_scoreAmount; // +5144. Default 900
		long int generalVictory_timeAmount; // +5148. Default 9000 "years"

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_SCENARIO_INFO) || (this->checksum == CHECKSUM_SCENARIO_INFO_BASE); }
	};
	static_assert(sizeof(STRUCT_SCENARIO_INFO) == 0x514C, "STRUCT_SCENARIO_INFO size");


#define CHECKSUM_SCENARIO_HEADER_BASE 0x00545E34 // Size=0x18. Constructor=0x474AE0(fileId), 0x474A40(scenarioInfo)
#define CHECKSUM_SCENARIO_HEADER 0x0054A3C0
	// Size=0x1C. Constructor=0x506770(fileId), 0x506710(scenarioInfo)
	class STRUCT_SCENARIO_HEADER {
	public:
		unsigned long int checksum; // 34 5E 54 00
		long int unknown_04; // +04. Set to 1 when initialized ? Or when empty ?
		long int unknown_08; // +08. Some enum ? Seen 2...
		char *scenarioHeaderDescription; // +0x0C. Result from 0x52668C.
		char *scenarioInstructions; // +10. Copy of scenario instructions from scenarioInformation.
		unsigned long int unknown_14;
		unsigned long int unknown_18;

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_SCENARIO_HEADER_BASE) || (this->checksum == CHECKSUM_SCENARIO_HEADER); }
	};
	static_assert(sizeof(STRUCT_SCENARIO_HEADER) == 0x1C, "STRUCT_SCENARIO_HEADER size");

}
