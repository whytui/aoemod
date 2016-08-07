
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


	// size 0xF8 - constructor=0x4C8B50
	class STRUCT_RESOURCE_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1007
		// 0xF0
		STRUCT_AI *ptrMainAI;
		unsigned long int resourceTypeCount; // = 4
	};
	static_assert(sizeof(STRUCT_RESOURCE_AI) == 0xF8, "STRUCT_RESOURCE_AI size");

}
