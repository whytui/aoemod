#include <tchar.h>
#include <string>

#pragma once

// This file contains DLL IDs for texts that are used in game or custom treatments

// *******************************************************
// Standard strings
// *******************************************************

const static unsigned long int LANG_ID_OK = 0xFA1;
const static unsigned long int LANG_ID_CANCEL = 0xFA2;
const static unsigned long int LANG_ID_PLAYERX_BASE = 10340; // LANG_ID_PLAYERX_BASE + 1-8 = player1-player8
const static unsigned long int LANG_ID_PLAYER_NAME_BY_CIV_BASE = 0x1130; // LANG_ID_PLAYER_NAME_BY_CIV_BASE + 10*civ+(1->9) = a tribe name
const static unsigned long int LANG_ID_PERSONALITY = 10258;
const static unsigned long int LANG_ID_STRATEGY = 10259;
const static unsigned long int LANG_ID_GAME_OPTIONS = 9274; // 0x243A. For game menu options entry
const static unsigned long int LANG_ID_OPTIONS = 10017; // Just "options"
const static unsigned long int LANG_ID_AI_FILENAME_BASE = 26928; // 26928+i = filename for predefined stratgy #i. Total count 0x83=131. EXE uses index 158000+i, why ?
const static unsigned long int LANG_ID_AI_FILENAME_LAST_VALID = 27058; // Last valid AI filename
const static unsigned long int LANG_ID_STOP_CURRENT_ACTION = 0x1D4DD;
const static unsigned long int LANG_ID_NEXT_PAGE = 0x1D4D7;

const static unsigned long int LANG_ID_640_480 = 0x24E7;
const static unsigned long int LANG_ID_800_600 = 0x24E8;
const static unsigned long int LANG_ID_1024_768 = 0x24E9;

const static unsigned long int LANG_ID_HINT_CLICK_TO_FISH_HERE = 3707;
const static unsigned long int LANG_ID_HINT_CLICK_TO_GATHER_BUSH_HERE = 3823;
const static unsigned long int LANG_ID_HINT_TRADE_WITH = 4116;

const static unsigned long int LANG_ID_STONE_AGE = 4201;
const static unsigned long int LANG_ID_TOOL_AGE = 4202;
const static unsigned long int LANG_ID_BRONZE_AGE = 4203;
const static unsigned long int LANG_ID_IRON_AGE = 4204;

const static unsigned long int LANG_ID_RESEARCH_MONOTHEISM = 7018;
const static unsigned long int LANG_ID_RESEARCH_MARTYRDOM = 7095;



// *******************************************************
// Custom strings
// *******************************************************

const static unsigned long int CRLANG_ID_ROCKNROR = 42000;
const static unsigned long int CRLANG_ID_DEBUG_INIT = 42001;
const static unsigned long int CRLANG_ID_WARN_MISSING_FEATURE = 42002;
const static unsigned long int CRLANG_ID_MISSING_CIVS_IN_EMPIRES_DAT = 42003;
const static unsigned long int CRLANG_ID_CANT_WRITE_FILE = 42004;
const static unsigned long int CRLANG_ID_CANT_READ_XML_CONFIG = 42005;
const static unsigned long int CRLANG_ID_CANT_READ_CIV_XML_CONFIG = 42006;
const static unsigned long int CRLANG_ID_RUN_EXE_CONFIG_CHECKS = 42007;
const static unsigned long int CRLANG_ID_WELCOME1 = 42008;
const static unsigned long int CRLANG_ID_WELCOME2 = 42009;
const static unsigned long int CRLANG_ID_CUSTOM = 42085;
const static unsigned long int CRLANG_ID_ERROR_COULDNT_FIND = 42089;
const static unsigned long int CRLANG_ID_ERROR_EMPIRES_DAT_USED_INSTEAD = 42090;
const static unsigned long int CRLANG_ID_YES = 42094;
const static unsigned long int CRLANG_ID_NO = 42095;

