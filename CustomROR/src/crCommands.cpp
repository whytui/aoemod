#include "../include/crCommands.h"


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
		UnitSpawnShortcutInfo *sinfo = &CUSTOMROR::crInfo.configInfo.unitShortcutsInformation[shortcutId];
		if (sinfo->DAT_ID >= 0) {
			fprintf_s(f, "unitSpawn shortcut %d: unit=%03d onlyOneUnit=%d (%.12s)\n", shortcutId, sinfo->DAT_ID, sinfo->onlyOneUnit,
				GetHardcodedUnitName(sinfo->DAT_ID));
		}
	}
	fprintf_s(f, "autoRebuildFarms_maxFarms:                 %ld\n", CUSTOMROR::crInfo.configInfo.autoRebuildFarms_maxFarms);
	fprintf_s(f, "autoRebuildFarms_maxFood:                  %ld\n", CUSTOMROR::crInfo.configInfo.autoRebuildFarms_maxFood);
	fprintf_s(f, "autoRebuildFarms_minWood:                  %ld\n", CUSTOMROR::crInfo.configInfo.autoRebuildFarms_minWood);
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
	ChangeItfDRS_file();

	// Prepare custom DRS data
	this->LoadCustomDrsFiles();

	// Update "population limit getter" according to configuration
	SetMaxPopulationGetterInSPGames(CUSTOMROR::crInfo.configInfo.singlePlayerMaxPopulation);

#ifdef _DEBUG
	if (debugSerialization) {
		SetDeserializationDebugChange(true);
		WriteToFile("", serializationLogFilename, false); // reset file content
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
	for each (DrsFileToLoad *drs in CUSTOMROR::crInfo.configInfo.customDrsFilesList)
	{
		AOE_AddDrsFile(drs->filename.c_str(), drs->folder.c_str());
	}

	// Prepare custom DRS data
	if (CUSTOMROR::crInfo.configInfo.useImprovedButtonBar) {
		if (CUSTOMROR::crInfo.configInfo.showAlertOnMissingFeature && !FindDrsLinkForFile(CST_CUSTOMROR_DRS_FILENAME)) {
			MessageBoxA(0, "ERROR : Could not find customROR.drs or it is invalid.", "CustomROR", MB_ICONWARNING);
		}
		// Initialize global variable so we can retrieve our button icons when needed
		InitSlpInfoFromDrs(&CUSTOMROR::crInfo.customRorIcons, CST_CUSTOMROR_CMD_ICONS_SLP_ID);
		InitSlpInfoFromDrs(&CUSTOMROR::crInfo.customRorUnitShortcuts, CST_CUSTOMROR_UNIT_SHORTCUTS_SLP_ID);
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
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *gameSettings = *ROR_gameSettings;

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
				ChangeControlledPlayer(newId, true);
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
		AOE_clearSelectedUnits(player);
		if (unit && unit->IsCheckSumValidForAUnitClass()) {
			AOE_selectUnit(player, unit, true);
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
		if (player && player->IsCheckSumValid() && unitDef && unitDef->IsCheckSumValidForAUnitClass() && gameSettings->isSinglePlayer) {
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
			std::string s = ExportStrategyToText(&player->ptrAIStruct->structBuildAI);
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
		CallWriteText("Hello world !");
	}
	if (strcmp(command, "seed") == 0) {
		this->PrintMapSeed();
	}
	if ((strcmp(command, "t") == 0) || (strcmp(command, "time") == 0)) {
		this->PrintDateTime();
	}
	if ((strcmp(command, "ai") == 0) || (strcmp(command, "AI") == 0)) {
		this->RestoreAllAIFlags();
		CallWriteText(localizationHandler.GetTranslation(CRLANG_ID_AI_FLAGS_RECOMPUTED, "AI flags have been recomputed"));
	}
	if ((strcmp(command, "allai") == 0) || (strcmp(command, "allAI") == 0)) {
		this->SetAllAIFlags();
		CallWriteText(localizationHandler.GetTranslation(CRLANG_ID_AI_FLAGS_ALL_SET, "All players are now computer-managed."));
	}
	if (strcmp(command, "dump") == 0) {
		this->DumpDebugInfoToFile();
	}
	if (strcmp(command, "about") == 0) {
		std::string s = localizationHandler.GetTranslation(CRLANG_ID_CUSTOMROR, "CustomROR");
		s += " ";
		s += VER_FILE_VERSION_STR;
		CallWriteText(s.c_str());
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
		this->ChangeControlledPlayer(playerId, true);
	}
#endif

#ifdef _DEBUG
	// TEST temp
	if (strcmp(command, "b") == 0) {
		long int playerId = 1;
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		if (!player || !player->ptrAIStruct) { return; }
		STRUCT_AI_UNIT_LIST_INFO *l = &player->ptrAIStruct->structInfAI.unknown_0F0;
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
		/*AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
		mapInfo->GetTileInfo(1, 2)->terrainData.SetTerrainId(4);
		mapInfo->GetTileInfo(10, 2)->terrainData.SetTerrainId(6);
		mapInfo->GetTileInfo(20, 2)->terrainData.SetTerrainId(10);
		mapInfo->GetTileInfo(20, 2)->terrainData.SetAltitude(2);
		RefreshTerrainAfterManualChange(mapInfo, -1, -1, -1, -1);*/

		//AddResearchesInStrategyForUnit(player->ptrAIStruct, CST_UNITID_SHORT_SWORDSMAN, false, NULL);
		/*STRATEGY::StrategyBuilder *sb = new STRATEGY::StrategyBuilder(this->crInfo, player);
		//sb->GetStrategyGenerationInfo(player);
		sb->CreateStrategyFromScratch();
		delete sb;*/
	}
	if (strcmp(command, "a") == 0) {
		long int playerId = 1;
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		if (!player || !player->ptrAIStruct) { return; }
		
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)FindUnitWithShortcutNumberForPlayer(player, 1);
		if (unitBase) {
			unitBase->AOE_destructor(true);
			return;
		}
		AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *type50 = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)unitBase;
		if (!unitBase || !unitBase->IsCheckSumValidForAUnitClass()) { return; }
		if (unitBase->unitType != GLOBAL_UNIT_TYPES::GUT_BUILDING) { return; }
		AOE_STRUCTURES::STRUCT_UNIT_BUILDING *bld = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING*)unitBase;
		if (!bld->ptrHumanTrainQueueInformation) { return; }
		bld->unknown_1C4 = 2;
		bld->ptrHumanTrainQueueInformation[1].unitCount = 2;
		bld->ptrHumanTrainQueueInformation[1].DATID = 83;
		//AddUsefulMilitaryTechsToStrategy(player);
		//AddResearchesInStrategyForUnit(player->ptrAIStruct, CST_UNITID_SHORT_SWORDSMAN, true, NULL);
		//AddResearchesInStrategyForUnit(player->ptrAIStruct, CST_UNITID_ARMORED_ELEPHANT, true, NULL);

		std::vector<short int> v;
		v.push_back(64);
		v.push_back(77);
		//GetValidOrderedResearchesListWithDependencies(player, v);

		AOE_STRUCTURES::STRUCT_AI *ai = player->ptrAIStruct;
		AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = &ai->structTacAI;
		assert(tacAI->IsCheckSumValid());
		//float posX, posY;
		//bool ok = GetGamePositionUnderMouse(&posX, &posY);
		//AddInGameCommandButton(5, INGAME_UI_COMMAND_ID::CST_IUC_WORK, 83, false, "eee");
		//SetFarmCurrentTotalFood((AOE_STRUCTURES::STRUCT_UNIT_BUILDING*) (player->custom_selectedUnits[0]), 1);
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
			CallWriteText(buffer);

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
	_asm {
		MOV EAX, AOE_OFFSETS::ADDR_VAR_GAME_SETTINGS_STRUCT
		MOV EAX, DS:[EAX]
		MOV ESI, DS:[EAX+0xC24] // struct 9C 67 54 00 (game screen) //settings->ptrGameUIStruct;
		// This code is directly inspired from 0x480D4F (press on F11)
		PUSH 0
		MOV ECX, DWORD PTR SS:[ESI+0x514]
		PUSH 2
		MOV EAX, 0x004FAF80
		CALL EAX
		MOV ECX, DWORD PTR SS:[ESI+0x540]
		PUSH 1
		MOV EDX,DWORD PTR DS:[ECX]
		CALL DWORD PTR DS:[EDX+0x14]
		MOV ECX, ESI
		MOV EAX, 0x004839A0
		CALL EAX
		MOV ESI, DWORD PTR SS:[ESI+0x4B8]
		PUSH 1
		MOV ECX, ESI
		MOV EDX, DWORD PTR SS:[ESI]
		CALL DWORD PTR DS:[EDX+0x20]
	}
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
	this->DisableAIFlagsForHuman();

	// Do not allow using customROR on multiplayer games (however, always restore AI flags or human-controlled players will have an active AI !)
	if (IsMultiplayer()) { return; }

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();

	// Fix IsScenario flag to correct value (not set when a game is loaded)
	if (settings->isSavedGame) {
		settings->isScenario = 0; // Default: consider it is not a scenario
		// Guess if it is a scenario from scenario information
		if (settings->ptrGlobalStruct && settings->ptrGlobalStruct->IsCheckSumValid()) {
			AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = settings->ptrGlobalStruct->scenarioInformation;
			if (scInfo && scInfo->IsCheckSumValid() && (scInfo->scenarioFileName[0] != 0)) {
				// scenarioFileName is reliable: it is always correctly set: when starting a new RM/DM/scenario, loading a saved game, etc
				settings->isScenario = 1;
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

	// Triggers
	this->ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_GAME_START);

	// Special trigger: disable units for house, villager, fishing ship, trade ship (exceptions that don't work because of techs that do several things)
	// For farm, see notify event method (managed on market construction notification).
	this->ManageTriggerDisableUnitsForExceptions();

	long int **p = (long int **)AOE_OFFSETS::ADDR_VAR_ACTIVE_UI_STRUCT;
	if ((*p) && (**p != 0x0054679C)) {
		// This is NOT game screen, do not try to interact with it !
		return;
	}

	// REQUIRES game UI to be active
	if (!CUSTOMROR::crInfo.configInfo.hideWelcomeMessage && !settings->isMultiplayer) {
		std::string msg = localizationHandler.GetTranslation(CRLANG_ID_WELCOME1, "Welcome. CustomROR");
		msg += " "; 
		msg += VER_FILE_VERSION_STR;
		msg += " ";
		msg += localizationHandler.GetTranslation(CRLANG_ID_WELCOME2, "plugin is active.");
		CallWriteText(msg.c_str());
	}

	// Show automatically "F11" information at game startup
	if (!settings->isMultiplayer) {
		this->ShowF11_zone();
	}

	// Force shared exploration => always ON (if config says so and not in MP)
	// Does not impact scenarios
	if (!settings->isCampaign && !settings->isScenario) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
		if (CUSTOMROR::crInfo.configInfo.allyExplorationIsAlwaysShared &&
			!settings->isMultiplayer && global && global->IsCheckSumValid()) {
			for (int i = 1; i < global->playerTotalCount; i++) {
				SetPlayerSharedExploration_safe(i);
				//ApplyResearchForPlayer(player, CST_RSID_WRITING);
				// Could disable the research ?
			}
		}
	}

#ifdef _DEBUG
	WriteDebugLogForDeserializedData(0, NULL, 0); // Force flush if there is remaining output in buffer
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
	if (settings->isCampaign || settings->isSavedGame || settings->isScenario) {
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
	if (settings->isCampaign || settings->isSavedGame || settings->isScenario) {
		return false;
	}
#pragma message("Customization on random game start: MP not supported")
	if (settings->isMultiplayer) {
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
	for (long int playerId = 1; playerId <= settings->playerCount; playerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		for (int rt = 0; rt < 4; rt++) {
			player->SetResourceValue((AOE_CONST_FUNC::RESOURCE_TYPES) rt, (float)initialResources[rt]);
		}
	}

	// SN Numbers
	for (long int playerId = 1; playerId <= settings->playerCount; playerId++) {
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
		for (long int playerId = 1; playerId <= settings->playerCount; playerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
			if (player && player->IsCheckSumValid()) {
				if (settings->teamNumbers[playerId - 1] == 1) { // Note: teamNumber 1 means "no team"
					// No team: set default diplomacy (game default is neutral, can be set to enemy to avoid all players being against human)
					assert(player->ptrDiplomacyStances != NULL);
					for (int otherPlayerId = 1; otherPlayerId <= settings->playerCount; otherPlayerId++) {
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
			for (long int playerId = 1; playerId <= settings->playerCount; playerId++) {
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


// This is called while scenarioInfo structure is read from a file
// Scenario texts (including players strategies) havealready been read, but not other information, be careful.
void CustomRORCommand::InitScenarioInfoTextData(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scenarioInfo) {
	this->ReloadTriggersFromGameData(scenarioInfo);

	for (int playerId = 0; playerId < 9; playerId++) {
		if (CUSTOMROR::crInfo.triggerSet != NULL) {
			CUSTOMROR::crInfo.FillUnitDefToDisableFromString(playerId, CUSTOMROR::crInfo.triggerSet->disable_unitDefIDTextList[playerId].c_str());
			CUSTOMROR::crInfo.FillResearchesToDisableFromString(playerId, CUSTOMROR::crInfo.triggerSet->disable_researchIDTextList[playerId].c_str());
		}
	}
}


// Initialize internal game-specific variables (called on each game start/load)
void CustomRORCommand::InitMyGameInfo() {
	CUSTOMROR::crInfo.ResetVariables();
	// Prevent 0% speed at game startup (occurs because of rounding in registry saved value)
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (global && global->IsCheckSumValid() && (global->gameSpeed == 0)) {
		global->gameSpeed = 1;
	}
}


// Use this API to get "do we improve AI" information. This takes care of customROR configuration + possible custom rules.
// So we can add custom rules for debugging
bool CustomRORCommand::IsImproveAIEnabled(int playerId) {
#ifdef _DEBUG
	return (CUSTOMROR::crInfo.configInfo.improveAILevel > 0) && _DEBUG_improveThisPlayer(playerId);
#else
	return (CUSTOMROR::crInfo.configInfo.improveAILevel > 0);
#endif
}


// Returns true if RPG mode is active in current game
bool CustomRORCommand::IsRpgModeEnabled() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	if (!settings->isSinglePlayer) { return false; } // Disable in MP
	// Note: to work well on saved games, this requires customROR's fix on game's IsScenario information.
	bool isScenario = (settings->isCampaign || settings->isScenario);
	if (isScenario) {
		return CUSTOMROR::crInfo.configInfo.enableRPGModeInScenario;
	} else {
		return CUSTOMROR::crInfo.configInfo.enableRPGModeInRandomGames;
	}
}


// Disable AI flags for human players, based on game initial settings (to be used at game startup)
void CustomRORCommand::DisableAIFlagsForHuman() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }

	bool isSinglePlayer = !IsMultiplayer();

	AOE_STRUCTURES::STRUCT_MP_COMMUNICATION *unknownStruct = *(AOE_STRUCTURES::STRUCT_MP_COMMUNICATION **)AOE_OFFSETS::ADDR_MP_COMM_STRUCT;
	assert(unknownStruct != NULL);
	int aValidAIPlayer = -1;
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStructPtrTable()[loopPlayerId];
		bool thisPlayerIsHuman = (isSinglePlayer && (loopPlayerId == global->humanPlayerId)) || // SP: use global struct's human playerId: it is always reliable even in loaded games
			(!isSinglePlayer && (unknownStruct->playerTypes[loopPlayerId] == 2)); // MP: use initial setting from "common MP struct". This is not reliable for saved games but OK in MP games.
		if (thisPlayerIsHuman) {
			player->isComputerControlled = 0;
			player->SetCustomAIFlag(0);
		}
		if ((!thisPlayerIsHuman) && (aValidAIPlayer == -1)) {
			// Need to save one arbitrary AI player ID to set "currently managed AI player" or everything will be stuck
			aValidAIPlayer = loopPlayerId;
		}
	}
	global->currentlyManagedAIPlayer = aValidAIPlayer; // Required to avoid stuck AI
}


// Restore AI flags based on human-controlled playerID (to be used in SP games only)
void CustomRORCommand::RestoreAllAIFlags() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	
	int aValidAIPlayer = -1;
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStructPtrTable()[loopPlayerId];
		if (loopPlayerId == global->humanPlayerId) {
			player->isComputerControlled = 0;
			player->SetCustomAIFlag(0);
		} else {
			if (player->isComputerControlled == 0) {
				// Player was NOT AI-controlled. We need to update its strategy with (potentially) human-created units
				this->CheckAIWhenEnablingAIControl(player);
			}
			player->isComputerControlled = 1;
			player->SetCustomAIFlag(1);
			// Need to save an arbitrary AI player ID to set "currently managed AI player" or everything will be stuck
			if (aValidAIPlayer == -1) {
				aValidAIPlayer = loopPlayerId;
			}
		}
		
	}
	global->currentlyManagedAIPlayer = aValidAIPlayer; // Required to avoid stuck AI
}


// This enables AI flags for all players
void CustomRORCommand::SetAllAIFlags() {
	if (IsMultiplayer()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStructPtrTable()[loopPlayerId];
		if (player->isComputerControlled == 0) {
			// Player was NOT AI-controlled. We need to update its strategy with (potentially) human-created units
			this->CheckAIWhenEnablingAIControl(player);
		}
		player->isComputerControlled = 1;
		player->SetCustomAIFlag(1);
	}
}


// Change human control to another player and set AI flags accordingly (if updateAIFlags is true)
void CustomRORCommand::ChangeControlledPlayer(int playerId, bool updateAIFlags) {
	if ((playerId < 0) || (playerId > 8)) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
	// Do not switch to defeated (or invalid) player
	if (!player || !player->IsCheckSumValid() || (player->aliveStatus == 2)) { return; }
	_asm {
		MOV ESI, DWORD PTR DS:[0x580E38] // Game settings
		MOV ECX, ESI
		MOV EAX, 0x00419B70
		CALL EAX // Get currently managed player
		MOV ECX, EAX
		MOV EAX, 0x45DCB0
		CALL EAX // Clear selected units
		CMP DWORD PTR SS:[ECX+0x1B4], 6 // mouseActionType
		JNZ step2
		CMP DWORD PTR SS:[ECX+0x1B8], 1
		JNZ step2
		MOV EAX, DWORD PTR SS:[ESI]
		PUSH 0
		PUSH 0
		MOV ECX, ESI
		CALL DWORD PTR DS:[EAX+0x10]
	step2:
		MOV EDX, DWORD PTR SS:[ESI]
		MOV EAX, playerId
		PUSH EAX
		MOV ECX, ESI
		CALL DWORD PTR DS:[EDX+0x14] // Change controlled player
	}
	if (updateAIFlags) {
		this->RestoreAllAIFlags();
	}
}


// Call this when changing a player from "AI control" disabled to enabled
// This will run various actions to fix strategy, etc (example: do not build buildings human already built).
// The method will do nothing if player is NULL or if its AI structure is NULL.
void CustomRORCommand::CheckAIWhenEnablingAIControl(int playerId) {
	CheckAIWhenEnablingAIControl(GetPlayerStruct(playerId));
}

// Call this when changing a player from "AI control" disabled to enabled
// This will run various actions to fix strategy, etc (example: do not build buildings human already built).
// The method will do nothing if player is NULL or if its AI structure is NULL.
void CustomRORCommand::CheckAIWhenEnablingAIControl(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	UpdateStrategyWithUnreferencedExistingUnits(player);
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
	if (gameSettings->isDeathMatch) {
		if (civ) {
			strcpy_s(aiFileBuffer, 255, civ->deathmatch_AI_file.c_str());
			return true;
		}
	}

	// RM or scenario (for players that don't have a strategy).
	if (civ) {
		switch (gameSettings->mapTypeChoice) {
		case 0:
		case 1:
			strcpy_s(aiFileBuffer, 255, civ->RM_AI_file_much_water.c_str());
			break;
		case 4:
		case 7:
			strcpy_s(aiFileBuffer, 255, civ->RM_AI_file_no_water.c_str());
			break;
		default:
			strcpy_s(aiFileBuffer, 255, civ->RM_AI_file_some_water.c_str());
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
	this->ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_DISCOVER_GAIA_UNIT, evtInfo);

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
	if (FindElementInStrategy(player, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding, CST_UNITID_HOUSE) < 0) {
		// Our current criterion is "no house = not initialized". Can we do better ?
		return false;
	}
	return true;
}



// Overloads the "tactical AI update" event that occurs regularly for each AI player.
// For the algorithm to work well, requires also "FixUnitIdForInProgressBuilding", "FixResetStratElemForUnitId"
void CustomRORCommand::ManageTacAIUpdate(AOE_STRUCTURES::STRUCT_AI *ai) {
	assert(ai && ai->IsCheckSumValid());
	if (!ai || !ai->IsCheckSumValid()) { return; }
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
		return;
	}
	if (player->aliveStatus != 0) {
		return; // Do not update players that are not playing anymore.
	}

	short int numPlayer = player->playerId;

	// Recompute SNScalingFrequency from configuration (only the first call will occur too early, using ROR's default value (4 if not overloaded)
	// Warning: SNScalingFrequency unit is normally MINUTES. CustomROR switched it to seconds.
	// Do not update if the current value is acceptable (it might be a saved game)
	if (tacAI->SNNumber[0x63] < CUSTOMROR::crInfo.configInfo.MINVALUE_tacticalAIUpdateDelay) {
		tacAI->SNNumber[0x63] = CUSTOMROR::crInfo.configInfo.tacticalAIUpdateDelay; // Update SNScalingFrequency with customROR config value
	}

	int ironAgeResearched = IsTechResearched(player, CST_RSID_IRON_AGE);

	// Fix AI only if config allows improving AI (and NOT in MP games). Keep this info in a local variable.
	bool applyAIFix = (!gameSettings->isMultiplayer && (this->IsImproveAIEnabled(numPlayer)));

	// Only for the FIRST tactical update (last one's time is 0): one-shot initializations
	if (tacAI->lastTacticalUpdateTime <= 0) {
		if (applyAIFix) {
			if (gameSettings->difficultyLevel <= GAME_DIFFICULTY_LEVEL::GDL_HARD) {
				// Search for techs that can be added to strategy and would improve my military units
				AddUsefulMilitaryTechsToStrategy(player);
			}
			AdaptStrategyToMaxPopulation(player);
		}
	}

	if (globalStruct->currentGameTime < 10000) { // currentGameTime is in milliseconds
		// This is the begin-of-game call. Do nothing. Next call will really use the appropriate delay (in seconds).
		return;
	}

	if (applyAIFix) {
		// Here: a standard call for update tactical AI that waited the normal delay.
		this->AnalyzeStrategy(&ai->structBuildAI);
	}
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
		UpdateStrategyWithUnreferencedExistingUnits(buildAI, newElem->unitDAT_ID, newElem->elementType);

		// Granary / SP that are inserted at very beginning of strategy: move it after first house.
		if (((newElem->unitDAT_ID == CST_UNITID_GRANARY) || (newElem->unitDAT_ID == CST_UNITID_STORAGE_PIT)) && (insertionPos < 2)) {
			// Warning: this may move strategy element !
			MoveStrategyElement_after_ifWrongOrder(buildAI, CST_UNITID_HOUSE, TAIUnitClass::AIUCBuilding, newElem->unitDAT_ID, TAIUnitClass::AIUCNone);
		}
		curElem = curElem->next; // We are sure curElem did not move, use this (newElem may have been moved forward in strategy)
	}
}


// Analyze strategy and fixes what's necessary. Called every <crInfo.configInfo.tacticalAIUpdateDelay> seconds.
void CustomRORCommand::AnalyzeStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI) {
	// Exit if AI improvement is not enabled.
	if (!this->IsImproveAIEnabled(buildAI->commonAIObject.playerId)) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	bool difficultyIsEasy = (settings->difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASY); // True for easy / easiest
	if (difficultyIsEasy) { return; } // Do not improve strategy in easy levels.

	assert(buildAI != NULL);
	assert(buildAI->IsCheckSumValid());

	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeFirstStratElem = &buildAI->fakeFirstStrategyElement;
	if (fakeFirstStratElem == NULL) { return; } // Just a security
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeFirstStratElem->next;
	assert(buildAI->mainAI != NULL);
	AOE_STRUCTURES::STRUCT_PLAYER *player = buildAI->mainAI->ptrStructPlayer;
	assert(player != NULL);

	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *elemMainTownCenter = NULL; // Main town center
	int townCentersCount = 0; // Total number of town centers.
	bool foundCounter_one = false; // set to true if we passed the "counter=1" element. Allow us to identify "panic units" from normal strategy units
	int currentPopulation = 0; // Does not count limited-retrains units

	int toolAgeResearched = IsTechResearched(player, CST_RSID_TOOL_AGE);
	int bronzeAgeResearched = IsTechResearched(player, CST_RSID_BRONZE_AGE);
	int ironAgeResearched = IsTechResearched(player, CST_RSID_IRON_AGE);
	bool canGoToBronzeAge = (bronzeAgeResearched != 0); // Useful to detect strategies that do NOT have bronze (scenarios ?)
	bool canGoToIronAge = (ironAgeResearched != 0); // Useful to detect strategies that do NOT have iron (scenarios ?)
	bool foundWheel = false;
	bool foundArchitecture = false;
	bool foundWoodWorking = false, foundArtisanShip = false, foundCraftManShip = false;
	bool foundToolWorking = false;
	bool foundDomestication = false;
	//bool foundWatchTower = false, foundSentryTower = false, foundGuardTower = false, foundBallistaTower = false;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *resWatchTower = NULL, *resSentryTower = NULL, *resGuardTower = NULL, *resBallistaTower = NULL;
	bool foundBallistics = false;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *wonder = NULL;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *lastNonTowerElement = fakeFirstStratElem;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *goodDevelopmentPointElement = NULL; // Represents the location in strategy where we can assume player is strong enough to insert optional researches. Be careful, AI can skip a certain number of items (10 according to SN numbers)
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem_bronzeAge = NULL;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *customRorMaxPopBegin = GetCustomRorMaxPopulationBeginStratElem(buildAI);
	float *resources = (float *)player->ptrResourceValues;

	// Do only 1 loop on strategy and collect all necessary information.
	while (currentStratElem && currentStratElem != fakeFirstStratElem) {
		assert(currentStratElem != NULL);
		// Save some useful elements/information as we're passing on it
		if (currentStratElem->unitDAT_ID == CST_UNITID_FORUM) {
			townCentersCount++;
			if (!elemMainTownCenter) {
				elemMainTownCenter = currentStratElem;
			}
		}
		if (currentStratElem->elemId == 1) { foundCounter_one = true; }
		if ((currentStratElem->unitDAT_ID != CST_UNITID_WATCH_TOWER) || (currentStratElem->elementType != AIUCBuilding)) {
			lastNonTowerElement = currentStratElem;
		}
		if ((currentStratElem->elementType == AIUCLivingUnit) && (currentStratElem->retrains == -1)) {
			currentPopulation++;
			// minPopulationBeforeBuildOptionalItems+1 : take next one and insert BEFORE (inserting BEFORE an given element preserves insert order consistency. After reverses all).
			if (!goodDevelopmentPointElement && ironAgeResearched && (currentPopulation >= CUSTOMROR::crInfo.configInfo.minPopulationBeforeBuildOptionalItems + 1)) {
				// We store the location where player theoritically has a population=[minPopulationBeforeBuildOptionalItems] (we also ask for iron age).
				// BUT AI can skip items so this strategy element is triggered BEFORE the [minPopulationBeforeBuildOptionalItems] units are really trained (alive).
				// SO: Use a HIGHER value for the tested population amount than the value we really would like to use.
				// For example: add ~5 or ~10 to the population number we want AI to train before triggering optionals.
				goodDevelopmentPointElement = currentStratElem;
			}
		}

		// On iron age, disable limited-retrains weak units (except panic units, if any). Make sure NOT to disable villagers.
		if (ironAgeResearched && foundCounter_one && (currentStratElem->elementType == AIUCLivingUnit) && (currentStratElem->retrains > -1) &&
			(currentStratElem->inProgressCount == 0)) {
			long int DATID = currentStratElem->unitDAT_ID;
			if ((DATID == CST_UNITID_SLINGER) ||
				(DATID == CST_UNITID_BOWMAN) ||
				(DATID == CST_UNITID_CLUBMAN) ||
				(DATID == CST_UNITID_SCOUT)
				) {
				currentStratElem->totalCount = 2;
				currentStratElem->retrains = 1;
				currentStratElem->aliveCount = 0; // Not really necessary...
			}
		}

		// Unblock "stuck buildings". We intentionally set unitInstanceId to -1 but keep "inprogress" when an unfinished building is destroyed.
		// The goal is to wait (forw THIS update) before trying to build it again.
		// Warning: The 2 other "fixes" about destroyed buildings are absolutely required or AI will build, build and rebuild the same thing forever.
		if (CUSTOMROR::crInfo.hasFixForBuildingStratElemUnitId && (currentStratElem->inProgressCount == 1) && (currentStratElem->elementType == AIUCBuilding) && (currentStratElem->unitInstanceId == -1)) {
			currentStratElem->inProgressCount = 0;
		}

		// Collect info on existing elements...
		// True if current strategy element is a research (including "critical")
		bool isTechOrCritical = (currentStratElem->elementType == AIUCTech) || (currentStratElem->elementType == AIUCCritical);

		if ((currentStratElem->unitDAT_ID == CST_RSID_WHEEL) && isTechOrCritical) {
			foundWheel = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_ARCHITECTURE) && (isTechOrCritical)) {
			foundArchitecture = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_WOOD_WORKING) && (isTechOrCritical)) {
			foundWoodWorking = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_ARTISANSHIP) && (isTechOrCritical)) {
			foundArtisanShip = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_CRAFTMANSHIP) && (isTechOrCritical)) {
			foundCraftManShip = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_TOOL_WORKING) && (isTechOrCritical)) {
			foundToolWorking = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_DOMESTICATION) && (isTechOrCritical)) {
			foundDomestication = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_WATCH_TOWER) && (isTechOrCritical)) {
			resWatchTower = currentStratElem;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_SENTRY_TOWER) && (isTechOrCritical)) {
			resSentryTower = currentStratElem;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_GUARD_TOWER) && (isTechOrCritical)) {
			resGuardTower = currentStratElem;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_BALLISTA_TOWER) && (isTechOrCritical)) {
			resBallistaTower = currentStratElem;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_BALLISTICS) && (isTechOrCritical)) {
			foundBallistics = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_BRONZE_AGE) && (isTechOrCritical)) {
			stratElem_bronzeAge = currentStratElem;
			canGoToBronzeAge = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_RSID_IRON_AGE) && (isTechOrCritical)) {
			canGoToIronAge = true;
		}
		if ((currentStratElem->unitDAT_ID == CST_UNITID_WONDER) && (
			(currentStratElem->elementType == AIUCBuilding))
			) {
			wonder = currentStratElem;
		}

		currentStratElem = currentStratElem->next;
	}

	//AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *endElement; // the element before which to insert, for elements we want to add at the end.
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *optionalsLocation; // the element before which to insert, for optionals elements.
	if (goodDevelopmentPointElement) {
		optionalsLocation = goodDevelopmentPointElement;
	} else {
		if (customRorMaxPopBegin) { optionalsLocation = customRorMaxPopBegin; }
		else {
			if (wonder) { optionalsLocation = wonder; } else {
				optionalsLocation = lastNonTowerElement; // fakeFirstStratElem->previous if it could not be set during loop, so it's never NULL.
			}
		}
	}

	// Improve farms management (can be dynamically added: do not do it in initialization ! They will not exist yet)
	// TO DO

	// Add useful (available) researches when they are missing, under some conditions.
	// Note: testing if the relevant age is researched is useful for performance (stored in a boolean variable, avoids some GetResearchStatus calls)
	int numberOfAddedResearches = 0;
	bool isVeryRich = (resources[CST_RES_ORDER_FOOD] > 6000) &&
		(resources[CST_RES_ORDER_WOOD] > 6000) &&
		(resources[CST_RES_ORDER_GOLD] > 3000) &&
		(resources[CST_RES_ORDER_STONE] > 1000); // Generally corresponds to deathmatch situation.
	bool isStrong = (resources[CST_RES_ORDER_CURRENT_POPULATION] > 40) && ironAgeResearched &&
		(resources[CST_RES_ORDER_FOOD] > 800) &&
		(resources[CST_RES_ORDER_WOOD] > 700) &&
		(resources[CST_RES_ORDER_GOLD] > 500); // Generally corresponds to a random map player that is well developed, with very high population and a minimum resources amount.

	// TO DO: get count of "customROR-added" researches that are NOT done, to limit the number of pending additions ?

	// Wheel & architecture: very important researches !
	// We always add wheel if available in tech tree (and if strategy goes to bronze, at least).
	if (!foundWheel && canGoToBronzeAge && IsResearchRelevantForStrategy(player, CST_RSID_WHEEL)) {
		//AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_WHEEL, player, "customROR-Wheel");
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *tmpElemForWheel = stratElem_bronzeAge;
		// get element AFTER bronze to insert before it
		// Indeed, to add researches, we must use "AddUnitInStrategy_before" method, no "AddUnitInStrategy".
		if (tmpElemForWheel) { tmpElemForWheel = tmpElemForWheel->next; }
		if (!tmpElemForWheel) { // if not found, use our good old "optionals" location.
			tmpElemForWheel = optionalsLocation;
		}
		AddUnitInStrategy_before(buildAI, tmpElemForWheel, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_WHEEL, player, "customROR-Wheel");
		numberOfAddedResearches++;
		std::string msg = "Added wheel in strategy for player #";
		msg += to_string(player->playerId);
		traceMessageHandler.WriteMessageNoNotification(msg);
		// Make sure we added this after market
		//this->MoveStrategyElement_after_ifWrongOrder(buildAI, market, optionalsLocation->previous);
	}
	if (!foundArchitecture && /*canGoToBronzeAge &&*/ bronzeAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_ARCHITECTURE)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GOVERNMENT_CENTER, AIUCTech, CST_RSID_ARCHITECTURE, player, "customROR-Architecture");
		numberOfAddedResearches++;
		//this->MoveStrategyElement_after_ifWrongOrder(buildAI, gov_siege, optionalsLocation->previous);
	}
	// Towers: note: only one of them can be "available to be researched" at once, so this can only add one of them during this tactical update.
	if (!resWatchTower && toolAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_WATCH_TOWER)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GRANARY, AIUCTech, CST_RSID_WATCH_TOWER, player, "customROR-WatchTower");
		numberOfAddedResearches++;
	}
	if (!resSentryTower && bronzeAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_SENTRY_TOWER)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GRANARY, AIUCTech, CST_RSID_SENTRY_TOWER, player, "customROR-SentryTower");
		numberOfAddedResearches++;
		// Make sure new item is after watch tower tech
		MoveStrategyElement_after_ifWrongOrder(buildAI, resWatchTower, optionalsLocation->previous);
	}
	// if (CUSTOMROR::crInfo.configInfo.improveAILevel <= ...) or IsImproveAIEnabled(player->playerId)
	// then add only at a certain level of AI improvement ? (if implemented some day)
	if (!resGuardTower && ironAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_GUARD_TOWER)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GRANARY, AIUCTech, CST_RSID_GUARD_TOWER, player, "customROR-GuardTower");
		numberOfAddedResearches++;
		// Make sure new item is after sentry tower tech
		MoveStrategyElement_after_ifWrongOrder(buildAI, resSentryTower, optionalsLocation->previous);
	}
	// Ballista tower: only add in deathmatch situation
	if (!resBallistaTower && ironAgeResearched && isVeryRich && IsResearchRelevantForStrategy(player, CST_RSID_BALLISTA_TOWER) &&
		(resources[CST_RES_ORDER_STONE] > 2500)) {
		bool canAdd = true;
		// Make sure requirement is ok - add it if needed.
		if (!foundBallistics) {
			AOE_CONST_FUNC::RESEARCH_STATUSES rs = GetResearchStatus(player, CST_RSID_BALLISTICS);
			if ((rs == RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) ||
				(rs == RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE)) {
				AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GOVERNMENT_CENTER, AIUCTech, CST_RSID_BALLISTICS, player, "customROR-Ballistics");
				numberOfAddedResearches++;
			}
			if (rs == RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) { canAdd = false; }
		}
		if (canAdd) {
			AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GRANARY, AIUCTech, CST_RSID_BALLISTA_TOWER, player, "customROR-BallistaTower");
			numberOfAddedResearches++;
			// Make sure new item it after guard tower tech
			MoveStrategyElement_after_ifWrongOrder(buildAI, resGuardTower, optionalsLocation->previous);
		}
	}

	// The +1 range bonuses: note: only one of them can be "available to be researched" at once, so this can only add one of them during this tactical update.
	// Of course, next time this method is called, there's a chance next one can be added too.
	if (!foundWoodWorking && toolAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_WOOD_WORKING)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_WOOD_WORKING, player, "customROR-WoodWorking");
		numberOfAddedResearches++;
	}
	if (!foundArtisanShip && bronzeAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_ARTISANSHIP)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_ARTISANSHIP, player, "customROR-Artisanship");
		numberOfAddedResearches++;
	}
	if (!foundCraftManShip && ironAgeResearched && (isVeryRich || isStrong) && IsResearchRelevantForStrategy(player, CST_RSID_CRAFTMANSHIP)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_CRAFTMANSHIP, player, "customROR-Craftmanship");
		numberOfAddedResearches++;
	}
	// Some low-level cheap researches that are almost always useful: 1st farm improvement, 1st +2 attack bonus.
	// Limit to 2 added researches max (total) if we are using the condition "isStrong".
	if (!foundToolWorking && toolAgeResearched && IsResearchRelevantForStrategy(player, CST_RSID_TOOL_WORKING) &&
		(((numberOfAddedResearches < 2) && isStrong) || isVeryRich)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_STORAGE_PIT, AIUCTech, CST_RSID_TOOL_WORKING, player, "customROR-ToolWorking");
		numberOfAddedResearches++;
	}
	if (!foundDomestication && toolAgeResearched && IsResearchRelevantForStrategy(player, CST_RSID_DOMESTICATION) &&
		(((numberOfAddedResearches < 2) && isStrong) || isVeryRich)) {
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_MARKET, AIUCTech, CST_RSID_DOMESTICATION, player, "customROR-Domestication");
		numberOfAddedResearches++;
	}
	// Add a secondary town center when there is only one. It's a good security.
	if ((townCentersCount == 1) && (settings->difficultyLevel <= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_HARD)) { // Hard/hardest only
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, -1, AIUCBuilding, CST_UNITID_FORUM, player, "customROR-BackupTC");
		townCentersCount++;
	}
}


