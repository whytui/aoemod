
#pragma once

#include <assert.h>
#include <ROR_API_pub.h>
#include <AOE_struct_game_settings.h>
#include <AOE_const_internal.h>
#include <AOE_const_language.h>
#include <AOE_commands.h>
#include <ROR_global_variables.h>

#include "AOEPrimitives_global.h"
#include "mainStructuresHandling.h"
#include "AOE_memory.h"
#include "AOE_map.h"
#include "language.h"
#include "RockNRorLocalization.h"
#include "unitDefHandling.h"
#include "AOEPrimitives_gameCommand.h"
#include "playerHandling.h"


// Methods in this file are simple primitives that don't depend on RockNRor configuration. They generally are calls to existing game methods.




/* ----------- GETTERS ------------- */


// Returns true for maps where AI does build a dock and boats. Warning: for unknown map type (custom), this returns true.
bool IsDockRelevantForMap(MAP_TYPE_INDEX mti);

// Calculate distance between 2 points (without optimization)
float GetDistance(float x1, float y1, float x2, float y2);

// Returns true if the cost could be correctly computed.
// Returns false if not. costTable content may have been modified though.
// Make sure costTable is large enough (cf MAX_RESOURCE_TYPE_ID)
bool GetUnitCost(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID, float costTable[]);

// Fill resourceTypesOrder with ordered resource types: lower value in resourceAmounts = first position in (out) resourceTypesOrder
void SortResourceTypes(const int resourceAmounts[], int resourceTypesOrder[]);



/* ----------- "ACTIVE" methods ------------- */


// If resourceTable has enough resources, returns true and deduces the cost from resourceTable.
// If not, returns false and does not modify any value.
bool ApplyCostIfPossible(float costTable[], float resourceTable[]);
