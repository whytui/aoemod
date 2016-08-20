
#pragma once

#include <string>
#include <assert.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include "crCommon.h"

namespace RPG_MODE {
	void OnUnitKill(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *killedUnit, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *actorUnit);

	// Create a dedicated unit definition for unit, only if it does not already have one
	// Unit name is modified so that it includes unit instance ID.
	// Returns true if a unit definition was created, false if there already was a dedicated unit definition.
	bool CreateDedicatedUnitDef(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);

	// Increase unit's total HP by given percentage. Increase amount is always at least 1.
	void IncreaseTotalHPByPercentage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage);

	// Increase unit's (main) attack by given percentage. Increase amount is always at least 1.
	bool IncreaseAttackByPercentage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage);

	// Applies effects of upgrading unit's level by 1.
	void UpgradeUnitLevel(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);

}
