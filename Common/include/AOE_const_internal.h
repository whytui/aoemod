
#pragma once
#include <tchar.h>

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
		GSUS_UNKNOWN_6 = 6, // ingame too ? In 50015B, treated as 4 (searches for play zone UI object)
		GSUS_IN_EDITOR = 7
	};

	// Avoid confusion with diplomacy values (PLAYER_DIPLOMACY_VALUES) !
	enum PLAYER_DIPLOMACY_STANCES : char {
		CST_PDS_ALLY = 0, // For both "me" and my allies
		CST_PDS_NEUTRAL = 1,
		CST_PDS_UNUSED_2 = 2,
		CST_PDS_ENEMY = 3
	};

	// Avoid confusion with PLAYER_DIPLOMACY_STANCES !
	enum PLAYER_DIPLOMACY_VALUES : long int {
		CST_PDV_GAIA_SPECIAL = 0, // Gaia has 0 vs all players
		CST_PDV_SELF = 1,
		CST_PDV_ALLY = 2,
		CST_PDV_NEUTRAL = 3,
		CST_PDV_ENEMY = 4
	};

	enum GAME_INITIAL_AGES : long int {
		GIA_DEFAULT = 0,
		GIA_NOMAD = 1,
		GIA_STONE = 2,
		GIA_TOOL = 3,
		GIA_BRONZE = 4,
		GIA_IRON = 5,
		GIA_POST_IRON = 6
	};

	enum GAME_PLAYER_TYPES : long int {
		GPT_GAIA_OR_OTHER = 0,
		GPT_UNKNOWN_1 = 1,
		GPT_HUMAN_CONTROLLED = 2,
		GPT_UNKNOWN_3 = 3,
		GPT_COMPUTER_CONTROLLED = 4,
	};

	enum MOUSE_ACTION_TYPES : long int {
		CST_MAT_NORMAL = 0,
		CST_MAT_UNKNOWN_01 = 1,
		CST_MAT_UNKNOWN_02_SELECT_ZONE = 2, // Used to define special victory conditions ?
		CST_MAT_UNKNOWN_03_SELECT_ZONE = 3, // Used to define special victory conditions ?
		CST_MAT_UNKNOWN_04 = 4,
		CST_MAT_UNKNOWN_05 = 5,
		CST_MAT_VILLAGER_BUILD_MENU = 6,
		CST_MAT_VILLAGER_SET_BUILDING_LOCATION = 7,
		CST_MAT_EDITOR_SET_UNIT_LOCATION = 8,
		CST_MAT_SET_TERRAIN_TYPE = 9,
		CST_MAT_SET_ALTITUDE = 0x0A,
		CST_MAT_UNKNOWN_0E = 0x0E,
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
		CST_MAT_SELECT_UNKNOWN_67 = 0x67, // Some sub-action (in-game)
		CST_MAT_SELECT_REPAIR_TARGET = 0x68,
		CST_MAT_SELECT_UNKNOWN_69 = 0x69 // Some sub-action (in-game)
	};

	enum INTERNAL_MAIN_CATEGORIES : long int {
		CST_IMC_ERRORS = 1,
		CST_IMC_RESOURCE_NAMES = 0x64,
		CST_IMC_AGE_NAMES = 0x65,
		CST_IMC_UI_COMMANDS = 0x66,
		CST_IMC_IN_GAME_COMMAND_ERRORS = 0x67, // Yellow error message + sound
		CST_IMC_UNKNOWN_UNUSED = 0x68, // repair ?
		CST_IMC_CIV_NAMES = 0x69
	};

	enum INGAME_COMMAND_ERROR_ID : long int { // TO DO check type
		CST_ICE_NOT_ENOUGH_FOOD = 0,
		CST_ICE_NOT_ENOUGH_WOOD = 1,
		CST_ICE_NOT_ENOUGH_STONE = 2,
		CST_ICE_NOT_ENOUGH_GOLD = 3,
		CST_ICE_NOT_ENOUGH_HOUSES = 4,
		CST_ICE_REACHED_MAX_POPULATION = 0x20,
		CST_ICE_NO_DOCK_TO_TRADE_TO = 0x65,
		CST_ICE_TOO_FAR_FROM_LAND_TO_UNLOAD = 0x66,
		CST_ICE_NOT_ENOUGH_ROOM_TO_UNLOAD = 0x67,
		CST_ICE_RECEIVED_GOLD_TRIBUTE = 0x68,
		CST_ICE_CANT_TRADE_WITH_ENEMY = 0x69,
		CST_ICE_NOT_ENOUGH_RESOURCE_TO_REPAIR = 0x6A,
		CST_ICE_NOT_ENOUGH_SPACE_TO_CREATE_UNIT = 0x3E8
	};

	// IDs for unit-command buttons under game zone. See 481445 switch for execution.
	enum INGAME_UI_COMMAND_ID : long int {
		CST_IUC_WORK = 1, // And this works ! Hotkey=W. No suitable icon though :(
		CST_IUC_MOVE = 2, // And this works ! Hotkey=M. And even has a suitable icon (id=1)
		CST_IUC_BUILD = 3,
		CST_IUC_EXCHANGE = 4, // WTF !! unused (no effect).
		CST_IUC_STOP = 5, // iconId=3 (hand)
		CST_IUC_CANCEL_SELECTION = 6, // icon = 0xA. Unselect unit
		CST_IUC_UNLOAD_TRANSPORT = 7, // icon 5
		CST_IUC_REGROUP = 8, // icon 7. cf 48260A.
		CST_IUC_UNGROUP = 9, // cf icon 8(slp 50721). Ungroup
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
		CST_IUC_DROP_FROM_QUEUE = 0x21
	};

	enum GAME_EVENT_SOUNDS : long int {
		CST_GES_ERROR_IMPOSSIBLE = 3,
		CST_GES_RESEARCH_COMPLETE = 5,
		CST_GES_PLAYER_DEATH = 6, // Any player's death (not specific to user-controlled)
		CST_GES_UNKNOWN_7_DISCONNECTED = 7,
		CST_GES_WONDER_STARTED = 8,
		CST_GES_WONDER_FINISHED = 9,
		CST_GES_WONDER_DESTROYED = 0x0A,
		CST_GES_ALL_ARTEFACTS_CAPTURED = 0x0B, // Relics or ruins
		CST_GES_UNIT_HAS_BEEN_CONVERTED = 0x0D,
		CST_GES_AGE_RESEARCHED = 0x0F,
		CST_GES_FARM_DEPLETED = 0x10
	};

	enum GAME_EVENT_TYPES : long int {
		CST_GET_INVALID = -1, // For customROR internal usage
		CST_GET_CANT_UNLOAD_NO_ROOM = 01,
		CST_GET_CANT_UNLOAD_TOO_FAR = 02,
		CST_GET_CANT_TRADE_WITH_ENEMY = 03,
		CST_GET_UNKNOWN_04_PLAYER_DISCONNECTED = 04,
		CST_GET_UNKNOWN_05_PLAYER_DISCONNECTED_ABANDONED = 05,
		CST_GET_PLAYER_LOST = 06,
		CST_GET_TRIBUTE_PAID = 07,
		CST_GET_DIPLOMACY_CHANGED = 8,
		CST_GET_RESEARCH_COMPLETE = 0x64,
		CST_GET_CANCELED_RESEARCH = 0x65,
		CST_GET_UNIT_SPAWNED = 0x66,
		CST_GET_CANCEL_TRAIN_UNIT = 0x67,
		CST_GET_CANNOT_SPAWN_NO_ROOM = 0x68,
		CST_GET_BUILDING_COMPLETE = 0x69,
		CST_GET_UNKNOWN_6A = 0x6A,
		CST_GET_CANNOT_REPAIR_NO_RESOURCES = 0x6B, // See also CST_GET_NOT_ENOUGH_RESOURCES for train/build missing resources errors
		CST_GET_WONDER_START_BUILDING = 0x6C,
		CST_GET_WONDER_FINISHED = 0x6D,
		CST_GET_WONDER_DESTROYED = 0x6E,
		CST_GET_ALL_RELICS_CAPTURED = 0x72,
		CST_GET_ALL_RELICS_LOST_CONTROL = 0x73,
		CST_GET_ALL_RUINS_CAPTURED = 0x74,
		CST_GET_ALL_RUINS_LOST_CONTROL = 0x75,
		CST_GET_CANNOT_CONVERT_NO_FAITH = 0x76,
		CST_GET_CANNOT_CONVERT_NEED_MONOTHEISM_PRIEST = 0x77,
		CST_GET_CANNOT_CONVERT_NEED_MONOTHEISM_BUILDING = 0x78,
		CST_GET_CANNOT_CONVERT_TC = 0x79,
		CST_GET_CANNOT_CONVERT_WONDER = 0x7A,
		CST_GET_SUCCESSFULLY_CONVERTED_UNIT = 0x7B,
		CST_GET_ADD_REMOVE_IN_TRAIN_QUEUE = 0x7C,
		CST_GET_NOT_ENOUGH_RESOURCES = 0x7D, // need houses, food/wood or maxPop reached (to build or train, NOT for repair)
		CST_GET_FARM_DEPLETED = 0x7E, // Sound 0x10
		CST_GET_UNKNOWN_1F9 = 0x1F9, // see 0x4F3531
		CST_GET_UNIT_ATTACKED = 0x201,
		CST_GET_MOVEMENT_FINISHED = 0x202, // unsure
		CST_GET_TRIBUTE_RECEIVED = 0x20A,
		CST_GET_SEE_UNIT = 0x20D
	};

	enum PLACEMENT_TYPES : long int {
		CST_PT_STORAGE_PIT = 1,
		CST_PT_GRANARY = 2,
		CST_PT_TOWERS = 3, // Does NOT include ballista tower !
		CST_PT_WALLS = 4,
		CST_PT_DEFAULT = 5, // for spawn buildings like barracks, etc
		CST_PT_NON_SPAWN_BUILDNGS = 6,
		CST_PT_HOUSE = 6,
		CST_PT_GOV_CENTER = 6,
		CST_PT_MARKET = 6,
		CST_PT_TRADE_WORKSHOP = 6,
		CST_PT_WONDER = 6,
		CST_PT_DOCK = 7,
		CST_PT_FARM = 8,
		CST_PT_TOWN_CENTER = 9
	};

	enum ERROR_FOR_UNIT_CREATION : long int {
		CST_EUC_OK = 0,
		CST_EUC_UNKNOWN_1 = 1, // Related to terrain issue ?
		CST_EUC_BAD_TERRAIN_RESTRICTION = 2,
		CST_EUC_ELEVATION_ISSUE = 3,
		CST_EUC_UNUSED_4 = 4,
		CST_EUC_NOT_EXPLORED = 5,
		CST_EUC_CONFLICTING_WITH_UNIT = 6,
		CST_EUC_OUT_OF_MAP = 7, // Checked with higher priority
		CST_EUC_UNKNOWN = 8 // see 4B8D25
	};

	enum INTERNAL_MILITARY_UNIT_TYPES : long int {
		CST_IMUT_OTHER_OR_INVALID = -1, // includes buildings, etc
		CST_IMUT_CLUBMAN = 0,
		CST_IMUT_SWORDSMAN = 1,
		CST_IMUT_CAVALRY = 2, // Mounted soldier
		CST_IMUT_PHALANX = 3,
		CST_IMUT_SIEGE_WEAPON = 4,
		CST_IMUT_PRIEST = 5,
		CST_IMUT_BOWMAN_AND_HORSE_ARCHER = 6, // includes improved bowman + compie
		CST_IMUT_CHARIOT = 7,
		CST_IMUT_UNKNOWN_8, // for unitClasses >= 0x16=22 ? Really unsure
		CST_IMUT_ELEPHANTS = 9, // include elephant archer
		CST_IMUT_CHARIOT_ARCHER = 0x0A,
		CST_IMUT_COUNT = 0x0B // If this is wrong and you fix it, please fix also infAI struct definition
	};

	enum UNIT_STATUS : char {
		CST_US_NOT_BUILT = 0, // Relevant for buildings
		CST_US_UNKNOWN_1 = 1,
		CST_US_READY = 2, // Initial status for living units, normal status for buildings (once built)/living units
		// In terms of gameplay, statuses >2 are dead/dying units, but can be used for resources/technical units...
		// Units with status>2 have no action, no longer count in player resources, etc.
		CST_US_UNKNOWN_3 = 3, // used to trigger unit death (intermediate status ?)
		CST_US_BERRY_BUSH = 3,
		CST_US_CUT_TREE = 3, // A tree that is ready to be gathered
		CST_US_DYING_ANIMATION = 4, // Relevant for living units (including animals)
		CST_US_UNKNOWN_5 = 5, // frequent for smoke units ?
		CST_US_MINES = 5, // Gold mine, stone mine
		CST_US_GATHERABLE_ANIMAL, // Animal dead bodys that still provide meat.
		CST_US_UNKNOWN_6 = 6, // not used for living units. Not used at all ?
		CST_US_UNKNOWN_7 = 7, // Death, unit no longer produces "visibility". Unit is no longer displayed at all ?
		CST_US_TO_DESTROY = 8 // Unit structure plays no role, just waiting to be destroyed/freed ?
	};

	// For "action" structures. There are NOT values for "command" structures
	enum INTERNAL_ACTION_ID : short int {
		CST_IAI_MOVE_1 = 1, // Used as sub-action to move to target ? Used to capture relics
		CST_IAI_FOLLOW = 2, // TO CONFIRM (AGE3)
		CST_IAI_GARRISON_ENTER = 3, // Enter
		CST_IAI_EXPLORE_04 = 4, // TO CONFIRM
		CST_IAI_GATHER_NO_ATTACK = 5, // Gathering with NO attack phase. Excludes fishing !
		CST_IAI_UNKNOWN_06 = 6, // Natural wonders cheat??? attack or flee from (seen) unit ? Animal ability ?
		CST_IAI_UNKNOWN_7 = 7, // Attack ?? unsure
		CST_IAI_SHOOT = 8, // TO CONFIRM
		CST_IAI_ATTACK_9 = 9,
		CST_IAI_FLY = 0x0A, // TO CONFIRM
		CST_IAI_SCARE_HUNT = 0x0B, // TO CONFIRM.
		CST_IAI_TRANSPORT = 0x0C, // OR UNLOAD ???
		CST_IAI_GUARD = 0x0D, // TO CONFIRM or auto-attack ?
		CST_IAI_EXPLORE_UNSURE_1E = 0x1E, // unsure... see also 4 ?
		CST_IAI_UNKNOWN_ESCAPE = 20, // ? AGE3 ?
		CST_IAI_UNKNOWN_MAKE = 21, // AGE3 ?
		CST_IAI_UNKNOWN_FOR_BIRD = 0x28, // fly ?
		CST_IAI_CHEAT_SPAWN_UNIT = 0x64, // bigdaddy, etc
		CST_IAI_BUILD = 0x65, // For villagers (builders)
		CST_IAI_MAKE_OBJECT = 0x66,
		CST_IAI_TRAIN = 0x66, // =MakeObject
		CST_IAI_MAKE_TECH = 0x67, // Apply a technology, including cheats and game speed change !
		CST_IAI_RESEARCH = 0x67, // =MakeTech
		CST_IAI_CONVERT = 0x68,
		CST_IAI_HEAL = 0x69,
		CST_IAI_REPAIR = 0x6A,
		CST_IAI_ARTIFACT_CAPTURE = 0x6B, // Get auto-converted = "Be discovered". To confirm / get precisions
		CST_IAI_DISCOVERY_CAPTURE = 0x6C, // Be discovered for discovery (NOT relics/ruins)
		CST_IAI_RETREAT_TO_SHOOTING_RANGE = 0x6D,
		CST_IAI_GATHER_WITH_ATTACK = 0x6E, // (110) Hunt, cut wood, fish. Gather activities with an attack phase (includes fishing !)
		CST_IAI_TRADE = 0x6F,
		CST_IAI_GENERATE_WONDER_VICTORY = 120, // AGE3?
		CST_IAI_DESELECT_WHEN_TASKED = 121, // AGE3?
		CST_IAI_LOOT = 122, // AGE3?
		CST_IAI_HOUSING = 123, // AGE3?
		CST_IAI_UNPACK_ATTACK = 125, // AGE3?
		CST_IAI_UNKNOWN_131 = 131, //?
		CST_IAI_PICKUP_UNIT = 132, // AGE3???
		CST_IAI_SHEAR = 149 // AGE3???
	};

	// Unsure / to debug
	enum ACTION_STATUS : char {
		CST_AS_UNKNOWN_01 = 1, // Executing action ?? 40544D
		CST_AS_UNKNOWN_02 = 2, // Seen when moving was finished? (idle)
		CST_AS_UNKNOWN_03 = 3, // Can start moving to action location (temporary state) ? 40547F
		CST_AS_UNKNOWN_04 = 4, // Moving to action location ?
		CST_AS_UNKNOWN_06 = 6, // "start" in progress ? (analog to 3?)
		CST_AS_UNKNOWN_07 = 7, // in progress ? (analog to 4?). For both attack, build, etc
		CST_AS_UNKNOWN_0B = 0x0B, // Seen when moving to bird location (after killing it)
		CST_AS_UNKNOWN_0C = 0x0C, // waiting for requirement ? Really unsure
		CST_AS_UNKNOWN_0D = 0x0D, // Set when unit can't move (speed==0) ?? 405472
		CST_AS_UNKNOWN_0E = 0x0E,
		CST_AS_UNKNOWN_0F = 0x0F // Seen while moving (just moving)
	};

	enum INTERNAL_COMMAND_ID : char {
		CST_ICI_RIGHT_CLICK = 0, // Some kind of joker: for right-click: will guess command according to unit def interaction, target, etc
		CST_ICI_STOP_ACTION = 1, // This command is concerned by farm bug in 1.0b version (resets food). 1.0c "Hack" fix is in 0x42A75B.
		CST_ICI_UNKNOWN_02_TASK = 2, // task ? task from idle ? see 42A780
		CST_ICI_ADD_RESOURCE = 0x05, // Only used for AI "personality resource bonus" ?
		CST_ICI_TASK_UNIT = 0x0A, // Task from existing activity ?
		CST_ICI_RESIGN = 0x0B, // Only for MP games when clicking resign in menu. See exec=42B290
		CST_ICI_HOLD_POSITION = 0x11, // To confirm
		CST_ICI_TRIGGER_TRAIN_UNIT = 0x64, // unsure, when AI triggers a train unit action ? + some cheats?
		CST_ICI_RESEARCH = 0x65,
		CST_ICI_BUILD = 0x66,
		CST_ICI_CHANGE_SETTING = 0x67, // Includes cheats, game speed change, diplomacy change
		CST_ICI_KILL_UNIT = 0x6A,
		CST_ICI_PAY_TRIBUTE = 0x6C,
		CST_ICI_SET_TRADE_RESOURCE_TYPE = 0x6D, // Set resource to trade in a trade ship.
		CST_ICI_QUEUE_UNQUEUE_UNIT = 0x77, // Only for human-called "train unit" ? Include right-click to unqueue.
		CST_ICI_ADD_SCORE_ELEM = (char)0xFF // A special one...
	};
	enum INTERNAL_COMMAND67_SUBTYPE : char {
		CST_IC67_CHANGE_DIPLOMACY = 0,
		CST_IC67_CHANGE_GAME_SPEED = 1,
		CST_IC67_ADD_RESOURCE = 2,
		CST_IC67_SET_STEROIDS_MODE = 4,
		CST_IC67_SET_ALLY_VICTORY = 5,
		CST_IC67_CHEAT = 6,
		CST_IC67_APPLY_WRITING_TECH = 7,
		CST_IC67_SYNC_ERROR = 8 // Save game to syncerr?.gam (!) and quit game + sync error message. Saved game seems NOT to be playable
	};

	enum INTERNAL_CHEAT_ID : short int {
		CST_CHT_PHOTON_MAN = 0x0, // unit 274 = nuke trooper
		CST_CHT_E_MC2_TROOPER = 0x1, // unit 248 = nuke trooper (e=mc2 trooper)
		CST_CHT_JACK_BE_NIMBLE = 0x2, // catapult projectile. Tech 127
		CST_CHT_BIG_BERTHA = 0x3, // tech 128
		CST_CHT_FLYING_DUTCHMAN = 0x4, // tech 129
		CST_CHT_IMPROVED_LIONS = 0x5, // tech 130 (unused)
		CST_CHT_VILLAGER_NUKE_TROOPER = 0x6, // tech 131 (unused)
		CST_CHT_CHARIOT_ARCHER_CHEAT = 0x7, // tech 132
		CST_CHT_HOYO_HOYO = 0x8, // tech 133
		CST_CHT_LAZER_TOWER = 0x9, // tech 134 (no actual cheat to run this ?)
		CST_CHT_DARK_RAIN = 0xA, // tech 135
		CST_CHT_DARK_RIDER = 0xB, // tech 136
		CST_CHT_MEDUSA = 0xC, // tech 137 (medusa)
		CST_CHT_ICBM = 0xD, // tech 138 (ballista range)
		CST_CHT_STORMBILLY = 0xE, // unit 386 = zug209
		CST_CHT_SAINT_FRANCIS = 0xF, // unit 0x18A
		CST_CHT_BIG_MOMMA = 0x10, // unit 0x18D
		CST_CHT_BABY_PREZ = 0x11, // create unit 398
		CST_CHT_KING_ANIMALS = 0x12, // => apply tech 216
		CST_CHT_KING_ARTHUR = 0x13, // birds = dragons
		CST_CHT_FOOD = 0x14, // add 1000 resource0
		CST_CHT_COINAGE = 0x15, // add 1000 resource3
		CST_CHT_QUARRY = 0x16, // add 1000 resource2
		CST_CHT_WOODSTOCK = 0x17, // add 1000 resource1
		CST_CHT_STEROIDS = 0x18, // Can be cancelled if called again !
		CST_CHT_DIEDIEDIE = 0x64, // Kill all enemy players' units
		CST_CHT_KILLME = 0x65, // Kill all my units. what was the cheat for that ?
		CST_CHT_HOME_RUN = 0x66, // Make me win :)
		CST_CHT_RESIGN = 0x67, // Abandon :(
		CST_CHT_KILL1 = 0xC9,
		CST_CHT_KILL2 = 0xCA,
		CST_CHT_KILL3 = 0xCB,
		CST_CHT_KILL4 = 0xCC,
		CST_CHT_KILL5 = 0xCD,
		CST_CHT_KILL6 = 0xCE,
		CST_CHT_KILL7 = 0xCF,
		CST_CHT_KILL9 = 0xD0, // actually, kills player #8
		CST_CHT_BIGDADDY = 0xDC,
		CST_CHT_REVEAL_MAP = 0xE6,
		CST_CHT_NO_FOG = 0xE7
	};

	// For activity.task IDs
	enum ACTIVITY_TASK_IDS : long int {
		CST_ATI_NONE = -1, // Used a lot in game code
		CST_ATI_ATTACK = 0x258,
		CST_ATI_DEFEND_OR_CAPTURE = 0x259, // unsure, but it is used to capture relics
		CST_ATI_BUILD = 0x25A,
		CST_ATI_HEAL = 0x25B,
		CST_ATI_CONVERT = 0x25C,
		CST_ATI_EXPLORE = 0x25D,
		CST_ATI_GATHER_NOATTACK = 0x261, // Forager, farmer, miners (gathering that doesn't have an "attack" phase.
		CST_ATI_MOVE = 0x262, // Eg: used to capture relic. More specific than just move ?
		CST_ATI_GATHER_ATTACK = 0x265, // woodcutter, hunter, fisherman (gatherer that have an "attack" phase - not really true for fish, but anyway !)
		CST_ATI_UNKNOWN_266 = 0x266, // regroup in unit group ??? Related to SNAttackGroupGatherSpacing in 4CD551?
		CST_ATI_UNKNOWN_268 = 0x268, // some movement ?
		CST_ATI_GO_TO_TRANSPORT = 0x269, // Unsure
		CST_ATI_REPAIR = 0x26A,
		CST_ATI_HUMAN_TRAIN_UNIT = 0x26B, // only when triggered by human ?
		CST_ATI_RESEARCH_TECH = 0x26C, // includes train unit when triggered by AI.
		CST_ATI_UNLOAD = 0x26D, // unsure. Includes move+unload. Such unit can't be used as actor in strategy element.
		// TO DO: "when attacked" values, =x+100 (x+0x64) ?
		CST_ATI_UNKNOWN_2BB = 0x2BB, // when target unit dies ? or "owned" projectile "dies" ? Example: targetted farm
		CST_ATI_UNKNOWN_2BC_ATTACKING = 0x2BC, // 
		CST_ATI_UNKNOWN_2BD = 0x2BD, // Defend unit (related to activity.unitIdToDefend) ? Do NOT auto-attack nearby units? See 4DB9F0=tacAI.captureArtefact?(myUnitId, targetUnitId)
		CST_ATI_UNKNOWN_2BE = 0x2BE,
		CST_ATI_UNKNOWN_2C1 = 0x2C1, // Task unit ??
		CST_ATI_UNKNOWN_2C2 = 0x2C2, // DeTask unit ??
		CST_ATI_UNKNOWN_2C6 = 0x2C6, // Used for villagers ? 4DA2BF...
		CST_ATI_UNKNOWN_2C9 = 0x2C9, // (0x265+0x64) gatherer reaction => flee (military) or fight (animals)
		CST_ATI_HOLD_POSITION = 0x2D5, // Do not auto-attack. See 4E6740 for cats (don't search for target), 413F89 for other units (cancel attack if not at range?)
		CST_ATI_UNKNOWN_2D9 = 0x2D9
	};


	// See 507E72 for research correspondance
	enum SC_ED_DISABLE_TECHNOLOGY_INDICES : long int {
		CST_DTI_GRANARY = 0, // research 0x58
		CST_DTI_STORAGE_PIT = 1, // 0x59
		CST_DTI_DOCK = 2, // 0x5A
		CST_DTI_BARRACKS = 3, // 0x5B
		CST_DTI_MARKET = 4, // 0x5E
		CST_DTI_RANGE = 5, // 0x5F
		CST_DTI_STABLE = 6, // 0x61
		CST_DTI_TEMPLE = 7, // 0x62
		CST_DTI_GOVERNMENT_CENTER = 8, // 0x5D
		CST_DTI_SIEGE_WORKSHOP = 9, // 0x60
		CST_DTI_ACADEMY = 0x0A, // 0x5C
		CST_DTI_TOOL_AGE = 0x0B, // 0x65
		CST_DTI_BRONZE_AGE = 0x0C, // 0x66
		CST_DTI_IRON_AGE = 0x0D, // 0x67
		CST_DTI_TOWN_CENTER = 0x0E, // 0x73
		CST_DTI_WONDER = 0x0F, // 0x74
		// Values 0x10-0x13: unused (declared as array[20] in game code !)
		CST_DTI_COUNT = 0x14
	};

	// General victory condition choice
	enum GENERAL_VICTORY_CONDITION : long int {
		GVC_STANDARD = 0,
		GVC_CONQUEST = 1,
		GVC_SCORE = 2,
		GVC_TIME_LIMIT = 3,
		GVC_CUSTOM = 4
	};

	// See 4DF84C
	enum UNIT_GROUP_TYPES : long int {
		CST_UGT_LAND_ATTACK = 0x64, // Related to SNMaximumAttackGroupSize
		CST_UGT_LAND_DEFEND = 0x65, // Related to SNMaximumDefendGroupSize
		CST_UGT_LAND_EXPLORE = 0x66, // Related to SNMaximumExploreGroupSize
		CST_UGT_BOAT_ATTACK = 0x67, // Related to SNMaximumBoatAttackGroupSize
		CST_UGT_BOAT_DEFEND = 0x68, // Related to SNMaximumBoatDefendGroupSize
		CST_UGT_BOAT_EXPLORE = 0x69, // Related to SNMaximumBoatExploreGroupSize
		// Next ones : trade/fish/transport escorts ? Just a supposition
		CST_UGT_UNKNOWN_6A = 0x6A,
		CST_UGT_UNKNOWN_6B = 0x6B,
		CST_UGT_UNKNOWN_6C = 0x6C,
		CST_UGT_ARTEFACT = 0x6D // Seen with target=own TC.
	};

	enum UNIT_GROUP_TASK_IDS : long int {
		CST_UGT_UNKNOWN_00 = 0, // stop ? 0x4CD349
		CST_UGT_UNKNOWN_01 = 1, // Idle ? Set in 4CD110. Default=1 (0 in constructor, but 1 after init)
		CST_UGT_UNKNOWN_02_ATTACK = 2,
		CST_UGT_RETREAT = 3, // 4CD640
		CST_UGT_DEFEND_UNIT = 4, // 4CE309
		CST_UGT_UNKNOWN_05 = 5, // "add up HPs of my units" ?
		CST_UGT_UNKNOWN_06 = 6, // "add up HPs of my units" ?
		CST_UGT_EXTERMINATE = 7, // 4CD7A9
		CST_UGT_EXPLORE = 8, // 4CD977
		CST_UGT_UNKNOWN_09 = 9, // 4CD425. Regroup ? Uses SNAttackGroupGatherSpacing
		CST_UGT_GO_FISHING = 0x0A, // 4CE4AB
		CST_UGT_TRADE = 0x0B, // 4CE651
		CST_UGT_UNKNOWN_0C = 0x0C, // "add up HPs of my units" ?
		CST_UGT_UNUSED_0D = 0x0D, // SEEMS to be unused (invalid)
		CST_UGT_UNKNOWN_0E = 0x0E, // 4CD425. Regroup ?
		CST_UGT_UNUSED_0F = 0x0F, // SEEMS to be unused (invalid)
		CST_UGT_UNUSED_10 = 0x10, // SEEMS to be unused (invalid)
		CST_UGT_UNUSED_11 = 0x11, // SEEMS to be unused (invalid)
		CST_UGT_UNUSED_12 = 0x12, // SEEMS to be unused (invalid)
		CST_UGT_UNKNOWN_13 = 0x13, // "add up HPs of my units" ?
		CST_UGT_ATTACK_ROUNDUP_TARGET = 0x14,
		CST_UGT_UNKNOWN_15_ATTACK = 15
	};

	// See 0x4D0880, 0x4D1BB5
	enum AI_UPDATE_TYPES : long int {
		CST_AUT_SETUP_SOLDIER_GROUPS = 0, // "Setup SolGroups"
		CST_AUT_EVAL_CIVILIAN_DISTRIB = 1, // "Eval CivDist". Compute desired count of villager activities.
		CST_AUT_TASK_CIVILIAN = 2, // "Task Civs". Assign activities to vilagers
		CST_AUT_SET_BOAT_GROUPS = 3, // "Setup BoatGroups"
		CST_AUT_FILL_BOAT_GROUPS = 4, // "Fill BoatGroups"
		CST_AUT_TASK_BOATS = 5, // "Task Boats"
		CST_AUT_FILL_SOLDIER_GROUPS = 6, // "Fill SolGroups"
		CST_AUT_TASK_IDLE_SOLDIER = 7, // "Task IdleSol"
		CST_AUT_TASK_ACTIVE_SOLDIER = 8, // "Task ActSol"
		CST_AUT_PLAYTASKING = 9, // "Playtasking" - WTF ??? special attacks?
		CST_AUT_TASK_UNGRP_SOLDIER = 10, // "TaskUngrpSol"
		CST_AUT_RESEARCH = 11, // "Research"
		CST_AUT_TRAIN = 12, // "Train"
		CST_AUT_BUILD_LIST = 13, // "BldLst"
		CST_AUT_HELP_BUILD = 14, // "Help Build"
		CST_AUT_REPAIR_BUILDING = 15, // "Repair Bldg"
		CST_AUT_REPAIR_WALL = 16, // "Repair Wall"
		CST_AUT_BUILD = 17, // "Build" (trigger new constructions)
		CST_AUT_OPEN_BUILDS = 18, // "Open Builds"
		CST_AUT_OPEN_TASKS = 19, // "Open Tasks"
		CST_AUT_FOOD_DROPSITE = 20, // "Food Dropsite"
		CST_AUT_BUILD_LIST_INSERTIONS = 21 // "BldLst Insertions" 0x15
		// Other: "Invalid"
	};

	// Used in STRUCT_INF_AI_BUILDS_HISTORY
	enum INFAI_BLD_HISTORY_STATUS : char {
		CST_BHS_INACTIVE = 0, // Default (initial) value. Should generally be used in conjunction with a unitDefId == -1
		CST_BHS_ALIVE = 1, // When construction has actually been triggered at given position.
		CST_BHS_FAILED = 2, // Only used when createUnit method returned an error code (conflicting object, etc) and construction has been canceled
		CST_BHS_REMOVED = 3, // Removed, the slot can be used again in infAI.addOrUpdateBldHistory
		CST_BHS_MAX_VALUE = 3
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
		CST_GSBI_CUSTOM_OPTIONS = 0x20,
		CST_GSBI_CUSTOM_EXITDLG_YES = 0x21,
		CST_GSBI_CUSTOM_EXITDLG_NO = 0x22
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


	// Some mysterious internal IDs about MP COMMunication / status
	enum COMM_LINK_LEVEL: long int {
		CST_CLL_UNKNOWN_0 = 0,
		CST_CLL_UNKNOWN_1 = 1,
		CST_CLL_UNKNOWN_2 = 2, // see 0x422D2D
		CST_CLL_UNKNOWN_3 = 3, // see 0x422F9B
		CST_CLL_RELEASE_COMM = 4, // see 0x423207
		CST_CLL_SERVICE_AVAILABLE = 5, // see 0x42325A
		CST_CLL_SESSION_AVAILABLE = 6, // see 0x4232D8
		CST_CLL_SESSION_HOSTED = 7, // see 0x42346E
		CST_CLL_UNKNOWN_8 = 8, // see 0x42372F
		CST_CLL_PLAYER_CREATED = 9, // see 0x4235F2
		CST_CLL_COMM_SETTINGS_EXCHANGED = 0xA, // see 0x42368B
		CST_CLL_PLAYER_SET = 0xB, // see 0x423630
		CST_CLL_GAME_OPTIONS_EXCHANGED = 0xC, // see 0x42365D
		CST_CLL_GAME_IS_LOADING = 0xD, // see 0x4236BB
		CST_CLL_GAME_IS_RUNNING = 0xE, // see 0x4236E5
		CST_CLL_GAME_IS_PAUSED = 0xF, // see 0x423722
	};

	// 0->7 values to indicate the 8 horizontal/vertical/diagonal directions for some technical map algorithms
	enum POSITION_ALGORITHM_DIRECTIONS : long int {
		CST_DIR_SW = 0, // x-, y-
		CST_DIR_WEST = 1, // x-
		CST_DIR_NW = 2, // x-, y+
		CST_DIR_NORTH = 3, // y+
		CST_DIR_NE = 4, // x+, y+
		CST_DIR_EAST = 5, // x+
		CST_DIR_SE = 6, // x+, y-
		CST_DIR_SOUTH = 7 // y-
	};
}
