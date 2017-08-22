
#pragma once

#include <AOE_const_functional.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// External dependencies
	class STRUCT_PLAYER;



	// size = 0x20, to confirm
	class STRUCT_SCORE_ELEM {
	public:
		char unknown_00;
		char unknown_01;
		char unknown_02;
		AOE_CONST_FUNC::SCORE_CATEGORIES category;
		long int resourceId; // +04 ; RESOURCE_TYPES
		long int listCounter; // To confirm
		long int pointsPerScoreElement;
		// 0x10
		long int currentScoreAmount; // +10. Calculated using pointsPerScoreElement and resource value (=number to multiply)
		float value;
		STRUCT_SCORE_ELEM *next;
		unsigned long int unknown_1C; // unused ?
	};


#define CHECKSUM_VICTORY_CONDITIONS_BASE 0x0054A60C // (parent) constructor=0x50D380, 0x50D6D0
#define CHECKSUM_VICTORY_CONDITIONS 0x0054A3E4
	// Size=0x20 (both). Constructor=0x5084C0, 0x5084F0. "VictoryCondition" ??
	class STRUCT_VICTORY_CONDITIONS {
	public:
		unsigned long int checksum; // E4 A3 54 00
		unsigned long int unknown_04; // a pointer
		unsigned long int unknown_08; // a number ?
		STRUCT_PLAYER *player; // +0C.
		unsigned long int unknown_10;
		STRUCT_SCORE_ELEM *firstScoreElement;
		unsigned long int unknown_18;
		long int currentTotalScore;

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_VICTORY_CONDITIONS_BASE) || (this->checksum == CHECKSUM_VICTORY_CONDITIONS); }
	};
	static_assert(sizeof(STRUCT_VICTORY_CONDITIONS) == 0x20, "STRUCT_VICTORY_CONDITIONS size");

}
