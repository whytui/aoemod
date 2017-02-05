#pragma once
#include <string>
#include <AOE_const_functional.h>
#include <AOE_struct_player.h>
#include <AOE_struct_main_ai.h>
#include "CustomRORInfo.h"
#include "StrategyBuilder.h"


using namespace AOE_STRUCTURES;

namespace CUSTOMROR {
namespace PLAYER {
;

// Set a SN number value in both strategy AI and tac AI.
void SetSNNumberInStrategyAndTacAI(AOE_STRUCTURES::STRUCT_AI *ai, AOE_CONST_FUNC::SN_NUMBERS snNumber, long int value);


// Applies customization on SN numbers for specified player (at game start), according to configuration.
void ApplySNNumberCustomizationOnPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player);


// Generates a dynamic strategy for the specified player, if configuration allows it.
void ApplyStrategyGenerationOnPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player);


}

}
