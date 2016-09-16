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




	// Create a new unitDef, copied for provided one, using actual derived class.
	// Returns a STRUCT_UNITDEF_BASE, but it can be any derived class (living, building, etc)
	// This is a shortcut to avoid writing a big "switch" everywhere... The correct class/size will be allocated/initialized according to source class to copy.
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *CopyUnitDefToNewUsingGoodClass(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *existingUnitDef) {
		if (!existingUnitDef || !existingUnitDef->IsCheckSumValidForAUnitClass()) {
			return NULL;
		}
		switch (existingUnitDef->unitType) {
		case GLOBAL_UNIT_TYPES::GUT_COMMANDABLE:
			return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE>((AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE*)existingUnitDef);
			break;
		case GLOBAL_UNIT_TYPES::GUT_BUILDING:
			return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING>((AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING*)existingUnitDef);
			break;
		case GLOBAL_UNIT_TYPES::GUT_MOVABLE:
			return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE>((AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE*)existingUnitDef);
			break;
		case GLOBAL_UNIT_TYPES::GUT_DOPPLEGANGER:
			return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_DOPPLEGANGER>((AOE_STRUCTURES::STRUCT_UNITDEF_DOPPLEGANGER*)existingUnitDef);
			break;
		case GLOBAL_UNIT_TYPES::GUT_EYE_CANDY:
			return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_BASE>((AOE_STRUCTURES::STRUCT_UNITDEF_BASE*)existingUnitDef);
			break;
		case GLOBAL_UNIT_TYPES::GUT_FLAGS:
			return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_FLAG>((AOE_STRUCTURES::STRUCT_UNITDEF_FLAG*)existingUnitDef);
			break;
		case GLOBAL_UNIT_TYPES::GUT_TRAINABLE:
			return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE>((AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE*)existingUnitDef);
			break;
		case GLOBAL_UNIT_TYPES::GUT_PROJECTILE:
			return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE>((AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE*)existingUnitDef);
			break;
		case GLOBAL_UNIT_TYPES::GUT_TREE:
			return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_TREE>((AOE_STRUCTURES::STRUCT_UNITDEF_TREE*)existingUnitDef);
			break;
		case GLOBAL_UNIT_TYPES::GUT_ATTACKABLE:
			return CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE>((AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE*)existingUnitDef);
			break;
		default:
			traceMessageHandler.WriteMessage("Unhandled unit definition class.");
			return NULL;
		}
	}


	// Extends a player's unitDef table to add a new one (unitDef).
	// unitDef's DAT_ID1 and DAT_ID2 are modified with new ID.
	// Returns -1 on failure, new unitDefId on success.
	short int AddUnitDefToPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
		if (!player || !player->IsCheckSumValid() || !unitDef || !unitDef->IsCheckSumValidForAUnitClass()) {
			return -1;
		}

		short int nbDef = player->structDefUnitArraySize + 1; // new number of unit definitions
		short int newDATID = player->structDefUnitArraySize;
		long int newSizeInBytes = nbDef * 4;
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE **oldArray = player->ptrStructDefUnitTable;
		// Run some checks
		assert(oldArray != NULL);
		if (oldArray == NULL) { return -1; }
		assert(0 <= newDATID);

		// Create new array
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE **newArray = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE **)AOEAlloc(newSizeInBytes);
		if (newArray == NULL) { return -1; } // nothing allocated: return an error

		// Copy old array into new (for all existing unitDefs => copy pointers)
		memcpy(newArray, oldArray, newSizeInBytes);
		AOEFree(oldArray); // old array is freed, we replace it by new (larger) one
		player->ptrStructDefUnitTable = newArray;
		player->structDefUnitArraySize = nbDef;
		player->ptrStructDefUnitTable[newDATID] = unitDef;
		unitDef->DAT_ID1 = newDATID;
		unitDef->DAT_ID2 = newDATID;
		return newDATID;
	}


	// Modifies walking graphics "angle count" to allow units to move correctly.
	// Returns true if successful. If modified, this impacts all units using the graphics, and is not reversible.
	// Restricted to unit definitions that do NOT use attack sounds.
	// You may also want (need) to modify unit definition's speed, terrain restriction, interactionMode...
	bool ForceAllowMovementForUnitWalkingGraphics(AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE *unitDef) {
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass() || !unitDef->DerivesFromMovable()) {
			return false;
		}
		if (!unitDef->ptrWalkingGraphic1) {
			return false;
		}
		if (unitDef->ptrWalkingGraphic1->angleCount >= 8) {
			return true; // Did nothing, but should be (already) compatible with movement
		}
		if (!unitDef->ptrWalkingGraphic1->attackSoundUsed) {
			assert(unitDef->ptrWalkingGraphic1->attackSoundsArray == NULL);
			unitDef->ptrWalkingGraphic1->angleCount = 8;
			return true;
		}
		/*
		// This part MODIFIES the graphics, is not reversible and impacts everything using the graphics
		int initialCount = unitDef->ptrWalkingGraphic1->angleCount;
		unitDef->ptrWalkingGraphic1->angleCount = 8;
		if (unitDef->ptrWalkingGraphic1->attackSoundUsed && unitDef->ptrWalkingGraphic1->attackSoundsArray) {
		STRUCT_GRAPHIC_ATTACK_SOUND_INFO *newArray = (STRUCT_GRAPHIC_ATTACK_SOUND_INFO*)AOEAllocZeroMemory(8, sizeof(STRUCT_GRAPHIC_ATTACK_SOUND_INFO));
		memcpy_s(newArray, sizeof(STRUCT_GRAPHIC_ATTACK_SOUND_INFO) * initialCount, unitDef->ptrWalkingGraphic1->attackSoundsArray, sizeof(STRUCT_GRAPHIC_ATTACK_SOUND_INFO) * initialCount);

		for (int i = initialCount + 1; i <= 8; i++) {
		for (int j = 0; j < 3; j++) {
		unitDef->ptrWalkingGraphic1->attackSoundsArray[i].pSound[j]->filesCount = 0;;
		unitDef->ptrWalkingGraphic1->attackSoundsArray[i].pSound[j]->fileInfoArray = NULL;
		unitDef->ptrWalkingGraphic1->attackSoundsArray[i].pSound[j]->soundId = -1;
		unitDef->ptrWalkingGraphic1->attackSoundsArray[i].soundDelay[j] = 0;
		}
		}
		AOEFree(unitDef->ptrWalkingGraphic1->attackSoundsArray);
		unitDef->ptrWalkingGraphic1->attackSoundsArray = newArray;
		}
		return true;
		*/
		return false;
	}

	// Make a building capable of moving.
	// This may affect other buildings that use the same walking graphic (including other players !) ; however other player's unit won't move till you change their speed.
	// Returns true if successful
	bool AllowMovementForBuilding(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef, float speed) {
		if (!unitDef || !unitDef->IsCheckSumValid()) { return false; }
		if (!ForceAllowMovementForUnitWalkingGraphics(unitDef)) {
			return false;
		}
		unitDef->speed = speed;
		unitDef->rotationSpeed = 0;
		unitDef->terrainRestriction = TERRAIN_RESTRICTION::CST_TR_LAND_LIVING_UNITS;
		unitDef->interactionMode = INTERACTION_MODES::CST_IM_LIVING_UNITS;
		return true;
	}


	short int GetDAT_ID2(AOE_STRUCTURES::STRUCT_UNITDEF_BASE **defUnitTable, short int DAT_ID1) {
		assert(DAT_ID1 >= 0);
		if (DAT_ID1 < 0) { return DAT_ID1; }
		return defUnitTable[DAT_ID1]->DAT_ID2;
	}


	// Returns true if unit definition is a tower (using unit type and the fact it has attacks or not)
	// See also IsTower(datid) in AOE_empires_dat.h, which uses a hardcoded list.
	bool IsTower(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
		if (!unitDef->DerivesFromAttackable()) {
			return false;
		}
		AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDef50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDef;
		if (!unitDef50->IsCheckSumValidForAUnitClass()) {
			return false;
		}
		// A tower is generally a building with attack, BUT be careful, some towers are defined as living units with speed=0
		// We try to use the most generic criteria: speed=0 & attack>0.
		return (unitDef50->speed == 0) && (unitDef50->attacksCount > 0);
	}


	// Get strategy element type for a unit
	// Returns AIUCNone if result could not be determined.
	AOE_CONST_FUNC::TAIUnitClass GetUnitStrategyElemClass(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
		TAIUnitClass unitStrategyClass = AOE_CONST_FUNC::TAIUnitClass::AIUCNone;
		if (!unitDef) { return unitStrategyClass; }
		if (unitDef->unitType == GUT_BUILDING) { unitStrategyClass = AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding; }
		if (unitDef->unitType == GUT_TRAINABLE) { unitStrategyClass = AOE_CONST_FUNC::TAIUnitClass::AIUCLivingUnit; }
		return unitStrategyClass;
	}

}

