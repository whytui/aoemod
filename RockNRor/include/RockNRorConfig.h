
#pragma once

#include "../../../tinyxml/tinyxml.h"
#include <string>
#include <list>
#include <algorithm>
#include <AOE_const_functional.h>
#include <AOE_SN_numbers.h>
#include <AOE_empires_dat.h>
#include "commonConfigEnums.h"
#include "autoRebuildFarmConfig.h"
#include "civilizationInfo.h"
#include "autoAttackPolicy.h"
#include "TileSetHandler.h"

using namespace std;

// Defines the maximum acceptable interval in milliseconds under which gameTimerSlowDownFactor doesn't need to be decreased
#define CST_TIMER_INTERVAL_TOO_SLOW 80
// Defines the minimum acceptable interval in milliseconds under which gameTimerSlowDownFactor needs to be increased
#define CST_TIMER_INTERVAL_TOO_FAST 4
#define CST_MAXIMUM_SLOW_DOWN_FACTOR 50

#define CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS 10 // counts 0 that is unused (used for "no shortcut" in fact)

namespace ROCKNROR {
namespace CONFIG {
;


// Describes the in-game "F5" debug levels.
enum INGAME_DEBUG_LEVEL {
	IDL_STANDARD_TIMES_AVG = 0, // After pressing once on F5.
	IDL_STANDARD_TIMES_MAX = 1, // After pressing twice on F5.
	IDL_STANDARD_MAX_ALLOWED_LEVEL = 1, // In standard game, a third press on F5 returns to normal mode.
	IDL_HIDDEN_COMM = 2, // Corresponds to the hidden debug infos about communications (useful in MP games)
	IDL_HIDDEN_AI = 3, // Corresponds to the hidden debug infos about AI player (currently selected as human-controlled)
	IDL_CUSTOM = 4,
	IDL_COUNT
};


class UnitSpawnShortcutInfo {
public:
	UnitSpawnShortcutInfo();

	short int DAT_ID;
	bool onlyOneUnit;
};

// Represents information about a DRS file to load at startup.
class DrsFileToLoad {
public:
	DrsFileToLoad(std::string folder, std::string filename) {
		this->folder = folder;
		this->filename = filename;
		if (this->folder.length() > 0) {
			if (this->folder[this->folder.length() - 1] != '\\') {
				this->folder += '\\';
			}
		}
	}
	inline bool operator == (const DrsFileToLoad &drs) const
	{
		return ((drs.filename == this->filename) &&
			(drs.folder == this->folder));
	}
	bool IsValid() {
		auto it = std::find_if(this->filename.begin(), this->filename.end(),
			[](char c) {return c == '\\'; }
		);
		if (it != this->filename.end()) {
			return false; // string contains "\\"
		}
		return !this->filename.empty();
	}
	std::string folder;
	std::string filename;
};


// This class manages all information read from RockNRor configuration files.
class RockNRorConfig {
public:
	RockNRorConfig();
	~RockNRorConfig();

