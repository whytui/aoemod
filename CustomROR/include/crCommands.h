#include <string>
#include <Windows.h>
#include <time.h>
#include <vector>
#include <list>
#include <algorithm>
#include <mystrings.h>

#include <AOE_const_language.h>
#include <ROR_struct_export.h>
#include <AOE_SN_numbers.h>
#include <AOE_empires_dat.h>
#include "Version.h"
#include "crCommon.h"
#include "UI_utilities.h"
#include "crTrigger.h"
#include "crTriggerSet.h"
#include "strategy.h"
#include "interface.h"
#include "EXESelfEditor.h"
#include "crPatcher.h"
#include "unitShortcuts.h"
#include "researches.h"
#include "crLocalization.h"

#pragma once

#define CRCMD_TEXT_BUFFER_SIZE 256


// TEMPORARY
#ifdef _DEBUG
static bool _DEBUG_improveThisPlayer(int playerId) {
	bool applyFilter = false; // TRUE = DEBUG, false = normal

	static bool msgDone = false;
	if (!msgDone) {
		msgDone = true;
		if (applyFilter)
			traceMessageHandler.WriteMessage("Using DEBUG AI improvement filter");
	}
	if (applyFilter) {
		// Filter clause (DEBUG)
		return playerId >= 5;
	}

	// Normal: no filter
	return true;
}
#endif




enum CUSTOM_GAME_POPUP_TYPES {
	CGPT_NONE = -1,
	CGPT_UNKNOWN = 0, // Can be used for popups that have no special treatment (no need to handle button actions, collect input, etc: just an informative popup)
	CGPT_EDITOR_EDIT_UNIT,
	CGPT_LARGE_EDIT_TEXT, // A popup with a large textarea
	CGPT_SCENARIO_INFO,
	CGPT_EDIT_XY_MAP_SIZE,
	CGPT_EDIT_TERRAIN
};


/* This class manages commands/actions to be executed by CustomROR plugin.
It contains all main "functional" methods whose behaviour depends on CustomROR configuration/context.
"static" common methods (whose behaviour does not depend on any configuraiton, eg. get a unit struct from its ID) do NOT belong to this class, see crCommon.h.
*/

class CustomRORCommand {
public:
	CustomRORCommand();
	CustomRORCommand(CustomRORInfo *crInfo);
	~CustomRORCommand();

	CustomRORInfo *crInfo;

private:
	const float distanceToConsiderVeryClose = 1.0; // Distance (added to range) under which a target is considered "within range". Please leave this > 0.

	// UpdatedValue: if <0, it is ignored
	void UpdateWorkRateWithMessage(short int DATID, float updatedValue);

	// Update a technology that increases work rate: change value used to increase.
	// UpdatedValue: if <0, it is ignored
	void UpdateTechAddWorkRateWithMessage(short int techId, short int unitDefId, float updatedValue);

public:
	// Returns true if all features for customROR are enabled in game executable code.
	bool CheckEnabledFeatures();

	// Initialization that only takes place once, at executable startup.
	void OneShotInit();

	// Reads game executable to determine if player struct is extended to use custom memory zone to host selected units
	void ReadIfCustomSelectedUnitsMemoryZoneIsUsed();
	// Reads game executable to determine if player struct is extended to use custom memory zone to host selected units
	void ReadIfManageAIIsOn();

	// Reads game executable to determine if various sequences are installed or not
	void ReadOtherSequencesStatus();

	// Load custom strings files
	void LoadCustomLocalizationFiles();

	// Load custom DRS files
	void LoadCustomDrsFiles();

	// Execute a command from custom options window
	// Returns true if the provided command is valid (and executed)
	// After the call, output points to a char* string containing command's output, if any.
	// Do NOT free output variable, it uses an internal buffer.
	bool ExecuteCommand(char *command, char **output);
	void HandleChatCommand(char *command);

	void ShowF11_zone();

	// This is called just after empires.dat is loaded.
	// Warning: changes here are applied on civ definitions are done once and for all, and impact all games.
	void OnAfterLoadEmpires_DAT();

	// Called when launching a new game. Game info has NOT been loaded yet. Global structure already exists, but be careful with its data...
	// Most structures have not been initialized/loaded yet.
	// This method should only reset internal data
	void OnNewGame(bool isSavedGame);

	// Called on every game start, including RM, DM, campaign/scenario, load game, and even MP games.
	// Warning: for scenarios, if there is an introduction screen, this method is called at that moment
	// (game might not be displayed yet)
	void OnGameStart();

