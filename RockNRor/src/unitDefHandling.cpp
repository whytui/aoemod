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


	// Add armor (using provided value, default 0) to armor list if it does not already exists.
	// This does nothing if the unitDef already has an armor for this attack class.
	// Return true if the armor was added.
	bool AddArmorIfMissing(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS armorClass, int value) {
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromAttackable());
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass() || !unitDef->DerivesFromAttackable()) { return false; }
		assert(unitDef->armorsCount >= 0);
		if (unitDef->armorsCount < 0) {
			unitDef->armorsCount = 0;
		}
		for (int i = 0; i < unitDef->armorsCount; i++) {
			if (unitDef->ptrArmorsList[i].classId == armorClass) {
				return false;
			}
		}
		// Not found: add it
		return AddArmorToList(unitDef, armorClass, value);
	}

	// Add attack (using provided value, default 0) to attack list if it does not already exists.
	// This does nothing if the unitDef already has an attack for this attack class.
	// Return true if the attack was added.
	bool AddAttackIfMissing(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS attackClass, int value) {
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromAttackable());
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass() || !unitDef->DerivesFromAttackable()) { return false; }
		assert(unitDef->attacksCount >= 0);
		if (unitDef->attacksCount < 0) {
			unitDef->attacksCount = 0;
		}
		for (int i = 0; i < unitDef->attacksCount; i++) {
			if (unitDef->ptrAttacksList[i].classId == attackClass) {
				return false;
			}
		}
		// Not found: add it
		return AddArmorToList(unitDef, attackClass, value);
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
	// A tower is a unit that can attack (including priests) and can't move.
	// See also IsTower(datid) in AOE_empires_dat.h, which uses a hardcoded list.
	bool IsTower(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
		if (!unitDef || !unitDef->DerivesFromAttackable()) {
			return false;
		}
		AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDef50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDef;
		if (!unitDef50->IsCheckSumValidForAUnitClass()) {
			return false;
		}
		// A tower is generally a building with attack, BUT be careful, some towers are defined as living units with speed=0
		// We try to use the most generic criteria: speed=0 & attack>0. Note that speed attributes comes from flag (T20) so it always exists for here (Type50).
		bool canAttack = (unitDef50->attacksCount > 0) || (unitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest);
		return (unitDef50->speed == 0) && canAttack;
	}


	// Returns true if a unit is capable of attacking (conversion or physical attack)
	// WARNING: this does not take care of villager mode (for example, will return false for a farmer !)
	bool UnitDefCanAttack(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
		// Non-commandable (type 40) units can't attack
		if (!unitDef || !unitDef->DerivesFromCommandable()) {
			return false;
		}
		AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *unitDefCommandable = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)unitDef;
		// Special: if it can convert, return true
		if (unitDefCommandable->ptrUnitCommandHeader && unitDefCommandable->ptrUnitCommandHeader->IsCheckSumValid()) {
			for (int i = 0; i < unitDefCommandable->ptrUnitCommandHeader->commandCount; i++) {
				if (unitDefCommandable->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_CONVERT) {
					return true;
				}
			}
		}
		// Now handle physical attacks: non-type50 units can't attack physically
		AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDef50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDef;
		if (!unitDef50->IsCheckSumValidForAUnitClass()) {
			return false;
		}
		// Unit can attack <=> it has at least 1 attack specified
		// Note: will return false for archimedes (although there is a bug in AI that allows AI to have him attack with something like 0.00001 attack !)
		return (unitDef50->attacksCount > 0);
	}

	// Returns true if a unit is capable of attacking (conversion or physical attack)
	// Returns "returnValueForCivilian" if unitdef class is 4 (civilian)
	// WARNING: this does not take care of villager mode (for example, will return false for a farmer !)
	bool UnitDefCanAttack(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef, bool returnValueForCivilian) {
		// Non-commandable (type 40) units can't attack
		if (!unitDef || !unitDef->DerivesFromCommandable()) {
			return false;
		}
		if (unitDef->unitAIType == TribeAIGroupCivilian) {
			return returnValueForCivilian;
		}
		return UnitDefCanAttack(unitDef);
	}

	// Returns 0 for classes that do NOT have a speed !
	float GetUnitDefSpeed(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef) {
		// Non-flag (type 20) units can't move and have no speed
		if (!unitDef || !unitDef->DerivesFromFlag()) {
			return 0.f;
		}
		AOE_STRUCTURES::STRUCT_UNITDEF_FLAG *unitDefFlag = (AOE_STRUCTURES::STRUCT_UNITDEF_FLAG *)unitDef;
		return unitDefFlag->speed;
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


	// Returns a weight for a military unit. Scale is totally subjective ! Just a tool for algorithms...
	// Super units have a high weight.
	// Weak units have a low weight
	int GetUnitWeight(short int DAT_ID) {
		switch (DAT_ID) {
			// Super units
		case CST_UNITID_ARMORED_ELEPHANT:
		case CST_UNITID_CATAPHRACT:
		case CST_UNITID_CENTURION:
		case CST_UNITID_HEAVY_CATAPULT:
		case CST_UNITID_HEAVY_CHARIOT:
		case CST_UNITID_HEAVY_HORSE_ARCHER:
		case CST_UNITID_HELEPOLIS:
		case CST_UNITID_LEGION:
			return 1000;
			// Naval best units
		case CST_UNITID_JUGGERNAUGHT:
		case CST_UNITID_TRIREME:
			return 900;
			// Intermediate "almost-super" units
		case CST_UNITID_PHALANX:
		case CST_UNITID_CATAPULT:
		case CST_UNITID_HEAVY_CAVALRY:
			return 600;
		case CST_UNITID_PRIEST:
			return 600; // ? Depends on researches !
		case CST_UNITID_ELEPHANT_ARCHER:
			return 400;

		case CST_UNITID_HORSE_ARCHER:
			return 300;
		case CST_UNITID_LONG_SWORDSMAN:
		case CST_UNITID_BALLISTA:
		case CST_UNITID_HOPLITE:
		case CST_UNITID_STONE_THROWER:
		case CST_UNITID_WAR_ELEPHANT:
		case CST_UNITID_WAR_GALLEY:
		case CST_UNITID_CATAPULT_TRIREME:
			return 200;
		case CST_UNITID_CAVALRY:
		case CST_UNITID_CHARIOT:
			return 100;
			// Units under 100 do NOT have upgrades to super units except CST_UNITID_SHORT_SWORDSMAN and CST_UNITID_SCOUT_SHIP
			// (this is nothing but a choice from method writer ;)
		case CST_UNITID_COMPOSITE_BOWMAN:
			return 90;
		case CST_UNITID_CAMEL:
			return 50;

			// Weak units...
		case CST_UNITID_CHARIOT_ARCHER:
		case CST_UNITID_SCOUT_SHIP:
			return 25;
		case CST_UNITID_IMPROVED_BOWMAN:
		case CST_UNITID_BROAD_SWORDSMAN:
			return 10;
		case CST_UNITID_SHORT_SWORDSMAN:
			return 5;
		case CST_UNITID_SLINGER:
		case CST_UNITID_SCOUT:
		case CST_UNITID_BOWMAN:
			return 2;
		case CST_UNITID_CLUBMAN:
			return 1;
		default:
			return 0;
			break;
		}
	}


	// Returns true if unit class corresponds to one of
	// - Artefact/flag
	// - Gatherable unit (mine, tree, gazelle - but not other animals, bushes...)
	// - Units that can be created by players: buildings, living units
	// WARNING: this does not includes bugfixes, see EconomyAI method !
	bool IsClassArtefactOrGatherableOrCreatable(GLOBAL_UNIT_AI_TYPES unitClass) {
		/*AOE_STRUCTURES::STRUCT_INF_AI fakeInfAI; // unused, ROR method must be static (not using members)
		const unsigned long int addr = 0x4BE100;
		long int res;
		long int dword_unitClass = unitClass;
		_asm {
			LEA ECX, fakeInfAI;
			PUSH dword_unitClass;
			CALL addr;
			MOV res, EAX;
		}
		return (res != 0);*/
		// Cf 0x4BE100
		if (IsClassPlayerCreatable(unitClass)) {
			return true;
		}
		if (IsClassTargetableAsResource(unitClass)) {
			return true;
		}
		return IsArtefactOrFlag(unitClass);
	}


	// Returns true if unit class corresponds to units that can be created by players: buildings, living units
	bool IsClassPlayerCreatable(GLOBAL_UNIT_AI_TYPES unitClass) {
		/*AOE_STRUCTURES::STRUCT_INF_AI fakeInfAI; // unused, ROR method must be static (not using members)
		const unsigned long int addr = 0x4BE140;
		long int res;
		long int dword_unitClass = unitClass;
		_asm {
			LEA ECX, fakeInfAI;
			PUSH dword_unitClass;
			CALL addr;
			MOV res, EAX;
		}
		return (res != 0);*/
		switch (unitClass) {
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupFishingBoat:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupFootSoldier:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupHero:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupUnusedHealer:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupMountedSoldier:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupPhalanx:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupTradeBoat:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupTransportBoat:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupWall:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupWarBoat:
		case AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger:
			return true;
		default:
			return false;
		}
	}

	// Returns true if unit class is targetable as a resource from AI point of view: fish, gazelle, bush, trees, mines
	// WARNING: this does not includes bugfixes, see EconomyAI method !
	bool IsClassTargetableAsResource(GLOBAL_UNIT_AI_TYPES unitClass) {
		switch (unitClass) {
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupUnknownFish:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSeaFish:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupShoreFish:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupBerryBush:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPreyAnimal:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupStoneMine:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupGoldMine:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupTree:
			return true;
		default:
			return false;
		}
	}

	// Returns true if unit class is artefact or flag
	bool IsArtefactOrFlag(GLOBAL_UNIT_AI_TYPES unitClass) {
		// Cf 0x4BE200
		switch (unitClass) {
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupFlag:
			return true;
		default:
			return false;
		}
	}


	// Get a unit *internal* name from empires.dat data (read from civ 0)
	// Returns NULL if not found. This requires that empires.dat file has already been read to global structure.
	const char *GetUnitInternalName(short int unitDefId) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (!global || !global->IsCheckSumValid()) { return NULL; }
		if (global->civCount <= 0) { return NULL; }
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = global->civilizationDefinitions[0]->GetUnitDef(unitDefId);
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) { return NULL; }
		return unitDef->ptrUnitName;
	}


	// Get a unit name from empires.dat data (read from civ 0)
	// Returns "" if not found. This requires that empires.dat file has already been read to global structure.
	std::string GetUnitName(short int unitDefId) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (!global || !global->IsCheckSumValid()) { return ""; }
		if (global->civCount <= 0) { return ""; }
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = global->civilizationDefinitions[0]->GetUnitDef(unitDefId);
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) { 
			// Some units (like specialized villagers) do not exist in gaia civ
			if (global->civCount <= 1) { return ""; }
			unitDef = global->civilizationDefinitions[1]->GetUnitDef(unitDefId);
			if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) { return ""; }
		}
		std::string result = GetLanguageDllText(unitDef->languageDLLID_Name);
		if (result.empty()) {
			result = unitDef->ptrUnitName;
		}
		return result;
	}


	// Get a *possible* unitDefId ("DAT_ID") that corresponds to a civilian that gathers such type of resource
	// E.g. returns 120 (forager) for "berry storage" type (16). Note that 259 (farmer) would also match
	// Returns -1 if no type matches (non-default gatherable resource types)
	short int GetCivilianUnitDefIdForResourceType_hardcoded(RESOURCE_TYPES resourceType) {
		switch (resourceType) {
		case RESOURCE_TYPES::CST_RES_ORDER_FOOD:
		case RESOURCE_TYPES::CST_RES_ORDER_MEAT_STORAGE:
			return CST_UNITID_HUNTER;
		case RESOURCE_TYPES::CST_RES_ORDER_WOOD:
			return CST_UNITID_LUMBERJACK;
		case RESOURCE_TYPES::CST_RES_ORDER_STONE:
			return CST_UNITID_MINERSTONE;
		case RESOURCE_TYPES::CST_RES_ORDER_GOLD:
			return CST_UNITID_MINERGOLD;
		case RESOURCE_TYPES::CST_RES_ORDER_BERRY_STORAGE:
			return CST_UNITID_FORAGER;
		case RESOURCE_TYPES::CST_RES_ORDER_FISH_STORAGE:
			return CST_UNITID_FISHERMAN;
		}
		return -1;
	}

}