	// Variables - RockNRor-specific
	bool couldNotReadXMLConfig;
	bool couldNotReadCivXMLConfig;
	bool couldNotReadTilesetXMLConfig;
	bool showAlertOnMissingFeature;
	bool hideWelcomeMessage;
	bool showRockNRorMenu;
	bool showCustomPopInfo;
	bool useImprovedGameSpeeds;
	float improvedGameSpeedFactor; // Value to multiply/divide when increasing/decreasing game speed
	bool fixLogisticsNoHouseBug;
	bool editor_disableTerrainRestrictions;
	bool editor_disableHillModeChecks;
	bool editor_allowUnitOverlapping;
	bool fixHumanPlayer_specificSeeUnit;
	bool allyExplorationIsAlwaysShared;
	long int singlePlayerMaxPopulation;
	bool forceMPCompatibility; // If true, try to protect MP games from sync errors from RockNRor treatements ; some features might be restricted/disabled.
	// Technical
	bool autoFixMissingFeatures;
	// If set (not recommended), all (...most) "generic" fixes (fixes without a specific config to enable/disable) won't be applied. Can be used for troubleshooting.
	bool doNotApplyFixes;
	bool doNotUpdateVirtualMethods; // default=false. Can be used for trobleshooting.
	long int gameTimerSlowDownFactor;
	bool gameTimerSlowDownAutoFix;
	long int unitAIDetectNearbyUnitsMinimumDelay; // Default=4000. Minimum delay in milliseconds between each detection of nearby units. Affects AI/performance.
	long int collectRORDebugLogs; // 0=no, 1=yes, filtered, 2=yes, all
	bool showLogsInReverseOrder; // No input parameter for this (init in constructor only)
	bool showRockNRorNotifications; // Use this to disable RockNRor message notifications (not recommended)
	bool enableRPGModeInRandomGames; // Use "RPG" mode in DM/RM games
	bool enableRPGModeInScenario; // Use "RPG" mode in scenario/campaign games
	std::list<DrsFileToLoad*> customDrsFilesList; // List of DRS files to load at startup.
	std::list<std::string> customStringsFilesList; // List of files containing custom localized strings.
	std::string customEmpiresDatRelativePath; // Relative path to empires.dat file to use

	// Random Games settings
	bool noNeutralInitialDiplomacy; // If true, initial diplomacy for no-team players will be enemy, not neutral. AI won't become all allied against human.
	long int initialResourcesByChoice_RM[4][4]; // First index=user choice (default/small/medium/large), Second index=resource type.
	long int initialResources_DM[4]; // Initial resources in deathmatch
	long int initialResourceHardestAIBonus_RM[4]; // Initial bonus for AI players in hardest in random games
	long int initialResourceHardestAIBonus_DM[4]; // Initial bonus for AI players in hardest in deathmatches
	long int defaultPerNumbers_RM[AOE_CONST_FUNC::CST_LAST_SN_NUMBER + 1];
	bool defaultPerNumbers_RM_isSet[AOE_CONST_FUNC::CST_LAST_SN_NUMBER + 1]; // set to true to indicate when corresponding defaultPerNumbers_RM value is relevant
	long int defaultPerNumbers_DM[AOE_CONST_FUNC::CST_LAST_SN_NUMBER + 1];
	bool defaultPerNumbers_DM_isSet[AOE_CONST_FUNC::CST_LAST_SN_NUMBER + 1]; // set to true to indicate when corresponding defaultPerNumbers_DM value is relevant

	// Related to empires.dat loaded data
	bool fixVillagerWorkRates;
	bool noWalls;
	bool noDockInMostlyLandMaps;
	int unitResourceAmountTree; // <=0 means use empires.dat default (do nothing, in fact)
	int unitResourceAmountTreeForest; // <=0 means use empires.dat default (do nothing, in fact)
	int unitResourceAmountBerryBush; // <=0 means use empires.dat default (do nothing, in fact)
	int unitResourceAmountGazelle; // <=0 means use empires.dat default (do nothing, in fact)
	int unitResourceAmountLion; // <=0 means use empires.dat default (do nothing, in fact)
	int unitResourceAmountElephant; // <=0 means use empires.dat default (do nothing, in fact)
	int unitResourceAmountAlligator; // <=0 means use empires.dat default (do nothing, in fact)
	int unitResourceAmountGoldMine; // <=0 means use empires.dat default (do nothing, in fact)
	int unitResourceAmountStoneMine; // <=0 means use empires.dat default (do nothing, in fact)
	int unitResourceAmountFish; // <=0 means use empires.dat default (do nothing, in fact)

