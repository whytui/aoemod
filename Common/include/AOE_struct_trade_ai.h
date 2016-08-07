
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


	// Size 0xF4 - constructor=0x40ED50
	class STRUCT_TRADE_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1010
		// 0xF0
		STRUCT_AI *ptrMainAI;
		bool IsCheckSumValid() { return this->checksum == 0x00542CD0; }
	};
	static_assert(sizeof(STRUCT_TRADE_AI) == 0xF4, "STRUCT_TRADE_AI size");


}
