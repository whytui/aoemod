#include <string>
#include <Windows.h>
#include <time.h>
#include <vector>
#include <list>
#include <algorithm>
#include <macroUtil.h>
#include <mystrings.h>

#include <AOE_const_language.h>
#include <AOE_const_internal.h>
#include <AOE_const_game_events.h>
#include <AOE_struct_export.h>
#include <AOE_SN_numbers.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <UI\AOE_struct_ui_popup_dialog.h>
#include <AOE_hardcoded_unit_names.h>
#include "Version.h"
#include "AOE_map.h"
#include "RockNRorCommon.h"
#include "RockNRorInfo.h"
#include "UI_utilities.h"
#include "crTrigger.h"
#include "crTriggerSet.h"
#include "AOE_strategy.h"
#include "interface.h"
#include "EXESelfEditor.h"
#include "crPatcher.h"
#include "unitShortcuts.h"
#include "researches.h"
#include "RockNRorLocalization.h"
#include "StrategyBuilder.h"
#include "basicFilesHandling.h"
#include "crMapCopier.h"
#include "crRPGMode.h"
#include "buttonBar.h"
#include "playerTargeting.h"
#include "crDebugOutputPrimitives.h"
#include "AOEPrimitives_UI_gameMain.h"
#include "StrategyUpdater.h"
#include "cityPlan.h"
#include "unitTargeting.h"
#include "playerTargeting.h"
#include "combatAI.h"
#include "mainCustomAI.h"
#include "customAIMilitaryInfo.h"
#include "autoRebuildFarmConfig.h"
#include "UnitCustomMethods.h"
#include "GameTriggerHandling.h"
#include "PlayerCustomMethods.h"
#include "SimpleEditText.h"
#include "UnitExtensionHandler.h"
#include "interruption.h"
#include "CustomCiv.h"
#include "TechTreeAnalyzer.h"
#include "playerHandling.h"

#pragma once


namespace ROCKNROR {



enum CUSTOM_GAME_POPUP_TYPES {
	CGPT_NONE = -1,
	CGPT_UNKNOWN = 0, // Can be used for popups that have no special treatment (no need to handle button actions, collect input, etc: just an informative popup)
	CGPT_EDITOR_EDIT_UNIT,
	CGPT_LARGE_EDIT_TEXT, // A popup with a large textarea
	CGPT_SCENARIO_INFO,
	CGPT_EDIT_XY_MAP_SIZE,
	CGPT_EDIT_TERRAIN
};


/* This class manages commands/actions to be executed by RockNRor plugin.
It contains all main "functional" methods whose behaviour depends on RockNRor configuration/context.
"static" common methods (whose behaviour does not depend on any configuration, eg. get a unit struct from its ID) do NOT belong to this class, see crCommon.h.
*/

class RockNRorCommand {
public:
	RockNRorCommand();
	~RockNRorCommand();

private:
	// Handler that contains information about current players civilization data (available tech tree...)
	ROCKNROR::CUSTOMCIV::CustomCivHandler customCivHandler;

	// UpdatedValue: if <0, it is ignored
	void UpdateWorkRateWithMessage(short int DATID, float updatedValue);

	// Update a technology that increases work rate: change value used to increase.
	// UpdatedValue: if <0, it is ignored
	void UpdateTechAddWorkRateWithMessage(short int techId, short int unitDefId, float updatedValue);

public:
	// Empties RockNRor log file
	void ResetLogFile();

	// Returns true if all features for RockNRor are enabled in game executable code.
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

	// Get custom empires.dat filename (with relative path)
	const char *GetCustomEmpiresDatRelativeFileName(AOE_STRUCTURES::STRUCT_COMMAND_LINE_INFO *cmdLineInfo);

	// Execute a command from custom options window
	// Returns true if the provided command is valid (and executed)
	// After the call, output is a string containing command's output, if any.
	bool ExecuteCommand(char *command, std::string &output);
	void HandleChatCommand(char *command);


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
	// This method is called but does no customization for scenario/campaign/load saved game.
	// Warning: most game structures are not available yet ! It is recommended to only work with STRUCT_GAME_SETTINGS, nothing else.
	// However GLOBAL structure (always) already exists and DAT file HAS (always) already been loaded into internal data.
	// Return false if failed.
	bool ApplyCustomizationOnRandomGameSettings();