// Returns true if a construction should NOT be triggered.
// Default result is false (standard ROR behavior), this returns true only for specific (custom) cases.
// Warning: it could be wise for this method to be kept simple... and fast. It's called quite often.
bool CustomRORCommand::ShouldNotTriggerConstruction(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem) {
	if (!tacAI || !tacAI->IsCheckSumValid()) { return false; } // error: use default
	if (!stratElem || !stratElem->IsCheckSumValid()) { return false; } // error: use default

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; } // error: use default

	// AI improvements are not enabled => return default value (like in normal ROR code)
	if (!this->IsImproveAIEnabled(tacAI->commonAIObject.playerId)) {
		return false; // error: use default
	}

	// Easy difficulty levels / MP games: default behavior too
	if (settings->isMultiplayer || (settings->difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASY)) { return false; } // use default

	assert(stratElem->elementType == TAIUnitClass::AIUCBuilding);
	long int villagerTotalCount = tacAI->allVillagers.usedElements;
	if (villagerTotalCount <= 0) {
		return true; // No villager: don't build, of course ! (yes, ROR tries to start construction without villagers ;)
	}

	AOE_STRUCTURES::STRUCT_AI *ai = tacAI->ptrMainAI;
	if (!ai || !ai->IsCheckSumValid()) { return false; } // error: use default
	AOE_STRUCTURES::STRUCT_PLAYER *player = ai->ptrStructPlayer;
	if (!player || !player->IsCheckSumValid()) { return false; } // error: use default

	// Situation 1 : I'm going to build a wonder
	if (stratElem->unitDAT_ID == CST_UNITID_WONDER) {
		// *****
		// Make sure all researches from my strategy are already done before starting a wonder
		if (!IsStrategyCompleteForWonder(ai)) {
			return true;
		}

		// *****
		// Make sure I have extra resources before starting a wonder

		// Collect desired extra amounts
		float myVirtualResources[RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT];
		// Start with a resource "debt" = extra amount I want to keep on hand
		myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_FOOD] = (float)-tacAI->SNNumber[SN_NUMBERS::SNMinimumFood];
		myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_WOOD] = (float)-tacAI->SNNumber[SN_NUMBERS::SNMinimumWood];
		myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_STONE] = (float)-tacAI->SNNumber[SN_NUMBERS::SNMinimumStone];
		myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_GOLD] = (float)-tacAI->SNNumber[SN_NUMBERS::SNMinimumGold];
		// Apply some hardcoded minimum requirements (for hard levels - excludes "medium")
		// Do not use hardcoded values for campaign / scenario : as we're using SN numbers, if the scenario if correctly designed, it will still work normally.
		if ((settings->difficultyLevel < AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_MEDIUM) && (!settings->isCampaign) && (!settings->isScenario)) {
			// I know that wonders don't require food, be let's remain generic... 
			// Moreover, that can prevent from starting a wonder when food is very low, which is a healthy restriction.
			if (myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_FOOD] > -300) {
				myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_FOOD] = -300;
			}
			if (myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_WOOD] > -300) {
				myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_WOOD] = -300;
			}
			if (myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_STONE] > -200) {
				myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_STONE] = -200;
			}
			if (myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_GOLD] > -250) {
				myVirtualResources[RESOURCE_TYPES::CST_RES_ORDER_GOLD] = -250;
			}
		}

		long int unitDefCount = player->structDefUnitArraySize;
		if (!player->ptrStructDefUnitTable) { unitDefCount = 0; }
		if ((stratElem->unitDAT_ID < 0) || (stratElem->unitDAT_ID >= unitDefCount)) {
			traceMessageHandler.WriteMessage("Error: Could not find unit definition for wonder");
			return true; // Unit definition does not exist !
		}
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *) player->ptrStructDefUnitTable[stratElem->unitDAT_ID];
		assert(unitDef && unitDef->DerivesFromTrainable());
		if (!unitDef || !unitDef->DerivesFromTrainable()) { return true; } // Unit definition does not exist !
		AOE_STRUCTURES::STRUCT_COST *costs = unitDef->costs;
		for (int i = 0; i < 3; i++) {
			if (costs[i].costPaid && (costs[i].costType >= 0) &&
				(costs[i].costType < RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT)) {
				// (valid) cost used: substract amount from my resources. We only car about food/wood/stone/gold here.
				myVirtualResources[costs[i].costType] -= (float)costs[i].costAmount;
			}
		}
		// Add player's actual owned resources and check the total
		for (int resourceType = 0; resourceType < RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT; resourceType++) {
			myVirtualResources[resourceType] += player->GetResourceValue((RESOURCE_TYPES)resourceType);
			if (myVirtualResources[resourceType] <= 0) { return true; } // A resource is missing. Cancel construction !
		}

		// *****
		// Another player already has a wonder
		// TO DO. To define what are the rules we want to apply.

		// [Wonder] We found no reason to cancel construction, so... let's proceed !
		return false;
	}

	// Situation 2 : I already have a lot of pending constructions
	if (stratElem->unitDAT_ID != CST_UNITID_FORUM) { // Allow building a TC even if I have many pending constructions*
		// First get the number of pending constructions
		long int unfinishedBuildings = 1; // We count the potential new building in the total.
		for (int i = 0; i < ai->structInfAI.buildingUnits.arraySize; i++) {
			long int bldId = ai->structInfAI.buildingUnits.unitIdArray[i];
			AOE_STRUCTURES::STRUCT_UNIT_BUILDING *unit = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)GetUnitStruct(bldId);
			if (unit && unit->IsCheckSumValid()) { // check IS building type
				if (unit->unitStatus < 2) {
					unfinishedBuildings++;
				}
			}
		}
		long int allowedExtraConstructionCount = 0;
		// Is total number of construction (including new potential one) too large ?
		if (villagerTotalCount + allowedExtraConstructionCount < unfinishedBuildings) {
#ifdef _DEBUG
			std::string msg = "Cancel construction launch: too many pending constructions / p#";
			msg += to_string(player->playerId);
			traceMessageHandler.WriteMessageNoNotification(msg); // Temporary message for debug
#endif
			return true; // There are too many constructions, more than my villagers count. Do NOT add a new construction !
		}
	}

	// [General] We found no reason to cancel construction, so... let's proceed !
	return false;
}


// Calls appropriate "manage panic mode" treatments.
// Returns true if we want to force usage of original ROR's (bugged) code
// In most cases, this returns FALSE and ROR's code is NOT used.
bool CustomRORCommand::RunManagePanicMode_isUsageOfRORCodeWanted(AOE_STRUCTURES::STRUCT_AI *mainAI, long int enemyPlayerId, long int timeSinceLastPanicMode_s, long int currentGameTime_ms) {
	if (!mainAI || !mainAI->IsCheckSumValid()) { return false; }
	long int actorPlayerId = mainAI->structMainDecisionAI.playerId;
	if (this->IsImproveAIEnabled(actorPlayerId)) {
		// When AI improvements are on, used our treatments, not ROR code.
		this->ManagePanicMode(mainAI, enemyPlayerId, timeSinceLastPanicMode_s, currentGameTime_ms);
		return false;
	}

	// Here: AI improvements are OFF (not recommended). Force usage of old code
	// But first do the check on "last execution" (this test has been overriden, and is not done anymore in ROR code).
	if (timeSinceLastPanicMode_s <= 180) { // 0xB4
		return false; // Next execution time has not been reached. Do NOT execute ROR treatments.
	}
	// Delay is reached, ROR code can be called.
	return true;
}


