
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
#include "buttonBarConst.h"


// Methods in this file are simple primitives that don't depend on customROR configuration. They generally are calls to existing game methods.

#define UNUSED_PLID_FOR_TRIGGERS 9
static_assert((UNUSED_PLID_FOR_TRIGGERS >= 0) && (UNUSED_PLID_FOR_TRIGGERS < 16), "Bad player ID for UNUSED_PLID_FOR_TRIGGERS");




/* ----------- GETTERS ------------- */


// Useful to get structure from a unit id. May return NULL !
// Only works for creatable (unitId >= 0). This is just a choice to avoid writing same bugs as ROR
// (some functions use -1 as <No unit> but get an irrevant unit struct then because -1 is not tested before calling getUnitStruct(...))
AOE_STRUCTURES::STRUCT_UNIT_BASE *GetUnitStruct(long int unitId);

// Returns a unit definition if valid, NULL otherwise.
AOE_STRUCTURES::STRUCT_UNITDEF_BASE *GetUnitDefStruct(AOE_STRUCTURES::STRUCT_PLAYER *player, short int unitDefId);

// Get a unit name from empires.dat data (read from civ 0)
// Returns NULL if not found. This requires that empires.dat file has already been read to global structure.
const char *GetUnitName(short int unitDefId);

// Return NULL if one of the objects is NULL/missing
AOE_STRUCTURES::STRUCT_RESEARCH_DEF *GetResearchDef(const AOE_STRUCTURES::STRUCT_PLAYER *player, short int researchId);

// Returns true if current game is multiplayer, false otherwise (including error cases)
bool IsMultiplayer();

// Returns true if the game is currently running
bool IsGameRunning();

// Returns true for maps where AI does build a dock and boats. Warning: for unknown map type (custom), this returns true.
bool IsDockRelevantForMap(MAP_TYPE_INDEX mti);

// Return the matching score element
// Warning: May return NULL.
AOE_STRUCTURES::STRUCT_SCORE_ELEM *FindScoreElement(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_CONST_FUNC::SCORE_CATEGORIES category, AOE_CONST_FUNC::RESOURCE_TYPES resourceId);

// Calculate distance
float GetDistance(float x1, float y1, float x2, float y2);

// Returns true if the cost could be correctly computed.
// Returns false if not. costTable content may have been modified though.
bool GetUnitCost(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID, float costTable[]);

// Fill resourceTypesOrder with ordered resource types: lower value in resourceAmounts = first position in (out) resourceTypesOrder
void SortResourceTypes(const int resourceAmounts[], int resourceTypesOrder[]);

// Common function for panic mode unit searching.
// Returns true if it is possible to train the unit. In such case, cost is decreased from remainingResources and actorCounter is decreased too.
// Returns false and does not change values if it is not possible (warning: tempCost can be modified in all cases)
bool PrepareUnitToAddIfPossible(AOE_STRUCTURES::STRUCT_PLAYER *player, short int unitId_toAdd, short int unitId_actor,
	long int *actorCounter, short int *lastCostDAT_ID, float remainingResources[], float tempCost[]);

// Return a list of all unitDefIds that are/can be enabled in player's tech tree.
std::list<long int> GetActivableUnitDefIDs(AOE_STRUCTURES::STRUCT_PLAYER *player);


// Calls AOE's code mainAI.findUnit(DAT_Id)
AOE_STRUCTURES::STRUCT_UNIT_BASE *AOE_MainAI_findUnit(AOE_STRUCTURES::STRUCT_AI *mainAI, long int DAT_ID);


// Returns <> 0 if a unit is (currently) available for a given player. Returns the unit def pointer.
// Does not consider potential researches that could enable the unit afterwards.
// Warning: this only works for non-upgraded unit def IDs. OK for clubman, not for axeman (because axeman's DATID1 is clubman's DATID).
long unsigned int IsUnitAvailableForPlayer(short int DAT_ID, AOE_STRUCTURES::STRUCT_PLAYER *player);

// Searches (at least) a unit with provided shortcut number for given player.
// The first matching unit is returned (arbitrary), even if there are several.
AOE_STRUCTURES::STRUCT_UNIT_BASE *FindUnitWithShortcutNumberForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, char shortcutNumber);

// Get number of matching units (for a unitDef ID) for given player. Restricted to "creatable" units
// -1 are jokers for DAT_ID, unitAIType, unitStatus
long int GetPlayerUnitCount(AOE_STRUCTURES::STRUCT_PLAYER *player, short int DAT_ID, 
	AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType, char minUnitStatus, char maxUnitStatus);

