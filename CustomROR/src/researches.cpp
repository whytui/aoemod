#include "../include/researches.h"

using namespace ROR_STRUCTURES_10C;
using namespace AOE_CONST_FUNC;


// Get a technology name from languagex.dll or language.dll.
// Technologies don't really have a name, we use matching research to find it. Works in many cases, not all.
std::string GetTechnologyLocalizedName(short int techId) {
	if (techId < 0) {
		return "invalid-tech";
	}
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = *ROR_gameSettings;
	assert(settings != NULL);
	if (!settings) { return ""; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->researchDefInfo) {
		return "";
	}
	for (int i = 0; i < global->researchDefInfo->researchCount; i++) {
		if (global->researchDefInfo->researchDefArray[i].technologyId == techId) {
			char buffer[100];
			buffer[0] = 0;
			GetLanguageDllText(global->researchDefInfo->researchDefArray[i].languageDLLName, buffer, sizeof(buffer),
				global->researchDefInfo->researchDefArray[i].researchName);
			return std::string(buffer);
		}
	}
	std::string name = "techid-";
	name += std::to_string(techId);
	return name;
}


// Get a research name from languagex.dll or language.dll. Or internal name if no localization was found.
std::string GetResearchLocalizedName(short int researchId) {
	if (researchId < 0) {
		return "invalid-research";
	}
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = *ROR_gameSettings;
	assert(settings != NULL);
	if (!settings) { return ""; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->researchDefInfo) {
		return "";
	}
	if (researchId >= global->researchDefInfo->researchCount) {
		return "invalid-research";
	}
	char buffer[100];
	buffer[0] = 0;
	GetLanguageDllText(global->researchDefInfo->researchDefArray[researchId].languageDLLName, buffer, sizeof(buffer),
		global->researchDefInfo->researchDefArray[researchId].researchName);
	return std::string(buffer);
}


// Returns true if the research has more required researches than "minimum required researches count" (all requirements are not mandatory)
bool ResearchHasOptionalRequirements(STRUCT_RESEARCH_DEF *resDef) {
	if (!resDef) { return false; }
	int actualCount = 0;
	for (int i = 0; i < 4; i++) {
		if (resDef->requiredResearchId[i] > -1) {
			actualCount++;
		}
	}
	return (actualCount > resDef->minRequiredResearchesCount);
}

// Returns the researchId of an age (stone/tool/bronze/iron) if found in one of the 4 research's requirements. NOT recursive.
// Returns -1 if not found, a value in [CST_RSID_STONE_AGE, CST_RSID_IRON_AGE] if found.
short int GetAgeResearchFromDirectRequirement(STRUCT_RESEARCH_DEF *researchDef) {
	for (int iReq = 0; iReq < 4; iReq++) {
		// Age researches are consecutive. Republic age w ould be next one (104) if enabled
		if ((researchDef->requiredResearchId[iReq] >= CST_RSID_STONE_AGE) &&
			(researchDef->requiredResearchId[iReq] <= CST_RSID_IRON_AGE)) {
			return researchDef->requiredResearchId[iReq];
		}
	}
	return -1;
}

