#include "../include/AOEPrimitives_units.h"


namespace AOE_METHODS {


namespace UNIT {
;

/* *** some basic getters *** */

// All unit-classes-compatible getter for (maximum) speed cf unit definition. NOT current speed !
float GetSpeed(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return 0; }
	float result;
	_asm {
		MOV ECX, unit;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0xFC]; // returns 0 for classes with no speed attribute
		FSTP DS:[result]; // REQUIRED to compensate the FLD from called method (for float stack consistency)
	}
	return result;
}


// All-unit classes compatible getter for reload time.
float GetReloadTime1(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return 0; }
	float result;
	_asm {
		MOV ECX, unit;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x100]; // returns 0 for classes with no reload attribute
		FSTP DS:[result]; // REQUIRED to compensate the FLD from called method (for float stack consistency)
	}
	return result;
}


// Get Melee armor (for display) for attackable units.
// Returns NULL if unit type is incompatible (and sets values to 0).
bool GetMeleeArmor(STRUCT_UNIT_ATTACKABLE *unit, short int &meleeDisplayedValue, short int &meleeTotalValue) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->DerivesFromAttackable()) {
		meleeDisplayedValue = 0;
		meleeTotalValue = 0;
		return false;
	}
	_asm {
		PUSH meleeDisplayedValue;
		PUSH meleeTotalValue;
		MOV ECX, unit;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x230]; // Get melee armor
	}
	return true;
}

// Get Pierce armor (for display) for trainable units.
// Returns NULL if unit type is incompatible (and sets values to 0).
bool GetPierceArmor(STRUCT_UNIT_TRAINABLE *unit, short int &pierceDisplayedValue, short int &pierceTotalValue) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->DerivesFromTrainable()) {
		pierceDisplayedValue = 0;
		pierceTotalValue = 0;
		return false;
	}
	_asm {
		PUSH pierceDisplayedValue;
		PUSH pierceTotalValue;
		MOV ECX, unit;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x248]; // Get melee armor (for types 70+ only)
	}
	return true;
}


// Securely gets maximum range (only applicable to attackable (type50))
// This is analog to "EDX+0x10C" call on Unit.
float GetMaxRange(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->DerivesFromAttackable()) {
		return 0.f;
	}
	return ((STRUCT_UNITDEF_ATTACKABLE*)unit->unitDefinition)->maxRange;
}

// Securely gets attack amount (only applicable to attackable (type50))
// This is analog to "EDX+0x108" call on Unit (but "EDX+0x108" call returns a float in ST, even if attack data are INTs)
long int GetAttacksAmount(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->DerivesFromAttackable()) {
		return 0;
	}
	STRUCT_UNITDEF_ATTACKABLE *unitDef = (STRUCT_UNITDEF_ATTACKABLE*)unit->unitDefinition;
	int atkCount = unitDef->attacksCount;
	long int result = 0;
	for (int i = 0; i < atkCount; i++) {
		short int thisAmount = unitDef->ptrAttacksList[i].amount;
		if (thisAmount > 0) {
			result += thisAmount;
		}
	}
	return result;
}

// Returns if unit is ready to attack, regarding reload time and last attack execution.
bool IsReadyToAttack(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->DerivesFromAttackable()) {
		return false;
	}
	return ((STRUCT_UNIT_ATTACKABLE*)unit)->pendingReloadTime <= 0; // cf 0x4269F6 (unit+0x7C call)
}



/* *** Other... *** */



// Exact role to confirm.
// MAYBE this method allows finding path with enemy units blocking the way. Such units are added to path finding struct's unitid array (unknown_11DCE4) ?
// arg6: seen -1 or 0x1B (hardcoded). Could be a AI unit class (walls) ?
// tempList should be empty in input, filled by this method. Please free the array (if non-NULL) afterwards !
// Call [unit+0x1A0], for example 0x44E130.
bool MoveToTarget_1A0(STRUCT_UNIT_BASE *unit, long int targetUnitId, float range, long int arg3,
	long int arg4, long int targetPlayerId, long int arg6, STRUCT_MANAGED_ARRAY *tempList) {
	assert(unit && unit->IsCheckSumValidForAUnitClass());
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	long int callResult;
	_asm {
		MOV ECX, unit;
		PUSH tempList;
		PUSH arg6;
		PUSH targetPlayerId;
		PUSH arg4;
		PUSH arg3;
		PUSH range;
		PUSH unit;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX+0x1A0];
		MOV callResult, EAX;
	}
	return (callResult != 0);
}


