#include <AOE_empires_dat.h>
#include <string>

#pragma once

/*
 * This file contains public functional values that generally correspond to empires.dat data.
*/

namespace AOE_CONST_FUNC {

	static const char *UNIT_CLASS_NAMES[] { "Foot archer", // 0
		"Artefact",
		"Trade Boat",
		"Building",
		"Civilian",
		"Sea Fish",
		"Barracks Infantry", // TribeAIGroupFootSoldier
		"Berry Bush",
		"Stone Mine",
		"Prey Animal",

		"Predator Animal", // 10
		"OtherDeadProjectile",
		"Cavalry", // TribeAIGroupMountedSoldier
		"Siege Weapon",
		"Terrain", // For terrain units
		"Tree",
		"TreeStump", //16
		"Healer (non-default)", // unused in standard game
		"Priest",
		"Trade Cart",

		"Transport Boat", // 20
		"Fishing Boat",
		"War Boat", // Including fire galley, catapult trireme...
		"Chariot Archer",
		"Elephant (melee)",
		"Hero",
		"Elephant Archer",
		"Wall",
		"Phalanx",
		"Domesticated Animal",

		"Flag", // 30
		"Unknown Fish",
		"Gold Mine",
		"Shore Fish",
		"Cliff",
		"Chariot (melee)", // 35
		"Horse archer",
		"Smoke",
		"Bird",
		"Slinger",

		"?", // 40
		"?",
		"?",
		"?",
		"?",
		"?",
		"?",
		"?",
		"v",
		"?",
		"Farm (non-default)",

		"?", //50
		"?",
		"Tower (non-default)",
		"?",
		"?",
		"?",
		"?",
		"?",
		"?",
		"?",
		"?",

		"Horse" // 61
	};

	// Returns hardcoded unit class name (AI type), in english.
	static const char *GetUnitClassName(GLOBAL_UNIT_AI_TYPES c) {
		if (c < 0) { return "None"; }
		if (c > GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorse) { return "Invalid"; }
		return UNIT_CLASS_NAMES[c];
	}


	static const char *RESOURCE_TYPE_NAMES[] { "Food",
		"Wood",
		"Stone",
		"Gold",
		"Population headroom",
		"Unknown05",
		"Current age",
		"Relics count",
		"Trade bonus",
		"Trade goods", // Available trade goods, visible in each dock

		"Trade production", // Default 1.
		"Current population",
		"Decay Time", // 0 for players, used at unit level
		"Discoveries count",
		"Ruins count",
		"Meat storage", // unused ?
		"Berry storage", // unused ?
		"Fish storage", // unused ?
		"Unknown18",
		"Total units owned", // Total number of (*all*) units, not decreased when a unit is lost

		"Kills", // 20
		"Technologies count",
		"Exploration",
		"Bronze age ID",
		"Iron age ID",
		"Tool age ID",
		"Attack alert sound",
		"Can convert priests",
		"Can convert buildings",
		"Unknown29",

		"Building count", // 30
		"Unknown31",
		"Population limit", // Default 50
		"Unknown33",
		"Faith", // Used at unit level (priest), not at player level
		"Faith recharging rate", // Default 2
		"Farm food amount",
		"Civilian population", // counts TribeAIGroupCivilian, TribeAIGroupFishingBoat, TribeAIGroupTradeBoat
		"Unknown38",
		"All techs",

		// Current number of military units, including towers, living units except (TribeAIGroupPriest, TribeAIGroupCivilian, TribeAIGroupFishingBoat, TribeAIGroupTradeBoat).
		// All units count as 1 here, even infantry with logistics.
		"Military population",
		"Conversions count",
		"Standing wonders count",
		"Razings",
		"Kill ratio",
		"Survival", // 0/1
		"Tribute cost", // 0.25 default
		"Gold mining productivity",
		"Town center unavailable",
		"Gold counter", // Amount of collected gold (excluding tributes and initial resources), cf score (achievements screen)

		"Has shared exploration", // 50
		"Houses count",
		"Temples count",
		"Paid tribute",
		"All ruins",
		"All relics",
		"Healing rate",
		"Can sacrifice priest" // 1 if martyrdom has been developed
	};

	// Returns hardcoded resource name, in english
	static const char *GetResourceName(RESOURCE_TYPES r) {
		if (r < 0) { return "None"; }
		if (r >= RESOURCE_TYPES::CST_RES_COUNT) { return "Invalid"; }
		return RESOURCE_TYPE_NAMES[r];
	}


	static const char *ATTACK_CLASS_NAMES[] { "Ballista on buildings",
		"Slinger on archers",
		"Unknown2",
		"Base pierce",
		"Base melee",
		"Unknown5",
		"Catapults on buildings",
		"Chariots on priests",
		"Camel on Cavalry",
		"Cavalry on infantry",

		"Villager siegecraft",
		"Unknown11",
		"Agressive animals" // Only gazelles have armor against this, that's why they take only 2 damage vs lions
	};
	static const char *ATTACK_CLASS_SHORT_NAMES[] {
		"BallistaVsBuildings", "ArchersWeakness", "", "Pierce", "Melee", "Atk5?",
			"CatapultsVsBuildings", "PriestWeakness", "CavalryWeakness", "InfantryWeakness", "Siegecraft", "Atk11?", "AnimalAttack"
	};

	// Returns hardcoded attack class (or armor class) name, in english.
	// Custom classes are returned as "Invalid"
	static const char *GetAttackOrArmorClassName(ATTACK_CLASS a) {
		if (a < 0) { return "None"; }
		if (a >= CST_AC_COUNT) {
			return "Invalid";
		}
		return ATTACK_CLASS_NAMES[a];
	}

	// Returns hardcoded attack class (or armor class) name, in english
	// Custom classes are returned as "Custom_xxx" where xxx is class Id.
	static const std::string GetAttackOrArmorClassNameString(ATTACK_CLASS a) {
		if (a < 0) { return "None"; }
		if (a >= CST_AC_COUNT) {
			std::string tmp = std::string("Custom_") + std::to_string(a);
			return tmp.c_str();
		}
		return ATTACK_CLASS_NAMES[a];
	}

	// Tech effects unit attribute names for RGE attribute types (<100)
	static const char* RGE_TECH_UNIT_ATTRIBUTE_NAMES[] { "Hit Points",
		"Line Of Sight",
		"Garrison Capacity",
		"SizeY",
		"SizeX",
		"Speed",
		"Rotation Speed",
		"Unknown07",
		"Armor",
		"Attack",
		"Attack Reload Time", // 10
		"Accuracy percent",
		"Range",
		"Work rate",
		"Resource capacity",
		"Default armor",
		"Projectile unit",
		"Angle",
		"Damage Terrain Restriction",
		"Intelligent Projectile" // 19=last from RGE format
	};

	// Tech effects unit attribute names for Tribe attribute types (>=100)
	static const char* TRIBE_TECH_UNIT_ATTRIBUTE_NAMES[] { "Add cost amount", "Population cost"};

	// Safely gets a technology effect attribute type name
	static const char *GetTechUnitAttributeName(TECH_UNIT_ATTRIBUTES a) {
		if (a < 0) { return "None"; }
		if (a <= TUA_INTELLIGENT_PROJECTILE) {
			return RGE_TECH_UNIT_ATTRIBUTE_NAMES[a];
		}
		if ((a >= 100) && (a <= TUA_POPULATION_COST)) {
			return TRIBE_TECH_UNIT_ATTRIBUTE_NAMES[a];
		}
		return "Invalid";
	}
}
