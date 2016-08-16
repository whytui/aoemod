
#pragma once

#include <AOE_struct_tech_def.h>
#include <AOE_struct_civilization_def.h>
#include <AOE_struct_player.h>
#include <AOE_struct_scenario_info.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {


	// Size = ? - 04 99 54 00
	class STRUCT_GAME_COMMANDS_INFO {
	public:
		unsigned long int checksum;
		STRUCT_GAME_GLOBAL *global;
		unsigned long int currentCommand; // unsure
		long int currentCommandSize; // unsure
		// 0x10
		long int timestamp; // Value from timeGetTime
		unsigned long int *unknown_14; // Ptr to struct similar to [580DA8]

		bool IsCheckSumValid() { return this->checksum == 0x00549904; }
	};


#ifdef GAMEVERSION_AOE10b
#define CHECKSUM_GAME_GLOBAL1 0x00554A58
#define CHECKSUM_GAME_GLOBAL2 0x00553230
#endif
#ifdef GAMEVERSION_AOE10c
#define CHECKSUM_GAME_GLOBAL1 0x005432AC
#define CHECKSUM_GAME_GLOBAL2 0x005436FC
#endif
#ifdef GAMEVERSION_ROR10b // cf 0x50C250
#define CHECKSUM_GAME_GLOBAL1 0x0054F990
#define CHECKSUM_GAME_GLOBAL2 0x0054FE58
#endif
#ifdef GAMEVERSION_ROR10c
#define CHECKSUM_GAME_GLOBAL1 0x0054A4F8
#define CHECKSUM_GAME_GLOBAL2 0x0054A954
#endif
	// Size = 0x124 - F8 A4 54 00. Parent=54 A9 54 00
	// Constructor 0x509AE0 (parent 51B140)
	class STRUCT_GAME_GLOBAL {
	public:
		unsigned long int checksum;
		unsigned long int currentGameTime; // in milliseconds
		unsigned long int unknown_008;
		unsigned long int unknown_00C;
		unsigned long int unknown_010;
		unsigned long int timeGetTimeValue;
		float gameSpeed;
		char gamePaused; // confirm exact role, do not use it directly.
		char gameRunStatus; // 0 = Game in progress. 1 = Game Over. 2 = Not ingame. (Default). 3 = Generate Map (when starting game).
		short int unknown_01E;
		// +0x20
		unsigned long int unknown_020;
		unsigned long int unknown_024;
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +28. Map related data. (F4 99 54 00)
		short int soundsArrayElemCount;
		short int unknown_02E; // maybe 2 bytes, not 1 word
		// +0x30
		unsigned long int ptrSoundsArray;
		unsigned long int unknown_034;
		unsigned long int ptrGraphicsArray; // ptr to list of elements (size=0x48) starting by a unit name?
		short int playerTotalCount; // Player count including gaia = real player count + 1
		short int unknown_03E;
		// +0x40
		STRUCT_PLAYER **ptrPlayerStructPtrTable; // points to table of player struct addresses (pointers)
		short int civCount; // +44. Number of civilizations.
		short int unknown_046;
		STRUCT_CIVILIZATION_DEF **civilizationDefinitions; // +48. A pointer to the array of pointers to civ definitions. Array size is civCount
		STRUCT_TECH_DEF_INFO *technologiesInfo; // +4C. 20 99 54 00
		// +0x50
		unsigned short int terrainRestrictionCount; // Size for ptrTerrainRestrictionArray array
		unsigned short int nbOfTerrainPerTerrainRestriction; // +52. How many terrain for each terrain restriction (common for all)
		float **ptrTerrainRestrictionArray; // +54. Array of pointers to terrain restriction info (if an array of float=0/1). Example 426951. ptrTerrainRestrictionArray[terrainRestriction][terrainId]=float : 0=inaccessible, othervalue=accessible=damagefactor
		STRUCT_GAME_COMMANDS_INFO *commandsInfo; // +58. 04 99 54 00.
		STRUCT_SCENARIO_INFO *scenarioInformation; // +5C.
		// +0x60
		unsigned short int unknown_060; // size for +64 ?
		unsigned short int unknown_062;
		unsigned long int unknown_064; // .col list ?
		long int seqUnitId; // Sequence for unit instance IDs (for creatable only ?)
		unsigned long int unknown_06C_unitIdSeqForTempUnits; // Unsure
		// +0x70
		long int forcedNextUnitId; // Unsure. Forced when loading a scenario.
		long int forceUseSpecifiedUnitId; // +74. If set (temporarily), use "+70" as next unitInstanceId instead of reading sequence.
		unsigned long int unknown_078; // random value for seed ?
		short int humanPlayerId; // +7C. PlayerId that is human controlled (controlledPlayerId)
		short int unknown_07E;
		// +0x80
		unsigned long int unknown_080; // list of unit data??
		float timerInterval;
		STRUCT_UNIT_BASE **ptrUnitPointersList; // +88. pointer to Array of unit struct pointers
		unsigned long int unknown_08C;
		// +0x90
		long int unitPointersListSize; // Size of the ptrUnitPointersList Array
		unsigned long int unknown_094;
		unsigned long int unknown_098;
		long int unknown_094_ListSize; // Size of x+0x94 array
		// +0xA0
		char unknown_0A0[0x10]; // A0: related to aoe.ply ?
		// +0xB0
		unsigned long int unknown_0B0_playerId; // currently managed playerid?
		unsigned long int unknownPlayerVar[0x09]; // 9 variables for each player, including gaia
		// +0xD8
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type10; // +D8. For unit type = eye candy
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type20; // +DC. For unit type = flag
		// +0xE0
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type30; // +E0. For unit type = killable ?
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type40; // +E4. For unit type = birds
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type50; // +E8. For unit type = type50
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type60; // +EC. For unit type = projectiles. 0x520BD0
		// +0xF0
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_doppleganger; // list of all dopplegangers ?
		unsigned long int unknown_0F4;
		long int tmp_allowedTimeForAITreatment; // +F8. For internal treatments (temporary, reset to 0 when finished). This is a time value allowed for AI treatments
		long int currentlyManagedAIPlayer; // +FC. A player id. "currentUpdateComputerPlayer"
		// +0x100
		unsigned long int unknown_100_difficultyLevel; // NOT always diff. level. See in game settings struct.
		STRUCT_RESEARCH_DEF_INFO *researchDefInfo; // +104.
		unsigned long int unknown_108;
		unsigned long int relicsCount;
		// +0x110
		unsigned long int ruinsCount;
		char unknown_114;
		char unknown_115[3]; // unused ?
		unsigned long int unknown_118;
		char WRONG_showScores; // 0x11C. WRONG ?
		char temporaryTargetPlayerId; // 11D. Used for AI players. "ConCP", CP=computerPlayer? Con=?concerned?
		short int unknown_11E;
		// 0x120
		long int unknown_120;

		AOE_STRUCTURES::STRUCT_PLAYER ** GetPlayerStructPtrTable() const{
			return (AOE_STRUCTURES::STRUCT_PLAYER **) this->ptrPlayerStructPtrTable;
		}
		// Returns player structure corresponding to supplied ID. Returns NULL if not found
		AOE_STRUCTURES::STRUCT_PLAYER *GetPlayerStruct(int playerId) const {
			if (!this->IsCheckSumValid()) { return NULL; }
			if ((playerId < 0) || (playerId >= this->playerTotalCount) || (playerId >= 9)) { return NULL; }
			return this->ptrPlayerStructPtrTable[playerId];
		}
		// Get a technology definition, or NULL if not found
		AOE_STRUCTURES::STRUCT_TECH_DEF *GetTechDef(short int techDefId) const {
			if (!this->technologiesInfo || !this->technologiesInfo->IsCheckSumValid()) { return NULL; }
			return this->technologiesInfo->GetTechDef(techDefId);
		}
		// Returns the array of (float) values for demanded terrain restriction.
		// 0 = not accessible, >0 means accessible and is the value for armor damage multiplier
		// Returns NULL if restriction is invalid
		float *GetTerrainRestrictionValues(TERRAIN_RESTRICTION restriction) const {
			if ((restriction >= this->terrainRestrictionCount) || (restriction < 0)) { return NULL; }
			return this->ptrTerrainRestrictionArray[restriction];
		}
		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_GAME_GLOBAL1) || (this->checksum == CHECKSUM_GAME_GLOBAL2); }
	};


}
