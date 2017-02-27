#pragma once
#include <assert.h>
#include <string.h>
#include <list>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_def.h>
#include <AOE_const_functional.h>
#include "RockNRorCommon.h"
#include "RockNRorInfo.h"
#include "researches.h"
#include "AOE_strategy.h"
#include "playerHandling.h"
#include "playerTargeting.h"
#include "customAIMilitaryInfo.h"

using namespace AOE_CONST_FUNC;
using namespace AOE_STRUCTURES::PLAYER;


namespace CUSTOM_AI {

	namespace AI_CONST {
		const unsigned long int delayToRetryFailedConstruction = 5000; // Base delay in milliseconds after which a failed construction (destroyed by some enemy before it is finished) is tried again
	}
}



// Common function for panic mode unit searching.
// Returns true if it is possible to train the unit. In such case, cost is decreased from remainingResources and actorCounter is decreased too.
// Returns false and does not change values if it is not possible (warning: tempCost can be modified in all cases)
bool PrepareUnitToAddIfPossible(AOE_STRUCTURES::STRUCT_PLAYER *player, short int unitId_toAdd, short int unitId_actor,
	long int *actorCounter, short int *lastCostDAT_ID, float remainingResources[], float tempCost[]);



namespace STRATEGY {

	// Class that handles in-game strategy handling
	class StrategyUpdater {
	public:
		StrategyUpdater() {
			this->ResetAllInfo();
		}
		
		void ResetAllInfo() {
			this->inProgressBuildingLastFailuresWithGameTime.clear();
		}

		// Returns game time (milliseconds) when a building was last destroyed while "in progress" in strategy, referred by its elemId in strategy.
		// Returns -1 if no information was found (building has not been destroyed while "in progress").
		long int GetLastInProgressFailureGameTimeForBldStratElem(long int elemId) {
			for each (std::pair<long int, long int> info in this->inProgressBuildingLastFailuresWithGameTime)
			{
				if (info.first == elemId) { return info.second; }
			}
			return -1;
		}

		// Add an entry in internal list of last times when buildings were destroyed while "in progress".
		// Previous information for elemId is overwritten (delete if existing + add)
		void AddLastInProgressFailureInfoForStratElem(long int elemId, long int gameTime_ms) {
			this->RemoveLastInProgressFailureInfoForStratElem(elemId);
			this->inProgressBuildingLastFailuresWithGameTime.push_back(std::pair<long int, long int>(elemId, gameTime_ms));
		}

		// Removes information about "building was destroyed while in progress" for the specified strategy element (elemId)
		void RemoveLastInProgressFailureInfoForStratElem(long int elemId) {
			auto it = remove_if(this->inProgressBuildingLastFailuresWithGameTime.begin(), this->inProgressBuildingLastFailuresWithGameTime.end(), 
				[elemId](std::pair<long int, long int> info) {return info.first == elemId; });
			this->inProgressBuildingLastFailuresWithGameTime.erase(it, this->inProgressBuildingLastFailuresWithGameTime.end());
		}
	private:
		std::list<std::pair<long int, long int>> inProgressBuildingLastFailuresWithGameTime; // first=stratElem.elemId, second=failureGameTime_ms

	};

	extern StrategyUpdater strategyUpdater[9];

	// This is called from buildAI.resetStratElemForUnitId, when testing is "elem.alive" field.
	// The fixes in this method are "technical" and are always applied (even if improve AI is disabled)
	void CheckStratElemAliveForReset(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *currentElement);

	// Analyze strategy and fixes what's necessary. Called every <crInfo.configInfo.tacticalAIUpdateDelay> seconds.
	void AnalyzeStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI);


	// Manage strategy updates for panic mode (AI player is being attacked and seems not to have enough defences).
	// This method is only called if an attack occurs in (near) "my" town, and if "panic mode delay" has passed since last execution.
	// This strategy is not supposed to impact combat or anything else than strategy (at least in standard game !)
	void ManagePanicMode(AOE_STRUCTURES::STRUCT_AI *mainAI, long int enemyPlayerId, CUSTOM_AI::CustomAIMilitaryInfo *militaryAIInfo);


	// Returns true if a construction should NOT be triggered.
	// Default result is false, this returns true only for specific (custom) cases.
	bool ShouldNotTriggerConstruction(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem);

}
