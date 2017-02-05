#include "../include/PlayerCustomMethods.h"

namespace CUSTOMROR {
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
			if (CUSTOMROR::crInfo.configInfo.defaultPerNumbers_DM_isSet[i]) {
				CUSTOMROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, (AOE_CONST_FUNC::SN_NUMBERS)i,
					CUSTOMROR::crInfo.configInfo.defaultPerNumbers_DM[i]);
			}
		}
	} else {
		// Random map
		for (int i = 0; i <= AOE_CONST_FUNC::CST_LAST_SN_NUMBER; i++) {
			if (CUSTOMROR::crInfo.configInfo.defaultPerNumbers_RM_isSet[i]) {
				CUSTOMROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, (AOE_CONST_FUNC::SN_NUMBERS)i,
					CUSTOMROR::crInfo.configInfo.defaultPerNumbers_RM[i]);
			}
		}
	}

	// Manage the 4 "hardest difficulty" SN numbers
	if (settings->difficultyLevelChoice == 0) {
		CUSTOMROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialFood,
			isDM ? CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_FOOD] :
			CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_FOOD]);
		CUSTOMROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialWood,
			isDM ? CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_WOOD] :
			CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_WOOD]);
		CUSTOMROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialStone,
			isDM ? CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_STONE] :
			CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_STONE]);
		CUSTOMROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialGold,
			isDM ? CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_DM[RESOURCE_TYPES::CST_RES_ORDER_GOLD] :
			CUSTOMROR::crInfo.configInfo.initialResourceHardestAIBonus_RM[RESOURCE_TYPES::CST_RES_ORDER_GOLD]);
	} else {
		// Not hardest : disable the bonus for AI
		CUSTOMROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialFood, 0);
		CUSTOMROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialWood, 0);
		CUSTOMROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialStone, 0);
		CUSTOMROR::PLAYER::SetSNNumberInStrategyAndTacAI(ai, SNInitialGold, 0);
	}

}


// Generates a dynamic strategy for the specified player, if configuration allows it.
void ApplyStrategyGenerationOnPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid() || !player->ptrAIStruct || !player->ptrAIStruct->IsCheckSumValid()) {
		return;
	}
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	bool isDM = (settings->isDeathMatch != 0);
	if ((!isDM && CUSTOMROR::crInfo.configInfo.generateStrategyForRM) ||
		(isDM && CUSTOMROR::crInfo.configInfo.generateStrategyForDM)) {
		if (isDM) {
			traceMessageHandler.WriteMessage("Strategy generation for deathmatch games is not supported yet");
		} else {
			STRATEGY::StrategyBuilder sb = STRATEGY::StrategyBuilder(&CUSTOMROR::crInfo, player);
			sb.CreateStrategyFromScratch();
		}
	}
}


}
}
