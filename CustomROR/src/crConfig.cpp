#include "../include/crConfig.h"


// Constructor
UnitSpawnShortcutInfo::UnitSpawnShortcutInfo() {
	this->DAT_ID = -1;
	this->onlyOneUnit = false;
}


// Constructor
CustomRORConfig::CustomRORConfig() {
	// Hardcoded initialization. If values are provided in config XML file, it will overload this.
	this->couldNotReadXMLConfig = false;
	this->couldNotReadCivXMLConfig = false;
	this->showAlertOnMissingFeature = true;
	this->hideWelcomeMessage = false;
	this->showCustomRORMenu = false;
	this->showCustomPopInfo = false;
	this->useImprovedGameSpeeds = false;
	this->showLogsInReverseOrder = true;
	this->showCustomRORNotifications = true; // Recommended
	this->collectRORDebugLogs = 0; // Game default (sort of): do not intercept not-implemented debug log calls.
	this->improvedGameSpeedFactor = 1.25;
	this->conversionResistance_Boats = 2;
	this->conversionResistance_Chariots = 8;
	this->conversionResistance_Macedonian = 4;
	this->conversionResistance_Priests = 2;
	this->conversionResistance_WarElephants = 1.25;
	this->conversionResistance_WarElephants_Persian = 1.25;
	this->improveAILevel = 0; // ROR Default (not active)
	this->tacticalAIUpdateDelay = 30;
	this->panicModeDelay = 20;
	this->maxPanicUnitsCountToAddInStrategy = 7;
	this->minPopulationBeforeBuildOptionalItems = 35;
	this->maxFarmsInDeathMatch = 3;
	this->dislikeComputeInterval = 15;
	this->dislike_allArtefacts = 5;
	this->dislike_humanPlayer = 1;
	this->cityPlanLikeValuesEnhancement = false;
	//this->cityPlanHouseDistanceFromTownCenter = 3; // Game default
	//this->cityPlanHouseDistanceFromStorageBld = 3; // Game default
	//this->cityPlanHouseDistanceFromOtherBld = 3; // Game default
	this->cityPlanBerryBushWeightForGranary = 0x28; // NOT game default (0x80) because it doesn't work well.
	this->civCount = AOE_CONST_FUNC::CIVILIZATIONS::CST_CIVID_STANDARD_MAX;
	for (int i = 0; i < 256; i++) {
		allCivInfo[i] = NULL;
	}
	this->enableCallNearbyIdleMilitaryUnits = true;
	this->distanceToCallNearbyIdleMilitaryUnits = 25;
	this->gameTimerSlowDownFactor = 5;
	this->gameTimerSlowDownAutoFix = false;
	this->enableSpawnUnitsMoveToLocation = false;
	this->enableSpawnUnitsAutoTarget = false;
	this->enableSpawnUnitAutoRepairTC = false;
	this->unitShortcutsPriorityReverseOrder = false;
	for (int i = 0; i < CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS; i++) {
		this->unitShortcutsInformation[i].DAT_ID = -1;
		this->unitShortcutsInformation[i].onlyOneUnit = false;
	}
	this->enableAdditionalNumpadShortcuts = false;
	this->useCustomMapDimensions = false;
	this->showHiddenTerrainsInEditor = false;
	this->showHiddenUnitsInEditor = 0;
	this->editor_allowUnitOverlapping = false;
	this->editor_disableHillModeChecks = false;
	this->editor_disableTerrainRestrictions = false;
	this->fixInvisibleTree = false;
	this->fixLogisticsNoHouseBug = false;
	this->fixHumanPlayer_specificSeeUnit = false;
	this->allyExplorationIsAlwaysShared = false; // Game default
	this->enableAutoRebuildFarms = false;
	this->autoRebuildFarms_maxFarms = 4;
	this->autoRebuildFarms_maxFood = 3000;
	this->autoRebuildFarms_minWood = 200;

	// Map generation
	this->randomMapRelicsCount = 5;
	this->randomMapRuinsCount = 5;
	this->useMapGenerationCustomElevationCalculation = false;
	for (int mapType = 0; mapType < AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_MAP_TYPES_COUNT; mapType++) {
		this->mapGenerationCustomElevationFactor[mapType] = 1; // Default factor: *1 does not change value.
	}
}