// Disable all impossible researches for a specific player.
// An impossible research is a research that is waiting for requirements, including ones that can never be satisfied.
// Example in original game: irrigation for persian, armored elephant for yamato, etc.
int DisablePlayerImpossibleResearches(STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) {
		return 0;
	}
	STRUCT_PLAYER_RESEARCH_INFO *rinfo = player->ptrResearchesStruct;
	if (!rinfo) { return 0; }
	int resCount = rinfo->researchCount;
	STRUCT_PLAYER_RESEARCH_STATUS *statuses = rinfo->researchStatusesArray;
	STRUCT_RESEARCH_DEF_INFO *resInfoArray = rinfo->ptrResearchDefInfo;
	if (!resInfoArray) { return 0; }
	STRUCT_RESEARCH_DEF *resDefArray = resInfoArray->researchDefArray;

	std::vector<short int> availableResearches; // status in AVAILABLE, BEING_RESEARCHED, DONE_OR_INVALID
	std::vector<short int> disabledResearches; // CST_RESEARCH_STATUS_DISABLED
	std::vector<short int> potentialResearches; // CST_RESEARCH_STATUS_WAITING_REQUIREMENT. We don't know if they can REALLy become available.
	std::vector<short int> potentialToRemove; // internal temp buffer for loop

	// Loop 1 : sort researches into 3 categories (disabled, done or available, unknown)
	for (int i = 0; i < resCount; i++) {
		if (statuses[i].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) {
			disabledResearches.push_back(i);
		} else {
			if (statuses[i].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) {
				potentialResearches.push_back(i);
			} else {
				availableResearches.push_back(i);
			}
		}
	}

	std::string playerString = "player #";
	playerString += std::to_string(player->playerId);
	playerString += " (";
	playerString += player->playerName_length16max;
	playerString += ")";

	// Loop 2 : "recursively" treat elements with "unknown" status (potentialResearches)
	// until it is empty or we don't progress anymore.
	int foundCount = 0;
	int remainingUnknownCount = potentialResearches.size();
	int previousRemainingCount = 9999;
	while ((remainingUnknownCount > 0) && (remainingUnknownCount != previousRemainingCount)) {
		for each (int curResId in potentialResearches) {
			ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef = &resDefArray[curResId];
			int minRequiredResearchesCount = resDef->minRequiredResearchesCount;
			int reqResOK = 0;
			int reqResImpossible = 0;

			for (int i = 0; i < 4; i++) {
				if ((resDef->requiredResearchId[i] == -1) ||
					(std::count(disabledResearches.begin(), disabledResearches.end(), resDef->requiredResearchId[i]) > 0)) {
					reqResImpossible++;
				}
				if (std::count(availableResearches.begin(), availableResearches.end(), resDef->requiredResearchId[i]) > 0) {
					reqResOK++;
				}
			}
			if (reqResOK >= minRequiredResearchesCount) {
				availableResearches.push_back(curResId);
				potentialToRemove.push_back(curResId);
			}
			if (4 - reqResImpossible < minRequiredResearchesCount) {
				// We found an impossible research (some requirements can't be obtained)
				disabledResearches.push_back(curResId);
				potentialToRemove.push_back(curResId);
				foundCount++;
				assert(statuses[curResId].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT);
				statuses[curResId].currentStatus = RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED;
				std::string tmpmsg = playerString;
				tmpmsg += ": this research can never become available: ";
				tmpmsg += std::to_string(curResId);
				tmpmsg += " = ";
				char tmpbuffer[100];
				GetLanguageDllText(resDef->languageDLLName, tmpbuffer, sizeof(tmpbuffer), resDef->researchName);
				tmpmsg += tmpbuffer;
				traceMessageHandler.WriteMessageNoNotification(tmpmsg);
			}
		}
		// Do not do the remove IN the loop ! Do it once iterations are finished
		for each (auto curResId in potentialToRemove)
		{
			auto it = remove_if(potentialResearches.begin(), potentialResearches.end(), [curResId](short int i) {return i == curResId; });
			potentialResearches.erase(it, potentialResearches.end());
		}
		potentialToRemove.clear(); // reset temp loop buffer

		previousRemainingCount = remainingUnknownCount;
		remainingUnknownCount = potentialResearches.size();
	}
	if (remainingUnknownCount > 0) {
		std::string msg = "Could not complete researches tree analysis for ";
		msg += playerString;
		traceMessageHandler.WriteMessage(msg);
	}
	return foundCount;
}

// Disable all impossible researches for all players.
// An impossible research is a research that is waiting for requirements, including ones that can never be satisfied.
// Example in original game: irrigation for persian, armored elephant for yamato, etc.
int DisableImpossibleResearches() {
	STRUCT_GAME_SETTINGS *settings = *ROR_gameSettings;
	if (!settings || !settings->IsCheckSumValid()) { return 0; }
	STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->ptrPlayerStructPtrTable) {
		return 0;
	}
	int result = 0;
	for (int i = 1; i < global->playerTotalCount; i++) {
		STRUCT_PLAYER *player = global->ptrPlayerStructPtrTable[i];
		result += DisablePlayerImpossibleResearches(player);
	}
	return result;
}