// Manage strategy updates for panic mode.
void CustomRORCommand::ManagePanicMode(AOE_STRUCTURES::STRUCT_AI *mainAI, long int enemyPlayerId, long int timeSinceLastPanicMode_s, long int currentGameTime_ms) {
	// CONFIG
	const long int panicModeDelay = CUSTOMROR::crInfo.configInfo.panicModeDelay;
	long int maxPanicModeUnitsInStrategy = CUSTOMROR::crInfo.configInfo.maxPanicUnitsCountToAddInStrategy;
	const long int maxPanicModeSeekEnemyDistance = 20; // 0x14. Hardcoded in original code
#define PANIC_MODE_ARRAYS_MAX_SIZE 20 // technical limit to the number of panic mode units
	if (maxPanicModeUnitsInStrategy > PANIC_MODE_ARRAYS_MAX_SIZE) { maxPanicModeUnitsInStrategy = PANIC_MODE_ARRAYS_MAX_SIZE; }

	// Collect context information/variables
	assert(mainAI != NULL);
	assert(mainAI->IsCheckSumValid());
	if (!mainAI || (!mainAI->IsCheckSumValid())) { return; }

	// Other variables
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = &mainAI->structBuildAI;
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = &mainAI->structInfAI;
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = &mainAI->structTacAI;
	assert(buildAI->IsCheckSumValid());
	assert(infAI->IsCheckSumValid());
	assert(tacAI->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *player = mainAI->ptrStructPlayer;
	assert(player != NULL);
	assert(player->IsCheckSumValid());
	char myCivId = player->civilizationId;

	long int myMilitaryUnitsCount = tacAI->militaryUnits.usedElements; // does not count villagers, nor towers
	//long unsigned int structPlayer = (long unsigned int) player;

	// Same control as original: abort if delay is not complete
	if (timeSinceLastPanicMode_s < panicModeDelay) {
		return;
	}

	assert((enemyPlayerId >= 0) && (enemyPlayerId < 9));
	AOE_STRUCTURES::STRUCT_PLAYER *enemyPlayer = GetPlayerStruct((short int)enemyPlayerId);
	assert(enemyPlayer != NULL);
	assert(enemyPlayer->IsCheckSumValid());
	if (!enemyPlayer) { return; }
	char enemyCivId = enemyPlayer->civilizationId;


	AOE_STRUCTURES::STRUCT_UNIT_BASE *forumUnitStruct;
	forumUnitStruct = AOE_MainAI_findUnit(mainAI, CST_UNITID_FORUM);
	if (forumUnitStruct == NULL) { return; } // TO DO: make special treatments if no town center (maybe we still have other buildings... Especially temple!)
	assert(forumUnitStruct->IsCheckSumValidForAUnitClass());
	// Add "fake" enemys "priestVictims" so that we create units to defend what's left and possibly convert a villager ! Use fake position ?
	long int forumPosX = (long int)forumUnitStruct->positionX;
	long int forumPosY = (long int)forumUnitStruct->positionY;

	// My resources
	float remainingResources[MAX_RESOURCE_TYPE_ID];
	remainingResources[CST_RES_ORDER_FOOD] = player->GetResourceValue(CST_RES_ORDER_FOOD); // *(float *)(ptrResources + 4 * CST_RES_ORDER_FOOD);
	remainingResources[CST_RES_ORDER_WOOD] = player->GetResourceValue(CST_RES_ORDER_WOOD); //*(float *)(ptrResources + 4 * CST_RES_ORDER_WOOD);
	remainingResources[CST_RES_ORDER_STONE] = player->GetResourceValue(CST_RES_ORDER_STONE); //*(float *)(ptrResources + 4 * CST_RES_ORDER_STONE);
	remainingResources[CST_RES_ORDER_GOLD] = player->GetResourceValue(CST_RES_ORDER_GOLD); //*(float *)(ptrResources + 4 * CST_RES_ORDER_GOLD);
	remainingResources[CST_RES_ORDER_POPULATION_HEADROOM] = player->GetResourceValue(CST_RES_ORDER_POPULATION_HEADROOM); //*(float *)(ptrResources + 0x10); // free population left in houses
	// Keep some necessary basic resources left to create villagers/forum if needed
	remainingResources[CST_RES_ORDER_FOOD] -= 150;
	remainingResources[CST_RES_ORDER_WOOD] -= 200;
	if (remainingResources[CST_RES_ORDER_POPULATION_HEADROOM] <= 0) { return; } // No room for new units
	// We don't use pop_headroom anymore at this point. We could use it to limit the number of added units.
	if ((remainingResources[CST_RES_ORDER_FOOD] < 40) && (remainingResources[CST_RES_ORDER_WOOD] < 20) &&
		(remainingResources[CST_RES_ORDER_GOLD] < 15)) {
		return;
	} // Out of resources: exit now. Optional check.

	float tempCost[MAX_RESOURCE_TYPE_ID + 1];
	short int lastCostDAT_ID = -1; // DAT ID to which last computed cost corresponds to.
	for (int i = 0; i < 4; i++) { tempCost[i] = 0; }

	long int totalEnemyCount = 0; // Total count of enemy units in my town
	long int enemyPriestCount = 0;
	long int enemyCamelVictimsCount = 0; // Units I can kill with camels: chariots, cavalry, horse archers
	long int enemySlingerVictimsCount = 0; // All archers but elephant (they are slow, they can't make my units run after them)
	long int enemySiegeWeaponsCount = 0;
	long int enemyPriestVictimsCount = 0; // Easily convertible units: slow units like elephants or possibly hoplites
	long int enemyChariotsCount = 0;
	long int enemyTowersCount = 0;
	long int myBarracksCount = 0;
	long int myRangeCount = 0;
	long int myStableCount = 0;
	long int myTempleCount = 0;
	long int mySiegeWorkshopCount = 0;
	long int myAcademyCount = 0;
	long int myDockCount = 0;
	bool hasBoats = false;
	long int square_maxPanicModeSeekEnemyDistance = maxPanicModeSeekEnemyDistance * maxPanicModeSeekEnemyDistance;
	long int unitListElementCount = infAI->unitElemListSize;
	long int unitListBase = (long int)infAI->unitElemList;
	assert((unitListElementCount == 0) || (infAI->unitElemList != NULL));
	// Collect info about ATTACKER's units in my town. Note: the unit list we use may not be up to date depending on "my" exploration and what happened to involved units
	// In standard game, infAI->unitElemList contains many obsolete information (never cleaned up !). This is improved in customROR.
	for (int i = 0; i < unitListElementCount; i++) {
		char *unitElementNumPlayer = (char *)(unitListBase + i * 0x24 + 0x0B);

		// We could count "my" buildings from this list but it is incomplete ! Original game searches there but it's bad. We will search directly in player.building_list list.
		if (*unitElementNumPlayer == enemyPlayerId) {
			// Count enemy units in my town
			long int diffX = (forumPosX - infAI->unitElemList[i].posX);
			long int diffY = (forumPosY - infAI->unitElemList[i].posY);
			if ((diffX * diffX) + (diffY * diffY) <= square_maxPanicModeSeekEnemyDistance) {
				totalEnemyCount++;
				if (infAI->unitElemList[i].unitDATID == CST_UNITID_PRIEST) { enemyPriestCount++; }
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_CAVALRY) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HEAVY_CAVALRY) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CATAPHRACT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HORSE_ARCHER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HEAVY_HORSE_ARCHER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CHARIOT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CHARIOT_ARCHER)) {
					enemyCamelVictimsCount++;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_SCOUT_SHIP) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_WAR_GALLEY) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_TRIREME) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CATAPULT_TRIREME) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_JUGGERNAUGHT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_FIRE_GALLEY)) {
					hasBoats = true;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_BOWMAN) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_IMPROVED_BOWMAN) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_COMPOSITE_BOWMAN) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CHARIOT_ARCHER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HORSE_ARCHER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HEAVY_HORSE_ARCHER) // ||
					// Remove elephant archer ? His slowness makes him quite different for defence strategy (he won't make my slow units run after him)
					//(infAI->unitElemList[i].unitDATID == CST_UNITID_ELEPHANT_ARCHER)
					) {
					enemySlingerVictimsCount++;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_BALLISTA) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HELEPOLIS) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_STONE_THROWER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CATAPULT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HEAVY_CATAPULT)) {
					enemySiegeWeaponsCount++;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_ELEPHANT_ARCHER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_WAR_ELEPHANT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_ARMORED_ELEPHANT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_HOPLITE) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_PHALANX) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CENTURION)) {
					enemyPriestVictimsCount++;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_HEAVY_CHARIOT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CHARIOT) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_CHARIOT_ARCHER)) {
					enemyChariotsCount++;
				}
				if ((infAI->unitElemList[i].unitDATID == CST_UNITID_WATCH_TOWER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_SENTRY_TOWER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_GUARD_TOWER) ||
					(infAI->unitElemList[i].unitDATID == CST_UNITID_BALLISTA_TOWER)) {
					enemyTowersCount++;
				}
			}
		}
	}
	// Get my buildings list: quite efficient because we have a dedicated list (only buildings) at player level (does not rely on AI structures)
	assert(player->ptrBuildingsListHeader != NULL);
	long int buildingList_count = 0;
	AOE_STRUCTURES::STRUCT_UNIT_BASE **buildingsList = NULL;
	if (player->ptrBuildingsListHeader) {
		buildingList_count = player->ptrBuildingsListHeader->buildingsArrayElemCount;
		buildingsList = player->ptrBuildingsListHeader->ptrBuildingsArray;
	}
	assert((buildingList_count == 0) || (buildingsList != NULL));
	for (int i = 0; i < buildingList_count; i++) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *loopUnit = NULL;
		assert(player->ptrBuildingsListHeader != NULL);
		loopUnit = buildingsList[i];
		if (loopUnit != NULL) {
			// We are running so much asserts because player->ptrBuildingsListHeader sometimes contain invalid data !
			// It happens because of buildings conversion bug
			assert(loopUnit->IsCheckSumValidForAUnitClass());
			assert(GetUnitStruct(player->ptrBuildingsListHeader->ptrBuildingsUnitIDs[i]) == loopUnit);
			assert(GetUnitStruct(loopUnit->unitInstanceId) == loopUnit);
			assert(loopUnit->unitType == GUT_BUILDING);
			assert(loopUnit->unitDefinition != NULL);
			if ((loopUnit->IsCheckSumValidForAUnitClass()) && (GetUnitStruct(player->ptrBuildingsListHeader->ptrBuildingsUnitIDs[i]) == loopUnit) &&
				(loopUnit->unitType == GUT_BUILDING) && (loopUnit->unitDefinition != NULL)) {
				if ((loopUnit->unitStatus == 2) && (loopUnit->unitDefinition != NULL)) {
					short int DAT_ID = loopUnit->unitDefinition->DAT_ID1;
					if (DAT_ID == CST_UNITID_ACADEMY) { myAcademyCount++; }
					if (DAT_ID == CST_UNITID_BARRACKS) { myBarracksCount++; }
					if (DAT_ID == CST_UNITID_DOCK) { myDockCount++; }
					if (DAT_ID == CST_UNITID_RANGE) { myRangeCount++; }
					if (DAT_ID == CST_UNITID_STABLE) { myStableCount++; }
					if (DAT_ID == CST_UNITID_SIEGE_WORKSHOP) { mySiegeWorkshopCount++; }
					if (DAT_ID == CST_UNITID_TEMPLE) { myTempleCount++; }
				}
			} else {
				// For debug - or just in case...
				// THIS CODE is NO LONGER needed since I fixed the building conversion bug.
				AOE_STRUCTURES::STRUCT_UNIT_BASE *invalidUnit = loopUnit;
				player->ptrBuildingsListHeader->ptrBuildingsArray[i] = NULL; // Remove invalid entry from list, it could cause a crash when some code accesses it.
				char buffer[300];
				sprintf_s(buffer, "ERROR: invalid unit list entry for player #%ld. ptr=%08lX. unitId=%ld GUT=%d, index=%d/%ld.\n",
					player->playerId, (long int)invalidUnit, invalidUnit->unitInstanceId, (int)invalidUnit->unitType, i, buildingList_count);
				//AddTraceToFile(buffer);
				traceMessageHandler.WriteMessage(buffer);
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *invalidUnitDef = invalidUnit->unitDefinition;
				long checksum = ((long)invalidUnitDef < 0x10000) ? -1 : invalidUnitDef->checksum; // if invalidUnitDef ptr is low it is probably invalid, don't access it
				int tmp_DAT_ID = -1;
				if (checksum == 0x00549930) { // checksum is correct (building unitDef)
					tmp_DAT_ID = invalidUnitDef->DAT_ID1;
				}
				sprintf_s(buffer, "unitDef=%08lX checksum=%08lX, DAT_ID=%d\n", (long)invalidUnitDef, checksum, tmp_DAT_ID);
				//AddTraceToFile(buffer); // Do it separately in case we have a crash because of unitDef (even if we try to be careful, we can't detect invalid pointers with 100% confidence)
				traceMessageHandler.WriteMessage(buffer);

				//sprintf_s(buffer, "An error occurred. CustomROR could prevent game crash. See error log file on c:\\ (or C:\\Users\\...\\AppData\\Local\\VirtualStore) and report to developer.");
				traceMessageHandler.WriteMessage("An error occurred. CustomROR could prevent game crash. Please report to developer.");
				//this->OpenCustomDialogMessage(buffer, 450, 250);
			}
		}
	}

	if (!myAcademyCount && !myBarracksCount && !myRangeCount && !mySiegeWorkshopCount && !myStableCount && !myTempleCount) {
		return; // No building, no need to continue
	}
	// For large enemy armies, ignore units when their number is very low (example: do not impact choices because of 1 single cavalry).
	if (totalEnemyCount > 6) {
		if (enemyCamelVictimsCount * 100 / totalEnemyCount < 16) { enemyCamelVictimsCount = 0; }
		if (enemySiegeWeaponsCount * 100 / totalEnemyCount < 16) { enemySiegeWeaponsCount = 0; }
		if (enemyChariotsCount * 100 / totalEnemyCount < 20) { enemyChariotsCount = 0; }
		if (enemyPriestVictimsCount * 100 / totalEnemyCount < 16) { enemyPriestVictimsCount = 0; }
		if (enemySlingerVictimsCount * 100 / totalEnemyCount < 16) { enemySlingerVictimsCount = 0; }
		// Do NOT update enemyPriestCount. Always take care of priest, it is too important, even alone
		// Same for towers
	}

	// Basic units (where DATID1=DATID2)
	int canTrainSlinger = IsUnitAvailableForPlayer(CST_UNITID_SLINGER, player);
	int canTrainClubman = IsUnitAvailableForPlayer(CST_UNITID_CLUBMAN, player);
	// + axe
	int canTrainSwordsman = IsUnitAvailableForPlayer(CST_UNITID_SHORT_SWORDSMAN, player); //+others
	// broad, long sword
	int canTrainScout = IsUnitAvailableForPlayer(CST_UNITID_SCOUT, player);
	int canTrainCavalry = IsUnitAvailableForPlayer(CST_UNITID_CAVALRY, player);
	int canTrainChariot = IsUnitAvailableForPlayer(CST_UNITID_CHARIOT, player);
	int canTrainCamel = IsUnitAvailableForPlayer(CST_UNITID_CAMEL, player);
	int canTrainWarElephant = IsUnitAvailableForPlayer(CST_UNITID_WAR_ELEPHANT, player);
	int canTrainBowman = IsUnitAvailableForPlayer(CST_UNITID_BOWMAN, player);
	int canTrainImprovedBowman = IsUnitAvailableForPlayer(CST_UNITID_IMPROVED_BOWMAN, player);
	// + composite
	int canTrainChariotArcher = IsUnitAvailableForPlayer(CST_UNITID_CHARIOT_ARCHER, player);
	int canTrainHorseArcher = IsUnitAvailableForPlayer(CST_UNITID_HORSE_ARCHER, player);
	int canTrainElephantArcher = IsUnitAvailableForPlayer(CST_UNITID_ELEPHANT_ARCHER, player);
	int canTrainPriest = IsUnitAvailableForPlayer(CST_UNITID_PRIEST, player);
	int canTrainHoplite = IsUnitAvailableForPlayer(CST_UNITID_HOPLITE, player);
	int canTrainBallista = IsUnitAvailableForPlayer(CST_UNITID_BALLISTA, player);
	int canTrainStoneThrower = IsUnitAvailableForPlayer(CST_UNITID_STONE_THROWER, player);
	// Fire galley, scout ship...

	// Upgraded units (DATID1 <> DATID2) => do not search in unit_def array but search if tech has been researched
	// The canTrainxxx && clause is useful for performance, we check the prerequisite first as we already have this information
	int canTrainLegion = canTrainSwordsman && IsTechResearched(player, CST_RSID_LEGION);
	int canTrainHeavyCavalry = canTrainCavalry && IsTechResearched(player, CST_RSID_HEAVY_CAVALRY);
	int canTrainCataphract = canTrainHeavyCavalry && IsTechResearched(player, CST_RSID_CATAPHRACT);
	int canTrainHeavyChariot = canTrainChariot && IsTechResearched(player, CST_RSID_HEAVY_CHARIOT);
	int canTrainArmoredElephant = canTrainWarElephant && IsTechResearched(player, CST_RSID_ARMORED_ELEPHANT);
	int canTrainPhalanx = canTrainHoplite && IsTechResearched(player, CST_RSID_PHALANX);
	int canTrainCenturion = canTrainPhalanx && IsTechResearched(player, CST_RSID_CENTURION);
	int canTrainHelepolis = canTrainBallista && IsTechResearched(player, CST_RSID_HELEPOLIS);
	int canTrainCatapult = canTrainStoneThrower && IsTechResearched(player, CST_RSID_CATAPULT);
	int canTrainHeavyCatapult = canTrainCatapult && IsTechResearched(player, CST_RSID_HEAVY_CATAPULT);
	int canTrainHeavyHorseArcher = canTrainHorseArcher && IsTechResearched(player, CST_RSID_HEAVY_HORSE_ARCHER);

	// Other info about researches
	int hasAristocracy = IsTechResearched(player, CST_RSID_ARISTOCRACY);

	// Collect info on current strategy status (panic mode elements, if any).
	//long int currentStratElem = buildAI + 0xF0;
	long int currentStratElem = ((long int)buildAI) + 0xF0;
	long int firstStratElem = currentStratElem;
	bool go_on = true;
	long int inProgressPanicUnitCount = 0;
	long int panicStratElemToReuse[PANIC_MODE_ARRAYS_MAX_SIZE];
	long int panicStratElemToReuseCount = 0;
	for (int i = 0; i < PANIC_MODE_ARRAYS_MAX_SIZE; i++) { panicStratElemToReuse[i] = 0; }
	// Loop only on the beginning of strategy. Stop at "counter=1" or when meeting a "setGather..." (panic mode units always are before those).
	while ((currentStratElem != NULL) && (go_on)) {
		currentStratElem = *(long int *)(currentStratElem + 0x6C); // next
		long int elemDATID = *(long int *)(currentStratElem + 0x04);
		long int elemCounter = *(long int *)(currentStratElem + 0x0C);
		long int elemType = *(long int *)(currentStratElem + 0x74);
		long int elemInProgress = *(long int *)(currentStratElem + 0x80);
		long int elemAliveCount = *(long int *)(currentStratElem + 0x84);
		long int elemActorDATID = *(long int *)(currentStratElem + 0x8C);
		long int elemTotalCount = *(long int *)(currentStratElem + 0xA0);
		long int elemRetrains = *(long int *)(currentStratElem + 0xA4);
		// 0x00542A3C identifies the "fake" 1st element of the list. If we get back to it, we looped on all elements (should not happen unless the strategy is empty)
		if ((elemType == 6) || (elemCounter == 1) || (currentStratElem == firstStratElem)) { go_on = false; }

		bool isPanicModeElement = false;
		if ((elemRetrains == 1) && // retrainable element
			((elemType == 2) || ((elemType == 0) && (elemDATID == CST_UNITID_WATCH_TOWER))) // living OR tower
			) {
			isPanicModeElement = true;
		}

		// Test if it is an obsolete panic mode element (retrains=1, already trained... and dead).
		if (isPanicModeElement) {
			//if ((elemInProgress == 0) && (elemAliveCount == 0) && (elemTotalCount > 0)) {
			if (elemInProgress == 0) {
				// Can reuse this one (or update it, if not trained yet).
				if (panicStratElemToReuseCount < 20) { // TODO use a constant
					panicStratElemToReuse[panicStratElemToReuseCount] = currentStratElem;
					panicStratElemToReuseCount++;
				}
			} else {
				inProgressPanicUnitCount++; // Do NOT modify this strategy element. It is being trained.
				// This also means that a building is currently used to train the unit: remove it from our available buildings list
				if (elemActorDATID == CST_UNITID_BARRACKS) { myBarracksCount--; }
				if (elemActorDATID == CST_UNITID_RANGE) { myRangeCount--; }
				if (elemActorDATID == CST_UNITID_STABLE) { myStableCount--; }
				if (elemActorDATID == CST_UNITID_DOCK) { myDockCount--; }
				if (elemActorDATID == CST_UNITID_TEMPLE) { myTempleCount--; }
				if (elemActorDATID == CST_UNITID_SIEGE_WORKSHOP) { mySiegeWorkshopCount--; }
				if (elemActorDATID == CST_UNITID_ACADEMY) { myAcademyCount--; }
			}
		}
	}
	if (inProgressPanicUnitCount >= maxPanicModeUnitsInStrategy) { return; } // panic mode unit count in strategy is already at maximum

	bool strategyUpdated = false; // Set to true if we add something in strategy
	long int totalPanicModeElemCount = inProgressPanicUnitCount;
	long int unitToCreateCount = maxPanicModeUnitsInStrategy; // <= 0 means NO panic mode. Max value is maxPanicModeUnitsInStrategy.
	if (totalEnemyCount - myMilitaryUnitsCount < maxPanicModeUnitsInStrategy) {
		unitToCreateCount = totalEnemyCount - myMilitaryUnitsCount;
	}

	// Main loop to insert panic units one by one
	while (totalPanicModeElemCount < unitToCreateCount) {
		long int reuseIndex = totalPanicModeElemCount - inProgressPanicUnitCount;
		short int unitId_toAdd = -1;
		short int unitId_actor = -1;
		bool found = false;

		// 1 : enemy has priests. Create as much chariots as possible
		if (enemyPriestCount > 0) {
			// 1a (scythe) chariot
			if ((!found) && canTrainChariot && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CHARIOT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// 1b : chariot archer
			if ((!found) && (canTrainChariotArcher) && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_CHARIOT_ARCHER;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
		}

		// TODO insert 2 ? If any kind of tower, siege, armored ele, slinger, infantry... avoid priest.
		// if (!found && (enemyTowersCount > 0))

		// 2 : Super units - but avoid heavy catapults ?
		// Warning: in strategy, use DATAID1 (base unit), not DATID2 (upgraded unit)
		if (!found) {
			if (!found && canTrainCenturion && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainLegion && (myBarracksCount > 0)) {
				unitId_toAdd = CST_UNITID_SHORT_SWORDSMAN;
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// For civs that have both armored elephants and cataphracts (and/or heavy chariots), try elephant before cataphract (if no priest, and not too much range units)
			// Elephant is stronger against infantry
			if (!found && canTrainArmoredElephant && (enemyPriestCount == 0) && (myStableCount > 0) &&
				((enemySlingerVictimsCount + enemySiegeWeaponsCount) * 100 / totalEnemyCount < 30) &&
				((myCivId == CST_CIVID_PERSIAN) || (myCivId == CST_CIVID_PALMYRA) || (myCivId == CST_CIVID_HITTITE) || (myCivId == CST_CIVID_MACEDONIAN))) {
				unitId_toAdd = CST_UNITID_WAR_ELEPHANT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainCataphract && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CAVALRY;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainHeavyChariot && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CHARIOT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainHelepolis && (mySiegeWorkshopCount > 0)) {
				unitId_toAdd = CST_UNITID_BALLISTA;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Avoid training armored elephants when enemy priests are up there. We better train anything else, even a scout
			if (!found && canTrainArmoredElephant && (enemyPriestCount == 0) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_WAR_ELEPHANT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainHeavyHorseArcher && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_HORSE_ARCHER;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// If I have a temple, I may suppose I am a "good-priest" civ, at least I have priests in my normal strategy
			// This is not perfectly correct: Yamato just builds a temple in DM as requirement to upgrade to iron.
			// Also, in RM, priest researches may not be available/researched yet
			// That's why we must NOT place this in top priority in "super units" section
			// Also, avoid this if there is too much chariots in enemy's army
			if (!found && canTrainPriest && (myTempleCount > 0) && (enemyCivId != CST_CIVID_MACEDONIAN)) {
				if (!((enemyChariotsCount > 3) || // More than 3 chariots: always consider it is too much
					((enemyChariotsCount > 0) && (totalEnemyCount < 5)) || // only a few enemy units, including chariot: do NOT train priests
					((enemyChariotsCount * 100) / totalEnemyCount > 20) // More than 20% of enemy units are chariots: do NOT train priests
					)) {
					unitId_toAdd = CST_UNITID_PRIEST;
					unitId_actor = CST_UNITID_TEMPLE;
					found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myTempleCount, &lastCostDAT_ID, remainingResources, tempCost);
				}
			}

			// Heavy Catapults: very destructive in my own town, not on top priority in such a situation !
			// TODO. Put it in lower priority ?
		}

		// 3 : Specific counter-attack units
		// Priests already have been added in "super" unit section with necessary criteria
		// Slingers: only do them if enemy has much bowmen and if I can't do better than clubmen
		// Chariot (against priests) have been managed in 1st priority rules
		if (!found) {
			// 3a : Camel (if we can do cataphracts or armored elephants, it is has already been done to its maximum number in super units section)
			// TODO : improve this. If I can train (heavy) cavalry and enemy has more "other" units than chariots/cavalry, train cavalry instead
			if (!found && canTrainCamel && (enemyCamelVictimsCount > 0) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CAMEL;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Against siege weapons, try to create fast/strong units, avoid archers
			if (!found && canTrainCavalry && (enemySiegeWeaponsCount > 0) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CAVALRY; // cavalry or heavy cav (cataphract: already managed)
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainChariot && (enemySiegeWeaponsCount > 0) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CHARIOT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Catapult (not stone thrower). Only if several enemy siege weapons
			if (!found && canTrainCatapult && (enemySiegeWeaponsCount > 2) && (mySiegeWorkshopCount > 0)) {
				unitId_toAdd = CST_UNITID_CATAPULT;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// If greek or macedonian (they have bonuses): hoplite BEFORE standard infantry
			if (!found && (enemySiegeWeaponsCount > 2) && ((myCivId == CST_CIVID_MACEDONIAN) || (myCivId == CST_CIVID_GREEK)) && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainSwordsman && (enemySiegeWeaponsCount > 0) && (myBarracksCount > 0)) {
				unitId_toAdd = CST_UNITID_SHORT_SWORDSMAN;
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			if (!found && canTrainHoplite && (enemySiegeWeaponsCount > 0) && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Scout. TODO Try more complex rules (only if many siege weapons)
			if (!found && canTrainScout && (enemySiegeWeaponsCount > 1) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_SCOUT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Clubman: only if there are a lot of siege weapon (otherwise, try others like archers first)
			if (!found && canTrainClubman && (enemySiegeWeaponsCount > 5) && (myBarracksCount > 0)) {
				unitId_toAdd = CST_UNITID_CLUBMAN; // or axeman
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
		}

		// 4 : Other units
		if (!found) {
			if (!found && canTrainPhalanx && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Only if greek or macedonian (they have bonuses) + if aristocracy is ON (faster hoplites): try hoplite BEFORE some other units.
			// NOT for roman: try standard infantry first because it has a bonus
			if (!found && ((myCivId == CST_CIVID_GREEK) || (myCivId == CST_CIVID_MACEDONIAN) || (hasAristocracy)) && (myCivId != CST_CIVID_ROMAN) && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Heavy cavalry
			if (!found && (canTrainHeavyCavalry) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CAVALRY; // heavy cav (cataphract: already managed)
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Horse archer
			if (!found && (canTrainHorseArcher) && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_HORSE_ARCHER;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// All swordsmen but legion (already managed). Fast to train. In addition, I have only 1 barracks (or this means I have good infantry)
			if (!found && canTrainSwordsman && (myBarracksCount > 0)) {
				unitId_toAdd = CST_UNITID_SHORT_SWORDSMAN; // or axeman
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// basic chariot
			if (!found && (canTrainChariot) && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_CHARIOT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Basic hoplite.
			if (!found && canTrainHoplite && (myAcademyCount > 0)) {
				unitId_toAdd = CST_UNITID_HOPLITE;
				unitId_actor = CST_UNITID_ACADEMY;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myAcademyCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Elephant archer (if no priest)
			if (!found && (enemyPriestCount == 0) && (canTrainElephantArcher) && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_ELEPHANT_ARCHER;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Catapult against archers
			if (!found && canTrainCatapult && (mySiegeWorkshopCount > 0) && (enemySlingerVictimsCount * 100 / totalEnemyCount > 30)) {
				unitId_toAdd = CST_UNITID_STONE_THROWER;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Ballista (better than basic stone thrower but not as good as catapult)
			if (!found && canTrainBallista && (mySiegeWorkshopCount > 0)) {
				unitId_toAdd = CST_UNITID_BALLISTA;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Stone thrower against archers (after ballista)
			if (!found && canTrainStoneThrower && (mySiegeWorkshopCount > 0) && (enemySlingerVictimsCount * 100 / totalEnemyCount > 30)) {
				unitId_toAdd = CST_UNITID_STONE_THROWER;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Improved/composite bowman
			if (!found && canTrainImprovedBowman && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_IMPROVED_BOWMAN;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Scout
			if (!found && canTrainScout && (myStableCount > 0)) {
				unitId_toAdd = CST_UNITID_SCOUT;
				unitId_actor = CST_UNITID_STABLE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myStableCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Slinger => only if there are much bowmen
			if (!found && canTrainSlinger && (myBarracksCount > 0) && (enemySlingerVictimsCount * 100 / totalEnemyCount > 30)) {
				unitId_toAdd = CST_UNITID_SLINGER; // or axeman
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Basic bowman
			if (!found && canTrainBowman && (myRangeCount > 0)) {
				unitId_toAdd = CST_UNITID_BOWMAN;
				unitId_actor = CST_UNITID_RANGE;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myRangeCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// Clubman (or axe)
			if (!found && canTrainClubman && (myBarracksCount > 0)) {
				unitId_toAdd = CST_UNITID_CLUBMAN; // or axeman
				unitId_actor = CST_UNITID_BARRACKS;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &myBarracksCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
			// stone thrower: last call? (can be useful when out of food)
			if (!found && canTrainStoneThrower && (mySiegeWorkshopCount > 0)) {
				unitId_toAdd = CST_UNITID_STONE_THROWER;
				unitId_actor = CST_UNITID_SIEGE_WORKSHOP;
				found = PrepareUnitToAddIfPossible(player, unitId_toAdd, unitId_actor, &mySiegeWorkshopCount, &lastCostDAT_ID, remainingResources, tempCost);
			}
		}

		// Add the unit in strategy
		//if (found || (unitId_toAdd > -1)) {
		if (found) {
			strategyUpdated = true;
			if (panicStratElemToReuse[reuseIndex] != NULL) {
				// update strategy element
				long int *p = (long int*)panicStratElemToReuse[reuseIndex];
				*(p + 1) = unitId_toAdd;
				*(p + 2) = -1; // unit instance id
				// Note: we could search for name in defUnit instead of using hardcoded one. But this is faster and this name is unused by the game code.
				const char *buf = GetUnitName(unitId_toAdd);
				strcpy_s((char *)(p + 4), 20, buf);
				*(p + 0x1D) = 2; // +0x74 unit type
				*(p + 0x20) = 0; // +0x80 in progress
				*(p + 0x21) = 0; // +0x84 alive
				*(p + 0x22) = 0; // +0x88 attempts
				*(p + 0x23) = unitId_actor; // +0x8C actor
				*(p + 0x28) = 0; // +0xA0 total count
				*(p + 0x29) = 1; // +0xA4 retrains
			} else {
				// Insert new
				AddUnitInStrategy((AOE_STRUCTURES::STRUCT_BUILD_AI *) buildAI, 0, 1, unitId_actor, AIUCLivingUnit, unitId_toAdd, player);
			}

			totalPanicModeElemCount++;
		} else {
			totalPanicModeElemCount = 999; // stop !
		}
	}

	if (strategyUpdated) {
		_asm {
			// Update "last panic mode time" variable... Only if we did something.
			MOV ECX, tacAI
				MOV EAX, currentGameTime_ms
				MOV DWORD PTR DS : [ECX + 0xF94], EAX
		}
	}

	// If NOT panic mode, cancel panic mode units that are not being trained (no need to train them anymore). Too late for "in progress" ones...
	// Limitation: this "clean" can only be executed if I am attacked but out of my town :(
	if ((unitToCreateCount < 0) && (panicStratElemToReuse[0] != 0)) {
		for (int i = 0; i < PANIC_MODE_ARRAYS_MAX_SIZE; i++) {
			long int *p = (long int*)panicStratElemToReuse[i];
			if ((p != NULL) && (*(p + 0x20) == 0)) { // Not in progress
				*(p + 0x28) += 2; // +0xA0 total count. Make sure it is >=1 (original game often uses 2 and not 1, why ? Maybe just a mistake)
			}
		}
	}
}
// End of panic mode management


void CustomRORCommand::PrintDateTime() {
	time_t rawtime;
	char timebuf[50];
	char timebuf2[50];
	time(&rawtime);
	ctime_s(timebuf2, 30, &rawtime);
	sprintf_s(timebuf, "Time=%s", timebuf2, 40);
	CallWriteText(timebuf);
}


void CustomRORCommand::PrintMapSeed() {
	long int *addr = (long int *)AOE_OFFSETS::ADDR_VAR_GAME_SETTINGS_STRUCT;
	unsigned long int *mapSeed = (unsigned long int *) ((*addr) + 0x34);
	char text[50];
	sprintf_s(text, 50, "Map seed=%d.", *mapSeed);
	CallWriteText(text);
}


void CustomRORCommand::DumpDebugInfoToFile() {
#ifdef _DEBUG
	char buf[] = "Error opening debug file :       ";
	FILE *f;
	int res = fopen_s(&f, "F:\\AOEDebug.txt", "w+"); // overwrite if already existing
	if (res) {
		sprintf_s(buf + 28, 5, "%d", res);
		CallWriteText(buf);
		return;
	}

	AOE_STRUCTURES::AOE_STRUCT_EXPORTER writer;
	std::string objToString = "";

	fprintf_s(f, "AOE Debug information\n\n");
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settingsStruct = *ROR_gameSettings;
	fprintf_s(f, "Global struct = 0x%08lX - %d players (including gaia)\n", (long int)globalStruct, globalStruct->playerTotalCount);
	fprintf_s(f, "Game settings = 0x%08lX - Map seed=%d - Deathmatch=%d\n", (long int)settingsStruct, settingsStruct->actualMapSeed, settingsStruct->isDeathMatch);
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		AOE_STRUCTURES::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		//fprintf_s(f, "Player %d struct=0x%08lX - Resources=0x%08lX - AI=0x%08lX - %-16s\n", i, (long int)currentPlayer, currentPlayer->ptrResourceValues, currentPlayer->ptrAIStruct, currentPlayer->playerName_length16max);
		
		objToString = writer.ExportStruct(currentPlayer);
		fprintf_s(f, objToString.c_str());
	}

	// Diplomacy values
	fprintf_s(f, "\n*** Diplomacy values ***\n");
	fprintf_s(f, "\nDislike vs p0  p1  p2  p3  p4  p5  p6  p7  p8  - Like vs p0  p1  p2  p3  p4  p5  p6  p7  p8  \n");
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		AOE_STRUCTURES::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		AOE_STRUCTURES::STRUCT_AI *ai = currentPlayer->ptrAIStruct;
		if (ai == NULL) {
			fprintf_s(f, "[No AI structure for player #%d.]\n", i);
		} else {
			fprintf_s(f, "player %d :", i);
			for (int iTargetPlayer = 0; iTargetPlayer < 9; iTargetPlayer++) {
				fprintf_s(f, " %-3ld", ai->structDiplAI.dislikeTable[iTargetPlayer]);
			}
			fprintf_s(f, " -        ");
			for (int iTargetPlayer = 0; iTargetPlayer < 9; iTargetPlayer++) {
				fprintf_s(f, " %-3ld", ai->structDiplAI.likeTable[iTargetPlayer]);
			}
			fprintf_s(f, "\n");
		}
	}

	// Strategy
	fprintf_s(f, "\n*** Strategies ***\n");
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		AOE_STRUCTURES::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		fprintf_s(f, "\nPlayer %d:\nCounter Class DAT_ID Name                           Actor Unitid InProgress Alive Attempts #created Retrains Ptr\n", i);
		AOE_STRUCTURES::STRUCT_AI *ai = currentPlayer->ptrAIStruct;
		if (ai == NULL) {
			fprintf_s(f, "No AI structure for this player.\n");
		} else {
			AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeFirstStratElem = &ai->structBuildAI.fakeFirstStrategyElement;
			AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeFirstStratElem->next;
			while (currentStratElem && (currentStratElem != fakeFirstStratElem)) {
				fprintf_s(f, "%03ld     %1ld     %-3ld    %-30s %-3ld   %-6ld %-2ld         %-2ld    %-4ld     %-4ld     %-3ld      0x%08lX\n", currentStratElem->elemId, currentStratElem->elementType, currentStratElem->unitDAT_ID,
					currentStratElem->unitName, currentStratElem->actor, currentStratElem->unitInstanceId, currentStratElem->inProgressCount,
					currentStratElem->aliveCount, currentStratElem->buildAttempts, currentStratElem->totalCount, currentStratElem->retrains, (long int)currentStratElem
					);
				currentStratElem = currentStratElem->next;
			}
		}
	}

	fprintf_s(f, "\n*** SN values ***\n");
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		AOE_STRUCTURES::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		fprintf_s(f, "\nPlayer %d:\n", i);
		AOE_STRUCTURES::STRUCT_AI *ai = currentPlayer->ptrAIStruct;
		if (ai == NULL) {
			fprintf_s(f, "No AI structure for this player.\n");
		} else {
			for (int sn = 0; sn < 227; sn++) {
				fprintf_s(f, "SN%-3ld   value=%ld\n", sn, ai->structTacAI.SNNumber[sn]);
			}
		}
	}

	fclose(f);
#endif
}


// Returns how many units were told to move.
// Returns <0 if there is an error
int CustomRORCommand::MoveIdleMilitaryUnitsToMousePosition(AOE_STRUCTURES::STRUCT_PLAYER *player, float maxDistance) {
	if (!player) { return -1; }
	AOE_STRUCTURES::STRUCT_AI *ai = player->ptrAIStruct;
	if (!ai || !ai->allMyUnits.unitIdArray) { return -1; }
	if (ai->structTacAI.militaryUnits.usedElements == 0) { return 0; }

	float posY;
	float posX;
	if (!GetGamePositionUnderMouse(&posX, &posY)) { return 0; }
	if ((posX < 0) || (posY < 0)) { return 0; }
	int result = 0;

	long int unitCount = ai->allMyUnits.usedElements;
	if (ai->allMyUnits.arraySize < unitCount) {
		unitCount = ai->allMyUnits.arraySize;
		traceMessageHandler.WriteMessage("Invalid element count in AI.allMyUnits !");
	}
	long int *myUnitsList = ai->allMyUnits.unitIdArray;
	for (long int index = 0; index < unitCount; index++) {
		long int unitId = myUnitsList[index];
		AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *unit = NULL;
		if (unitId > 0) {
			unit = (AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *)GetUnitStruct(unitId);
		}
		if (unit != NULL) {
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
			assert(unitDef != NULL);
			if ((unit->DerivesFromCommandable()) &&
				(unitDef->unitAIType != TribeAIGroupCivilian) &&
				(unitDef->unitAIType != TribeAIGroupFishingBoat) &&
				(unitDef->unitAIType != TribeAIGroupTradeBoat) &&
				(unitDef->unitAIType != TribeAIGroupTransportBoat) &&
				(unitDef->unitAIType != TribeAIGroupWarBoat) &&
				(unitDef->unitAIType != TribeAIGroupArtefact)
				) {
				if (IsUnitIdle(unit)) {
					// maxDistance <= 0 : argument is ignored (=> always true). Otherwise: check distance condition
					if ((maxDistance <= 0) || (GetDistance(posX, posY, unit->positionX, unit->positionY) <= maxDistance)) {
						CreateCmd_RightClick(unit->unitInstanceId, -1, posX, posY);
						//MoveUnitToTargetOrPosition(unit, NULL, posX, posY); // not MP-compatible at this point
						result++;
					}
				}
			}
		}
	}

	return result;
}


// Searches all idle units in a specified range (see config) and order them to come at screen location
// Requires ManageAI !
// Disabled in MP games.
void CustomRORCommand::CallNearbyIdleMilitaryUnits() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	short int playerId = globalStruct->humanPlayerId;
	if ((playerId < 0) || (playerId > 8)) { return; }

	if (IsMultiplayer()) { return; } // MP protection
	AOE_STRUCTURES::STRUCT_PLAYER **playerTable = globalStruct->GetPlayerStructPtrTable();
	if (!playerTable || !playerTable[playerId]) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = playerTable[playerId];
	this->MoveIdleMilitaryUnitsToMousePosition(player, (float) CUSTOMROR::crInfo.configInfo.distanceToCallNearbyIdleMilitaryUnits);
}


// Select next idle military unit for current player
void CustomRORCommand::SelectNextIdleMilitaryUnit() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	short int playerId = globalStruct->humanPlayerId;
	if ((playerId < 0) || (playerId > 8)) { return; }

	AOE_STRUCTURES::STRUCT_PLAYER **playerTable = globalStruct->GetPlayerStructPtrTable();
	if (!playerTable || !playerTable[playerId]) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = playerTable[playerId];
	if (!player || !player->ptrCreatableUnitsListLink || (player->ptrCreatableUnitsListLink->listElemCount <= 0)) {
		return;
	}
	assert(player->ptrCreatableUnitsListLink->IsCheckSumValid());

	AOE_STRUCTURES::STRUCT_UNIT_BASE *mainSelectedUnit = CUSTOMROR::crInfo.GetMainSelectedUnit(player);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *firstIgnoredUnit = NULL; // in case seelcted unit = last from list, keep in memory first one to loop back
	bool ignoreUntilSelectedUnitIsMet = false;
	if (mainSelectedUnit && mainSelectedUnit->IsCheckSumValidForAUnitClass()) {
		ignoreUntilSelectedUnitIsMet = true;
	}

	AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *currentUnitElem = player->ptrCreatableUnitsListLink->lastListElement;
	while (currentUnitElem) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)currentUnitElem->unit;
		if (unitBase && unitBase->IsCheckSumValidForAUnitClass() && unitBase->unitDefinition && unitBase->unitDefinition->IsCheckSumValidForAUnitClass()) {
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unitBase->unitDefinition;
			char result;
			if ((unitBase->transporterUnit == NULL) && IsNonTowerMilitaryUnit(unitDefBase->unitAIType)) { // Excludes towers
				_asm {
					MOV ECX, unitBase;
					MOV EDX, DS:[ECX];
					CALL DS:[EDX + 0x210]; // unit.IsIdle() (this just test action status : not expensive)
					MOV result, AL
				}
				if (result) {
					if (!ignoreUntilSelectedUnitIsMet) {
						SelectOneUnit(player, unitBase, true);
						return;
					}
					if (firstIgnoredUnit == NULL) {
						firstIgnoredUnit = unitBase;
					}
				}
			}
			if (unitBase == mainSelectedUnit) {
				ignoreUntilSelectedUnitIsMet = false;
			}
		}
		currentUnitElem = currentUnitElem->previousElement;
	}
	if (firstIgnoredUnit) {
		SelectOneUnit(player, firstIgnoredUnit, true);
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
	if (isInGameSpawnUnit && this->IsRpgModeEnabled()) {
		assert(actionStruct->actor->IsCheckSumValidForAUnitClass());
		if (!((AOE_STRUCTURES::STRUCT_UNIT_BUILDING*)actionStruct->actor)->IsTypeValid()) {
			assert(false && "actor not a building");
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
		AutoAssignShortcutToUnit(unit);
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
					canInteractWithTarget = canInteractWithTarget && IsFogVisibleForPlayer(player->playerId, (long)target->positionX, (long)target->positionY);
				}
			}
		}
		if (canInteractWithTarget) {
			if (unitDef->DerivesFromCommandable() && unitDef->ptrUnitCommandHeader) {
				if (GetUnitDefCommandForTarget(unit, target, true) != NULL) {
					TellUnitToInteractWithTarget(unit, target);
					commandCreated = true;
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
			IsTower(targetUnitDef->DAT_ID1) || (targetUnitDef->unitType == GUT_TRAINABLE)
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
	UpdateStrategyWithExistingUnit(buildAI_actor, targetUnit);

	// Update all players' infAI.unitElemList because it would contain an erroneous playerId, and this would never be fixed/updated
	// This leads to incorrect behaviours (unit groups stuck because they try to attack their own units...)
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = actorPlayer->ptrGlobalStruct;
	assert(global && global->IsCheckSumValid());
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *loopPlayer = GetPlayerStruct(loopPlayerId);
		if (loopPlayer && loopPlayer->IsCheckSumValid() && loopPlayer->ptrAIStruct && loopPlayer->ptrAIStruct->IsCheckSumValid() &&
			this->IsImproveAIEnabled(loopPlayerId)) {
			AOE_STRUCTURES::STRUCT_INF_AI *loopInfAI = &loopPlayer->ptrAIStruct->structInfAI;
			assert(loopInfAI->IsCheckSumValid());
			// Fix (or remove) unit from list for each player. We MUST NOT let a bad playerId be stored in unit elem list.
			UpdateOrResetInfAIUnitListElem(loopInfAI, FindInfAIUnitElemInList(loopInfAI, targetUnit->unitInstanceId));
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
	AOE_ChangeUnitOwner(targetUnit, actorPlayer);
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
	if (isInGame && player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid() && (unit->unitInstanceId >= 0) && this->IsImproveAIEnabled(player->playerId)) {
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
		if (isBuilding && (unit->unitStatus == AOE_CONST_INTERNAL::UNIT_STATUS::CST_US_READY)) { // status=2 means it is a conversion (or gaia unit captured)
			// Remove building from player buildings list (bug in original game), this is done in unit destructor but NOT at unit conversion
			// Note that because of this call, the remove operation is done twice (will be done again in destructor). But the 2nd time, it will just do nothing.
			AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader = player->ptrBuildingsListHeader;
			AOE_playerBldHeader_RemoveBldFromArrays(buildingsHeader, (STRUCT_UNIT_BASE*)unit);
		}

		// Fix constructions history array
		if (isInGame && isBuilding && player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid() &&
			// Not fixed in easiest... But as it is a technical fix, we apply it even if "improve AI" is not enabled.
			// Note that it has a strong (positive) impact on AI farming + the fact that granary/SP construction is no longer blocked for no valid reason
			settings->difficultyLevel < AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASIEST) {
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
	if (isInGame && unit && unit->IsCheckSumValidForAUnitClass() && isBuilding && CUSTOMROR::crInfo.configInfo.enableAutoRebuildFarms) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
		// If this is a farm, and if I have "farm rebuild info" for this position (not in "not rebuild" mode), then trigger a rebuild.
		if (unitDef && unitDef->IsCheckSumValidForAUnitClass() && (unitDef->DAT_ID1 == CST_UNITID_FARM) && player->ptrGlobalStruct) {
			FarmRebuildInfo *fInfo = CUSTOMROR::crInfo.myGameObjects.FindFarmRebuildInfo(unit->positionX, unit->positionY);
			if (fInfo && (fInfo->playerId == player->playerId) && !fInfo->forceNotRebuild &&
				(fInfo->villagerUnitId >= 0) && IsUnitAvailableForPlayer(CST_UNITID_FARM, player)) {
				// As long as we use a game command, it is compatible with multiplayer.
				CreateCmd_Build(fInfo->villagerUnitId, CST_UNITID_FARM, fInfo->posX, fInfo->posY);
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
		!settings->isMultiplayer && (unit->unitInstanceId >= 0)) {
		CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
		evtInfo.unitId = unit->unitInstanceId;
		this->ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_UNIT_LOSS, evtInfo);
	}
}


// Returns true if a shortcut has been added/modified
bool CustomRORCommand::AutoAssignShortcutToUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	// Only care about living units or building
	if ((unit->unitType != GUT_BUILDING) && (unit->unitType != GUT_TRAINABLE)) {
		return false;
	}
	// We choose not to modify existing shortcut
	if (unit->shortcutNumber != 0) { return false; }

	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
	assert(unitDef != NULL);
	assert(unitDef->IsCheckSumValidForAUnitClass());

	int shortcutId = CUSTOMROR::crInfo.configInfo.unitShortcutsPriorityReverseOrder ? 9 : 1;
	for (int currentIndex = 1; currentIndex < CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS; currentIndex++) {
		if (CUSTOMROR::crInfo.configInfo.unitShortcutsPriorityReverseOrder) {
			shortcutId = CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS - currentIndex;
		}
		assert(shortcutId < CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS);
		assert(shortcutId >= 0);
		UnitSpawnShortcutInfo *shortcutInfo = &CUSTOMROR::crInfo.configInfo.unitShortcutsInformation[shortcutId];
		if (shortcutInfo->DAT_ID == unitDef->DAT_ID1) {
			// This rule matches our unit DAT ID.
			// If "only 1 unit" is NOT enabled, always assign. Otherwise, check the shortcut number is still unused.
			if (!shortcutInfo->onlyOneUnit || FindUnitWithShortcutNumberForPlayer(unit->ptrStructPlayer, shortcutId) == NULL) {
				unit->shortcutNumber = shortcutId;
				return true;
			}
		}
	}
	return false;
}


// Returns true if a unit should change target to new one (default), false if it should keep attacking current one.
// This choice is very important, unit must NOT change its mind on each call because it is called too often and unit just attacks none of its "attackers"
// Which is a common issue in original game (example: 2-priest trick to safely convert an enemy)
// For not managed cases: Default behaviour is return true (change target) because it allows original code to be executed.
// The goal is to detect the more relevant situations where we can return false (force attacking current unit: do not switch).
// The improved algorithm is only used if ImproveAI config is ON.
bool CustomRORCommand::ShouldChangeTarget(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity, long int targetUnitId) {
	assert(this->distanceToConsiderVeryClose > 0);

	// Error cases: let ROR code handle them.
	if (!activity) { return true; }
	if (targetUnitId < 0) { return true; } // important check because we call GetUnitStruct on this.
	//assert(activity->IsCheckSumValid());

	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit = activity->ptrUnit;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *newTargetUnit = GetUnitStruct(targetUnitId);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *oldTargetUnit = GetUnitStruct(activity->targetUnitId);
	if (!actorUnit || !newTargetUnit /*|| !oldTargetUnit*/) { return true; }
	if (!oldTargetUnit) {
		// activity target unit can be reset by pending treatments. Find if action HAS a valid target. We may want to keep this target.
		AOE_STRUCTURES::STRUCT_ACTION_BASE *action = GetUnitAction(actorUnit); // May return NULL
		if (!action || (action->targetUnitId < 0)) {
			return true;
		}
		oldTargetUnit = GetUnitStruct(action->targetUnitId);
		activity->targetUnitId = action->targetUnitId; // A bit dirty. Fixes the activity.targetUnitId==-1 case.
	}
	if (!oldTargetUnit) { return true; } // We really could not find the target, it seems there is none: change target !

	assert(newTargetUnit->IsCheckSumValidForAUnitClass());
	assert(actorUnit->IsCheckSumValidForAUnitClass());
	if (!newTargetUnit->IsCheckSumValidForAUnitClass() || !actorUnit->IsCheckSumValidForAUnitClass()) {
		return true; // invalid data. Let original code be executed.
	}
	if (oldTargetUnit == newTargetUnit) {
		return false;
	} // same unit: do not change (=no stupid hesitation)

	// Checks for invalid/dead targets
	if ((oldTargetUnit->remainingHitPoints <= 0) || (oldTargetUnit->unitType < GLOBAL_UNIT_TYPES::GUT_TRAINABLE) ||
		(oldTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_TREE) || (oldTargetUnit->unitStatus > 2)) {
		return true; // switch
	}
	if ((newTargetUnit->remainingHitPoints <= 0) || (newTargetUnit->unitType < GLOBAL_UNIT_TYPES::GUT_TRAINABLE) ||
		(newTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_TREE) || (newTargetUnit->unitStatus > 2)) {
		return false; // Keep. Not sure if this case can happen. Maybe when a projectile from a dead unit hits me afterwards ?
	}
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return true; }
	assert((global->gameMapInfo != NULL) && (global->gameMapInfo->IsCheckSumValid()));
	long mapSizeX = global->gameMapInfo->mapArraySizeX;
	long mapSizeY = global->gameMapInfo->mapArraySizeY;
	assert((long)newTargetUnit->positionX <= mapSizeX);
	assert((long)actorUnit->positionX <= mapSizeX);
	assert((long)oldTargetUnit->positionX <= mapSizeX);
	assert((long)newTargetUnit->positionY <= mapSizeY);
	assert((long)actorUnit->positionY <= mapSizeY);
	assert((long)oldTargetUnit->positionY <= mapSizeY);
	assert(newTargetUnit->positionX >= 0);
	assert(newTargetUnit->positionY >= 0);
	assert(actorUnit->positionX >= 0);
	assert(actorUnit->positionY >= 0);
	assert(oldTargetUnit->positionX >= 0);
	assert(oldTargetUnit->positionY >= 0);
	if (((long)newTargetUnit->positionX > mapSizeX) ||
		((long)newTargetUnit->positionY > mapSizeY) ||
		((long)actorUnit->positionX > mapSizeX) ||
		((long)actorUnit->positionY > mapSizeY) ||
		((long)oldTargetUnit->positionX > mapSizeX) ||
		((long)oldTargetUnit->positionX > mapSizeY) ||
		((long)newTargetUnit->positionX < 0) ||
		((long)newTargetUnit->positionY < 0) ||
		((long)actorUnit->positionX < 0) ||
		((long)actorUnit->positionY < 0) ||
		((long)oldTargetUnit->positionX < 0) ||
		((long)oldTargetUnit->positionY < 0)
		) {
		return true;
	}

	// Collect info on the 2 units to attack (and "me")
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = actorUnit->ptrStructPlayer;
	assert(actorPlayer != NULL);
	assert(actorPlayer->IsCheckSumValid());
	float *resources = (float *)actorPlayer->ptrResourceValues;
	assert(resources != NULL);
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *actorUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE*)actorUnit->unitDefinition;
	assert(actorUnitDef != NULL);
	assert(actorUnitDef->IsCheckSumValidForAUnitClass());
	assert(actorUnitDef->DerivesFromAttackable());
	if (!actorUnitDef || !actorUnitDef->DerivesFromAttackable()) { return true; } // error case
	bool actorIsPriest = (actorUnitDef->DAT_ID1 == CST_UNITID_PRIEST);
	bool actorIsArcher = (actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher) ||
		(actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher) ||
		(actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher) ||
		(actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher); // True if actor unit is any type of archer (not counting slingers, towers)
	// Also consider war boats as archer (excluding fire galley and "catapult" ones)
	if ((actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupWarBoat) &&
		(actorUnitDef->DAT_ID1 != CST_UNITID_CATAPULT_TRIREME) &&
		(actorUnitDef->DAT_ID1 != CST_UNITID_JUGGERNAUGHT) &&
		(actorUnitDef->DAT_ID1 != CST_UNITID_FIRE_GALLEY)
		) {
		actorIsArcher = true;
	}
	assert(newTargetUnit->unitDefinition != NULL);
	assert(oldTargetUnit->unitDefinition != NULL);
	if ((newTargetUnit->unitDefinition == NULL) || (oldTargetUnit->unitDefinition == NULL)) {
		return true; // error case
	}

	// Is AI Improvement feature ON ?
	if (!this->IsImproveAIEnabled(actorPlayer->playerId)) {
		return true; // AI improvement is disabled: let ROR original code do its normal behavior.
	}
	// Do not improve in easy levels
	if (GetGameSettingsPtr() && (GetGameSettingsPtr()->difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASY)) { return true; }

	// A target is mine or allied
	if (actorUnit->ptrStructPlayer->ptrDiplomacyStances[oldTargetUnit->ptrStructPlayer->playerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY) {
		return true; // change to new because old target is allied (may happen after a conversion or maybe diplomacy change)
	}
	if (actorUnit->ptrStructPlayer->ptrDiplomacyStances[newTargetUnit->ptrStructPlayer->playerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY) {
		return false; // keep old because new target is allied (can this happen ? Not sure)
	}
	
	bool newTargetIsVillager = (newTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian);// IsVillager(newTargetUnit->unitDefinition->DAT_ID1);
	bool actionTargetIsVillager = (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian); // IsVillager(oldTargetUnit->unitDefinition->DAT_ID1);
	bool newTargetIsPriest = (newTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_PRIEST);
	bool actionTargetIsPriest = (oldTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_PRIEST);
	bool newTargetIsTower = AOE_CONST_FUNC::IsTower(newTargetUnit->unitDefinition->DAT_ID1);
	bool actionTargetIsTower = AOE_CONST_FUNC::IsTower(oldTargetUnit->unitDefinition->DAT_ID1);
	bool newTargetIsWall = (newTargetUnit->unitDefinition->unitAIType == TribeAIGroupWall);
	bool actionTargetIsWall = (oldTargetUnit->unitDefinition->unitAIType == TribeAIGroupWall);
	// Position/distances
	float myPosX = actorUnit->positionX;
	float myPosY = actorUnit->positionY;
	float distanceX = (newTargetUnit->positionX - myPosX);
	float distanceY = (newTargetUnit->positionY - myPosY);
	float squareDistanceNewTarget = (distanceX * distanceX) + (distanceY * distanceY);
	distanceX = (oldTargetUnit->positionX - myPosX);
	distanceY = (oldTargetUnit->positionY - myPosY);
	float squareDistanceOldTarget = (distanceX * distanceX) + (distanceY * distanceY);
	bool isOldTargetVisible = IsFogVisibleForPlayer(actorPlayer->playerId, (long int)oldTargetUnit->positionX, (long int)oldTargetUnit->positionY);
	// The value to test distances, taking into account unit range.
	float squareVeryCloseTestDistance = (actorUnitDef->maxRange + distanceToConsiderVeryClose);
	squareVeryCloseTestDistance = squareVeryCloseTestDistance * squareVeryCloseTestDistance;

	bool almostKilled_actionTarget = (actorUnitDef->displayedAttack > oldTargetUnit->remainingHitPoints); // Very approximative but fast ;)
	bool almostKilled_newTarget = (actorUnitDef->displayedAttack > newTargetUnit->remainingHitPoints); // Very approximative but fast ;)
	bool alreadyVeryClose_oldTarget = (squareDistanceOldTarget <= squareVeryCloseTestDistance); // If true, consider I don't need moving to attack
	bool alreadyVeryClose_newTarget = (squareDistanceNewTarget <= squareVeryCloseTestDistance); // If true, consider I don't need moving to attack

	// VERY FIRST priority decision: fog-hidden enemy: must change target or this would be cheating !!!
	// Exception for building (they don't move !)
	if (!isOldTargetVisible && (oldTargetUnit->unitDefinition->unitType != GLOBAL_UNIT_TYPES::GUT_BUILDING)) {
		return true; // Change target
	}

	// Special: actor is priest ("almost killed" values don't have any importance here). Against tower, consider "melee" distance, not priest's range
	if (actorIsPriest) {
		// Additional information
		bool canConvertTowers = (resources[RESOURCE_TYPES::CST_RES_ORDER_CAN_CONVERT_BUILDING] > 0);
		bool alreadyVeryClose_oldTarget_melee = (squareDistanceOldTarget <= distanceToConsiderVeryClose); // For priests (that have both range and "melee" (vs buildings) conversion)
		bool alreadyVeryClose_newTarget_melee = (squareDistanceOldTarget <= distanceToConsiderVeryClose);
		bool newTargetIsChariot = (newTargetUnit->unitDefinition->unitAIType == TribeAIGroupChariot) ||
			(newTargetUnit->unitDefinition->unitAIType == TribeAIGroupChariotArcher);
		bool actionTargetIsChariot = (oldTargetUnit->unitDefinition->unitAIType == TribeAIGroupChariot) ||
			(oldTargetUnit->unitDefinition->unitAIType == TribeAIGroupChariotArcher);

		// Top priority: priests can't convert TC, wonder, walls or unfinished buildings => next 2 rules
		if ((newTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) && (newTargetUnit->unitStatus < 2)) {
			return false; // Force keep if "new" target is a not-fully built building.
		}
		if (newTargetIsWall || (newTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) || (newTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_WONDER)) {
			return false; // Force keep if "new" target can't be converted.
		}
		if ((oldTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) && (oldTargetUnit->unitStatus < 2)) {
			return true; // Change if current target is a not-fully built building
		}
		if (actionTargetIsWall || (oldTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) || (oldTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_WONDER)) {
			return true;  // Change if current target can't be converted
		}

		// Priest that are very close to towers should try to convert them before the towers kills them
		if (alreadyVeryClose_newTarget_melee && newTargetIsTower && canConvertTowers) {
			return true; // Change to "new" target = tower
		}
		if (alreadyVeryClose_oldTarget_melee && actionTargetIsTower && canConvertTowers) {
			// Especially, note that towers have less resistance than enemy priests. If a tower is near, it is better to convert it.
			return false; // Force keep
		}
		if (actionTargetIsPriest && canConvertTowers) {
			return false; // Force keep - do not hesitate against priest (except for close towers but it has already been tested before)
		}
		if (newTargetIsPriest && canConvertTowers) {
			return true; // do not hesitate against priest (except for close towers but it has already been tested before)
		}
		// Avoid trying to convert chariots...
		if (newTargetIsChariot && !actionTargetIsChariot) {
			return false; // Force keep action target which is not a chariot
		}
		if (!newTargetIsChariot && actionTargetIsChariot) {
			return true; // Switch to "new" target which is not a chariot
		}
		// Manage case: Only one of the 2 targets is in range
		if (!alreadyVeryClose_newTarget && alreadyVeryClose_oldTarget) {
			return false; // Force keep
		}
		if (alreadyVeryClose_newTarget && !alreadyVeryClose_oldTarget) {
			return true; // Switch
		}
		// Old target is a building not at range, other is a living unit: switch
		if (!alreadyVeryClose_oldTarget_melee &&
			(oldTargetUnit->unitDefinition->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) &&
			(newTargetUnit->unitDefinition->unitType != GLOBAL_UNIT_TYPES::GUT_BUILDING)
			) {
			return true;
		}
		// TO DO: add other rules to accept some target changes when useful.

		// Default: priests are weak, they must avoid changing target while converting.
		return false; // Force keep
	}

	// "New" target is priest, close and almost killed: switch !
	if (newTargetIsPriest && almostKilled_newTarget && alreadyVeryClose_newTarget) {
		return true;
	}
	// Action target is priest, close and almost killed: keep !
	if (actionTargetIsPriest && almostKilled_actionTarget && alreadyVeryClose_oldTarget) {
		return false; // Force keep
	}
	// Only target is a priest
	if (newTargetIsPriest && !actionTargetIsPriest) {
		return true; // Change
	}
	// Both are priests: attack one that is in range, priority goes to current one.
	if (newTargetIsPriest && actionTargetIsPriest) {
		if (alreadyVeryClose_oldTarget) { return false; } // Keep
		if (alreadyVeryClose_newTarget) { return true; } // Change
	}
	// Other cases where action target is a priest : keep current target (don't hesitate against priests !)
	if (actionTargetIsPriest) {
		return false; // Force keep
	}

	// Now deal with cases with no priest
	// "Easy" case: "New" target is close and almost killed: switch !
	if (almostKilled_newTarget && alreadyVeryClose_newTarget) {
		return true;
	}
	// "Easy" case: Action target is close and almost killed: keep !
	if (almostKilled_actionTarget && alreadyVeryClose_oldTarget) {
		return false; // Force keep
	}

	// I am an archer: if possible, avoid attacking towers (or even buildings) because I'm not good for that
	if (actorIsArcher) {
		bool actionTargetIsBuilding = (oldTargetUnit->unitDefinition->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING);
		bool targetIsBuilding = (newTargetUnit->unitDefinition->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING);

		// Top-Priority exception: try to kill towers with only 1 HP (already done by "easy case" above?)
		if (newTargetIsTower && (newTargetUnit->remainingHitPoints < 2)) {
			return true; // Change to "1 HP" tower to instant kill it !
		}
		if (actionTargetIsTower && (oldTargetUnit->remainingHitPoints < 2)) {
			return false; // Keep target="1 HP" tower to instant kill it !
		}

		if (targetIsBuilding && !actionTargetIsBuilding) {
			return false; // Force keep
		}
		if (!targetIsBuilding && actionTargetIsBuilding) {
			return true; // current target is a building, should be better to change to non-building target !
		}
	}

	// When attacking some "priority" units (that are in my range), force keep: siege weapons, some archers
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon)) {
		return false; // force keep
	}
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher)) {
		return false; // force keep
	}
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher)) {
		return false; // force keep
	}
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher)) {
		return false; // force keep
	}
	// Slingers: attack them, but NOT if I am an archer
	if (alreadyVeryClose_oldTarget && !actorIsArcher && (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger)) {
		return false; // force keep
	}
	// Enemy tower: keep if I am war elephant
	if (alreadyVeryClose_oldTarget && (actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider) && (IsTower(oldTargetUnit->unitDefinition->DAT_ID1))) {
		return false; // force keep
	}


	// If only 1 of the 2 targets has me as its target, attack this one
	AOE_STRUCTURES::STRUCT_ACTION_BASE *newTargetUnitAction = NULL;
	AOE_STRUCTURES::STRUCT_ACTION_BASE *actionTargetUnitAction = NULL;
	STRUCT_UNIT_COMMANDABLE *newTargetUnitBird = (STRUCT_UNIT_COMMANDABLE *)newTargetUnit;
	STRUCT_UNIT_COMMANDABLE *oldTargetUnitBird = (STRUCT_UNIT_COMMANDABLE *)oldTargetUnit;
	if (newTargetUnitBird->DerivesFromCommandable() && (newTargetUnitBird->ptrActionInformation) && (newTargetUnitBird->ptrActionInformation->ptrActionLink) &&
		(newTargetUnitBird->ptrActionInformation->ptrActionLink->actionStruct)) {
		newTargetUnitAction = (AOE_STRUCTURES::STRUCT_ACTION_BASE *)newTargetUnitBird->ptrActionInformation->ptrActionLink->actionStruct;
	}
	if (oldTargetUnitBird->DerivesFromCommandable() && (oldTargetUnitBird->ptrActionInformation) && (oldTargetUnitBird->ptrActionInformation->ptrActionLink) &&
		(oldTargetUnitBird->ptrActionInformation->ptrActionLink->actionStruct)) {
		actionTargetUnitAction = (AOE_STRUCTURES::STRUCT_ACTION_BASE *)oldTargetUnitBird->ptrActionInformation->ptrActionLink->actionStruct;
	}
	bool newTargetAttacksMe = newTargetUnitAction && (
		(newTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_UNKNOWN_7) ||
		(newTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_ATTACK_9) ||
		(newTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_CONVERT)
		) &&
		(newTargetUnitAction->targetUnit == actorUnit);
	bool actionTargetAttacksMe = actionTargetUnitAction && (
		(actionTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_UNKNOWN_7) ||
		(actionTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_ATTACK_9) ||
		(actionTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_CONVERT)
		) &&
		(actionTargetUnitAction->targetUnit == actorUnit);

	if (newTargetAttacksMe && !actionTargetAttacksMe) {
		return true; // change
	}
	if (!newTargetAttacksMe && actionTargetAttacksMe) {
		return false; // force keep
	}

	// Same enemy unit AI type: just compare HP (they are similar units)
	if (newTargetUnit->unitDefinition->unitAIType == oldTargetUnit->unitDefinition->unitAIType) {
		if (newTargetUnit->remainingHitPoints > oldTargetUnit->remainingHitPoints) {
			return false; // force keep if both units have the same unit type and current one has LESS HP (weaker: keep).
		} // Else: let other tests be run (just in case they find some reason to choose), if needed it will go to this function's ending "return true;"
	}

	// None is attacking me ?
	if (!newTargetAttacksMe && !actionTargetAttacksMe) {
		if (newTargetIsVillager && !actionTargetIsVillager) {
			return false; // Current is NOT a villager, attack it (keep)
		}
		if (!newTargetIsVillager && actionTargetIsVillager) {
			return true; // New is NOT a villager, attack it (change)
		}
	}

	// TO DO
	// Try a circular list (~10 ?) to log all last attacker unitIDs + game time of last attack.
	// If a time is quite old (several seconds), remove from list
	// Each time we come here, update in the list or add if not present.
	// So we can know how many units are attacking the unit. If there are too many, do a "force keep" for one of them
	return true; // Default: change action (let the original code decide)
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
	assert(this->distanceToConsiderVeryClose > 0);
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
	float actorRange = actorUnitDef->maxRange + this->distanceToConsiderVeryClose;
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
			if ((currentTargetAsBld->constructionProgress > 75) && (IsTower(currentTargetDef->DAT_ID1))) {
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

	if (this->IsImproveAIEnabled(tacAI->commonAIObject.playerId)) {
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
			if ((activity->internalId_whenAttacked == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_UNKNOWN_2BC_ATTACKING) ||
				(activity->internalId_whenAttacked == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_GATHERER_REACTION_WHEN_ATTACKED)) {
				attackTower = false;
			}
		}

		//This custom part (AI improvement) depends on improveAILevel parameter (can be soft-disabled)
		if (this->IsImproveAIEnabled(tacAI->commonAIObject.playerId)) {
			// Additional "custom" treatments. Try to avoid complex treatments and try to optimize as this code is called very frequently.
			// Let's use our common function to handle decisions for units against tower in "my" town.
			if (!this->ShouldAttackTower_towerPanic(unit, enemyTower)) {
				attackTower = false;
			}
		}

		if (valid && attackTower) {
			// Tell villager to go and attack the tower
			if (MoveAndAttackTarget(tacAI, unit, (AOE_STRUCTURES::STRUCT_UNIT_BASE*)enemyTower)) {
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
short int CustomRORCommand::DuplicateUnitDefinitionForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int srcDAT_ID, char *name) {
	if (!player) { return -1; }

	short int newDAT_ID = player->structDefUnitArraySize;
	if (newDAT_ID >= 0x7FFF) {
		traceMessageHandler.WriteMessage("Error: tried to add too many unit definitions.");
		return -1;
	}
	short int nbDef = player->structDefUnitArraySize + 1; // new number of unit definitions
	long int newSizeInBytes = nbDef * 4;
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE **oldArray = player->ptrStructDefUnitTable;
	// Run some checks
	assert(oldArray != NULL);
	if (oldArray == NULL) { return -1; }
	assert(player->structDefUnitArraySize > srcDAT_ID);
	assert(0 <= srcDAT_ID);
	if ((srcDAT_ID < 0) || (srcDAT_ID >= player->structDefUnitArraySize)) {
		std::string msg = std::string("ERROR: tried to copy a unit definition that does not exist, ID=") + std::to_string(srcDAT_ID);
		traceMessageHandler.WriteMessage(msg);
		return -1;
	}

	// Create new array
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE **newArray = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE **)AOEAlloc(newSizeInBytes);
	if (newArray == NULL) { return -1; } // nothing allocated: return an error
		
	// Copy old array into new (for all existing unitDefs => copy pointers)
	memcpy(newArray, oldArray, newSizeInBytes);
	AOEFree(oldArray); // old array is freed, we replace it by new (larger) one
	player->ptrStructDefUnitTable = newArray;
	player->structDefUnitArraySize = nbDef;
	player->ptrStructDefUnitTable[newDAT_ID] = NULL; // until we set a valid pointer.

	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *srcDef_base = player->ptrStructDefUnitTable[srcDAT_ID];
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *srcDef_type50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)srcDef_base;
	assert(srcDef_base != NULL);
	if (srcDef_base == NULL) { return -1; }
	if ((srcDef_base->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) || (srcDef_base->unitType == GLOBAL_UNIT_TYPES::GUT_TRAINABLE)) {
		traceMessageHandler.WriteMessage("WARNING: adding unitdef for living/buildings is disabled due to conversion bug.");
		// To solve the limitation, a solution could be to add it for all players !
		return -1;
	}

	long int objectSize = 0;
	bool hasAttacksAndArmors = srcDef_base->DerivesFromAttackable();
	bool hasCommandsHeader = srcDef_base->DerivesFromCommandable();
	switch (srcDef_base->unitType) {
	case GUT_EYE_CANDY:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BASE);
		break;
	case GUT_FLAGS:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_FLAG);
		break;
	case GUT_DOPPLEGANGER:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_DOPPLEGANGER);
		break;
	case GUT_MOVABLE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE);
		break;
	case GUT_COMMANDABLE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE);
		break;
	case GUT_ATTACKABLE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE);
		break;
	case GUT_PROJECTILE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE);
		break;
	case GUT_TRAINABLE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE);
		break;
	case GUT_BUILDING:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING);
		break;
	case GUT_TREE:
		objectSize = sizeof(AOE_STRUCTURES::STRUCT_UNITDEF_TREE);
		break;
	default:
		// Not supported
		assert(0 && "Trying to create a non-supported unit type.");
		traceMessageHandler.WriteMessage("ERROR: Tried to create a non-supported unit type.");
		return -1;
	}
	if (objectSize <= 0) { return -1; }

	// Create new defUnit in the free room we just created
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *newUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *) AOEAlloc(objectSize);
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *newUnitDefType50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)newUnitDef;
	if (!newUnitDef) { return -1; } // nothing allocated: return an error

	player->ptrStructDefUnitTable[newDAT_ID] = newUnitDef;

	memcpy(newUnitDef, srcDef_base, objectSize);

	// Very important: update new unitDef's DATIDs or some features will not work
	newUnitDef->DAT_ID1 = newDAT_ID;
	newUnitDef->DAT_ID2 = newDAT_ID;

	// Manage damage graphics array (allocated for each unitDef)
	// It belongs to base class => do this for all unit types, even if it seems this array is only used for buildings
	if (newUnitDef->damageGraphicCount > 0) {
		newUnitDef->damageGraphicsArray = (STRUCT_DAMAGE_GRAPHIC *)AOEAlloc(sizeof(STRUCT_DAMAGE_GRAPHIC) * newUnitDef->damageGraphicCount);
		memcpy(newUnitDef->damageGraphicsArray, srcDef_base->damageGraphicsArray, sizeof(AOE_STRUCTURES::STRUCT_DAMAGE_GRAPHIC) * newUnitDef->damageGraphicCount);
	} else {
		assert(newUnitDef->damageGraphicsArray == NULL);
		newUnitDef->damageGraphicsArray = NULL; // Should already be NULL in this case
		newUnitDef->damageGraphicCount = 0;// Should not be <0
	}

	if (hasAttacksAndArmors && srcDef_type50->DerivesFromAttackable()) {
		// Attacks and armors are allocated for each unitDef. We need to duplicate them (or ROR would free them twice)
		newUnitDefType50->ptrArmorsList = NULL; // Default value, will be overwritten if necessary
		newUnitDefType50->ptrAttacksList = NULL; // Default value, will be overwritten if necessary
		if (newUnitDefType50->armorsCount > 0) {
			newUnitDefType50->ptrArmorsList = (AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK *)AOEAlloc(sizeof(AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK) * newUnitDefType50->armorsCount);
			memcpy(newUnitDefType50->ptrArmorsList, srcDef_type50->ptrArmorsList, sizeof(AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK) * newUnitDefType50->armorsCount);
		}
		if (newUnitDefType50->attacksCount > 0) {
			newUnitDefType50->ptrAttacksList = (AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK *)AOEAlloc(sizeof(AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK) * newUnitDefType50->attacksCount);
			memcpy(newUnitDefType50->ptrAttacksList, srcDef_type50->ptrAttacksList, sizeof(AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK) * newUnitDefType50->attacksCount);
		}
	}

	// Graphics structures are COMMON (not freed at unitDef level): we can keep them, they won't be freed twice

	if (hasCommandsHeader && srcDef_type50->DerivesFromAttackable()) {
		// Commands: we need to duplicate them (or ROR would free them twice)
		assert(srcDef_type50->ptrUnitCommandHeader != NULL);
		if (srcDef_type50->ptrUnitCommandHeader != NULL) {
			AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF_HEADER *newCmdHeader = (AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF_HEADER *) AOEAlloc(sizeof(AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF_HEADER));
			newUnitDefType50->ptrUnitCommandHeader = newCmdHeader;
			newCmdHeader->checksum = srcDef_type50->ptrUnitCommandHeader->checksum;
			newCmdHeader->commandCount = srcDef_type50->ptrUnitCommandHeader->commandCount;
			if (newCmdHeader->commandCount > 0) {
				newCmdHeader->ptrCommandArray = (AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF**) AOEAlloc(newCmdHeader->commandCount * 4);
			} else {
				newCmdHeader->ptrCommandArray = NULL;
			}

			for (long int i = 0; i < newCmdHeader->commandCount; i++) {
				newCmdHeader->ptrCommandArray[i] = (AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF*) AOEAlloc(sizeof(AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF)); // Alloc
				memcpy(newCmdHeader->ptrCommandArray[i], srcDef_type50->ptrUnitCommandHeader->ptrCommandArray[i], sizeof(AOE_STRUCTURES::STRUCT_UNIT_COMMAND_DEF)); // Copy from source
			}
		} else {
			newUnitDefType50->ptrUnitCommandHeader = NULL; // This should NOT happen
		}
	}

	// Note: If we keep ptrLanguageDLLName, source unit name will appear in game. If we set to NULL, it will use ptrUnitName instead.
	if (!name || (*name == 0)) {
		return newDAT_ID;
	}
	newUnitDef->languageDLLID_Name = 0;
	newUnitDef->languageDLLID_Creation = 0;
	char *newName = (char *)AOEAlloc(0x30);
	strcpy_s(newName, 0x30, name);
	newUnitDef->ptrUnitName = newName; // issues with the name ?

	return newDAT_ID;
}



void CustomRORCommand::ComputeDislikeValues() {
	if ((CUSTOMROR::crInfo.configInfo.dislike_allArtefacts <= 0) || (CUSTOMROR::crInfo.configInfo.dislike_humanPlayer <= 0)) { return; }

	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	assert(globalStruct->GetPlayerStructPtrTable() != NULL);
	unsigned long int newDislikeValues[9];
	for (int i = 0; i < 9; i++) { newDislikeValues[i] = CST_DISLIKE_INITIAL_VALUE; }

	// Calculate dislike "penalty" for each player.
	for (int iPlayerId = 1; iPlayerId < globalStruct->playerTotalCount; iPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = globalStruct->GetPlayerStructPtrTable()[iPlayerId];
		if (player && player->ptrAIStruct) {
			float *resources = (float *)player->ptrResourceValues;
			if (resources[CST_RES_ORDER_STANDING_WONDERS]) { newDislikeValues[iPlayerId] += CUSTOMROR::crInfo.configInfo.dislike_allArtefacts; }
			if (resources[CST_RES_ORDER_ALL_RUINS]) { newDislikeValues[iPlayerId] += CUSTOMROR::crInfo.configInfo.dislike_allArtefacts; }
			if (resources[CST_RES_ORDER_ALL_RELICS]) { newDislikeValues[iPlayerId] += CUSTOMROR::crInfo.configInfo.dislike_allArtefacts; }
			if (player->isComputerControlled == 0) { newDislikeValues[iPlayerId] += CUSTOMROR::crInfo.configInfo.dislike_humanPlayer; }
		}
	}

	for (int iPlayerId = 1; iPlayerId < globalStruct->playerTotalCount; iPlayerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = globalStruct->GetPlayerStructPtrTable()[iPlayerId];
		if (player) {
			for (int iTargetPlayerId = 1; iTargetPlayerId < globalStruct->playerTotalCount; iTargetPlayerId++) {
				AOE_STRUCTURES::STRUCT_PLAYER *targetPlayer = globalStruct->GetPlayerStructPtrTable()[iTargetPlayerId];
				if ((iPlayerId != iTargetPlayerId) && (player->diplomacyVSPlayers[iTargetPlayerId] > 2) && (player->ptrAIStruct)) {
					// Set the new calculated dislike value against player #iTargetPlayerId
					player->ptrAIStruct->structDiplAI.dislikeTable[iTargetPlayerId] = newDislikeValues[iTargetPlayerId];
				}
			}
		}
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
				PUSH tile
				MOV ECX, elevInfo
				MOV EAX, 0x46C310
				CALL EAX
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
	CallWriteText(buf);
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
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_GetScreenFromName(scenarioEditorScreenName);
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
	char *mapSeedText = AOE_GetEditText(scEditor->map_edt_seed);
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
		AOE_SetLabelText(scEditor->map_edt_seed_whenReadOnly, bufferSeed);
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
			AOE_STRUCTURES::STRUCT_PLAYER_UNKNOWN_58_AND_6C *unknown_struct = &player->unknown_058;
			_asm {
				MOV EDX, 0x45EB40 // realloc ?
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
	AOE_ShowUIObject(bigLbl, false); // Remove the "generating" label
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
				AOE_listbox_addItem(listBox, listBox->itemCount, terrainDef->terrainName, terrainId);
			}
		}
	}

	// Manage custom map dimensions
	if (CUSTOMROR::crInfo.configInfo.useCustomMapDimensions &&
		scEditor->map_cbb_mapSize && scEditor->map_cbb_mapSize->IsCheckSumValid() &&
		scEditor->map_cbb_mapSize->underlyingListBox && scEditor->map_cbb_mapSize->underlyingListBox->IsCheckSumValid()) {
		AOE_listbox_addItem(scEditor->map_cbb_mapSize->underlyingListBox, 
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
	bool doFix = true;

#ifdef _DEBUG
#pragma message("DEBUG trick path finding/gathering")
	// DEBUG ONLY
	doFix = doFix && player && this->IsImproveAIEnabled(player->playerId);
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
	long int pathFindingResult = callFindPathForUnit(pathFindingArgs);

	if (doFix) {
		// Do NOT forget to restore correct values !!!!
		actorAsType50->positionX = oldX;
		actorAsType50->positionY = oldY;
	}
	return pathFindingResult;
}


// Write the F11 centered text (if displayed)
// Warning, this is only refreshed when population changes ?
void CustomRORCommand::WriteF11PopInfoText(AOE_STRUCTURES::STRUCT_UI_F11_POP_PANEL *f11panel, char *bufferToWrite, char *defaultFormat,
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
		villCount = GetPlayerUnitCount(player, -1, TribeAIGroupCivilian, 2, 2);
		boatVillCount = GetPlayerUnitCount(player, -1, TribeAIGroupFishingBoat, 2, 2) +
			GetPlayerUnitCount(player, -1, TribeAIGroupTradeBoat, 2, 2) +
			GetPlayerUnitCount(player, -1, TribeAIGroupTransportBoat, 2, 2);
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


// Handles the event "farm is depleted". NOT called when a farm is destroyed/killed.
// Warning: this event occurs before the farm unit is actually "killed"
void CustomRORCommand::OnFarmDepleted(long int farmUnitId) {
	AOE_STRUCTURES::STRUCT_UNIT_BUILDING *farm = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)GetUnitStruct(farmUnitId);
	if (!farm || !farm->IsCheckSumValid()) { return; } // (test BUILDING checksum)
	AOE_STRUCTURES::STRUCT_PLAYER *player = farm->ptrStructPlayer;
	if (!player || !player->IsCheckSumValid()) { return; }
	if (player != GetControlledPlayerStruct_Settings()) { return; } // Only for human-controlled player
	if (!player->ptrCreatableUnitsListLink || !player->ptrCreatableUnitsListLink->IsCheckSumValid()) { return; }
	if (!IsGameRunning()) {
		assert(false);
		traceMessageHandler.WriteMessage("OnFarmDepleted called but game not running");
		return;
	}

	// Is feature enabled ?
	if (!CUSTOMROR::crInfo.configInfo.enableAutoRebuildFarms) { return; }

	// Check auto-rebuild farms conditions (parameters)
	if (player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_FOOD) > CUSTOMROR::crInfo.configInfo.autoRebuildFarms_maxFood) { return; }
	if (player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_WOOD) < CUSTOMROR::crInfo.configInfo.autoRebuildFarms_minWood) { return; }
	// Remark : currentFarmCount includes current farm (that is going to be deleted)
	long int currentFarmCount = GetPlayerUnitCount(player, CST_UNITID_FARM, GLOBAL_UNIT_AI_TYPES::TribeAINone, 0, 2); // Include being-built farms
	bool farmCountConditionIsOK = (currentFarmCount <= CUSTOMROR::crInfo.configInfo.autoRebuildFarms_maxFarms);

	AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *farmerUnit = NULL;
	// Search for the farmer that was working on this farm (first -arbitrary- one if there are many)
	AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *curElem = player->ptrCreatableUnitsListLink->lastListElement;
	while ((curElem != NULL) && (farmerUnit == NULL)) {
		if (curElem->unit && curElem->unit->IsCheckSumValidForAUnitClass() && curElem->unit->unitDefinition &&
			curElem->unit->unitDefinition->IsCheckSumValidForAUnitClass() && (curElem->unit->unitDefinition->DAT_ID1 == CST_UNITID_FARMER)) {
			AOE_STRUCTURES::STRUCT_ACTION_BASE *curUnitAction = GetUnitAction(curElem->unit);
			// There is 1 special case when farmer's resourceType is NOT berryBush: when AI player repairs a farm (bug: villager type is farmer instead of repairman)
			if (curUnitAction && (curUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_GATHER_NO_ATTACK)) {
				if (curUnitAction->targetUnitId == farmUnitId) {
					assert(curElem->unit->resourceTypeId == AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_BERRY_STORAGE);
					farmerUnit = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)curElem->unit;
				}
			}
		}
		curElem = curElem->previousElement;
	}
	if (farmerUnit) {
		if (!farmCountConditionIsOK) {
			FarmRebuildInfo *ftmp = CUSTOMROR::crInfo.myGameObjects.FindFarmRebuildInfo(farm->positionX, farm->positionY);
			if (ftmp && ftmp->forceRebuild) {
				farmCountConditionIsOK = true; // "Force rebuild" flag is stronger than farm number limitation.
			}
		}

		if (farmCountConditionIsOK) {
			// Add/Update farm rebuild info to trigger construction of a new farm when "this" one is actually removed.
			FarmRebuildInfo *f = CUSTOMROR::crInfo.myGameObjects.FindOrAddFarmRebuildInfo(farm->positionX, farm->positionY);
			f->villagerUnitId = farmerUnit->unitInstanceId;
			f->playerId = player->playerId;
			f->posX = farm->positionX; // Only useful if just added (otherwise, unchanged)
			f->posY = farm->positionY; // Only useful if just added (otherwise, unchanged)
			f->gameTime = (player->ptrGlobalStruct != NULL) ? player->ptrGlobalStruct->currentGameTime : 0;
		}
	}
}


// Disable dock for all players on maps where AI does NOT builds docks.
void CustomRORCommand::DisableWaterUnitsIfNeeded() {
	if (!CUSTOMROR::crInfo.configInfo.noDockInMostlyLandMaps) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings != NULL);
	assert(settings->IsCheckSumValid());
	if (!settings) { return; } // Should never happen
	if (settings->isCampaign || settings->isScenario || settings->isSavedGame || settings->isMultiplayer) { return; }

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
	if (settings->isCampaign || settings->isScenario || settings->isSavedGame || settings->isMultiplayer) { return; }
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
			AOE_enableResearch(player, CST_RSID_SMALL_WALL, false);
			AOE_enableResearch(player, CST_RSID_MEDIUM_WALL, false);
			AOE_enableResearch(player, CST_RSID_FORTIFICATION, false);
		}
	}
}


