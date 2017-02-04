#include "../include/crCommands.h"


namespace CUSTOMROR {



// Global static objects
CustomRORCommand CUSTOMROR::crCommand;


char outputBuffer[CRCMD_TEXT_BUFFER_SIZE];



CustomRORCommand::CustomRORCommand() {
}



CustomRORCommand::~CustomRORCommand() {
}


bool CustomRORCommand::CheckEnabledFeatures() {
	char strNotEnabled[] = " NOT ENABLED !";
	FILE *f;
	int res = fopen_s(&f, "CustomROR\\CustomROR.log", "w+"); // overwrite if already existing
	if (res) {
		std::string msg = std::string(localizationHandler.GetTranslation(CRLANG_ID_CANT_WRITE_FILE, "ERROR: cannot write to"));
		msg += " ";
		msg += "CustomROR\\CustomROR.log";
		traceMessageHandler.WriteMessage(msg);
		return false;
	}
	fprintf_s(f, "CustomROR %s\n", VER_FILE_VERSION_STR);
	if (CUSTOMROR::crInfo.configInfo.couldNotReadXMLConfig) {
		fprintf_s(f, localizationHandler.GetTranslation(CRLANG_ID_CANT_READ_XML_CONFIG, "Warning: configuration could not be read from XML file (missing or incorrect file)."));
		fprintf_s(f, "\n\n");
	}
	if (CUSTOMROR::crInfo.configInfo.couldNotReadCivXMLConfig) {
		fprintf_s(f, localizationHandler.GetTranslation(CRLANG_ID_CANT_READ_CIV_XML_CONFIG, "customROR_civs.xml could not be read (missing or incorrect file). This is not an error unless you want to use custom civilizations."));
		fprintf_s(f, "\n\n");
	}

	fprintf_s(f, localizationHandler.GetTranslation(CRLANG_ID_RUN_EXE_CONFIG_CHECKS, "Running checks on active features in game executable."));
	fprintf_s(f, "\nThis detects if some features from this version of CustomROR will not work because you need to enable them with \"CustomizeAOE\".\n");
	
	// Analyze EXE memory and check all necessary ROR_API calls are enabled.
	bool RORAPIFullyInstalled = CheckRorApiSequencesAreInstalled(f, CUSTOMROR::crInfo.configInfo.autoFixMissingFeatures);

	fprintf_s(f, "\nEnd of checks.\n\nConfiguration:\n");
	// General - not directly related to game
	fprintf_s(f, "hideWelcomeMessage:                        %d\n", CUSTOMROR::crInfo.configInfo.hideWelcomeMessage);
	fprintf_s(f, "showAlertOnMissingFeature:                 %d\n", CUSTOMROR::crInfo.configInfo.showAlertOnMissingFeature);
	fprintf_s(f, "autoFixMissingFeatures:                    %d\n", CUSTOMROR::crInfo.configInfo.autoFixMissingFeatures);
	fprintf_s(f, "empires.dat relative path to use:          %s\n", CUSTOMROR::crInfo.configInfo.customEmpiresDatRelativePath.c_str());
	fprintf_s(f, "showCustomRORMenu:                         %d\n", CUSTOMROR::crInfo.configInfo.showCustomRORMenu);
	fprintf_s(f, "showCustomPopulationInfo:                  %d\n", CUSTOMROR::crInfo.configInfo.showCustomPopInfo);
	fprintf_s(f, "useImprovedGameSpeeds:                     %d\n", CUSTOMROR::crInfo.configInfo.useImprovedGameSpeeds);
	fprintf_s(f, "collectRORDebugLogs:                       %d\n", CUSTOMROR::crInfo.configInfo.collectRORDebugLogs);
	fprintf_s(f, "showCustomRORNotifications:                %d\n", CUSTOMROR::crInfo.configInfo.showCustomRORNotifications);
	fprintf_s(f, "enableRPGModeInRandomGames:                %d\n", CUSTOMROR::crInfo.configInfo.enableRPGModeInRandomGames);
	fprintf_s(f, "enableRPGModeInScenario:                   %d\n", CUSTOMROR::crInfo.configInfo.enableRPGModeInScenario);
	fprintf_s(f, "gameTimerSlowDownAutoFix:                  %d\n", CUSTOMROR::crInfo.configInfo.gameTimerSlowDownAutoFix);
	fprintf_s(f, "gameTimerSlowDownFactor:                   %d\n", CUSTOMROR::crInfo.configInfo.gameTimerSlowDownFactor);
	fprintf_s(f, "fixInvisibleTree:                          %d\n", CUSTOMROR::crInfo.configInfo.fixInvisibleTree);
	fprintf_s(f, "fixHumanPlayer_specific_seeUnit:           %d\n", CUSTOMROR::crInfo.configInfo.fixHumanPlayer_specificSeeUnit);
	fprintf_s(f, "useImprovedButtonBar:                      %d\n", CUSTOMROR::crInfo.configInfo.useImprovedButtonBar);
	fprintf_s(f, "allowMultiQueueing:                        %d\n", CUSTOMROR::crInfo.configInfo.allowMultiQueueing);
	// General - related to game
	fprintf_s(f, "unit resource amounts: alligator=%d bush=%d elephant=%d\n"
		"...gazelle=%d gold=%d lion=%d stone=%d tree=%d forest=%d fish=%d\n",
		CUSTOMROR::crInfo.configInfo.unitResourceAmountAlligator,
		CUSTOMROR::crInfo.configInfo.unitResourceAmountBerryBush, 
		CUSTOMROR::crInfo.configInfo.unitResourceAmountElephant, 
		CUSTOMROR::crInfo.configInfo.unitResourceAmountGazelle, 
		CUSTOMROR::crInfo.configInfo.unitResourceAmountGoldMine, 
		CUSTOMROR::crInfo.configInfo.unitResourceAmountLion, 
		CUSTOMROR::crInfo.configInfo.unitResourceAmountStoneMine, 
		CUSTOMROR::crInfo.configInfo.unitResourceAmountTree, 
		CUSTOMROR::crInfo.configInfo.unitResourceAmountTreeForest,
		CUSTOMROR::crInfo.configInfo.unitResourceAmountFish);
	// Random games settings
	fprintf_s(f, "noWalls:                                   %ld\n", CUSTOMROR::crInfo.configInfo.noWalls);
	fprintf_s(f, "noNeutralInitialDiplomacy:                 %d\n", CUSTOMROR::crInfo.configInfo.noNeutralInitialDiplomacy);
	fprintf_s(f, "noWalls:                                   %d\n", CUSTOMROR::crInfo.configInfo.noWalls);
	fprintf_s(f, "[RM] initial food (default/small/med/large) : %ld/%ld/%ld/%ld\n",
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[0][RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[1][RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[2][RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[3][RESOURCE_TYPES::CST_RES_ORDER_FOOD]);
	fprintf_s(f, "[RM] initial wood (default/small/med/large) : %ld/%ld/%ld/%ld\n",
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[0][RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[1][RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[2][RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[3][RESOURCE_TYPES::CST_RES_ORDER_WOOD]);
	fprintf_s(f, "[RM] initial stone (default/small/med/large): %ld/%ld/%ld/%ld\n",
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[0][RESOURCE_TYPES::CST_RES_ORDER_STONE],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[1][RESOURCE_TYPES::CST_RES_ORDER_STONE],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[2][RESOURCE_TYPES::CST_RES_ORDER_STONE],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[3][RESOURCE_TYPES::CST_RES_ORDER_STONE]);
	fprintf_s(f, "[RM] initial gold (default/small/med/large) : %ld/%ld/%ld/%ld\n",
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[0][RESOURCE_TYPES::CST_RES_ORDER_GOLD],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[1][RESOURCE_TYPES::CST_RES_ORDER_GOLD],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[2][RESOURCE_TYPES::CST_RES_ORDER_GOLD],
		CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[3][RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
	fprintf_s(f, "[DM] initial resources (food/wood/stone/gold): %ld/%ld/%ld/%ld\n",
		CUSTOMROR::crInfo.configInfo.initialResources_DM[RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		CUSTOMROR::crInfo.configInfo.initialResources_DM[RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		CUSTOMROR::crInfo.configInfo.initialResources_DM[RESOURCE_TYPES::CST_RES_ORDER_STONE],
		CUSTOMROR::crInfo.configInfo.initialResources_DM[RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
	fprintf_s(f, "[RM] initialBonus for AI (food/wood/stone/gold): %ld/%ld/%ld/%ld\n",
		CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_STONE],
		CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
	fprintf_s(f, "[DM] initialBonus for AI (food/wood/stone/gold): %ld/%ld/%ld/%ld\n",
		CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_STONE],
		CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
	for (int i = 0; i <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER; i++) {
		if (CUSTOMROR::crInfo.configInfo.defaultPerNumbers_RM_isSet[i]) {
			fprintf_s(f, "[RM] Force PER number #%ld to %ld\n", i, CUSTOMROR::crInfo.configInfo.defaultPerNumbers_RM[i]);
		}
	}
	for (int i = 0; i <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER; i++) {
		if (CUSTOMROR::crInfo.configInfo.defaultPerNumbers_DM_isSet[i]) {
			fprintf_s(f, "[DM] Force PER number #%ld to %ld\n", i, CUSTOMROR::crInfo.configInfo.defaultPerNumbers_DM[i]);
		}
	}
	// Human interface / shortcuts
	fprintf_s(f, "useNumPadUnitShortcuts:                    %d\n", CUSTOMROR::crInfo.configInfo.enableAdditionalNumpadShortcuts);
	fprintf_s(f, "enableCallNearbyIdleMilitaryUnits:         %d\n", CUSTOMROR::crInfo.configInfo.enableCallNearbyIdleMilitaryUnits);
	fprintf_s(f, "distanceToCallNearbyIdleMilitaryUnits:     %ld\n", CUSTOMROR::crInfo.configInfo.distanceToCallNearbyIdleMilitaryUnits);
	fprintf_s(f, "enableAutoMoveToLocation:                  %d\n", CUSTOMROR::crInfo.configInfo.enableSpawnUnitsMoveToLocation);
	fprintf_s(f, "enableSpawnUnitsAutoTarget:                %d\n", CUSTOMROR::crInfo.configInfo.enableSpawnUnitsAutoTarget);
	fprintf_s(f, "enableSpawnUnitAutoRepairTC:               %d\n", CUSTOMROR::crInfo.configInfo.enableSpawnUnitAutoRepairTC);
	fprintf_s(f, "unitSpawnShortcutReverseOrder:             %d\n", CUSTOMROR::crInfo.configInfo.unitShortcutsPriorityReverseOrder);
	for (int shortcutId = 1; shortcutId < CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS; shortcutId++) {
		CONFIG::UnitSpawnShortcutInfo *sinfo = &CUSTOMROR::crInfo.configInfo.unitShortcutsInformation[shortcutId];
		if (sinfo->DAT_ID >= 0) {
			fprintf_s(f, "unitSpawn shortcut %d: unit=%03d onlyOneUnit=%d (%.12s)\n", shortcutId, sinfo->DAT_ID, sinfo->onlyOneUnit,
				GetHardcodedUnitName(sinfo->DAT_ID));
		}
	}
	for (CUSTOMROR::ConfigGameType i = (CUSTOMROR::ConfigGameType)(CUSTOMROR::CFG_GAME_UNKNOWN + 1); i < CUSTOMROR::CFG_GAME_TYPES_COUNT; i = (CUSTOMROR::ConfigGameType)(i + 1)) {
		const char *name = CUSTOMROR::ConfigGameTypeNames[i];
		fprintf_s(f, "[%s] autoRebuildFarms:                %ld\n", name, CUSTOMROR::crInfo.configInfo.autoRebuildFarmsConfig[i].enableAutoRebuildFarms ? 1 : 0);
		fprintf_s(f, "[%s] autoRebuildFarms_maxFarms:       %ld\n", name, CUSTOMROR::crInfo.configInfo.autoRebuildFarmsConfig[i].autoRebuildFarms_maxFarms);
		fprintf_s(f, "[%s] autoRebuildFarms_maxFood:        %ld\n", name, CUSTOMROR::crInfo.configInfo.autoRebuildFarmsConfig[i].autoRebuildFarms_maxFood);
		fprintf_s(f, "[%s] autoRebuildFarms_minWood:        %ld\n", name, CUSTOMROR::crInfo.configInfo.autoRebuildFarmsConfig[i].autoRebuildFarms_minWood);
	}
	fprintf_s(f, "useEnhancedRulesForAutoAttack:             %ld\n", CUSTOMROR::crInfo.configInfo.useEnhancedRulesForAutoAttackTargetSelection);
	fprintf_s(f, "autoAttackPolicy vs towers/military/buildings/villagers/walls\n");
	fprintf_s(f, "- For Melee Units:  %d/%d/%d/%d/%d\n", CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits.attackTowers,
		CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits.attackMilitary, 
		CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits.attackNonTowerBuildings, 
		CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits.attackVillagers, 
		CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits.attackWalls);
	fprintf_s(f, "- For Ranged Units: %d/%d/%d/%d/%d\n", CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits.attackTowers,
		CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits.attackMilitary,
		CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits.attackNonTowerBuildings,
		CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits.attackVillagers,
		CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits.attackWalls);

	// Conversion
	fprintf_s(f, "conversionResistance_Boats:                %f\n", CUSTOMROR::crInfo.configInfo.conversionResistance_Boats);
	fprintf_s(f, "conversionResistance_Chariots:             %f\n", CUSTOMROR::crInfo.configInfo.conversionResistance_Chariots);
	fprintf_s(f, "conversionResistance_Macedonian:           %f\n", CUSTOMROR::crInfo.configInfo.conversionResistance_Macedonian);
	fprintf_s(f, "conversionResistance_Priests:              %f\n", CUSTOMROR::crInfo.configInfo.conversionResistance_Priests);
	fprintf_s(f, "conversionResistance_WarElephants:         %f\n", CUSTOMROR::crInfo.configInfo.conversionResistance_WarElephants);
	fprintf_s(f, "conversionResistance_WarElephants_Persian: %f\n", CUSTOMROR::crInfo.configInfo.conversionResistance_WarElephants_Persian);
	// Various AI
	fprintf_s(f, "improveAILevel:                            %ld\n", CUSTOMROR::crInfo.configInfo.improveAILevel);
	fprintf_s(f, "tacticalAIUpdateDelay:                     %ld\n", CUSTOMROR::crInfo.configInfo.tacticalAIUpdateDelay);
	fprintf_s(f, "minPopulationBeforeOptionalItems:          %ld\n", CUSTOMROR::crInfo.configInfo.minPopulationBeforeBuildOptionalItems);
	fprintf_s(f, "maxPanicUnitsCountToAddInStrategy:         %ld\n", CUSTOMROR::crInfo.configInfo.maxPanicUnitsCountToAddInStrategy);
	fprintf_s(f, "panicModeDelay:                            %ld\n", CUSTOMROR::crInfo.configInfo.panicModeDelay);
	fprintf_s(f, "dislike computation interval:              %ld\n", CUSTOMROR::crInfo.configInfo.dislikeComputeInterval);
	fprintf_s(f, "dislike value - all artefacts/wonder:      %ld\n", CUSTOMROR::crInfo.configInfo.dislike_allArtefacts);
	fprintf_s(f, "dislike value - human player:              %ld\n", CUSTOMROR::crInfo.configInfo.dislike_humanPlayer);
	fprintf_s(f, "fixLogisticsNoHouseBug:                    %d\n", CUSTOMROR::crInfo.configInfo.fixLogisticsNoHouseBug ? 1: 0);
	fprintf_s(f, "fixVillagerWorkRates:                      %d\n", CUSTOMROR::crInfo.configInfo.fixVillagerWorkRates);
	// City plan
	fprintf_s(f, "cityPlanLikeValuesEnhancement:             %d\n", CUSTOMROR::crInfo.configInfo.cityPlanLikeValuesEnhancement);
	//fprintf_s(f, "cityPlanHouseDistanceFromTownCenter:       %f\n", CUSTOMROR::crInfo.configInfo.cityPlanHouseDistanceFromTownCenter);
	//fprintf_s(f, "cityPlanHouseDistanceFromStorageBld:       %f\n", CUSTOMROR::crInfo.configInfo.cityPlanHouseDistanceFromStorageBld);
	//fprintf_s(f, "cityPlanHouseDistanceFromOtherBld:         %f\n", CUSTOMROR::crInfo.configInfo.cityPlanHouseDistanceFromOtherBld);
	fprintf_s(f, "cityPlanBerryBushWeightForGranary:         %ld\n", CUSTOMROR::crInfo.configInfo.cityPlanBerryBushWeightForGranary);
	// Map generation
	fprintf_s(f, "random map relics count:                   %ld\n", CUSTOMROR::crInfo.configInfo.randomMapRelicsCount);
	fprintf_s(f, "random map ruins count:                    %ld\n", CUSTOMROR::crInfo.configInfo.randomMapRuinsCount);
	fprintf_s(f, "use map gen. elevation customization:      %d\n", CUSTOMROR::crInfo.configInfo.useMapGenerationCustomElevationCalculation ? 1: 0);
	for (int mapType = 0; mapType < 10; mapType++) {
		fprintf_s(f, "map gen. elevation factor  mapType=%ld value=%f\n", mapType, CUSTOMROR::crInfo.configInfo.mapGenerationCustomElevationFactor[mapType]);
	}
	fprintf_s(f, "disable dock in mostly-land maps:          %ld\n", CUSTOMROR::crInfo.configInfo.noDockInMostlyLandMaps ? 1: 0);
	// Scenario Editor
	fprintf_s(f, "showHiddenTerrainInEditor:                 %d\n", CUSTOMROR::crInfo.configInfo.showHiddenTerrainsInEditor);
	fprintf_s(f, "showHiddenUnitsInEditor:                   %ld\n", CUSTOMROR::crInfo.configInfo.showHiddenUnitsInEditor);
	fprintf_s(f, "useCustomMapDimensionsInEditor:            %d\n", CUSTOMROR::crInfo.configInfo.useCustomMapDimensions);

	// Other
	fprintf_s(f, "civilizations count:                       %ld\n", CUSTOMROR::crInfo.configInfo.civCount);

	fprintf_s(f, "End of configuration.\n\n");
	fclose(f);
	return RORAPIFullyInstalled;
}


// Initialization that only takes place once, at executable startup.
void CustomRORCommand::OneShotInit() {
	// Collect information about EXE version
	this->ReadIfCustomSelectedUnitsMemoryZoneIsUsed();
	this->ReadIfManageAIIsOn();
	this->ReadOtherSequencesStatus();

	// Load/Prepare localization strings
	this->LoadCustomLocalizationFiles();

	// Manage interfac.drs file to use
	AOE_CONST_DRS::ChangeItfDRS_file();

	// Prepare custom DRS data
	this->LoadCustomDrsFiles();

	// Update "population limit getter" according to configuration
	SetMaxPopulationGetterInSPGames(CUSTOMROR::crInfo.configInfo.singlePlayerMaxPopulation);

#ifdef _DEBUG
	if (CR_DEBUG::debugSerialization) {
		SetDeserializationDebugChange(true);
		WriteToFile("", CR_DEBUG::serializationLogFilename, false); // reset file content
	}
#endif
}


// Reads game executable to determine if player struct is extended to use custom memory zone to host selected units
void CustomRORCommand::ReadIfCustomSelectedUnitsMemoryZoneIsUsed() {
	// TO DO: check all sequences ?
	CUSTOMROR::crInfo.hasCustomSelectedUnitsMemory = IsBinaryChangeOn(BINSEQ_CATEGORIES::BC_SELECTED_UNITS, "InitBuffer1");
}

// Reads game executable to determine if ManageAI is installed (does game use player->unused_customAIFlag ?)
void CustomRORCommand::ReadIfManageAIIsOn() {
	CUSTOMROR::crInfo.hasManageAIFeatureON = IsBinaryChangeOn(BINSEQ_CATEGORIES::BC_MANAGE_AI, "Init_is_computer_for_AI_1");
}


// Reads game executable to determine if various sequences are installed or not
void CustomRORCommand::ReadOtherSequencesStatus() {
	CUSTOMROR::crInfo.hasRemovePlayerInitialAgeInScenarioInit = IsBinaryChangeOn(BINSEQ_CATEGORIES::BC_ROR_API, "FixScenarioBadInitialAgeApplication_removeBad");
	CUSTOMROR::crInfo.hasFixForBuildingStratElemUnitId = IsBinaryChangeOn(BINSEQ_CATEGORIES::BC_ROR_API, "FixUnitIdForInProgressBuilding");
}

// Load custom strings files
void CustomRORCommand::LoadCustomLocalizationFiles() {
	for each (std::string filename in CUSTOMROR::crInfo.configInfo.customStringsFilesList) {
		std::string msg;
		if (!localizationHandler.LoadTranslationsFromFile(filename)) {
			msg = "Failed to load strings from ";
		} else {
			msg = "Successfully loaded strings from ";
		}
		msg += filename;
		traceMessageHandler.WriteMessageNoNotification(msg);
	}
}

// Load custom DRS files
void CustomRORCommand::LoadCustomDrsFiles() {
	for each (CONFIG::DrsFileToLoad *drs in CUSTOMROR::crInfo.configInfo.customDrsFilesList)
	{
		AOE_METHODS::AddDrsFile(drs->filename.c_str(), drs->folder.c_str());
		if (FindDrsLinkForFile(drs->filename.c_str())) {
			traceMessageHandler.WriteMessageNoNotification(drs->folder + std::string("\\") +drs->filename + std::string(" has been loaded"));
		} else {
			traceMessageHandler.WriteMessage(std::string("ERROR : ") + drs->folder + std::string("\\") + drs->filename + std::string(" could NOT be loaded"));
		}
	}

	// Prepare custom DRS data
	if (CUSTOMROR::crInfo.configInfo.useImprovedButtonBar) {
		if (CUSTOMROR::crInfo.configInfo.showAlertOnMissingFeature && !FindDrsLinkForFile(CST_CUSTOMROR_DRS_FILENAME)) {
			MessageBoxA(0, "ERROR : Could not find customROR.drs or it is invalid.", "CustomROR", MB_ICONWARNING);
		}
		// Initialize global variable so we can retrieve our button icons when needed
		AOE_METHODS::InitSlpInfoFromDrs(&CUSTOMROR::crInfo.customRorIcons, AOE_CONST_DRS::CST_CUSTOMROR_CMD_ICONS_SLP_ID);
		AOE_METHODS::InitSlpInfoFromDrs(&CUSTOMROR::crInfo.customRorUnitShortcuts, AOE_CONST_DRS::CST_CUSTOMROR_UNIT_SHORTCUTS_SLP_ID);
	}
}


// Get custom empires.dat filename (with relative path)
const char *CustomRORCommand::GetCustomEmpiresDatRelativeFileName(AOE_STRUCTURES::STRUCT_COMMAND_LINE_INFO *cmdLineInfo) {
	if (!CUSTOMROR::crInfo.configInfo.customEmpiresDatRelativePath.empty()) { // TODO use dedicated/correct config
		const char *filename = CUSTOMROR::crInfo.configInfo.customEmpiresDatRelativePath.c_str();
		if (CheckFileExistence(filename)) {
			return filename;
		}
		else {
			std::string msg = localizationHandler.GetTranslation(CRLANG_ID_ERROR_COULDNT_FIND, "ERROR : Could not find");
			msg += " ";
			msg += filename;
			msg += " ";
			msg += localizationHandler.GetTranslation(CRLANG_ID_ERROR_EMPIRES_DAT_USED_INSTEAD, "file. data2\\empires.dat will be used instead.");
			MessageBoxA(0, msg.c_str(), "CustomROR", MB_ICONWARNING);
		}
	}

	// Standard behaviour : use game's string
	return cmdLineInfo->relativePath_empires_dat;
}


// Execute a command from custom options window
// Returns true if the provided command is valid (and executed)
// After the call, output points to a char* string containing command's output, if any.
// Do NOT free output variable, it uses an internal buffer.
bool CustomRORCommand::ExecuteCommand(char *command, char **output) {
	static const char *PREFIX_SET = "set ";
	*output = outputBuffer;
	outputBuffer[0] = 0;

	if (!*command) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *gameSettings = GetGameSettingsPtr();

	if (!_strnicmp(command, PREFIX_SET, sizeof(PREFIX_SET))) {
		// This is a "set ___=___" command.
		char *subCmd = command + sizeof(PREFIX_SET);
		if (!_strnicmp(subCmd, "seed=", 5)) {
			subCmd += 5;
			int newSeed = atoi(subCmd);
			if (newSeed) {
				gameSettings->scenarioMapSeed = newSeed;
				gameSettings->actualMapSeed = newSeed;
				sprintf_s(outputBuffer, "Map seed=%d", newSeed);
			}
			return true;
		}
		if (!_strnicmp(subCmd, "maptype=", 8)) {
			subCmd += 8;
			int newType = StrToInt(subCmd, -1);
			if ((newType >= 0) && (newType < AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_MAP_TYPES_COUNT)) {
				gameSettings->mapTypeChoice = (AOE_CONST_FUNC::MAP_TYPE_INDEX) newType;
				sprintf_s(outputBuffer, "Map type=%d", newType);
			}
			return true;
		}

		if (!_strnicmp(subCmd, "control=", 8)) {
			subCmd += 8;
			int newId = StrToInt(subCmd, -1);
			if (((newId <= 0) && (newId < 9))) {
				AOE_METHODS::PLAYER::ChangeControlledPlayer(newId, true);
				sprintf_s(outputBuffer, "Controlled player#=%d", newId);
			}
			return true;
		}

		// Change maximum population value for this game (and next ones)
		if (!_strnicmp(subCmd, "maxpop=", 7)) {
			subCmd += 7;
			int newMaxPop = atoi(subCmd);
			if ((newMaxPop > 0) && (newMaxPop <= 255)) {
				if (SetMaxPopulationGetterInSPGames(newMaxPop)) {
					AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
					if (global && global->IsCheckSumValid() && global->ptrPlayerStructPtrTable) {
						for (int i = 0; i < global->playerTotalCount; i++) {
							AOE_STRUCTURES::STRUCT_PLAYER *player = global->ptrPlayerStructPtrTable[i];
							if (player && player->IsCheckSumValid()) {
								player->SetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_POPULATION_LIMIT, (float)newMaxPop);
							}
						}
					}
					sprintf_s(outputBuffer, "Max population=%d", newMaxPop);
				}
			}
			return true;
		}
	}

	// Useful for debugging too: select from unitId
	if (!_strnicmp(command, "select=", 7) && (command[7] >= '0') && (command[7] <= '9')) {
		char *sUnitId = command + 7;
		short int id = StrToInt(sUnitId, -1);
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(id);
		AOE_METHODS::PLAYER::ClearSelectedUnits(player);
		if (unit && unit->IsCheckSumValidForAUnitClass()) {
			AOE_METHODS::PLAYER::SelectUnit(player, unit, true);
			char *name = "?";
			if (unit->unitDefinition && unit->unitDefinition->IsCheckSumValidForAUnitClass()) {
				name = unit->unitDefinition->ptrUnitName;
			}
			std::string s = std::string("selected ") + std::string(name);
			sprintf_s(outputBuffer, s.c_str());
		}
	}

	// A good nice trick : add any unit at any (explored) location using scenario editor feature... in game !
	// Type addXXX where XXX is a unitDefId.
	if (!_strnicmp(command, "add", 3) && (command[3] >= '0') && (command[3] <= '9')) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		assert(player != NULL);
		char *aid = command + 3;
		short int id = atoi(aid);
		if ((id == 0) && (*aid != '0')) { id = -1; }
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = GetUnitDefStruct(player, id);
		if (player && player->IsCheckSumValid() && unitDef && unitDef->IsCheckSumValidForAUnitClass() && gameSettings->rgeGameOptions.isSinglePlayer) {
			gameSettings->mouseActionType = AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_EDITOR_SET_UNIT_LOCATION;
			gameSettings->editorUserSelectedUnitDefId = id;
			sprintf_s(outputBuffer, "Add %s. Right-click to quit add mode.", unitDef->ptrUnitName);
		}
	}

	if (!_strnicmp(command, "strat", 5)) {
		command += 5;
		int playerId = atoi(command);
		if ((playerId == 0) && (*command != '0')) { playerId = -1; }
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		if (global && global->IsCheckSumValid() && (playerId > 0) && (playerId < global->playerTotalCount)) {
			player = GetPlayerStruct(playerId);
		}
		if (player && player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid()) {
			std::string s = STRATEGY::ExportStrategyToText(&player->ptrAIStruct->structBuildAI);
			traceMessageHandler.WriteMessage(s);
			//this->OpenCustomDialogMessage(s.c_str(), 700, 550);
			sprintf_s(outputBuffer, "Close this window to see strategy.");
		}
		return true;
	}
#ifdef _DEBUG

	// BETA - TEST
	if (!_strnicmp(command, "test", 5)) {
		assert(gameSettings != NULL);
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = gameSettings->ptrGlobalStruct;
		//short int humanPlayerId = global->humanPlayerId;
		AOE_STRUCTURES::STRUCT_PLAYER *humanPlayer = GetControlledPlayerStruct_Settings();
		assert(humanPlayer != NULL);

		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDefLiving =
			//(AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *) GetUnitDefStruct(humanPlayer, 62);
			(AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *) GetUnitDefStruct(humanPlayer, CST_UNITID_SHORT_SWORDSMAN);
		if (unitDefLiving && unitDefLiving->IsCheckSumValidForAUnitClass()) {
			unitDefLiving->availableForPlayer = 1;
			unitDefLiving->trainButton = 13;
			unitDefLiving->trainLocation = 109;
			//unitDefLiving->blastLevel = AOE_CONST_FUNC::BLAST_LEVELS::CST_BL_DAMAGE_RESOURCES;
			//unitDefLiving->blastRadius = 5;
		}
		unitDefLiving = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *) GetUnitDefStruct(humanPlayer, 227);
		if (unitDefLiving && unitDefLiving->IsCheckSumValidForAUnitClass()) {
			unitDefLiving->availableForPlayer = 1;
			unitDefLiving->trainButton = 27;
			unitDefLiving->trainLocation = 109;
		}
	}

	char *c = "Game Settings Screen";
	//char *c = "MP Setup Screen";
	long int testvar;
	if (!_strnicmp(command, "test2", 6)) {
		_asm {
			// To show Game Settings Screen (need to catch onclick on ok/cancel to avoid crash and do custom code) - TO DO
			PUSH 0x560
			MOV EAX, 0x52601D // alloc
			CALL EAX
			ADD ESP, 4
			MOV testvar, EAX
			CMP EAX, 0
			JE fail
			MOV ECX, EAX
			MOV EAX, 0x49BF40
			CALL EAX
			MOV ECX, 0x5830E8
			PUSH 0
			MOV EAX, c
			PUSH EAX
			MOV EAX, 0x451BE0
			CALL EAX
		}
		return true;
	}

fail:
#endif
	return false;
}


void CustomRORCommand::HandleChatCommand(char *command) {
	if (strcmp(command, "hi") == 0) {
		AOE_METHODS::CallWriteText("Hello world !");
	}
	if (strcmp(command, "seed") == 0) {
		this->PrintMapSeed();
	}
	if ((strcmp(command, "t") == 0) || (strcmp(command, "time") == 0)) {
		this->PrintDateTime();
	}
	if ((strcmp(command, "ai") == 0) || (strcmp(command, "AI") == 0)) {
		AOE_METHODS::PLAYER::RestoreAllAIFlags();
		AOE_METHODS::CallWriteText(localizationHandler.GetTranslation(CRLANG_ID_AI_FLAGS_RECOMPUTED, "AI flags have been recomputed"));
	}
	if ((strcmp(command, "allai") == 0) || (strcmp(command, "allAI") == 0)) {
		AOE_METHODS::PLAYER::SetAllAIFlags();
		AOE_METHODS::CallWriteText(localizationHandler.GetTranslation(CRLANG_ID_AI_FLAGS_ALL_SET, "All players are now computer-managed."));
	}
	if (strcmp(command, "dump") == 0) {
#ifdef _DEBUG
		CR_DEBUG::DumpDebugInfoToFile();
#endif
	}
	if (strcmp(command, "about") == 0) {
		std::string s = localizationHandler.GetTranslation(CRLANG_ID_CUSTOMROR, "CustomROR");
		s += " ";
		s += VER_FILE_VERSION_STR;
		AOE_METHODS::CallWriteText(s.c_str());
	}
	if (strcmp(command, "timer stats") == 0) {
		this->DisplayTimerStats();
	}
	if (strcmp(command, "dat check") == 0) {
		AnalyzeEmpiresDatQuality();
	}

#ifdef _DEBUG
	// p0 to p8: quick player change + set AI flags automatically
	if ((*command == 'p') && (*(command + 1) >= '0') && (*(command + 1) < '9')) {
		int playerId = *(command + 1) - '0';
		AOE_METHODS::PLAYER::ChangeControlledPlayer(playerId, true);
	}

	// analyze ustom unit groups AI
	if (strcmp(command, "group") == 0) {
		std::string s = "";
		for (int i = 0; i < GetGameGlobalStructPtr()->playerTotalCount; i++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(i);
			if (player && player->IsCheckSumValid() && player->isComputerControlled) {
				if (CUSTOM_AI::customAIHandler.IsAliveAI(i)) {
					s += std::string("[p#") + std::to_string(i) + std::string("]");
					s += CUSTOM_AI::customAIHandler.GetCustomPlayerAI(i)->unitGroupAI.lastDebugInfo;
					s += "\r\n";
				}
			}
		}
		traceMessageHandler.WriteMessage(s);
	}
	if (strcmp(command, "pause") == 0) {
		if (GetGameGlobalStructPtr()->gameSpeed < 0.1f) {
			GetGameGlobalStructPtr()->gameSpeed = 0.5f;
		} else {
			GetGameGlobalStructPtr()->gameSpeed = 0.01f;
		}
	}

	if (strcmp(command, "assassin") == 0) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		assert((player != NULL) && player->IsCheckSumValid());
		if (!player || !player->IsCheckSumValid()) { return; }
		AOE_STRUCTURES::STRUCT_UNIT_BASE **unitArray = CUSTOMROR::crInfo.GetRelevantSelectedUnitsPointer(player);
		if (unitArray && unitArray[0] && unitArray[0]->IsCheckSumValidForAUnitClass()) {
			if (unitArray[0]->DerivesFromTrainable()) {
				GAME_COMMANDS::CreateCmd_KillUnit(unitArray[0]->unitInstanceId);
			}
		}
	}
#endif

#ifdef _DEBUG
	// TEST temp
	if (strcmp(command, "b") == 0) {
		long int playerId = 1;
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		if (!player || !player->ptrAIStruct) { return; }
		STRUCT_MANAGED_ARRAY *l = &player->ptrAIStruct->structInfAI.unknown_0F0;
		int c = l->usedElements;
		for (int i = 0; i < c; i++) {
			int id = l->unitIdArray[i];
			if (id == -1) { id = -2; }
		}

		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		assert((global != NULL) && global->IsCheckSumValid());
		if (!global || !global->IsCheckSumValid()) { return; }
		assert((global->gameMapInfo != NULL) && (global->gameMapInfo->IsCheckSumValid()));
		if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return; }
	}
	if (strcmp(command, "a") == 0) {
		long int playerId = 1;
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		if (!player || !player->ptrAIStruct) { return; }
		
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)PLAYER::FindUnitWithShortcutNumberForPlayer(player, 1);
		if (unitBase) {
			unitBase->AOE_destructor(true);
			return;
		}
		AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *type50 = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)unitBase;
		if (!unitBase || !unitBase->IsCheckSumValidForAUnitClass()) { return; }

		AOE_STRUCTURES::STRUCT_AI *ai = player->ptrAIStruct;
		AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = &ai->structTacAI;
		assert(tacAI->IsCheckSumValid());
	}

	// TEST strategy
	if (strcmp(command, "strat") == 0) {

	}
#endif

	// TEST conversion
	if (strcmp(command, "conversion") == 0) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		assert(global != NULL);
		for (int i = 1; i < global->playerTotalCount; i++) {
			int FOR_attempt = CUSTOMROR::crInfo.activeConversionAttemptsCount[i];
			int FOR_success = CUSTOMROR::crInfo.activeConversionSuccessfulAttemptsCount[i];
			float FOR_pct = FOR_attempt == 0 ? 0 : ((float)FOR_success) / ((float)FOR_attempt);
			int AGAINST_attempt = CUSTOMROR::crInfo.passiveConversionAttemptsCount[i];
			int AGAINST_success = CUSTOMROR::crInfo.passiveConversionSuccessfulAttemptsCount[i];
			float AGAINST_pct = AGAINST_attempt == 0 ? 0 : ((float)AGAINST_success) / ((float)AGAINST_attempt);
			char buffer[200];
			sprintf_s(buffer, "Conversions FOR p%d: %d/%d = %f%%   AGAINST p%d: %d/%d = %f%%.",
				i, FOR_success, FOR_attempt, FOR_pct * 100,
				i, AGAINST_success, AGAINST_attempt, AGAINST_pct * 100);
			AOE_METHODS::CallWriteText(buffer);

			FILE *f;
			int res = fopen_s(&f, "F:\\AOEDebug2.txt", "a+"); // overwrite if already existing
			if (!res) {
				sprintf_s(buffer, "Conversions FOR p%d: %d/%d = %f   AGAINST p%d: %d/%d = %f.",
					i, FOR_success, FOR_attempt, FOR_pct * 100,
					i, AGAINST_success, AGAINST_attempt, AGAINST_pct * 100);
				fprintf_s(f, buffer);
				fprintf_s(f, "\n");
				fclose(f);
			}

		}
	}
}


void CustomRORCommand::ShowF11_zone() {
	// Show automatically "F11" information at game startup
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid() || !settings->ptrGameUIStruct || !settings->ptrGameUIStruct->IsCheckSumValid()) {
		return;
	}
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI = settings->ptrGameUIStruct;
	AOE_STRUCTURES::STRUCT_ANY_UI *topleft = gameMainUI->topLeftInfoLabel;
	AOE_STRUCTURES::STRUCT_ANY_UI *popInfo = gameMainUI->populationInfoPanel;
	AOE_STRUCTURES::STRUCT_ANY_UI *playZone = gameMainUI->gamePlayUIZone;
	assert(topleft && popInfo && playZone);
	if (!topleft || !popInfo || !playZone) { return; }
	const unsigned long int addr1 = 0x004FAF80;
	const unsigned long int addr2 = 0x004839A0;
	_asm {
		MOV ECX, topleft;
		PUSH 0;
		PUSH 2;
		CALL addr1;
	}
	AOE_METHODS::UI_BASE::ShowUIObject(popInfo, true);
	_asm {
		MOV ECX, gameMainUI;
		CALL addr2;
	}
	AOE_METHODS::UI_BASE::RefreshUIObject(playZone, 1);
}


// UpdatedValue: if <0, it is ignored
void CustomRORCommand::UpdateWorkRateWithMessage(short int DATID, float updatedValue) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) {
		return;
	}
	bool firstOne = true;
	for (int civId = 0; civId < global->civCount; civId++) {
		AOE_STRUCTURES::STRUCT_CIVILIZATION_DEF *civDef = global->civilizationDefinitions[civId];
		if (civDef && civDef->IsCheckSumValid()) {
			AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)civDef->GetUnitDef(DATID);
			char msgBuffer[100];
			if (unitDef && unitDef->DerivesFromCommandable()) {
				if (firstOne) { // Message only once
					if (updatedValue < 0) {
						sprintf_s(msgBuffer, "%s (%d): work rate=%f", unitDef->ptrUnitName, unitDef->DAT_ID1, unitDef->workRate);
					} else {
						sprintf_s(msgBuffer, "%s (%d): work rate=%f updated to %f", unitDef->ptrUnitName, unitDef->DAT_ID1, unitDef->workRate, updatedValue);
					}
					traceMessageHandler.WriteMessageNoNotification(msgBuffer);
					firstOne = false;
				}
				if (updatedValue >= 0) {
					unitDef->workRate = updatedValue;
				}
			}
		}
	}
}


// Update a technology that increases work rate: change value used to increase.
// UpdatedValue: if <0, it is ignored
void CustomRORCommand::UpdateTechAddWorkRateWithMessage(short int techId, short int unitDefId, float updatedValue) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) {
		return;
	}
	AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *techInfo = global->technologiesInfo;
	if (techInfo && techInfo->IsCheckSumValid() && techInfo->technologyCount >= 111) {
		AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = techInfo->GetTechDef(techId);
		for (int i = 0; i < techDef->effectCount; i++) {
			if ((techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) &&
				(techDef->ptrEffects[i].effectUnit == unitDefId) &&
				(techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_WORK_RATE) // working rate
				) {
				char msgBuffer[100];
				if (updatedValue < 0) {
					sprintf_s(msgBuffer, "%s (%d): tech work rate add.=%f", GetTechnologyLocalizedName(techId).c_str(), techId, techDef->ptrEffects[i].effectValue);
				} else {
					sprintf_s(msgBuffer, "%s (%d): tech work rate add.=%f updated to %f", GetTechnologyLocalizedName(techId).c_str(), techId, techDef->ptrEffects[i].effectValue, updatedValue);
				}
				traceMessageHandler.WriteMessageNoNotification(msgBuffer);
				techDef->ptrEffects[i].effectValue = updatedValue;
			}
		}
	}
}


