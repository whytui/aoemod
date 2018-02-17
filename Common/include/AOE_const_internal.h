
#pragma once
#include "AOE_const_unit_ai.h"
#include "AOE_const_internal_UI.h"

/* This file contains various constants that are used in game EXE
*/

namespace AOE_CONST_INTERNAL
{
	// The maximum possible value for "population limit" regarding technical considerations.
	const long int TECH_LIMIT_MAXIMUM_POPULATION = 255;


	// Avoid confusion with diplomacy values (PLAYER_DIPLOMACY_VALUES) !
	enum PLAYER_DIPLOMACY_STANCES : char {
		CST_PDS_UNKNOWN = -1, // For internal treatments only !
		CST_PDS_ALLY = 0, // For my allies
		CST_PDS_NEUTRAL = 1, // Including "me" ?
		CST_PDS_UNUSED_2 = 2,
		CST_PDS_ENEMY = 3
	};

	// Avoid confusion with PLAYER_DIPLOMACY_STANCES !
	enum PLAYER_DIPLOMACY_VALUES : long int {
		CST_PDV_UNKNOWN, // For internal treatments only !
		CST_PDV_GAIA_SPECIAL = 0, // Gaia has 0 vs all players
		CST_PDV_SELF = 1,
		CST_PDV_ALLY = 2,
		CST_PDV_NEUTRAL = 3,
		CST_PDV_ENEMY = 4
	};

