
#pragma once
#include <tchar.h> // _T()

/*
 * This file contains public functional values that generally correspond to empires.dat data.
*/

namespace AOE_CONST_FUNC {

	// All game civilizations (technically, from empires.dat, but sometimes hardcoded in game)
	enum CIVILIZATIONS : char {
		CST_CIVID_GAIA = 0x00,
		CST_CIVID_EGYPTIAN = 0x01,
		CST_CIVID_GREEK = 0x02,
		CST_CIVID_BABYLONIAN = 0x03,
		CST_CIVID_ASSYRIAN = 0x04,
		CST_CIVID_MINOAN = 0x05,
		CST_CIVID_HITTITE = 0x06,
		CST_CIVID_PHOENICIAN = 0x07,
		CST_CIVID_SUMERIAN = 0x08,
		CST_CIVID_PERSIAN = 0x09,
		CST_CIVID_SHANG = 0x0A,
		CST_CIVID_YAMATO = 0x0B,
		CST_CIVID_CHOSON = 0x0C,
		CST_CIVID_ROMAN = 0x0D,
		CST_CIVID_CARTHAGE = 0x0E,
		CST_CIVID_PALMYRA = 0x0F,
		CST_CIVID_MACEDONIAN = 0x10,
		CST_CIVID_STANDARD_MAX = 0x10
	};


	// Global unit types (AGE3 = type)
	// WARNING: datatype is not always consistent for these (1-byte or 2-bytes or 4-bytes) ?
	enum GLOBAL_UNIT_TYPES : char {
		GUT_EYE_CANDY = 10,
		GUT_FLAGS = 20,
		GUT_DOPPLEGANGER = 25, // temporary objects?
		GUT_DEAD_UNITS = 30, // includes resources like "cut" trees, mines... Dead/fish in AGE3.
		GUT_BIRD = 40,
		GUT_TYPE50 = 50,
		GUT_PROJECTILE = 60,
		GUT_LIVING_UNIT = 70,
		GUT_BUILDING = 80,
		GUT_TREE = 90 // "not cut" trees
	};


	// Unit (type) numbers - From rules.rps - corresponds to "class" in AGE3
	enum GLOBAL_UNIT_AI_TYPES : short int {
		TribeAINone = -1, // use as a joker in some methods
		TribeAIGroupArcher = 0, // bowman + improved/composite
		TribeAIGroupBuilding = 3,
		TribeAIGroupCivilian = 4, // All villager excluding boats !
		TribeAIGroupFootSoldier = 6, // Barracks infantry
		TribeAIGroupMountedSoldier = 12, // 0xC ; cavalry
		TribeAIGroupSiegeWeapon = 13, // 0x0D
		TribeAIGroupPriest = 18, // 0x12
		TribeAIGroupFishingBoat = 21, // 0x15
		//TribeAIGroupHeavyMountedSoldier = 23, // 0x17 - name from rules.rps, not very exact/explicit (only chariot archer has it)
		TribeAIGroupChariotArcher = 23, // 0x17. Only chariot archer
		TribeAIGroupElephantRider = 24, // 0x18. war elephant + armored elephant, including hannibal
		TribeAIGroupHeavyFootSoldier = 25, // 0x19 - Hero_Archimedes, Hero-Tiberius, Hero-Perseus, Hero-Xerxes, Hero_Jason
		TribeAIGroupElephantArcher = 26, // 0x1A
		TribeAIGroupPhalanx = 28, // 0x1C
		TribeAIGroupTradeBoat = 2,
		TribeAIGroupTransportBoat = 20, // 0x14
		TribeAIGroupWarBoat = 22, //0x16 - Includes fire galley, Jagannath...
		TribeAIGroupWall = 27,  // 0x1B
		// Added from AGE3 information
		TribeAIGroupArtefact = 1, // ruins/artefacts
		TribeAIGroupUnused = 17, // 0x11. TODO : mistake ?
		TribeAIGroupChariot = 35, // 0x23. Chariot/scythe (excluding chariot archer)
		TribeAIGroupHorseArcher = 36, // 0x24
		TribeAIGroupSlinger = 39, // 0x27
		TribeAIGroupUnused_Farm = 49, // 0x31 UNUSED ? Farms are buildings (3) !
		TribeAIGroupUnused_Tower = 52, // 0x34 UNUSED ? Towers are buildings (3) ! Don't use this number in your algorithms ;)
		// Gatherable
		TribeAIGroupSeaFish = 5,
		TribeAIGroupBerryBush = 7,
		TribeAIGroupStoneMine = 8,
		TribeAIGroupPreyAnimal = 9,
		TribeAIGroupTree = 15, // 0xF
		TribeAIGroupGoldMine = 32, // 0x20
		TribeAIGroupShoreFish = 33, // 0x21
		TribeAIGroupPredatorAnimal = 10, // 0xA
		TribeAIGroupUnknownFish = 31, // 0x1F - a gatherable unused fish type.
		// Other
		TribeAIGroupOther_Dead_Projectile = 11, // 0xB
		TribeAIGroupTerrain = 14, // 0x0E
		TribeAIGroupTreeStump = 16, // 0x10
		TribeAIGroupTradeCart = 19, // 0x13. It is a villager type (seen in 0x485ADD).
		TribeAIGroupFlag = 30,
		TribeAIGroupCliff = 34, // 0x22
		TribeAIGroupBird = 38,
		TribeAIGroupHorse = 61,
		TribeAIGroupDomesticatedAnimal = 29 // Lion_trained (unit 60)
		//37=smoke? (dolphin??)
	};


	// Basic resource types
	static const short int MAX_RESOURCE_TYPE_ID = 57;
	enum RESOURCE_TYPES : short int {
		// Basic resources
		CST_RES_ORDER_FOOD = 0,
		CST_RES_ORDER_WOOD = 1,
		CST_RES_ORDER_STONE = 2,
		CST_RES_ORDER_GOLD = 3,
		// Other resources
		CST_RES_ORDER_POPULATION_HEADROOM = 4,
		CST_RES_BASIC_RESOURCE_COUNT = 4, // many locations use these 4 resources, including AI structures. Put after pop headroom for practical debugging display.
		CST_RES_ORDER_UNKNOWN_05 = 5, // ?
		CST_RES_ORDER_CURRENT_AGE = 6, // TO CONFIRM
		CST_RES_ORDER_RELICS = 7, // TO CONFIRM,
		CST_RES_ORDER_GAME_TIME = 9, // in seconds WRONG ? Housage ?
		CST_RES_ORDER_TRADE_PRODUCTION = 10, // Default 1 ?
		CST_RES_ORDER_CURRENT_POPULATION = 11,
		CST_RES_ORDER_CORPSE_DECAY_TIME = 12, // TO CONFIRM. Remains 0 in game ?
		CST_RES_ORDER_DISCOVERIES = 13, // TO CONFIRM
		CST_RES_ORDER_RUINS = 14, // TO CONFIRM
		CST_RES_ORDER_MEAT_STORAGE = 15, // TO CONFIRM. Remains 0 in game ?
		CST_RES_ORDER_BERRY_STORAGE = 16, // TO CONFIRM
		CST_RES_ORDER_FISH_STORAGE = 17, // TO CONFIRM
		CST_RES_ORDER_TOTAL_UNITS_OWNED = 19,
		CST_RES_ORDER_KILLS = 20,
		CST_RES_ORDER_TECHNOLOGIES = 21, // Number of researched technologies (as in score)
		CST_RES_ORDER_MAP_EXPLORATION = 22, // Percentage
		CST_RES_ORDER_BRONZE_AGE = 23,
		CST_RES_ORDER_IRON_AGE = 24,
		CST_RES_ORDER_TOOL_AGE = 25,
		CST_RES_ORDER_ATTACK_ALERT_SOUND_ID = 26,
		CST_RES_ORDER_CAN_CONVERT_PRIEST = 27,
		CST_RES_ORDER_CAN_CONVERT_BUILDING = 28,
		CST_RES_ORDER_BUILDING_COUNT = 30, // TO confirm. Init=500, a building limit?
		CST_RES_ORDER_POPULATION_LIMIT = 32, // offset +0x80
		CST_RES_ORDER_FAITH = 34, // 0x22
		CST_RES_ORDER_FAITH_RECHARGING_RATE = 35, // default 2 ?
		CST_RES_ORDER_FARM_FOOD_AMOUNT = 36, // default 250 ?
		CST_RES_ORDER_CIVILIAN_POPULATION = 37, // Current number of villagers. Counts TribeAIGroupCivilian, TribeAIGroupFishingBoat, TribeAIGroupTradeBoat
		CST_RES_ORDER_ALL_TECHNOLOGIES = 39,
		// Current number of military units, including towers, living units except (TribeAIGroupPriest, TribeAIGroupCivilian, TribeAIGroupFishingBoat, TribeAIGroupTradeBoat).
		// All units count as 1 here, even infantry with logistics.
		CST_RES_ORDER_MILITARY_POPULATION = 40,
		CST_RES_ORDER_CONVERSIONS = 41,
		CST_RES_ORDER_STANDING_WONDERS = 42,
		CST_RES_ORDER_RAZINGS = 43,
		CST_RES_ORDER_KILL_RATIO = 44,
		CST_RES_ORDER_SURVIVAL = 45,
		CST_RES_ORDER_TRIBUTE_COST = 46, // 0.25 default (0x45B991), 0 for palmyra or all techs.
		CST_RES_ORDER_GOLD_MINING_PODUCTIVITY = 47, // 1 default ?
		CST_RES_ORDER_TOWN_CENTER_UNAVAILABLE = 48,
		CST_RES_ORDER_GOLD_COUNTER = 49,
		CST_RES_ORDER_SHARED_EXPLORATION = 50,
		CST_RES_ORDER_HOUSES_AMOUNT = 51,
		CST_RES_ORDER_TEMPLE_COUNT = 52,
		CST_RES_ORDER_TRIBUTE_SENT = 53,
		CST_RES_ORDER_ALL_RUINS = 54,
		CST_RES_ORDER_ALL_RELICS = 55,
		CST_RES_ORDER_HEALING = 56, // Affect priests healing rate, see tech 211 (medecine)
		CST_RES_ORDER_PRIEST_SACRIFICE = 57 // If 1, you can delete (sacrifice) priest while converting to make conversion successful. See tech 212.
	};


