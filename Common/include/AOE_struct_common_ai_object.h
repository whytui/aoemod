
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
	// The common object included in each AI section (with xxxAI text). "AIModuleId"
	// Size = 0xEC. Constructor=0x40BFD0
	class STRUCT_COMMON_AI_OBJECT { // 00 2C 54 00 in 1.0c
	public:
		unsigned long int checksum;
		// Here starts "AIModule" object
		unsigned long int AITypeId; // 1000=MainDecisionAI, 1001=BuildAI, 1002=ConAI... Not really used.
		char AIType_text[0x38]; // Exact size ?
		// 0x40
		unsigned long int unknown_040;
		unsigned long int unknown_044;
		long int playerId; // +48
		char playerName[0x40]; // +4C. size=64
		long int unknown_08C; // +8C. started ?
		// 0x90
		long int unknown_90; // +90. paused ?
		long int unknown_94; // +94. logging history (debug) ?
		long int unknown_98; // +98. logging common history (debug) ?
		long int unknown_9C;
		// 0xA0
		char unknown_0A0[0x40];
		// 0xE0
		long int intelligenceLevel; // +E0. init = 5.
		long int priority; // +E4. Default 50. Range 0-100.
		unsigned long unknown_0E8;
		// 0xEC: end

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_COMMON_AI_OBJECT; }
	};
	static_assert(sizeof(STRUCT_COMMON_AI_OBJECT) == 0xEC, "STRUCT_COMMON_AI_OBJECT size");

}
