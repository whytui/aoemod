
#pragma once

#include <assert.h>
#include <list>
#include <algorithm>
#include <ROR_API_pub.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_main_ai.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_structures_drs.h>
#include <AOE_offsets.h>
#include <AOE_const_internal.h>
#include <AOE_const_language.h>
#include <triggerDefinition.h>
#include <mystrings.h>
#include <AOE_commands.h>
#include <Windows.h>
#include <WinUser.h> // get mouse pos
#include <ROR_global_variables.h>
#include <basicFilesHandling.h>

#include "AOEPrimitives_global.h"
#include "mainStructuresHandling.h"
#include "AOE_memory.h"
#include "AOE_map.h"
#include "UI_utilities.h"
#include "RockNRorConfig.h"
#include "civilizationInfo.h"
#include "crTrigger.h"
#include "crTriggerSet.h"
#include "traceMessage.h"
#include "interface.h" // SLP/icon ids...
#include "language.h"
#include "researches.h"
#include "RockNRorLocalization.h"
#include "unitDefHandling.h"
#include "buttonBarCommon.h"
#include "AOEPrimitives_gameCommand.h"
#include "playerHandling.h"
#include "AOEPrimitives_lists.h"


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


// Update an element in infAI.unitElemList if the unit is visible.
// Reset the element otherwise.
// If the element is reset, it is ALSO REMOVED from infAI lists.
// Return true if the element was updated/reset.
//bool UpdateOrResetInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *elem);

// Change unit owner in InfAI unitListElem according to unit visibility.
// Return true if updated.
//bool UpdateUnitOwnerInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, long int newPlayerId);