	// Does all custom stuff on random maps / deathmatches at game start : custom personnality values, strategy, etc.
	// These are changes that are applied when game is loaded (do not interfere here with settings like map size, etc)
	// Does NOT apply to scenario/campaign/load saved game.
	// Note: when this is called, TECH tree/initial age researches have ALREADY been applied.
	// Return false if failed.
	bool ApplyCustomizationOnRandomGameStart();


	// Initialize internal game-specific variables (to call on each game start/load)
	void InitMyGameInfo();


	// This fixes needGameStartAIInit flag for all players (useful for loaded games)
	void FixGameStartAIInitForPlayers();

	// Returns true if AI file selection was overriden (do NOT let normal code be executed), false to let normal code be executed
	bool ManageAIFileSelectionForPlayer(char civilizationId, char *aiFileBuffer);

	// Process event when (human) player sees a unit. This is where gaia units are captured by human player.
	// Return true if the unit must NOT be captured
	bool HumanSpecific_onCapturableUnitSeen(AOE_STRUCTURES::STRUCT_UNIT_BASE *beingSeenUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer);

	// Detects if game start strategy init has been done because nextStrategyAIExecutionCounter can be wrong with saved games
	// (savegame file format does NOT save this information).
	// Returns true if we think this init has been done. False otherwise.
	// Warning: this initialization adds resources (SN numbers 8A-8D) to the player, so it must not be run several times !
	bool FindIfGameStartStrategyInitHasBeenDone(AOE_STRUCTURES::STRUCT_PLAYER *player);

	// Displays system date/time in "in-game" chat
	void PrintDateTime();
	// Displays game map seed (from game settings) in "in-game" chat
	void PrintMapSeed();

	// This is called very frequently when game is running. Not called if game is paused or stuck.
	void OnGameTimer();

	// Main method to manage "OnCreate" event for unit.
	// This event occurs after the unit is created and added in player/global structs/lists.
	// This event is triggered for ALL units (creatable or not, projectile, smoke, trees...)
	// This event is triggered during game but as well in scenario editor or during game creation.
	// actionStruct parameter can be NULL if it could not be determined
	void OnLivingUnitCreation(AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS UIStatus, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit,
		AOE_STRUCTURES::STRUCT_ACTION_MAKE_OBJECT *actionStruct);

	// Fixes missing treatments when a unit changes owner (eg. unit conversion)
	// This should be called before the actual unit owner change process is called.
	// targetUnit is the "victim" (unit that changes owner), actorPlayer is the new owner (player)
	// Technical note: in ROR, unit.changeOwner(newplayer) is [EDX+0x44] call.
	void OnUnitChangeOwner_fixes(AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer);

	// Change a unit's owner, for example like a conversion.
	// I don't see any other possible event than CST_ATI_CONVERT. Use CST_GET_INVALID to trigger NO notification.
	bool ChangeUnitOwner(AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer, AOE_CONST_INTERNAL::GAME_EVENT_TYPE notifyEvent = GAME_EVENT_TYPE::EVENT_INVALID);

	// Custom Fixes/features on player.addUnit calls.
	// player is the NEW owner (for conversion case) and unit->ptrStructPlayer is (still) the OLD owner (will be updated afterwards by ROR)
	// Note for conversion : unit is currently (temporarily) unavailable in global->GetUnitStruct(unitId) !
	void OnPlayerAddUnitCustomTreatments(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, bool isTempUnit, bool isNotCreatable);

	// Custom Fixes/features on player.removeUnit calls.
	void OnPlayerRemoveUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, bool isTempUnit, bool isNotCreatable);

	// Returns true if a unit should change target to new one, false if it should keep attacking current one.
	// To be used when target unit is a tower in actor's town
	bool ShouldAttackTower_towerPanic(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actorUnit, AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyTower);

