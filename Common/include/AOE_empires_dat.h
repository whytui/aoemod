
#pragma once

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
		GUT_MOVABLE = 30, // includes resources like "cut" trees, mines... Dead/fish in AGE3.
		GUT_COMMANDABLE = 40,
		GUT_ATTACKABLE = 50,
		GUT_PROJECTILE = 60,
		GUT_TRAINABLE = 70,
		GUT_BUILDING = 80,
		GUT_TREE = 90 // "not cut" trees
	};


	// Unit (type) numbers - From rules.rps - corresponds to "class" in AGE3
	enum GLOBAL_UNIT_AI_TYPES : short int {
		TribeAINone = -1, // use as a joker in some methods
		TribeAIGroupArcher = 0, // foot archers : bowman + improved/composite
		TribeAIGroupBuilding = 3, // All buildings, including towers, wonders, farms, excluding walls.
		TribeAIGroupCivilian = 4, // All villagers excluding boats !
		TribeAIGroupFootSoldier = 6, // Barracks infantry, excluding slinger
		TribeAIGroupMountedSoldier = 12, // 0xC ; cavalry
		TribeAIGroupSiegeWeapon = 13, // 0x0D
		TribeAIGroupPriest = 18, // 0x12
		TribeAIGroupChariotArcher = 23, // 0x17. Only chariot archer. "TribeAIGroupHeavyMountedSoldier" in rules.rps file.
		TribeAIGroupElephantRider = 24, // 0x18. war elephant + armored elephant, including hannibal
		TribeAIGroupHero = 25, // 0x19 - Hero_Archimedes, Hero-Tiberius, Hero-Perseus, Hero-Xerxes, Hero_Jason
		TribeAIGroupElephantArcher = 26, // 0x1A
		TribeAIGroupPhalanx = 28, // 0x1C
		TribeAIGroupChariot = 35, // 0x23. Chariot/scythe (excluding chariot archer)
		TribeAIGroupHorseArcher = 36, // 0x24
		TribeAIGroupSlinger = 39, // 0x27
		TribeAIGroupTradeBoat = 2,
		TribeAIGroupTransportBoat = 20, // 0x14
		TribeAIGroupFishingBoat = 21, // 0x15
		TribeAIGroupWarBoat = 22, //0x16 - Includes fire galley, Jagannath...
		TribeAIGroupWall = 27,  // 0x1B
		TribeAIGroupArtefact = 1, // ruins/artefacts
		TribeAIGroupUnusedHealer = 17, // 0x11. Could be TribeAIGroupHealer, given it *seems* to have healing interaction but not conversion
		// Unused... If only these WERE used !
		TribeAIGroupUnused_Farm = 49, // 0x31 UNUSED ? Farms are buildings (3) !
		TribeAIGroupUnused_Tower = 52, // 0x34 UNUSED ? Towers are buildings (3) ! Don't use this number in your algorithms ;)
		// Gatherable
		TribeAIGroupSeaFish = 5, // no unitAI
		TribeAIGroupBerryBush = 7, // no unitAI
		TribeAIGroupStoneMine = 8, // no unitAI
		TribeAIGroupPreyAnimal = 9, // gazelle (+king) and horse.
		TribeAIGroupTree = 15, // 0xF. no unitAI
		TribeAIGroupGoldMine = 32, // 0x20. no unitAI
		TribeAIGroupShoreFish = 33, // 0x21. no unitAI
		TribeAIGroupPredatorAnimal = 10, // 0xA
		TribeAIGroupUnknownFish = 31, // 0x1F - a gatherable unused fish type. no unitAI
		// Other
		TribeAIGroupOther_Dead_Projectile = 11, // 0xB. no unitAI
		TribeAIGroupTerrain = 14, // 0x0E. no unitAI
		TribeAIGroupTreeStump = 16, // 0x10. no unitAI
		TribeAIGroupTradeCart = 19, // 0x13. It is a villager type (seen in 0x485ADD).
		TribeAIGroupFlag = 30, // 0x1E. no unitAI
		TribeAIGroupCliff = 34, // 0x22
		TribeAIGroupBird = 38,
		TribeAIGroupHorse = 61, // The last one. In ROR executable, AI Type indexes are capped at 64 (0x40) in 0x4448B4
		TribeAIGroupDomesticatedAnimal = 29, // Lion_trained (unit 60). no unitAI (!)
		//37=smoke? (dolphin??)
		TribeAIGroupStandardCount = 62 // Do not use !
	};


	// Basic resource types
	static const short int MAX_RESOURCE_TYPE_ID = 57;
	enum RESOURCE_TYPES : short int {
		// Basic resources
		CST_RES_ORDER_FOOD = 0, // See also CST_RES_ORDER_MEAT_STORAGE, CST_RES_ORDER_BERRY_STORAGE, CST_RES_ORDER_FISH_STORAGE
		CST_RES_ORDER_WOOD = 1,
		CST_RES_ORDER_STONE = 2,
		CST_RES_ORDER_GOLD = 3,
		// Other resources
		CST_RES_ORDER_POPULATION_HEADROOM = 4,
		CST_RES_BASIC_RESOURCE_COUNT = 4, // many locations use these 4 resources, including AI structures. Put after pop headroom for practical debugging display.
		CST_RES_ORDER_UNKNOWN_05 = 5, // ?
		CST_RES_ORDER_CURRENT_AGE = 6, // Set (NOT increased) in each xxx age technology. Stone=1, Iron=4
		CST_RES_ORDER_RELICS = 7, // Number of owned relics
		CST_RES_ORDER_UNKNOWN_08_TRADE_BONUS = 8, // Trade bonus ? Supposed to have an impact on trading (trade workshop gives 10 of this !)
		CST_RES_ORDER_TRADE_GOODS = 9, // The trade goods amount we can see in docks. Only impacts other players (that trade) ! Cf 0x4F8FC2.
		CST_RES_ORDER_TRADE_PRODUCTION = 10, // Default 1 ?
		CST_RES_ORDER_CURRENT_POPULATION = 11,
		CST_RES_ORDER_CORPSE_DECAY_TIME_SECONDS = 12, // Number of seconds for decay, used for flare (unitDef 112). Always 0 for players, used at unit level only ?
		CST_RES_ORDER_DISCOVERIES = 13, // Number of found discoveries (a player flag is set on such discoveries)
		CST_RES_ORDER_RUINS = 14, // Number of owned ruins
		CST_RES_ORDER_MEAT_STORAGE = 15, // Animal food. Used at unit level to know if transported food is hunt-type (from animal) or berry/farm or fish
		CST_RES_ORDER_BERRY_STORAGE = 16, // Berry OR farm food. Used at unit level to know if transported food is hunt-type (from animal) or berry/farm or fish
		CST_RES_ORDER_FISH_STORAGE = 17, // Used at unit level to know if transported food is hunt-type (from animal) or berry/farm or fish
		CST_RES_ORDER_TOTAL_UNITS_OWNED = 19, // Total number of (*all*) units, not decreased when a unit is lost
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
		CST_RES_ORDER_FARM_FOOD_AMOUNT = 36, // Farm initial food potential. Default 250 ?
		CST_RES_ORDER_CIVILIAN_POPULATION = 37, // Current number of villagers. Counts TribeAIGroupCivilian, TribeAIGroupFishingBoat, TribeAIGroupTradeBoat
		CST_RES_ORDER_UNKNOWN_38 = 38, // is this even used ?
		CST_RES_ORDER_ALL_TECHNOLOGIES = 39, // Unused ???
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
		CST_RES_ORDER_GOLD_COUNTER = 49, // Amount of collected gold (excluding tributes and initial resources), cf score (achievements screen)
		CST_RES_ORDER_SHARED_EXPLORATION = 50,
		CST_RES_ORDER_HOUSES_AMOUNT = 51,
		CST_RES_ORDER_TEMPLE_COUNT = 52, // Cf score (achievements screen)
		CST_RES_ORDER_TRIBUTE_SENT = 53,
		CST_RES_ORDER_ALL_RUINS = 54,
		CST_RES_ORDER_ALL_RELICS = 55,
		CST_RES_ORDER_HEALING = 56, // Affect priests healing rate, see tech 211 (medecine)
		CST_RES_ORDER_PRIEST_SACRIFICE = 57, // If 1, you can delete (sacrifice) priest while converting to make conversion successful. See tech 212.
		CST_RES_COUNT = 58 // 0x3A = corresponds to player+0x4C
	};

	// Gatherable resource types (different from standard resource types). Animal food is not in the list !
	// Internal role only ???
	enum PLAYER_GATHERABLE_RESOURCE_CATEGORIES : char {
		CST_PGR_WOOD = 0,
		CST_PGR_FORAGE = 1, // For berry bush
		CST_PGR_FISH = 2,
		CST_PGR_STONE = 3,
		CST_PGR_GOLD = 4,
		CST_PGR_COUNT = 5
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

	// ATTACK_CLASS: for both attack and armor types.
	// http://agecommunity.wikia.com/wiki/Attack_Data (not 100% correct)
	// If an armor class isn't defined, its amount is always 0 
	// If an attack class isn't defined, it won't have any effect (even if attacked unit has a negative amount) 
	// Damage for an attack class is max(attack-armor, 0). attack&armor can be negative numbers !
	// ... so negative attack may only affect units with (more) negative armor.
	// ... so negative armor is only used if attacking unit HAS an attack for that class.
	// For living units (not buildings), minimum "total" damage is 1.
	enum ATTACK_CLASS: short int {
		CST_AC_NONE = -1,
		CST_AC_BALLISTA_ON_BUILDINGS = 0, // Ballista, Helepolis on buildings
		CST_AC_SLINGER_ON_ARCHERS = 1, // And on towers/walls too
		CST_AC_CHEAT_UNITS = 2, // Bigdadday, photon man, mirror tower... Units 248,372,383,386,397
		CST_AC_BASE_PIERCE = 3, // Common archers attack class
		CST_AC_BASE_MELEE = 4, // The most common attack type. Melee + catapults.
		CST_AC_CATAPULTS_ON_BUILDINGS = 6,
		CST_AC_CHARIOTS_ON_PRIESTS = 7, // Attack class for additional damage on priests
		CST_AC_CAMEL_ON_CAVALRY = 8, // On cavalry/horse archer/chariots
		CST_AC_CAVALRY_ON_INFANTRY = 9, // on clubmen/short sword + upgrades, NOT on slingers, hoplites...
		CST_AC_VILLAGER_SIEGECRAFT = 10, // Siegecraft effect: villagers bonus against towers/walls
		CST_AC_AGGRESSIVE_ANIMAL = 12, // Animal attacks (lion/lion king + alligator king). Only gazelles have armor against this.
		CST_AC_COUNT
	};

	// Returns true if attack class is a standard one (like base melee/piercee + "standard" siege weapons attacks)
	// Returns false if it represents some bonus/malus over specific units.
	static bool IsStandardAttack(ATTACK_CLASS a) {
		return (a == CST_AC_BASE_PIERCE) || (a == CST_AC_BASE_MELEE) || (a == CST_AC_BALLISTA_ON_BUILDINGS) || 
			(a == CST_AC_CATAPULTS_ON_BUILDINGS) || (a == CST_AC_CHEAT_UNITS);
	}

	// Refer to 0x4EBB10 to view implementation
	enum TECH_DEF_EFFECTS : char {
		TDE_INVALID = -1,
		TDE_ATTRIBUTE_MODIFIER_SET = 0, // Warning: For attribute 17 (icon), the effect is ADD, not SET (?)
		TDE_RESOURCE_MODIFIER_ADD_SET = 1, // add or set value (according to "mode" = effectClass field: 0=set, 1=add) to resource #(effectUnit). Don't mess with "technical" resources... EffectAttribute=unused.
		TDE_ENABLE_DISABLE_UNIT = 2, // effect class is "enable": 0=disable, 1=enable
		TDE_UPGRADE_UNIT = 3, // "source" unit = effectUnit, "target" (upgraded) unit = effectClass
		TDE_ATTRIBUTE_MODIFIER_ADD = 4, // add value
		TDE_ATTRIBUTE_MODIFIER_MULT = 5, // multiply by a value
		TDE_RESOURCE_MODIFIER_MULT = 6, // multiply by a value. Not supported natively by AOE/ROR !
		TDE_RESEARCH_COST_MODIFIER = 101, // Not supported by AOE/ROR. Don't implement it as ResearchDefs are common to all players and must NOT be modified (it would even impact other games)
		TDE_DISABLE_RESEARCH = 102, // "value" contains researchId (as float), other fields unused.
		TDE_RESEARCH_TIME_MODIFIER = 103 // Not supported by AOE/ROR. Don't implement it as ResearchDefs are common to all players and must NOT be modified (it would even impact other games)
	};

	/* Attributes for technology effects. 1-byte (char), sometimes stored on 2 bytes (short int)
	 * - 0x45D6A0 = player.applyAttributeModifierSet(unit, class, f_value, attribute)
	 * - 0x45D7A0 = player.applyAttributeModifierAdd((word)unit,(word)class, (float)value, (byte)effectAttribute)
	 * - 0x45D8A0 = player.applyAttributeModifierMult((word)unit,(word)class, (float)value, (byte)effectAttribute)
	 * Methods to apply "Add attribute" effects on unitDef: [EDX+C] 0x442370(base), 0x43ED00(flag), 0x440EC0(deadfish), 0x43E7B0(bird), 0x43FA60(type50), 0x4ED010(living), 0x4EC5B0(bld). Not for 7,18,19,...101?
	 * Methods to apply "Add attribute" effects on unit: [EDX+4C] 0x4AA050(living) -> only HP, LOS, other info are only in unitDef
	 * Methods to apply "Multiply attribute" effects on unitDef: [EDX+10] 0x442420(base) 0x43ED30(flag) 0x440EF0(deadfish) 0x43E7E0(bird) 0x43FC40(type50), 0x4ED060(living), 0x4EC5E0(bld) => 0 1 2 3 4 5 6 A B C D E F 10 12 64 (8,9,17=nothing)
	 * Methods to apply "Multiply attribute" effects on unit: [EDX+50] 0x4AA0E0(base) -> only HP, LOS, other info are only in unitDef
	 * Methods to apply "Set attribute" effects on unitDef: [EDX+8] 0x4422D0(base) 0x43ECD0(flag) 0x440E90(deadfish) 0x43E780(bird) 0x43F920(type50) 0x4ECF20(living) 0x4EC580(bld) 1 2 3 4 5 6 8 9 A B C D E F 10 12 17 64 65
	 * Methods to apply "Set attribute" effects on unit: [EDX+48] 0x4A9FD0(base) 0x4AE820(living) -> 0,1,101. Others=nothing to do at unit level
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

		// Warning: value contains 2 informations using a modulo 256. First round the value, then use bitwise operators
		// Warning: in standard game, such effect only works if the unit already has the specified armor type
		// (can be zero, but must exist). With RockNRor, such effects are supported.
		TUA_ARMOR = 8,

		// Warning: value contains 2 informations using a modulo 256. First round the value, then use bitwise operators
		// Warning: in standard game, such effect only works if the unit already has the specified armor type
		// (can be zero, but must exist). With RockNRor, such effects are supported.
		TUA_ATTACK = 9,

		TUA_ATTACK_RELOAD_TIME = 10, // reload time 1
		TUA_ACCURACY_PERCENT = 11,
		TUA_RANGE = 12, // Updates maxRange
		TUA_WORK_RATE = 13,
		TUA_RESOURCE_CAPACITY = 14, // Amount of resource a unit can "own"
		TUA_DEFAULT_ARMOR = 15,
		TUA_PROJECTILE_UNIT = 16, // Set a new projectileUnitId
		TUA_ANGLE = 17, // icon or graphic angle (changes displayed standing graphic frame?). Always ADD ?
		TUA_TERRAIN_RESTRICTION_FOR_DAMAGE = 18,
		TUA_INTELLIGENT_PROJECTILE = 19, // Applies on PROJECTILES unitDef, not archers/towers/etc ! 0 = false, 1 = true (better... generally)
		TUA_ADD_COST_AMOUNT = 100, // (wrong:Add) MULTIPLIES a value to unit cost. Impacts ALL "paid" costs.
		TUA_POPULATION_COST = 101 // population a unit "counts". 1 for most units, (total)0.5 for infantry with logistics. Can only SET (no add/mult)
	};


	// For "action" structures.
	// AOE 2.4.97: see 0x4BD23A - ROR03.01.0717: see 0x403FC0
	enum UNIT_ACTION_ID : short int {
		CST_IAI_MOVE_TO = 1, // Used as sub-action to move to target ? Used to capture relics
		CST_IAI_FOLLOW = 2, // TO CONFIRM (AGE3)
		CST_IAI_GARRISON_ENTER = 3, // Enter
		CST_IAI_EXPLORE = 4, // 
		CST_IAI_GATHER_NO_ATTACK = 5, // Gathering with NO attack phase. Excludes fishing !
		CST_IAI_UNKNOWN_06 = 6, // Natural wonders cheat??? attack or flee from (seen) unit ? Animal ability ?
		CST_IAI_UNKNOWN_7 = 7, // Attack (combat) in unitDefCommands ?? unsure
		CST_IAI_MISSILE = 8, // "projectile movement". 7C 26 54 00
		CST_IAI_ATTACK = 9,
		CST_IAI_FLY = 0x0A, // "Bird"
		CST_IAI_SCARE_HUNT = 0x0B, // TO CONFIRM. Lion has this "action" on gazelle (id=65).
		CST_IAI_TRANSPORT = 0x0C,
		CST_IAI_GUARD = 0x0D, // TO CONFIRM or auto-attack ?
		CST_IAI_RUNAWAY = 0x14, // "Escape" in AGE3
		CST_IAI_MAKE = 0x15, // "Make". Like produce farm food.
		CST_IAI_EXPLORE_UNSURE_1E = 0x1E, // unsure... see also 4 ?
		CST_IAI_UNKNOWN_FOR_BIRD = 0x28, // fly ?
		CST_IAI_CHEAT_SPAWN_UNIT = 0x64, // bigdaddy, etc
		CST_IAI_BUILD = 0x65, // For villagers (builders)
		CST_IAI_MAKE_OBJECT = 0x66,
		CST_IAI_TRAIN = 0x66, // =MakeObject
		CST_IAI_MAKE_TECH = 0x67, // Apply a technology, including cheats and game speed change !
		CST_IAI_RESEARCH = 0x67, // =MakeTech
		CST_IAI_CONVERT = 0x68,
		CST_IAI_HEAL = 0x69,
		CST_IAI_REPAIR = 0x6A, // 106
		CST_IAI_ARTIFACT_CAPTURE = 0x6B, // Get auto-converted = "Be discovered". To confirm / get precisions
		CST_IAI_DISCOVERY_CAPTURE = 0x6C, // Be discovered for discovery (NOT relics/ruins)
		CST_IAI_RETREAT_TO_SHOOTING_RANGE = 0x6D, // Or Explore ? (0x45C2D0 in AOE 2.4.97)
		CST_IAI_GATHER_WITH_ATTACK = 0x6E, // (110) Hunt, cut wood, fish. Gather activities with an attack phase (includes fishing !)
		CST_IAI_TRADE = 0x6F,
		CST_IAI_GENERATE_WONDER_VICTORY = 120, // 0x78. See 0x4C5A4C. Triggers the "wonder winning condition delay" (if victory conditions are compatible) and sets visibility for the building to all players.
		CST_IAI_DESELECT_WHEN_TASKED = 121, // AGE3?
		CST_IAI_LOOT = 122, // AGE3?
		CST_IAI_HOUSING = 123, // AGE3?
		CST_IAI_UNPACK_ATTACK = 125, // AGE3?
		CST_IAI_UNKNOWN_131 = 131, //?
		CST_IAI_PICKUP_UNIT = 132, // AGE3???
		CST_IAI_SHEAR = 149 // AGE3???
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

	// Resources
	const short int CST_UNITID_GAZELLE = 65;
	const short int CST_UNITID_GAZELLE_KING = 384;
	const short int CST_UNITID_ALLIGATOR = 1;
	const short int CST_UNITID_ALLIGATOR_KING = 362;
	const short int CST_UNITID_ELEPHANT = 48;
	const short int CST_UNITID_ELEPHANT_KING = 90;
	const short int CST_UNITID_LION = 126;
	const short int CST_UNITID_LION_KING = 89;
	const short int CST_UNITID_BERRY_BUSH = 59;
	const short int CST_UNITID_FORAGE_FRUIT = 59; // synonym to CST_UNITID_BERRY_BUSH

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
	const short int CST_RSID_LOGISTICS = 121; // barracks units (classes 6 and 39) count as half a unit in population
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
		CST_TCH_CRAFTSMANSHIP = 109, // FR: artisanat �volu� (iron age - 3rd tech for wood cutting/range)
		CST_TCH_SIEGECRAFT = 110,
		CST_TCH_MEDECINE = 211,
		CST_TCH_JIHAD = 23,
		CST_TCH_MARTYRDOM = 212, // Enables priest sacrifice to convert immediately
		CST_TCH_BALLISTA_TOWER = 102
	};

	enum PROJECTILE_TRAJECTORY_TYPE : char {
		CST_PTT_DEFAULT = 0, // Normal arrow, bolt or stone trajectory
		CST_PTT_FALL_DOWN = 1, // Does not move forward, just falls down from start position (like lightning for st francis).
		CST_PTT_FALL_DOWN2 = 2, // Supported but unused (redundant with 1). Same behaviour as 1. See 0x404CC0.
		CST_PTT_TELEPORT = 3 // Projectile instantly hits enemy units
	};

	enum PROJECTILE_PENETRATION_MODE : char {
		CST_PPM_STOP_ON_MY_TARGET = 0, // Default: projectile stops when hitting *the* target, the shot is effective at this moment. Other units are all ignored (no contact/hit).
		CST_PPM_CONTINUE_THROUGH = 1, // Projectile does NOT hit target but goes through, and only stops when its trajectory ends (reaches the ground). Always misses target, may only work with blast damage ?
		CST_PPM_STOP_ON_ANY_UNIT = 2 // Projectile stops on the first encountered unit, even trees. However, own units can't be hit directly
	};

	// Defines the dopplegagner creation policy when a unit dies.
	// This allows depleted mines/bushes/fishes + trees to remain visible for players that explored the tile, but don't see it (fog) when it becomes depleted/cut.
	// Warning: this has nothing to do with buildings that remain visible for players that don't see it when it is destroyed !
	enum UNIT_DEATH_DOPPLEGANGER_CREATION_MODE : char {
		CST_DCM_NONE = 0, // Default: do not create a doppleganger when unit dies
		CST_DCM_WHEN_RESOURCE_IS_DEPLETED = 1, // Create a doppleganger after unit dies AND its owned resource is depleted
		CST_DCM_IMMEDIATE_AT_DEATH = 2, // Create a doppleganger immediately when unit dies (even if it still has resources)
	};

	enum UNITDEF_COMBAT_LEVEL : char {
		CST_UCL_NONE = 0,
		CST_UCL_BASE = 1,
		CST_UCL_BUILDING = 2,
		CST_UCL_CIVILIAN = 3,
		CST_UCL_MILITARY = 4,
		CST_UCL_PRIEST = 5
	};

	enum UNIT_SORT_NUMBER : char {
		CST_ESN_CAN_PUT_ON_TOP_OF_OTHER_UNITS = 0,
		CST_ESN_CANNOT_PUT_ON_TOP_OF_OTHER_UNITS = 5
	};

	enum UNIT_HILL_PLACEMENT_MODE : char {
		CST_HPM_NO_RESTRICTION = 0,
		CST_HPM_NOT_ON_HILL_CORNER = 1, // Not used in ROR ?
		CST_HPM_FLAT_LAND_ONLY = 2, // Most buildings... And cliffs !
		CST_HPM_ALLOW_ONE_ELEVATION_DIFFERENCE = 3 // Not used in ROR ?
	};

	enum TERRAIN_RESTRICTION : short int {
		CST_TR_FLYING = 0, // projectiles, flags, dead units
		CST_TR_DOMESTICATED_ANIMALS = 1, // Only lion_trained !!!
		CST_TR_UNKNOWN_BEACH = 2,
		CST_TR_BOATS = 3,
		CST_TR_BUILDINGS = 4, // All buildings, including towers, but NOT dock, farms...
		CST_TR_ALL_INACCESSIBLE = 5, // unused ?
		CST_TR_DOCK = 6, // Specific for dock (water with land nearby)
		CST_TR_LAND_LIVING_UNITS = 7,
		CST_TR_UNKNOWN_08 = 8, // unused ?
		CST_TR_FARM = 9, // Specific for farms
		CST_TR_WALLS = 10
	};

	// Represents boundaries between 2 terrain types.
	// Border-terrain types are limited: dark water, water(includes shallows), desert(includes various terrains), grass(includes forests, etc)
	enum GROUND_BORDERS : char {
		CST_GB_NO_BORDER = 0, // When terrain is not a border (in a zone where terrain is constant)
		CST_GB_GB_VOID = 1,
		CST_GB_DESERT_WATER = 2,
		CST_GB_GRASS_WATER = 3,
		CST_GB_GRASS_DESERT = 4,
		CST_GB_GRASS_FOREST = 5,
		CST_GB_GRASS_DESERT2 = 6, // Commonly found
		CST_GB_WATER_DARK = 7, // Light water / dark water
		CST_GB_UNKNOWN_8 = 8,
		CST_GB_UNKNOWN_9 = 9,
		CST_GB_UNKNOWN_10 = 10,
		CST_GB_UNKNOWN_11 = 11,
		CST_GB_UNKNOWN_12 = 12,
		CST_GB_UNKNOWN_13 = 13,
		CST_GB_UNKNOWN_14 = 14,
		CST_GB_UNKNOWN_15 = 15,
	};


	enum ELEVATION_GRAPHICS_TYPE {
		EGT_UNKNOWN_00 = 0, // Flat terrain ?
		EGT_UNKNOWN_01 = 1,
		EGT_UNKNOWN_02 = 2,
		EGT_UNKNOWN_03 = 3,
		EGT_UNKNOWN_04 = 4,
		EGT_UNKNOWN_05 = 5,
		EGT_UNKNOWN_06 = 6,
		EGT_UNKNOWN_07 = 7,
		EGT_UNKNOWN_08 = 8,
		EGT_UNKNOWN_09 = 9,
		EGT_UNKNOWN_10 = 10,
		EGT_UNKNOWN_11 = 11,
		EGT_UNKNOWN_12 = 12,
		EGT_UNKNOWN_13 = 13,
		EGT_UNKNOWN_14 = 14,
		EGT_UNKNOWN_15 = 15,
		EGT_UNKNOWN_16 = 16,
		EGT_UNKNOWN_17 = 17,
		EGT_UNKNOWN_18 = 18,
		EGT_COUNT
	};
	static_assert(EGT_COUNT == 0x13, "Number of elevation graphics types");


	// ----------------------------------------------------------------------------------------------
	// Common functional methods


	// Returns true if the DAT_ID is a villager unit type
	// DOES NOT return true for fishing ships/trade ships
	// *Hardcoded* list ! Please use "class=4" condition instead
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
	// *Hardcoded* list ! Please use "class=4 or 19,21" (even 20?) condition instead
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

	// Returns true if a unit is a tower (hardcoded list). Please do not use.
	// See also IsTower overload in unitDefHandling.h.
	static bool IsTower_hardcoded(short int dat_id) {
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

	// Returns true if unit is a military unit, EXCLUDING towers, based on unit class. See also IsTower(unitDefId) function.
	static bool IsNonTowerMilitaryUnit(AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES aiType) {
		switch (aiType) {
		case TribeAIGroupArcher:
		case TribeAIGroupChariot:
		case TribeAIGroupChariotArcher:
		case TribeAIGroupElephantArcher:
		case TribeAIGroupElephantRider:
		case TribeAIGroupFootSoldier:
		case TribeAIGroupHero:
		//case TribeAIGroupHeavyMountedSoldier (chariot archer)
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


	// Returns true if a tree unit def ID is a forest (40 food by default), false for "normal" trees (75 wood by default)
	// unitDefId MUST be a tree (not verified). This is HARDCODED using IDs.
	static bool TreeUnitIsForest(short int unitDefId) {
		if ((unitDefId >= 134) && (unitDefId <= 153)) { // serie of basic trees (default 75)
			return false;
		}
		switch (unitDefId) {
		case 56:
		case 113:
		case 114:
		case 121:
		case 129:
		case 161:
		case 195:
		case 198:
		case 203:
		case 206:
		case 365:
		case 366:
		case 367:
			// Forest (default 40)
			return true;
		case 192:
		case 193:
		case 194:
		case 197:
		case 391:
		case 392:
		case 393:
			// basic trees (default 75)
			return false;
		default:
			return false; // Unknown units (not trees or custom ?)
		}
	}


	// Returns true if unit def ID is a "Vanilla AOE" unit (not added by ROR or other extensions/mods)
	// This is a HARDCODED list !
	static bool IsVanillaAoeUnit(short int unitDefId) {
		if ((unitDefId < 0) || (unitDefId > 373)) {
			// 373 is the last unitDefId in standard "AOE" empires.dat
			// 374-399 are ROR units (mostly eye candies, cheat units, heroes...)
			return false;
		}
		if (unitDefId <= 337) {
			return true; // non ROR unit below ID 337 (to confirm)
		}
		// Above 337: Cherry-picking of "known units" from vanilla AOE data
		// 338/339 = camel/scythe (ROR)
		if ((unitDefId >= 340) && (unitDefId <= 344)) {
			return true;
		}
		// 345 = armored ele ; 346=a ROR projectile
		if ((unitDefId >= 348) && (unitDefId <= 359)) {
			return true; // terrain_path_* (gaia units)
		}
		// 360=fire galley, 361-363=other various ROR units (364 unused in AOE/ROR)
		if ((unitDefId >= 365) && (unitDefId <= 370)) {
			return true; // 3 trees + 3 other units
		}
		// 371=unused
		if ((unitDefId >= 372) && (unitDefId <= 373)) {
			return true;
		}
		return false;
	}

}