// Returns true if technology has at least one effect on provided unit definition.
// Effect can be negative too.
// Priest sacrifice tech is IGNORED here.
bool DoesTechAffectUnit(STRUCT_TECH_DEF *techDef, STRUCT_UNITDEF_BASE *unitDef) {
	if (!techDef || !unitDef || !unitDef->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		// Does this affect affect units ?
		if (techEffect && (techEffect->HasValidEffect()) && (
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) ||
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) ||
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT)
			)) {
			if (techEffect->effectUnit == unitDef->DAT_ID1) { return true; }
			if (techEffect->effectClass == unitDef->unitAIType) { return true; }
		}
		if (techEffect->IsEnableUnit(unitDef->DAT_ID1)) {
			return true;
		}
		if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT)) {
			// Upgrade unit : effectClass field is "TO" unit
			if ((techEffect->effectUnit == unitDef->DAT_ID1) || (techEffect->effectClass == unitDef->DAT_ID1)) {
				return true;
			}
		}
		bool isResourceModifier = ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET) ||
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_MULT));
		// Very special cases: some resources can improve units: priest/building conversion, etc
		if (unitDef->unitAIType == TribeAIGroupPriest) {
			if (isResourceModifier && (
				(techEffect->effectUnit == CST_RES_ORDER_CAN_CONVERT_BUILDING) ||
				(techEffect->effectUnit == CST_RES_ORDER_CAN_CONVERT_PRIEST) ||
				(techEffect->effectUnit == CST_RES_ORDER_FAITH_RECHARGING_RATE) ||
				(techEffect->effectUnit == CST_RES_ORDER_HEALING) // (medecine tech)
				// (techEffect->effectUnit == CST_RES_ORDER_PRIEST_SACRIFICE) // AI will never use it. Ignore it.
			)) {
				return true; // Priest & building/priest conversion + faith recharging rate
			}
		}
		if (IsVillager(unitDef->DAT_ID1)) { // excluding boats
			if (isResourceModifier) {
				if (techEffect->effectUnit == CST_RES_ORDER_GOLD_MINING_PODUCTIVITY) { return true; } // Villagers: mining productivity
			}
		}
		if (unitDef->DAT_ID1 == CST_UNITID_FARM) {
			if (isResourceModifier) {
				if (techEffect->effectUnit == CST_RES_ORDER_FARM_FOOD_AMOUNT) { return true; } // Farms : farm amount
			}
		}
	}
	return false;
}


// Returns true if technology enables provided unit (including upgrade TO provided unit, if provided unit is an upgrade).
bool DoesTechEnableUnit(STRUCT_TECH_DEF *techDef, short int unitDefId) {
	if (!techDef || (unitDefId < 0)) { // -1 CAN'T be a joker here !
		return false;
	}
	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		if (techEffect) {
			// Enable unit
			if (techEffect->IsEnableUnit(unitDefId)) { return true; }
			// Upgrade some unit to THIS unit
			if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) && (techEffect->effectClass == unitDefId)) {
				return true;
			}
		}
	}
	return false;
}


