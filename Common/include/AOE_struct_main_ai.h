
#pragma once

#include <AOE_struct_build_ai.h>
#include <AOE_struct_con_ai.h>
#include <AOE_struct_diplomacy_ai.h>
#include <AOE_struct_emotional_ai.h>
#include <AOE_struct_inf_ai.h>
#include <AOE_struct_resource_ai.h>
#include <AOE_struct_strategy_ai.h>
#include <AOE_struct_tac_ai.h>
#include <AOE_struct_trade_ai.h>
#include <AOE_struct_managed_array.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {
	class STRUCT_PLAYER;

#ifndef GAMEVERSION_AOK0005030706
#define CHECKSUM_AI_PARENT 0x00542C04
#define CHECKSUM_AI 0x00548BF0
#endif

#ifdef GAMEVERSION_AOK0005030706
#define CHECKSUM_AI_PARENT 0
#define CHECKSUM_AI 0x005B1848 // Ccor 0x510150, size=0x13564
#endif
	// The main AI object. Total size = 0x125C8. Constructor = 0x4C6960.
	// Checksums: F0 8B 54 00 (normal) + parents 04 2C 54 00, B0 2B 54 00
	// +0x00 = destructor(do_free)
	// +0x08 = save?
	// +0x18 = AI.startAIHistoryFile(arg1)
	// +0x30 = update(timeLimit) : 0x4C71A0
	// +0x48 = mainAI.getUnitClassFromTaskId(taskId), for tasks 25A 25D 261 265 26B 26C only
	class STRUCT_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT structMainDecisionAI; // size 0xEC
		// 0xF0
		STRUCT_PLAYER *ptrStructPlayer;

		STRUCT_MANAGED_ARRAY allMyUnits; // +F4. Can contain empty elements ? Remove in 40B950.
		STRUCT_BUILD_AI structBuildAI; // +0x104
		STRUCT_CON_AI structConAI; // +0x6C4 ; city plan [+ECC aok?]
		STRUCT_DIPLOMACY_AI structDiplAI; // +0x9DC [+11E4 aok?]
		STRUCT_EMOTIONAL_AI structEmotionalAI; // +0xB2C [+1334 aok?]
		STRUCT_INF_AI structInfAI; // +0xCEC  [+14F4 aok?]
		STRUCT_RESOURCE_AI structResourceAI; // +0x10D7C [+11D88 aok?]
		STRUCT_STRATEGY_AI structStrategyAI; // +0x10E74 [+11E80 aok?]
		STRUCT_TAC_AI structTacAI; // +0x1146C [+12480 aok?]
		STRUCT_TRADE_AI structTradeAI; // +0x12478 [+13414 aok?]

		STRUCT_PLAYER *player; // +1256C. Pointer to player structure
		// +12570. =1 until game start AI init has been done. Includes log like values, add dynamic strategy, resource AI bonus, tribute info init
		// Bug: this information is NOT saved in savegame files (whereas hasDiplomacyInitBeenDone is). When loading a game, it is set to 0 even if init has never been run.
		long int needGameStartAIInit;
		unsigned long int unknown_12574;
		long int nextStrategyAIExecutionCounter; // +12578. Increased, when = 0x14=20 then reset and execute strategy AI.
		unsigned long int unknown_1257C; // A milliseconds time value ? Related to diplomacy management ?
		long int unknown_12580; // +12580. Last mainAI update time
		unsigned long int unknown_12584; // +12584. A milliseconds time value ? Related to tributes ??
		unsigned long int tributeChatFrequency; // +12588. Including randomness. Calculated from SNTributeChatFrequency and SNTributeChatRandomness
		unsigned long int unknown_1258C;
		// 0x12590
		unsigned long int unknown_12590;
		unsigned long int unknown_12594_tributeIsNOTPersistent; // REALLY unsure.
		unsigned long int unknown_12598;
		unsigned long int hasDiplomacyInitBeenDone; // +1259C. 0/1 flag. Waits 10 seconds (0x2710=10000ms) before this init. cf 4C783F.
		unsigned long int requiredTributeToChangeDiplomacy; // +125A0
		long int receivedTributeFromPlayer[9]; // +125A4. Total tribute received from a player (no distinction on resource type). See 4C83B5


		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_AI; }
	};
#ifndef GAMEVERSION_AOK0005030706
	static_assert(sizeof(STRUCT_AI) == 0x125C8, "STRUCT_AI size");
#endif

}