	// This is called for each player at game initialization, after applying tech trees and starting age.
	// This is called for all game types (SP / MP, RM/DM/scenario) but NOT for load game.
	void OnGameInitAfterApplyingTechTrees(long int playerId);

	// Apply starting age to a player (only for scenarios)
	void ApplyScenarioSpecificPlayerStartingAge(long int playerId);


	// Does all custom stuff on random maps / deathmatches before game start : changes on game settings (map type/size, etc)
	// Does NOT apply to scenario/campaign/load saved game.
	// Warning: most game structures are not available yet ! It is recommended to only work with STRUCT_GAME_SETTINGS, nothing else.
	// Return false if failed.
	bool ApplyCustomizationOnRandomGameSettings();

	// Does all custom stuff on random maps / deathmatches at game start : custom personnality values, strategy, etc.
	// These are changes that are applied when game is loaded (do not interfere here with settings like map size, etc)
	// Does NOT apply to scenario/campaign/load saved game.
	// Return false if failed.
	bool ApplyCustomizationOnRandomGameStart();

	// Set a SN number value in both strategy AI and tac AI.
	void SetSNNumberInStrategyAndTacAI(ROR_STRUCTURES_10C::STRUCT_AI *ai, AOE_CONST_FUNC::SN_NUMBERS snNumber, long int value);

	// This is called while scenarioInfo structure is read from a file
	// Scenario texts (including players strategies) havealready been read, but not other information, be careful.
	void InitScenarioInfoTextData(ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *scenarioInfo);

	// Initialize internal game-specific variables (to call on each game start/load)
	void InitMyGameInfo();

	// Use this API to get "do we improve AI" information. This takes care of customROR configuration + possible custom rules.
	// So we can add custom rules for debugging
	bool IsImproveAIEnabled(int playerId);

