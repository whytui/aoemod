
#pragma once

/* This file contains various constants that are used in game EXE
*/

namespace AOE_CONST_INTERNAL
{
	enum GAME_SETTINGS_UI_STATUS : long int {
		GSUS_SOME_GAME_MENUS = 0, // load game, save game, "scenario" screen ?...
		GSUS_UNKNOWN_1_PLAY_CINEMATICS = 1,
		GSUS_MENU_OR_LOADING_NEW_GAME = 2,
		GSUS_LOADING_MP = 3,
		GSUS_PLAYING = 4, // For both MP / SP
		GSUS_GAME_OVER_BUT_STILL_IN_GAME = 5, //004F8110, 004849FE: you win or in MP: resigned, defeated
		GSUS_UNKNOWN_6 = 6, // ingame too ? In 50015B, treated as 4 (searches for play zone UI object). Used in 0x4850CE (if set, do commands before showing menu)
		GSUS_IN_EDITOR = 7
	};


	enum MOUSE_BUTTON_ACTION : long int {
		CST_MBA_RELEASE_CLICK = 0x28, // 40
		CST_MBA_LEFT_CLICK = 0x29, // 41
		CST_MBA_RIGHT_CLICK = 0x2A // 42
	};

	// Defines mouse behaviour (when left/right/double clicking, moving...) in playing zone.
	enum MOUSE_ACTION_TYPES : long int {
		CST_MAT_NORMAL = 0,
		CST_MAT_UNKNOWN_01 = 1,
		CST_MAT_UNKNOWN_02_SELECT_ZONE = 2, // Used to define special victory conditions ?
		CST_MAT_UNKNOWN_03_SELECT_ZONE = 3, // Used to define special victory conditions ?
		CST_MAT_UNKNOWN_04 = 4,
		CST_MAT_UNKNOWN_05 = 5,
		CST_MAT_VILLAGER_BUILD_MENU = 6,
		CST_MAT_VILLAGER_SET_BUILDING_LOCATION = 7,
		CST_MAT_EDITOR_SET_UNIT_LOCATION = 8, // create units, including buildings (fully built !)
		CST_MAT_SET_TERRAIN_TYPE = 9,
		CST_MAT_SET_ALTITUDE = 0x0A,
		CST_MAT_EDITOR_DELETE_UNIT = 0x0E, // Delete unit in scenario editor
		CST_MAT_EDITOR_MOVE_UNIT = 0x0F,
		CST_MAT_EDITOR_SELECT_TARGET_UNIT = 0x10, // For victory conditions
		// 0x10 : selects unit then restored to 0 ?
		CST_MAT_UNKNOWN_11_SELECT_ZONE = 0x11,
		CST_MAT_UNKNOWN_12_SELECT_UNIT_CANT_UNSELECT = 0x12, // can still unselect using ESC, but clicking on nothing does not unselect.
		CST_MAT_SET_CLIFFS = 0x13,
		CST_MAT_EDITOR_CHANGE_ORIENTATION = 0x14,
		CST_MAT_SET_WALLS = 0x15, // For both editor or villagers (in-game). Works also for other units ! (create series of aligned units)
		CST_MAT_CLICK_ZONE_TO_ATTACK = 0x64,
		CST_MAT_SELECT_CONVERT_TARGET = 0x65,
		CST_MAT_SELECT_HEAL_TARGET = 0x66,
		CST_MAT_SELECT_UNKNOWN_67 = 0x67, // Related to VILLAGER_SET_BUILDING_LOCATION?
		CST_MAT_SELECT_REPAIR_TARGET = 0x68,
		CST_MAT_SELECT_UNLOAD_TRANSPORT = 0x69,

		// Custom mouse action types
		// Avoid using -1, just in case
		CST_MAT_CR_PROTECT_UNIT_OR_ZONE = -2
	};