// Finds all (non disabled) researches that affect a unit (definition)
// If ignoreUndesirableTechs==true, techs from LST_TECHS_TO_IGNORE are ignored (jihad, etc)
// Returns a list of research IDs.
std::vector<short int> FindResearchesThatAffectUnit(STRUCT_PLAYER *player, long int unitDefId, bool ignoreUndesirableTechs) {
	std::vector<short int> result;
	if (!player || !player->IsCheckSumValid() || (player->structDefUnitArraySize < unitDefId)) { return result; }
	STRUCT_UNITDEF_BASE *unitDefBase = (STRUCT_UNITDEF_BASE *)player->ptrStructDefUnitTable[unitDefId];
	STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid()) { return result; }
	if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) { return result; }

	STRUCT_PLAYER_RESEARCH_INFO *rinfo = player->ptrResearchesStruct;
	if (!rinfo) { return result; }
	int resCount = rinfo->researchCount;
	STRUCT_PLAYER_RESEARCH_STATUS *statuses = rinfo->researchStatusesArray;
	STRUCT_RESEARCH_DEF_INFO *resInfoArray = rinfo->ptrResearchDefInfo;
	if (!resInfoArray) { return result; }
	STRUCT_RESEARCH_DEF *resDefArray = resInfoArray->researchDefArray;

	for (int researchId = 0; researchId < resCount; researchId++) {
		if ((statuses[researchId].currentStatus != RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) &&
			(resDefArray[researchId].researchName != NULL)) { // to filter out "New research" (invalid researches have no name)
			bool added = false;
			short int techId = resDefArray[researchId].technologyId;
			if ((techId >= 0) && (techId < global->technologiesInfo->technologyCount) &&
				(global->technologiesInfo->ptrTechDefArray[techId].effectCount > 0)) {
				// We have a valid technology id. Add to result if it affects our unit.
				if (!added && DoesTechAffectUnit(&global->technologiesInfo->ptrTechDefArray[techId], unitDefBase)) {
					result.push_back(researchId);
					added = true;
				}
			}
		}
	}
	return result;
}


// Returns the first (non disabled) research ID found that enables a unit. Returns -1 if not found.
// startAtResearchId : -1=ignored (joker). If >=0, than the search will start at this index and ignore previous researches.
short int FindResearchThatEnableUnit(STRUCT_PLAYER *player, short int unitDefId, short int startAtResearchId) {
	if (!player || !player->IsCheckSumValid() || !player->ptrGlobalStruct || !player->ptrGlobalStruct->IsCheckSumValid() ||
		!player->ptrGlobalStruct->technologiesInfo || !player->ptrGlobalStruct->technologiesInfo->IsCheckSumValid() ||
		!player->ptrGlobalStruct->technologiesInfo->ptrTechDefArray) {
		return -1;
	}
	if (startAtResearchId < 0) { startAtResearchId = 0; }
	STRUCT_PLAYER_RESEARCH_INFO *rinfo = player->ptrResearchesStruct;
	if (!rinfo) { return -1; }
	int resCount = rinfo->researchCount;
	STRUCT_PLAYER_RESEARCH_STATUS *statuses = rinfo->researchStatusesArray;
	STRUCT_RESEARCH_DEF_INFO *resInfoArray = rinfo->ptrResearchDefInfo;
	if (!resInfoArray || !statuses) { return -1; }
	STRUCT_RESEARCH_DEF *resDefArray = resInfoArray->researchDefArray;
	for (int researchId = startAtResearchId; researchId < resCount; researchId++) {
		if (statuses[researchId].currentStatus > RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) {
			short int techId = resDefArray[researchId].technologyId;
			if ((techId >= 0) && DoesTechEnableUnit(&player->ptrGlobalStruct->technologiesInfo->ptrTechDefArray[techId], unitDefId)) {
				return researchId;
			}
		}
	}
	return -1;
}


