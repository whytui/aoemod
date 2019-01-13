
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
#include <mystrings.h>
#include <Windows.h>
#include <ROR_global_variables.h>
#include <macroUtil.h>

#include "mainStructuresHandling.h"
#include "UI_utilities.h"
#include "RockNRorConfig.h"
#include "civilizationInfo.h"
#include "traceMessage.h"
#include "RockNRorGameObjects.h"
#include "interface.h" // SLP/icon ids...
#include "language.h"
#include "RockNRorLocalization.h"
#include "RockNRorCommon.h"
#include "unitDefHandling.h"
#include "TechTreeAnalyzer.h"
#include "RockNRorUIHelper.h"
#include "crTriggerSet.h"


// Defines common objects/variables/methods for RockNRor

#define CST_TIMER_STATS_ARRAY_SIZE 20

using namespace AOE_CONST_FUNC;
using namespace AOE_STRUCTURES;
using namespace ROCKNROR;


// Constants
static char *txtHumanPenalty = "Dislike penalty for human player";
static char *txtGameSpeedFactor = "Game speed change factor*100 (125 for 1.25)";
static char *txtAutoRebuildFarms = "Auto. rebuild farms";
static char *txtAutoRebuildFarmsMaxNumber = "Auto. rebuild farms max number";
static char *txtAutoRebuildFarmsMaxFood = "Auto. rebuild farms max food";
static char *txtAutoRebuildFarmsMinWood = "Auto. rebuild farms min wood";


// Max X/Y value we can quick-calculate distances on.
#define CST_INT_DISTANCE_VALUES_MAX_COORDINATE 30


// External classes
namespace ROCKNROR {
	namespace UI {
		class RnrUIHelper;
	}
}


// Internal structures/variables for RockNRor that are not read from configuration
class RockNRorInfo {
public:
	RockNRorInfo();
	~RockNRorInfo();

	// RockNRor Configuration
	static CONFIG::RockNRorConfig configInfo;
	bool hasFixForBuildingStratElemUnitId;
	bool hasManageAIFeatureON; // Is "ManageAI" feature installed (use custom flag "isAI" in player struct)
	bool hasCustomSelectedUnitsMemory; // Player struct size is extended to host more selected units
	bool hasRemovePlayerInitialAgeInScenarioInit;
	// Internal variables
	vector<short int> unitDefToDisable[9]; // List of unitDefIDs to disable because of triggers (for scenarios)
	vector<short int> researchesToDisable[9]; // List of researchIDs to disable because of triggers (for scenarios)
	RockNRorGameObjects myGameObjects;
	std::map<unsigned long int, long int> longestTimes_ms; // Collects longest RockNRor treatment times for each entry point.
	std::map<unsigned long int, long int> executionCounts; // Collects RockNRor execution count for each entry point.
	int gameTimerSlowDownCounter;
	long int CollectedTimerIntervalsIndex;
	long int CollectedTimerIntervals_ms[CST_TIMER_STATS_ARRAY_SIZE];
	int LastGameTimerAutoFix_second;
	long int lastRockNRorTimeExecution_gameTime_s; // In seconds. Used to limit RockNRor treatments in ROR's timer execution.
	int LastDislikeValuesComputationTime_second; // Stores game time when we last computed players dislike values.
	// To know which civ player names are already used when choosing names. Values: -1=unused, >=0=used by player #i
	// Warning: playerName index are from 0 to 8 here instead of 1 to 9 ! Be careful.
	char nameIndexIsUsedByPlayer[CST_MAX_TOTAL_CIV_COUNT][CST_MAX_NUMBER_OF_PLAYER_NAMES_PER_CIV];
	// Own DRS objects
	STRUCT_SLP_INFO rockNRorIcons; // RockNRor-specific icons
	STRUCT_SLP_INFO rockNRorUnitShortcuts; // RockNRor-specific SLP for unit shortcuts (number displayed nearby unit selection white square)

	// Triggers
	CR_TRIGGERS::crTriggerSet *triggerSet; // all information about custom triggers.
	long int triggersLastCheckTime_s; // In seconds. Last game time when triggers have been checked (for "timer" trigger types)

	// Tech tree handler: analysis is run when empires.dat is loaded: do not use features before this (cf IsReady method).
	ROCKNROR::STRATEGY::TechTreeAnalyzer techTreeAnalyzer;

	// To monitor conversion efficiency
	long int activeConversionAttemptsCount[9]; // Per playerId
	long int activeConversionSuccessfulAttemptsCount[9]; // Per playerId
	long int passiveConversionAttemptsCount[9]; // Per playerId
	long int passiveConversionSuccessfulAttemptsCount[9]; // Per playerId

