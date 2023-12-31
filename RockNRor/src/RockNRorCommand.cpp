#include "../include/RockNRorCommand.h"


namespace ROCKNROR {



// Global static objects
RockNRorCommand ROCKNROR::crCommand;


RockNRorCommand::RockNRorCommand() {
}



RockNRorCommand::~RockNRorCommand() {
	this->FreeCustomVisibilityMap();
}


// Empties RockNRor log file
void RockNRorCommand::ResetLogFile() {
	FILE *f;
	int res = fopen_s(&f, MOD_NAME "\\" MOD_NAME ".log", "w+"); // overwrite if already existing
	fclose(f);
}

bool RockNRorCommand::CheckEnabledFeatures() {
	char strNotEnabled[] = " NOT ENABLED !";
	FILE *f;
	int res = fopen_s(&f, MOD_NAME "\\" MOD_NAME ".log", "a+"); // append (do not overwrite)
	if (res) {
		std::string msg = std::string(localizationHandler.GetTranslation(CRLANG_ID_CANT_WRITE_FILE, "ERROR: cannot write to"));
		msg += " ";
		msg += MOD_NAME "\\" MOD_NAME ".log";
		traceMessageHandler.WriteMessage(msg);
		return false;
	}
	fprintf_s(f, MOD_NAME " %s\n", VER_FILE_VERSION_STR);
	if (ROCKNROR::crInfo.configInfo.couldNotReadXMLConfig) {
		fprintf_s(f, localizationHandler.GetTranslation(CRLANG_ID_CANT_READ_XML_CONFIG, "Warning: configuration could not be read from XML file (missing or incorrect file)."));
		fprintf_s(f, "\n\n");
	}
	if (ROCKNROR::crInfo.configInfo.couldNotReadCivXMLConfig) {
		fprintf_s(f, localizationHandler.GetTranslation(CRLANG_ID_CANT_READ_CIV_XML_CONFIG, "RockNRor_civs.xml could not be read (missing or incorrect file). This is not an error unless you want to use custom civilizations."));
		fprintf_s(f, "\n\n");
	}

	fprintf_s(f, localizationHandler.GetTranslation(CRLANG_ID_RUN_EXE_CONFIG_CHECKS, "Running checks on active features in game executable."));
	fprintf_s(f, "\nThis detects if some features from this version of " MOD_NAME " will not work because you need to enable them with \"RockNRorAdmin\".\n");
	
	// Analyze EXE memory and check all necessary ROR_API calls are enabled.
	bool RORAPIFullyInstalled = ROCKNROR::PATCHER::CheckRorApiSequencesAreInstalled(f, ROCKNROR::crInfo.configInfo.autoFixMissingFeatures);

	fprintf_s(f, "\nEnd of checks.\n\nConfiguration:\n");
	// General - not directly related to game
	fprintf_s(f, "hideWelcomeMessage:                        %d\n", ROCKNROR::crInfo.configInfo.hideWelcomeMessage);
	fprintf_s(f, "showAlertOnMissingFeature:                 %d\n", ROCKNROR::crInfo.configInfo.showAlertOnMissingFeature);
	fprintf_s(f, "autoFixMissingFeatures:                    %d\n", ROCKNROR::crInfo.configInfo.autoFixMissingFeatures);
	fprintf_s(f, "empires.dat relative path to use:          %s\n", ROCKNROR::crInfo.configInfo.customEmpiresDatRelativePath.c_str());
	fprintf_s(f, "showRockNRorMenu:                          %d\n", ROCKNROR::crInfo.configInfo.showRockNRorMenu);
	fprintf_s(f, "showCustomPopulationInfo:                  %d\n", ROCKNROR::crInfo.configInfo.showCustomPopInfo);
	fprintf_s(f, "useImprovedGameSpeeds:                     %d\n", ROCKNROR::crInfo.configInfo.useImprovedGameSpeeds);
	fprintf_s(f, "collectRORDebugLogs:                       %ld\n", ROCKNROR::crInfo.configInfo.collectRORDebugLogs);
	fprintf_s(f, "showRockNRorNotifications:                 %d\n", ROCKNROR::crInfo.configInfo.showRockNRorNotifications);
	fprintf_s(f, "randomTechTreeForRMGames:                  %d\n", ROCKNROR::crInfo.configInfo.randomTechTreeForRMGames);
	fprintf_s(f, "enableRPGModeInRandomGames:                %d\n", ROCKNROR::crInfo.configInfo.enableRPGModeInRandomGames);
	fprintf_s(f, "enableRPGModeInScenario:                   %d\n", ROCKNROR::crInfo.configInfo.enableRPGModeInScenario);
	fprintf_s(f, "gameTimerSlowDownAutoFix:                  %d\n", ROCKNROR::crInfo.configInfo.gameTimerSlowDownAutoFix);
	fprintf_s(f, "gameTimerSlowDownFactor:                   %ld\n", ROCKNROR::crInfo.configInfo.gameTimerSlowDownFactor);
	fprintf_s(f, "baseDelayForDetectUnit:                    %ld\n", ROCKNROR::crInfo.configInfo.unitAIDetectNearbyUnitsMinimumDelay);
	fprintf_s(f, "fixInvisibleTree:                          %d\n", ROCKNROR::crInfo.configInfo.fixInvisibleTree);
	fprintf_s(f, "fixHumanPlayer_specific_seeUnit:           %d\n", ROCKNROR::crInfo.configInfo.fixHumanPlayer_specificSeeUnit);
	fprintf_s(f, "useImprovedButtonBar:                      %d\n", ROCKNROR::crInfo.configInfo.useImprovedButtonBar);
	fprintf_s(f, "fixTechEffectAddSetArmorAttack:            %d\n", ROCKNROR::crInfo.configInfo.fixTechEffectAddSetArmorAttack);
	fprintf_s(f, "allowMultiQueueing:                        %d\n", ROCKNROR::crInfo.configInfo.allowMultiQueueing);
	fprintf_s(f, "unitMovementFormations:                    %s\n", ROCKNROR::CONFIG::RockNRorConfig::GetFormationTypeText(ROCKNROR::crInfo.configInfo.currentRightClickMoveFormation));
	fprintf_s(f, "unitMovementFormations with CTRL key:      %s\n", ROCKNROR::CONFIG::RockNRorConfig::GetFormationTypeText(ROCKNROR::crInfo.configInfo.currentCtrlRightClickMoveFormation));
	// General - related to game
	fprintf_s(f, "unit resource amounts: alligator=%d bush=%d elephant=%d\n"
		"...gazelle=%d gold=%d lion=%d stone=%d tree=%d forest=%d fish=%d\n",
		ROCKNROR::crInfo.configInfo.unitResourceAmountAlligator,
		ROCKNROR::crInfo.configInfo.unitResourceAmountBerryBush, 
		ROCKNROR::crInfo.configInfo.unitResourceAmountElephant, 
		ROCKNROR::crInfo.configInfo.unitResourceAmountGazelle, 
		ROCKNROR::crInfo.configInfo.unitResourceAmountGoldMine, 
		ROCKNROR::crInfo.configInfo.unitResourceAmountLion, 
		ROCKNROR::crInfo.configInfo.unitResourceAmountStoneMine, 
		ROCKNROR::crInfo.configInfo.unitResourceAmountTree, 
		ROCKNROR::crInfo.configInfo.unitResourceAmountTreeForest,
		ROCKNROR::crInfo.configInfo.unitResourceAmountFish);
	// Random games settings
	fprintf_s(f, "noWalls:                                   %ld\n", ROCKNROR::crInfo.configInfo.noWalls);
	fprintf_s(f, "noNeutralInitialDiplomacy:                 %d\n", ROCKNROR::crInfo.configInfo.noNeutralInitialDiplomacy);
	fprintf_s(f, "[RM] initial food (default/small/med/large) : %ld/%ld/%ld/%ld\n",
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[0][RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[1][RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[2][RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[3][RESOURCE_TYPES::CST_RES_ORDER_FOOD]);
	fprintf_s(f, "[RM] initial wood (default/small/med/large) : %ld/%ld/%ld/%ld\n",
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[0][RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[1][RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[2][RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[3][RESOURCE_TYPES::CST_RES_ORDER_WOOD]);
	fprintf_s(f, "[RM] initial stone (default/small/med/large): %ld/%ld/%ld/%ld\n",
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[0][RESOURCE_TYPES::CST_RES_ORDER_STONE],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[1][RESOURCE_TYPES::CST_RES_ORDER_STONE],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[2][RESOURCE_TYPES::CST_RES_ORDER_STONE],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[3][RESOURCE_TYPES::CST_RES_ORDER_STONE]);
	fprintf_s(f, "[RM] initial gold (default/small/med/large) : %ld/%ld/%ld/%ld\n",
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[0][RESOURCE_TYPES::CST_RES_ORDER_GOLD],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[1][RESOURCE_TYPES::CST_RES_ORDER_GOLD],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[2][RESOURCE_TYPES::CST_RES_ORDER_GOLD],
		ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[3][RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
	fprintf_s(f, "[DM] initial resources (food/wood/stone/gold): %ld/%ld/%ld/%ld\n",
		ROCKNROR::crInfo.configInfo.initialResources_DM[RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		ROCKNROR::crInfo.configInfo.initialResources_DM[RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		ROCKNROR::crInfo.configInfo.initialResources_DM[RESOURCE_TYPES::CST_RES_ORDER_STONE],
		ROCKNROR::crInfo.configInfo.initialResources_DM[RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
	fprintf_s(f, "[RM] initialBonus for AI (food/wood/stone/gold): %ld/%ld/%ld/%ld\n",
		ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_STONE],
		ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
	fprintf_s(f, "[DM] initialBonus for AI (food/wood/stone/gold): %ld/%ld/%ld/%ld\n",
		ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_FOOD],
		ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_WOOD],
		ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_STONE],
		ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
	for (int i = 0; i <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER; i++) {
		if (ROCKNROR::crInfo.configInfo.defaultPerNumbers_RM_isSet[i]) {
			fprintf_s(f, "[RM] Force PER number #%ld to %ld\n", i, ROCKNROR::crInfo.configInfo.defaultPerNumbers_RM[i]);
		}
	}
	for (int i = 0; i <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER; i++) {
		if (ROCKNROR::crInfo.configInfo.defaultPerNumbers_DM_isSet[i]) {
			fprintf_s(f, "[DM] Force PER number #%ld to %ld\n", i, ROCKNROR::crInfo.configInfo.defaultPerNumbers_DM[i]);
		}
	}
	// Human interface / shortcuts
	fprintf_s(f, "useNumPadUnitShortcuts:                    %d\n", ROCKNROR::crInfo.configInfo.enableAdditionalNumpadShortcuts);
	fprintf_s(f, "enableCallNearbyIdleMilitaryUnits:         %d\n", ROCKNROR::crInfo.configInfo.enableCallNearbyIdleMilitaryUnits);
	fprintf_s(f, "distanceToCallNearbyIdleMilitaryUnits:     %ld\n", ROCKNROR::crInfo.configInfo.distanceToCallNearbyIdleMilitaryUnits);
	fprintf_s(f, "enableAutoMoveToLocation:                  %d\n", ROCKNROR::crInfo.configInfo.enableSpawnUnitsMoveToLocation);
	fprintf_s(f, "enableSpawnUnitsAutoTarget:                %d\n", ROCKNROR::crInfo.configInfo.enableSpawnUnitsAutoTarget);
	fprintf_s(f, "enableSpawnUnitAutoRepairTC:               %d\n", ROCKNROR::crInfo.configInfo.enableSpawnUnitAutoRepairTC);
	fprintf_s(f, "unitSpawnShortcutReverseOrder:             %d\n", ROCKNROR::crInfo.configInfo.unitShortcutsPriorityReverseOrder);
	for (int shortcutId = 1; shortcutId < CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS; shortcutId++) {
		CONFIG::UnitSpawnShortcutInfo *sinfo = &ROCKNROR::crInfo.configInfo.unitShortcutsInformation[shortcutId];
		if (sinfo->DAT_ID >= 0) {
			fprintf_s(f, "unitSpawn shortcut %d: unit=%03d onlyOneUnit=%d (%.12s)\n", shortcutId, sinfo->DAT_ID, sinfo->onlyOneUnit,
				GetHardcodedUnitName(sinfo->DAT_ID));
		}
	}
	for (ROCKNROR::ConfigGameType i = (ROCKNROR::ConfigGameType)(ROCKNROR::CFG_GAME_UNKNOWN + 1); i < ROCKNROR::CFG_GAME_TYPES_COUNT; i = (ROCKNROR::ConfigGameType)(i + 1)) {
		const char *name = ROCKNROR::ConfigGameTypeNames[i];
		fprintf_s(f, "[%s] autoRebuildFarms:                %ld\n", name, ROCKNROR::crInfo.configInfo.autoRebuildFarmsConfig[i].enableAutoRebuildFarms ? 1 : 0);
		fprintf_s(f, "[%s] autoRebuildFarms_maxFarms:       %ld\n", name, ROCKNROR::crInfo.configInfo.autoRebuildFarmsConfig[i].autoRebuildFarms_maxFarms);
		fprintf_s(f, "[%s] autoRebuildFarms_maxFood:        %ld\n", name, ROCKNROR::crInfo.configInfo.autoRebuildFarmsConfig[i].autoRebuildFarms_maxFood);
		fprintf_s(f, "[%s] autoRebuildFarms_minWood:        %ld\n", name, ROCKNROR::crInfo.configInfo.autoRebuildFarmsConfig[i].autoRebuildFarms_minWood);
	}
	fprintf_s(f, "autoGatherAfterBuildDeposit:               %ld\n", ROCKNROR::crInfo.configInfo.autoGatherAfterBuildDeposit);
	fprintf_s(f, "useEnhancedRulesForAutoAttack:             %ld\n", ROCKNROR::crInfo.configInfo.useEnhancedRulesForAutoAttackTargetSelection);
	fprintf_s(f, "autoAttackPolicy vs towers/military/buildings/villagers/walls\n");
	fprintf_s(f, "- For Melee Units:  %d/%d/%d/%d/%d\n", ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits.attackTowers,
		ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits.attackMilitary, 
		ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits.attackNonTowerBuildings, 
		ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits.attackVillagers, 
		ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits.attackWalls);
	fprintf_s(f, "- For Ranged Units: %d/%d/%d/%d/%d\n", ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits.attackTowers,
		ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits.attackMilitary,
		ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits.attackNonTowerBuildings,
		ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits.attackVillagers,
		ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits.attackWalls);
	fprintf_s(f, "markUnitsWithCivBonus:                     %ld\n", ROCKNROR::crInfo.configInfo.markUnitsWithCivBonus);

	// Conversion
	if (ROCKNROR::crInfo.configInfo.conversionResistanceIgnoreMacedonianBonus) {
		fprintf_s(f, "conversionResistance: do NOT apply Macedonian civ bonus (do not force resistance to 4 on all units).\n");
	}
	if (ROCKNROR::crInfo.configInfo.conversionResistanceAttackClassEnabled) {
		fprintf_s(f, "conversionResistance_armorClass:           %d\n", ROCKNROR::crInfo.configInfo.conversionResistanceAttackClass);
	} else {
		fprintf_s(f, "conversionResistance_Boats:                %f\n", ROCKNROR::crInfo.configInfo.conversionResistance_Boats);
		fprintf_s(f, "conversionResistance_Chariots:             %f\n", ROCKNROR::crInfo.configInfo.conversionResistance_Chariots);
		if (!ROCKNROR::crInfo.configInfo.conversionResistanceIgnoreMacedonianBonus) {
			fprintf_s(f, "conversionResistance_Macedonian:           %f\n", ROCKNROR::crInfo.configInfo.conversionResistance_Macedonian);
		}
		fprintf_s(f, "conversionResistance_Priests:              %f\n", ROCKNROR::crInfo.configInfo.conversionResistance_Priests);
		fprintf_s(f, "conversionResistance_WarElephants:         %f\n", ROCKNROR::crInfo.configInfo.conversionResistance_WarElephants);
		fprintf_s(f, "conversionResistance_WarElephants_Persian: %f\n", ROCKNROR::crInfo.configInfo.conversionResistance_WarElephants_Persian);
	}
	// Various AI
	fprintf_s(f, "improveAILevel:                            %ld\n", ROCKNROR::crInfo.configInfo.improveAILevel);
	fprintf_s(f, "testCompareAI:                             %ld\n", ROCKNROR::crInfo.configInfo.enableTestCompareAI ? 1: 0);
	fprintf_s(f, "tacticalAIUpdateDelay:                     %ld\n", ROCKNROR::crInfo.configInfo.tacticalAIUpdateDelay);
	fprintf_s(f, "minPopulationBeforeOptionalItems:          %ld\n", ROCKNROR::crInfo.configInfo.minPopulationBeforeBuildOptionalItems);
	fprintf_s(f, "maxPanicUnitsCountToAddInStrategy:         %ld\n", ROCKNROR::crInfo.configInfo.maxPanicUnitsCountToAddInStrategy);
	fprintf_s(f, "panicModeDelay:                            %ld\n", ROCKNROR::crInfo.configInfo.panicModeDelay);
	fprintf_s(f, "dislike computation interval:              %ld\n", ROCKNROR::crInfo.configInfo.dislikeComputeInterval);
	fprintf_s(f, "dislike value - all artefacts/wonder:      %ld\n", ROCKNROR::crInfo.configInfo.dislike_allArtefacts);
	fprintf_s(f, "dislike value - human player:              %ld\n", ROCKNROR::crInfo.configInfo.dislike_humanPlayer);
	fprintf_s(f, "fixLogisticsNoHouseBug:                    %d\n", ROCKNROR::crInfo.configInfo.fixLogisticsNoHouseBug ? 1: 0);
	fprintf_s(f, "fixVillagerWorkRates:                      %d\n", ROCKNROR::crInfo.configInfo.fixVillagerWorkRates);
	// City plan
	fprintf_s(f, "cityPlanLikeValuesEnhancement:             %d\n", ROCKNROR::crInfo.configInfo.cityPlanLikeValuesEnhancement);
	//fprintf_s(f, "cityPlanHouseDistanceFromTownCenter:       %f\n", ROCKNROR::crInfo.configInfo.cityPlanHouseDistanceFromTownCenter);
	//fprintf_s(f, "cityPlanHouseDistanceFromStorageBld:       %f\n", ROCKNROR::crInfo.configInfo.cityPlanHouseDistanceFromStorageBld);
	//fprintf_s(f, "cityPlanHouseDistanceFromOtherBld:         %f\n", ROCKNROR::crInfo.configInfo.cityPlanHouseDistanceFromOtherBld);
	fprintf_s(f, "cityPlanBerryBushWeightForGranary:         %ld\n", ROCKNROR::crInfo.configInfo.cityPlanBerryBushWeightForGranary);
	// Forced researches
	std::string tmpMsg = "";
	for each (auto oneResearchId in ROCKNROR::crInfo.configInfo.rmAIForcedResearches)
	{
		if (!tmpMsg.empty()) {
			tmpMsg += ", ";
		}
		tmpMsg += std::to_string(oneResearchId);
	}
	fprintf_s(f, "strategy/RM/forced researches: %s\n", tmpMsg.c_str());
	tmpMsg.clear();
	for each (auto oneResearchId in ROCKNROR::crInfo.configInfo.dmAIForcedResearches)
	{
		if (!tmpMsg.empty()) {
			tmpMsg += ", ";
		}
		tmpMsg += std::to_string(oneResearchId);
	}
	fprintf_s(f, "strategy/DM/forced researches: %s\n", tmpMsg.c_str());
	// Map generation
	fprintf_s(f, "random map relics count:                   %ld\n", ROCKNROR::crInfo.configInfo.randomMapRelicsCount);
	fprintf_s(f, "random map ruins count:                    %ld\n", ROCKNROR::crInfo.configInfo.randomMapRuinsCount);
	fprintf_s(f, "use map gen. elevation customization:      %d\n", ROCKNROR::crInfo.configInfo.useMapGenerationCustomElevationCalculation ? 1: 0);
	for (int mapType = 0; mapType < 10; mapType++) {
		fprintf_s(f, "map gen. elevation factor  mapType=%ld value=%f\n", mapType, ROCKNROR::crInfo.configInfo.mapGenerationCustomElevationFactor[mapType]);
	}
	fprintf_s(f, "disable dock in mostly-land maps:          %ld\n", ROCKNROR::crInfo.configInfo.noDockInMostlyLandMaps ? 1: 0);
	// Scenario Editor
	fprintf_s(f, "showHiddenTerrainInEditor:                 %d\n", ROCKNROR::crInfo.configInfo.showHiddenTerrainsInEditor);
	fprintf_s(f, "showHiddenUnitsInEditor:                   %ld\n", ROCKNROR::crInfo.configInfo.showHiddenUnitsInEditor);
	fprintf_s(f, "useCustomMapDimensionsInEditor:            %d\n", ROCKNROR::crInfo.configInfo.useCustomMapDimensions);

	// Other
	fprintf_s(f, "civilizations count:                       %ld\n", ROCKNROR::crInfo.configInfo.civCount);
	fprintf_s(f, "Single player population limit:            %ld\n", ROCKNROR::crInfo.configInfo.singlePlayerMaxPopulation);

	fprintf_s(f, "End of configuration.\n\n");
	fclose(f);
	return RORAPIFullyInstalled;
}


// Initialization that only takes place once, at executable startup.
void RockNRorCommand::OneShotInit() {
	// Collect information about EXE version
	this->ReadIfCustomSelectedUnitsMemoryZoneIsUsed();
	this->ReadIfManageAIIsOn();
	this->ReadOtherSequencesStatus();

	// Load/Prepare localization strings
	this->LoadCustomLocalizationFiles();

	// Manage interfac.drs file to use
	ROCKNROR::crInfo.usingCustomInterfaceDrsFile = ROCKNROR::PATCHER::ChangeItfDRS_file();

	// Prepare custom DRS data
	this->LoadCustomDrsFiles();

	// Update "population limit getter" according to configuration
	ROCKNROR::PATCHER::SetMaxPopulationGetterInSPGames(ROCKNROR::crInfo.configInfo.singlePlayerMaxPopulation);

	this->InitCustomVisibilityMap();

#ifdef _DEBUG
	if (CR_DEBUG::debugSerialization) {
		ROCKNROR::PATCHER::SetDeserializationDebugChange(true);
		WriteToFile("", CR_DEBUG::serializationLogFilename, false); // reset file content
	}
#endif
}


// Returns true if game version matches this DLL's version (eg AOE 1.0b or ROR 1.0c...)
bool RockNRorCommand::CheckGameVersion() {
	// If wrong game version, binary sequence won't match, IsBinaryChangeOn returns false in this case.
	// Remark: do not use "ROR_API_VarDataUpdate" for this test because it's in data memory section and may change in run-time.
	// "ROR_API_DLL_initialization" is safer because in text memory (not supposed to change at all).
	return ROCKNROR::PATCHER::IsBinaryChangeOn(BINSEQ_CATEGORIES::BC_ROR_API, "ROR_API_DLL_initialization");
}


// Reads game executable to determine if player struct is extended to use custom memory zone to host selected units
void RockNRorCommand::ReadIfCustomSelectedUnitsMemoryZoneIsUsed() {
	// TO DO: check all sequences ?
	ROCKNROR::crInfo.hasCustomSelectedUnitsMemory = ROCKNROR::PATCHER::IsBinaryChangeOn(BINSEQ_CATEGORIES::BC_SELECTED_UNITS, "InitBuffer1");
}

// Reads game executable to determine if ManageAI is installed (does game use player->unused_customAIFlag ?)
void RockNRorCommand::ReadIfManageAIIsOn() {
	ROCKNROR::crInfo.hasManageAIFeatureON = ROCKNROR::PATCHER::IsBinaryChangeOn(BINSEQ_CATEGORIES::BC_MANAGE_AI, "Init_is_computer_for_AI_1");
}


// Reads game executable to determine if various sequences are installed or not
void RockNRorCommand::ReadOtherSequencesStatus() {
	ROCKNROR::crInfo.hasRemovePlayerInitialAgeInScenarioInit = ROCKNROR::PATCHER::IsBinaryChangeOn(BINSEQ_CATEGORIES::BC_ROR_API, "FixScenarioBadInitialAgeApplication_removeBad");
	ROCKNROR::crInfo.hasFixForBuildingStratElemUnitId = ROCKNROR::PATCHER::IsBinaryChangeOn(BINSEQ_CATEGORIES::BC_ROR_API, "FixUnitIdForInProgressBuilding");
}

// Load custom strings files
void RockNRorCommand::LoadCustomLocalizationFiles() {
	for each (std::string filename in ROCKNROR::crInfo.configInfo.customStringsFilesList) {
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
void RockNRorCommand::LoadCustomDrsFiles() {
	for each (CONFIG::DrsFileToLoad *drs in ROCKNROR::crInfo.configInfo.customDrsFilesList)
	{
		AOE_METHODS::AddDrsFile(drs->filename.c_str(), drs->folder.c_str());
		if (FindDrsLinkForFile(drs->filename.c_str())) {
			traceMessageHandler.WriteMessageNoNotification(drs->folder + std::string("\\") +drs->filename + std::string(" has been loaded"));
		} else {
			traceMessageHandler.WriteMessage(std::string("ERROR : ") + drs->folder + std::string("\\") + drs->filename + std::string(" could NOT be loaded"));
		}
	}

	// Prepare custom DRS data
	if (ROCKNROR::crInfo.configInfo.useImprovedButtonBar) {
		if (ROCKNROR::crInfo.configInfo.showAlertOnMissingFeature && !FindDrsLinkForFile(CST_ROCKNROR_DRS_FILENAME)) {
			MessageBoxA(0, "ERROR : Could not find RockNRor.drs or it is invalid.", MOD_NAME, MB_ICONWARNING);
		}
		// Initialize global variable so we can retrieve our button icons when needed
		AOE_METHODS::InitSlpInfoFromDrs(&ROCKNROR::crInfo.rockNRorIcons, AOE_CONST_DRS::CST_ROCKNROR_CMD_ICONS_SLP_ID);
		AOE_METHODS::InitSlpInfoFromDrs(&ROCKNROR::crInfo.rockNRorUnitShortcuts, AOE_CONST_DRS::CST_ROCKNROR_UNIT_SHORTCUTS_SLP_ID);
	}
}


// Automatically patch executable to adapt to DRS files screen resolution
void RockNRorCommand::PatchExeForDrsScreenResolution(STRUCT_GAME_SETTINGS *settings) {
	// Should we try to patch exe for resolution when using interfac.drs (standard) files ? why not, after all...
	/*if (!ROCKNROR::crInfo.usingCustomInterfaceDrsFile) {
		return;
	}*/
	if (!settings || !settings->IsCheckSumValid()) {
		return;
	}
	long int slpId = ROCKNROR::crInfo.configInfo.autoDetectDrsScreenSizeSlpID;
	// We may use any SLP whose size is full screen size... 
	// But it better be a SLP which is ALWAYS consistent with target screen size (some DRS may not be fully adapted to target resolution...)
	STRUCT_SLP_INFO *screenSizedSlp = (STRUCT_SLP_INFO*)AOEAlloc(0x20);
	AOE_METHODS::InitSlpInfoFromDrs(screenSizedSlp, slpId, "");
	if (screenSizedSlp->slpFileData) {
		long int drsScreenSizeX = screenSizedSlp->slpFileData->xSize;
		long int drsScreenSizeY = screenSizedSlp->slpFileData->ySize;

		std::string msg = "Detected DRS screen resolution is ";
		std::string resolutionAsText = std::to_string(drsScreenSizeX) + std::string(" * ") + std::to_string(drsScreenSizeY);
		msg += resolutionAsText;
		traceMessageHandler.WriteMessageNoNotification(msg);
		ROCKNROR::PATCHER::ChangeExeResolution(drsScreenSizeX, drsScreenSizeY);

		// Add/update the "1024*768" label so the correct values are displayed in "game options" popup
		localizationHandler.SetString(LANG_ID_1024_768, resolutionAsText.c_str());
	}
	AOE_METHODS::FreeSlpInfo(screenSizedSlp);
	screenSizedSlp = NULL;
}


// Initialize custom visibility map to replace "0x7D205C" data array
void RockNRorCommand::InitCustomVisibilityMap() {
	long int maxSize = ROCKNROR::crInfo.configInfo.maximumMapSize;
	if (maxSize <= ROCKNROR::crInfo.configInfo.MINVALUE_maxMapSize) {
		return;
	}
	this->pCustomVisibilityMap = AOEAllocZeroMemory(maxSize * maxSize, sizeof(DWORD));
	unsigned long int customMapAddr = (unsigned long int)this->pCustomVisibilityMap;

	BinarySeqDefSet *seqDefSet = ROCKNROR::PATCHER::aoeBinData.GetSeqDefSet(GetBuildVersion(), BINSEQ_CATEGORIES::BC_LARGER_MAPS);
	if (!seqDefSet) {
		return;
	}
	std::string msg = "Patching game to allow map size up to ";
	msg += to_string(maxSize);
	msg += "*";
	msg += to_string(maxSize);
	traceMessageHandler.WriteMessageNoNotification(msg);

	// Modify the pseudo-const used for map coordinates checks
	AOE_MAX_ALLOWED_MAP_SIZE = maxSize;

	int seqCount = seqDefSet->GetCount();
	for (int i = 0; i < seqCount; i++) {
		BinarySeqDefinition *seqDef = seqDefSet->GetBinSeqDefinition(i);
		if ((seqDef->GetFuncMeaning(0) == FUNC_MEANING::FM_NO_CHOICE) && (seqDef->GetVarType(0) == SEQ_VAR_TYPES::SVT_INT_4B)) {
			std::string seqName = narrow(seqDef->GetSeqName());
			unsigned long int currentValue = ROCKNROR::PATCHER::GetBinaryChangeVarValue(BC_LARGER_MAPS, seqName, 0);
			unsigned long int newValue = customMapAddr;
			unsigned long diff = currentValue - ADDR_MAP_VISIBILITY_INFO;
			// Dirty to use -4 as unsigned long, but it works anyway
			switch (diff)
			{
				// This switch checks that we only encounter 7D2058/7D205C/7D2060 (for ROR10c offsets)
			case -4:
			case 0:
			case 4:
				newValue += diff;
				ROCKNROR::PATCHER::SetBinaryChangeVarValue(BINSEQ_CATEGORIES::BC_LARGER_MAPS, seqName, 0, newValue);
				break;
			default:
				// Should not occur
				break;
			}
		}
		else {
			// TODO ON/OFF sequences if any
		}

	}
}


// Free Custom visibility map array, if any
void RockNRorCommand::FreeCustomVisibilityMap() {
	if (this->pCustomVisibilityMap) {
		AOEFree(this->pCustomVisibilityMap);
		this->pCustomVisibilityMap = NULL;
	}
}


// Get custom empires.dat filename (with relative path)
const char *RockNRorCommand::GetCustomEmpiresDatRelativeFileName(AOE_STRUCTURES::STRUCT_COMMAND_LINE_INFO *cmdLineInfo) {
	if (!ROCKNROR::crInfo.configInfo.customEmpiresDatRelativePath.empty()) { // TODO use dedicated/correct config
		const char *filename = ROCKNROR::crInfo.configInfo.customEmpiresDatRelativePath.c_str();
		if (CheckFileExistence(filename)) {
			return filename;
		}
		else {
			std::string msg = localizationHandler.GetTranslation(CRLANG_ID_ERROR_COULDNT_FIND, "ERROR : Could not find");
			msg += " ";
			msg += filename;
			msg += " ";
			msg += localizationHandler.GetTranslation(CRLANG_ID_ERROR_EMPIRES_DAT_USED_INSTEAD, "file. data2\\empires.dat will be used instead.");
			MessageBoxA(0, msg.c_str(), MOD_NAME, MB_ICONWARNING);
		}
	}

	// Standard behaviour : use game's string
	return cmdLineInfo->relativePath_empires_dat;
}


// Execute a command from custom options window
// Returns true if the provided command is valid (and executed)
// After the call, output is a string containing command's output, if any.
bool RockNRorCommand::ExecuteCommand(char *command, std::string &output) {
	static const char *PREFIX_SET = "set ";
	output = "";

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
				output = "Map seed=";
				output += std::to_string(newSeed);
			}
			return true;
		}
		if (!_strnicmp(subCmd, "maptype=", 8)) {
			subCmd += 8;
			int newType = StrToInt(subCmd, -1);
			if ((newType >= 0) && (newType < AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_MAP_TYPES_COUNT)) {
				gameSettings->mapTypeChoice = (AOE_CONST_FUNC::MAP_TYPE_INDEX) newType;
				output = "Map type=";
				output += std::to_string(newType);
			}
			return true;
		}

		if (!_strnicmp(subCmd, "control=", 8)) {
			subCmd += 8;
			int newId = StrToInt(subCmd, -1);
			if (((newId <= 0) && (newId < 9))) {
				AOE_METHODS::PLAYER::ChangeControlledPlayer(newId, true);
				output = "Controlled player#=";
				output += std::to_string(newId);
			}
			return true;
		}

		// Change maximum population value for this game (and next ones)
		if (!_strnicmp(subCmd, "maxpop=", 7)) {
			subCmd += 7;
			int newMaxPop = atoi(subCmd);
			if ((newMaxPop > 0) && (newMaxPop <= 255)) {
				if (ROCKNROR::PATCHER::SetMaxPopulationGetterInSPGames(newMaxPop)) {
					AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
					if (global && global->IsCheckSumValid() && global->ptrPlayerStructPtrTable) {
						for (int i = 0; i < global->playerTotalCount; i++) {
							AOE_STRUCTURES::STRUCT_PLAYER *player = global->ptrPlayerStructPtrTable[i];
							if (player && player->IsCheckSumValid()) {
								player->SetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_POPULATION_LIMIT, (float)newMaxPop);
							}
						}
					}
					output = "Max population=";
					output += std::to_string(newMaxPop);
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
		if (unit && unit->IsCheckSumValidForAUnitClass()) {
			AOE_METHODS::PLAYER::ClearSelectedUnits(player);
			AOE_METHODS::PLAYER::SelectUnit(player, unit, true);
			char *name = "?";
			if (unit->unitDefinition && unit->unitDefinition->IsCheckSumValidForAUnitClass()) {
				name = unit->unitDefinition->ptrUnitName;
			}
			output = std::string("selected ") + std::string(name);
		}
		else {
			output = std::string("Unit not found");
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
			output = "Add ";
			output += unitDef->ptrUnitName;
			output += ". Right-click to quit add mode.";
		}
	}

	if (!_strnicmp(command, "strat", 5)) {
		bool useStandardAIFormat = false;
		char firstCharAfter = command[5];
		bool isValid = (firstCharAfter == 0) || ((firstCharAfter >= '0') && (firstCharAfter <= '9'));
		if (!isValid && (firstCharAfter == ' ')) {
			isValid = (!_strnicmp(command, "strat export", 12));
			if (isValid) {
				useStandardAIFormat = true;
				firstCharAfter = command[12];
				isValid = (firstCharAfter == 0) || ((firstCharAfter >= '0') && (firstCharAfter <= '9'));
				command += 12;
			}
		} else {
			command += 5;
		}
		if (isValid) {
			int playerId = atoi(command);
			if ((playerId == 0) && (*command != '0')) { playerId = -1; }
			AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
			if (global && global->IsCheckSumValid() && (playerId > 0) && (playerId < global->playerTotalCount)) {
				player = GetPlayerStruct(playerId);
			}
			if (player && player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid()) {
				std::string s = STRATEGY::ExportStrategyToText(&player->ptrAIStruct->structBuildAI, useStandardAIFormat);
				traceMessageHandler.WriteMessage(s);
				output = "Close this window to see strategy.";
			}
			return true;
		}
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


void RockNRorCommand::HandleChatCommand(char *command) {
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
		AOE_METHODS::PLAYER::SetAllAIFlags(true);
		AOE_METHODS::CallWriteText(localizationHandler.GetTranslation(CRLANG_ID_AI_FLAGS_ALL_SET, "All players are now computer-managed."));
	}
	if ((strcmp(command, "noai") == 0) || (strcmp(command, "NOAI") == 0)) {
		AOE_METHODS::PLAYER::SetAllAIFlags(false);
		AOE_METHODS::CallWriteText(localizationHandler.GetTranslation(CRLANG_ID_AI_FLAGS_NONE_SET, "AI is disabled for all players"));
	}
	if ((strcmp(command, "oneai") == 0) || (strcmp(command, "ONEAI") == 0)) {
		AOE_METHODS::PLAYER::SetAllAIFlags(false);
		STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (global) {
			AOE_METHODS::PLAYER::SetAIFlag(global->humanPlayerId, 1);
			AOE_METHODS::CallWriteText(localizationHandler.GetTranslation(CRLANG_ID_AI_FLAGS_ONLY_CURRENT_SET, "AI is disabled for all players but current one"));
		}
	}
	if ((strcmp(command, "myai") == 0) || (strcmp(command, "MYAI") == 0)) {
		STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (global) {
			AOE_METHODS::PLAYER::SetAIFlag(global->humanPlayerId, -1);
			AOE_METHODS::CallWriteText(localizationHandler.GetTranslation(CRLANG_ID_AI_FLAG_SWITCH_CURRENT, "Switched AI mode for current player"));
		}
	}
	if (strcmp(command, "dump") == 0) {
#ifdef _DEBUG
		CR_DEBUG::DumpDebugInfoToFile();
#endif
	}
	if (strcmp(command, "about") == 0) {
		std::string s = localizationHandler.GetTranslation(CRLANG_ID_ROCKNROR, MOD_NAME);
		s += " ";
		s += VER_FILE_VERSION_STR;
		AOE_METHODS::CallWriteText(s.c_str());
	}
	if (strcmp(command, "timer stats") == 0) {
		this->DisplayTimerStats();
	}
	if (strcmp(command, "dat check") == 0) {
		AOE_METHODS::SetGamePause(true);
		CR_DEBUG::AnalyzeEmpiresDatQuality();
	}
	if (!strcmp(command, "reload game")) {
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		if (settings && settings->IsCheckSumValid() && !AOE_METHODS::IsMultiplayer()) {
			const char saveFilename[] = "_autosave-reload-RockNRor";
#ifdef GAME_IS_ROR
			const char saveFilenameExt[] = "_autosave-reload-RockNRor.gmx";
#else
			const char saveFilenameExt[] = "_autosave-reload-RockNRor.gam";
#endif
			bool res = AOE_METHODS::SaveCurrentGame(saveFilenameExt); // Will also save rnr data
			settings->isSavedGame = true;
			strcpy_s(settings->loadGameName, saveFilename);
			strcpy_s(settings->saveGameName, saveFilename);
			std::string msg = "Save game (before reload) to ";
			msg += saveFilename;
			msg += ": ";
			msg += (res ? "success" : "failure");
			traceMessageHandler.WriteMessageNoNotification(msg);
			res = AOE_METHODS::RestartGame();
			if (!res) {
				traceMessageHandler.WriteMessageNoNotification("Reload failed. Game was saved in _autosave-reload-RockNRor.gmx");
			}
		}
	}

	if (strcmp(command, "civ bonus") == 0) {
		// Display civilization bonus if using random civ bonus/tech tree
		STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (global && (global->humanPlayerId >= 0) && (global->humanPlayerId < 9)) {
			auto listForHumanPlayer = ROCKNROR::crInfo.myGameObjects.civBonusInGameTextLinesByPlayerId[global->humanPlayerId];
			for (auto it = listForHumanPlayer.cbegin(); it != listForHumanPlayer.cend(); it++) {
				short int textIndex = *it;
				std::string text = ROCKNROR::crInfo.myGameObjects.inGameTextHandler.GetText(textIndex);
				AOE_METHODS::CallWriteText(text.c_str());
			}
		}
	}

#ifdef _DEBUG
	if (strcmp(command, "unit analyze") == 0) {
		AOE_METHODS::SetGamePause(true);
		CR_DEBUG::AnalyzeMilitaryUnitsEfficency();
	}
	// p0 to p8: quick player change + set AI flags automatically
	if ((*command == 'p') && (*(command + 1) >= '0') && (*(command + 1) < '9')) {
		int playerId = *(command + 1) - '0';
		AOE_METHODS::PLAYER::ChangeControlledPlayer(playerId, true);
	}
	// g0 to g8: quick player change + keep screen position
	if ((*command == 'g') && (*(command + 1) >= '0') && (*(command + 1) < '9')) {
		int playerId = *(command + 1) - '0';
		AOE_STRUCTURES::STRUCT_PLAYER *oldPlayer = GetControlledPlayerStruct_Settings();
		AOE_STRUCTURES::STRUCT_PLAYER *newPlayer = GetPlayerStruct(playerId);
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		if (oldPlayer && newPlayer && settings && settings->ptrGameUIStruct && (newPlayer->aliveStatus != 2)) {
			AOE_METHODS::PLAYER::CopyScreenPosition(oldPlayer->playerId, playerId);
			AOE_METHODS::PLAYER::ChangeControlledPlayer(playerId, false);
		}
	}

	if (strcmp(command, "mapbmp") == 0) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (global && global->IsCheckSumValid() && global->scenarioInformation && global->scenarioInformation->IsCheckSumValid()) {
			if (_BITMAP::BitmapExporter::SaveTPictureToBmp(global->scenarioInformation->scenarioMapPicture, "c:\\temp\\scnMap.bmp")) {
				AOE_METHODS::UI_BASE::CallWriteText("Successfully saved scenario map");
			}
		}
	}

	if (strcmp(command, "empty farm") == 0) {
		// Sometimes we need to have almost empty farms for testing...
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		if (!player) { return; }
		AOE_STRUCTURES::STRUCT_UNIT_BASE *selectedUnit = ROCKNROR::crInfo.GetRelevantMainSelectedUnitPointer(player);
		if (selectedUnit && ((STRUCT_UNIT_BUILDING*)(selectedUnit))->IsCheckSumValid() &&
			(selectedUnit->ptrStructPlayer == player)) {
			SetFarmCurrentTotalFood((STRUCT_UNIT_BUILDING*)(selectedUnit), 2);
		}
	}

	// analyze custom unit groups AI
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
		ROCKNROR::crInfo.configInfo.assassinMode = !ROCKNROR::crInfo.configInfo.assassinMode;
	}
	if (strcmp(command, "rich") == 0) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		if (player && player->IsCheckSumValid()) {
			player->SetResourceValue(AOE_CONST_FUNC::CST_RES_ORDER_FOOD, 50000);
			player->SetResourceValue(AOE_CONST_FUNC::CST_RES_ORDER_WOOD, 50000);
			player->SetResourceValue(AOE_CONST_FUNC::CST_RES_ORDER_GOLD, 20000);
			player->SetResourceValue(AOE_CONST_FUNC::CST_RES_ORDER_STONE, 10000);
		}
	}
	if (strcmp(command, "allrich") == 0) {
		for (int i = 0; i < 9; i++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(i);
			if (player && player->IsCheckSumValid()) {
				player->SetResourceValue(AOE_CONST_FUNC::CST_RES_ORDER_FOOD, 50000);
				player->SetResourceValue(AOE_CONST_FUNC::CST_RES_ORDER_WOOD, 50000);
				player->SetResourceValue(AOE_CONST_FUNC::CST_RES_ORDER_GOLD, 20000);
				player->SetResourceValue(AOE_CONST_FUNC::CST_RES_ORDER_STONE, 10000);
			}
		}
	}
	int goAge = 0;
	if (strcmp(command, "gotool") == 0) {
		goAge = 1;
	}
	if (strcmp(command, "gobronze") == 0) {
		goAge = 2;
	}
	if (strcmp(command, "goiron") == 0) {
		goAge = 3;
	}
	if (goAge > 0) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		if (player && player->IsCheckSumValid()) {
			auto x = GetResearchStatus(player, CST_RSID_TOOL_AGE);
			if ((x == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) ||
				(x == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE)) {
				ApplyResearchForPlayer(player, CST_RSID_TOOL_AGE);
			}
		}
	}
	if (goAge > 1) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		if (player && player->IsCheckSumValid()) {
			auto x = GetResearchStatus(player, CST_RSID_BRONZE_AGE);
			if ((x == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) ||
				(x == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE)) {
				ApplyResearchForPlayer(player, CST_RSID_BRONZE_AGE);
			}
		}
	}
	if (goAge > 2) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		if (player && player->IsCheckSumValid()) {
			auto x = GetResearchStatus(player, CST_RSID_IRON_AGE);
			if ((x == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) ||
				(x == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE)) {
				ApplyResearchForPlayer(player, CST_RSID_IRON_AGE);
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
			int FOR_attempt = ROCKNROR::crInfo.activeConversionAttemptsCount[i];
			int FOR_success = ROCKNROR::crInfo.activeConversionSuccessfulAttemptsCount[i];
			float FOR_pct = FOR_attempt == 0 ? 0 : ((float)FOR_success) / ((float)FOR_attempt);
			int AGAINST_attempt = ROCKNROR::crInfo.passiveConversionAttemptsCount[i];
			int AGAINST_success = ROCKNROR::crInfo.passiveConversionSuccessfulAttemptsCount[i];
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


// UpdatedValue: if <0, it is ignored
void RockNRorCommand::UpdateWorkRateWithMessage(short int DATID, float updatedValue) {
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
void RockNRorCommand::UpdateTechAddWorkRateWithMessage(short int techId, short int unitDefId, float updatedValue) {
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


// This is called during GameSettings Setup handling (after GameSettingsBase.setup, after opening DRS files)
void RockNRorCommand::OnGameSettingsSetup() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) {
		return;
	}
	if (settings->commandLineInfo && ROCKNROR::crInfo.configInfo.disableMusic) {
		// To disable music at startup (prevents from reading the CD in physical device...)
		settings->commandLineInfo->enableMusic = 0;
	}
}


// This is called just after empires.dat is loaded.
// Warning: any change done here is applied on civ definitions are done once and for all, and impact all games.
void RockNRorCommand::OnAfterLoadEmpires_DAT() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global);
	if (!global || !global->IsCheckSumValid()) { return; }

	// Save some original values before they (possibly) change
	ROCKNROR::crInfo.empiresDatTechDefCount = (global->technologiesInfo == NULL) ? 0 : global->technologiesInfo->technologyCount;
	ROCKNROR::crInfo.empiresDatCivCount = global->civCount;

	// Show hidden units in editor
	assert(global->civCount < 256); // it is a short (2 bytes)
	if (ROCKNROR::crInfo.configInfo.showHiddenUnitsInEditor > 0) {
		bool excludeAnnoyingUnits = (ROCKNROR::crInfo.configInfo.showHiddenUnitsInEditor == 1);
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
	if (ROCKNROR::crInfo.configInfo.fixInvisibleTree) {
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
	if (ROCKNROR::crInfo.configInfo.fixVillagerWorkRates && (global->civCount > 1)) {
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
#pragma TODO("Use config for gathering bonus adjustment values")
		const float miningBonus = 0.15f;
		const float lumberjackBonus = 0.11f;
		const float palmyraWorkRateBonus = 0.15f; // enough, even better than tech bonuses. Original = 0.20 for all gatherer types
		// TODO: bonus % is not the same for all villager types (most have 0.45 work rate, lumberjack 0.55, etc)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_STONE_MINING, CST_UNITID_MINERSTONE, miningBonus); // +33% instead of +67% (was +0.3 / 0.45 initial)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_GOLD_MINING, CST_UNITID_MINERGOLD, miningBonus); // +33% instead of +67% (was +0.3 / 0.45 initial)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_SIEGECRAFT, CST_UNITID_MINERSTONE, miningBonus); // +33% instead of +67% (was +0.3 / 0.45 initial). 2nd tech for stone mining (iron age)
		// Wood cutting work rate improvements :
		// WARNING: there is a hardcoded addition of -0.15 (decrease, actually) to phoenician to compensate too big bonus on lumberjack ! See 0x50B883, disabled by RemoveHardcodedCivBonusInit BinarySequence.
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_WOODWORKING, CST_UNITID_LUMBERJACK, lumberjackBonus); // +20% instead of +36% (+0.2 / 0.55 initial)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_ARTISANSHIP, CST_UNITID_LUMBERJACK, lumberjackBonus); // +20% instead of +36% (+0.2 / 0.55 initial)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_CRAFTSMANSHIP, CST_UNITID_LUMBERJACK, lumberjackBonus); // +20% instead of +36% (+0.2 / 0.55 initial)
		this->UpdateTechAddWorkRateWithMessage(CST_TCH_TECH_TREE_PHOENICIAN, CST_UNITID_LUMBERJACK, lumberjackBonus); // +20% instead of +36% (+0.2 / 0.55 initial)
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
						if (ROCKNROR::crInfo.configInfo.unitResourceAmountBerryBush > 0) {
							unitDef->resourceCapacity = ROCKNROR::crInfo.configInfo.unitResourceAmountBerryBush;
							unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSeaFish:
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupShoreFish:
						if (ROCKNROR::crInfo.configInfo.unitResourceAmountFish > 0) {
							unitDef->resourceCapacity = ROCKNROR::crInfo.configInfo.unitResourceAmountFish;
							unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupGoldMine:
						if (ROCKNROR::crInfo.configInfo.unitResourceAmountGoldMine > 0) {
							unitDef->resourceCapacity = ROCKNROR::crInfo.configInfo.unitResourceAmountGoldMine;
							unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupStoneMine:
						if (ROCKNROR::crInfo.configInfo.unitResourceAmountStoneMine > 0) {
							unitDef->resourceCapacity = ROCKNROR::crInfo.configInfo.unitResourceAmountStoneMine;
							unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPreyAnimal:
						if (unitDefId == CST_UNITID_GAZELLE) { // Exclude gazelle king, horse
							if (ROCKNROR::crInfo.configInfo.unitResourceAmountGazelle > 0) {
								unitDef->resourceCapacity = ROCKNROR::crInfo.configInfo.unitResourceAmountGazelle;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPredatorAnimal:
						if (unitDefId == CST_UNITID_LION) {
							if (ROCKNROR::crInfo.configInfo.unitResourceAmountLion > 0) {
								unitDef->resourceCapacity = ROCKNROR::crInfo.configInfo.unitResourceAmountLion;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						}
						if (unitDefId == CST_UNITID_ALLIGATOR) {
							if (ROCKNROR::crInfo.configInfo.unitResourceAmountAlligator > 0) {
								unitDef->resourceCapacity = ROCKNROR::crInfo.configInfo.unitResourceAmountAlligator;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						}
						if (unitDefId == CST_UNITID_ELEPHANT) {
							if (ROCKNROR::crInfo.configInfo.unitResourceAmountElephant > 0) {
								unitDef->resourceCapacity = ROCKNROR::crInfo.configInfo.unitResourceAmountElephant;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						}
						break;
					case GLOBAL_UNIT_AI_TYPES::TribeAIGroupTree:
						// There is no way finding automatically if trees are "normal" or forest.
						if (TreeUnitIsForest(unitDefId)) {
							// Forest (default 40 wood)
							if (ROCKNROR::crInfo.configInfo.unitResourceAmountTreeForest > 0) {
								unitDef->resourceCapacity = ROCKNROR::crInfo.configInfo.unitResourceAmountTreeForest;
								unitDef->resourceStorageAmount_1 = (float)unitDef->resourceCapacity;
							}
						} else {
							// Basic trees (default 75 wood)
							if (ROCKNROR::crInfo.configInfo.unitResourceAmountTree > 0) {
								unitDef->resourceCapacity = ROCKNROR::crInfo.configInfo.unitResourceAmountTree;
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

	if (!ROCKNROR::crInfo.techTreeAnalyzer.AnalyzeTechTree()) {
		traceMessageHandler.WriteMessage("Failed to analyze tech tree");
	}
	assert(ROCKNROR::crInfo.techTreeAnalyzer.IsReady());
}


// Called when launching a new game. Game info has NOT been loaded yet. Global structure already exists, but be careful with its data...
// Most structures have not been initialized/loaded yet.
// This method should only reset internal data
void RockNRorCommand::OnNewGame(bool isSavedGame) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (settings && settings->IsCheckSumValid()) {
		settings->isSavedGame = isSavedGame; // ensure this information is correctly set for the methods called below
	}

	// Resets internal variables.
	this->InitMyGameInfo();

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }
	// Manage game settings customization
	this->ApplyCustomizationOnRandomGameSettings();
}


// Called on every game start, including RM, DM, campaign/scenario, load game, and even MP games.
// This occurs after everything has been loaded
// Note: normally, OnNewGame(...) has always been called just before.
// Warning: for scenarios, if there is an introduction screen, this method is called at that moment
// (game might not be displayed yet - and some methods may not be used yet)
void RockNRorCommand::OnGameStart() {
	// Make sure human player(s) do NOT have AI flags ON at startup (due to ManageAI feature)
	AOE_METHODS::PLAYER::DisableAIFlagsForHuman();

	// Do not allow using RockNRor on multiplayer games (however, always restore AI flags or human-controlled players will have an active AI !)
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

	// This block fixes some flags if needed: isScenario, isDeathMatch, player->techTreeId, scInfo->fullTechTree
	if (settings->isSavedGame) {
		// Fix IsScenario flag to correct value (not set when a game is *loaded*)
		settings->rgeGameOptions.isScenario = 0; // Default: consider it is not a scenario
		// Guess if it is a scenario from scenario information
		if (scInfo && scInfo->IsCheckSumValid()) {
			if (scInfo->scenarioFileName[0] != 0) {
				// scenarioFileName is reliable: it is always correctly set: when starting a new RM/DM/scenario, loading a saved game, etc
				settings->rgeGameOptions.isScenario = 1;
			}

			// Fix isDeathMatch flag in saved games
			int initialFood = scInfo->startingResources[1][CST_RES_ORDER_FOOD]; // Warning: by default, this is wrong (too) in DM games !!! Requires RockNRor's fix
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
	}
	// For new games (RM/DM) = exclude saved games, exclude campaign/scenario : put some info in scenarioInfo so that they get saved in savegame files.
	if (!settings->isSavedGame && !settings->rgeGameOptions.isScenario && !settings->isCampaign) {
		if (scInfo && settings->allTechs) {
			scInfo->fullTechTree = 1; // this field is saved, but not initialized in standard code (always 0), so we do the init here.
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

	// Warning: settings->allTechs only makes sense for "new game" (RM/DM). settings->allTechs is set to 0 for scenarios (which is good).
	bool allTechs = settings->allTechs || (scInfo && scInfo->fullTechTree);
	ROCKNROR::crInfo.myGameObjects.currentGameHasAllTechs = allTechs; // guarantees that this flag is always consistent with current game.
	if (allTechs) {
		ROCKNROR::crInfo.myGameObjects.doNotApplyHardcodedCivBonus = true; // indicates we must not apply Macedonian conversion resistance, palmyra free trade, etc.
	}
	if (ROCKNROR::crInfo.myGameObjects.doNotApplyHardcodedCivBonus) {
		// If "generate tech tree/civ bonus" option is on, flag the game as "full tech tree"
		// - Tech trees (and other options) have already been applied, so this has no impact on game init
		// - This information will be saved and available in "load game", so that loaded games will know they must NOT apply hardcoded civ bonuses (macedonian conversion resistance, etc)
		scInfo->fullTechTree = 1;
		//settings->allTechs = 1; // TO CONFIRM. impacts game if 'restart' (BUG) !!!
		// We could also set player->techTreeId to -1 so it is consistent for "new games" and "loaded games".
		// This is better as techTreeId is correct OR NOT depending on the situation (normal "all techs" game=incorrect, "generate tech tree/civ bonus"=correct for new game but not when saved)
		for (int i = 0; i < global->playerTotalCount; i++) {
			AOE_STRUCTURES::STRUCT_PLAYER *curPlayer = global->GetPlayerStruct(i);
			assert(curPlayer && curPlayer->IsCheckSumValid());
			if (curPlayer && curPlayer->IsCheckSumValid()) {
				curPlayer->techTreeId = -1;
			}
		}
	}

	if (ROCKNROR::crInfo.myGameObjects.doNotApplyHardcodedCivBonus) {
		// Make sure hardcoded stuff for civs is disabled when running "all techs" games (or "generated tech tree/civ bonus" games)
		// Some of those hardcoded values are handled by ROR code in "all techs" games, but NOT in "generated tech tree/civ bonus" games so we need to check it here.
		int playerTotalCount = global ? global->playerTotalCount : 0;
		for (int i = 0; i < playerTotalCount; i++) {
			AOE_STRUCTURES::STRUCT_PLAYER *curPlayer = global->GetPlayerStruct(i);
			assert(curPlayer && curPlayer->IsCheckSumValid());
			if (curPlayer->civilizationId == CST_CIVID_PHOENICIAN) {
				// Lumberjack work rate -0.15 *harcoded* adjustment has been disabled (binary sequence RemoveHardcodedCivBonusInit)
				// Nothing to do here (see also OnAfterLoadEmpires_DAT for work rates adjustments)
			}
			if (curPlayer->civilizationId == CST_CIVID_PALMYRA) {
				// Free tribute cost bonus is "canceled" in 0x45B975 when all techs flag is ON (already OK in original code)
				// This cancellation is useless if empires.dat is fixed: set palmyra civ resource46 to 0.25 like others (an effect exists in palmyra tech tree to set this to 0)
				// Hardcoded bonus on trading efficiency is fixed in ShouldNotApplyPalmyraTradingBonus using crInfo.myGameObjects.doNotApplyHardcodedCivBonus
			}
			if (curPlayer->civilizationId == CST_CIVID_MACEDONIAN) {
				// We CAN'T fix hardcoded conversion resistance here - but RockNRor will use crInfo.myGameObjects.doNotApplyHardcodedCivBonus flag.
			}
			if (curPlayer->civilizationId == CST_CIVID_SHANG) {
				// reduced cost for villager is handled by a tech tree effect from emipres.dat, the hardcoded part has been disabled (binary sequence RemoveHardcodedCivBonusInit) 
				// initial food penalty (to compensate villager cost bonus) is NOT applied when all techs flag is ON (already OK in original code)
				// initial food penalty in scenario : we must NOT apply it ! (already ok in original code)
			}
		}
	} else {
		// For shang, ensure the -40 initial food value effect is applied => done in ApplyCustomizationOnRandomGameStart method
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

	// Manage game settings customization for new random games (including start scenario game)
	this->ApplyCustomizationOnRandomGameStart();

	// Fix Initial resources in Scenario information (after applying customization, because it includes setting initial resource values.
	if (!settings->isSavedGame && global) {
		if (scInfo && scInfo->IsCheckSumValid()) {
			for (int playerIndex = 0; playerIndex < _countof(scInfo->startingResources); playerIndex++) {
				AOE_STRUCTURES::STRUCT_PLAYER *player = global->GetPlayerStruct(playerIndex + 1);
				if (player && player->IsCheckSumValid()) {
					scInfo->startingResources[playerIndex][SCENARIO_INFO_RESOURCE_TYPE::WRT_FOOD] = (long int)player->GetResourceValue(CST_RES_ORDER_FOOD);
					scInfo->startingResources[playerIndex][SCENARIO_INFO_RESOURCE_TYPE::WRT_WOOD] = (long int)player->GetResourceValue(CST_RES_ORDER_WOOD);
					scInfo->startingResources[playerIndex][SCENARIO_INFO_RESOURCE_TYPE::WRT_STONE] = (long int)player->GetResourceValue(CST_RES_ORDER_STONE);
					scInfo->startingResources[playerIndex][SCENARIO_INFO_RESOURCE_TYPE::WRT_GOLD] = (long int)player->GetResourceValue(CST_RES_ORDER_GOLD);
				}
			}
		}
	}

	// Create missing Gaia unit classes (copy from player1)
	AOE_STRUCTURES::STRUCT_PLAYER *gaia = global->GetPlayerStruct(0);
	AOE_STRUCTURES::STRUCT_PLAYER *player1 = global->GetPlayerStruct(1);
	if (gaia && gaia->IsCheckSumValid() && ROCKNROR::crInfo.configInfo.addMissingGaiaVillagerUnits) {
		short int unitDefIds[] = { AOE_CONST_FUNC::CST_UNITID_BUILDER, AOE_CONST_FUNC::CST_UNITID_EXPLORER,
			AOE_CONST_FUNC::CST_UNITID_FARMER, AOE_CONST_FUNC::CST_UNITID_FISHERMAN,
			AOE_CONST_FUNC::CST_UNITID_FORAGER, AOE_CONST_FUNC::CST_UNITID_HUNTER,
			AOE_CONST_FUNC::CST_UNITID_LUMBERJACK, AOE_CONST_FUNC::CST_UNITID_MAN, AOE_CONST_FUNC::CST_UNITID_MAN2,
			AOE_CONST_FUNC::CST_UNITID_MINERGOLD, AOE_CONST_FUNC::CST_UNITID_MINERSTONE,
			AOE_CONST_FUNC::CST_UNITID_REPAIRMAN
		};
		for each (short int unitDefId in unitDefIds) {
			// If the ID is in range and does not already exist, then create it
			if ((unitDefId < gaia->structDefUnitArraySize) && (gaia->GetUnitDefBase(unitDefId) == NULL)) {
				// Create a copy (new object with all necessary dependencies) of unit definition, 
				// so when gaia player destructor runs, it will free it just fine, like other unit definitions
				STRUCT_UNITDEF_BASE *newUnitDef = AOE_STRUCTURES::CopyUnitDefToNewUsingGoodClass(player1->GetUnitDefBase(unitDefId));
				if (newUnitDef) {
					std::string msg = "Added unit definition for gaia : unitDefId #";
					msg += std::to_string(unitDefId);
					traceMessageHandler.WriteMessageNoNotification(msg.c_str());
					gaia->ptrStructDefUnitTable[unitDefId] = newUnitDef;
				}
			}
		}
	}

	// Initialize custom AI objects (technical init, before loading RockNRor savegame data)
	CUSTOM_AI::customAIHandler.GameStartInit();

	// Load RockNRor game data, if this is a saved game and if the rnr savegame file exists.
	if (settings->isSavedGame) {
		loadRockNRorGameDataNoExt(settings->loadGameName);
	}

	// Triggers
	ROCKNROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_GAME_START);

	// Special trigger: disable units for house, villager, fishing ship, trade ship (exceptions that don't work because of techs that do several things)
	// For farm, see notify event method (managed on market construction notification).
	ROCKNROR::TRIGGER::ManageTriggerDisableUnitsForExceptions();

	AOE_STRUCTURES::STRUCT_UI_EASY_PANEL *currentPanel = AOE_STRUCTURES::GetCurrentScreen();
	if (currentPanel && (currentPanel->checksum == CHECKSUM_UI_IN_GAME_MAIN)) {
		// REQUIRES game UI to be active
		if (!ROCKNROR::crInfo.configInfo.hideWelcomeMessage && !settings->rgeGameOptions.isMultiPlayer) {
			std::string msg = localizationHandler.GetTranslation(CRLANG_ID_WELCOME1, "Welcome. " MOD_NAME);
			msg += " ";
			msg += VER_FILE_VERSION_STR;
			msg += " ";
			msg += localizationHandler.GetTranslation(CRLANG_ID_WELCOME2, "plugin is active.");
			AOE_METHODS::CallWriteText(msg.c_str());
		}

		// Display civilization bonus if using random civ bonus/tech tree
		if (global && (global->humanPlayerId >= 0) && (global->humanPlayerId < 9) && !settings->rgeGameOptions.isMultiPlayer &&
			!settings->rgeGameOptions.isScenario && !settings->isCampaign && !settings->isSavedGame
			) {
			auto listForHumanPlayer = ROCKNROR::crInfo.myGameObjects.civBonusInGameTextLinesByPlayerId[global->humanPlayerId];
			for (auto it = listForHumanPlayer.cbegin(); it != listForHumanPlayer.cend(); it++) {
				short int textIndex = *it;
				std::string text = ROCKNROR::crInfo.myGameObjects.inGameTextHandler.GetText(textIndex);
				AOE_METHODS::CallWriteText(text.c_str());
			}
		}

		// Show automatically "F11" information at game startup
		if (!settings->rgeGameOptions.isMultiPlayer) {
			AOE_METHODS::UI_BASE::ShowF11_zone();
			AOE_METHODS::ShowScores(true);
		}
	}

	// Force shared exploration => always ON (if config says so and not in MP)
	// Does not impact scenarios
	if (!settings->isCampaign && !settings->rgeGameOptions.isScenario) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
		if (ROCKNROR::crInfo.configInfo.allyExplorationIsAlwaysShared &&
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
// This method is called but does no customization for scenario/campaign/load saved game.
// Warning: most game structures are not available yet ! It is recommended to only work with STRUCT_GAME_SETTINGS, nothing else.
// However GLOBAL structure (always) already exists and DAT file HAS (always) already been loaded into internal data.
// Return false if failed.
bool RockNRorCommand::ApplyCustomizationOnRandomGameSettings() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	if (settings->isCampaign || settings->isSavedGame || settings->rgeGameOptions.isScenario) {
		// Collect info about (standard) tech trees
		this->customCivHandler.CreateInternalDataForGameWithStandardCivs(true);
		return false;
	}

	// Do custom stuff on "settings" here...

	bool resetCustomCivInfo = true;
	if (ROCKNROR::crInfo.configInfo.randomTechTreeForRMGames &&
		!settings->isSavedGame && !settings->isCampaign && !settings->rgeGameOptions.isScenario && !settings->rgeGameOptions.isMultiPlayer) {
		// Generate random tech tree and civ bonus for each player
		this->customCivHandler.CreateFakeRandomCivsForAllPlayers();
		resetCustomCivInfo = false;
	}
	// Collect info about (standard) tech trees
	this->customCivHandler.CreateInternalDataForGameWithStandardCivs(resetCustomCivInfo);

	return true;
}


// Does all custom stuff on random maps / deathmatches at game start : custom personality values, strategy, initial resources, etc.
// These are changes that are applied when game is loaded (do not interfere here with settings like map size, etc)
// Does NOT apply to scenario/campaign/load saved game.
// Note: when this is called, TECH tree/initial age researches have ALREADY been applied.
// Return false if failed.
bool RockNRorCommand::ApplyCustomizationOnRandomGameStart() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return false; }
	
	// Check game type : allow only random map & deathmatch
	if (settings->isCampaign || settings->isSavedGame) {
		return true;
	}

	if (settings->rgeGameOptions.isScenario) {
		for (int i = 0; i <= settings->rgeGameOptions.playerCountWithoutGaia; i++) {
			if (global->scenarioInformation && (global->scenarioInformation->personalityFileSize[i] <= 0)) {
				// Player does not have a specified PERsonality, which means it is initialized as a Random Map.
				// => We can apply fixes on PER numbers.
				ROCKNROR::PLAYER::ApplySNNumberCustomizationOnPlayer(global->GetPlayerStruct(i));
			}
			if (global->scenarioInformation && (global->scenarioInformation->strategyFileSize[i] <= 0)) {
				// Player does not have a specified strategy, which means it is initialized as a Random Map.
				// => We can apply custom strategy generation.
				ROCKNROR::PLAYER::ApplyStrategyGenerationOnPlayer(GetPlayerStruct(i), this->customCivHandler.GetCustomPlayerInfo(i));
			}
		}
		return false;
	}

#pragma message("Customization on random game start: MP not supported")
	if (settings->rgeGameOptions.isMultiPlayer) {
		return false;
	}
	bool isDM = (settings->isDeathMatch != 0);

	if (ROCKNROR::crInfo.configInfo.noWalls) {
		this->DisableWalls();
	}

	// Initial resources : read from config & apply to game
	if (ROCKNROR::crInfo.configInfo.initialResourcesEnabled) {
		long int initialResources[4] = { 200, 200, 150, 0 };
		if (isDM) {
			for (int i = 0; i < 4; i++) {
				initialResources[i] = ROCKNROR::crInfo.configInfo.initialResources_DM[i];
			}
		}
		else {
			int choice = settings->initialResourcesChoice;
			if ((choice < 0) || (choice > 3)) { choice = 0; } // Make sure we have a valid choice 0-3
			for (int i = 0; i < 4; i++) {
				initialResources[i] = ROCKNROR::crInfo.configInfo.initialResourcesByChoice_RM[choice][i];
			}
		}
		for (long int playerId = 1; playerId <= settings->rgeGameOptions.playerCountWithoutGaia; playerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
			for (int rt = 0; rt < 4; rt++) {
				player->SetResourceValue((AOE_CONST_FUNC::RESOURCE_TYPES) rt, (float)initialResources[rt]);
			}
			if (!ROCKNROR::crInfo.myGameObjects.doNotApplyHardcodedCivBonus && !isDM) {
				// After setting initial resources, re-apply shang food penalty to match original code (RM only, if NOT all techs)
				if (player->civilizationId == CST_CIVID_SHANG) {
					player->SetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_FOOD,
						player->GetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_FOOD) - 40);
				}
			}
		}
	}

	// SN Numbers (update both strategyAI and tacAI)
	for (long int playerId = 1; playerId <= settings->rgeGameOptions.playerCountWithoutGaia; playerId++) {
		ROCKNROR::PLAYER::ApplySNNumberCustomizationOnPlayer(global->GetPlayerStruct(playerId));
	}

	// Initial diplomacy (default=enemy, not neutral) - only if enabled in config
	if (ROCKNROR::crInfo.configInfo.noNeutralInitialDiplomacy) {
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
								player->ptrAIStruct->structDiplAI.isChangeable[otherPlayerId] = 0;
							}
						}
					}
				}
			}
		}
	}

	// Strategy
	for (long int playerId = 1; playerId <= settings->rgeGameOptions.playerCountWithoutGaia; playerId++) {
		ROCKNROR::PLAYER::ApplyStrategyGenerationOnPlayer(GetPlayerStruct(playerId), this->customCivHandler.GetCustomPlayerInfo(playerId));
	}

	// Custom civs
	if (!this->customCivHandler.lastGenerationSummary.empty()) {
		this->customCivHandler.WriteSummaryToScenarioInstructions();
	}

	// Apply maximum age to all players (note: do nothing if max age is iron)
	if ((!settings->isCampaign && !settings->isSavedGame && !settings->rgeGameOptions.isScenario) &&
		(ROCKNROR::crInfo.configInfo.maxAgeRM_DM >= AOE_CONST_FUNC::CST_RSID_STONE_AGE) &&
		(ROCKNROR::crInfo.configInfo.maxAgeRM_DM < AOE_CONST_FUNC::CST_RSID_IRON_AGE)) {
		for (short int ageToDisable = (short int)(ROCKNROR::crInfo.configInfo.maxAgeRM_DM + 1); ageToDisable <= AOE_CONST_FUNC::CST_RSID_IRON_AGE; ageToDisable++) {
			for (long int playerId = 1; playerId <= settings->rgeGameOptions.playerCountWithoutGaia; playerId++) {
				AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
				if (!player || !player->IsCheckSumValid()) {
					continue;
				}
				STRUCT_PLAYER_RESEARCH_STATUS *s = player->GetResearchStatus(ageToDisable);
				if ((s->currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE) ||
					(s->currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT)) {
					s->currentStatus = RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED;
				}
			}
		}
	}

	return true;
}


// Initialize internal game-specific variables (called on each game start/load)
void RockNRorCommand::InitMyGameInfo() {
	ROCKNROR::crInfo.ResetVariables();
	CUSTOM_AI::customAIHandler.ResetAllInfo(); // Reset all CusotmROR AI internal structures/info
	ROCKNROR::unitExtensionHandler.ResetAllInfo();
	
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }
	// Prevent 0% speed at game startup (occurs because of rounding in registry saved value)
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (global && global->IsCheckSumValid() && (global->gameSpeed == 0)) {
		global->gameSpeed = 1;
	}
}


// This fixes needGameStartAIInit flag for all players (useful for loaded games)
void RockNRorCommand::FixGameStartAIInitForPlayers() {
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
bool RockNRorCommand::ManageAIFileSelectionForPlayer(char civilizationId, char *aiFileBuffer) {
	bool isStandardCiv = (civilizationId > 0) && (civilizationId <= CIVILIZATIONS::CST_CIVID_STANDARD_MAX);
	assert(aiFileBuffer != NULL);
	if (!aiFileBuffer) { return false; }

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *gameSettings = GetGameSettingsPtr();
	assert(gameSettings != NULL);
	CivilizationInfo *civ = ROCKNROR::crInfo.configInfo.GetCivInfo(civilizationId);
	std::string s = "";
	int size = 0;
	int randomChoice = 0;
	if (gameSettings->isDeathMatch) {
		if (civ) {
			switch (gameSettings->mapTypeChoice) {
			case 0:
			case 1:
				// Much water
				size = civ->deathmatch_water_AI_file.size();
				if (size > 0) {
					randomChoice = randomizer.GetRandomValue(0, size - 1);
					s = std::string(civ->deathmatch_water_AI_file.at(randomChoice));
				}
				break;
			case 4:
			case 7:
				size = civ->deathmatch_AI_file.size();
				if (size > 0) {
					randomChoice = randomizer.GetRandomValue(0, size - 1);
					s = std::string(civ->deathmatch_AI_file.at(randomChoice));
				}
				break;
			default:
				// Some water
				size = civ->deathmatch_water_AI_file.size();
				if (size > 0) {
					randomChoice = randomizer.GetRandomValue(0, size - 1);
					s = std::string(civ->deathmatch_water_AI_file.at(randomChoice));
				}
			}
			strcpy_s(aiFileBuffer, 255, s.c_str());
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
	if (s.empty() && isStandardCiv) {
		// Using a standard civ, there is no "overload" in config: let standard code run and choose AI file.
		return false; // continue "player.chooseAIFileName" function normally
	}

	// Crucial check: The game freezes if we choose an invalid file, because it will call this again and again.
	char bufFileName[512];
	sprintf_s(bufFileName, "%s\\%s", gameSettings->gameDirFullPath, aiFileBuffer);
	if (!CheckFileExistence(bufFileName)) {
		std::string msg = std::string("Error (strategy): Could not find file: ") + bufFileName;
		traceMessageHandler.WriteMessage(msg);
		*aiFileBuffer = 0; // clear filename because it's invalid.
	}
	return true;
}


// Process event when (human) player discovers a unit. This is where gaia units are captured by human player.
// This happens only once ! Once an objects has been discovered, this event is never triggered again.
// Return true if the unit must NOT be captured
// Default: return false (normal ROR treatments)
bool RockNRorCommand::HumanSpecific_onCapturableUnitSeen(AOE_STRUCTURES::STRUCT_UNIT_BASE *beingSeenUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer) {
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
	ROCKNROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_DISCOVER_GAIA_UNIT, evtInfo);

	if (ROCKNROR::crInfo.configInfo.fixHumanPlayer_specificSeeUnit &&
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
bool RockNRorCommand::FindIfGameStartStrategyInitHasBeenDone(AOE_STRUCTURES::STRUCT_PLAYER *player) {
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
bool RockNRorCommand::ManageTacAIUpdate(AOE_STRUCTURES::STRUCT_AI *ai) {
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
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return true; }

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
void RockNRorCommand::AfterAddElementInStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, 
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
		ROCKNROR::STRATEGY::UpdateStrategyWithUnreferencedExistingUnits(buildAI, newElem->unitDAT_ID, newElem->elementType);

		// Granary / SP that are inserted at very beginning of strategy: move it after first house.
		if (((newElem->unitDAT_ID == CST_UNITID_GRANARY) || (newElem->unitDAT_ID == CST_UNITID_STORAGE_PIT)) && (insertionPos < 2)) {
			// Warning: this may move strategy element !
			ROCKNROR::STRATEGY::MoveStrategyElement_after_ifWrongOrder(buildAI, CST_UNITID_HOUSE, TAIUnitClass::AIUCBuilding, newElem->unitDAT_ID, TAIUnitClass::AIUCNone);
		}
		curElem = curElem->next; // We are sure curElem did not move, use this (newElem may have been moved forward in strategy)
	}
}


void RockNRorCommand::PrintDateTime() {
	time_t rawtime;
	char timebuf[50];
	char timebuf2[50];
	time(&rawtime);
	ctime_s(timebuf2, 30, &rawtime);
	sprintf_s(timebuf, "Time=%s", timebuf2, 40);
	AOE_METHODS::CallWriteText(timebuf);
}


void RockNRorCommand::PrintMapSeed() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	char text[50];
	sprintf_s(text, 50, "Map seed=%ld.", settings->actualMapSeed);
	AOE_METHODS::CallWriteText(text);
}


// This is called very frequently when game is running. Not called if game is paused or stuck.
void RockNRorCommand::OnGameTimer() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global != NULL);
	if (!global) { return; }
	long int currentGameTime = global->currentGameTime / 1000;
	if ((ROCKNROR::crInfo.configInfo.dislikeComputeInterval > 0) &&
		(currentGameTime >= (ROCKNROR::crInfo.LastDislikeValuesComputationTime_second + ROCKNROR::crInfo.configInfo.dislikeComputeInterval))) {
		ROCKNROR::crInfo.LastDislikeValuesComputationTime_second = currentGameTime;
		CUSTOM_AI::playerTargetingHandler.ComputeDislikeValues();
	}

	// Manage triggers
	if (ROCKNROR::crInfo.triggersLastCheckTime_s + 1 <= currentGameTime) {
		ROCKNROR::crInfo.triggersLastCheckTime_s = currentGameTime;

		// Timer trigger
		CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
		::memset(&evtInfo, -1, sizeof(evtInfo));
		evtInfo.currentGameTime_s = currentGameTime;
		ROCKNROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_TIMER, evtInfo);

		// "Passive" triggers (we need to test their criteria regulary)

		// Trigger type: Reached some resource value...?
		::memset(&evtInfo, -1, sizeof(evtInfo)); // Reset event info
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
					ROCKNROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_RESOURCE_VALUE_MORE_THAN, evtInfo);
					ROCKNROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_RESOURCE_VALUE_LESS_THAN, evtInfo);
				}
			}
		}
	}

	// If there are pending messages, show them
	if (traceMessageHandler.HasUnreadMessages() && ROCKNROR::crInfo.configInfo.showRockNRorNotifications) {
		bool isSuccess = ROCKNROR::UI::SimpleEditTextPopup::OpenTraceMessagePopup();
		// If popup couldn't be opened, maybe there is already another one... The message popup will show up when current one is closed (mark as unread).
		traceMessageHandler.MarkAsRead(isSuccess);
	}

	// Other RockNRor "timer" treatments: do them only once every second maximum (for performance)
	if (ROCKNROR::crInfo.lastRockNRorTimeExecution_gameTime_s + 1 <= currentGameTime) {
		ROCKNROR::crInfo.lastRockNRorTimeExecution_gameTime_s = currentGameTime;
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
void RockNRorCommand::OnLivingUnitCreation(AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS UIStatus, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit,
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
	if (isInGameSpawnUnit && ROCKNROR::IsRpgModeEnabled()) {
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
	if (!player->IsAIActive(ROCKNROR::crInfo.hasManageAIFeatureON)) {
		ROCKNROR::UNIT::AutoAssignShortcutToUnit(unit);
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
		parentInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(parentUnitId); // Result can be NULL (not found)
	}

	bool commandCreated = false; // To make sure we create only 1 command (or 0)

	// Auto target
	if (ROCKNROR::crInfo.configInfo.enableSpawnUnitsAutoTarget && !player->IsAIActive(ROCKNROR::crInfo.hasManageAIFeatureON) &&
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
					canInteractWithTarget = canInteractWithTarget && IsUnitPositionKnown(player, target);
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
				ROCKNROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(parentInfo->unitId);
			}
		}
	}

	// Move spawned unit to building's target location if we have/found one. And if option is enabled.
	if (!commandCreated && ROCKNROR::crInfo.configInfo.enableSpawnUnitsMoveToLocation && !player->IsAIActive(ROCKNROR::crInfo.hasManageAIFeatureON) &&
		parentInfo && (parentInfo->spawnUnitMoveToPosX >= 0) && (parentInfo->spawnUnitMoveToPosY >= 0)) {
		MoveUnitToTargetOrPosition(unit, NULL, parentInfo->spawnUnitMoveToPosX, parentInfo->spawnUnitMoveToPosY);
		commandCreated = true;
	}

	// Auto-repair TC for villagers
	if (!commandCreated && ROCKNROR::crInfo.configInfo.enableSpawnUnitAutoRepairTC && IsVillager(unit->unitDefinition->DAT_ID1) &&
		parentUnit && !player->IsAIActive(ROCKNROR::crInfo.hasManageAIFeatureON)) {
		if (parentUnit->remainingHitPoints < (float)parentUnit->unitDefinition->totalHitPoints) {
			TellUnitToInteractWithTarget(unit, parentUnit);
			commandCreated = true;
		}
	}
}


// Fixes missing treatments when a unit changes owner (eg. unit conversion)
// This is called BEFORE the actual unit owner change process is called. (this is called at the very beginning of unit conversion process)
// targetUnit is the "victim" (unit that changes owner), actorPlayer is the new owner (player)
// Target (converted) unit may have 0 HP, and be gonna die: this happens rarely, but is not an error.
// Technical note: in ROR, unit.changeOwner(newplayer) is [EDX+0x44] call.
// TODO: this is not called when relics/ruins are taken ?
void RockNRorCommand::OnUnitChangeOwner_fixes(AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer) {
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

	// Handle internal structures
	ROCKNROR::crInfo.myGameObjects.RemoveAllInfoForUnit(targetUnit->unitInstanceId, targetUnit->positionX, targetUnit->positionY, true);

	// If the unit benefits from a civ bonus, make sure to save this information
	// Note : does not apply to "non-transmissible" civ bonuses (cf nonTransmissibleBonusInGameTextByPlayerAndUnitDefId)
	if (targetUnit->DerivesFromTrainable()) {
		STRUCT_UNIT_TRAINABLE *unitAsTrainable = (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE*)targetUnit;
		if (!unitAsTrainable->hasDedicatedUnitDef) {
			UnitCustomInfo *info = ROCKNROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(targetUnit->unitInstanceId);
			if (ROCKNROR::crInfo.myGameObjects.HasUnitDefBonusTextInfo(targetPlayer->playerId, targetUnit->unitDefinition->DAT_ID1)) {
				int bonusTextIndex = ROCKNROR::crInfo.myGameObjects.bonusInGameTextByPlayerAndUnitDefId[targetPlayer->playerId]
					[targetUnit->unitDefinition->DAT_ID1];
				if (bonusTextIndex >= 0) {
					if (info && (info->bonusTextIndex < 0)) { // Do not overwrite if there is already some text
						info->bonusTextIndex = bonusTextIndex;
					}
				}
			}
			if (info) {
				info->SetCreatorIfUnknown(targetPlayer->playerId);
			}
		}
	}

	// Adapt strategy for "actor" player
	AOE_STRUCTURES::STRUCT_AI *mainAI_actor = actorPlayer->ptrAIStruct;
	if (mainAI_actor == NULL) { return; }
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI_actor = &mainAI_actor->structBuildAI;
	assert(buildAI_actor != NULL);
	if (buildAI_actor == NULL) { return; }
	STRATEGY::UpdateStrategyWithExistingUnit(buildAI_actor, targetUnit);

	long int convertedUnitId = targetUnit->unitInstanceId;
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();

	if (targetPlayer && IsImproveAIEnabled(targetPlayer->playerId) && targetPlayer->ptrAIStruct && targetPlayer->ptrAIStruct->IsCheckSumValid()) {
		// Notify custom AI that a conversion occurred.
		if (CUSTOM_AI::customAIHandler.IsAliveAI(targetPlayer->playerId) && IsImproveAIEnabled(targetPlayer->playerId)) {
			if (IsArtefactOrFlag(targetUnitDef->unitAIType)) {
				// TODO: actually this case never occurs yet, this method is not called in such event.
				CUSTOM_AI::customAIHandler.GetCustomPlayerAI(targetPlayer->playerId)->OnArtefactControlLoss(targetUnit, actorPlayer);
			} else {
				CUSTOM_AI::customAIHandler.GetCustomPlayerAI(targetPlayer->playerId)->OnUnitConverted(targetUnit, actorPlayer);
			}
		}

		// Fix unit memory items that refer to that unit, for both players + all their allied
		// Beware: in conversion process, "RockNRorInstance::ManageOnPlayerAddUnit" is called (0x4F2AAB), which will also update some unit memory items for that unit
		for (int i = 1; (global != nullptr) && (i < global->playerTotalCount); i++) {
			// Is current player allied with one of impacted players ? Note : x<=CST_PDV_ALLY can be gaia, self or ally. Ignore gaia here as i>0.
			// COMMENTED OUT: keeping invalid playerIds in infAI data is a pain and leads to issues. For now, "cheat" on this case (this does not really benefits to AI, but mostly avoid issues)
			/*if ((AOE_STRUCTURES::PLAYER::GetDiplomacyValueForPlayer(targetPlayer, i) <= PLAYER_DIPLOMACY_VALUES::CST_PDV_ALLY) ||
				(AOE_STRUCTURES::PLAYER::GetDiplomacyValueForPlayer(actorPlayer, i) <= PLAYER_DIPLOMACY_VALUES::CST_PDV_ALLY)
			) {*/
				// For all allied players, update owner id in "unit memory". Consider AI players "talk" to give this information. This avoids bugs trying to target allied/owned units...
				STRUCT_UNIT_MEMORY *mem = ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(convertedUnitId, i);
				if (mem) {
					mem->playerId = (char)actorPlayer->playerId;
				}
			//}
		}

		// Fix back-reference (if any) in target unit's activity
		if (targetUnit->currentActivity) {
			STRUCT_UNIT_BASE *targetTargetUnit = GetUnitStruct(targetUnit->currentActivity->targetUnitId);
			if (targetTargetUnit && targetTargetUnit->IsCheckSumValidForAUnitClass()) {
				// Only if target's target has an activity ! Here it could be a tree, a mine, etc (for example if targetUnit is a villager)
				if (targetTargetUnit->currentActivity && targetTargetUnit->currentActivity->IsCheckSumValid()) {
					targetTargetUnit->currentActivity->unitIDsThatAttackMe.Remove(convertedUnitId);
				}
			}
		}
		STRUCT_ACTION_BASE *unitAction = AOE_STRUCTURES::GetUnitAction(targetUnit);
		if (unitAction && unitAction->targetUnit && unitAction->targetUnit->currentActivity) {
			// Remove converted unit's ID from converted unit's target unit's activity information
			unitAction->targetUnit->currentActivity->unitIDsThatAttackMe.Remove(convertedUnitId);
		}
	}
}


// Change a unit's owner, for example like a conversion.
// Capturing an artefact does NOT call this.
// I don't see any other possible event than CST_ATI_CONVERT. Use CST_GET_INVALID to trigger NO notification.
bool RockNRorCommand::ChangeUnitOwner(AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit, AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer,
	AOE_CONST_INTERNAL::GAME_EVENT_TYPE notifyEvent) {
	if (!targetUnit || !actorPlayer || !targetUnit->IsCheckSumValidForAUnitClass() || !actorPlayer->IsCheckSumValid()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_PLAYER *oldOwner = targetUnit->ptrStructPlayer;
	this->OnUnitChangeOwner_fixes(targetUnit, actorPlayer);
	AOE_METHODS::UNIT::ChangeUnitOwner(targetUnit, actorPlayer);

	if ((notifyEvent == GAME_EVENT_TYPE::EVENT_INVALID) || (!oldOwner || !oldOwner->IsCheckSumValid())) {
		return true; // No notification to handle
	}

	// Not handled: other events than conversion (is there any ?)

	// Warning, for a conversion, arg2= actor playerId (priest), arg3=victim playerId
	AOE_METHODS::CallGameSettingsNotifyEvent(notifyEvent, actorPlayer->playerId, oldOwner->playerId, (long int)targetUnit->positionY, (long int)targetUnit->positionX);
	return true;
}


// Custom Fixes/features on player.addUnit calls.
// player is the NEW owner (for conversion case) and unit->ptrStructPlayer is (still) the OLD owner (will be updated afterwards by ROR)
// Note for conversion : unit is currently (temporarily) unavailable in global->GetUnitStruct(unitId) !
void RockNRorCommand::OnPlayerAddUnitCustomTreatments(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, bool isTempUnit, bool isNotCreatable) {
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
				MOV EAX, pMapInfo;
				MOV ECX, buildingsHeader;
				MOV EDX, unit;
				PUSH EAX; // arg2 = pMapVisibilityInfo
				PUSH EDX; // arg1 = unit
				MOV EAX, 0x004368F0; // playerUnitsHeader.addBuildingToArrays(ptrUnit, arg2_mapinfo)
				CALL EAX;
			}

			unit->ptrStructPlayer = oldUnitPlayer; // Restore "old" player (for conversion, it will be updated a bit later)
		}
	}

	// Force update infAI elem (owner attribute) for players that currently SEE the unit (because unit visibility may not change = no update)
	// For other players, this will be updated when the see it.
	// This is very important for artefacts. Probably useful for other units (avoid having wrong playerId in list), but impacts are to be analyzed.
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return; }
	long int playerTotalCount = global->playerTotalCount;

	// Trick for conversion: again, temporarily set unit's player to "future" one so that our updates are correct
	AOE_STRUCTURES::STRUCT_PLAYER *oldUnitPlayer = unit->ptrStructPlayer;
	unit->ptrStructPlayer = player;

	if ((global->gameRunStatus == 0) && (global->currentGameTime > 0) && (!isTempUnit && !isNotCreatable) &&
		(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING)) {
		for (int curPlayerId = 1; curPlayerId < playerTotalCount; curPlayerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *curPlayer = GetPlayerStruct(curPlayerId);
			if (IsImproveAIEnabled(curPlayerId) && 
				AOE_STRUCTURES::PLAYER::IsFogVisibleForPlayer(curPlayer, (long int)unit->positionX, (long int)unit->positionY)) {
				ROCKNROR::unitExtensionHandler.AddUpdateUnitInInfAILists(unit, curPlayerId);
			}
		}
	}

	unit->ptrStructPlayer = oldUnitPlayer; // Restore "old" player (for conversion, it will be updated a bit later)
}


// Custom Fixes/features on player.removeUnit calls.
// Note: player.RemoveUnit is called BY unit.destructor.
// Here unit status can be 2 (if conversion), 7 (quitting editor?), 8 (in-game dying unit) ? +maybe other values
// Warning: this method is called in many situations, game might NOT be running.
void RockNRorCommand::OnPlayerRemoveUnit(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, bool isTempUnit, bool isNotCreatable) {
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
	bool unitIsDying = isInGame && (unit->unitStatus > 2);

	// Update AI struct unit lists that are never updated by ROR (we choose to do this only if AI improvement is enabled).
	if (isInGame && unitIsDying && player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid() && (unit->unitInstanceId >= 0)) {
		// If unit is dying, remove it from all players infAI elem list
		// Critical for unitExtensions because the unit pointer will become obsolete ! => do this even if improveAI is currently disabled
		// Remark: this is not necessary for conversion, the entry will be updated as soon as the unit is seen again.
		ROCKNROR::unitExtensionHandler.RemoveAllInfAIUnitMemoryForUnit(unit->unitInstanceId);
	}

	if (!isNotCreatable && !isTempUnit) {
		// We have a creatable unit
		if (isBuilding && (unit->unitStatus == AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY)) { // status=2 means it is a conversion (or gaia unit captured)
			// Remove building from player buildings list (bug in original game), this is done in unit destructor but NOT at unit conversion
			// Note that because of this call, the remove operation is done twice (will be done again in destructor). But the 2nd time, it will just do nothing.
			AOE_STRUCTURES::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader = player->ptrBuildingsListHeader;
			AOE_METHODS::LISTS::PlayerBldHeader_RemoveBldFromArrays(buildingsHeader, (STRUCT_UNIT_BASE*)unit);
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
					AOE_METHODS::LISTS::InfAIBuildHistory_setStatus(infAI, posX, posY, unitDef->DAT_ID1, AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS::CST_BHS_REMOVED);
				}
			}
		}

		// If unit is dying: clean AI "unitId" lists: it is useless for lists that only store unit IDs to keep non-existing unitIDs.
		// This allows to avoid getting very large UnitID arrays (especially creatableAndGatherableUnits that can contain thousand of elements without this optimization)
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		assert(global && global->IsCheckSumValid());
		if (isInGame && global && global->IsCheckSumValid() && unitIsDying) {
			// For all valid players, remove unitId from AI lists...
			for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
				AOE_STRUCTURES::STRUCT_PLAYER *loopPlayer = GetPlayerStruct(loopPlayerId);
				if (loopPlayer && loopPlayer->IsCheckSumValid() && loopPlayer->ptrAIStruct && (loopPlayer->aliveStatus == 0) &&
					loopPlayer->ptrAIStruct->IsCheckSumValid() && IsImproveAIEnabled(loopPlayerId)) {
					AOE_STRUCTURES::STRUCT_INF_AI *loopInfAI = &loopPlayer->ptrAIStruct->structInfAI;
					assert(loopInfAI->IsCheckSumValid());
					if (IsClassArtefactOrGatherableOrCreatable(unitDefBase->unitAIType)) {
						loopInfAI->artefactsCreatableGatherableUnits.Remove(unit->unitInstanceId);
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
	bool enableAutoRebuildFarms = ROCKNROR::crInfo.configInfo.GetAutoRebuildFarmConfig(settings->rgeGameOptions.isScenario || settings->isCampaign, settings->isDeathMatch)->enableAutoRebuildFarms;
	if (isInGame && unit && unit->IsCheckSumValidForAUnitClass() && isBuilding && enableAutoRebuildFarms) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
		// If this is a farm, and if I have "farm rebuild info" for this position (not in "not rebuild" mode), then trigger a rebuild.
		if (unitDef && unitDef->IsCheckSumValidForAUnitClass() && (unitDef->DAT_ID1 == CST_UNITID_FARM) && player->ptrGlobalStruct) {
			FarmRebuildInfo *fInfo = ROCKNROR::crInfo.myGameObjects.FindFarmRebuildInfo(unit->positionX, unit->positionY);
			if (fInfo && (fInfo->playerId == player->playerId) && !fInfo->forceNotRebuild &&
				(fInfo->villagerUnitId >= 0) && AOE_STRUCTURES::PLAYER::IsUnitAvailableForPlayer(CST_UNITID_FARM, player)) {
				// As long as we use a game command, it is compatible with multiplayer.
				GAME_COMMANDS::CreateCmd_Build(fInfo->villagerUnitId, CST_UNITID_FARM, fInfo->posX, fInfo->posY);
				fInfo->villagerUnitId = -1;
			}
			if (fInfo && !fInfo->forceNotRebuild && !fInfo->forceRebuild) {
				// Remove farm rebuild info when it was added automatically (none of the force rebuild/not rebuild are set)
				ROCKNROR::crInfo.myGameObjects.RemoveFarmRebuildInfo(unit->positionX, unit->positionY);
			}
		}
	}
	if (isInGame && unit && unit->IsCheckSumValidForAUnitClass() && unitIsDying) {
		ROCKNROR::crInfo.myGameObjects.RemoveUnitCustomInfo(unit->unitInstanceId);
	}

	// Triggers
	if (isInGame && unit && unit->IsCheckSumValidForAUnitClass() && !isTempUnit &&
		!settings->rgeGameOptions.isMultiPlayer && (unit->unitInstanceId >= 0)) {
		CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
		evtInfo.unitId = unit->unitInstanceId;
		ROCKNROR::TRIGGER::ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_UNIT_LOSS, evtInfo);
	}
}


// Custom Fixes/features on player.addUnitToSelection.
// If no unit is selected yet and "this" unit is a building, then display a flag to show the rally point (if any)
void RockNRorCommand::OnPlayerAddUnitToSelection(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	if (!player || !unit || !player->IsCheckSumValid() || !unit->IsCheckSumValidForAUnitClass()) { return; }
	
	if (!ROCKNROR::crInfo.configInfo.enableSpawnUnitsAutoTarget || player->IsAIActive(ROCKNROR::crInfo.hasManageAIFeatureON)) {
		return;
	}

	if (unit->ptrStructPlayer != player) { return; }
	if (player != GetControlledPlayerStruct_Settings()) { return; }

	if (player->selectedUnitCount > 0) {
		return;
	}

	long int screenPosX = 0;
	long int screenPosY = 0;
	if (!AOE_METHODS::UI_BASE::GetScreenPosForUnit(unit, &screenPosX, &screenPosY)) {
		return;
	}

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return; }
	if (settings->currentUIStatus != GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) { return; }

	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *uiGameMain = settings->ptrGameUIStruct;
	if (!uiGameMain) { return; }

	UnitCustomInfo *customInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(unit->unitInstanceId);
	if (customInfo) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *target = GetUnitStruct(customInfo->spawnTargetUnitId);
		if (target && target->IsCheckSumValidForAUnitClass() && IsUnitPositionKnown(player, target)) {
			if (!AOE_METHODS::UI_BASE::GetScreenPosForUnit(unit, &screenPosX, &screenPosY)) {
				return;
			}
			AOE_METHODS::UI_BASE::DisplayGreenBlinkingOnUnit(uiGameMain->gamePlayUIZone, target->unitInstanceId, 1500);
			return;
		} else {
			if (!AOE_METHODS::UI_BASE::GetScreenPosFromGamePos(customInfo->spawnUnitMoveToPosX, customInfo->spawnUnitMoveToPosY,
				&screenPosX, &screenPosY)) {
				return;
			}
		}
	} else {
		return;
	}

	long int drawInterval = 100; // use a custom value (faster) instead of AOE_CONST_DRS::DRAW_INTERVAL_MOVE_RED_CROSS
	AOE_STRUCTURES::STRUCT_SLP_INFO *slp = settings->ptrInfosSLP[AOE_CONST_DRS::AoeInGameFlagsIconId::IGF_MOVETO_INTERMEDIATE_STEP_FLAG];
	AOE_METHODS::UI_BASE::DisplayInGameSign(uiGameMain->gamePlayUIZone, slp, screenPosX, screenPosY, 2, drawInterval); // includes the required refresh on UIGameZone
}


// Returns true if a unit should change target to attack tower, false if it should keep its current activity.
// Default return value is true (for error/invalid cases). Note: it is better to return true in most case to make sure the tower can be destroyed !
// To be used when target unit is a tower in actor's town
// Warning: try to keep this function fast and optimized as much as possible. It may be called quite often.
// The improved algorithm is only used if ImproveAI config is ON.
bool RockNRorCommand::ShouldAttackTower_towerPanic(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *actorUnit, AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyTower) {
	// Run various consistency checks
	assert(ROCKNROR::COMBAT::COMBAT_CONST::distanceToConsiderVeryClose > 0);
	if (!actorUnit || !enemyTower || !actorUnit->ptrStructPlayer) { return true; }
	if (!IsImproveAIEnabled(actorUnit->ptrStructPlayer->playerId)) { return true; } // improve AI is disabled. Return default value.
	assert(actorUnit->currentActivity != NULL);
	assert(actorUnit->unitDefinition != NULL);
	assert(enemyTower->unitDefinition != NULL);
	if ((!actorUnit->currentActivity) || (!actorUnit->unitDefinition) || (!enemyTower->unitDefinition)) { return true; }
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = actorUnit->currentActivity; // Guaranteed non-NULL
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *actorUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)actorUnit->unitDefinition; // Guaranteed non-NULL
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *enemyTowerDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)enemyTower->unitDefinition; // Guaranteed non-NULL
	assert(actorUnitDef->DerivesFromAttackable());
	assert(enemyTowerDef->DerivesFromAttackable());
	if (!actorUnitDef->DerivesFromAttackable() || !enemyTowerDef->DerivesFromAttackable()) { return true; }

	if (activity->currentTaskId == AOE_CONST_INTERNAL::ACTIVITY_TASK_ID::CST_ATI_TASK_CONVERT) {
		return false; // Converting priest: let him proceed. TO DO: improve !
	}

	// Check if tower is mine or allied (it may have been converted)
	if (actorUnit->ptrStructPlayer->ptrDiplomacyStances[enemyTower->ptrStructPlayer->playerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY) {
		if ((activity->targetUnitId == enemyTower->unitInstanceId) &&
			((activity->currentTaskId == AOE_CONST_INTERNAL::ACTIVITY_TASK_ID::CST_ATI_TASK_CONVERT) ||
			(activity->currentTaskId == AOE_CONST_INTERNAL::ACTIVITY_TASK_ID::CST_ATI_TASK_ATTACK)
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
	float actorRange = actorUnitDef->maxRange + ROCKNROR::COMBAT::COMBAT_CONST::distanceToConsiderVeryClose;
	assert(actorRange > 0);

	// All unit types: if tower is at reach AND is has 1 HP, quickly destroy it at any cost ! Even if a priest is converting me
	if ((enemyTower->remainingHitPoints < 2) && (distanceToTower < actorRange)) {
		return true; // Just destroy this tower while it's weak !
	}

	// Actor = catapult: always attack the tower... unless priest is very close AND being converting (not moving - can not escape my shot)
	if (actorUnitDef->DAT_ID1 == CST_UNITID_STONE_THROWER) { // DAT_ID1 : also matches for catapult upgrades
		if ((distanceToCurrentTarget < actorRange) && (currentTargetDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) &&
			(currentTarget->currentActivity) && (currentTarget->currentActivity->currentTaskId == AOE_CONST_INTERNAL::ACTIVITY_TASK_ID::CST_ATI_TASK_CONVERT)) {
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
		if (currentTargetAsBld->IsCheckSumValid() && (activity->currentTaskId == AOE_CONST_INTERNAL::ACTIVITY_TASK_ID::CST_ATI_TASK_BUILD) &&
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
		if ((activity->currentTaskId == AOE_CONST_INTERNAL::ACTIVITY_TASK_ID::CST_ATI_TASK_BUILD) &&
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
void RockNRorCommand::towerPanic_LoopOnVillagers(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyTower,
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
	::memset(squareDistances, -1, CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT * sizeof(long int));
	::memset(orderedUnitIDs, -1, CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT * sizeof(long int));
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
			if ((activity->orderId == AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_ATTACK) ||
				(activity->orderId == AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_GATHER_ATTACK)) {
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
void RockNRorCommand::MoveFireGalleyIconIfNeeded(AOE_STRUCTURES::STRUCT_PLAYER *player) {
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
void RockNRorCommand::MoveFireGalleyIconIfNeeded(short int playerId) {
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
	if (player != NULL) {
		this->MoveFireGalleyIconIfNeeded(player);
	}
}



// Technical fix for a method about elevation application when generating map. Original method contains many bugs.
void RockNRorCommand::Fixed_MapGen_applyElevation(long int posX, long int posY, long int distance, AOE_STRUCTURES::STRUCT_RMM_ELEVATION_GENERATOR *elevInfo) {
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
void RockNRorCommand::DisplayTimerStats() {
	long int result = 0;
	for (int i = 0; i < CST_TIMER_STATS_ARRAY_SIZE; i++) {
		result += ROCKNROR::crInfo.CollectedTimerIntervals_ms[i];
	}
	result = result / CST_TIMER_STATS_ARRAY_SIZE;
	char buf[70];
	sprintf_s(buf, "Avg timer interval is %ld ms - slow down factor is %ld", result, ROCKNROR::crInfo.configInfo.gameTimerSlowDownFactor);
	AOE_METHODS::CallWriteText(buf);
#ifdef _DEBUG
	// Write in log window ALL execution times
	long int curExecCount = 0;
	long int highestTimeSpent = 0;
	unsigned long int curHighest = 0;
	for (auto it = ROCKNROR::crInfo.longestTimes_ms.cbegin(); it != ROCKNROR::crInfo.longestTimes_ms.cend(); it++)
	{
		unsigned long int retAddr = it->first;
		long int timeSpent = it->second;
		if ((timeSpent > highestTimeSpent) && (retAddr != AOE_ROR_API_FIRST_CALL_RETURN_ADDRESS)) {
			highestTimeSpent = timeSpent;
			curHighest = retAddr;
			curExecCount = ROCKNROR::crInfo.executionCounts[retAddr];
		}
		std::string msg = "EntryPoint ";
		msg += GetHexStringAddress(retAddr);
		msg += "  exec_count=";
		msg += std::to_string(ROCKNROR::crInfo.executionCounts[retAddr]);
		msg += "  longest=";
		msg += std::to_string(ROCKNROR::crInfo.longestTimes_ms[retAddr]);
		traceMessageHandler.WriteMessageNoNotification(msg);
	}
	if (curHighest > 0) {
		sprintf_s(buf, "Longest RnR treatment in 0x%08X with %ld ms (total %ld exec)", curHighest, highestTimeSpent, curExecCount);
		AOE_METHODS::CallWriteText(buf);
	}
#endif
}


// This method automatically recalculates gameTimerSlowDownFactor
void RockNRorCommand::AutoFixGameTimer() {
	bool reset = false;
	time_t t = time(0); // get current time
	struct tm now;
	localtime_s(&now, &t);
	if (ROCKNROR::crInfo.LastGameTimerAutoFix_second == now.tm_sec) { return; } // Prevents from doing this more than once per second
	long int result = 0;
	for (int i = 0; i < CST_TIMER_STATS_ARRAY_SIZE; i++) {
		if (ROCKNROR::crInfo.CollectedTimerIntervals_ms[i] < 0) { return; } // does not update if we did not collect enough stats (invalid values remain)
		result += ROCKNROR::crInfo.CollectedTimerIntervals_ms[i];
	}
	result = result / CST_TIMER_STATS_ARRAY_SIZE; // Get average
	// Hardcoded limit. Choose a better value. A variable ?
	if ((result < CST_TIMER_INTERVAL_TOO_FAST) && (ROCKNROR::crInfo.configInfo.gameTimerSlowDownFactor < CST_MAXIMUM_SLOW_DOWN_FACTOR)) {
		ROCKNROR::crInfo.configInfo.gameTimerSlowDownFactor++;
		reset = true;
	}
	if ((result > CST_TIMER_INTERVAL_TOO_SLOW) && (ROCKNROR::crInfo.configInfo.gameTimerSlowDownFactor > 1)) {
		ROCKNROR::crInfo.configInfo.gameTimerSlowDownFactor--;
		reset = true;
	}
	ROCKNROR::crInfo.LastGameTimerAutoFix_second = now.tm_sec;

	if (reset) {
		for (int i = 0; i < CST_TIMER_STATS_ARRAY_SIZE; i++) {
			ROCKNROR::crInfo.CollectedTimerIntervals_ms[i] = -1; // Reset statistics
		}
	}
}


// Generate map (overload) for scenario editor
bool RockNRorCommand::ScenarioEditor_customGenerateMap(long int sizeX, long int sizeY) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) { return false; }
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_METHODS::GetScreenFromName(scenarioEditorScreenName);
	assert(scEditor && scEditor->IsCheckSumValid());
	if (!scEditor || !scEditor->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = scEditor->global;
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	if (!mapInfo || !mapInfo->IsCheckSumValid()) { return false; }

	assert(sizeX > 0); assert(sizeX < AOE_MAX_ALLOWED_MAP_SIZE);
	assert(sizeY > 0); assert(sizeY < AOE_MAX_ALLOWED_MAP_SIZE);
	if ((sizeX <= 0) || (sizeY <= 0) || (sizeX >= AOE_MAX_ALLOWED_MAP_SIZE) || (sizeY >= AOE_MAX_ALLOWED_MAP_SIZE)) { return false; }
	bool isUnsupportedMapSize = (sizeX >= 256) || (sizeY >= 256);
	if (isUnsupportedMapSize && (scEditor->currentMapGenerationChoice != 1)) {
		traceMessageHandler.WriteMessage("For larger maps than 256*256, you can only create empty maps. Keep in mind that adding/moving units can be unstable.");
		return false;
	}
	AOE_STRUCTURES::STRUCT_PLAYER *gaia = NULL;
	if (!isUnsupportedMapSize) {
		// For standard map sizes (less than 256), get gaia player, which allows creation of random decorations on terrain
		// The condition (keeping NULL gaia for larger maps) is a workaround to avoid game crash, 
		// because decorations creation is NOT called when player is NULL. Fortunately, passing NULL player has no other side effects here.
		// Adding decorations involves updating pathing sytem structures, which does not support larger maps.
		// It seems to be the only incompatible treatment done in map creation [as long as we create an empty map]
		gaia = GetPlayerStruct(0);
		assert(gaia && gaia->IsCheckSumValid());
		if (!gaia || !gaia->IsCheckSumValid()) { return false; }
	}

	long int mapType = scEditor->map_cbb_mapType->GetListSelectedIndex();
	assert(scEditor && scEditor->map_edt_seed->IsCheckSumValid());
	char *mapSeedText = AOE_METHODS::UI_BASE::GetEditText(scEditor->map_edt_seed);
	long int terrainId = 0;
	long int mapSeed = -1; // Default = -1 (= random)
	long int playerCount = (scEditor->pl_cbb_playerCount->GetListSelectedIndex()) + 1;

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
		switch (scEditor->map_cbb_defaultTerrain->GetListSelectedIndex()) {
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
					MOV ECX, player;
					MOV EDX, 0x45DB40; // clearSelectedUnits
					CALL EDX;
				}
			}
		}
		// Generate "random" map (with or without seed)
		_asm {
			PUSH playerCount;
			PUSH mapType;
			PUSH sizeX;
			PUSH sizeY;
			PUSH global;
			MOV ECX, mapInfo;
			MOV EDX, DS:[ECX];
			CALL DS:[EDX+0x28];
		}

		settings->scenarioMapSeed = previousScenarioSeed;
		char bufferSeed[10] = "";
		sprintf_s(bufferSeed, "%ld", settings->actualMapSeed);
		assert(scEditor->map_edt_seed_whenReadOnly);
		bool test = scEditor->map_edt_seed_whenReadOnly->IsCheckSumValid();
		AOE_METHODS::UI_BASE::SetLabelText(scEditor->map_edt_seed_whenReadOnly, bufferSeed);
		_asm {
			MOV EDX, 0x52605D;
			CALL EDX; // Recalculate pseudo random
		}
	}

	// Post-generation common actions
	long int posX = sizeX / 2;
	long int posY = sizeY / 2;
	float fposX = (float)posX;
	float fposY = (float)posY;
	// Manage player screen position
	for (int playerId = 0; playerId < 9; playerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		assert(player && player->IsCheckSumValid());
		if (player && player->IsCheckSumValid()) {
			_asm {
				PUSH posX;
				PUSH posY;
				MOV EDX, 0x45D310;
				MOV ECX, player;
				CALL EDX;
				PUSH fposX;
				PUSH fposY;
				MOV EDX, 0x45D330;
				MOV ECX, player;
				CALL EDX;
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
				MOV EDX, 0x45EB40; // resetArray()
				MOV ECX, unknown_struct;
				CALL EDX;
				MOV EDX, 0x45E940; // player.SetVisibilityInfoOn?()
				MOV ECX, player;
				CALL EDX;
			}
		}
	}
	AOE_STRUCTURES::STRUCT_UI_LABEL *bigLbl = scEditor->map_lbl_bigGeneratingMapNotification;
	assert(bigLbl && bigLbl->IsCheckSumValid());
	_asm {
		MOV EDX, 0x4901B0; // UIscenarioEditor.refreshDiamondMap()
		MOV ECX, scEditor;
		CALL EDX;
	}
	AOE_METHODS::UI_BASE::ShowUIObject(bigLbl, false); // Remove the "generating" label
	return true;
}


// Handles all custom treatments at scenario editor UI creation (this is called at the end of standard UI creation).
void RockNRorCommand::CustomScenarioEditorUICreation(AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor) {
	if (!scEditor || !scEditor->IsCheckSumValid()) { return; }

	// Manage hidden terrains
	AOE_STRUCTURES::STRUCT_UI_LISTBOX *listBox = scEditor->trn_lst_terrainList;
	assert(listBox && listBox->IsCheckSumValid());
	if (listBox && listBox->IsCheckSumValid() && ROCKNROR::crInfo.configInfo.showHiddenTerrainsInEditor) {
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
	if (ROCKNROR::crInfo.configInfo.useCustomMapDimensions &&
		scEditor->map_cbb_mapSize && scEditor->map_cbb_mapSize->IsCheckSumValid() &&
		scEditor->map_cbb_mapSize->underlyingListBox && scEditor->map_cbb_mapSize->underlyingListBox->IsCheckSumValid()) {
		AOE_METHODS::UI_BASE::Listbox_addItem(scEditor->map_cbb_mapSize->underlyingListBox,
			scEditor->map_cbb_mapSize->underlyingListBox->itemCount, localizationHandler.GetTranslation(CRLANG_ID_CUSTOM, "Custom"), 0);
	}
}


// This methods modifies provided variables that will be passed to "check unit placement" method before adding a unit in scenario editor.
// For example, to allow creating several units at the same location, force checkConflictingUnits to 0.
void RockNRorCommand::ApplyCustomizationOnEditorAddUnit(long int &checkVisibility, long int &checkHillMode, long int &editorMode, long int &checkAirModeAndHPBar, long int &checkConflictingUnits, bool &IgnoreTerrainRestrictions) {
	checkConflictingUnits = !ROCKNROR::crInfo.configInfo.editor_allowUnitOverlapping;
	checkHillMode = !ROCKNROR::crInfo.configInfo.editor_disableHillModeChecks;
	IgnoreTerrainRestrictions = ROCKNROR::crInfo.configInfo.editor_disableTerrainRestrictions;
}


// Get matching terrainId when user selected a row in terrain list in scenario editor/terrain tab.
// Returns a terrainId, returns -1 if we want to use default behaviour (no custom choice)
long int RockNRorCommand::GetTerrainIdForSelectedTerrainIndex(AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor, long int selectedIndex) {
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
void RockNRorCommand::HandleRORDebugLogCall(unsigned long int firstRORCallTextParamAddress) {
	char **stackArgText = (char**)firstRORCallTextParamAddress;
	std::string msgInitial = "";
	if (*stackArgText) {
		msgInitial = *stackArgText;
	}

	if (msgInitial.length() <= 0) { return; }

	// Filter some messages if option is set (param=1 means filtered)
	if (ROCKNROR::crInfo.configInfo.collectRORDebugLogs == 1) {
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
long int RockNRorCommand::GathererCheckPathFinding(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *actorAsType50, long int *pathFindingArgs) {
	AOE_STRUCTURES::STRUCT_PLAYER *player = actorAsType50->ptrStructPlayer;
	assert(player && player->IsCheckSumValid());
	bool doFix = !ROCKNROR::crInfo.configInfo.doNotApplyFixes; // *True* unless the "no fix" flag is set.

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
void RockNRorCommand::WriteF11PopInfoText(AOE_STRUCTURES::STRUCT_UI_F11_POP_LABEL *f11panel, char *bufferToWrite, char *defaultFormat,
	char *localizedText, long int currentPop, long int houseMaxPop) {
	const int bufferSize = 200; // do not know for sure.
	if (!ROCKNROR::crInfo.configInfo.showCustomPopInfo) {
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
		villCount = AOE_STRUCTURES::PLAYER::GetPlayerUnitCount(player, -1, TribeAIGroupCivilian, 2, 2);
		boatVillCount = AOE_STRUCTURES::PLAYER::GetPlayerUnitCount(player, -1, TribeAIGroupFishingBoat, 2, 2) +
			AOE_STRUCTURES::PLAYER::GetPlayerUnitCount(player, -1, TribeAIGroupTradeBoat, 2, 2) +
			AOE_STRUCTURES::PLAYER::GetPlayerUnitCount(player, -1, TribeAIGroupTransportBoat, 2, 2);
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
void RockNRorCommand::DisableWaterUnitsIfNeeded() {
	if (!ROCKNROR::crInfo.configInfo.noDockInMostlyLandMaps) { return; }
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
void RockNRorCommand::DisableWalls() {
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


// Called at the end of showUnitCommandButtons
void RockNRorCommand::AfterShowUnitCommandButtons(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI) {
	assert(gameMainUI && gameMainUI->IsCheckSumValid());
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		return;
	}
	if (gameMainUI->panelSelectedUnit == NULL) {
		return;
	}
	if (!IsGameRunning() || !ROCKNROR::crInfo.configInfo.useImprovedButtonBar) {
		return;
	}

	BUTTONBAR::AfterShowUnitCommandButtons(gameMainUI);
}


// User interface command handler for 1 single unit.
// isPanelUnit = true if unitBase is the main selected unit (the one visible in bottom-left unit info zone)
// "Common" treatments (interface updates, etc) are only done when isPanelUnit = true
// This checks if the unit belongs to controlled player before running treatements
bool RockNRorCommand::ApplyUserCommandForUnit(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
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
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_DEFEND_STOP) {
		if (unitIsMine && (settings->mouseActionType == MOUSE_ACTION_TYPES::CST_MAT_CR_PROTECT_UNIT_OR_ZONE)) {
			UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(unitBase->unitInstanceId);
			if (unitInfo) {
				unitInfo->ResetProtectInfo();
				ROCKNROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(unitBase->unitInstanceId);
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
bool RockNRorCommand::OnGameCommandButtonClick(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI,
	AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID uiCommandId, long int infoValue) {

	if (!ROCKNROR::crInfo.configInfo.useImprovedButtonBar) {
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
	bool isMultiPlayer = (settings->rgeGameOptions.isMultiPlayer != 0);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *panelUnitBase = NULL;
	AOE_STRUCTURES::STRUCT_PLAYER *player = NULL;
	if (gameMainUI->panelSelectedUnit && gameMainUI->panelSelectedUnit->IsCheckSumValidForAUnitClass()) {
		panelUnitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) gameMainUI->panelSelectedUnit;
		if (panelUnitBase && panelUnitBase->IsCheckSumValidForAUnitClass()) {
			player = panelUnitBase->ptrStructPlayer;
		}
	}
	// No additional actions when viewing a unit that is not mine !
	if (!panelUnitBase || !panelUnitBase->unitDefinition || !player || !player->IsCheckSumValid() ||
		(player != GetControlledPlayerStruct_Settings())) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *panelUnitTrainable = NULL;
	if (panelUnitBase->DerivesFromTrainable()) {
		panelUnitTrainable = (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *)panelUnitBase;
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


	// Handle next page. Note: in ROR, see 0x485140 (for villager build menu only)
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_NEXT_PAGE) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = panelUnitBase->unitDefinition;
		if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) {
			return false;
		}
		// Do not customize villagers... at this point
		if (IsVillager(unitDefBase->DAT_ID1)) {
			return false;
		}
#ifdef GAMEVERSION_ROR10c
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
#else
		return false;
#endif
	}

	// Unit-specific treatments
	bool eventFullyHandled = false;
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = ROCKNROR::crInfo.GetRelevantSelectedUnitsPointer(player);
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
		UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(panelUnitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = true;
		unitInfo->autoAttackPolicy.attackVillagers = false;
		if (!panelUnitTrainable->hasDedicatedUnitDef) {
			unitInfo->SetCreatorIfUnknown(player->playerId);
		}
		flagsToApply.attackVillagers = true; // this flag has been updated
		BUTTONBAR::RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_DONT_ATTACK_BUILDINGS) {
		UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(panelUnitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = true;
		unitInfo->autoAttackPolicy.attackNonTowerBuildings = false;
		if (!panelUnitTrainable->hasDedicatedUnitDef) {
			unitInfo->SetCreatorIfUnknown(player->playerId);
		}
		flagsToApply.attackNonTowerBuildings = true; // this flag has been updated
		BUTTONBAR::RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_NO_AUTO_ATTACK) {
		UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(panelUnitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = true;
		unitInfo->autoAttackPolicy.attackMilitary = false;
		unitInfo->autoAttackPolicy.attackNonTowerBuildings = false;
		unitInfo->autoAttackPolicy.attackVillagers = false;
		unitInfo->autoAttackPolicy.attackTowers = false;
		if (!panelUnitTrainable->hasDedicatedUnitDef) {
			unitInfo->SetCreatorIfUnknown(player->playerId);
		}
		flagsToApply.attackMilitary = true; // this flag has been updated
		flagsToApply.attackNonTowerBuildings = true; // this flag has been updated
		flagsToApply.attackVillagers = true; // this flag has been updated
		flagsToApply.attackTowers = true; // this flag has been updated
		BUTTONBAR::RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_RESET_AUTO_ATTACK) {
		UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(panelUnitBase->unitInstanceId);
		unitInfo->autoAttackPolicyIsSet = false;
		unitInfo->autoAttackPolicy.SetDefaultValues();
		if (!panelUnitTrainable->hasDedicatedUnitDef) {
			unitInfo->SetCreatorIfUnknown(player->playerId);
		}
		flagsToApply.SetAllValues(true); // All flags have been updated.
		ROCKNROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(panelUnitBase->unitInstanceId);
		BUTTONBAR::RefreshCustomAutoAttackButtons(gameMainUI, &unitInfo->autoAttackPolicy);
		updateAutoAttackInfo = true;
	}
	// Apply changes on ALL compatible selected units
	if (updateAutoAttackInfo) {
		UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(panelUnitBase->unitInstanceId);
		if (!panelUnitTrainable->hasDedicatedUnitDef) {
			unitInfo->SetCreatorIfUnknown(player->playerId);
		}
		assert(unitInfo != NULL); // Was just added
		if (!unitInfo) { return false; } // this is an error case
		ROCKNROR::crInfo.ApplyAutoAttackPolicyToPlayerSelectedUnits(player, unitInfo->autoAttackPolicy, flagsToApply);
	}

	// Button "protect unit or protect zone" : set mouse custom cursor type
	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_DEFEND) {
		AOE_METHODS::SetGameCursor(GAME_CURSOR::GC_GROUP);
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		if (settings && settings->IsCheckSumValid()) {
			settings->mouseActionType = MOUSE_ACTION_TYPES::CST_MAT_CR_PROTECT_UNIT_OR_ZONE;
			AOE_METHODS::CallWriteCenteredText(localizationHandler.GetTranslation(CRLANG_ID_BTN_UNIT_SET_PROTECT_OBJECT, "Right-click to define the unit or the position to protect"));
			if (settings->ptrGameUIStruct && settings->ptrGameUIStruct->gamePlayUIZone) {
				UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(panelUnitBase->unitInstanceId);
				if (unitInfo && (unitInfo->protectUnitId > -1)) {
					// If a "protect" order already exists, show it in the UI (=> blink target object)
					AOE_METHODS::DisplayGreenBlinkingOnUnit(settings->ptrGameUIStruct->gamePlayUIZone, unitInfo->protectUnitId, 1000);
				}
			}
		}
		BUTTONBAR::SetButtonBarForDefendUnitOrZone(gameMainUI, (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE*)panelUnitBase);
	}

	if (uiCommandId == AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID::CST_IUC_CROR_GATE_OPEN_CLOSE) {
		if (isMultiPlayer) {
			// Transforming a unit is not MP-compliant
			return true; // Event considered as taken care of
		}
		if (panelUnitBase && panelUnitBase->IsCheckSumValidForAUnitClass() && panelUnitBase->unitDefinition &&
			panelUnitBase->unitDefinition->IsCheckSumValidForAUnitClass()) {
			AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)panelUnitBase->unitDefinition;
			assert(unitDef->IsCheckSumValid());
			char taskSwapGroupId = 0;
			if (unitDef->IsCheckSumValid() && (unitDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupWall)) {
				taskSwapGroupId = unitDef->unitDefinitionSwitchGroupId;
			}
			if (taskSwapGroupId > 0) {
				for (int curUnitDefId = 0; curUnitDefId < player->structDefUnitArraySize; curUnitDefId++) {
					AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *curUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)player->GetUnitDefBase(curUnitDefId);
					if (!curUnitDef || !curUnitDef->IsCheckSumValidForAUnitClass() || !curUnitDef->DerivesFromCommandable()) {
						continue;
					}
					if ((curUnitDefId != unitDef->DAT_ID1) && (taskSwapGroupId == curUnitDef->unitDefinitionSwitchGroupId)) {
						bool compatible = false;
						for (int cmdIndex = 0; cmdIndex < curUnitDef->ptrUnitCommandHeader->commandCount; cmdIndex++) {
							if ((curUnitDef->ptrUnitCommandHeader->ptrCommandArray[cmdIndex]->commandType == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_MAKE_OBJECT) &&
								(curUnitDef->ptrUnitCommandHeader->ptrCommandArray[cmdIndex]->unitDefId == unitDef->DAT_ID1)) {
								// Found a command "make object" on "source unitDefId", this means this is the matching gate object
								// (this is a configuration rule we decided here, of course there is no such rule in original game)
								// (this custom rule allows us to make sure gate objects go by pairs, even if we have unit upgrades with same "taskSwapGroupId")
								compatible = true;
								break;
							}
						}
						if (compatible) {
							// Is it possible to put "target" unitDef ?
							// If a unit is under, it is not allowed to close the gate
							AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION res = AOE_METHODS::GetErrorForUnitCreationAtLocation(player,
								curUnitDef, panelUnitBase->positionY, panelUnitBase->positionX, false, false, false, false, true);
							if (res == AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION::CST_EUC_OK) {
								AOE_METHODS::UNIT::UnitTransform(panelUnitBase, player->GetUnitDefBase(curUnitDefId));
							}
						}
					}
				}
			}
		}
	}

	return false;
}


// Manages the display of a unit shortcut for non-standard shortcuts.
// Returns true if we want to let standard game code execute the shortcut display operation. (default false)
bool RockNRorCommand::DisplayCustomUnitShortcutSymbol(AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase,
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

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return false;
	}

	// Here: unit has a custom shortcut OR shortcut 10 (not displayed in standard game)

	if (ROCKNROR::crInfo.rockNRorIcons.slpSize <= 0) {
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
			AOE_CONST_DRS::CST_ROCKNROR_SLP_INDEX_FOR_UNIT_SHORTCUT_10; // Add offset to position on "shortcut 10" index in SLP file
	} else {
		slpFileIndex = AOE_CONST_DRS::CST_ROCKNROR_SLP_INDEX_FOR_GROUPED_UNIT; // shortcut corresponds to a unit group, not a shortcut
	}
	slpArrayIndex = slpFileIndex - 1;
	if ((slpFileIndex < 0) || (slpFileIndex > AOE_CONST_DRS::CST_ROCKNROR_MAX_SLP_INDEX_IN_UNIT_SHORTCUTS_FILE) ||
		((slpFileIndex > AOE_CONST_DRS::CST_ROCKNROR_MAX_SLP_INDEX_FOR_UNIT_SHORTCUTS) && (slpFileIndex != AOE_CONST_DRS::CST_ROCKNROR_SLP_INDEX_FOR_GROUPED_UNIT))
		) {
		std::string msg = "ERROR: tried to use a wrong Slp.itemIndex: ";
		msg += std::to_string(slpFileIndex);
		traceMessageHandler.WriteMessage(msg);
		return false;
	}

	AOE_STRUCTURES::STRUCT_SLP_FILE_HEADER *slpHeader = ROCKNROR::crInfo.rockNRorUnitShortcuts.slpFileHeader;
	// slpFrameHeaderBase = first array element (in slp frame headers array)
	AOE_STRUCTURES::STRUCT_SLP_FRAME_HEADER *slpFrameHeaderBase = (AOE_STRUCTURES::STRUCT_SLP_FRAME_HEADER *)
		(slpHeader + 1); // Dirty, but works because structs have same size (done like this in ROR code)
	AOE_STRUCTURES::STRUCT_SLP_FRAME_HEADER *slpFrameHeader = slpFrameHeaderBase + slpArrayIndex; // Move to correct frame in array

	AOE_METHODS::DisplayUnitShortcutSymbol(slpHeader, slpFrameHeader, posX - 9, posY - 8, unknown_arg3);
	return false;
}


// Adds custom attributes (armor) in buildings' unit info zone.
// currentLine is incremented if lines are added.
void RockNRorCommand::DisplayCustomBuildingAttackAttributesInUnitInfo(AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone, long int &currentLine) {
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
bool RockNRorCommand::OnHoverOnUnit(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit, STRUCT_PLAYER *controlledPlayer, long int unitPlayerId,
	UNIT_INTERACTION_ID &foundInteraction, long int &foundHintDllId, GAME_CURSOR &cursorToForce) {
	cursorToForce = (GAME_CURSOR)-1; // Default
	if (!ROCKNROR::crInfo.configInfo.useImprovedButtonBar) { // TODO: use a dedicated config
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
		mainSelectedUnit = ROCKNROR::crInfo.GetMainSelectedUnit(controlledPlayer);
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
	// Unit holding resource -> drop site (exclude priest ! They have own a resource=faith !)
	if (mainSelectedUnit && (mainSelectedUnit->resourceValue > 0) && (actorAIType != TribeAIGroupPriest) &&
		selectedUnitDef && selectedUnitDef->DerivesFromCommandable()) {
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
void RockNRorCommand::OnInGameRightClickCustomAction(float posX, float posY, AOE_STRUCTURES::STRUCT_UNIT_BASE *mouseTargetUnit,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit) {
	if (!actorUnit || !actorUnit->IsCheckSumValidForAUnitClass()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = GetControlledPlayerStruct_Settings();
	assert(controlledPlayer && controlledPlayer->IsCheckSumValid());
	bool actorIsMyUnit = (actorUnit->ptrStructPlayer == controlledPlayer);
	UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(actorUnit->unitInstanceId);

	switch (settings->mouseActionType) {
	case CST_MAT_CR_PROTECT_UNIT_OR_ZONE:
		if (actorIsMyUnit && (actorUnit == mouseTargetUnit)) {
			unitInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(actorUnit->unitInstanceId);
			if (unitInfo) {
				unitInfo->ResetProtectInfo();
				ROCKNROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(actorUnit->unitInstanceId);
			}
			AOE_METHODS::CallWriteCenteredText("Removed protect information for current unit.");
			if (settings->ptrGameUIStruct && settings->ptrGameUIStruct->gamePlayUIZone) {
				AOE_METHODS::DisplayGreenBlinkingOnUnit(settings->ptrGameUIStruct->gamePlayUIZone, actorUnit->unitInstanceId, 1000);
			}
			break;
		}
		if (actorIsMyUnit && actorUnit && actorUnit->DerivesFromMovable()) {

			if (!unitInfo) {
				unitInfo = ROCKNROR::crInfo.myGameObjects.FindOrAddUnitCustomInfo(actorUnit->unitInstanceId);
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
void RockNRorCommand::OnInGameRightClickCustomAction(float posX, float posY, AOE_STRUCTURES::STRUCT_UNIT_BASE *mouseTargetUnit) {
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

	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit = ROCKNROR::crInfo.GetMainSelectedUnit(controlledPlayer);
	bool actorIsMyUnit = false;
	if (actorUnit) {
		assert(actorUnit->IsCheckSumValidForAUnitClass());
		if (!actorUnit->IsCheckSumValidForAUnitClass()) { actorUnit = NULL; }
		actorIsMyUnit = (actorUnit->ptrStructPlayer == controlledPlayer);
	}
	if (!actorIsMyUnit) { return; }
	
	AOE_STRUCTURES::STRUCT_UNIT_BASE **selectedUnits = ROCKNROR::crInfo.GetRelevantSelectedUnitsPointer(controlledPlayer);
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
void RockNRorCommand::OnUnitActivityStop(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity) {
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
		GAME_EVENT_TYPE taskId = activity->notifyQueue[i].eventId;
		if (taskId != GAME_EVENT_TYPE::EVENT_ACTION_COMPLETED) {
			noNextActivity = false;
		}
	}
	UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(unit->unitInstanceId);

	// Auto-protect unit/zone (unit will go back to zone/unit to defend if its current activity is over).
	if (noNextActivity && unitInfo && unitInfo->HasValidProtectInfo()
		&& ROCKNROR::crInfo.configInfo.useImprovedButtonBar /*enableCallNearbyIdleMilitaryUnits*/) { // TODO: specific config
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
				ROCKNROR::crInfo.myGameObjects.RemoveUnitCustomInfoIfEmpty(unitInfo->unitId);
			}
		}
		if ((refX >= 0) || (refY >= 0)) {
			if ((abs(unit->positionX - refX) > distance) || (abs(unit->positionY - refY) > distance)) {
				if (AOE_METHODS::UNIT::IsUnitIdle(unit) && unit->ptrActionInformation) {
					if (!unit->ptrActionInformation->ptrActionLink || !unit->ptrActionInformation->ptrActionLink->actionStruct) {
						// For MP compabitiliy; use a command
						if (!ROCKNROR::crInfo.configInfo.forceMPCompatibility) {
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
bool RockNRorCommand::HandleShowDebugGameInfo(AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings) {
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	if (*AOE_VAR_F5_DEBUG_INFO_TYPE <= ROCKNROR::CONFIG::IDL_STANDARD_MAX_ALLOWED_LEVEL) {
		return false; // Do standard treatments.
	}
	// 2+ are custom debugging states. Here we are in such situation.

	switch (*AOE_VAR_F5_DEBUG_INFO_TYPE) {
	case ROCKNROR::CONFIG::IDL_HIDDEN_COMM:
		return AOE_METHODS::UI_BASE::ShowHiddenDebugComm(settings);
	case ROCKNROR::CONFIG::IDL_HIDDEN_AI:
		return AOE_METHODS::UI_BASE::ShowHiddenDebugAIInfo(settings);
	case ROCKNROR::CONFIG::IDL_CUSTOM:
#ifdef _DEBUG
		return CR_DEBUG::HandleRockNRorInGameF5DebugInfo(settings);
#else
		return true;
#endif
	default:
		break;
	}
	return true;
}


// Changes current in-game debug info level (F5)
void RockNRorCommand::SetNextInGameDebugInfoLevel() {
#ifdef GAMEVERSION_ROR10c
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) {
		return;
	}
	// Note: See 0x484BE0 = mainGameUI.OnF5() => is executed BEFORE this method ! So "debug level" has already been updated.
	// Note: mainGameUI.OnF5() does NOT reset AOE_VAR_F5_DEBUG_INFO_TYPE when debugging is disabled: we can guess what what the last debugging level.
	if (!settings->showDebugTimings && (*AOE_VAR_F5_DEBUG_INFO_TYPE >= ROCKNROR::CONFIG::IDL_STANDARD_MAX_ALLOWED_LEVEL) &&
		ROCKNROR::crInfo.configInfo.enableInGameDisplayDebugInfo) {
		assert(*AOE_VAR_F5_DEBUG_INFO_TYPE <= ROCKNROR::CONFIG::IDL_COUNT);
		// Force additional states of debugging (custom debug levels).
		(*AOE_VAR_F5_DEBUG_INFO_TYPE)++;
		if (*AOE_VAR_F5_DEBUG_INFO_TYPE < ROCKNROR::CONFIG::IDL_COUNT) {
			//std::string msg = std::string("Debug info : #") + std::to_string(*AOE_VAR_F5_DEBUG_INFO_TYPE);
			//AOE_METHODS::CallWriteText(msg.c_str());
			settings->showDebugTimings = 1;
			// Restore debug label visibility instead of resources (optional: depends if we want to show stuff in there)
			if (settings->ptrGameUIStruct && ROCKNROR::crInfo.configInfo.useF5LabelZoneForCustomDebugInfo &&
				(*AOE_VAR_F5_DEBUG_INFO_TYPE != ROCKNROR::CONFIG::IDL_HIDDEN_AI) && // This one does not use F5 debug text bar
				(*AOE_VAR_F5_DEBUG_INFO_TYPE != ROCKNROR::CONFIG::IDL_HIDDEN_COMM) // This one does not use F5 debug text bar
				) {
				AOE_METHODS::UI_BASE::ShowUIObject(settings->ptrGameUIStruct->lblF5debugInfo, true);
				AOE_METHODS::UI_BASE::ShowUIObject(settings->ptrGameUIStruct->resourceValuesIndicator, false);
			}
		}
	}
#endif
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
Returns true if the event should be ignored by ROR (skip original treatments). Default false (most cases)
*/
bool RockNRorCommand::EntryPoint_GameSettingsNotifyEvent(AOE_CONST_INTERNAL::GAME_EVENT_TYPE eventId, short int playerId, long int arg3, long int arg4, long int arg5) {
	bool mustSkipOriginalTreatments = false;

	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_RESEARCH_COMPLETE) {
		long int research_id = arg3;
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		assert(player && player->IsCheckSumValid());
		if (research_id == AOE_CONST_FUNC::CST_RSID_CATAPULT_TRIREME) {
			this->MoveFireGalleyIconIfNeeded(player);
		}
	}

	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_BUILDING_COMPLETE) {
		// Remark : here we don't know unitId
		// Using GameEventHandler's "OnConstructionComplete" method might be preferable for custom treatments.
	}

	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_FARM_DEPLETED) {
		// This is called when a farm is fully depleted (and dies), NOT when a farm is destroyed/killed
		// Warning: when this event is called, the farm is killed just after, so be careful about modifications done here !
		long int farmUnitId = arg3;
		ROCKNROR::UNIT::OnFarmDepleted(farmUnitId);
	}

	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_SUCCESSFULLY_CONVERTED_UNIT) {
		// For this event, arg2 is ACTOR player id, arg3 = VICTIM player id
		long int victimPlayerId = arg3;
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (global && global->IsCheckSumValid() && (victimPlayerId != global->humanPlayerId) && (playerId != global->humanPlayerId)) {
			// This event does not concern human player and must not trigger the adding of a new entry in "event locations history"
			// Original code does a check... But only skips the alert sound, not the "add location in history" part.
			mustSkipOriginalTreatments = true;
		}
	}

	// Now manage triggers...
	ROCKNROR::TRIGGER::ManageTriggersOnGameNotifyEvent(eventId, playerId, arg3, arg4, arg5);

	return mustSkipOriginalTreatments;
}


// This is called for each player at game initialization, after applying tech trees and starting age.
// Resources and score info have not been initialized yet.
// This is called for all game types (SP / MP, RM/DM/scenario) but NOT for load game.
void RockNRorCommand::OnGameInitAfterApplyingTechTrees(long int playerId) {
	this->ApplyScenarioSpecificPlayerStartingAge(playerId);

	// For current player, disable the researches that can never be available
	DisablePlayerImpossibleResearches(GetPlayerStruct(playerId));
}


// Apply starting age to a player (only for scenarios). Player's specific starting age is read in STRUCT_SCENARIO_INFO
void RockNRorCommand::ApplyScenarioSpecificPlayerStartingAge(long int playerId) {
	if (!ROCKNROR::crInfo.hasRemovePlayerInitialAgeInScenarioInit) {
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
