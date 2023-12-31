
#pragma once

#include <AOE_struct_tech_def.h>
#include <AOE_struct_civilization_def.h>
#include <AOE_struct_player.h>
#include <AOE_struct_scenario_info.h>
#include <AOE_struct_dat_sound.h>
#include <AOE_struct_mp_communication.h>
#include <AOE_struct_graphics.h>
#include <AOE_struct_color_def.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// Those dumbasses managed to have 2 inverted values between scenario info and global struct (although values are copied directly from each other)
	// Don't confuse with GENERAL_VICTORY_CONDITION values.
	enum GAME_GLOBAL_GENERAL_VICTORY_CONDITION : long int {
		GGVC_STANDARD = 0,
		GGVC_CONQUEST = 1,
		GGVC_SCORE = 3, // stupid developers ! 0x50B4D5
		GGVC_TIME_LIMIT = 2, // stupid developers ! 0x50B4AC
		GGVC_CUSTOM = 4
	};



#define CHECKSUM_RGE_COMMAND 0x00543498
#define CHECKSUM_GAME_COMMANDS_INFO 0x00549904 // =Tribe_Command
	// Size = 0x18 for both classes RGE_command/Tribe_Command
	// Constructor = 0x42A0B0 (RGE_command) - 00 54 34 98
	// Constructor = 0x4E8390 (Tribe_Command) - 04 99 54 00
	class STRUCT_GAME_COMMANDS_INFO {
	public:
		unsigned long int checksum;
		STRUCT_GAME_GLOBAL *global;
		unsigned long int currentCommand; // unsure
		long int currentCommandSize; // unsure
		// 0x10
		long int timestamp; // Value from timeGetTime
		STRUCT_MP_COMMUNICATION *MPCommunicationStruct; // +18. cf also [0x580DA8]

		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_GAME_COMMANDS_INFO) || (this->checksum == CHECKSUM_RGE_COMMAND); }
	};
	static_assert(sizeof(STRUCT_GAME_COMMANDS_INFO) == 0x18, "STRUCT_GAME_COMMANDS_INFO size");


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
#define CHECKSUM_GAME_GLOBAL1 0x0054A4F8 // Tribe
#define CHECKSUM_GAME_GLOBAL2 0x0054A954 // RGE (size=0x104)
#endif
	// Size = 0x124 - F8 A4 54 00 (Tribe_World). Parent=54 A9 54 00. "RGE_Game_World" in source code.
	// Constructor 0x509AE0 (parent 0x51B140)
	// +0x00 = global.data_load_world(struct _iobuf *)
	// +0x04 = global.data_load_terrain_tables(char *)
	// +0x08 = unsigned char RGE_Game_World::data_load_players(char *)
	// +0x0C = unsigned char RGE_Game_World::data_load_objects(char *)
	// +0x10 = unsigned char RGE_Game_World::data_load_sounds(char *)
	// +0x14 = unsigned char RGE_Game_World::data_load_color_tables(char *)
	// +0x18 = unsigned char RGE_Game_World::data_load_sprites(char *)
	// +0x1C = void TRIBE_World::data_load_players_type(short,short,struct _iobuf *)
	// +0x20 = void RGE_Game_World::data_load_effects(char *)
	// +0x24 = void TRIBE_World::data_load_map(char *,char *,char *,char *,short,short,short,RGE_Sound * *,char *)
	// +0x30 = unsigned char RGE_Game_World::init_player(int) => global.readCivsFromDAT(internalFileId)
	// +0x38 = global.readColorsFromDAT(internalFileId)
	// +0x3C = global.readTerrainsFromDAT(internalFileId)
	// +0x40 = global.readSoundsFromDAT(internalFileId)
	// +0x44 = global.readSpritesFromDAT(internalFileId)
	// +0x48 = global.readGameMapInfoFromDAT(internalFileId)
	// +0x4C = global.read techs from DAT
	// +0x50 = global.readCivs(internalFileId)
	// +0x54 = void TRIBE_World::command_init(int,TCommunications_Handler *)
	// +0x58 = void TRIBE_World::world_init(int,TSound_Driver *,TCommunications_Handler *) => global.LoadEmpiresDatNoHeader(filename, pSoundStruct, pMPInfo?)
	// +0x5C = void TRIBE_World::setup_gaia(void)
	// +0x60 = void TRIBE_World::setup_players(struct RGE_Player_Info &) globalStruct.createPlayers(playerNamesArray)
	// +0x64 = unsigned char RGE_Game_World::new_random_game(struct RGE_Player_Info &)
	// +0x68 = void RGE_Game_World::save(int)
	// +0x6C = void TRIBE_World::scenario_make_player(short,unsigned char,unsigned char,unsigned char,char *)
	// +0x70 = void RGE_Game_World::scenario_make_map(int) => global.readMapInfo(internalFileId) ? (only terrain ?)
	// +0x74 = unsigned char RGE_Game_World::load_scenario(struct RGE_Player_Info &)
	// +0x78 = unsigned char RGE_Game_World::load_scenario(char *,struct RGE_Player_Info &)
	// +0x7C = global.readScenarioFile_1.01(internalFileId, RGE_Player_Info &)
	// +0x80 = global.readScenarioFile_1.02(internalFileId, RGE_Player_Info &). 1.03 version uses this (but read something more first)
	// +0x84 = global.readScenarioFile_1.04(internalFileId, RGE_Player_Info &)
	// +0x88 = global.readScenarioFile_1.05(internalFileId, RGE_Player_Info &)
	// +0x8C = global.readScenarioFile_1.06(internalFileId, RGE_Player_Info &)
	// +0x90 = global.readScenarioFile_1.07(internalFileId, RGE_Player_Info &)
	// +0x94 = global.readScenarioFile_1.08(internalFileId, RGE_Player_Info &)
	// +0x98 = global.readScenarioFile_1.09_1.12(internalFileId, RGE_Player_Info &) Actually even .scn use this. Note: empires.exe knows up to 1.11. ROR writes 1.11 !
	// +0xA0 = destructor(do_free).
	// +0xA4, 0xA8 = set player colors?
	// +0xAC = global.quickLoad(empiresDatRelativePath, tr_wrld.txt) = unsigned char RGE_Game_World::data_load(char *,char *)?
	// +0xB0 = globalStruct.LoadEmpiresDat(empiresDAT_filename, pSoundStruct, pMPInfo) = unsigned char TRIBE_World::init(char *,TSound_Driver *,TCommunications_Handler *)
	// +0xB4 = void RGE_Game_World::turn_sound_off(void)
	// +0xB8 = void RGE_Game_World::del_game_info(void) (reset) Called at init in 1-player games ?
	// +0xBC = unsigned char RGE_Game_World::update(void) ("main timer")
	// +0xC0 = unsigned char RGE_Game_World::get_game_state(void)
	// +0xC4 = unsigned char RGE_Game_World::check_game_state(void)
	// +0xC8 = unsigned char RGE_Game_World::load_world(int) = global.loadSavedGameNoHeader(internalFileId)
	// +0xCC = unsigned char RGE_Game_World::load_game(char *, arg2)
	// +0xD0 = global.newGameInit(struct RGE_Player_Info &, arg2) = unsigned char TRIBE_World::new_game(struct RGE_Player_Info &,int)
	// +0xD4 = ?? unsigned char RGE_Game_World::new_scenario(struct RGE_Player_Info &,int)
	// +0xD8 = global.saveGameToFile(filename_nopath)
	// +0xDC = void RGE_Game_World::base_save(char *)
	// +0xE0 = void TRIBE_World::base_save(int)
	// +0xE4 = bool global.saveScenario(filename) ; argument=filename no path, e.g. myscenario.scx. ROR saves in version 1.11.
	// +0xE8 = RGE_Scenario * RGE_Game_World::get_scenario_info(char *) (returns NULL !)
	// +0xEC = void RGE_Game_World::pause(unsigned char)
	// +0xF0 = global.createScenarioInfo?(arg1)
	// +0xF4 = global.createScenarioInfo(internalFileId, globalStruct)
	// +0xF8 = global.addUnit(unit)
	// +0xFC = int RGE_Game_World::removeObject(unitId) = remove a unit from array of ids->pointers. Does not destroy unit itself.
	class STRUCT_GAME_GLOBAL {
	public:
		unsigned long int checksum;
		long int currentGameTime; // in milliseconds. Note: 2 game "years" = 1 second. Originally named "world_time".
		long int oldGameTime; // +8. This value is serialized to saved games. Related to time
		long int gameTimeDelta; // +C. This value is serialized to saved games. Related to time
		unsigned long int unknown_010_timer;
		unsigned long int timeGetTimeValue; // +14. "old time" ?
		float gameSpeed; // +18
		char gamePaused; // confirm exact role, do not use it directly. "tempPause"
		char gameRunStatus; // +1D. 0 = Game in progress. 1 = Game Over. 2 = Not ingame. (Default). 3 = Generate Map (when starting game).
		short int unknown_01E; // +E1, status for general victory conditions? Set to 0x65 when all relics timer completes 0x50C2D0, 0x67=genvictoryTimerCompleted?
		// +0x20
		unsigned long int unknown_020;
		unsigned long int unknown_024;
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +28. Map related data. (F4 99 54 00)
		short int soundsArrayElemCount; // +2C.
		short int unknown_02E; // maybe 2 bytes, not 1 word
		STRUCT_DAT_SOUND **ptrSoundsArray; // +30.
		unsigned long int graphicsArrayElemCount; // +34. Sprites count
		STRUCT_GRAPHICS **ptrGraphicsArray; // +38. "Sprites". ptr to list of elements (size=0x48) starting by a unit name?
		short int playerTotalCount; // +3C. Player count including gaia = real player count + 1
		short int unknown_03E;
		// +0x40
		STRUCT_PLAYER **ptrPlayerStructPtrTable; // points to table of player struct addresses (pointers)
		short int civCount; // +44. Number of civilizations, including gaia (index 0).
		short int unknown_046;
		STRUCT_CIVILIZATION_DEF **civilizationDefinitions; // +48. A pointer to the array of pointers to civ definitions. Array size is civCount
		STRUCT_TECH_DEF_INFO *technologiesInfo; // +4C. 20 99 54 00 or BC 34 54 00 (parent?)
		// +0x50
		short int terrainRestrictionCount; // Size for ptrTerrainRestrictionArray array
		short int nbOfTerrainPerTerrainRestriction; // +52. How many terrain for each terrain restriction (common for all).
		float **ptrTerrainRestrictionArray; // +54. Array of pointers to terrain restriction info (if an array of float=0/1). Example 426951. ptrTerrainRestrictionArray[terrainRestriction][terrainId]=float : 0=inaccessible, othervalue=accessible=damagefactor
		STRUCT_GAME_COMMANDS_INFO *commandsInfo; // +58. 04 99 54 00.
		STRUCT_SCENARIO_INFO *scenarioInformation; // +5C.
		// +0x60
		short int colorCount; // +60. Number of colors (size of colors table)
		unsigned short int unknown_062;
		STRUCT_COLOR_DEF **colorsTable; // +64. Color table. The table size in bytes is colorCount*4. Each DWORD is a pointer to color definition;
		long int seqUnitId; // +68. Sequence for unit instance IDs (for creatable only ?). *Next* object ID.
		long int unitIdSeqForTempUnits; // +6C. Used for "negative" units.
		// +0x70
		long int forcedNextUnitId; // Unsure. Forced when loading a scenario.
		char forceUseSpecifiedUnitId; // +74. If set (temporarily), use "+70" as next unitInstanceId instead of reading sequence.
		char unknown_75[3]; // +75. unused ?
		unsigned long int unknown_078; // random value for seed ?
		short int humanPlayerId; // +7C. PlayerId that is human controlled (controlledPlayerId)
		short int unknown_07E;
		// +0x80
		unsigned long int unknown_080; // list of unit data??
		float timerInterval; // +84. Unit is *half* a "game year" (like the 2000 years for wonder victory) ?
		STRUCT_UNIT_BASE **ptrUnitPointersList; // +88. pointer to Array of unit struct pointers. Similar to 0x7D2054 global variable.
		long int numberObjects; // +8C
		// +0x90
		long int unitPointersListSize; // Size (elem count) of the ptrUnitPointersList Array. "MaxNumberObjects"
		STRUCT_UNIT_BASE **negativeUnitPointersList; // +94. array of DWORDS. Temporary units array (units with negative IDs) ?
		long int numberNegativeObjects; // +98
		long int negativeUnitPointersListSize; // +9C. Size of x+0x94 array. Default=1000. "MaxNumberNegativeObjects".
		// +0xA0
		unsigned long int unknown_0A0; // A0: related to aoe.ply ?
		long int currentCampaignIndex; // +A4. Identifies the campaign being played. Values are 0-(n-1), but serialized to file as a "1-(n-1)" value ? With a bug handling "-1" in 0051CC92.
		long int currentCampaignUserIndex; // +A8. Identifies the campaign user ("account")
		long int currentCampaignScenarioIndex; // +AC. Identifies the scenario index in campaign being played
		// +0xB0
		unsigned long int currentlyHandledPlayerId; // +B0. currently managed playerid. Cf 0x51DD10.
		unsigned long int accumulatedTimeDelta[0x09]; // +B4. 9 variables for each player, including gaia (really unsigned int)
		// +0xD8. "unused cached units" = "removed" units that were not freed (so that memory is reused). Typically for flares, dead units.
		STRUCT_OBJECT_LIST *unusedCachedUnits_type10; // +D8. Allocated but unused units for unit type = eye candy
		STRUCT_OBJECT_LIST *unusedCachedUnits_type20; // +DC. Allocated but unused units for unit type = flag
		// +0xE0
		STRUCT_OBJECT_LIST *unusedCachedUnits_type30; // +E0. Allocated but unused units for unit type = movable
		STRUCT_OBJECT_LIST *unusedCachedUnits_type40; // +E4. Allocated but unused units for unit type = birds
		STRUCT_OBJECT_LIST *unusedCachedUnits_type50; // +E8. Allocated but unused units for unit type = type50
		STRUCT_OBJECT_LIST *unusedCachedUnits_type60; // +EC. Allocated but unused units for unit type = projectiles. 0x520BD0
		STRUCT_OBJECT_LIST *unusedCachedUnits_doppleganger; // +F0. list of all dopplegangers ?
		long int global_allowedTimeForAITreatment_ms; // +F4. Time to allocate for AI treatments
		long int tmp_allowedTimeForAITreatment; // +F8. For internal treatments (temporary, reset to 0 when finished). This is a time (ms) value allowed for AI treatments.
		long int currentlyManagedAIPlayer; // +FC. A player id. "currentUpdateComputerPlayer"
		// +0x100
		unsigned long int unknown_100_difficultyLevel; // NOT always diff. level. See in game settings struct.
		// Base class ends here
		STRUCT_RESEARCH_DEF_INFO *researchDefInfo; // +104. WARNING: ResearchDefs are shared by all players and even "empires.dat reference". DO NOT MODIFY IT !
		GAME_GLOBAL_GENERAL_VICTORY_CONDITION generalVictoryCondition; // +108. 0=standard, 1=conquest...
		long int relicsCount;
		// +0x110
		long int ruinsCount;
		char generalVictoryConditionIsScore; // +114. Unsure of the exact meaning, but set to 1 only when general condition=score
		char unknown_115[3]; // unused ?
		float generalVictory_remainingTimeAmount; // +118. Used when victory condition is "score". compared to 1 in 0x50BF38
		char showScores; // 0x11C. Call 0x484F90 to set it (global.showScores(bool))
		char temporaryTargetPlayerId; // 11D. Used for AI players. "ConCP", CP=computerPlayer? Con=?concerned?
		short int unknown_11E;
		// 0x120
		long int unknown_120;

		AOE_STRUCTURES::STRUCT_PLAYER ** GetPlayerStructPtrTable() const {
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
		// Get a civilization definition, or NULL if not found
		AOE_STRUCTURES::STRUCT_CIVILIZATION_DEF *GetCivDef(short int civDefId) const {
			if (!this->civilizationDefinitions || (this->civCount < civDefId)) { return NULL; }
			return this->civilizationDefinitions[civDefId];
		}
		// Returns the array of (float) values for demanded terrain restriction.
		// 0 = not accessible, >0 means accessible and is the value for armor damage multiplier
		// Returns NULL if restriction is invalid
		float *GetTerrainRestrictionValues(TERRAIN_RESTRICTION restriction) const {
			if ((restriction >= this->terrainRestrictionCount) || (restriction < 0)) { return NULL; }
			return this->ptrTerrainRestrictionArray[restriction];
		}
		// Returns a unit structure from its ID. Only works for >=0 IDs.
		STRUCT_UNIT_BASE *GetUnitFromId(long int unitId) const {
			if ((unitId < 0) || (unitId >= this->seqUnitId)) {
				return NULL;
			}
			return ptrUnitPointersList[unitId];
		}
		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_GAME_GLOBAL1) || (this->checksum == CHECKSUM_GAME_GLOBAL2); }
	};


}