// This is called just after empires.dat is loaded.
// Warning: changes here are applied on civ definitions are done once and for all, and impact all games.
void CustomRORCommand::OnAfterLoadEmpires_DAT() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global);
	if (!global || !global->IsCheckSumValid()) { return; }
	// Show hidden units in editor
	assert(global->civCount < 256); // it is a short (2 bytes)
	if (CUSTOMROR::crInfo.configInfo.showHiddenUnitsInEditor > 0) {
		bool excludeAnnoyingUnits = (CUSTOMROR::crInfo.configInfo.showHiddenUnitsInEditor == 1);
		for (int civid = 0; civid < global->civCount; civid++) {
			AOE_STRUCTURES::STRUCT_CIVILIZATION_DEF *civDef = global->civilizationDefinitions[civid];
			if (civDef && civDef->IsCheckSumValid()) {
				for (int datid = 0; datid < civDef->civUnitDefCount; datid++) {
					AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = civDef->GetUnitDef(datid);
					// If config says to exclude annoying units, test if current unitDef is one of banned ones
					if (unitDef && unitDef->IsCheckSumValidForAUnitClass()) {
						bool doNotShowThisOne = excludeAnnoyingUnits && (
							(unitDef->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_BARRACKS_2_UPGRADE) ||
							(unitDef->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_TC_2_UPGRADE) ||
							(unitDef->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_STABLE_2_UPGRADE) ||
							(unitDef->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_STORAGE_PIT_2_UPGRADE) ||
							(unitDef->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_MARKET_2_UPGRADE) ||
							(unitDef->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_MAN2) ||
							(unitDef->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_DOCK_2_UPGRADE) ||
							(unitDef->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_HOUSE_2_UPGRADE)
							);
						if (!doNotShowThisOne) {
							unitDef->hideInEditor = 0;
						}
					}
				}
			}
		}
	}

	// Assign graphics to the invisible tree.
	// Note that adding this at game start does not work, it's too late.
	if (CUSTOMROR::crInfo.configInfo.fixInvisibleTree) {
		for (int civid = 0; civid < global->civCount; civid++) {
			AOE_STRUCTURES::STRUCT_CIVILIZATION_DEF *civDef = global->civilizationDefinitions[civid];
			if (civDef && civDef->IsCheckSumValid()) {
				if (civDef->civUnitDefCount > 393) {
					// add condition SLP=799 ?
					AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefDest = civDef->GetUnitDef(393); // this tree has graphics with invalid SLP (799)
					AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefSrc = civDef->GetUnitDef(391);
					if (unitDefDest && unitDefSrc && unitDefDest->IsCheckSumValidForAUnitClass() && unitDefSrc->IsCheckSumValidForAUnitClass()) {
						unitDefDest->ptrStandingGraphics = unitDefSrc->ptrStandingGraphics;
					}
				}
			}
		}
	}

	// Fix gaia attack alert sound once and for all (can be applied on all games, including MP/Scenario, doesn't affect anything).
	if (global->civCount > 0) {
		AOE_STRUCTURES::STRUCT_CIVILIZATION_DEF *civDefGaia = global->civilizationDefinitions[0];
		if (civDefGaia->civResourcesCount > CST_RES_ORDER_ATTACK_ALERT_SOUND_ID) {
			civDefGaia->SetResourceValue(CST_RES_ORDER_ATTACK_ALERT_SOUND_ID, 10);
		}
	}

	// Fix villager work rates
	if (CUSTOMROR::crInfo.configInfo.fixVillagerWorkRates && (global->civCount > 1)) {
		traceMessageHandler.WriteMessageNoNotification("Fixing villager work rates");
		this->UpdateWorkRateWithMessage(CST_UNITID_VILLAGER, -1); // Original=1
		this->UpdateWorkRateWithMessage(CST_UNITID_VILLAGER2, -1); // Original=1
		this->UpdateWorkRateWithMessage(CST_UNITID_BUILDER, -1); // Original=1
		this->UpdateWorkRateWithMessage(CST_UNITID_FISHERMAN, (float)0.45); // ROR original is 0.6
		this->UpdateWorkRateWithMessage(CST_UNITID_FORAGER, -1); // Original=0.45
		this->UpdateWorkRateWithMessage(CST_UNITID_HUNTER, -1); // Original=0.45
		this->UpdateWorkRateWithMessage(CST_UNITID_LUMBERJACK, -1); // Original=0.55
		this->UpdateWorkRateWithMessage(CST_UNITID_MINERSTONE, -1); // Original=0.45
		this->UpdateWorkRateWithMessage(CST_UNITID_EXPLORER, -1); // Original=1
		this->UpdateWorkRateWithMessage(CST_UNITID_REPAIRMAN, -1); // Original=0.4
		this->UpdateWorkRateWithMessage(CST_UNITID_MINERGOLD, -1); // Original=0.45
		this->UpdateWorkRateWithMessage(CST_UNITID_FARMER, -1); // Original=0.45
		// Note for upgrades (techs): if using another value than 0.15, also fix egypt/baby tech trees ! (+palmyra, see below)
		// Mining: could use +0.09 (would be +20%) ?
		const float miningBonus = 0.15f;
		const float lumberjackBonus = 0.11f;
		const float palmyraWorkRateBonus = 0.15f; // enough, even better than tech bonuses. Original = 0.20 for all gatherer types
		// TODO: bonus % is not the same for all villager types (most have 0.45 work rate, lumberjack 0.55, etc)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_STONE_MINING, CST_UNITID_MINERSTONE, miningBonus); // +33% instead of +67% (was +0.3 / 0.45 initial)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_GOLD_MINING, CST_UNITID_MINERGOLD, miningBonus); // +33% instead of +67% (was +0.3 / 0.45 initial)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_SIEGECRAFT, CST_UNITID_MINERSTONE, miningBonus); // +33% instead of +67% (was +0.3 / 0.45 initial). 2nd tech for stone mining (iron age)
		// Wood cutting work rate improvements :
		// WARNING: there is a hardcoded addition of -0.15 (decrease, actually) to phoenician to compensate too big bonus on lumberjack ! See 0x50B883.
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_WOODWORKING, CST_UNITID_LUMBERJACK, lumberjackBonus); // +20% instead of +36% (+0.2 / 0.55 initial)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_ARTISANSHIP, CST_UNITID_LUMBERJACK, lumberjackBonus); // +20% instead of +36% (+0.2 / 0.55 initial)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_CRAFTSMANSHIP, CST_UNITID_LUMBERJACK, lumberjackBonus); // +20% instead of +36% (+0.2 / 0.55 initial)
		// Palmyra tech tree: adjust villager work rate bonuses
		AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *techDefInfo = global->technologiesInfo;
		if (techDefInfo && techDefInfo->IsCheckSumValid() && (techDefInfo->technologyCount > CST_TCH_TECH_TREE_PALMYRA)) {
			AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = techDefInfo->GetTechDef(CST_TCH_TECH_TREE_PALMYRA);
			for (int i = 0; i < techDef->effectCount; i++) {
				if ((techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) &&
					(techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_WORK_RATE) && // work rate
					// For villagers OR farm (refer to palmyra tech tree !)
					((IsVillager(techDef->ptrEffects[i].effectUnit) || (techDef->ptrEffects[i].effectUnit == CST_UNITID_FARM)) &&
					(techDef->ptrEffects[i].effectValue > palmyraWorkRateBonus))
					) {
					float newValue = palmyraWorkRateBonus;
					std::string msg = "Palmyra tech tree: gathering bonus changed from ";
					msg += std::to_string(techDef->ptrEffects[i].effectValue);
					msg += " to ";
					msg += std::to_string(newValue);
					msg += " for ";
					AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = NULL;
					if (global->civCount > 1) { // Use an arbitrary civ, just to get unit name.
						unitDef = global->civilizationDefinitions[0]->GetUnitDef(techDef->ptrEffects[i].effectUnit);
					}
					if (unitDef && unitDef->IsCheckSumValidForAUnitClass()) {
						char tmpbuf[80];
						GetLanguageDllText(unitDef->languageDLLID_Name, tmpbuf, sizeof(tmpbuf), unitDef->ptrUnitName);
						msg += tmpbuf;
					} else {
						msg += "unitDefId=";
						msg += std::to_string(techDef->ptrEffects[i].effectUnit);
					}
					traceMessageHandler.WriteMessageNoNotification(msg);
					techDef->ptrEffects[i].effectValue = newValue;
				}
			}
		}
	}

	// Override resource values according to configuration
	for (int civid = 0; civid < global->civCount; civid++) {
		AOE_STRUCTURES::STRUCT_CIVILIZATION_DEF *civDef = global->civilizationDefinitions[civid];
		if (civDef && civDef->IsCheckSumValid()) {
			for (int unitDefId = 0; unitDefId < civDef->civUnitDefCount; unitDefId++) {
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = civDef->GetUnitDef(unitDefId);
				if (unitDef && unitDef->IsCheckSumValidForAUnitClass()) {
					switch (unitDef->unitAIType) {
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupBerryBush:
						if (CUSTOMROR::crInfo.configInfo.unitResourceAmountBerryBush > 0) {
							unitDef->resourceCapacity = CUSTOMROR::crInfo.configInfo.unitResourceAmountBerryBush;
							unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSeaFish:
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupShoreFish:
						if (CUSTOMROR::crInfo.configInfo.unitResourceAmountFish > 0) {
							unitDef->resourceCapacity = CUSTOMROR::crInfo.configInfo.unitResourceAmountFish;
							unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupGoldMine:
						if (CUSTOMROR::crInfo.configInfo.unitResourceAmountGoldMine > 0) {
							unitDef->resourceCapacity = CUSTOMROR::crInfo.configInfo.unitResourceAmountGoldMine;
							unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupStoneMine:
						if (CUSTOMROR::crInfo.configInfo.unitResourceAmountStoneMine > 0) {
							unitDef->resourceCapacity = CUSTOMROR::crInfo.configInfo.unitResourceAmountStoneMine;
							unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPreyAnimal:
						if (unitDefId == CST_UNITID_GAZELLE) { // Exclude gazelle king, horse
							if (CUSTOMROR::crInfo.configInfo.unitResourceAmountGazelle > 0) {
								unitDef->resourceCapacity = CUSTOMROR::crInfo.configInfo.unitResourceAmountGazelle;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPredatorAnimal:
						if (unitDefId == CST_UNITID_LION) {
							if (CUSTOMROR::crInfo.configInfo.unitResourceAmountLion > 0) {
								unitDef->resourceCapacity = CUSTOMROR::crInfo.configInfo.unitResourceAmountLion;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						}
						if (unitDefId == CST_UNITID_ALLIGATOR) {
							if (CUSTOMROR::crInfo.configInfo.unitResourceAmountAlligator > 0) {
								unitDef->resourceCapacity = CUSTOMROR::crInfo.configInfo.unitResourceAmountAlligator;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						}
						if (unitDefId == CST_UNITID_ELEPHANT) {
							if (CUSTOMROR::crInfo.configInfo.unitResourceAmountElephant > 0) {
								unitDef->resourceCapacity = CUSTOMROR::crInfo.configInfo.unitResourceAmountElephant;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupTree:
						// There is no way finding automatically if trees are "normal" or forest.
						if (TreeUnitIsForest(unitDefId)) {
							// Forest (default 40 wood)
							if (CUSTOMROR::crInfo.configInfo.unitResourceAmountTreeForest > 0) {
								unitDef->resourceCapacity = CUSTOMROR::crInfo.configInfo.unitResourceAmountTreeForest;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						} else {
							// Basic trees (default 75 wood)
							if (CUSTOMROR::crInfo.configInfo.unitResourceAmountTree > 0) {
								unitDef->resourceCapacity = CUSTOMROR::crInfo.configInfo.unitResourceAmountTree;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						}
					default:
						break;
					}
				}
			}
		}
	}
}


// Called when launching a new game. Game info has NOT been loaded yet. Global structure already exists, but be careful with its data...
// Most structures have not been initialized/loaded yet.
// This method should only reset internal data
void CustomRORCommand::OnNewGame(bool isSavedGame) {
	// Resets internal variables.
	this->InitMyGameInfo();

	if (CUSTOMROR::crInfo.configInfo.doNotApplyFixes) { return; }
	// Manage game settings customization
	this->ApplyCustomizationOnRandomGameSettings();
}


// Called on every game start, including RM, DM, campaign/scenario, load game, and even MP games.
// This occurs after everything has been loaded
// Note: normally, OnNewGame(...) has always been called just before.
// Warning: for scenarios, if there is an introduction screen, this method is called at that moment
// (game might not be displayed yet - and some methods may not be used yet)
void CustomRORCommand::OnGameStart() {
	// Make sure human player(s) do NOT have AI flags ON at startup (due to ManageAI feature)
	AOE_METHODS::PLAYER::DisableAIFlagsForHuman();

	// Do not allow using customROR on multiplayer games (however, always restore AI flags or human-controlled players will have an active AI !)
	if (IsMultiplayer()) { return; }

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (global && !global->IsCheckSumValid()) {
		global = NULL;
		assert(false && "Bad global checksum");
	}
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = NULL;
	if (global) {
		scInfo = global->scenarioInformation;
		if (scInfo && !scInfo->IsCheckSumValid()) {
			assert(false && "Bad checksum for scenario information");
			scInfo = NULL;
		}
	}

	// Fix IsScenario flag to correct value (not set when a game is loaded)
	if (settings->isSavedGame) {
		settings->rgeGameOptions.isScenario = 0; // Default: consider it is not a scenario
		// Guess if it is a scenario from scenario information
		if (scInfo && scInfo->IsCheckSumValid()) {
			if (scInfo->scenarioFileName[0] != 0) {
				// scenarioFileName is reliable: it is always correctly set: when starting a new RM/DM/scenario, loading a saved game, etc
				settings->rgeGameOptions.isScenario = 1;
			}

			// Fix isDeathMatch flag in saved games
			int initialFood = scInfo->startingResources[1][CST_RES_ORDER_FOOD]; // Warning: by default, this is wrong (too) in DM games !!! Requires customROR's fix
			if (initialFood > 2000) {
				settings->isDeathMatch = 1;
			}
		}
		if (global) {
			for (int i = 0; i < global->playerTotalCount; i++) {
				AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStruct(i);
				if (player && player->IsCheckSumValid()) {
					if (scInfo && scInfo->fullTechTree) {
						player->techTreeId = -1;
					} else {
						player->techTreeId = -2; // NOT set when loading a game (not serialized in savegame format) ! Do not use -1 (generally means all tech)
						// Try to get the correct tech tree id from civ definition. Risky when playing saved games from a different DAT version.
						if ((player->civilizationId >= 0) && (player->civilizationId < global->civCount)) {
							player->techTreeId = global->civilizationDefinitions[player->civilizationId]->techTreeId;
						}
					}
				}
			}
		}
	} else {
		if (scInfo && settings->allTechs) {
			scInfo->fullTechTree = 1;
			for (int i = 0; i < _countof(scInfo->playersStartingAge); i++) {
				switch (settings->initialAgeChoice) {
					// Warning: scInfo->playersStartingAge values are a different enum (0-4)
				case GAME_INITIAL_AGES::GIA_TOOL:
					scInfo->playersStartingAge[i] = 1;
					break;
				case GAME_INITIAL_AGES::GIA_BRONZE:
					scInfo->playersStartingAge[i] = 2;
					break;
				case GAME_INITIAL_AGES::GIA_IRON:
					scInfo->playersStartingAge[i] = 3;
					break;
				case GAME_INITIAL_AGES::GIA_POST_IRON:
					scInfo->playersStartingAge[i] = 4;
					break;
				default:
					// Others: nomad/stone/default
					scInfo->playersStartingAge[i] = 0;
				}
			}
		}
	}

	// Check AI initialization for loaded games (fix a saved game bug)
	this->FixGameStartAIInitForPlayers();

	this->DisableWaterUnitsIfNeeded();

	// Fix fire galley icon on post-iron age start games (research done notification - for trireme - is NOT triggered in such case !)
	assert(settings != NULL);
	if (!settings) { return; } // Should never happen
	if (settings->initialAgeChoice == AOE_CONST_INTERNAL::GAME_INITIAL_AGES::GIA_POST_IRON) { // Post-iron age startup
		for (int playerId = 1; playerId < 9; playerId++) {
			this->MoveFireGalleyIconIfNeeded(playerId);
		}
	}

	// Manage game settings customization for new random games
	this->ApplyCustomizationOnRandomGameStart();

	// Fix Initial resources in Scenario information (after applying customization, because it includes setting initial resource values.
	if (!settings->isSavedGame && global) {
		if (scInfo && scInfo->IsCheckSumValid()) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStruct(1);
			if (!player) {
				player = global->GetPlayerStruct(0);
				assert(false && "Could not find player 1");
			}
			for (int i = 0; i < _countof(scInfo->startingResources); i++) {
				scInfo->startingResources[i][CST_RES_ORDER_FOOD] = (long int)player->GetResourceValue(CST_RES_ORDER_FOOD);
				scInfo->startingResources[i][CST_RES_ORDER_WOOD] = (long int)player->GetResourceValue(CST_RES_ORDER_WOOD);
				scInfo->startingResources[i][CST_RES_ORDER_STONE] = (long int)player->GetResourceValue(CST_RES_ORDER_STONE);
				scInfo->startingResources[i][CST_RES_ORDER_GOLD] = (long int)player->GetResourceValue(CST_RES_ORDER_GOLD);
			}
		}
	}

	// Initialize custom AI objects
	CUSTOM_AI::customAIHandler.GameStartInit();

	// Triggers
	CUSTOMROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_GAME_START);

	// Special trigger: disable units for house, villager, fishing ship, trade ship (exceptions that don't work because of techs that do several things)
	// For farm, see notify event method (managed on market construction notification).
	CUSTOMROR::TRIGGER::ManageTriggerDisableUnitsForExceptions();

	long int **p = (long int **)AOE_OFFSETS::ADDR_VAR_ACTIVE_UI_STRUCT;
	if ((*p) && (**p != 0x0054679C)) {
		// This is NOT game screen, do not try to interact with it !
		return;
	}

	// REQUIRES game UI to be active
	if (!CUSTOMROR::crInfo.configInfo.hideWelcomeMessage && !settings->rgeGameOptions.isMultiPlayer) {
		std::string msg = localizationHandler.GetTranslation(CRLANG_ID_WELCOME1, "Welcome. CustomROR");
		msg += " "; 
		msg += VER_FILE_VERSION_STR;
		msg += " ";
		msg += localizationHandler.GetTranslation(CRLANG_ID_WELCOME2, "plugin is active.");
		AOE_METHODS::CallWriteText(msg.c_str());
	}

	// Show automatically "F11" information at game startup
	if (!settings->rgeGameOptions.isMultiPlayer) {
		this->ShowF11_zone();
	}

	// Force shared exploration => always ON (if config says so and not in MP)
	// Does not impact scenarios
	if (!settings->isCampaign && !settings->rgeGameOptions.isScenario) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
		if (CUSTOMROR::crInfo.configInfo.allyExplorationIsAlwaysShared &&
			!settings->rgeGameOptions.isMultiPlayer && global && global->IsCheckSumValid()) {
			for (int i = 1; i < global->playerTotalCount; i++) {
				SetPlayerSharedExploration_safe(i);
				//ApplyResearchForPlayer(player, CST_RSID_WRITING);
				// Could disable the research ?
			}
		}
	}

#ifdef _DEBUG
	CR_DEBUG::WriteDebugLogForDeserializedData(0, NULL, 0); // Force flush if there is remaining output in buffer
#endif
}


// Does all custom stuff on random maps / deathmatches before game start : changes on game settings (map type/size, etc)
// Does NOT apply to scenario/campaign/load saved game.
// Warning: most game structures are not available yet ! It is recommended to only work with STRUCT_GAME_SETTINGS, nothing else.
// Return false if failed.
bool CustomRORCommand::ApplyCustomizationOnRandomGameSettings() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	if (settings->isCampaign || settings->isSavedGame || settings->rgeGameOptions.isScenario) {
		return false;
	}

	// Do custom stuff on "settings" here...

	return true;
}


// Set a SN number value in both strategy AI and tac AI.
void CustomRORCommand::SetSNNumberInStrategyAndTacAI(AOE_STRUCTURES::STRUCT_AI *ai, AOE_CONST_FUNC::SN_NUMBERS snNumber, long int value) {
	if (!ai || !ai->IsCheckSumValid()) { return; }
	assert((snNumber >= 0) && (snNumber <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER));
	if ((snNumber < 0) || (snNumber > AOE_CONST_FUNC::CST_LAST_SN_NUMBER)) { return; }
	ai->structStrategyAI.SNNumber[snNumber] = value;
	ai->structTacAI.SNNumber[snNumber] = value;
}


// Does all custom stuff on random maps / deathmatches at game start : custom personality values, strategy, initial resources, etc.
// These are changes that are applied when game is loaded (do not interfere here with settings like map size, etc)
// Does NOT apply to scenario/campaign/load saved game.
// Return false if failed.
bool CustomRORCommand::ApplyCustomizationOnRandomGameStart() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return false; }
	
	// Check game type : allow only random map & deathmatch
	if (settings->isCampaign || settings->isSavedGame || settings->rgeGameOptions.isScenario) {
		return false;
	}
#pragma message("Customization on random game start: MP not supported")
	if (settings->rgeGameOptions.isMultiPlayer) {
		return false;
	}
	bool isDM = (settings->isDeathMatch != 0);

	if (CUSTOMROR::crInfo.configInfo.noWalls) {
		this->DisableWalls();
	}

	// Initial resources : read from config & apply to game
	long int initialResources[4] = { 200, 200, 150, 0 };
	if (isDM) {
		for (int i = 0; i < 4; i++) {
			initialResources[i] = CUSTOMROR::crInfo.configInfo.initialResources_DM[i];
		}
	} else {
		int choice = settings->initialResourcesChoice;
		if ((choice < 0) || (choice > 3)) { choice = 0; } // Make sure we have a valid choice 0-3
		for (int i = 0; i < 4; i++) {
			initialResources[i] = CUSTOMROR::crInfo.configInfo.initialResourcesByChoice_RM[choice][i];
		}
	}
	for (long int playerId = 1; playerId <= settings->rgeGameOptions.playerCountWithoutGaia; playerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		for (int rt = 0; rt < 4; rt++) {
			player->SetResourceValue((AOE_CONST_FUNC::RESOURCE_TYPES) rt, (float)initialResources[rt]);
		}
	}

	// SN Numbers (update both strategyAI and tacAI)
	for (long int playerId = 1; playerId <= settings->rgeGameOptions.playerCountWithoutGaia; playerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		if (player && player->IsCheckSumValid() && player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid()) {
			AOE_STRUCTURES::STRUCT_AI *ai = player->ptrAIStruct;
			
			if (isDM) {
				// Deathmatch
				for (int i = 0; i <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER; i++) {
					if (CUSTOMROR::crInfo.configInfo.defaultPerNumbers_DM_isSet[i]) {
						this->SetSNNumberInStrategyAndTacAI(ai, (AOE_CONST_FUNC::SN_NUMBERS)i,
							CUSTOMROR::crInfo.configInfo.defaultPerNumbers_DM[i]);
					}
				}
			} else {
				// Random map
				for (int i = 0; i <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER; i++) {
					if (CUSTOMROR::crInfo.configInfo.defaultPerNumbers_RM_isSet[i]) {
						this->SetSNNumberInStrategyAndTacAI(ai, (AOE_CONST_FUNC::SN_NUMBERS)i,
							CUSTOMROR::crInfo.configInfo.defaultPerNumbers_RM[i]);
					}
				}
			}

			// Manage the 4 "hardest difficulty" SN numbers
			if (settings->difficultyLevelChoice == 0) {
				this->SetSNNumberInStrategyAndTacAI(ai, SNInitialFood,
					isDM ? CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_FOOD] :
					CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_FOOD]);
				this->SetSNNumberInStrategyAndTacAI(ai, SNInitialWood,
					isDM ? CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_WOOD] :
					CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_WOOD]);
				this->SetSNNumberInStrategyAndTacAI(ai, SNInitialStone,
					isDM ? CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_STONE] :
					CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_STONE]);
				this->SetSNNumberInStrategyAndTacAI(ai, SNInitialGold,
					isDM ? CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_GOLD] :
					CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
			} else {
				// Not hardest : disable the bonus for AI
				this->SetSNNumberInStrategyAndTacAI(ai, SNInitialFood, 0);
				this->SetSNNumberInStrategyAndTacAI(ai, SNInitialWood, 0);
				this->SetSNNumberInStrategyAndTacAI(ai, SNInitialStone, 0);
				this->SetSNNumberInStrategyAndTacAI(ai, SNInitialGold, 0);
			}
		}
	}

	// Initial diplomacy (default=enemy, not neutral) - only if enabled in config
	if (CUSTOMROR::crInfo.configInfo.noNeutralInitialDiplomacy) {
		for (long int playerId = 1; playerId <= settings->rgeGameOptions.playerCountWithoutGaia; playerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
			if (player && player->IsCheckSumValid()) {
				if (settings->rgeGameOptions.teamNumber[playerId - 1] == 1) { // Note: teamNumber 1 means "no team"
					// No team: set default diplomacy (game default is neutral, can be set to enemy to avoid all players being against human)
					assert(player->ptrDiplomacyStances != NULL);
					for (int otherPlayerId = 1; otherPlayerId <= settings->rgeGameOptions.playerCountWithoutGaia; otherPlayerId++) {
						if ((otherPlayerId != playerId) && player->ptrDiplomacyStances) {
							player->ptrDiplomacyStances[otherPlayerId] = AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ENEMY;
							player->diplomacyVSPlayers[otherPlayerId] = AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES::CST_PDV_ENEMY;
							if (player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid()) {
								// It's crucial to update AI structure (remove the "is neutral" status), otherwise AI might update diplomacy back to neutral
								player->ptrAIStruct->structDiplAI.isNeutral[otherPlayerId] = 0;
							}
						}
					}
				}
			}
		}
	}

	// Strategy
	if ((!settings->isDeathMatch && CUSTOMROR::crInfo.configInfo.generateStrategyForRM) ||
		(settings->isDeathMatch && CUSTOMROR::crInfo.configInfo.generateStrategyForDM)) {
		if (settings->isDeathMatch) {
			traceMessageHandler.WriteMessage("Strategy generation for deathmatch games is not supported yet");
		} else {
			for (long int playerId = 1; playerId <= settings->rgeGameOptions.playerCountWithoutGaia; playerId++) {
				AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
				if (player && player->IsCheckSumValid()) {
					STRATEGY::StrategyBuilder sb = STRATEGY::StrategyBuilder(&CUSTOMROR::crInfo, player);
					sb.CreateStrategyFromScratch();
				}
			}
		}
	}

	return true;
}


