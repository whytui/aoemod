
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
*
* Very useful for debugging: 0x4E83E0 = commandsInfo.executeCommand(ptrCommand)
*/
namespace AOE_STRUCTURES
{
	// Type 0x00. Size = 0x1C + actorCount*4. Create = 0x42B730. Execute=0x42A280
	// Actual effect depends on unit def commands, target unit type...
	struct COMMAND_RIGHT_CLICK {
	public:
		char cmdId;
		char actorCount; // Elem count for actorIdList
		char unused_02[2]; // +2
		long int targetUnitId; // +4.
		float posY;
		float posX; // +C.
		// 0x10
		char humanPlayerId; // +10. Only used in MP, useful if 2 human players manage the same game player.
		char targetVisibleInFog; // +11. Used in both SP and MP ?
		char sharedExploration; // +12. sharedExploration ? Only in MP games.
		char unknown_13; // Init =0 in 42B800. Only in MP games.
		long int unknown_14; // related to fogMask & explorationMask ? Only in MP games.
		// unknown_18: STRUCT_MAP_VISIBILITY_INFO (4 bytes) = fogVisibilityMask + explorationVisibilityMask. Filled from 7D205C array.
		short int fogVisibilityMask; // +18
		short int explorationVisibilityMask; // +1A

		long int actorIdList[1]; // +1C.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_RIGHT_CLICK; }
	};
	static_assert(sizeof(COMMAND_RIGHT_CLICK) >= 0x1C, "COMMAND_RIGHT_CLICK size");