	// For game settings
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
		GPT_GAIA_OR_OTHER = 0, // "-" in 0x423C96
		GPT_UNKNOWN_1 = 1, // unused ?
		GPT_HUMAN_CONTROLLED = 2, // "W"
		GPT_UNKNOWN_3 = 3, // unused ?
		GPT_COMPUTER_CONTROLLED = 4, // "C"
		GPT_UNKNOWN_5 = 5, // "X" Human controlled to (could be secondary human on same player ????)
		GPT_UNKNOWN_6 = 6 // "V" (viewing?)
	};

	// Represents game difficulty levels. Warning: lower level = harder !
	enum GAME_DIFFICULTY_LEVEL : char {
		GDL_HARDEST = 0,
		GDL_HARD = 1,
		GDL_MEDIUM = 2,
		GDL_EASY = 3,
		GDL_EASIEST = 4
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

	// Ordered statuses for units. Units lifecycle go from 0 to 7 (8?). Some steps may not be used for all unit types.
	// When a status is not used/has no specific role for a unit, then the status advances to next status (e.g. status 1 => 2 immediately)
	// unit.setStatus = [EDX+5C]. unit.manageStatusChange?=[EDX+24]
	enum GAME_UNIT_STATUS : char {
		GUS_0_NOT_BUILT = 0, // Seems to be relevant only for buildings. This status is kept as long as building is not fully built.
		GUS_1_UNKNOWN_1 = 1, // Not sure this status is really used/has a valid role ?
		GUS_2_READY = 2, // Standard "alive" status for most units
		
		// Triggered when HP < 1. From here, unit can no longer have actions, no longer provides fog visibility (except the tile itself).
		// Berry bushes, "cut" trees have this status ? Unit no longer receives damage except blast damage (trees/animals destroyed by catapults...)
		// Unit's graphic is now dying graphic (unitDef+0x1C), dying sound is played when status becomes 3.
		// Go to status=4 if unit has a dyingAnimation1. Otherwise, this status has the same role as status=5 (in this case, go directly to 6 or 7 once depleted).
		GUS_3_WAIT_RESOURCE_DEPLETION_NO_DYING_ANIMATION = 3,
		GUS_4_DYING_ANIMATION = 4, // Dying animation is being run (?)
		GUS_5_WAIT_RESOURCE_DEPLETION_AFTER_ANIMATION = 5, // Unit keeps this status until owned resources are depleted (down to 0), depending on resourceDecay and being gathered by some unit. Unit only produces visibility on its own tile ?
		GUS_6_DYING_ANIMATION_2 = 6, // "undead animation". Used only when useUndeadStatus=true (NOT used in standard game). This status uses dyingGraphic2.
		GUS_7_UNKNOWN_7 = 7, // Unit actual death. Unit is no longer displayed at all and "dead unit" is created.
		GUS_8_INVALID_TO_DESTROY = 8 // Invalid, waiting to be actually removed. In unit.Update() in 0x4A91E0, this status returns 1 so that unit is destroyed in 0x451292.
	};


	// Unsure / to debug
	enum ACTION_STATUS : char {
		CST_AS_01_NONE = 1, // ?? 40544D. Considered as idle (0x40662A)
		CST_AS_02_DONE = 2, // Seen when moving was finished? (idle). For gen. wonder victory, means wonder is unfinished?
		CST_AS_03_SEARCH = 3, // Can start moving to action location (temporary state) ? 40547F. Considered idle !
		CST_AS_04_GOTO = 4, // Moving to action location ? (a transitional state)
		CST_AS_05_GOTO2 = 5,
		CST_AS_06_WORK = 6, // "start" in progress ? (analog to 3?). For gen. wonder victory, means timer has started? (a transitional state)
		CST_AS_07_WORK2 = 7, // in progress ? (analog to 4?). For both attack, build, etc. Corresponds to the moment reload timer is reset.
		CST_AS_08_RETURN = 8, // Return to deposit when fully loaded (cf action.target2): gathering,trading. Projectiles use it too ?
		CST_AS_09_TURN = 9, // ???
		CST_AS_0A_DELAY = 0x0A, // Seen on stuck villager (repairman that could not get to target)
		CST_AS_0B_MOVE = 0x0B, // Seen when moving to bird location (after killing it). For transport too in some case ?
		CST_AS_0C_ATTACK = 0x0C, // waiting for requirement ? Really unsure
		CST_AS_0D_FAILED = 0x0D, // Set when unit can't move (speed==0) ?? 0x405472. This status is an Idle status. Related to notification 1F9(failed)
		CST_AS_0E_INVALIDATED = 0x0E, // Considered idle. Related to notificationId 1FB.
		CST_AS_0F_MOVE_NO_SEARCH = 0x0F // Seen while moving (just moving). 0x4075D6.
	};

	/* Action update return values
	0 = action continues ? 0x405830
	1 = action finished / restore other action if any ? 0x40586C
	2 = action completed, set to idle ? 0x405614, 0x405865
	3 = action failed ? 0x405793
	4 = action invalidated ? 0x40559C
	5 = ? 0x40552F, 0x405575. When target unit no longer exists?
	*/

	enum INTERNAL_COMMAND_ID : char {
		CST_ICI_RIGHT_CLICK = 0, // Some kind of joker: for right-click: will guess command according to unit def interaction, target, etc
		CST_ICI_STOP_ACTION = 1, // This command is concerned by farm bug in 1.0b version (resets food). 1.0c "Hack" fix is in 0x42A75B.
		CST_ICI_WORK = 2, // task ? see 0x42A780 "work"
		CST_ICI_MOVE = 3, // 0x42BA70
		CST_ICI_CREATE = 4, // Create a fully built building (not construction step). Also works to create other units. No check is made before creating unit !
		CST_ICI_ADD_RESOURCE = 0x05, // Only used for AI "personality resource bonus" ? "add attribute"
		CST_ICI_FORMATION = 0x06, // different from group ? Walk in formation ? // 0x42BB60
		CST_ICI_GIVE_RESOURCE = 0x07, // Give a resource amount from a player to another. Tribute taxes are NOT supported here.
		CST_ICI_CREATE_GROUP = 0x08, // group units 0x42BBE0. Used by AI groups.
		CST_ICI_REMOVE_GROUP = 0x09, // "destroy group". Used by AI groups.
		CST_ICI_TASK_UNIT = 0x0A, // AI order.
		CST_ICI_RESIGN = 0x0B, // Only for MP games when clicking resign in menu. See exec=42B290
		CST_ICI_ADD_ONE_INTERMEDIATE_MOVEMENT_STEP = 0x0C, // Unused ?
		CST_ICI_UNKNOWN_D = 0x0D, // unused ?
		CST_ICI_ADD_TO_GROUP = 0x0E, // exec=0x42B4A0. Add to group (cf numeric shortcuts?)
		CST_ICI_REMOVE_FROM_GROUP = 0x0F, // exec=0x42B4E0 0x42BC80. Remove from group (cf numeric shortcuts?)
		CST_ICI_ADD_GRP_INTERMEDIATE_MOVEMENT_STEP = 0x10, // When shift-right clicking to specify intermediate movement step
		CST_ICI_GROUP_AI_TASK = 0x11, // To confirm (old: hold position was probably wrong)
		CST_ICI_TRIGGER_TRAIN_UNIT = 0x64, // unsure, when AI triggers a train unit action ? + some cheats?
		CST_ICI_RESEARCH = 0x65,
		CST_ICI_BUILD = 0x66,
		CST_ICI_CHANGE_SETTING = 0x67, // Includes cheats(6), game speed change(1), diplomacy change(0)
		CST_ICI_EXPLORE = 0x68, // Not sure this does more than just moving to the specified position
		CST_ICI_BUILD_MULTIPLE = 0x69,// Build wall-like construction (many contiguous building units)
		CST_ICI_KILL_UNIT = 0x6A, // "Cancel build"
		CST_ICI_ATTACK_POSITION = 0x6B, // For example, used for catapults...
		CST_ICI_PAY_TRIBUTE = 0x6C,
		CST_ICI_SET_TRADE_RESOURCE_TYPE = 0x6D, // Set resource to trade in a trade ship.
		CST_ICI_REPAIR = 0x6E, // Repair (using repair button, not via right-click)
		CST_ICI_UNLOAD_TRANSPORT = 0x6F, // Unload units from transport units
		CST_ICI_QUEUE_UNQUEUE_UNIT = 0x77, // Only for human-called "train unit" ? Include right-click to unqueue.
		CST_ICI_ADD_SCORE_ELEM = (char)0xFF // A special one...
	};
	enum INTERNAL_COMMAND67_SUBTYPE : char {
		CST_IC67_CHANGE_DIPLOMACY = 0,
		CST_IC67_CHANGE_GAME_SPEED = 1,
		CST_IC67_ADD_RESOURCE = 2, // "inventory"
		CST_IC67_UPGRADE_TOWN = 3, // Unused
		CST_IC67_SET_STEROIDS_MODE = 4, // "quick build"
		CST_IC67_SET_ALLY_VICTORY = 5,
		CST_IC67_CHEAT = 6,
		CST_IC67_APPLY_WRITING_TECH = 7, // set shared exploration
		CST_IC67_SYNC_ERROR = 8 // Save game to syncerr?.gam (!) and quit game + sync error message. Saved game seems NOT to be playable
	};

	enum INTERNAL_UNIT_FORMATION_ID : long int {
		CST_UFI0_LEFT_RIGHT_AXIS = 0, // Using space=1 align on +X+Y diagonal (left to right?)
		CST_UFI1_BOTTOM_TOP_AXIS = 1, // Using space=1.5 align on -X+Y diagonal (bottom to top?)
		CST_UFI2_Y_AXIS = 2, // Using space=2, align on Y axis
		CST_UFI3_X_AXIS = 3, // Using space=2, align on X axis
		CST_UFI4_UNKNOWN_V = 4, // Using space=1.5? Draws a "V" ?
		CST_UFI5_UNKNOWN_V = 5 // Using space=1.5? Draws a "V" ?
	};

	// Identifies cheats in command 0x67/subtype 6.
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


	// See 0x507E72 for research correspondance
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

	// General victory condition choice. WARNING: in game global, 2 values are inverted.
	enum GENERAL_VICTORY_CONDITION : long int {
		GVC_STANDARD = 0,
		GVC_CONQUEST = 1,
		GVC_SCORE = 2,
		GVC_TIME_LIMIT = 3,
		GVC_CUSTOM = 4
	};

	// To be used only in scenario information.
	enum SCENARIO_INFO_AGE_ID : long int {
		SIA_NONE = 0,
		SIA_STONE_AGE = 1,
		SIA_TOOL_AGE = 2,
		SIA_BRONZE_AGE = 3,
		SIA_GOLD_AGE = 4
	};

	// To be used only in scenario information: those resource IDs don't match the "official" ones.
	enum SCENARIO_INFO_RESOURCE_TYPE : long int {
		WRT_NONE = -1, // For custom usage
		WRT_GOLD = 0,
		WRT_WOOD = 1,
		WRT_FOOD = 2,
		WRT_STONE = 3,
		WRT_POPULATION = 4,
		WRT_AGE = 5, // Here stone age=1, tool=2, bronze=3, iron=4
		WRT_EXPLORATION = 6
	};

	// Types of individual victory conditions
	enum INDIVIDUAL_VICTORY_CONDITION : long int {
		IVC_NONE = 0, // Not set.
		IVC_DESTROY_OBJECT_OR_PLAYER = 1, // If only targetPlayerId is set (non-zero), then "kill a player"
		IVC_CREATE_OBJECTS = 2, // Create some amount, in a zone...
		IVC_MOVE_OBJECT = 3, // To other object, to zone...
		IVC_SPECIFIC_RESOURCE = 4, // Reach some "resource" amount (gold, exploration, age...), using non-standard resource IDs: IDs: 0=gold/wood/food/stone/4=pop/5=age/6=explore
		IVC_CAPTURE_OBJECT = 5,
		IVC_STANDARD_RESOURCES = 6, // In this case, resourceType actually refers to resource IDs !
		IVC_DO_SPECIFIC_RESEARCH = 7 // Do a specific research, given its research ID (NOT techId)
	};

	// See 0x4DF84C
	enum UNIT_GROUP_TYPES : long int {
		CST_UGT_LAND_ATTACK = 0x64, // (100) Related to SNMaximumAttackGroupSize
		CST_UGT_LAND_DEFEND = 0x65, // (101) Related to SNMaximumDefendGroupSize
		CST_UGT_LAND_EXPLORE = 0x66, // (102) Related to SNMaximumExploreGroupSize. NOT for exploring villagers (?). See SNNumberExploreGroups
		CST_UGT_BOAT_ATTACK = 0x67, // (103) Related to SNMaximumBoatAttackGroupSize
		CST_UGT_BOAT_DEFEND = 0x68, // (104) Related to SNMaximumBoatDefendGroupSize
		CST_UGT_BOAT_EXPLORE = 0x69, // (105) Related to SNMaximumBoatExploreGroupSize. NOT for exploring fishing ships
		// Next ones : trade/fish/transport escorts ? Just a supposition
		CST_UGT_TRANSPORT_BOAT = 0x6A, // (106) Transport boats (1 per group). Using task 12 to transport units.
		CST_UGT_FISHING_SHIP = 0x6B, // (107) Fishing ships (1 per group) Can go fishing(10), exploring(8). Can have idle task(1) but still fishing anyway !
		CST_UGT_TRADE_SHIP = 0x6C, // (108) Trade ships (1 per group ?). Task 11 = trade.
		CST_UGT_ARTEFACT = 0x6D // (109) Seen with target=own TC. There is 1 group per relic ?
		// It *seems* there is no other group type
	};

	enum UNIT_GROUP_TASK_IDS : long int {
		CST_UGT_NOT_SET = 0, // stop ? 0x4CD349
		CST_UGT_IDLE = 1, // Idle ? Set in 4CD110. Default=1 (0 in constructor, but 1 after init)
		CST_UGT_ATTACK_02 = 2, // The basic ATTACKING task (used in status and in TaskGroup method). Using this in tasking call does not guarantees units won't change target on their way.
		CST_UGT_RETREAT = 3, // 4CD640, 0x4D51DF
		CST_UGT_DEFEND_UNIT = 4, // 4CE309, 0x4D5E6D
		CST_UGT_UNKNOWN_05 = 5, // "add up HPs of my units" ?
		CST_UGT_UNKNOWN_06 = 6, // "add up HPs of my units" ?
		CST_UGT_EXTERMINATE = 7, // This is used to set an "explore" attack mode without a specific target. Leader=explorer, others=defenders. This explores from current position, does not use target pos ? 0x4CD7A9. Set in 0x4D50AB, 0x4D5DD7 See 0x4D535D
		CST_UGT_EXPLORE = 8, // 4CD977. For boats too. NOT for civilians/fishing ships.
		
		// 0x4CD425. Uses SNAttackGroupGatherSpacing or artefact return distance. See also CST_UGT_REGROUP_NO_SPACING.
		// Assigned to idle groups when some units are far from leader.
		// The "regroup" location is unitGroup.posX/Y.
		// Uses unit ORDER 0x2D4 to operate regrouping.
		CST_UGT_REGROUP = 9,
		CST_UGT_GO_FISHING = 0x0A, // (10) Active fishing ships may also have an idle (1) group task ! See 0x4CE4AB
		CST_UGT_TRADE = 0x0B, // (11) 0x4CE651. targetUnitId is (always) other player's dock.
		CST_UGT_TRANSPORT_UNITS = 0x0C, // (12) transport units, includes: go to rally point, wait/load, transport and unload phases. See 0x4D5130. Group has no target unit/player, but a target destination
		CST_UGT_UNKNOWN_0D = 0x0D, // (13) See 4D4729,4D5F32. Waiting for a free transport ? really unsure
		
		// (14) 0x4CD425, 0x4D3DAC,4D3F95,4D44E0.
		// Similar to regroup, but does not use SNAttackGroupGatherSpacing. Always(?) followed by task 0xD?
		// The "regroup" location is unitGroup.posX/Y.
		// Uses unit ORDER 0x2D4 to operate regrouping.
		CST_UGT_REGROUP_NO_SPACING = 0x0E,

		CST_UGT_LOAD_INTO_TRANSPORT = 0x0F, // (15) Go to transport. When transport is full, remaining units are excluded from group (?). 0x4D484F,0x4D48EE.
		CST_UGT_IN_TRANSPORT = 0x10, // (16) In a transport/waiting to be unloaded. See 0x4D4D03.
		CST_UGT_UNKNOWN_11 = 0x11, //
		CST_UGT_UNKNOWN_12 = 0x12, // 
		CST_UGT_UNKNOWN_13 = 0x13, // (19) Send artefacts to TC ? See 0x4D5140,0x4D38F3.
		CST_UGT_ATTACK_ROUNDUP_TARGET = 0x14, // Focused attack on a specific target. Units will avoid being distracted by other potential nearby targets. Incompatible with attackPlayId (must be -1). Wait 10s from last attack tasking time. Used for tasking method, not as a "group current task" (grp task is 2) ?
		CST_UGT_ATTACK_15 = 0x15, // (21) Attack, just orders a MoveTo without a specific target? . 0x4CE12F. Used for tasking method, not as a "group current task" (grp task is 2) ?
		CST_UGT_UNKNOWN_16 = 0x16 // Set in 0x4D4C5F
	};

	// See 0x4D1BB5 (tacAI.Update), 0x4D0880 (get name)
	enum AI_UPDATE_TYPES : long int {
		CST_AUT_SETUP_SOLDIER_GROUPS = 0, // "Setup SolGroups"
		CST_AUT_EVAL_CIVILIAN_DISTRIB = 1, // "Eval CivDist". Compute desired count of villager activities.
		CST_AUT_TASK_CIVILIAN = 2, // "Task Civs". Assign activities to villagers
		CST_AUT_SET_BOAT_GROUPS = 3, // "Setup BoatGroups"
		CST_AUT_FILL_BOAT_GROUPS = 4, // "Fill BoatGroups"
		CST_AUT_TASK_BOATS = 5, // "Task Boats"
		CST_AUT_FILL_SOLDIER_GROUPS = 6, // "Fill SolGroups"
		CST_AUT_TASK_IDLE_SOLDIER = 7, // "Task IdleSol"
		CST_AUT_TASK_ACTIVE_SOLDIER = 8, // "Task ActSol"
		CST_AUT_PLAYTASKING = 9, // "Playtasking" - WTF ??? special attacks?
		//CST_AUT_TASK_UNGRP_SOLDIER = 10, // "TaskUngrpSol" Wrong label in ROR code ???
		CST_AUT_ATTACK_ENEMY_TOWER_IN_MY_TOWN = 10,
		CST_AUT_RESEARCH = 11, // "Research"
		CST_AUT_TRAIN = 12, // "Train"
		CST_AUT_BUILD_LIST = 13, // "BldLst" = load other strategy file (never used in standard game)
		CST_AUT_HELP_BUILD = 14, // "Help Build"
		CST_AUT_REPAIR_BUILDING = 15, // "Repair Bldg"
		CST_AUT_REPAIR_WALL = 16, // "Repair Wall"
		CST_AUT_BUILD = 17, // "Build" = trigger new constructions, cf SNInitialExplorationRequired, SNCapCivilianBuilders... Prepare info in ConAI ?
		CST_AUT_OPEN_BUILDS = 18, // "Open Builds". Actually starts constructions ?
		CST_AUT_OPEN_TASKS = 19, // "Open Tasks". Various stuff like tributes, trade ships, "upgrade age ASAP"
		CST_AUT_FOOD_DROPSITE = 20, // "Food Dropsite"
		CST_AUT_BUILD_LIST_INSERTIONS = 21 // "BldLst Insertions" 0x15 / 0x4D240C. Add the first SP/granary + add tower cf SNMaxTower + add farms if iron&all buildings finished & cf SNMaxFarms
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

	// Tile exploration status for infAI
	enum INFAI_TILE_EXPLORATION_STATUS : char {
		CST_TES_UNEXPLORED = -1,
		CST_TES_EXPLORED = 0,
		CST_TES_FAILED = 1, // Failed getting to location
		CST_TES_TO_REEXPLORE_MILITARY = 2 // marked for re-exploration by military units
		// 0xF9 ??? 0x5179F1
	};


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

	// Unit interaction ID. See also UNIT_INTERACTION_MASK_BIT for combinations of such values.
	// Used to guess mouse cursor to set when hovering on units.
	enum UNIT_INTERACTION_ID : long int {
		UII_NO_INTERACTION = 0,
		UII_PHYSICAL_ATTACK = 1, // Attacks other than conversion. Only walls have this setting instead of UII_ATTACK_GENERIC ?
		UII_ENTER_TRANSPORT = 2,
		UII_CAPTURE_ARTEFACT = 3,
		UII_HEAL = 4, // unsure
		UII_REPAIR_BUILD = 5, // unsure
		UII_GATHER_RESOURCES = 6, // NOT gather ?
		UII_CONVERT = 7, // convert?
		UII_ATTACK_GENERIC = 8, // includes convert and standard attack ?
		UII_ATTACK_ANIMAL = 9, // Hunt for villagers, attack for other units
		UII_COUNT
	};

	// Each value represents a bit in unit interaction masks. For example first (lowest) bit means "can attack".
	// Most military units have 7 (attack + U2 +U4) ; not warships. Priests have 0x96 (2+4+0x10+0x80). See switch in 0x4F6F9F.
	// See also UNIT_INTERACTION_ID for individual values (indexes)
	enum UNIT_INTERACTION_MASK_BIT : unsigned long int {
		UIM_NO_INTERACTION = 0, // When no bit is set, there is no possible interaction
		UIM_MILITARY_ATTACK = 1, // Sword: NOT for civilian hunting
		UIM_ENTER_TRANSPORT = 2, // Bits 10
		UIM_CAPTURE_ARTEFACT = 4, // Bits 100. Can capture relics/ruins (same cursor as priest actions)
		UIM_UNKNOWN_8 = 8, // Bits 1000. Unused ?
		UIM_HEAL = 16, // Bits 10000 0x10. Priest's healing ability.
		UIM_REPAIR_BUILD = 32, // Bits 100000 0x20. Build or repair ability
		UIM_GATHER_RESOURCES = 64, // Bits 1000000 0x40. Gather from resource units, including hunting animals.
		UIM_CONVERT = 128 // Bits 1000000 0x80
	};

}