// Add a unit to all relevant infAI lists. Cf 0x4BE0D0
bool AddUnitInInfAILists(STRUCT_INF_AI *infAI, long int unitId) {
	assert(infAI && infAI->IsCheckSumValid());
	if (!infAI || !infAI->IsCheckSumValid()) { return false; }
	unsigned long int callAddr = 0x4BE0D0;
	_asm {
		MOV ECX, infAI;
		PUSH unitId;
		CALL callAddr;
	}
	return true;
}


// Returns true if a unit can convert another (actually, for priests)
bool CanConvert(STRUCT_UNIT_BASE *unit, long int targetUnitId) {
	assert(unit && unit->IsCheckSumValidForAUnitClass());
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || (targetUnitId < 0)) {
		return false;
	}
	STRUCT_UNIT_ACTIVITY *activity = unit->currentActivity;
	if (!activity || !activity->IsCheckSumValid()) { return false; }
	long int result;
	_asm {
		MOV ECX, activity;
		PUSH targetUnitId;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0xC4];
		MOV result, EAX;
	}
	return (result != 0);
}



// Returns true if unit can attack target, taking into account faith for priests, etc.
bool CanAttackTarget(AOE_STRUCTURES::STRUCT_UNIT_BASE *actor, AOE_STRUCTURES::STRUCT_UNIT_BASE *target) {
	if (!actor || !target || !actor->IsCheckSumValidForAUnitClass() || !target->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	STRUCT_UNIT_ACTIVITY *activity = actor->currentActivity;
	if (!activity || !activity->IsCheckSumValid()) { return false; }
	long int result;
	_asm {
		MOV ECX, activity;
		PUSH target; // arg1 is unit pointer, NOT unit ID.
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x50];
		MOV result, EAX;
	}
	return (result != 0);
}


// Gets the damage from an attacked over a target (defender)
float CalcDamage(STRUCT_UNIT_BASE *attacker, STRUCT_UNIT_BASE *defender) {
	if (!attacker || !attacker->IsCheckSumValidForAUnitClass()) { return 0; }
	if (!defender || !defender->IsCheckSumValidForAUnitClass()) { return 0; }
	float result;
	_asm {
		MOV ECX, defender;
		PUSH attacker;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x104]; // returns 0 for classes with no attack
		FSTP DS:[result]; // REQUIRED to compensate the FLD from called method (for float stack consistency)
	}
	return result;
}



// Returns true if a unit is idle
bool IsUnitIdle(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	int result;
	_asm {
		MOV ECX, unit;
		MOV EDX, DS:[ECX]; //0x00406610
		CALL DS:[EDX + 0x210];
		MOV result, EAX;
	}
	return (result != 0);
}


// Make progress in building construction according to timeToAdd (in seconds)
void BuildingUnitDoConstruct(STRUCT_UNIT_BUILDING *building, float timeToAdd_seconds) {
	if (!building || !building->IsCheckSumValid()) {
		return;
	}
	_asm {
		PUSH timeToAdd_seconds;
		MOV ECX, building;
		MOV EAX, 0x4AD7B0; // For ROR 1.0c
		CALL EAX;
	}
}


// Create unit activity using derived class that corresponds to supplied checksum (calls the correct constructor)
// Any unit (even base type) can have an activity, even if ROR originally only does that for living&building.
// Returns true if successful.
bool CreateUnitActivity(STRUCT_UNIT_BASE *unit, unsigned long int activityClassChecksum) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !activityClassChecksum) { return false; }
	if (!isAValidRORChecksum(activityClassChecksum)) { return false; }
	unsigned long int ccorAddr = STRUCT_UNIT_ACTIVITY::ConstructorAddress(activityClassChecksum);
	if (!ccorAddr) { return false; }
	STRUCT_UNIT_ACTIVITY *activity = (STRUCT_UNIT_ACTIVITY*)AOEAllocZeroMemory(1, sizeof(STRUCT_UNIT_ACTIVITY));
	if (!activity) { return false; }
	STRUCT_UNIT_ACTIVITY *result_check = NULL;
	long int arg2 = 0x0A; // Note: arg2=0x0A in all cases but artefacts(5)
	if (unit->unitDefinition && (unit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact)) {
		arg2 = 5;
	}
	_asm {
		MOV ECX, activity;
		PUSH arg2;
		PUSH unit;
		CALL ccorAddr;
		MOV result_check, EAX;
	}
	assert(result_check == activity);
	unit->currentActivity = activity;
	return true;
}