// Returns an ordered list of research IDs that allow researching input researches (researchesList) with all dependencies
// Returned collection includes all dependencies, including "shadow" researches (no location).
// Warning: dependency with choices (optional requirements, e.g. research74="A01, B01, F01, I01 ->Tool Age" are returned
// ... BUT underlying requirements are NOT analyzed (as we can't choose which requirements should be used / ignored)
// The caller MUST handle such researches (tip: use ResearchHasOptionalRequirements(...) to identify them)
// All research IDs in returned collection are available in tech tree. Impossible research IDs from researchesList are NOT in returned collection.
std::vector<short int> GetValidOrderedResearchesListWithDependencies(STRUCT_PLAYER *player, std::vector<short int> researchesList) {
	std::vector<short int> allValidResearchesToReturn;
	if (!player || !player->IsCheckSumValid()) { return allValidResearchesToReturn; }
	STRUCT_PLAYER_RESEARCH_INFO *rinfo = player->ptrResearchesStruct;
	if (!rinfo) { return allValidResearchesToReturn; }
	int resCount = rinfo->researchCount;
	STRUCT_PLAYER_RESEARCH_STATUS *statuses = rinfo->researchStatusesArray;
	STRUCT_RESEARCH_DEF_INFO *resInfoArray = rinfo->ptrResearchDefInfo;
	if (!resInfoArray || !statuses) { return allValidResearchesToReturn; }
	STRUCT_RESEARCH_DEF *resDefArray = resInfoArray->researchDefArray;

	std::set<short int> requiredResearches; // Set of research IDs that are required (including input list)
	for each (short int researchId in researchesList) {
		requiredResearches.insert(researchId);
	}

	// First collect all required researches (input list + dependencies), recursively (for dependencies)
	int previousCount = 0;
	int currentCount = requiredResearches.size();
	while (previousCount < currentCount) {
		std::set<short int> elementsToAdd;
		// Collect required reseaches (=>elementsToAdd) for all researches in list (requiredResearches)
		for each (short int researchId in requiredResearches)
		{
			STRUCT_RESEARCH_DEF *resDef = &resDefArray[researchId];
			for (int i = 0; i < 4; i++) {
				if (!ResearchHasOptionalRequirements(resDef) && (resDef->requiredResearchId[i] > -1)) {
					// Don't add optional requirements (we can't guess which ones should be actually added)
					elementsToAdd.insert(resDef->requiredResearchId[i]);
				}
			}
		}
		// Do not insert into requiredResearches while looping in it !
		for each (short int curResId in elementsToAdd) {
			requiredResearches.insert(curResId); // will not insert duplicates
		}
		elementsToAdd.clear();
		previousCount = currentCount;
		currentCount = requiredResearches.size();
	}

	// Move available researches to "ok" list until requiredResearches is empty (or we're stuck)

	int remainingRequirementsCount = requiredResearches.size();
	int previousRemainingCount = 9999;
	std::set<short int> requiredElementsToRemove;
	//std::set<short int> impossibleResearches;

	while ((remainingRequirementsCount > 0) && (remainingRequirementsCount != previousRemainingCount)) {
		for each (short int curResId in requiredResearches) {
			STRUCT_RESEARCH_DEF *resDef = &resDefArray[curResId];
			switch (statuses[curResId].currentStatus) {
			case RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE:
			case RESEARCH_STATUSES::CST_RESEARCH_STATUS_BEING_RESEARCHED:
			case RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID:
				// This research is OK for tech tree
				requiredElementsToRemove.insert(curResId);
				allValidResearchesToReturn.push_back(curResId);
				break;
			case RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT:
				// If all requirements are OK, then it's ok. Otherwise, wait for a next loop (some dependencies may evolve).
				bool requirementsAreOk;
				// Compiler bug: can't initialize vars in declaration in a case ! (lol)
				requirementsAreOk = true;
				if (ResearchHasOptionalRequirements(resDef)) {
					// When a research with "choices" is met, do not go further and consider it OK
					// Such researches are shadow technologies for buildings/ages. 
					// The caller *MUST* check their requirements, we don't have enough context here (when minReq=2 and we have 4 requirements, which ones should we choose ?)
					requirementsAreOk = true;
				} else {
					for (int i = 0; i < 4; i++) {
						short int requiredResearchId = resDef->requiredResearchId[i];
						if (requiredResearchId > -1) {
							bool requiredResearchIsAvailable = (statuses[requiredResearchId].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE) ||
								(statuses[requiredResearchId].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_BEING_RESEARCHED) ||
								(statuses[requiredResearchId].currentStatus == RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID) ||
								// Shadow intermediate researches (no location) are always considered OK here
								// They can be Market+Bronze Age->Temple, {2 bronze bld}->Iron Age, etc
								(resDef->researchLocation == -1);
							// If requirement is NOT currently available and NOT in our "OK list", then the dependency can't be satisfied (for now)
							if (!requiredResearchIsAvailable &&
								(std::count(allValidResearchesToReturn.begin(), allValidResearchesToReturn.end(), requiredResearchId) == 0)) {
								requirementsAreOk = false;
							}
						}
					}
				}
				if (requirementsAreOk) {
					requiredElementsToRemove.insert(curResId);
					allValidResearchesToReturn.push_back(curResId);
				}
				break;
			case RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED:
				//impossibleResearches.insert(curResId); // Unused / should not happen if tech tree is consistent ?
				break;
			default:
				break;
			}
		}

		// Do not do the remove IN the loop ! Do it once iterations are finished
		for each (short int curResId in requiredElementsToRemove)
		{
			requiredResearches.erase(curResId);
		}
		requiredElementsToRemove.clear(); // reset temp loop buffer
		previousRemainingCount = remainingRequirementsCount;
		remainingRequirementsCount = requiredResearches.size();
	}

	if (remainingRequirementsCount > 0) {
		std::string msg = "[Build research requirements tree] some researches are not available: ";
		bool first = true;
		for each (short int researchId in requiredResearches)
		{
			if (!first) {
				msg += ", ";
			}
			first = false;
			msg += std::to_string(researchId);
		}
		traceMessageHandler.WriteMessageNoNotification(msg);
	}
	return allValidResearchesToReturn;
}


