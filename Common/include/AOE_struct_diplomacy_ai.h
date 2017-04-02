
#pragma once

#include <AOE_struct_common_ai_object.h>


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	class STRUCT_AI;

#define CHECKSUM_DIPLOMACY_AI 0x00542AF4
	// size 0x150 - constructor=0x40A780
	// Checksums : F4 2A 54 00, parents=04 2C 54 00
	class STRUCT_DIPLOMACY_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1003
		STRUCT_AI *ptrMainAI; // +F0. Back-pointer to main AI.
		unsigned long int dislikeTable[10]; // +F4. The 0-100 dislike values against each player. Why is there a "player9" value ?
		unsigned long int likeTable[10]; // +11C. The 0-100 like values against each players. Why is there a "player9" value ?
		// 0x144
		char isNeutral[9]; // 0 or 1. To confirm
		char unused_14D; // An unused "player9" value ?
		short int unused;
		// 0x150: end
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_DIPLOMACY_AI; }
	};
	static_assert(sizeof(STRUCT_DIPLOMACY_AI) == 0x150, "STRUCT_DIPLOMACY_AI size");


}
