#include "../include/researches.h"

using namespace ROR_STRUCTURES_10C;
using namespace AOE_CONST_FUNC;


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
			int minRequiredResearches = resDef->minRequiredResearches;
			int reqResOK = 0;
			int reqResImpossible = 0;

			if ((resDef->requiredResearchId1 == -1) ||
				(std::count(disabledResearches.begin(), disabledResearches.end(), resDef->requiredResearchId1) > 0)) {
				reqResImpossible++;
			}
			if ((resDef->requiredResearchId2 == -1) ||
				(std::count(disabledResearches.begin(), disabledResearches.end(), resDef->requiredResearchId2) > 0)) {
				reqResImpossible++;
			}
			if ((resDef->requiredResearchId3 == -1) ||
				(std::count(disabledResearches.begin(), disabledResearches.end(), resDef->requiredResearchId3) > 0)) {
				reqResImpossible++;
			}
			if ((resDef->requiredResearchId4 == -1) ||
				(std::count(disabledResearches.begin(), disabledResearches.end(), resDef->requiredResearchId4) > 0)) {
				reqResImpossible++;
			}
			if (std::count(availableResearches.begin(), availableResearches.end(), resDef->requiredResearchId1) > 0) {
				reqResOK++;
			}
			if (std::count(availableResearches.begin(), availableResearches.end(), resDef->requiredResearchId2) > 0) {
				reqResOK++;
			}
			if (std::count(availableResearches.begin(), availableResearches.end(), resDef->requiredResearchId3) > 0) {
				reqResOK++;
			}
			if (std::count(availableResearches.begin(), availableResearches.end(), resDef->requiredResearchId4) > 0) {
				reqResOK++;
			}
			if (reqResOK >= minRequiredResearches) {
				availableResearches.push_back(curResId);
				potentialToRemove.push_back(curResId);
			}
			if (4 - reqResImpossible < minRequiredResearches) {
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