	// Game map type indexes
	enum MAP_TYPE_INDEX : long int {
		MTI_SMALL_ISLANDS = 0,
		MTI_LARGE_ISLANDS = 1,
		MTI_COASTAL = 2,
		MTI_INLAND = 3, // "continental" in french !!!
		MTI_HIGHLAND = 4,
		MTI_CONTINENTAL = 5, // "continent" in french !!!
		MTI_MEDITERRANEAN = 6,
		MTI_HILLS = 7,
		MTI_NARROWS = 8, // "Passages"
		MTI_HIDDEN_MAP_TYPE = 9,
		MTI_MAP_TYPES_COUNT = 10
	};

	enum MAP_WATER_TYPE {
		MWT_UNKNOWN_MAP = -1,
		MWT_ALL_WATER_MAP = 0, // Only for small islands.
		MWT_MOSTLY_WATER_MAP = 1, // Only for large islands
		MWT_LAND_AND_WATER_MAP = 2, // Most common case. AI DOES build dock/boats here.
		MWT_MOSTLY_LAND_MAP = 3, // AI builds no dock/boats in such maps, but there would be enough water to (might be limited to a corner of the map though)
		MWT_ALL_LAND_MAP = 4 // No water at all or just rivers, AI builds no dock/boats here.
	};

	// http://agecommunity.wikia.com/wiki/Attack_Data
	// For both attack and armor types.
	enum ATTACK_CLASS: short int {
		CST_AC_BALLISTA_ON_BUILDINGS = 0,
		CST_AC_SLINGER_ON_ARCHERS = 1, // And on towers/walls too
		CST_AC_BASE_PIERCE = 3, // Common archers attack class
		CST_AC_BASE_MELEE = 4, // The most common attack type
		CST_AC_CATAPULTS_ON_BUILDINGS = 6,
		CST_AC_CHARIOTS_ON_PRIESTS = 7, // Attack class for additional damage on priests
		CST_AC_CAMEL_ON_CAVALRY = 8, // And on chariots too
		CST_AC_CAVALRY_ON_INFANTRY = 9, // on clubmen/short sword + upgrades, NOT on slingers, hoplites...
		CST_AC_VILLAGER_SIEGECRAFT = 10, // Siegecraft effect: villagers bonus against towers/walls
		CST_AC_AGGRESSIVE_ANIMAL = 12, // Animal attacks (lion/lion king + alligator king). Only gazelles have armor against this.
		CST_AC_COUNT
	};

	static const char *attackClassNames[] = { "BallistaVsBuildings", "ArchersWeakness", "", "Pierce", "Melee", "" /*5*/,
		"CatapultsVsBuildings", "PriestWeakness", "CavalryWeakness", "InfantryWeakness", "Siegecraft", "", "AnimalAttack"};
	// Returns true if attack class is a standard one (like base melee/piercee + "standard" siege weapons attacks), false if it represents some bonus/malus
	static bool IsStandardAttack(ATTACK_CLASS a) {
		return (a == CST_AC_BASE_PIERCE) || (a == CST_AC_BASE_MELEE) || (a == CST_AC_BALLISTA_ON_BUILDINGS) || (a == CST_AC_CATAPULTS_ON_BUILDINGS);
	}
	static const char * GetAttackClassName(ATTACK_CLASS a) {
		if ((a < 0) || (a >= CST_AC_COUNT)) { return ""; }
		return attackClassNames[a];
	}

	enum TECH_DEF_EFFECTS : char {
		TDE_INVALID = -1,
		TDE_ATTRIBUTE_MODIFIER_SET = 0,
		TDE_RESOURCE_MODIFIER_ADD_SET = 1, // add or set value (according to "mode" = effectClass field)
		TDE_ENABLE_DISABLE_UNIT = 2,
		TDE_UPGRADE_UNIT = 3, // "source" unit = effectUnit, "target" (upgraded) unit = effectClass
		TDE_ATTRIBUTE_MODIFIER_ADD = 4, // add value
		TDE_ATTRIBUTE_MODIFIER_MULT = 5, // multiply by a value
		TDE_RESOURCE_MODIFIER_MULT = 6, // multiply by a value
		TDE_RESEARCH_COST_MODIFIER = 101,
		TDE_DISABLE_RESEARCH = 102, // "value" contains researchId (as float), other fields unused.
		TDE_RESEARCH_TIME_MODIFIER = 103
	};

