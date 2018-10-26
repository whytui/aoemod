
#pragma once

#include <assert.h>
#include <string>
#include <AOE_struct_units.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_game_settings.h>
#include "AOEPrimitives_units.h"
#include "AOE_map.h"
#include "unitDefHandling.h"
#include "mainStructuresHandling.h"
#include "playerHandling.h"
#include "AOEPrimitives_gameCommand.h"

/* This file contains primitives about units handling = basic operations */


namespace AOE_STRUCTURES {
;

// Securely get an action pointer without having to re-write all checks/gets for intermediate objects.
// Return NULL if one of the objects is NULL/missing
AOE_STRUCTURES::STRUCT_ACTION_BASE *GetUnitAction(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

// Return the total remaining food amount for a farm ("immediatly available" + "action-remaining").
// Returns 0 in error cases (please check it is actually a farm !)
float GetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit);

// Modifies the total remaining food amount for a farm ("immediately available" + "action-remaining").
bool SetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit, float newAmount);

// Returns true if unit is currently fighting against (or moving towards) a MILITARY target (excluding houses, villagers, etc)
// Returns false if target is not visible or out of reach
bool HasVisibleMilitaryTarget(STRUCT_UNIT_BASE *unit);

// Returns current action target IF it is an attack action (including conversion)
STRUCT_UNIT_BASE *GetAttackTarget(STRUCT_UNIT_BASE *unit);

// Return the target position for a unit/action
// The result might be {-1, -1}
std::pair<float, float> GetActionTargetPosition(STRUCT_ACTION_BASE *action);
std::pair<float, float> GetActionTargetPosition(STRUCT_UNIT_BASE *unit);

// Returns true if "player" knows where targetUnit is located: unit is visible OR its location is explored and its speed=0
bool IsUnitPositionKnown(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *targetUnit);


// Change line of sight for a unique unit. Fails if unit does NOT have a dedicated unit definition.
bool ChangeLOSForUniqueUnit(STRUCT_UNIT_TRAINABLE *unit, float newLOS);


// Given a list of (actor) units, tell them to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitsToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE **actorUnitsList, long int actorUnitsCount, AOE_STRUCTURES::STRUCT_UNIT_BASE *target);

// Tell a unit to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitToInteractWithTarget(STRUCT_UNIT_COMMANDABLE *actorUnit, STRUCT_UNIT_BASE *target);


// Calls ROR's method to change a unit's action so it will move to supplied unit/position
// target can be NULL (only position will matter)
// unitToMove->ptrActionInformation is required to be NON-NULL ! Or the method will return without doing anything.
void MoveUnitToTargetOrPosition(STRUCT_UNIT_COMMANDABLE *unitToMove, STRUCT_UNIT_BASE *target, float posX, float posY);


// Update current action's status to "search" so that it will search for another target (if any)
void ForceUnitChangeTarget(STRUCT_UNIT_BASE *unit);


// Returns true if targetUnitDefId creates renewable resource:
// - farms (immediately available food is limited and increases progressively)
// - docks or trade buildings (trade goods are unlimited, but need time to resplenish)
bool CanGetRenewableResourceFrom(STRUCT_UNIT_BASE *unit, long int targetUnitDefId);

// Returns true if it is possible to trade with this unit (it has trade goods in its resource storage)
bool UnitOffersTrading(STRUCT_UNIT_BASE *unit);

// Returns true if it is possible to trade with this unit (it has trade goods in its resource storage)
bool UnitDefOffersTrading(STRUCT_UNITDEF_BASE *unitDef);

// Returns the "out" resource Id for trading with target unit, taking into account the correct unitDefCommand.
long int GetTradeOutResourceId(STRUCT_UNIT_BASE *actor, STRUCT_UNIT_BASE *targetUnit);

// Returns true if killed unit should keep its owned resource (food for gazelle, etc) when killed = most cases (default)
// Returns false if killed unit should lose its owned resource when killed = for example, gazelle killed by a military unit.
bool ShouldKeepOwnedResourceWhenKilledByClass(STRUCT_UNIT_BASE *killedUnit, GLOBAL_UNIT_AI_TYPES attackerClass);

// Returns true if a builder can become farmer to gather the farm he just built
// Returns false if he should not (because there already is a farmer, because he already carries a different resource)
// This does not detect pending commands (if some villager has just been told to gather this farm)
bool CanBuilderSwitchToFarmer(STRUCT_UNIT_BASE *builder, STRUCT_UNIT_BASE *farm);


// Returns the diplomacy stance regarding "unitOther", from unitMe's point of view.
// ***WARNING*** this returns "neutral" for "self". You might prefer using GetDiplomacyValueForPlayer.
// Returns CST_PDS_ENEMY in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES GetDiplomacyStanceForUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitMe, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitOther);

// Returns the diplomacy stance regarding "unitOther", from playerMe's point of view.
// ***WARNING*** this returns "neutral" for "self". You might prefer using GetDiplomacyValueForPlayer.
// Returns CST_PDS_ENEMY in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES GetDiplomacyStanceForUnit(AOE_STRUCTURES::STRUCT_PLAYER *playerMe, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitOther);

// Returns the diplomacy value regarding "unitOther", from unitMe's point of view.
// Returns CST_PDV_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES GetDiplomacyValueForUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitMe, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitOther);

// Returns the diplomacy value regarding "unitOther", from playerMe's point of view.
// Returns CST_PDV_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES GetDiplomacyValueForUnit(AOE_STRUCTURES::STRUCT_PLAYER *playerMe, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitOther);

// Delete all units whose "definition ID" is not a vanilla-AOE one.
// To be used in scenario editor
bool DeleteAllNonVanillaAoeUnits();


}
