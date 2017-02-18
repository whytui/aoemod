#pragma once
#include <string>
#include <AOE_struct_player.h>
#include <AOE_struct_units.h>
#include <AOE_struct_main_ai.h>
#include "AOE_map.h"
#include "AOEPrimitives_units.h"
#include "unitHandling.h"
#include "RockNRorCommon.h"
#include "RockNRorInfo.h"
#include "crRPGMode.h"
#include "mainCustomAI.h"


using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace UNIT {
;


// Returns how many units where told to move.
// If maxDistance > 0, only units at a maximum distance of maxDistance are told to move.
// Returns <0 if there is an error
int MoveIdleMilitaryUnitsToMousePosition(STRUCT_PLAYER *player, float maxDistance = 0);


// Searches all idle units in a specified range (see config) and order them to come at screen location
// Requires ManageAI !
void CallNearbyIdleMilitaryUnits();


// Select next idle military unit for current player
void SelectNextIdleMilitaryUnit();


// Handles the event "farm is depleted". NOT called when a farm is destroyed/killed.
// Warning: this event occurs before the farm unit is actually "killed"
void OnFarmDepleted(long int farmUnitId);


// Returns true if a shortcut has been added/modified
bool AutoAssignShortcutToUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);


// Occurs when a unit is killed by an attack (EXCLUDES suicide with DEL, transported units whose transport is destroyed, conversion)
void OnAttackableUnitKilled(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *killedUnit, AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit);

// Entry point when creating unit activity structure.
// Returns true if ROR code can create unitAI (activity) on its own (default).
// Returns false if we want to skip ROR code (so that it does not create unitAI)
bool OnUnitCreateActivityStruct(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase);

// Returns true if the unit specified can have a unit activity.
bool AllowCreateActivityStructForUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase);

// Returns a "infAI elem list" pointer of a trade target if found, NULL if not found
// By default (game code), this searches for the closest unit that does not belong to "me", is a dock, whose player has trade goods>0.
AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *FindTradeTargetElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int actorUnitId);


}

}