	/* Attributes for technology effects
	 * Methods to apply "Add attribute" effects on unitDef: [EDX+C] 0x442370(base), 0x43ED00(flag), 0x440EC0(deadfish), 0x43E7B0(bird), 0x43FA60(type50), 0x4ED010(living). Not for 7,17,18,19,...101?
	 * Methods to apply "Add attribute" effects on unit: [EDX+4C] 0x4AA050(living) (only HP, LOS, other info are only in unitDef)
	 * Methods to apply "Multiply attribute" effects on unitDef: [EDX+10] 0x442420(base) 0x43ED30(flag) 0x440EF0(deadfish) 0x43E7E0(bird) 0x43FC40(type50), 0x4ED060(living) => 0 1 2 3 4 5 6 A B C D E F 10 12 64 (8,9=nothing)
	 * Methods to apply "Multiply attribute" effects on unit: [EDX+50] 0x4AA0E0(bld)
	 * Methods to apply "Set attribute" effects on unitDef: [EDX+8] 0x4422D0(base) 0x43ECD0(flag) 0x440E90(deadfish) 0x43E780(bird) 0x43F920(type50) 0x4ECF20(living) 0x4EC580(bld) 1 2 3 4 5 6 8 9 A B C D E F 10 12 17 64 65
	*/
	enum TECH_UNIT_ATTRIBUTES : short int {
		TUA_NONE = -1,
		TUA_HP = 0, // Health points
		TUA_LOS = 1, // Line of sight
		TUA_GARRISON_CAPACITY = 2,
		TUA_SIZE_RADIUS1 = 3,
		TUA_SIZE_RADIUS2 = 4,
		TUA_SPEED = 5,
		TUA_ROTATION_SPEED = 6,
		TUA_UNKNOWN_07 = 7, // seems to be unused.
		TUA_ARMOR = 8, // Warning: value contains 2 informations
		TUA_ATTACK = 9, // Warning: value contains 2 informations
		TUA_ATTACK_RELOAD_TIME = 10, // reload time 1
		TUA_ACCURACY_PERCENT = 11,
		TUA_RANGE = 12, // Updates maxRange
		TUA_WORK_RATE = 13,
		TUA_RESOURCE_CAPACITY = 14, // Amount of resource a unit can "own"
		TUA_DEFAULT_ARMOR = 15,
		TUA_PROJECTILE_UNIT = 16, // Set a new projectileUnitId
		TUA_ANGLE = 17, // graphic angle (changes displayed standing graphic frame?)
		TUA_TERRAIN_RESTRICTION_FOR_DAMAGE = 18,
		TUA_INTELLIGENT_PROJECTILE = 19, // Applies on PROJECTILES unitDef, not archers/towers/etc ! 0 = false, 1 = true (better... generally)
		TUA_ADD_COST_AMOUNT = 100, // Add a value to unit cost. Impacts ALL "used" costs.
		TUA_POPULATION_COST = 101 // population a unit "counts". 1 for most units, (total)0.5 for infantry with logistics. Can only SET (no add/mult)
	};


	// ------------------------------------------------------------------
	// Unit ids : Villagers
	const static int CST_UNITID_VILLAGER = 83; // 0x53 - A synonym for MAN
	const static int CST_UNITID_MAN = 83; // 0x53
	const static int CST_UNITID_BUILDER = 118;
	const static int CST_UNITID_FISHERMAN = 119;
	const static int CST_UNITID_FORAGER = 120;
	const static int CST_UNITID_HUNTER = 122;
	const static int CST_UNITID_LUMBERJACK = 123;
	const static int CST_UNITID_MINERSTONE = 124;
	const static int CST_UNITID_EXPLORER = 127;
	const static int CST_UNITID_REPAIRMAN = 156;
	const static int CST_UNITID_MINERGOLD = 251;
	const static int CST_UNITID_FARMER = 259;
	// Military and buildings
	const static int CST_UNITID_FORUM = 109; // 0x6D - Town center
	const short int CST_UNITID_BARRACKS = 0x0C; //12
	const short int CST_UNITID_CLUBMAN = 0x49;
	const short int CST_UNITID_AXEMAN = 0x4A;
	const short int CST_UNITID_SHORT_SWORDSMAN = 0x4B;
	const short int CST_UNITID_BROAD_SWORDSMAN = 0x4C;
	const short int CST_UNITID_LONG_SWORDSMAN = 0x4D; // 77
	const short int CST_UNITID_LEGION = 282;
	const short int CST_UNITID_SLINGER = 347;

	const short int CST_UNITID_STABLE = 0x65; //101. NOT 86 which is upgraded one !
	const short int CST_UNITID_SCOUT = 299;
	const short int CST_UNITID_CAVALRY = 0x25; //37
	const short int CST_UNITID_HEAVY_CAVALRY = 38;
	const short int CST_UNITID_CATAPHRACT = 283;
	const short int CST_UNITID_CHARIOT = 0x28; //40
	const short int CST_UNITID_HEAVY_CHARIOT = 339;
	const short int CST_UNITID_WAR_ELEPHANT = 0x2E; //46
	const short int CST_UNITID_ARMORED_ELEPHANT = 345;
	const short int CST_UNITID_CAMEL = 338;

	const short int CST_UNITID_RANGE = 0x57; // 87. Does not have an "upgraded" unit ?
	const short int CST_UNITID_BOWMAN = 4;
	const short int CST_UNITID_IMPROVED_BOWMAN = 5;
	const short int CST_UNITID_COMPOSITE_BOWMAN = 6;
	const short int CST_UNITID_HORSE_ARCHER = 0x27; // 39
	const short int CST_UNITID_HEAVY_HORSE_ARCHER = 281;
	const short int CST_UNITID_ELEPHANT_ARCHER = 0x19; // 25
	const short int CST_UNITID_CHARIOT_ARCHER = 0x29; // 41

	const short int CST_UNITID_SIEGE_WORKSHOP = 0x31; // 49
	const short int CST_UNITID_BALLISTA = 0x0B;// 11
	const short int CST_UNITID_HELEPOLIS = 279;
	const short int CST_UNITID_STONE_THROWER = 0x23; //35
	const short int CST_UNITID_CATAPULT = 0x24; //36
	const short int CST_UNITID_HEAVY_CATAPULT = 0x118; // 280

	const short int CST_UNITID_ACADEMY = 0;
	const short int CST_UNITID_HOPLITE = 0x5D; //93
	const short int CST_UNITID_PHALANX = 0x5E; //94
	const short int CST_UNITID_CENTURION = 291;

	const short int CST_UNITID_TEMPLE = 0x68;
	const short int CST_UNITID_PRIEST = 0x7D;

	const short int CST_UNITID_DOCK = 0x2D; //45
	const short int CST_UNITID_FISHING_BOAT = 0x0D; // 13
	const short int CST_UNITID_FISHING_BOAT2 = 0x0E; // 14 - upgraded fishing ship
	const short int CST_UNITID_TRADE_BOAT = 0x0F; //15
	const short int CST_UNITID_TRANSPORT_BOAT = 0x11;
	const short int CST_UNITID_H_TRANSPORT_BOAT = 0x12;
	const short int CST_UNITID_SCOUT_SHIP = 0x13;
	const short int CST_UNITID_WAR_GALLEY = 0x14; //20
	const short int CST_UNITID_TRIREME = 0x15; //21
	const short int CST_UNITID_CATAPULT_TRIREME = 0xFA; //250
	const short int CST_UNITID_JUGGERNAUGHT = 0x115; //277
	const short int CST_UNITID_FIRE_GALLEY = 360;

	const short int CST_UNITID_FARM = 0x32; //50
	const short int CST_UNITID_GRANARY = 0x44; // 68
	const short int CST_UNITID_HOUSE = 0x46; //70
	const short int CST_UNITID_GOVERNMENT_CENTER = 0x52; //82
	const short int CST_UNITID_MARKET = 0x54; //84
	const short int CST_UNITID_STORAGE_PIT = 0x67; //103
	const short int CST_UNITID_WONDER = 0x114; //276

	// Towers & Walls
	const short int CST_UNITID_WATCH_TOWER = 0x4F; //79
	const short int CST_UNITID_SENTRY_TOWER = 0xC7; //199
	const short int CST_UNITID_GUARD_TOWER = 0x45; //69
	const short int CST_UNITID_BALLISTA_TOWER = 0x116; //278
	const short int CST_UNITID_SMALL_WALL = 72;
	const short int CST_UNITID_MEDIUM_WALL = 117;
	const short int CST_UNITID_FORTIFICATION = 155;

	// "Wrong" buildings+men (updates), not to be used directly
	const short int CST_UNITID_TC_2_UPGRADE = 71; // Town_Center2. base is 109
	const short int CST_UNITID_STABLE_2_UPGRADE = 86; // base is 101
	const short int CST_UNITID_STORAGE_PIT_2_UPGRADE = 105; // Storage_Pit2. base is 103
	const short int CST_UNITID_MARKET_2_UPGRADE = 116; // Market2. base is 84
	const short int CST_UNITID_BARRACKS_2_UPGRADE = 132; // Barracks2. base is 12
	const short int CST_UNITID_DOCK_2_UPGRADE = 133; // base is 45
	const short int CST_UNITID_HOUSE_2_UPGRADE = 154; // house1. base is 70
	const short int CST_UNITID_VILLAGER2 = 293; // A synonym for MAN2
	const short int CST_UNITID_MAN2 = 293;

