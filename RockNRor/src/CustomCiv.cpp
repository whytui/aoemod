#include "../include/CustomCiv.h"


// \r is required to display correctly in ROR editable text components
#define NEWLINE "\r\n"


namespace ROCKNROR {
	namespace CUSTOMCIV {
;


// Initialize each player data for current (or new) game
// Sets various info such as civId, techTreeId, isActive
void CustomCivHandler::InitForCurrentGame() {
	this->ResetPlayers();
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) {
		return;
	}
	for (int playerId = 1; playerId < global->playerTotalCount; playerId++) {
		STRUCT_PLAYER *player = global->GetPlayerStruct(playerId);
		if (!player || !player->IsCheckSumValid()) { continue; }
		this->InitValidPlayer(playerId, player->civilizationId, player->techTreeId);
	}
}


// Init data for standard games (using standard civs/tech tree)
bool CustomCivHandler::CreateInternalDataForGameWithStandardCivs() {
	this->InitForCurrentGame();

	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return false; }

	for (int playerId = 1; playerId < global->playerTotalCount; playerId++) {
		ROCKNROR::STRATEGY::CustomPlayerInfo *pi = this->GetCustomPlayerInfo(playerId);
		if (pi) {
			pi->CollectInfoFromExistingTechTree();
		}
	}
	return true;
}


// Init data and create tech tree for custom-civ games
bool CustomCivHandler::CreateFakeRandomCivsForAllPlayers() {
	this->InitForCurrentGame();

	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!settings || !settings->IsCheckSumValid() || !global || !global->IsCheckSumValid()) {
		return false;
	}
	if (!global->technologiesInfo) { return false; }
	int playerCount = global->playerTotalCount - 1; // do not include gaia here.
	
	// Indicate to RockNRor that all hardcoded civ bonuses must be disabled for current game (eg macedonian conversion resistance).
	crInfo.myGameObjects.doNotApplyHardcodedCivBonus = true;

	long int initialTechDefCount = ROCKNROR::crInfo.empiresDatTechDefCount;
	if (initialTechDefCount <= 0) { return false; } // maybe empires.dat has not been read yet.
	AOE_STRUCTURES::ReallocTechDefArray(initialTechDefCount + 8);
	ROCKNROR::crInfo.techTreeAnalyzer.RefreshTechDefPointers(); // Replace obsolete pointers by correct ones

	for (int playerId = 1; playerId <= playerCount; playerId++) {
		int techTreeIndex = initialTechDefCount + playerId - 1;
		// Clean previous effects
		CleanEffectsForTechDef(global->technologiesInfo->GetTechDef(techTreeIndex));
		STRUCT_PLAYER *player = global->GetPlayerStruct(playerId);
		if (!player || !player->IsCheckSumValid()) { continue; }
		player->techTreeId = techTreeIndex; // used at game init to apply civilization bonus (malus), then not really used.
		this->playersInfo[playerId].myTechTreeId = techTreeIndex;

		STRUCT_TECH_DEF *techDef = global->technologiesInfo->GetTechDef(techTreeIndex);
		ROCKNROR::STRATEGY::TechTreeCreator ttc;
		ttc.CreateRandomTechTree(techDef);
		this->lastGenerationSummary += "Pl#";
		this->lastGenerationSummary += std::to_string(playerId);
		this->lastGenerationSummary += " (";
		this->lastGenerationSummary += player->playerName_length16max;
		this->lastGenerationSummary += ")" NEWLINE "Unavailable units:" NEWLINE;
		this->lastGenerationSummary += ttc.GetDisabledUnitsText();
		this->lastGenerationSummary += "Unavailable researches:" NEWLINE;
		this->lastGenerationSummary += ttc.GetDisabledResearchesText();
		this->lastGenerationSummary += "Civilization bonuses:" NEWLINE;
		this->lastGenerationSummary += ttc.GetCivBonusText();
		this->lastGenerationSummary += NEWLINE;
	}

	return true;
}


void CustomCivHandler::WriteSummaryToScenarioInstructions() {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return; }

	char **ptrToUse = &global->scenarioInformation->scenarioTips;
	assert(ptrToUse != NULL);

	if (global->scenarioInformation && global->scenarioInformation->IsCheckSumValidForAScenarioInfoClass()) {
		std::string instructions = "";
		if (*ptrToUse) {
			instructions = std::string(*ptrToUse);
			instructions.append(NEWLINE NEWLINE);
			AOEFree(*ptrToUse);
			*ptrToUse = NULL;
		}
		instructions.append(this->lastGenerationSummary);
		long int size = instructions.size() + 1;
		*ptrToUse = (char*)AOEAlloc(size);
		(*ptrToUse)[size - 1] = 0;
		strcpy_s(*ptrToUse, size, instructions.c_str());
	}
}


}
}