// Returns a pointer to trigger data
char *GetTriggerDataPtr(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo);
// Set trigger data pointer. Returns false if failed.
bool SetTriggerDataPtr(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo, char *newDataPtr);
// Returns trigger data size
long int GetTriggerDataSize(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo);
// Set trigger data size. Returns false if failed.
bool SetTriggerDataSize(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo, long int sizeToSet);

// Returns true if provided trigger text contains the END tag before any trigger information.
bool TriggerTextContainsENDTagAtBeginning(char *triggerText);

// Returns the number of queued units for a given DATID.
long int GetTotalQueueNumberForUnit(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *bld, short int unitDefId);


/* ----------- "ACTIVE" methods ------------- */


// Remove all AI-controlled flags for currently controlled player (cf game settings structure).
// Only for single player games.
void RemoveAIFlagsForCurrentlyControlledPlayer();

// Call this to make sure "currently managed AI player" is valid, so that AI is not stuck.
void SetAValidCurrentAIPlayerId();

// Pause/unpause the game
void SetGamePause(bool pauseOn);

// This will display a text in the game screen (like chat)
void CallWriteText(const char *txt);

// This will display a text in the game screen (like yellow/orange error messages)
void CallWriteCenteredText(const char *txt, long int numLine = 1, long int color = 0x55, long int background = 0);


// If resourceTable has enough resources, returns true and deduces the cost from resourceTable.
// If not, returns false and does not modify any value.
bool ApplyCostIfPossible(float costTable[], float resourceTable[]);

// Calls ROR's method to change a unit's action so it will move to supplied unit/position
// target can be NULL (only position will matter)
// unitToMove->ptrActionInformation is required to be NON-NULL ! Or the method will return without doing anything.
void MoveUnitToTargetOrPosition(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *unitToMove, AOE_STRUCTURES::STRUCT_UNIT_BASE *target, float posX, float posY);

// Tells a unit to (move and) attack another unit (using tacAI)
// Returns true if successful
bool MoveAndAttackTarget(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actor, AOE_STRUCTURES::STRUCT_UNIT_BASE *target);

// Given a list of (actor) units, tell them to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitsToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE **actorUnitsList, long int actorUnitsCount, AOE_STRUCTURES::STRUCT_UNIT_BASE *target);

// Tell a unit to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actorUnit, AOE_STRUCTURES::STRUCT_UNIT_BASE *target);

// Returns a unitDefCommand object if actor unit has a valid right-click command on target unit.
// Returns NULL if there no possible interaction
AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF *GetUnitDefCommandForTarget(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actorUnit,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *target, bool canSwitchForVillager);


// Creates a unit at provided location. Does NOT make checks on location, please first make sure GetErrorForUnitCreationAtLocation returns 0.
// You can use 0 as posZ value.
// Returns NULL if it failed
AOE_STRUCTURES::STRUCT_UNIT_BASE *CreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID, float posY, float posX, float posZ);

// Creates a unit at provided location. Does NOT make checks on location, please first make sure GetErrorForUnitCreationAtLocation returns 0.
// You can use 0 as posZ value.
// Warning: orientation IS unit.orientation for types 30-80. But for types 10/20/90, is corresponds to orientationIndex as float.
// Returns NULL if it failed
// Compatible with both in-game and editor screens. This overload corresponds to deserialize (and create) unit operation.
AOE_STRUCTURES::STRUCT_UNIT_BASE *CreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID, float posY, float posX, float posZ,
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

// Enable or disable a research for a player
void AOE_enableResearch(AOE_STRUCTURES::STRUCT_PLAYER *player, long int research_id, bool enable);

// Disable the research that enables a given unit, so that it can't be trained.
// For units that are enabled by a "visible" research, like short sword, the research won't be visible (disabled).
// To be called before game starts
// Returns true if a research was found and disabled.
bool DisableUnitForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID);

// Calls AOE's method to change a unit owner. Warning, this has bugs, see customROR / crCommand.
void AOE_ChangeUnitOwner(AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer);

// Call AOE's Notify event method. Warning, the parameters can have different types.
// Use the overload with pointers to make sure you don't have cast issues.
void AOE_callNotifyEvent(long int eventId, long int playerId, long int variant_arg3, long int variant_arg4, long int variant_arg5);
// Call AOE's Notify event method. Warning, the parameters can have different types. Here pointers here so there is no undesired cast !
void AOE_callNotifyEvent(long int eventId, long int playerId, void *variant_arg3, void *variant_arg4, void *variant_arg5);

