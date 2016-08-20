
#include "../include/crRPGMode.h"

namespace RPG_MODE {

	void OnUnitKill(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *killedUnit, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *actorUnit) {
		if (!actorUnit || !actorUnit->IsCheckSumValidForAUnitClass()) { return; }
		if (!killedUnit || !killedUnit->IsCheckSumValidForAUnitClass()) { return; }
		assert(killedUnit->DerivesFromAttackable());
		if (!killedUnit->DerivesFromAttackable()) { return; }
		assert(killedUnit->DerivesFromTrainable());
		if (!actorUnit->DerivesFromTrainable()) { return; }

		assert(actorUnit->unitDefinition && actorUnit->unitDefinition->IsCheckSumValidForAUnitClass());
		assert(killedUnit->unitDefinition && killedUnit->unitDefinition->IsCheckSumValidForAUnitClass());

		if (!actorUnit->ptrStructPlayer || !killedUnit->ptrStructPlayer || (killedUnit->ptrStructPlayer->playerId == 0)) {
			return; // Killing gaia units has no effect here
		}
		if (actorUnit->ptrStructPlayer == killedUnit->ptrStructPlayer) {
			return; // No custom treatments for killing own units
		}
		if (killedUnit->ptrStructPlayer->ptrDiplomacyStances[actorUnit->ptrStructPlayer->playerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY) {
			return; // No custom treatments for killing allied units
		}

		bool actorIsBuilding = false;
		if (actorUnit->unitDefinition->unitType == GUT_BUILDING) {
			actorIsBuilding = true;
			assert(((AOE_STRUCTURES::STRUCT_UNIT_BUILDING*)actorUnit)->IsCheckSumValid());
		}
		bool killedIsBuilding = false;
		if (killedUnit->unitDefinition->unitType == GUT_BUILDING) {
			killedIsBuilding = true;
			assert(((AOE_STRUCTURES::STRUCT_UNIT_BUILDING*)killedUnit)->IsCheckSumValid());
		}

		AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *actorUnitTrainable = (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *)actorUnit;
		assert(actorUnitTrainable->DerivesFromTrainable());

		bool hasNewUnitDef = CreateDedicatedUnitDef(actorUnitTrainable); // True only if the dedicated unitDef is new (unit was still a "standard" unit).
		assert(actorUnitTrainable->unitDefinition && actorUnitTrainable->unitDefinition->IsCheckSumValidForAUnitClass());
		if (!actorUnitTrainable->unitDefinition) { return; }

		// Handle "XP"
		int addedXP = 1;
		if (killedUnit->resourceTypeId == CST_RES_ORDER_KILLS) {
			int lootXP = (int)(killedUnit->resourceValue / 5);
			if (lootXP > 0) {
				addedXP += lootXP; // Gain a part of killed unit's XP (bonus)
			}
		}

		if ((actorUnitTrainable->resourceValue == 0) || (actorUnitTrainable->resourceTypeId == CST_RES_ORDER_KILLS)) {
			actorUnitTrainable->resourceTypeId = CST_RES_ORDER_KILLS;
			if (actorUnitTrainable->resourceValue < 0) { actorUnitTrainable->resourceValue = 0; }
			int currentLevel = (int)actorUnitTrainable->resourceValue / 10;
			actorUnitTrainable->resourceValue += addedXP;
			int newLevel = (int)actorUnitTrainable->resourceValue / 10;
			if ((newLevel > currentLevel) && (newLevel < maxUnitLevel)) {
				//std::string msg = "Unit ";
				//msg += std::to_string(actorUnit->unitInstanceId);
				std::string msg = actorUnitTrainable->unitDefinition->ptrUnitName;
				msg += " upgraded to level ";
				msg += std::to_string(newLevel);
				CallWriteText(msg.c_str());
				// TODO: Add an event in "home key" history
				for (int i = currentLevel; i <= newLevel; i++) {
					UpgradeUnitLevel(actorUnitTrainable);
				}
				AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
				if (settings && settings->IsCheckSumValid()) {
					settings->AddEventInHistory((long int)actorUnit->positionX, (long int)actorUnit->positionY);
				}
			}
		}
	}

	// Create a dedicated unit definition for unit, only if it does not already have one
	// Unit name is modified so that it includes unit instance ID.
	// Returns true if a unit definition was created, false if there already was a dedicated unit definition.
	bool CreateDedicatedUnitDef(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit) {
		if (unit->hasDedicatedUnitDef) {
			return false;
		}

		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *newUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)CopyUnitDefToNewUsingGoodClass(unit->unitDefinition);
		// To be more precise, newUnitDef is STRUCT_UNITDEF_BUILDING if actorUnitTrainable is a building
		unit->hasDedicatedUnitDef = 1; // ROR will automatically free our new "dedicated" unitDef when necessary (this is a standard feature)
		unit->unitDefinition = newUnitDef;
		assert(newUnitDef && newUnitDef->DerivesFromTrainable());
		char *initialName = newUnitDef->ptrUnitName;
		std::string name = GetLanguageDllText(newUnitDef->languageDLLID_Name);
		if (name.empty()) {
			name = std::string(newUnitDef->ptrUnitName);
		}
		if (!name.empty()) {
			name += " ";
		}
		name += std::to_string(unit->unitInstanceId);

		char *newName = (char*)AOEAlloc(name.length() + 1); // unitdef name has its own allocated memory.
		sprintf_s(newName, name.length() + 1, "%s", name.c_str());
		AOEFree(initialName); // Free previous name memory
		newUnitDef->ptrUnitName = newName; // Replace unitdef name.
		newUnitDef->languageDLLID_Name = -1; // Prevent from using name from language(x).dll
		return true;
	}


