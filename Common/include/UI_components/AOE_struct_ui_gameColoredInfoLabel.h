#include <UI_components\AOE_struct_any_ui.h>
#include <AOE_struct_game_global.h>
#include <AOE_struct_player.h>

#pragma once

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	enum GAME_COLORED_INFO_LABEL_INFO_TYPE : long int {
		ILT_NONE = 0, // Hidden : no information to show
		ILT_UP_TIME_AND_SPEED = 1, // Show absolute time (EXE up time, it seems). For top-left corner ?
		ILT_GAME_TIME_AND_SPEED = 2, // Show game time and speed (top-left corner, F11 key)
		ILT_GENERAL_REMAINING_TIME_AMOUNT = 3, // Show remaining time amount for "time limit" victory condition (for top-right lines)
		ILT_WONDER_VICTORY_DELAY = 4, // Show remaining years till a wonder generates victory (for top-right lines)
		ILT_ALL_RELICS = 5, // Show remaining years till a wonder generates victory (for top-right lines)
		ILT_ALL_RUINS = 6 // Show remaining years till a wonder generates victory (for top-right lines)
		// No more than 6.
	};


#define CHECKSUM_GAME_COLORED_INFO_LABEL 0x0054A050
	// "TribePanelTime". Size=0x174. Constructor=0x4FAC10 timerInfoGameLabel.constructor(arg1=parentUI.unknown_020, gameUIZone)
	// Destructor = 0x4FAD30
	// +0x50: SetPosition(x, y) ?
	class STRUCT_GAME_COLORED_INFO_LABEL : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4;
		unsigned long int unknown_0F8;
		unsigned long int unknown_0FC;
		GAME_COLORED_INFO_LABEL_INFO_TYPE currentInformationType; // +100. "clockType"
		unsigned long int unknown_104;
		long int unknown_108; // Values 2 (for info types 1,2), 1 (other)
		unsigned long int *unknown_10C;
		unsigned long int unknown_110;
		STRUCT_GAME_GLOBAL *globalStruct; // +114
		STRUCT_PLAYER *currentPlayer; // +118. Can be gaia (game time/speed info) or a real player (victory timer info)
		long int lastUpdateTimeGetTimeValue; // +11C
		long int newTimeInformationValue; // +120. Can represent game time (type2), number of remaining years (types 4, 5,6)
		long int previousTimeInformationValue; // +124. Can represent game time (type2), number of remaining years (types 4, 5,6)
		char unknown_128[0x174 - 0x128];

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_GAME_COLORED_INFO_LABEL); }

		// Set the type of information this colored info label should display.
		// Use ILT_NONE to hide it.
		// PlayerId impacts the color, you are allowed to use 0 (white for gaia/neutral).
		void SetInfoType(GAME_COLORED_INFO_LABEL_INFO_TYPE infoType, long int playerId) {
			const unsigned long int addr = 0x4FAF80;
			assert((infoType >= 0) && (infoType <= ILT_ALL_RUINS));
			assert((playerId >= 0) && (playerId <= 8));
			if ((playerId < 0) || (playerId > 8)) { return; }
			if (this->globalStruct) {
				assert(this->globalStruct->IsCheckSumValid());
				assert(playerId < this->globalStruct->playerTotalCount);
				if (playerId >= this->globalStruct->playerTotalCount) { return; }
			}
			_asm {
				MOV ECX, this;
				PUSH playerId;
				PUSH infoType;
				CALL addr;
			}
		}
	};
	static_assert(sizeof(STRUCT_GAME_COLORED_INFO_LABEL) == 0x174, "STRUCT_GAME_COLORED_INFO_LABEL size");

}

