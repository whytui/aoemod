
#pragma once

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


#define CHECKSUM_VICTORY_CONDITION_RULE_SYSTEM 0x00548C9C
#define CHECKSUM_STRATEGY_AI_PARENT 0x00542C58
#define CHECKSUM_STRATEGY_AI_CHILD 0x00548CA0

	// size 0x5F8 - constructor=0x4CB030(parent), 0x4CB030(child), 0x4CB180(child)
	// update=0x4CB730
	class STRUCT_STRATEGY_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1008
		// 0xF0
		STRUCT_AI *ptrMainAI;
		long int currentVictoryCondition; // +F4. Related to tasking idle soldiers ? 0x4D39A8. Some enum ? Same as group.subTaskId? =-1 until strategyAI init is done in 4CC0F0, then hardcoded=0xB.
		char perFileName[0x100]; // +F8. Name of .per file ("rules set")
		unsigned long int unknown_1F8;
		long int targetId; // +1FC
		long int targetType; // +200. UnitDefID?
		long int secondTargetId; // +204
		long int secondTargetType; // +208. UnitDefID?
		long int targetPoint1[4]; // +20C
		long int targetPoint2[4]; // +21C
		long int targetAttribute; // +22C
		long int targetNumber; // +230
		unsigned long int unknown_234; // +234
		STRUCT_MANAGED_ARRAY unknown_238; // List of SN numbers (index) ? For debugging ?
		STRUCT_MANAGED_ARRAY unknown_248; // List of SN numbers (index) ? For debugging ?
		STRUCT_MANAGED_ARRAY unknown_258; // List of SN numbers (index) ? For debugging ?
		// Here is "victory condition rules system". In fact it's just a copy of the set of SN numbers (+ the checksum)
		unsigned long int structPersonalityChecksum; // +268. 9C 8C 54 00 -> CHECKSUM_VICTORY_CONDITION_RULE_SYSTEM
		long int SNNumber[0xE2]; // +26C. only used in init phases ? 0xE2 IS the correct count here cf 0x49C540.
		unsigned long int unknown_5F4; // NOT a SN number, but part of victory condition rule system ?
		// 0x5F8: end
	};
	static_assert(sizeof(STRUCT_STRATEGY_AI) == 0x5F8, "STRUCT_STRATEGY_AI size");


}
