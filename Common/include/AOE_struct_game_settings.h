
#pragma once

#include <AOE_empires_dat.h>
#include <AOE_struct_command_line_info.h>
#include <AOE_struct_sound.h>
#include <AOE_struct_mp_communication.h>
#include <AOE_structures_drs.h>
#include <AOE_struct_game_global.h>
#include <AOE_struct_campaign_info.h>
#include <gameVersion.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/

using namespace AOE_CONST_INTERNAL;

namespace AOE_STRUCTURES {
	// External dependencies
	class STRUCT_UI_IN_GAME_MAIN;

	// Size 0x2C. Used in 0x4F6D7E
	// Provides interaction mode information for each AI type (considering unit=potential target of a right-click)
	// Information depends on diplomacy values (cf AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES): e.g. a building can be repaired if allied OR attacked if enemy.
	class STRUCT_AITYPE_RIGHTCLICK_INFO {
	public:
		GLOBAL_UNIT_AI_TYPES unitTypeId;
		short int unknown_02; // maybe unused OR unitTypeId is a long int
		UNIT_INTERACTION_ID selfUnitInteraction; // +04. Mask of UNIT_INTERACTION_MASK_BIT bits, for the player itself (dipl1 / actor&target units have same owner)
		long int selfLangDllId; // +08. Text for "self" (dipl1). Example: repair, heal...
		UNIT_INTERACTION_ID gaiaUnitInteraction; // +0C. Mask of UNIT_INTERACTION_MASK_BIT bits, for "gaia" (dipl0)
		long int gaiaLangDllId; // +10. Text for "gaia" targets (dipl0)
		UNIT_INTERACTION_ID alliedUnitInteraction; // +14. Mask of UNIT_INTERACTION_MASK_BIT bits, for allied diplomacy
		long int alliedLangDllId; // +18. Text for allied diplomacy. Example: repair, heal...
		UNIT_INTERACTION_ID neutralUnitInteraction; // +1C. Mask of UNIT_INTERACTION_MASK_BIT bits, for neutral diplomacy
		long int neutralLangDllId; // +20. Text for neutral diplomacy
		UNIT_INTERACTION_ID enemyUnitInteraction; // +24. Mask of UNIT_INTERACTION_MASK_BIT bits, for enemy diplomacy
		long int enemyLangDllId; // +28. Text for enemy diplomacy
	};
	static_assert(sizeof(STRUCT_AITYPE_RIGHTCLICK_INFO) == 0x2C, "STRUCT_AITYPE_RIGHTCLICK_INFO size");


#ifdef GAMEVERSION_AOE10b
#define CHECKSUM_GAME_SETTINGS1 0x005509D8
#define CHECKSUM_GAME_SETTINGS2 0x00553B78 // parent class
#endif
#ifdef GAMEVERSION_AOE10c
#define CHECKSUM_GAME_SETTINGS1 0x0054301C
#define CHECKSUM_GAME_SETTINGS2 0x0053C0A4 // parent class
#endif
#ifdef GAMEVERSION_ROR10b
#define CHECKSUM_GAME_SETTINGS1 0x0054F6E0
#define CHECKSUM_GAME_SETTINGS2 0x00547DE8 // parent class
#endif
#ifdef GAMEVERSION_ROR10c
#define CHECKSUM_GAME_SETTINGS1 0x0054A264
#define CHECKSUM_GAME_SETTINGS2 0x005430B4 // parent class
#endif
	// Size = 0x11A0 (AOE1.0b&c). Constructor 0x4F91C0 (empires.exe)
	// Size = 0x1254. Constructor 0x5004C0(1.0b), 0x4FDFA0(1.0c) - arg1=commandLineInfo, arg2=?
	class STRUCT_GAME_SETTINGS {
	public:
		unsigned long int checksum;
		GAME_NFO_DATA *game_nfo_data; // +04. no checksum
		unsigned long int *unknown_008_ptr_scenario_data; // +08. scenario.inf path... +other data?
		STRUCT_COMMAND_LINE_INFO *commandLineInfo;
		// 0x10
		char unknown_010[0x1C];
		unsigned long int scenarioMapSeed; // +2C. When supplied manually through scenario editor. -1 for RM / DM
		unsigned long int unknown_030;
		unsigned long int actualMapSeed; //+34.  THE actual map seed
		unsigned long int unknown_038;
		unsigned long int unknown_03C;
		// 0x40
		char unknown_040[0x1C];
		STRUCT_SLP_INFO **ptrInfosSLP; // +5C. Pointer to array slpInfo, size=3 ? index 0=shortcut numbers
		// 0x60
		STRUCT_MAIN_SOUND *pSoundStruct; // Size 69C, see 41894B. Generally =NULL, only set while being used.
		STRUCT_MAIN_MUSIC *pMusicStruct; // Size 3F8, see 418B14. Generally =NULL, only set while being used.
		long int commonSoundDataArrayElemCount; // +68. Default 0x11 (see 4FEF2D).
		unsigned long int ptrCommonSoundDataArray; // +6C.
		// 0x70
		char musicFormat; // 0=AudioCD, 1=MIDI
		char unknown_071;
		char unknown_072;
		char unknown_073;
		long int minMusicTrack;
		long int maxMusicTrack;
		long int currentMusicTrack;
		// 0x80
		char musicFilename[0x184 - 0x80]; // Chack this...
		long int musicIsLooping; // +184
		long int musicSeekPos; // Resume from minimized window
		STRUCT_MP_COMMUNICATION *MPCommunicationStruct; // +18C
		// 0x190
		char unknown_190[0x1AC - 0x190];
		unsigned long int unknown_1AC; // related to registry ?
		// 0x1B0
		AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS currentUIStatus; // 0=loading,2=inMenu,4=playing,7=editor
		AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES mouseActionType; // +1B4. Set this during game, you'll be able to edit the map !! userSelected* fields
		long int unknown_1B8; // Values 1,4,6,... ? SubMouseActionType ? Used when mouseActionType=6 (villager build menu) or in editor. Seen 1=editor_moveUnit?, 3, 4(editor_putunit?). Unsure !
		long int unknown_1BC;
		// 0x1C0
		char unknown_1C0[0x10];
		// 0x1D0
		unsigned long int unknown_1D0;
		long int hCursor; // Cursor handle, default=10003
		unsigned long int unknown_1D8;
		unsigned long int unknown_1DC;
		// 0x1E0
		long int unknown_1E0;
		long int unknown_ptrFonts; // Ptr to array, elemSize=0x0C, elemIndex = fontId
		char gameDirFullPath[0x105]; // To confirm
		char languageDLLRelativePath[0x105]; // 2ED. "language.dll"
		char unknown_3F2; // unused ?
		char unknown_3F3; // unused ?
		STRUCT_GAME_GLOBAL *ptrGlobalStruct; // +3F4
		unsigned long int unknown_3F8;
		short int editorUserSelectedUnitDefId; // +3FC: the unitDefID that user selected in "units" tab.
		short int editorUserSelectedTerrainId; // Can be changed in editor/terrain tab
		// 0x400
		short int editorUserSelectedAltitude; // in editor/terrain tab
		short int editorTerrainPencilSize; // in editor/terrain tab
		char debugString_404[0x508 - 0x404]; // "Avg view time"...
		long int unknown_508;
		long int unknown_50C; // a counter
		char unknown_510[0x528 - 0x510];
		char unknown_528[0x8E0 - 0x528]; // An included array, elemSize=0x20; getter=417430
		// +4F0 : pointer ? +100=Affects DisableMapSizeSetting
		// 0x8E0
		long int screenSizeX; // To confirm + confirm type
		long int screenSizeY; // To confirm + confirm type
		unsigned long int unknown_8E8;
		unsigned long int unknown_8EC;
		unsigned long int unknown_8F0;
		unsigned long int unknown_8F4;
		float gameSpeed; // +8F8. TO confirm - a struct starts here ?
		char isScenario; // +8FC. 1 for scenario/campaign, 0 for random game/deathMatch. value is ARBITRARY for loaded games, unless we fix it manually (done by customROR)
		char scenarioFileName[3 + 0x7C]; // +8FD.
		// 0x900
		char unknown_97C;
		char isSinglePlayer;
		char isMultiplayer; // 0x97E
		char mapSizeX;
		// 0x980
		char mapSizeY;
		char unknown_981; // Related to map size. Always 8 ? Max player # ?
		char enableCheatMode; // +982. Set in 41BAE0
		char unknown_983; // +983. path finding for MP ? 0-2 default/medium/high ?
		char unknown_984; // Something to do with screen width and height??
		char revealMap; // 0x985. Set in 41BB00
		char noFog; // 0x986. Set in 41BB10
		char unknown_987; // default 1 ? Set in 41BB20
		char playerCount; // +988.
		char unknown_989; // can set speed > 16 ?
		char playerGameVersion[0x08]; // 0-127 ; 0="", 1="1.0", 2="1.0a", etc (array offset in 0-7)
		char unknown_992;
		AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL difficultyLevel; // +993. 0=hardest, 1=hard, 2, 3, 4=easiest. Set in 41BBC0
		char teamNumbers[0x08]; // player 1-8 (array offset in 0-7). Values: 1=noTeam, >1=teamId.
		char unknown_99C;
		// 0x9A0
		long int isCampaign; // get in 41B720, set in 41BA50
		long int isSavedGame; // +9A4. set in 41BA60
		long int hasSteroidsCheatCode; // +9A8. set in 50CA2B, 4E90B6. Can be cancelled !!!
		long int has_CD; //see 4169D7
		// 0x9B0
		unsigned long int chosenPlayerIndex[9]; // index in 0-8 (playerId, including gaia)
		// to 9D4...
		unsigned long int unknown_9D4;
		unsigned long int unknown_9D8[9]; // ???
		// to 9FC...
		unsigned long int gameIsRunning; // You can simply set it to 0 to pause game.
		// 0xA00
		char unknown_A00[0x0C];
		float unknown_A0C;
		char difficultyLevelChoice; // +A10. Get in 41C190. Copied into +993
		char unknown_A11;
		char unknown_A12;
		char unknown_A13;
		char pathFindingChoice; // 0xA14 (path finding value = 0-2)
		char playerIsInactive[0x09]; // +A15. Index is a "playerIndex"(???Wrong?) (cf +9B0), not playerID. 1 for player that lost/resigned..., 0 if alive
		char unknown_A1E;
		char unknown_A1F;
		// 0xA20
		unsigned long int unknown_A20;
		STRUCT_AITYPE_RIGHTCLICK_INFO *rightClickAITypeInfo; // +A24. index=aitype. Used for (player+19C != 0). Data=0x55A238.
		long int rightClickAITypeInfoArraySize; // +A28. Actually, is the used number of AI types. Value=const from 0x55AFF8.
		STRUCT_AITYPE_RIGHTCLICK_INFO *basicRightClickAITypeInfo; // +A2C. index=aitype. Interactions that are ALWYAS available (no matter (if)which unit is selected). Data=0x55A918.
		long int basicRightClickAITypeInfoArraySize; // +A30. Actually, is the used number of AI types. Value=const from 0x55AFFC.
		long int unknown_A34[9]; // Indexed by playerId. Value = an int ?
		char unknown_A58[0xA7C - 0xA58];
		unsigned long int mapSizeChoice; // +A7C
		// 0xA80
		AOE_CONST_FUNC::MAP_TYPE_INDEX mapTypeChoice; // 0=small islands, 1=large islands... See empires.dat
		unsigned long int unknownA84; // +A84. set in 505340. Default=1. Unused in SP RM ?
		unsigned long int unknownA88; // +A88. set in 505360. Default=1. Unused in SP RM ?
		unsigned long int victoryConditionChoice; // 0=standard, 1=conquest, 7=time limit, 8=score
		// 0xA90
		unsigned long int victoryConditionChoice_parameter; // or float ?
		unsigned long int unknownA94; // Bytes for chosen civs ?
		unsigned long int unknownA98;
		unsigned long int unknownA9C;
		// 0xAA0
		char unknown_AA0[0x20]; // dwords index=0-7. Limits player # ? 8 Players: [0,1,2,3,4,5,6,7] 4 Players: [0,1,2,3,0,1,2,3] 2 Players: [0,1,0,1,0,1,0,1]
		// 0XAC0
		unsigned long int unknownAC0;
		char chosenPlayerNum[8]; // Corresponds to the colored number in game settings screen
		char unknown_ACC;
		// 0xACD
		char playerNameCivDLLID_Offset[8]; // per player. 4400+x=language_dll_id of a civ (player) name.
		// 0xAD5
		char unknown_AD5;
		char unknown_AD6; // +AD6. Set in 505520.
		char unknown_AD7; // +AD7. Set in 505530.
		char fixedPositions; // +AD8. Set in 505540
		char allTechs; // +AD9. Set in 505550
		char unknown_ADA;
		char unknown_ADB;
		unsigned long int initialResourcesChoice; // +ADC. 0=default, 3=large. Set in 505560.
		// 0xAE0
		AOE_CONST_INTERNAL::GAME_INITIAL_AGES initialAgeChoice; // 0=default, 1=nomad, 2=stone, 3=tool 4=bronze, 5=iron, 6=post-iron. Set in 505570
		char unknown_AE4; // +AE4. Set in 505580
		char isDeathMatch; // +AE5. Set in 505590
		char maxPopulation; // +AE6. For MP games. Set in 5055A0, Get in 5054F0. [+A9E in aoe10b]
		char unknown_AE7[0x09];
		// 0xAF0
		char unknown_AF0[0xC20 - 0xAF0];
		// 0xC20
		unsigned long int unknown_C20;
		STRUCT_UI_IN_GAME_MAIN *ptrGameUIStruct; // +C24
		long int lastEventPositionsY[5]; // +C28. Used when pressing HOME key
		// 0xC3C
		long int lastEventPositionsX[5]; // +C3C. Used when pressing HOME key
		// 0xC50
		long int lastEventIndex; // +C50. -1=no "last event". Or a 0-4 value
		long int lastAccessedEventIndex; // +C54. -1=no "last event". Or a 0-4 value. To remember which index was used in last "home key" press.
		// 0xC58
		char unknown_C58[0x106C - 0xC58];
		char debugString[0x1190 - 0x106C]; // Unknown_ size
		// 0x1190
		char isInitMPScreen; // +1190. Set in 5055B0. ??? to confirm. is MP game ?
		char unknown_1191;
		char unknown_1192;
		char unknown_1193;
		char unknown_1194; // default -1 ?
		char unknown_1195;
		char unknown_1196;
		char unknown_1197;
		char civPlayerNameIsUsed[0x08 * 0x0A]; // +1198. 8=number of players, 0x0A=number of names (0=unused, 1-9=used)
		char unknown_11E8[0x1254 - 0x11E8];

		bool IsCheckSumValid() const {
			return (this->checksum == CHECKSUM_GAME_SETTINGS1) || (this->checksum == CHECKSUM_GAME_SETTINGS2);
		}
		// Add an event position in history (for HOME key)
		void AddEventInHistory(long int posX, long int posY) {
			const unsigned long int addr = 0x5064D0;
			_asm {
				PUSH posX;
				PUSH posY;
				MOV ECX, this;
				CALL addr;
			}
		}
	};
	//GAMEVERSION_AOE10c : size 0x11A0
#if defined(GAMEVERSION_ROR10b) || defined(GAMEVERSION_ROR10c)
	static_assert(sizeof(STRUCT_GAME_SETTINGS) == 0x1254, "STRUCT_GAME_SETTINGS size");
#endif


}