// Returns a unit (building) definition that enables provided research ID.
// For example, for research 17, it will return temple's definition because temple's unitDef.initiatesResearch == 17.
// Returns NULL if not found.
ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *FindBuildingDefThatEnablesResearch(STRUCT_PLAYER *player, short int researchId) {
	if (!player || !player->IsCheckSumValid() || researchId < 0) { return NULL; }
	
	for (int unitDefId = 0; unitDefId < player->structDefUnitArraySize; unitDefId++) {
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDefBuilding = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING*)player->ptrStructDefUnitTable[unitDefId];
		if (unitDefBuilding && unitDefBuilding->IsCheckSumValid() && unitDefBuilding->IsTypeValid()) {
			if (unitDefBuilding->initiatesResearch == researchId) {
				return unitDefBuilding;
			}
		}
	}
	return NULL;
}


// [private] Writes a string representing tech tree information for a given research
// defaultResearchName = name to write for research, if none can be found automatically
std::string GetResearchTechTreeLine(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, short int researchId, 
	const char *defaultResearchName) {
	std::string result;
	char nameBuffer[50];
	nameBuffer[0] = 0;
	bool isAutomaticTech = (player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[researchId].researchTime <= 0);
	result = "techId ";
	result += std::to_string(player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[researchId].technologyId);
	result += " = ";
	GetLanguageDllText(player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[researchId].languageDLLName, nameBuffer, sizeof(nameBuffer) - 1,
		player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[researchId].researchName);
	if (isAutomaticTech) { result += "("; }
	result += ((nameBuffer[0] == 0) ? defaultResearchName : nameBuffer);
	if (isAutomaticTech) { result += ")"; }

	ROR_STRUCTURES_10C::STRUCT_PLAYER_RESEARCH_STATUS *rs = player->ptrResearchesStruct->researchStatusesArray; // ->currentStatus
	if (rs[researchId].currentStatus == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) {
		result += " (";
		result += localizationHandler.GetTranslation(CRLANG_ID_NOT_AVAILABLE_YET, "not available yet");
		result += " )";
	}
	return result;
}

