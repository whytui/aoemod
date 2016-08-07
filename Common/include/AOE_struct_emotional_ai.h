
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
	// External dependencies
	class STRUCT_AI;

#define CHECKSUM_EMOTIONAL_AI 0x00542B34
	// size 0x1C0 - constructor=0x40AFC0. (mainAI+0xB2C)
	// Checksums : 34 2B 54 00, parents=04 2C 54 00
	class STRUCT_EMOTIONAL_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1004
		// 0xF0
		STRUCT_AI *mainAI;
		long int attitudeValues[6]; // +F4
		char attitudeNames[6][0x1E];
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_EMOTIONAL_AI; }
	};
	static_assert(sizeof(STRUCT_EMOTIONAL_AI) == 0x1C0, "STRUCT_EMOTIONAL_AI size");


}
