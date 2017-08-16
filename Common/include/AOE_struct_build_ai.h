
#pragma once

#include <AOE_const_functional.h>
#include <AOE_struct_strategy_element.h>
#include <AOE_struct_common_ai_object.h>
#include <AOE_struct_managed_array.h>

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

#define CHECKSUM_BUILD_AI_PARENT 0x542A40
#define CHECKSUM_BUILD_AI 0x00548ACC

	// Size 0x5C0 - Constructor 0x4B7650
	// Checksums: CC 8A 54 00 (normal), parent=40 2A 54 00
	class STRUCT_BUILD_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1001
		// 0xF0: 1st strategy element
		STRUCT_STRATEGY_ELEMENT fakeFirstStrategyElement; // The first strategy element (empty)
		// 0x1A0
		long int strategyElementCount; // = number of strategy elements (NOT counting fake first elem) = ID for the next element to add
		char strategyFileName[0x40]; // +1A4. Size: unsure
		char unknown_1E4[0x2A5 - 0x1E4];
		char lastBuildName[0x101]; // +2A5: text
		char currentBuildName[0x101]; // +3A6: text (desired build unit name?)
		char nextBuildName[0x101]; // +4A7: text (desired build unit name?)
		long int currentIDInStrategy; // +5A8. Corresponds to stratelem.elemId, not "index". ID 2 might NOT be 3rd element due to inserted elems.
		STRUCT_MANAGED_ARRAY autoInsertedBuildingDATIDs; // +5AC. Actually, IDs are DATID, not unit instance IDs. Automatically added buildings ?
		STRUCT_AI *mainAI; // +5BC. Set in 0x4B76E0.
		// 0x5C0: end
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_BUILD_AI; }
	};
	static_assert(sizeof(STRUCT_BUILD_AI) == 0x5C0, "STRUCT_BUILD_AI Size");


}