	// Special
	const short int CST_UNITID_RELIC = 0x9F; // 159.
	const short int CST_UNITID_RUIN = 0x9E; // 158.
	const short int CST_UNITID_RUIN2 = 0xA3; // 163. The unused (but supported) ruins

	// Research IDs from empires.dat
	const short int CST_RSID_STONE_AGE = 100;
	const short int CST_RSID_TOOL_AGE = 101;
	const short int CST_RSID_BRONZE_AGE = 102;
	const short int CST_RSID_IRON_AGE = 103;
	// Research IDs: unit upgrades
	const short int CST_RSID_MEDIUM_WAR_SHIP = 5;
	const short int CST_RSID_TRIREME = 7;
	const short int CST_RSID_HEAVY_TRANSPORT = 8;
	const short int CST_RSID_CATAPULT_TRIREME = 9;
	const short int CST_RSID_NAVAL_RAM = 118; // fire galley
	const short int CST_RSID_FIRE_GALLEY = 118; // or "naval ram"
	const short int CST_RSID_JUGGERNAUGHT = 25;
	const short int CST_RSID_HELEPOLIS = 27;
	const short int CST_RSID_HEAVY_CATAPULT = 36;
	const short int CST_RSID_HEAVY_HORSE_ARCHER = 38;
	const short int CST_RSID_CATAPULT = 54;
	const short int CST_RSID_COMPOSITE_ARCHER = 57;
	const short int CST_RSID_AXE = 63;
	const short int CST_RSID_BROAD_SWORD = 65;
	const short int CST_RSID_LONG_SWORD = 66;
	const short int CST_RSID_HEAVY_CAVALRY = 71;
	const short int CST_RSID_PHALANX = 73;
	const short int CST_RSID_LEGION = 77;
	const short int CST_RSID_CATAPHRACT = 78;
	const short int CST_RSID_CENTURION = 79;
	const short int CST_RSID_ARMORED_ELEPHANT = 125;
	const short int CST_RSID_HEAVY_CHARIOT = 126;
	const short int CST_RSID_WATCH_TOWER = 16;
	const short int CST_RSID_SENTRY_TOWER = 12;
	const short int CST_RSID_GUARD_TOWER = 15;
	const short int CST_RSID_BALLISTA_TOWER = 2;

	// Research IDs: military
	const short int CST_RSID_TOOL_WORKING = 46; // Tool age - +2 close-range attack. Useful for a lot of units !
	const short int CST_RSID_ARISTOCRACY = 113; // speed up hoplites
	const short int CST_RSID_BALLISTICS = 117; // projectile intelligence - required for ballista towers
	const short int CST_RSID_MONOTHEISM = 19;
	const short int CST_RSID_FANATICISM = 20;
	const short int CST_RSID_MEDECINE = 119;
	// Research IDs: economy/general
	const short int CST_RSID_WHEEL = 28;
	const short int CST_RSID_ARCHITECTURE = 112; // Bronze age
	const short int CST_RSID_WOOD_WORKING = 107; // Tool age - +1 range attack
	const short int CST_RSID_ARTISANSHIP = 32; // bronze age - +1 range attack
	const short int CST_RSID_CRAFTMANSHIP = 110; // Iron age - +1 range attack
	const short int CST_RSID_DOMESTICATION = 81; // Tool age - for farms
	const short int CST_RSID_PLOW = 31; // Bronze age - for farms
	const short int CST_RSID_IRRIGATION = 80; // Iron age - for farms
	const short int CST_RSID_GOLD_MINING = 108; // Tool age
	const short int CST_RSID_COINAGE = 30; // Iron age (gold mining + tax-free tribute)
	const short int CST_RSID_STONE_MINING = 109; // Tool age
	const short int CST_RSID_SIEGE_CRAFT = 111; // Iron age - Villagers can destroy towers
	const short int CST_RSID_WRITING = 114; // Share exploration
	// Research IDs: shadow researches
	const short int CST_RSID_MARKET_FARM = 26; // Enable farms
	const short int CST_RSID_STORAGE_PIT = 39; // Enable storage pit
	// Walls
	const short int CST_RSID_SMALL_WALL = 11; // Wall #1
	const short int CST_RSID_MEDIUM_WALL = 13; // Wall #2
	const short int CST_RSID_FORTIFICATION = 14; // Wall #3

	// Technology IDs (cf AGE3)
	enum TECHNOLOGIES : short int {
		// Tech trees
		CST_TCH_TECH_TREE_ASSYRIAN = 81,
		CST_TCH_TECH_TREE_BABYLONIAN = 82,
		CST_TCH_TECH_TREE_EGYPTIAN = 83,
		CST_TCH_TECH_TREE_GREEK = 84,
		CST_TCH_TECH_TREE_HITTITE = 85,
		CST_TCH_TECH_TREE_MINOAN = 86,
		CST_TCH_TECH_TREE_PERSIAN = 87,
		CST_TCH_TECH_TREE_PHOENICIAN = 88,
		CST_TCH_TECH_TREE_SHANG = 89,
		CST_TCH_TECH_TREE_SUMERIAN = 90,
		CST_TCH_TECH_TREE_CHOSON = 91,
		CST_TCH_TECH_TREE_YAMATO = 92,
		CST_TCH_TECH_TREE_CARTHAGE = 205,
		CST_TCH_TECH_TREE_MACEDONIAN = 206,
		CST_TCH_TECH_TREE_PALMYRA = 207,
		CST_TCH_TECH_TREE_ROMAN = 208,
		CST_TCH_TECH_TREE_ALL_TECHS = 210,
		// Other
		CST_TCH_ARTISANSHIP = 32, // Bronze age (2nd  tech for wood cutting/range)
		CST_TCH_STONE_AGE = 94,
		CST_TCH_TOOL_AGE = 95,
		CST_TCH_BRONZE_AGE = 96,
		CST_TCH_IRON_AGE = 97,
		CST_TCH_WOODWORKING = 106, // FR: menuiserie (tool age - 1st tech for wood cutting/range)
		CST_TCH_GOLD_MINING = 107,
		CST_TCH_STONE_MINING = 108,
		CST_TCH_CRAFTSMANSHIP = 109, // FR: artisanat évolué (iron age - 3rd tech for wood cutting/range)
		CST_TCH_SIEGECRAFT = 110,
		CST_TCH_MEDECINE = 211,
		CST_TCH_JIHAD = 23,
		CST_TCH_MARTYRDOM = 212, // Enables priest sacrifice to convert immediately
		CST_TCH_BALLISTA_TOWER = 102
	};