// RockNRor main settings
const static unsigned long int CRLANG_ID_CLOSE = 42126;
const static unsigned long int CRLANG_ID_ROCKNROR_SETTINGS_TITLE = 42127;
const static unsigned long int CRLANG_ID_ROCKNROR_GLOBAL_SETTINGS = 42169;
const static unsigned long int CRLANG_ID_RNR_STTGS_INTRO_TEXT = 42128;
const static unsigned long int CRLANG_ID_RNR_STTGS_MODS_SECTION = 42129;
const static unsigned long int CRLANG_ID_RNR_STTGS_GEN_RANDOM_TECH_TREES_RM = 42130;
const static unsigned long int CRLANG_ID_RNR_STTGS_GEN_RANDOM_TECH_TREES_BONUS_COUNT = 42160;
const static unsigned long int CRLANG_ID_RNR_STTGS_GEN_RANDOM_TECH_TREES_BONUS_COUNT_DTL = 42161;
const static unsigned long int CRLANG_ID_RNR_STTGS_RPG_MODE_RANDOM_GAMES = 42131;
const static unsigned long int CRLANG_ID_RNR_STTGS_RPG_MODE_SCENARIO = 42132;
const static unsigned long int CRLANG_ID_RNR_STTGS_GEN_STRATEGY_RM = 42133;
const static unsigned long int CRLANG_ID_RNR_STTGS_GEN_STRATEGY_DM = 42134;
const static unsigned long int CRLANG_ID_RNR_STTGS_NO_WALLS_RANDOM_GAMES = 42135;
const static unsigned long int CRLANG_ID_RNR_STTGS_NO_NEUTRAL_DIPL_RANDOM_GAMES = 42136;
const static unsigned long int CRLANG_ID_RNR_STTGS_NO_WALL_RANDOM_GAMES = 42137;
const static unsigned long int CRLANG_ID_RNR_STTGS_SHARE_ALLY_EXPLORATION_IN_RANDOM_GAMES = 42138;
const static unsigned long int CRLANG_ID_RNR_STTGS_RNR_AI_OPTIONS_SECTION = 42139;
const static unsigned long int CRLANG_ID_RNR_STTGS_IMPROVE_AI = 42140;
const static unsigned long int CRLANG_ID_RNR_STTGS_IMPROVE_CITY_PLAN_AI = 42141;
const static unsigned long int CRLANG_ID_RNR_STTGS_FIX_LOGISTICS_BUG = 42142;
const static unsigned long int CRLANG_ID_RNR_STTGS_NEED_HIGHER_RESOLUTION = 42143;
const static unsigned long int CRLANG_ID_RNR_STTGS_CHANGE_SCREEN_RESOLUTION = 42144;
const static unsigned long int CRLANG_ID_RNR_STTGS_RESOLUTION_SMALL = 42145;
const static unsigned long int CRLANG_ID_RNR_STTGS_RESOLUTION_MEDIUM = 42146;
const static unsigned long int CRLANG_ID_RNR_STTGS_RESOLUTION_HIGH = 42147;
const static unsigned long int CRLANG_ID_RNR_STTGS_ERROR_MSG_TITLE = 42148;
const static unsigned long int CRLANG_ID_RNR_STTGS_RESOLUTION_USE_STD_OPTIONS = 42149;
const static unsigned long int CRLANG_ID_RNR_STTGS_SP_MAXIMUM_POPULATION = 42150;
const static unsigned long int CRLANG_ID_RNR_STTGS_ENTER_VALUE_SP_MAX_POP = 42151;
const static unsigned long int CRLANG_ID_RNR_STTGS_SP_MAX_POP_LINK = 42152;
const static unsigned long int CRLANG_ID_RNR_STTGS_RANDOM_GAMES_RELICS_COUNT_LINK = 42153;
const static unsigned long int CRLANG_ID_RNR_STTGS_RANDOM_GAMES_RUINS_COUNT_LINK = 42154;
const static unsigned long int CRLANG_ID_RNR_STTGS_RANDOM_GAMES_RELICS_COUNT_TITLE = 42155;
const static unsigned long int CRLANG_ID_RNR_STTGS_RANDOM_GAMES_RUINS_COUNT_TITLE = 42156;
const static unsigned long int CRLANG_ID_RNR_STTGS_ENTER_VALUE_RANDOM_GAMES_RELICS_COUNT = 42157;
const static unsigned long int CRLANG_ID_RNR_STTGS_ENTER_VALUE_RANDOM_GAMES_RUINS_COUNT = 42158;
const static unsigned long int CRLANG_ID_RNR_STTGS_RANDOM_GAMES_MAX_AGE = 42159;
const static unsigned long int CRLANG_ID_RNR_STTGS_NEXT_PAGE = 42162;
const static unsigned long int CRLANG_ID_RNR_STTGS_BACK_TO_TOP = 42163;