// Called on each loop in infAI.FindEnemyUnitIdWithinRange(ptrMyReferenceUnit, maxDistance, DATID, DATID, DATID, DATID)
// This is called quite often (only if improve AI is enabled in customROR configuration)
void CustomRORCommand::OnFindEnemyUnitIdWithinRangeLoop(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *currentUnitListElem) {
	if (!infAI || !infAI->IsCheckSumValid() || !currentUnitListElem) { return; }
	if (IsMultiplayer()) { return; }

	AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(currentUnitListElem->unitId);
	bool elementWasReset = false;
	// Custom treatment: clean obsolete units
	// If element "memorized" position is visible...
	if (IsFogVisibleForPlayer(infAI->commonAIObject.playerId, currentUnitListElem->posX, currentUnitListElem->posY)) {
		// Clean entry if: (we are in the case when "unit list" position is visible, so we can update it without cheating !)
		// - unit no longer exist
		// - unit moved to a position which is NO LONGER visible to me
		if (!unitBase || !unitBase->ptrStructPlayer ||
			(!IsFogVisibleForPlayer(infAI->commonAIObject.playerId, (long int)unitBase->positionX, (long int)unitBase->positionY))) {
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
	
	// Collect info
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
	if (!player || !player->IsCheckSumValid()) {
		return;
	}

	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) gameMainUI->panelSelectedUnit;
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->unitDefinition || !unit->unitDefinition->IsCheckSumValidForAUnitClass()) {
		return;
	}
	if (player != unit->ptrStructPlayer) { return; } // unit does not belong to "me"
	if (unit->unitStatus != 2) {
		return;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
	bool isBuilding = (unitDef->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING);
	bool isLiving = (unitDef->unitType == GLOBAL_UNIT_TYPES::GUT_TRAINABLE);
	if (unitDef->commandAttribute == COMMAND_ATTRIBUTES::CST_CA_NONE) { return; } // Corresponds to unselectable, like eye candy

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) {
		return;
	}

	// Current limitation
	if (!isLiving && !isBuilding) {
		return;
	}
	// For living units
	if (isLiving) {
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDefLiving = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unitDef;
		if (settings->mouseActionType == MOUSE_ACTION_TYPES::CST_MAT_NORMAL) {
			if ((unitDefLiving->blastLevel != BLAST_LEVELS::CST_BL_DAMAGE_TARGET_ONLY) && (unitDefLiving->blastRadius > 0)) {
				UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(unit->unitInstanceId);
				// TODO: localization
				AddInGameCommandButton(CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_VILLAGERS, INGAME_UI_COMMAND_ID::CST_IUC_CROR_DONT_ATTACK_VILLAGERS, 0, false, "Click to prevent unit from attacking villagers automatically",
					&CUSTOMROR::crInfo.customRorIcons, true);
				AddInGameCommandButton(CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_BUILDINGS, INGAME_UI_COMMAND_ID::CST_IUC_CROR_DONT_ATTACK_BUILDINGS, 0, false, "Click to prevent unit from attacking buildings automatically",
					&CUSTOMROR::crInfo.customRorIcons, false);
				AddInGameCommandButton(CST_CUSTOM_BUTTONID_AUTO_ATTACK_DISABLED, INGAME_UI_COMMAND_ID::CST_IUC_CROR_NO_AUTO_ATTACK, 0, false, "Click to prevent unit from attacking other units automatically",
					&CUSTOMROR::crInfo.customRorIcons, false);
				AddInGameCommandButton(CST_CUSTOM_BUTTONID_AUTO_ATTACK_SET_DEFAULT, INGAME_UI_COMMAND_ID::CST_IUC_CROR_RESET_AUTO_ATTACK, 0, false, "Click to restore normal auto-attack behaviour",
					&CUSTOMROR::crInfo.customRorIcons, false);
				const AutoAttackPolicy *aap = (unitInfo && unitInfo->autoAttackPolicyIsSet) ? &unitInfo->autoAttackPolicy : &CUSTOMROR::crInfo.configInfo.autoAttackOptionDefaultValues;
				this->RefreshCustomAutoAttackButtons(gameMainUI, aap);
			}
			AddInGameCommandButton(CST_CUSTOM_BUTTONID_DEFEND_ZONE_OR_UNIT, INGAME_UI_COMMAND_ID::CST_IUC_CROR_DEFEND, 0, false, "Click to select a unit or a position to defend",
				NULL /*&CUSTOMROR::crInfo.customRorIcons*/, false);
		}
		return;
	}
	
	bool multiQueueing = CUSTOMROR::crInfo.configInfo.allowMultiQueueing; // If true, a building can have >1 unit "def" in queue.
	bool buttonIsVisible[12]; // Is button visible after standard buttons display.
	bool forceRefresh[12]; // Used to force refresh of buttons : text or read-only status can be wrong when switching between 2 similar units that have different current actions.
	bool currentButtonDoesNotBelongToThisPage[12];
	long int bestElemTotalCost[12];
	long int bestElemDATID[12];
	bool bestElemIsResearch[12];
	bool bestElemIsAvailable[12]; // Used to disable items (visible, but not clickable)
	short int bestElemLangNameId[12];
	for (int i = 0; i < 12; i++) {
		buttonIsVisible[i] = IsInGameUnitCommandButtonVisible(gameMainUI, i);
		bestElemTotalCost[i] = -1;
		bestElemDATID[i] = -1;
		bestElemIsResearch[i] = false;
		bestElemLangNameId[i] = -1;
		currentButtonDoesNotBelongToThisPage[i] = false;
		bestElemIsAvailable[i] = false;
		forceRefresh[i] = (buttonIsVisible[i]); // Always force refresh for "already displayed" buttons (useful when previous selection had a greyed out button, and new selection has same button - not disabled.
		// TODO: it would be better to force refresh another way to gain performance. But we would need to do this BEFORE buttons are updated (_Before_ShowUnitCommandButtons)
	}
	long int currentActionDATID = -1;
	bool currentActionIsResearch = false;
	long int currentActionButtonIndex = -1;
	short int currentActionLangNameId = -1;
	char *currentActionName = NULL; // useful when currentActionLangNameId is invalid.
	std::list<long int> activableUnitDefIDs = GetActivableUnitDefIDs(player); // A list of unitDef IDs that can be enabled thanks to (available) researches.
	// To support >1 page
	long int minButtonId = gameMainUI->panelButtonIdPageOffset;
	long int minButtonIdNextPage = gameMainUI->panelButtonIdPageOffset + 10 + 1; // 10=pageSize, +1 for next button after "this" last !
	long int maxFoundButtonId = -1;

	// TO DO: manage isBusy for living unit ? (not required as long as we don't customize buttons for living units...)
	AOE_STRUCTURES::STRUCT_UNIT_BUILDING *unitAsBuilding = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)unit;
	bool isBusy = false;
	// Fix queue numbers (otherwise, previous selected building's numbers will still display - and are irrelevant here)
	int buttonWithQueueNumber = -1;
	int queueNumberToDisplay = 0;
	if (unitAsBuilding && unitAsBuilding->IsCheckSumValid()) {
		isBusy = (unitAsBuilding->trainUnitQueueCurrentElemCount > 0);
		if (unitAsBuilding->ptrHumanTrainQueueInformation) {
			queueNumberToDisplay = unitAsBuilding->ptrHumanTrainQueueInformation->unitCount;
			if (queueNumberToDisplay > 0) {
				AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *queuedUnitDefBase = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *) player->GetUnitDefBase(unitAsBuilding->ptrHumanTrainQueueInformation->DATID);
				if (queuedUnitDefBase && queuedUnitDefBase->IsCheckSumValidForAUnitClass()) {
					buttonWithQueueNumber = GetButtonInternalIndexFromDatBtnId(queuedUnitDefBase->trainButton);
				}
			}
		}
	}
	for (int currentBtnId = 0; currentBtnId < 12; currentBtnId++) {
		AOE_STRUCTURES::STRUCT_UI_BUTTON_WITH_NUMBER *curBtn = gameMainUI->unitCommandButtons[currentBtnId];
		if (curBtn && curBtn->IsCheckSumValid()) {
			if ((curBtn->commandIDs[0] == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_ADD_TO_QUEUE) ||
				(AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN)) {
				long int n = GetTotalQueueNumberForUnit(unitAsBuilding, (short int)curBtn->buttonInfoValue[0]);
				curBtn->showNumber = (n > 0) ? 1 : 0;
				curBtn->numberToDisplay = n;
				forceRefresh[currentBtnId] = true;
			}
		}

	}
	AOE_STRUCTURES::STRUCT_ACTION_BASE *currentAction = GetUnitAction(unit);
	// For buildings, currentAction is Non-NULL when researching tech, when AI-triggered "train unit", but NULL for human-triggered "train unit"
	AOE_STRUCTURES::STRUCT_ACTION_MAKE_OBJECT *currentActionAsMakeObject = (AOE_STRUCTURES::STRUCT_ACTION_MAKE_OBJECT *)currentAction;
	AOE_STRUCTURES::STRUCT_ACTION_MAKE_TECH *currentActionAsMakeTech = (AOE_STRUCTURES::STRUCT_ACTION_MAKE_TECH *)currentAction;
	if (currentAction != NULL) {
		isBusy = true;
		if (currentActionAsMakeObject && currentActionAsMakeObject->IsCheckSumValid() &&
			(currentActionAsMakeObject->actionTypeID == currentActionAsMakeObject->GetExpectedInternalActionId())) {
			// This only handles AI-triggered MakeObject, not human-triggered (see below).
			currentActionDATID = currentActionAsMakeObject->targetUnitDAT_ID;
			currentActionIsResearch = false;
		}
		if (currentActionAsMakeTech && currentActionAsMakeTech->IsCheckSumValid() &&
			(currentActionAsMakeTech->actionTypeID == currentActionAsMakeTech->GetExpectedInternalActionId())) {
			// This handles ALL MakeTech actions (both AI or human triggered)
			currentActionDATID = currentActionAsMakeObject->targetUnitDAT_ID;
			currentActionIsResearch = true;
			AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = GetResearchDef(player, currentActionAsMakeObject->targetUnitDAT_ID);
			if (resDef) {
				currentActionLangNameId = resDef->languageDLLName;
				currentActionName = resDef->researchName;
			}
		}

	}
	if ((unitAsBuilding->trainUnitQueueCurrentElemCount > 0) && (unitAsBuilding->ptrHumanTrainQueueInformation != NULL)) {
		// This only handles human-triggered MakeObject (not AI-triggered).
		currentActionDATID = unitAsBuilding->ptrHumanTrainQueueInformation->DATID;
		currentActionIsResearch = false;
	}
	// Get names for current action unit (when training a unit).
	if ((currentActionDATID != -1) && (!currentActionIsResearch)) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *curActionUnitDefBase = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)GetUnitDefStruct(player, (short int)currentActionDATID);
		if (curActionUnitDefBase && curActionUnitDefBase->IsCheckSumValidForAUnitClass()) {
			if (currentActionName == NULL) {
				currentActionName = curActionUnitDefBase->ptrUnitName;
			}
			if (currentActionLangNameId == -1) {
				currentActionLangNameId = curActionUnitDefBase->languageDLLID_Name;
			}
		}
	}


	// Handle more than 1 page : ROR code doesn't, so we have to hide the buttons it added if current page is not 1st one.
	// For the moment, just collect info.
	for (int currentBtnId = 0; currentBtnId < 12; currentBtnId++) {
		AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID curBtnCmdId = (AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID)gameMainUI->unitCommandButtons[currentBtnId]->commandIDs[0];
		if (curBtnCmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN) {
			AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *tmpUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)
				GetUnitDefStruct(player, (short int)gameMainUI->unitCommandButtons[currentBtnId]->buttonInfoValue[0]);

			if (tmpUnitDef && tmpUnitDef->IsCheckSumValidForAUnitClass() && tmpUnitDef->IsTypeValid() &&
				((tmpUnitDef->trainButton >= minButtonIdNextPage) || (tmpUnitDef->trainButton < minButtonId))) {
				currentButtonDoesNotBelongToThisPage[currentBtnId] = true;
			}
		}
		if (curBtnCmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH) {
			AOE_STRUCTURES::STRUCT_RESEARCH_DEF *tmpResearchDef = GetResearchDef(player, 
				(short int)gameMainUI->unitCommandButtons[currentBtnId]->buttonInfoValue[0]);
			if (tmpResearchDef && ((tmpResearchDef->buttonId >= minButtonIdNextPage) || (tmpResearchDef->buttonId < minButtonId))) {
				currentButtonDoesNotBelongToThisPage[currentBtnId] = true;
			}
		}
	}


	// Add custom buttons : not-yet available units/techs

	// RESEARCHES
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *playerResInfo = player->ptrResearchesStruct;
	AOE_STRUCTURES::STRUCT_RESEARCH_DEF_INFO *resDefInfo = NULL;
	if (playerResInfo) {
		resDefInfo = playerResInfo->ptrResearchDefInfo;
	}
	if (playerResInfo && resDefInfo) {
		for (int researchId = 0; (researchId < playerResInfo->researchCount) && (researchId < resDefInfo->researchCount); researchId++) {
			AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_STATUS *status = &playerResInfo->researchStatusesArray[researchId];
			AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = resDefInfo->GetResearchDef(researchId);
			assert(resDef);
			int rawButtonId = resDef->buttonId;
			if ((resDef->researchLocation == unitDef->DAT_ID1) &&
				(rawButtonId > maxFoundButtonId)) {
				maxFoundButtonId = rawButtonId;
			}
			long int buttonIndex = -1;
			if ((rawButtonId >= minButtonId) && (rawButtonId < minButtonIdNextPage)) {
				buttonIndex = GetButtonInternalIndexFromDatBtnId(rawButtonId);
			}
			assert(buttonIndex <= 12); // But can be -1
			// rawButtonId=0 for non-visible researches, we get buttonIndex=-1 in this case. Exclude it.
			bool researchIsAvailable = (status->currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE);
			if ((buttonIndex > -1) && 
				((!buttonIsVisible[buttonIndex]) || (currentButtonDoesNotBelongToThisPage[buttonIndex])) &&
				(	(status->currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) || 
					(researchIsAvailable && isBusy)
				)
				) {
				// Sorting strategy:
				// We suppose higher cost=further research, so lower cost should be first (next) available research.
				long int thisCost = 0;
				if (resDef->costUsed1) {
					thisCost += resDef->costAmount1;
				}
				if (resDef->costUsed2) {
					thisCost += resDef->costAmount2;
				}
				if (resDef->costUsed3) {
					thisCost += resDef->costAmount3;
				}
				bool costIsBetter = (bestElemTotalCost[buttonIndex] == -1) || (thisCost < bestElemTotalCost[buttonIndex]);
				
				if (((isBusy && researchIsAvailable) || costIsBetter) && // When we choose to show available researches, they have best priority.
					(resDef->researchLocation == unitDef->DAT_ID1) &&
					((resDef->researchTime > 0) || researchIsAvailable) // Hide unavailable immediate researches
					) {
					bestElemDATID[buttonIndex] = researchId;
					bestElemIsResearch[buttonIndex] = true;
					bestElemTotalCost[buttonIndex] = thisCost;
					bestElemLangNameId[buttonIndex] = resDef->languageDLLName;
					bestElemIsAvailable[buttonIndex] = researchIsAvailable && !isBusy;
				}
			}
		}
	}

	// TRAIN UNITS
	if (player->ptrStructDefUnitTable) {
		for (int loopUnitDefId = 0; loopUnitDefId < player->structDefUnitArraySize; loopUnitDefId++) {
			AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *loopUnitDef = 
				(AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)player->ptrStructDefUnitTable[loopUnitDefId];
			if (loopUnitDef && loopUnitDef->IsCheckSumValidForAUnitClass() && loopUnitDef->IsTypeValid()) { // Only for living units
				int rawButtonId = loopUnitDef->trainButton;
				if ((loopUnitDef->trainLocation == unitDef->DAT_ID1) && (rawButtonId > maxFoundButtonId)) {
					maxFoundButtonId = rawButtonId;
				}
				long int buttonIndex = -1;
				if ((rawButtonId >= minButtonId) && (rawButtonId < minButtonIdNextPage)) {
					buttonIndex = GetButtonInternalIndexFromDatBtnId(rawButtonId);
				}
				assert(buttonIndex <= 12); // But can be -1
				if ((buttonIndex >= 0) && (buttonIndex <= 12) &&
					((!buttonIsVisible[buttonIndex]) || (currentButtonDoesNotBelongToThisPage[buttonIndex])) &&
					(loopUnitDef->trainLocation == unitDef->DAT_ID1)) {
					long int thisCost = 0;
					if (loopUnitDef->costs[0].costPaid) {
						thisCost += loopUnitDef->costs[0].costAmount;
					}
					if (loopUnitDef->costs[1].costPaid) {
						thisCost += loopUnitDef->costs[1].costAmount;
					}
					if (loopUnitDef->costs[2].costPaid) {
						thisCost += loopUnitDef->costs[2].costAmount;
					}
					bool preferThisUnit = false;
					bool costIsBetter = (bestElemTotalCost[buttonIndex] == -1) || (thisCost < bestElemTotalCost[buttonIndex]);
					if (loopUnitDef->availableForPlayer) { preferThisUnit = true; } // if unit is currently available, it has best priority !
					if (costIsBetter && !loopUnitDef->availableForPlayer) { preferThisUnit = true; }
					if (preferThisUnit && (!bestElemIsResearch) && (bestElemDATID[buttonIndex] >= 0)) { // only if we already selected a valid DATID (unit, ont research)
						AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *previousUnit = 
							(AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)player->ptrStructDefUnitTable[bestElemDATID[buttonIndex]];
						// Cancel "prefer" if new one is stronger (stronger = less priority, because further in tech tree)
						if ((previousUnit->totalHitPoints < loopUnitDef->totalHitPoints) || // Use only attributes that ALWAYS increase ! Not speed (risky)
							(previousUnit->displayedAttack < loopUnitDef->displayedAttack)
							) {
							preferThisUnit = false; // "new" unit seems stronger, keep previous one.
						}
					}
					bool unitAvailableInTechTree = (loopUnitDef->availableForPlayer != 0);
					if (!loopUnitDef->availableForPlayer) {
						auto it = std::find_if(activableUnitDefIDs.begin(), activableUnitDefIDs.end(),
							[loopUnitDefId](long int tmpUnitDefId) { return (tmpUnitDefId == loopUnitDefId); });
						if (it != activableUnitDefIDs.end()) { // Found: this unit CAN be enabled in tech tree.
							unitAvailableInTechTree = true;
						}
					}

					if (preferThisUnit && unitAvailableInTechTree) {
						bestElemDATID[buttonIndex] = loopUnitDefId;
						bestElemIsResearch[buttonIndex] = false;
						bestElemTotalCost[buttonIndex] = thisCost;
						bestElemLangNameId[buttonIndex] = loopUnitDef->languageDLLID_Name;
						bestElemIsAvailable[buttonIndex] = (loopUnitDef->availableForPlayer != 0);
					}
				}
			}
		}
	}

	// Hide buttons added by ROR and that do not belong to current page
	for (int currentBtnId = 0; currentBtnId < 12; currentBtnId++) {
		if (currentButtonDoesNotBelongToThisPage[currentBtnId]) {
			bool forceShowAnyway = false;
			// There is only 1 exception : currently-being trained unit should always be displayed
			if (isBuilding && (currentActionDATID > -1)) {
				AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID curBtnCmdId = (AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID)gameMainUI->unitCommandButtons[currentBtnId]->commandIDs[0];
				long int btnDATID = gameMainUI->unitCommandButtons[currentBtnId]->buttonInfoValue[0];
				if ((curBtnCmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN) &&
					!currentActionIsResearch && (btnDATID == currentActionDATID) ) {
					forceShowAnyway = true;
				}
				if ((curBtnCmdId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH) &&
					currentActionIsResearch && (btnDATID == currentActionDATID) ) {
					forceShowAnyway = true;
				}
			}
			if (!forceShowAnyway) {
				AOE_ShowUIObject(gameMainUI->unitCommandButtons[currentBtnId], false);
				gameMainUI->unitCommandButtons[currentBtnId]->buttonInfoValue[0] = -1;
				gameMainUI->unitCommandButtons[currentBtnId]->commandIDs[0] = 0;
			}
		}
	}
	
	if (minButtonId > 0) {
		for (int buttonIndex = 0; buttonIndex < 12; buttonIndex++) {
			forceRefresh[buttonIndex] = true; // Force text refresh on ROR-added buttons on page 2+ (not well supported natively, no text is displayed)
		}
	}
	for (int buttonIndex = 0; buttonIndex < 12; buttonIndex++) {
		if (!bestElemIsResearch[buttonIndex] && multiQueueing) {
			forceRefresh[buttonIndex] = true; // Force text refresh all "train unit" buttons when multi queueing is enabled, to show correct queue number/enabled status
		}
	}
	for (int buttonIndex = 0; buttonIndex < 12; buttonIndex++) {
		AOE_STRUCTURES::STRUCT_UI_BUTTON_WITH_NUMBER *sb = gameMainUI->unitCommandButtons[buttonIndex];
		if (buttonIsVisible[buttonIndex] && forceRefresh[buttonIndex]) {
			long int correctButtonInfoValue = sb->buttonInfoValue[0];
			long int correctButtonCmdId = sb->commandIDs[0];
			if (correctButtonCmdId > 0) {
				bool disabled = (sb->readOnly != 0);
				AddInGameCommandButton(buttonIndex, (INGAME_UI_COMMAND_ID)correctButtonCmdId, correctButtonInfoValue,
					disabled, NULL, NULL, true);
			}
		}
	}

	// Now display our custom buttons (... on free slots)
	char nameBuffer[200];
	for (int buttonIndex = 0; buttonIndex < 12; buttonIndex++) {
		std::string elementInfo;
		if (bestElemDATID[buttonIndex] != -1) {
			GetLanguageDllText(bestElemLangNameId[buttonIndex], nameBuffer, sizeof(nameBuffer), "");
			if (bestElemIsResearch[buttonIndex]) {
				AddInGameCommandButton(buttonIndex, INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH, bestElemDATID[buttonIndex], !bestElemIsAvailable[buttonIndex], NULL /*elementInfo.c_str()*/, NULL, true);
				AOE_STRUCTURES::STRUCT_UI_BUTTON_WITH_NUMBER *sb = gameMainUI->unitCommandButtons[buttonIndex];
				sb->unknown_2C4;
				sb->helpDllId = sb->helpDllId;
				sb->winHelpDataDllId;
				sb->buttonInfoValue;
			} else {
				AddInGameCommandButton(buttonIndex, INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN, bestElemDATID[buttonIndex],
					!bestElemIsAvailable[buttonIndex] || !multiQueueing, NULL /*elementInfo.c_str()*/, NULL, true);
			}
		}
	}

	// Add custom buttons : if top-right button is free, show current action
	const int buttonIdForInProgress = 5;
	if ((currentActionDATID != -1) && !IsInGameUnitCommandButtonVisible(buttonIdForInProgress)) {
		GetLanguageDllText(currentActionLangNameId, nameBuffer, sizeof(nameBuffer), "");
		if ((nameBuffer[0] == 0) && (currentActionName != NULL)) {
			strcpy_s(nameBuffer, currentActionName);
		}
		std::string inProgressInfo;
		if (currentActionIsResearch) {
			inProgressInfo = localizationHandler.GetTranslation(CRLANG_ID_BEING_RESEARCHED, "Being researched:");
			inProgressInfo += " ";
			inProgressInfo += nameBuffer;
			AddInGameCommandButton(buttonIdForInProgress, INGAME_UI_COMMAND_ID::CST_IUC_DO_RESEARCH, currentActionDATID, true, inProgressInfo.c_str(), NULL, true);
		} else {
			inProgressInfo = localizationHandler.GetTranslation(CRLANG_ID_BEING_TRAINED, "Being trained:");
			inProgressInfo += " ";
			inProgressInfo += nameBuffer;
			AddInGameCommandButton(buttonIdForInProgress, INGAME_UI_COMMAND_ID::CST_IUC_DO_TRAIN, currentActionDATID, true, inProgressInfo.c_str(), NULL, true);
		}
	}

	// When controlled player is an AI player, AI-triggered "make objects" (train unit) can't be stopped. Fix it.
	// Note: for human player-triggered "train unit", a STOP button is already visible (command=STOP), leave it unchanged.
	// WARNING: clicking on STOP for AI-triggered "train unit" does not update strategy and units will never be trained again ! Needs a fix !
	const int buttonIdForStop = 6;
	if (isBuilding && (currentAction != NULL) && (currentAction->actionTypeID == UNIT_ACTION_ID::CST_IAI_MAKE_OBJECT) &&
		(gameMainUI->unitCommandButtons[buttonIdForStop]->commandIDs[0] != (long int)INGAME_UI_COMMAND_ID::CST_IUC_STOP)) {
		GetLanguageDllText(LANG_ID_STOP_CURRENT_ACTION, nameBuffer, sizeof(nameBuffer), "Stop current action");
		AddInGameCommandButton(buttonIdForStop, INGAME_UI_COMMAND_ID::CST_IUC_STOP, 0, false, nameBuffer, NULL, true);
	}

	// Show Next page button (not if busy, because we already have current action there)
	bool hasNextPage = (maxFoundButtonId >= minButtonIdNextPage); // has more buttons (Next actually goes to next page, NOT to first one)
	if (!isBusy && (hasNextPage || (minButtonId > 0))) {
		// There are buttons to display on next page: show button
		AddInGameCommandButton(5, INGAME_UI_COMMAND_ID::CST_IUC_NEXT_PAGE, 0, false, NULL, NULL, true);
		// We store the information "this is not last page" in button's infoValue.
		gameMainUI->unitCommandButtons[5]->buttonInfoValue[0] = hasNextPage;
	}
}



