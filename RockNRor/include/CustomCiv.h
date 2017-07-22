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

#pragma once


namespace ROCKNROR {
	namespace CUSTOMCIV {
;


class CustomPlayerInfo {
public:
	CustomPlayerInfo() {
		this->ResetAndInit(-1, -1, -1);
	}
	~CustomPlayerInfo() {}

	long int myPlayerId;
	long int myCivId;
	long int myTechTreeId;
	bool isActive; // True if player is active/valid for current game
	std::set<ROCKNROR::STRATEGY::TTDetailedBuildingDef*>enabledBuildingsInfo;
	std::set<ROCKNROR::STRATEGY::TTDetailedBuildingDef*>disabledBuildingsInfo; // techtree BuildingDef info for non-available buildings. Most commonly disabled buildings are walls, sometimes temple, academy, ballista tower... (+hero buildings)
	std::set<ROCKNROR::STRATEGY::TTDetailedUnitDef*>enabledTrainableUnitInfos;
	std::set<ROCKNROR::STRATEGY::TTDetailedUnitDef*>disabledTrainableUnitInfos;
	std::set<ROCKNROR::STRATEGY::TTDetailedResearchDef*>enabledResearchesInfoAll; // All non-disabled (by tech tree) researches, including shadow researches
	std::set<ROCKNROR::STRATEGY::TTDetailedResearchDef*>enabledResearchesInfo; // All non-disabled (by tech tree) researches, excluding shadow researches
	std::set<ROCKNROR::STRATEGY::TTDetailedResearchDef*>disabledResearchesInfo;

	// Reset internal data and re-init.
	void ResetAndInit(long int playerId, long int civId, long int techTreeId) {
		this->myCivId = civId;
		this->myTechTreeId = techTreeId;
		this->myPlayerId = playerId;
		this->isActive = false;
		this->enabledBuildingsInfo.clear();
		this->enabledTrainableUnitInfos.clear();
		this->disabledBuildingsInfo.clear();
		this->disabledTrainableUnitInfos.clear();
		this->enabledResearchesInfo.clear();
		this->disabledResearchesInfo.clear();
	}

	void CollectInfoFromExistingTechTree();
};


class CustomCivHandler {
public:
	CustomCivHandler() {
		this->ResetPlayers();
	}

	CustomPlayerInfo playersInfo[9];

	bool CreateFakeRandomCivsForAllPlayers();


	// Safely get a player info object
	CustomPlayerInfo *GetCustomPlayerInfo(long int playerId) {
		if ((playerId < 0) || (playerId > 8)) { return NULL; }
		return &this->playersInfo[playerId];
	}

	// Reset all players data
	void ResetPlayers() {
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
};



}
}