void AOE_clearSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player);
// select: if true, add unit to selection. If false, remove from selection.
bool AOE_selectUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, bool select);

// Calls AOE's path finding method, using 0x583BC8 (not 0x6A1CC0)
// allArgs indices are 1-15 (do NOT use 0). Warning, allArgs[6] is a float, not an int.
// Arguments (1-15) are:
// srcPosY, srcPosX, destPosY, destPosX, ptrActorUnit, f_range?, targetUnitId, updateUnitPathInfo?(!OnlyCheck),
// arg9, arg10, arg11, arg12, arg13(int_distance?), arg14(unitGrp?), arg15
// See also AOE_calcPathForMove
long int callFindPathForUnit(long int allArgs[15 + 1]);

// pathFindingStruct can be 0x583BC8 or 0x6A1CC0. Don't know the specific roles yet :(
// updateUnitPathInfo = "do move". If false, this just checks if movement is possible.
long int AOE_calcPathForMove(STRUCT_UNKNOWN_MAP_DATA_F04C *pathFindingStruct,
	long int srcPosY, long int srcPosX, long int destPosY, long int destPosX, 
	AOE_STRUCTURES::STRUCT_UNIT_BASE *ptrActorUnit, float maxRange, long int targetUnitId, long int updateUnitPathInfo,
	long int arg9, long int arg10, long int arg11, long int arg12, 
	long int distance_unsure, long int targetPlayerId, long int unknown_unitClass);


// Set "shared exploration" flag for a given player to true or false. Do not use this with MP game (not sure if it causes sync error)
void SetPlayerSharedExploration_hard(long int playerId, bool enable);

// Set "shared exploration" flag for a given player to true or false. This version should be compatible with MP games (uses ROR command system)
void SetPlayerSharedExploration_safe(long int playerId);

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

// Change unit owner in InfAI unitListElem according to unit visibility.
// Return true if updated.
bool UpdateUnitOwnerInfAIUnitListElem(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, long int newPlayerId);

// Set status for an element in infAI "construction history" array.
void AOE_InfAIBuildHistory_setStatus(AOE_STRUCTURES::STRUCT_INF_AI *infAI, long int posX, long int posY, long int unitDefId,
	AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS status);

// Remove a building from arrays for a player
void AOE_playerBldHeader_RemoveBldFromArrays(AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader, 
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

// Clear player selection then select provided unit. Compatible with editor too.
// If centerScreen is true, player's screen will be centered to unit.
void SelectOneUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase, bool centerScreen);

// Add a line with an attribute icon/value in game unit info zone (bottom left)
// If a line is added, lineIndex is incremented.
void UnitInfoZoneAddAttributeLine(AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone,
	long int iconId, long int displayType, long int displayedValue, long int totalValue, long int &lineIndex);


// -- Commands

// commandStruct must have been allocated with a "AOE" alloc method like AOEAlloc(...)
// It is freed by game code, don't use it / free it afterwards !
// Returns false if failed
bool AddCommandToGameCmdQueue(void *commandStruct, long int structSize);

// Create a "ROR" command struct (right-click). Returns false if failed.
bool CreateCmd_RightClick(long int actorUnitId, long int targetUnitId, float posX, float posY);
bool CreateCmd_RightClick(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE **actorUnitsList, long int actorUnitsCount, long int targetUnitId, float posX, float posY);

// Create a "ROR" command struct (build). Returns false if failed.
bool CreateCmd_Build(long int actorUnitId, short int DATID, float posX, float posY);

// Create a "ROR" command struct (change diplomacy). Returns false if failed.
bool CreateCmd_ChangeDiplomacy(short int playerId, short int targetPlayerId, PLAYER_DIPLOMACY_STANCES diplomacyStance);

// Create a "ROR" command struct (set ally victory flag). Returns false if failed.
bool CreateCmd_SetAllyVictory(short int playerId, bool enable);

// Create a "ROR" command struct (give writing - set shared exploration). Returns false if failed.
bool CreateCmd_SetSharedExploration(short int playerId);

// Create a "ROR" command struct (change game speed). Returns false if failed.
bool CreateCmd_ChangeGameSpeed(float newSpeed);

// Create a "ROR" command struct (add a resource amount to a player). Returns false if failed.
bool CreateCmd_AddResource(short int playerId, short int resourceId, float value);

// Create a "ROR" command struct (set steroids mode ON/OFF). Returns false if failed.
bool CreateCmd_SetSteroids(bool enable);

