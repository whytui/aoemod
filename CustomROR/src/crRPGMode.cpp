
#include "../include/crRPGMode.h"

namespace RPG_MODE {

	// Handle RPG events when a unit kills another one
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

		//bool hasNewUnitDef = CreateDedicatedUnitDef(actorUnitTrainable); // True only if the dedicated unitDef is new (unit was still a "standard" unit).
		//assert(actorUnitTrainable->unitDefinition && actorUnitTrainable->unitDefinition->IsCheckSumValidForAUnitClass());
		//if (!actorUnitTrainable->unitDefinition) { return; }

		// Handle "XP"
		int addedXP = 1;
		if (killedUnit->resourceTypeId == resourceTypeUsedForXp) {
			int lootXP = (int)(killedUnit->resourceValue / xpStolenProportionFromKilledUnit);
			if (lootXP > 0) {
				addedXP += lootXP; // Gain a part of killed unit's XP (bonus)
			}
		}

		if ((actorUnitTrainable->resourceValue == 0) || (actorUnitTrainable->resourceTypeId == resourceTypeUsedForXp)) {
			actorUnitTrainable->resourceTypeId = resourceTypeUsedForXp;
			if (actorUnitTrainable->resourceValue < 0) { actorUnitTrainable->resourceValue = 0; }
			int currentLevel = (int)actorUnitTrainable->resourceValue / killsToLevelUp;
			actorUnitTrainable->resourceValue += addedXP;
			int newLevel = (int)actorUnitTrainable->resourceValue / killsToLevelUp;
			if ((newLevel > currentLevel) && (newLevel <= maxUnitLevel)) {
				for (int i = currentLevel; i < newLevel; i++) {
					UpgradeUnitLevel(actorUnitTrainable);
				}
				if (DisplayLevelUpMessage(actorUnitTrainable, newLevel)) { // True only if unit belongs to human player
					AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
					if (settings && settings->IsCheckSumValid()) {
						settings->AddEventInHistory((long int)actorUnit->positionX, (long int)actorUnit->positionY);
					}
				}
			}
		}
	}

	// Create a dedicated unit definition for unit, only if it does not already have one
	// Unit name is modified so that it includes unit instance ID.
	// Returns true if a unit definition was created, false if there already was a dedicated unit definition.
	// WARNING: if this returns true, make sure you are not using "unitDef" pointers on old definition !
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


		if (newUnitDef->resourceDecay <= 0) {
			newUnitDef->resourceDecay = 2; // Otherwise, units remain stuck at status 5 and never completely die.
		}
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
		if (newValue < unitDef->ptrAttacksList[bestAttackIndex].amount) {
			return false; // Do not decrease existing value
		}
		unitDef->ptrAttacksList[bestAttackIndex].amount = newValue;
		return true;
	}


	// Increase unit's armor by given percentage. Increase amount is always at least 1 (capped by maxArmorIncrease)
	bool IncreaseArmorByPercentage(AOE_CONST_FUNC::ATTACK_CLASS armorClass, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage, int maxIncreaseValue) {
		assert(percentage >= 0);
		percentage += 100;
		assert(unit && unit->DerivesFromTrainable());
		assert(unit->unitDefinition && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition->DerivesFromTrainable());
		CreateDedicatedUnitDef(unit); // Ensure we modify a specific unit definition !
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;

		short int currentArmor = AOE_STRUCTURES::GetArmorFromList(unitDef, armorClass, -1);
		int newValue = currentArmor * percentage / 100;
		if (newValue <= currentArmor) {
			newValue = currentArmor + 1;
		}
		if (newValue > currentArmor + maxIncreaseValue) { newValue = currentArmor + maxIncreaseValue; } // limit the increase.
		AOE_STRUCTURES::SetArmorInList(unitDef, armorClass, newValue, true); // Add armor if not existing in list
		return true;
	}


	// Increase unit's armor by a given value. 
	// maxTotalValue is ignored if <0
	bool IncreaseArmorByRawValue(AOE_CONST_FUNC::ATTACK_CLASS armorClass, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int valueToAdd, int maxTotalValue) {
		assert(unit && unit->DerivesFromTrainable());
		assert(unit->unitDefinition && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition->DerivesFromTrainable());
		CreateDedicatedUnitDef(unit); // Ensure we modify a specific unit definition !
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
		
		short int currentArmor = AOE_STRUCTURES::GetArmorFromList(unitDef, armorClass, -1);
		int newValue = currentArmor + valueToAdd;
		if ((newValue > maxTotalValue) && (maxTotalValue >= 0)) {
			newValue = maxTotalValue;
		}
		if (newValue < currentArmor) {
			return false; // Do not decrease existing value
		}
		AOE_STRUCTURES::SetArmorInList(unitDef, armorClass, newValue, true); // Add armor if not existing in list
		return true;
	}


	// Increase unit's range (for ranged units) and LOS
	// maxTotalValue is ignored if <0
	bool IncreaseRangeByRawValue(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int valueToAdd, int maxTotalValue) {
		assert(unit && unit->DerivesFromTrainable());
		assert(unit->unitDefinition && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition->DerivesFromTrainable());
		CreateDedicatedUnitDef(unit); // Ensure we modify a specific unit definition !
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass());
		if (unitDef->maxRange + valueToAdd > maxTotalValue) {
			valueToAdd = maxTotalValue - (int)unitDef->maxRange;
		}
		if (unitDef->maxRange > 0) {
			unitDef->maxRange += valueToAdd;
		}
		unitDef->lineOfSight += valueToAdd; // Add same bonus to LOS
		return true;
	}


	// Applies effects of upgrading unit's level by 1.
	void UpgradeUnitLevel(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit) {
		assert(unit && unit->IsCheckSumValidForAUnitClass());
		if (!unit || !unit->ptrStructPlayer) { return; }
		STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		assert(settings && settings->IsCheckSumValid());
		assert(global && global->IsCheckSumValid());
		if (!settings || !settings->IsCheckSumValid() || !global || !global->IsCheckSumValid()) { return; }
		bool isHumanPlayer = (global->humanPlayerId == unit->ptrStructPlayer->playerId);
		if ((settings->difficultyLevel >= 3) && (!isHumanPlayer)) {
			return; // No upgrades for AI players in easy levels
		}

		bool hasNewUnitDef = CreateDedicatedUnitDef(unit); // True only if the dedicated unitDef is new (unit was still a "standard" unit).
		assert(unit->unitDefinition && unit->unitDefinition->IsCheckSumValidForAUnitClass());
		if (!unit->unitDefinition) {
			traceMessageHandler.WriteMessage("Duplicate unit definition appears to have failed, this is serious :(");
			return;
		}

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


	// Applies random effects to a unit (this method creates a dedicated unit definition, if necessary)
	// Returns true if at least 1 attribute has been updated
	// If result is false, dedicated unit definition may NOT have been created
	bool ApplyRandomUniqueAttributes(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit) {
		assert(unit && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition);
		if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->unitDefinition) { return false; }
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
		bool hasImprovedAttribute = false;
		bool isMelee = (unitDef->projectileUnitId < 0) && (unitDef->unitAIType != TribeAIGroupPriest);
		bool isRanged = (unitDef->projectileUnitId >= 0); // Ranged or (ranged)siege weapons

		// Settings for attribute upgrades choice
		const int chancesUpgradeAttack = (unitDef->unitAIType != TribeAIGroupPriest) ? 20 : 0;
		const int chancesUpgradeMeleeArmor = 20;
		const int chancesUpgradePierceArmor = 5;
		const int chancesUpgradeRange = isMelee ? 1 : 10; // 1% chance to upgrade LOS (only - no range !) for melee units.
		const int chancesUpgradeHP = 40;

		const bool improveAttack = (unitDef->unitAIType != TribeAIGroupPriest) && (randomizer.GetRandomNonZeroPercentageValue() <= chancesUpgradeAttack);
		const bool improveMeleeArmor = (randomizer.GetRandomNonZeroPercentageValue() <= chancesUpgradeMeleeArmor);
		const bool improvePierceArmor = (randomizer.GetRandomNonZeroPercentageValue() <= chancesUpgradePierceArmor);
		const bool improveRange = isRanged && (randomizer.GetRandomNonZeroPercentageValue() <= chancesUpgradeRange);
		const bool improveHP = (randomizer.GetRandomNonZeroPercentageValue() <= chancesUpgradeHP);

		if (!improveAttack && !improveMeleeArmor && !improvePierceArmor && !improveRange && !improveHP) {
			return false; // Note: we did NOT create a dedicated unit def as it is not necessary
		}

		CreateDedicatedUnitDef(unit); // Ensure the unit definition is dedicated so we can customize it.
		unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition; // IMPORTANT: update our local pointer, old one is now invalid
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass());
		DisplayEpicUnitSpawnedMessage(unit);

		// Attack (all but priests)
		if (improveAttack) {
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
			hasImprovedAttribute |= IncreaseAttackByRawValue(unit, maxRawValue, -1);
		}

		// Melee Armor (all, limited amount for "no-armor" units)
		if (improveMeleeArmor) {
			int maxRawValue = 2;
			if ((unitDef->unitAIType != TribeAIGroupPriest) && (unitDef->unitAIType != TribeAIGroupSiegeWeapon)) {
				// Allow better armor upgrades, but not for "no-armor" units (excludes priests, siege weapons...)
				maxRawValue = randomizer.GetRandomValue_normal_moreFlat(1, 5);
			}
			hasImprovedAttribute |= IncreaseArmorByRawValue(AOE_CONST_FUNC::CST_AC_BASE_MELEE, unit, maxRawValue, -1);
		}

		// Pierce armor (low chances) (amount = 1 always)
		if (improvePierceArmor) {
			hasImprovedAttribute |= IncreaseArmorByRawValue(AOE_CONST_FUNC::CST_AC_BASE_PIERCE, unit, 1, -1);
		}

		// Range (archers, limited amount)
		if (improveRange) {
			int maxImprovement = 5;
			if (unitDef->unitAIType == TribeAIGroupSiegeWeapon) { maxImprovement = 3; }
			if (unitDef->unitAIType == TribeAIGroupPriest) { maxImprovement = 3; } // Note: at this point, priests are excluded from this part cf isRanged
			if (unitDef->displayedRange - unitDef->maxRange < maxImprovement) {
				hasImprovedAttribute |= IncreaseRangeByRawValue(unit, 1, -1);
			}
		}

		// HP (all)
		if (improveHP) {
			hasImprovedAttribute |= IncreaseTotalHPByPercentage(unit, randomizer.GetRandomValue_normal_moreFlat(10, 35), 100);
		}
		if (!hasImprovedAttribute) {
			// Unit has no upgraded attribute. Force one (with a slight HP boost).
			// Now we are sure the unit has at least 1 specificity
			unitDef->totalHitPoints += 15;
		}
		return true;
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

		STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		assert(settings && settings->IsCheckSumValid());
		assert(global && global->IsCheckSumValid());
		if (!settings || !settings->IsCheckSumValid() || !global || !global->IsCheckSumValid()) { return false; }
		bool isHumanPlayer = (global->humanPlayerId == unit->ptrStructPlayer->playerId);
		if ((settings->difficultyLevel >= 3) && (!isHumanPlayer)) {
			return false; // No epic units for AI players in easy levels
		}

		int random = randomizer.GetRandomNonZeroPercentageValue();
		if (random > epicUnitPercentageChances) {
			return false;
		}
		// Here we decided to actually make the unit unique.
		return ApplyRandomUniqueAttributes(unit);
	}


	// Display a "level up" message if unit belongs to human-controlled player
	bool DisplayLevelUpMessage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int newLevel) {
		assert(unit && unit->unitDefinition);
		if (!unit || !unit->unitDefinition) { return false; }
		if (unit->ptrStructPlayer != GetControlledPlayerStruct_Settings()) {
			return false; // only display messages for owned units
		}
		std::string msg = unit->unitDefinition->ptrUnitName;
		msg += " upgraded to level ";
		msg += std::to_string(newLevel);
		CallWriteText(msg.c_str());
		return true;
	}

	// Display a "new epic unit spawned" message if unit belongs to human-controlled player
	bool DisplayEpicUnitSpawnedMessage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit) {
		assert(unit && unit->unitDefinition && unit->unitDefinition->IsCheckSumValidForAUnitClass());
		if (!unit || !unit->unitDefinition) { return false; }
		if (unit->ptrStructPlayer != GetControlledPlayerStruct_Settings()) {
			return false; // only display messages for owned units
		}
		std::string msg = unit->unitDefinition->ptrUnitName;
		msg += " spawned as an epic unit";
		CallWriteText(msg.c_str());
		return true;
	}
}
