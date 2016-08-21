#include "../include/unitDefHandling.h"

namespace AOE_STRUCTURES {

	// This adds an armor class to unitdef's armors list. This does NOT do any check on potential duplicates !
	bool AddArmorToList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS armorClass, int value) {
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromAttackable());
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass() || !unitDef->DerivesFromAttackable()) { return false; }
		assert(unitDef->armorsCount >= 0);
		if (unitDef->armorsCount < 0) {
			unitDef->armorsCount = 0;
		}
		STRUCT_ARMOR_OR_ATTACK *oldPtr = unitDef->ptrArmorsList;
		unitDef->ptrArmorsList = (STRUCT_ARMOR_OR_ATTACK*)AOEAllocZeroMemory(unitDef->armorsCount + 1, sizeof(STRUCT_ARMOR_OR_ATTACK));
		// Copy existing armor data
		size_t copyBytesSize = sizeof(STRUCT_ARMOR_OR_ATTACK) * unitDef->armorsCount;
		size_t actualBufferSize = sizeof(STRUCT_ARMOR_OR_ATTACK) * (unitDef->armorsCount + 1);
		memcpy_s(unitDef->ptrArmorsList, actualBufferSize, oldPtr, copyBytesSize);
		unitDef->ptrArmorsList[unitDef->armorsCount].amount = value;
		unitDef->ptrArmorsList[unitDef->armorsCount].classId = armorClass;
		unitDef->armorsCount++;
		AOEFree(oldPtr);
		return true;
	}

	// This adds an attack class to unitdef's attacks list. This does NOT do any check on potential duplicates !
	bool AddAttackToList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS attackClass, int value) {
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromAttackable());
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass() || !unitDef->DerivesFromAttackable()) { return false; }
		assert(unitDef->attacksCount >= 0);
		if (unitDef->attacksCount < 0) {
			unitDef->attacksCount = 0;
		}
		STRUCT_ARMOR_OR_ATTACK *oldPtr = unitDef->ptrAttacksList;
		unitDef->ptrAttacksList = (STRUCT_ARMOR_OR_ATTACK*)AOEAllocZeroMemory(unitDef->attacksCount + 1, sizeof(STRUCT_ARMOR_OR_ATTACK));
		// Copy existing attack data
		size_t copyBytesSize = sizeof(STRUCT_ARMOR_OR_ATTACK) * unitDef->attacksCount;
		size_t actualBufferSize = sizeof(STRUCT_ARMOR_OR_ATTACK) * (unitDef->attacksCount + 1);
		memcpy_s(unitDef->ptrAttacksList, actualBufferSize, oldPtr, copyBytesSize);
		unitDef->ptrAttacksList[unitDef->attacksCount].amount = value;
		unitDef->ptrAttacksList[unitDef->attacksCount].classId = attackClass;
		unitDef->attacksCount++;
		AOEFree(oldPtr);
		return true;
	}


	// Set armor value for a specific armor class.
	// If addIfMissing is set and armor class does not exist in list, then it is created.
	bool SetArmorInList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS armorClass, int value, bool addIfMissing) {
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromAttackable());
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass() || !unitDef->DerivesFromAttackable()) { return false; }
		assert(unitDef->armorsCount >= 0);
		if (unitDef->armorsCount < 0) {
			unitDef->armorsCount = 0;
		}
		for (int i = 0; i < unitDef->armorsCount; i++) {
			if (unitDef->ptrArmorsList[i].classId == armorClass) {
				unitDef->ptrArmorsList[i].amount = value;
				return true;
			}
		}
		// Not found
		if (!addIfMissing) { return false; }
		return AddArmorToList(unitDef, armorClass, value);
	}

	// Set attack value for a specific attack class.
	// If addIfMissing is set and attack class does not exist in list, then it is created
	bool SetAttackInList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS attackClass, int value, bool addIfMissing) {
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromAttackable());
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass() || !unitDef->DerivesFromAttackable()) { return false; }
		assert(unitDef->attacksCount >= 0);
		if (unitDef->attacksCount < 0) {
			unitDef->attacksCount = 0;
		}
		for (int i = 0; i < unitDef->attacksCount; i++) {
			if (unitDef->ptrAttacksList[i].classId == attackClass) {
				unitDef->ptrAttacksList[i].amount = value;
				return true;
			}
		}
		// Not found
		if (!addIfMissing) { return false; }
		return AddAttackToList(unitDef, attackClass, value);
	}


	// Returns armor value for a specific armor class. Returns defaultValueIfMissing if armor class is not defined.
	short int GetArmorFromList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS armorClass, short int defaultValueIfMissing) {
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromAttackable());
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass() || !unitDef->DerivesFromAttackable()) { return false; }
		assert(unitDef->armorsCount >= 0);
		for (int i = 0; i < unitDef->armorsCount; i++) {
			if (unitDef->ptrArmorsList[i].classId == armorClass) {
				return unitDef->ptrArmorsList[i].amount;
			}
		}
		return defaultValueIfMissing; // not found
	}

	// Returns attack value for a specific attack class. Returns defaultValueIfMissing if attack class is not defined.
	short int GetAttackFromList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS attackClass, short int defaultValueIfMissing) {
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromAttackable());
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass() || !unitDef->DerivesFromAttackable()) { return false; }
		assert(unitDef->attacksCount >= 0);
		for (int i = 0; i < unitDef->attacksCount; i++) {
			if (unitDef->ptrAttacksList[i].classId == attackClass) {
				return unitDef->ptrAttacksList[i].amount;
			}
		}
		return defaultValueIfMissing; // not found
	}

}

