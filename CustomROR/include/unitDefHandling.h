#include <assert.h>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_def.h>
#include "AOE_memory.h"

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
}