// Destructor
CustomRORConfig::~CustomRORConfig() {
	for (int i = 0; i < 256; i++) {
		if (allCivInfo[i]) {
			free(allCivInfo[i]);
			allCivInfo[i] = NULL;
		}
	}
}


// Get a bool value from a XML element and an attribute name.
// If attribute is not found, returns false.
// If attribute value is "0" or "false", returns false.
// All other values = return true.
bool XML_GetBoolElement(TiXmlElement *elem, const char *attributeName) {
	if (!elem) { return false; }
	const char *attr = elem->Attribute(attributeName);
	if (!attr) { return false; }
	return (strcmp(attr, "0") != 0) && (strcmp(attr, "false") != 0);
}


// Get setup from XML file to this object variables.
bool CustomRORConfig::ReadXMLConfigFile(char *fileName) {
	TiXmlDocument doc(fileName);
	if (!doc.LoadFile()) {
		this->couldNotReadXMLConfig = true;
		return false;
	}

	TiXmlHandle hdl(&doc);
	string mainNodeTag = hdl.FirstChildElement().Element()->Value();
	if (mainNodeTag != "customROR") {
		// Not a valid customROR config XML file.
		this->couldNotReadXMLConfig = true;
		return false;
	}
	TiXmlElement *elem = hdl.FirstChildElement().FirstChildElement().Element();
	if (!elem) {
		return false;
	}

	while (elem) {
		int callResult;
		float floatValue;
		int intValue;
		string elemName = elem->Value();
		string categoryName;
		if (elemName == "alertOnMissingFeatures") {
			this->showAlertOnMissingFeature = XML_GetBoolElement(elem, "enable");
		}
		if (elemName == "showCustomRORMenu") {
			this->showCustomRORMenu = XML_GetBoolElement(elem, "enable");
		}
		if (elemName == "showCustomPopInfo") {
			this->showCustomPopInfo = XML_GetBoolElement(elem, "enable");
		}
		if (elemName == "improvedGameSpeeds") {
			categoryName = elem->Attribute("name");
			if (categoryName == "enable") {
				this->useImprovedGameSpeeds = XML_GetBoolElement(elem, "value");
			}
			if (categoryName == "factor") {
				callResult = elem->QueryFloatAttribute("value", &floatValue);
				if (callResult == TIXML_SUCCESS) { improvedGameSpeedFactor = floatValue; }
			}
		}
		if (elemName == "collectRORDebugLogs") {
			callResult = elem->QueryIntAttribute("value", &intValue);
			if (callResult == TIXML_SUCCESS) { this->collectRORDebugLogs = intValue; }
		}
		if (elemName == "showCustomRORNotifications") {
			this->showCustomRORNotifications = XML_GetBoolElement(elem, "enable");
		}
		if (elemName == "gameTimerSlowDownAutoFix") {
			this->gameTimerSlowDownAutoFix = XML_GetBoolElement(elem, "enable");
		}
		if (elemName == "gameTimerSlowDownFactor") {
			callResult = elem->QueryIntAttribute("value", &intValue);
			if (callResult == TIXML_SUCCESS) { this->gameTimerSlowDownFactor = intValue; }
		}
		if (elemName == "fixLogisticsNoHouseBug") {
			this->fixLogisticsNoHouseBug = XML_GetBoolElement(elem, "enable");
		}
		if (elemName == "fixInvisibleTree") {
			this->fixInvisibleTree = XML_GetBoolElement(elem, "enable");
		}
		if (elemName == "fixHumanPlayer_specific_seeUnit") {
			this->fixHumanPlayer_specificSeeUnit = XML_GetBoolElement(elem, "enable");
		}
		if (elemName == "allyExplorationIsAlwaysShared") {
			this->allyExplorationIsAlwaysShared = XML_GetBoolElement(elem, "enable");
		}
		// TO DO ? this->hideWelcomeMessage
		// Conversion resistance values
		if (elemName == "conversionResistance") {
			categoryName = elem->Attribute("name");
			if (categoryName == "warElephants") {
				callResult = elem->QueryFloatAttribute("value", &floatValue);
				if (callResult == TIXML_SUCCESS) { this->conversionResistance_WarElephants = floatValue; }
			}
			if (categoryName == "warElephants_Persian") {
				callResult = elem->QueryFloatAttribute("value", &floatValue);
				if (callResult == TIXML_SUCCESS) { this->conversionResistance_WarElephants_Persian = floatValue; }
			}
			if (categoryName == "Macedonian") {
				callResult = elem->QueryFloatAttribute("value", &floatValue);
				if (callResult == TIXML_SUCCESS) { this->conversionResistance_Macedonian = floatValue; }
			}
			if (categoryName == "priests") {
				callResult = elem->QueryFloatAttribute("value", &floatValue);
				if (callResult == TIXML_SUCCESS) { this->conversionResistance_Priests = floatValue; }
			}
			if (categoryName == "chariots") {
				callResult = elem->QueryFloatAttribute("value", &floatValue);
				if (callResult == TIXML_SUCCESS) { this->conversionResistance_Chariots = floatValue; }
			}
			if (categoryName == "boats") {
				callResult = elem->QueryFloatAttribute("value", &floatValue);
				if (callResult == TIXML_SUCCESS) { this->conversionResistance_Boats = floatValue; }
			}
		}
		// Map options
		if (elemName == "map") {
			categoryName = elem->Attribute("name");
			if (categoryName == "randomMapRelicsCount") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->randomMapRelicsCount = intValue; }
			}
			if (categoryName == "randomMapRuinsCount") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->randomMapRuinsCount = intValue; }
			}
			if (categoryName == "useCustomElevationComputation") {
				this->useMapGenerationCustomElevationCalculation = XML_GetBoolElement(elem, "enable");
			}
			if (categoryName == "elevatedTerrainAmountMultiplyFactor") {
				callResult = elem->QueryIntAttribute("mapTypeId", &intValue);
				if ((callResult == TIXML_SUCCESS) && (intValue >= AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_SMALL_ISLANDS) &&
					(intValue < AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_HIDDEN_MAP_TYPE)) {
					callResult = elem->QueryFloatAttribute("value", &floatValue);
					if (callResult == TIXML_SUCCESS) { this->mapGenerationCustomElevationFactor[intValue] = floatValue; }
				}
			}
		}
		// Scenario editor
		if (elemName == "editor") {
			categoryName = elem->Attribute("name");
			if (categoryName == "showHiddenTerrain") {
				this->showHiddenTerrainsInEditor = XML_GetBoolElement(elem, "enable");
			}
			if (categoryName == "showHiddenUnits") {
				//this->showHiddenUnitsInEditor = XML_GetBoolElement(elem, "enable"); => changed to int value
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->showHiddenUnitsInEditor = intValue; }
			}
			if (categoryName == "UseCustomMapDimensions") {
				this->useCustomMapDimensions = XML_GetBoolElement(elem, "enable");
			}
		}
		// Global AI options
		if (elemName == "improveAI") {
			callResult = elem->QueryIntAttribute("value", &intValue);
			if (callResult == TIXML_SUCCESS) { this->improveAILevel = intValue; }
		}
		// Tactical AI options
		if (elemName == "tacticalAI") {
			categoryName = elem->Attribute("name");
			if (categoryName == "updateDelay") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->tacticalAIUpdateDelay = intValue; }
			}
			if (categoryName == "minPopulationBeforeOptionalItems") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->tacticalAIUpdateDelay = intValue; }
			}
			if (categoryName == "maxFarmsInDeathMatch") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->maxFarmsInDeathMatch = intValue; }
			}
		}
		// Dislike
		if (elemName == "dislikeValues") {
			categoryName = elem->Attribute("name");
			if (categoryName == "dislikeComputeInterval") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->dislikeComputeInterval = intValue; }
			}
			if (categoryName == "dislikeAllArtefacts") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->dislike_allArtefacts = intValue; }
			}
			if (categoryName == "dislikeHuman") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->dislike_humanPlayer = intValue; }
			}
		}
		// Panic mode options
		if (elemName == "panicMode") {
			categoryName = elem->Attribute("name");
			if (categoryName == "delay") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->panicModeDelay = intValue; }
			}
			if (categoryName == "maxUnitCount") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->maxPanicUnitsCountToAddInStrategy = intValue; }
			}
		}
		// City plan options
		if (elemName == "cityPlan") {
			std::string buildingName = elem->Attribute("building");
			categoryName = elem->Attribute("name");
			if (buildingName == "all") {
				if (categoryName == "enhanceLikeValues") {
					this->cityPlanLikeValuesEnhancement = XML_GetBoolElement(elem, "enable");
				}
			}
			/*if (buildingName == "house") {
				if (categoryName == "distanceFromTownCenter") {
					callResult = elem->QueryFloatAttribute("value", &floatValue);
					if (callResult == TIXML_SUCCESS) { this->cityPlanHouseDistanceFromTownCenter = floatValue; }
				}
				if (categoryName == "distanceFromStorageBuildings") {
					callResult = elem->QueryFloatAttribute("value", &floatValue);
					if (callResult == TIXML_SUCCESS) { this->cityPlanHouseDistanceFromStorageBld = floatValue; }
				}
				if (categoryName == "distanceFromOtherBuildings") {
					callResult = elem->QueryFloatAttribute("value", &floatValue);
					if (callResult == TIXML_SUCCESS) { this->cityPlanHouseDistanceFromOtherBld = floatValue; }
				}
			}*/
			if (buildingName == "granary") {
				if (categoryName == "berryBushUnitWeight") {
					callResult = elem->QueryIntAttribute("value", &intValue);
					if (callResult == TIXML_SUCCESS) { this->cityPlanBerryBushWeightForGranary = intValue; }
				}
			}
		}

		if (elemName == "gameKey") {
			categoryName = elem->Attribute("name");
			if (categoryName == "callIdleNearbyArmies") {
				this->enableCallNearbyIdleMilitaryUnits = XML_GetBoolElement(elem, "enable");
			}
			if (categoryName == "callIdleNearbyArmies_distance") {
				callResult = elem->QueryIntAttribute("value", &intValue);
				if (callResult == TIXML_SUCCESS) { this->distanceToCallNearbyIdleMilitaryUnits = intValue; }
			}
			if (categoryName == "useNumPadUnitShortcuts") {
				this->enableAdditionalNumpadShortcuts = XML_GetBoolElement(elem, "enable");
			}
		}

		if (elemName == "autoRebuildFarms") {
			this->enableAutoRebuildFarms = XML_GetBoolElement(elem, "enable");
			callResult = elem->QueryIntAttribute("maxFarms", &intValue);
			if (callResult == TIXML_SUCCESS) { this->autoRebuildFarms_maxFarms = intValue; }
			callResult = elem->QueryIntAttribute("minWood", &intValue);
			if (callResult == TIXML_SUCCESS) { this->autoRebuildFarms_minWood = intValue; }
			callResult = elem->QueryIntAttribute("maxFood", &intValue);
			if (callResult == TIXML_SUCCESS) { this->autoRebuildFarms_maxFood = intValue; }
		}

		if (elemName == "unitSpawn") {
			categoryName = elem->Attribute("name");
			if (categoryName == "autoMoveToLocation") {
				this->enableSpawnUnitsMoveToLocation = XML_GetBoolElement(elem, "enable");
			}
			if (categoryName == "autoTarget") {
				this->enableSpawnUnitsAutoTarget = XML_GetBoolElement(elem, "enable");
			}
			if (categoryName == "autoRepairTC") {
				this->enableSpawnUnitAutoRepairTC = XML_GetBoolElement(elem, "enable");
			}
			if (categoryName == "shortcuts") {
				this->unitShortcutsPriorityReverseOrder = XML_GetBoolElement(elem, "reverseOrder");
			}
			if (categoryName == "shortcut") {
				int shortcutNumber = 0;
				callResult = elem->QueryIntAttribute("shortcut", &intValue);
				if (callResult == TIXML_SUCCESS) { shortcutNumber = intValue; }
				if (shortcutNumber >= CST_NUMBER_OF_UNIT_SHORTCUT_NUMBERS) { shortcutNumber = 0; } // prevents from going out of bounds

				if (shortcutNumber > 0) {
					callResult = elem->QueryIntAttribute("datid", &intValue);
					if (callResult == TIXML_SUCCESS) {
						this->unitShortcutsInformation[shortcutNumber].DAT_ID = intValue;
					}
					this->unitShortcutsInformation[shortcutNumber].onlyOneUnit = XML_GetBoolElement(elem, "onlyOneUnit");
				}
			}
		}



		elem = elem->NextSiblingElement(); // next XML element 
	}

	// Run some checks on values
	if (this->improvedGameSpeedFactor < this->MINVALUE_improvedGameSpeedFactor) { this->improvedGameSpeedFactor = this->MINVALUE_improvedGameSpeedFactor; }
	if (this->collectRORDebugLogs < this->MINVALUE_collectRORDebugLogs) { this->collectRORDebugLogs = this->MINVALUE_collectRORDebugLogs; }
	if (this->collectRORDebugLogs > this->MAXVALUE_collectRORDebugLogs) { this->collectRORDebugLogs = this->MAXVALUE_collectRORDebugLogs; }
	if (this->gameTimerSlowDownFactor < this->MINVALUE_gameTimerSlowDownFactor) { this->gameTimerSlowDownFactor = this->MINVALUE_gameTimerSlowDownFactor; }
	if (this->conversionResistance_Boats < this->MINVALUE_conversionResistance) { this->conversionResistance_Boats = this->MINVALUE_conversionResistance; }
	if (this->conversionResistance_Chariots < this->MINVALUE_conversionResistance) { this->conversionResistance_Chariots = this->MINVALUE_conversionResistance; }
	if (this->conversionResistance_Macedonian < this->MINVALUE_conversionResistance) { this->conversionResistance_Macedonian = this->MINVALUE_conversionResistance; }
	if (this->conversionResistance_Priests < this->MINVALUE_conversionResistance) { this->conversionResistance_Priests = this->MINVALUE_conversionResistance; }
	if (this->conversionResistance_WarElephants < this->MINVALUE_conversionResistance) { this->conversionResistance_WarElephants = this->MINVALUE_conversionResistance; }
	if (this->conversionResistance_WarElephants_Persian < this->MINVALUE_conversionResistance) { this->conversionResistance_WarElephants_Persian = this->MINVALUE_conversionResistance; }
	if (this->improveAILevel < this->MINVALUE_improveAILevel) { this->improveAILevel = this->MINVALUE_improveAILevel; }
	if (this->improveAILevel > this->MAXVALUE_improveAILevel) { this->improveAILevel = this->MAXVALUE_improveAILevel; }
	if (this->tacticalAIUpdateDelay < this->MINVALUE_tacticalAIUpdateDelay) { this->tacticalAIUpdateDelay = this->MINVALUE_tacticalAIUpdateDelay; }
	if (this->panicModeDelay < this->MINVALUE_panicModeDelay) { this->panicModeDelay = this->MINVALUE_panicModeDelay; }
	if (this->maxPanicUnitsCountToAddInStrategy < this->MINVALUE_maxPanicUnitsCountToAddInStrategy) { this->maxPanicUnitsCountToAddInStrategy = this->MINVALUE_maxPanicUnitsCountToAddInStrategy; }
	if (this->minPopulationBeforeBuildOptionalItems < this->MINVALUE_minPopulationBeforeBuildOptionalItems) { this->minPopulationBeforeBuildOptionalItems = this->MINVALUE_minPopulationBeforeBuildOptionalItems; }
	if (this->maxFarmsInDeathMatch < this->MINVALUE_maxFarmsInDeathMatch) { this->maxFarmsInDeathMatch = this->MINVALUE_maxFarmsInDeathMatch; }
	if (this->autoRebuildFarms_maxFarms < this->MINVALUE_maxFarmsInDeathMatch) { this->autoRebuildFarms_maxFarms = this->MINVALUE_maxFarmsInDeathMatch; }
	if (this->autoRebuildFarms_maxFood < this->MINVALUE_resourceAmount) { this->autoRebuildFarms_maxFood = this->MINVALUE_resourceAmount; }
	if (this->autoRebuildFarms_minWood < this->MINVALUE_resourceAmount) { this->autoRebuildFarms_minWood = this->MINVALUE_resourceAmount; }
	if ((this->dislikeComputeInterval != 0) && (this->dislikeComputeInterval < this->MINVALUE_dislikeComputeInterval)) { this->dislikeComputeInterval = this->MINVALUE_dislikeComputeInterval; }
	if (this->dislike_allArtefacts < this->MINVALUE_dislike) { this->dislike_allArtefacts = this->MINVALUE_dislike; }
	if (this->dislike_humanPlayer < this->MINVALUE_dislike) { this->dislike_humanPlayer = this->MINVALUE_dislike; }
	if (this->dislike_allArtefacts > this->MAXVALUE_dislike) { this->dislike_allArtefacts = this->MAXVALUE_dislike; }
	if (this->dislike_humanPlayer > this->MAXVALUE_dislike) { this->dislike_humanPlayer = this->MAXVALUE_dislike; }
	if (this->randomMapRelicsCount < this->MINVALUE_artefactCount) { this->randomMapRelicsCount = this->MINVALUE_artefactCount; }
	if (this->randomMapRelicsCount > this->MAXVALUE_artefactCount) { this->randomMapRelicsCount = this->MAXVALUE_artefactCount; }
	if (this->randomMapRuinsCount < this->MINVALUE_artefactCount) { this->randomMapRuinsCount = this->MINVALUE_artefactCount; }
	if (this->randomMapRuinsCount > this->MAXVALUE_artefactCount) { this->randomMapRuinsCount = this->MAXVALUE_artefactCount; }
	// No check on elevation factors
	/*if (this->cityPlanHouseDistanceFromTownCenter < this->MINVALUE_cityPlanBuildingDistances) { this->cityPlanHouseDistanceFromTownCenter = this->MINVALUE_cityPlanBuildingDistances; }
	if (this->cityPlanHouseDistanceFromTownCenter > this->MAXVALUE_cityPlanBuildingDistances) { this->cityPlanHouseDistanceFromTownCenter = this->MAXVALUE_cityPlanBuildingDistances; }
	if (this->cityPlanHouseDistanceFromStorageBld < this->MINVALUE_cityPlanBuildingDistances) { this->cityPlanHouseDistanceFromStorageBld = this->MINVALUE_cityPlanBuildingDistances; }
	if (this->cityPlanHouseDistanceFromStorageBld > this->MAXVALUE_cityPlanBuildingDistances) { this->cityPlanHouseDistanceFromStorageBld = this->MAXVALUE_cityPlanBuildingDistances; }
	if (this->cityPlanHouseDistanceFromOtherBld < this->MINVALUE_cityPlanBuildingDistances) { this->cityPlanHouseDistanceFromOtherBld = this->MINVALUE_cityPlanBuildingDistances; }
	if (this->cityPlanHouseDistanceFromOtherBld > this->MAXVALUE_cityPlanBuildingDistances) { this->cityPlanHouseDistanceFromOtherBld = this->MAXVALUE_cityPlanBuildingDistances; }
	*/
	if (this->cityPlanBerryBushWeightForGranary < this->MINVALUE_cityPlanBerryBushWeightForGranary) { this->cityPlanBerryBushWeightForGranary = this->MINVALUE_cityPlanBerryBushWeightForGranary; }
	if (this->cityPlanBerryBushWeightForGranary > this->MAXVALUE_cityPlanBerryBushWeightForGranary) { this->cityPlanBerryBushWeightForGranary = this->MAXVALUE_cityPlanBerryBushWeightForGranary; }
	if (this->distanceToCallNearbyIdleMilitaryUnits < this->MINVALUE_distanceToCallNearbyIdleMilitaryUnits) { this->distanceToCallNearbyIdleMilitaryUnits = this->MINVALUE_distanceToCallNearbyIdleMilitaryUnits; }
	if (this->distanceToCallNearbyIdleMilitaryUnits > this->MAXVALUE_distanceToCallNearbyIdleMilitaryUnits) { this->distanceToCallNearbyIdleMilitaryUnits = this->MAXVALUE_distanceToCallNearbyIdleMilitaryUnits; }

	return true;
}