// Add visibility for given unit to specified player (might NOT be unit owner)
// If useUnitSizeRadius, do not use distance but unit.unitDef.sizeRadius instead.
bool AddVisibility(STRUCT_UNIT_BASE *unit, STRUCT_PLAYER *playerToUpdate, bool useUnitSizeRadius, long int distance) {
	if (!unit || !playerToUpdate || !unit->IsCheckSumValidForAUnitClass() || !playerToUpdate->IsCheckSumValid() || !unit->unitDefinition || !unit->unitDefinition->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	long int arg2 = useUnitSizeRadius ? 1 : 0;
	_asm {
		MOV ECX, unit;
		PUSH distance;
		PUSH arg2;
		PUSH playerToUpdate;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX+0xD8];
	}
	return true;
}


// Remove visibility for given unit from specified player (might NOT be unit owner)
// If useUnitSizeRadius, do not use distance but unit.unitDef.sizeRadius instead.
bool RemoveVisibility(STRUCT_UNIT_BASE *unit, STRUCT_PLAYER *playerToUpdate, bool useUnitSizeRadius, long int distance) {
	if (!unit || !playerToUpdate || !unit->IsCheckSumValidForAUnitClass() || !playerToUpdate->IsCheckSumValid() || !unit->unitDefinition || !unit->unitDefinition->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	long int arg2 = useUnitSizeRadius ? 1 : 0;
	_asm {
		MOV ECX, unit;
		PUSH distance;
		PUSH arg2;
		PUSH playerToUpdate;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX+0xDC];
	}
	return true;
}


// Calls AOE's method to change a unit owner. Warning, this has bugs, see crCommand.OnUnitChangeOwner_fixes to call missing treatments.
void ChangeUnitOwner(AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer) {
	if (!targetUnit || !actorPlayer || !targetUnit->IsCheckSumValidForAUnitClass() || !actorPlayer->IsCheckSumValid()) {
		return;
	}
	_asm {
		MOV ECX, targetUnit
			MOV EAX, actorPlayer
			MOV EDX, DS:[ECX]
			PUSH EAX
			CALL DS : [EDX + 0x44] // unit.convertToPlayer(ptrPlayer)
	}
}


// Creates a unit at provided location. Does NOT make checks on location, please first make sure GetErrorForUnitCreationAtLocation returns 0.
// Please use CheckAndCreateUnit instead (unless you explicitely do NOT want to check target location)
// You can use 0 as posZ value.
// Returns NULL if it failed
AOE_STRUCTURES::STRUCT_UNIT_BASE *CreateUnitNoMapCheck(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID, float posY, float posX, float posZ) {
	if (!player || !player->IsCheckSumValid()) {
		return NULL;
	}
	long int res;
	_asm {
		MOV ECX, player
			PUSH 0 // arg5=unused ?
			PUSH posZ
			PUSH posX
			PUSH posY
			PUSH DAT_ID
			MOV EAX, 0x4F0B50 // does not check if there is free space to accept the new unit nor terrain restriction... (check it first)
			CALL EAX
			MOV res, EAX
	}
	return (AOE_STRUCTURES::STRUCT_UNIT_BASE *) res;
}


// Creates a unit at provided location. Does NOT make checks on location, please first make sure GetErrorForUnitCreationAtLocation returns 0.
// Please use CheckAndCreateUnit instead (unless you explicitely do NOT want to check target location)
// You can use 0 as posZ value.
// Warning: orientation IS unit.orientation for types 30-80. But for types 10/20/90, is corresponds to orientationIndex as float.
// Returns NULL if it failed
// Compatible with both in-game and editor screens. This overload corresponds to deserialize (and create) unit operation.
AOE_STRUCTURES::STRUCT_UNIT_BASE *CreateUnitNoMapCheck(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID, float posY, float posX, float posZ,
	long int status, float orientation) {
	if (!player || !player->IsCheckSumValid()) {
		return NULL;
	}
	long int res;
	_asm {
		PUSH orientation;
		PUSH status;
		PUSH DAT_ID;
		PUSH posZ;
		PUSH posX;
		PUSH posY;
		MOV ECX, player;
		MOV EAX, DS:[ECX];
		CALL DS:[EAX + 0x60];
		MOV res, EAX;
	}
	return (AOE_STRUCTURES::STRUCT_UNIT_BASE *) res;
}