// Refresh status for custom auto-attack policy buttons
void CustomRORCommand::RefreshCustomAutoAttackButtons(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
	const AutoAttackPolicy *attackPolicy) {
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		return;
	}
	if (!attackPolicy) {
		attackPolicy = &CUSTOMROR::crInfo.configInfo.autoAttackOptionDefaultValues;
	}
	int value = attackPolicy->attackVillagers ? 0 : 1;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_VILLAGERS]->showNumber = value;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_VILLAGERS]->numberToDisplay = value;
	AOE_RefreshUIObject(gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_VILLAGERS]);
	value = attackPolicy->attackNonTowerBuildings ? 0 : 1;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_BUILDINGS]->showNumber = value;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_BUILDINGS]->numberToDisplay = value;
	AOE_RefreshUIObject(gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_NOT_BUILDINGS]);
	value = (!attackPolicy->attackMilitary && !attackPolicy->attackNonTowerBuildings && !attackPolicy->attackTowers && !attackPolicy->attackVillagers);
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_DISABLED]->showNumber = value;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_DISABLED]->numberToDisplay = value;
	AOE_RefreshUIObject(gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_DISABLED]);
	value = (attackPolicy->Equals(CUSTOMROR::crInfo.configInfo.autoAttackOptionDefaultValues)) ? 1 : 0;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_SET_DEFAULT]->showNumber = value;
	gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_SET_DEFAULT]->numberToDisplay = value;
	AOE_RefreshUIObject(gameMainUI->unitCommandButtons[CST_CUSTOM_BUTTONID_AUTO_ATTACK_SET_DEFAULT]);
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
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = NULL;
	AOE_STRUCTURES::STRUCT_PLAYER *player = NULL;
	if (gameMainUI->panelSelectedUnit && gameMainUI->panelSelectedUnit->IsCheckSumValidForAUnitClass()) {
		unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) gameMainUI->panelSelectedUnit;
		if (unitBase && unitBase->IsCheckSumValidForAUnitClass()) {
			player = unitBase->ptrStructPlayer;
		}
	}
	// No additional actions when viewing a unit that is not mine !
	if (!unitBase || !player || !player ->IsCheckSumValid() || (player != GetControlledPlayerStruct_Settings())) {
		return false;
	}

	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CANCEL_SELECTION) {
		if (settings->mouseActionType == MOUSE_ACTION_TYPES::CST_MAT_CR_PROTECT_UNIT_OR_ZONE) {
			settings->mouseActionType = MOUSE_ACTION_TYPES::CST_MAT_NORMAL;
			SetGameCursor(GAME_CURSOR::GC_NORMAL);
			return true; // this action "exited" from "select unit to defend" action. It must not unselect unit too ! Mark event as handled.
		}
	}

	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_STOP) {
		// Fix strategy when AI is enabled and some action is interrupted by human player
		if (gameMainUI->panelSelectedUnit && gameMainUI->panelSelectedUnit->IsCheckSumValidForAUnitClass()) {
			AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = GetStrategyElementForActorBuilding(player, unitBase->unitInstanceId);
			ResetStrategyElementStatus(stratElem); // does nothing if stratElem is NULL.
		}
		return false; // Let ROR code execute normally here, we just ran "auxiliary" treatments.
	}

	// Handle next page. Note: in ROR, see 485140 (for villager build menu only)
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_NEXT_PAGE) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unitBase->unitDefinition;
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

	// Custom buttons
	bool updateAutoAttackInfo = false;
	AutoAttackPolicy flagsToApply = { false, false, false, false, false };
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_DONT_ATTACK_VILLAGERS) {
		UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(unitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = true;
		unitInfo->autoAttackPolicy.attackVillagers = false;
		flagsToApply.attackVillagers = true; // this flag has been updated
		RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_DONT_ATTACK_BUILDINGS) {
		UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(unitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = true;
		unitInfo->autoAttackPolicy.attackNonTowerBuildings = false;
		flagsToApply.attackNonTowerBuildings = true; // this flag has been updated
		RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_NO_AUTO_ATTACK) {
		UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(unitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = true;
		unitInfo->autoAttackPolicy.attackMilitary = false;
		unitInfo->autoAttackPolicy.attackNonTowerBuildings = false;
		unitInfo->autoAttackPolicy.attackVillagers = false;
		unitInfo->autoAttackPolicy.attackTowers = false;
		flagsToApply.attackMilitary = true; // this flag has been updated
		flagsToApply.attackNonTowerBuildings = true; // this flag has been updated
		flagsToApply.attackVillagers = true; // this flag has been updated
		flagsToApply.attackTowers = true; // this flag has been updated
		RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_RESET_AUTO_ATTACK) {
		UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(unitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = false;
		unitInfo->autoAttackPolicy.SetDefaultValues();
		flagsToApply.SetAllValues(true); // All flags have been updated.
		CUSTOMROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(unitBase->unitInstanceId);
		RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	// Apply changes on ALL compatible selected units
	if (updateAutoAttackInfo) {
		UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(unitBase->unitInstanceId);
		assert(unitInfo != NULL); // Was just added
		if (!unitInfo) { return false; } // this is an error case
		CUSTOMROR::crInfo.ApplyAutoAttackPolicyToPlayerSelectedUnits(player, unitInfo->autoAttackPolicy, flagsToApply);
	}
	// Button "protect unit or protect zone" : set mouse custom cursor type
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_DEFEND) {
		SetGameCursor(GAME_CURSOR::GC_GROUP);
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		if (settings && settings->IsCheckSumValid()) {
			settings->mouseActionType = MOUSE_ACTION_TYPES::CST_MAT_CR_PROTECT_UNIT_OR_ZONE;
			CallWriteCenteredText("Right-click to define the unit or the position to protect"); // TODO localization
		}
	}

	return false;
}


// Custom treatment to decide if a potential target unit should be ignored
// Overload standard rules for catapults(ignores building if HP=1) and "target=wall" cases.
// Default result=false (on error cases)
bool CustomRORCommand::AutoSearchTargetShouldIgnoreUnit(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *potentialTargetUnit) {
	assert(CUSTOMROR::crInfo.configInfo.useEnhancedRulesForAutoAttackTargetSelection); // should have been checked before calling this
	if (!activity || !potentialTargetUnit || !potentialTargetUnit->IsCheckSumValidForAUnitClass()) {
		return false; // error case
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *targetUnitDefBase = potentialTargetUnit->unitDefinition;
	if (!targetUnitDefBase || !targetUnitDefBase->IsCheckSumValidForAUnitClass()) {
		return false; // error case
	}
	AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)activity->ptrUnit;
	if (!actorUnit || !actorUnit->IsCheckSumValidForAUnitClass()) {
		return false; // We expect ACTOR unit to derive from type50 ! (should be living or building)
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *actorUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)actorUnit->unitDefinition;
	if (!actorUnitDef || !actorUnitDef->IsCheckSumValidForAUnitClass()) {
		return false; // We expect ACTOR unit to derive from type50 ! (should be living or building)
	}
	
	bool canHurtOtherUnits = (actorUnitDef->blastLevel != AOE_CONST_FUNC::BLAST_LEVELS::CST_BL_DAMAGE_TARGET_ONLY) &&
		(actorUnitDef->blastRadius > 0);
	bool isMelee = (actorUnitDef->maxRange == 0);
	bool isCatapult = canHurtOtherUnits && (actorUnitDef->unitAIType == TribeAIGroupSiegeWeapon);
	// Note: catapult trireme, juggernaught, cleopatra are warships (22), not TribeAIGroupSiegeWeapon.
	bool targetIsWall = (targetUnitDefBase->unitAIType == TribeAIGroupWall);
	bool targetIsTower = IsTower(targetUnitDefBase);
	bool targetIsMilitary = IsNonTowerMilitaryUnit(targetUnitDefBase->unitAIType);

	bool applyWeirdRuleOnCatsWith1HPBuilding = true; // True if we want the same (weird) rule as original ROR code on catapults and buildings with HP=1
	if (applyWeirdRuleOnCatsWith1HPBuilding && isCatapult && (potentialTargetUnit->remainingHitPoints <= 1)) {
		return true; // Ignore unit. Not sure why there is that check !
	}


	// Custom rules
	bool hasRuleAtUnitLevel = false;
	AutoAttackPolicy *policy = isMelee ? &CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits :
		&CUSTOMROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits;
	// If there is a config at unit level, use it instead of global parameter.
	UnitCustomInfo *unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(actorUnit->unitInstanceId);
	if (unitInfo && unitInfo->autoAttackPolicyIsSet) {
		policy = &unitInfo->autoAttackPolicy;
		hasRuleAtUnitLevel = true;
	}

	bool targetIsVillager = IsVillager_includingShips(targetUnitDefBase->DAT_ID1);
	bool targetIsNonTowerBuilding = (targetUnitDefBase->unitAIType == TribeAIGroupBuilding) && (!IsTower(targetUnitDefBase->DAT_ID1));

	bool policyTellsToIgnore =
		(!policy->attackVillagers && targetIsVillager) ||
		(!policy->attackNonTowerBuildings && targetIsNonTowerBuilding) ||
		(!policy->attackTowers && targetIsTower) ||
		(!policy->attackMilitary && targetIsMilitary) ||
		(!policy->attackWalls && targetIsWall);

	if ((canHurtOtherUnits || hasRuleAtUnitLevel) && policyTellsToIgnore) {
		return true;
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
			CST_CUSTOMROR_SLP_INDEX_FOR_UNIT_SHORTCUT_10; // Add offset to position on "shortcut 10" index in SLP file
	} else {
		slpFileIndex = CST_CUSTOMROR_SLP_INDEX_FOR_GROUPED_UNIT; // shortcut corresponds to a unit group, not a shortcut
	}
	slpArrayIndex = slpFileIndex - 1;
	if ((slpFileIndex < 0) || (slpFileIndex > CST_CUSTOMROR_MAX_SLP_INDEX_IN_UNIT_SHORTCUTS_FILE) ||
		((slpFileIndex > CST_CUSTOMROR_MAX_SLP_INDEX_FOR_UNIT_SHORTCUTS) && (slpFileIndex != CST_CUSTOMROR_SLP_INDEX_FOR_GROUPED_UNIT))
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

	DisplayUnitShortcutSymbol(slpHeader, slpFrameHeader, posX - 9, posY - 8, unknown_arg3);
	return false;
}


// Adds custom attributes (armor) in buildings' unit info zone.
// currentLine is incremented if lines are added.
void CustomRORCommand::DisplayCustomBuildingAttributesInUnitInfo(AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone, long int &currentLine) {
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
	long int iconIdMeleeArmor = 8; // melee armor icon in SLP
	long int iconIdPierce = 10; // pierce armor icon in SLP
	_asm {
		MOV ECX, unit50;
		LEA EAX, pierceDisplayedValue;
		LEA EDX, pierceTotalValue;
		PUSH EAX;
		PUSH EDX;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x248]; // Get pierce armor
		MOV ECX, unit50;
		LEA EAX, meleeDisplayedValue;
		LEA EDX, meleeTotalValue;
		PUSH EAX;
		PUSH EDX;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0x230]; // Get melee armor
	}
	if (meleeTotalValue > 0) {
		// Note: line is incremented in the method if a line is added.
		UnitInfoZoneAddAttributeLine(unitInfoZone, iconIdMeleeArmor, 1, meleeDisplayedValue, meleeTotalValue, currentLine);
	}
	if (pierceTotalValue > 0) {
		// Note: line is incremented in the method if a line is added.
		UnitInfoZoneAddAttributeLine(unitInfoZone, iconIdPierce, 1, pierceDisplayedValue, pierceTotalValue, currentLine);
	}
}


// Get a localized string to overload ROR's string localization system (language(x).dll)
// Returns true if a (custom) localized string has been written into buffer.
bool CustomRORCommand::GetLocalizedString(long int stringId, char *buffer, long int bufferSize) {
	if (!buffer) { return false; }
	return localizationHandler.ReadTranslation((short int)stringId, buffer, bufferSize);
}


// Returns false if we should prevent unit from moving back (to maxrange) after shooting. Default result=true
bool CustomRORCommand::ShouldRetreatAfterShooting(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity) {
	assert(activity);
	if (!activity || !activity->ptrUnit) { return true; }
	AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *actorUnitType50 = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE*)activity->ptrUnit;
	if (!actorUnitType50 || !actorUnitType50->IsCheckSumValidForAUnitClass() || !actorUnitType50->DerivesFromAttackable()) { return true; }
	if (!actorUnitType50->ptrStructPlayer || !actorUnitType50->ptrStructPlayer->IsCheckSumValid()) { return true; }
	// This feature is an AI improvement. Do it only if config says so.
	if (!this->IsImproveAIEnabled(actorUnitType50->ptrStructPlayer->playerId)) {
		return true;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *actorUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)actorUnitType50->unitDefinition;
	if (!actorUnitDef || !actorUnitDef->IsCheckSumValidForAUnitClass() || !actorUnitDef->DerivesFromAttackable()) { return true; }
	if (!actorUnitDef->DerivesFromAttackable()) { return true; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(activity->targetUnitId);
	if (!targetUnit || !targetUnit->IsCheckSumValidForAUnitClass()) { return true; }
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *targetUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)targetUnit->unitDefinition;
	if (targetUnitDef->DerivesFromAttackable()) {
		if ((targetUnitDef->projectileUnitId >= 0) && (targetUnitDef->maxRange > actorUnitDef->maxRange)) {
			// Force NOT retreat after shooting, because enemy has a better range than me !
			return false;
		}
	}
	return true; // Default behaviour
}


// Occurs when a unit is killed by an attack (excludes suicide with DEL, transported units whose transport is destroyed, conversion)
void CustomRORCommand::OnAttackableUnitKilled(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *killedUnit, AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit) {
	if (!actorUnit || !actorUnit->IsCheckSumValidForAUnitClass()) {
		return;
	}
	if (!killedUnit || !killedUnit->IsCheckSumValidForAUnitClass()) {
		return;
	}
	assert(killedUnit->DerivesFromAttackable());
	if (!killedUnit->DerivesFromAttackable()) { return; }

	if (this->IsRpgModeEnabled() && actorUnit->DerivesFromTrainable()) {
		RPG_MODE::OnUnitKill(killedUnit, (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE*)actorUnit);
	}

	// Handle internal objects
	CUSTOMROR::crInfo.myGameObjects.RemoveAllInfoForUnit(killedUnit->unitInstanceId, killedUnit->positionX, killedUnit->positionY);
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
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !controlledPlayer || !controlledPlayer->IsCheckSumValid() || unitPlayerId < 0) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
	assert(unitDef && unitDef->IsCheckSumValidForAUnitClass());
	if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) { return false; }
	bool updatedValues = false;
	PLAYER_DIPLOMACY_VALUES diplValue = controlledPlayer->diplomacyVSPlayers[unitPlayerId];
	GLOBAL_UNIT_AI_TYPES actorAIType = TribeAINone; // controlled player's main selected unit AI type
	bool actorIsVillager = false; // true if controlled player's main selected unit is a civilian (class 4)

	if (unit->DerivesFromTrainable()) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *mainSelectedUnit = CUSTOMROR::crInfo.GetMainSelectedUnit(controlledPlayer);
		if (mainSelectedUnit && mainSelectedUnit->IsCheckSumValidForAUnitClass() && mainSelectedUnit->DerivesFromTrainable()) {
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *selectedUnitDef = mainSelectedUnit->unitDefinition;
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
		cursorToForce = GetCursorForCustomActionType(settings->mouseActionType); // Force usage of this mouse cursor
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
	return updatedValues;
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
	UnitCustomInfo *unitInfo = NULL;
	if (actorUnit) {
		unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(actorUnit->unitInstanceId);
	}

	switch (settings->mouseActionType) {
	case CST_MAT_CR_PROTECT_UNIT_OR_ZONE:
		if (actorIsMyUnit && (actorUnit == mouseTargetUnit)) {
			unitInfo = CUSTOMROR::crInfo.myGameObjects.FindUnitCustomInfo(actorUnit->unitInstanceId);
			if (unitInfo) {
				unitInfo->ResetProtectInfo();
				CUSTOMROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(actorUnit->unitInstanceId);
			}
			CallWriteCenteredText("Removed protect information for current unit.");
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

	// Restore mouse status before quitting
	settings->mouseActionType = MOUSE_ACTION_TYPES::CST_MAT_NORMAL;
	SetGameCursor(GAME_CURSOR::GC_NORMAL);
}


// Entry point when a unit activity stops.
// This is NOT called when 
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
	for (int i = 0; i < activity->nextActivityQueueUsedElems; i++) {
		ACTIVITY_TASK_IDS taskId = activity->nextActivitiesQueue_unsure[i].activityId;
		if (taskId != ACTIVITY_TASK_IDS::CST_ATI_NOTIFY_ACTIVITY_COMPLETED) {
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
				bool unitIsVisible = IsFogVisibleForPlayer(controlledPlayer->playerId, (long int)unitToProtect->positionX, (long int)unitToProtect->positionY);
				if (!unitIsVisible) {
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
				if (IsUnitIdle(unit) && unit->ptrActionInformation) {
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
							CreateCmd_RightClick(unit->unitInstanceId, -1, refX, refY);
						}
					}
				}
			}
		}
	}
}


