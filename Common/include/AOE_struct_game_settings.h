
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
	class STRUCT_ANY_UI;
	class STRUCT_UI_DRAW_SYSTEM;
	class STRUCT_UI_DRAW_AREA;

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

	// Size=0x254. Constructor=0x4496E0. See variable AOE_VAR_CURSORS_INFO
	// "TMousePointer" ?
	class STRUCT_CURSOR_SLP_INFO
	{
	public:
		long int unknown000;
		unsigned long int *unknown004;
		unsigned long int *unknown008;
		unsigned long int *unknown00C;
		long int shapeCount; // +10
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
		char unknown_040[0x5C - 0x40];
		STRUCT_SLP_INFO *currentSlpInfo; // +5C
		long int unknown_060;
		long int unknown_064;
		char unknown_068[0x8C - 0x68];
		long int currentCursorId; // +8C : mouse cursor id
		char unknown_090[0x10C - 0x90];
		long int gameWindowLeft; // +10C
		long int gameWindowTop; // +110
		long int gameWindowRight; // +114
		long int gameWindowBottom; // +118
		char unknown_11C[0x138 - 0x11C];
		long int gameMode; // +138
		long int gameEnable; // +13C. Set in 0x449B00
		unsigned long int unknown_140;
		unsigned long int unknown_144;
		unsigned long int unknown_148;
		char currentSlpName[0x104]; // +14C : "mcursors.slp"
		long int currentSlpCount; // +250
	};
	static_assert(sizeof(STRUCT_CURSOR_SLP_INFO) == 0x254, "STRUCT_CURSOR_SLP_INFO size");


	// Size=0x110. Constructor=0x46AD60.
	// There are 2 instances: 1 pointed by 0x580DC0 and 1 pointed by game settings.
	class STRUCT_TREGISTRY {
	public:
		unsigned long int unknown_00;
		// TODO
	};

	// Size=0x7B0. Constructor=0x42BF80.
	class STRUCT_TDEBUGGING_LOG {
		unsigned long int unknown_00;
		// TODO
	};

	// Size=0x100. Constructor=0x41C830
	// For chatting (with taunts...)
	class STRUCT_TCHAT {
		STRUCT_SOUND_DRIVER *pSoundDriver; // +00. TO CONFIRM
		unsigned long int unknown_04[9]; // +04. Set in 0x41CA50. Indexes 0-9 (0 is accepted in 'get' in 0x41CA70).
		unsigned long int unknown_28;
		unsigned long int unknown_2C[0x33]; // +2C. 51 slots for taunts ?
		unsigned long int unknown_F8; // +F8.
		unsigned long int unknown_FC; // Struct, size=0x198
	};
	static_assert(sizeof(STRUCT_TCHAT) == 0x100, "STRUCT_TCHAT size");


	// Not sure this really is a structure. Seems so ? Cf 0x417310.
	class STRUCT_GAME_SETTINGS_TIMINGS_UI_INFO {
	public:
		unsigned long int accumulatedTime;
		unsigned long int lastTime; // +04.
		unsigned long int unknown_08;
		unsigned long int unknown_0C;
		unsigned long int unknown_10;
		unsigned long int unknown_14; // "last single time" ?
		unsigned long int maxTime;
		unsigned long int lastMaxTime; // +1C.
	};
	static_assert(sizeof(STRUCT_GAME_SETTINGS_TIMINGS_UI_INFO) == 0x20, "STRUCT_GAME_SETTINGS_TIMINGS_UI_INFO size");


