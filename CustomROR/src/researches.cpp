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
	STRUCT_GAME_GLOBAL *global = *ROR_gameGlobal;
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
bool DoesTechAffectUnit(STRUCT_TECH_DEF *techDef, STRUCT_UNITDEF_BASE *unitDef) {
	if (!techDef || !unitDef || !unitDef->IsCheckSumValidForAUnitClass()) {
		return false;
	}

	for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
		STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
		// Does this affect affect units ?
		if (techEffect && (
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) ||
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) ||
			(techEffect->effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT)
			)) {
			if (techEffect->effectUnit == unitDef->DAT_ID1) { return true; }
			if (techEffect->effectClass == unitDef->unitAIType) { return true; }
		}
		if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (techEffect->effectClass == 1)) {
			if (techEffect->effectUnit == unitDef->DAT_ID1) {
				return true;
			}
		}
		if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT)) {
			// Upgrade unit : effectClass field is "TO" unit
			if ((techEffect->effectUnit == unitDef->DAT_ID1) || (techEffect->effectClass == unitDef->DAT_ID1)) {
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


// Returns an ordered list of research IDs that allow researching input researches (researchesList) with all dependencies
// Returned collection includes all dependencies, including "shadow" researches (no location).
// Warning: dependency with choices (optional requirements, e.g. research74="A01, B01, F01, I01 ->Tool Age" are returned
// ... BUT underlying requirements are NOT analyzed (as we can't choose which requirements should be used / ignored)
// The caller MUST handle such researches (tip: use ResearchHasOptionalRequirements(...) to identify them)
// All research IDs in returned collection are available in tech tree. Impossible research IDs from researchesList are NOT in returned collection.
std::vector<short int> GetValidOrderedResearchesListWithDependencies(STRUCT_PLAYER *player, std::vector<short int> researchesList) {
	std::vector<short int> allValidResearchesToReturn;
	//std::set<short int> allValidResearchesToReturn;
	if (!player || !player->IsCheckSumValid()) { return allValidResearchesToReturn; }
	STRUCT_PLAYER_RESEARCH_INFO *rinfo = player->ptrResearchesStruct;
	if (!rinfo) { return allValidResearchesToReturn; }
	int resCount = rinfo->researchCount;
	STRUCT_PLAYER_RESEARCH_STATUS *statuses = rinfo->researchStatusesArray;
	STRUCT_RESEARCH_DEF_INFO *resInfoArray = rinfo->ptrResearchDefInfo;
	if (!resInfoArray) { return allValidResearchesToReturn; }
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

								//(allValidResearchesToReturn.count(requiredResearchId) == 0)) {
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

