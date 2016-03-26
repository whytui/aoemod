
#pragma once

#include <AOE_const_internal.h>

using namespace AOE_CONST_INTERNAL;

/*
* This file contains empiresX.exe structures definition for commands
* Commands are very simple structs that describe basic "user commands"
* Mostly, it corresponds to human commands (right-click, click to train/research, "stop", change game speed, cheat, etc)
* Sometimes AI triggers some commands too (task units...)
* "commands" seem to be created by client and executed by game host (same machine on SP, but only 1 host in MP)
* Which could explain why MP games are extremely laggy (lag between click and actual action).
*/
namespace ROR_STRUCTURES_10C
{
	// Type 0x00. Size = 0x1C + actorCount*4. Create = 0x42B730
	// Actual effect depends on unit def commands, target unit type...
	struct COMMAND_RIGHT_CLICK {
		char cmdId;
		char actorCount; // Elem count for actorIdList
		char unused_02[2]; // +2
		long int targetUnitId; // +4.
		float posY;
		float posX; // +C.
		// 0x10
		char humanPlayerId; // +10. Only used in MP, useful if 2 human players manage the same game player.
		char targetVisibleInFog; // +11. Used in both SP and MP ?
		char unknown_12; // = sharedExploration ? Only in MP games.
		char unknown_13; // Init =0 in 42B800. Only in MP games.
		long int unknown_14; // related to fogMask & explorationMask ? Only in MP games.
		// unknown_18: STRUCT_MAP_VISIBILITY_INFO (4 bytes) = fogVisibilityMask + explorationVisibilityMask. Filled from 7D205C array.
		short int fogVisibilityMask;
		short int explorationVisibilityMask;

		long int actorIdList[1]; // +1C.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_RIGHT_CLICK; }
	};
	static_assert(sizeof(COMMAND_RIGHT_CLICK) >= 0x1C, "COMMAND_RIGHT_CLICK size");

	// Type 0x64. Size = 0x10 ? Create=0x4E9490, 0x4E94E0
	// NOT enqueue. Mainly used by AI. + some cheats use it ?
	// For human interaction, see COMMAND_QUEUE_UNIT
	struct COMMAND_TRIGGER_TRAIN_UNIT {
		char cmdId;
		char unknown_01[3]; // +1
		long int actorUnitId; // +4
		char playerId; // +8.
		char unknown_09;
		short int DATID; // +A. target's unit definition ID
		long int stratElemId; // +0xC. To confirm.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_TRIGGER_TRAIN_UNIT; }
	};
	static_assert(sizeof(COMMAND_TRIGGER_TRAIN_UNIT) == 0x10, "COMMAND_TRIGGER_TRAIN_UNIT size");


	// Type 0x65. Size = 0x10
	struct COMMAND_START_RESEARCH {
		char cmdId;
		char unused_01[3]; // +1
		long int actorUnitId; // +4
		char playerId; // +8.
		char unknown_09; // unused?
		short int researchId; // +0xA
		long int stratElemId; // +0xC. To confirm.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_RESEARCH; }
	};
	static_assert(sizeof(COMMAND_START_RESEARCH) == 0x10, "COMMAND_START_RESEARCH size");

	// Type 0x66. Size = 0x14 + 4*actorCount. Create=0x4E95D0, 0x4E9680
	// Build a new construction
	struct COMMAND_MAKE_OBJECT {
		char cmdId;
		char builderCount; // +1. Provides the array size of builderIdList
		char playerId; // +2
		char unused_03;
		float posY; // +4
		float posX; // +8
		short int unitDefIdToBuild; // +0x. DATID to build
		short int unused_0E;
		// 0x10
		long int stratElemId; // Id of corresponding strategy element, if triggered by AI. -1 otherwise.
		long int builderIdList[1]; // +14. Array size is only known dynamically.
		// AOE code must look like struct COMMAND_MAKE_OBJECT *test = malloc(sizeof(struct COMMAND_MAKE_OBJECT) + 4*(myLength - 1));

		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_BUILD; }
	};
	static_assert(sizeof(COMMAND_MAKE_OBJECT) >= 0x14, "COMMAND_START_RESEARCH size (variable)");

	// Type 0x6D. Size = 0x8 + actorCount*4. Create=0x4E9A60, 0x4E9AE0
	struct COMMAND_SET_TRADE_SHIP_RESOURCE {
		char cmdId;
		char actorCount; // Elem count for actorIdList
		char unused_02[2]; // +2
		long int resourceType; // +4.
		long int actorIdList[1]; // +8.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_SET_TRADE_RESOURCE_TYPE; }
	};
	static_assert(sizeof(COMMAND_SET_TRADE_SHIP_RESOURCE) >= 0xC, "COMMAND_SET_TRADE_SHIP_RESOURCE size");


	// Type 0x77. Size = 0xC. Create=0x4E9C20. For both "enqueue" and "dequeue" unit
	struct COMMAND_QUEUE_UNIT {
		char cmdId;
		char unused_01[3]; // +1
		long int actorUnitId; // +4.
		short int DATID;
		short int enqueueCount; // +A. Can be <0 (remove from queue)
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_QUEUE_UNQUEUE_UNIT; }
	};
	static_assert(sizeof(COMMAND_QUEUE_UNIT) == 0xC, "COMMAND_QUEUE_UNIT size");


}

