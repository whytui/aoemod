
#pragma once

#include "../../../tinyxml/tinyxml.h"
#include <string>
#include <AOE_const_functional.h>
#include <AOE_SN_numbers.h>
#include <AOE_empires_dat.h>
#include "civilizationInfo.h"

using namespace std;

// Defines the maximum acceptable interval in milliseconds under which gameTimerSlowDownFactor doesn't need to be decreased
#define CST_TIMER_INTERVAL_TOO_SLOW 80
// Defines the minimum acceptable interval in milliseconds under which gameTimerSlowDownFactor needs to be increased
#define CST_TIMER_INTERVAL_TOO_FAST 3
#define CST_MAXIMUM_SLOW_DOWN_FACTOR 50

#define CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS 10 // counts 0 that is unused (used for "no shortcut" in fact)

class UnitSpawnShortcutInfo {
public:
	UnitSpawnShortcutInfo();

	short int DAT_ID;
	bool onlyOneUnit;
};


enum AUTO_ATTACK_POLICIES {
	AAP_DEFAULT = 1,
	AAP_IGNORE_VILLAGERS, // auto-attack every unit except villagers (including non-war ships)
	AAP_IGNORE_BUILDINGS, // auto-attack every unit except non-tower buildings
	AAP_IGNORE_VILLAGERS_AND_BLDINGS, // auto-attack every unit except villagers, non-tower buildings
	AAP_IGNORE_ALL // never auto-attack any unit
};

// This class manages all information read from CustomROR configuration files.
class CustomRORConfig {
public:
	CustomRORConfig();
	~CustomRORConfig();

	// Variables - CustomROR-specific
	bool couldNotReadXMLConfig;
	bool couldNotReadCivXMLConfig;
	bool showAlertOnMissingFeature;
	bool hideWelcomeMessage;
	bool showCustomRORMenu;
	bool showCustomPopInfo;
	bool useImprovedGameSpeeds;
	float improvedGameSpeedFactor; // Value to multiply/divide when increasing/decreasing game speed
	bool fixLogisticsNoHouseBug;
	bool editor_disableTerrainRestrictions;
	bool editor_disableHillModeChecks;
	bool editor_allowUnitOverlapping;
	bool fixHumanPlayer_specificSeeUnit;
	bool allyExplorationIsAlwaysShared;
	// Technical
	bool autoFixMissingFeatures;
	long int gameTimerSlowDownFactor;
	bool gameTimerSlowDownAutoFix;
	long int collectRORDebugLogs; // 0=no, 1=yes, filtered, 2=yes, all
	bool showLogsInReverseOrder; // No input parameter for this (init in constructor only)
	bool showCustomRORNotifications; // Use this to disable customROR message notifications (not recommended)

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

	// Conversion resistance
	float conversionResistance_Macedonian;
	float conversionResistance_Boats;
	float conversionResistance_Priests;
	float conversionResistance_Chariots;
	float conversionResistance_WarElephants;
	float conversionResistance_WarElephants_Persian;
	// AI
	long int improveAILevel;
	long int tacticalAIUpdateDelay;
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
	long int randomMapRelicsCount;
	long int randomMapRuinsCount;
	bool useMapGenerationCustomElevationCalculation;
	float mapGenerationCustomElevationFactor[AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_MAP_TYPES_COUNT];
	// Scenario editor
	bool showHiddenTerrainsInEditor;
	long int showHiddenUnitsInEditor;
	bool fixInvisibleTree;
	bool useCustomMapDimensions;
	// Civilizations
	long int civCount; // Does NOT count gaia (index=0)
	// Human interface
	bool enableCallNearbyIdleMilitaryUnits;
	long int distanceToCallNearbyIdleMilitaryUnits;
	bool enableSpawnUnitsMoveToLocation;
	bool enableSpawnUnitsAutoTarget;
	bool enableSpawnUnitAutoRepairTC;
	bool unitShortcutsPriorityReverseOrder; // If true, try to set shortcut=9 before 8,7... 1
	UnitSpawnShortcutInfo unitShortcutsInformation[CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS]; // index 0 is unused
	bool enableAdditionalNumpadShortcuts; // If true, numpad 0-9 keys are additional unit shortcuts.
	bool enableAutoRebuildFarms; // Enable the feature, rebuilding a farm also has other conditions...
	long int autoRebuildFarms_maxFarms; // Maximum number of farms that can be reached with "auto construction" of farms
	long int autoRebuildFarms_maxFood; // Maximum amount of food we allow before automatically rebuilding a farm.
	long int autoRebuildFarms_minWood; // Minimum amount of wood we require before automatically rebuilding a farm.
	bool useImprovedButtonBar;
	bool useEnhancedRulesForAutoAttackTargetSelection;
	AUTO_ATTACK_POLICIES autoAttackOptionForBlastMeleeUnits;
	AUTO_ATTACK_POLICIES autoAttackOptionForBlastRangedUnits;

	// Methods
	// Read CustomROR main configuration XML file
	bool ReadXMLConfigFile(char *fileName);
	// Get a bool value from a XML element and an attribute name.
	// If attribute is not found, returns false.
	// If attribute value is "0" or "false", returns false.
	// All other values = return true.
	bool XML_GetBoolElement(TiXmlElement *elem, const char *attributeName);
	// Returns matching attribute value, if found, or empty string if not found.
	const char * XML_GetAttributeValue(TiXmlElement *elem, const char *attributeName);
	AUTO_ATTACK_POLICIES GetAutoAttackPolicyFromText(const char *text);
	// Read civilizations info XML file
	bool ReadCivXMLConfigFile(char *fileName);
	CivilizationInfo *GetCivInfo(int civId);

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

private:
	CivilizationInfo *allCivInfo[CST_MAX_TOTAL_CIV_COUNT];
	void InitializeDefaultResourceValues();
};

// Get a bool value from a XML element and an attribute name.
// If attribute is not found, returns false.
// If attribute value is "0" or "false", returns false.
// All other values = return true.
bool XML_GetBoolElement(TiXmlElement *elem, const char *attributeName);
