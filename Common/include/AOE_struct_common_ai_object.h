
#pragma once

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

#define CHECKSUM_COMMON_AI_OBJECT 0x00542C00
	// The common object included in each AI section (with xxxAI text)
	// Size = 0xEC
	class STRUCT_COMMON_AI_OBJECT { // 00 2C 54 00 in 1.0c
	public:
		unsigned long int checksum;
		unsigned long int AITypeId; // 1000=MainDecisionAI, 1001=BuildAI, 1002=ConAI... Not really used.
		char AIType_text[0x38]; // Exact size ?
		// 0x40
		unsigned long int unknown_040;
		unsigned long int unknown_044;
		long int playerId;
		char playerName[0x40]; // size=64
		unsigned long int unknown_08C; // (3 bytes + 1 unused?)
		// 0x90
		char unknown_090[0x10];
		// 0xA0
		char unknown_0A0[0x40];
		// 0xE0
		unsigned long unknown_0E0; // init = 5 ?
		unsigned long unknown_0E4; // maximum population ?
		unsigned long unknown_0E8;
		// 0xEC: end

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_COMMON_AI_OBJECT; }
	};
	static_assert(sizeof(STRUCT_COMMON_AI_OBJECT) == 0xEC, "STRUCT_COMMON_AI_OBJECT size");

}