	// IDs for unit-command buttons under game zone. See 0x481445 switch for execution.
	enum INGAME_UI_COMMAND_ID : long int {
		CST_IUC_WORK = 1, // And this works ! Hotkey=W. No suitable icon though :(
		CST_IUC_MOVE = 2, // And this works ! Hotkey=M. And even has a suitable icon (id=1)
		CST_IUC_BUILD = 3,
		CST_IUC_EXCHANGE = 4, // WTF !! unused (no effect).
		CST_IUC_STOP = 5, // iconId=3 (hand)
		CST_IUC_CANCEL_SELECTION = 6, // icon = 0xA. Unselect unit
		CST_IUC_UNLOAD_TRANSPORT = 7, // icon 5
		CST_IUC_REGROUP = 8, // icon 7. cf 0x48260A.
		CST_IUC_UNGROUP = 9, // cf icon 8(slp 50721). Ungroup.
		CST_IUC_FORMATION = 0x0A, // what is this?
		CST_IUC_CANCEL_OR_BACK = 0x0B, // ESC key to cancel command or sub-menu(build, repair...)). icon 0x0A (always?)
		CST_IUC_NEXT_PAGE = 0x0C, // in build menu. Effect in 0x485140.
		CST_IUC_CHAT = 0x0D,
		CST_IUC_DIPLOMACY = 0x0E,
		CST_IUC_MENU = 0x0F, // Actually opens the menu
		CST_IUC_TRADE_WITH = 0x10,
		CST_IUC_DO_RESEARCH = 0x11, // see 0x483184
		CST_IUC_DO_TRAIN = 0x12,
		CST_IUC_DO_BUILD = 0x13,
		CST_IUC_CANCEL_BUILD = 0x14, // (click to cancel pending action). Icon 3.
		CST_IUC_SHOW_HELP = 0x15,
		CST_IUC_HOLD_POSITION = 0x16,
		CST_IUC_ATTACK_POSITION = 0x17, // icon 0x0C
		CST_IUC_SHOW_SCORES = 0x18, // 47DCF9.
		CST_IUC_TRADE_FOOD_FOR_GOLD = 0x19,
		CST_IUC_TRADE_WOOD_FOR_GOLD = 0x1A,
		CST_IUC_TRADE_STONE_FOR_GOLD = 0x1B,
		CST_IUC_HEAL = 0x1C, // icon 0xD
		CST_IUC_CONVERT = 0x1D,
		CST_IUC_ATTACK = 0x1E,
		CST_IUC_REPAIR = 0x1F,
		CST_IUC_ADD_TO_QUEUE = 0x20,
		CST_IUC_DROP_FROM_QUEUE = 0x21,
		// From here: custom IDs (not supported in standard code)
		CST_IUC_CROR_DONT_ATTACK_VILLAGERS = 100, // Do not auto-attack villagers
		CST_IUC_CROR_DONT_ATTACK_BUILDINGS, // Do not auto-attack non-tower buildings
		CST_IUC_CROR_NO_AUTO_ATTACK, // Do not auto-attack any unit
		CST_IUC_CROR_RESET_AUTO_ATTACK, // Restore auto-attack behaviour
		CST_IUC_CROR_DEFEND, // Protect a unit/zone
		CST_IUC_CROR_DEFEND_STOP, // STOP protecting a unit/zone
		CST_IUC_CROR_GATE_OPEN_CLOSE // Switch gate status open/closed
	};


	// See also 481445 switch
	enum GAME_SCREEN_BUTTON_IDS {
		// Standard game button IDs
		CST_GSBI_ANY_OTHER = 0, // all diplomacy btn, exit popup buttons...
		CST_GSBI_MENU_SAVE = 1,
		CST_GSBI_MENU_LOAD = 3,
		CST_GSBI_MENU_QUIT = 4,
		CST_GSBI_MENU_OPTIONS = 5,
		CST_GSBI_BUTTONBAR_STOP_HAND = 5,
		CST_GSBI_MENU_CANCEL = 6,
		CST_GSBI_BUTTONBAR_CANCEL = 6,
		CST_GSBI_MENU_ACHIEVEMENTS = 7,
		CST_GSBI_MENU_HELP = 8,
		CST_GSBI_BUTTONBAR_REGROUP = 8,
		CST_GSBI_MENU_SCENARIO = 9,
		CST_GSBI_BUTTONBAR_UNGROUP = 9,
		CST_GSBI_MENU_ABOUT = 0x0A,
		CST_GSBI_MENU_RESTART = 0x0B,
		CST_GSBI_MENU_RESIGN = 0x0C,
		CST_GSBI_DIPLOMACY = 0x0E,
		CST_GSBI_MENU = 0x0F,
		CST_GSBI_BUTTONBAR_RESEARCH_TECH = 0x11,
		CST_GSBI_BUTTONBAR_TRAIN_UNIT = 0x12,
		CST_GSBI_BUTTONBAR_CONTEXT_HELP = 0x15, // ? button in bottom-right corner
		CST_GSBI_BUTTONBAR_HOLD_POSITION = 0x16,
		CST_GSBI_BUTTONBAR_ATTACK_POSITION = 0x17,
		CST_GSBI_BUTTONBAR_SHOW_SCORES = 0x18,
		CST_GSBI_BUTTONBAR_HEAL = 0x1C,
		CST_GSBI_BUTTONBAR_CONVERT = 0x1D,
		// Custom button IDs
		CST_GSBI_CUSTOM_OPTIONS = 0x20
	};

	// Button IDs in dialog messages. Also used for UI components events
	enum DIALOG_BUTTON_IDS {
		CST_DBI_OK = 0,
		CST_DBI_YES = 0,
		CST_DBI_VALIDATE = 0, // for text areas - to confirm, just guessed
		CST_DBI_NO = 1,
		CST_DBI_LOSE_FOCUS = 1, // for text areas ?
		CST_DBI_CANCEL = 2,
		CST_DBI_EXIT_DROPDOWN = 2 // It's also a cancel operation
	};

	static const char customDialogScreenName[] = "CloseProgramDialog"; // This dialog is overriden for our custom use.


	// Mouse cursors ID. Cf SLPID 51000.
	enum GAME_CURSOR {
		GC_NORMAL = 0,
		GC_WAIT = 1,
		GC_GROUP = 2, // is it used ?
		GC_HAND = 3, // Gather, repair, build...
		GC_ATTACK = 4, // Sword cursor
		GC_CONVERT_HEAL_CAPTURE = 5, // For priests + capture artefacts + enter transport
		GC_GET_HELP = 6, // Pointer + question mark (when clicking on "?" to get context help)
		GC_COUNT
	};
}
