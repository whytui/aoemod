
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

	enum INTERNAL_MAIN_CATEGORIES : long int { // TO DO check type
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

	enum INGAME_UI_COMMAND_ID : long int { // TO DO check type
		//CST_IUC_OK = 1,
		//CST_IUC_CANCEL = 2,
		CST_IUC_BUILD_UNSURE = 3,
		//CST_IUC_NO = 4, // cancel selection ?
		CST_IUC_RETRY = 5,
		CST_IUC_CANCEL_2 = 6,
		CST_IUC_ABANDON = 7,
		CST_IUC_ERROR = 8,
		CST_IUC_UNKNOWN_09 = 9,
		CST_IUC_FORMATION = 0x0A,
		CST_IUC_CANCEL_SELECT = 0x0B,
		CST_IUC_NEXT = 0x0C,
		CST_IUC_CHAT = 0x0D,
		CST_IUC_DIPLOMACY = 0x0E,
		CST_IUC_MENU = 0x0F,
		CST_IUC_TRADE_WITH = 0x10,
		CST_IUC_DO_RESEARCH = 0x11,
		CST_IUC_DO_TRAIN = 0x12,
		CST_IUC_DO_BUILD = 0x13,
		CST_IUC_CANCEL_BUILD = 0x14,
		CST_IUC_HELP = 0x15,
		CST_IUC_HOLD_POSITION = 0x16, // DLL id 4122
		CST_IUC_ATTACK_POSITION = 0x17,
		CST_IUC_TRADE_FOOD_FOR_GOLD = 0x19,
		CST_IUC_TRADE_WOOD_FOR_GOLD = 0x1A,
		CST_IUC_TRADE_STONE_FOR_GOLD = 0x1B,
		CST_IUC_HEAL = 0x1C,
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
		CST_GET_UNIT_ATTACKED = 0x201,
		CST_GET_MOVEMENT_FINISHED = 0x202, // unsure
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

	// For "action" structures. There are NOT values for "command" structures
	enum INTERNAL_ACTION_ID : short int {
		CST_IAI_MOVE_1 = 1, // unsure
		CST_IAI_FOLLOW = 2, // TO CONFIRM (AGE3)
		CST_IAI_GARRISON = 3, // Enter
		CST_IAI_EXPLORE_04 = 4, // TO CONFIRM
		CST_IAI_GATHER_05 = 5, // ability to gather/rebuild 1. Includes farming, ...
		CST_IAI_UNKNOWN_06 = 6, // Natural wonders cheat??? attack or flee from (seen) unit ?
		CST_IAI_ATTACK_7 = 7, // unsure
		CST_IAI_SHOOT = 8, // TO CONFIRM
		CST_IAI_ATTACK_9 = 9,
		CST_IAI_FLY = 0x0A, // TO CONFIRM
		CST_IAI_SCARE_HUNT = 0x0B, // TO CONFIRM
		CST_IAI_TRANSPORT = 0x0C, // OR UNLOAD ???
		CST_IAI_GUARD = 0x0D, // TO CONFIRM
		CST_IAI_EXPLORE_UNSURE_1E = 0x1E,
		CST_IAI_UNKNOWN_ESCAPE = 20, // ? AGE3 ?
		CST_IAI_UNKNOWN_MAKE = 21, // AGE3 ?
		CST_IAI_UNKNOWN_FOR_BIRD = 0x28, // fly ?
		CST_IAI_CHEAT_SPAWN_UNIT = 0x64, // bigdaddy, etc
		CST_IAI_BUILD = 0x65, // WRONG? =research
		CST_IAI_MAKE_OBJECT = 0x66,
		CST_IAI_TRAIN = 0x66, // =MakeObject
		CST_IAI_MAKE_TECH = 0x67, // Apply a technology, including cheats and game speed change !
		CST_IAI_RESEARCH = 0x67, // =MakeTech
		CST_IAI_CONVERT = 0x68,
		CST_IAI_HEAL = 0x69,
		CST_IAI_REPAIR = 0x6A,
		CST_IAI_ARTIFACT_UNKNOWN = 0x6B, // Get auto-converted
		CST_IAI_DISCOVERY_ARTIFACT = 0x6C,
		CST_IAI_RETREAT_TO_SHOOTING_RANGE = 0x6D,
		CST_IAI_HUNT =0x6E,
		CST_IAI_TRADE = 0x6F,
		CST_IAI_GENERATE_WONDER_VICTORY = 120, // AGE3?
		CST_IAI_DESELECT_WHEN_TASKED = 121, // AGE3?
		CST_IAI_LOOT = 122, // AGE3?
		CST_IAI_HOUSING = 123, // AGE3?
		CST_IAI_UNPACK_ATTACK = 125, // AGE3?
		CST_IAI_PICKUP_UNIT = 132, // AGE3???
		CST_IAI_SHEAR = 149 // AGE3???
	};

	enum INTERNAL_COMMAND_ID : char {
		CST_ICI_RIGHT_CLICK = 0, // Some kind of joker: for right-click: will guess command according to unit def interaction, target, etc
		CST_ICI_ADD_RESOURCE = 0x05, // Only used for AI "personality resource bonus" ?
		CST_ICI_TASK_UNIT = 0x0A,
		CST_ICI_TRIGGER_TRAIN_UNIT = 0x64, // unsure, when AI triggers a train unit action ? + some cheats?
		CST_ICI_RESEARCH = 0x65,
		CST_ICI_BUILD = 0x66,
		CST_ICI_MAKE_CHEAT = 0x67, // Includes cheats, game speed change
		CST_ICI_KILL_UNIT = 0x6A,
		CST_ICI_SET_TRADE_RESOURCE_TYPE = 0x6D, // Set resource to trade in a trade ship.
		CST_ICI_QUEUE_UNQUEUE_UNIT = 0x77, // Only for human-called "train unit" ? Include right-click to unqueue.
		CST_ICI_ADD_SCORE_ELEM = (char)0xFF // A special one...
	};

	// For activity.task IDs
	enum ACTIVITY_TASK_IDS : long int {
		CST_ATI_ATTACK = 0x258,
		CST_ATI_BUILD = 0x25A,
		CST_ATI_HEAL = 0x25B,
		CST_ATI_CONVERT = 0x25C,
		CST_ATI_EXPLORE = 0x25D,
		CST_ATI_GATHER_NOATTACK = 0x261, // Forager, farmer, miners
		CST_ATI_MOVE = 0x262,
		CST_ATI_GATHER_ATTACK = 0x265, // woodcutter, hunter, fisherman (gatherer that have an "attack" phase - not really true for fish, but anyway !)
		CST_ATI_GO_TO_TRANSPORT = 0x269, // Unsure
		CST_ATI_REPAIR = 0x26A,
		CST_ATI_HUMAN_TRAIN_UNIT = 0x26B, // only when triggered by human ?
		CST_ATI_RESEARCH_TECH = 0x26C, // includes train unit when triggered by AI.
		CST_ATI_UNLOAD = 0x26D, // unsure. Includes move+unload. Such unit can't be used as actor in strategy element.
		// TO DO: "when attacked" values, =x+100 (x+0x64) ?
		CST_ATI_UNKNOWN_2BB = 0x2BB, // when target unit dies ? or "owned" projectile "dies" ? Example: targetted farm
		CST_ATI_UNKNOWN_2BC_ATTACKING = 0x2BC,
		CST_ATI_UNKNOWN_2BE = 0x2BE,
		CST_ATI_UNKNOWN_2C1 = 0x2C1, // Task unit ??
		CST_ATI_UNKNOWN_2C2 = 0x2C2, // DeTask unit ??
		CST_ATI_UNKNOWN_2C6 = 0x2C6, // Used for villagers ? 4DA2BF...
		CST_ATI_UNKNOWN_2C9 = 0x2C9,
		CST_ATI_UNKNOWN_2D9 = 0x2D9,
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


	enum UNIT_GROUP_TYPES : long int {
		CST_UGT_UNKNOWN_64 = 0x64,
		CST_UGT_UNKNOWN_65 = 0x65,
		CST_UGT_UNKNOWN_66 = 0x66,
		CST_UGT_UNKNOWN_67 = 0x67,
		CST_UGT_UNKNOWN_6A = 0x6A,
		CST_UGT_UNKNOWN_6B = 0x6B,
		CST_UGT_UNKNOWN_6C = 0x6C,
		CST_UGT_ARTEFACT = 0x6D
	};

	enum UNIT_GROUP_TASK_IDS : long int {
		CST_UGT_UNKNOWN_00 = 0,
		CST_UGT_UNKNOWN_01 = 1,
		CST_UGT_UNKNOWN_02_ATTACK = 2,
		CST_UGT_RETREAT = 3, // 4CD640
		CST_UGT_DEFEND_UNIT = 4, // 4CE309
		CST_UGT_UNKNOWN_05 = 5, // "add up HP" ?
		CST_UGT_UNKNOWN_06 = 6, // "add up HP" ?
		CST_UGT_EXTERMINATE = 7, // 4CD7A9
		CST_UGT_EXPLORE = 8, // 4CD977
		CST_UGT_UNKNOWN_09 = 9, // 4CD425
		CST_UGT_GO_FISHING = 0x0A, // 4CE4AB
		CST_UGT_TRADE = 0x0B, // 4CE651
		CST_UGT_UNKNOWN_0C = 0x0C, // "add up HP" ?
		CST_UGT_UNUSED_0D = 0x0D, // SEEMS to be unused (invalid)
		CST_UGT_UNKNOWN_0E = 0x0E, // 4CD425
		CST_UGT_UNUSED_0F = 0x0F, // SEEMS to be unused (invalid)
		CST_UGT_UNUSED_10 = 0x10, // SEEMS to be unused (invalid)
		CST_UGT_UNUSED_11 = 0x11, // SEEMS to be unused (invalid)
		CST_UGT_UNUSED_12 = 0x12, // SEEMS to be unused (invalid)
		CST_UGT_UNKNOWN_13 = 0x13, // "add up HP" ?
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
		CST_AUT_PLAYTASKING = 9, // "Playtasking" - WTF ???
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
