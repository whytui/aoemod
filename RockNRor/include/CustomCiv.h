#include <assert.h>
#include <set>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_global.h>
#include "RockNRorConfig.h"
#include "RockNRorInfo.h"
#include "AOE_memory.h"
#include "traceMessage.h"
#include "mainStructuresHandling.h"
#include "unitDefHandling.h"
#include "techDefHandling.h"
#include "PlayerTechTree.h"
#include "AOE_time.h"

#pragma once


namespace ROCKNROR {
	namespace CUSTOMCIV {
;


class CustomCivHandler {
public:
	CustomCivHandler() {
		this->ResetPlayers();
	}

	std::string lastGenerationSummary;
	std::list<std::string> lastGenerationBonusLinesForHumanPlayer;

	// Init data for standard games (using standard civs/tech tree)
	bool CreateInternalDataForGameWithStandardCivs(bool resetInternalData);

	// Init data and create tech tree for custom-civ games
	bool CreateFakeRandomCivsForAllPlayers();


	// Safely get a player info object
	ROCKNROR::STRATEGY::CustomPlayerInfo *GetCustomPlayerInfo(long int playerId) {
		if ((playerId < 0) || (playerId > 8)) { return NULL; }
		return &this->playersInfo[playerId];
	}

	// Reset all players data
	void ResetPlayers() {
		this->lastGenerationSummary.clear();
		this->lastGenerationBonusLinesForHumanPlayer.clear();
		for (int i = 0; i < 9; i++) {
			this->playersInfo[i].ResetAndInit(i, -1, -1);
		}
	}

	bool InitValidPlayer(long int playerId, long int civId, long int techTreeId) {
		if ((playerId < 0) || (playerId > 9)) { return false; }
		this->playersInfo[playerId].ResetAndInit(playerId, civId, techTreeId);
		this->playersInfo[playerId].isActive = true;
		return true;
	}

	// Initialize each player data for current (or new) game
	// Sets various info such as civId, techTreeId, isActive
	void InitForCurrentGame();

	void WriteSummaryToScenarioInstructions();

	// Force conversion resistance using armor class specified in configuration (if enabled),
	// For all units that do NOT already possess a value for that armor class
	// The conversion resistance value used is game default (priests=2, chariot=8, etc), ignoring Macedonian "rule"
	// The goal is to "force cancel" Macedonian hardcoded bonus.
	void ForceDefaultConversionResistanceArmorValue(AOE_STRUCTURES::STRUCT_PLAYER *player);

private:
	ROCKNROR::STRATEGY::CustomPlayerInfo playersInfo[9];

};



}
}