// Creates a unit at provided location only if GetErrorForUnitCreationAtLocation agrees !
// Returns NULL if it failed
AOE_STRUCTURES::STRUCT_UNIT_BASE *CheckAndCreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef,
	float posX, float posY, bool checkVisibility, bool checkHills, bool checkConflictingUnits) {
	if (AOE_METHODS::GetErrorForUnitCreationAtLocation(player, unitDef, posY, posX, checkVisibility, checkHills, false, true, checkConflictingUnits) != 0) {
		return NULL;
	}
	return CreateUnitNoMapCheck(player, unitDef->DAT_ID1, posY, posX, 0);
}


// Will has the same effect as "CTRL-0" or "CTRL-1" etc: assigns a shortcut number to units (and removes this shortcut from old units that had it)
// Do not use numbers > 10 (0x0A), they are used for groups. You can use negative values.
// shortcutNumber must be an "internal" shortcut number ([-11;-2], 0 or [1-10] for shortcuts). This is not checked here.
// shortcutNumber>10 values correspond to groups.
// Other numbers than 1-9 will never be displayed as SLP file contains only 1-9 images (unless using custom DRS/SLPs)
// Returns 1 on success.
long int AssignShortcutToSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player, long int shortcutNumber) {
	if (!player) { return 0; }
	// shortcutNumber is a char, actually.
	if ((shortcutNumber < -128) || (shortcutNumber > 127)) { return 0; }
	char c_shortcut = (char)shortcutNumber;
	long int res = 0;
	//GetPlayerStruct(playerId);
	_asm {
		MOV ECX, player;
		MOVSX EAX, c_shortcut;
		PUSH EAX;
		MOV EDX, 0x0045DE00; // player.assignShortcut(shortcutNumber)
		CALL EDX;
		MOV res, EAX;
	}
	return res;
}


// Selects units that have a given shortcut number.
// shortcutNumber must be an "internal" shortcut number ([-11;-2], 0 or [1-10] for shortcuts). This is not checked here.
// shortcutNumber>10 values correspond to groups.
// Returns 1 on success.
long int SelectUnitsUsingShortcut(AOE_STRUCTURES::STRUCT_PLAYER *player, long int shortcutNumber, bool addToSelection) {
	if (!player) { return 0; }
	// shortcutNumber is a char, actually.
	//if ((shortcutNumber < 0) || (shortcutNumber > 0xFF)) { return 0; }
	if ((shortcutNumber < -128) || (shortcutNumber > 127)) { return 0; }
	char c_shortcut = (char)shortcutNumber;
	long int res = 0;
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	if (!addToSelection) { // Clear selection first
		_asm {
			MOV ECX, player;
			MOV EDX, 0x0045DCB0; // player.ClearSelectedUnits
			CALL EDX;
		}
	}
	_asm {
		MOV ECX, player;
		MOVSX EAX, c_shortcut;
		PUSH EAX;
		MOV EDX, 0x0045DF40; // player.selectUnitsForShortcut(shortcutNumber)
		CALL EDX;
		MOV res, EAX;
	}
	return res;
}


// Update unit display
void UpdateDisplay(STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return; }
	_asm {
		MOV ECX, unit;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX+0x24]; // update
	}
}


// Get distance between 2 units, taking into account unit radiuses
float GetContactDistanceTo(STRUCT_UNIT_BASE *actor, STRUCT_UNIT_BASE *target) {
	if (!actor || !target || !actor->IsCheckSumValidForAUnitClass() || !target->IsCheckSumValidForAUnitClass()) {
		return 0.f;
	}
	unsigned long int addr = 0x4AAEB0;
	float result = 0;
	_asm {
		MOV ECX, actor;
		PUSH target;
		CALL addr;
		FSTP DS:[result]; // REQUIRED to compensate the FLD from called method (for float stack consistency)
	}
	return result;
}


