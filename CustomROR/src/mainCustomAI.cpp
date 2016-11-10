#include "../include/mainCustomAI.h"


namespace CUSTOM_AI {
;

// Common AI handler
CustomAIHandler customAIHandler;


CustomPlayerAI::CustomPlayerAI() {
	this->Reset();
}


// Reset all information (game (re)start, etc)
void CustomPlayerAI::Reset() {
	this->mainAI = NULL;
	this->myPlayer = NULL;
	this->myPlayerId = -1;
	this->isValidPlayer = false;
	this->isPlayerAlive = false;
	this->lastStrategyAnalysisTime = 0;
}


// Game start init
void CustomPlayerAI::Init(STRUCT_GAME_GLOBAL *global, long int playerId) {
	assert(global != NULL);
	if (!global) { return; }
	this->myPlayerId = playerId;
	this->myPlayer = global->GetPlayerStruct(playerId);
	if (this->myPlayer) {
		this->mainAI = this->myPlayer->ptrAIStruct;
		if (this->mainAI && this->mainAI->IsCheckSumValid()) {
			this->isValidPlayer = true;
		}
		this->isPlayerAlive = (myPlayer->aliveStatus == 0);
	}
	this->lastStrategyAnalysisTime = 0;
}


// Returns true if player is enabled, alive, has AI structure
bool CustomPlayerAI::IsValidAIPlayer() {
	return this->isValidPlayer && this->isPlayerAlive && (this->myPlayerId >= 0) && (this->mainAI != NULL) &&
		(this->myPlayer != NULL);
}


// Runs strategy update if needed.
// Returns true if treatements were executed, false if not (not enough time has passed since last execution)
bool CustomPlayerAI::RunStrategyUpdate(long int currentGameTime) {
	if (!this->IsValidAIPlayer()) { return false; }
	long int updateDelay_ms = CUSTOMROR::crInfo.configInfo.tacticalAIUpdateDelay * 1000;
	if (this->isValidPlayer && this->isPlayerAlive &&
		(currentGameTime - this->lastStrategyAnalysisTime > updateDelay_ms)) {
		STRATEGY::AnalyzeStrategy(&this->mainAI->structBuildAI);
		return true;
	}
	return false;
}


// One-shot strategy analysis, only run at game beginning. Not even re-run when loading a saved game.
void CustomPlayerAI::RunInitialStrategyAnalysis() {
	if (!this->IsValidAIPlayer()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *gameSettings = GetGameSettingsPtr();
	assert(gameSettings && gameSettings->IsCheckSumValid());
	if (!gameSettings || !gameSettings->IsCheckSumValid()) { return; }
	if (gameSettings->difficultyLevel <= GAME_DIFFICULTY_LEVEL::GDL_HARD) {
		// Hard/hardest: Search for techs that can be added to strategy and would improve my military units
		STRATEGY::AddUsefulMilitaryTechsToStrategy(this->myPlayer);
	}
	if (gameSettings->difficultyLevel <= GAME_DIFFICULTY_LEVEL::GDL_EASY) {
		// All levels except easiest
		STRATEGY::AdaptStrategyToMaxPopulation(this->myPlayer);
	}
}


CustomAIHandler::CustomAIHandler() {
	this->ResetAllInfo();
}

// To be executed each time a game starts/is loaded
void CustomAIHandler::ResetAllInfo() {
	this->currentGameTotalPlayerCount = 0;
	for (int i = 0; i < _countof(this->playerAITable); i++) {
		this->playerAITable[i].Reset();
	}
}


// To be executed at game start. Required for all other treatments to work.
void CustomAIHandler::GameStartInit() {
	assert(this->currentGameTotalPlayerCount == 0); // If not, ResetAllInfo has NOT been called at game init, which is bad.
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }
	this->currentGameTotalPlayerCount = global->playerTotalCount;
	for (int i = 1; i < this->currentGameTotalPlayerCount; i++) {
		this->playerAITable[i].Init(global, i);
	}

}


}