// Initialize internal game-specific variables (called on each game start/load)
void CustomRORCommand::InitMyGameInfo() {
	CUSTOMROR::crInfo.ResetVariables();
	CUSTOM_AI::customAIHandler.ResetAllInfo(); // Reset all CusotmROR AI internal structures/info
	
	if (CUSTOMROR::crInfo.configInfo.doNotApplyFixes) { return; }
	// Prevent 0% speed at game startup (occurs because of rounding in registry saved value)
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (global && global->IsCheckSumValid() && (global->gameSpeed == 0)) {
		global->gameSpeed = 1;
	}
}


// This fixes nextStrategyAIExecutionCounter flag for all players (useful for loaded games)
void CustomRORCommand::FixGameStartAIInitForPlayers() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStructPtrTable()[loopPlayerId];
		if ((player->ptrAIStruct != NULL) && !this->FindIfGameStartStrategyInitHasBeenDone(player)) {
			// It seems we found a player that has not had his "game start AI init", including addition of dynamic strategy elements.
			// Force the flag to trigger it now (in fact, init will be triggered when AI control is enabled).
			player->ptrAIStruct->needGameStartAIInit = 1;
			// Note: when strategy elements are added to strategy, we'll need to run UpdateStrategyWithUnreferencedExistingUnits
			// if we want AI not to rebuild already existing houses (+docks, boats...)
		}
	}
}


// Returns true if AI file selection was overriden (do NOT let normal code be executed), false to let normal code be executed
bool CustomRORCommand::ManageAIFileSelectionForPlayer(char civilizationId, char *aiFileBuffer) {
	// Standard civ: continue "player.chooseAIFileName" function normally
	if ((civilizationId > 0) && (civilizationId <= CIVILIZATIONS::CST_CIVID_STANDARD_MAX)) {
		return false;
	}
	assert(aiFileBuffer != NULL);
	if (!aiFileBuffer) { return false; }

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *gameSettings = GetGameSettingsPtr();
	assert(gameSettings != NULL);
	CivilizationInfo *civ = CUSTOMROR::crInfo.configInfo.GetCivInfo(civilizationId);
	std::string s;
	int size = 0;
	int randomChoice = 0;
	if (gameSettings->isDeathMatch) {
		if (civ) {
			size = civ->deathmatch_AI_file.size();
			if (size > 0) {
				randomChoice = randomizer.GetRandomValue(0, size - 1);
				s = civ->deathmatch_AI_file.at(randomChoice);
				strcpy_s(aiFileBuffer, 255, s.c_str());
			}
		}
	} else {
		// RM or scenario (for players that don't have a strategy).
		if (civ) {
			switch (gameSettings->mapTypeChoice) {
			case 0:
			case 1:
				size = civ->RM_AI_file_much_water.size();
				if (size > 0) {
					randomChoice = randomizer.GetRandomValue(0, size - 1);
					s = std::string(civ->RM_AI_file_much_water.at(randomChoice));
				}
				break;
			case 4:
			case 7:
				size = civ->RM_AI_file_no_water.size();
				if (size > 0) {
					randomChoice = randomizer.GetRandomValue(0, size - 1);
					s = std::string(civ->RM_AI_file_no_water.at(randomChoice));
				}
				break;
			default:
				size = civ->RM_AI_file_some_water.size();
				if (size > 0) {
					randomChoice = randomizer.GetRandomValue(0, size - 1);
					s = std::string(civ->RM_AI_file_some_water.at(randomChoice));
				}
			}
			strcpy_s(aiFileBuffer, 255, s.c_str());
		}
	}

	// Crucial check: The game freezes if we choose an invalid file, because it will call this again and again.
	char bufFileName[512];
	sprintf_s(bufFileName, "%s\\%s", gameSettings->gameDirFullPath, aiFileBuffer);
	if (!CheckFileExistence(bufFileName)) {
		std::string msg = std::string("Error: Could not find file: ") + bufFileName;
		traceMessageHandler.WriteMessage(msg);
		*aiFileBuffer = 0; // clear filename because it's invalid.
	}
	return true;
}


