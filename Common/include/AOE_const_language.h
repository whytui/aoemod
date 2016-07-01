#include <tchar.h>
#include <string>

#pragma once

// This file contains DLL IDs for texts that are used in game or custom treatments

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

// Custom strings
const static unsigned long int CRLANG_ID_CUSTOMROR = 42000;
const static unsigned long int CRLANG_ID_DEBUG_INIT = 42001;
const static unsigned long int CRLANG_ID_WARN_MISSING_FEATURE = 42002;
const static unsigned long int CRLANG_ID_MISSING_CIVS_IN_EMPIRES_DAT = 42003;
const static unsigned long int CRLANG_ID_CANT_WRITE_FILE = 42004;
const static unsigned long int CRLANG_ID_CANT_READ_XML_CONFIG = 42005;
const static unsigned long int CRLANG_ID_CANT_READ_CIV_XML_CONFIG = 42006;
const static unsigned long int CRLANG_ID_RUN_EXE_CONFIG_CHECKS = 42007;
const static unsigned long int CRLANG_ID_WELCOME1 = 42008;
const static unsigned long int CRLANG_ID_WELCOME2 = 42009;

// CustomRor options
const static unsigned long int CRLANG_ID_CUSTOMROR_OPTIONS_POPUP_TITLE = 42020;
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

const static unsigned long int CRLANG_ID_DEFAULT_CIV_NAME = 42023;
// Custom buttons
const static unsigned long int CRLANG_ID_BEING_RESEARCHED = 42024;
const static unsigned long int CRLANG_ID_BEING_TRAINED = 42025;

const static unsigned long int CRLANG_ID_AI_FLAGS_RECOMPUTED = 42026;
const static unsigned long int CRLANG_ID_AI_FLAGS_ALL_SET = 42027;

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

const static unsigned long int CRLANG_ID_CUSTOM = 42085;
