
#pragma once

#include <assert.h>
#include <string>
#include <AOE_struct_managed_array.h>
#include <AOE_struct_units.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_tac_ai.h>
#include <AOE_struct_inf_ai.h>
#include <AOE_struct_game_settings.h>
#include "AOE_memory.h"
#include "mainStructuresHandling.h"
#include "AOE_map.h"

/* This file contains AOE primitives about units handling */

using namespace AOE_STRUCTURES;

namespace AOE_METHODS {
namespace UNIT {
;

/* *** some basic getters *** */

// All unit-classes-compatible getter for (maximum) speed cf unit definition. NOT current speed !
float GetSpeed(STRUCT_UNIT_BASE *unit);

// All unit-classes-compatible getter for reload time.
float GetReloadTime1(STRUCT_UNIT_BASE *unit);

// Get Melee armor (for display) for attackable units.
// Returns NULL if unit type is incompatible (and sets values to 0).
bool GetMeleeArmor(STRUCT_UNIT_ATTACKABLE *unit, short int &meleeDisplayedValue, short int &meleeTotalValue);

// Get Pierce armor (for display) for trainable units.
// Returns NULL if unit type is incompatible (and sets values to 0).
bool GetPierceArmor(STRUCT_UNIT_TRAINABLE *unit, short int &pierceDisplayedValue, short int &pierceTotalValue);

// Securely gets maximum range (only applicable to attackable (type50))
// This is analog to "EDX+0x10C" call on Unit.
float GetMaxRange(STRUCT_UNIT_BASE *unit);

// Securely gets attack amount (only applicable to attackable (type50))
// This is analog to "EDX+0x108" call on Unit (but "EDX+0x108" call returns a float in ST, even if attack data are INTs)
long int GetAttacksAmount(STRUCT_UNIT_BASE *unit);

// Returns if unit is ready to attack, regarding reload time and last attack execution.
bool IsReadyToAttack(STRUCT_UNIT_BASE *unit);


/* *** Other... *** */


// Update unit display
void UpdateDisplay(STRUCT_UNIT_BASE *unit);


// Get distance between 2 units, taking into account unit radiuses
float GetContactDistanceTo(STRUCT_UNIT_BASE *actor, STRUCT_UNIT_BASE *target);


// Returns the number of queued units for a given DATID.
long int GetTotalQueueNumberForUnit(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *bld, short int unitDefId);


// Update an action's status
void UnitActionSetStatus(STRUCT_ACTION_BASE *action, ACTION_STATUS newStatus);


// Returns true if unit can move as close to target as specified by range (for example, for attacking).
// Usually called with maxRange=unit.getMaxRange() to check if unit can attack the target.
// isGroupAlgorithm : false for "individual" methods (unit/activity calls) => path finding 0x6A1CC0
// isGroupAlgorithm : true for "group" methods (AI methods) => path finding 0x583BC8. Cf STRUCT_UNKNOWN_PATH_FINDING
// This does NOT triggers a movement action (check only)
bool CanMoveToTarget(STRUCT_UNIT_BASE *unit, long int targetUnitId, float maxRange, bool isGroupAlgorithm);


// Exact role to confirm. What is sure: if successful, triggers an *actual* movement action.
// MAYBE this method allows finding path with enemy units blocking the way. Such units are added to path finding struct's unitid array (unknown_11DCE4) ?
// arg6: seen 0x1B (hardcoded)
// tempList should be empty in input, filled by this method. Please free the array (if non-NULL) afterwards !
// Call [unit+0x1A0], for example 0x44E130.
bool MoveToTarget_1A0(STRUCT_UNIT_BASE *unit, long int targetUnitId, float range, long int arg3,
	long int arg4, long int targetPlayerId, long int arg6, STRUCT_MANAGED_ARRAY *tempList);


// Add a unit to all relevant infAI lists. Cf 0x4BE0D0
bool AddUnitInInfAILists(STRUCT_INF_AI *infAI, long int unitId);

// Returns true if unit can attack (or convert) target, taking into account faith for priests, etc.
// This is about unit "compatibility", it does not take care of unit positions !!!
bool CanAttackTarget(AOE_STRUCTURES::STRUCT_UNIT_BASE *actor, AOE_STRUCTURES::STRUCT_UNIT_BASE *target);

// Returns true if a unit can convert another (actually, for priests)
bool CanConvert(STRUCT_UNIT_BASE *unit, long int targetUnitId);

// Gets the damage from an attacked over a target (defender)
float CalcDamage(STRUCT_UNIT_BASE *attacker, STRUCT_UNIT_BASE *defender);

bool IsUnitIdle(STRUCT_UNIT_BASE *unit);

// Make progress in building construction according to timeToAdd (in seconds)
void BuildingUnitDoConstruct(STRUCT_UNIT_BUILDING *building, float timeToAdd_seconds);

// Create unit activity using derived class that corresponds to supplied checksum (calls the correct constructor)
// Any unit (even base type) can have an activity, even if ROR originally only does that for living&building.
// Returns true if successful.
bool CreateUnitActivity(STRUCT_UNIT_BASE *unit, unsigned long int activityClassChecksum);

// Add visibility for given unit to specified player (might NOT be unit owner)
// If useUnitSizeRadius, do not use distance but unit.unitDef.sizeRadius instead.
bool AddVisibility(STRUCT_UNIT_BASE *unit, STRUCT_PLAYER *playerToUpdate, bool useUnitSizeRadius, long int distance);

// Remove visibility for given unit from specified player (might NOT be unit owner)
// If useUnitSizeRadius, do not use distance but unit.unitDef.sizeRadius instead.
bool RemoveVisibility(STRUCT_UNIT_BASE *unit, STRUCT_PLAYER *playerToUpdate, bool useUnitSizeRadius, long int distance);


// Calls AOE's method to change a unit owner. Warning, this has bugs, see RockNRor / crCommand.
void ChangeUnitOwner(AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer);


// Creates a unit at provided location. Does NOT make checks on location, please first make sure GetErrorForUnitCreationAtLocation returns 0.
// You can use 0 as posZ value.
// Returns NULL if it failed
AOE_STRUCTURES::STRUCT_UNIT_BASE *CreateUnitNoMapCheck(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID, float posY, float posX, float posZ);

// Creates a unit at provided location. Does NOT make checks on location, please first make sure GetErrorForUnitCreationAtLocation returns 0.
// You can use 0 as posZ value.
// Warning: orientation IS unit.orientation for types 30-80. But for types 10/20/90, is corresponds to orientationIndex as float.
// Returns NULL if it failed
// Compatible with both in-game and editor screens. This overload corresponds to deserialize (and create) unit operation.
AOE_STRUCTURES::STRUCT_UNIT_BASE *CreateUnitNoMapCheck(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID, float posY, float posX, float posZ,
	long int status, float orientation);

// Creates a unit at provided location only if GetErrorForUnitCreationAtLocation agrees !
// Returns NULL if it failed
AOE_STRUCTURES::STRUCT_UNIT_BASE *CheckAndCreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef,
	float posY, float posX, bool checkVisibility, bool checkHills, bool checkConflictingUnits);


// Has the same effect as "CTRL-0" or "CTRL-1" etc: assigns a shortcut number to units (and removes this shortcut from old units that had it)
// Returns 1 on success.
long int AssignShortcutToSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player, long int shortcutNumber);

// Selects units that have a given shortcut number.
long int SelectUnitsUsingShortcut(AOE_STRUCTURES::STRUCT_PLAYER *player, long int shortcutNumber, bool addToSelection = false);


// Returns a unitDefCommand object if actor unit has a valid right-click command on target unit.
// Returns NULL if there no possible interaction
AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF *GetUnitDefCommandForTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actorUnit,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *target, bool canSwitchForVillager);


// Execute unit.transform(unitDef*) method that consists in changing a unit's unit definition
// Typically for villagers (task switching)
void UnitTransform(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, AOE_STRUCTURES::STRUCT_UNITDEF_BASE *newUnitDef);

// Delete a unit (destroys the object, removes references...)
// This is a brutal deletion (not using async commands...), better use this in scenario editor only
void CallUnitDestructor(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, boolean doFree);

}
}
