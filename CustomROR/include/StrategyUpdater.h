#pragma once
#include <assert.h>
#include <string.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_def.h>
#include <AOE_const_functional.h>
#include "crCommon.h"
#include "CustomRORInfo.h"
#include "researches.h"
#include "AOE_strategy.h"
//#include "customFuncRules.h"
#include "playerHandling.h"
#include "playerTargeting.h"


using namespace AOE_CONST_FUNC;
using namespace PLAYER;

namespace STRATEGY {

	// Class that handles in-game strategy handling
	/*class StrategyUpdater {
	public:
		
	private:
	
	};*/

	// Analyze strategy and fixes what's necessary. Called every <crInfo.configInfo.tacticalAIUpdateDelay> seconds.
	void AnalyzeStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI);


	// Manage strategy updates for panic mode (AI player is being attacked and seems not to have enough defences).
	// This method can be called very often (no delay): standard game uses a minimum delay between 2 panic mode evaluations, NOT US !
	// This strategy is not supposed to impact combat or anything else than strategy (at least in standard game !)
	void ManagePanicMode(AOE_STRUCTURES::STRUCT_AI *mainAI, long int enemyPlayerId, long int timeSinceLastPanicMode_s, long int currentGameTime_ms);


	// Returns true if a construction should NOT be triggered.
	// Default result is false, this returns true only for specific (custom) cases.
	bool ShouldNotTriggerConstruction(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem);

}