// Process event when (human) player discovers a unit. This is where gaia units are captured by human player.
// This happens only once ! Once an objects has been discovered, this event is never triggered again.
// Return true if the unit must NOT be captured
// Default: return false (normal ROR treatments)
bool CustomRORCommand::HumanSpecific_onCapturableUnitSeen(AOE_STRUCTURES::STRUCT_UNIT_BASE *beingSeenUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer) {
	if (!beingSeenUnit || !beingSeenUnit->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = beingSeenUnit->unitDefinition;
	if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) {
		return false;
	}

	CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
	evtInfo.unitId = beingSeenUnit->unitInstanceId;
	evtInfo.playerId = actorPlayer->playerId;
	CUSTOMROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_DISCOVER_GAIA_UNIT, evtInfo);

	if (CUSTOMROR::crInfo.configInfo.fixHumanPlayer_specificSeeUnit &&
		(unitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact)) {
		// Do not capture artefacts in human-specific section, let "normal"/common code do it.
		// This prevents human from capturing artefacts faster than AI players.
		// Moreover, this fix is required by discovery resource update bug. (see DiscoveryAction_GetFoundByPlayerFlagsFromPreviousAction)
		// We could also disable the call for animals (they are not captured !)
		return true;
	}

	return false;
}


// Detects if game start strategy init has been done because nextStrategyAIExecutionCounter can be wrong with saved games
// (savegame file format does NOT save this information).
// Returns true if we think this init has been done. False otherwise.
// Warning: this initialization adds resources (SN numbers 8A-8D) to the player, so it must not be run several times !
// That's why when unsure this returns true (including invalid cases)
bool CustomRORCommand::FindIfGameStartStrategyInitHasBeenDone(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player) { return true; }
	AOE_STRUCTURES::STRUCT_AI *mainAI = player->ptrAIStruct;
	if (!mainAI) { return true; }
	if (STRATEGY::FindElementInStrategy(player, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding, CST_UNITID_HOUSE) < 0) {
		// Our current criterion is "no house = not initialized". Can we do better ?
		return false;
	}
	return true;
}



// Overloads the "tactical AI update" event that occurs regularly for each AI player.
// For the algorithm to work well, requires also "FixUnitIdForInProgressBuilding", "FixResetStratElemForUnitId"
// Returns false is the player is dead/invalid. Returns true in most cases.
bool CustomRORCommand::ManageTacAIUpdate(AOE_STRUCTURES::STRUCT_AI *ai) {
	assert(ai && ai->IsCheckSumValid());
	if (!ai || !ai->IsCheckSumValid()) { return false; }
	assert(ai->structTacAI.IsCheckSumValid());
	// Get some important / useful objects
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *gameSettings = GetGameSettingsPtr();
	AOE_STRUCTURES::STRUCT_PLAYER *player = ai->ptrStructPlayer;
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = &ai->structTacAI;
	assert(globalStruct && globalStruct->IsCheckSumValid());
	assert(gameSettings && gameSettings->IsCheckSumValid());
	assert(player && player->IsCheckSumValid());
	if (!globalStruct || !globalStruct->IsCheckSumValid() || !gameSettings || !gameSettings->IsCheckSumValid() ||
		!player || !player->IsCheckSumValid()) {
		return false;
	}
	if (player->aliveStatus != 0) {
		return false; // Do not update players that are not playing anymore.
	}
	if (gameSettings->rgeGameOptions.isMultiPlayer || (!IsImproveAIEnabled(player->playerId))) { return true; }
	if (CUSTOMROR::crInfo.configInfo.doNotApplyFixes) { return true; }

	// Only for the FIRST tactical update (last one's time is 0): one-shot initializations
	if (tacAI->lastScalingUpdate <= 0) {
		tacAI->lastScalingUpdate = 1; // setting it to 1 ms won't impact the game and allows us to know this init has been run
		if (CUSTOM_AI::customAIHandler.IsAliveAI(player->playerId)) {
			CUSTOM_AI::customAIHandler.GetCustomPlayerAI(player->playerId)->RunInitialStrategyAnalysis();
		}
	}

	if (CUSTOM_AI::customAIHandler.IsAliveAI(player->playerId)) {
		CUSTOM_AI::customAIHandler.GetCustomPlayerAI(player->playerId)->RunStrategyUpdate(globalStruct->currentGameTime);
		CUSTOM_AI::customAIHandler.GetCustomPlayerAI(player->playerId)->economyAI.RunFixesOnVillagerActions(player, globalStruct->currentGameTime);
	}
	return true;
}


// Entry point aftre a strategy element has been added in buildAI
void CustomRORCommand::AfterAddElementInStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, 
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *posToAdd, int countAdded) {
	assert(posToAdd != NULL);
	assert(posToAdd->IsCheckSumValid());
	if (!posToAdd || !posToAdd->IsCheckSumValid()) { return; }

	int insertionPos = 0;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = buildAI->fakeFirstStrategyElement.next;
	while (curElem && (curElem != &buildAI->fakeFirstStrategyElement) && (curElem != posToAdd) ) {
		insertionPos++;
		curElem = curElem->next;
	}
	// Here curElem is == posToAdd (if found)

	for (int i = 0; i < countAdded; i++) {
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *newElem = curElem->next;
		// Try to update strategy if there is an "unreferenced" existing unit that match newly-added element
		STRATEGY::UpdateStrategyWithUnreferencedExistingUnits(buildAI, newElem->unitDAT_ID, newElem->elementType);

		// Granary / SP that are inserted at very beginning of strategy: move it after first house.
		if (((newElem->unitDAT_ID == CST_UNITID_GRANARY) || (newElem->unitDAT_ID == CST_UNITID_STORAGE_PIT)) && (insertionPos < 2)) {
			// Warning: this may move strategy element !
			STRATEGY::MoveStrategyElement_after_ifWrongOrder(buildAI, CST_UNITID_HOUSE, TAIUnitClass::AIUCBuilding, newElem->unitDAT_ID, TAIUnitClass::AIUCNone);
		}
		curElem = curElem->next; // We are sure curElem did not move, use this (newElem may have been moved forward in strategy)
	}
}


void CustomRORCommand::PrintDateTime() {
	time_t rawtime;
	char timebuf[50];
	char timebuf2[50];
	time(&rawtime);
	ctime_s(timebuf2, 30, &rawtime);
	sprintf_s(timebuf, "Time=%s", timebuf2, 40);
	AOE_METHODS::CallWriteText(timebuf);
}


void CustomRORCommand::PrintMapSeed() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	char text[50];
	sprintf_s(text, 50, "Map seed=%ld.", settings->actualMapSeed);
	AOE_METHODS::CallWriteText(text);
}


// This is called very frequently when game is running. Not called if game is paused or stuck.
void CustomRORCommand::OnGameTimer() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global != NULL);
	if (!global) { return; }
	long int currentGameTime = global->currentGameTime / 1000;
	if ((CUSTOMROR::crInfo.configInfo.dislikeComputeInterval > 0) &&
		(currentGameTime >= (CUSTOMROR::crInfo.LastDislikeValuesComputationTime_second + CUSTOMROR::crInfo.configInfo.dislikeComputeInterval))) {
		CUSTOMROR::crInfo.LastDislikeValuesComputationTime_second = currentGameTime;
		CUSTOM_AI::playerTargetingHandler.ComputeDislikeValues();
	}

	// Manage triggers
	if (CUSTOMROR::crInfo.triggersLastCheckTime_s + 1 <= currentGameTime) {
		CUSTOMROR::crInfo.triggersLastCheckTime_s = currentGameTime;

		// Timer trigger
		CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
		memset(&evtInfo, -1, sizeof(evtInfo));
		evtInfo.currentGameTime_s = currentGameTime;
		CUSTOMROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_TIMER, evtInfo);

		// "Passive" triggers (we need to test their criteria regulary)

		// Trigger type: Reached some resource value...?
		memset(&evtInfo, -1, sizeof(evtInfo)); // Reset event info
		for (int iPlayerId = 1; iPlayerId < global->playerTotalCount; iPlayerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(iPlayerId);
			if (player && player->IsCheckSumValid()) {
				// We set a limitation for performance: only the 4 main resources are supported. We could just remove the limitation in this "for" loop.
				for (int currentResourceId = AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_FOOD;
					currentResourceId < AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT; currentResourceId++) {
					float value = player->GetResourceValue((AOE_CONST_FUNC::RESOURCE_TYPES)currentResourceId);
					evtInfo.currentGameTime_s = currentGameTime;
					evtInfo.playerId = iPlayerId;
					evtInfo.resourceId = currentResourceId;
					evtInfo.resourceValue = value;
					CUSTOMROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_RESOURCE_VALUE_MORE_THAN, evtInfo);
					CUSTOMROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_RESOURCE_VALUE_LESS_THAN, evtInfo);
				}
			}
		}
	}

	// Other customROR "timer" treatments: do them only once every second maximum (for performance)
	if (CUSTOMROR::crInfo.lastCustomRORTimeExecution_gameTime_s + 1 <= currentGameTime) {
		CUSTOMROR::crInfo.lastCustomRORTimeExecution_gameTime_s = currentGameTime;
	} else {
		// No enough time has run since last execution.
		return;
	}
}


// Main method to manage "OnCreate" event for living units.
// This event occurs after the constructor has finished its execution.
// This event is triggered for all living units (type=70), not others.
// This event is triggered during game but as well in scenario editor or during game creation.
// actionStruct parameter can be NULL if it could not be determined
// Note: we choose NOT to apply custom treatments when controlled player has an active playing AI (both controlled by human and AI)
void CustomRORCommand::OnLivingUnitCreation(AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS UIStatus, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit,
	AOE_STRUCTURES::STRUCT_ACTION_MAKE_OBJECT *actionStruct) {
	assert(unit && unit->IsCheckSumValidForAUnitClass() && unit->IsTypeValid());
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->IsTypeValid()) { return; }

	// Collect some info
	bool isInGameSpawnUnit = (actionStruct && actionStruct->IsCheckSumValid() && (UIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING));
	bool isInGame = (UIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING);
	bool isScenarioEditor = (UIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR);
	bool isGameInit = (!isInGame && !isScenarioEditor);
	AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unit->unitDefinition;
	assert(unitDef && unitDef->IsTypeValid());
	if (!unitDef || !unitDef->IsTypeValid()) { return; }

	if (isScenarioEditor || isGameInit) {
		return;
	}
	AOE_STRUCTURES::STRUCT_PLAYER *player = unit->ptrStructPlayer;
	assert(player != NULL);
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	assert(globalStruct != NULL);
	if (IsMultiplayer()) { return; } // can provoke out of sync errors

	// RPG mode, if enabled
	if (isInGameSpawnUnit && CUSTOMROR::IsRpgModeEnabled()) {
		assert(actionStruct->actor->IsCheckSumValidForAUnitClass());
		if (!((AOE_STRUCTURES::STRUCT_UNIT_BUILDING*)actionStruct->actor)->IsTypeValid()) {
			assert(false && "actor is not a building");
			return;
		}
		if (RPG_MODE::MakeEpicUnitWithRandomCondition((AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE*)unit)) {
			assert(unit->unitDefinition && unit->unitDefinition->IsCheckSumValidForAUnitClass());
			unit->remainingHitPoints = unit->unitDefinition->totalHitPoints; // Make sure new unit starts with 100% HP.
		}
	}

	// Does spawned unit belong to currently human-controlled player ?
	if (player->playerId != globalStruct->humanPlayerId) {
		return; // Next treatments do not apply when AI players have a new unit... (nor gaia)
	}

	// Assign a shortcut to new unit if config says to - and only if AI is not active for this player
	if (!player->IsAIActive(CUSTOMROR::crInfo.hasManageAIFeatureON)) {
		CUSTOMROR::UNIT::AutoAssignShortcutToUnit(unit);
	}

	// Get info on parent unit if possible
	AOE_STRUCTURES::STRUCT_UNIT_BASE *parentUnit = NULL;
	long int parentUnitId = -1;
	if (actionStruct) {
		parentUnit = actionStruct->actor;
		assert(parentUnit->IsCheckSumValidForAUnitClass());
		if (!parentUnit->IsCheckSumValidForAUnitClass()) { parentUnit = NULL; }
		if (parentUnit) {
			parentUnitId = parentUnit->unitInstanceId;
		}
	}

	UnitCustomInfo *parentInfo = NULL;
	if (parentUnitId >= 0) {
		parentInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(parentUnitId); // Result can be NULL (not found)
	}

	bool commandCreated = false; // To make sure we create only 1 command (or 0)

	// Auto target
	if (CUSTOMROR::crInfo.configInfo.enableSpawnUnitsAutoTarget && !player->IsAIActive(CUSTOMROR::crInfo.hasManageAIFeatureON) &&
		parentInfo && (parentInfo->spawnTargetUnitId >= 0)) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *target = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(parentInfo->spawnTargetUnitId);
		// Note: target may not exist (anymore)
		bool canInteractWithTarget = target && target->IsCheckSumValidForAUnitClass();
		if (canInteractWithTarget) {
			AOE_STRUCTURES::STRUCT_UNIT_FLAG *targetUnitFlag = (AOE_STRUCTURES::STRUCT_UNIT_FLAG*)target;
			if (targetUnitFlag->DerivesFromFlag()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_FLAG *targetUnitDefFlag = (AOE_STRUCTURES::STRUCT_UNITDEF_FLAG *)targetUnitFlag->unitDefinition;
				// For units that can move, check fog visibility
				if (targetUnitDefFlag->speed > 0) {
					canInteractWithTarget = canInteractWithTarget && PLAYER::IsFogVisibleForPlayer(player, (long)target->positionX, (long)target->positionY);
				}
			}
		}
		if (canInteractWithTarget) {
			if (unitDef->DerivesFromCommandable() && unitDef->ptrUnitCommandHeader) {
				if (AOE_METHODS::UNIT::GetUnitDefCommandForTarget(unit, target, true) != NULL) {
					commandCreated = TellUnitToInteractWithTarget(unit, target);
				}
			}
		} else {
			if (parentInfo) { // double-check ;)  Here target no longer exists: remove auto-target
				parentInfo->ResetSpawnAutoTargetInfo();
				CUSTOMROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(parentInfo->unitId);
			}
		}
	}

	// Move spawned unit to building's target location if we have/found one. And if option is enabled.
	if (!commandCreated && CUSTOMROR::crInfo.configInfo.enableSpawnUnitsMoveToLocation && !player->IsAIActive(CUSTOMROR::crInfo.hasManageAIFeatureON) &&
		parentInfo && (parentInfo->spawnUnitMoveToPosX >= 0) && (parentInfo->spawnUnitMoveToPosY >= 0)) {
		MoveUnitToTargetOrPosition(unit, NULL, parentInfo->spawnUnitMoveToPosX, parentInfo->spawnUnitMoveToPosY);
		commandCreated = true;
	}

	// Auto-repair TC for villagers
	if (!commandCreated && CUSTOMROR::crInfo.configInfo.enableSpawnUnitAutoRepairTC && IsVillager(unit->unitDefinition->DAT_ID1) &&
		parentUnit && !player->IsAIActive(CUSTOMROR::crInfo.hasManageAIFeatureON)) {
		if (parentUnit->remainingHitPoints < (float)parentUnit->unitDefinition->totalHitPoints) {
			TellUnitToInteractWithTarget(unit, parentUnit);
			commandCreated = true;
		}
	}
}


// Fixes missing treatments when a unit changes owner (eg. unit conversion)
// This is called BEFORE the actual unit owner change process is called. (this is called at the very beginning of unit conversion process)
// targetUnit is the "victim" (unit that changes owner), actorPlayer is the new owner (player)
// Technical note: in ROR, unit.changeOwner(newplayer) is [EDX+0x44] call.
// TODO: this is not called when relics/ruins are taken ?
void CustomRORCommand::OnUnitChangeOwner_fixes(AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer) {
	if (!targetUnit || !actorPlayer) { return; }
	assert(targetUnit->IsCheckSumValidForAUnitClass());
	assert(actorPlayer->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *targetPlayer = targetUnit->ptrStructPlayer; // The victim (if unit has been converted)
	assert(targetPlayer != NULL);
	assert(targetPlayer->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *targetUnitDef = targetUnit->unitDefinition;
	assert(targetUnitDef != NULL);
	assert(targetUnitDef->IsCheckSumValidForAUnitClass());

	// Fix resource bugs
	if (IsVillager_includingShips(targetUnitDef->DAT_ID1)) {
		// When a villager is converted, resources are NOT updated and max villager score is wrong for both players.
		actorPlayer->SetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_CIVILIAN_POPULATION,
			actorPlayer->GetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_CIVILIAN_POPULATION) + 1
			);
		targetPlayer->SetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_CIVILIAN_POPULATION,
			targetPlayer->GetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_CIVILIAN_POPULATION) - 1
			);
	} else {
		// HERE: not villager (and not fishing ship, etc)
		if ((targetUnitDef->DAT_ID1 != CST_UNITID_PRIEST) &&
			(
			IsTower(targetUnitDef) || (targetUnitDef->unitType == GUT_TRAINABLE)
			)) {
			// living units other than {villagers(+fishing+trade), priests} + towers : update military units count (same bug as villager count)
			actorPlayer->SetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_MILITARY_POPULATION,
				actorPlayer->GetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_MILITARY_POPULATION) + 1
				);
			targetPlayer->SetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_MILITARY_POPULATION,
				targetPlayer->GetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_MILITARY_POPULATION) - 1
				);
		}
	}

	// Adapt strategy for "actor" player
	AOE_STRUCTURES::STRUCT_AI *mainAI_actor = actorPlayer->ptrAIStruct;
	if (mainAI_actor == NULL) { return; }
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI_actor = &mainAI_actor->structBuildAI;
	assert(buildAI_actor != NULL);
	if (buildAI_actor == NULL) { return; }
	STRATEGY::UpdateStrategyWithExistingUnit(buildAI_actor, targetUnit);

	// Update all players' infAI.unitElemList because it would contain an erroneous playerId, and this would never be fixed/updated
	// This leads to incorrect behaviours (unit groups stuck because they try to attack their own units...)
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = actorPlayer->ptrGlobalStruct;
	assert(global && global->IsCheckSumValid());
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *loopPlayer = GetPlayerStruct(loopPlayerId);
		if (loopPlayer && loopPlayer->IsCheckSumValid() && loopPlayer->ptrAIStruct && loopPlayer->ptrAIStruct->IsCheckSumValid() &&
			IsImproveAIEnabled(loopPlayerId)) {
			AOE_STRUCTURES::STRUCT_INF_AI *loopInfAI = &loopPlayer->ptrAIStruct->structInfAI;
			assert(loopInfAI->IsCheckSumValid());
			// Fix (or remove) unit from list for each player. We MUST NOT let a bad playerId be stored in unit elem list.
			UpdateOrResetInfAIUnitListElem(loopInfAI, FindInfAIUnitElemInList(loopInfAI, targetUnit->unitInstanceId));
		}
	}

	if (targetPlayer && IsImproveAIEnabled(targetPlayer->playerId) && targetPlayer->ptrAIStruct && targetPlayer->ptrAIStruct->IsCheckSumValid()) {
		// Notify custom AI that a conversion occurred.
		if (CUSTOM_AI::customAIHandler.IsAliveAI(actorPlayer->playerId) && IsImproveAIEnabled(actorPlayer->playerId)) {
			if (IsArtefactOrFlag(targetUnitDef->unitAIType)) {
				// TODO: actually this case never occurs yet, this method is not called in such event.
				CUSTOM_AI::customAIHandler.GetCustomPlayerAI(actorPlayer->playerId)->OnArtefactControlLoss(targetUnit, actorPlayer);
			} else {
				CUSTOM_AI::customAIHandler.GetCustomPlayerAI(actorPlayer->playerId)->OnUnitConverted(targetUnit, actorPlayer);
			}
		}
	}
}


// Change a unit's owner, for example like a conversion.
// Capturing an artefact does NOT call this.
// I don't see any other possible event than CST_ATI_CONVERT. Use CST_GET_INVALID to trigger NO notification.
bool CustomRORCommand::ChangeUnitOwner(AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer,
	AOE_CONST_INTERNAL::GAME_EVENT_TYPES notifyEvent) {
	if (!targetUnit || !actorPlayer || !targetUnit->IsCheckSumValidForAUnitClass() || !actorPlayer->IsCheckSumValid()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_PLAYER *oldOwner = targetUnit->ptrStructPlayer;
	this->OnUnitChangeOwner_fixes(targetUnit, actorPlayer);
	AOE_METHODS::UNIT::ChangeUnitOwner(targetUnit, actorPlayer);
	if ((notifyEvent == AOE_CONST_INTERNAL::CST_GET_INVALID) || (!oldOwner || !oldOwner->IsCheckSumValid())) {
		return true; // No notification to handle
	}

	// Handle internal structures
	CUSTOMROR::crInfo.myGameObjects.RemoveAllInfoForUnit(targetUnit->unitInstanceId, targetUnit->positionX, targetUnit->positionY);

	// Not handled: other events than conversion (is there any ?)

	// Warning, for a conversion, arg2= actor playerId (priest), arg3=victim playerId
	AOE_callNotifyEvent(notifyEvent, actorPlayer->playerId, oldOwner->playerId, (long int)targetUnit->positionY, (long int)targetUnit->positionX);
	return true;
}


// Custom Fixes/features on player.addUnit calls.
// Note for conversion : unit is currently (temporarily) unavailable in global->GetUnitStruct(unitId) !
void CustomRORCommand::OnPlayerAddUnitCustomTreatments(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, bool isTempUnit, bool isNotCreatable) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return; }
	if (!player || !player->IsCheckSumValid()) { return; }

	if (!isNotCreatable && !isTempUnit) {
		// We have a creatable unit
		if ((unit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) && (unit->unitStatus == 2)) {
			// Add building to player buildings list (bug in original game), this is done in unit constructor but NOT at unit conversion
			AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader = player->ptrBuildingsListHeader;
			assert(buildingsHeader != NULL);
			if (!buildingsHeader) {
				traceMessageHandler.WriteMessage("buildingsHeader is NULL for player #" + std::to_string(player->playerId));
				return;
			}

			long int posX = (long int)unit->positionX;
			long int posY = (long int)unit->positionY;
			unsigned long int **ppMapInfo = (unsigned long int **)AOE_OFFSETS::ADDR_MAP_VISIBILITY_INFO;
			unsigned long int *pMapInfoX = ppMapInfo[posX];
			unsigned long int *pMapInfo = &pMapInfoX[posY];

			// Trick for conversion: addBuildingToArrays does not work when unit->player is not the good owner player
			// So we update it temporarily (ROR code will actually update it later)
			AOE_STRUCTURES::STRUCT_PLAYER *oldUnitPlayer = unit->ptrStructPlayer;
			unit->ptrStructPlayer = player;

			_asm {
				MOV EAX, pMapInfo
				MOV ECX, buildingsHeader
				MOV EDX, unit
				PUSH EAX // arg2 = pMapVisibilityInfo
				PUSH EDX // arg1 = unit
				MOV EAX, 0x004368F0 // playerUnitsHeader.addBuildingToArrays(ptrUnit, arg2_mapinfo)
				CALL EAX
			}

			unit->ptrStructPlayer = oldUnitPlayer; // Restore player (for conversion, it will be updated a bit later)
		}
	}

	// Update unit owner in players' unit list if it is visible to them (useful for conversion / unit capture)
	// This is very important for artefacts. Probably useful for other units (avoid having wrong playerId in list), but impacts are to be analyzed.
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return; }
	long int playerTotalCount = global->playerTotalCount;
	if ((global->gameRunStatus == 0) && (global->currentGameTime > 0) && (!isTempUnit && !isNotCreatable) &&
		(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING)) {
		for (int curPlayerId = 1; curPlayerId < playerTotalCount; curPlayerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *curPlayer = GetPlayerStruct(curPlayerId);
			AOE_STRUCTURES::STRUCT_AI *curAI = curPlayer->ptrAIStruct;
			if (curAI && curAI->IsCheckSumValid() && curAI->structInfAI.IsCheckSumValid()) {
				UpdateUnitOwnerInfAIUnitListElem(&curAI->structInfAI, (AOE_STRUCTURES::STRUCT_UNIT_BASE*)unit, player->playerId);
			}
		}
	}
}