// RockNRor options
const static unsigned long int CRLANG_ID_ROCKNROR_OPTIONS_POPUP_TITLE = 42020;
const static unsigned long int CRLANG_ID_TECH_TREE = 42021;
const static unsigned long int CRLANG_ID_TECH_TREE_CAN_BE_RESEARCHED_TITLE = 42028;
const static unsigned long int CRLANG_ID_NOT_AVAILABLE_YET = 42029;
const static unsigned long int CRLANG_ID_OTHER_COMMANDS_LABEL = 42022;
const static unsigned long int CRLANG_ID_DISLIKE_PENALTY = 42030;
const static unsigned long int CRLANG_ID_GAME_SPEED_CHANGE_FACTOR = 42031;
const static unsigned long int CRLANG_ID_AUTO_REBUILD_FARMS = 42032;
const static unsigned long int CRLANG_ID_AUTO_REBUILD_FARMS_MAX_NUMBER = 42033;
const static unsigned long int CRLANG_ID_AUTO_REBUILD_FARMS_MAX_FOOD = 42034;
const static unsigned long int CRLANG_ID_AUTO_REBUILD_FARMS_MIN_WOOD = 42035;
const static unsigned long int CRLANG_ID_MAP_COPY = 42172;
const static unsigned long int CRLANG_ID_RESTART_NEW_SEED = 42173;

const static unsigned long int CRLANG_ID_DEFAULT_CIV_NAME = 42023;

// Custom buttons
const static unsigned long int CRLANG_ID_BEING_RESEARCHED = 42024;
const static unsigned long int CRLANG_ID_BEING_TRAINED = 42025;

const static unsigned long int CRLANG_ID_AI_FLAGS_RECOMPUTED = 42026;
const static unsigned long int CRLANG_ID_AI_FLAGS_ALL_SET = 42027;
const static unsigned long int CRLANG_ID_AI_FLAGS_NONE_SET = 42010;
const static unsigned long int CRLANG_ID_AI_FLAGS_ONLY_CURRENT_SET = 42011;
const static unsigned long int CRLANG_ID_AI_FLAG_SWITCH_CURRENT = 42012;
const static unsigned long int CRLANG_ID_CANT_TRAIN = 42087;
const static unsigned long int CRLANG_ID_CANT_RESEARCH = 42088;
const static unsigned long int CRLANG_ID_BTN_DONT_ATTACK_VILLAGER = 42116;
const static unsigned long int CRLANG_ID_BTN_DONT_ATTACK_BUILDINGS = 42117;
const static unsigned long int CRLANG_ID_BTN_DONT_ATTACK_OTHER = 42118;
const static unsigned long int CRLANG_ID_BTN_RESTORE_ATTACK_BEHAVIOUR = 42119;
const static unsigned long int CRLANG_ID_BTN_UNIT_PROTECT_TEXT = 42120;
const static unsigned long int CRLANG_ID_BTN_UNIT_PROTECT_HOTKEY = 42121;
const static unsigned long int CRLANG_ID_BTN_BACK = 42122;
const static unsigned long int CRLANG_ID_BTN_UNIT_STOP_PROTECTING = 42123;
const static unsigned long int CRLANG_ID_BTN_UNIT_SET_PROTECT_OBJECT = 42124;
const static unsigned long int CRLANG_ID_BTN_UNIT_MAKE_MAIN_FOR_KEY_SHORTCUT_SELECTION = 42125;
const static unsigned long int CRLANG_ID_BTN_OPEN_CLOSE_GATE = 42170;
const static unsigned long int CRLANG_ID_BTN_OPEN_CLOSE_GATE_HOTKEY = 42171;