	// Conversion resistance
	float conversionResistance_Macedonian;
	float conversionResistance_Boats;
	float conversionResistance_Priests;
	float conversionResistance_Chariots;
	float conversionResistance_WarElephants;
	float conversionResistance_WarElephants_Persian;
	// AI
	long int improveAILevel; // 0=disable AI improvements. >0 to allow AI improvements (maybe there will be several improvement levels in the future). See also IsImproveAIEnabled(playerId)
	bool enableTestCompareAI; // If set, players 1,3,5,7 are "AI improved" whereas players 2,4,6,8 are NOT. To be used with improveAILevel>0.
	long int tacticalAIUpdateDelay; // In seconds
	bool generateStrategyForRM;
	bool generateStrategyForDM;
	// Panic mode options
	long int panicModeDelay; // seconds between 2 runs of panic mode algorithm
	long int maxPanicUnitsCountToAddInStrategy;
	long int minPopulationBeforeBuildOptionalItems; // An estimated population value AI must train before triggering optional strategy items we added. Actual behaviour depends on amount of skipped strategy items.
	// Dislike values
	long int dislikeComputeInterval; // In seconds
	long int dislike_allArtefacts; // all relics/all ruins/wonder
	long int dislike_humanPlayer;
	// City Plan
	bool cityPlanLikeValuesEnhancement; // (de)activate city plan like calculations enhancements
	//float cityPlanHouseDistanceFromTownCenter; // Preferred distance to build houses from town center. Default 3.
	//float cityPlanHouseDistanceFromStorageBld; // Preferred distance to build houses from granary/storage pit. Default 3.
	//float cityPlanHouseDistanceFromOtherBld; // Preferred distance to build houses from other buildings (non-storage). Default 3.
	long int cityPlanBerryBushWeightForGranary; // Weight for each bush unit in granary location computation. Default is 0x80 but it is too much (2 already reach maximum weight=0xFE)
	// Map generation
	long int randomMapRelicsCount; // Default 5.
	long int randomMapRuinsCount; // Default 5.
	bool useMapGenerationCustomElevationCalculation;
	float mapGenerationCustomElevationFactor[AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_MAP_TYPES_COUNT];
	// Scenario editor
	bool showHiddenTerrainsInEditor;
	long int showHiddenUnitsInEditor;
	bool fixInvisibleTree;
	bool useCustomMapDimensions;
	// Civilizations
	long int civCount; // Does NOT count gaia (index=0)
	bool allowPickingCustomCivsInRandomInit;
	bool randomTechTreeForRMGames;
	// Human interface
	bool enableCallNearbyIdleMilitaryUnits;
	long int distanceToCallNearbyIdleMilitaryUnits;
	bool enableSpawnUnitsMoveToLocation;
	bool enableSpawnUnitsAutoTarget;
	bool enableSpawnUnitAutoRepairTC;
	bool unitShortcutsPriorityReverseOrder; // If true, try to set shortcut=9 before 8,7... 1
	UnitSpawnShortcutInfo unitShortcutsInformation[CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS]; // index 0 is unused
	bool enableAdditionalNumpadShortcuts; // If true, numpad 0-9 keys are additional unit shortcuts.
	ROCKNROR::CONFIG::AutoRebuildFarmConfig autoRebuildFarmsConfig[ROCKNROR::CFG_GAME_TYPES_COUNT];
	bool useImprovedButtonBar;
	bool allowMultiQueueing; // Allow queuing different unit types. Default is false.
	bool useEnhancedRulesForAutoAttackTargetSelection;
	AutoAttackPolicy autoAttackOptionForBlastMeleeUnits;
	AutoAttackPolicy autoAttackOptionForBlastRangedUnits;
	const AutoAttackPolicy autoAttackOptionDefaultValues = AutoAttackPolicy(true, true, true, true, false);
	bool enableInGameDisplayDebugInfo; // If true, allows more "F5 debug levels" in game screen.
	bool useF5LabelZoneForCustomDebugInfo; // If true, resource info bar can be replaced by debug text.
	bool assassinMode; // If true, player can kill any unit with DEL key (not for MP games). Enabled via a cheat code only

