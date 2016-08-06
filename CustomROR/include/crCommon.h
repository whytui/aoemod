
#pragma once

#include <assert.h>
#include <list>
#include <algorithm>
#include <ROR_API_pub.h>
#include <AOE_structures.h>
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
#include "crGameObjects.h"
#include "interface.h" // SLP/icon ids...
#include "language.h"
#include "researches.h"
#include "crLocalization.h"


// Defines common objects/variables/methods for CustomROR
// Methods in this file are simple primitives that don't depend on customROR configuration. They generally are calls to existing game methods.

#define CST_TIMER_STATS_ARRAY_SIZE 20
#define UNUSED_PLID_FOR_TRIGGERS 9
static_assert((UNUSED_PLID_FOR_TRIGGERS >= 0) && (UNUSED_PLID_FOR_TRIGGERS < 16), "Bad player ID for UNUSED_PLID_FOR_TRIGGERS");

using namespace AOE_CONST_FUNC;
using namespace AOE_STRUCTURES;


// Constants
static char *txtHumanPenalty = "Dislike penalty for human player";
static char *txtGameSpeedFactor = "Game speed change factor*100 (125 for 1.25)";
static char *txtAutoRebuildFarms = "Auto. rebuild farms";
static char *txtAutoRebuildFarmsMaxNumber = "Auto. rebuild farms max number";
static char *txtAutoRebuildFarmsMaxFood = "Auto. rebuild farms max food";
static char *txtAutoRebuildFarmsMinWood = "Auto. rebuild farms min wood";


// Max X/Y value we can quick-calculate distances on.
#define CST_INT_DISTANCE_VALUES_MAX_COORDINATE 30


// Internal structures/variables for CustomROR that are not read from configuration
class CustomRORInfo {
public:
	CustomRORInfo();
	~CustomRORInfo();

	// Custom ROR Configuration
	CustomRORConfig configInfo;
	bool hasFixForBuildingStratElemUnitId;
	bool hasManageAIFeatureON; // Is "ManageAI" feature installed (use custom flag "isAI" in player struct)
	bool hasCustomSelectedUnitsMemory; // Player struct size is extended to host more selected units
	bool hasRemovePlayerInitialAgeInScenarioInit;
	// Internal variables
	vector<short int> unitDefToDisable[9];
	vector<short int> researchesToDisable[9];
	CrGameObjects myGameObjects;
	int gameTimerSlowDownCounter;
	long int CollectedTimerIntervalsIndex;
	long int CollectedTimerIntervals_ms[CST_TIMER_STATS_ARRAY_SIZE];
	int LastGameTimerAutoFix_second;
	long int lastCustomRORTimeExecution_gameTime_s; // In seconds. Used to limit customROR treatments in ROR's timer execution.
	int LastDislikeValuesComputationTime_second; // Stores game time when we last computed players dislike values.
	// To know which civ player names are already used when choosing names. Values: -1=unused, >=0=used by player #i
	// Warning: playerName index are from 0 to 8 here instead of 1 to 9 ! Be careful.
	char nameIndexIsUsedByPlayer[CST_MAX_TOTAL_CIV_COUNT][CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV];
	// Own DRS objects
	STRUCT_SLP_INFO customRorIcons;
	STRUCT_SLP_INFO customRorUnitShortcuts;

	// Triggers
	CR_TRIGGERS::crTriggerSet *triggerSet; // all information about custom triggers.
	long int triggersLastCheckTime_s; // In seconds. Last game time when triggers have been checked (for "timer" trigger types)

	// To monitor conversion efficiency
	long int activeConversionAttemptsCount[9]; // Per playerId
	long int activeConversionSuccessfulAttemptsCount[9]; // Per playerId
	long int passiveConversionAttemptsCount[9]; // Per playerId
	long int passiveConversionSuccessfulAttemptsCount[9]; // Per playerId