#ifdef GAMEVERSION_AOE10b
#define CHECKSUM_GAME_SETTINGS 0x005509D8
#define CHECKSUM_GAME_SETTINGS_BASE 0x00553B78 // Base class
#endif
#ifdef GAMEVERSION_AOE10c
#define CHECKSUM_GAME_SETTINGS 0x0054301C
#define CHECKSUM_GAME_SETTINGS_BASE 0x0053C0A4 // Base class
#endif
#ifdef GAMEVERSION_ROR10b
#define CHECKSUM_GAME_SETTINGS 0x0054F6E0
#define CHECKSUM_GAME_SETTINGS_BASE 0x00547DE8 // Base class
#endif
#ifdef GAMEVERSION_ROR10c
#define CHECKSUM_GAME_SETTINGS 0x0054A264 // "Tribe Game"
#define CHECKSUM_GAME_SETTINGS_BASE 0x005430B4 // "RGE_Base_Game". Base class. Constructor=0x4162B0
#endif
	// Size = 0x11A0 (AOE1.0b&c). Constructor 0x4F91C0 (empires.exe). "RGE_Base_Game"
	// Size = 0x1254. Constructor 0x5004C0(1.0b), 0x4FDFA0(1.0c) - arg1=commandLineInfo, arg2=?
	// Warning: this mainly represents information from UI, many fields are NOT RELEVANT (and arbitratry) when loading a saved game.
	// +0x00 = gameSettings.destructor(do_free)
	// +0x04 = gameSettings.run()
	// +0x08 = gameSettings.HandleMessage(arg1, arg2, WM_xxx, arg4, arg5) "wndProc"
	// +0x0C = gameSettings.setProgMode(int) = setScreenStatus
	// +0x10 = gameSettings.setGameMode(int, int) = setMouseStatus
	// +0x14 = gameSettings.setControlledPlayer(playerId)
	// +0x18 = gameSettings.getErrorCode()
	// +0x1C = gameSettings.getString(int, long, char* int)
	// +0x20 = gameSettings.getString(long,char *,int)
	// +0x24 = gameSettings.LoadStringLanguage(stringID)
	// +0x28 = gameSettings.getString2(int, long, long, char*, int)
	// +0x2C = gameSettings.GetGameZoneUIObj() = gameSettings.getViewPanel
	// +0x30 = gameSettings.GetDiamondMapObj()
	// +0x34 = gameSettings.GetNewScenarioHeader(scenarioInfo)
	// +0x38 = gameSettings.GetNewScenarioHeader(internalFileId)
	// +0x3C = gameSettings.GetNewScenarioInfo(internalFileId)
	// +0x40 = gameSettings.NotifyEvent(eventId, playerId, DATID, posY, posX). (0x501980)
	// +0x44 = gameSettings.ResetComm()
	// +0x48 = gameSettings.SendGameOptions()
	// +0x4C = gameSettings.ReceiveGameOptions()
	// +0x54 = gameSettings.DoCheatCode(playerId, text)
	// +0x58 = gameSettings.InitializeMusic() ?
	// +0x5C = gameSettings.ShutdownMusic() ?
	// +0x68 = gameSettings.setup()
	// +0x6C = gameSettings.setupCmdLineOptions()
	// +0x70 = gameSettings.setupClass()
	// +0x74 = gameSettings.setupMainWindow()
	// +0x78 = gameSettings.setupGraphicsSystem()?
	// +0x7C = gameSettings.setupPalette()
	// +0x80 = gameSettings.setupMouse()
	// +0x84 = gameSettings.setupRegistry()
	// +0x88 = gameSettings.SetupDebugLogs()
	// +0x8C = gameSettings.setupChat()
	// +0x90 = gameSettings.setupComm()
	// +0x94 = gameSettings.setupSoundSystem()
	// +0x98 = gameSettings.setupFonts()
	// +0x9C = gameSettings.setupSounds()
	// +0xA0 = gameSettings.setupSLP??()
	// +0xA4 = gameSettings.setupBlankScreen()
	// +0xA8 = gameSettings.setupTimings()
	// +0xAC = gameSettings.stopSoundSystem()
	// +0xB0 = gameSettings.restartSoundSystem()
	// +0xB4 = gameSettings.GetMusicStatus(...) ? Or setupMusic?
	// +0xB8 = gameSettings.PlayMusic() ?
	// +0xBC = gameSettings.createGlobal()
	// +0xC0 = int RGE_Base_Game::handle_message(struct tagMSG *)
	// +0xC4 = int RGE_Base_Game::handle_idle(void)
	// +0xC8 = gameSettings.handleMouseMove?(parg1, uint, uint, long)
	// +0xCC = gameSettings.OnKeyDown(parg1, eventID?, key, arg4)
	// +0xD0 = gameSettings.handle_user_command(void *,unsigned int,unsigned int,long)
	// +0xD4 = gameSettings.handleCommand(void*, uint, uint, long)
	// +0xD8 = gameSettings.handleMusicDone(void*, uint, uint, long)
	// +0xDC = gameSettings.handlePaint(void*, uint, uint, long)
	// +0xE0 = gameSettings.handleActivate()
	// +0xE4 = gameSettings.handleInitMenu()
	// +0xE8 = gameSettings.handleExitMenu()
	// +0xEC = gameSettings.handleSize(parg1, uint, uint, long)
	// +0xF0 = gameSettings.handlePaletteChanged(parg1, uint, uint, long)
	// +0xF4 = gameSettings.handleQueryNewPalette(parg1, uint, uint, long)
	// +0xF8 = gameSettings.HandleClose?(arg1, WM_xx, arg3, arg4)
	// +0xFC = gameSettings.HandleDestroy(void*, arg2, arg3, arg4)
	// +0x100 = gameSettings.actionUpdate()? Does nothing?
	// +0x104 = int RGE_Base_Game::action_mouse_move(long,long,int,int,int,int)
	// +0x108 = int RGE_Base_Game::action_key_down(unsigned long,int,int,int,int)
	// +0x10C = int RGE_Base_Game::action_user_command(unsigned long,unsigned long)
	// +0x110 = int RGE_Base_Game::action_command(unsigned long,unsigned long)
	// +0x114 = int RGE_Base_Game::action_music_done(void)
	// +0x118 = int RGE_Base_Game::action_activate(void)
	// +0x11C = int RGE_Base_Game::action_deactivate(void)
	// +0x120 = int RGE_Base_Game::action_init_menu(void)
	// +0x124 = int RGE_Base_Game::action_exit_menu(void)
	// +0x128 = int RGE_Base_Game::action_size(void)
	// +0x12C = gameSettings.action_close(void)
	// +0x130 = gameSettings.calcTimings()
	// +0x134 = gameSettings.calcTimingsText()
	// +0x138 = gameSettings.showTimings(). Called from 0x41AA5D from gameSettings.handlePaint(void*, uint, uint, long). Called when showDebugTimings=1.
	// +0x13C = gameSettings.showComm(). Can be called to display MP comm info in bottom-left in colored chat-like !
	// +0x140 = gameSettings.showAI(). Can be called to display AI info in bottom-left in colored chat-like !
	// +0x144 = 
	// +0x148 = [Last for both classes]
	class STRUCT_GAME_SETTINGS {
	public:
		enum GAME_UNITS_OUTLINE_MODE : char {
			OUTLINE_RECTANGLE = 0, // Draws a white rectangle around selected unit. Quite good for scenario editor ! Fishes are always "outlined" in scEditor.
			OUTLINE_NONE = 1, // No visible effect on selected unit
			OUTLINE_NORMAL = 2, // White square under selected unit
			OUTLINE_EXTENDED = 3 // Normal + show a light square under all (most) units, cf CTRL-A in scenario editor
		};

		unsigned long int checksum;
		GAME_NFO_DATA *game_nfo_data; // +04. no checksum
		STRUCT_SCENARIO_FILE_INFO *scenarioInfData; // +08. scenario.inf path... +other data?
		STRUCT_COMMAND_LINE_INFO *commandLineInfo;
		// 0x10
		unsigned long int unknown_010_hwnd; // +10. Some hwnd.
		unsigned long int unknown_014;
		unsigned long int unknown_018;
		unsigned long int unknown_01C;
		unsigned long int unknown_020;
		unsigned long int unknown_024;
		unsigned long int unknown_028;
		unsigned long int scenarioMapSeed; // +2C. When supplied manually through scenario editor. -1 for RM / DM
		unsigned long int unknown_030;
		unsigned long int actualMapSeed; //+34.  THE actual map seed
		unsigned long int unknown_038;
		unsigned long int unknown_03C;
		// 0x40
		unsigned long int errorCode; // +40. Get in 0x417460.
		unsigned long int unknown_044;
		STRUCT_UI_DRAW_SYSTEM *drawSystem; // +48. TDrawSystem. Init 0x41829B.
		STRUCT_UI_DRAW_AREA *renderArea; // +4C. basegame ?? Ptr to struct size>=478. Corresponds to TDrawSystem+0x0C
		GAME_UNITS_OUTLINE_MODE outline; // +50. Values 0-3. to test
		char unknown_051[3];
		unsigned long int unknown_054;
		long int infoSLPCount; // +58. Number of elements in ptrInfosSLP array.
		STRUCT_SLP_INFO **ptrInfosSLP; // +5C. Pointer to array slpInfo, default size=3. index 0=shortcut numbers, 1=MoveStepFlag, 2=move red cross, see AoeInGameFlagsIconId
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
		long int stopIfSyncFail; // +190. Unsure.
		long int dropPacketsIntentionally; // +194. Unsure. Not sure it does have an effect in final versions ?
		long int showSyncChatMessages; // 198. Unsure.
		long int enableStepMode; // +19C. Unsure. Not sure it does have an effect in final versions ?
		long int enableSpeedControl; // +1A0. Unsure.
		STRUCT_TDEBUGGING_LOG *appLogger; // +1A4.
		unsigned long int unknown_1A8; // +1A8. Related to logger.
		STRUCT_TREGISTRY *unknown_1AC_registry; // related to registry ?
		// 0x1B0
		AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS currentUIStatus; // 0=loading,2=inMenu,4=playing,7=editor "programMode".
		AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES mouseActionType; // +1B4. "Game mode". Set this during game, you'll be able to edit the map !! userSelected* fields
		long int gameModeSub; // +1B8. Values 1,4,6,... ? SubMouseActionType ? Used when mouseActionType=6 (villager build menu) or in editor. Seen 1=editor_moveUnit?, 3, 4(editor_putunit?). Unsure !
		long int unknown_1BC;
		// 0x1C0
		STRUCT_CURSOR_SLP_INFO *mousePointer; // +1C0. Pointer to something about mouse cursors ? Same as *(0x582EDC). "mouseFacet".
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
		long int unknown_3F8_renderAll; // See 0x41C290
		short int editorUserSelectedUnitDefId; // +3FC: the unitDefID that user selected in "units" tab.
		short int editorUserSelectedTerrainId; // Can be changed in editor/terrain tab
		// 0x400
		short int editorUserSelectedAltitude; // in editor/terrain tab
		short int editorTerrainPencilSize; // +402. in editor/terrain tab
		char debugStringTimings[0x508 - 0x404]; // +404. Timings text. "Avg view time"... See 0x41B390.
		// +4F0 : pointer ? +100=Affects DisableMapSizeSetting
		long int worldUpdateCount; // +508.
		unsigned long int viewUpdateCount; // +50C.
		unsigned long int unknown_510;
		long int lastWorldUpdateCount; // +514
		long int lastViewUpdateCount; // +518
		unsigned long int unknown_51C;
		unsigned long int unknown_520; // first value in "t%lu,f%lu,max(r(v%lu,m%lu,o%lu),s%lu,u%lu,c%lu,ls%lu,lg%lu,p%lu,o%lu)"
		unsigned long int unknown_524; // second value in "t%lu,f%lu,max(r(v%lu,m%lu,o%lu),s%lu,u%lu,c%lu,ls%lu,lg%lu,p%lu,o%lu)"
		// +528. elemSize=0x20; getter=417430. See 4190B0=setupTimings. Not really an array (in fact, indexes correspond to specific objects and are hardcoded)
		// Index correspondance in debug strings: 0=? 1=u%lu 2=? 3=? 4=o%lu(last) 5=? 6=v%lu 7=m%lu 8=? 9=? 10=ls%lu 11=? 12=lg%lu 13=p%lu
		STRUCT_GAME_SETTINGS_TIMINGS_UI_INFO timingsInfo[16]; // actual number is unknown !!!
		char unknown_5A8[0x8E0 - 0x728]; // etc
		// 0x8E0
		long int screenSizeX; // To confirm + confirm type WRONG ?
		long int screenSizeY; // To confirm + confirm type WRONG ?
		long int showDebugTimings; // +8E8. Show debug timings instead of resource icon/values in top-left section. See also AOE_VAR_F5_DEBUG_INFO_TYPE.
		unsigned long int unknown_8EC;
		unsigned long int unknown_8F0;
		unsigned long int unknown_8F4;
		STRUCT_RGE_GAME_OPTIONS rgeGameOptions; // +8A8. Size=0xA8. Cf 0x41B430.
		// 0x9A0
		long int isCampaign; // get in 41B720, set in 41BA50
		long int isSavedGame; // +9A4. set in 41BA60
		long int hasSteroidsCheatCode; // +9A8. set in 50CA2B, 4E90B6. Can be cancelled !!! Used in 0x4B1AA2 for construction.
		long int has_CD; //see 4169D7
		// 0x9B0
		long int chosenPlayerIndex[9]; // index in 0-8 (playerId, including gaia)
		// to 9D4...
		unsigned long int unknown_9D4;
		long int playerWondersVictoryDelays[9]; // +9D8. index in 0-8 (playerId, including gaia). Remaining "years" for each player's wonder victory trigger. See 0x41C100. If a wonder is destroyed but player has still another one, the counter is NOT reset. -1=not relevant.
		// to 9FC...
		long int gameIsRunning; // +9FC. You can simply set it to 0 to pause game.
		// 0xA00
		long int unknown_A00; // +A00. 1 if timer is stopped ?
		long int unknown_A04; // +A04. Related with timer status too ?
		unsigned long int unknown_A08;
		float gameSpeed; // +A0C. Get in 0x41C170.
		AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL difficultyLevelChoice; // +A10. Get in 41C190. Copied into +993=rgeGameOptions.
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
		long int unused_animals; // +A84. set in 0x505340. Default=1. Unused.
		long int unused_predators; // +A88. set in 0x505360. Default=1. Unused.
		long int victoryConditionChoice; // 0=standard, 1=conquest, 7=time limit, 8=score
		// 0xA90
		long int victoryConditionChoice_parameter; // or float ?
		unsigned char chosenCivs[9]; // +A94. Selected civ ID for each player (in game settings screen). Index is playerId-1. Set in 0x5053C0. See also 0x48D13E for random picking
		unsigned char unknownA9D[3]; // +A9D. unused
		// 0xAA0
		long int scenarioPlayerChoices[9]; // +AA0. Limits player # ? 8 Players: [0,1,2,3,4,5,6,7] 4 Players: [0,1,2,3,0,1,2,3] 2 Players: [0,1,0,1,0,1,0,1]
		char chosenPlayerColor[9]; // +AC4. Corresponds to the colored number in game settings screen
		char playerNameCivDLLID_Offset[9]; // +ACD. per player. 4400+x=language_dll_id of a civ (player) name.
		char unused_allowTrading; // +AD6. Set in 0x505520. Seems unused.
		char lengthenCombat; // +AD7. Set in 505530.
		char fixedPositions; // +AD8. Set in 505540. "RandomizePositions"
		char allTechs; // +AD9. Set in 0x505550. Used for new games. For other games, refer to scenarioInfo structure (fullTechTree value is fixed by RockNRor). "Used" in 0x50B833 to apply techtree=0xD2=210. Set to 1 when using "custom tech trees".
		char unknown_ADA;
		char unknown_ADB;
		long int initialResourcesChoice; // +ADC. 0=default, 3=large. Set in 505560. "ResourceLevel".
		// 0xAE0
		AOE_CONST_INTERNAL::GAME_INITIAL_AGES initialAgeChoice; // 0=default, 1=nomad, 2=stone, 3=tool 4=bronze, 5=iron, 6=post-iron. Set in 505570
		char unused_startingUnits; // +AE4. Set in 0x505580. Unused ?
		char isDeathMatch; // +AE5. Set in 0x505590. NOT correct when loading a saved game (but fixed by RockNRor)
		char maxPopulation; // +AE6. For MP games. Set in 0x5055A0, Get in 0x5054F0. [+A9E in aoe10b]
		char unknown_AE7;
		char unknown_scenarioAutoSaveFileName[0x104]; // +AE8. Used in 0x502A55
		// 0xBEC
		char unknown_BEC[0xC20 - 0xBEC];
		// 0xC20
		unsigned long int unknown_C20;
		STRUCT_UI_IN_GAME_MAIN *ptrGameUIStruct; // +C24.
		long int lastEventPositionsY[5]; // +C28. Used when pressing HOME key
		// 0xC3C
		long int lastEventPositionsX[5]; // +C3C. Used when pressing HOME key
		// 0xC50
		long int lastEventIndex; // +C50. -1=no "last event". Or a 0-4 value
		long int lastAccessedEventIndex; // +C54. -1=no "last event". Or a 0-4 value. To remember which index was used in last "home key" press.
		// 0xC58
		char unknown_C58[0xE64 - 0xC58];
		char saveGameName[0x104]; // +E64.
		char loadGameName[0x104]; // +F68.
		char debugString[0x1190 - 0x106C]; // +106C. Unknown size
		// 0x1190
		char isInitMPScreen; // +1190. Set in 0x5055B0. ??? to confirm. is MP game ? "quickStartGame"
		char unknown_1191;
		char unknown_1192;
		char unknown_1193;
		char unused_randomStartValue; // +1194. default -1. Does not seem to be used at all.
		char unknown_1195;
		char unknown_1196;
		char unknown_1197;
		char civPlayerNameIsUsed[0x08 * 0x0A]; // +1198. 8=number of players, 0x0A=number of names (0=unused, 1-9=used)
		char unknown_11E8[0x1254 - 0x11E8];

		bool IsCheckSumValid() const {
			return (this->checksum == CHECKSUM_GAME_SETTINGS) || (this->checksum == CHECKSUM_GAME_SETTINGS_BASE);
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
