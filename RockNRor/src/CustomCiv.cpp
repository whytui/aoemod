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
bool CustomCivHandler::CreateInternalDataForGameWithStandardCivs(bool resetInternalData) {
	if (resetInternalData) {
		this->InitForCurrentGame();
	}

	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return false; }

	for (int playerId = 1; playerId < global->playerTotalCount; playerId++) {
		ROCKNROR::STRATEGY::CustomPlayerInfo *pi = this->GetCustomPlayerInfo(playerId);
		if (pi) {
			pi->CollectInfoFromExistingTechTree();
			pi->MarkCivBonusUnitNames();
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


	// Run simulations and collect statistics for a better evaluation of tech tree/bonus weight => try to get fair tech trees/bonuses.
	long int timeBegin = AOE_METHODS::TimeGetTime();
	double cumulatedWeight = 0;
	const int simulationsCount = ROCKNROR::crInfo.configInfo.randomTechTreeSimulationCount;
	// Run a few "fake" civilization techtree/bonus generation to get statistics.
	for (int i = 0; i < simulationsCount; i++) {
		int playerId = 1;
		int techTreeIndex = initialTechDefCount + playerId - 1;
		CleanEffectsForTechDef(global->technologiesInfo->GetTechDef(techTreeIndex));

		STRUCT_TECH_DEF *techDef = global->technologiesInfo->GetTechDef(techTreeIndex);
		ROCKNROR::STRATEGY::TechTreeCreator ttc(true);
		ttc.CreateRandomTechTree(techDef);
		double calculatedWeight = ttc.totalDisableWeight;
		cumulatedWeight += calculatedWeight;
		// Cancel changes
		CleanEffectsForTechDef(global->technologiesInfo->GetTechDef(techTreeIndex));
		ttc.Reset();
	}
	double finalAverageDisableWeight = cumulatedWeight / simulationsCount;
	long int timeEnd = AOE_METHODS::TimeGetTime();
	long int timeSpent = timeEnd - timeBegin;
#ifdef _DEBUG
	this->lastGenerationSummary += "Simulations/stats: time taken=";
	this->lastGenerationSummary += std::to_string(timeSpent);
	this->lastGenerationSummary += " ms. ExpectedWeight=";
	this->lastGenerationSummary += std::to_string(finalAverageDisableWeight);
	this->lastGenerationSummary += "." NEWLINE;
#endif
	// End of simulations/statistics


	for (int playerId = 1; playerId <= playerCount; playerId++) {
		int techTreeIndex = initialTechDefCount + playerId - 1;
		// Clean previous effects
		CleanEffectsForTechDef(global->technologiesInfo->GetTechDef(techTreeIndex));
		STRUCT_PLAYER *player = global->GetPlayerStruct(playerId);
		if (!player || !player->IsCheckSumValid()) { continue; }
		player->techTreeId = techTreeIndex; // used at game init to apply civilization bonus (malus), then not really used.
		assert(this->GetCustomPlayerInfo(playerId) != NULL);
		this->GetCustomPlayerInfo(playerId)->myTechTreeId = techTreeIndex;

		// Make sure Macedonian no longer receives conversion resistance !
		if ((ROCKNROR::crInfo.configInfo.conversionResistanceAttackClassEnabled) && (player->civilizationId == CIVILIZATIONS::CST_CIVID_MACEDONIAN)) {
			this->ForceDefaultConversionResistanceArmorValue(player);
		}

		STRUCT_TECH_DEF *techDef = global->technologiesInfo->GetTechDef(techTreeIndex);
		ROCKNROR::STRATEGY::TechTreeCreator ttc(finalAverageDisableWeight);
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
		if ((global->humanPlayerId >= 0) && (global->humanPlayerId == playerId)) {
			this->lastGenerationBonusLinesForHumanPlayer = ttc.GetHumanBonusTextLines();
		}

		// Warning : GetCustomPlayerInfo(playerId) has NOT analyzed the tech tree yet

		this->GetCustomPlayerInfo(playerId)->MarkCivBonusUnitNames();

		const std::map<GLOBAL_UNIT_AI_TYPES, std::string> bonusTextByClass = ttc.GetBonusTextByClassMap();
		this->GetCustomPlayerInfo(playerId)->AddCivBonusUnitCustomText(bonusTextByClass);
		const std::map<GLOBAL_UNIT_AI_TYPES, std::string> nonTransmissibleBonusTextByClass = ttc.GetNonTransmissibleBonusesText();
		this->GetCustomPlayerInfo(playerId)->AddNonTransmissibleCivBonusCustomText(nonTransmissibleBonusTextByClass);
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


// Force conversion resistance using armor class specified in configuration (if enabled),
// For all units that do NOT already possess a value for that armor class
// The conversion resistance value used is game default (priests=2, chariot=8, etc), ignoring Macedonian "rule"
// The goal is to "force cancel" Macedonian hardcoded bonus.
// This has a crucial important for saved games : at loading time, we wouldn't be able to know if this was a game with or withOUT Mace. bonus.
void CustomCivHandler::ForceDefaultConversionResistanceArmorValue(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) { return; }
	if (!ROCKNROR::crInfo.configInfo.conversionResistanceAttackClassEnabled) { return; }
	int unitDefCount = player->structDefUnitArraySize;
	if (ROCKNROR::crInfo.configInfo.conversionResistanceAttackClass < 0) { return; }
	AOE_CONST_FUNC::ATTACK_CLASS armorClass = (AOE_CONST_FUNC::ATTACK_CLASS)ROCKNROR::crInfo.configInfo.conversionResistanceAttackClass;
	if (armorClass < 0) { return; }

	for (int unitDefId = 0; unitDefId < unitDefCount; unitDefId++) {
		AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)player->GetUnitDefBase(unitDefId);
		if ((unitDef == NULL) || (!unitDef->DerivesFromAttackable())) {
			continue;
		}

		short int armorRawValue = GetArmorFromList(unitDef, armorClass, -1);
		if (armorRawValue >= 0) {
			// There already IS an armor defined for conversion resistance. Keep it as is.
			continue;
		}
		// No armor value for conversion resistance. Set it using game default.
		int fakeCivIdToGetDefault = -1; // NOT macedonian, so that we get game default for that unit.
		float gameDefaultValue = ROCKNROR::crInfo.GetConversionResistance(fakeCivIdToGetDefault, unitDef);
		armorRawValue = (short int)(gameDefaultValue * 100);
		SetArmorInList(unitDef, armorClass, armorRawValue, true);
	}
}


}
}