// Create a "ROR" command struct (pay tribute). Returns false if failed.
bool CreateCmd_PayTribute(long int actorPlayerId, long int targetPlayerId, AOE_CONST_FUNC::RESOURCE_TYPES resourceType, float amount, float tributeInefficiency);


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


// Add a command button in unit-commands zone (under game zone).
// buttonIndex: 0-4 = first row, 6-10=second row, 5 and 11 are "special" right buttons (11=unselect/cancel, generally)
// DATID can be a unitDefId (train), researchId (do_research)... Set it to 0 when not relevant.
// isDisabled : set it to true to get a read only button (no possible click)
// creationText can be left NULL to display a text using unit/research/etc's LanguageDLLID.
// iconSlpInfo can be left NULL unless we want to use specific SLP resource for icon.
// Technically, this just updates the button (no button object is created).
// Refresh may NOT be performed if underlying info (command id, DATID) are unchanged, which may lead to wrong updates
bool AddInGameCommandButton(long int buttonIndex, AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID UICmdId,
	long int DATID, bool isDisabled, const char *creationText, AOE_STRUCTURES::STRUCT_SLP_INFO *iconSlpInfo,
	bool forceUpdate);

// Returns true if the button is visible. Use this overload for performance if you already have STRUCT_UI_IN_GAME_MAIN pointer.
// Returns false if the button is hidden, or if an error occurs.
bool IsInGameUnitCommandButtonVisible(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, long int buttonIndex);
// Returns true if the button is visible
// Returns false if the button is hidden, or if an error occurs.
bool IsInGameUnitCommandButtonVisible(long int buttonIndex);

// To be used with button IDs from unit defintion/researches to get a buttonIndex for game main UI structure (command buttons)
// WARNING: returns -1 if DATButtonId is -1 or invalid.
long int GetButtonInternalIndexFromDatBtnId(char DATButtonId);


// ---------- Other

// Automatically detects issues in empires.dat (requires to have been loaded already) and writes logs about errors in trace message handler.
bool AnalyzeEmpiresDatQuality();


// TODO: move in a dedicated file
// Returns the cursor that corresponds to a given mouse action type (for custom ones)
static GAME_CURSOR GetCursorForCustomActionType(MOUSE_ACTION_TYPES mouseActionType) {
	switch (mouseActionType) {
	case AOE_CONST_INTERNAL::CST_MAT_CR_PROTECT_UNIT_OR_ZONE:
		return GAME_CURSOR::GC_GROUP;
	case AOE_CONST_INTERNAL::CST_MAT_VILLAGER_SET_BUILDING_LOCATION:
		return GAME_CURSOR::GC_HAND;
	case AOE_CONST_INTERNAL::CST_MAT_NORMAL:
	case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_01:
	case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_02_SELECT_ZONE:
	case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_03_SELECT_ZONE:
	case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_04:
	case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_05:
	case AOE_CONST_INTERNAL::CST_MAT_VILLAGER_BUILD_MENU:
	case AOE_CONST_INTERNAL::CST_MAT_EDITOR_SET_UNIT_LOCATION:
	case AOE_CONST_INTERNAL::CST_MAT_SET_TERRAIN_TYPE:
	case AOE_CONST_INTERNAL::CST_MAT_SET_ALTITUDE:
	case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_0E:
	case AOE_CONST_INTERNAL::CST_MAT_EDITOR_MOVE_UNIT:
	case AOE_CONST_INTERNAL::CST_MAT_EDITOR_SELECT_TARGET_UNIT:
	case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_11_SELECT_ZONE:
	case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_12_SELECT_UNIT_CANT_UNSELECT:
	case AOE_CONST_INTERNAL::CST_MAT_SET_CLIFFS:
	case AOE_CONST_INTERNAL::CST_MAT_EDITOR_CHANGE_ORIENTATION:
	case AOE_CONST_INTERNAL::CST_MAT_SET_WALLS:
	case AOE_CONST_INTERNAL::CST_MAT_CLICK_ZONE_TO_ATTACK:
	case AOE_CONST_INTERNAL::CST_MAT_SELECT_CONVERT_TARGET:
	case AOE_CONST_INTERNAL::CST_MAT_SELECT_HEAL_TARGET:
	case AOE_CONST_INTERNAL::CST_MAT_SELECT_UNKNOWN_67:
	case AOE_CONST_INTERNAL::CST_MAT_SELECT_REPAIR_TARGET:
	case AOE_CONST_INTERNAL::CST_MAT_SELECT_UNKNOWN_69:
	default:
		return GAME_CURSOR::GC_NORMAL; // This is NOT correct for all types... This method is supposed to handle custom types, not standard ones.
	}
}