// Unit properties
const static unsigned long int CRLANG_ID_UNITPROP_INITIAL = 42036;
const static unsigned long int CRLANG_ID_UNITPROP_NOTBUILT = 42037;
const static unsigned long int CRLANG_ID_UNITPROP_GATHERABLE = 42038;
const static unsigned long int CRLANG_ID_UNITPROP_DYING = 42039;
const static unsigned long int CRLANG_ID_UNITPROP_TITLE = 42040;
const static unsigned long int CRLANG_ID_UNITPROP_NORMAL_STATUS = 42041;
const static unsigned long int CRLANG_ID_UNITPROP_UNIT_TERM = 42042;
const static unsigned long int CRLANG_ID_UNITPROP_SHORTCUT_TERM = 42043;
const static unsigned long int CRLANG_ID_UNITPROP_NONE_TERM = 42044;
const static unsigned long int CRLANG_ID_UNITPROP_INVALID_TERM = 42045;
const static unsigned long int CRLANG_ID_UNITPROP_PART_OF_GROUP = 42046;
const static unsigned long int CRLANG_ID_UNITPROP_BUILDING = 42047;
const static unsigned long int CRLANG_ID_UNITPROP_CHILDREN_UNITS_AUTOMOVE_TO = 42048;
const static unsigned long int CRLANG_ID_UNITPROP_AUTO_REBUILD_THIS_FARM_WHEN_DEPLETED = 42049;
const static unsigned long int CRLANG_ID_UNITPROP_TECHID_ABBREV = 42050;
const static unsigned long int CRLANG_ID_UNITPROP_INGAME_TITLE = 42051;
const static unsigned long int CRLANG_ID_UNITPROP_DISABLE_AUTO_MOVE = 42052;
const static unsigned long int CRLANG_ID_UNITPROP_DEFAULT = 42053;
const static unsigned long int CRLANG_ID_UNITPROP_ALWAYS = 42054;
const static unsigned long int CRLANG_ID_UNITPROP_NEVER = 42055;
const static unsigned long int CRLANG_ID_UNITPROP_AUTO_ATTACK_UNITS = 42056;
const static unsigned long int CRLANG_ID_UNITPROP_TOWERS = 42057;
const static unsigned long int CRLANG_ID_UNITPROP_MILITARY = 42058;
const static unsigned long int CRLANG_ID_UNITPROP_BUILDINGS = 42059;
const static unsigned long int CRLANG_ID_UNITPROP_VILLAGERS = 42060;
const static unsigned long int CRLANG_ID_UNITPROP_WALLS = 42061;
const static unsigned long int CRLANG_ID_UNITPROP_FUTURE_UNAVAILABLE_TECHS = 42062;
const static unsigned long int CRLANG_ID_UNITPROP_ATTACK_BONUSES_FOR = 42083;
const static unsigned long int CRLANG_ID_UNITPROP_ARMOR_WEAKNESSES = 42084;
const static unsigned long int CRLANG_ID_UNITPROP_CONVERSION_RESISTANCE = 42086;
const static unsigned long int CRLANG_ID_UNITPROP_CONVERSION_EFFICIENCY = 42174;
const static unsigned long int CRLANG_ID_UNITPROP_CONVERSION_RECHARGING_RATE = 42175;
const static unsigned long int CRLANG_ID_UNITPROP_HEALING_RATE_BONUS = 42176;
const static unsigned long int CRLANG_ID_UNITPROP_BONUS = 42177;
const static unsigned long int CRLANG_ID_UNITPROP_CREATOR = 42178;

