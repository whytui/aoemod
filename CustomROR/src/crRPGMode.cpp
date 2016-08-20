
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
#ifdef _DEBUG
		// TEST - TODO: add parameter, to finish...
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
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *newUnitDef = NULL;

		bool hasNewUnitDef = CreateDedicatedUnitDef(actorUnitTrainable); // True only if the dedicated unitDef is new (unit was still a "standard" unit).

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
			if ((newLevel > currentLevel) && (newLevel < 100)) {
				std::string msg = "Unit ";
				msg += std::to_string(actorUnit->unitInstanceId);
				msg += " upgraded to level ";
				msg += std::to_string(newLevel);
				CallWriteText(msg.c_str());
				// TODO: Add an event in "home key" history
				for (int i = currentLevel; i <= newLevel; i++) {
					UpgradeUnitLevel(actorUnitTrainable);
				}
			}
		}
#endif
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


	// Increase unit's total HP by given percentage. Total HP is always increased by at least 1.
	// percentage should be between 0 and 100 (in most cases !).
	void IncreaseTotalHPByPercentage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage) {
		assert(percentage >= 0);
		percentage += 100;
		assert(unit && unit->DerivesFromTrainable());
		assert(unit->unitDefinition && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition->DerivesFromTrainable());
		CreateDedicatedUnitDef(unit); // Ensure we modify a specific unit definition !
		int currentTotalHP = unit->unitDefinition->totalHitPoints;
		int newTotalHP = currentTotalHP * percentage / 100;
		if (newTotalHP <= currentTotalHP) {
			newTotalHP = currentTotalHP + 1;
		}
		if (newTotalHP > currentTotalHP + 20) { newTotalHP = currentTotalHP + 20; } // limit the increase.

		unit->unitDefinition->totalHitPoints = newTotalHP;
	}


	// Increase unit's (main) attack by given percentage. Increase amount is always at least 1.
	bool IncreaseAttackByPercentage(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit, int percentage) {
		assert(percentage >= 0);
		percentage += 100;
		assert(unit && unit->DerivesFromTrainable());
		assert(unit->unitDefinition && unit->IsCheckSumValidForAUnitClass() && unit->unitDefinition->DerivesFromTrainable());
		CreateDedicatedUnitDef(unit); // Ensure we modify a specific unit definition !
		AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unit->unitDefinition;
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
		if (newValue > bestAttackValue + 4) { newValue = bestAttackValue + 4; } // limit the increase. Attacks are generally low values !
		unitDef->ptrAttacksList[bestAttackIndex].amount = newValue;
		return true;
	}


	// Applies effects of upgrading unit's level by 1.
	void UpgradeUnitLevel(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit) {
		IncreaseTotalHPByPercentage(unit, 5);
	}
}
