#include <assert.h>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_def.h>
#include "AOE_memory.h"
#include "traceMessage.h"

#pragma once

/*
This file contains primitives to manipulate unit definition objects.
*/

namespace AOE_STRUCTURES {
	// This adds an armor class to unitdef's armors list. This does NOT do any check on potential duplicates !
	// [private] bool AddArmorToList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS armorClass, int value);

	// This adds an attack class to unitdef's attacks list. This does NOT do any check on potential duplicates !
	// [private] bool AddAttackToList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS attackClass, int value);

	// Set armor value for a specific armor class.
	// If addIfMissing is set and armor class does not exist in list, then it is created.
	bool SetArmorInList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS armorClass, int value, bool addIfMissing);

	// Set attack value for a specific attack class.
	// If addIfMissing is set and attack class does not exist in list, then it is created
	bool SetAttackInList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS attackClass, int value, bool addIfMissing);

	// Returns armor value for a specific armor class. Returns defaultValueIfMissing if armor class is not defined.
	short int GetArmorFromList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS armorClass, short int defaultValueIfMissing);

	// Returns attack value for a specific attack class. Returns defaultValueIfMissing if attack class is not defined.
	short int GetAttackFromList(STRUCT_UNITDEF_ATTACKABLE *unitDef, AOE_CONST_FUNC::ATTACK_CLASS attackClass, short int defaultValueIfMissing);


	// Create a new unitDef, copied for provided one, using actual derived class.
	// Returns a STRUCT_UNITDEF_BASE, but it can be any derived class (living, building, etc)
	// This is a shortcut to avoid writing a big "switch" everywhere... The correct class/size will be allocated/initialized according to source class to copy.
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *CopyUnitDefToNewUsingGoodClass(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *existingUnitDef);

	// Extends a player's unitDef table to add a new one (unitDef).
	// unitDef's DAT_ID1 and DAT_ID2 are modified with new ID.
	// Returns -1 on failure, new unitDefId on success.
	short int AddUnitDefToPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef);

	// A constructor for unitDef that copies an existing one.
	template<typename UnitDef> static UnitDef *CopyUnitDefToNew(UnitDef *existingUnitDef) {
		if (!existingUnitDef || !existingUnitDef->IsCheckSumValid()) {
			return NULL;
		}
		UnitDef *newUnitDef = (UnitDef *)AOEAlloc(sizeof(UnitDef));
		if (!newUnitDef) {
			return newUnitDef;
		}
		unsigned long int addr = existingUnitDef->GetCopyConstructorAddress();
		if (!addr) {
			return NULL;
		}
		_asm {
			MOV ECX, newUnitDef;
			PUSH 1; // 1 means this is "initial" call (not inherited call from child class)
			PUSH existingUnitDef;
			CALL addr
		}
		return newUnitDef;
	}


	// Modifies walking graphics "angle count" to allow units to move correctly.
	// Returns true if successful. If modified, this impacts all units using the graphics, and is not reversible.
	// Restricted to unit definitions that do NOT use attack sounds.
	// You may also want (need) to modify unit definition's speed, terrain restriction, interactionMode...
	bool ForceAllowMovementForUnitWalkingGraphics(AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE *unitDef);

	// Make a building capable of moving.
	// This may affect other buildings that use the same walking graphic (including other players !) ; however other player's unit won't move till you change their speed.
	// Returns true if successful
	bool AllowMovementForBuilding(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef, float speed);


	// Returns a unit DAT_ID2 (including upgrades) given "base" unit ID (DAT_ID1)
	// You have to make sure DAT_ID1 is not out of bounds. It depends on empires.dat so it can't be checked with a hardcoded value.
	short int GetDAT_ID2(AOE_STRUCTURES::STRUCT_UNITDEF_BASE **defUnitTable, short int DAT_ID1);


	// Returns true if unit definition is a tower (using unit type and the fact it has attacks or not)
	// See also IsTower(datid) in AOE_empires_dat.h, which uses a hardcoded list.
	bool IsTower(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef);


	// Get strategy element type for a unit
	AOE_CONST_FUNC::TAIUnitClass GetUnitStrategyElemClass(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef);

}