// Scenario editor
const static unsigned long int CRLANG_IDCUSTOM_EDITOR_MENU = 42063;
const static unsigned long int CRLANG_IDPLAYER_ID = 42064;
const static unsigned long int CRLANG_IDTRIGGERS = 42065;
const static unsigned long int CRLANG_IDTERRAIN_EDIT = 42066;
const static unsigned long int CRLANG_IDVICTORY_CONDITIONS = 42067;
const static unsigned long int CRLANG_IDUNIT_OVERLAPPING = 42068;
const static unsigned long int CRLANG_IDBUILD_ON_ELEVATION = 42069;
const static unsigned long int CRLANG_IDDISABLE_TERRAIN_RESTRICTION = 42070;
const static unsigned long int CRLANG_IDLENGTHEN_COMBAT_MODE = 42071;
const static unsigned long int CRLANG_ID_EXPORT_MAP_TO_BMP_BUTTON = 42164;
const static unsigned long int CRLANG_ID_EXPORT_MAP_TO_BMP_POPUP_TITLE = 42165;
const static unsigned long int CRLANG_ID_EXPORT_MAP_OK_MESSAGE = 42166;
const static unsigned long int CRLANG_ID_EXPORT_MAP_FAILED_MESSAGE = 42167;
const static unsigned long int CRLANG_ID_SAVE_AS_SCN_BTN = 42168;

const static unsigned long int CRLANG_ID_EDIT_TRIGGERS = 42072;
const static unsigned long int CRLANG_ID_GET_STRATEGY = 42073;
const static unsigned long int CRLANG_ID_GET_PERSONALITY = 42074;
const static unsigned long int CRLANG_ID_GOLD_VICT_CONDITION = 42075;
const static unsigned long int CRLANG_ID_GOLD_VICT_CONDITION_DETAILS = 42076;
const static unsigned long int CRLANG_ID_TYPE_X_Y_MAP_SIZE = 42077;
const static unsigned long int CRLANG_ID_POSX_MINMAX = 42078;
const static unsigned long int CRLANG_ID_POSY_MINMAX = 42079;
const static unsigned long int CRLANG_ID_TERRAINID = 42080;
const static unsigned long int CRLANG_ID_ELEVATION = 42081;
const static unsigned long int CRLANG_ID_TIP_TERRAIN_LEAVE_EMPTY = 42082;
const static unsigned long int CRLANG_ID_SOFTEN_TERRAIN_OPTION = 42091;
const static unsigned long int CRLANG_ID_MOUSE_POSITION = 42092;
const static unsigned long int CRLANG_ID_SUCCESS = 42093;
const static unsigned long int CRLANG_ID_COPY_ALL_MAP = 42096;
const static unsigned long int CRLANG_ID_COPY_MAP_ZONE = 42097;
const static unsigned long int CRLANG_ID_PASTE_MAP_ZONE = 42098;
const static unsigned long int CRLANG_ID_COPY_MAP_UNITS = 42099;
const static unsigned long int CRLANG_ID_COPY_PASTE_MAP_ZONES_TITLE = 42100;
const static unsigned long int CRLANG_ID_COPY_FROM_MAP_LABEL = 42101;
const static unsigned long int CRLANG_ID_COPY_TO_MAP_LABEL = 42102;
const static unsigned long int CRLANG_ID_PASTE_TO_MAP_LABEL = 42103;
const static unsigned long int CRLANG_ID_MAP_COPY_CURRENT_BUFFER_SIZE = 42104;
const static unsigned long int CRLANG_ID_MAP_COPY_CURRENT_BUFFER_HAS_UNITS = 42105;
const static unsigned long int CRLANG_ID_GEN_TRIGGER_SAMPLES = 42106;
const static unsigned long int CRLANG_ID_SAVE = 42107;
const static unsigned long int CRLANG_ID_RELOAD = 42108;
const static unsigned long int CRLANG_ID_GENERATE_TRIGGER_SAMPLE = 42109;
const static unsigned long int CRLANG_ID_TRG_CHOOSE_EVENT = 42110;
const static unsigned long int CRLANG_ID_TRG_CHOOSE_ACTION = 42111; 
const static unsigned long int CRLANG_ID_TRG_RUN_BEFORE = 42112;
const static unsigned long int CRLANG_ID_TRG_RUN_AFTER = 42113;
const static unsigned long int CRLANG_ID_TRG_DIFF_LEVEL_CONDITION = 42114;
const static unsigned long int CRLANG_ID_TRG_REFER_TO_ROCKNRORADMIN_FOR_DOC = 42115;

