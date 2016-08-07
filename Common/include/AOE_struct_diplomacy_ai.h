
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

#define CHECKSUM_DIPLOMACY_AI 0x00542AF4
	// size 0x150 - constructor=0x40A780
	// Checksums : F4 2A 54 00, parents=04 2C 54 00
	class STRUCT_DIPLOMACY_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1003
		// 0xF0
		unsigned long int ptrMainAI; // +F0.
		// 0xF4
		unsigned long int dislikeTable[10]; // +F4. dislike values against all players. Why is there a "player9" value ?
		//unsigned long int unknown_118;
		// 0x11C
		unsigned long int likeTable[10]; // +11C. like values against all players. Why is there a "player9" value ?
		//unsigned long int unknown_140;
		// 0x144
		char isNeutral[9]; // 0 or 1. To confirm
		char unused_14D;
		short int unused;
		// 0x150: end
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_DIPLOMACY_AI; }
	};
	static_assert(sizeof(STRUCT_DIPLOMACY_AI) == 0x150, "STRUCT_DIPLOMACY_AI size");


}