	// UI Variables : CustomROR yes/no dialog
	unsigned long int *customYesNoDialogVar;  // customROR's dialog struct. NULL means dialog does not exist.
	AOE_STRUCTURES::STRUCT_UI_BUTTON *customGameMenuOptionsBtnVar;
private:
	// "OK" button of customROR's game popup. NULL means popup does not exist. See HasOpenedCustomGamePopup.
	// It is CRUCIAL that this variable is always correctly set (reset).
	// Never affect this variable manually, let the open/close methods do it.
	AOE_STRUCTURES::STRUCT_UI_BUTTON *customGamePopupButtonVar;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *customGamePopupCancelBtnVar;
	AOE_STRUCTURES::STRUCT_ANY_UI *customGamePopupVar; // Pointer to our custom game popup object when open. Should be NULL <=> customGamePopupButtonVar==NULL
	std::vector<AOE_STRUCTURES::STRUCT_ANY_UI*> objectsToFree; // Popup's UI components list. Useful to destroy all of them automatically
	std::vector<AOE_STRUCTURES::STRUCT_ANY_UI*> garbageComponentsToFree; // Popup's UI components that are waiting to be destroyed (objects that could not be destroyed at popup closing)
	std::vector<AOE_STRUCTURES::STRUCT_ANY_UI*> garbageComponentsToFree_older; // Popup's UI components that are waiting to be destroyed and that are no longer related the current events (now ready to be destroyed)

public:

	// Reset variables that are used during game.
	// Only reset internal variables that are game-dependent !
	// This is called on each game start/load
	void ResetVariables();

	// Resets and fills the list of unitDefID to disable for a player from a comma-separated list.
	void FillUnitDefToDisableFromString(long int playerId, const char *text);
	// Resets and fills the list of researchID to disable for a player from a comma-separated list.
	void FillResearchesToDisableFromString(long int playerId, const char *text);

	// Compute conversion resistance
	float GetConversionResistance(char civId, short int unitClass);

	// Applies an "auto-attack policy" on all player's selected units (only for owned units !)
	// flagsToApply is used to determine which flags have to be updated using values from autoAttackPolicyValues.
	// If flagsToApply.xxx is true, then update unit's auto_attack_policy.xxx to "autoAttackPolicyValues.xxx" value.
	void ApplyAutoAttackPolicyToPlayerSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player, 
		const AutoAttackPolicy &autoAttackPolicyValues, const AutoAttackPolicy &flagsToApply);

	// Returns true if a custom game popup is opened (only for popups, not dialogs !)
	bool HasOpenedCustomGamePopup();
	// Returns true if a custom dialog is opened (only for dialogs, not popups !)
	bool HasOpenedCustomDialog();

	// Opens a custom popup window in game screen/editor. The created popup window is empty !
	// You have to add UI elements afterwards (use GetCustomGamePopup() to get parent object=popup).
	// Return popup UI object if OK, NULL if failed
	// Fails if another game popup (including options) is already open. Fails if dimensions are too small.
	// Pauses the game if running (only if a popup is successfully opened)
	AOE_STRUCTURES::STRUCT_ANY_UI *OpenCustomGamePopup(long int hSize, long int vSize, bool hasCancelBtn = false);

	// Use it to list all UI components (labels, buttons...) that are created(added) to popup content, so they are automatically freed when popup is closed.
	// The method is protected against duplicates, you can safely call it more than once.
	// Returns true if obj has actually been added to list.
	bool AddObjectInPopupContentList(AOE_STRUCTURES::STRUCT_ANY_UI *obj);

private:
	// Free (destroys using AOE destructor) all popup's components
	// The reason we don't free those immediatly is because they might be used in current event (button onclick, etc).
	// Warning: this must NOT be called at any moment, make sure none of the concerned object is currently being used (specially for onclick events...)
	void FreePopupAddedObjects();
	// Free remaining UI components that could not be freed yet
	void FreeGarbagePopupComponents();

