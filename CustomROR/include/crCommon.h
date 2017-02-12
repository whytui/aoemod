
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

#include "mainStructuresHandling.h"
#include "AOE_memory.h"
#include "AOE_map.h"
#include "UI_utilities.h"
#include "crConfig.h"
#include "civilizationInfo.h"
#include "crTrigger.h"
#include "crTriggerSet.h"
#include "traceMessage.h"
#include "interface.h" // SLP/icon ids...
#include "language.h"
#include "researches.h"
#include "crLocalization.h"
#include "unitDefHandling.h"
#include "buttonBarCommon.h"
#include "AOEPrimitives_gameCommand.h"
#include "playerHandling.h"


// Methods in this file are simple primitives that don't depend on customROR configuration. They generally are calls to existing game methods.




/* ----------- GETTERS ------------- */


// Returns true for maps where AI does build a dock and boats. Warning: for unknown map type (custom), this returns true.
bool IsDockRelevantForMap(MAP_TYPE_INDEX mti);

// Calculate distance (without optimization)
float GetDistance(float x1, float y1, float x2, float y2);

// Returns true if the cost could be correctly computed.
// Returns false if not. costTable content may have been modified though.
bool GetUnitCost(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID, float costTable[]);

// Fill resourceTypesOrder with ordered resource types: lower value in resourceAmounts = first position in (out) resourceTypesOrder
void SortResourceTypes(const int resourceAmounts[], int resourceTypesOrder[]);



/* ----------- "ACTIVE" methods ------------- */


// Pause/unpause the game
void SetGamePause(bool pauseOn);


// If resourceTable has enough resources, returns true and deduces the cost from resourceTable.
// If not, returns false and does not modify any value.
bool ApplyCostIfPossible(float costTable[], float resourceTable[]);


// Call AOE's Notify event method. Warning, the parameters can have different types.
// Use the overload with pointers to make sure you don't have cast issues.
void AOE_callNotifyEvent(long int eventId, long int playerId, long int variant_arg3, long int variant_arg4, long int variant_arg5);


// Reset an element in infAI.unitElemList. The slot will be re-used later by ROR. cf 0x4BA401.
// Return true if the element was updated.
// Return true if the element was updated/reset.
bool ResetInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *elem);

// Update an element in infAI.unitElemList if the unit is visible.
// Reset the element otherwise.
// If the element is reset, it is ALSO REMOVED from infAI lists.
// Return true if the element was updated/reset.
bool UpdateOrResetInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *elem);

// Remove a unitId from infAI "info" lists (creatable, gatherable, "defendable", artefact units).
// This does NOT manage "all my units" and "my buildings" lists. This does NOT manage unitElemList neither.
// DATID and unitAIType (unitClass) are used for optimisation. You can provide -1 if you don't have the information.
// Returns false if failed.
bool RemoveFromInfAIInfoList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int unitId, short int DATID, AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType);

// Find a unitElem in infAI list, returns NULL if not found.
AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *FindInfAIUnitElemInList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int unitId);

// Add a unit in infAI elem list, or update if existing. Warning: this uses unit structure's info, even if it is not visible !
// Please check for visibility to avoid "cheating"
// Returns true if successful
bool AddUpdateInfAIElemList(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

// Change unit owner in InfAI unitListElem according to unit visibility.
// Return true if updated.
bool UpdateUnitOwnerInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, long int newPlayerId);

// Set status for an element in infAI "construction history" array.
void AOE_InfAIBuildHistory_setStatus(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int posX, long int posY, long int unitDefId,
	AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS status);

// Remove a building from arrays for a player
void AOE_playerBldHeader_RemoveBldFromArrays(AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader, 
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);


// Get a localized string using ROR method.
// Returns true on success.
bool AOE_ReadLanguageTextForCategory(INTERNAL_MAIN_CATEGORIES category, long int commandId, long int subParam, char *buffer, long int bufferSize);

// Generate full creation text for a button (research, train) with costs and everything.
// buffer size must be at least 0x200.
// unitButtonInfo is allowed to be NULL.
// Cost info come from unitButtonInfo.
// elemLanguageCreationDllId is unit (or research) creationDllId (it includes shortcut key, if any)
void AOE_GetUIButtonCreationText(char *buffer, AOE_STRUCTURES::STRUCT_UI_UNIT_BUTTON_INFO *unitButtonInfo,
	AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID uiCmdId, long int elemLanguageCreationDllId);


// ---------- Other

// Automatically detects issues in empires.dat (requires to have been loaded already) and writes logs about errors in trace message handler.
bool AnalyzeEmpiresDatQuality();


// TO MOVE
namespace AOE_METHODS {
	

	// ROR's method to get a pseudo-random value
	static long int GetAndReCalcPseudoRandomValue() {
		long int result;
		_asm {
			MOV EDX, 0x52605D;
			CALL EDX; // Recalculate pseudo random
			MOV result, EAX;
		}
		return result;
	}
}

