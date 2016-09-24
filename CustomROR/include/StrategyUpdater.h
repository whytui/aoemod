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
#include "playerTargeting.h"


using namespace AOE_CONST_FUNC;

namespace STRATEGY {

	// Class that handles in-game strategy handling
	/*class StrategyUpdater {
	public:
		
	private:
	
	};*/

	// Analyze strategy and fixes what's necessary. Called every <crInfo.configInfo.tacticalAIUpdateDelay> seconds.
	void AnalyzeStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI);


	// Manage strategy updates for panic mode.
	void ManagePanicMode(AOE_STRUCTURES::STRUCT_AI *mainAI, long int enemyPlayerId, long int timeSinceLastPanicMode_s, long int currentGameTime_ms);


}