	// Disable AI flags for human players, based on game initial settings (to be used at game startup)
	void DisableAIFlagsForHuman();
	// Restore AI flags based on human-controlled playerID (to be used in SP games only)
	void RestoreAllAIFlags();
	void SetAllAIFlags();
	// Change human control to another player and set AI flags accordingly (if updateAIFlags is true)
	void ChangeControlledPlayer(int playerId, bool updateAIFlags);
	// Call this when changing a player from "AI control" disabled to enabled
	// This will run various actions to fix strategy, etc (example: do not build buildings human already built).
	// The method will do nothing if player is NULL or if its AI structure is NULL.
	void CheckAIWhenEnablingAIControl(int playerId);
	void CheckAIWhenEnablingAIControl(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

	// This fixes nextStrategyAIExecutionCounter flag for all players (useful for loaded games)
	void FixGameStartAIInitForPlayers();

	// Process event when (human) player sees a unit. This is where gaia units are captured by human player.
	// Return true if the unit must NOT be captured
	bool HumanSpecific_onCapturableUnitSeen(ROR_STRUCTURES_10C::STRUCT_UNIT *beingSeenUnit, ROR_STRUCTURES_10C::STRUCT_PLAYER *actorPlayer);

	// Detects if game start strategy init has been done because nextStrategyAIExecutionCounter can be wrong with saved games
	// (savegame file format does NOT save this information).
	// Returns true if we think this init has been done. False otherwise.
	// Warning: this initialization adds resources (SN numbers 8A-8D) to the player, so it must not be run several times !
	bool FindIfGameStartStrategyInitHasBeenDone(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

	// Displays system date/time in "in-game" chat
	void PrintDateTime();
	// Displays game map seed (from game settings) in "in-game" chat
	void PrintMapSeed();
	// Dumps useful information for debugging.
	void DumpDebugInfoToFile();

	// Compute conversion resistance
	float GetConversionResistance(char civId, short int unitClass);

	// Returns how many units where told to move.
	// If maxDistance > 0, only units at a maximum distance of maxDistance are told to move.
	// Returns <0 if there is an error
	int MoveIdleMilitaryUnitsToMousePosition(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, float maxDistance = 0);

	// Searches all idle units in a specified range (see config) and order them to come at screen location
	// Requires ManageAI !
	void CallNearbyIdleMilitaryUnits();

	// Select next idle military unit for current player
	void SelectNextIdleMilitaryUnit();

	// Main method to manage "OnCreate" event for unit.
	// This event occurs after the unit is created and added in player/global structs/lists.
	// This event is triggered for ALL units (creatable or not, projectile, smoke, trees...)
	// This event is triggered during game but as well in scenario editor or during game creation.
	// actionStruct parameter can be NULL if it could not be determined
	void OnLivingUnitCreation(AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS UIStatus, ROR_STRUCTURES_10C::STRUCT_UNIT *unit,
		ROR_STRUCTURES_10C::STRUCT_ACTION_MAKE_OBJECT *actionStruct);

	// Fixes missing treatments when a unit changes owner (eg. unit conversion)
	// This should be called before the actual unit owner change process is called.
	// targetUnit is the "victim" (unit that changes owner), actorPlayer is the new owner (player)
	// Technical note: in ROR, unit.changeOwner(newplayer) is [EDX+0x44] call.
	void OnUnitChangeOwner_fixes(ROR_STRUCTURES_10C::STRUCT_UNIT *targetUnit, ROR_STRUCTURES_10C::STRUCT_PLAYER *actorPlayer);

	// Change a unit's owner, for example like a conversion.
	// I don't see any other possible event than CST_ATI_CONVERT. Use CST_GET_INVALID to trigger NO notification.
	bool ChangeUnitOwner(ROR_STRUCTURES_10C::STRUCT_UNIT *targetUnit, ROR_STRUCTURES_10C::STRUCT_PLAYER *actorPlayer, AOE_CONST_INTERNAL::GAME_EVENT_TYPES notifyEvent = AOE_CONST_INTERNAL::CST_GET_INVALID);

	// Custom Fixes/features on player.addUnit calls.
	void OnPlayerAddUnitCustomTreatments(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, ROR_STRUCTURES_10C::STRUCT_UNIT *unit, bool isTempUnit, bool isNotCreatable);

	// Custom Fixes/features on player.removeUnit calls.
	void OnPlayerRemoveUnit(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, ROR_STRUCTURES_10C::STRUCT_UNIT *unit, bool isTempUnit, bool isNotCreatable);

	// Returns true if a shortcut has been added/modified
	bool AutoAssignShortcutToUnit(ROR_STRUCTURES_10C::STRUCT_UNIT *unit);

	// Returns true if a unit should change target to new one, false if it should keep attacking current one.
	// This choice is very important, unit must NOT change its mind on each call because it is called too often and unit just attacks none of its "attackers"
	// Which is a common issue in original game (example: 2-priest trick to safely convert an enemy)
	bool ShouldChangeTarget(ROR_STRUCTURES_10C::STRUCT_UNIT_ACTIVITY *activity, long int targetUnitId);

	// Returns true if a unit should change target to new one, false if it should keep attacking current one.
	// To be used when target unit is a tower in actor's town
	bool ShouldAttackTower_towerPanic(ROR_STRUCTURES_10C::STRUCT_UNIT *actorUnit, ROR_STRUCTURES_10C::STRUCT_UNIT *enemyTower);

	// Replaces ROR's loop on villagers to pick some and task them to attack enemy tower.
	// assignedUnitsCounter is IN OUT: input value can be >0 (number of military units that have already been tasked). We should stop when assignedUnitsCounter reaches 6 (original code's behaviour)
	void towerPanic_LoopOnVillagers(ROR_STRUCTURES_10C::STRUCT_TAC_AI *tacAI, ROR_STRUCTURES_10C::STRUCT_UNIT *enemyTower, long int *pAssignedUnitsCount, ROR_STRUCTURES_10C::STRUCT_POSITION_INFO *pTCPositionInfo);

	// If fire galley iconId conflicts with catapult trireme, this will move it to a free location (trireme tech because now it's free)
	void MoveFireGalleyIconIfNeeded(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);
	void MoveFireGalleyIconIfNeeded(short int playerId);

	// Duplicates an existing unit definition (srcDAT_ID) into a new unit definition for given player.
	// New unitDef will be available in player's list. Everything is managed so that the game will automatically free everything correctly.
	// You can send "" as name (in that case original unit's name will be used). Max length = 0x2F.
	// Returns the new DAT_ID if successful, -1 otherwise.
	// It is not recommended to call this too much ! It would add a lot of unit definition and would impact seriously game performance.
	short int DuplicateUnitDefinitionForPlayer(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, short int srcDAT_ID, char *name);


	// Analyze strategy and fixes what's necessary. Called every <crInfo.configInfo.tacticalAIUpdateDelay> seconds.
	void AnalyzeStrategy(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

	// Overloads the "tactical AI update" event that occurs regularly for each AI player.
	// For the algorithm to work well, requires also "FixUnitIdForInProgressBuilding", "FixResetStratElemForUnitId"
	void ManageTacAIUpdate(ROR_STRUCTURES_10C::STRUCT_AI *ai);

	// Returns true if a construction should NOT be triggered.
	// Default result is false, this returns true only for specific (custom) cases.
	bool ShouldNotTriggerConstruction(ROR_STRUCTURES_10C::STRUCT_TAC_AI *tacAI, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *stratElem);

	// Calls appropriate "manage panic mode" treatments.
	// Returns true if we want to force usage of original ROR's (bugged) code
	// In most cases, this returns true and ROR's code is not used.
	bool RunManagePanicMode_isUsageOfRORCodeWanted(ROR_STRUCTURES_10C::STRUCT_AI *mainAI, long int enemyPlayerId, long int timeSinceLastPanicMode_s, long int currentGameTime_ms);

	// Manage strategy updates for panic mode.
	void ManagePanicMode(ROR_STRUCTURES_10C::STRUCT_AI *mainAI, long int enemyPlayerId, long int timeSinceLastPanicMode_s, long int currentGameTime_ms);

	void ComputeDislikeValues();

	// Technical fix for a method about elevation application when generating map. Original method contains many bugs.
	void Fixed_MapGen_applyElevation(long int posX, long int posY, long int distance, ROR_STRUCTURES_10C::STRUCT_MAPGEN_ELEVATION_INFO *elevInfo);

	void DisplayTimerStats();
	void AutoFixGameTimer();

	// Generate map (overload) for scenario editor
	bool ScenarioEditor_customGenerateMap(long int sizeX, long int sizeY);

	// Handles all custom treatments at scenario editor UI creation (this is called at the end of standard UI creation).
	void CustomScenarioEditorUICreation(ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor);

	// This methods modifies provided variables that will be passed to "check unit placement" method before adding a unit in scenario editor.
	// For example, to allow creating several units at the same location, force checkConflictingUnits to 0.
	void ApplyCustomizationOnEditorAddUnit(long int &checkVisibility, long int &checkHillMode, long int &editorMode, long int &checkAirModeAndHPBar, long int &checkConflictingUnits, bool &IgnoreTerrainRestrictions);

	// Get matching terrainId when user selected a row in terrain list in scenario editor/terrain tab.
	// Returns a terrainId, returns -1 if we want to use default behaviour (no custom choice)
	long int GetTerrainIdForSelectedTerrainIndex(ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor, long int selectedIndex);

	// Handle a ROR's original code call to trace method (0x42C330)
	// firstRORCallTextParamAddress is the address in stack of the text argument "xxx".
	// Next potential arguments (%xxx) are next data in stack.
	void HandleRORDebugLogCall(unsigned long int firstRORCallTextParamAddress);

	// Fix the check on path finding when trying to assign a task to a villager
	long int GathererCheckPathFinding(ROR_STRUCTURES_10C::STRUCT_UNIT_TYPE50 *actorAsType50, long int *pathFindingArgs);

	// Write the F11 centered text (if displayed)
	// Warning, this is only refreshed when population changes ?
	void WriteF11PopInfoText(ROR_STRUCTURES_10C::STRUCT_UI_F11_POP_PANEL *f11panel, char *bufferToWrite, char *defaultFormat,
		char *localizedText, long int currentPop, long int houseMaxPop);

	// Handles the event "farm is depleted". NOT called when a farm is destroyed/killed.
	// Warning: this event occurs before the farm unit is actually "killed"
	void OnFarmDepleted(long int farmUnitId);

	// Disable dock for all players on maps where AI does NOT builds docks.
	void DisableWaterUnitsIfNeeded();

	// Disable all walls for current game.
	void DisableWalls();

	// Called on each loop in infAI.FindEnemyUnitIdWithinRange(ptrMyReferenceUnit, maxDistance, DATID, DATID, DATID, DATID)
	void OnFindEnemyUnitIdWithinRangeLoop(ROR_STRUCTURES_10C::STRUCT_INF_AI *infAI, ROR_STRUCTURES_10C::STRUCT_INF_AI_UNIT_LIST_ELEM *currentUnitListElem);

	// Called at the end of showUnitCommandButtons
	void AfterShowUnitCommandButtons(ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *gameMainUI);

	// Called when a game UI command button is clicked.
	// Returns true if event has been handled and must NOT be handle by ROR standard code.
	// Returns false by default (most cases) !
	bool OnGameCommandButtonClick(ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
		AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID uiCommandId, long int infoValue);

	// Refresh status for custom auto-attack policy buttons
	void RefreshCustomAutoAttackButtons(ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *gameMainUI, const AutoAttackPolicy *attackPolicy);

	// Custom treatment to decide if a potential target unit should be ignored
	// Overload standard rules for catapults(ignores building if HP=1) and "target=wall" cases.
	// Default result=false (on error cases)
	bool AutoSearchTargetShouldIgnoreUnit(ROR_STRUCTURES_10C::STRUCT_UNIT_ACTIVITY *activity,
		ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *potentialTargetUnit);

	// Manages the display of a unit shortcut for non-standard shortcuts.
	// Returns true if we want to let standard game code execute the shortcut display operation. (default false)
	bool DisplayCustomUnitShortcutSymbol(ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *unitBase,
		long int posX, long int posY, long int unknown_arg3);

	// Adds custom attributes (armor) in buildings' unit info zone.
	// currentLine is incremented if lines are added.
	void DisplayCustomBuildingAttributesInUnitInfo(ROR_STRUCTURES_10C::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone, long int &currentLine);

	// Get a localized string to overload ROR's string localization system (language(x).dll)
	// Returns true if a (custom) localized string has been written into buffer.
	bool GetLocalizedString(long int stringId, char *buffer, long int bufferSize);


	// ----------------------------------
	// City plan

	// Computes (existing) building influence zone for farm placement map like values computation.
	// Updates existingBldInfluenceZone with the influence distance we want to use for provided building (positions near building will be preferred)
	// Updates skipThisBuilding to true if the building must be ignored for farm position computation. If true, existingBldInfluenceZone returned value should be ignored.
	void FixCityPlanFarmPlacement(ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *existingBuilding, long int &existingBldInfluenceZone, bool &skipThisBuilding);

	// Updates temp map like data for choosing a new building location according to existing buildings
	void ManageCityPlanOtherBuildingsImpact(ROR_STRUCTURES_10C::STRUCT_INF_AI *infAI, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *stratElem, ROR_STRUCTURES_10C::STRUCT_POSITION_INFO *TCPosition);

	// ----------------------------------
	// Methods about internal UI framework

	// Opens a custom dialog message (based on CloseProgramDialog)
	// Return true if OK, false if failed
	// Fails if another custom dialog is already open
	bool OpenCustomDialogMessage(const char *dialogText, long int hSize, long int vSize);

	// Closes currently opened custom dialog message.
	// Returns -1 if an error occurred, including "no custom dialog is opened".
	// Other results are 0=Yes/OK, 1=No, 2=Cancel
	long int CloseCustomDialogMessage(ROR_STRUCTURES_10C::STRUCT_UI_POPUP_DIALOG *ptrDialog, unsigned long int ptrSender);


	// ---------------------------------
	// Some Entry points / triggers

	// This is called on GameSettings.NotifyEvent game method call.
	void EntryPoint_GameSettingsNotifyEvent(long int eventId, short int playerId, long int arg3, long int arg4, long int arg5);

	// This is dedicated to triggers handling on "GameSettingsNotifyEvent" event.
	void ManageTriggersOnGameNotifyEvent(long int eventId, short int playerId, long int arg3, long int arg4, long int arg5);

	// Entry point to make custom treatments at "disable research" init at game start (for scenarios)
	// This is only executed for scenarios, not DM/RM !
	void OnGameInitDisableResearchesEvent(ROR_STRUCTURES_10C::STRUCT_PLAYER_RESEARCH_INFO *playerResearchInfo);

	// Manage disable (via trigger) units for villager, house, fishing ship, trade ship, farms
	// This method should be called at game start, after "initial" technologies have been applied, so we can override some (and force to disable again some units)
	void ManageTriggerDisableUnitsForExceptions();
	// Manage disable (via trigger) units for farms
	// Farm is enabled by market construction, which may occur both "ingame" or at game loading (if a market exists, or if starting at bronze).
	void ManageDisableUnitsForFarms(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

	// Triggers
	void ReloadTriggersFromGameData();
	void ReloadTriggersFromGameData(ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *scenarioInfo);
	// Write trigger to game data, using customROR internal data.
	bool WriteTriggersFromInternalToGameData(bool onlyEnabledTriggers);
	// Write trigger to game data, using provided text trigger data.
	bool WriteTriggersInGameData(char *triggerText);
	// Export CustomROR's internal data triggers to Trigger text format.
	std::string GetTriggerTextFromInternalData();

	// Manage triggers execution for a specific event
	void ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES evt);
	void ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES evt, CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER &evtInfo);

	// Execute a trigger (considering requirements are OK)
	void ExecuteTriggerAction(CR_TRIGGERS::crTrigger *trigger);
	private:
	// Just execute a "standard" trigger. this only does the action part, does not check and does not manage other impacts.
	// Do not call directly.
	void Trigger_JustDoAction(CR_TRIGGERS::crTrigger *trigger);

	public:

};