	// Methods
	// Read RockNRor main configuration XML file
	bool ReadXMLConfigFile(char *fileName);
	// Get a bool value from a XML element and an attribute name.
	// If attribute is not found, returns false.
	// If attribute value is "0" or "false", returns false.
	// All other values = return true.
	bool XML_GetBoolElement(TiXmlElement *elem, const char *attributeName);
	// Returns matching attribute value, if found, or empty string if not found.
	const char * XML_GetAttributeValue(TiXmlElement *elem, const char *attributeName);

	// Reads the gameType attribute and returns the result in ConfigGameType enum. Returns CFG_GAME_UNKNOWN if not found
	ROCKNROR::ConfigGameType XML_ReadGameTypeAttribute(TiXmlElement *elem);

	void SetAutoAttackPolicyFromAttributes(AutoAttackPolicy *aap, TiXmlElement *elem);
	// Read civilizations info XML file
	bool ReadCivXMLConfigFile(char *fileName);
	CivilizationInfo *GetCivInfo(int civId);

	// Read tileset info XML file
	bool ReadTilesetXMLConfigFile(char *fileName);

	ROCKNROR::CONFIG::AutoRebuildFarmConfig *GetAutoRebuildFarmConfig(long int isScenario, long int isDM);


	// Constants (not customizable)
	const float MINVALUE_improvedGameSpeedFactor = (float) 1.1;
	const long int MINVALUE_collectRORDebugLogs = 0;
	const long int MAXVALUE_collectRORDebugLogs = 2;
	const long int MINVALUE_gameTimerSlowDownFactor = 0;
	const long int MINVALUE_improveAILevel = 0;
	const long int MAXVALUE_improveAILevel = 1; // Update this if more levels are added
	const long int MINVALUE_tacticalAIUpdateDelay = 15;
	const long int MINVALUE_minPopulationBeforeBuildOptionalItems = 5;
	const long int MINVALUE_panicModeDelay = 5;
	const long int MINVALUE_maxPanicUnitsCountToAddInStrategy = 0;
	const long int MINVALUE_dislikeComputeInterval = 3;
	const long int MINVALUE_dislike = 0;
	const long int MAXVALUE_dislike = 99;
	const float MINVALUE_conversionResistance = (float) 0.2;
	const long int MINVALUE_artefactCount = 0;
	const long int MAXVALUE_artefactCount = 200; // more than 10 is huge already ;)
	const long int MINVALUE_civCount = 16;
	const long int MAXVALUE_civCount = 255;
	const float MINVALUE_cityPlanBuildingDistances = 1;
	const float MAXVALUE_cityPlanBuildingDistances = 50; // 50 is huge already ;)
	const long int MINVALUE_cityPlanBerryBushWeightForGranary = 1; // But >10 is strongly recommended
	const long int MAXVALUE_cityPlanBerryBushWeightForGranary = 254; // But <80 or even <50 is strongly recommended.
	const long int MINVALUE_distanceToCallNearbyIdleMilitaryUnits = 0;
	const long int MAXVALUE_distanceToCallNearbyIdleMilitaryUnits = 254;
	const long int MINVALUE_resourceAmount = 0;
	const long int MINVALUE_maxPopulation = 0;
	const long int MAXVALUE_maxPopulation = 255;
	const long int MINVALUE_unitAIDetectNearbyUnitsMinimumDelay = 300; // milliseconds
	const long int MAXVALUE_unitAIDetectNearbyUnitsMinimumDelay = 20000; // milliseconds

private:
	CivilizationInfo *allCivInfo[CST_MAX_TOTAL_CIV_COUNT];
	void InitializeDefaultResourceValues();
};

// Get a bool value from a XML element and an attribute name.
// If attribute is not found, returns false.
// If attribute value is "0" or "false", returns false.
// All other values = return true.
bool XML_GetBoolElement(TiXmlElement *elem, const char *attributeName);


}
}