// Computes (existing) building influence zone for farm placement map like values computation.
// existingBuilding must be a building (current building from loop, to take into account in map like values)
// Updates existingBldInfluenceZone with the influence distance we want to use for provided building (positions near building will be preferred)
// Updates skipThisBuilding to true if the building must be ignored for farm position computation. If true, existingBldInfluenceZone returned value should be ignored.
// Improvements depend on configuration (useEnhancedFeature and improveAILevel) + game difficulty level
void CustomRORCommand::FixCityPlanFarmPlacement(AOE_STRUCTURES::STRUCT_UNIT_BASE *existingBuilding, long int &existingBldInfluenceZone, bool &skipThisBuilding) {
	skipThisBuilding = false;
	existingBldInfluenceZone = 3;
	if (!existingBuilding || !existingBuilding->IsCheckSumValidForAUnitClass()) { return; }

	// Default behavior / values
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef_base = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)existingBuilding->unitDefinition;
	assert(unitDef_base && unitDef_base->IsCheckSumValidForAUnitClass());
	if (!unitDef_base && !unitDef_base->IsCheckSumValidForAUnitClass()) { return; }
	
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());

	// Set some default values
	existingBldInfluenceZone = (int)unitDef_base->lineOfSight; // Default (from ROR code: use existing bld LOS)
	int difficultyLevel = settings->difficultyLevel;
	bool useEnhancedFeature = CUSTOMROR::crInfo.configInfo.cityPlanLikeValuesEnhancement;
	bool isRelevantDropSite = (unitDef_base->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_GRANARY) ||
		(unitDef_base->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_FORUM); // Note: forager's storage sites are defined in empires.dat (Drop site in AGE3) => granary (68) and TC (109)
	
	if (!useEnhancedFeature) {
		return; // Feature is not enabled: exit and use defaults.
	}

	if (existingBuilding->ptrStructPlayer && existingBuilding->ptrStructPlayer->IsCheckSumValid() &&
		!this->IsImproveAIEnabled(existingBuilding->ptrStructPlayer->playerId)) {
		return; // Do not improve if config says no AI improvement
	}

	// Improved farming is so good for AI we must apply this carefully, according to AI level, so AI doesn't get too strong in easy levels.
	if (difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASY) {
		return; // Easy (3) and easiest (4): use default behavior (exit and use defaults)
	}

	// Medium difficulty : just exclude some buildings (hard to find a compromise between no fix at all and very important fix)
	if (difficultyLevel == AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_MEDIUM) {
		// Do not add "like" value near storage pit because it disturbs other gatherers
		// Do not add "like" value near towers because they almost always are far from TC
		if ((unitDef_base->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_STORAGE_PIT) ||
			IsTower(unitDef_base->DAT_ID1) ){
			skipThisBuilding = true;
		}
		if (!isRelevantDropSite) {
			// Keep default influence zone for normal buildings (just decrease it by 1, to get a slightly better result)
			if (existingBldInfluenceZone > 3) {
				existingBldInfluenceZone--;
			}
		} else {
			// TC or granary: extend influence to get a slightly better result
			if (existingBldInfluenceZone < 5) {
				existingBldInfluenceZone = 5;
			}
			existingBldInfluenceZone++;
		}
		// No more improvements in medium level.
		return;
	}

	// HERE: feature is ON and we are in hard or hardest level.

	// Ignore all buildings that are not farming drop sites, there is no reason to build farms near them !!!
	if (!isRelevantDropSite) {
		skipThisBuilding = true;
		return;
	}

	// Loop (existing) building is a relevant drop site: set a correct influence zone
	// This influence zone must be high enough to cover most of the city (so we're sure to find a value if not much space is left)
	// ROR code will divide influence zone into 3 zones with progressive priorities (closest=best score)
	existingBldInfluenceZone = 11;
}


