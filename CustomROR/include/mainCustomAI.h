#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include <AOE_struct_main_ai.h>
#include "CustomRORInfo.h"
#include "customAIMilitaryInfo.h"
#include "StrategyUpdater.h"
#include "unitTargeting.h"
#include "playerTargeting.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {


	class CustomPlayerAI {
	public:
		CustomPlayerAI();

		long int myPlayerId;
		bool isPlayerAlive; // False when player is defeated
		STRUCT_AI *mainAI; // ROR main AI structure
		STRUCT_PLAYER *myPlayer; // ROR player structure

		CustomAIMilitaryInfo militaryAI;
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

		// Triggered each time an AI player's unit is attacked (possibly by a gaia unit)
		// rorOriginalPanicModMethodHasBeenRun indicates if ROR's panic mode method has been run. If so, panic mode treatments should not be run here.
		void OnUnitAttacked(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *myUnit,
			AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyUnit, bool rorOriginalPanicModeMethodHasBeenRun);

	private:
		bool isValidPlayer;
	};


	// Global object that handles all CustomROR-related AI structures and treatments
	class CustomAIHandler {
	public:
		CustomAIHandler();

		long int currentGameTotalPlayerCount;
		CustomPlayerAI playerAITable[9];

		// To be executed each time a game starts/is loaded
		// This method is in charge of resetting all AI-related CustomROR structures
		void ResetAllInfo();

		// To be executed at game start. Required for all other treatments to work.
		void GameStartInit();

		// Returns true if playerId is valid, and if corresponding player has a valid (custom) AI and is still alive
		bool IsAliveAI(long int playerId) {
			return (playerId > 0) && (playerId < 9) && (this->playerAITable[playerId].IsValidAIPlayer()) && 
				(this->playerAITable[playerId].isPlayerAlive);
		}

		// Triggered each time an AI player's unit is attacked (possibly by a gaia unit)
		// rorOriginalPanicModMethodHasBeenRun indicates if ROR's panic mode method has been run. If so, panic mode treatments should not be run here.
		void OnUnitAttacked(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *myUnit, 
			AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyUnit, bool rorOriginalPanicModeMethodHasBeenRun);

		CustomPlayerAI *GetCustomPlayerAI(long int playerId) {
			assert(playerId > 0);
			assert(playerId < 9);
			return &this->playerAITable[playerId];
		}
	};

	// Handler for all players' customROR AI
	extern CustomAIHandler customAIHandler;
	
}
