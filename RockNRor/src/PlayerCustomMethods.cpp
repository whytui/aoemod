#include "../include/PlayerCustomMethods.h"

namespace ROCKNROR {
	namespace PLAYER {
;



// Set a SN number value in both strategy AI and tac AI.
void SetSNNumberInStrategyAndTacAI(AOE_STRUCTURES::STRUCT_AI *ai, AOE_CONST_FUNC::SN_NUMBERS snNumber, long int value) {
	if (!ai || !ai->IsCheckSumValid()) { return; }
	assert((snNumber >= 0) && (snNumber <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER));
	if ((snNumber < 0) || (snNumber > AOE_CONST_FUNC::CST_LAST_SN_NUMBER)) { return; }
	ai->structStrategyAI.SNNumber[snNumber] = value;
	ai->structTacAI.SNNumber[snNumber] = value;
}


// Applies customization on SN numbers for specified player (at game start), according to configuration.
void ApplySNNumberCustomizationOnPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid() || !player->ptrAIStruct || !player->ptrAIStruct->IsCheckSumValid()) {
		return;
	}
	AOE_STRUCTURES::STRUCT_AI *ai = player->ptrAIStruct;
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	bool isDM = (settings->isDeathMatch != 0);

	if (isDM) {
		// Deathmatch
		for (int i = 0; i <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER; i++) {
			if (ROCKNROR::crInfo.configInfo.defaultPerNumbers_DM_isSet[i]) {
				ROCKNROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, (AOE_CONST_FUNC::SN_NUMBERS)i,
					ROCKNROR::crInfo.configInfo.defaultPerNumbers_DM[i]);
			}
		}
	} else {
		// Random map
		for (int i = 0; i <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER; i++) {
			if (ROCKNROR::crInfo.configInfo.defaultPerNumbers_RM_isSet[i]) {
				ROCKNROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, (AOE_CONST_FUNC::SN_NUMBERS)i,
					ROCKNROR::crInfo.configInfo.defaultPerNumbers_RM[i]);
			}
		}
	}

	// Manage the 4 "hardest difficulty" SN numbers
	if (settings->difficultyLevelChoice == 0) {
		ROCKNROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialFood,
			isDM ? ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_FOOD] :
			ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_FOOD]);
		ROCKNROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialWood,
			isDM ? ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_WOOD] :
			ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_WOOD]);
		ROCKNROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialStone,
			isDM ? ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_STONE] :
			ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_STONE]);
		ROCKNROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialGold,
			isDM ? ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_GOLD] :
			ROCKNROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
	} else {
		// Not hardest : disable the bonus for AI
		ROCKNROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialFood, 0);
		ROCKNROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialWood, 0);
		ROCKNROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialStone, 0);
		ROCKNROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialGold, 0);
	}

}


// Generates a dynamic strategy for the specified player, if configuration allows it.
void ApplyStrategyGenerationOnPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid() || !player->ptrAIStruct || !player->ptrAIStruct->IsCheckSumValid()) {
		return;
	}
	if (ROCKNROR::crInfo.configInfo.enableTestCompareAI) {
		// For "AI comparison" mode, consider strategy generation is an AI improvement.
		if (!IsImproveAIEnabled(player->playerId)) { return; }
	}
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	if (settings->isCampaign || settings->rgeGameOptions.isScenario && settings->ptrGlobalStruct) {
		// Check if scenario provides an explicit strategy
		AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = settings->ptrGlobalStruct->scenarioInformation;
		long int playerIndex = player->playerId - 1;
		if (scInfo->strategyFileSize[playerIndex] != 0) {
			std::string msg = "Player #";
			msg += std::to_string(player->playerId);
			msg += " has an explicit strategy from scenario information.";
			traceMessageHandler.WriteMessageNoNotification(msg.c_str());
			return;
		}
	}
	bool isDM = (settings->isDeathMatch != 0);
	if ((!isDM && ROCKNROR::crInfo.configInfo.generateStrategyForRM) ||
		(isDM && ROCKNROR::crInfo.configInfo.generateStrategyForDM)) {
		if (isDM) {
			traceMessageHandler.WriteMessage("Strategy generation for deathmatch games is not supported yet");
		} else {
			STRATEGY::StrategyBuilder sb = STRATEGY::StrategyBuilder(&ROCKNROR::crInfo, player);
			sb.CreateStrategyFromScratch();
		}
	}
}


}
}