	// Increase unit's total HP by given percentage. Total HP is always increased by at least 1 (capped by maxHPIncrease)
	// percentage should be between 0 and 100 (in most cases !).
	bool IncreaseTotalHPByPercentage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage, int maxIncreaseValue) {
		assert(percentage >= 0);
		percentage += 100;
		assert(unit && unit->DerivesFromTrainable());
		if (!unit || !unit->DerivesFromTrainable()) { return false; }
		assert(unit->unitDefinition && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition->DerivesFromTrainable());
		CreateDedicatedUnitDef(unit); // Ensure we modify a specific unit definition !
		int currentTotalHP = unit->unitDefinition->totalHitPoints;
		int newTotalHP = currentTotalHP * percentage / 100;
		if (newTotalHP <= currentTotalHP) {
			newTotalHP = currentTotalHP + 1;
		}
		if (newTotalHP > currentTotalHP + maxIncreaseValue) { newTotalHP = currentTotalHP + maxIncreaseValue; } // limit the increase.

		unit->unitDefinition->totalHitPoints = newTotalHP;
		return true;
	}


	// Increase unit's (main) attack by given percentage. Increase amount is always at least 1 (capped by maxAttackIncrease)
	bool IncreaseAttackByPercentage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage, int maxIncreaseValue) {
		assert(percentage >= 0);
		percentage += 100;
		assert(unit && unit->DerivesFromTrainable());
		assert(unit->unitDefinition && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition->DerivesFromTrainable());
		CreateDedicatedUnitDef(unit); // Ensure we modify a specific unit definition !
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
		if (unitDef->attacksCount <= 0) { return false; }
		int bestAttackValue = 0; // This automatically excludes negative/null attacks
		int bestAttackIndex = -1;
		for (int i = 0; i < unitDef->attacksCount; i++) {
			if (unitDef->ptrAttacksList[i].amount > bestAttackValue) {
				bestAttackIndex = i;
				bestAttackValue = unitDef->ptrAttacksList[i].amount;
			}
		}
		if (bestAttackIndex < 0) { return false; } // Failed finding a relevant attack to upgrade.
		int newValue = bestAttackValue * percentage / 100;
		if (newValue <= bestAttackValue) {
			newValue = bestAttackValue + 1;
		}
		if (newValue > bestAttackValue + maxIncreaseValue) { newValue = bestAttackValue + maxIncreaseValue; } // limit the increase.
		unitDef->ptrAttacksList[bestAttackIndex].amount = newValue;
		return true;
	}


	// Increase unit's (main) attack by a given value. 
	// maxTotalValue is ignored if <0
	bool IncreaseAttackByRawValue(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int valueToAdd, int maxTotalValue) {
		assert(unit && unit->DerivesFromTrainable());
		assert(unit->unitDefinition && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition->DerivesFromTrainable());
		CreateDedicatedUnitDef(unit); // Ensure we modify a specific unit definition !
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
		if (unitDef->attacksCount <= 0) { return false; }
		int bestAttackValue = 0; // This automatically excludes negative/null attacks
		int bestAttackIndex = -1;
		for (int i = 0; i < unitDef->attacksCount; i++) {
			if (unitDef->ptrAttacksList[i].amount > bestAttackValue) {
				bestAttackIndex = i;
				bestAttackValue = unitDef->ptrAttacksList[i].amount;
			}
		}
		if (bestAttackIndex < 0) { return false; } // Failed finding a relevant attack to upgrade.
		int newValue = unitDef->ptrAttacksList[bestAttackIndex].amount + valueToAdd;
		if ((newValue > maxTotalValue) && (maxTotalValue >= 0)) {
			newValue = maxTotalValue;
		}
		unitDef->ptrAttacksList[bestAttackIndex].amount = newValue;
		return true;
	}


	// Increase unit's melee armor by given percentage. Increase amount is always at least 1 (capped by maxArmorIncrease)
	bool IncreaseArmorByPercentage(AOE_CONST_FUNC::ATTACK_CLASS armorClass, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage, int maxIncreaseValue) {
		assert(percentage >= 0);
		percentage += 100;
		assert(unit && unit->DerivesFromTrainable());
		assert(unit->unitDefinition && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition->DerivesFromTrainable());
		CreateDedicatedUnitDef(unit); // Ensure we modify a specific unit definition !
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
		for (int i = 0; i < unitDef->armorsCount; i++) {
			if (unitDef->ptrArmorsList[i].classId == armorClass) {
				int armorValue = unitDef->ptrArmorsList[i].amount;
				int newValue = armorValue * percentage / 100;
				if (newValue <= armorValue) {
					newValue = armorValue + 1;
				}
				if (newValue > armorValue + maxIncreaseValue) { newValue = armorValue + maxIncreaseValue; } // limit the increase.
				unitDef->ptrArmorsList[i].amount = newValue;
				return true;
			}
		}
		// TODO: add melee to armors ?
		return false;
	}


	// Increase unit's melee armor by a given value. 
	// maxTotalValue is ignored if <0
	bool IncreaseeArmorByRawValue(AOE_CONST_FUNC::ATTACK_CLASS armorClass, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int valueToAdd, int maxTotalValue) {
		assert(unit && unit->DerivesFromTrainable());
		assert(unit->unitDefinition && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition->DerivesFromTrainable());
		CreateDedicatedUnitDef(unit); // Ensure we modify a specific unit definition !
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
		for (int i = 0; i < unitDef->armorsCount; i++) {
			if (unitDef->ptrArmorsList[i].classId == armorClass) {
				int newValue = unitDef->ptrArmorsList[i].amount + valueToAdd;
				if ((newValue > maxTotalValue) && (maxTotalValue >= 0)) {
					newValue = maxTotalValue;
				}
				unitDef->ptrArmorsList[i].amount = newValue;
				return true;
			}
		}
		// TODO: add melee to armors ?
		return false;
	}


	// Applies effects of upgrading unit's level by 1.
	void UpgradeUnitLevel(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit) {
		int random = randomizer.GetRandomNonZeroPercentageValue();
		if (random < 6) { // values 1-5
			IncreaseAttackByPercentage(unit, 3, maxLevelAttackIncrease);
			return;
		}
		if (random < 11) { // values 6-10
			IncreaseArmorByPercentage(AOE_CONST_FUNC::CST_AC_BASE_MELEE, unit, 3, maxLevelArmorIncrease);
			return;
		}
		if (random < 13) { // values 11-12 (low chances)
			IncreaseArmorByPercentage(AOE_CONST_FUNC::CST_AC_BASE_PIERCE, unit, 1, 1);
			return;
		}

		IncreaseTotalHPByPercentage(unit, 5, maxLevelHPIncrease);
	}

	// Transforms a unit into a unique "epic" unit if randoms decides to.
	// Returns true if unit has been transformed into a unique unit.
	bool MakeEpicUnitWithRandomCondition(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit) {
		assert(unit && unit->IsCheckSumValidForAUnitClass());
		if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
		if (!unit->IsTypeValid()) { return false; } // Only trainable (excluding buildings) can become unique units here
		if (!unit->unitDefinition || !unit->unitDefinition->IsCheckSumValidForAUnitClass()) { return false; }
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
		if (unitDef->villagerMode) { return false; } // unit that can switch unit definition can't be unique (changing unit definition would loose their specificity).

		int random = randomizer.GetRandomNonZeroPercentageValue();
		if (random > epicUnitPercentageChances) {
			return false;
		}
		// Here we decided to actually make the unit unique.
		CreateDedicatedUnitDef(unit); // Ensure the unit definition is dedicated so we can customize it.
		assert(unit && unit->unitDefinition && unit->unitDefinition->IsCheckSumValidForAUnitClass());
		std::string msg = unit->unitDefinition->ptrUnitName;
		msg += " spawned as an epic unit";
		CallWriteText(msg.c_str());
		/* Do not add an event history position, because there's already one when a unit spawns.
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		if (settings && settings->IsCheckSumValid()) {
			settings->AddEventInHistory((long int)unit->positionX, (long int)unit->positionY);
		}*/

		random = randomizer.GetRandomNonZeroPercentageValue(); // To decide the type of bonus
		bool success = false;
		bool isMelee = (unitDef->maxRange == 0);
		if ((random <= 25) && (unitDef->unitAIType != TribeAIGroupPriest)) {
			// Attack (all but priests)
			int maxRawValue = 1;
			if (isMelee || (unitDef->displayedAttack > 20)) { // Melee or siege weapons
				if (unitDef->blastRadius > 0) {
					maxRawValue = randomizer.GetRandomValue_normal_moreFlat(1, 3); // Limited upgrade value for units with blast damage
				} else {
					maxRawValue = randomizer.GetRandomValue_normal_moreFlat(1, 5);
				}
			}
			if (maxRawValue > unitDef->displayedAttack) {
				maxRawValue = unitDef->displayedAttack; // Do not add more than "nominal" (base) value
			}
			success = IncreaseAttackByRawValue(unit, maxRawValue, -1);
			if (!success) {
				traceMessageHandler.WriteMessageNoNotification("failed");
			}
			return true;
		}
		if (random <= 50) {
			// Melee Armor (all, limited amount for "no-armor" units)
			int maxRawValue = 1;
			if ((unitDef->unitAIType != TribeAIGroupPriest) && (unitDef->unitAIType != TribeAIGroupSiegeWeapon)) {
				// Allow better armor upgrades, but not for "no-armor" units (excludes priests, siege weapons...)
				maxRawValue = randomizer.GetRandomValue_normal_moreFlat(1, 5);
			}
			success = IncreaseeArmorByRawValue(AOE_CONST_FUNC::CST_AC_BASE_MELEE, unit, maxRawValue, 5);
			if (!success) {
				traceMessageHandler.WriteMessageNoNotification("failed");
			}
			return true;
		}
		// Pierce armor (low chances) (amount = 1 always)

		// Range (archers, limited amount)

		// HP (all)
		success = IncreaseTotalHPByPercentage(unit, randomizer.GetRandomValue_normal_moreFlat(10, 30), 100);
		if (!success) {
			traceMessageHandler.WriteMessageNoNotification("failed");
		}
		return true;
	}
}
