
#pragma once
#include <tchar.h>

/* This file contains all functional constants for Age of Empires
 * These values can be used in rules.rps, AI files, AGE3 (empires.dat)...
 * Some of them are only used in game EXE.
 * They do not depend on the EXE version.
 * See also AOE_SN_numbers.h, AOE_empires_dat.h
*/

namespace AOE_CONST_FUNC
{

	// Values for TAIUnitClass (strategy elements class)
	enum TAIUnitClass : long int {
		AIUCNone = -1, // For internal usage
		AIUCBuilding = 0,
		AIUCTech = 1,
		AIUCLivingUnit = 2,
		AIUCLoadNewStrategy = 3, // See 4D1E8D. Supported but undocumented and unused !
		AIUCCritical = 4, // "C" techs (critical: do not skip, AI MUST first do this before looking after)
		AIUCEnableAttack = 5, // Unused but supported ! When "executed", this strategy element sets enableAttack=true in AI (AI will start attacking other players)
		AIUCStrategyCmd = 6 // Dynamically added items for setGatherPercentage commands
	};


	// Score categories
	enum SCORE_CATEGORIES : char {
		CST_SC_MILITARY = 0,
		CST_SC_ECONOMY = 1,
		CST_SC_RELIGION = 2,
		CST_SC_TECHNOLOGY = 3,
		CST_SC_SURVIVAL = 4
		// Wonder = 5 ??
	};

	enum SCORE_TYPE : char {
		CST_ST_KILLS_COUNT = 0,
		CST_ST_RAZINGS_COUNT = 1,
		CST_ST_KILL_RATIO = 2,
		CST_ST_LARGEST_ARMY = 3,
		CST_ST_GOLD_GATHERED =  4,
		CST_ST_MAX_VILLAGER_COUNT = 5,
		CST_ST_VILLAGER_BONUS = 6,
		CST_ST_EXPLORATION = 7,
		CST_ST_MOST_EXPLORED = 8,
		CST_ST_TRIBUTE_PAID = 9,
		CST_ST_CONVERSIONS_COUNT = 0x0A,
		CST_ST_MOST_CONVERSION = 0x0B,
		CST_ST_RUINS_OWNED = 0x0C,
		CST_ST_RELICS_OWNED = 0x0D,
		CST_ST_TEMPLES_COUNT = 0x0E,
		CST_ST_ALL_RUINS = 0x0F,
		CST_ST_ALL_RELICS = 0x10,
		CST_ST_TECHNOLOGIES_COUNT = 0x11,
		CST_ST_MOST_TECHNOLOGIES = 0x12,
		CST_ST_FIRST_TO_BRONZE_AGE = 0x13,
		CST_ST_FIRST_TO_IRON_AGE = 0x14,
		CST_ST_SURVIVAL = 0x16,
		CST_ST_STANDING_WONDER = 0x17
	};
	
	// Internal values for AI control mode (strategy, villager tasks...) This is NOT "is_computer_controlled"
	const static int CST_AI_CONTROL_MODE_HUMAN = 1;
	const static int CST_AI_CONTROL_MODE_GAIA = 2;
	const static int CST_AI_CONTROL_MODE_COMPUTER = 3;

	// Diplomacy values (diplomacy struct)
	const static int CST_DIPL_ALLIED = 0;
	const static int CST_DIPL_NEUTRAL = 1;
	const static int CST_DIPL_UNUSED = 2; // Do not use, game code seems not to use/support it.
	const static int CST_DIPL_ENEMY = 3;
	// Diplomacy values (flags)
	//const static int CST_DIPLAI_FLAG_DIPL_ALLIED = 0;
	//const static int CST_DIPLAI_FLAG_DIPL_ENEMY = 1;

	// Like/Dislike initial values
	const static int CST_DISLIKE_INITIAL_VALUE = 74; // "init2" hardcoded initial value for Dislike against non-allied player
	const static int CST_LIKE_INITIAL_VALUE = 24;    // "init2" hardcoded initial value for Like against non-allied player
	const static int CST_DISLIKE_MAX_VALUE = 100;    // it is also controlled/fixed by the game

	// Alive / lost
	const static int CST_PLAYER_IS_ALIVE = 0;
	const static int CST_PLAYER_HAS_LOST = 2;

