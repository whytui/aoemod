#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_main_ai.h>
#include "CustomRORInfo.h"
#include "StrategyUpdater.h"


using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	class CustomPlayerAI {
	public:
		CustomPlayerAI();

		long int myPlayerId;
		bool isPlayerAlive; // False when player is defeated
		STRUCT_AI *mainAI; // ROR main AI structure
		STRUCT_PLAYER *myPlayer; // ROR player structure

		long int lastStrategyAnalysisTime; // Last time (milliseconds) strategy was analyzed by customROR

		// Reset all information (game (re)start, etc)
		void Reset();

		// Game start init
		void Init(STRUCT_GAME_GLOBAL *global, long int playerId);

		// Returns true if player is enabled, alive, has AI structure
		bool IsValidAIPlayer();

		// Functional treatments
		// Runs strategy update if needed.
		// Returns true if treatements were executed, false if not (not enough time has passed since last execution)
		bool RunStrategyUpdate(long int currentGameTime);

		// One-shot strategy analysis, only run at game beginning. Not even re-run when loading a saved game.
		void RunInitialStrategyAnalysis();

	private:
		bool isValidPlayer;
	};

	class CustomAIHandler {
	public:
		CustomAIHandler();

		long int currentGameTotalPlayerCount;
		CustomPlayerAI playerAITable[9];

		// To be executed each time a game starts/is loaded
		void ResetAllInfo();

		// To be executed at game start. Required for all other treatments to work.
		void GameStartInit();

		// Returns true if playerId is valid, and if corresponding player has a valid (custom) AI and is still alive
		bool IsAliveAI(long int playerId) {
			return (playerId > 0) && (playerId < 9) && (this->playerAITable[playerId].IsValidAIPlayer()) && 
				(this->playerAITable[playerId].isPlayerAlive);
		}
		CustomPlayerAI *GetCustomPlayerAI(long int playerId) {
			assert(playerId > 0);
			assert(playerId < 9);
			return &this->playerAITable[playerId];
		}
	};

	//extern CustomPlayerAI playerAITable[9];
	extern CustomAIHandler customAIHandler;
	
}
