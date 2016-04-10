#include "../include/crCommands.h"


char outputBuffer[CRCMD_TEXT_BUFFER_SIZE];



CustomRORCommand::CustomRORCommand() {
	this->crInfo = NULL;
}

CustomRORCommand::CustomRORCommand(CustomRORInfo *crInfo) {
	this->crInfo = crInfo;
}


CustomRORCommand::~CustomRORCommand() {
}


bool CustomRORCommand::CheckEnabledFeatures() {
	char strNotEnabled[] = " NOT ENABLED !";
	FILE *f;
	int res = fopen_s(&f, "CustomROR.log", "w+"); // overwrite if already existing
	if (res) {
		traceMessageHandler.WriteMessage("ERROR: cannot write to CustomROR.log");
		return false;
	}
	fprintf_s(f, "CustomROR %s\n", VER_FILE_VERSION_STR);
	if (this->crInfo->configInfo.couldNotReadXMLConfig) {
		fprintf_s(f, "Warning: configuration could not be read from XML file (missing or incorrect file).\n\n");
	}
	if (this->crInfo->configInfo.couldNotReadCivXMLConfig) {
		fprintf_s(f, "customROR_civs.xml could not be read (missing or incorrect file). This is not an error unless you want to use custom civilizations.\n\n");
	}

	fprintf_s(f, "Running checks on active features in game executable.\n");
	fprintf_s(f, "This detects if some features from this version of CustomROR will not work because you need to enable them with \"CustomizeAOE\".\n");
	fprintf_s(f, "\nFeature: Init...");
	// Note: use return address for checks (instruction AFTER call to ROR_API)
	bool result = IsRORAPICallEnabled(0x443713);
	if (!result) {
		fprintf_s(f, strNotEnabled);
	}
	fprintf_s(f, "\nFeature: Game start/restart...");
	if (!IsRORAPICallEnabled(0x00502A3D)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Game start after load game...");
	if (!IsRORAPICallEnabled(0x00503054)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Init before game start - not saved games...");
	if (!IsRORAPICallEnabled(0x51D50E)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Init before game start - saved games...");
	if (!IsRORAPICallEnabled(0x00502E2A)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Init trigger data when loading scenario file...");
	if (!IsRORAPICallEnabled(0x0050693C)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Read from chat...");
	if (!IsRORAPICallEnabled(0x435F74)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Conversion resistance...");
	if (!IsRORAPICallEnabled(0x4B24F4)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Conversions monitoring...");
	if (!IsRORAPICallEnabled(0x4B2578)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix wonder construction flag reset...");
	if (!IsRORAPICallEnabled(0x004B8BE5)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Additional checks/fixes when triggering constructions...");
	if (!IsRORAPICallEnabled(0x004D221D)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix building strategy element id...");
	this->crInfo->hasFixForBuildingStratElemUnitId = true;
	if (!IsRORAPICallEnabled(0x004AC92F)) {
		fprintf_s(f, strNotEnabled);
		result = false;
		this->crInfo->hasFixForBuildingStratElemUnitId = false;
	}
	fprintf_s(f, "\nFeature: Manage add strategy element...");
	if (!IsRORAPICallEnabled(0x00408F92)) {
		fprintf_s(f, strNotEnabled);
		result = false;
		this->crInfo->hasFixForBuildingStratElemUnitId = false;
	}
	fprintf_s(f, "\nFeature: Test strategy element alive or in progress...");
	if (!IsRORAPICallEnabled(0x004B8C74)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix AutoBuildWarships (add warship)...");
	if (!IsRORAPICallEnabled(0x004B7D89)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix AutoBuildHouses (count houses)...");
	if (!IsRORAPICallEnabled(0x004B7E23)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: AfterAddDynamicStratElemsEntryPoint...");
	if (!IsRORAPICallEnabled(0x004B801C)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Manage panic mode...");
	if (!IsRORAPICallEnabled(0x04E2300)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Overload set strategy element in progress...");
	if (!IsRORAPICallEnabled(0x004DC088)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Overload reset AI for unit in destructor...");
	if (!IsRORAPICallEnabled(0x004F2B4F)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Improve tactical updates...");
	if (!IsRORAPICallEnabled(0x004D09C2)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix defeated AI players AI update...");
	if (!IsRORAPICallEnabled(0x004D1B81)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Improve unit target choice (military)...");
	if (!IsRORAPICallEnabled(0x00411D5D)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Improve unit target choice (convert)...");
	if (!IsRORAPICallEnabled(0x00411F2B)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: AI military units reaction in tower-panic mode...");
	if (!IsRORAPICallEnabled(0x004E37B4)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: AI villagers reaction in tower-panic mode...");
	if (!IsRORAPICallEnabled(0x004E39DC)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: reaction when attacked: handle current action...");
	if (!IsRORAPICallEnabled(0x04D829E)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix canConvertUnit function...");
	if (!IsRORAPICallEnabled(0x04E5006)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: CheckPopCostWithLogistics...");
	if (!IsRORAPICallEnabled(0x004F16BB)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Manage number of relics...");
	if (!IsRORAPICallEnabled(0x050B6C7)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Manage number of ruins...");
	if (!IsRORAPICallEnabled(0x050B70C)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: OnLivingUnitSpawn...");
	if (!IsRORAPICallEnabled(0x004ED189)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: OnGameRightClickUpEvent...");
	if (!IsRORAPICallEnabled(0x0051A508)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: OnGameRightClickUpCustomRedCrossDisplay...");
	if (!IsRORAPICallEnabled(0x0051A5DA)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: OnPlayerRemoveUnit...");
	if (!IsRORAPICallEnabled(0x0045E8D7)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: OnPlayerAddUnit...");
	if (!IsRORAPICallEnabled(0x004F2AAB)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: WMCloseMessageEntryPoint...");
	if (!IsRORAPICallEnabled(0x041AEFA)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix unit status in editor after loading file...");
	if (!IsRORAPICallEnabled(0x004EFF14)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Entry point after loading empires.dat...");
	if (!IsRORAPICallEnabled(0x051C903)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix human player OnUnitSeen call for arterfacts...");
	if (!IsRORAPICallEnabled(0x00426D0F)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Custom unit selection behaviours...");
	if (!IsRORAPICallEnabled(0x051A700)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: ROR debug logs handling...");
	if (!IsRORAPICallEnabled(0x042C335)) {
		fprintf_s(f, strNotEnabled);
		//result = false;
	}
	fprintf_s(f, "\nFeature: ROR debug logs 2 handling...");
	if (!IsRORAPICallEnabled(0x040C245)) {
		fprintf_s(f, strNotEnabled);
		//result = false;
	}
	fprintf_s(f, "\nFeature: AffectGathererCheckPathFindingReturn...");
	if (!IsRORAPICallEnabled(0x044E02F)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Show shortcuts in editor...");
	if (!IsRORAPICallEnabled(0x04A78DA)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Custom F11 population info...");
	if (!IsRORAPICallEnabled(0x004FAA36)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix select unit in AI list...");
	if (!IsRORAPICallEnabled(0x004C41BB)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix unit id in activity target...");
	if (!IsRORAPICallEnabled(0x0041498B)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix unit id when setting activity target...");
	if (!IsRORAPICallEnabled(0x00412F34)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix unit id when reading activity target (case200)...");
	if (!IsRORAPICallEnabled(0x04E64BD)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix unit id when reading activity target (case1F4)...");
	if (!IsRORAPICallEnabled(0x004E479B)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix game crash when using kill* cheat code on a non-existing player...");
	if (!IsRORAPICallEnabled(0x0050CB6F)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	// MAP
	fprintf_s(f, "\nFeature: Generated map elevation customization...");
	if (!IsRORAPICallEnabled(0x00472C2F)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Fix buggy method for map generation: terrain elevation...");
	if (!IsRORAPICallEnabled(0x0046F5AD)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	// City plan
	// 0x004C253D call is obsolete and no longer required.
	/*fprintf_s(f, "\nFeature: City plan: houses distance from other buildings...");
	if (!IsRORAPICallEnabled(0x004C253D)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}*/
	fprintf_s(f, "\nFeature: City plan: compute building position priorities 1...");
	if (!IsRORAPICallEnabled(0x004BE8DD)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: City plan: compute building position priorities 2...");
	if (!IsRORAPICallEnabled(0x004C1CB4)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: City plan: Manage bush like value to add for granary...");
	if (!IsRORAPICallEnabled(0x004C2386)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: City plan: Manage farms placement...");
	if (!IsRORAPICallEnabled(0x004C27D6)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	/*fprintf_s(f, "\nFeature: City plan: Building location validation...");
	if (!IsRORAPICallEnabled(0x004BEFB2)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}*/
	// UI
	fprintf_s(f, "\nFeature: Add custom menu button...");
	if (!IsRORAPICallEnabled(0x00433E7C)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Manage custom menu button...");
	if (!IsRORAPICallEnabled(0x0043424D)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Manage game speed change...");
	if (!IsRORAPICallEnabled(0x0484CBF)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Manage custom options menu button...");
	if (!IsRORAPICallEnabled(0x0043136D)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Manage UI dialog events...");
	if (!IsRORAPICallEnabled(0x004348DE)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Manage textboxes events...");
	if (!IsRORAPICallEnabled(0x00462E65)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Scenario editor creation entry point...");
	if (!IsRORAPICallEnabled(0x00499143)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Scenario editor change selected terrain...");
	if (!IsRORAPICallEnabled(0x004926B5)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Scenario editor customize unit placement...");
	if (!IsRORAPICallEnabled(0x0051A3B9)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	// Civs
	fprintf_s(f, "\nFeature: Control civ id in game settings selection...");
	if (!IsRORAPICallEnabled(0x00503417)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Get civ name DLL string ID offest...");
	if (!IsRORAPICallEnabled(0x005034AF)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Get civ name DLL string ID offset 2...");
	if (!IsRORAPICallEnabled(0x0050386A)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Choose AI file for player...");
	if (!IsRORAPICallEnabled(0x004F36F1)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Handle civs in game settings combobox...");
	if (!IsRORAPICallEnabled(0x0048829F)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Handle civs in scenario editor combobox...");
	if (!IsRORAPICallEnabled(0x00490961)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: OnButtonClick entry point...");
	if (!IsRORAPICallEnabled(0x004604C9)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Scenario editor key press...");
	if (!IsRORAPICallEnabled(0x0041A6C7)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: In-game get custom civ name...");
	if (!IsRORAPICallEnabled(0x004FF852)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Player creation checks at game startup (comp)...");
	if (!IsRORAPICallEnabled(0x0050A574)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Player creation checks at game startup (human)...");
	if (!IsRORAPICallEnabled(0x0050A5C3)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	// TRIGGERS / entry points
	fprintf_s(f, "\nFeature: GameSettings.NotifyEvent...");
	if (!IsRORAPICallEnabled(0x00501985)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: scenarioInfo.applyDisabledResearches...");
	if (!IsRORAPICallEnabled(0x00507F7A)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	fprintf_s(f, "\nFeature: Before save game...");
	if (!IsRORAPICallEnabled(0x0051E0E1)) {
		fprintf_s(f, strNotEnabled);
		result = false;
	}
	// Optional
	fprintf_s(f, "\nFeature: [Optional] Skip timer cycles to avoid game time freeze...");
	if (!IsRORAPICallEnabled(0x51D91A)) {
		fprintf_s(f, strNotEnabled);
		// Do NOT set result to false. This feature is optional and only useful on fast computers.
	}
	fprintf_s(f, "\nFeature: [Optional] Get game time stats and auto-compute slow down factor...");
	if (!IsRORAPICallEnabled(0x51D9EC)) {
		fprintf_s(f, strNotEnabled);
		// Do NOT set result to false. This feature is optional and only useful on fast computers.
	}

	fprintf_s(f, "\nEnd of checks.\n\nConfiguration:\n");
	// General - not directly related to game
	fprintf_s(f, "hideWelcomeMessage:                        %d\n", this->crInfo->configInfo.hideWelcomeMessage ? 1 : 0);
	fprintf_s(f, "showAlertOnMissingFeature:                 %d\n", this->crInfo->configInfo.showAlertOnMissingFeature ? 1 : 0);
	fprintf_s(f, "showCustomRORMenu:                         %d\n", this->crInfo->configInfo.showCustomRORMenu ? 1 : 0);
	fprintf_s(f, "showCustomPopulationInfo:                  %d\n", this->crInfo->configInfo.showCustomPopInfo ? 1 : 0);
	fprintf_s(f, "useImprovedGameSpeeds:                     %d\n", this->crInfo->configInfo.useImprovedGameSpeeds ? 1 : 0);
	fprintf_s(f, "collectRORDebugLogs:                       %d\n", this->crInfo->configInfo.collectRORDebugLogs);
	fprintf_s(f, "showCustomRORNotifications:                %d\n", this->crInfo->configInfo.showCustomRORNotifications);
	fprintf_s(f, "gameTimerSlowDownAutoFix:                  %d\n", this->crInfo->configInfo.gameTimerSlowDownAutoFix ? 1 : 0);
	fprintf_s(f, "gameTimerSlowDownFactor:                   %d\n", this->crInfo->configInfo.gameTimerSlowDownFactor);
	fprintf_s(f, "fixInvisibleTree:                          %d\n", this->crInfo->configInfo.fixInvisibleTree ? 1 : 0);
	fprintf_s(f, "fixHumanPlayer_specific_seeUnit:           %d\n", this->crInfo->configInfo.fixHumanPlayer_specificSeeUnit ? 1 : 0);
	// General - related to game
	fprintf_s(f, "allyExplorationIsAlwaysShared:             %d\n", this->crInfo->configInfo.allyExplorationIsAlwaysShared ? 1 : 0);
	// Human interface / shortcuts
	fprintf_s(f, "useNumPadUnitShortcuts:                    %d\n", this->crInfo->configInfo.enableAdditionalNumpadShortcuts ? 1 : 0);
	fprintf_s(f, "enableCallNearbyIdleMilitaryUnits:         %d\n", this->crInfo->configInfo.enableCallNearbyIdleMilitaryUnits ? 1 : 0);
	fprintf_s(f, "distanceToCallNearbyIdleMilitaryUnits:     %ld\n", this->crInfo->configInfo.distanceToCallNearbyIdleMilitaryUnits);
	fprintf_s(f, "enableAutoMoveToLocation:                  %d\n", this->crInfo->configInfo.enableSpawnUnitsMoveToLocation ? 1 : 0);
	fprintf_s(f, "enableSpawnUnitsAutoTarget:                %d\n", this->crInfo->configInfo.enableSpawnUnitsAutoTarget ? 1 : 0);
	fprintf_s(f, "enableSpawnUnitAutoRepairTC:               %d\n", this->crInfo->configInfo.enableSpawnUnitAutoRepairTC ? 1 : 0);
	fprintf_s(f, "unitSpawnShortcutReverseOrder:             %d\n", this->crInfo->configInfo.unitShortcutsPriorityReverseOrder ? 1 : 0);
	for (int shortcutId = 1; shortcutId < CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS; shortcutId++) {
		UnitSpawnShortcutInfo *sinfo = &this->crInfo->configInfo.unitShortcutsInformation[shortcutId];
		if (sinfo->DAT_ID >= 0) {

			fprintf_s(f, "unitSpawn shortcut %d: unit=%03d onlyOneUnit=%d (%.12s)\n", shortcutId, sinfo->DAT_ID, sinfo->onlyOneUnit ? 1 : 0,
				GetUnitName(sinfo->DAT_ID));
		}
	}
	fprintf_s(f, "autoRebuildFarms_maxFarms:                 %ld\n", this->crInfo->configInfo.autoRebuildFarms_maxFarms);
	fprintf_s(f, "autoRebuildFarms_maxFood:                  %ld\n", this->crInfo->configInfo.autoRebuildFarms_maxFood);
	fprintf_s(f, "autoRebuildFarms_minWood:                  %ld\n", this->crInfo->configInfo.autoRebuildFarms_minWood);

	// Conversion
	fprintf_s(f, "conversionResistance_Boats:                %f\n", this->crInfo->configInfo.conversionResistance_Boats);
	fprintf_s(f, "conversionResistance_Chariots:             %f\n", this->crInfo->configInfo.conversionResistance_Chariots);
	fprintf_s(f, "conversionResistance_Macedonian:           %f\n", this->crInfo->configInfo.conversionResistance_Macedonian);
	fprintf_s(f, "conversionResistance_Priests:              %f\n", this->crInfo->configInfo.conversionResistance_Priests);
	fprintf_s(f, "conversionResistance_WarElephants:         %f\n", this->crInfo->configInfo.conversionResistance_WarElephants);
	fprintf_s(f, "conversionResistance_WarElephants_Persian: %f\n", this->crInfo->configInfo.conversionResistance_WarElephants_Persian);
	// Various AI
	fprintf_s(f, "improveAILevel:                            %ld\n", this->crInfo->configInfo.improveAILevel);
	fprintf_s(f, "tacticalAIUpdateDelay:                     %ld\n", this->crInfo->configInfo.tacticalAIUpdateDelay);
	fprintf_s(f, "minPopulationBeforeOptionalItems:          %ld\n", this->crInfo->configInfo.minPopulationBeforeBuildOptionalItems);
	fprintf_s(f, "maxPanicUnitsCountToAddInStrategy:         %ld\n", this->crInfo->configInfo.maxPanicUnitsCountToAddInStrategy);
	fprintf_s(f, "panicModeDelay:                            %ld\n", this->crInfo->configInfo.panicModeDelay);
	fprintf_s(f, "dislike computation interval:              %ld\n", this->crInfo->configInfo.dislikeComputeInterval);
	fprintf_s(f, "dislike value - all artefacts/wonder:      %ld\n", this->crInfo->configInfo.dislike_allArtefacts);
	fprintf_s(f, "dislike value - human player:              %ld\n", this->crInfo->configInfo.dislike_humanPlayer);
	fprintf_s(f, "maxFarmsInDeathMatch:                      %d\n", this->crInfo->configInfo.maxFarmsInDeathMatch);
	fprintf_s(f, "fixLogisticsNoHouseBug:                    %d\n", this->crInfo->configInfo.fixLogisticsNoHouseBug ? 1: 0);
	// City plan
	fprintf_s(f, "cityPlanLikeValuesEnhancement:             %d\n", this->crInfo->configInfo.cityPlanLikeValuesEnhancement ? 1 : 0);
	//fprintf_s(f, "cityPlanHouseDistanceFromTownCenter:       %f\n", this->crInfo->configInfo.cityPlanHouseDistanceFromTownCenter);
	//fprintf_s(f, "cityPlanHouseDistanceFromStorageBld:       %f\n", this->crInfo->configInfo.cityPlanHouseDistanceFromStorageBld);
	//fprintf_s(f, "cityPlanHouseDistanceFromOtherBld:         %f\n", this->crInfo->configInfo.cityPlanHouseDistanceFromOtherBld);
	fprintf_s(f, "cityPlanBerryBushWeightForGranary:         %ld\n", this->crInfo->configInfo.cityPlanBerryBushWeightForGranary);
	// Map generation
	fprintf_s(f, "random map relics count:                   %ld\n", this->crInfo->configInfo.randomMapRelicsCount);
	fprintf_s(f, "random map ruins count:                    %ld\n", this->crInfo->configInfo.randomMapRuinsCount);
	fprintf_s(f, "use map gen. elevation customization:      %d\n", this->crInfo->configInfo.useMapGenerationCustomElevationCalculation ? 1: 0);
	for (int mapType = 0; mapType < 10; mapType++) {
		fprintf_s(f, "map gen. elevation factor  mapType=%ld value=%f\n", mapType, this->crInfo->configInfo.mapGenerationCustomElevationFactor[mapType]);
	}
	fprintf_s(f, "disable dock in mostly-land maps:          %ld\n", this->crInfo->configInfo.noDockInMostlyLandMaps ? 1: 0);
	// Scenario Editor
	fprintf_s(f, "showHiddenTerrainInEditor:                 %d\n", this->crInfo->configInfo.showHiddenTerrainsInEditor ? 1 : 0);
	fprintf_s(f, "showHiddenUnitsInEditor:                   %ld\n", this->crInfo->configInfo.showHiddenUnitsInEditor);
	fprintf_s(f, "useCustomMapDimensionsInEditor:            %d\n", this->crInfo->configInfo.useCustomMapDimensions ? 1 : 0);

	// Other
	fprintf_s(f, "civilizations count:                       %ld\n", this->crInfo->configInfo.civCount);

	fprintf_s(f, "End of configuration.\n\n");
	fclose(f);
	return result;
}


// Reads game executable to determine if player struct is extended to use custom memory zone to host selected units
void CustomRORCommand::ReadIfCustomSelectedUnitsMemoryZoneIsUsed() {
	this->crInfo->hasCustomSelectedUnitsMemory = false;
	char *gameMemPtr = (char *)0x0050A3D4;
	char alloc1[] = { 0x68, 0x5C, 0x08, 0x00, 0x00 };
	if (memcmp(gameMemPtr, alloc1, sizeof(alloc1))) { // memcmp ==0 means different from standard => using custom memory zone for selected units
		this->crInfo->hasCustomSelectedUnitsMemory = true;
	}
	// TO DO: check all sequences ?
}

// Reads game executable to determine if ManageAI is installed (does game use player->unused_customAIFlag ?)
void CustomRORCommand::ReadIfManageAIIsOn() {
	this->crInfo->hasManageAIFeatureON = false;
	char *gameMemPtr = (char *)0x004EFBF3;
	unsigned char Init_is_computer_for_AI_1[] = { 0x8A, 0x44, 0x24, 0x38, 0x57 };
	if (memcmp(gameMemPtr, Init_is_computer_for_AI_1, sizeof(Init_is_computer_for_AI_1))) { // memcmp ==0 means different from standard => using custom feature
		this->crInfo->hasManageAIFeatureON = true;
	}
	// TO DO: check all sequences ?
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
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *gameSettings = *pGameSettingsStruct;

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
			int newType = atoi(subCmd);
			if (newType) {
				gameSettings->mapTypeChoice = (AOE_CONST_FUNC::MAP_TYPE_INDEX) newType;
				sprintf_s(outputBuffer, "Map type=%d", newType);
			}
			return true;
		}

		if (!_strnicmp(subCmd, "control=", 8)) {
			subCmd += 8;
			int newId = atoi(subCmd);
			if ((*subCmd == '0' && (*(subCmd+1) == 0)) || (newId && (newId < 9))) {
				ChangeControlledPlayer(newId, true);
				sprintf_s(outputBuffer, "Controlled player#=%d", newId);
			}
			return true;
		}
	}


	// A good nice trick : add any unit at any (explored) location using scenario editor feature... in game !
	// Type addXXX where XXX is a unitDefId.
	if (!_strnicmp(command, "add", 3) && (command[3] >= '0') && (command[3] <= '9')) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
		assert(player != NULL);
		char *aid = command + 3;
		short int id = atoi(aid);
		if ((id == 0) && (*aid != '0')) { id = -1; }
		ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = GetUnitDefStruct(player, id);
		if (player && player->IsCheckSumValid() && unitDef && unitDef->IsCheckSumValid() && gameSettings->isSinglePlayer) {
			gameSettings->mouseActionType = AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_EDITOR_SET_UNIT_LOCATION;
			gameSettings->userSelectedDATID = id;
			sprintf_s(outputBuffer, "Add %s. Right-click to quit add mode.", unitDef->ptrUnitName);
		}
	}

#ifdef _DEBUG

	// BETA - TEST
	if (!_strnicmp(command, "free", 5)) {
		assert(gameSettings != NULL);
		ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = gameSettings->ptrGlobalStruct;
		//short int humanPlayerId = global->humanPlayerId;
		ROR_STRUCTURES_10C::STRUCT_PLAYER *humanPlayer = GetControlledPlayerStruct_Settings();
		assert(humanPlayer != NULL);
		if (humanPlayer->selectedUnitCount > 0) {
			ROR_STRUCTURES_10C::STRUCT_UNIT **selectedUnits = this->crInfo->GetRelevantSelectedUnitsPointer(humanPlayer);
			assert(selectedUnits != NULL);
			ROR_STRUCTURES_10C::STRUCT_UNIT *selectedUnit = selectedUnits[0];

			ROR_STRUCTURES_10C::STRUCT_PLAYER **playersArray = global->GetPlayerStructPtrTable();
			assert(playersArray != NULL);
			for (int iPlayerId = 1; iPlayerId < global->playerTotalCount; iPlayerId++) {
				ROR_STRUCTURES_10C::STRUCT_PLAYER *currentPlayer = playersArray[iPlayerId];
				assert(currentPlayer != NULL);
				if (currentPlayer->isComputerControlled && currentPlayer->IsAIActive(this->crInfo->hasManageAIFeatureON) &&
					(currentPlayer->GetAIStruct() != NULL)) {
					ROR_STRUCTURES_10C::STRUCT_AI *ai = currentPlayer->GetAIStruct();
					ROR_STRUCTURES_10C::STRUCT_INF_AI *infAI = &ai->structInfAI;
					for (long index = 0; index < infAI->creatableAndGatherableUnits.arraySize; index++) {
						long int unitId = infAI->creatableAndGatherableUnits.unitIdArray[index];
						if (unitId == selectedUnit->unitInstanceId) {
							infAI->creatableAndGatherableUnits.unitIdArray[index] = 0;
						}
					}
					for (long index = 0; index < infAI->playerCreatableUnits.usedElements; index++) {
						long int unitId = infAI->playerCreatableUnits.unitIdArray[index];
						if (unitId == selectedUnit->unitInstanceId) {
							infAI->playerCreatableUnits.unitIdArray[index] = 0;
						}
					}
				}

			}
		}
	}

	char *c = "Game Settings Screen";
	//char *c = "MP Setup Screen";
	long int testvar;
	if (!_strnicmp(command, "new", 4)) {
		float x, y;
		unsigned long int res = 0;
		ROR_STRUCTURES_10C::STRUCT_PLAYER *p = GetControlledPlayerStruct_Settings();
		_asm {
			MOV ECX, gameSettings
			MOV EAX, 0x419B70
			CALL EAX
			MOV p, EAX
		}
		x = p->screenPositionX;
		y = p->screenPositionY;
		ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *def = p->ptrStructDefUnitTable[CST_UNITID_FORAGER];
		res = GetErrorForUnitCreationAtLocation(p, def, y, x, false, true, false, true, true);
		sprintf_s(outputBuffer, "Placement Error=%d", res);
		if (!res) {
			res = (unsigned long int) CreateUnit(p, CST_UNITID_EXPLORER, y, x, 0);
		}
	}

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
		CallWriteText("AI flags have been recomputed");
	}
	if ((strcmp(command, "allai") == 0) || (strcmp(command, "allAI") == 0)) {
		this->SetAllAIFlags();
		CallWriteText("All players are now computer-managed.");
	}
	if (strcmp(command, "dump") == 0) {
		this->DumpDebugInfoToFile();
	}
	if (strcmp(command, "about") == 0) {
		char buf[30];
		sprintf_s(buf, "CustomROR %s\n", VER_FILE_VERSION_STR);
		CallWriteText(buf);
	}
	if (strcmp(command, "timer stats") == 0) {
		this->DisplayTimerStats();
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
	if (strcmp(command, "a") == 0) {
		long int playerId = 1;
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		if (!player || !player->ptrAIStruct) { return; }
		ROR_STRUCTURES_10C::STRUCT_AI *ai = player->ptrAIStruct;
		ROR_STRUCTURES_10C::STRUCT_TAC_AI *tacAI = &ai->structTacAI;
		assert(tacAI->IsCheckSumValid());
		float posX, posY;
		bool ok = GetGamePositionUnderMouse(&posX, &posY);
	}

	// TEST strategy
	if (strcmp(command, "strat") == 0) {
		ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *scinfo = GetGameGlobalStructPtr()->scenarioInformation;
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();

		_asm {
			MOV ECX, player
			MOV EDX, 0x004F2160
			PUSH -1
			CALL EDX
		}

		ROR_STRUCTURES_10C::STRUCT_PLAYER *player1 = GetPlayerStruct(1);
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player2 = GetPlayerStruct(2);
		player->ptrScoreInformation;
		player1->ptrScoreInformation;
		player2->ptrScoreInformation;
		player->ptrResourceValues;
		player1->ptrResourceValues;
		player2->ptrResourceValues;
		ROR_STRUCTURES_10C::STRUCT_SCORE_ELEM *scoreElem1 = FindScoreElement(player1, AOE_CONST_FUNC::SCORE_CATEGORIES::CST_SC_ECONOMY, AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_CIVILIAN_POPULATION);
		ROR_STRUCTURES_10C::STRUCT_SCORE_ELEM *scoreElem2 = FindScoreElement(player2, AOE_CONST_FUNC::SCORE_CATEGORIES::CST_SC_ECONOMY, AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_CIVILIAN_POPULATION);
		float a1 = scoreElem1->value;
		float a2 = scoreElem2->value;
		int x = player->AIControlMode;
	}
#endif

	// TEST conversion
	if (strcmp(command, "conversion") == 0) {
		ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = *pGlobalStruct;
		assert(global != NULL);
		for (int i = 1; i < global->playerTotalCount; i++) {
			int FOR_attempt = this->crInfo->activeConversionAttemptsCount[i];
			int FOR_success = this->crInfo->activeConversionSuccessfulAttemptsCount[i];
			float FOR_pct = FOR_attempt == 0 ? 0 : ((float)FOR_success) / ((float)FOR_attempt);
			int AGAINST_attempt = this->crInfo->passiveConversionAttemptsCount[i];
			int AGAINST_success = this->crInfo->passiveConversionSuccessfulAttemptsCount[i];
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
		MOV EAX, AOE_OFFSETS_10C::ADDR_VAR_GAME_SETTINGS_STRUCT
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


// This is called just after empires.dat is loaded.
void CustomRORCommand::OnAfterLoadEmpires_DAT() {
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global);
	if (!global || !global->IsCheckSumValid()) { return; }
	// Show hidden units in editor
	assert(this->crInfo);
	assert(global->civCount < 256); // it is a short (2 bytes)
	if (this->crInfo->configInfo.showHiddenUnitsInEditor > 0) {
		bool excludeAnnoyingUnits = (this->crInfo->configInfo.showHiddenUnitsInEditor == 1);
		for (int civid = 0; civid < global->civCount; civid++) {
			ROR_STRUCTURES_10C::STRUCT_DEF_CIVILIZATION *civDef = global->civilizationDefinitions[civid];
			if (civDef && civDef->IsCheckSumValid()) {
				for (int datid = 0; datid < civDef->civUnitDefCount; datid++) {
					ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = civDef->GetUnitDef(datid);
					// If config says to exclude annoying units, test if current unitDef is one of banned ones
					if (unitDef && unitDef->IsCheckSumValid()) {
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
	// Assign graphics to the invisible tree
	if (this->crInfo->configInfo.fixInvisibleTree) {
		for (int civid = 0; civid < global->civCount; civid++) {
			ROR_STRUCTURES_10C::STRUCT_DEF_CIVILIZATION *civDef = global->civilizationDefinitions[civid];
			if (civDef && civDef->IsCheckSumValid()) {
				if (civDef->civUnitDefCount > 393) {
					// add condition SLP=799 ?
					ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDefDest = civDef->GetUnitDef(393);
					ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDefSrc = civDef->GetUnitDef(391);
					if (unitDefDest && unitDefSrc && unitDefDest->IsCheckSumValid() && unitDefSrc->IsCheckSumValid()) {
						unitDefDest->ptrStandingGraphics = unitDefSrc->ptrStandingGraphics;
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

	// Check AI initialization for loaded games (fix a saved game bug)
	this->FixGameStartAIInitForPlayers();

	this->DisableWaterUnitsIfNeeded();

	// Fix fire galley icon on post-iron age start games (research done notification - for trireme - is NOT triggered in such case !)
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings != NULL);
	if (!settings) { return; } // Should never happen
	if (settings->initialAgeChoice == 6) { // Post-iron age startup
		for (int playerId = 1; playerId < 9; playerId++) {
			this->MoveFireGalleyIconIfNeeded(playerId);
		}
	}

	// Could be done in "after empires.dat loading" ?
	ROR_STRUCTURES_10C::STRUCT_PLAYER *gaia = GetPlayerStruct(0);
	if (gaia && gaia->IsCheckSumValid()) {
		// Fix gaia attack sound
		gaia->SetResourceValue(CST_RES_ORDER_ATTACK_ALERT_SOUND_ID, 10);
	}

	// Triggers
	this->ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_GAME_START);

	// Special trigger: disable units for house, villager, fishing ship, trade ship (exceptions that don't work because of techs that do several things)
	// For farm, see notify event method (managed on market construction notification).
	this->ManageTriggerDisableUnitsForExceptions();

	long int **p = (long int **)AOE_OFFSETS_10C::ADDR_VAR_ACTIVE_UI_STRUCT;
	if ((*p) && (**p != 0x0054679C)) {
		// This is NOT game screen, do not try to interact with it !
		return;
	}

	// REQUIRES game UI to be active
	if (!this->crInfo->configInfo.hideWelcomeMessage && !settings->isMultiplayer) {
		CallWriteText("Welcome. CustomROR " VER_FILE_VERSION_STR " plugin is active.");
		CallWriteText("See log file to check active features (custom conversion resistance...)");
	}

	// Show automatically "F11" information at game startup
	if (!settings->isMultiplayer) {
		this->ShowF11_zone();
	}

	// Force shared exploration => always ON (if config says so and not in MP)
	// Does not impact scenarios
	if (!settings->isCampaign && !settings->isScenario) {
		ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
		if (this->crInfo->configInfo.allyExplorationIsAlwaysShared &&
			!settings->isMultiplayer && global && global->IsCheckSumValid()) {
			for (int i = 1; i < global->playerTotalCount; i++) {
				SetPlayerSharedExploration_safe(i);
				//ApplyResearchForPlayer(player, CST_RSID_WRITING);
				// Could disable the research ?
			}
		}
	}
}


// This is called while scenarioInfo structure is read from a file
// Scenario texts (including players strategies) havealready been read, but not other information, be careful.
void CustomRORCommand::InitScenarioInfoTextData(ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *scenarioInfo) {
	this->ReloadTriggersFromGameData(scenarioInfo);

	for (int playerId = 0; playerId < 9; playerId++) {
		if (this->crInfo->triggerSet != NULL) {
			this->crInfo->FillUnitDefToDisableFromString(playerId, this->crInfo->triggerSet->disable_unitDefIDTextList[playerId].c_str());
			this->crInfo->FillResearchesToDisableFromString(playerId, this->crInfo->triggerSet->disable_researchIDTextList[playerId].c_str());
		}
	}
}


// Initialize internal game-specific variables (called on each game start/load)
void CustomRORCommand::InitMyGameInfo() {
	this->crInfo->ResetVariables();
	// Prevent 0% speed at game startup (occurs because of rounding in registry saved value)
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = *pGlobalStruct;
	if (global && global->IsCheckSumValid() && (global->gameSpeed == 0)) {
		global->gameSpeed = 1;
	}
}


// Use this API to get "do we improve AI" information. This takes care of customROR configuration + possible custom rules.
// So we can add custom rules for debugging
bool CustomRORCommand::IsImproveAIEnabled(int playerId) {
#ifdef _DEBUG
	return (this->crInfo->configInfo.improveAILevel > 0) && _DEBUG_improveThisPlayer(playerId);
#else
	return (this->crInfo->configInfo.improveAILevel > 0);
#endif
}


// Disable AI flags for human players, based on game initial settings (to be used at game startup)
void CustomRORCommand::DisableAIFlagsForHuman() {
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }

	bool isSinglePlayer = !IsMultiplayer();

	ROR_STRUCTURES_10C::STRUCT_MP_COMMUNICATION *unknownStruct = *(ROR_STRUCTURES_10C::STRUCT_MP_COMMUNICATION **)AOE_OFFSETS_10C::ADDR_MP_COMM_STRUCT;
	assert(unknownStruct != NULL);
	int aValidAIPlayer = -1;
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = global->GetPlayerStructPtrTable()[loopPlayerId];
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
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	
	int aValidAIPlayer = -1;
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = global->GetPlayerStructPtrTable()[loopPlayerId];
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
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = global->GetPlayerStructPtrTable()[loopPlayerId];
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
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
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
void CustomRORCommand::CheckAIWhenEnablingAIControl(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	UpdateStrategyWithUnreferencedExistingUnits(player);
}


// This fixes nextStrategyAIExecutionCounter flag for all players (useful for loaded games)
void CustomRORCommand::FixGameStartAIInitForPlayers() {
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	for (int loopPlayerId = 1; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = global->GetPlayerStructPtrTable()[loopPlayerId];
		if ((player->GetAIStruct() != NULL) && !this->FindIfGameStartStrategyInitHasBeenDone(player)) {
			// It seems we found a player that has not had his "game start AI init", including addition of dynamic strategy elements.
			// Force the flag to trigger it now (in fact, init will be triggered when AI control is enabled).
			player->GetAIStruct()->structTradeAI.needGameStartAIInit = 1;
			// Note: when strategy elements are added to strategy, we'll need to run UpdateStrategyWithUnreferencedExistingUnits
			// if we want AI not to rebuild already existing houses (+docks, boats...)
		}
	}
}


// Process event when (human) player discovers a unit. This is where gaia units are captured by human player.
// This happens only once ! Once an objects has been discovered, this event is never triggered again.
// Return true if the unit must NOT be captured
// Default: return false (normal ROR treatments)
bool CustomRORCommand::HumanSpecific_onCapturableUnitSeen(ROR_STRUCTURES_10C::STRUCT_UNIT *beingSeenUnit, ROR_STRUCTURES_10C::STRUCT_PLAYER *actorPlayer) {
	if (!beingSeenUnit || !beingSeenUnit->IsCheckSumValid()) {
		return false;
	}
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = beingSeenUnit->ptrStructDefUnit;
	if (!unitDef || !unitDef->IsCheckSumValid()) {
		return false;
	}

	CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
	evtInfo.unitId = beingSeenUnit->unitInstanceId;
	evtInfo.playerId = actorPlayer->playerId;
	this->ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_DISCOVER_GAIA_UNIT, evtInfo);

	if (this->crInfo->configInfo.fixHumanPlayer_specificSeeUnit &&
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
bool CustomRORCommand::FindIfGameStartStrategyInitHasBeenDone(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	if (!player) { return true; }
	ROR_STRUCTURES_10C::STRUCT_AI *mainAI = player->GetAIStruct();
	if (!mainAI) { return true; }
	if (HasElementInStrategy(player, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding, CST_UNITID_HOUSE) < 0) {
		// Our current criterion is "no house = not initialized". Can we do better ?
		return false;
	}
	return true;
}



// Overloads the "tactical AI update" event that occurs regularly for each AI player.
// For the algorithm to work well, requires also "FixUnitIdForInProgressBuilding", "FixResetStratElemForUnitId"
void CustomRORCommand::ManageTacAIUpdate(ROR_STRUCTURES_10C::STRUCT_AI *ai) {
	assert(ai && ai->IsCheckSumValid());
	if (!ai || !ai->IsCheckSumValid()) { return; }
	assert(ai->structTacAI.IsCheckSumValid());
	// Get some important / useful objects
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *gameSettings = GetGameSettingsPtr();
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = ai->ptrStructPlayer;
	ROR_STRUCTURES_10C::STRUCT_TAC_AI *tacAI = &ai->structTacAI;
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
	if (tacAI->SNNumber[0x63] < this->crInfo->configInfo.MINVALUE_tacticalAIUpdateDelay) {
		tacAI->SNNumber[0x63] = this->crInfo->configInfo.tacticalAIUpdateDelay; // Update SNScalingFrequency with customROR config value
	}

	int ironAgeResearched = IsTechResearched(player, CST_RSID_IRON_AGE);

	// Fix AI only if config allows improving AI (and NOT in MP games). Keep this info in a local variable.
	bool applyAIFix = (!gameSettings->isMultiplayer && (this->IsImproveAIEnabled(numPlayer)));

	// Only for the FIRST tactical update (last one's time is 0): one-shot initializations
	if (tacAI->lastTacticalUpdateTime <= 0) {
		if (applyAIFix) {
			// Limit farm number
			if (gameSettings->isDeathMatch) {
				if (tacAI->SNNumber[SN_NUMBERS::SNMaxFarms] > this->crInfo->configInfo.maxFarmsInDeathMatch) {
					tacAI->SNNumber[SN_NUMBERS::SNMaxFarms] = this->crInfo->configInfo.maxFarmsInDeathMatch;
					std::string msg = "Fixed farms number to ";
					msg += to_string(this->crInfo->configInfo.maxFarmsInDeathMatch);
					msg += " for player ";
					msg += to_string(numPlayer);
					traceMessageHandler.WriteMessageNoNotification(msg);
				}
				// Don't wait for granary / storage pit for start building strategy elements in deathmatch (nor a minimal exploration %)
				tacAI->SNNumber[SN_NUMBERS::SNRequiredFirstBuilding] = 0; // 0=No restriction. VERY useful in deathmatch.
				tacAI->SNNumber[SN_NUMBERS::SNInitialExplorationRequired] = 1; // I think game default is 2... This change is not needed?
			}
			// TO DO:
			//AdaptStrategyToMaxPopulation(player); // Does not work properly
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


// Analyze strategy and fixes what's necessary. Called every <crInfo.configInfo.tacticalAIUpdateDelay> seconds.
void CustomRORCommand::AnalyzeStrategy(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI) {
	// Exit if AI improvement is not enabled.
	if (!this->IsImproveAIEnabled(buildAI->commonAIObject.playerId)) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	bool difficultyIsEasy = (settings->difficultyLevel >= 3); // True for easy / easiest
	if (difficultyIsEasy) { return; } // Do not improve strategy in easy levels.

	assert(buildAI != NULL);
	assert(buildAI->IsCheckSumValid());

	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstStratElem = &buildAI->fakeFirstStrategyElement;
	if (fakeFirstStratElem == NULL) { return; } // Just a security
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeFirstStratElem->next;
	assert(buildAI->mainAI != NULL);
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = buildAI->mainAI->ptrStructPlayer;
	assert(player != NULL);

	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elemMainForum = NULL;
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
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *resWatchTower = NULL, *resSentryTower = NULL, *resGuardTower = NULL, *resBallistaTower = NULL;
	bool foundBallistics = false;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *wonder = NULL;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *lastNonTowerElement = fakeFirstStratElem;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *goodDevelopmentPointElement = NULL; // Represents the location in strategy where we can assume player is strong enough to insert optional researches. Be careful, AI can skip a certain number of items (10 according to SN numbers)
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *stratElem_bronzeAge = NULL;
	float *resources = (float *)player->ptrResourceValues;

	// Do only 1 loop on strategy and collect all necessary information.
	while (currentStratElem && currentStratElem != fakeFirstStratElem) {
		assert(currentStratElem != NULL);
		// Save some useful elements/information as we're passing on it
		if ((!elemMainForum) && (currentStratElem->unitDAT_ID == CST_UNITID_FORUM)) { elemMainForum = currentStratElem; }
		if (currentStratElem->counter == 1) { foundCounter_one = true; }
		if ((currentStratElem->unitDAT_ID != CST_UNITID_WATCH_TOWER) || (currentStratElem->elementType != AIUCBuilding)) { lastNonTowerElement = currentStratElem; }
		if ((currentStratElem->elementType == AIUCLivingUnit) && (currentStratElem->retrains == -1)) {
			currentPopulation++;
			// minPopulationBeforeBuildOptionalItems+1 : take next one and insert BEFORE (inserting BEFORE an given element preserves insert order consistency. After reverses all).
			if (!goodDevelopmentPointElement && ironAgeResearched && (currentPopulation >= this->crInfo->configInfo.minPopulationBeforeBuildOptionalItems + 1)) {
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
		if (this->crInfo->hasFixForBuildingStratElemUnitId && (currentStratElem->inProgressCount == 1) && (currentStratElem->elementType == AIUCBuilding) && (currentStratElem->unitInstanceId == -1)) {
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

	//ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *endElement; // the element before which to insert, for elements we want to add at the end.
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *optionalsLocation; // the element before which to insert, for optionals elements.
	if (goodDevelopmentPointElement) {
		optionalsLocation = goodDevelopmentPointElement;
	} else {
		if (wonder) { optionalsLocation = wonder; }
		else {
			optionalsLocation = lastNonTowerElement; // fakeFirstStratElem->previous if it could not be set during loop, so it's never NULL.
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
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *tmpElemForWheel = stratElem_bronzeAge;
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
		AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GOVERNMENT_SIEGE, AIUCTech, CST_RSID_ARCHITECTURE, player, "customROR-Architecture");
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
	// if (this->crInfo->configInfo.improveAILevel <= ...) or IsImproveAIEnabled(player->playerId)
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
				AddUnitInStrategy_before(buildAI, optionalsLocation, -1, CST_UNITID_GOVERNMENT_SIEGE, AIUCTech, CST_RSID_BALLISTICS, player, "customROR-Ballistics");
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
}


// Returns true if a construction should NOT be triggered.
// Default result is false (standard ROR behavior), this returns true only for specific (custom) cases.
// Warning: it could be wise for this method to be kept simple... and fast. It's called quite often.
bool CustomRORCommand::ShouldNotTriggerConstruction(ROR_STRUCTURES_10C::STRUCT_TAC_AI *tacAI, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *stratElem) {
	if (!tacAI || !tacAI->IsCheckSumValid()) { return false; } // error: use default
	if (!stratElem || !stratElem->IsCheckSumValid()) { return false; } // error: use default

	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return false; } // error: use default

	// AI improvements are not enabled => return default value (like in normal ROR code)
	if (!this->IsImproveAIEnabled(tacAI->commonAIObject.playerId)) {
		return false; // error: use default
	}

	// Easy difficulty levels / MP games: default behavior too
	if (settings->isMultiplayer || (settings->difficultyLevel >= 3)) { return false; } // use default

	ROR_STRUCTURES_10C::STRUCT_AI *ai = tacAI->ptrMainAI;
	if (!ai || !ai->IsCheckSumValid()) { return false; } // error: use default
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = ai->ptrStructPlayer;
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
		if ((settings->difficultyLevel < 2) && (!settings->isCampaign) && (!settings->isScenario)) {
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
		ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = player->ptrStructDefUnitTable[stratElem->unitDAT_ID];
		assert(unitDef && unitDef->IsCheckSumValid());
		if (!unitDef || !unitDef->IsCheckSumValid()) { return true; } // Unit definition does not exist !
		ROR_STRUCTURES_10C::STRUCT_COST *costs = unitDef->costs;
		for (int i = 0; i < 3; i++) {
			if (costs[i].costUsed && (costs[i].costType >= 0) &&
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
			ROR_STRUCTURES_10C::STRUCT_UNIT_BUILDING *unit = (ROR_STRUCTURES_10C::STRUCT_UNIT_BUILDING *)GetUnitStruct(bldId);
			if (unit && unit->IsCheckSumValid()) {
				if (unit->unitStatus < 2) {
					unfinishedBuildings++;
				}
			}
		}
		long int allowedExtraConstructionCount = 0;
		long int villagerTotalCount = tacAI->allVillagers.usedElements;
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
bool CustomRORCommand::RunManagePanicMode_isUsageOfRORCodeWanted(ROR_STRUCTURES_10C::STRUCT_AI *mainAI, long int enemyPlayerId, long int timeSinceLastPanicMode_s, long int currentGameTime_ms) {
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
void CustomRORCommand::ManagePanicMode(ROR_STRUCTURES_10C::STRUCT_AI *mainAI, long int enemyPlayerId, long int timeSinceLastPanicMode_s, long int currentGameTime_ms) {
	// CONFIG
	const long int panicModeDelay = this->crInfo->configInfo.panicModeDelay;
	long int maxPanicModeUnitsInStrategy = this->crInfo->configInfo.maxPanicUnitsCountToAddInStrategy;
	const long int maxPanicModeSeekEnemyDistance = 20; // 0x14. Hardcoded in original code
#define PANIC_MODE_ARRAYS_MAX_SIZE 20 // technical limit to the number of panic mode units
	if (maxPanicModeUnitsInStrategy > PANIC_MODE_ARRAYS_MAX_SIZE) { maxPanicModeUnitsInStrategy = PANIC_MODE_ARRAYS_MAX_SIZE; }

	// Collect context information/variables
	assert(mainAI != NULL);
	assert(mainAI->IsCheckSumValid());
	if (!mainAI || (!mainAI->IsCheckSumValid())) { return; }

	// Other variables
	ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI = &mainAI->structBuildAI;
	ROR_STRUCTURES_10C::STRUCT_INF_AI *infAI = &mainAI->structInfAI;
	ROR_STRUCTURES_10C::STRUCT_TAC_AI *tacAI = &mainAI->structTacAI;
	assert(buildAI->IsCheckSumValid());
	assert(infAI->IsCheckSumValid());
	assert(tacAI->IsCheckSumValid());
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = mainAI->ptrStructPlayer;
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
	ROR_STRUCTURES_10C::STRUCT_PLAYER *enemyPlayer = GetPlayerStruct((short int)enemyPlayerId);
	assert(enemyPlayer != NULL);
	assert(enemyPlayer->IsCheckSumValid());
	if (!enemyPlayer) { return; }
	char enemyCivId = enemyPlayer->civilizationId;


	ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *forumUnitStruct;
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
	ROR_STRUCTURES_10C::STRUCT_UNIT **buildingsList = NULL;
	if (player->ptrBuildingsListHeader) {
		buildingList_count = player->ptrBuildingsListHeader->buildingsArrayElemCount;
		buildingsList = player->ptrBuildingsListHeader->ptrBuildingsArray;
	}
	assert((buildingList_count == 0) || (buildingsList != NULL));
	for (int i = 0; i < buildingList_count; i++) {
		ROR_STRUCTURES_10C::STRUCT_UNIT *loopUnit = NULL;
		assert(player->ptrBuildingsListHeader != NULL);
		loopUnit = buildingsList[i];
		if (loopUnit != NULL) {
			// We are running so much asserts because player->ptrBuildingsListHeader sometimes contain invalid data !
			// It happens because of buildings conversion bug
			assert(loopUnit->IsCheckSumValid());
			assert(GetUnitStruct(player->ptrBuildingsListHeader->ptrBuildingsUnitIDs[i]) == loopUnit);
			assert(GetUnitStruct(loopUnit->unitInstanceId) == loopUnit);
			assert(loopUnit->unitType == GUT_BUILDING);
			assert(loopUnit->ptrStructDefUnit != NULL);
			if ((loopUnit->IsCheckSumValid()) && (GetUnitStruct(player->ptrBuildingsListHeader->ptrBuildingsUnitIDs[i]) == loopUnit) &&
				(loopUnit->unitType == GUT_BUILDING) && (loopUnit->ptrStructDefUnit != NULL)) {
				if ((loopUnit->unitStatus == 2) && (loopUnit->ptrStructDefUnit != NULL)) {
					short int DAT_ID = loopUnit->ptrStructDefUnit->DAT_ID1;
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
				ROR_STRUCTURES_10C::STRUCT_UNIT *invalidUnit = loopUnit;
				player->ptrBuildingsListHeader->ptrBuildingsArray[i] = NULL; // Remove invalid entry from list, it could cause a crash when some code accesses it.
				char buffer[300];
				sprintf_s(buffer, "ERROR: invalid unit list entry for player #%ld. ptr=%08lX. unitId=%ld GUT=%d, index=%d/%ld.\n",
					player->playerId, (long int)invalidUnit, invalidUnit->unitInstanceId, (int)invalidUnit->unitType, i, buildingList_count);
				//AddTraceToFile(buffer);
				traceMessageHandler.WriteMessage(buffer);
				ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *invalidUnitDef = invalidUnit->ptrStructDefUnit;
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
				AddUnitInStrategy((ROR_STRUCTURES_10C::STRUCT_BUILD_AI *) buildAI, 0, 1, unitId_actor, AIUCLivingUnit, unitId_toAdd, player);
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
	long int *addr = (long int *)AOE_OFFSETS_10C::ADDR_VAR_GAME_SETTINGS_STRUCT;
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

	ROR_STRUCTURES_10C::ROR_STRUCT_EXPORTER writer;
	std::string objToString = "";

	fprintf_s(f, "AOE Debug information\n\n");
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *globalStruct = *pGlobalStruct;
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settingsStruct = *pGameSettingsStruct;
	fprintf_s(f, "Global struct = 0x%08lX - %d players (including gaia)\n", (long int)globalStruct, globalStruct->playerTotalCount);
	fprintf_s(f, "Game settings = 0x%08lX - Map seed=%d - Deathmatch=%d\n", (long int)settingsStruct, settingsStruct->actualMapSeed, settingsStruct->isDeathMatch);
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		//fprintf_s(f, "Player %d struct=0x%08lX - Resources=0x%08lX - AI=0x%08lX - %-16s\n", i, (long int)currentPlayer, currentPlayer->ptrResourceValues, currentPlayer->ptrAIStruct, currentPlayer->playerName_length16max);
		
		objToString = writer.ExportStruct(currentPlayer);
		fprintf_s(f, objToString.c_str());
	}

	// Diplomacy values
	fprintf_s(f, "\n*** Diplomacy values ***\n");
	fprintf_s(f, "\nDislike vs p0  p1  p2  p3  p4  p5  p6  p7  p8  - Like vs p0  p1  p2  p3  p4  p5  p6  p7  p8  \n");
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		ROR_STRUCTURES_10C::STRUCT_AI *ai = currentPlayer->GetAIStruct();
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
		ROR_STRUCTURES_10C::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		fprintf_s(f, "\nPlayer %d:\nCounter Class DAT_ID Name                           Actor Unitid InProgress Alive Attempts #created Retrains Ptr\n", i);
		ROR_STRUCTURES_10C::STRUCT_AI *ai = currentPlayer->GetAIStruct();
		if (ai == NULL) {
			fprintf_s(f, "No AI structure for this player.\n");
		} else {
			ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstStratElem = &ai->structBuildAI.fakeFirstStrategyElement;
			ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeFirstStratElem->next;
			while (currentStratElem && (currentStratElem != fakeFirstStratElem)) {
				fprintf_s(f, "%03ld     %1ld     %-3ld    %-30s %-3ld   %-6ld %-2ld         %-2ld    %-4ld     %-4ld     %-3ld      0x%08lX\n", currentStratElem->counter, currentStratElem->elementType, currentStratElem->unitDAT_ID,
					currentStratElem->unitName, currentStratElem->actor, currentStratElem->unitInstanceId, currentStratElem->inProgressCount,
					currentStratElem->aliveCount, currentStratElem->buildAttempts, currentStratElem->totalCount, currentStratElem->retrains, (long int)currentStratElem
					);
				currentStratElem = currentStratElem->next;
			}
		}
	}

	fprintf_s(f, "\n*** SN values ***\n");
	for (int i = 0; i < globalStruct->playerTotalCount; i++) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *currentPlayer = globalStruct->GetPlayerStructPtrTable()[i];
		fprintf_s(f, "\nPlayer %d:\n", i);
		ROR_STRUCTURES_10C::STRUCT_AI *ai = currentPlayer->GetAIStruct();
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


// Procedure to calculate conversion resistance for a giver unit from a given civ.
// This replaces game's algorithm.
float CustomRORCommand::GetConversionResistance(char civId, short int unitClass) {
	// Standard resistances (original game)
	// Macedonian
	if (civId == CST_CIVID_MACEDONIAN) {
		return this->crInfo->configInfo.conversionResistance_Macedonian;
	}
	// Special unit classes
	if (
		(unitClass == 21) || // TribeAIGroupFishingBoat
		(unitClass == 2) || // trade boat
		(unitClass == 20) || // TribeAIGroupTransportBoat
		(unitClass == 22)) // TribeAIGroupWarBoat
	{
		return this->crInfo->configInfo.conversionResistance_Boats;
	}
	if (unitClass == 18) { // TribeAIGroupPriest
		return this->crInfo->configInfo.conversionResistance_Priests;
	}
	if (
		(unitClass == 35) || // Chariot
		(unitClass == 23) // Chariot archer
		) {
		return this->crInfo->configInfo.conversionResistance_Chariots;
	}

	// Custom resistances (only for single player games)
	if ((!IsMultiplayer()) && (unitClass == 24)) { // War elephant, armored elephant, including Hannibal
		return (civId == CST_CIVID_PERSIAN) ? this->crInfo->configInfo.conversionResistance_WarElephants_Persian : this->crInfo->configInfo.conversionResistance_WarElephants;
	}
	return 1;
}


// Returns how many units were told to move.
// Returns <0 if there is an error
int CustomRORCommand::MoveIdleMilitaryUnitsToMousePosition(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, float maxDistance) {
	if (!player) { return -1; }
	ROR_STRUCTURES_10C::STRUCT_AI *ai = player->GetAIStruct();
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
		ROR_STRUCTURES_10C::STRUCT_UNIT *unit = NULL;
		if (unitId > 0) {
			 unit = GetUnitStruct(unitId);
		}
		if (unit != NULL) {
			ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = unit->ptrStructDefUnit;
			assert(unitDef != NULL);
			if ((unit->unitType == GLOBAL_UNIT_TYPES::GUT_LIVING_UNIT) &&
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
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *globalStruct = *pGlobalStruct;
	short int playerId = globalStruct->humanPlayerId;
	if ((playerId < 0) || (playerId > 8)) { return; }

	if (IsMultiplayer()) { return; } // MP protection
	ROR_STRUCTURES_10C::STRUCT_PLAYER **playerTable = globalStruct->GetPlayerStructPtrTable();
	if (!playerTable || !playerTable[playerId]) { return; }
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = playerTable[playerId];
	this->MoveIdleMilitaryUnitsToMousePosition(player, (float) this->crInfo->configInfo.distanceToCallNearbyIdleMilitaryUnits);
}


// Main method to manage "OnCreate" event for living units.
// This event occurs after the constructor has finished its execution.
// This event is triggered for all living units (type=70), not others.
// This event is triggered during game but as well in scenario editor or during game creation.
// actionStruct parameter can be NULL if it could not be determined
// Note: we choose NOT to apply custom treatments when controlled player has an active playing AI (both controlled by human and AI)
void CustomRORCommand::OnLivingUnitCreation(AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS UIStatus, ROR_STRUCTURES_10C::STRUCT_UNIT *unit,
	ROR_STRUCTURES_10C::STRUCT_ACTION_MAKE_OBJECT *actionStruct) {
	if (!unit) { return; }
	if (UIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) {
		return;
	}
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = unit->ptrStructPlayer;
	assert(player != NULL);
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *globalStruct = *pGlobalStruct;
	assert(globalStruct != NULL);
	// Does spawned unit belong to currently human-controlled player ?
	if (player->playerId != globalStruct->humanPlayerId) {
		return; // At this point we do nothing when AI players have a new unit... (nor gaia)
	}
	if (IsMultiplayer()) { return; } // can provoke out of sync errors

	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = unit->ptrStructDefUnit;
	assert(unitDef != NULL);
	// Assign a shortcut to new unit if config says to - and only if AI is not active for this player
	if (!player->IsAIActive(this->crInfo->hasManageAIFeatureON)) {
		AutoAssignShortcutToUnit(unit);
	}

	// Get info on parent unit if possible
	ROR_STRUCTURES_10C::STRUCT_UNIT *parentUnit = NULL;
	long int parentUnitId = -1;
	if (actionStruct) {
		parentUnit = actionStruct->actor;
		assert(parentUnit->IsCheckSumValid());
		if (!parentUnit->IsCheckSumValid()) { parentUnit = NULL; }
		if (parentUnit) {
			parentUnitId = parentUnit->unitInstanceId;
		}
	}

	UnitCustomInfo *parentInfo = NULL;
	if (parentUnitId >= 0) {
		assert(this->crInfo != NULL);
		parentInfo = this->crInfo->myGameObjects.FindUnitCustomInfo(parentUnitId); // Result can be NULL (not found)
	}

	bool commandCreated = false; // To make sure we create only 1 command (or 0)

	// Auto target
	if (this->crInfo->configInfo.enableSpawnUnitsAutoTarget && !player->IsAIActive(this->crInfo->hasManageAIFeatureON) &&
		parentInfo && (parentInfo->spawnTargetUnitId >= 0)) {
		ROR_STRUCTURES_10C::STRUCT_UNIT *target = GetUnitStruct(parentInfo->spawnTargetUnitId);
		// Note: target may not exist (anymore)
		if (target && target->IsCheckSumValid()) {
			if (unitDef->ptrUnitCommandHeader) {
				if (GetUnitDefCommandForTarget(unit, target, true) != NULL) {
					TellUnitToInteractWithTarget(unit, target);
					commandCreated = true;
				}
			}
		} else {
			if (parentInfo) { // double-check ;)
				this->crInfo->myGameObjects.RemoveUnitCustomInfo(parentInfo->spawnTargetUnitId);
			}
		}
	}

	// Move spawned unit to building's target location if we have/found one. And if option is enabled.
	if (!commandCreated && this->crInfo->configInfo.enableSpawnUnitsMoveToLocation && !player->IsAIActive(this->crInfo->hasManageAIFeatureON) &&
		parentInfo && (parentInfo->spawnUnitMoveToPosX >= 0) && (parentInfo->spawnUnitMoveToPosY >= 0)) {
		MoveUnitToTargetOrPosition(unit, NULL, parentInfo->spawnUnitMoveToPosX, parentInfo->spawnUnitMoveToPosY);
		commandCreated = true;
	}

	// Auto-repair TC for villagers
	if (this->crInfo->configInfo.enableSpawnUnitAutoRepairTC && IsVillager(unit->ptrStructDefUnit->DAT_ID1) &&
		parentUnit && !player->IsAIActive(this->crInfo->hasManageAIFeatureON)) {
		if (parentUnit->remainingHitPoints < (float)parentUnit->ptrStructDefUnit->totalHitPoints) {
			TellUnitToInteractWithTarget(unit, parentUnit);
			commandCreated = true;
		}
	}
}


// Fixes missing treatments when a unit changes owner (eg. unit conversion)
// This should be called before the actual unit owner change process is called.
// targetUnit is the "victim" (unit that changes owner), actorPlayer is the new owner (player)
// Technical note: in ROR, unit.changeOwner(newplayer) is [EDX+0x44] call.
void CustomRORCommand::OnUnitChangeOwner_fixes(ROR_STRUCTURES_10C::STRUCT_UNIT *targetUnit, ROR_STRUCTURES_10C::STRUCT_PLAYER *actorPlayer) {
	if (!targetUnit || !actorPlayer) { return; }
	assert(targetUnit->IsCheckSumValid());
	assert(actorPlayer->IsCheckSumValid());
	ROR_STRUCTURES_10C::STRUCT_PLAYER *targetPlayer = targetUnit->ptrStructPlayer; // The victim (if unit has been converted)
	assert(targetPlayer != NULL);
	assert(targetPlayer->IsCheckSumValid());
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *targetUnitDef = targetUnit->ptrStructDefUnit;
	assert(targetUnitDef != NULL);
	assert(targetUnitDef->IsCheckSumValid());

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
			IsTower(targetUnitDef->DAT_ID1) || (targetUnitDef->unitType == GUT_LIVING_UNIT)
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
	ROR_STRUCTURES_10C::STRUCT_AI *mainAI_actor = actorPlayer->GetAIStruct();
	if (mainAI_actor == NULL) { return; }
	ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI_actor = &mainAI_actor->structBuildAI;
	assert(buildAI_actor != NULL);
	if (buildAI_actor == NULL) { return; }
	UpdateStrategyWithExistingUnit(buildAI_actor, targetUnit);
}


// Change a unit's owner, for example like a conversion.
// Capturing an artefact does NOT call this.
// I don't see any other possible event than CST_ATI_CONVERT. Use CST_GET_INVALID to trigger NO notification.
bool CustomRORCommand::ChangeUnitOwner(ROR_STRUCTURES_10C::STRUCT_UNIT *targetUnit, ROR_STRUCTURES_10C::STRUCT_PLAYER *actorPlayer, 
	AOE_CONST_INTERNAL::GAME_EVENT_TYPES notifyEvent) {
	if (!targetUnit || !actorPlayer || !targetUnit->IsCheckSumValid() || !actorPlayer->IsCheckSumValid()) {
		return false;
	}
	ROR_STRUCTURES_10C::STRUCT_PLAYER *oldOwner = targetUnit->ptrStructPlayer;
	this->OnUnitChangeOwner_fixes(targetUnit, actorPlayer);
	AOE_ChangeUnitOwner(targetUnit, actorPlayer);
	if ((notifyEvent == AOE_CONST_INTERNAL::CST_GET_INVALID) || (!oldOwner || !oldOwner->IsCheckSumValid())) {
		return true; // No notification to handle
	}

	// Not handled: other events than conversion (is there any ?)

	// Warning, for a conversion, arg2= actor playerId (priest), arg3=victim playerId
	AOE_callNotifyEvent(notifyEvent, actorPlayer->playerId, oldOwner->playerId, (long int)targetUnit->positionY, (long int)targetUnit->positionX);
	return true;
}


// Custom Fixes/features on player.addUnit calls.
void CustomRORCommand::OnPlayerAddUnitCustomTreatments(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, ROR_STRUCTURES_10C::STRUCT_UNIT *unit, bool isTempUnit, bool isNotCreatable) {
	if (!unit || !unit->IsCheckSumValid()) { return; }
	if (!player || !player->IsCheckSumValid()) { return; }

	if (!isNotCreatable && !isTempUnit) {
		// We have a creatable unit
		if ((unit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) && (unit->unitStatus == 2)) {
			// Add building to player buildings list (bug in original game), this is done in unit constructor but NOT at unit conversion
			ROR_STRUCTURES_10C::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader = player->ptrBuildingsListHeader;
			assert(buildingsHeader != NULL);
			if (!buildingsHeader) {
				traceMessageHandler.WriteMessage("buildingsHeader is NULL for player #" + std::to_string(player->playerId));
				return;
			}

			long int posX = (long int)unit->positionX;
			long int posY = (long int)unit->positionY;
			unsigned long int **ppMapInfo = (unsigned long int **)AOE_OFFSETS_10C::ADDR_MAP_VISIBILITY_INFO;
			unsigned long int *pMapInfoX = ppMapInfo[posX];
			unsigned long int *pMapInfo = &pMapInfoX[posY];

			// Trick for conversion: addBuildingToArrays does not work when unit->player is not the good owner player
			// So we update it temporarily (ROR code will actually update it later)
			ROR_STRUCTURES_10C::STRUCT_PLAYER *oldUnitPlayer = unit->ptrStructPlayer;
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
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return; }
	long int playerTotalCount = global->playerTotalCount;
	if ((global->gameRunStatus == 0) && (global->currentGameTime > 0) && (!isTempUnit && !isNotCreatable) &&
		(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING)) {
		for (int curPlayerId = 1; curPlayerId < playerTotalCount; curPlayerId++) {
			ROR_STRUCTURES_10C::STRUCT_PLAYER *curPlayer = GetPlayerStruct(curPlayerId);
			ROR_STRUCTURES_10C::STRUCT_AI *curAI = curPlayer->ptrAIStruct;
			if (curAI && curAI->IsCheckSumValid() && curAI->structInfAI.IsCheckSumValid()) {
				UpdateUnitOwnerInfAIUnitListElem(&curAI->structInfAI, (ROR_STRUCTURES_10C::STRUCT_UNIT_BASE*)unit, player->playerId);
			}
		}
	}

}


// Custom Fixes/features on player.removeUnit calls.
// Here unit status can be 2 (if conversion), 7 (quitting editor?), 8 (in-game dying unit) ? +maybe other values
// Warning: this method is called in many situations, game might NOT be running.
void CustomRORCommand::OnPlayerRemoveUnit(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, ROR_STRUCTURES_10C::STRUCT_UNIT *unit, bool isTempUnit, bool isNotCreatable) {
	if (!player || !unit) { return; }
	assert(player->IsCheckSumValid() && unit->IsCheckSumValid());
	if (!player->IsCheckSumValid() || !unit->IsCheckSumValid()) { return; }

	// Gather information
	assert(unit->ptrStructPlayer != NULL);
	if (!unit->ptrStructPlayer) { return; }
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDefBase = unit->GetUnitDefBase();
	assert(unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass());
	if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) { return; }
	bool isBuilding = (unitDefBase->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING); // Warning: using unit->unitType is risky (not always correct?)
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());

	// Update AI struct unit lists that are never updated by ROR (we choose to do this only if AI improvement is enabled).
	if (player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid() && (unit->unitInstanceId >= 0) && this->IsImproveAIEnabled(player->playerId)) {
		ROR_STRUCTURES_10C::STRUCT_INF_AI *infAI = &player->ptrAIStruct->structInfAI;
		assert(infAI->IsCheckSumValid());
		// unitElemList (infAI)
		long int size = infAI->unitElemListSize;
		for (long int i = 0; i < size; i++) {
			ROR_STRUCTURES_10C::STRUCT_INF_AI_UNIT_LIST_ELEM *curElem = &infAI->unitElemList[i];
			// I lost one of my units ? update infAI list !
			if ((curElem->playerId == player->playerId) && (curElem->unitId == unit->unitInstanceId)) {
				if (unitDefBase->unitAIType == TribeAIGroupArtefact) {
					// Artefact don't die, just get captured. BUT we don't know who just captured it (will be updated afterwards)
					// The Add unit treatment (for other player) will update this information (again), so in fact it is not really useful to set playerId to 0.
					curElem->playerId = 0;
				} else {
					ResetInfAIUnitListElem(curElem);
				}
			}
		}
		// Manage the 3 STRUCT_AI_UNIT_LIST_INFO from infAI in which elements are not removed by ROR
		if (IsClassArtefactOrGatherableOrCreatable(unitDefBase->unitAIType)) {
			infAI->creatableAndGatherableUnits.Remove(unit->unitInstanceId);
		}
		if (IsClassPlayerCreatable(unitDefBase->unitAIType)) {
			infAI->playerCreatableUnits.Remove(unit->unitInstanceId);
		}
		if ((unitDefBase->unitAIType == TribeAIGroupArtefact) || (unitDefBase->unitAIType == TribeAIGroupFlag)) {
			infAI->artefactsAndFlags.Remove(unit->unitInstanceId);
		}
		// Hardcoded list of units: cf 0x4C1730 = infAI.AddUnitToDefend(unitStruct)
		if ((unitDefBase->DAT_ID1 == CST_UNITID_FORUM) || (unitDefBase->DAT_ID1 == CST_UNITID_DOCK) ||
			(unitDefBase->DAT_ID1 == CST_UNITID_RELIC) ||
			(unitDefBase->DAT_ID1 == CST_UNITID_RUIN) || (unitDefBase->DAT_ID1 == CST_UNITID_RUIN2) ||
			(unitDefBase->unitAIType == TribeAIGroupStoneMine) || (unitDefBase->unitAIType == TribeAIGroupGoldMine) ||
			(unitDefBase->unitAIType == TribeAIGroupBerryBush)) {
			// Note: gaia elements to defend are never removed when deleted (depleted)... (because not belonging to the same player !)
			infAI->elementsToDefend.Remove(unit->unitInstanceId);
		}
	}

	if (!isNotCreatable && !isTempUnit) {
		// We have a creatable unit
		if (isBuilding && (unit->unitStatus == 2)) { // status=2 means it is a conversion
			// Remove building from player buildings list (bug in original game), this is done in unit destructor but NOT at unit conversion
			// Note that because of this call, the remove operation is done twice (will be done again in destructor). But the 2nd time, it will just do nothing.
			ROR_STRUCTURES_10C::STRUCT_PLAYER_BUILDINGS_HEADER *buildingsHeader = player->ptrBuildingsListHeader;
			AOE_playerBldHeader_RemoveBldFromArrays(buildingsHeader, unit);
		}

		// Fix constructions history array
		if (isBuilding && player->ptrAIStruct && player->ptrAIStruct->IsCheckSumValid() &&
			// Not fixed in easiest... But as it is a technical fix, we apply it even if "improve AI" is not enabled.
			// Note that it has a strong (positive) impact on AI farming + the fact that granary/SP construction is no longer blocked for no valid reason
			settings->difficultyLevel < 4) {
			ROR_STRUCTURES_10C::STRUCT_INF_AI *infAI = &player->ptrAIStruct->structInfAI;
			assert(infAI->IsCheckSumValid());
			if ((infAI->buildHistoryArray != NULL) && (infAI->buildHistoryArraySize > 0)) {
				// Disable or reset element for current building
				long int posX = (long int)unit->positionX;
				long int posY = (long int)unit->positionY;
				ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDef = unit->GetUnitDefBase();
				if (unitDef && unitDef->IsCheckSumValidForAUnitClass()) {
					// Setting status to "reset" (3 / =removed) will unblock further construction of same kind of buildings, especially granary/SP.
					// This could also fix the bad farms placement after some game time (because "existing" farms apply a negative likee value on nearby tiles) ? To verify
					// Moreover, it allows the array data to be valid !
					AOE_InfAIBuildHistory_setStatus(infAI, posX, posY, unitDef->DAT_ID1, AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS::CST_BHS_REMOVED);
				}
			}
		}
	}

	// Auto rebuild farms
	if (unit && unit->IsCheckSumValid() && isBuilding && this->crInfo->configInfo.enableAutoRebuildFarms) {
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDef = unit->GetUnitDefBase();
		if (unitDef && unitDef->IsCheckSumValidForAUnitClass() && (unitDef->DAT_ID1 == CST_UNITID_FARM) && player->ptrGlobalStruct) {
			this->crInfo->myGameObjects.FlushObsoleteFarmInfo(player->ptrGlobalStruct->currentGameTime);
			FarmRebuildInfo *fInfo = this->crInfo->myGameObjects.FindFarmRebuildInfo(unit->unitInstanceId);
			if (fInfo && (fInfo->playerId == player->playerId) && IsUnitAvailableForPlayer(CST_UNITID_FARM, player)) {
				// As long as we use a game command, it is compatible with multiplayer.
				CreateCmd_Build(fInfo->villagerUnitId, CST_UNITID_FARM, fInfo->posX, fInfo->posY);
			}
			this->crInfo->myGameObjects.RemoveFarmRebuildInfo(unit->unitInstanceId);
		}
	}

	// Triggers
	if (unit && unit->IsCheckSumValid() && !isTempUnit && (settings->currentUIStatus == AOE_CONST_INTERNAL::GSUS_PLAYING) &&
		!settings->isMultiplayer && (unit->unitInstanceId >= 0)) {
		CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
		evtInfo.unitId = unit->unitInstanceId;
		this->ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_UNIT_LOSS, evtInfo);
	}
}


// Returns true if a shortcut has been added/modified
bool CustomRORCommand::AutoAssignShortcutToUnit(ROR_STRUCTURES_10C::STRUCT_UNIT *unit) {
	if (!unit || !unit->IsCheckSumValid()) { return false; }
	// Only care about living units or building
	if ((unit->unitType != GUT_BUILDING) && (unit->unitType != GUT_LIVING_UNIT)) {
		return false;
	}
	// We choose not to modify existing shortcut
	if (unit->shortcutNumber != 0) { return false; }

	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = unit->ptrStructDefUnit;
	assert(unitDef != NULL);

	int shortcutId = this->crInfo->configInfo.unitShortcutsPriorityReverseOrder ? 9 : 1;
	for (int currentIndex = 1; currentIndex < CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS; currentIndex++) {
		if (this->crInfo->configInfo.unitShortcutsPriorityReverseOrder) {
			shortcutId = CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS - currentIndex;
		}
		assert(shortcutId < CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS);
		assert(shortcutId >= 0);
		UnitSpawnShortcutInfo *shortcutInfo = &this->crInfo->configInfo.unitShortcutsInformation[shortcutId];
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
bool CustomRORCommand::ShouldChangeTarget(ROR_STRUCTURES_10C::STRUCT_UNIT_ACTIVITY *activity, long int targetUnitId) {
	assert(this->distanceToConsiderVeryClose > 0);

	// Error cases: let ROR code handle them.
	if (!activity) { return true; }
	if (targetUnitId < 0) { return true; } // important check because we call GetUnitStruct on this.
	//assert(activity->IsCheckSumValid());

	ROR_STRUCTURES_10C::STRUCT_UNIT *actorUnit = activity->ptrUnit;
	ROR_STRUCTURES_10C::STRUCT_UNIT *newTargetUnit = GetUnitStruct(targetUnitId);
	ROR_STRUCTURES_10C::STRUCT_UNIT *oldTargetUnit = GetUnitStruct(activity->targetUnitId);
	if (!actorUnit || !newTargetUnit /*|| !oldTargetUnit*/) { return true; }
	if (!oldTargetUnit) {
		// activity target unit can be reset by pending treatments. Find if action HAS a valid target. We may want to keep this target.
		ROR_STRUCTURES_10C::STRUCT_ACTION_BASE *action = GetUnitAction(actorUnit); // May return NULL
		if (!action || (action->targetUnitId < 0)) {
			return true;
		}
		oldTargetUnit = GetUnitStruct(action->targetUnitId);
		activity->targetUnitId = action->targetUnitId; // A bit dirty. Fixes the activity.targetUnitId==-1 case.
	}
	if (!oldTargetUnit) { return true; } // We really could not find the target, it seems there is none: change target !

	assert(newTargetUnit->IsCheckSumValid());
	assert(actorUnit->IsCheckSumValid());
	if (!newTargetUnit->IsCheckSumValid() || !actorUnit->IsCheckSumValid()) {
		return true; // invalid data. Let original code be executed.
	}
	if (oldTargetUnit == newTargetUnit) {
		return false;
	} // same unit: do not change (=no stupid hesitation)

	// Checks for invalid/dead targets
	if ((oldTargetUnit->remainingHitPoints <= 0) || (oldTargetUnit->unitType < GLOBAL_UNIT_TYPES::GUT_LIVING_UNIT) ||
		(oldTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_TREE) || (oldTargetUnit->unitStatus > 2)) {
		return true; // switch
	}
	if ((newTargetUnit->remainingHitPoints <= 0) || (newTargetUnit->unitType < GLOBAL_UNIT_TYPES::GUT_LIVING_UNIT) ||
		(newTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_TREE) || (newTargetUnit->unitStatus > 2)) {
		return false; // Keep. Not sure if this case can happen. Maybe when a projectile from a dead unit hits me afterwards ?
	}
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
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
	ROR_STRUCTURES_10C::STRUCT_PLAYER *actorPlayer = actorUnit->ptrStructPlayer;
	assert(actorPlayer != NULL);
	assert(actorPlayer->IsCheckSumValid());
	float *resources = (float *)actorPlayer->ptrResourceValues;
	assert(resources != NULL);
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *actorUnitDef = actorUnit->ptrStructDefUnit;
	assert(actorUnitDef != NULL);
	assert(actorUnitDef->IsCheckSumValid());
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
	assert(newTargetUnit->ptrStructDefUnit != NULL);
	assert(oldTargetUnit->ptrStructDefUnit != NULL);
	if ((newTargetUnit->ptrStructDefUnit == NULL) || (oldTargetUnit->ptrStructDefUnit == NULL)) {
		return true; // error case
	}

	// Is AI Improvement feature ON ?
	if (!this->IsImproveAIEnabled(actorPlayer->playerId)) {
		return true; // AI improvement is disabled: let ROR original code do its normal behavior.
	}
	// Do not improve in easy levels
	if (GetGameSettingsPtr() && (GetGameSettingsPtr()->difficultyLevel >= 3)) { return true; }

	// A target is mine or allied
	if (actorUnit->ptrStructPlayer->ptrDiplomacyStances[oldTargetUnit->ptrStructPlayer->playerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY) {
		return true; // change to new because old target is allied (may happen after a conversion or maybe diplomacy change)
	}
	if (actorUnit->ptrStructPlayer->ptrDiplomacyStances[newTargetUnit->ptrStructPlayer->playerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY) {
		return false; // keep old because new target is allied (can this happen ? Not sure)
	}
	
	bool newTargetIsVillager = (newTargetUnit->ptrStructDefUnit->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian);// IsVillager(newTargetUnit->ptrStructDefUnit->DAT_ID1);
	bool actionTargetIsVillager = (oldTargetUnit->ptrStructDefUnit->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian); // IsVillager(oldTargetUnit->ptrStructDefUnit->DAT_ID1);
	bool newTargetIsPriest = (newTargetUnit->ptrStructDefUnit->DAT_ID1 == CST_UNITID_PRIEST);
	bool actionTargetIsPriest = (oldTargetUnit->ptrStructDefUnit->DAT_ID1 == CST_UNITID_PRIEST);
	bool newTargetIsTower = AOE_CONST_FUNC::IsTower(newTargetUnit->ptrStructDefUnit->DAT_ID1);
	bool actionTargetIsTower = AOE_CONST_FUNC::IsTower(oldTargetUnit->ptrStructDefUnit->DAT_ID1);
	bool newTargetIsWall = (newTargetUnit->ptrStructDefUnit->unitAIType == TribeAIGroupWall);
	bool actionTargetIsWall = (oldTargetUnit->ptrStructDefUnit->unitAIType == TribeAIGroupWall);
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
	if (!isOldTargetVisible && (oldTargetUnit->ptrStructDefUnit->unitType != GLOBAL_UNIT_TYPES::GUT_BUILDING)) {
		return true; // Change target
	}

	// Special: actor is priest ("almost killed" values don't have any importance here). Against tower, consider "melee" distance, not priest's range
	if (actorIsPriest) {
		// Additional information
		bool canConvertTowers = (resources[RESOURCE_TYPES::CST_RES_ORDER_CAN_CONVERT_BUILDING] > 0);
		bool alreadyVeryClose_oldTarget_melee = (squareDistanceOldTarget <= distanceToConsiderVeryClose); // For priests (that have both range and "melee" (vs buildings) conversion)
		bool alreadyVeryClose_newTarget_melee = (squareDistanceOldTarget <= distanceToConsiderVeryClose);
		bool newTargetIsChariot = (newTargetUnit->ptrStructDefUnit->unitAIType == TribeAIGroupChariot) ||
			(newTargetUnit->ptrStructDefUnit->unitAIType == TribeAIGroupChariotArcher);
		bool actionTargetIsChariot = (oldTargetUnit->ptrStructDefUnit->unitAIType == TribeAIGroupChariot) ||
			(oldTargetUnit->ptrStructDefUnit->unitAIType == TribeAIGroupChariotArcher);

		// Top priority: priests can't convert TC, wonder, walls or unfinished buildings => next 2 rules
		if ((newTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) && (newTargetUnit->unitStatus < 2)) {
			return false; // Force keep if "new" target is a not-fully built building.
		}
		if (newTargetIsWall || (newTargetUnit->ptrStructDefUnit->DAT_ID1 == CST_UNITID_FORUM) || (newTargetUnit->ptrStructDefUnit->DAT_ID1 == CST_UNITID_WONDER)) {
			return false; // Force keep if "new" target can't be converted.
		}
		if ((oldTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) && (oldTargetUnit->unitStatus < 2)) {
			return true; // Change if current target is a not-fully built building
		}
		if (actionTargetIsWall || (oldTargetUnit->ptrStructDefUnit->DAT_ID1 == CST_UNITID_FORUM) || (oldTargetUnit->ptrStructDefUnit->DAT_ID1 == CST_UNITID_WONDER)) {
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
			(oldTargetUnit->ptrStructDefUnit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) &&
			(newTargetUnit->ptrStructDefUnit->unitType != GLOBAL_UNIT_TYPES::GUT_BUILDING)
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
		bool actionTargetIsBuilding = (oldTargetUnit->ptrStructDefUnit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING);
		bool targetIsBuilding = (newTargetUnit->ptrStructDefUnit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING);

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
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->ptrStructDefUnit->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon)) {
		return false; // force keep
	}
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->ptrStructDefUnit->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher)) {
		return false; // force keep
	}
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->ptrStructDefUnit->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher)) {
		return false; // force keep
	}
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->ptrStructDefUnit->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher)) {
		return false; // force keep
	}
	// Slingers: attack them, but NOT if I am an archer
	if (alreadyVeryClose_oldTarget && !actorIsArcher && (oldTargetUnit->ptrStructDefUnit->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger)) {
		return false; // force keep
	}
	// Enemy tower: keep if I am war elephant
	if (alreadyVeryClose_oldTarget && (actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider) && (IsTower(oldTargetUnit->ptrStructDefUnit->DAT_ID1))) {
		return false; // force keep
	}


	// If only 1 of the 2 targets has me as its target, attack this one
	ROR_STRUCTURES_10C::STRUCT_ACTION_BASE *newTargetUnitAction = NULL;
	ROR_STRUCTURES_10C::STRUCT_ACTION_BASE *actionTargetUnitAction = NULL;
	if ((newTargetUnit->ptrActionInformation) && (newTargetUnit->ptrActionInformation->ptrActionLink) &&
		(newTargetUnit->ptrActionInformation->ptrActionLink->actionStruct)) {
		newTargetUnitAction = (ROR_STRUCTURES_10C::STRUCT_ACTION_BASE *)newTargetUnit->ptrActionInformation->ptrActionLink->actionStruct;
	}
	if ((oldTargetUnit->ptrActionInformation) && (oldTargetUnit->ptrActionInformation->ptrActionLink) &&
		(oldTargetUnit->ptrActionInformation->ptrActionLink->actionStruct)) {
		actionTargetUnitAction = (ROR_STRUCTURES_10C::STRUCT_ACTION_BASE *)oldTargetUnit->ptrActionInformation->ptrActionLink->actionStruct;
	}
	bool newTargetAttacksMe = newTargetUnitAction && (
		(newTargetUnitAction->actionTypeID == AOE_CONST_INTERNAL::INTERNAL_ACTION_ID::CST_IAI_ATTACK_7) ||
		(newTargetUnitAction->actionTypeID == AOE_CONST_INTERNAL::INTERNAL_ACTION_ID::CST_IAI_ATTACK_9) ||
		(newTargetUnitAction->actionTypeID == AOE_CONST_INTERNAL::INTERNAL_ACTION_ID::CST_IAI_CONVERT)
		) &&
		(newTargetUnitAction->targetUnit == actorUnit);
	bool actionTargetAttacksMe = actionTargetUnitAction && (
		(actionTargetUnitAction->actionTypeID == AOE_CONST_INTERNAL::INTERNAL_ACTION_ID::CST_IAI_ATTACK_7) ||
		(actionTargetUnitAction->actionTypeID == AOE_CONST_INTERNAL::INTERNAL_ACTION_ID::CST_IAI_ATTACK_9) ||
		(actionTargetUnitAction->actionTypeID == AOE_CONST_INTERNAL::INTERNAL_ACTION_ID::CST_IAI_CONVERT)
		) &&
		(actionTargetUnitAction->targetUnit == actorUnit);

	if (newTargetAttacksMe && !actionTargetAttacksMe) {
		return true; // change
	}
	if (!newTargetAttacksMe && actionTargetAttacksMe) {
		return false; // force keep
	}

	// Same enemy unit AI type: just compare HP (they are similar units)
	if (newTargetUnit->ptrStructDefUnit->unitAIType == oldTargetUnit->ptrStructDefUnit->unitAIType) {
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
bool CustomRORCommand::ShouldAttackTower_towerPanic(ROR_STRUCTURES_10C::STRUCT_UNIT *actorUnit, ROR_STRUCTURES_10C::STRUCT_UNIT *enemyTower) {
	if (this->crInfo->configInfo.improveAILevel <= 0) {
		return true; // improve AI is disabled. Return default value.
	}
	
	// Run various consistency checks
	assert(this->distanceToConsiderVeryClose > 0);
	if (!actorUnit || !enemyTower) { return false; }
	assert(actorUnit->currentActivity != NULL);
	assert(actorUnit->ptrStructDefUnit != NULL);
	assert(enemyTower->ptrStructDefUnit != NULL);
	if ((!actorUnit->currentActivity) || (!actorUnit->ptrStructDefUnit) || (!enemyTower->ptrStructDefUnit)) { return false; }
	ROR_STRUCTURES_10C::STRUCT_UNIT_ACTIVITY *activity = actorUnit->currentActivity; // Guaranteed non-NULL
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *actorUnitDef = actorUnit->ptrStructDefUnit; // Guaranteed non-NULL
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *enemyTowerDef = enemyTower->ptrStructDefUnit; // Guaranteed non-NULL
	assert(actorUnitDef->IsCheckSumValid());
	assert(enemyTowerDef->IsCheckSumValid());
	if (!actorUnitDef->IsCheckSumValid() || !enemyTowerDef->IsCheckSumValid()) { return false; }

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
			ROR_STRUCTURES_10C::STRUCT_UNIT_ACTION_INFO * actionInfo = actorUnit->ptrActionInformation;
			if (actionInfo) {
				_asm {
					MOV ECX, actionInfo
					MOV EAX, 0x403E50
					CALL EAX // removeAction
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
	ROR_STRUCTURES_10C::STRUCT_UNIT *currentTarget = GetUnitStruct(currentTargetId);
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *currentTargetDef = NULL;
	if (currentTarget) {
		currentTargetDef = currentTarget->ptrStructDefUnit;
	} else {
		return true;
	}

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
		if ((activity->currentActionId == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_BUILD) && 
			(currentTarget->ptrStructPlayer == actorUnit->ptrStructPlayer)// Just a check
			) {
			if (currentTargetDef->DAT_ID1 == CST_UNITID_FORUM) {
				return false; // Keep building TC
			}
			if (currentTarget->constructionProgress > 85) {
				return false; // Almost finished: continue !
			}
			if ((currentTarget->constructionProgress > 75) && (IsTower(currentTargetDef->DAT_ID1) )) {
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
	if ((currentTargetDef->unitAIType == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) && (currentTargetDef->displayedAttack > 0)
		&& (distanceToCurrentTarget < 18)) { // distanceToCurrentTarget < 18: approximative, but we don't want to search for TC + calculate distance, it's unnecessary processing.
		float HPDifference = currentTarget->remainingHitPoints - enemyTower->remainingHitPoints;
		float attackDifference = (float)(currentTargetDef->displayedAttack - enemyTowerDef->displayedAttack);
		
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
void CustomRORCommand::towerPanic_LoopOnVillagers(ROR_STRUCTURES_10C::STRUCT_TAC_AI *tacAI, ROR_STRUCTURES_10C::STRUCT_UNIT *enemyTower,
	long int *pAssignedUnitsCount, ROR_STRUCTURES_10C::STRUCT_POSITION_INFO *pTCPositionInfo) {
	// Invalid/NULL objects ?
	if (!tacAI || !enemyTower || !pTCPositionInfo || !tacAI->IsCheckSumValid() || !enemyTower->IsCheckSumValid() || !tacAI->ptrMainAI) { return; }
	if (*pAssignedUnitsCount < 0) { return; } // Bad input value
	if (*pAssignedUnitsCount >= 6) { return; } // Already reached maximum assigned tasks
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = tacAI->ptrMainAI->ptrStructPlayer;
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
			ROR_STRUCTURES_10C::STRUCT_UNIT *currentVillager = GetUnitStruct(tacAI->allVillagers.unitIdArray[tacAIArrayIndex]);
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
		ROR_STRUCTURES_10C::STRUCT_UNIT *unit = GetUnitStruct(orderedUnitIDs[currentIndex]);
		ROR_STRUCTURES_10C::STRUCT_UNIT_ACTIVITY *activity = NULL;
		bool valid = unit && unit->IsCheckSumValid();
		if (valid) { activity = unit->currentActivity; }
		bool attackTower = true; // Default: attack = most cases (in original code, villager almost always attack the tower)
		if (valid && activity) {
			// This IF corresponds to original code: do NOT re-task villager that are currently attacking something ("whenattacked=2BC" or that...?(2C9))
			// Corresponds to filter (exclude 2C9 and 2BC) in original game code
			// Don't know why first loop does NOT exclude 2C9 (only excludes 2BC)
			if ((activity->internalId_whenAttacked == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_UNKNOWN_2BC_ATTACKING) ||
				(activity->internalId_whenAttacked == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_UNKNOWN_2C9)) {
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
			if (MoveAndAttackTarget(tacAI, unit, enemyTower)) {
				(*pAssignedUnitsCount)++;
			}
		}

		currentIndex++;
	}
}
#undef CRCOMMANDS_MAX_HANDLED_VILLAGER_COUNT


// If fire galley iconId conflicts with catapult trireme, this will move it to a free location (trireme tech because now it's free)
void CustomRORCommand::MoveFireGalleyIconIfNeeded(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	if (!player) { return; }
	// Check fire galley unit exists for this player (it is a valid DAT_ID)
	if (CST_UNITID_FIRE_GALLEY >= player->structDefUnitArraySize) { return; }
	// Check the researches we use exist for this player
	ROR_STRUCTURES_10C::STRUCT_PLAYER_RESEARCH_INFO *researchInfo = player->ptrResearchesStruct;
	assert(researchInfo != NULL);
	if ((CST_RSID_TRIREME >= researchInfo->researchCount) || (CST_RSID_FIRE_GALLEY >= researchInfo->researchCount) ) { return; }
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT **arrUnitDef = player->ptrStructDefUnitTable;
	if (arrUnitDef) {
		ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = arrUnitDef[CST_UNITID_FIRE_GALLEY];
		if (!unitDef || (unitDef->trainButton == 9)) { return; } // Already at the custom location (also return if invalid unit def)
		AOE_CONST_FUNC::RESEARCH_STATUSES triremeStatus = GetResearchStatus(player, CST_RSID_TRIREME);
		AOE_CONST_FUNC::RESEARCH_STATUSES fireGalleyStatus = GetResearchStatus(player, CST_RSID_FIRE_GALLEY);
		// If player has fire galley + trireme in his tech tree, make sure both buttons can be visible at any time (if available)
		if ((fireGalleyStatus != AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) &&
			(triremeStatus != AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED)) {
			// Trireme is researched: research button location is now free to use
			if (triremeStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID) {
				unitDef->trainButton = 9; // Fire galley uses trireme's tech button now it won't appear again.
			} else {
				unitDef->trainButton = 5; // Use standard location (same as trireme) until trireme is available too.
			}
		}
	}
}
// If fire galley iconId conflicts with catapult trireme, this will move it to a free location (trireme tech because now it's free)
// If playerId is invalid, does nothing (no error).
void CustomRORCommand::MoveFireGalleyIconIfNeeded(short int playerId) {
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
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
short int CustomRORCommand::DuplicateUnitDefinitionForPlayer(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, short int srcDAT_ID, char *name) {
	if (!player) { return -1; }

	short int newDAT_ID = player->structDefUnitArraySize;
	if (newDAT_ID >= 0x7FFF) {
		traceMessageHandler.WriteMessage("Error: tried to add too many unit definitions.");
		return -1;
	}
	short int nbDef = player->structDefUnitArraySize + 1; // new number of unit definitions
	long int newSizeInBytes = nbDef * 4;
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT **oldArray = player->ptrStructDefUnitTable;
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
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT **newArray = (ROR_STRUCTURES_10C::STRUCT_DEF_UNIT **)AOEAlloc(newSizeInBytes);
	if (newArray == NULL) { return -1; } // nothing allocated: return an error
		
	// Copy old array into new (for all existing unitDefs => copy pointers)
	memcpy(newArray, oldArray, newSizeInBytes);
	AOEFree(oldArray); // old array is freed, we replace it by new (larger) one
	player->ptrStructDefUnitTable = newArray;
	player->structDefUnitArraySize = nbDef;
	player->ptrStructDefUnitTable[newDAT_ID] = NULL; // until we set a valid pointer.

	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *srcDef = player->ptrStructDefUnitTable[srcDAT_ID];
	assert(srcDef != NULL);
	if (srcDef == NULL) { return -1; }
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *srcDef_base = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *)srcDef;

	long int objectSize = 0;
	bool hasAttacksAndArmors = srcDef_base->DerivesFromType50();
	bool hasCommandsHeader = srcDef_base->DerivesFromBird();
	switch (srcDef->unitType) {
	case GUT_EYE_CANDY:
		objectSize = sizeof(ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE);
		break;
	case GUT_FLAGS:
		objectSize = sizeof(ROR_STRUCTURES_10C::STRUCT_UNITDEF_FLAG);
		break;
	case GUT_DOPPLEGANGER:
		objectSize = sizeof(ROR_STRUCTURES_10C::STRUCT_UNITDEF_DOPPLEGANGER);
		break;
	case GUT_DEAD_UNITS:
		objectSize = sizeof(ROR_STRUCTURES_10C::STRUCT_UNITDEF_DEAD_FISH);
		break;
	case GUT_BIRD:
		objectSize = sizeof(ROR_STRUCTURES_10C::STRUCT_UNITDEF_BIRD);
		break;
	case GUT_TYPE50:
		objectSize = sizeof(ROR_STRUCTURES_10C::STRUCT_UNITDEF_TYPE50);
		break;
	case GUT_PROJECTILE:
		objectSize = sizeof(ROR_STRUCTURES_10C::STRUCT_UNITDEF_PROJECTILE);
		break;
	case GUT_LIVING_UNIT:
		objectSize = sizeof(ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING);
		break;
	case GUT_BUILDING:
		objectSize = sizeof(ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING);
		break;
	case GUT_TREE:
		objectSize = sizeof(ROR_STRUCTURES_10C::STRUCT_UNITDEF_TREE);
		break;
	default:
		// Not supported
		assert(0 && "Trying to create a non-supported unit type.");
		traceMessageHandler.WriteMessage("ERROR: Tried to create a non-supported unit type.");
		return -1;
	}
	if (objectSize <= 0) { return -1; }

	// Create new defUnit in the free room we just created
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *newUnitDef = (ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *) AOEAlloc(objectSize);
	if (!newUnitDef) { return -1; } // nothing allocated: return an error

	player->ptrStructDefUnitTable[newDAT_ID] = newUnitDef;

	memcpy(newUnitDef, srcDef, objectSize);

	// Very important: update new unitDef's DATIDs or some features will not work
	newUnitDef->DAT_ID1 = newDAT_ID;
	newUnitDef->DAT_ID2 = newDAT_ID;

	// Manage damage graphics array (allocated for each unitDef)
	// It belongs to base class => do this for all unit types, even if it seems this array is only used for buildings
	if (newUnitDef->damageGraphicCount > 0) {
		newUnitDef->damageGraphicsArray = (unsigned long int *)AOEAlloc(sizeof(unsigned long int) * newUnitDef->damageGraphicCount);
		memcpy(newUnitDef->damageGraphicsArray, srcDef->damageGraphicsArray, sizeof(ROR_STRUCTURES_10C::STRUCT_ARMOR_OR_ATTACK) * newUnitDef->damageGraphicCount);
	} else {
		assert(newUnitDef->damageGraphicsArray == NULL);
		newUnitDef->damageGraphicsArray = NULL; // Should already be NULL in this case
		newUnitDef->damageGraphicCount = 0;// Should not be <0
	}

	if (hasAttacksAndArmors) {
		// Attacks and armors are allocated for each unitDef. We need to duplicate them (or ROR would free them twice)
		newUnitDef->ptrArmorsList = NULL; // Default value, will be overwritten if necessary
		newUnitDef->ptrAttacksList = NULL; // Default value, will be overwritten if necessary
		if (newUnitDef->armorsCount > 0) {
			newUnitDef->ptrArmorsList = (ROR_STRUCTURES_10C::STRUCT_ARMOR_OR_ATTACK *)AOEAlloc(sizeof(ROR_STRUCTURES_10C::STRUCT_ARMOR_OR_ATTACK) * newUnitDef->armorsCount);
			memcpy(newUnitDef->ptrArmorsList, srcDef->ptrArmorsList, sizeof(ROR_STRUCTURES_10C::STRUCT_ARMOR_OR_ATTACK) * newUnitDef->armorsCount);
		}
		if (newUnitDef->attacksCount > 0) {
			newUnitDef->ptrAttacksList = (ROR_STRUCTURES_10C::STRUCT_ARMOR_OR_ATTACK *)AOEAlloc(sizeof(ROR_STRUCTURES_10C::STRUCT_ARMOR_OR_ATTACK) * newUnitDef->attacksCount);
			memcpy(newUnitDef->ptrAttacksList, srcDef->ptrAttacksList, sizeof(ROR_STRUCTURES_10C::STRUCT_ARMOR_OR_ATTACK) * newUnitDef->attacksCount);
		}
	}

	// Graphics structures are COMMON (not freed at unitDef level): we can keep them, they won't be freed twice

	if (hasCommandsHeader) {
		// Commands: we need to duplicate them (or ROR would free them twice)
		assert(srcDef->ptrUnitCommandHeader != NULL);
		if (srcDef->ptrUnitCommandHeader != NULL) {
			ROR_STRUCTURES_10C::STRUCT_DEF_UNIT_COMMAND_HEADER *newCmdHeader = (ROR_STRUCTURES_10C::STRUCT_DEF_UNIT_COMMAND_HEADER *) AOEAlloc(sizeof(ROR_STRUCTURES_10C::STRUCT_DEF_UNIT_COMMAND_HEADER));
			newUnitDef->ptrUnitCommandHeader = newCmdHeader;
			newCmdHeader->checksum = srcDef->ptrUnitCommandHeader->checksum;
			newCmdHeader->commandCount = srcDef->ptrUnitCommandHeader->commandCount;
			if (newCmdHeader->commandCount > 0) {
				newCmdHeader->ptrCommandArray = (ROR_STRUCTURES_10C::STRUCT_DEF_UNIT_COMMAND**) AOEAlloc(newCmdHeader->commandCount * 4);
			} else {
				newCmdHeader->ptrCommandArray = NULL;
			}

			for (long int i = 0; i < newCmdHeader->commandCount; i++) {
				newCmdHeader->ptrCommandArray[i] = (ROR_STRUCTURES_10C::STRUCT_DEF_UNIT_COMMAND*) AOEAlloc(sizeof(ROR_STRUCTURES_10C::STRUCT_DEF_UNIT_COMMAND)); // Alloc
				memcpy(newCmdHeader->ptrCommandArray[i], srcDef->ptrUnitCommandHeader->ptrCommandArray[i], sizeof(ROR_STRUCTURES_10C::STRUCT_DEF_UNIT_COMMAND)); // Copy from source
			}
		} else {
			newUnitDef->ptrUnitCommandHeader = NULL; // This should NOT happen
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
	if ((this->crInfo->configInfo.dislike_allArtefacts <= 0) || (this->crInfo->configInfo.dislike_humanPlayer <= 0)) { return; }

	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	assert(globalStruct->GetPlayerStructPtrTable() != NULL);
	unsigned long int newDislikeValues[9];
	for (int i = 0; i < 9; i++) { newDislikeValues[i] = CST_DISLIKE_INITIAL_VALUE; }

	// Calculate dislike "penalty" for each player.
	for (int iPlayerId = 1; iPlayerId < globalStruct->playerTotalCount; iPlayerId++) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = globalStruct->GetPlayerStructPtrTable()[iPlayerId];
		if (player && player->GetAIStruct()) {
			float *resources = (float *)player->ptrResourceValues;
			if (resources[CST_RES_ORDER_STANDING_WONDERS]) { newDislikeValues[iPlayerId] += this->crInfo->configInfo.dislike_allArtefacts; }
			if (resources[CST_RES_ORDER_ALL_RUINS]) { newDislikeValues[iPlayerId] += this->crInfo->configInfo.dislike_allArtefacts; }
			if (resources[CST_RES_ORDER_ALL_RELICS]) { newDislikeValues[iPlayerId] += this->crInfo->configInfo.dislike_allArtefacts; }
			if (player->isComputerControlled == 0) { newDislikeValues[iPlayerId] += this->crInfo->configInfo.dislike_humanPlayer; }
		}
	}

	for (int iPlayerId = 1; iPlayerId < globalStruct->playerTotalCount; iPlayerId++) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = globalStruct->GetPlayerStructPtrTable()[iPlayerId];
		if (player) {
			for (int iTargetPlayerId = 1; iTargetPlayerId < globalStruct->playerTotalCount; iTargetPlayerId++) {
				ROR_STRUCTURES_10C::STRUCT_PLAYER *targetPlayer = globalStruct->GetPlayerStructPtrTable()[iTargetPlayerId];
				if ((iPlayerId != iTargetPlayerId) && (player->diplomacyVSPlayers[iTargetPlayerId] > 2) && (player->GetAIStruct())) {
					// Set the new calculated dislike value against player #iTargetPlayerId
					player->GetAIStruct()->structDiplAI.dislikeTable[iTargetPlayerId] = newDislikeValues[iTargetPlayerId];
				}
			}
		}
	}
}


// Technical fix for a method about elevation application when generating map. Original method contains many bugs.
void CustomRORCommand::Fixed_MapGen_applyElevation(long int posX, long int posY, long int distance, ROR_STRUCTURES_10C::STRUCT_MAPGEN_ELEVATION_INFO *elevInfo) {
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

	ROR_STRUCTURES_10C::STRUCT_GAME_MAP_TILE_INFO **pTileInfoRows = elevInfo->unknown_02C;
	// Replace loops in 46F5E1. Was this code written by the internship guy ? It's the only method I saw that is so buggy and so badly written
	for (long int x = minPosX; x <= maxPosX; x++) {
		for (long int y = maxPosY; y >= minPosY; y--) {
			ROR_STRUCTURES_10C::STRUCT_GAME_MAP_TILE_INFO *tile = &pTileInfoRows[x][y];
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
		result += this->crInfo->CollectedTimerIntervals_ms[i];
	}
	result = result / CST_TIMER_STATS_ARRAY_SIZE;
	char buf[70];
	sprintf_s(buf, "Avg timer interval is %ld ms - slow down factor is %ld", result, this->crInfo->configInfo.gameTimerSlowDownFactor);
	CallWriteText(buf);
}


// This method automatically recalculates gameTimerSlowDownFactor
void CustomRORCommand::AutoFixGameTimer() {
	bool reset = false;
	time_t t = time(0); // get current time
	struct tm now;
	localtime_s(&now, &t);
	if (this->crInfo->LastGameTimerAutoFix_second == now.tm_sec) { return; } // Prevents from doing this more than once per second
	long int result = 0;
	for (int i = 0; i < CST_TIMER_STATS_ARRAY_SIZE; i++) {
		if (this->crInfo->CollectedTimerIntervals_ms[i] < 0) { return; } // does not update if we did not collect enough stats (invalid values remain)
		result += this->crInfo->CollectedTimerIntervals_ms[i];
	}
	result = result / CST_TIMER_STATS_ARRAY_SIZE; // Get average
	// Hardcoded limit. Choose a better value. A variable ?
	if ((result < CST_TIMER_INTERVAL_TOO_FAST) && (this->crInfo->configInfo.gameTimerSlowDownFactor < CST_MAXIMUM_SLOW_DOWN_FACTOR)) {
		this->crInfo->configInfo.gameTimerSlowDownFactor++;
		reset = true;
	}
	if ((result > CST_TIMER_INTERVAL_TOO_SLOW) && (this->crInfo->configInfo.gameTimerSlowDownFactor > 1)) {
		this->crInfo->configInfo.gameTimerSlowDownFactor--;
		reset = true;
	}
	this->crInfo->LastGameTimerAutoFix_second = now.tm_sec;

	if (reset) {
		for (int i = 0; i < CST_TIMER_STATS_ARRAY_SIZE; i++) {
			this->crInfo->CollectedTimerIntervals_ms[i] = -1; // Reset statistics
		}
	}
}


// Generate map (overload) for scenario editor
bool CustomRORCommand::ScenarioEditor_customGenerateMap(long int sizeX, long int sizeY) {
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) { return false; }
	ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *)AOE_GetScreenFromName(scenarioEditorScreenName);
	assert(scEditor && scEditor->IsCheckSumValid());
	if (!scEditor || !scEditor->IsCheckSumValid()) { return false; }
	ROR_STRUCTURES_10C::STRUCT_PLAYER *gaia = GetPlayerStruct(0);
	assert(gaia && gaia->IsCheckSumValid());
	if (!gaia || !gaia->IsCheckSumValid()) { return false; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = scEditor->global;
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return false; }
	ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	if (!mapInfo || !mapInfo->IsCheckSumValid()) { return false; }

	assert(sizeX > 0); assert(sizeX <= 0xFF);
	assert(sizeY > 0); assert(sizeY <= 0xFF);
	if ((sizeX <= 0) || (sizeY <= 0) || (sizeX > 0xFF) || (sizeY > 0xFF)) { return false; }

	long int mapType = AOE_GetComboSelectedIndex(scEditor->map_cbb_mapType);
	char *mapSeedText = AOE_GetEditText(scEditor->map_edt_seed);
	long int terrainId = 0;
	long int mapSeed = -1; // Default = -1 (= random)
	long int playerCount = AOE_GetComboSelectedIndex(scEditor->pl_cbb_playerCount) + 1;

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
		switch (AOE_GetComboSelectedIndex(scEditor->map_cbb_defaultTerrain)) {
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
			ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
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
		AOE_SetEditText(scEditor->map_edt_seed_whenReadOnly, bufferSeed);
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
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
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
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		assert(player && player->IsCheckSumValid());
		if (player && player->IsCheckSumValid()) {
			ROR_STRUCTURES_10C::STRUCT_PLAYER_UNKNOWN_58_AND_6C *unknown_struct = &player->unknown_058;
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
	ROR_STRUCTURES_10C::STRUCT_UI_LABEL *bigLbl = scEditor->map_lbl_bigGeneratingMapNotification;
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
void CustomRORCommand::CustomScenarioEditorUICreation(ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor) {
	if (!scEditor || !scEditor->IsCheckSumValid()) { return; }

	// Manage hidden terrains
	ROR_STRUCTURES_10C::STRUCT_UI_LISTBOX *listBox = scEditor->trn_lst_terrainList;
	assert(listBox && listBox->IsCheckSumValid());
	if (listBox && listBox->IsCheckSumValid() && this->crInfo->configInfo.showHiddenTerrainsInEditor) {
		// Note: unavailable terrainIDs are 2,3,5,7,8,9,11,12,14,15,16,17,18,21,23-31
		ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = scEditor->global;
		assert(global && global->IsCheckSumValid());
		if (!global && !global->IsCheckSumValid()) { return; }
		ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		if (!gameMapInfo && !gameMapInfo->IsCheckSumValid()) { return; }
		for (int terrainId = 2; terrainId < gameMapInfo->terrainCount; terrainId++) {
			ROR_STRUCTURES_10C::STRUCT_TERRAIN_DEF *terrainDef = &gameMapInfo->terrainDefinitions[terrainId];
			// Exclude standard terrains (already in list)
			if ((terrainId > 1) && (terrainId != 10) && (terrainId != 13) &&
				(terrainId != 20) && (terrainId != 4) && (terrainId != 19) && (terrainId != 22)) {
				AOE_listbox_addItem(listBox, listBox->itemCount, terrainDef->terrainName, terrainId);
			}
		}
	}

	// Manage custom map dimensions
	if (this->crInfo->configInfo.useCustomMapDimensions &&
		scEditor->map_cbb_mapSize && scEditor->map_cbb_mapSize->IsCheckSumValid() &&
		scEditor->map_cbb_mapSize->underlyingListBox && scEditor->map_cbb_mapSize->underlyingListBox->IsCheckSumValid()) {
		AOE_listbox_addItem(scEditor->map_cbb_mapSize->underlyingListBox, 
			scEditor->map_cbb_mapSize->underlyingListBox->itemCount, "Custom", 0);
	}
}


// This methods modifies provided variables that will be passed to "check unit placement" method before adding a unit in scenario editor.
// For example, to allow creating several units at the same location, force checkConflictingUnits to 0.
void CustomRORCommand::ApplyCustomizationOnEditorAddUnit(long int &checkVisibility, long int &checkHillMode, long int &editorMode, long int &checkAirModeAndHPBar, long int &checkConflictingUnits, bool &IgnoreTerrainRestrictions) {
	checkConflictingUnits = !this->crInfo->configInfo.editor_allowUnitOverlapping;
	checkHillMode = !this->crInfo->configInfo.editor_disableHillModeChecks;
	IgnoreTerrainRestrictions = this->crInfo->configInfo.editor_disableTerrainRestrictions;
}


// Get matching terrainId when user selected a row in terrain list in scenario editor/terrain tab.
// Returns a terrainId, returns -1 if we want to use default behaviour (no custom choice)
long int CustomRORCommand::GetTerrainIdForSelectedTerrainIndex(ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor, long int selectedIndex) {
	if (!scEditor || !scEditor->IsCheckSumValid()) { return -1; }
	// In default game, the terrain list contains 9 terrains (indexes 0-8).
	// Standard available terrainIDs are 0=grass,10=forest,1=water,13=desert_palm,20=jungle,4=shallows,19=pine_forest,22=water_dark
	if (selectedIndex <= 8) { return -1; } // Standard terrains: let normal code be executed.
	ROR_STRUCTURES_10C::STRUCT_UI_LISTBOX *terrains = scEditor->trn_lst_terrainList;
	assert(terrains && terrains->IsCheckSumValid());
	if (!terrains || !terrains->IsCheckSumValid()) { return -1; }
	if (selectedIndex >= terrains->itemCount) { return -1; } // Invalid entry. Should never occur.
	// Here we have selected a custom entry
	
	// unavailable terrainIDs are 2,3,5,7,8,9,11,12,14,15,16,17,18,21,23-31
	// TODO : get terrainId from listbox item "optionalId"
	// We can retrieve terrainId from listbox item "optionalId"
	ROR_STRUCTURES_10C::STRUCT_UI_LIST_ITEM *currentItem = terrains->firstItem;
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
	if (this->crInfo->configInfo.collectRORDebugLogs == 1) {
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
		if ((pos < length) && (msgInitial[pos + 1]) == 'f') {
			floatArgsCount++; // %f means 2 DWORDS in stack instead of 1.
		}
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

	std::string msgFinal = std::string(buf);
	// Do not trigger notifications for ROR executable logs.
	traceMessageHandler.WriteMessageNoNotification(msgFinal);
}


// Fix the check on path finding when trying to assign a task to a villager
long int CustomRORCommand::GathererCheckPathFinding(ROR_STRUCTURES_10C::STRUCT_UNIT_TYPE50 *actorAsType50, long int *pathFindingArgs) {
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = actorAsType50->ptrStructPlayer;
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
void CustomRORCommand::WriteF11PopInfoText(ROR_STRUCTURES_10C::STRUCT_UI_F11_POP_PANEL *f11panel, char *bufferToWrite, char *defaultFormat,
	char *localizedText, long int currentPop, long int houseMaxPop) {
	const int bufferSize = 200; // do not know for sure.
	if (!this->crInfo->configInfo.showCustomPopInfo) {
		// Default behaviour
		sprintf_s(bufferToWrite, 200, defaultFormat, localizedText, currentPop, houseMaxPop);
		return;
	}
	// Custom treatments: build a custom text
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
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
	ROR_STRUCTURES_10C::STRUCT_UNIT_BUILDING *farm = (ROR_STRUCTURES_10C::STRUCT_UNIT_BUILDING *)GetUnitStruct(farmUnitId);
	if (!farm || !farm->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = farm->ptrStructPlayer;
	if (!player || !player->IsCheckSumValid()) { return; }
	if (player != GetControlledPlayerStruct_Settings()) { return; } // Only for human-controlled player
	if (!player->ptrCreatableUnitsListLink || !player->ptrCreatableUnitsListLink->IsCheckSumValid()) { return; }

	// Is feature enabled ?
	if (!this->crInfo->configInfo.enableAutoRebuildFarms) { return; }

	// Check auto-rebuild farms conditions (parameters)
	if (player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_FOOD) > this->crInfo->configInfo.autoRebuildFarms_maxFood) { return; }
	if (player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_WOOD) < this->crInfo->configInfo.autoRebuildFarms_minWood) { return; }
	// Remark : currentFarmCount includes current farm (that is going to be deleted)
	long int currentFarmCount = GetPlayerUnitCount(player, CST_UNITID_FARM, GLOBAL_UNIT_AI_TYPES::TribeAINone, 0, 2); // Include being-built farms
	if (currentFarmCount > this->crInfo->configInfo.autoRebuildFarms_maxFarms) { return; }

	ROR_STRUCTURES_10C::STRUCT_UNIT_LIVING *farmerUnit = NULL;
	// Search for the farmer that was working on this farm (first -arbitrary- one if there are many)
	ROR_STRUCTURES_10C::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *curElem = player->ptrCreatableUnitsListLink->lastListElement;
	while ((curElem != NULL) && (farmerUnit == NULL)) {
		if (curElem->unit && curElem->unit->IsCheckSumValid() && curElem->unit->ptrStructDefUnit &&
			curElem->unit->ptrStructDefUnit->IsCheckSumValid() && (curElem->unit->ptrStructDefUnit->DAT_ID1 == CST_UNITID_FARMER)) {
			ROR_STRUCTURES_10C::STRUCT_ACTION_BASE *curUnitAction = GetUnitAction(curElem->unit);
			// There is 1 special case when farmer's resourceType is NOT berryBush: when AI player repairs a farm (bug: villager type is farmer instead of repairman)
			if (curUnitAction && (curUnitAction->actionTypeID == AOE_CONST_INTERNAL::INTERNAL_ACTION_ID::CST_IAI_GATHER_05)) {
				if (curUnitAction->targetUnitId == farmUnitId) {
					assert(curElem->unit->resourceTypeId == AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_BERRY_STORAGE);
					farmerUnit = (ROR_STRUCTURES_10C::STRUCT_UNIT_BUILDING *)curElem->unit;
				}
			}
		}
		curElem = curElem->previousElement;
	}
	if (farmerUnit) {
		// Add info in an internal list to trigger construction of a new farm when "this" one is actually removed.
		FarmRebuildInfo *f = this->crInfo->myGameObjects.FindOrAddFarmRebuildInfo(farmUnitId);
		f->villagerUnitId = farmerUnit->unitInstanceId;
		f->playerId = player->playerId;
		f->posX = farm->positionX;
		f->posY = farm->positionY;
		f->gameTime = (player->ptrGlobalStruct != NULL) ? player->ptrGlobalStruct->currentGameTime : 0;
	}
}


void CustomRORCommand::DisableWaterUnitsIfNeeded() {
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings != NULL);
	assert(settings->IsCheckSumValid());
	if (!settings) { return; } // Should never happen
	if (settings->isCampaign || settings->isScenario || settings->isSavedGame || settings->isMultiplayer) { return; }

	if (!IsDockRelevantForMap(settings->mapTypeChoice)) {
		ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
		if (!global || !global->IsCheckSumValid() || !global->ptrPlayerStructPtrTable) { return; }
		for (int i = 1; i < global->playerTotalCount; i++) {
			ROR_STRUCTURES_10C::STRUCT_PLAYER *player = global->ptrPlayerStructPtrTable[i];
			if (player && player->IsCheckSumValid() && (player->structDefUnitArraySize > CST_UNITID_DOCK) &&
				player->ptrStructDefUnitTable && player->ptrStructDefUnitTable[CST_UNITID_DOCK]) {
				traceMessageHandler.WriteMessageNoNotification(std::string("Disabled dock for player #") + std::to_string(i));
				player->ptrStructDefUnitTable[CST_UNITID_DOCK]->availableForPlayer = 0;
			}
		}
	}
}


// Called on each loop in infAI.FindEnemyUnitIdWithinRange(ptrMyReferenceUnit, maxDistance, DATID, DATID, DATID, DATID)
// This is called quite often (only if improve AI is enabled in customrOR configuration)
void CustomRORCommand::OnFindEnemyUnitIdWithinRangeLoop(ROR_STRUCTURES_10C::STRUCT_INF_AI *infAI, ROR_STRUCTURES_10C::STRUCT_INF_AI_UNIT_LIST_ELEM *currentUnitListElem) {
	if (!infAI || !infAI->IsCheckSumValid() || !currentUnitListElem) { return; }
	if (IsMultiplayer()) { return; }

	ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *unitBase = (ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *)GetUnitStruct(currentUnitListElem->unitId);
	// Custom treatment: clean obsolete units
	if (IsFogVisibleForPlayer(infAI->commonAIObject.playerId, currentUnitListElem->posX, currentUnitListElem->posY)) {
		// Clean entry if: (we are in the case when "unit list" position is visible, so we can update it without cheating !)
		// - unit no longer exist
		// - unit moved to a position which is NO LONGER visible to me
		if (!unitBase || (!IsFogVisibleForPlayer(infAI->commonAIObject.playerId, (long int)unitBase->positionX, (long int)unitBase->positionY))) {
			ResetInfAIUnitListElem(currentUnitListElem);
		}
	}
}


// Computes (existing) building influence zone for farm placement map like values computation.
// existingBuilding must be a building (current building from loop, to take into account in map like values)
// Updates existingBldInfluenceZone with the influence distance we want to use for provided building (positions near building will be preferred)
// Updates skipThisBuilding to true if the building must be ignored for farm position computation. If true, existingBldInfluenceZone returned value should be ignored.
// Improvements depend on configuration (useEnhancedFeature and improveAILevel) + game difficulty level
void CustomRORCommand::FixCityPlanFarmPlacement(ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *existingBuilding, long int &existingBldInfluenceZone, bool &skipThisBuilding) {
	skipThisBuilding = false;
	existingBldInfluenceZone = 3;
	if (!existingBuilding || !existingBuilding->IsCheckSumValidForAUnitClass()) { return; }

	// Default behavior / values
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDef_base = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *)existingBuilding->ptrStructDefUnit;
	assert(unitDef_base && unitDef_base->IsCheckSumValidForAUnitClass());
	if (!unitDef_base && !unitDef_base->IsCheckSumValidForAUnitClass()) { return; }
	
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());

	// Set some default values
	existingBldInfluenceZone = (int)unitDef_base->lineOfSight; // Default (from ROR code: use existing bld LOS)
	int difficultyLevel = settings->difficultyLevel;
	bool useEnhancedFeature = this->crInfo->configInfo.cityPlanLikeValuesEnhancement;
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
	if (difficultyLevel >= 3) {
		return; // Easy (3) and easiest (4): use default behavior (exit and use defaults)
	}

	// Medium difficulty : just exclude some buildings (hard to find a compromise between no fix at all and very important fix)
	if (difficultyLevel == 2) {
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
void CustomRORCommand::ManageCityPlanOtherBuildingsImpact(ROR_STRUCTURES_10C::STRUCT_INF_AI *infAI, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *stratElem, ROR_STRUCTURES_10C::STRUCT_POSITION_INFO *TCPosition) {
	if (!infAI || !stratElem || !TCPosition) { return; }
	assert(infAI->IsCheckSumValid());
	assert(stratElem->IsCheckSumValid());
	if (!infAI->IsCheckSumValid() || !stratElem->IsCheckSumValid()) { return; }
	long int TC_x = TCPosition->posX;
	long int TC_y = TCPosition->posY;
	assert(TC_x >= 0);
	assert(TC_y >= 0);

	// Stop here is feature is not enabled in configuration
	if (!this->crInfo->configInfo.cityPlanLikeValuesEnhancement) { return; }

	if (!this->IsImproveAIEnabled(infAI->commonAIObject.playerId)) {
		return;
	}

	// Collect useful context information : player, strategy element to build...
	ROR_STRUCTURES_10C::STRUCT_AI *ai = infAI->ptrMainAI;
	assert(ai && ai->IsCheckSumValid());
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = ai->ptrStructPlayer;
	assert(player != NULL);
	assert(player->IsCheckSumValid());
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *globalStruct = *pGlobalStruct;
	assert(globalStruct != NULL);
	assert(globalStruct->IsCheckSumValid());

	ROR_STRUCTURES_10C::STRUCT_TEMP_MAP_BUILD_LIKE_INFOS *mapInfosStruct = pTempMapBuildLikeInfos;
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
		ROR_STRUCTURES_10C::STRUCT_UNIT *unit = (ROR_STRUCTURES_10C::STRUCT_UNIT *) globalStruct->ptrUnitPointersList[unitId];
		if (unit) {
			ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *defUnit = unit->ptrStructDefUnit;
			if (defUnit) {
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
		ROR_STRUCTURES_10C::STRUCT_UNIT *unit = (ROR_STRUCTURES_10C::STRUCT_UNIT *) globalStruct->ptrUnitPointersList[unitId];
		if (unit) {
			ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *defUnit = unit->ptrStructDefUnit;
			if (defUnit) {
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
				mapInfosStruct->ProtectBuildingOppositeSides(unitX, unitY, defUnit->sizeRadius1, protectOppositeSidesValue);

				// Storage buildings: protect (more) their neighborhood, they need to be easily accessible by gatherers (or new villagers)
				// Moreover, TC and granary need space to build farms nearby

				// substract this value to X/Y positions that are > to center position to fix rounding.
				int unitRadiusDecreaseForHighPositions = 1 - (((int)(defUnit->sizeRadius1 * 2)) % 2); // 0 for values like 1.5 or 2.5, 1 for round values
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
bool CustomRORCommand::OpenCustomDialogMessage(char *dialogText, long int hSize, long int vSize) {
	if (this->crInfo->customYesNoDialogVar) { return false; } // Already an opened custom dialog

	ROR_STRUCTURES_10C::STRUCT_ANY_UI *customDialogPtr = AOE_GetScreenFromName(AOE_CONST_INTERNAL::customDialogScreenName);
	if (customDialogPtr != NULL) { return false; } // A CloseProgramDialog seems to be already open

	SetGamePause(true);
	this->crInfo->customYesNoDialogVar = (unsigned long int*) AOE_CreateDialogPopup(dialogText, hSize, vSize);

	return true;
}


// Closes currently opened custom dialog message.
// Returns -1 if an error occurred, including "no custom dialog is opened".
// Other results are AOE_CONST_INTERNAL::DIALOG_BUTTON_IDS => Yes/No/Cancel
long int CustomRORCommand::CloseCustomDialogMessage(ROR_STRUCTURES_10C::STRUCT_UI_POPUP_DIALOG *ptrDialog, unsigned long int ptrSender) {
	if (this->crInfo->customYesNoDialogVar == NULL) { return -1; } // No opened custom dialog
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
	this->crInfo->customYesNoDialogVar = NULL;

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
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		assert(player && player->IsCheckSumValid());
		if (research_id == AOE_CONST_FUNC::CST_RSID_CATAPULT_TRIREME) {
			this->MoveFireGalleyIconIfNeeded(player);
		}
	}

	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPES::CST_GET_BUILDING_COMPLETE) {
		short int bldDAT_ID = (short int)arg3;
		if (bldDAT_ID = CST_UNITID_MARKET) {
			// Building a market (first one) enables farms.
			ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
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
void CustomRORCommand::OnGameInitDisableResearchesEvent(ROR_STRUCTURES_10C::STRUCT_PLAYER_RESEARCH_INFO *playerResearchInfo) {
	assert(playerResearchInfo != NULL);
	if (!playerResearchInfo) { return; }
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = playerResearchInfo->ptrPlayer;
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = player->GetGlobalStruct();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }

	// For current player, disable the units we have from trigger data.
	std::vector<short int>::const_iterator it;
	for (it = this->crInfo->unitDefToDisable[player->playerId].begin(); it != this->crInfo->unitDefToDisable[player->playerId].end(); it++) {
		short int DAT_ID = *it;
		if (DisableUnitForPlayer(player, DAT_ID)) {
			// success
		} else {
			// Exceptions: villager, house, farm, fishing ship, trade ship. DisableUnitForPlayer doesn't support them because of techs that do several things at once.
			// Those exceptions are manage in another location
		}
	}

	// Disable researches from trigger data
	for (it = this->crInfo->researchesToDisable[player->playerId].begin(); it != this->crInfo->researchesToDisable[player->playerId].end(); it++) {
		short int researchId = *it;
		AOE_enableResearch(player, researchId, false);
	}
}


// Manage disable (via trigger) units for villager, house, fishing ship, trade ship
// This method should be called at game start, after "initial" technologies have been applied, so we can override some (and force to disable again some units)
void CustomRORCommand::ManageTriggerDisableUnitsForExceptions() {
	for (int playerId = 0; playerId < 9; playerId++) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		if (player) {
			std::vector<short int>::const_iterator it;
			for (it = this->crInfo->unitDefToDisable[player->playerId].begin(); it != this->crInfo->unitDefToDisable[player->playerId].end(); it++) {
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
					ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDef = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *)player->ptrStructDefUnitTable[DAT_ID];
					unitDef->availableForPlayer = 0;
				}
			}
		}
	}
}

// Manage disable (via trigger) units for farms.
// Farm is enabled by market construction, which may occur both "ingame" or at game loading (if a market exists, or if starting at bronze).
void CustomRORCommand::ManageDisableUnitsForFarms(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	std::vector<short int>::const_iterator it;
	for (it = this->crInfo->unitDefToDisable[player->playerId].begin(); it != this->crInfo->unitDefToDisable[player->playerId].end(); it++) {
		short int DAT_ID = *it;
		if (DAT_ID == CST_UNITID_FARM) {
			if ((DAT_ID < 0) || (DAT_ID >= player->structDefUnitArraySize)) { return; }
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDef = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *)player->ptrStructDefUnitTable[DAT_ID];
			unitDef->availableForPlayer = 0;
			return;
		}
	}
}

void CustomRORCommand::ReloadTriggersFromGameData(ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *scenarioInfo) {
	if (this->crInfo->triggerSet) {
		delete this->crInfo->triggerSet;
		this->crInfo->triggerSet = NULL;
	}
	this->crInfo->triggerSet = new CR_TRIGGERS::crTriggerSet();
	if (!scenarioInfo || (!scenarioInfo->IsCheckSumValid())) { return; }

	this->crInfo->triggerSet->ReadFromFileContent(GetTriggerDataPtr(scenarioInfo));
}

void CustomRORCommand::ReloadTriggersFromGameData() {
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	if (!global->scenarioInformation) { return; }
	this->ReloadTriggersFromGameData(global->scenarioInformation);
}


// Write trigger to game data, using customROR internal data.
bool CustomRORCommand::WriteTriggersFromInternalToGameData(bool onlyEnabledTriggers) {
	if (!this->crInfo->triggerSet) { return false; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->scenarioInformation) { return false; }
	ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *sc_info = global->scenarioInformation; // Guaranteed non-NULL by previous "if"
	assert(sc_info != NULL);
	if (!sc_info) { return false; }
	std::string tmpStr = this->crInfo->triggerSet->GetStringForScenarioInfo(onlyEnabledTriggers);
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
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->scenarioInformation) { return false; }
	ROR_STRUCTURES_10C::STRUCT_SCENARIO_INFO *sc_info = global->scenarioInformation; // Guaranteed non-NULL by previous "if"
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
	if (!this->crInfo->triggerSet) { return ""; }
	return this->crInfo->triggerSet->GetStringForScenarioInfo(false);
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
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) || settings->isMultiplayer) {
		return;
	}

	// Make sure difficulty level is always set
	if (evtInfo.difficultyLevel < 0) {
		ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		assert(settings);
		if (!settings) { return; }
		evtInfo.difficultyLevel = settings->difficultyLevel;
	}
	// Make sure current game time is always set
	if (evtInfo.currentGameTime_s < 0) {
		ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		assert(global);
		if (!global) { return; }
		evtInfo.currentGameTime_s = global->currentGameTime / 1000;
	}
	// Loop on triggers and execute those that are ready
	if (this->crInfo->triggerSet && (!this->crInfo->triggerSet->IsEmpty())) {
		CR_TRIGGERS::crTrigger *currentTrigger = this->crInfo->triggerSet->IteratorFirst();
		while (currentTrigger) {
			if (currentTrigger->CanExecuteNow(evt, evtInfo)) {
				this->ExecuteTriggerAction(currentTrigger);
			}
			currentTrigger = this->crInfo->triggerSet->IteratorNext();
		}
	}
}


// Just execute a "standard" trigger. this only does the action part, does no check and does not manage other impacts.
// Do not call this directly ! (private)
void CustomRORCommand::Trigger_JustDoAction(CR_TRIGGERS::crTrigger *trigger) {
	if (!trigger) { return; }
	if (!trigger->IsValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
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
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
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
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
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
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (player) {
			ROR_STRUCTURES_10C::STRUCT_PLAYER_RESEARCH_INFO *pr = player->ptrResearchesStruct;
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
			ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(i);
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
		if (this->crInfo->triggerSet) {
			char *targetTriggerName = trigger->GetParameterValue(CR_TRIGGERS::KW_TARGET_TRIGGER_NAME, "");
			CR_TRIGGERS::crTrigger *otherTrigger = this->crInfo->triggerSet->GetTrigger(targetTriggerName);
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
		ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		assert(gameMapInfo && gameMapInfo->IsCheckSumValid());
		if (!gameMapInfo || !gameMapInfo->IsCheckSumValid()) { return; }
		if ((minPosX >= gameMapInfo->mapArraySizeX) || (minPosY >= gameMapInfo->mapArraySizeY) ||
			(maxPosX >= gameMapInfo->mapArraySizeX) || (maxPosX >= gameMapInfo->mapArraySizeX)
			) { return; }
		for (int x = minPosX; x <= maxPosX; x++) {
			for (int y = minPosY; y <= maxPosY; y++) {
				ROR_STRUCTURES_10C::STRUCT_GAME_MAP_TILE_INFO *tile = gameMapInfo->GetTileInfo(x, y);
				if (tile) {
					// Note: tile's "Set" methods are both secure (check the value is in bounds)
					if (elevationLevel >= 0) { tile->SetAltitude((char)elevationLevel); } // TO DO: this does not work well in-game + side effects
					if (terrainId >= 0) { tile->SetTerrainId((char)terrainId); } // Does not update minimap, does not refresh map immediatly
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

		ROR_STRUCTURES_10C::STRUCT_UNIT *targetUnit = GetUnitStruct(actionUnitId);
		if (!targetUnit || !targetUnit->IsCheckSumValid()) { return; }
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
		if ((graphicsAge >= 0) && (graphicsAge <= 2)) { // TO DO (does not work)
			targetUnit->currentGraphicsAge = graphicsAge;
		}
		if ((orientation >= 0) && (orientation <= 7)) { // Not sure about valid values
			targetUnit->orientation = orientation;
		}
		if ((posX >= 0) && (posY >= 0)) { // This has important side effect, not recommended !!! TO DO.
			ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
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
		ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		assert(gameMapInfo && gameMapInfo->IsCheckSumValid());
		if ((posX >= gameMapInfo->mapArraySizeX) || (posY >= gameMapInfo->mapArraySizeY)) { return; }
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (!player || !player->IsCheckSumValid()) { return; }
		if (actionUnitDefId >= player->structDefUnitArraySize) { return; }
		ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = player->ptrStructDefUnitTable[actionUnitDefId];
		if (!unitDef || !unitDef->IsCheckSumValid()) { return; }
		/*ROR_STRUCTURES_10C::STRUCT_UNIT *unit = */CheckAndCreateUnit(player, unitDef, posX, posY, false, true, true);
	}

	// A big one: create unit def (actually, duplicate + customize)
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ADD_UNIT_DEF) {
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		long int sourceUnitDefId = trigger->GetParameterValue(CR_TRIGGERS::KW_FROM_UNIT_DEF_ID, -2);
		char *newUnitName = trigger->GetParameterValue(CR_TRIGGERS::KW_UNIT_NAME, "");
		if ((sourceUnitDefId < 0) || (actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount)) {
			return;
		}
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (!player || !player->IsCheckSumValid()) { return; }
		if (sourceUnitDefId >= player->structDefUnitArraySize) { return; }
		ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *sourceUnitDef = player->ptrStructDefUnitTable[sourceUnitDefId];
		if (!sourceUnitDef || !sourceUnitDef->IsCheckSumValid()) { return; }

		// OK let's go, create new unitDef:
		short int newUnitDefId = this->DuplicateUnitDefinitionForPlayer(player, (short int)sourceUnitDefId, newUnitName);
		if (newUnitDefId < 0) { return; } // Failed

		ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *newUnitDef = player->ptrStructDefUnitTable[newUnitDefId];
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *newUnitDef_base = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE*) player->ptrStructDefUnitTable[newUnitDefId];
		assert(newUnitDef_base && newUnitDef_base->IsCheckSumValidForAUnitClass());
		if (!newUnitDef_base || !newUnitDef_base->IsCheckSumValidForAUnitClass()) { return; }
		// We manipulate here objects from different possible classes. Make sure we work on members that DO exist in our object instance !
		bool isType50OrChild = newUnitDef_base->DerivesFromType50();
		bool isFlagOrChild = newUnitDef_base->DerivesFromFlag();
		bool isDeadFishOrChild = newUnitDef_base->DerivesFromDead_fish();
		bool isBirdOrChild = newUnitDef_base->DerivesFromBird();
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_FLAG *newUnitDef_flag = NULL;
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_DEAD_FISH *newUnitDef_dead_fish = NULL;
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_BIRD *newUnitDef_bird = NULL;
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_TYPE50 *newUnitDef_type50 = NULL;
		if (isFlagOrChild) { newUnitDef_flag = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_FLAG *)newUnitDef_base; }
		if (isDeadFishOrChild) { newUnitDef_dead_fish = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_DEAD_FISH *)newUnitDef_base; }
		if (isBirdOrChild) { newUnitDef_bird = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BIRD *)newUnitDef_base; }
		if (isType50OrChild) { newUnitDef_type50 = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_TYPE50 *)newUnitDef_base; }

		// Apply supplied properties to new unit def.
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_TOTAL_HP)) {
			newUnitDef_base->totalHitPoints = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_TOTAL_HP, 1);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_RANGE)) {
			float suppliedRange = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_RANGE, -1);
			if (suppliedRange >= 0) {
				newUnitDef_base->lineOfSight = suppliedRange;
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
		if (isDeadFishOrChild && newUnitDef_dead_fish && (trigger->IsParameterDefined(CR_TRIGGERS::KW_ROTATION_SPEED))) {
			newUnitDef_dead_fish->rotationSpeed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_ROTATION_SPEED, newUnitDef_dead_fish->rotationSpeed);
		}
		if (isBirdOrChild && newUnitDef_bird && (trigger->IsParameterDefined(CR_TRIGGERS::KW_WORK_RATE))) {
			newUnitDef_bird->workRate = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_WORK_RATE, newUnitDef_bird->workRate);
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
			newUnitDef_base->visibleInFog = (char)trigger->GetParameterValue(CR_TRIGGERS::KW_VISIBLE_IN_FOG, (long int)newUnitDef_base->visibleInFog);
			if (newUnitDef_base->visibleInFog != 0) {
				newUnitDef_base->visibleInFog = 1; // just in case.
			}
		}

		// Update other triggers with new unitDefId
		CR_TRIGGERS::crTrigger *currentTrg = this->crInfo->triggerSet->IteratorFirst();
		while (currentTrg) {
			// Search for enabled triggers for same player, that want to spawn a unit...
			if (currentTrg->enabled && (currentTrg->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ADD_UNIT_INSTANCE) &&
				currentTrg->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -2) == actionPlayerId) {
				// Find if the spawn unit action uses "unit_name" as reference
				char *otherTrgUnitName = currentTrg->GetParameterValue(CR_TRIGGERS::KW_UNIT_NAME, "");
				if (strcmp(newUnitName, otherTrgUnitName) == 0) {
					// Found a trigger which is based on the unit name we just created: set the unitDefId value.
					currentTrg->SetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_DEF_ID, newUnitDefId);
				}
			}
			currentTrg = this->crInfo->triggerSet->IteratorNext();
		}

		trigger->enabled = false; // Creating unit def is not repeatable. Force disable.
	}

	// Give a command to a unit
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_UNIT_TARGET) {
		long int actionUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_ID, -1);
		long int targetUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_TARGET_UNIT_ID, -1);
		if ((actionUnitId < 0) || (targetUnitId < 0)) { return; }
		TellUnitToInteractWithTarget(GetUnitStruct(actionUnitId), GetUnitStruct(targetUnitId));
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
		ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		assert(gameMapInfo && gameMapInfo->IsCheckSumValid());
		if ((posX >= gameMapInfo->mapArraySizeX) || (posY >= gameMapInfo->mapArraySizeY)) { return; }
		MoveUnitToTargetOrPosition(GetUnitStruct(actionUnitId), NULL, posX, posY);
	}

	// Make gaia units human-capturable (or not)
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_GAIA_CAPTURABLE) {
		// Not MP-compatible (to confirm)
		int valueToSet = trigger->GetParameterValue(CR_TRIGGERS::KW_ENABLE, -1);
		if ((valueToSet < 0) || (valueToSet > 1)) { // only 0 and 1 are accepted
			return;
		}
		ROR_STRUCTURES_10C::STRUCT_PLAYER *gaia = GetPlayerStruct(0);
		assert(gaia && gaia->IsCheckSumValid());
		if (!gaia || !gaia->IsCheckSumValid()) {
			return;
		}
		ROR_STRUCTURES_10C::STRUCT_PER_TYPE_UNIT_LIST_LINK *creatablesLink = gaia->ptrCreatableUnitsListLink;
		if (!creatablesLink || !creatablesLink->IsCheckSumValid()) {
			return;
		}
		if (creatablesLink->listElemCount <= 0) {
			return;
		}
		ROR_STRUCTURES_10C::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *currentElem = creatablesLink->lastListElement;
		int counter = creatablesLink->listElemCount;
		while ((currentElem != NULL) && (counter >= 1)) {
			ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *unitBase = (ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *)currentElem->unit;
			if (unitBase && unitBase->IsCheckSumValidForAUnitClass()) {
				ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDefBase = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *)unitBase->ptrStructDefUnit;
				if (unitDefBase && unitDefBase &&
					((unitDefBase->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING) || (unitDefBase->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_LIVING_UNIT)) &&
					(unitDefBase->unitAIType != AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact)
					) {
					// Unit is a building or a living unit, so it is capturable
					// Very important : exclude artefacts to avoid human players from capturing artefacts more easily than AI players

					// We can convert into type50 because we checked unit type is 70 or 80
					ROR_STRUCTURES_10C::STRUCT_UNIT_TYPE50 *unit50 = (ROR_STRUCTURES_10C::STRUCT_UNIT_TYPE50 *)unitBase;
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
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
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
	if (!trigger->triggerNameToRunBefore.empty() && this->crInfo->triggerSet) {
		CR_TRIGGERS::crTrigger *trBefore = this->crInfo->triggerSet->GetTrigger(trigger->triggerNameToRunBefore);
		// Do not allow calling repeatable triggers: those triggers may remain enabled (because they are repeatable), which may lead to infinite dependencies.
		if (trBefore && (trBefore->enabled) && (!trBefore->IsRepeatable())) {
			this->ExecuteTriggerAction(trBefore);
		}
	}

	// Use a dedicated routine to execute action.
	this->Trigger_JustDoAction(trigger);

	// Common for all triggers types: manage trigger to run "after".
	if (!trigger->triggerNameToRunAfter.empty() && this->crInfo->triggerSet) {
		CR_TRIGGERS::crTrigger *trAfter = this->crInfo->triggerSet->GetTrigger(trigger->triggerNameToRunAfter);
		// Do not allow calling repeatable triggers: those triggers may remain enabled (because they are repeatable), which may lead to infinite dependencies.
		if (trAfter && (trAfter->enabled) && (!trAfter->IsRepeatable())) {
			ExecuteTriggerAction(trAfter);
		}
	}
}