	// Info about DAT file and its data
	short int empiresDatCivCount; // Number of civilizations in empires.dat, INCLUDING gaia
	long int empiresDatTechDefCount; // Number of tech def in empires.dat

	ROCKNROR::UI::RnrUIHelper *rnrUIHelper; // Main handler for custom UI
	AOE_STRUCTURES::STRUCT_UI_BUTTON *customGameMenuOptionsBtnVar; // References the "Rocknror options" button in game menu, if currently displayed

public:

	// Reset variables that are used during game.
	// Only reset internal variables that are game-dependent !
	// This is called on each game start/load
	void ResetVariables();

	// Resets and fills the list of unitDefID to disable for a player from a comma-separated list.
	void FillUnitDefToDisableFromString(long int playerId, const char *text);
	// Resets and fills the list of researchID to disable for a player from a comma-separated list.
	void FillResearchesToDisableFromString(long int playerId, const char *text);

	// Function to calculate conversion resistance for a giver unit from a given civ.
	// This replaces game's algorithm.
	// civId is only used if no conversion resistance is configured for unit, or if "improved conversion resistance mode" is disabled.
	float GetConversionResistance(char civId, AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef);

	// Function to calculate conversion resistance for a giver unit from a given civ.
	// This replaces game's algorithm.
	// Use civId=0 (same as "gaia" civ) to exclude any civ bonus/malus from calculation
	float GetConversionResistance_hardcodedWithConf(char civId, short int unitClass);

	// Applies an "auto-attack policy" on all player's selected units (only for owned units !)
	// flagsToApply is used to determine which flags have to be updated using values from autoAttackPolicyValues.
	// If flagsToApply.xxx is true, then update unit's auto_attack_policy.xxx to "autoAttackPolicyValues.xxx" value.
	void ApplyAutoAttackPolicyToPlayerSelectedUnits(AOE_STRUCTURES::STRUCT_PLAYER *player, 
		const AutoAttackPolicy &autoAttackPolicyValues, const AutoAttackPolicy &flagsToApply);

	// Returns true if a custom game popup is opened (only for popups, not dialogs !)
	bool HasOpenedCustomGamePopup();

	// Get main (first) selected unit, or NULL if none is selected.
	// Works in-game and in editor.
	// See SelectOneUnit (in playerHandling) for unit selection + AOE_selectUnit, AOE_clearSelectedUnits
	AOE_STRUCTURES::STRUCT_UNIT_BASE *GetMainSelectedUnit(AOE_STRUCTURES::STRUCT_PLAYER *player);

	// Get relevant "main selected unit" pointer according to game EXE status (using custom memory or not ?)
	AOE_STRUCTURES::STRUCT_UNIT_BASE *GetRelevantMainSelectedUnitPointer(AOE_STRUCTURES::STRUCT_PLAYER *player);

	// Get relevant "selected units" array pointer according to game EXE status (using custom memory or not ?)
	AOE_STRUCTURES::STRUCT_UNIT_BASE **GetRelevantSelectedUnitsPointer(AOE_STRUCTURES::STRUCT_PLAYER *player);

	// Fast-computes the integer distance for X and Y delta values (sqrt(X^2 + Y^2) as an integer).
	// Returns -1 if invalid (diffX and diffY values are capped at CST_INT_DISTANCE_VALUES_MAX_COORDINATE)
	static char GetIntDistance(char diffX, char diffY);

private:
	// Private method: fill a vector from a list of numeric values (short int)
	// Separator is , or ;
	void FillIDVectorFromString(vector<short int> &v, long int playerId, const char *text);

};




namespace ROCKNROR {
	extern RockNRorInfo crInfo;

	// Use this API to get "do we improve AI" information. This takes care of RockNRor configuration + possible custom rules.
	// Remark: AI can be currently disabled for a player, but "improved AI" will still remain true.
	// Useful to add custom rules for debugging
	bool IsImproveAIEnabled(int playerId);

	// Returns true if RPG mode is active in current game
	bool IsRpgModeEnabled();


	// Returns true if we want to use ROR's method to handle panic mode (not recommended): not optimized, and not very good
	// Returns false if we want to disable completely ROR's method, and use RockNRor methods for panic mode instead (recommended)
	bool ShouldUseOriginalPanicModeMethod();


	// Get a localized string to overload ROR's string localization system (language(x).dll)
	// Returns true if a (custom) localized string has been written into buffer.
	bool GetLocalizedString(long int stringId, char *buffer, long int bufferSize);

}