// Custom Fixes/features on player.removeUnit calls.
// Note: player.RemoveUnit is called BY unit.destructor.
// Here unit status can be 2 (if conversion), 7 (quitting editor?), 8 (in-game dying unit) ? +maybe other values
// Warning: this method is called in many situations, game might NOT be running.
void CustomRORCommand::OnPlayerRemoveUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, bool isTempUnit, bool isNotCreatable) {
	if (!player || !unit) { return; }
	assert(player->IsCheckSumValid() && unit->IsCheckSumValidForAUnitClass());
	if (!player->IsCheckSumValid() || !unit->IsCheckSumValidForAUnitClass()) { return; }

	// Gather information
	assert(unit->ptrStructPlayer != NULL);
	if (!unit->ptrStructPlayer) { return; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unit->unitDefinition;
	assert(unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass());
	if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) { return; }
	bool isBuilding = (unitDefBase->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING); // Warning: using unit->unitType is risky (not always correct?)
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	bool isInGame = (settings->currentUIStatus == AOE_CONST_INTERNAL::GSUS_PLAYING) || (settings->currentUIStatus == AOE_CONST_INTERNAL::GSUS_GAME_OVER_BUT_STILL_IN_GAME);

	// Update AI struct unit lists that are never updated by ROR (we choose to do this only if AI improvement is enabled).
	if (isInGame && player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid() && (unit->unitInstanceId >= 0) && IsImproveAIEnabled(player->playerId)) {
		AOE_STRUCTURES::STRUCT_INF_AI *infAI = &player->ptrAIStruct->structInfAI;
		assert(infAI->IsCheckSumValid());
		// Update infAI lists (those that are not updated by ROR)
		long int size = infAI->unitElemListSize;
		for (long int i = 0; i < size; i++) {
			AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *curElem = &infAI->unitElemList[i];
			if (curElem->unitId == unit->unitInstanceId) {
				UpdateOrResetInfAIUnitListElem(infAI, curElem);
			}
		}
	}

	if (!isNotCreatable && !isTempUnit) {
		// We have a creatable unit
		if (isBuilding && (unit->unitStatus == AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY)) { // status=2 means it is a conversion (or gaia unit captured)
			// Remove building from player buildings list (bug in original game), this is done in unit destructor but NOT at unit conversion
			// Note that because of this call, the remove operation is done twice (will be done again in destructor). But the 2nd time, it will just do nothing.
			AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader = player->ptrBuildingsListHeader;
			AOE_playerBldHeader_RemoveBldFromArrays(buildingsHeader, (STRUCT_UNIT_BASE*)unit);
		}

		// Fix constructions history array
		if (isInGame && isBuilding && player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid() &&
			// Not fixed in easiest... But as it is a technical fix, we apply it even if "improve AI" is not enabled.
			// Note that it has a strong (positive) impact on AI farming + the fact that granary/SP construction is no longer blocked for no valid reason
			settings->rgeGameOptions.difficultyLevel < AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASIEST) {
			AOE_STRUCTURES::STRUCT_INF_AI *infAI = &player->ptrAIStruct->structInfAI;
			assert(infAI->IsCheckSumValid());
			if ((infAI->buildHistoryArray != NULL) && (infAI->buildHistoryArraySize > 0)) {
				// Disable or reset element for current building
				long int posX = (long int)unit->positionX;
				long int posY = (long int)unit->positionY;
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
				if (unitDef && unitDef->IsCheckSumValidForAUnitClass()) {
					// Setting status to "reset" (3 / =removed) will unblock further construction of same kind of buildings, especially granary/SP.
					// This could also fix the bad farms placement after some game time (because "existing" farms apply a negative likee value on nearby tiles) ? To verify
					// Moreover, it allows the array data to be valid !
					AOE_InfAIBuildHistory_setStatus(infAI, posX, posY, unitDef->DAT_ID1, AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS::CST_BHS_REMOVED);
				}
			}
		}

		// If unit is dying: clean AI "unitId" lists: it is useless for lists that only store unit IDs to keep non-existing unitIDs.
		// This allows to avoid getting very large UnitID arrays (especially creatableAndGatherableUnits that can contain thousand of elements without this optimization)
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		assert(global && global->IsCheckSumValid());
		if (isInGame && global && global->IsCheckSumValid() && (unit->unitStatus > 2)) {
			// For all valid players, remove unitId from AI lists...
			for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
				AOE_STRUCTURES::STRUCT_PLAYER *loopPlayer = GetPlayerStruct(loopPlayerId);
				if (loopPlayer && loopPlayer->IsCheckSumValid() && loopPlayer->ptrAIStruct && (loopPlayer->aliveStatus == 0) &&
					loopPlayer->ptrAIStruct->IsCheckSumValid() && IsImproveAIEnabled(loopPlayerId)) {
					AOE_STRUCTURES::STRUCT_INF_AI *loopInfAI = &loopPlayer->ptrAIStruct->structInfAI;
					assert(loopInfAI->IsCheckSumValid());
					if (IsClassArtefactOrGatherableOrCreatable(unitDefBase->unitAIType)) {
						loopInfAI->creatableAndGatherableUnits.Remove(unit->unitInstanceId);
					}
					if (IsClassPlayerCreatable(unitDefBase->unitAIType)) {
						loopInfAI->playerCreatableUnits.Remove(unit->unitInstanceId);
					}
					if ((unitDefBase->DAT_ID1 == CST_UNITID_FORUM) || 
						(unitDefBase->DAT_ID1 == CST_UNITID_DOCK) ||
						(unitDefBase->DAT_ID1 == CST_UNITID_RELIC) ||
						(unitDefBase->DAT_ID1 == CST_UNITID_RUIN) ||
						(unitDefBase->DAT_ID1 == CST_UNITID_RUIN2) ||
						(unitDefBase->unitAIType == TribeAIGroupStoneMine) ||
						(unitDefBase->unitAIType == TribeAIGroupGoldMine) ||
						(unitDefBase->unitAIType == TribeAIGroupBerryBush)
					) {
						loopInfAI->elementsToDefend.Remove(unit->unitInstanceId);
					}
				}
			}
		}
	}

	// Auto rebuild farms
	bool enableAutoRebuildFarms = CUSTOMROR::crInfo.configInfo.GetAutoRebuildFarmConfig(settings->rgeGameOptions.isScenario || settings->isCampaign, settings->isDeathMatch)->enableAutoRebuildFarms;
	if (isInGame && unit && unit->IsCheckSumValidForAUnitClass() && isBuilding && enableAutoRebuildFarms) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
		// If this is a farm, and if I have "farm rebuild info" for this position (not in "not rebuild" mode), then trigger a rebuild.
		if (unitDef && unitDef->IsCheckSumValidForAUnitClass() && (unitDef->DAT_ID1 == CST_UNITID_FARM) && player->ptrGlobalStruct) {
			FarmRebuildInfo *fInfo = CUSTOMROR::crInfo.myGameObjects.FindFarmRebuildInfo(unit->positionX, unit->positionY);
			if (fInfo && (fInfo->playerId == player->playerId) && !fInfo->forceNotRebuild &&
				(fInfo->villagerUnitId >= 0) && PLAYER::IsUnitAvailableForPlayer(CST_UNITID_FARM, player)) {
				// As long as we use a game command, it is compatible with multiplayer.
				GAME_COMMANDS::CreateCmd_Build(fInfo->villagerUnitId, CST_UNITID_FARM, fInfo->posX, fInfo->posY);
				fInfo->villagerUnitId = -1;
			}
			if (fInfo && !fInfo->forceNotRebuild && !fInfo->forceRebuild) {
				// Remove farm rebuild info when it was added automatically (none of the force rebuild/not rebuild are set)
				CUSTOMROR::crInfo.myGameObjects.RemoveFarmRebuildInfo(unit->positionX, unit->positionY);
			}
		}
	}

	// Triggers
	if (isInGame && unit && unit->IsCheckSumValidForAUnitClass() && !isTempUnit &&
		!settings->rgeGameOptions.isMultiPlayer && (unit->unitInstanceId >= 0)) {
		CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
		evtInfo.unitId = unit->unitInstanceId;
		CUSTOMROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_UNIT_LOSS, evtInfo);
	}
}


// Returns true if a unit should change target to attack tower, false if it should keep its current activity.
// Default return value is true (for error/invalid cases). Note: it is better to return true in most case to make sure the tower can be destroyed !
// To be used when target unit is a tower in actor's town
// Warning: try to keep this function fast and optimized as much as possible. It may be called quite often.
// The improved algorithm is only used if ImproveAI config is ON.
bool CustomRORCommand::ShouldAttackTower_towerPanic(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actorUnit, AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyTower) {
	if (CUSTOMROR::crInfo.configInfo.improveAILevel <= 0) {
		return true; // improve AI is disabled. Return default value.
	}
	
	// Run various consistency checks
	assert(COMBAT::COMBAT_CONST::distanceToConsiderVeryClose > 0);
	if (!actorUnit || !enemyTower) { return false; }
	assert(actorUnit->currentActivity != NULL);
	assert(actorUnit->unitDefinition != NULL);
	assert(enemyTower->unitDefinition != NULL);
	if ((!actorUnit->currentActivity) || (!actorUnit->unitDefinition) || (!enemyTower->unitDefinition)) { return false; }
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = actorUnit->currentActivity; // Guaranteed non-NULL
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *actorUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)actorUnit->unitDefinition; // Guaranteed non-NULL
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *enemyTowerDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)enemyTower->unitDefinition; // Guaranteed non-NULL
	assert(actorUnitDef->DerivesFromAttackable());
	assert(enemyTowerDef->DerivesFromAttackable());
	if (!actorUnitDef->DerivesFromAttackable() || !enemyTowerDef->DerivesFromAttackable()) { return false; }

	if (activity->currentActionId == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_CONVERT) {
		return false; // Converting priest: let him proceed. TO DO: improve !
	}

	// Check if tower is mine or allied (it may have been converted)
	if (actorUnit->ptrStructPlayer->ptrDiplomacyStances[enemyTower->ptrStructPlayer->playerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY) {
		if ((activity->targetUnitId == enemyTower->unitInstanceId) &&
			((activity->currentActionId == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_CONVERT) ||
			(activity->currentActionId == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_ATTACK)
			)) {
			// Stop attacking/converting allied !
			AOE_STRUCTURES::STRUCT_UNIT_ACTION_INFO * actionInfo = actorUnit->ptrActionInformation;
			if (actionInfo) {
				_asm {
					MOV ECX, actionInfo;
					MOV EAX, 0x403E50;
					CALL EAX; // removeAction
				}
			}
			
		}
		return false; // Do not attack a friend (unit would get stuck)
	}

	// Actor = Priest
	if (actorUnitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
		if ((actorUnit->resourceTypeId == CST_RES_ORDER_FAITH) && (actorUnit->resourceValue < 100)) {
			return false; // Priest that does not have 100% faith: let him do something else.
		}
		bool canConvertBuildings = (actorUnit->ptrStructPlayer->GetResourceValue(CST_RES_ORDER_CAN_CONVERT_BUILDING) > 0);
		if (!canConvertBuildings) { return false; } // I CAN'T convert the tower ! Anything else is better.
	}

	long int currentTargetId = actorUnit->currentActivity->targetUnitId;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *currentTarget = GetUnitStruct(currentTargetId);
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *currentTargetDef = NULL;
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *currentTargetDefAttackable = NULL;
	if (currentTarget) {
		currentTargetDef = currentTarget->unitDefinition;
		if (currentTargetDef->DerivesFromAttackable()) {
			currentTargetDefAttackable = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)currentTargetDef;
		}
	} else {
		return true;
	}
	assert(currentTargetDef && currentTargetDef->IsCheckSumValidForAUnitClass());

	float distanceToTower = GetDistance(actorUnit->positionX, actorUnit->positionY, enemyTower->positionX, enemyTower->positionY);
	float distanceToCurrentTarget = GetDistance(actorUnit->positionX, actorUnit->positionY, currentTarget->positionX, currentTarget->positionY);
	float actorRange = actorUnitDef->maxRange + COMBAT::COMBAT_CONST::distanceToConsiderVeryClose;
	assert(actorRange > 0);

	// All unit types: if tower is at reach AND is has 1 HP, quickly destroy it at any cost ! Even if a priest is converting me
	if ((enemyTower->remainingHitPoints < 2) && (distanceToTower < actorRange)) {
		return true; // Just destroy this tower while it's weak !
	}

	// Actor = catapult: always attack the tower... unless priest is very close AND being converting (not moving - can not escape my shot)
	if (actorUnitDef->DAT_ID1 == CST_UNITID_STONE_THROWER) { // DAT_ID1 : also matches for catapult upgrades
		if ((distanceToCurrentTarget < actorRange) && (currentTargetDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) &&
			(currentTarget->currentActivity) && (currentTarget->currentActivity->currentActionId == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_CONVERT)) {
			return false; // In that specific case, attack current target (priest)
		} else {
			return true; // Otherwise, catapults just have to attack the tower
		}
	}

	// Some Villager priority rules
	if (actorUnitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian) {
		// If I have siege weapons tech (villager have a very strong bonus agains towers), attack the tower without hesitation. Even if there is a priest.
		bool villagerCanKillTowers = (GetResearchStatus(actorUnit->ptrStructPlayer, CST_RSID_SIEGE_CRAFT) == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID);
		if (villagerCanKillTowers) {
			return true;
		}
		// Builder: keep for some critical constructions
		STRUCT_UNIT_BUILDING *currentTargetAsBld = (STRUCT_UNIT_BUILDING *)currentTarget;
		if (currentTargetAsBld->IsCheckSumValid() && (activity->currentActionId == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_BUILD) &&
			(currentTarget->ptrStructPlayer == actorUnit->ptrStructPlayer)// Just a check
			) {
			if (currentTargetDef->DAT_ID1 == CST_UNITID_FORUM) {
				return false; // Keep building TC
			}
			if (currentTargetAsBld->constructionProgress > 85) {
				return false; // Almost finished: continue !
			}
			if ((currentTargetAsBld->constructionProgress > 75) && (IsTower(currentTargetDef))) {
				return false; // Almost finished tower: continue !
			}
		}
		// Repairman: keep for some critical constructions
		if ((activity->currentActionId == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_BUILD) &&
			(currentTarget->ptrStructPlayer == actorUnit->ptrStructPlayer)// Just a check
			) {
			if ((currentTargetDef->DAT_ID1 == CST_UNITID_FORUM) && (currentTarget->remainingHitPoints < 400)) {
				return false; // Keep building TC
			}
		}
	}

	// Current target is a priest
	if (currentTarget && currentTargetDef && (currentTargetDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest)) {
		if ((actorUnitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher) ||
			(actorUnitDef->DAT_ID2 == CST_UNITID_CHARIOT) || // Using DATID2 (with upgrades) to exclude scythe chariot => will use more complex rules.
			(actorUnitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher) ||
			(actorUnitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher) ||
			(actorUnitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher) ||
			(actorUnitDef->DAT_ID1 == CST_UNITID_SCOUT_SHIP) ||
			(actorUnitDef->DAT_ID1 == CST_UNITID_BALLISTA)
			// Not catapult, they might miss their shot, whereas they won't miss the tower (and do a lot of damage to them)
			) {
			// Units that are clearly better against priests: keep aiming at priests, they can be extremely destructive in such situations
			// Especially for archers, because they are weak against towers (and can easily attack priests thanks to range attack).
			return false;
		}

		// Ambiguous cases (tower / priest): decide according to distance
		if (distanceToCurrentTarget < actorRange) {
			return false; // Keep attacking current (priest) target if it's in range.
		}
		if ((distanceToTower < actorRange) && (distanceToCurrentTarget > actorRange + 3)) {
			return true; // Attack tower if it's close and priest if not very close
		}

		if (actorUnitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot) {
			if (distanceToCurrentTarget < distanceToTower + 5) {
				return false; // Attack the priest even if it's a bit further.
			} else {
				// Attack the tower if priest is too far. I could be dead before I go there to attack him.
				// My trample damage may prove useful against units that defend/repair the tower.
				return true;
			}
		}

		// HERE: none of the target is in range.
		// Slow / medium speed units : don't move too far
		// Note: remaining "slow" units are all good against towers (war elephant, catapults, infantry...) because archers (and priest) have already been managed.
		if (actorUnitDef->speed <= 1.2) {
			if (distanceToCurrentTarget + 3 < distanceToTower) {
				// Current target is closer (and difference between the distances is large enough)
				return false; // Attack priest as it's really closer
			} else {
				return true; // Attack the tower (I am slow and I am qui good against towers and I am closer to it).
			}
		}
		// Remaining cases: fast units (important: excluding siege weapons because they are slow)
		if (distanceToCurrentTarget < distanceToTower) {
			return false; // Attack the priest if it's closer
		}

		return true; // Could not take a decision: attack the tower.
	}


	// Current target is also a tower in my town. To simplify, we consider a tower is a builing with a (displayed) attack
	if ((currentTargetDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) && currentTargetDefAttackable && (currentTargetDefAttackable->displayedAttack > 0)
		&& (distanceToCurrentTarget < 18)) { // distanceToCurrentTarget < 18: approximative, but we don't want to search for TC + calculate distance, it's unnecessary processing.
		float HPDifference = currentTarget->remainingHitPoints - enemyTower->remainingHitPoints;
		float attackDifference = (float)(currentTargetDefAttackable->displayedAttack - enemyTowerDef->displayedAttack);
		
		// Keep attacking the target that already is in range, if possible
		if (distanceToTower < actorRange) {
			return true; // attack tower ("triggered" one)
		}
		if (distanceToCurrentTarget < actorRange) {
			return false; // attack current target (tower too)
		}
		// Same strength (threshold=1), almost same HP: choose closer one
		if ((abs(HPDifference) < 30) && (abs(attackDifference) <= 1)) {
			if (distanceToCurrentTarget < distanceToTower) {
				return false; // Keep current
			} else {
				return true; // change to "triggered" tower
			}
		}
		// Attack strongest if possible (not too far). Note: base tower attacks are 3, 4, 6, 20
		if (attackDifference >= 1) { // currentTarget attack is at least 1 point more than "triggered" tower
			if (distanceToCurrentTarget < distanceToTower + 8) { // current target is maybe a bit further than the other, but not too much
				return false; // Keep current
			} else {
				return true; // change to "triggered" tower (really too far)
			}
		}
		if (attackDifference <= -1) { // "triggered" tower attack is at least 1 point more than currentTarget
			if (distanceToTower < distanceToCurrentTarget + 8) { // "triggered" tower is maybe a bit further than the other, but not too much
				return true; // change to "triggered" tower
			} else {
				return false; // Keep current (really too far)
			}
		}
		// substantial HP difference: attack weaker
		if (HPDifference > 40) { // current target has MORE hit points: avoid attacking it
			return true; // change to "triggered" tower
		}
		if (HPDifference < -40) { // current target has LESS hit points: attack it
			return false; // keep current target
		}
		return true; // No big difference: just attack "triggered" tower (like most units): more efficient, moreover it's closer to my TC
	}

	// TO DO: check if current target can be killed quickly

	// TO DO : check if target is mine or allied ! (it may have been converted !)
	return true;
}


