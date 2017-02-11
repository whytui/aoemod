
#pragma once

#include <AOE_const_internal.h>
#include <AOE_struct_map_base_common.h>
#include <AOE_struct_map_visibility_info.h>

using namespace AOE_CONST_INTERNAL;

#pragma pack(push, 1) // Prevent compiler from aligning on dwords (or other alignment). Very important here as there are some weird structures.

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !


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
	// Type 0x00 "Order". Size = 0x1C + actorCount*4. Create = 0x42B730. Execute=0x42A280
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
		STRUCT_MAP_VISIBILITY_MASK *visibilityMask; // +18

		long int actorIdList[1]; // +1C.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_RIGHT_CLICK; }
		long int MySizeInBytes() const { return 0x1C + 4 * this->actorCount; }
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
	static_assert(sizeof(COMMAND_STOP_ACTION) == 0x06, "COMMAND_STOP_ACTION size");


	// Type 0x02. Size= + actorCount*4. Create=. Execute=0x42A780
	// For AI (only) ? Task an idle villager ?
	struct COMMAND_WORK {
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
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_WORK; }
	};


	// Type 0x04. Size = 0x14. Create=0x42BA90
	// Create a fully-built construction (immediately ready), also works for any other unit (non building)
	// The unit is created even if map position, terrain restriction, etc are invalid.
	// Please make sure unitDefIdToCreate is valid (exists) for provided player
	struct COMMAND_CREATE {
	public:
		char cmdId;
		char unknown_01; // +1.
		short int unitDefIdToCreate; // +02. Identifies what object (def) to create
		short int playerId; // +04. Player Id for which create the unit
		short int unknown_06; // +06. Probably unused
		float posY; // +08
		float posX; // +0C
		float posZ; // +10
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CREATE; }
	};
	static_assert(sizeof(COMMAND_CREATE) == 0x14, "COMMAND_CREATE size");

	// Type 0x6 "Formation". Size = 0x08 + x*4. Create=0x42BB60, Exec=0x42AE20
	// Takes top-left unit as leader (lol). Other units move according to the formation specified (base = leader position)
	// This just orders a movement action, the formation is not preserved afterwards, and may not work well if units are moving (especially leader)
	// Not sure this always works very well, some units may ignore the movement.
	struct COMMAND_FORMATION {
	public:
		char cmdId;
		char unknown_01; // +1. Unused ?
		short int unknown_02; // +02. Unused ?
		long int unitCount; // +04. Number of unit IDs in unitIdArray.
		INTERNAL_UNIT_FORMATION_ID formationType; // +08. Represents how the type of formation units will organize into
		long int unitIdArray[1]; // +0C. Array size is only known dynamically.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_FORMATION; }
	};
	static_assert(sizeof(COMMAND_FORMATION) >= 0x10, "COMMAND_FORMATION size");

	// Type 0x07. Size = 0x08. Create=0x42BB20, Exec=0x42AD40
	// Commands a player to give a resource amount to another player. Tribute taxes are NOT supported here !
	struct COMMAND_GIVE_RESOURCE {
	public:
		char cmdId;
		char fromPlayerId; // +1. Player that will pay the resource
		char toPlayerId; // +02. Player that will receive the resource
		char resourceId; // +03
		float resourceValue; // +04
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_GIVE_RESOURCE; }
	};
	static_assert(sizeof(COMMAND_GIVE_RESOURCE) == 0x08, "COMMAND_GIVE_RESOURCE size");


	// Type 0xA "AI order". Size = 0x28. Create=42BCF0. Execute=0042B120
	// For AI (only).
	struct COMMAND_TASK_UNIT {
	public:
		char cmdId;
		char playerId_1;
		char playerId_2; // +2. Maybe for MP games with 2 human players for 1 "game" player ?
		char unknown_03; // unused?
		long int actorUnitId; // +4.
		short int activityId; // +8, a word. Cf ACTIVITY_TASK_IDS (but on 2 bytes;).
		char priority; // +A. =arg13 in 42BCF0, computed from ZeroPriorityDistance
		char unknown_0B; // +B. Unused ?
		long int targetUnitId; // DWORD.
		// 0x10
		char unknown_10; // 1 byte. =arg6 in 42BCF0
		char unknown_11[3]; // unused ?
		float posY; // +14.
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

	// Type 0x0C. Size = 0x9 + x*3 ? Create=0x42BE90. Exec=0x42B3B0
	// Set target position ?
	struct COMMAND_UNKNOWN_C {
	public:
		char cmdId;
		char playerId;
		short int unknown_02;
		long int unitId;
		unsigned char positionsCount; // +08. Number of elements in positionsArray (also gives structure size)
		STRUCT_UNIT_TARGET_POS positionsArray[1]; // +09. Array size is only known dynamically.

		bool IsCmdIdValid() const { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_UNKNOWN_C; }
		long int MySizeInBytes() const { return 0x09 + 3 * this->positionsCount; }
	};
	static_assert(sizeof(COMMAND_UNKNOWN_C) == 0x09 + 3, "COMMAND_UNKNOWN_C size");


	// Type 0x0E. Size = 0x10. Create=0x42BC40. exec=0x42B4A0
	struct COMMAND_UNKNOWN_E {
	public:
		char cmdId;
		char playerId; // +01. Player must be unit owner
		short int unknown_02;
		long int unitId; // +04. Actor unit ?
		float unknown_08; // +08. For unit+80 ?
		long int unknown_0C; // +0C. A unit id to add in group ?

		bool IsCmdIdValid() const { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_UNKNOWN_E; }
	};
	static_assert(sizeof(COMMAND_UNKNOWN_E) == 0x10, "COMMAND_UNKNOWN_E size");


	// Type 0x0F. Size = 0x0C. Create=0x42BC80. exec=0x42B4E0
	struct COMMAND_UNKNOWN_F {
	public:
		char cmdId;
		char playerId;
		short int unknown_02;
		long int unitId; // +04
		long int unknown_08; // +08
		bool IsCmdIdValid() const { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_UNKNOWN_F; }
	};
	static_assert(sizeof(COMMAND_UNKNOWN_F) == 0x0C, "COMMAND_UNKNOWN_F size");


	// Type 0x10. Size = 0x04 + x*4. Create=0x42BF00. Execute=0x42B3F0
	// Shift-right click for path finding
	struct COMMAND_ADD_INTERMEDIATE_MOVEMENT_STEP {
	public:
		char cmdId;
		char unitCount;
		unsigned char posY; // +02
		unsigned char posX; // +03
		long int unitIdArray[1]; // +04. Array size is only known dynamically.

		bool IsCmdIdValid() const { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_ADD_INTERMEDIATE_MOVEMENT_STEP; }
		long int MySizeInBytes() const { return 0x04 + 3 * this->unitCount; }
	};
	static_assert(sizeof(COMMAND_ADD_INTERMEDIATE_MOVEMENT_STEP) == 0x04 + 4, "COMMAND_ADD_INTERMEDIATE_MOVEMENT_STEP size");


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
	static_assert(sizeof(COMMAND_HOLD_POSITION) >= 0x24, "COMMAND_HOLD_POSITION size");


	// Type 0x64. Size = 0x10 ? Create=0x4E9490, 0x4E94E0
	// NOT enqueue. Mainly used by AI. + some cheats use it ? (does not require the building definition to be capable of training the unit)
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


	// Type 0x65. Size = 0x10. Create=0x4E9530
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
	static_assert(sizeof(COMMAND_MAKE_OBJECT) >= 0x14, "COMMAND_MAKE_OBJECT size (variable)");


	// Type 0x67. Size = 0x10. Common to several "subtypes". Those commands are executed in 4E8EB0.
	struct COMMAND_67_BASE {
	public:
		char cmdId;
		INTERNAL_COMMAND67_SUBTYPE subTypeId; // +1.
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING; }
	};
	static_assert(sizeof(COMMAND_67_BASE) == 0x2, "COMMAND_67_BASE size");
	

	// Type 0x67, subtype 0. Change diplomacy. Create = 04E9890. "Set relation"
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

	// Type 0x67, subtype 1. Change game speed. Create = 0x04E98F0.
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

	// Type 0x67, subtype 2. Add resource. "Inventory". Add some resource to player.
	// Unused but supported in ROR (not even a method to create this, but execution is still supported)
	struct COMMAND_ADD_RESOURCE : COMMAND_67_BASE {
	public:
		short int actorPlayerId; // +2
		short int resourceId; // +4
		short int unused_6;
		float resourceAmount; // +8. Amount to add
		short int unused_C; // +C
		short int unused_E;
		bool IsCmdIdValid() {
			return (this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING) && (this->subTypeId == INTERNAL_COMMAND67_SUBTYPE::CST_IC67_ADD_RESOURCE);
		}
	};
	static_assert(sizeof(COMMAND_ADD_RESOURCE) == 0x10, "COMMAND_ADD_RESOURCE size");

	// Type 0x67, subtype 3.  Unused in ROR. "Upgrade town". Does nothing ? Maybe this was never used at all !
	struct COMMAND_UPGRADE_TOWN : COMMAND_67_BASE {
	public:
		short int unknown_02;
		unsigned long int unknown_04;
		unsigned long int unknown_08; // not initialized ??
	};
	static_assert(sizeof(COMMAND_UPGRADE_TOWN) == 0x0C, "COMMAND_UPGRADE_TOWN size"); // UNSURE

	// Type 0x67, subtype 4. Set steroids mode ON/OFF. "quick build"
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

	// Type 0x67, subtype 5. Change diplomacy. Create = 04E9840.
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

	// Type 0x67, subtype 6. Change diplomacy. Create = 0x4E9840. Exec=0x50C570 ExecuteCheatCommand(playerId, subType)
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

	// Type 0x67, subtype 7. Apply writing tech (shared exploration). Unused in ROR ?
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

	// Type 0x67, subtype 8. Sync errors for network games. Not sure this command is really used ?
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


	// Type 0x68. Size = 0x0C + actorCount*4. Create=<none>. Exec=0x4E8E20
	// Is this no more than a move operation ?
	struct COMMAND_EXPLORE {
	public:
		char cmdId;
		char actorCount; // +01. Number of units in array
		char playerId; // +02
		char unused_03; // +03
		float posY; // +04
		float posX; // +08
		long int actorIdList[1]; // +0C.
		bool IsCmdIdValid() const { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_EXPLORE; }
		long int MySizeInBytes() const { return 0x0C + 4 * this->actorCount; }
	};
	static_assert(sizeof(COMMAND_EXPLORE) >= 0x010, "COMMAND_EXPLORE size");


	// Type 0x69. Size = 0x10 + 4*actorCount. Create=0x4E9710 = cmdInfo.createCmdBuildMultiple(pActorUnits, actorCount, unitDefId, minPosY, minPosX, maxPosY, maxPosX)
	// Exec=0x4E8B00
	// Build a new construction for wall-like buildings.
	struct COMMAND_BUILD_MULTIPLE {
	public:
		char cmdId;
		char builderCount; // +1. Provides the array size of builderIdList
		char playerId; // +2
		char minPosY; // +03. Start pos (Y) for construction
		char minPosX; // +04. Start pos (X) for construction
		char maxPosY; // +05. End pos (Y) for construction
		char maxPosX; // +06. End pos (X) for construction
		char unused_07; // +07.
		short int unitDefIdToBuild; // +0x08. DATID to build
		short int unused_0A;
		long int stratElemId; // +0C. To confirm. Default=-1
		// 0x10
		long int builderIdList[1]; // +10. Array size is only known dynamically.
		bool IsCmdIdValid() const { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_BUILD_MULTIPLE; }
		long int MySizeInBytes() const { return 0x10 + 4 * this->builderCount; }
	};
	static_assert(sizeof(COMMAND_BUILD_MULTIPLE) >= 0x14, "COMMAND_BUILD_MULTIPLE size (variable)");


	// Type 0x6A. Size = 0x04 + actorCount*4. Create=0x4E9920. Exec=0x4E9130
	// No check is performed at command execution (except that unit does exist)
	struct COMMAND_KILL_UNIT {
	public:
		char cmdId;
		char unused_01[3]; // +1
		long int unitId; // +04. ID of unit to kill
		bool IsCmdIdValid() const { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_KILL_UNIT; }
	};
	static_assert(sizeof(COMMAND_KILL_UNIT) == 0x08, "COMMAND_KILL_UNIT size");


	// Type 0x6B. Size = 0x0C + actorCount*4. Create=0x4E9950. Exec=0x4E9160
	struct COMMAND_ATTACK_POSITION {
	public:
		char cmdId;
		char actorCount; // Elem count for actorIdList
		char unused_02[2]; // +2
		float posY; // +04
		float posX; // +08
		long int actorIdList[1]; // +0C.
		bool IsCmdIdValid() const { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_ATTACK_POSITION; }
		long int MySizeInBytes() const { return 0x0C + 4 * this->actorCount; }
	};
	static_assert(sizeof(COMMAND_ATTACK_POSITION) >= 0x10, "COMMAND_ATTACK_POSITION size");


	// Type 0x6C. Pay tribute. Size=0xC. Create=04E9A10, execute=04E91B0
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


	// Type 0x6E. Size = 0x08 + actorCount*4. Create=0x4E9B20. Exec=0x4E9380
	// This command corresponds to using the "repair" button (NOT right-click to repair, see command COMMAND_RIGHT_CLICK)
	struct COMMAND_REPAIR {
	public:
		char cmdId;
		char actorCount; // Elem count for actorIdList. Should be >=1
		char unused_02[2]; // +2
		long int targetUnitId; // +4. Unit ID to repair
		long int actorIdList[1]; // +8. List of actors (unit IDs)
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_REPAIR; }
		long int MySizeInBytes() const { return 0x08 + 4 * this->actorCount; }
	};
	static_assert(sizeof(COMMAND_REPAIR) >= 0xC, "COMMAND_REPAIR size");


	// Type 0x6F. Size = 0x0C + actorCount*4. Create=0x4E9BA0=cmdInfo.createCmdUnload(pactorIdArray, actorCount, fposY, fposX)
	// Exec=0x4E93D0
	struct COMMAND_UNLOAD_TRANSPORT {
	public:
		char cmdId;
		char actorCount; // +01. Elem count for actorIdList. Should be >=1
		char unused_02[2]; // +2
		long int posY; // +4. 
		float posX; // +08
		long int actorIdList[1]; // +0x0C. List of actors (unit IDs)
		bool IsCmdIdValid() { return this->cmdId == INTERNAL_COMMAND_ID::CST_ICI_UNLOAD_TRANSPORT; }
		long int MySizeInBytes() const { return 0x0C + 4 * this->actorCount; }
	};
	static_assert(sizeof(COMMAND_UNLOAD_TRANSPORT) >= 0x10, "COMMAND_UNLOAD_TRANSPORT size");


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

#pragma pack(pop)
