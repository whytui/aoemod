
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
		unsigned long int unknown_10;
		float value;
		STRUCT_SCORE_ELEM *next;
		unsigned long int unknown_1C; // unused ?
	};


#define SCORE_HEADER_CHECKSUM 0x0054A3E4
	// Unknown size
	class STRUCT_SCORE_HEADER {
	public:
		unsigned long int checksum; // E4 A3 54 00
		unsigned long int unknown_04; // a pointer
		unsigned long int unknown_08; // a number ?
		STRUCT_PLAYER *player;
		// 0x10
		unsigned long int unknown_10;
		STRUCT_SCORE_ELEM *firstScoreElement;
		unsigned long int unknown_18;
		long int currentTotalScore;

		bool IsCheckSumValid() { return this->checksum == SCORE_HEADER_CHECKSUM; }
	};

}