// A limitation to number of units that will be analyzed. Wow, 200 villagers attacking the same tower ? We're secure, we've got extra room :)
#define CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT 256
// Replaces ROR's loop on villagers to pick some and task them to attack enemy tower.
// assignedUnitsCounter is IN OUT: input value can be >0 (number of military units that have already been tasked). We should stop when assignedUnitsCounter reaches 6 (original code's behaviour)
// In this method, we try to do "standard" treatments, and apply custom improvements only if improveAILevel is enabled in config.
// The improved algorithm is only used if ImproveAI config is ON.
void CustomRORCommand::towerPanic_LoopOnVillagers(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyTower,
	long int *pAssignedUnitsCount, AOE_STRUCTURES::STRUCT_POSITION_INFO *pTCPositionInfo) {
	// Invalid/NULL objects ?
	if (!tacAI || !enemyTower || !pTCPositionInfo || !tacAI->IsCheckSumValid() || !enemyTower->IsCheckSumValidForAUnitClass() || !tacAI->ptrMainAI) { return; }
	if (*pAssignedUnitsCount < 0) { return; } // Bad input value
	if (*pAssignedUnitsCount >= 6) { return; } // Already reached maximum assigned tasks
	AOE_STRUCTURES::STRUCT_PLAYER *player = tacAI->ptrMainAI->ptrStructPlayer;
	assert(player->IsCheckSumValid());
	if (!player->IsCheckSumValid()) { return; }
	if ((tacAI->allVillagers.arraySize <= 0) || (tacAI->allVillagers.usedElements <= 0)) { return; } // No villager to work on

	// Check enemyTower IS actually enemy
	if (!enemyTower->ptrStructPlayer || (player->ptrDiplomacyStances[enemyTower->ptrStructPlayer->playerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY)) {
		traceMessageHandler.WriteMessage("towerPanic_LoopOnVillagers: tower does not belong to an enemy/neutral player");
		return;
	}

	if (IsImproveAIEnabled(tacAI->commonAIObject.playerId)) {
		// A special case: poor little villager is alone and big nasty tower is strong. Don't commit suicide !
		if ((tacAI->allVillagers.arraySize <= 1) && (enemyTower->remainingHitPoints > 50) && (*pAssignedUnitsCount <= 0)) {
			return; // Lonely villager against a "healthy" tower: don't even try, it's useless. Better do anything else and wait for support !
		}
	}

	long int squareDistances[CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT];
	long int orderedUnitIDs[CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT];
	memset(squareDistances, -1, CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT * sizeof(long int));
	memset(orderedUnitIDs, -1, CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT * sizeof(long int));
	int arrayElemCount = 0;
	assert(tacAI->allVillagers.usedElements <= CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT);
	
	// Sort unit IDs by distance from provided position. complexity is n*sqrt(n), but then we will require no more treatments
	// (original code does 1 "best unit" search in each loop).
	// This loop only collects information (no treatments here)
	for (int tacAIArrayIndex = 0; tacAIArrayIndex < tacAI->allVillagers.arraySize; tacAIArrayIndex++) { // Loop on "all villagers"
		if (tacAI->allVillagers.unitIdArray[tacAIArrayIndex] >= 0) { // if current unitId is valid (!= -1)
			AOE_STRUCTURES::STRUCT_UNIT_BASE *currentVillager = GetUnitStruct(tacAI->allVillagers.unitIdArray[tacAIArrayIndex]);
			if (currentVillager) {
				int iArray = 0;
				bool found = false;
				while ((iArray < 200) && (!found)) { // Search correct position to insert in our array
					long int diffX = ((long int)currentVillager->positionX) - pTCPositionInfo->posX;
					long int diffY = ((long int)currentVillager->positionY) - pTCPositionInfo->posY;
					long int squareDist = (diffX * diffX) + (diffY * diffY);
					found = ((squareDist <= squareDistances[iArray]) || (squareDistances[iArray] < 0));
					if (found) {
						// Move (shift) all array elements that come after ("insert" operation)
						for (int i = arrayElemCount; i > iArray; i--) {
							assert(i > 0);
							if (i < CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT) {
								squareDistances[i] = squareDistances[i - 1];
								orderedUnitIDs[i] = orderedUnitIDs[i - 1];
							}
						}
						squareDistances[iArray] = squareDist;
						orderedUnitIDs[iArray] = currentVillager->unitInstanceId;
						arrayElemCount++;
					}
					iArray++;
				}
			}
		}
	}

	// Now we can loop on units with increasing distance. No need to make a search each time (unlike ROR original code)
	long int currentIndex = 0;
	while ((*pAssignedUnitsCount < 6) && (currentIndex < arrayElemCount)) {
		AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *unit = (AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *)GetUnitStruct(orderedUnitIDs[currentIndex]);
		AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = NULL;
		bool valid = unit && unit->IsCheckSumValidForAUnitClass() && unit->DerivesFromCommandable();
		if (valid) { activity = unit->currentActivity; }
		bool attackTower = true; // Default: attack = most cases (in original code, villager almost always attack the tower)
		if (valid && activity) {
			// This IF corresponds to original code: do NOT re-task villager that are currently attacking something ("whenattacked=2BC" or that...?(2C9))
			// Corresponds to filter (exclude 2C9 and 2BC) in original game code
			// Don't know why first loop does NOT exclude 2C9 (only excludes 2BC)
			if ((activity->orderTaskId == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_UNKNOWN_2BC_ATTACKING) ||
				(activity->orderTaskId == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_GATHERER_ATT_REACTION_WHEN_ATTACKED)) {
				attackTower = false;
			}
		}

		// This custom part (AI improvement) depends on improveAILevel parameter (can be soft-disabled)
		if (IsImproveAIEnabled(tacAI->commonAIObject.playerId)) {
			// Additional "custom" treatments. Try to avoid complex treatments and try to optimize as this code is called very frequently.
			// Let's use our common function to handle decisions for units against tower in "my" town.
			if (!this->ShouldAttackTower_towerPanic(unit, enemyTower)) {
				attackTower = false;
			}
		}

		if (valid && attackTower) {
			// Tell villager to go and attack the tower
			if (GAME_COMMANDS::CreateCmd_RightClick(unit->unitInstanceId, enemyTower->unitInstanceId, enemyTower->positionX, enemyTower->positionY)) {
				(*pAssignedUnitsCount)++;
			}
		}

		currentIndex++;
	}
}
#undef CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT


// If fire galley iconId conflicts with catapult trireme, this will move it to a free location (trireme tech because now it's free)
void CustomRORCommand::MoveFireGalleyIconIfNeeded(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player) { return; }
	// Check fire galley unit exists for this player (it is a valid DAT_ID)
	if (CST_UNITID_FIRE_GALLEY >= player->structDefUnitArraySize) { return; }
	// Check the researches we use exist for this player
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *researchInfo = player->ptrResearchesStruct;
	assert(researchInfo != NULL);
	if ((CST_RSID_TRIREME >= researchInfo->researchCount) || (CST_RSID_FIRE_GALLEY >= researchInfo->researchCount) ) { return; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE **arrUnitDef = player->ptrStructDefUnitTable;
	if (arrUnitDef) {
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)arrUnitDef[CST_UNITID_FIRE_GALLEY];
		if (!unitDef || !unitDef->DerivesFromTrainable() || (unitDef->trainButton == 9)) { return; } // Already at the custom location (also return if invalid unit def)
		AOE_CONST_FUNC::RESEARCH_STATUSES triremeStatus = GetResearchStatus(player, CST_RSID_TRIREME);
		AOE_CONST_FUNC::RESEARCH_STATUSES fireGalleyStatus = GetResearchStatus(player, CST_RSID_FIRE_GALLEY);
		// If player has fire galley + trireme in his tech tree, make sure both buttons can be visible at any time (if available)
		if ((fireGalleyStatus != AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) &&
			(triremeStatus != AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED)) {
			// Trireme is researched: research button location is now free to use
			if (triremeStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID) {
				unitDef->trainButton = CST_FIRE_GALLEY_CUSTOM_BUTTONDATID; // Fire galley uses trireme's tech button now it won't appear again.
			} else {
				unitDef->trainButton = CST_FIRE_GALLEY_ORIGINAL_BUTTONDATID; // Use standard location (same as trireme) until trireme is available too.
			}
		}
	}
}
// If fire galley iconId conflicts with catapult trireme, this will move it to a free location (trireme tech because now it's free)
// If playerId is invalid, does nothing (no error).
void CustomRORCommand::MoveFireGalleyIconIfNeeded(short int playerId) {
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
	if (player != NULL) {
		this->MoveFireGalleyIconIfNeeded(player);
	}
}



// Technical fix for a method about elevation application when generating map. Original method contains many bugs.
void CustomRORCommand::Fixed_MapGen_applyElevation(long int posX, long int posY, long int distance, AOE_STRUCTURES::STRUCT_MAPGEN_ELEVATION_INFO *elevInfo) {
	if ((distance <= 0) || (posX < 0) || (posY < 0)) { return; }
	assert(elevInfo != NULL);
	assert(elevInfo->IsCheckSumValid());
	if (!elevInfo || !elevInfo->IsCheckSumValid()) { return; }

	// Check positions
	long int minPosY = posY - distance;
	long int minPosX = posX - distance;
	long int maxPosY = posY + distance;
	long int maxPosX = posX + distance;
	if (minPosY < 0) { minPosY = 0; }
	if (minPosX < 0) { minPosX = 0; }
	if (maxPosY >= elevInfo->mapSizeY) { maxPosY = elevInfo->mapSizeY - 1; }
	if (maxPosX >= elevInfo->mapSizeX) { maxPosX = elevInfo->mapSizeX - 1; }

	AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO **pTileInfoRows = elevInfo->unknown_02C;
	// Replace loops in 46F5E1. Was this code written by the internship guy ? It's the only method I saw that is so buggy and so badly written
	for (long int x = minPosX; x <= maxPosX; x++) {
		for (long int y = maxPosY; y >= minPosY; y--) {
			AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO *tile = &pTileInfoRows[x][y];
			_asm {
				PUSH tile;
				MOV ECX, elevInfo;
				MOV EAX, 0x46C310;
				CALL EAX;
			}
		}
	}
}


// Displays timer stats as a game chat.
void CustomRORCommand::DisplayTimerStats() {
	long int result = 0;
	for (int i = 0; i < CST_TIMER_STATS_ARRAY_SIZE; i++) {
		result += CUSTOMROR::crInfo.CollectedTimerIntervals_ms[i];
	}
	result = result / CST_TIMER_STATS_ARRAY_SIZE;
	char buf[70];
	sprintf_s(buf, "Avg timer interval is %ld ms - slow down factor is %ld", result, CUSTOMROR::crInfo.configInfo.gameTimerSlowDownFactor);
	AOE_METHODS::CallWriteText(buf);
}


// This method automatically recalculates gameTimerSlowDownFactor
void CustomRORCommand::AutoFixGameTimer() {
	bool reset = false;
	time_t t = time(0); // get current time
	struct tm now;
	localtime_s(&now, &t);
	if (CUSTOMROR::crInfo.LastGameTimerAutoFix_second == now.tm_sec) { return; } // Prevents from doing this more than once per second
	long int result = 0;
	for (int i = 0; i < CST_TIMER_STATS_ARRAY_SIZE; i++) {
		if (CUSTOMROR::crInfo.CollectedTimerIntervals_ms[i] < 0) { return; } // does not update if we did not collect enough stats (invalid values remain)
		result += CUSTOMROR::crInfo.CollectedTimerIntervals_ms[i];
	}
	result = result / CST_TIMER_STATS_ARRAY_SIZE; // Get average
	// Hardcoded limit. Choose a better value. A variable ?
	if ((result < CST_TIMER_INTERVAL_TOO_FAST) && (CUSTOMROR::crInfo.configInfo.gameTimerSlowDownFactor < CST_MAXIMUM_SLOW_DOWN_FACTOR)) {
		CUSTOMROR::crInfo.configInfo.gameTimerSlowDownFactor++;
		reset = true;
	}
	if ((result > CST_TIMER_INTERVAL_TOO_SLOW) && (CUSTOMROR::crInfo.configInfo.gameTimerSlowDownFactor > 1)) {
		CUSTOMROR::crInfo.configInfo.gameTimerSlowDownFactor--;
		reset = true;
	}
	CUSTOMROR::crInfo.LastGameTimerAutoFix_second = now.tm_sec;

	if (reset) {
		for (int i = 0; i < CST_TIMER_STATS_ARRAY_SIZE; i++) {
			CUSTOMROR::crInfo.CollectedTimerIntervals_ms[i] = -1; // Reset statistics
		}
	}
}


// Generate map (overload) for scenario editor
bool CustomRORCommand::ScenarioEditor_customGenerateMap(long int sizeX, long int sizeY) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) { return false; }
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_METHODS::GetScreenFromName(scenarioEditorScreenName);
	assert(scEditor && scEditor->IsCheckSumValid());
	if (!scEditor || !scEditor->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_PLAYER *gaia = GetPlayerStruct(0);
	assert(gaia && gaia->IsCheckSumValid());
	if (!gaia || !gaia->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = scEditor->global;
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	if (!mapInfo || !mapInfo->IsCheckSumValid()) { return false; }

	assert(sizeX > 0); assert(sizeX <= 0xFF);
	assert(sizeY > 0); assert(sizeY <= 0xFF);
	if ((sizeX <= 0) || (sizeY <= 0) || (sizeX > 0xFF) || (sizeY > 0xFF)) { return false; }

	long int mapType = scEditor->map_cbb_mapType->GetSelectedIndex();
	assert(scEditor && scEditor->map_edt_seed->IsCheckSumValid());
	char *mapSeedText = AOE_METHODS::UI_BASE::GetEditText(scEditor->map_edt_seed);
	long int terrainId = 0;
	long int mapSeed = -1; // Default = -1 (= random)
	long int playerCount = (scEditor->pl_cbb_playerCount->GetSelectedIndex()) + 1;

	if ((scEditor->currentMapGenerationChoice < 1) || (scEditor->currentMapGenerationChoice > 3)) {
		return false;
	}
	bool usesRMGeneration = false;
	if ((scEditor->currentMapGenerationChoice == 2) || (scEditor->currentMapGenerationChoice == 3)) {
		usesRMGeneration = true;
		if (sizeX + sizeY < 30) {
			traceMessageHandler.WriteMessage("WARNING: map size is too small to generate a random map. Generation interrupted.");
			return false; // Too small maps crash at generation. Use an empty map instead.
		}
	}

	// Empty
	if (scEditor->currentMapGenerationChoice == 1) {
		assert(scEditor->map_cbb_defaultTerrain);
		switch (scEditor->map_cbb_defaultTerrain->GetSelectedIndex()) {
		case 0:
			terrainId = 0; // grass
			break;
		case 1:
			terrainId = 6; // desert
			break;
		case 2:
			terrainId = 1; // water
			break;
		case 3:
			terrainId = 0x16; // deep water
			break;
		default:
			break;
		}
		_asm {
			PUSH sizeX
			PUSH sizeY
			PUSH terrainId
			PUSH global
			PUSH gaia
			MOV ECX, mapInfo
			MOV EDX, 0x444580 // gameMapInfo.GenerateEmptyMap
			CALL EDX
		}
	}
	long int previousScenarioSeed = settings->scenarioMapSeed;
	if (scEditor->currentMapGenerationChoice == 2) {
		settings->scenarioMapSeed = -1; // Prepare (no) seed for map generation.
	}
	if (scEditor->currentMapGenerationChoice == 3) { // from seed
		mapSeed = atoi(mapSeedText);
		settings->scenarioMapSeed = mapSeed; // Prepare seed for map generation.
	}
	if (usesRMGeneration) { // random or seed
		for (int playerId = 0; playerId < 9; playerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
			assert(player && player->IsCheckSumValid());
			if (player && player->IsCheckSumValid()) {
				_asm {
					MOV ECX, player
					MOV EDX, 0x45DB40 // clearSelectedUnits
					CALL EDX
				}
			}
		}
		// Generate "random" map (with or without seed)
		_asm {
			PUSH playerCount
			PUSH mapType
			PUSH sizeX
			PUSH sizeY
			PUSH global
			MOV ECX, mapInfo
			MOV EDX, DS:[ECX]
			CALL DS:[EDX+0x28]
		}

		settings->scenarioMapSeed = previousScenarioSeed;
		char bufferSeed[10] = "";
		sprintf_s(bufferSeed, "%ld", settings->actualMapSeed);
		assert(scEditor->map_edt_seed_whenReadOnly);
		bool test = scEditor->map_edt_seed_whenReadOnly->IsCheckSumValid();
		AOE_METHODS::UI_BASE::SetLabelText(scEditor->map_edt_seed_whenReadOnly, bufferSeed);
		_asm {
			MOV EDX, 0x52605D
			CALL EDX // Recalculate pseudo random
		}
	}

	// Post-generation common actions
	long int posX = sizeX / 2;
	long int posY = sizeY / 2;
	float fposX = (float)posX;
	float fposY = (float)posY;
	// Mange player screen position
	for (int playerId = 0; playerId < 9; playerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		assert(player && player->IsCheckSumValid());
		if (player && player->IsCheckSumValid()) {
			_asm {
				PUSH posX
				PUSH posY
				MOV EDX, 0x45D310
				MOV ECX, player
				CALL EDX
				PUSH fposX
				PUSH fposY
				MOV EDX, 0x45D330
				MOV ECX, player
				CALL EDX
			}
		}
	}
	// Player data Realloc
	for (int playerId = 0; playerId < 9; playerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		assert(player && player->IsCheckSumValid());
		if (player && player->IsCheckSumValid()) {
			AOE_STRUCTURES::STRUCT_PLAYER_NEW_EXPLORED_TILES *unknown_struct = &player->newExploredTilesForInfAI;
			_asm {
				MOV EDX, 0x45EB40 // resetArray()
				MOV ECX, unknown_struct
				CALL EDX
				MOV EDX, 0x45E940 // player.SetVisibilityInfoOn?()
				MOV ECX, player
				CALL EDX
			}
		}
	}
	AOE_STRUCTURES::STRUCT_UI_LABEL *bigLbl = scEditor->map_lbl_bigGeneratingMapNotification;
	assert(bigLbl && bigLbl->IsCheckSumValid());
	_asm {
		MOV EDX, 0x4901B0 // UIscenarioEditor.refreshDiamondMap()
		MOV ECX, scEditor
		CALL EDX
	}
	AOE_METHODS::UI_BASE::ShowUIObject(bigLbl, false); // Remove the "generating" label
	return true;
}


// Handles all custom treatments at scenario editor UI creation (this is called at the end of standard UI creation).
void CustomRORCommand::CustomScenarioEditorUICreation(AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor) {
	if (!scEditor || !scEditor->IsCheckSumValid()) { return; }

	// Manage hidden terrains
	AOE_STRUCTURES::STRUCT_UI_LISTBOX *listBox = scEditor->trn_lst_terrainList;
	assert(listBox && listBox->IsCheckSumValid());
	if (listBox && listBox->IsCheckSumValid() && CUSTOMROR::crInfo.configInfo.showHiddenTerrainsInEditor) {
		// Note: unavailable terrainIDs are 2,3,5,7,8,9,11,12,14,15,16,17,18,21,23-31
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = scEditor->global;
		assert(global && global->IsCheckSumValid());
		if (!global && !global->IsCheckSumValid()) { return; }
		AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		if (!gameMapInfo && !gameMapInfo->IsCheckSumValid()) { return; }
		for (int terrainId = 2; terrainId < gameMapInfo->terrainCount; terrainId++) {
			AOE_STRUCTURES::STRUCT_TERRAIN_DEF *terrainDef = &gameMapInfo->terrainDefinitions[terrainId];
			// Exclude standard terrains (already in list)
			if ((terrainId > 1) && (terrainId != 10) && (terrainId != 13) &&
				(terrainId != 20) && (terrainId != 4) && (terrainId != 19) && (terrainId != 22)) {
				AOE_METHODS::UI_BASE::Listbox_addItem(listBox, listBox->itemCount, terrainDef->terrainName, terrainId);
			}
		}
	}

	// Manage custom map dimensions
	if (CUSTOMROR::crInfo.configInfo.useCustomMapDimensions &&
		scEditor->map_cbb_mapSize && scEditor->map_cbb_mapSize->IsCheckSumValid() &&
		scEditor->map_cbb_mapSize->underlyingListBox && scEditor->map_cbb_mapSize->underlyingListBox->IsCheckSumValid()) {
		AOE_METHODS::UI_BASE::Listbox_addItem(scEditor->map_cbb_mapSize->underlyingListBox,
			scEditor->map_cbb_mapSize->underlyingListBox->itemCount, localizationHandler.GetTranslation(CRLANG_ID_CUSTOM, "Custom"), 0);
	}
}


// This methods modifies provided variables that will be passed to "check unit placement" method before adding a unit in scenario editor.
// For example, to allow creating several units at the same location, force checkConflictingUnits to 0.
void CustomRORCommand::ApplyCustomizationOnEditorAddUnit(long int &checkVisibility, long int &checkHillMode, long int &editorMode, long int &checkAirModeAndHPBar, long int &checkConflictingUnits, bool &IgnoreTerrainRestrictions) {
	checkConflictingUnits = !CUSTOMROR::crInfo.configInfo.editor_allowUnitOverlapping;
	checkHillMode = !CUSTOMROR::crInfo.configInfo.editor_disableHillModeChecks;
	IgnoreTerrainRestrictions = CUSTOMROR::crInfo.configInfo.editor_disableTerrainRestrictions;
}


// Get matching terrainId when user selected a row in terrain list in scenario editor/terrain tab.
// Returns a terrainId, returns -1 if we want to use default behaviour (no custom choice)
long int CustomRORCommand::GetTerrainIdForSelectedTerrainIndex(AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor, long int selectedIndex) {
	if (!scEditor || !scEditor->IsCheckSumValid()) { return -1; }
	// In default game, the terrain list contains 9 terrains (indexes 0-8).
	// Standard available terrainIDs are 0=grass,10=forest,1=water,13=desert_palm,20=jungle,4=shallows,19=pine_forest,22=water_dark
	if (selectedIndex <= 8) { return -1; } // Standard terrains: let normal code be executed.
	AOE_STRUCTURES::STRUCT_UI_LISTBOX *terrains = scEditor->trn_lst_terrainList;
	assert(terrains && terrains->IsCheckSumValid());
	if (!terrains || !terrains->IsCheckSumValid()) { return -1; }
	if (selectedIndex >= terrains->itemCount) { return -1; } // Invalid entry. Should never occur.
	// Here we have selected a custom entry
	
	// unavailable terrainIDs are 2,3,5,7,8,9,11,12,14,15,16,17,18,21,23-31
	// TODO : get terrainId from listbox item "optionalId"
	// We can retrieve terrainId from listbox item "optionalId"
	AOE_STRUCTURES::STRUCT_UI_LIST_ITEM *currentItem = terrains->firstItem;
	long int currentIndex = 0;
	while ((currentIndex < terrains->itemCount) && (currentItem != NULL)) {
		if (currentIndex == selectedIndex) {
			return currentItem->optionalId;
		}
		currentIndex++;
		currentItem = currentItem->next;
	}
	
	return -1; // error case
}


// Handle a ROR's original code call to trace method (0x42C330) - supports other trace methods as long arguments are printf-like
// WARNING: Make sure the stack actually contains valid data for a sprintf call !
// (0x42C330 is used for other calls than log due to compiler optimizations)
// firstRORCallTextParamAddress is the address in stack of the text argument "xxx".
// Next potential arguments (%xxx) are next data in stack.
void CustomRORCommand::HandleRORDebugLogCall(unsigned long int firstRORCallTextParamAddress) {
	char **stackArgText = (char**)firstRORCallTextParamAddress;
	std::string msgInitial = "";
	if (*stackArgText) {
		msgInitial = *stackArgText;
	}

	if (msgInitial.length() <= 0) { return; }

	// Filter some messages if option is set (param=1 means filtered)
	if (CUSTOMROR::crInfo.configInfo.collectRORDebugLogs == 1) {
		// Exclude some technical messages that are not very interesting.
		if (msgInitial == "Resume requested") {
			return;
		}
		std::string s = msgInitial.substr(0, 5);
		if (s == "COMM:") {
			return;
		}
		if (s == "RGE_R") {
			return;
		}
	}

	// Count the number of arguments %... to guess the stack size to be provided to our call to sprintf.
	// Note: all args are DWORDS (4 bytes) except float (because they are double(=QWORD=8 bytes), actually)
	size_t pos = msgInitial.find('%');
	int additionalArgsCount = 0;
	int floatArgsCount = 0;
	size_t length = msgInitial.length();
	while (pos != std::string::npos) {
		additionalArgsCount++;
		char firstLetter = '\0';
		size_t posToFindLetter = pos+1;
		while ((posToFindLetter < length) && (firstLetter == '\0')) {
			char c = msgInitial[posToFindLetter];
			if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
				firstLetter = c;
			}
			if ((firstLetter == 'l') && (posToFindLetter + 1 < length) && (msgInitial[posToFindLetter + 1] == 'f')) {
				firstLetter = 'f'; // F***ing %lf
			}
			posToFindLetter++;
		}
		if (firstLetter == 'f') {
			floatArgsCount++; // %f (or %7.1f, etc) means 2 DWORDS in stack instead of 1.
		}
		/*if ((pos < length) && (msgInitial[pos + 1]) == 'f') {
			floatArgsCount++; // %f means 2 DWORDS in stack instead of 1.
		}*/
		pos = msgInitial.find('%', pos + 1);
	}
	long int totalStackDwords = additionalArgsCount + floatArgsCount; // takes into account the fact that float are indeed double (2 dwords, not 1)
	long int argsSizeInBytes = totalStackDwords * 4;
	char buf[200];
	int bufsize = sizeof(buf);
	// No choice, we NEED to execute a printf ourselves because of dynamic number/type of arguments
	_asm {
		SUB ESP, argsSizeInBytes // Alloc space in stack
		MOV ESI, stackArgText
		ADD ESI, 4 // Beginning position of stack to copy (source)
		MOV ECX, ESP // Beginning position of allocated space in stack for arguments (dest)
		XOR EDI, EDI // =0
	begin_loop:
		CMP EDI, totalStackDwords
		JGE end_loop
		MOV EAX, DS:[ESI]
		MOV DS:[ECX], EAX
		ADD ECX, 4
		ADD ESI, 4
		INC EDI
		CMP EDI, totalStackDwords
		JLE begin_loop
	end_loop:
		// Call printf
		MOV EDX, stackArgText
		MOV ESI, DS:[EDX] // text
		LEA EAX, buf
		PUSH ESI // text
#ifdef _CRT_SECURE_NO_WARNINGS
		// This version can be used if we don't want to use sprintf_s, but it's not recommended.
		PUSH EAX // buffer
		call sprintf
#else
		// Using sprintf_s is recommended.
		PUSH bufsize
		PUSH EAX // buffer
		call sprintf_s
		ADD ESP, 4 // for bufsize argument.
#endif
		// POP arguments
		MOV EAX, argsSizeInBytes // "%..." arguments
		ADD EAX, 8 // First 2 arguments
		ADD ESP, EAX
	}

	std::string msgFinal = "[AOE] "; // So we can distinguish game's messages and ours.
	msgFinal += std::string(buf);
	// Do not trigger notifications for ROR executable logs.
	traceMessageHandler.WriteMessageNoNotification(msgFinal);
}


// Fix the check on path finding when trying to assign a task to a villager
long int CustomRORCommand::GathererCheckPathFinding(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *actorAsType50, long int *pathFindingArgs) {
	AOE_STRUCTURES::STRUCT_PLAYER *player = actorAsType50->ptrStructPlayer;
	assert(player && player->IsCheckSumValid());
	bool doFix = !CUSTOMROR::crInfo.configInfo.doNotApplyFixes; // *True* unless the "no fix" flag is set.

#ifdef _DEBUG
#pragma message("DEBUG trick path finding/gathering")
	// DEBUG ONLY : condition on player
	doFix = doFix && player && IsImproveAIEnabled(player->playerId);
	// END DEBUG
#endif

	float oldX = actorAsType50->positionX;
	float oldY = actorAsType50->positionY;
	if (doFix) {
		// Fix: force unit position to path finding start position. Dirty but efficient.
		actorAsType50->positionX = (float)pathFindingArgs[2];
		actorAsType50->positionY = (float)pathFindingArgs[1];
	}

	// Do the call
	long int pathFindingResult = AOE_METHODS::CallFindPathForUnit(pathFindingArgs);

	if (doFix) {
		// Do NOT forget to restore correct values !!!!
		actorAsType50->positionX = oldX;
		actorAsType50->positionY = oldY;
	}
	return pathFindingResult;
}