public:
	// Use this to force values for "current custom popup". PLEASE AVOID using it !
	// Returns true if successful. Fails if a popup is already open.
	bool ForceSetCurrentGamePopup(AOE_STRUCTURES::STRUCT_ANY_UI *customGamePopup, AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK, AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCancel);
	// To be called when game menu is closed to free custom button
	void ForceClearCustomMenuObjects();

	// Closes currently opened custom popup window in game screen.
	void CloseCustomGamePopup();

	// Return true if provided memory address is our custom game popup OK button (excluding custom options)
	bool IsCustomGamePopupOKButton(unsigned long int UIObjectAddress);
	// Return true if provided memory address is our custom game popup Cancel button (excluding custom options)
	bool IsCustomGamePopupCancelButton(unsigned long int UIObjectAddress);

	// Returns custom popup window in game screen (excluding customROR options popup).
	// Returns NULL if this popup is not open.
	// This information is useful to add UI components to the popup.
	// When adding components, it is not necessary to store component pointers unless we need them to catch events (buttonClick) or get values (input objects)
	AOE_STRUCTURES::STRUCT_ANY_UI *GetCustomGamePopup();

	// Get main (first) selected unit, or NULL if none is selected.
	// Works in-game and in editor.
	// See SelectOneUnit for unit selection + AOE_selectUnit, AOE_clearSelectedUnits
	AOE_STRUCTURES::STRUCT_UNIT_BASE *GetMainSelectedUnit(AOE_STRUCTURES::STRUCT_PLAYER *player);

	// Get relevant "selected units" array pointer according to game EXE status (using custom memory or not ?)
	AOE_STRUCTURES::STRUCT_UNIT **GetRelevantSelectedUnitsPointer(AOE_STRUCTURES::STRUCT_PLAYER *player);
	AOE_STRUCTURES::STRUCT_UNIT_BASE **GetRelevantSelectedUnitsBasePointer(AOE_STRUCTURES::STRUCT_PLAYER *player);

	// Fast-computes the integer distance for X and Y delta values (sqrt(X^2 + Y^2) as an integer).
	// Returns -1 if invalid (diffX and diffY values are capped at CST_INT_DISTANCE_VALUES_MAX_COORDINATE)
	static char GetIntDistance(char diffX, char diffY);

private:
	// Private method: fill a vector from a list of numeric values (short int)
	// Separator is , or ;
	void FillIDVectorFromString(vector<short int> &v, long int playerId, const char *text);

};


/* ----------- GETTERS ------------- */


// Useful to get structure from a unit id. May return NULL !
// Only works for creatable (unitId >= 0). This is just a choice to avoid writing same bugs as ROR
// (some functions use -1 as <No unit> but get an irrevant unit struct then because -1 is not tested before calling getUnitStruct(...))
AOE_STRUCTURES::STRUCT_UNIT *GetUnitStruct(long int unitId);

// Returns a unit definition if valid, NULL otherwise.
AOE_STRUCTURES::STRUCT_DEF_UNIT *GetUnitDefStruct(AOE_STRUCTURES::STRUCT_PLAYER *player, short int unitDefId);

// Securely get an action pointer without having to re-write all checks/gets for intermediate objects.
// Return NULL if one of the objects is NULL/missing
// WARNING: this overload is risky (does not check actual unit structure type / might access wrong pointers !)
AOE_STRUCTURES::STRUCT_ACTION_BASE *GetUnitAction(AOE_STRUCTURES::STRUCT_UNIT *unit);
// Securely get an action pointer without having to re-write all checks/gets for intermediate objects.
// Return NULL if one of the objects is NULL/missing
// Please use THIS overload.
AOE_STRUCTURES::STRUCT_ACTION_BASE *GetUnitAction(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

// Returns true if unit definition is a tower (using unit type and the fact it has attacks or not)
// See also IsTower(datid) in AOE_empires_dat.h, which uses a hardcoded list.
bool IsTower(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef);

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

// Get strategy element type for a unit
AOE_CONST_FUNC::TAIUnitClass GetUnitStrategyElemClass(AOE_STRUCTURES::STRUCT_DEF_UNIT *unitDef);

// Calculate distance
float GetDistance(float x1, float y1, float x2, float y2);


// Returns a weight for a military unit. Scale is totally subjective ! Just a tool for algorithms...
// Super units have a high weight.
// Weak unit have a low weight
int GetUnitWeight(short int DAT_ID);


// Returns a unit DAT_ID2 (including upgrades) given "base" unit ID (DAT_ID1)
// You have to make sure DAT_ID1 is not out of bounds. It depends on empires.dat so it can't be checked with a hardcoded value.
short int GetDAT_ID2(AOE_STRUCTURES::STRUCT_DEF_UNIT **defUnitTable, short int DAT_ID1);

// Returns true if unit class corresponds to one of
// - Artefact/flag
// - Gatherable unit (mine, tree, gazelle - but not other animals, bushes...)
// - Units that can be created by players: buildings, living units
bool IsClassArtefactOrGatherableOrCreatable(GLOBAL_UNIT_AI_TYPES unitClass);

// Returns true if unit class corresponds to units that can be created by players: buildings, living units
bool IsClassPlayerCreatable(GLOBAL_UNIT_AI_TYPES unitClass);

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


// Returns true if a research has been researched for a given player
bool IsTechResearched(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id);
// Returns current status of a research for given player.
AOE_CONST_FUNC::RESEARCH_STATUSES GetResearchStatus(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id);

// Return a list of all unitDefIds that are/can be enabled in player's tech tree.
std::list<long int> GetActivableUnitDefIDs(AOE_STRUCTURES::STRUCT_PLAYER *player);

// Returns true if a unit is idle
bool IsUnitIdle(AOE_STRUCTURES::STRUCT_UNIT *unit);

// Calls AOE's code mainAI.findUnit(DAT_Id)
AOE_STRUCTURES::STRUCT_UNIT_BASE *AOE_MainAI_findUnit(AOE_STRUCTURES::STRUCT_AI *mainAI, long int DAT_ID);

// GetDamage(unitDef attacker, unitDef defender) returns long int
// Call 0043FF10

// Return the total remaining food amount for a farm ("immediatly available" + "action-remaining").
// Returns 0 in error cases (please check it is actually a farm !)
float GetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit);