	// Replaces ROR's loop on villagers to pick some and task them to attack enemy tower.
	// assignedUnitsCounter is IN OUT: input value can be >0 (number of military units that have already been tasked). We should stop when assignedUnitsCounter reaches 6 (original code's behaviour)
	void towerPanic_LoopOnVillagers(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyTower, long int *pAssignedUnitsCount, AOE_STRUCTURES::STRUCT_POSITION_INFO *pTCPositionInfo);

	// If fire galley iconId conflicts with catapult trireme, this will move it to a free location (trireme tech because now it's free)
	void MoveFireGalleyIconIfNeeded(AOE_STRUCTURES::STRUCT_PLAYER *player);
	void MoveFireGalleyIconIfNeeded(short int playerId);


	// Entry point aftre a strategy element has been added in buildAI
	void AfterAddElementInStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *posToAdd,
		int countAdded);

	// Overloads the "tactical AI update" event that occurs regularly for each AI player.
	// For the algorithm to work well, requires also "FixUnitIdForInProgressBuilding", "FixResetStratElemForUnitId"
	// Returns false is the player is dead/invalid. Returns true in most cases.
	bool ManageTacAIUpdate(AOE_STRUCTURES::STRUCT_AI *ai);


	// Technical fix for a method about elevation application when generating map. Original method contains many bugs.
	void Fixed_MapGen_applyElevation(long int posX, long int posY, long int distance, AOE_STRUCTURES::STRUCT_RMM_ELEVATION_GENERATOR *elevInfo);

	void DisplayTimerStats();
	void AutoFixGameTimer();

	// Generate map (overload) for scenario editor
	bool ScenarioEditor_customGenerateMap(long int sizeX, long int sizeY);

	// Handles all custom treatments at scenario editor UI creation (this is called at the end of standard UI creation).
	void CustomScenarioEditorUICreation(AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor);

	// This methods modifies provided variables that will be passed to "check unit placement" method before adding a unit in scenario editor.
	// For example, to allow creating several units at the same location, force checkConflictingUnits to 0.
	void ApplyCustomizationOnEditorAddUnit(long int &checkVisibility, long int &checkHillMode, long int &editorMode, long int &checkAirModeAndHPBar, long int &checkConflictingUnits, bool &IgnoreTerrainRestrictions);

	// Get matching terrainId when user selected a row in terrain list in scenario editor/terrain tab.
	// Returns a terrainId, returns -1 if we want to use default behaviour (no custom choice)
	long int GetTerrainIdForSelectedTerrainIndex(AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor, long int selectedIndex);

	// Handle a ROR's original code call to trace method (0x42C330)
	// firstRORCallTextParamAddress is the address in stack of the text argument "xxx".
	// Next potential arguments (%xxx) are next data in stack.
	void HandleRORDebugLogCall(unsigned long int firstRORCallTextParamAddress);

	// Fix the check on path finding when trying to assign a task to a villager
	long int GathererCheckPathFinding(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *actorAsType50, long int *pathFindingArgs);

	// Write the F11 centered text (if displayed)
	// Warning, this is only refreshed when population changes ?
	void WriteF11PopInfoText(AOE_STRUCTURES::STRUCT_UI_F11_POP_LABEL *f11panel, char *bufferToWrite, char *defaultFormat,
		char *localizedText, long int currentPop, long int houseMaxPop);

	// Disable dock for all players on maps where AI does NOT builds docks.
	void DisableWaterUnitsIfNeeded();

	// Disable all walls for current game.
	void DisableWalls();

	// Called on each loop in infAI.FindEnemyUnitIdWithinRange(ptrMyReferenceUnit, maxDistance, DATID, DATID, DATID, DATID)
	// *** OBSOLETE !!! ***
	void OnFindEnemyUnitIdWithinRangeLoop(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_UNIT_MEMORY *currentUnitListElem);

	// Called at the end of showUnitCommandButtons
	void AfterShowUnitCommandButtons(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI);

	// User interface command handler for 1 single unit.
	// isPanelUnit = true if unitBase is the main selected unit (the one visible in bottom-left unit info zone)
	// "Common" treatments (interface updates, etc) are only done when isPanelUnit = true
	bool ApplyUserCommandForUnit(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
		AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID uiCommandId, long int infoValue, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase, bool isPanelUnit);

	// Called when a game UI command button is clicked.
	// Returns true if event has been handled and must NOT be handle by ROR standard code.
	// Returns false by default (most cases) !
	bool OnGameCommandButtonClick(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
		AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID uiCommandId, long int infoValue);


	// Manages the display of a unit shortcut for non-standard shortcuts.
	// Returns true if we want to let standard game code execute the shortcut display operation. (default false)
	bool DisplayCustomUnitShortcutSymbol(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase,
		long int posX, long int posY, long int unknown_arg3);

	// Adds custom attributes (armor) in buildings' unit info zone.
	// currentLine is incremented if lines are added.
	void DisplayCustomBuildingAttackAttributesInUnitInfo(AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone, long int &currentLine);


	// Entry point when mouse hovers on a unit. foundInteraction and foundHintDllId values are IN/OUT, you are allowed to update them to overload ROR default behaviour.
	// Note: this only impacts mouse displayed cursor and hint text, not which right-click actions are actually possible.
	// If returned cursorToForce is >= 0, then stop other treatments and use this value as new cursor.
	// Returns true if output values have been updated.
	bool OnHoverOnUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, STRUCT_PLAYER *controlledPlayer, long int unitPlayerId,
		UNIT_INTERACTION_ID &foundInteraction, long int &foundHintDllId, GAME_CURSOR &cursorToForce);

	// Handles a right-click (in-game) when mouse action type is a custom one, for 1 single selected unit
	// posX and posY are game positions
	// mouseTargetUnit can be NULL
	void OnInGameRightClickCustomAction(float posX, float posY, AOE_STRUCTURES::STRUCT_UNIT_BASE *mouseTargetUnit, 
		AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit);

	// Handles a right-click (in-game) when mouse action type is a custom one.
	// posX and posY are game positions
	// mouseTargetUnit can be NULL
	void OnInGameRightClickCustomAction(float posX, float posY, AOE_STRUCTURES::STRUCT_UNIT_BASE *mouseTargetUnit);

	// Entry point when a unit activity stops.
	void OnUnitActivityStop(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity);


	// Handle the optional display of debug information (like F5 info)
	// Returns true if standard game info (F5 zone) must NOT be executed.
	bool HandleShowDebugGameInfo(AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings);

	// Changes current in-game debug info level (F5)
	void SetNextInGameDebugInfoLevel();


	/* This is called on GameSettings.NotifyEvent game method call (0x501980).
	playerId is the event's player Id = Player that lost/gained ruins control, built a wonder, researched a tech, etc.
	- arg3 depends on eventID, it can be a DAT_ID
	- arg4 depends on eventID, it can be a Y position (long int)
	- arg5 depends on eventID, it can be a X position (long int)
	CST_GET_FARM_DEPLETED => arg3 = unitId (farm)
	CST_GET_UNIT_SPAWNED => arg3=DAT_ID, arg4/5=position of building from where the unit spawns (NOT unit's exact location)
	CST_GET_RESEARCH_COMPLETE => arg3=researchId, arg4/5=position of building from where the research has been done
	CST_GET_BUILDING_COMPLETE => arg3=unitDATID, arg4/5=position of building
	All wonder events (start/finish/destroyed = 0x6C 6D 6E) => arg3=posY, arg4=posX
	Relics/ruins events => arg3/4/5 are unused ?
	Returns true if the event should be ignored by ROR (skip original treatments). Default false (most cases)
	*/
	bool EntryPoint_GameSettingsNotifyEvent(AOE_CONST_INTERNAL::GAME_EVENT_TYPE eventId, short int playerId, long int arg3, long int arg4, long int arg5);


};


// A unique global object for everyone
extern RockNRorCommand crCommand;
}