// Write the F11 centered text (if displayed)
// Warning, this is only refreshed when population changes ?
void CustomRORCommand::WriteF11PopInfoText(AOE_STRUCTURES::STRUCT_UI_F11_POP_LABEL *f11panel, char *bufferToWrite, char *defaultFormat,
	char *localizedText, long int currentPop, long int houseMaxPop) {
	const int bufferSize = 200; // do not know for sure.
	if (!CUSTOMROR::crInfo.configInfo.showCustomPopInfo) {
		// Default behaviour
		sprintf_s(bufferToWrite, 200, defaultFormat, localizedText, currentPop, houseMaxPop);
		return;
	}
	// Custom treatments: build a custom text
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
	assert(player && player->IsCheckSumValid());
	long int villCount = -1;
	long int boatVillCount = -1;
	// Try to get actual villager count (including fishing ships, etc...)
	if (player && player->IsCheckSumValid()) {
		villCount = PLAYER::GetPlayerUnitCount(player, -1, TribeAIGroupCivilian, 2, 2);
		boatVillCount = PLAYER::GetPlayerUnitCount(player, -1, TribeAIGroupFishingBoat, 2, 2) +
			PLAYER::GetPlayerUnitCount(player, -1, TribeAIGroupTradeBoat, 2, 2) +
			PLAYER::GetPlayerUnitCount(player, -1, TribeAIGroupTransportBoat, 2, 2);
		//villCount = (long int)player->GetResourceValue(CST_RES_ORDER_CIVILIAN_POPULATION); // DONT DO THIS because resource is NOT always up to date
	}
	std::string res = localizedText;
	res += std::to_string(currentPop);
	res += "/";
	res += std::to_string(houseMaxPop);
	if ((villCount >= 0) && (boatVillCount >= 0)) {
		res += " (";
		res += std::to_string(villCount);
		if (boatVillCount > 0) {
			res += "+";
			res += std::to_string(boatVillCount);
		}
		res += " vill.)";
	}
	strcpy_s(bufferToWrite, bufferSize, res.c_str());
}


// Disable dock for all players on maps where AI does NOT builds docks.
void CustomRORCommand::DisableWaterUnitsIfNeeded() {
	if (!CUSTOMROR::crInfo.configInfo.noDockInMostlyLandMaps) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings != NULL);
	assert(settings->IsCheckSumValid());
	if (!settings) { return; } // Should never happen
	if (settings->isCampaign || settings->rgeGameOptions.isScenario || settings->isSavedGame || settings->rgeGameOptions.isMultiPlayer) { return; }

	if (!IsDockRelevantForMap(settings->mapTypeChoice)) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
		if (!global || !global->IsCheckSumValid() || !global->ptrPlayerStructPtrTable) { return; }
		for (int i = 1; i < global->playerTotalCount; i++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = global->ptrPlayerStructPtrTable[i];
			if (player && player->IsCheckSumValid() && (player->structDefUnitArraySize > CST_UNITID_DOCK) &&
				player->ptrStructDefUnitTable && player->ptrStructDefUnitTable[CST_UNITID_DOCK]) {
				traceMessageHandler.WriteMessageNoNotification(std::string("Disabled dock for player #") + std::to_string(i));
				player->ptrStructDefUnitTable[CST_UNITID_DOCK]->availableForPlayer = 0;
			}
		}
	}
}

// Disable all walls for current game.
void CustomRORCommand::DisableWalls() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings != NULL);
	assert(settings->IsCheckSumValid());
	if (!settings) { return; } // Should never happen
	if (settings->isCampaign || settings->rgeGameOptions.isScenario || settings->isSavedGame || settings->rgeGameOptions.isMultiPlayer) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->ptrPlayerStructPtrTable) { return; }
	traceMessageHandler.WriteMessageNoNotification(std::string("Disable walls for all players"));
	for (int i = 1; i < global->playerTotalCount; i++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = global->ptrPlayerStructPtrTable[i];
		if (player && player->IsCheckSumValid())
		{
			if ((player->structDefUnitArraySize > CST_UNITID_SMALL_WALL) &&
				player->ptrStructDefUnitTable && player->ptrStructDefUnitTable[CST_UNITID_SMALL_WALL]) {
				player->ptrStructDefUnitTable[CST_UNITID_SMALL_WALL]->availableForPlayer = 0;
			}
			AOE_METHODS::AOE_enableResearch(player, CST_RSID_SMALL_WALL, false);
			AOE_METHODS::AOE_enableResearch(player, CST_RSID_MEDIUM_WALL, false);
			AOE_METHODS::AOE_enableResearch(player, CST_RSID_FORTIFICATION, false);
		}
	}
}


// Called on each loop in infAI.FindEnemyUnitIdWithinRange(ptrMyReferenceUnit, maxDistance, DATID, DATID, DATID, DATID)
// This is called quite often (only if improve AI is enabled in customROR configuration)
void CustomRORCommand::OnFindEnemyUnitIdWithinRangeLoop(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *currentUnitListElem) {
	if (!infAI || !infAI->IsCheckSumValid() || !currentUnitListElem || !infAI->ptrMainAI || !infAI->ptrMainAI->IsCheckSumValid()) { return; }
	if (IsMultiplayer()) { return; }

	AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(currentUnitListElem->unitId);
	bool elementWasReset = false;
	// Custom treatment: clean obsolete units
	// If element "memorized" position is visible...
	if (PLAYER::IsFogVisibleForPlayer(infAI->ptrMainAI->player, currentUnitListElem->posX, currentUnitListElem->posY)) {
		// Clean entry if: (we are in the case when "unit list" position is visible, so we can update it without cheating !)
		// - unit no longer exist
		// - unit moved to a position which is NO LONGER visible to me
		if (!unitBase || !unitBase->ptrStructPlayer ||
			(!PLAYER::IsFogVisibleForPlayer(infAI->ptrMainAI->player, (long int)unitBase->positionX, (long int)unitBase->positionY))) {
			if (CUSTOMROR::crInfo.configInfo.collectRORDebugLogs == 2) {
				int noLongerExists = (unitBase == NULL) ? 1 : 0;
				std::string s = "Removed unit #";
				s += std::to_string(currentUnitListElem->unitId);
				s += " from unitElemList (p#";
				s += std::to_string(infAI->commonAIObject.playerId);
				s += "), wasDead=";
				s += std::to_string(noLongerExists);
				traceMessageHandler.WriteMessageNoNotification(s.c_str());
			}
			elementWasReset = ResetInfAIUnitListElem(currentUnitListElem);
		}
	}
	if (!elementWasReset && unitBase && unitBase->ptrStructPlayer && (unitBase->ptrStructPlayer->playerId != currentUnitListElem->playerId)) {
		// If unit owner player does not match element's playerId (unit has been captured/converted) => important to fix, causes bad behaviors.
		if (UpdateOrResetInfAIUnitListElem(infAI, currentUnitListElem)) {
			if (CUSTOMROR::crInfo.configInfo.collectRORDebugLogs >= 2) {
				std::string s = "Updated unit #";
				s += std::to_string(currentUnitListElem->unitId);
				s += " owner from unitElemList (p#";
				s += std::to_string(infAI->commonAIObject.playerId);
				s += ")";
				traceMessageHandler.WriteMessage(s.c_str());
			}
		}
	}
}


// Called at the end of showUnitCommandButtons
void CustomRORCommand::AfterShowUnitCommandButtons(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI) {
	assert(gameMainUI && gameMainUI->IsCheckSumValid());
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		return;
	}
	if (gameMainUI->panelSelectedUnit == NULL) {
		return;
	}
	if (!IsGameRunning() || !CUSTOMROR::crInfo.configInfo.useImprovedButtonBar) {
		return;
	}

	BUTTONBAR::AfterShowUnitCommandButtons(gameMainUI);
}


// User interface command handler for 1 single unit.
// isPanelUnit = true if unitBase is the main selected unit (the one visible in bottom-left unit info zone)
// "Common" treatments (interface updates, etc) are only done when isPanelUnit = true
bool CustomRORCommand::ApplyUserCommandForUnit(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
	AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID uiCommandId, long int infoValue, AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase,
	bool isPanelUnit) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
	bool unitIsMine = (player == unitBase->ptrStructPlayer);

	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_STOP) {
		// Fix strategy when AI is enabled and some action is interrupted by human player
		if (unitIsMine && unitBase && unitBase->IsCheckSumValidForAUnitClass()) {
			AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = STRATEGY::GetStrategyElementForActorBuilding(player, unitBase->unitInstanceId);
			STRATEGY::ResetStrategyElementStatus(stratElem); // does nothing if stratElem is NULL.
		}
		return false; // Let ROR code execute normally here, we just ran "auxiliary" treatments.
	}
	if (uiCommandId == CST_IUC_CROR_DEFEND_STOP) {
		if (unitIsMine && (settings->mouseActionType == MOUSE_ACTION_TYPES::CST_MAT_CR_PROTECT_UNIT_OR_ZONE)) {
			UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(unitBase->unitInstanceId);
			if (unitInfo) {
				unitInfo->ResetProtectInfo();
				CUSTOMROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(unitBase->unitInstanceId);
#ifdef _DEBUG
				if (isPanelUnit) { AOE_METHODS::CallWriteCenteredText("Removed protect info"); }
#endif
			}
			if (isPanelUnit) {
				settings->mouseActionType = MOUSE_ACTION_TYPES::CST_MAT_NORMAL;
				AOE_METHODS::SetGameCursor(GAME_CURSOR::GC_NORMAL);
				BUTTONBAR::ForceRefresh(settings->ptrGameUIStruct);
			}
		}
		return true;
	}

	return false;
}


// Called when a game UI command button is clicked.
// Returns true if event has been handled and must NOT be handle by ROR standard code.
// Returns false by default (most cases) !
bool CustomRORCommand::OnGameCommandButtonClick(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
	AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID uiCommandId, long int infoValue) {

	if (!CUSTOMROR::crInfo.configInfo.useImprovedButtonBar) {
		return false;
	}

	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		return false;
	}
	if (!IsGameRunning()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *panelUnitBase = NULL;
	AOE_STRUCTURES::STRUCT_PLAYER *player = NULL;
	if (gameMainUI->panelSelectedUnit && gameMainUI->panelSelectedUnit->IsCheckSumValidForAUnitClass()) {
		panelUnitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) gameMainUI->panelSelectedUnit;
		if (panelUnitBase && panelUnitBase->IsCheckSumValidForAUnitClass()) {
			player = panelUnitBase->ptrStructPlayer;
		}
	}
	// No additional actions when viewing a unit that is not mine !
	if (!panelUnitBase || !player || !player->IsCheckSumValid() || (player != GetControlledPlayerStruct_Settings())) {
		return false;
	}

	if ((uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CANCEL_SELECTION) ||
		(uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CANCEL_OR_BACK)){
		if (settings->mouseActionType == MOUSE_ACTION_TYPES::CST_MAT_CR_PROTECT_UNIT_OR_ZONE) {
			settings->mouseActionType = MOUSE_ACTION_TYPES::CST_MAT_NORMAL;
			AOE_METHODS::SetGameCursor(GAME_CURSOR::GC_NORMAL);
			BUTTONBAR::ForceRefresh();
			return true; // this action "exited" from "select unit to defend" action. It must not unselect unit too ! Mark event as handled.
		}
	}


	// Handle next page. Note: in ROR, see 485140 (for villager build menu only)
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_NEXT_PAGE) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = panelUnitBase->unitDefinition;
		if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) {
			return false;
		}
		// Do not customize villagers... at this point
		if (IsVillager(unitDefBase->DAT_ID1)) {
			return false;
		}
		bool hasNextPage = (gameMainUI->unitCommandButtons[5]->buttonInfoValue[0] != 0); // this custom info is stored when button is added in AfterShowUnitCommandButtons
		if (hasNextPage) { // has a next page: increment button offset by 10.
			gameMainUI->panelButtonIdPageOffset += 10;
		} else { // No next page = go back to first page
			gameMainUI->panelButtonIdPageOffset = 0;
		}
		const unsigned long int addrShowUnitCommands = 0x4822A0;
		_asm {
			MOV ECX, gameMainUI;
			CALL addrShowUnitCommands;
		}
		return true; // Do not execute normal code for NEXT PAGE
	}

	// Unit-specific treatments
	bool eventFullyHandled = false;
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = CUSTOMROR::crInfo.GetRelevantSelectedUnitsBasePointer(player);
	assert(selectedUnits != NULL);
	for (int i = 0; i < player->selectedUnitCount; i++) {
		// Run treatments for all selected units (except "panel unit", will be done last)
		if (selectedUnits[i] && selectedUnits[i]->IsCheckSumValidForAUnitClass() && (selectedUnits[i] != panelUnitBase)) {
			// Note: also called for selected units that are NOT "mine", be careful.
			if (this->ApplyUserCommandForUnit(gameMainUI, uiCommandId, infoValue, selectedUnits[i], false)) {
				eventFullyHandled = true;
			}
		}
	}
	// Handle panel unit last, because common treatments are done there (may change Game ui, mouse type, etc)
	if (this->ApplyUserCommandForUnit(gameMainUI, uiCommandId, infoValue, panelUnitBase, true)) {
		eventFullyHandled = true;
	}
	if (eventFullyHandled) {
		return true;
	}

	// Custom buttons
	// Legacy code: auto-attack policies: update panel unit, then other selected units
	bool updateAutoAttackInfo = false;
	AutoAttackPolicy flagsToApply = { false, false, false, false, false };
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_DONT_ATTACK_VILLAGERS) {
		UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(panelUnitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = true;
		unitInfo->autoAttackPolicy.attackVillagers = false;
		flagsToApply.attackVillagers = true; // this flag has been updated
		BUTTONBAR::RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_DONT_ATTACK_BUILDINGS) {
		UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(panelUnitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = true;
		unitInfo->autoAttackPolicy.attackNonTowerBuildings = false;
		flagsToApply.attackNonTowerBuildings = true; // this flag has been updated
		BUTTONBAR::RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_NO_AUTO_ATTACK) {
		UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(panelUnitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = true;
		unitInfo->autoAttackPolicy.attackMilitary = false;
		unitInfo->autoAttackPolicy.attackNonTowerBuildings = false;
		unitInfo->autoAttackPolicy.attackVillagers = false;
		unitInfo->autoAttackPolicy.attackTowers = false;
		flagsToApply.attackMilitary = true; // this flag has been updated
		flagsToApply.attackNonTowerBuildings = true; // this flag has been updated
		flagsToApply.attackVillagers = true; // this flag has been updated
		flagsToApply.attackTowers = true; // this flag has been updated
		BUTTONBAR::RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_RESET_AUTO_ATTACK) {
		UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(panelUnitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = false;
		unitInfo->autoAttackPolicy.SetDefaultValues();
		flagsToApply.SetAllValues(true); // All flags have been updated.
		CUSTOMROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(panelUnitBase->unitInstanceId);
		BUTTONBAR::RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	// Apply changes on ALL compatible selected units
	if (updateAutoAttackInfo) {
		UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(panelUnitBase->unitInstanceId);
		assert(unitInfo != NULL); // Was just added
		if (!unitInfo) { return false; } // this is an error case
		CUSTOMROR::crInfo.ApplyAutoAttackPolicyToPlayerSelectedUnits(player, unitInfo->autoAttackPolicy, flagsToApply);
	}

	// Button "protect unit or protect zone" : set mouse custom cursor type
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_DEFEND) {
		AOE_METHODS::SetGameCursor(GAME_CURSOR::GC_GROUP);
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		if (settings && settings->IsCheckSumValid()) {
			settings->mouseActionType = MOUSE_ACTION_TYPES::CST_MAT_CR_PROTECT_UNIT_OR_ZONE;
			AOE_METHODS::CallWriteCenteredText(localizationHandler.GetTranslation(CRLANG_ID_BTN_UNIT_SET_PROTECT_OBJECT, "Right-click to define the unit or the position to protect"));
			if (settings->ptrGameUIStruct && settings->ptrGameUIStruct->gamePlayUIZone) {
				UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(panelUnitBase->unitInstanceId);
				if (unitInfo && (unitInfo->protectUnitId > -1)) {
					AOE_METHODS::DisplayGreenBlinkingOnUnit(settings->ptrGameUIStruct->gamePlayUIZone, unitInfo->protectUnitId, 1000);
				}
			}
		}
		BUTTONBAR::SetButtonBarForDefendUnitOrZone(gameMainUI, (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE*)panelUnitBase);
	}

	return false;
}


// Manages the display of a unit shortcut for non-standard shortcuts.
// Returns true if we want to let standard game code execute the shortcut display operation. (default false)
bool CustomRORCommand::DisplayCustomUnitShortcutSymbol(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase,
	long int posX, long int posY, long int unknown_arg3) {
	if (!unitBase || !unitBase->IsCheckSumValidForAUnitClass()) {
		return false;
	}

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	AOE_STRUCTURES::STRUCT_PLAYER *player = unitBase->ptrStructPlayer;
	assert(player && player->IsCheckSumValid() && global && global->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return false; }
	if (!global || !global->IsCheckSumValid()) { return false; }

	char shortcutInternalValue = unitBase->shortcutNumber;
	bool shortcutIsCompatibleWithStandardGame = (shortcutInternalValue > 0) && (shortcutInternalValue < 10); // Only 1-9 are supported in original code
	// Always show shortcut numbers in editor.
	if (shortcutIsCompatibleWithStandardGame && (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR)) {
		return true;
	}
	// Default behaviour : do NOT display shortcut from other players
	if (global->humanPlayerId != player->playerId) {
		return false;
	}
	// Valid situation (unit is mine = display the shortcut) : custom code to display custom shortcuts (other than 1-9)

	// Standard shortcuts = standard behaviour
	if (shortcutIsCompatibleWithStandardGame) {
		return true;
	}

	if (!IsValidInternalShortcutNumber(shortcutInternalValue) && (shortcutInternalValue <= 10)) {
		return false; // shortcutNumber is NOT a shortcut NOR a group ID : do not display anything (exit)
	}

	if (CUSTOMROR::crInfo.configInfo.doNotApplyFixes) {
		return false;
	}

	// Here: unit has a custom shortcut OR shortcut 10 (not displayed in standard game)

	if (CUSTOMROR::crInfo.customRorIcons.slpSize <= 0) {
		return false; // Error case: missing SLP data
	}

	long int slpFileIndex = 0; // slpFileIndex = SLP file index, index starting at 1.
	long int slpArrayIndex = 0; // Index starting at 0 in binary array
	if (IsValidInternalShortcutNumber(shortcutInternalValue)) {
		char shortcutDisplayValue = GetShortcutDisplayValueFromInternalValue(shortcutInternalValue); // should be 10-20
		assert((shortcutDisplayValue >= 10) && (shortcutDisplayValue <= 20));
		if ((shortcutDisplayValue < 10) || (shortcutDisplayValue > 20)) {
			return false;
		}
		slpFileIndex = (shortcutDisplayValue - 10) + // get a "index" 0-10 instead of a value 10-20
			AOE_CONST_DRS::CST_CUSTOMROR_SLP_INDEX_FOR_UNIT_SHORTCUT_10; // Add offset to position on "shortcut 10" index in SLP file
	} else {
		slpFileIndex = AOE_CONST_DRS::CST_CUSTOMROR_SLP_INDEX_FOR_GROUPED_UNIT; // shortcut corresponds to a unit group, not a shortcut
	}
	slpArrayIndex = slpFileIndex - 1;
	if ((slpFileIndex < 0) || (slpFileIndex > AOE_CONST_DRS::CST_CUSTOMROR_MAX_SLP_INDEX_IN_UNIT_SHORTCUTS_FILE) ||
		((slpFileIndex > AOE_CONST_DRS::CST_CUSTOMROR_MAX_SLP_INDEX_FOR_UNIT_SHORTCUTS) && (slpFileIndex != AOE_CONST_DRS::CST_CUSTOMROR_SLP_INDEX_FOR_GROUPED_UNIT))
		) {
		std::string msg = "ERROR: tried to use a wrong Slp.itemIndex: ";
		msg += std::to_string(slpFileIndex);
		traceMessageHandler.WriteMessage(msg);
		return false;
	}

	AOE_STRUCTURES::STRUCT_SLP_FILE_HEADER *slpHeader = CUSTOMROR::crInfo.customRorUnitShortcuts.slpFileHeader;
	// slpFrameHeaderBase = first array element (in slp frame headers array)
	AOE_STRUCTURES::STRUCT_SLP_FRAME_HEADER *slpFrameHeaderBase = (AOE_STRUCTURES::STRUCT_SLP_FRAME_HEADER *)
		(slpHeader + 1); // Dirty, but works because structs have same size (done like this in ROR code)
	AOE_STRUCTURES::STRUCT_SLP_FRAME_HEADER *slpFrameHeader = slpFrameHeaderBase + slpArrayIndex; // Move to correct frame in array

	AOE_METHODS::DisplayUnitShortcutSymbol(slpHeader, slpFrameHeader, posX - 9, posY - 8, unknown_arg3);
	return false;
}


// Adds custom attributes (armor) in buildings' unit info zone.
// currentLine is incremented if lines are added.
void CustomRORCommand::DisplayCustomBuildingAttackAttributesInUnitInfo(AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone, long int &currentLine) {
	if (!unitInfoZone || !unitInfoZone->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *unit50 = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)unitInfoZone->currentUnit;
	if (!unit50 || !unit50->IsCheckSumValidForAUnitClass()) {
		return; // Should not occur, this method is called during processing of buildings
	}

	// Show pierce if non-zero value
	short int pierceTotalValue = 0;
	short int pierceDisplayedValue = 0;
	short int meleeTotalValue = 0;
	short int meleeDisplayedValue = 0;
	long int iconIdMeleeArmor = AOE_CONST_DRS::UIZ_ICON_ARMOR; // melee armor icon in SLP (8)
	long int iconIdPierce = AOE_CONST_DRS::UIZ_ICON_SHIELD; // pierce armor icon in SLP (10)
	AOE_METHODS::UNIT::GetMeleeArmor(unit50, meleeDisplayedValue, meleeTotalValue);
	AOE_METHODS::UNIT::GetPierceArmor((AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE*)unit50, pierceDisplayedValue, pierceTotalValue);

	if (meleeTotalValue > 0) {
		// Note: line is incremented in the method if a line is added.
		AOE_METHODS::UI_BASE::UnitInfoZoneAddAttributeLine(unitInfoZone, iconIdMeleeArmor, 1, meleeDisplayedValue, meleeTotalValue, currentLine);
	}
	if (pierceTotalValue > 0) {
		// Note: line is incremented in the method if a line is added.
		AOE_METHODS::UI_BASE::UnitInfoZoneAddAttributeLine(unitInfoZone, iconIdPierce, 1, pierceDisplayedValue, pierceTotalValue, currentLine);
	}
}


// Entry point when mouse hovers on a unit. foundInteraction and foundHintDllId values are IN/OUT, you are allowed to update them to overload ROR default behaviour.
// Note: this only impacts mouse displayed cursor and hint text, not which right-click actions are actually possible.
// If returned cursorToForce is >= 0, then stop other treatments and use this value as new cursor.
// Returns true if output values have been updated.
bool CustomRORCommand::OnHoverOnUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, STRUCT_PLAYER *controlledPlayer, long int unitPlayerId,
	UNIT_INTERACTION_ID &foundInteraction, long int &foundHintDllId, GAME_CURSOR &cursorToForce) {
	cursorToForce = (GAME_CURSOR)-1; // Default
	if (!CUSTOMROR::crInfo.configInfo.useImprovedButtonBar) { // TODO: use a dedicated config
		return false;
	}
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !controlledPlayer || !controlledPlayer->IsCheckSumValid() || (unitPlayerId < 0)) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
	assert(unitDef && unitDef->IsCheckSumValidForAUnitClass());
	if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) { return false; }
	bool updatedValues = false;
	PLAYER_DIPLOMACY_VALUES diplValue = controlledPlayer->diplomacyVSPlayers[unitPlayerId];
	GLOBAL_UNIT_AI_TYPES actorAIType = TribeAINone; // controlled player's main selected unit AI type
	bool actorIsVillager = false; // true if controlled player's main selected unit is a civilian (class 4)
	AOE_STRUCTURES::STRUCT_UNIT_BASE *mainSelectedUnit = NULL;
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *selectedUnitDef = NULL;

	if (unit->DerivesFromTrainable()) {
		mainSelectedUnit = CUSTOMROR::crInfo.GetMainSelectedUnit(controlledPlayer);
		if (mainSelectedUnit && mainSelectedUnit->IsCheckSumValidForAUnitClass() && mainSelectedUnit->DerivesFromTrainable()) {
			selectedUnitDef = mainSelectedUnit->unitDefinition;
			if (selectedUnitDef && selectedUnitDef->IsCheckSumValidForAUnitClass()) {
				actorIsVillager = (selectedUnitDef->unitAIType == TribeAIGroupCivilian);
				actorAIType = selectedUnitDef->unitAIType;
			}
		}
	}

	// High priority : if mouse status is a custom one, prevent updating mouse cursor inconsistently
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	if (settings->mouseActionType < 0) {
		foundHintDllId = 0;
		foundInteraction = UNIT_INTERACTION_ID::UII_NO_INTERACTION;
		cursorToForce = BUTTONBAR::BUTTONBAR_CONST::GetCursorForCustomActionType(settings->mouseActionType); // Force usage of this mouse cursor
		return true; // Disable further treatments.
	}

	// Villagers and owned farm: show "hand" cursor
	if ((unit->unitStatus == 2) && (unitDef->DAT_ID1 == CST_UNITID_FARM) && actorIsVillager && (diplValue == PLAYER_DIPLOMACY_VALUES::CST_PDV_SELF)) {
		foundInteraction = UNIT_INTERACTION_ID::UII_GATHER_RESOURCES;
		foundHintDllId = LANG_ID_HINT_CLICK_TO_GATHER_BUSH_HERE; // not exactly correct, this is for bushes, not farms !
		updatedValues = true;
	}
	// Fishing ships and villagers can fish : show "hand" cursor. However issues with fish SHP files make that "mouse hover" is not correctly detected by game
	if ((actorAIType == TribeAIGroupFishingBoat) || (actorAIType == TribeAIGroupCivilian)) {
		if ((unitDef->unitAIType == TribeAIGroupSeaFish) || (unitDef->unitAIType == TribeAIGroupShoreFish) || (unitDef->unitAIType == TribeAIGroupUnknownFish)) {
			foundHintDllId = LANG_ID_HINT_CLICK_TO_FISH_HERE; // "Click to fish here"
			updatedValues = true;
		}
	}
	// Trade units
	if (selectedUnitDef && selectedUnitDef->DerivesFromCommandable()) {
		AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *selUnitDefCmd = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)selectedUnitDef;
		if (selUnitDefCmd->ptrUnitCommandHeader) {
			for (int i = 0; i < selUnitDefCmd->ptrUnitCommandHeader->commandCount; i++) {
				if (selUnitDefCmd->ptrUnitCommandHeader->ptrCommandArray[i] && (selUnitDefCmd->ptrUnitCommandHeader->ptrCommandArray[i]->commandType == UNIT_ACTION_ID::CST_IAI_TRADE)) {
					if ((selUnitDefCmd->ptrUnitCommandHeader->ptrCommandArray[i]->unitDefId == unit->unitDefinition->DAT_ID1) ||
						(selUnitDefCmd->ptrUnitCommandHeader->ptrCommandArray[i]->classId == unit->unitDefinition->unitAIType)) {
						foundInteraction = UNIT_INTERACTION_ID::UII_NO_INTERACTION;
						cursorToForce = GAME_CURSOR::GC_HAND; // TODO: it would be great to have a dedicated cursor !
						foundHintDllId = LANG_ID_HINT_TRADE_WITH;
						updatedValues = true;
					}
				}
			}
		}
	}
	// Unit holding resource -> drop site
	if (mainSelectedUnit && (mainSelectedUnit->resourceValue > 0) && selectedUnitDef && selectedUnitDef->DerivesFromCommandable()) {
		AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *selUnitDefCmd = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)selectedUnitDef;
		if ((selUnitDefCmd->dropSite1 == unit->unitDefinition->DAT_ID1) || (selUnitDefCmd->dropSite2 == unit->unitDefinition->DAT_ID1)) {
			foundInteraction = UNIT_INTERACTION_ID::UII_NO_INTERACTION;
			cursorToForce = GAME_CURSOR::GC_HAND; // TODO: it would be great to have a dedicated cursor !
			foundHintDllId = -1;
			updatedValues = true;
		}
	}
	return updatedValues;
}


