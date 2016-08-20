
#pragma once

#include <string>
#include <assert.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include "crCommon.h"
#include "randomizer.h"

namespace RPG_MODE {

	const int maxUnitLevel = 49;
	const int maxLevelHPIncrease = 20;
	const int maxLevelAttackIncrease = 2;
	const int maxLevelArmorIncrease = 2;
	const int epicUnitPercentageChances = 8; // Chances to get a unique unit (percentage value)

	void OnUnitKill(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *killedUnit, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *actorUnit);

	// Create a dedicated unit definition for unit, only if it does not already have one
	// Unit name is modified so that it includes unit instance ID.
	// Returns true if a unit definition was created, false if there already was a dedicated unit definition.
	bool CreateDedicatedUnitDef(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);

	// Increase unit's total HP by given percentage. Total HP is always increased by at least 1 (capped by maxIncreaseValue)
	// percentage should be between 0 and 100 (in most cases !).
	bool IncreaseTotalHPByPercentage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage, int maxIncreaseValue);

	// Increase unit's (main) attack by given percentage. Increase amount is always at least 1 (capped by maxIncreaseValue)
	bool IncreaseAttackByPercentage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage, int maxIncreaseValue);

	// Increase unit's (main) attack by a given value. 
	// maxTotalValue is ignored if <0
	bool IncreaseAttackByRawValue(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int valueToAdd, int maxTotalValue);

	// Increase unit's melee armor by given percentage. Increase amount is always at least 1 (capped by maxIncreaseValue)
	bool IncreaseArmorByPercentage(AOE_CONST_FUNC::ATTACK_CLASS armorClass, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage, int maxIncreaseValue);

	// Increase unit's melee armor by a given value. 
	// maxTotalValue is ignored if <0
	bool IncreaseeArmorByRawValue(AOE_CONST_FUNC::ATTACK_CLASS armorClass, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int valueToAdd, int maxTotalValue);

	// Applies effects of upgrading unit's level by 1.
	void UpgradeUnitLevel(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);

	// Transforms a unit into a unique "epic" unit if randoms decides to.
	// Returns true if unit has been transformed into a unique unit.
	bool MakeEpicUnitWithRandomCondition(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);
}