	// Unit names (this list has been directly generated from in-game data)
	const static int CST_UNIT_NAMES_MIN_ID = 0;
	const static int CST_UNIT_NAMES_MAX_ID = 399;
	// Do not use directly. Please use GetUnitName(DAT_ID)
	const static char *CST_UNIT_NAME_NONE = "None";
	const static wchar_t *CST_UNIT_NAME_W_NONE = _T("None");
	const static char *CST_UNIT_NAMES[CST_UNIT_NAMES_MAX_ID - CST_UNIT_NAMES_MIN_ID + 1] = {
		"Academy",
		"Alligator",
		"Unused",
		"Unused",
		"Soldier-Archer1",
		"Soldier-Archer2",
		"Soldier-Archer3",
		"Unused",
		"Unused",
		"Missle-Arrow",
		"Artifact_Discov",
		"Soldier-Balista",
		"Barracks1",
		"Boat-Fishing1",
		"Boat-Fishing2",
		"Boat-Trade1",
		"Boat-Trade2",
		"Boat-Transport1",
		"Boat-Transport2",
		"Boat-War1",
		"Boat-War2",
		"Boat-War3",
		"Rubble_1x1",
		"Rubble_2x2",
		"Rubble_3x3",
		"Soldier-El_Arch",
		"Terrain_Path_St",
		"Terrain_Path_St",
		"Terrain_Path_St",
		"Terrain_Path_Dr",
		"Terrain_Path_Dr",
		"Terrain_Path_Dr",
		"Terrain_Path_Dr",
		"Unused",
		"Missle-Catapult",
		"Soldier-Catapul",
		"Soldier-Catapul",
		"Soldier-Cavalry",
		"Soldier-Cavalry",
		"Soldier-Cavalry",
		"Soldier-Chariot",
		"Soldier-Chariot",
		"Unused",
		"Dead_Man",
		"Unused",
		"Dock_1",
		"Soldier-Elephan",
		"Terrain_Path_Dr",
		"Elephant",
		"Siege_Workshop",
		"Farm",
		"Unused",
		"Fish-tuna",
		"Fish-Salmon",
		"Terrain_Path_St",
		"Terrain_Path_St",
		"Tree94",
		"Unused",
		"Unused",
		"Forage-Fruit",
		"Unused",
		"Smoke",
		"Unused",
		"Unused",
		"Unused",
		"Gazelle",
		"Mine-Gold",
		"Unused",
		"Granary",
		"Guard_Tower",
		"House",
		"Town_Center2",
		"Wall_Small",
		"Soldier-Inf1",
		"Soldier-Inf2",
		"Soldier-Inf3",
		"Soldier-Inf4",
		"Soldier-Inf5",
		"Unused",
		"Watch_Tower",
		"Shallows",
		"Unused",
		"Government_Cent",
		"Man",
		"Market1",
		"Dead_Alligator",
		"Stable1",
		"Range1",
		"Unused",
		"Lion_King",
		"Elephant_King",
		"Unused",
		"Unused",
		"Soldier-Phal1",
		"Soldier-Phal2",
		"Pigeon",
		"Pigeon_Big",
		"Unused",
		"Unused",
		"Unused",
		"Missle-Spear",
		"Stable",
		"Mine-Stone",
		"Storage_Pit1",
		"Temple1",
		"Storage_Pit2",
		"Terrain-Bones",
		"Unused",
		"Artifact_War_Ch",
		"Town_Center1",
		"Trade_Wkshop",
		"Unused",
		"Flare",
		"Tree_Forest_j1",
		"Tree_Forest_j2",
		"Tree_Stump1",
		"Market2",
		"Wall_Med",
		"Unused",
		"Unused",
		"Unused",
		"Tree_Forest_j3",
		"Unused",
		"Unused",
		"Unused",
		"Priest",
		"Lion",
		"Unused",
		"Unused",
		"Tree_Forest_j4",
		"Tree_Stump2",
		"Tree_Stump3",
		"Barracks2",
		"Dock_2",
		"Tree01",
		"Tree02",
		"Tree03",
		"Tree04",
		"Tree05",
		"Tree06",
		"Tree07",
		"Tree08",
		"Tree09",
		"Tree10",
		"Tree11",
		"Tree12",
		"Tree13",
		"Tree14",
		"Tree15",
		"Tree16",
		"Tree17",
		"Tree18",
		"Tree19",
		"Tree20",
		"House1",
		"Wall_Large",
		"Unused",
		"Horse",
		"Artifact_Ruins1",
		"Artifact1_Gaia",
		"Unused",
		"Tree_Forest_p1",
		"Flag",
		"Artifact_Ruins2",
		"Cactus1",
		"Cactus2",
		"Cactus3",
		"Grass_Clump2",
		"Grass_Clump1",
		"Cactus4",
		"Grass_Clump3",
		"Grass_Clump4",
		"Grass_Clump5",
		"Grass_Clump6",
		"Grass_Clump7",
		"Grass_Clump8",
		"Grass_Clump9",
		"Grass_Clump10",
		"Dessert_Clump1",
		"Dessert_Clump2",
		"Dessert_Clump3",
		"Bones1",
		"Bones2",
		"Bones3",
		"Terrain_Rock_g2",
		"Terrain_Rock_s2",
		"Terrain_Rock_d1",
		"Terrain_Crack2",
		"Terrain_Crack3",
		"Terrain_Crack4",
		"Terrain_Crack5",
		"Terrain_Crack1",
		"Tree21",
		"Tree22",
		"Tree23",
		"Tree90",
		"Unused",
		"Tree24",
		"Tree_Forest_p2",
		"Sentry_Tower",
		"Unused",
		"Unused",
		"Unused",
		"Tree_Forest_p3",
		"Missle-Bolt",
		"Missle-Bolt-Fir",
		"Missle-Arrow-Fi",
		"Dead_Arch_1",
		"Dead_Arch_2",
		"Dead_Axeman",
		"Dead_B_Swd",
		"Dead_Balista",
		"Dead_Catapult",
		"Dead_Chariot",
		"Dead_H_Cavalry",
		"Dead_Infantry",
		"Dead_L_Swd",
		"Dead_L_cavalry",
		"Dead_Legion",
		"Dead_Long_Bow",
		"Unused",
		"Dead_Phalnx",
		"Dead_Priest",
		"Dead_S_Swd",
		"Dead_War_Ele",
		"Unused",
		"Tree_Forest_p4",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Hero_Catapult",
		"Hero_Tower",
		"Hero_Horse_Arch",
		"Dock_Destructio",
		"Unused",
		"Unused",
		"Unused",
		"Wall_L_Rubble",
		"Doppleganger",
		"Dead_Farm",
		"Unused",
		"Missile-Laser",
		"Laser_Trail",
		"Soldier_Nuker",
		"Missile-Nuke",
		"Boat-War4",
		"Unused",
		"Dead_Lumber",
		"Dead_Fisher",
		"Dead_Miner",
		"Dead_Hunter",
		"Dead_Builder",
		"Dead_Farmer",
		"Dead_Forager",
		"Unused",
		"Fish-Shore_Jump",
		"Dead_Chariot_2",
		"Dead_Gazelle",
		"Fish-Shore_Swim",
		"Cliffs_1",
		"Cliffs_2",
		"Cliffs_3",
		"Cliffs_4",
		"Cliffs_5",
		"Cliffs_6",
		"Cliffs_7",
		"Cliffs_8",
		"Cliffs_9",
		"Cliffs_10",
		"Soldier_Laser",
		"Missle-Catapult",
		"Wonder",
		"Boat-War5",
		"Guard_Tower2",
		"Soldier-Balista",
		"Soldier-Catapul",
		"Soldier-Cavalry",
		"Soldier-Inf6",
		"Soldier-Cavalry",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Unused",
		"Soldier-Phal3",
		"Raft",
		"Man2",
		"Wall_M_Rubble",
		"Wall_S_Rubble",
		"Cliff_Waterfall",
		"Hero_Juggernaut",
		"Hero_Medusa",
		"Soldier-Scout",
		"Missle-Aligator",
		"Unused",
		"Fire",
		"Dead_A_Cavalry",
		"Terrain_Rock_d2",
		"Terrain_Rock_d3",
		"Terrain_Rock_d4",
		"Terrain_Rock_g1",
		"Terrain_Rock_g3",
		"Terrain_Rock_g4",
		"Terrain_Rock_g5",
		"Terrain_Rock_g6",
		"Terrain_Rock_g7",
		"Terrain_Rock_g8",
		"Terrain_Rock_s1",
		"Terrain_Rock_s1",
		"Terrain_Rock_s1",
		"Terrain_Rock_s1",
		"Terrain_Rock_s3",
		"Terrain_Rock_s4",
		"Terrain_Rock_s5",
		"Terrain_Rock_s6",
		"Terrain_Rock_s7",
		"Terrain_Rock_s8",
		"Terrain_Rock_s9",
		"Unused",
		"Smoke_Battlex1",
		"Smoke_Battlex2",
		"Smoke_Battlex3",
		"Missle-Catapult",
		"Flag_B",
		"RubbleSmokex3",
		"Terrain_Rock_m1",
		"Terrain_Rock_m2",
		"Terrain_Rock_m3",
		"Dead_Elephant",
		"Dead_Elarch",
		"Space_Ship",
		"Soldier-Camel",
		"Soldier-Hvy_Cha",
		"Terrain_Path_St",
		"Dead_Scout",
		"Dead_Cat2",
		"Terrain_Dead_Tr",
		"Terrain_Path_Dr",
		"Soldier-Armor_E",
		"Missile-Warboat",
		"Soldier-Slinger",
		"Terrain_Path_Gr",
		"Terrain_Path_Gr",
		"Terrain_Path_Gr",
		"Terrain_Path_Gr",
		"Terrain_Path_Gr",
		"Terrain_Path_Gr",
		"Terrain_Path_St",
		"Terrain_Path_St",
		"Terrain_Path_St",
		"Terrain_Path_St",
		"Terrain_Path_St",
		"Terrain_Path_St",
		"Boat-War6",
		"Missile-Sling_B",
		"Alligator_King",
		"Dead_slinger",
		"Unused",
		"Tree91",
		"Tree92",
		"Tree93",
		"Missle-Boat_Sto",
		"Dead_Lion",
		"Fish_Whale",
		"Unused",
		"Winsett''s_Z",
		"Z_Rocket",
		"Dead_camel",
		"Hero_Hannibal",
		"Hero_Caesar",
		"Hero_Cleopatra",
		"Hero_Mark_Anton",
		"Hero_Scipio",
		"Hero_Zenobia''s_",
		"Unused",
		"Unused",
		"Mirror_Tower",
		"Gazelle_King",
		"Terrain_Beach_R",
		"Soldier_Mech",
		"Dead_Mech",
		"Missile-mech_gu",
		"Dead_H_Chariot",
		"Dead_Armored_El",
		"Tree_pine_10",
		"Tree_pine_9",
		"Tree_pine_imp",
		"Saint_Francis",
		"Lightning_Bolt",
		"Dragon",
		"Winsett''s_Mom''s",
		"Winsett''s_baby",
		"Dead_baby_z"
	};