// Modifies the total remaining food amount for a farm ("immediately available" + "action-remaining").
bool SetFarmCurrentTotalFood(AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farmUnit, float newAmount);

// Returns <> 0 if a unit is (currently) available for a given player. Returns the unit def pointer.
// Does not consider potential researches that could enable the unit afterwards.
// Warning: this only works for non-upgraded unit def IDs. OK for clubman, not for axeman (because axeman's DATID1 is clubman's DATID).
long unsigned int IsUnitAvailableForPlayer(short int DAT_ID, AOE_STRUCTURES::STRUCT_PLAYER *player);

// Searches (at least) a unit with provided shortcut number for given player.
// The first matching unit is returned (arbitrary), even if there are several.
AOE_STRUCTURES::STRUCT_UNIT *FindUnitWithShortcutNumberForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, char shortcutNumber);

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
void MoveUnitToTargetOrPosition(AOE_STRUCTURES::STRUCT_UNIT *unitToMove, AOE_STRUCTURES::STRUCT_UNIT *target, float posX, float posY);

// Tells a unit to (move and) attack another unit (using tacAI)
// Returns true if successful
bool MoveAndAttackTarget(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT *actor, AOE_STRUCTURES::STRUCT_UNIT *target);

// Given a list of (actor) units, tell them to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitsToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT **actorUnitsList, long int actorUnitsCount, AOE_STRUCTURES::STRUCT_UNIT *target);

// Tell a unit to interact with a target unit (like a right-click).
// This can result to an attack action, heal, convert, gather, etc, according to actor/target units.
// Return true if successful (we don't know if the created command makes sense and if it will actually do something)
// Compatible with MP games (uses "command" interface)
bool TellUnitToInteractWithTarget(AOE_STRUCTURES::STRUCT_UNIT *actorUnit, AOE_STRUCTURES::STRUCT_UNIT *target);

// Returns a unitDefCommand object if actor unit has a valid right-click command on target unit.
// Returns NULL if there no possible interaction
AOE_STRUCTURES::STRUCT_DEF_UNIT_COMMAND *GetUnitDefCommandForTarget(AOE_STRUCTURES::STRUCT_UNIT *actorUnit, 
	AOE_STRUCTURES::STRUCT_UNIT *target, bool canSwitchForVillager);


// Creates a unit at provided location. Does NOT make checks on location, please first make sure GetErrorForUnitCreationAtLocation returns 0.
// You can use 0 as posZ value.
// Returns NULL if it failed
AOE_STRUCTURES::STRUCT_UNIT *CreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID, float posY, float posX, float posZ);

// Creates a unit at provided location only if GetErrorForUnitCreationAtLocation agrees !
// Returns NULL if it failed
AOE_STRUCTURES::STRUCT_UNIT *CheckAndCreateUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_DEF_UNIT *unitDef,
	float posY, float posX, bool checkVisibility, bool checkHills, bool checkConflictingUnits);

// Has the same effect as "CTRL-0" or "CTRL-1" etc: assigns a shortcut number to units (and removes this shortcut from old units that had it)
// Returns 1 on success.
long int AssignShortcutToSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player, long int shortcutNumber);

// Selects units that have a given shortcut number.
long int SelectUnitsUsingShortcut(AOE_STRUCTURES::STRUCT_PLAYER *player, long int shortcutNumber, bool addToSelection = false);

// Apply a research for a player (if possible)
bool ApplyResearchForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id);
bool ApplyResearchForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int research_id);