	// Type 0x01. Size = 0x2 + actorCount*4 (yes !). Create=. Execute=42A730
	struct COMMAND_STOP_ACTION {
	public:
		char cmdId;
		char actorCount; // +1. Elem count for actorIdList
		long int unitId[1]; // +2. Array size is dynamic and depends on actorCount. Yes this dword is not aligned in memory !

		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_STOP_ACTION; }
	};


	// Type 0x02. Size= + actorCount*4. Create=. Execute=0x42A780
	// For AI (only) ? Task an idle villager ?
	struct COMMAND_UNKNOWN_02_TASK {
	public:
		char cmdId;
		char actorCount; // +1. Elem count for actorIdList
		char unknown_02[2]; // +02
		long int targetUnitId; // +04
		float posY; // +0x08
		float posX; // +0x0C
		// 0x10
		unsigned long int unknown_10;
		unsigned long int unknown_14;
		unsigned long int unknown_18;
		long int actorUnitIdList[1]; // +1C. Array size is only known dynamically.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_UNKNOWN_02_TASK; }
	};

	// Type 0xA. Size = 0x28. Create=42BCF0. Execute=0042B120
	// For AI (only) ?
	struct COMMAND_TASK_UNIT {
	public:
		char cmdId;
		char playerId_1;
		char playerId_2; // +2. Maybe for MP games with 2 human players for 1 "game" player ?
		char unknown_03; // unused?
		long int actorUnitId; // +4.
		short int activityId; // +8, a word. Cf ACTIVITY_TASK_IDS (but on 2 bytes;).
		char unknown_0A; // +A. =arg13 in 42BCF0
		char unknown_0B; // +B. Unused ?
		long int targetUnitId; // DWORD.
		// 0x10
		char unknown_10; // 1 byte. =arg6 in 42BCF0
		char unknown_11[3]; // unused ?
		float posY;
		float posX; // +18.
		float posZ; // +1C.
		// 0x20
		float maxDistance;
		char unknown_24; // 1 byte. =arg11 in 42BCF0
		char unknown_25; // 1 byte. =arg12 in 42BCF0
		char unused_26[2];

		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_TASK_UNIT; }
	};
	static_assert(sizeof(COMMAND_TASK_UNIT) == 0x28, "COMMAND_TASK_UNIT size");


	// Type 0x11. Size = 0x24+x*4. Create=. Execute=0042B1A0
	// For AI (only) ?
	struct COMMAND_HOLD_POSITION {
	public:
		char cmdId;
		char playerId; // +1. (player index, like settings.chosenPlayerIndex)
		char playerIdMPGame; // +2. For MP games, otherwise = playerId ? Only for human players ?
		char actorUnitCount; // +3. number of elements in +24 array (and actorUnitCount*4 = array size !)
		short int activityTaskId; // ACTIVITY_TASK_IDS as a word (2-bytes)
		char unknown_06; // some distance ?? seen 0x64
		char unknown_07; // possibly unused ?
		unsigned long int unknown_08; // dword. Target unit id ?
		char unknown_0C; // target player id ?
		char unknown_0D;
		char unknown_0E;
		char unknown_0F;
		// 0x10
		float posY;
		float posX;
		float posZ;
		float maxRange; // +1C. NOT unit's maxRange, just command's range !
		// 0x20
		char unknown_20;
		char unknown_21;
		char unknown_22;
		char unknown_23;
		long int actorUnitIdArray[1]; // +24. Array size is only known dynamically.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_HOLD_POSITION; }
	};
	static_assert(sizeof(COMMAND_TASK_UNIT) >= 0x24, "COMMAND_TASK_UNIT size");

	// Type 0x64. Size = 0x10 ? Create=0x4E9490, 0x4E94E0
	// NOT enqueue. Mainly used by AI. + some cheats use it ?
	// For human interaction, see COMMAND_QUEUE_UNIT
	struct COMMAND_TRIGGER_TRAIN_UNIT {
	public:
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
	public:
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
	public:
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


	// Type 0x67. Size = 0x10. Common to several "subtypes". Those commands are executed in 4E8EB0.
	struct COMMAND_67_BASE {
	public:
		char cmdId;
		INTERNAL_COMMAND67_SUBTYPE subTypeId; // +1.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING; }
	};
	static_assert(sizeof(COMMAND_67_BASE) == 0x2, "COMMAND_67_BASE size");
	

	// Type 67, subtype 0. Change diplomacy. Create = 04E9890.
	struct COMMAND_CHANGE_DIPLOMACY : COMMAND_67_BASE {
	public:
		short int actorPlayerId; // +2
		short int targetPlayerId; // +4
		short int unknown_6;
		float floatDiplomacyStance; // +8. New diplomacy stance value as a float
		short int diplomacyStance; // +C. Cf PLAYER_DIPLOMACY_STANCES (but on 2 bytes here)
		short int unused_E;
		bool IsCmdIdValid() {
			return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING &&
				this->subTypeId == INTERNAL_COMMAND67_SUBTYPE::CST_IC67_CHANGE_DIPLOMACY;
		}
	};
	static_assert(sizeof(COMMAND_CHANGE_DIPLOMACY) == 0x10, "COMMAND_CHANGE_DIPLOMACY size");

	// Type 67, subtype 1. Change game speed. Create = 04E98F0.
	struct COMMAND_CHANGE_GAME_SPEED : COMMAND_67_BASE {
	public:
		short int unused_2; // +2
		short int unused_4; // +4
		short int unused_6;
		float newSpeed; // +8.
		short int unused_C; // +C
		short int unused_E;
		bool IsCmdIdValid() {
			return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING && this->subTypeId == INTERNAL_COMMAND67_SUBTYPE::CST_IC67_CHANGE_GAME_SPEED;
		}
	};
	static_assert(sizeof(COMMAND_CHANGE_GAME_SPEED) == 0x10, "COMMAND_CHANGE_GAME_SPEED size");

	// Type 67, subtype 2. Add resource. Unused in ROR ?
	struct COMMAND_ADD_RESOURCE : COMMAND_67_BASE {
	public:
		short int actorPlayerId; // +2
		short int resourceId; // +4
		short int unused_6;
		float resourceAmount; // +8. Amount to add
		short int unused_C; // +C
		short int unused_E;
		bool IsCmdIdValid() {
			return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING && this->subTypeId == INTERNAL_COMMAND67_SUBTYPE::CST_IC67_ADD_RESOURCE;
		}
	};
	static_assert(sizeof(COMMAND_ADD_RESOURCE) == 0x10, "COMMAND_ADD_RESOURCE size");

	// Type 67, subtype 4. Set steroids mode ON/OFF. Unused in ROR ?
	struct COMMAND_SET_STEROIDS_MODE : COMMAND_67_BASE {
	public:
		short int steroidsModeFlag; // +2. 0 or 1.
		short int unused_4;
		short int unused_6;
		float unused_8;
		short int unused_C;
		short int unused_E;
		bool IsCmdIdValid() {
			return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING && this->subTypeId == INTERNAL_COMMAND67_SUBTYPE::CST_IC67_SET_STEROIDS_MODE;
		}
	};
	static_assert(sizeof(COMMAND_SET_STEROIDS_MODE) == 0x10, "COMMAND_SET_STEROIDS_MODE size");

	// Type 67, subtype 5. Change diplomacy. Create = 04E9840.
	struct COMMAND_SET_ALLY_VICTORY : COMMAND_67_BASE {
	public:
		short int actorPlayerId; // +2
		short int allyVictoryFlag; // +4. 0=OFF, 1=ON for ally victory
		short int unknown_6;
		float unused_8; // +8. Set to 0.
		short int unknown_C; // +C
		short int unused_E;
		bool IsCmdIdValid() {
			return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING && this->subTypeId == INTERNAL_COMMAND67_SUBTYPE::CST_IC67_SET_ALLY_VICTORY;
		}
	};
	static_assert(sizeof(COMMAND_SET_ALLY_VICTORY) == 0x10, "COMMAND_SET_ALLY_VICTORY size");

	// Type 67, subtype 6. Change diplomacy. Create = 04E9840.
	struct COMMAND_CHEAT : COMMAND_67_BASE {
	public:
		short int actorPlayerId; // +2
		short int cheatInternalId; // +4. Identifies the cheat to apply.
		short int unused_6;
		float unused_8;
		short int unused_C; // +C
		short int unused_E;
		bool IsCmdIdValid() {
			return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING && this->subTypeId == INTERNAL_COMMAND67_SUBTYPE::CST_IC67_CHEAT;
		}
	};
	static_assert(sizeof(COMMAND_CHEAT) == 0x10, "COMMAND_CHEAT size");

	// Type 67, subtype 7. Apply writing tech (shared exploration). Unused in ROR ?
	struct COMMAND_APPLY_WRITING : COMMAND_67_BASE {
	public:
		short int playerId; // +2. Player ID on which apply the technology 113 (writing).
		short int unused_4; // +4
		short int unused_6;
		float unused_8; // +8
		short int unused_C; // +C
		short int unused_E;
		bool IsCmdIdValid() {
			return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING && this->subTypeId == INTERNAL_COMMAND67_SUBTYPE::CST_IC67_APPLY_WRITING_TECH;
		}
	};
	static_assert(sizeof(COMMAND_APPLY_WRITING) == 0x10, "COMMAND_APPLY_WRITING size");

	// Type 67, subtype 8. Sync errors for network games. Not sure this command is really used ?
	struct COMMAND_SYNC_ERROR : COMMAND_67_BASE {
	public:
		short int unused_2; // +2.
		short int unused_4; // +4
		short int unused_6;
		float unused_8; // +8
		short int unused_C; // +C
		short int unused_E;
		bool IsCmdIdValid() {
			return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING && this->subTypeId == INTERNAL_COMMAND67_SUBTYPE::CST_IC67_APPLY_WRITING_TECH;
		}
	};
	static_assert(sizeof(COMMAND_SYNC_ERROR) == 0x10, "COMMAND_SYNC_ERROR size");


	// Type 6C. Pay tribute. Size=0xC. Create=04E9A10, execute=04E91B0
	struct COMMAND_PAY_TRIBUTE {
	public:
		char cmdId; // 0x6C
		char actorPlayerId; // +1
		char targetPlayerId; // +2
		char resourceId; // Cf RESOURCE_TYPES, but on 1 byte
		float amount; // +4. Raw tribute amount (NOT counting tax)
		float tributeTaxProportion; // +8. Tax to pay on tribute, e.g. 0.25 (25% tax, default for non-palmyra players). Cf resource 46 (tribute inefficiency)
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_PAY_TRIBUTE; }
	};
	static_assert(sizeof(COMMAND_PAY_TRIBUTE) == 0xC, "COMMAND_PAY_TRIBUTE size");


	// Type 0x6D. Size = 0x8 + actorCount*4. Create=0x4E9A60, 0x4E9AE0
	struct COMMAND_SET_TRADE_SHIP_RESOURCE {
	public:
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
	public:
		char cmdId;
		char unused_01[3]; // +1
		long int actorUnitId; // +4.
		short int DATID;
		short int enqueueCount; // +A. Can be <0 (remove from queue)
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_QUEUE_UNQUEUE_UNIT; }
	};
	static_assert(sizeof(COMMAND_QUEUE_UNIT) == 0xC, "COMMAND_QUEUE_UNIT size");


}
