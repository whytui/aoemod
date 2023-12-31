#include <assert.h>
#include <list>
#include <algorithm>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include <AOE_struct_main_ai.h>
#include "AOE_map.h"
#include "AOE_memory.h"
#include "traceMessage.h"
#include "mainStructuresHandling.h"
#include "researches.h"
#include "RockNRorInfo.h"
#include "AOEPrimitives_gameCommand.h"

#pragma once

/*
This file contains primitives to manipulate unit definition objects.
*/

using namespace AOE_STRUCTURES;


// External dependency because RockNRor fixes some bugs in player handling primitives
namespace ROCKNROR {
namespace STRATEGY {
	extern void UpdateStrategyWithUnreferencedExistingUnits(AOE_STRUCTURES::STRUCT_PLAYER *player);
}
}

namespace AOE_METHODS {
namespace PLAYER {
;


void ClearSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player);

// select: if true, add unit to selection. If false, remove from selection.
bool SelectUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, bool select);

// Return the matching score element
// Warning: May return NULL.
AOE_STRUCTURES::STRUCT_SCORE_ELEM *FindScoreElement(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_CONST_FUNC::SCORE_CATEGORIES category, AOE_CONST_FUNC::RESOURCE_TYPES resourceId);


// Call this when changing a player from "AI control" disabled to enabled
// This will run various actions to fix strategy, etc (example: do not build buildings human already built).
// The method will do nothing if player is NULL or if its AI structure is NULL.
void CheckAIWhenEnablingAIControl(int playerId);
void CheckAIWhenEnablingAIControl(AOE_STRUCTURES::STRUCT_PLAYER *player);

// Disable AI flags for human players, based on game initial settings (to be used at game startup)
void DisableAIFlagsForHuman();
// Restore AI flags based on human-controlled playerID (to be used in SP games only)
void RestoreAllAIFlags();
void SetAllAIFlags(bool enable);

// Set AI flag for specified player
// enableMode : 0=disable AI, 1=enable AI, -1=switch
void SetAIFlag(int playerId, int enableMode);

// Changes destination player id screen position from srcPlayerId's
bool CopyScreenPosition(const AOE_STRUCTURES::STRUCT_PLAYER *srcPlayer, AOE_STRUCTURES::STRUCT_PLAYER *destPlayer);
bool CopyScreenPosition(int srcPlayerId, int destPlayerId);

// Change human control to another player and set AI flags accordingly (if updateAIFlags is true)
AOE_STRUCTURES::STRUCT_PLAYER *ChangeControlledPlayer(int playerId, bool updateAIFlags);

// Return NULL if one of the objects is NULL/missing
AOE_STRUCTURES::STRUCT_RESEARCH_DEF *GetResearchDef(const AOE_STRUCTURES::STRUCT_PLAYER *player, short int researchId);


// Calls AOE's code mainAI.findUnit(DAT_Id)
AOE_STRUCTURES::STRUCT_UNIT_BASE *AOE_MainAI_findUnit(AOE_STRUCTURES::STRUCT_AI *mainAI, long int DAT_ID);

void GlobalSetNextManagedAIPlayer();

// Calls player.createGenericOrderCommandForSelectedUnits(targetUnit, f_posY, f_posX) (player+0xA0)
// Will find the matching command depending on target unit (might be NULL), play the command sound, create the "game command".
bool CreateCommandMoveForSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, float posX, float posY);

// Analog to player.createGenericOrderCommandForSelectedUnits(targetUnit, f_posY, f_posX) (player+0xA0)
// Will find the matching command depending on target unit (might be NULL), play the command sound, create the "game command".
// This overload does manually ROR's treatments, and create a "move" command including custom info
bool CreateCommandMoveForSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, float posX, float posY, unsigned char customFlag);

}
}


namespace AOE_STRUCTURES {
namespace PLAYER {
;

// Returns true if a position is fog-visible for a player.
// This is quite fast: directly accesses (optimized) memory, no underlying calls.
// Warning: posX/posY values are not controlled regarding map size !
bool IsFogVisibleForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int posX, long int posY);

// Returns true if a position has been explored by a player
// This is quite fast: directly accesses (optimized) memory, no underlying calls.
// Warning: posX/posY values are not controlled regarding map size !
bool IsExploredForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int posX, long int posY);


// Call this to make sure "currently managed AI player" is valid, so that game AI is not stuck.
// Preserves "current" active AI, if valid
void SetAValidCurrentAIPlayerId();

// Remove all AI-controlled flags for currently controlled player (cf game settings structure).
// Only for single player games.
void RemoveAIFlagsForCurrentlyControlledPlayer();


// Return a list of all unitDefIds that are/can be enabled in player's tech tree.
std::list<long int> GetActivableUnitDefIDs(AOE_STRUCTURES::STRUCT_PLAYER *player);

// Returns <> 0 if a unit is (currently) available for a given player. Returns the unit def pointer.
// Does not consider potential researches that could enable the unit afterwards.
// Warning: this only works for non-upgraded unit def IDs. OK for clubman, not for axeman (because axeman's DATID1 is clubman's DATID).
long unsigned int IsUnitAvailableForPlayer(short int DAT_ID, AOE_STRUCTURES::STRUCT_PLAYER *player);