bool CustomRORConfig::ReadCivXMLConfigFile(char *fileName) {
	TiXmlDocument doc(fileName);
	if (!doc.LoadFile()) {
		this->couldNotReadCivXMLConfig = true;
		return false;
	}

	TiXmlHandle hdl(&doc);
	string mainNodeTag = hdl.FirstChildElement().Element()->Value();
	if (mainNodeTag != "customROR_civs") {
		// Not a valid customROR config XML file.
		this->couldNotReadCivXMLConfig = true;
		return false;
	}
	TiXmlElement *elem = hdl.FirstChildElement().FirstChildElement().Element();
	if (!elem) {
		return false;
	}

	while (elem) {
		int callResult;
		int intValue;
		string elemName = elem->Value();
		string categoryName;
		if (elemName == "civilizations") {
			callResult = elem->QueryIntAttribute("count", &intValue);
			assert(abs(intValue) < 256);
			if (callResult == TIXML_SUCCESS) { this->civCount = intValue; }
		}

		if (elemName == "civ") {
			int civId = 0;
			callResult = elem->QueryIntAttribute("id", &intValue);
			if (callResult == TIXML_SUCCESS) { civId = intValue; }
			// if ((civId >= 0) && (civId < AOE_CONST_FUNC::CIVILIZATIONS::CST_CIVID_STANDARD_MAX)) {...} // if we want to allow changing AI files, etc for standard civs?
			if ((civId > AOE_CONST_FUNC::CIVILIZATIONS::CST_CIVID_STANDARD_MAX) && (civId < 256)) {
				if (!this->allCivInfo[civId]) {
					this->allCivInfo[civId] = new CivilizationInfo(civId);
				}

				TiXmlElement *subElem = elem->FirstChildElement();
				std::string mapCategory;
				while (subElem) {
					string subElemName = subElem->Value();
					if (subElemName == "civ_name") {
						this->allCivInfo[civId]->SetCivName(subElem->Attribute("value"));
					}
					if (subElemName == "deathmatch_aifile") {
						this->allCivInfo[civId]->deathmatch_AI_file = subElem->Attribute("value");
					}
					if (subElemName == "deathmatch_aifile_water") {
						this->allCivInfo[civId]->deathmatch_water_AI_file = subElem->Attribute("value");
					}
					if (subElemName == "rm_aifile") {
						mapCategory = subElem->Attribute("mapCategory");
						if (mapCategory == "no_water") {
							this->allCivInfo[civId]->RM_AI_file_no_water = subElem->Attribute("value");
						}
						if (mapCategory == "some_water") {
							this->allCivInfo[civId]->RM_AI_file_some_water = subElem->Attribute("value");
						}
						if (mapCategory == "much_water") {
							this->allCivInfo[civId]->RM_AI_file_much_water = subElem->Attribute("value");
						}
					}
					if (subElemName == "player_name") {
						int index = 0;
						callResult = subElem->QueryIntAttribute("index", &intValue);
						if (callResult == TIXML_SUCCESS) { index = intValue; }
						this->allCivInfo[civId]->SetPlayerName(index, subElem->Attribute("value"));
					}
					subElem = subElem->NextSiblingElement();
				}
			}
		}

		elem = elem->NextSiblingElement(); // next XML element 
	}

	// Run some checks on values
	if (this->civCount > this->MAXVALUE_civCount) { this->civCount = this->MAXVALUE_civCount; }
	if (this->civCount < this->MINVALUE_civCount) { this->civCount = this->MINVALUE_civCount; }

	return true;
}


CivilizationInfo *CustomRORConfig::GetCivInfo(int civId) {
	if ((civId < 0) || (civId >= CST_MAX_TOTAL_CIV_COUNT)) { return NULL; }
	return this->allCivInfo[civId];
}
