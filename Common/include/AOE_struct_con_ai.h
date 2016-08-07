
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


	// Size = 0x80. Included in ConAI
	// Some member names were copied from struct strategy element because it looks very similar. (could have same parent class)
	// But this has to be checked (maybe it's wrong)
	class STRUCT_CON_AI_CONSTRUCTION {
	public:
		unsigned long int checksum; // 94 2A 54 00. Parent = 38 2A 54 00 (same parent as stratElem !)
		long int unitDefId;
		unsigned long int unknown_08; // unit instance id ?
		unsigned long int unknown_0C; // counter ?
		// 0x10
		char unitName[0x40]; // 0x3F + \0 ?
		// 0x50
		float posY; // The position that will actually be used to build the new building
		float posX; // The position that will actually be used to build the new building
		unsigned long int unknown_58;
		unsigned long int unknown_5C;
		// 0x60
		float sizeRadius1; // TO VERIFY : +5C and 60 maybe
		float sizeRadius2; // TO VERIFY
		unsigned int unknown68_skipCount; // TO VERIFY // Unsure. "skip" term found in 408891
		STRUCT_CON_AI_CONSTRUCTION *next;
		// 0x70
		STRUCT_CON_AI_CONSTRUCTION *previous;
		unsigned long int unknown_74;
		unsigned long int unknown_78;
		unsigned long int unknown_7C;

		bool IsCheckSumValid() { return this->checksum == 0x00542A94; }
	};


	// size 0x318 - constructor 0x4B9660.
	// Checksums 18 8B 54 00, parents=98 2A 54 00
	// Construction AI
	class STRUCT_CON_AI {
	public:
		unsigned long int checksum; // 18 8B 54 00
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1002
		// 0xF0
		long int unknown_0F0; // Tested in 4B9A4D
		STRUCT_CON_AI_CONSTRUCTION tempFakeConstructionInfo1; // +F4. First element of a chained list. The constructions that still have to BE STARTED ?
		unsigned long int unknown_174;
		STRUCT_CON_AI_CONSTRUCTION tempFakeConstructionInfo2; // +178. First element of a chained list. Independant from tempConstructionInfo1 !
		char cityPlanName[0x100]; // +1F8. For scenario, selected city plan name. Size is unsure (maybe 101 + 3 unused bytes ?)
		unsigned long int unknown_2F8;
		float townCenter_posY;
		// 0x300
		float townCenter_posX;
		float townCenter_posZ; // unsure !
		long int mapSizeY; // +308
		long int mapSizeX; // +30C
		unsigned long int unknown_310; // unknown type
		unsigned long int ptrMainAI; // +314
		// 0x318: end
		bool IsCheckSumValid() { return this->checksum == 0x00548B18; }
	};
	static_assert(sizeof(STRUCT_CON_AI) == 0x318, "STRUCT_CON_AI size");

}