// Returns true if the specified research is enabled for given player.
// True if the research is waiting requirement, available, in progress, done.
// False if the research is disabled.
// False if the research is disabled (or invalid ID)
bool IsResearchEnabledForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int researchId);

// Returns true if player resources are sufficient to pay the specified unit's cost (building or living unit)
// This only checks the 4 basic resources food/wood/stone/gold. For example, this does not control population headroom, etc.
bool CanUnitCostBePaid(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID);

// Searches (at least) a unit with provided shortcut number for given player.
// The first matching unit is returned (arbitrary), even if there are several.
AOE_STRUCTURES::STRUCT_UNIT_BASE *FindUnitWithShortcutNumberForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, char shortcutNumber);

// Get number of matching units (for a unitDef ID) for given player. Restricted to "creatable" units
// -1 are jokers for DAT_ID, unitAIType, unitStatus
long int GetPlayerUnitCount(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID,
	AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType, char minUnitStatus, char maxUnitStatus);

// Disable the research that enables a given unit, so that it can't be trained.
// For units that are enabled by a "visible" research, like short sword, the research won't be visible (disabled).
// To be called before game starts
// Returns true if a research was found and disabled.
bool DisableUnitForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID);


// Clear player selection then select provided unit. Compatible with editor too.
// If centerScreen is true, player's screen will be centered to unit.
void SelectOneUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase, bool centerScreen);


// Transforms specified unit to "main" unit when a quick selection (via keyboard shortcut) is requested.
// For example, for key 'H', the "main" TC is selected. This method allows to set the "main" TC to select with 'H'.
// Actually, this moves the unit in player's "creatable" units list
// This only has a visible effect for units that can be selected with a shortcut key.
bool MakeMainUnitForShortcutSelection(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *unit);

// Transforms specified unit to "main" unit when a quick selection (via keyboard shortcut) is requested.
// For example, for key 'H', the "main" TC is selected. This method allows to set the "main" TC to select with 'H'.
// Actually, this moves the unit in player's "creatable" units list
// This only has a visible effect for units that can be selected with a shortcut key.
// This is only possible when unitId belongs to human-controlled player.
bool MakeMainUnitForShortcutSelection(long int unitId);


// OBSOLETE: avoid using it
// Duplicates an existing unit definition (srcDAT_ID) into a new unit definition for given player.
// New unitDef will be available in player's list. Everything is managed so that the game will automatically free everything correctly.
// You can send "" as name (in that case original unit's name will be used). Max length = 0x2F.
// Returns the new DAT_ID if successful, -1 otherwise.
// You can change any value for the new unitDef (but avoid messing with pointers?), but make sure you do it BEFORE you create units of this kind !
// It is not recommended to call this too much ! It would add a lot of unit definition and would impact seriously game performance.
// The maximum number of DAT_ID we allow here is 0x7FFF (to take no risk regarding signed short int values)
// The method supports all unit (def) types
// Warning: there might be issues when such units are converted (bug in destructor in AOE, a unitDef for a given DATID should exist for all civs ?)
// Maybe it's better not to use it for living/buildings units (because of conversion)
// Note: see also AOE unitDef constructors that copy an existing one, e.g. 4ED1B0 for living (type 70).
short int DuplicateUnitDefinitionForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int srcDAT_ID, char *name);


// Set "shared exploration" flag for a given player to true or false. Do not use this with MP game (not sure if it causes sync error)
void SetPlayerSharedExploration_hard(long int playerId, bool enable);

// Set "shared exploration" flag for a given player to true or false. This version should be compatible with MP games (uses ROR command system)
void SetPlayerSharedExploration_safe(long int playerId);


// Returns the most damaged unit, NULL if not found
// unitDefIdFilter = -1 in most cases (joker). Otherwise, THE only unit def to take into account
STRUCT_UNIT_BASE *GetPlayerMostDamagedUnit(STRUCT_AI *mainAI, GLOBAL_UNIT_AI_TYPES aiType, long int unitDefIdFilter);


// Returns the diplomacy stance regarding "otherPlayerId", from playerMe's point of view.
// ***WARNING*** this returns "neutral" for "self". You might prefer using GetDiplomacyValueForPlayer.
// Returns CST_PDS_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES GetDiplomacyStanceForPlayer(STRUCT_PLAYER *playerMe, long int otherPlayerId);

// Returns the diplomacy stance regarding "playerOther", from playerMe's point of view.
// ***WARNING*** this returns "neutral" for "self". You might prefer using GetDiplomacyValueForPlayer.
// Returns CST_PDS_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES GetDiplomacyStanceForPlayer(STRUCT_PLAYER *playerMe, STRUCT_PLAYER *playerOther);


// Returns the diplomacy stance regarding "otherPlayerId", from playerMe's point of view.
// Returns CST_PDV_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES GetDiplomacyValueForPlayer(STRUCT_PLAYER *playerMe, long int otherPlayerId);

// Returns the diplomacy stance regarding "playerOther", from playerMe's point of view.
// Returns CST_PDV_UNKNOWN in error cases
AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES GetDiplomacyValueForPlayer(STRUCT_PLAYER *playerMe, STRUCT_PLAYER *playerOther);

}
}