// Handles a right-click (in-game) when mouse action type is a custom one, for 1 single selected unit
// posX and posY are game positions
// mouseTargetUnit can be NULL
void CustomRORCommand::OnInGameRightClickCustomAction(float posX, float posY, AOE_STRUCTURES::STRUCT_UNIT_BASE *mouseTargetUnit,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit) {
	if (!actorUnit || !actorUnit->IsCheckSumValidForAUnitClass()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	assert(controlledPlayer && controlledPlayer->IsCheckSumValid());
	bool actorIsMyUnit = (actorUnit->ptrStructPlayer == controlledPlayer);
	UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(actorUnit->unitInstanceId);

	switch (settings->mouseActionType) {
	case CST_MAT_CR_PROTECT_UNIT_OR_ZONE:
		if (actorIsMyUnit && (actorUnit == mouseTargetUnit)) {
			unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(actorUnit->unitInstanceId);
			if (unitInfo) {
				unitInfo->ResetProtectInfo();
				CUSTOMROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(actorUnit->unitInstanceId);
			}
			AOE_METHODS::CallWriteCenteredText("Removed protect information for current unit.");
			if (settings->ptrGameUIStruct && settings->ptrGameUIStruct->gamePlayUIZone) {
				AOE_METHODS::DisplayGreenBlinkingOnUnit(settings->ptrGameUIStruct->gamePlayUIZone, actorUnit->unitInstanceId, 1000);
			}
			break;
		}
		if (actorIsMyUnit && actorUnit && actorUnit->DerivesFromMovable()) {

			if (!unitInfo) {
				unitInfo = CUSTOMROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(actorUnit->unitInstanceId);
			}
			if (unitInfo) {
				unitInfo->ResetProtectInfo();
				if (mouseTargetUnit) {
					unitInfo->protectUnitId = mouseTargetUnit->unitInstanceId;
					if (settings->ptrGameUIStruct && settings->ptrGameUIStruct->gamePlayUIZone) {
						AOE_METHODS::DisplayGreenBlinkingOnUnit(settings->ptrGameUIStruct->gamePlayUIZone, mouseTargetUnit->unitInstanceId, 1000);
					}
				} else {
					unitInfo->protectPosX = posX;
					unitInfo->protectPosY = posY;
				}
			}
		}
		break;
	default:
		break;
	}
}


// Handles a right-click (in-game) when mouse action type is a custom one.
// mouseTargetUnit can be NULL
void CustomRORCommand::OnInGameRightClickCustomAction(float posX, float posY, AOE_STRUCTURES::STRUCT_UNIT_BASE *mouseTargetUnit) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	assert(settings->mouseActionType < 0); // here, we should only handle custom action types
	if ((posX < 0) || (posY < 0)) { return; } // invalid position
	if (mouseTargetUnit) {
		assert(mouseTargetUnit->IsCheckSumValidForAUnitClass());
		if (!mouseTargetUnit->IsCheckSumValidForAUnitClass()) { return; }
	}
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	assert(controlledPlayer && controlledPlayer->IsCheckSumValid());

	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit = CUSTOMROR::crInfo.GetMainSelectedUnit(controlledPlayer);
	bool actorIsMyUnit = false;
	if (actorUnit) {
		assert(actorUnit->IsCheckSumValidForAUnitClass());
		if (!actorUnit->IsCheckSumValidForAUnitClass()) { actorUnit = NULL; }
		actorIsMyUnit = (actorUnit->ptrStructPlayer == controlledPlayer);
	}
	if (!actorIsMyUnit) { return; }
	
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = CUSTOMROR::crInfo.GetRelevantSelectedUnitsBasePointer(controlledPlayer);
	assert(selectedUnits != NULL);
	for (int i = 0; i < controlledPlayer->selectedUnitCount; i++) {
		// Run treatments for all selected units
		if (selectedUnits[i] && selectedUnits[i]->IsCheckSumValidForAUnitClass()) {
			// Note: also called for selected units that are NOT "mine", be careful.
			this->OnInGameRightClickCustomAction(posX, posY, mouseTargetUnit, selectedUnits[i]);
		}
	}

	// Common post-actions

	// Restore mouse status before quitting
	switch (settings->mouseActionType) {
	case CST_MAT_CR_PROTECT_UNIT_OR_ZONE:
		BUTTONBAR::ForceRefresh(settings->ptrGameUIStruct);
		break;
	default:
		break;
	}
	settings->mouseActionType = MOUSE_ACTION_TYPES::CST_MAT_NORMAL;
	AOE_METHODS::SetGameCursor(GAME_CURSOR::GC_NORMAL);
}


// Entry point when a unit activity stops.
void CustomRORCommand::OnUnitActivityStop(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity) {
	AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *unit = (AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *)activity->ptrUnit;
	if (!unit) { return; }
	assert(unit->IsCheckSumValidForAUnitClass());
	if (!unit->IsCheckSumValidForAUnitClass()) { return; }
	if (!unit->DerivesFromCommandable()) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	if (unit->ptrStructPlayer != controlledPlayer) { return; }
	if (controlledPlayer->isComputerControlled) { return; }

	bool noNextActivity = true;
	// Remark: if manually stopped, activity->nextActivityQueueUsedElems == 0
	for (int i = 0; i < activity->notifyQueueUsedElemCount; i++) {
		ACTIVITY_TASK_IDS taskId = activity->notifyQueue[i].activityId;
		if (taskId != ACTIVITY_TASK_IDS::CST_ATI_NOTIFY_ACTION_COMPLETED) {
			noNextActivity = false;
		}
	}
	UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(unit->unitInstanceId);

	// Auto-protect unit/zone (unit will go back to zone/unit to defend if its current activity is over).
	if (noNextActivity && unitInfo && unitInfo->HasValidProtectInfo()
		&& CUSTOMROR::crInfo.configInfo.useImprovedButtonBar /*enableCallNearbyIdleMilitaryUnits*/) { // TODO: specific config
		float refX = unitInfo->protectPosX;
		float refY = unitInfo->protectPosY;
		AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)unit->unitDefinition;
		assert(unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromCommandable());
		float distance = unitDef->searchRadius / 2; // use a constant (to be closer, for range units !) ?
		if (distance < 2) { distance = 2; } // use a config ?
		if (unitInfo->protectUnitId >= 0) {
			AOE_STRUCTURES::STRUCT_UNIT_BASE *unitToProtect = GetUnitStruct(unitInfo->protectUnitId);
			if (unitToProtect && !unitToProtect->IsCheckSumValidForAUnitClass()) {
				unitToProtect = NULL;
				assert(!"ERROR: unit to protect has an invalid checksum");
			}
			if (unitToProtect) {
				if (!IsUnitPositionKnown(controlledPlayer, unitToProtect)) {
					unitToProtect = NULL; // Can't protect (follow) a unit if I don't see it (would be cheating)
				}
			}
			if (unitToProtect) {
				refX = unitToProtect->positionX;
				refY = unitToProtect->positionY;
			} else {
				// Unit is specified but invalid (maybe it died): remove obsolete info.
				unitInfo->ResetProtectInfo();
				CUSTOMROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(unitInfo->unitId);
			}
		}
		if ((refX >= 0) || (refY >= 0)) {
			if ((abs(unit->positionX - refX) > distance) || (abs(unit->positionY - refY) > distance)) {
				if (AOE_METHODS::UNIT::IsUnitIdle(unit) && unit->ptrActionInformation) {
					if (!unit->ptrActionInformation->ptrActionLink || !unit->ptrActionInformation->ptrActionLink->actionStruct) {
						// For MP compabitiliy; use a command
						if (!CUSTOMROR::crInfo.configInfo.forceMPCompatibility) {
							AOE_STRUCTURES::STRUCT_ACTION_MOVE *move = (AOE_STRUCTURES::STRUCT_ACTION_MOVE *)AOEAlloc(sizeof(AOE_STRUCTURES::STRUCT_ACTION_MOVE));
							move->Constructor(unit, NULL, distance, NULL);
							move->targetUnitPositionX = refX;
							move->targetUnitPositionY = refY;
							move->maxDistanceFromTarget = distance;
							unit->ptrActionInformation->AssignAction(move);
						} else {
							GAME_COMMANDS::CreateCmd_RightClick(unit->unitInstanceId, -1, refX, refY);
						}
					}
				}
			}
		}
	}
}


// Handle the optional display of debug information (like F5 info)
// Returns true if standard game info (F5 zone) must NOT be executed.
bool CustomRORCommand::HandleShowDebugGameInfo(AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings) {
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	if (*AOE_VAR_F5_DEBUG_INFO_TYPE <= CUSTOMROR::CONFIG::IDL_STANDARD_MAX_ALLOWED_LEVEL) {
		return false; // Do standard treatments.
	}
	// 2+ are custom debugging states. Here we are in such situation.

	switch (*AOE_VAR_F5_DEBUG_INFO_TYPE) {
	case CUSTOMROR::CONFIG::IDL_HIDDEN_COMM:
		_asm {
			MOV ECX, settings;
			MOV EDX, DS:[ECX];
			CALL DS:[EDX + 0x13C]; // gameSettings.showComm()
		}
		return true;
	case CUSTOMROR::CONFIG::IDL_HIDDEN_AI:
		_asm {
			MOV ECX, settings;
			MOV EDX, DS:[ECX];
			CALL DS:[EDX + 0x140]; // gameSettings.showAI()
		}
		return true;
	case CUSTOMROR::CONFIG::IDL_CUSTOM:
		if (CUSTOMROR::crInfo.configInfo.useF5LabelZoneForCustomDebugInfo) {
			AOE_METHODS::UI_BASE::GameMainUI_writeF5DebugInfo(settings->ptrGameUIStruct, "Test debugging");
		}
		{
			STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
			if (!global) { return false; }
			std::string msg[9];
			for (int i = 2; i <= 7; i++) {
				CUSTOM_AI::AIPlayerTargetingInfo *info = CUSTOM_AI::playerTargetingHandler.GetPlayerInfo(i);
				if (!info) { msg[i] = ""; continue; }
				int curTargetPlayerId = info->GetCurrentTacAITargetPlayerId(global->GetPlayerStruct(i));
				msg[i] = std::string("p#") + std::to_string(i) + std::string(" targt=") + std::to_string(curTargetPlayerId) +
					std::string(" ; subdslk vs");
				for (int j = 1; j < global->playerTotalCount; j++) {
					msg[i] += std::string(" p") + std::to_string(j) + std::string("=");
					msg[i] += to_string(info->lastComputedDislikeSubScore[i]);
				}
			}
			AOE_METHODS::UI_BASE::GameMainUI_writeTextDebugLines(settings->ptrGameUIStruct, msg[2].c_str(),
				msg[3].c_str(),
				msg[4].c_str(),
				msg[5].c_str(),
				msg[6].c_str(),
				msg[7].c_str()
				);
		}
		return true;
	default:
		break;
	}
	return true;
}


// Changes current in-game debug info level (F5)
void CustomRORCommand::SetNextInGameDebugInfoLevel() {
#ifdef GAMEVERSION_ROR10c
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) {
		return;
	}
	// Note: See 0x484BE0 = mainGameUI.OnF5() => is executed BEFORE this method ! So "debug level" has already been updated.
	// Note: mainGameUI.OnF5() does NOT reset AOE_VAR_F5_DEBUG_INFO_TYPE when debugging is disabled: we can guess what what the last debugging level.
	if (!settings->showDebugTimings && (*AOE_VAR_F5_DEBUG_INFO_TYPE >= CUSTOMROR::CONFIG::IDL_STANDARD_MAX_ALLOWED_LEVEL) &&
		CUSTOMROR::crInfo.configInfo.enableInGameDisplayDebugInfo) {
		assert(*AOE_VAR_F5_DEBUG_INFO_TYPE <= CUSTOMROR::CONFIG::IDL_COUNT);
		// Force additional states of debugging (custom debug levels).
		(*AOE_VAR_F5_DEBUG_INFO_TYPE)++;
		if (*AOE_VAR_F5_DEBUG_INFO_TYPE < CUSTOMROR::CONFIG::IDL_COUNT) {
			settings->showDebugTimings = 1;
			// Restore debug label visibility instead of resources (optional: depends if we want to show stuff in there)
			if (settings->ptrGameUIStruct && CUSTOMROR::crInfo.configInfo.useF5LabelZoneForCustomDebugInfo &&
				(*AOE_VAR_F5_DEBUG_INFO_TYPE != CUSTOMROR::CONFIG::IDL_HIDDEN_AI) && // This one does not use F5 debug text bar
				(*AOE_VAR_F5_DEBUG_INFO_TYPE != CUSTOMROR::CONFIG::IDL_HIDDEN_COMM) // This one does not use F5 debug text bar
				) {
				AOE_METHODS::UI_BASE::ShowUIObject(settings->ptrGameUIStruct->lblF5debugInfo, true);
				AOE_METHODS::UI_BASE::ShowUIObject(settings->ptrGameUIStruct->resourceValuesIndicator, false);
			}
		}
	}
#endif
}


// Opens a custom dialog message (based on CloseProgramDialog) and pauses game execution (if running)
// Return true if OK, false if failed - Fails if another custom dialog (or quit game dialog) is already open
// Pauses the game if running (only if a dialog is successfully opened)
bool CustomRORCommand::OpenCustomDialogMessage(const char *dialogText, long int hSize, long int vSize) {
	if (CUSTOMROR::crInfo.customYesNoDialogVar) { return false; } // Already an opened custom dialog

	AOE_STRUCTURES::STRUCT_ANY_UI *customDialogPtr = AOE_METHODS::GetScreenFromName(AOE_CONST_INTERNAL::customDialogScreenName);
	if (customDialogPtr != NULL) { return false; } // A CloseProgramDialog seems to be already open

	SetGamePause(true);
	CUSTOMROR::crInfo.customYesNoDialogVar = (unsigned long int*) AOE_METHODS::AOE_CreateDialogPopup(dialogText, hSize, vSize);

	return true;
}


// Closes currently opened custom dialog message.
// Returns -1 if an error occurred, including "no custom dialog is opened".
// Other results are AOE_CONST_INTERNAL::DIALOG_BUTTON_IDS => Yes/No/Cancel
long int CustomRORCommand::CloseCustomDialogMessage(AOE_STRUCTURES::STRUCT_UI_POPUP_DIALOG *ptrDialog, unsigned long int ptrSender) {
	if (CUSTOMROR::crInfo.customYesNoDialogVar == NULL) { return -1; } // No opened custom dialog
	long int returnValue = -1;
	if (!ptrDialog) { return returnValue; }
	if (ptrSender == (unsigned long) ptrDialog->btnYes) {
		returnValue = AOE_CONST_INTERNAL::DIALOG_BUTTON_IDS::CST_DBI_YES;
	}
	if (ptrSender == (unsigned long) ptrDialog->btnNo) {
		returnValue = AOE_CONST_INTERNAL::DIALOG_BUTTON_IDS::CST_DBI_NO;
	}
	if (ptrSender == (unsigned long) ptrDialog->btnCancel) {
		returnValue = AOE_CONST_INTERNAL::DIALOG_BUTTON_IDS::CST_DBI_CANCEL;
	}
	// Close the dialog
	// TO DO: use CloseScreenFullTreatment(..) instead
	AOE_METHODS::RefreshScreen("Game Screen", 0);
	assert(strcmp(ptrDialog->screenName, AOE_CONST_INTERNAL::customDialogScreenName) == 0);
	AOE_METHODS::CloseScreenAndDestroy(AOE_CONST_INTERNAL::customDialogScreenName);
	CUSTOMROR::crInfo.customYesNoDialogVar = NULL;

	long int *Ihavenoideawhatthisis = (long int *)0x7C0338; // cf 0x481264
	Ihavenoideawhatthisis = 0;
	SetGamePause(false);

	return returnValue;
}


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
*/
void CustomRORCommand::EntryPoint_GameSettingsNotifyEvent(long int eventId, short int playerId, long int arg3, long int arg4, long int arg5) {
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_RESEARCH_COMPLETE) {
		long int research_id = arg3;
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		assert(player && player->IsCheckSumValid());
		if (research_id == AOE_CONST_FUNC::CST_RSID_CATAPULT_TRIREME) {
			this->MoveFireGalleyIconIfNeeded(player);
		}
	}

	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_BUILDING_COMPLETE) {
		short int bldDAT_ID = (short int)arg3;
		if (bldDAT_ID = CST_UNITID_MARKET) {
			// Building a market (first one) enables farms.
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
			assert(player && player->IsCheckSumValid());
			CUSTOMROR::TRIGGER::ManageDisableUnitsForFarms(player);
		}
	}

	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_FARM_DEPLETED) {
		// This is called when a farm is fully depleted (and dies), NOT when a farm is destroyed/killed
		// Warning: when this event is called, the farm is killed just after, so be careful about modifications done here !
		long int farmUnitId = arg3;
		CUSTOMROR::UNIT::OnFarmDepleted(farmUnitId);
	}

	// Now manage triggers...
	CUSTOMROR::TRIGGER::ManageTriggersOnGameNotifyEvent(eventId, playerId, arg3, arg4, arg5);
}


// This is called for each player at game initialization, after applying tech trees and starting age.
// Resources and score info have not been initialized yet.
// This is called for all game types (SP / MP, RM/DM/scenario) but NOT for load game.
void CustomRORCommand::OnGameInitAfterApplyingTechTrees(long int playerId) {
	this->ApplyScenarioSpecificPlayerStartingAge(playerId);

	// For current player, disable the researches that can never be available
	DisablePlayerImpossibleResearches(GetPlayerStruct(playerId));
}


// Apply starting age to a player (only for scenarios). Player's specific starting age is read in STRUCT_SCENARIO_INFO
void CustomRORCommand::ApplyScenarioSpecificPlayerStartingAge(long int playerId) {
	if (!CUSTOMROR::crInfo.hasRemovePlayerInitialAgeInScenarioInit) {
		return;
	}
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return; }

	if (settings->rgeGameOptions.isScenario != 0) {
		AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = global->scenarioInformation;
		assert(scInfo && scInfo->IsCheckSumValid());
		long int startingAge = scInfo->playersStartingAge[playerId - 1]; // Warning: index is playerId - 1
		assert(player && player->IsCheckSumValid());
		if (player && player->IsCheckSumValid()) {
			const long int addr = 0x4F18A0;
			long int argAgeRef = 0;
			switch (startingAge) {
			case 1:
				argAgeRef = 0x19; // corresponds to tool age
				break;
			case 2:
				argAgeRef = 0x17; // corresponds to bronze age
				break;
			case 3:
				argAgeRef = 0x18; // corresponds to iron age
				break;
			case 4:
				argAgeRef = 1; // corresponds to post iron
				break;
			}
			if (startingAge > 0) {
				_asm {
					MOV ECX, player;
					PUSH argAgeRef;
					CALL addr; // player.applyStartingAgeToResearches?(researchIdResourceId)
				}
			}
		}
	}

}


}