	// Ages 1=stone 2=tool 3=bronze 4=iron
	const static int CST_AGE_STONE = 1;
	const static int CST_AGE_TOOL = 2;
	const static int CST_AGE_BRONZE = 3;
	const static int CST_AGE_IRON = 4;


	// Researches status (EXE internal values)
	enum RESEARCH_STATUSES : short int {
		CST_RESEARCH_STATUS_DISABLED = -1, // The research is disabled for this player
		CST_RESEARCH_STATUS_WAITING_REQUIREMENT = 0, // The research exists in the tech tree but is not visible yet. A requirement is missing.
		CST_RESEARCH_STATUS_AVAILABLE = 1, // The research is available and can be researched now
		CST_RESEARCH_STATUS_BEING_RESEARCHED = 2, // The research is currently being researched
		CST_RESEARCH_STATUS_DONE_OR_INVALID = 3 // The research has been researched or is invalid. It has no more role/interaction (effect is active)
	};

	// Internal Action Ids. See INTERNAL_ACTION_ID in AOE_const_internal.
	

	enum TECH_DEF_EFFECTS : char {
		TDE_INVALID = -1,
		TDE_ATTRIBUTE_MODIFIER_SET = 0,
		TDE_RESOURCE_MODIFIER_ADD = 1, // add value
		TDE_ENABLE_DISABLE_UNIT = 2,
		TDE_UPGRADE_UNIT = 3,
		TDE_ATTRIBUTE_MODIFIER_ADD = 4, // add value
		TDE_ATTRIBUTE_MODIFIER_MULT = 5, // multiply by a value
		TDE_RESOURCE_MODIFIER_MULT = 6, // multiply by a value
		TDE_RESEARCH_COST_MODIFIER = 101,
		TDE_DISABLE_RESEARCH = 102,
		TDE_RESEARCH_TIME_MODIFIER = 103
	};

	enum INTERACTION_MODES : char {
		CST_IM_NONE = 0, // Unable to select, move, attack. Eye candy, dead units, projectiles...
		CST_IM_CAPTURE_ONLY = 1, // Unable to select, move, attack, but can capture (used in 162, 330: flags)
		CST_IM_RESOURCES = 2, // Can select, only villagers can target (+ military unit for animals...). Animals, trees, fish, mines, bushes.
		CST_IM_BUILDINGS = 3, // Can select, attack, unable to move. Buildings and ruins.
		CST_IM_LIVING_UNITS = 4 // Can select, attack, move. All living units/boats + some towers. NOT including animals.
	};

	enum COMMAND_ATTRIBUTES : char {
		CST_CA_NONE = 0, // Eye candy, dead units, projectiles, flags...
		CST_CA_GATHERABLE_TARGETS = 1, // Animals, trees, fish, mines, bushes.
		CST_CA_BUILDINGS = 2, // All buildings, including towers
		CST_CA_VILLAGERS = 3,
		CST_CA_MILITARY_UNITS = 4, // Water and land military units
		CST_CA_TRADE_UNITS = 5, // Trade ships only
		CST_CA_PRIESTS = 6,
		CST_CA_TRANSPORTS = 7, // Transport boats
		CST_CA_ARTEFACTS = 8, // Relics and ruins
		CST_CA_FISHING_SHIPS = 9,
		CST_CA_UNUSED_10 = 10, // Defined in AGE3, but unused in AOE/ROR
		CST_CA_UNUSED_11 = 11 // Defined in AGE3, but unused in AOE/ROR
	};

	// Note on blast levels: default should be 3, not 0.
	// Uing 0 for the most damaging case is a bad choice from game developers ! 0 should have been for TARGET_ONLY.
	// A lot of units have 0, 3 would be more appropriate.
	enum BLAST_LEVELS : char {
		// For many units (buildings, etc), but also boats ! No effect if blast radius is 0 though.
		// 0 damages trees, animals, and make mines/fish disappear !
		CST_BL_DAMAGE_RESOURCES = 0,
		CST_BL_DAMAGE_TREES = 1, // Heavy cats, Juggernaught...
		CST_BL_DAMAGE_NEARBY_UNITS = 2, // stone thrower, catapult
		CST_BL_DAMAGE_TARGET_ONLY = 3 // For most units !
	};
}