	const static wchar_t *CST_UNIT_NAMES_W[CST_UNIT_NAMES_MAX_ID - CST_UNIT_NAMES_MIN_ID + 1] = {
		_T("Academy"),
		_T("Alligator"),
		_T("Unused"),
		_T("Unused"),
		_T("Soldier-Archer1"),
		_T("Soldier-Archer2"),
		_T("Soldier-Archer3"),
		_T("Unused"),
		_T("Unused"),
		_T("Missle-Arrow"),
		_T("Artifact_Discov"),
		_T("Soldier-Balista"),
		_T("Barracks1"),
		_T("Boat-Fishing1"),
		_T("Boat-Fishing2"),
		_T("Boat-Trade1"),
		_T("Boat-Trade2"),
		_T("Boat-Transport1"),
		_T("Boat-Transport2"),
		_T("Boat-War1"),
		_T("Boat-War2"),
		_T("Boat-War3"),
		_T("Rubble_1x1"),
		_T("Rubble_2x2"),
		_T("Rubble_3x3"),
		_T("Soldier-El_Arch"),
		_T("Terrain_Path_St"),
		_T("Terrain_Path_St"),
		_T("Terrain_Path_St"),
		_T("Terrain_Path_Dr"),
		_T("Terrain_Path_Dr"),
		_T("Terrain_Path_Dr"),
		_T("Terrain_Path_Dr"),
		_T("Unused"),
		_T("Missle-Catapult"),
		_T("Soldier-Catapul"),
		_T("Soldier-Catapul"),
		_T("Soldier-Cavalry"),
		_T("Soldier-Cavalry"),
		_T("Soldier-Cavalry"),
		_T("Soldier-Chariot"),
		_T("Soldier-Chariot"),
		_T("Unused"),
		_T("Dead_Man"),
		_T("Unused"),
		_T("Dock_1"),
		_T("Soldier-Elephan"),
		_T("Terrain_Path_Dr"),
		_T("Elephant"),
		_T("Siege_Workshop"),
		_T("Farm"),
		_T("Unused"),
		_T("Fish-tuna"),
		_T("Fish-Salmon"),
		_T("Terrain_Path_St"),
		_T("Terrain_Path_St"),
		_T("Tree94"),
		_T("Unused"),
		_T("Unused"),
		_T("Forage-Fruit"),
		_T("Unused"),
		_T("Smoke"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Gazelle"),
		_T("Mine-Gold"),
		_T("Unused"),
		_T("Granary"),
		_T("Guard_Tower"),
		_T("House"),
		_T("Town_Center2"),
		_T("Wall_Small"),
		_T("Soldier-Inf1"),
		_T("Soldier-Inf2"),
		_T("Soldier-Inf3"),
		_T("Soldier-Inf4"),
		_T("Soldier-Inf5"),
		_T("Unused"),
		_T("Watch_Tower"),
		_T("Shallows"),
		_T("Unused"),
		_T("Government_Cent"),
		_T("Man"),
		_T("Market1"),
		_T("Dead_Alligator"),
		_T("Stable1"),
		_T("Range1"),
		_T("Unused"),
		_T("Lion_King"),
		_T("Elephant_King"),
		_T("Unused"),
		_T("Unused"),
		_T("Soldier-Phal1"),
		_T("Soldier-Phal2"),
		_T("Pigeon"),
		_T("Pigeon_Big"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Missle-Spear"),
		_T("Stable"),
		_T("Mine-Stone"),
		_T("Storage_Pit1"),
		_T("Temple1"),
		_T("Storage_Pit2"),
		_T("Terrain-Bones"),
		_T("Unused"),
		_T("Artifact_War_Ch"),
		_T("Town_Center1"),
		_T("Trade_Wkshop"),
		_T("Unused"),
		_T("Flare"),
		_T("Tree_Forest_j1"),
		_T("Tree_Forest_j2"),
		_T("Tree_Stump1"),
		_T("Market2"),
		_T("Wall_Med"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Tree_Forest_j3"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Priest"),
		_T("Lion"),
		_T("Unused"),
		_T("Unused"),
		_T("Tree_Forest_j4"),
		_T("Tree_Stump2"),
		_T("Tree_Stump3"),
		_T("Barracks2"),
		_T("Dock_2"),
		_T("Tree01"),
		_T("Tree02"),
		_T("Tree03"),
		_T("Tree04"),
		_T("Tree05"),
		_T("Tree06"),
		_T("Tree07"),
		_T("Tree08"),
		_T("Tree09"),
		_T("Tree10"),
		_T("Tree11"),
		_T("Tree12"),
		_T("Tree13"),
		_T("Tree14"),
		_T("Tree15"),
		_T("Tree16"),
		_T("Tree17"),
		_T("Tree18"),
		_T("Tree19"),
		_T("Tree20"),
		_T("House1"),
		_T("Wall_Large"),
		_T("Unused"),
		_T("Horse"),
		_T("Artifact_Ruins1"),
		_T("Artifact1_Gaia"),
		_T("Unused"),
		_T("Tree_Forest_p1"),
		_T("Flag"),
		_T("Artifact_Ruins2"),
		_T("Cactus1"),
		_T("Cactus2"),
		_T("Cactus3"),
		_T("Grass_Clump2"),
		_T("Grass_Clump1"),
		_T("Cactus4"),
		_T("Grass_Clump3"),
		_T("Grass_Clump4"),
		_T("Grass_Clump5"),
		_T("Grass_Clump6"),
		_T("Grass_Clump7"),
		_T("Grass_Clump8"),
		_T("Grass_Clump9"),
		_T("Grass_Clump10"),
		_T("Dessert_Clump1"),
		_T("Dessert_Clump2"),
		_T("Dessert_Clump3"),
		_T("Bones1"),
		_T("Bones2"),
		_T("Bones3"),
		_T("Terrain_Rock_g2"),
		_T("Terrain_Rock_s2"),
		_T("Terrain_Rock_d1"),
		_T("Terrain_Crack2"),
		_T("Terrain_Crack3"),
		_T("Terrain_Crack4"),
		_T("Terrain_Crack5"),
		_T("Terrain_Crack1"),
		_T("Tree21"),
		_T("Tree22"),
		_T("Tree23"),
		_T("Tree90"),
		_T("Unused"),
		_T("Tree24"),
		_T("Tree_Forest_p2"),
		_T("Sentry_Tower"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Tree_Forest_p3"),
		_T("Missle-Bolt"),
		_T("Missle-Bolt-Fir"),
		_T("Missle-Arrow-Fi"),
		_T("Dead_Arch_1"),
		_T("Dead_Arch_2"),
		_T("Dead_Axeman"),
		_T("Dead_B_Swd"),
		_T("Dead_Balista"),
		_T("Dead_Catapult"),
		_T("Dead_Chariot"),
		_T("Dead_H_Cavalry"),
		_T("Dead_Infantry"),
		_T("Dead_L_Swd"),
		_T("Dead_L_cavalry"),
		_T("Dead_Legion"),
		_T("Dead_Long_Bow"),
		_T("Unused"),
		_T("Dead_Phalnx"),
		_T("Dead_Priest"),
		_T("Dead_S_Swd"),
		_T("Dead_War_Ele"),
		_T("Unused"),
		_T("Tree_Forest_p4"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Hero_Catapult"),
		_T("Hero_Tower"),
		_T("Hero_Horse_Arch"),
		_T("Dock_Destructio"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Wall_L_Rubble"),
		_T("Doppleganger"),
		_T("Dead_Farm"),
		_T("Unused"),
		_T("Missile-Laser"),
		_T("Laser_Trail"),
		_T("Soldier_Nuker"),
		_T("Missile-Nuke"),
		_T("Boat-War4"),
		_T("Unused"),
		_T("Dead_Lumber"),
		_T("Dead_Fisher"),
		_T("Dead_Miner"),
		_T("Dead_Hunter"),
		_T("Dead_Builder"),
		_T("Dead_Farmer"),
		_T("Dead_Forager"),
		_T("Unused"),
		_T("Fish-Shore_Jump"),
		_T("Dead_Chariot_2"),
		_T("Dead_Gazelle"),
		_T("Fish-Shore_Swim"),
		_T("Cliffs_1"),
		_T("Cliffs_2"),
		_T("Cliffs_3"),
		_T("Cliffs_4"),
		_T("Cliffs_5"),
		_T("Cliffs_6"),
		_T("Cliffs_7"),
		_T("Cliffs_8"),
		_T("Cliffs_9"),
		_T("Cliffs_10"),
		_T("Soldier_Laser"),
		_T("Missle-Catapult"),
		_T("Wonder"),
		_T("Boat-War5"),
		_T("Guard_Tower2"),
		_T("Soldier-Balista"),
		_T("Soldier-Catapul"),
		_T("Soldier-Cavalry"),
		_T("Soldier-Inf6"),
		_T("Soldier-Cavalry"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Unused"),
		_T("Soldier-Phal3"),
		_T("Raft"),
		_T("Man2"),
		_T("Wall_M_Rubble"),
		_T("Wall_S_Rubble"),
		_T("Cliff_Waterfall"),
		_T("Hero_Juggernaut"),
		_T("Hero_Medusa"),
		_T("Soldier-Scout"),
		_T("Missle-Aligator"),
		_T("Unused"),
		_T("Fire"),
		_T("Dead_A_Cavalry"),
		_T("Terrain_Rock_d2"),
		_T("Terrain_Rock_d3"),
		_T("Terrain_Rock_d4"),
		_T("Terrain_Rock_g1"),
		_T("Terrain_Rock_g3"),
		_T("Terrain_Rock_g4"),
		_T("Terrain_Rock_g5"),
		_T("Terrain_Rock_g6"),
		_T("Terrain_Rock_g7"),
		_T("Terrain_Rock_g8"),
		_T("Terrain_Rock_s1"),
		_T("Terrain_Rock_s1"),
		_T("Terrain_Rock_s1"),
		_T("Terrain_Rock_s1"),
		_T("Terrain_Rock_s3"),
		_T("Terrain_Rock_s4"),
		_T("Terrain_Rock_s5"),
		_T("Terrain_Rock_s6"),
		_T("Terrain_Rock_s7"),
		_T("Terrain_Rock_s8"),
		_T("Terrain_Rock_s9"),
		_T("Unused"),
		_T("Smoke_Battlex1"),
		_T("Smoke_Battlex2"),
		_T("Smoke_Battlex3"),
		_T("Missle-Catapult"),
		_T("Flag_B"),
		_T("RubbleSmokex3"),
		_T("Terrain_Rock_m1"),
		_T("Terrain_Rock_m2"),
		_T("Terrain_Rock_m3"),
		_T("Dead_Elephant"),
		_T("Dead_Elarch"),
		_T("Space_Ship"),
		_T("Soldier-Camel"),
		_T("Soldier-Hvy_Cha"),
		_T("Terrain_Path_St"),
		_T("Dead_Scout"),
		_T("Dead_Cat2"),
		_T("Terrain_Dead_Tr"),
		_T("Terrain_Path_Dr"),
		_T("Soldier-Armor_E"),
		_T("Missile-Warboat"),
		_T("Soldier-Slinger"),
		_T("Terrain_Path_Gr"),
		_T("Terrain_Path_Gr"),
		_T("Terrain_Path_Gr"),
		_T("Terrain_Path_Gr"),
		_T("Terrain_Path_Gr"),
		_T("Terrain_Path_Gr"),
		_T("Terrain_Path_St"),
		_T("Terrain_Path_St"),
		_T("Terrain_Path_St"),
		_T("Terrain_Path_St"),
		_T("Terrain_Path_St"),
		_T("Terrain_Path_St"),
		_T("Boat-War6"),
		_T("Missile-Sling_B"),
		_T("Alligator_King"),
		_T("Dead_slinger"),
		_T("Unused"),
		_T("Tree91"),
		_T("Tree92"),
		_T("Tree93"),
		_T("Missle-Boat_Sto"),
		_T("Dead_Lion"),
		_T("Fish_Whale"),
		_T("Unused"),
		_T("Winsett''s_Z"),
		_T("Z_Rocket"),
		_T("Dead_camel"),
		_T("Hero_Hannibal"),
		_T("Hero_Caesar"),
		_T("Hero_Cleopatra"),
		_T("Hero_Mark_Anton"),
		_T("Hero_Scipio"),
		_T("Hero_Zenobia''s_"),
		_T("Unused"),
		_T("Unused"),
		_T("Mirror_Tower"),
		_T("Gazelle_King"),
		_T("Terrain_Beach_R"),
		_T("Soldier_Mech"),
		_T("Dead_Mech"),
		_T("Missile-mech_gu"),
		_T("Dead_H_Chariot"),
		_T("Dead_Armored_El"),
		_T("Tree_pine_10"),
		_T("Tree_pine_9"),
		_T("Tree_pine_imp"),
		_T("Saint_Francis"),
		_T("Lightning_Bolt"),
		_T("Dragon"),
		_T("Winsett''s_Mom''s"),
		_T("Winsett''s_baby"),
		_T("Dead_baby_z")
	};



	// ----------------------------------------------------------------------------------------------
	// Common functional methods

	// Get (hardcoded) name for a specific unit ID. Only for standard IDs !
	static const char *GetUnitName(unsigned short int dat_id) {
		if ((dat_id < CST_UNIT_NAMES_MIN_ID) || (dat_id > CST_UNIT_NAMES_MAX_ID)) {
			return CST_UNIT_NAME_NONE;
		}
		return CST_UNIT_NAMES[dat_id];
	}
	static const wchar_t *GetUnitNameW(unsigned short int dat_id) {
		if ((dat_id < CST_UNIT_NAMES_MIN_ID) || (dat_id > CST_UNIT_NAMES_MAX_ID)) {
			return CST_UNIT_NAME_W_NONE;
		}
		return CST_UNIT_NAMES_W[dat_id];
	}

	// Returns true if the DAT_ID is a villager unit type
	// DOES NOT return true for fishing ships/trade ships
	// Hardcoded list ! Please use class=4 instead
	static bool IsVillager(unsigned short int dat_id) {
		return (
			(dat_id == CST_UNITID_MAN) ||
			(dat_id == CST_UNITID_MAN2) ||
			(dat_id == CST_UNITID_BUILDER) ||
			(dat_id == CST_UNITID_FISHERMAN) ||
			(dat_id == CST_UNITID_FORAGER) ||
			(dat_id == CST_UNITID_HUNTER) ||
			(dat_id == CST_UNITID_LUMBERJACK) ||
			(dat_id == CST_UNITID_MINERSTONE) ||
			(dat_id == CST_UNITID_EXPLORER) || // Explorer: unused
			(dat_id == CST_UNITID_REPAIRMAN) ||
			(dat_id == CST_UNITID_MINERGOLD) ||
			(dat_id == CST_UNITID_FARMER)
			);
	}
	// Returns true if the DAT_ID is a villager unit type
	// Returns true too for fishing ships/trade ships
	static bool IsVillager_includingShips(unsigned short int dat_id) {
		return IsVillager(dat_id) ||
			(dat_id == CST_UNITID_FISHING_BOAT) ||
			(dat_id == CST_UNITID_FISHING_BOAT2) ||
			(dat_id == CST_UNITID_TRADE_BOAT);
	}

	// Returns true if the 2 DAT_ID are a same unit (all villager types are considered equals)
	static bool IsSameUnitTypeId(unsigned short int dat_id1, unsigned short int dat_id2) {
		return ((dat_id1 == dat_id2) ||
			(IsVillager(dat_id1) && IsVillager(dat_id2))
			);
	}

	// GetUnitTypeLabel


	// Returns the valid "DATID1" value to use for buildings.
	// For most units, returns the same value as provided.
	// For "building upgrades" IDs, returns the correct "base" ID that must be used for strategy, to get correct unitDef structure from ID, etc.
	static short int GetBaseBuildingUnitId(short int dat_id) {
		if (dat_id == CST_UNITID_STABLE_2_UPGRADE) { return CST_UNITID_STABLE; }
		if (dat_id == CST_UNITID_TC_2_UPGRADE) { return CST_UNITID_FORUM; }
		if (dat_id == CST_UNITID_STORAGE_PIT_2_UPGRADE) { return CST_UNITID_STORAGE_PIT; }
		if (dat_id == CST_UNITID_MARKET_2_UPGRADE) { return CST_UNITID_MARKET; }
		if (dat_id == CST_UNITID_BARRACKS_2_UPGRADE) { return CST_UNITID_BARRACKS; }
		if (dat_id == CST_UNITID_DOCK_2_UPGRADE) { return CST_UNITID_DOCK; }
		if (dat_id == CST_UNITID_HOUSE_2_UPGRADE) { return CST_UNITID_HOUSE; }
		//if (dat_id == CST_UNITID_VILLAGER2) { return CST_UNITID_MAN; }
		return dat_id;
	}

	// Returns true if a unit is a tower (hardcoded list)
	// See also IsTower overload in crCommon.h.
	static bool IsTower(short int dat_id) {
		return (dat_id == CST_UNITID_WATCH_TOWER) ||
			(dat_id == CST_UNITID_SENTRY_TOWER) ||
			(dat_id == CST_UNITID_GUARD_TOWER) ||
			(dat_id == CST_UNITID_BALLISTA_TOWER);
	}

	// Returns true if a building can train units (hardcoded list)
	static bool DoesBuildingTrainUnits(short int dat_id) {
		return (dat_id == CST_UNITID_ACADEMY) ||
			(dat_id == CST_UNITID_BARRACKS) ||
			(dat_id == CST_UNITID_DOCK) ||
			(dat_id == CST_UNITID_FORUM) ||
			(dat_id == CST_UNITID_RANGE) ||
			(dat_id == CST_UNITID_SIEGE_WORKSHOP) ||
			(dat_id == CST_UNITID_STABLE) ||
			(dat_id == CST_UNITID_TEMPLE);
	}

	// Returns true if unit is a military unit, EXCLUDING towers. See also IsTower(unitDefId) function.
	static bool IsNonTowerMilitaryUnit(AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES aiType) {
		switch (aiType) {
		case TribeAIGroupArcher:
		case TribeAIGroupChariot:
		case TribeAIGroupChariotArcher:
		case TribeAIGroupElephantArcher:
		case TribeAIGroupElephantRider:
		case TribeAIGroupFootSoldier:
		case TribeAIGroupHeavyFootSoldier:
		//case TribeAIGroupHeavyMountedSoldier:
		case TribeAIGroupHorse:
		case TribeAIGroupHorseArcher:
		case TribeAIGroupMountedSoldier:
		case TribeAIGroupPhalanx:
		case TribeAIGroupPriest:
		case TribeAIGroupSiegeWeapon:
		case TribeAIGroupSlinger:
		case TribeAIGroupWarBoat:
			return true;
		case TribeAIGroupArtefact:
		case TribeAINone:
		case TribeAIGroupBuilding:
		case TribeAIGroupCivilian:
		case TribeAIGroupFishingBoat:
		case TribeAIGroupTradeBoat:
		case TribeAIGroupTransportBoat:
		case TribeAIGroupOther_Dead_Projectile:
		case TribeAIGroupTradeCart:
		case TribeAIGroupBird:
		case TribeAIGroupUnused_Tower:
		default: // still many others
			return false;
		}
	}

	// Returns true if provided unit class corresponds to a water unit. Returns false for land units.
	static const bool IsWaterUnit(AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES aiType) {
		switch (aiType) {
		case TribeAIGroupWarBoat:
		case TribeAIGroupFishingBoat:
		case TribeAIGroupTradeBoat:
		case TribeAIGroupTransportBoat:
			return true;
		default:
			return false;
		}
	}


	// Provides water proportion (map water type) for each map type
	static const MAP_WATER_TYPE GetMapWaterType(MAP_TYPE_INDEX mti) {
		switch (mti) {
		case MTI_SMALL_ISLANDS:
			return MWT_ALL_WATER_MAP;
		case MTI_LARGE_ISLANDS:
			return MWT_MOSTLY_WATER_MAP;
		case MTI_COASTAL: // mediterranean/coastal/narrow are managed together in game code (eg 4F3966...)
		case MTI_MEDITERRANEAN: // Check me
		case MTI_NARROWS: // Check me
		case MTI_CONTINENTAL: // Check me. According to empires.dat, it is very similar to narrows
			return MWT_LAND_AND_WATER_MAP;
		case MTI_INLAND:
			return MWT_MOSTLY_LAND_MAP;
		case MTI_HIGHLAND:
		case MTI_HILLS:
			return MWT_ALL_LAND_MAP;
		case MTI_HIDDEN_MAP_TYPE:
		default:
			return MWT_UNKNOWN_MAP;
		}
	}

	// A very subjective weighted evaluation of costs. Don't know where to put it. Implicitely it is linked to DAT info and game behaviour.
	static int GetWeightedCostValue(long int rawCostValue, RESOURCE_TYPES resourceType) {
		switch (resourceType) {
		case RESOURCE_TYPES::CST_RES_ORDER_STONE:
			return (rawCostValue * 150) / 100; // Stone : 150% (?)
		case RESOURCE_TYPES::CST_RES_ORDER_GOLD:
			return (rawCostValue * 200) / 100; // Gold: 200% (?)
		case RESOURCE_TYPES::CST_RES_ORDER_WOOD:
			return (rawCostValue * 115) / 100; // Wood: 115% (?)
		case RESOURCE_TYPES::CST_RES_ORDER_FOOD:
		default:
			return rawCostValue;
		}
	}

}
