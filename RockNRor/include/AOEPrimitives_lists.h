#pragma once

#include <assert.h>
#include <gameVersion.h>
#include <AOE_struct_managed_array.h>
#include <AOE_struct_units.h>
#include <AOE_struct_inf_ai.h>
#include <AOE_struct_player.h>
#include "unitDefHandling.h"


/* This file contains AOE primitives about units handling */

using namespace AOE_STRUCTURES;

namespace AOE_METHODS {
namespace LISTS {
;


// Reset an element in infAI.unitElemList. The slot will be re-used later by ROR. cf 0x4BA401.
// Return true if the element was updated (reset).
// Please DO NOT USE directly. See unitExtensionHandler
bool ResetInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI_DETAILED_UNIT_INFO *elem);


// Find a unitElem in infAI list, returns NULL if not found.
AOE_STRUCTURES::STRUCT_INF_AI_DETAILED_UNIT_INFO *FindInfAIUnitElemInList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int unitId);


// Add a unit in infAI elem list, or update if existing. Warning: this uses unit structure's info, even if it is not visible !
// Please check for visibility to avoid "cheating"
// Returns true if successful.
// This executes ROR's code
// Please DO NOT USE directly. See unitExtensionHandler
bool AddUpdateInfAIElemList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);


// Remove a unitId from infAI "info" lists (creatable, gatherable, "defendable", artefact units).
// This does NOT manage "all my units" and "my buildings" lists. This does NOT manage unitElemList neither.
// DATID and unitAIType (unitClass) are used for optimisation. You can provide -1 if you don't have the information.
// Returns false if failed.
// Please DO NOT USE directly. See unitExtensionHandler
bool RemoveFromInfAIInfoList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int unitId, short int DATID, AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType);



// Remove a building from arrays for a player
void PlayerBldHeader_RemoveBldFromArrays(AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

// Set status for an element in infAI "construction history" array.
void InfAIBuildHistory_setStatus(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int posX, long int posY, long int unitDefId,
	AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS status);


}
}