// [private] Get a string representing all (future) available tech tree for building
// (researches and units that can become available in given building)
std::string GetRemainingTechTreeText(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *bldDef) {
	// Invalid (technical reasons)
	if (!bldDef || !bldDef->IsCheckSumValidForAUnitClass() || !bldDef->IsTypeValid() ||
		!player || !player->IsCheckSumValid()) { return ""; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid() ||
		!global->technologiesInfo->ptrTechDefArray) { return ""; }
	// Not relevant (functional reasons)
	if ((bldDef->unitType != AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING) ||
		(bldDef->hideInEditor != 0 /*exclude duplicates*/) || (bldDef->unitAIType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding/*excludes walls*/) ||
		(bldDef->interactionMode != 3/*exclude towers*/) || (bldDef->triggerType != 2)
		) { return ""; }
	std::string result = "";
	char nameBuffer[50];
	char nameBufferBackup[50];
	GetLanguageDllText(bldDef->languageDLLID_Name, nameBuffer, sizeof(nameBuffer), bldDef->ptrUnitName);
	result += std::string(nameBuffer) + std::string(": "); // Write building name as a "header" (title)

	bool foundItems = false;
	short int researchCount = player->ptrResearchesStruct->researchCount;
	ROR_STRUCTURES_10C::STRUCT_PLAYER_RESEARCH_STATUS *rs = player->ptrResearchesStruct->researchStatusesArray; // ->currentStatus
	// Search all researches that are developed in this building (and write text for each).
	for (int rid = 0; rid < researchCount; rid++) {
		*nameBuffer = 0; // Reset string for next usage
		*nameBufferBackup = 0;
		if ((rs[rid].currentStatus >= AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) &&
			(rs[rid].currentStatus < AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DONE_OR_INVALID)) {

			short int researchLocation = player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[rid].researchLocation;
			if (researchLocation < 0) {
				// Some research, like slinger, camel, are missing researchLocation (-1). We can try to retrieve it from technology/enabledUnit=>trainlocation
				short int techId = player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[rid].technologyId;
				if ((techId >= 0) && (techId < global->technologiesInfo->technologyCount)) {
					ROR_STRUCTURES_10C::STRUCT_TECH_DEF *techDef = &global->technologiesInfo->ptrTechDefArray[techId];
					short int enabledUnitDefID = -1;
					if (techDef) {
						// Find if this tech enables some unit
						for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
							STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
							if (techEffect && techEffect->HasValidEffect()) {
								// Enable unit
								if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (techEffect->effectClass == 1)) {
									enabledUnitDefID = techEffect->effectUnit;
								}
								// Upgrade *to* some unit
								if (techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) {
									enabledUnitDefID = techEffect->UpgradeUnitGetTargetUnit();
								}
							}
						}
					}
					if (enabledUnitDefID >= 0) {
						ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *enabledUnitDefBase = player->GetUnitDefBase(enabledUnitDefID);
						if (enabledUnitDefBase && enabledUnitDefBase->IsCheckSumValidForAUnitClass() && enabledUnitDefBase->DerivesFromLiving()) {
							ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *t = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *)enabledUnitDefBase;
							if (t->trainLocation == bldDef->DAT_ID1) {
								// Get unit name. Might be used if research does not have a name
								GetLanguageDllText(t->languageDLLID_Name, nameBufferBackup, sizeof(nameBufferBackup), t->ptrUnitName);
								result += "\r\n";
								result += GetResearchTechTreeLine(player, rid, nameBufferBackup);
								foundItems = true;
							}
						}
					}
				}
			} else {
				if ((researchLocation == bldDef->DAT_ID1)) {
					result += "\r\n";
					result += GetResearchTechTreeLine(player, rid, "");
					foundItems = true;
				}
			}
		}
	}
	if (foundItems) {
		result += std::string("\r\n");
	}
	return result;
}


// Writes text representing available tech tree (available technologies that have not been researched yet)
std::string GetRemainingTechTreeText(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) { return ""; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid() || !global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid() ||
		!global->technologiesInfo->ptrTechDefArray) {
		return "";
	}
	std::string result;
	int unitDefCount = player->structDefUnitArraySize;
	// Loop on all buildings that can potentially "host" researches
	for (int i = 0; i < unitDefCount; i++) {
		ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = player->ptrStructDefUnitTable[i];
		std::string s = GetRemainingTechTreeText(player, (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING*)unitDef);
		if (!s.empty()) {
			result += s;
			result += "\r\n"; // Leave an line break
		}
	}
	return result;
}

