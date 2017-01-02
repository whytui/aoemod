
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
	// Provides interaction mode information for an AI type (considering unit=potential target of a right-click)
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

	// Unknown size. See variable AOE_VAR_CURSORS_INFO
	class STRUCT_CURSOR_SLP_INFO
	{
	public:
		long int unknown000;
		unsigned long int *unknown004;
		unsigned long int *unknown008;
		unsigned long int *unknown00C;
		long int unknown010;
		long int unknown014;
		unsigned long int *unknown018;
		unsigned long int *unknown01C;
		unsigned long int *unknown020;
		unsigned long int *unknown024;
		unsigned long int *unknown028;
		unsigned long int *unknown02C;
		unsigned long int *unknown030;
		unsigned long int *unknown034;
		unsigned long int *unknown038;
		unsigned long int *unknown03C;
		char unknown_040[0x58 - 0x40];
		STRUCT_SLP_INFO *currentSlpInfo; // +5C
		char unknown_05C[0x8C - 0x5C];
		long int currentCursorId; // +8C : mouse cursor id
		char unknown_090[0x14C - 0x90];
		char currentSlpName[0x104]; // +14C : "mcursors.slp"
		long int currentSlpCount; // +250
	};


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
	// Warning: this mainly represents information from UI, many fields are NOT RELEVANT (and arbitratry) when loading a saved game.
	// [EDX+40] gameSettings.NotifyEvent(eventId, playerId, DATID, posY, posX). (0x501980)
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
		unsigned long int unknown_040;
		unsigned long int unknown_044;
		unsigned long int *unknown_048;
		unsigned long int *unknown_04C; // +4C. basegame ?? Ptr to struct size>=478
		char unknown_050[0xC];
		STRUCT_SLP_INFO **ptrInfosSLP; // +5C. Pointer to array slpInfo, size=3 ? index 0=shortcut numbers
		// 0x60
		STRUCT_SOUND_DRIVER *pSoundDriver; // +60. Size 69C, see 41894B. Generally =NULL, only set while being used.
		STRUCT_MAIN_MUSIC *pMusicStruct; // +64. Size 3F8, see 418B14. Generally =NULL, only set while being used.
		long int commonSoundDataArrayElemCount; // +68. Default 0x11 (see 4FEF2D).
		STRUCT_SOUND_TDIGITAL *ptrCommonSoundDataArray; // +6C. Elem size=0x3C, ccor=0x4A2BE0
		// 0x70
		char musicFormat; // 0=AudioCD, 1=MIDI
		char unknown_071;
		char unknown_072;
		char unknown_073;
		long int minMusicTrack;
		long int maxMusicTrack;
		long int currentMusicTrack;
		// 0x80
		char musicFilename[0x184 - 0x80]; // Check this...
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
		STRUCT_CURSOR_SLP_INFO *unknown_1C0; // +1C0. Pointer to something about mouse cursors ? Same as *(0x582EDC)
		char unknown_1C4[0xC];
		// 0x1D0
		unsigned long int unknown_1D0;
		long int hCursor; // Cursor handle, default=10003
		unsigned long int unknown_1D8;
		unsigned long int unknown_1DC;
		// 0x1E0
		long int fontsCount; // +1E0. 0x418E7B. Default=12.
		long int unknown_ptrFonts; // +1E4. Ptr to array, elemSize=0x0C, elemIndex = fontId
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
		float gameSpeed; // +8F8. A struct starts here ?
		char isScenario; // +8FC. 1 for scenario/campaign, 0 for random game/deathMatch. value is ARBITRARY for loaded games, unless we fix it manually (done by customROR)
		char scenarioFileName[3 + 0x7C]; // +8FD.
		// 0x900
		char unknown_97C;
		char isSinglePlayer; // 0x97D. Not correct AFTER player a MP game ? Set in 0x41BA70
		char isMultiplayer; // 0x97E. Set in 0x41BA90
		char mapSizeX;
		// 0x980
		char mapSizeY;
		char unknown_981_maxMapPlayerCount; // Related to map size. Always 8 ? Max player # ?
		char enableCheatMode; // +982. Set in 41BAE0
		char unknown_983; // +983. path finding for MP ? 0-2 default/medium/high ?
		char unknown_984; // Something to do with screen width and height?? annoyCheaters or cheatNotification ? Unsure
		char revealMap; // 0x985. Set in 41BB00
		char noFog; // 0x986. Set in 41BB10
		char unknown_987_coloredChat; // default 1 ? Set in 41BB20
		char playerCount; // +988. NOT counting gaia.
		char unknown_989_gameDeveloperOptionsEnabled; // can set speed > 16 ?
		char playerGameVersion[0x09]; // 0-127 ; 0="", 1="1.0", 2="1.0a", etc. Index is playerId
		AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL difficultyLevel; // +993. 0=hardest, 1=hard, 2, 3, 4=easiest. Set in 41BBC0
		char teamNumbers[0x09]; // +994. player 1-8 (array offset in 0-8). Values: 1=noTeam, >1=teamId.
		// 0x9A0
		long int isCampaign; // get in 41B720, set in 41BA50
		long int isSavedGame; // +9A4. set in 41BA60
		long int hasSteroidsCheatCode; // +9A8. set in 50CA2B, 4E90B6. Can be cancelled !!!
		long int has_CD; //see 4169D7
		// 0x9B0
		unsigned long int chosenPlayerIndex[9]; // index in 0-8 (playerId, including gaia)
		// to 9D4...
		unsigned long int unknown_9D4;
		long int playerWondersVictoryDelays[9]; // +9D8. index in 0-8 (playerId, including gaia). Remaining "years" for each player's wonder victory trigger. See 0x41C100. If a wonder is destroyed but player has still another one, the counter is NOT reset. -1=not relevant.
		// to 9FC...
		long int gameIsRunning; // +9FC. You can simply set it to 0 to pause game.
		// 0xA00
		long int unknown_A00; // +A00. 1 if timer is stopped ?
		long int unknown_A04; // +A04. Related with timer status too ?
		unsigned long int unknown_A08;
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
		STRUCT_AITYPE_RIGHTCLICK_INFO *forSelectionRightClickAITypeInfo; // +A24. index=aitype. Right-click info for current selection. Data=0x55A238.
		long int forSelectionRightClickAITypeInfoArraySize; // +A28. Actually, is the used number of AI types. Value=const from 0x55AFF8.
		STRUCT_AITYPE_RIGHTCLICK_INFO *basicRightClickAITypeInfo; // +A2C. index=aitype. Interactions that are ALWAYS available (no matter (if)which unit is selected). Data=0x55A918.
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
		unsigned char chosenCivs[9]; // +A94. Selected civ ID for each player (in game settings screen). Index is playerId-1. Set in 0x5053C0. See also 0x48D13E for random picking
		unsigned char unknownA9D[3]; // +A9D. unused
		// 0xAA0
		long int unknown_AA0[9]; // +AA0. Limits player # ? 8 Players: [0,1,2,3,4,5,6,7] 4 Players: [0,1,2,3,0,1,2,3] 2 Players: [0,1,0,1,0,1,0,1]
		char chosenPlayerNum[9]; // +AC4. Corresponds to the colored number in game settings screen
		// 0xACD
		char playerNameCivDLLID_Offset[9]; // per player. 4400+x=language_dll_id of a civ (player) name.
		char unknown_AD6; // +AD6. Set in 505520.
		char unknown_AD7; // +AD7. Set in 505530.
		char fixedPositions; // +AD8. Set in 505540
		char allTechs; // +AD9. Set in 505550
		char unknown_ADA;
		char unknown_ADB;
		long int initialResourcesChoice; // +ADC. 0=default, 3=large. Set in 505560.
		// 0xAE0
		AOE_CONST_INTERNAL::GAME_INITIAL_AGES initialAgeChoice; // 0=default, 1=nomad, 2=stone, 3=tool 4=bronze, 5=iron, 6=post-iron. Set in 505570
		char unknown_AE4; // +AE4. Set in 505580
		char isDeathMatch; // +AE5. Set in 505590. NOT correct when loading a saved game (but fixed by customROR)
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
		// withSelectedUnit: true to get info that applies for selected unit, false to get "basic" info (rules that always apply)
		// Returns NULL if not found
		STRUCT_AITYPE_RIGHTCLICK_INFO *GetRightClickInfoArrayBase(bool withSelectedUnit) {
			if (withSelectedUnit) {
				return this->forSelectionRightClickAITypeInfo;
			} else {
				return this->basicRightClickAITypeInfo;
			}
		}
		// Get right-click info for a given AI type.
		// withSelectedUnit: true to get info that applies for selected unit, false to get "basic" info (rules that always apply)
		// Returns NULL if not found
		STRUCT_AITYPE_RIGHTCLICK_INFO *GetRightClickInfo(GLOBAL_UNIT_AI_TYPES unitAIType, bool withSelectedUnit) {
			if (withSelectedUnit) {
				if ((unitAIType < 0) || (unitAIType >= this->forSelectionRightClickAITypeInfoArraySize)) {
					return NULL;
				}
			} else {
				if ((unitAIType < 0) || (unitAIType >= this->basicRightClickAITypeInfoArraySize)) {
					return NULL;
				}
			}
			STRUCT_AITYPE_RIGHTCLICK_INFO *rci = this->GetRightClickInfoArrayBase(withSelectedUnit);
			if (!rci) { return NULL; }
			return &rci[unitAIType];
		}
	};
	//GAMEVERSION_AOE10c : size 0x11A0
#if defined(GAMEVERSION_ROR10b) || defined(GAMEVERSION_ROR10c)
	static_assert(sizeof(STRUCT_GAME_SETTINGS) == 0x1254, "STRUCT_GAME_SETTINGS size");
#endif


}
