
#pragma once

#include <AOE_struct_common_ai_object.h>
#include <AOE_struct_ai_unit_list_info.h>

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


	// size 0x5F8 - constructor=0x4CB030
	class STRUCT_STRATEGY_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1008
		// 0xF0
		STRUCT_AI *ptrMainAI;
		unsigned long int unknown_0F4;
		char tempPerFile[0x100]; // +F8
		char unknown_1F8[0x234 - 0x1F8];
		long int unknown_counter; // Updated each time strategy AI is run ? (in fact, always reset to 0)
		STRUCT_AI_UNIT_LIST_INFO unknown_238; // List of SN numbers (index) ? For debugging ?
		STRUCT_AI_UNIT_LIST_INFO unknown_248; // List of SN numbers (index) ? For debugging ?
		STRUCT_AI_UNIT_LIST_INFO unknown_258; // List of SN numbers (index) ? For debugging ?
		unsigned long int structPersonalityChecksum; // +268. 9C 8C 54 00
		long int SNNumber[0xE3]; // +26C. only used in init phases ?
		// 0x5F8: end
	};


}