// Enable or disable a research for a player
void AOE_enableResearch(AOE_STRUCTURES::STRUCT_PLAYER *player, long int research_id, bool enable);

// Disable the research that enables a given unit, so that it can't be trained.
// For units that are enabled by a "visible" research, like short sword, the research won't be visible (disabled).
// To be called before game starts
// Returns true if a research was found and disabled.
bool DisableUnitForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int DAT_ID);

// Apply a technology for a player. To be used only when there is no research for given technology (like cheat techs).
// In most cases, please use ApplyResearchForPlayer instead.
bool ApplyTechnologyForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int tech_id);
bool ApplyTechnologyForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int tech_id);

// Calls AOE's method to change a unit owner. Warning, this has bugs, see customROR / crCommand.
void AOE_ChangeUnitOwner(AOE_STRUCTURES::STRUCT_UNIT *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer);

// Call AOE's Notify event method. Warning, the parameters can have different types.
// Use the overload with pointers to make sure you don't have cast issues.
void AOE_callNotifyEvent(long int eventId, long int playerId, long int variant_arg3, long int variant_arg4, long int variant_arg5);
// Call AOE's Notify event method. Warning, the parameters can have different types. Here pointers here so there is no undesired cast !
void AOE_callNotifyEvent(long int eventId, long int playerId, void *variant_arg3, void *variant_arg4, void *variant_arg5);

void AOE_clearSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player);
// select: if true, add unit to selection. If false, remove from selection.
bool AOE_selectUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT *unit, bool select);

// Calls AOE's path finding method.
// allArgs indices are 1-15 (do NOT use 0). Warning, some arguments type are NOT long int (there are floats).
long int callFindPathForUnit(long int allArgs[15 + 1]);

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
void AOE_playerBldHeader_RemoveBldFromArrays(AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader, AOE_STRUCTURES::STRUCT_UNIT *unit);

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
bool CreateCmd_RightClick(AOE_STRUCTURES::STRUCT_UNIT **actorUnitsList, long int actorUnitsCount, long int targetUnitId, float posX, float posY);

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


// ---------- Unit def

// Create a new unitDef, copied for provided one, using actual derived class.
// Returns a STRUCT_UNITDEF_BASE, but it can be any derived class (living, building, etc)
AOE_STRUCTURES::STRUCT_UNITDEF_BASE *CopyUnitDefToNewUsingGoodClass(AOE_STRUCTURES::STRUCT_UNITDEF_BASE *existingUnitDef);

// Extends a player's unitDef table to add a new one (unitDef).
// unitDef's DAT_ID1 and DAT_ID2 are modified with new ID.
// Returns -1 on failure, new unitDefId on success.
short int AddUnitDefToPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef);

// A constructor for unitDef that copies an existing one.
template<typename UnitDef> static UnitDef *CopyUnitDefToNew(UnitDef *existingUnitDef) {
	if (!existingUnitDef || !existingUnitDef->IsCheckSumValid()) {
		return NULL;
	}
	UnitDef *newUnitDef = (UnitDef *)AOEAlloc(sizeof(UnitDef));
	if (!newUnitDef) {
		return newUnitDef;
	}
	unsigned long int addr = existingUnitDef->GetCopyConstructorAddress();
	if (!addr) {
		return NULL;
	}
	_asm {
		MOV ECX, newUnitDef;
		PUSH 1;
		PUSH existingUnitDef;
		CALL addr
	}
	return newUnitDef;
}

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


// Returns the icon id relevant for provided UI command id, if found.
// Returns -1 if not found.
long int GuessIconIdFromUICommandId(AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID UICmdId);

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


// To customize for debug purpose...
void DebugDumpAllUnits();

// Writes in a text buffer the content of a "temp map like info" zone.
char *DumpPosToTextBuffer(AOE_STRUCTURES::STRUCT_TEMP_MAP_BUILD_LIKE_INFOS *mapInfosStruct, long int posX, long int posY, int radius);

#ifdef _DEBUG
static const char serializationLogFilename[] = "D:\\AOESrlz.txt";
extern bool debugSerialization;
// Write deserialization data into a buffer, then to a log file.
// This affects greatly performance !!! Debug only.
void WriteDebugLogForDeserializedData(unsigned long int callAddr, unsigned char *buffer, long int bufferSize);
#endif

// Automatically detects issues in empires.dat (requires to have been loaded already) and writes logs about errors in trace message handler.
bool AnalyzeEmpiresDatQuality();