// Returns the number of queued units for a given DATID.
long int GetTotalQueueNumberForUnit(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *bld, short int unitDefId) {
	if (!bld || !bld->IsTypeValid() || !bld->ptrHumanTrainQueueInformation) { return 0; }
	if (unitDefId < 0) { return 0; }
	long int result = 0;
	for (int index = 0; index < bld->trainUnitQueueCurrentElemCount; index++) {
		if ((bld->ptrHumanTrainQueueInformation[index].DATID == unitDefId) && (bld->ptrHumanTrainQueueInformation[index].unitCount >= 0)) {
			result += bld->ptrHumanTrainQueueInformation[index].unitCount;
		}
	}
	return result;
}


// Returns a unitDefCommand object if actor unit has a valid right-click command on target unit.
// Returns NULL if there no possible interaction
AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF *GetUnitDefCommandForTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actorUnit,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *target, bool canSwitchForVillager) {
	if (!actorUnit || !target || !actorUnit->IsCheckSumValidForAUnitClass() || !actorUnit->DerivesFromCommandable() ||
		!target->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef_base = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)actorUnit->unitDefinition;
	if (!unitDef_base->DerivesFromCommandable()) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *unitDef_asBird = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)unitDef_base;
	if (!unitDef_asBird->ptrUnitCommandHeader || !unitDef_asBird->ptrUnitCommandHeader->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF_HEADER *cmdh = unitDef_asBird->ptrUnitCommandHeader;
	AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF *result;
	assert(cmdh != NULL);
	if (!cmdh) {
		return NULL;
	}
	_asm {
		MOV ECX, cmdh;
		MOV EAX, DS:[ECX];
		PUSH 0;
		PUSH 0;
		PUSH 0;
		PUSH target;
		PUSH actorUnit;
		CALL DS:[EAX + 4];
		MOV result, EAX;
	}
	if ((result != NULL) || !canSwitchForVillager) {
		return result; // Not villager mode: return what we just found.
	}

	// Here: not found but we can try other villager types if unit is a villager
	char switchingId = unitDef_asBird->unitDefinitionSwitchGroupId;
	if (!unitDef_asBird->unitDefinitionSwitchGroupId) {
		return NULL;
	}
	// Try other villager types
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = actorUnit->ptrStructPlayer;
	if (!actorPlayer || !actorPlayer->IsCheckSumValid()) {
		return NULL;
	}
	for (int i = 0; i < actorPlayer->structDefUnitArraySize; i++) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *tmpUnitDefBase = actorPlayer->GetUnitDefBase((short int)i);
		if (tmpUnitDefBase && tmpUnitDefBase->IsCheckSumValidForAUnitClass() && tmpUnitDefBase->DerivesFromCommandable() &&
			(((STRUCT_UNITDEF_COMMANDABLE*)tmpUnitDefBase)->unitDefinitionSwitchGroupId == switchingId)) {
			if (tmpUnitDefBase && tmpUnitDefBase->IsCheckSumValidForAUnitClass() && tmpUnitDefBase->DerivesFromCommandable()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *tmpUnitDefBird = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)tmpUnitDefBase;
				cmdh = tmpUnitDefBird->ptrUnitCommandHeader;
				if (cmdh) {
					assert(cmdh->IsCheckSumValid());
					if (cmdh->IsCheckSumValid()) {
						// Try other villager type's command (with current "actorUnit")
						_asm {
							MOV ECX, cmdh;
							MOV EAX, DS:[ECX];
							PUSH 0;
							PUSH 0;
							PUSH 0;
							PUSH target;
							PUSH actorUnit;
							CALL DS:[EAX + 4];
							MOV result, EAX;
						}
					} else {
						std::string msg = "Bad commandHeader at ";
						msg += GetHexStringAddress((long)cmdh);
						traceMessageHandler.WriteMessage(msg);
						result = NULL;
					}
				}
				if (result) {
					return result; // If successful, return the command found. Otherwise, continue.
				}
			}
		}
	}
	return NULL;
	// Old hardcoded villager list:static const long int villagerDATIDs[] = { 83, 118, 119, 120, 122, 123, 124, 127, 156, 251, 259 }; +293(upgraded villager)
}


}
}

