
#pragma once

#include <string>
#include <assert.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include "randomizer.h"
#include "unitDefHandling.h"
#include "RockNRorCommon.h"
#include "AOEPrimitives_UI_gameMain.h"

namespace RPG_MODE {

	const bool collectXpOnKills = true;
	const int maxUnitLevel = 9;
	const int maxLevelHPIncrease = 20;
	const int maxLevelAttackIncrease = 2;
	const int maxLevelArmorIncrease = 2;
	const int epicUnitPercentageChances = 8; // Chances to get a unique unit (percentage value)
	const int killsToLevelUp = 10;
	const int xpStolenProportionFromKilledUnit = 5; // 1/xpStolenProportionFromKilledUnit of killed unit's XP is credited to actor unit.
	static_assert(killsToLevelUp > 0, "killsToLevelUp must be strictly positive");
	const short int resourceTypeUsedForXp = AOE_CONST_FUNC::CST_RES_ORDER_KILLS;
	//const short int resourceTypeUsedForXp = AOE_CONST_FUNC::CST_RES_ORDER_GOLD; // Nice for testing

	// Handle RPG events when a unit kills another one
	void OnUnitKill(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *killedUnit, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *actorUnit);

	// Create a dedicated unit definition for unit, only if it does not already have one
	// Unit name is modified so that it includes unit instance ID.
	// Returns true if a unit definition was created, false if there already was a dedicated unit definition.
	// WARNING: if this returns true, make sure you are not using "unitDef" pointers on old definition !
	bool CreateDedicatedUnitDef(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);

	// Increase unit's total HP by given percentage. Total HP is always increased by at least 1 (capped by maxIncreaseValue)
	// percentage should be between 0 and 100 (in most cases !).
	// Actual increase amount is also added to unit's current HP
	bool IncreaseTotalHPByPercentage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage, int maxIncreaseValue);

	// Increase unit's (main) attack by given percentage. Increase amount is always at least 1 (capped by maxIncreaseValue)
	bool IncreaseAttackByPercentage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage, int maxIncreaseValue);

	// Increase unit's (main) attack by a given value. 
	// maxTotalValue is ignored if <0
	bool IncreaseAttackByRawValue(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int valueToAdd, int maxTotalValue);

	// Increase unit's armor by given percentage. Increase amount is always at least 1 (capped by maxIncreaseValue)
	bool IncreaseArmorByPercentage(AOE_CONST_FUNC::ATTACK_CLASS armorClass, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage, int maxIncreaseValue);

	// Increase unit's armor by a given value. 
	// maxTotalValue is ignored if <0
	bool IncreaseArmorByRawValue(AOE_CONST_FUNC::ATTACK_CLASS armorClass, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int valueToAdd, int maxTotalValue);

	// Increase unit's range (for ranged units) and LOS
	// maxTotalValue is ignored if <0
	bool IncreaseRangeByRawValue(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int valueToAdd, int maxTotalValue);

	// Applies effects of upgrading unit's level by 1.
	void UpgradeUnitLevel(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);

	// Applies random effects to a unit (this method creates a dedicated unit definition, if necessary)
	// Returns true if at least 1 attribute has been updated
	// If result is false, dedicated unit definition may NOT have been created
	bool ApplyRandomUniqueAttributes(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);

	// Transforms a unit into a unique "epic" unit if randoms decides to.
	// Returns true if unit has been transformed into a unique unit.
	bool MakeEpicUnitWithRandomCondition(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);

	// Display a "level up" message if unit belongs to human-controlled player
	bool DisplayLevelUpMessage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int newLevel);

	// Display a "new epic unit spawned" message if unit belongs to human-controlled player
	bool DisplayEpicUnitSpawnedMessage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);
}
