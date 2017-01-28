#pragma once
#include <string>
#include <AOE_struct_player.h>
#include <AOE_struct_units.h>
#include <AOE_struct_main_ai.h>
#include "AOE_map.h"
#include "AOEPrimitives_units.h"
#include "unitHandling.h"
#include "crCommon.h"
#include "CustomRORInfo.h"
#include "crRPGMode.h"
#include "mainCustomAI.h"


using namespace AOE_STRUCTURES;

namespace CUSTOMROR {
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


}

}