// Updates temp map like data for choosing a new building location according to existing buildings
// Improvements depend on configuration (useEnhancedFeature and improveAILevel) + game difficulty level
void CustomRORCommand::ManageCityPlanOtherBuildingsImpact(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem, AOE_STRUCTURES::STRUCT_POSITION_INFO *TCPosition) {
	if (!infAI || !stratElem || !TCPosition) { return; }
	assert(infAI->IsCheckSumValid());
	assert(stratElem->IsCheckSumValid());
	if (!infAI->IsCheckSumValid() || !stratElem->IsCheckSumValid()) { return; }
	long int TC_x = TCPosition->posX;
	long int TC_y = TCPosition->posY;
	assert(TC_x >= 0);
	assert(TC_y >= 0);

	// Stop here is feature is not enabled in configuration
	if (!CUSTOMROR::crInfo.configInfo.cityPlanLikeValuesEnhancement) { return; }

	if (!this->IsImproveAIEnabled(infAI->commonAIObject.playerId)) {
		return;
	}

	// Collect useful context information : player, strategy element to build...
	AOE_STRUCTURES::STRUCT_AI *ai = infAI->ptrMainAI;
	assert(ai && ai->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *player = ai->ptrStructPlayer;
	assert(player != NULL);
	assert(player->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	assert(globalStruct != NULL);
	assert(globalStruct->IsCheckSumValid());

	AOE_STRUCTURES::STRUCT_TEMP_MAP_BUILD_LIKE_INFOS *mapInfosStruct = ROR_pTempMapBuildLikeInfos;
	assert(mapInfosStruct != NULL);
	assert(mapInfosStruct->IsCheckSumValid());

	// Note about positions:
	// (tileX,tileY) tile positions are integer. So tile area is game (float) positions tileX<=x<=tileX+1 (same for Y).
	// For 2*2 building: buildingPos=(5,10) (for example) => tiles are (4,9) (4,10) (5,9) (5,10)
	// For 3*3 building: buildingPos=(5.5,10.5) (for example) => tiles are (4,9)->(6,9) (4,10)->(6,10) (4,11)->(6,11)
#ifdef _DEBUG
	// Note: x appears vertically in this representation
	char *test = DumpPosToTextBuffer(mapInfosStruct, TC_x, TC_y, 25); // TEST - TO DO : REMOVE
#endif

	bool hasHouse = false; // True if at least 1 house already exists
	bool hasTC = false; // True if at least 1 Town center already exists

	// 1) Loop on my buildings just to collect info
	for (long int index = 0; index < infAI->buildingUnits.usedElements; index++) {
		short int unitId = (short int)infAI->buildingUnits.unitIdArray[index];
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = globalStruct->ptrUnitPointersList[unitId];
		if (unit) {
			assert(unit->IsCheckSumValidForAUnitClass());
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *defUnit = unit->unitDefinition;
			if (defUnit) {
				assert(defUnit->IsCheckSumValidForAUnitClass());
				if (defUnit->DAT_ID1 == CST_UNITID_HOUSE) {
					// Mark if we found a house (later we will have special treatment for 1st house to build it faster)
					hasHouse = true;
				}
				if (defUnit->DAT_ID1 == CST_UNITID_FORUM) {
					// Mark if we found a TC
					hasTC = true;
				}
			}
		}
	}

	bool isFirstHouse = (!hasHouse && (stratElem->unitDAT_ID == CST_UNITID_HOUSE));
	bool isFarm = (stratElem->unitDAT_ID == CST_UNITID_FARM);

	// 2) Active loop on my buildings
	for (long int index = 0; index < infAI->buildingUnits.usedElements; index++) {
		short int unitId = (short int)infAI->buildingUnits.unitIdArray[index];
		AOE_STRUCTURES::STRUCT_UNIT_BUILDING *unit = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)globalStruct->ptrUnitPointersList[unitId];
		if (unit) {
			assert(unit->IsCheckSumValid()); // IS a building (no other unit type)
			AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *defUnit = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)unit->unitDefinition;
			if (defUnit) {
				assert(defUnit->IsCheckSumValid()); // IS a building definition (no other unit type)
				// Make sure buildings are not all side by side with no space: if 1 side is "all blocked", make sure we don't build on opposite side.
				// 2 consecutive buildings in a row will be possible, but not more.
				int unitX = (int)unit->positionX;
				int unitY = (int)unit->positionY;
				char protectOppositeSidesValue = 1; // a very low like value, tiles won't be used unless AI is forced to (really no other room for new buildings?)
				if (stratElem->unitDAT_ID == CST_UNITID_HOUSE) {
					// It seems AI only cares about DISABLED tiles when placing houses ? Setting to 0 or 1 won't prevent from building there
					// Too bad because this call's goal is not to disable, but to set a very low priority :-(
					protectOppositeSidesValue = -1;
				}
				mapInfosStruct->ProtectBuildingOppositeSides(unitX, unitY, defUnit->sizeRadiusY, protectOppositeSidesValue);

				// Storage buildings: protect (more) their neighborhood, they need to be easily accessible by gatherers (or new villagers)
				// Moreover, TC and granary need space to build farms nearby

				// substract this value to X/Y positions that are > to center position to fix rounding.
				int unitRadiusDecreaseForHighPositions = 1 - (((int)(defUnit->sizeRadiusY * 2)) % 2); // 0 for values like 1.5 or 2.5, 1 for round values
				bool loopUnitIsDropSite = false;
				long int protectDistance = 0;
				long int protectValueLargeZone = -8;
				long int protectValueSmallZone = -10;
				if (defUnit->DAT_ID1 == CST_UNITID_FORUM) {
					loopUnitIsDropSite = true;
					protectDistance = 10;
					protectValueLargeZone = -9;
					protectValueSmallZone = -18;
				}
				if (defUnit->DAT_ID1 == CST_UNITID_GRANARY) {
					loopUnitIsDropSite = true;
					protectDistance = 8;
					protectValueLargeZone = -6;
					protectValueSmallZone = -10;
				}
				if (defUnit->DAT_ID1 == CST_UNITID_STORAGE_PIT) {
					loopUnitIsDropSite = true;
					protectDistance = 5;
					protectValueLargeZone = -6;
					protectValueSmallZone = -10;
				}
				if (loopUnitIsDropSite && isFirstHouse) {
					// Be less restrictive for first house to get a faster start
					protectDistance = 5;
					protectValueLargeZone = -4;
					protectValueSmallZone = -8;
				}
				// Protect drop sites
				if (loopUnitIsDropSite && !isFarm) {
					mapInfosStruct->AddMapLikeValueRectangle(unitX - protectDistance, unitY - protectDistance,
						unitX + protectDistance - unitRadiusDecreaseForHighPositions, unitY + protectDistance - unitRadiusDecreaseForHighPositions,
						protectValueLargeZone);
					protectDistance = (protectDistance * 2) / 3;
					// Even more in near positions
					mapInfosStruct->AddMapLikeValueRectangle(unitX - protectDistance, unitY - protectDistance,
						unitX + protectDistance - unitRadiusDecreaseForHighPositions, unitY + protectDistance - unitRadiusDecreaseForHighPositions,
						protectValueSmallZone);
				}
				// Farms : we don't need to add a bonus near TC/granary, ROR code will do it (see ManageCityMapLikeValueFarmPlacement(...) !)
			}
		}
	}


#ifdef _DEBUG
	//mapInfosStruct->SetMapLikeValue(TC_x, TC_y, 0); // TEST - TO DO : REMOVE
	char oldtest[20000];
	strcpy_s(oldtest, test);
	test = DumpPosToTextBuffer(mapInfosStruct, TC_x, TC_y, 25); // TEST - TO DO : REMOVE
#endif
}



// Opens a custom dialog message (based on CloseProgramDialog) and pauses game execution (if running)
// Return true if OK, false if failed - Fails if another custom dialog (or quit game dialog) is already open
// Pauses the game if running (only if a dialog is successfully opened)
bool CustomRORCommand::OpenCustomDialogMessage(const char *dialogText, long int hSize, long int vSize) {
	if (CUSTOMROR::crInfo.customYesNoDialogVar) { return false; } // Already an opened custom dialog

	AOE_STRUCTURES::STRUCT_ANY_UI *customDialogPtr = AOE_GetScreenFromName(AOE_CONST_INTERNAL::customDialogScreenName);
	if (customDialogPtr != NULL) { return false; } // A CloseProgramDialog seems to be already open

	SetGamePause(true);
	CUSTOMROR::crInfo.customYesNoDialogVar = (unsigned long int*) AOE_CreateDialogPopup(dialogText, hSize, vSize);

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
	AOE_RefreshScreen("Game Screen", 0);
	assert(strcmp(ptrDialog->screenName, AOE_CONST_INTERNAL::customDialogScreenName) == 0);
	AOE_CloseScreenAndDestroy(AOE_CONST_INTERNAL::customDialogScreenName);
	CUSTOMROR::crInfo.customYesNoDialogVar = NULL;

	long int *Ihavenoideawhatthisis = (long int *)0x7C0338; // cf 0x481264
	Ihavenoideawhatthisis = 0;
	SetGamePause(false);

	return returnValue;
}


/* This is called on GameSettings.NotifyEvent game method call (00501980).
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
			this->ManageDisableUnitsForFarms(player);
		}
	}

	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_FARM_DEPLETED) {
		// This is called when a farm is fully depleted (and dies), NOT when a farm is destroyed/killed
		// Warning: when this event is called, the farm is killed just after, so be careful about modifications done here !
		long int farmUnitId = arg3;
		this->OnFarmDepleted(farmUnitId);
	}


	// Now manage triggers...
	this->ManageTriggersOnGameNotifyEvent(eventId, playerId, arg3, arg4, arg5);
}


// This is dedicated to triggers handling on "GameSettingsNotifyEvent" event.
void CustomRORCommand::ManageTriggersOnGameNotifyEvent(long int eventId, short int playerId, long int arg3, long int arg4, long int arg5) {
	CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
	CR_TRIGGERS::TRIGGER_EVENT_TYPES evtType = CR_TRIGGERS::EVENT_NONE;
	memset(&evtInfo, -1, sizeof(evtInfo));
	evtInfo.playerId = playerId;

	// Manage triggers
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_RESEARCH_COMPLETE) {
		evtInfo.researchId = arg3;
		evtInfo.posX = arg5;
		evtInfo.posY = arg4;
		evtType = CR_TRIGGERS::EVENT_RESEARCH_COMPLETED;
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_BUILDING_COMPLETE) {
		evtInfo.unitDefId = arg3;
		evtInfo.posX = arg5;
		evtInfo.posY = arg4;
	}
	/*if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_ALL_RELICS_CAPTURED) {
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_ALL_RELICS_LOST_CONTROL) {
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_ALL_RUINS_CAPTURED) {
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_ALL_RUINS_LOST_CONTROL) {
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_PLAYER_LOST) {
	}*/
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_UNIT_SPAWNED) {
		evtInfo.unitDefId = arg3;
		evtInfo.posX = arg5;
		evtInfo.posY = arg4;
	}
	// Warning, for all 3 "wonder" events, position is arg3/4, not 4/5
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_WONDER_DESTROYED) {
		evtInfo.posX = arg4;
		evtInfo.posY = arg3;
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_WONDER_FINISHED) {
		evtInfo.posX = arg4;
		evtInfo.posY = arg3;
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_WONDER_START_BUILDING) {
		evtInfo.posX = arg4;
		evtInfo.posY = arg3;
	}
	if (evtType != CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_NONE) {
		this->ExecuteTriggersForEvent(evtType, evtInfo);
	}
}


// Entry point to make custom treatments at "disable research" init at game start (for scenarios)
// This is only executed for scenarios, not DM/RM !
void CustomRORCommand::OnGameInitDisableResearchesEvent(AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *playerResearchInfo) {
	assert(playerResearchInfo != NULL);
	if (!playerResearchInfo) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = playerResearchInfo->ptrPlayer;
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return; }
	// Note: get global structure from player, because global variable might not be set yet.
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = player->GetGlobalStruct();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }

	// For current player, disable the units we have from trigger data.
	std::vector<short int>::const_iterator it;
	for (it = CUSTOMROR::crInfo.unitDefToDisable[player->playerId].begin(); it != CUSTOMROR::crInfo.unitDefToDisable[player->playerId].end(); it++) {
		short int DAT_ID = *it;
		if (DisableUnitForPlayer(player, DAT_ID)) {
			// success
		} else {
			// Exceptions: villager, house, farm, fishing ship, trade ship. DisableUnitForPlayer doesn't support them because of techs that do several things at once.
			// Those exceptions are manage in another location
		}
	}

	// Disable researches from trigger data
	for (it = CUSTOMROR::crInfo.researchesToDisable[player->playerId].begin(); it != CUSTOMROR::crInfo.researchesToDisable[player->playerId].end(); it++) {
		short int researchId = *it;
		AOE_enableResearch(player, researchId, false);
	}
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

	if (settings->isScenario != 0) {
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
					CALL addr // player.applyStartingAgeToResearches?(researchIdResourceId)
				}
			}
		}
	}

}


// Manage disable (via trigger) units for villager, house, fishing ship, trade ship
// This method should be called at game start, after "initial" technologies have been applied, so we can override some (and force to disable again some units)
void CustomRORCommand::ManageTriggerDisableUnitsForExceptions() {
	for (int playerId = 0; playerId < 9; playerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		if (player) {
			std::vector<short int>::const_iterator it;
			for (it = CUSTOMROR::crInfo.unitDefToDisable[player->playerId].begin(); it != CUSTOMROR::crInfo.unitDefToDisable[player->playerId].end(); it++) {
				short int DAT_ID = *it;
				if (
					(DAT_ID == CST_UNITID_FARM) || // See also ManageDisableUnitsForFarms - it can be enabled during the game
					// All 4 other units are ALWAYS enabled at game initialization (they have no pre-requisite)
					// so this method (called at startup) is always sufficient (those units won't be enabled again)
					(DAT_ID == CST_UNITID_HOUSE) ||
					(DAT_ID == CST_UNITID_VILLAGER) ||
					(DAT_ID == CST_UNITID_FISHING_BOAT) ||
					(DAT_ID == CST_UNITID_TRADE_BOAT)
					) {
					if ((DAT_ID < 0) || (DAT_ID >= player->structDefUnitArraySize)) { return; }
					AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)player->ptrStructDefUnitTable[DAT_ID];
					unitDef->availableForPlayer = 0;
				}
			}
		}
	}
}

// Manage disable (via trigger) units for farms.
// Farm is enabled by market construction, which may occur both "ingame" or at game loading (if a market exists, or if starting at bronze).
void CustomRORCommand::ManageDisableUnitsForFarms(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	std::vector<short int>::const_iterator it;
	for (it = CUSTOMROR::crInfo.unitDefToDisable[player->playerId].begin(); it != CUSTOMROR::crInfo.unitDefToDisable[player->playerId].end(); it++) {
		short int DAT_ID = *it;
		if (DAT_ID == CST_UNITID_FARM) {
			if ((DAT_ID < 0) || (DAT_ID >= player->structDefUnitArraySize)) { return; }
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)player->ptrStructDefUnitTable[DAT_ID];
			unitDef->availableForPlayer = 0;
			return;
		}
	}
}

void CustomRORCommand::ReloadTriggersFromGameData(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scenarioInfo) {
	if (CUSTOMROR::crInfo.triggerSet) {
		delete CUSTOMROR::crInfo.triggerSet;
		CUSTOMROR::crInfo.triggerSet = NULL;
	}
	CUSTOMROR::crInfo.triggerSet = new CR_TRIGGERS::crTriggerSet();
	if (!scenarioInfo || (!scenarioInfo->IsCheckSumValid())) { return; }

	CUSTOMROR::crInfo.triggerSet->ReadFromFileContent(GetTriggerDataPtr(scenarioInfo));
}

void CustomRORCommand::ReloadTriggersFromGameData() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	if (!global->scenarioInformation) { return; }
	this->ReloadTriggersFromGameData(global->scenarioInformation);
}


// Write trigger to game data, using customROR internal data.
bool CustomRORCommand::WriteTriggersFromInternalToGameData(bool onlyEnabledTriggers) {
	if (!CUSTOMROR::crInfo.triggerSet) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->scenarioInformation) { return false; }
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *sc_info = global->scenarioInformation; // Guaranteed non-NULL by previous "if"
	assert(sc_info != NULL);
	if (!sc_info) { return false; }
	std::string tmpStr = CUSTOMROR::crInfo.triggerSet->GetStringForScenarioInfo(onlyEnabledTriggers);
	char *trgData = GetTriggerDataPtr(sc_info);

	// Free previous value, if any
	if (trgData != NULL) {
		AOEFree(trgData);
		SetTriggerDataSize(sc_info, 0);
	}
	// Write triggers data in scenario information
	return this->WriteTriggersInGameData((char *)tmpStr.c_str());
}


// Write trigger to game data, using provided text trigger data.
bool CustomRORCommand::WriteTriggersInGameData(char *triggerText) {
	if (!triggerText) { triggerText = ""; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->scenarioInformation) { return false; }
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *sc_info = global->scenarioInformation; // Guaranteed non-NULL by previous "if"
	assert(sc_info != NULL);
	if (sc_info == NULL) { return false; }

	bool hasEndTag = TriggerTextContainsENDTagAtBeginning(triggerText);

	long int size = strlen(triggerText); // can be 0.
	long int sizeToAlloc = size + 1;
	if (!hasEndTag) { sizeToAlloc += SpecialKeyword_getLength(CR_TRIGGERS::KW_END_LINE); }
	char *strToSave = (char *)AOEAlloc(sizeToAlloc);
	if (!hasEndTag) {
		strcpy_s(strToSave, SpecialKeyword_getLength(CR_TRIGGERS::KW_END_LINE) + 1, GetTriggerSpecialKeyword(CR_TRIGGERS::KW_END_LINE)); // +1 for (temporary) \0
		strcpy_s(strToSave + SpecialKeyword_getLength(CR_TRIGGERS::KW_END_LINE), size + 1, triggerText); // Concatenate (overwrite "previous" ending \0)
	} else {
		strcpy_s(strToSave, sizeToAlloc, triggerText);
	}
	SetTriggerDataSize(sc_info, sizeToAlloc);
	SetTriggerDataPtr(sc_info, strToSave);
	return true;
}


// Export CustomROR's internal data triggers to Trigger text format.
std::string CustomRORCommand::GetTriggerTextFromInternalData() {
	if (!CUSTOMROR::crInfo.triggerSet) { return ""; }
	return CUSTOMROR::crInfo.triggerSet->GetStringForScenarioInfo(false);
}


// Manage triggers execution for a specific event
void CustomRORCommand::ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES evt) {
	CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
	memset(&evtInfo, -1, sizeof(evtInfo));
	this->ExecuteTriggersForEvent(evt, evtInfo);
}

// Manage triggers execution for a specific event
void CustomRORCommand::ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES evt, CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER &evtInfo) {
	// Make sure we never execute trigger when game is not running
	// Also disabled in multiplayer games.
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) || settings->isMultiplayer) {
		return;
	}

	// Make sure difficulty level is always set
	if (evtInfo.difficultyLevel < 0) {
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		assert(settings);
		if (!settings) { return; }
		evtInfo.difficultyLevel = settings->difficultyLevel;
	}
	// Make sure current game time is always set
	if (evtInfo.currentGameTime_s < 0) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		assert(global);
		if (!global) { return; }
		evtInfo.currentGameTime_s = global->currentGameTime / 1000;
	}
	// Loop on triggers and execute those that are ready
	if (CUSTOMROR::crInfo.triggerSet && (!CUSTOMROR::crInfo.triggerSet->IsEmpty())) {
		CR_TRIGGERS::crTrigger *currentTrigger = CUSTOMROR::crInfo.triggerSet->IteratorFirst();
		while (currentTrigger) {
			if (currentTrigger->CanExecuteNow(evt, evtInfo)) {
				this->ExecuteTriggerAction(currentTrigger);
			}
			currentTrigger = CUSTOMROR::crInfo.triggerSet->IteratorNext();
		}
	}
}


// Just execute a "standard" trigger. this only does the action part, does no check and does not manage other impacts.
// Do not call this directly ! (private)
void CustomRORCommand::Trigger_JustDoAction(CR_TRIGGERS::crTrigger *trigger) {
	if (!trigger) { return; }
	if (!trigger->IsValid()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global != NULL);
	if (!global) { return; }
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_WRITE_CHAT) {
		CallWriteText(trigger->GetParameterValue(CR_TRIGGERS::KW_MESSAGE, ""));
	}
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_WRITE_CENTERED_MESSAGE) {
		CallWriteCenteredText(trigger->GetParameterValue(CR_TRIGGERS::KW_MESSAGE, ""));
	}
	long int actionPlayerId = -1;
	long int actionResourceId = -1;
	float actionResourceValue = 0;
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_RESOURCE) {
		// Not MP-compatible (to confirm)
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		actionResourceId = trigger->GetParameterValue(CR_TRIGGERS::KW_RESOURCE_ID, -1);
		actionResourceValue = trigger->GetParameterValue(CR_TRIGGERS::KW_RESOURCE_VALUE, 0.0);
		if ((actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount)) { return; }
		if ((actionResourceId < 0) || (actionResourceId > AOE_CONST_FUNC::CST_RES_ORDER_ALL_RELICS)) { return; }
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (player) {
			player->SetResourceValue((AOE_CONST_FUNC::RESOURCE_TYPES)actionResourceId, actionResourceValue);
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ADD_RESOURCE) {
		// Not MP-compatible (to confirm)
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		actionResourceId = trigger->GetParameterValue(CR_TRIGGERS::KW_RESOURCE_ID, -1);
		actionResourceValue = trigger->GetParameterValue(CR_TRIGGERS::KW_RESOURCE_VALUE, 0.0);
		if ((actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount)) { return; }
		if ((actionResourceId < 0) || (actionResourceId > AOE_CONST_FUNC::CST_RES_ORDER_ALL_RELICS)) { return; }
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (player) {
			float oldvalue = player->GetResourceValue((AOE_CONST_FUNC::RESOURCE_TYPES)actionResourceId);
			player->SetResourceValue((AOE_CONST_FUNC::RESOURCE_TYPES)actionResourceId, oldvalue + actionResourceValue);
		}
	}

	if ((trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_DISABLE_RESEARCH) ||
		(trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ENABLE_RESEARCH) ||
		(trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_APPLY_RESEARCH)
		) {
		// Not MP-compatible (to confirm)
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		long int actionResearchId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_RESEARCH_ID, -1);
		// This feature is not available for invalid players, nor for gaia player.
		if ((actionPlayerId <= 0) || (actionPlayerId >= global->playerTotalCount) || (actionResearchId < 0)) { return; }
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (player) {
			AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *pr = player->ptrResearchesStruct;
			if (!pr || !pr->researchStatusesArray || (pr->researchCount <= actionResearchId)) { return; }
			if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_DISABLE_RESEARCH) {
				// We allow to disable researches only if: they are not being researched and not already researched.
				if (pr->researchStatusesArray[actionResearchId].currentStatus < AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_BEING_RESEARCHED) {
					pr->researchStatusesArray[actionResearchId].currentStatus = AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED;
				}
			}
			if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ENABLE_RESEARCH) {
				// We allow to enable researches only if: they are disabled or not available yet (waiting for requirement)
				if (pr->researchStatusesArray[actionResearchId].currentStatus < AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE) {
					pr->researchStatusesArray[actionResearchId].currentStatus = AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE;
				}
			}
			if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_APPLY_RESEARCH) {
				// We allow to "apply" researches only if: they are not being researched and not already researched.
				if (pr->researchStatusesArray[actionResearchId].currentStatus < AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_BEING_RESEARCHED) {
					ApplyResearchForPlayer(player, actionResearchId);
				}
			}
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_APPLY_TECH) {
		// Not MP-compatible (to confirm)
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		long int actionTechId = trigger->GetParameterValue(CR_TRIGGERS::KW_TECHNOLOGY_ID, -1);
		if ((actionPlayerId >= global->playerTotalCount) || (actionTechId < 0)) { return; }
		if (actionPlayerId < -1) { return; } // invalid and not a joker (-1)
		// Set min and max playerid: by default they are the same (apply on only 1 player)
		long int minPlayerId = actionPlayerId;
		long int maxPlayerId = actionPlayerId;
		if (actionPlayerId == -1) { // -1 is a joker: apply on all players
			minPlayerId = 0;
			maxPlayerId = global->playerTotalCount - 1;
		}
		for (int i = minPlayerId; i <= maxPlayerId; i++) { // This for does only 1 loop in standard case (if NOT using a joker)
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(i);
			if (player) {
				ApplyTechnologyForPlayer(player, actionTechId);
			}
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_DIPL_CHANGE) {
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		long int targetPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_TARGET_PLAYER_ID, -1);
		long int diplomacyValue = trigger->GetParameterValue(CR_TRIGGERS::KW_DIPLOMACY_VALUE, -1);
		long int isMutual = trigger->GetParameterValue(CR_TRIGGERS::KW_MUTUAL, 0); // default = false (0)
		if ((actionPlayerId >= global->playerTotalCount) || (actionPlayerId < 0)) { return; }
		if ((targetPlayerId >= global->playerTotalCount) || (targetPlayerId < 0)) { return; }
		if (actionPlayerId == targetPlayerId) { return; }
		if ((diplomacyValue < 0) || (diplomacyValue > 3)) { return; }
		if (diplomacyValue == 2) { diplomacyValue = 3; } // 2 is unused, let's use it as "enemy" like 3.
		CreateCmd_ChangeDiplomacy((short int)actionPlayerId, (short int)targetPlayerId, (PLAYER_DIPLOMACY_STANCES)diplomacyValue);
		if (isMutual) {
			CreateCmd_ChangeDiplomacy((short int)targetPlayerId, (short int)actionPlayerId, (PLAYER_DIPLOMACY_STANCES)diplomacyValue);
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_DISABLE_TRIGGER) {
		if (CUSTOMROR::crInfo.triggerSet) {
			char *targetTriggerName = trigger->GetParameterValue(CR_TRIGGERS::KW_TARGET_TRIGGER_NAME, "");
			CR_TRIGGERS::crTrigger *otherTrigger = CUSTOMROR::crInfo.triggerSet->GetTrigger(targetTriggerName);
			if (otherTrigger) {
				otherTrigger->enabled = false;
			}
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_TERRAIN) {
		// Not MP-compatible (to confirm)
		long int minPosX, minPosY, maxPosX, maxPosY, elevationLevel, terrainId;
		minPosX = trigger->GetParameterValue(CR_TRIGGERS::KW_MIN_POS_X, -1);
		minPosY = trigger->GetParameterValue(CR_TRIGGERS::KW_MIN_POS_Y, -1);
		maxPosX = trigger->GetParameterValue(CR_TRIGGERS::KW_MAX_POS_X, -1);
		maxPosY = trigger->GetParameterValue(CR_TRIGGERS::KW_MAX_POS_Y, -1);
		elevationLevel = trigger->GetParameterValue(CR_TRIGGERS::KW_ELEVATION_LEVEL, -1);
		terrainId = trigger->GetParameterValue(CR_TRIGGERS::KW_TERRAIN_ID, -1);
		// Test invalid coordinates or missing information cases
		if ((minPosX < 0) || (minPosY < 0) || (minPosX > 255) || (minPosY > 255) ||
			(maxPosX < 0) || (maxPosY < 0) || (maxPosX > 255) || (maxPosY > 255) ||
			(minPosX > maxPosX) || (minPosY > maxPosY) ||
			((elevationLevel < 0) && (terrainId) < 0)) {
			return;
		}
		AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		assert(gameMapInfo && gameMapInfo->IsCheckSumValid());
		if (!gameMapInfo || !gameMapInfo->IsCheckSumValid()) { return; }
		if ((minPosX >= gameMapInfo->mapArraySizeX) || (minPosY >= gameMapInfo->mapArraySizeY) ||
			(maxPosX >= gameMapInfo->mapArraySizeX) || (maxPosX >= gameMapInfo->mapArraySizeX)
			) { return; }
		for (int x = minPosX; x <= maxPosX; x++) {
			for (int y = minPosY; y <= maxPosY; y++) {
				AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO *tile = gameMapInfo->GetTileInfo(x, y);
				if (tile) {
					// Note: tile's "Set" methods are both secure (check the value is in bounds)
					if (elevationLevel >= 0) { tile->terrainData.SetAltitude((char)elevationLevel); } // TO DO: this does not work well in-game + side effects
					if (terrainId >= 0) { tile->terrainData.SetTerrainId((char)terrainId); } // Does not update minimap, does not refresh map immediatly
				}
			}
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_MODIFY_UNIT) {
		// Not MP-compatible (to confirm)
		long int actionUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_ID, -1);
		if (actionUnitId < 0) { return; }
		float unitHP_add = trigger->GetParameterValue(CR_TRIGGERS::KW_ADD_HP, (float)0);
		float unitHP_set = trigger->GetParameterValue(CR_TRIGGERS::KW_SET_HP, (float)-1);
		float resourceAmount_add = trigger->GetParameterValue(CR_TRIGGERS::KW_ADD_RESOURCE, (float)0);
		float resourceAmount_set = trigger->GetParameterValue(CR_TRIGGERS::KW_SET_RESOURCE, (float)-1);
		char orientation = (char) trigger->GetParameterValue(CR_TRIGGERS::KW_UNIT_ORIENTATION_INDEX, -1);
		char graphicsAge = (char) trigger->GetParameterValue(CR_TRIGGERS::KW_UNIT_GRAPHICS_AGE, -1);
		float posX = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_X, (float) -1);
		float posY = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_Y, (float)-1);
		long int newOwnerId = trigger->GetParameterValue(CR_TRIGGERS::KW_OWNER_PLAYER_ID, -1);

		AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(actionUnitId);
		if (!targetUnit || !targetUnit->IsCheckSumValidForAUnitClass()) { return; }
		if (unitHP_set >= 0) {
			targetUnit->remainingHitPoints = unitHP_set;
		}
		if (unitHP_add != 0) {
			targetUnit->remainingHitPoints += unitHP_add;
		}
		if (resourceAmount_set >= 0) {
			targetUnit->resourceValue = resourceAmount_set;
		}
		if (resourceAmount_add != 0) {
			targetUnit->resourceValue += resourceAmount_add;
		}
		//if ((graphicsAge >= 0) && (graphicsAge <= 2)) { // TO DO (does not work) => use orientation ?
			//targetUnit->currentGraphicsDamageIndex = graphicsAge;
		//}
		if ((orientation >= 0) && (orientation <= 7)) {
			targetUnit->orientationIndex = orientation;
		}
		if ((posX >= 0) && (posY >= 0)) { // This has important side effect, not recommended !!! TO DO.
			AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
			if ((posX < gameMapInfo->mapArraySizeX) && (posY < gameMapInfo->mapArraySizeY)) {
				targetUnit->positionX = posX;
				targetUnit->positionY = posY;
			}
		}
		// Last but not least: change unit owner !
		if ((newOwnerId >= 0) && (newOwnerId < global->playerTotalCount)) {
			this->ChangeUnitOwner(targetUnit, GetPlayerStruct(newOwnerId));
		}
	}

	// Spawn a unit
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ADD_UNIT_INSTANCE) {
		// Not MP-compatible (to confirm)
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -2);
		long int actionUnitDefId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_DEF_ID, -1);
		float posX = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_X, (float)-1);
		float posY = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_Y, (float)-1);
		if ((actionUnitDefId < 0) || (actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount) || (posX < 0) || (posY < 0)) {
			return;
		}
		AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		assert(gameMapInfo && gameMapInfo->IsCheckSumValid());
		if ((posX >= gameMapInfo->mapArraySizeX) || (posY >= gameMapInfo->mapArraySizeY)) { return; }
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (!player || !player->IsCheckSumValid()) { return; }
		if (actionUnitDefId >= player->structDefUnitArraySize) { return; }
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = player->ptrStructDefUnitTable[actionUnitDefId];
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) { return; }
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = CheckAndCreateUnit(player, unitDef, posX, posY, false, true, true);
	}

	// Make unique (create dedicated unit definition)
	// Only supported for living units and buildings.
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_MAKE_UNIQUE) {
		// Not MP-compatible (to confirm)
		long int actionUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_ID, -1);
		if (actionUnitId < 0) { return; }
		char *newUnitName = trigger->GetParameterValue(CR_TRIGGERS::KW_UNIT_NAME, "");
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(actionUnitId);
		if (!unitBase || !unitBase->IsCheckSumValidForAUnitClass()) { return; }
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unitBase->unitDefinition;
		if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) { return; }
		// Only living units & buildings are allowed
		if (!unitDefBase->DerivesFromTrainable()) {
			traceMessageHandler.WriteMessage("ERROR: 'Make unique' triggers only support living units and buildings.");
			return;
		}
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDefLiving = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unitDefBase;
		if (!unitDefLiving || !unitDefLiving->DerivesFromTrainable()) { return; }

		AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unitLiving = (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *)unitBase;
		if (!unitLiving || !unitLiving->DerivesFromTrainable()) { return; }

		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefToFree = NULL; // Only used for units that have a temporary unitDef (converted units)
		if (unitLiving->hasDedicatedUnitDef) {
			unitDefToFree = unitDefBase; // previous unitDef was (also) a temporary one. We will free it.
		}

		// Save HP % before modification to preserve it (example: if maxHP changes from 20 to 40, if old unit had 10/20 HP, new one should have 20/40 !)
		float HPProportion = unitLiving->remainingHitPoints / ((float)unitDefLiving->totalHitPoints);

		// Create the new (temporary) unit definition.
		AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *newUnitDefBuilding = NULL;
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *newUnitDefLiving = NULL;
		if (unitLiving->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) {
			// Our unit is a building
			//unitDefBuilding = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)unitDefBase;
			assert(((AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)unitDefBase)->IsCheckSumValid());
			newUnitDefBuilding = CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING>((AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)unitDefBase);
			newUnitDefLiving = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE*)newUnitDefBuilding;
		} else {
			// We actually have a living unit
			newUnitDefLiving = CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE>((AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unitDefBase);
		}

		// Here newUnitDefLiving is ALWAYS our new unitDef (cast as living=parent class if needed)
		// newUnitDefBuilding is only non-NULL if our unit is actually a building. It can be used if we modify building-specific attributes.

		// Apply supplied properties to new unit def.
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_TOTAL_HP)) {
			newUnitDefLiving->totalHitPoints = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_TOTAL_HP, 1);
			// Fix actual Hit points
			unitLiving->remainingHitPoints = newUnitDefLiving->totalHitPoints * HPProportion;
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_RANGE)) {
			float suppliedRange = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_RANGE, -1);
			if (suppliedRange >= 0) {
				newUnitDefLiving->lineOfSight = suppliedRange;
				float upgradeRelatedRange = newUnitDefLiving->maxRange - newUnitDefLiving->displayedRange;
				if (upgradeRelatedRange < 0) { upgradeRelatedRange = 0; }
				newUnitDefLiving->displayedRange = suppliedRange - upgradeRelatedRange; // the figure before "+". For a "7+1" range, this corresponds to 7.
				newUnitDefLiving->maxRange = suppliedRange; // Total range. For a "7+1" range, this corresponds to 8.
			}
		}

		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_SPEED)) {
			newUnitDefLiving->speed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_SPEED, newUnitDefLiving->speed);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_ROTATION_SPEED)) {
			newUnitDefLiving->rotationSpeed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_ROTATION_SPEED, newUnitDefLiving->rotationSpeed);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_WORK_RATE)) {
			newUnitDefLiving->workRate = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_WORK_RATE, newUnitDefLiving->workRate);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_ACCURACY_PERCENT)) {
			newUnitDefLiving->accuracyPercent = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_ACCURACY_PERCENT, (long int)newUnitDefLiving->accuracyPercent);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_DISPLAYED_ARMOR)) {
			newUnitDefLiving->displayedArmor = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_DISPLAYED_ARMOR, (long int)newUnitDefLiving->displayedArmor);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_DISPLAYED_ATTACK)) {
			newUnitDefLiving->displayedAttack = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_DISPLAYED_ATTACK, (long int)newUnitDefLiving->displayedAttack);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_VISIBLE_IN_FOG)) {
			newUnitDefLiving->visibleInFog = (char)trigger->GetParameterValue(CR_TRIGGERS::KW_VISIBLE_IN_FOG, (long int)newUnitDefLiving->visibleInFog);
			if (newUnitDefLiving->visibleInFog != 0) {
				newUnitDefLiving->visibleInFog = 1; // just in case.
			}
		}

		// Now associate new unit definition to our unit
		unitLiving->unitDefinition = newUnitDefLiving;
		unitLiving->hasDedicatedUnitDef = 1; // don't forget to set this flag so that ROR correctly frees everything later.
		// Force use provided name (if provided)
		if (*newUnitName != 0) {
			if (newUnitDefLiving->ptrUnitName) {
				AOEFree(newUnitDefLiving->ptrUnitName);
			}
			char *newAllocatedName = (char *)AOEAlloc(0x30);
			strcpy_s(newAllocatedName, 0x30, newUnitName);
			newUnitDefLiving->ptrUnitName = newAllocatedName;
			newUnitDefLiving->languageDLLID_Name = 0;
			newUnitDefLiving->languageDLLID_Creation = 0;
		}

		// Free previous unitdef if unit already had hasDedicatedUnitDef==1
		if (unitDefToFree) {
			AOEFree(unitDefToFree);
		}
	}

	// A big one: create unit def (actually, duplicate + customize)
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ADD_UNIT_DEF) {
		//actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		long int sourceUnitDefId = trigger->GetParameterValue(CR_TRIGGERS::KW_FROM_UNIT_DEF_ID, -2);
		char *newUnitName = trigger->GetParameterValue(CR_TRIGGERS::KW_UNIT_NAME, "");
		if ((sourceUnitDefId < 0) /*|| (actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount)*/) {
			return;
		}

		// A first loop to run checks
		int commonArraySize = -1;
		for (int loopPlayerId = 0; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(loopPlayerId);
			if (!player || !player->IsCheckSumValid()) { return; }
			// Make sure that for all players source UnitDefId exists
			if (sourceUnitDefId >= player->structDefUnitArraySize) { return; }
			// Make sure source unitDef exists for all players (remember: it can exist and be disabled, like gaia units for other civs)
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *sourceUnitDef = player->ptrStructDefUnitTable[sourceUnitDefId];
			if (!sourceUnitDef || !sourceUnitDef->IsCheckSumValidForAUnitClass()) { return; }
			// Make sure maximum unitDefId is the same for ally players (synchronized)
			if (commonArraySize == -1) {
				commonArraySize = player->structDefUnitArraySize; // first loop: initialize.
			} else {
				// after first loop: make sure we always get the same value
				if (player->structDefUnitArraySize != commonArraySize) {
					traceMessageHandler.WriteMessage("Player unit definitions are not synchronized for all players, cannot add.");
					return;
				}
			}
		}
		assert(commonArraySize >= 0);
		if (commonArraySize < 0) {
			traceMessageHandler.WriteMessage("ERROR: commonArraySize<0. How is this possible ?");
			return;
		}
		int newUnitDefId = commonArraySize;

		// A second loop to do the job
		for (int loopPlayerId = 0; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(loopPlayerId);
			assert(player && player->IsCheckSumValid()); // should not happen, already checked
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *sourceUnitDefBase = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE*) player->ptrStructDefUnitTable[sourceUnitDefId];
			assert(sourceUnitDefBase && sourceUnitDefBase->IsCheckSumValidForAUnitClass()); // should not happen, already checked
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *newUnitDefBase = CopyUnitDefToNewUsingGoodClass(sourceUnitDefBase);
			if (!newUnitDefBase) {
				traceMessageHandler.WriteMessage("ERROR: Failed to copy unit definition");
				return;
			}
			assert(newUnitDefId == player->structDefUnitArraySize); // Note: structDefUnitArraySize has not been updated yet
			assert(newUnitDefBase && newUnitDefBase->IsCheckSumValidForAUnitClass());
			assert(newUnitDefBase->unitType == sourceUnitDefBase->unitType);
			// Add in player unit definition table
			short int tmpAddedUnitDefId = AddUnitDefToPlayer(player, newUnitDefBase);
			assert(newUnitDefId == player->structDefUnitArraySize - 1); // Note: structDefUnitArraySize has been updated now
			if (tmpAddedUnitDefId == -1) {
				assert(false);
				traceMessageHandler.WriteMessage("An error occurred while adding new unit definition");
				return;
			}
			if (newUnitDefId != tmpAddedUnitDefId) {
				assert(false);
				traceMessageHandler.WriteMessage("Inconsistent unitDefID while adding new unit definition");
				return;
			}

			// Force use provided name (if provided)
			if (*newUnitName != 0) {
				if (newUnitDefBase->ptrUnitName) {
					AOEFree(newUnitDefBase->ptrUnitName);
				}
				char *newAllocatedName = (char *)AOEAlloc(0x30);
				strcpy_s(newAllocatedName, 0x30, newUnitName);
				newUnitDefBase->ptrUnitName = newAllocatedName;
				newUnitDefBase->languageDLLID_Name = 0;
				newUnitDefBase->languageDLLID_Creation = 0;
			}

			// Apply custom modifications from trigger data
			// We manipulate here objects from different possible classes. Make sure we work on members that DO exist in our object instance !
			bool isType50OrChild = newUnitDefBase->DerivesFromAttackable();
			bool isFlagOrChild = newUnitDefBase->DerivesFromFlag();
			bool isMovableOrChild = newUnitDefBase->DerivesFromMovable();
			bool isCommandableOrChild = newUnitDefBase->DerivesFromCommandable();
			AOE_STRUCTURES::STRUCT_UNITDEF_FLAG *newUnitDef_flag = NULL;
			AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE *newUnitDef_movable = NULL;
			AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *newUnitDef_commandable = NULL;
			AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *newUnitDef_type50 = NULL;
			if (isFlagOrChild) { newUnitDef_flag = (AOE_STRUCTURES::STRUCT_UNITDEF_FLAG *)newUnitDefBase; }
			if (isMovableOrChild) { newUnitDef_movable = (AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE *)newUnitDefBase; }
			if (isCommandableOrChild) { newUnitDef_commandable = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)newUnitDefBase; }
			if (isType50OrChild) { newUnitDef_type50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)newUnitDefBase; }

			// Apply supplied properties to new unit def.
			if (trigger->IsParameterDefined(CR_TRIGGERS::KW_TOTAL_HP)) {
				newUnitDefBase->totalHitPoints = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_TOTAL_HP, 1);
			}
			if (trigger->IsParameterDefined(CR_TRIGGERS::KW_RANGE)) {
				float suppliedRange = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_RANGE, -1);
				if (suppliedRange >= 0) {
					newUnitDefBase->lineOfSight = suppliedRange;
					if (isType50OrChild && newUnitDef_type50) {
						float upgradeRelatedRange = newUnitDef_type50->maxRange - newUnitDef_type50->displayedRange;
						if (upgradeRelatedRange < 0) { upgradeRelatedRange = 0; }
						newUnitDef_type50->displayedRange = suppliedRange - upgradeRelatedRange; // the figure before "+". For a "7+1" range, this corresponds to 7.
						newUnitDef_type50->maxRange = suppliedRange; // Total range. For a "7+1" range, this corresponds to 8.
					}
				}
			}

			if (isFlagOrChild && newUnitDef_flag && (trigger->IsParameterDefined(CR_TRIGGERS::KW_SPEED))) {
				newUnitDef_flag->speed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_SPEED, newUnitDef_flag->speed);
			}
			if (isMovableOrChild && newUnitDef_movable && (trigger->IsParameterDefined(CR_TRIGGERS::KW_ROTATION_SPEED))) {
				newUnitDef_movable->rotationSpeed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_ROTATION_SPEED, newUnitDef_movable->rotationSpeed);
			}
			if (isCommandableOrChild && newUnitDef_commandable && (trigger->IsParameterDefined(CR_TRIGGERS::KW_WORK_RATE))) {
				newUnitDef_commandable->workRate = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_WORK_RATE, newUnitDef_commandable->workRate);
			}
			if (isType50OrChild && newUnitDef_type50 && (trigger->IsParameterDefined(CR_TRIGGERS::KW_ACCURACY_PERCENT))) {
				newUnitDef_type50->accuracyPercent = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_ACCURACY_PERCENT, (long int)newUnitDef_type50->accuracyPercent);
			}
			if (isType50OrChild && newUnitDef_type50 && (trigger->IsParameterDefined(CR_TRIGGERS::KW_DISPLAYED_ARMOR))) {
				newUnitDef_type50->displayedArmor = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_DISPLAYED_ARMOR, (long int)newUnitDef_type50->displayedArmor);
			}
			if (isType50OrChild && newUnitDef_type50 && (trigger->IsParameterDefined(CR_TRIGGERS::KW_DISPLAYED_ATTACK))) {
				newUnitDef_type50->displayedAttack = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_DISPLAYED_ATTACK, (long int)newUnitDef_type50->displayedAttack);
			}
			if (trigger->IsParameterDefined(CR_TRIGGERS::KW_VISIBLE_IN_FOG)) {
				newUnitDefBase->visibleInFog = (char)trigger->GetParameterValue(CR_TRIGGERS::KW_VISIBLE_IN_FOG, (long int)newUnitDefBase->visibleInFog);
				if (newUnitDefBase->visibleInFog != 0) {
					newUnitDefBase->visibleInFog = 1; // just in case.
				}
			}
		}

		// Update other triggers with new unitDefId
		CR_TRIGGERS::crTrigger *currentTrg = CUSTOMROR::crInfo.triggerSet->IteratorFirst();
		while (currentTrg) {
			// Search for enabled triggers for same player, that want to spawn a unit...
			if (currentTrg->enabled && (currentTrg->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ADD_UNIT_INSTANCE)) {
				// Find if the spawn unit action uses "unit_name" as reference
				char *otherTrgUnitName = currentTrg->GetParameterValue(CR_TRIGGERS::KW_UNIT_NAME, "");
				if (strcmp(newUnitName, otherTrgUnitName) == 0) {
					// Found a trigger which is based on the unit name we just created: set the unitDefId value.
					currentTrg->SetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_DEF_ID, newUnitDefId);
				}
			}
			currentTrg = CUSTOMROR::crInfo.triggerSet->IteratorNext();
		}

		trigger->enabled = false; // Creating unit def is not repeatable. Force disable.
	}

	// Give a command to a unit
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_UNIT_TARGET) {
		long int actionUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_ID, -1);
		long int targetUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_TARGET_UNIT_ID, -1);
		if ((actionUnitId < 0) || (targetUnitId < 0)) { return; }
		TellUnitToInteractWithTarget((AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE*)GetUnitStruct(actionUnitId), (AOE_STRUCTURES::STRUCT_UNIT_BASE*)GetUnitStruct(targetUnitId));
	}

	// Move a unit
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_UNIT_DESTINATION) {
		// Not MP-compatible (to confirm)
		long int actionUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_ID, -1);
		float posX = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_X, (float)-1);
		float posY = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_Y, (float)-1);
		if ((actionUnitId < 0) || (posX < 0) || (posY < 0)) {
			return;
		}
		AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		assert(gameMapInfo && gameMapInfo->IsCheckSumValid());
		if ((posX >= gameMapInfo->mapArraySizeX) || (posY >= gameMapInfo->mapArraySizeY)) { return; }
		MoveUnitToTargetOrPosition((AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE*)GetUnitStruct(actionUnitId), NULL, posX, posY);
	}

	// Make gaia units human-capturable (or not)
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_GAIA_CAPTURABLE) {
		// Not MP-compatible (to confirm)
		int valueToSet = trigger->GetParameterValue(CR_TRIGGERS::KW_ENABLE, -1);
		if ((valueToSet < 0) || (valueToSet > 1)) { // only 0 and 1 are accepted
			return;
		}
		AOE_STRUCTURES::STRUCT_PLAYER *gaia = GetPlayerStruct(0);
		assert(gaia && gaia->IsCheckSumValid());
		if (!gaia || !gaia->IsCheckSumValid()) {
			return;
		}
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_LINK *creatablesLink = gaia->ptrCreatableUnitsListLink;
		if (!creatablesLink || !creatablesLink->IsCheckSumValid()) {
			return;
		}
		if (creatablesLink->listElemCount <= 0) {
			return;
		}
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *currentElem = creatablesLink->lastListElement;
		int counter = creatablesLink->listElemCount;
		while ((currentElem != NULL) && (counter >= 1)) {
			AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)currentElem->unit;
			if (unitBase && unitBase->IsCheckSumValidForAUnitClass()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)unitBase->unitDefinition;
				if (unitDefBase && unitDefBase &&
					((unitDefBase->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING) || (unitDefBase->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TRAINABLE)) &&
					(unitDefBase->unitAIType != AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact)
					) {
					// Unit is a building or a living unit, so it is capturable
					// Very important : exclude artefacts to avoid human players from capturing artefacts more easily than AI players

					// We can convert into type50 because we checked unit type is 70 or 80
					AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *unit50 = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)unitBase;
					unit50->stillToBeDiscoveredByHuman = valueToSet;
				}
			}

			currentElem = currentElem->previousElement;
			counter--;
		}	
	}

	// Change "viewed" playerId
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_VIEWED_PLAYER) {
		long int actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		if ((actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount)) {
			return;
		}
		this->ChangeControlledPlayer(actionPlayerId, false);
	}
}


// Execute a trigger (considering requirements are OK)
void CustomRORCommand::ExecuteTriggerAction(CR_TRIGGERS::crTrigger *trigger) {
	if (!trigger) { return; }
	if (IsMultiplayer()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global != NULL);
	if (!global) { return; }
	if (!trigger->enabled) { return; }

	// Mark trigger as disabled now it has been executed (except repeatable triggers)
	if (trigger->triggerEventType != CR_TRIGGERS::EVENT_TIMER) {
		trigger->enabled = false;
	} else {
		long int timerInterval = trigger->GetParameterValue(CR_TRIGGERS::KW_TIMER_INTERVAL, 0);
		long int timerTimeValue = trigger->GetParameterValue(CR_TRIGGERS::KW_TIMER_VALUE, 0);
		if (timerInterval < 1) {
			trigger->enabled = false; // invalid timer interval OR one-shot only timer.
		} else {
			timerTimeValue += timerInterval;
			trigger->SetParameterValue(CR_TRIGGERS::KW_TIMER_VALUE, timerTimeValue);
		}
	}

	// Common for all triggers types: manage trigger to run "before".
	if (!trigger->triggerNameToRunBefore.empty() && CUSTOMROR::crInfo.triggerSet) {
		CR_TRIGGERS::crTrigger *trBefore = CUSTOMROR::crInfo.triggerSet->GetTrigger(trigger->triggerNameToRunBefore);
		// Do not allow calling repeatable triggers: those triggers may remain enabled (because they are repeatable), which may lead to infinite dependencies.
		if (trBefore && (trBefore->enabled) && (!trBefore->IsRepeatable())) {
			this->ExecuteTriggerAction(trBefore);
		}
	}

	// Use a dedicated routine to execute action.
	this->Trigger_JustDoAction(trigger);

	// Common for all triggers types: manage trigger to run "after".
	if (!trigger->triggerNameToRunAfter.empty() && CUSTOMROR::crInfo.triggerSet) {
		CR_TRIGGERS::crTrigger *trAfter = CUSTOMROR::crInfo.triggerSet->GetTrigger(trigger->triggerNameToRunAfter);
		// Do not allow calling repeatable triggers: those triggers may remain enabled (because they are repeatable), which may lead to infinite dependencies.
		if (trAfter && (trAfter->enabled) && (!trAfter->IsRepeatable())) {
			ExecuteTriggerAction(trAfter);
		}
	}
}

