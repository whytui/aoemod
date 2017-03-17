
#pragma once


/* This file contains various constants that are used in game EXE for unit AI
*/

namespace AOE_CONST_INTERNAL
{


	// For activity.task IDs (unit AI tasks)
	enum ACTIVITY_TASK_IDS : long int {
		CST_ATI_NONE = -1, // Used a lot in game code
		CST_ATI_TASK_ATTACK = 0x258, // Why does this value also appear in *notify* methods (as an event) ?
		CST_ATI_TASK_DEFEND_OR_CAPTURE = 0x259, // primary role is defend (position or object), but it is used to capture relics
		CST_ATI_TASK_BUILD = 0x25A,
		CST_ATI_TASK_HEAL = 0x25B,
		CST_ATI_TASK_CONVERT = 0x25C,
		CST_ATI_TASK_EXPLORE = 0x25D,
		CST_ATI_TASK_GATHER_NOATTACK = 0x261, // Forager, farmer, miners (gathering that doesn't have an "attack" phase.
		CST_ATI_TASK_MOVE = 0x262, // 
		// 263: unused
		CST_ATI_TASK_FOLLOW_OBJECT = 0x264, // Follow an object ?
		CST_ATI_TASK_GATHER_ATTACK = 0x265, // woodcutter, hunter, fisherman (gatherer that have an "attack" phase - not really true for fish, but anyway !)
		CST_ATI_TASK_UNKNOWN_266 = 0x266, // regroup in unit group ??? Related to SNAttackGroupGatherSpacing in 0x4CD551? Used to have transport go to "unload" destination(order=2CA)
		CST_ATI_TASK_TRADE_WITH_OBJECT = 0x267,
		CST_ATI_TASK_UNKNOWN_268 = 0x268, // some movement ?
		CST_ATI_TASK_ENTER_TRANSPORT = 0x269, // Cf 0x412350.
		CST_ATI_TASK_REPAIR = 0x26A, // Repair a building or a boat
		CST_ATI_TASK_HUMAN_TRAIN_UNIT = 0x26B, // only when triggered by human ?
		CST_ATI_TASK_RESEARCH_TECH = 0x26C, // includes train unit when triggered by AI.
		CST_ATI_TASK_TRANSPORT = 0x26D, // unsure. Includes move+unload. Such unit can't be used as actor in strategy element.
		CST_ATI_TASK_UNKNOWN_26E = 0x26E, // load troops ? see 0x412DF0
		CST_ATI_TASK_UNKNOWN_26F = 0x26F, // see 0x412E60
		CST_ATI_TASK_UNKNOWN_274 = 0x274, // Related to order 0x2D8
		// (699) Stop being targeted by another unit (any action: repair,attack,heal,...).
		// This occurs when the other unit (that targets "me") dies or changes target.
		// Used as a *notification*. Generic_arg4=otherUnitClass. Related to update of unit.unitCountThatAreTargetingMe
		// Some units do NOT raise this event: trees, mines, etc
		CST_ATI_NOTIFY_RELEASE_BEING_WORKED_ON = 0x2BB
	};

	// Represents explicit orders given to units (unitAI).
	enum UNIT_AI_ORDER : long int {
		CST_ORDER_NONE = -1, // Used a lot in game code
		// Starts at 700 (0x2BC)
		CST_ORDER_ATTACK = 0x2BC, // React to agression but also "AI" attacks.
		CST_ORDER_DEFEND_UNIT = 0x2BD, // Defend unit (related to activity.unitIdToDefend) ? Do NOT auto-attack nearby units? See 4DB9F0=tacAI.defend/followUnit?(myUnitId, targetUnitId)
		CST_ORDER_BUILD = 0x2BE, // Build a construction
		CST_ORDER_HEAL = 0x2BF, // For priest: heal an allied unit
		CST_ORDER_CONVERT = 0x2C0, // For priest: convert an enemy
		CST_ORDER_EXPLORE = 0x2C1, // Explore the map
		CST_ORDER_UNKNOWN_2C2 = 0x2C2, // DeTask unit ??
		// 2C3, 2C4: unused ?
		CST_ORDER_GATHER_NOATTACK = 0x2C5, // Gather a target that does not require an "attack" phase: bushes, mines...
		CST_ORDER_UNKNOWN_2C6_ORDER_MOVE = 0x2C6, // Order move ? Used for villagers ? 4DA2BF... Used to retreat to position 4DA160. Used also in attacks grpType0x15? 4DA193
		// 2C7: unused ?
		CST_ORDER_FOLLOW_OBJECT = 0x2C8, // Follow object ? see 0x41363F
		CST_ORDER_GATHER_ATTACK = 0x2C9, // Gather a target that requires an "attack" phase: trees, animals
		CST_ORDER_TRANSPORT_AND_UNLOAD = 0x2CA, // cf activity+8C, 0x4136A0. Sets task=266=regroup to move to target unload position.
		// 2CB, 2CC: unused ?
		CST_ORDER_ENTER_TRANSPORT = 0x2CD, // Enter a transport.
		CST_ORDER_REPAIR = 0x2CE, // Repair. see 0x413595
		// 2CF: unused ?
		// 2D0: unused ?
		CST_ORDER_UNLOAD = 0x2D1, // Unload??? (=transport to...) see 0x4135A7, 0x4E942B(execCmdUnload). When unload manually ????
		CST_ORDER_LOAD_TROOPS = 0x2D2, // transport load (for transport unit) = go to meeting location and wait there ?
		CST_ORDER_UNKNOWN_2D3 = 0x2D3, // 
		CST_ORDER_UNKNOWN_2D4 = 0x2D4, // 0x41363F. Move to TC for artefacts, regroup and/or regroup to transport meeting location ?
		CST_ORDER_HOLD_POSITION = 0x2D5, // Do not auto-attack. See 0x4E6740 for cats (don't search for target), 413F89 for other units (cancel attack if not at range?)
		CST_ORDER_UNKNOWN_2D6 = 0x2D6, // ? see 0x4133DC
		CST_ORDER_UNKNOWN_2D7 = 0x2D7, // ? see 0x4134F8
		CST_ORDER_UNKNOWN_2D8 = 0x2D8, // ? see 0x4136E2. Related to task 0x274
		CST_ORDER_UNKNOWN_2D9_POP_TARGET = 0x2D9 // Similar to attack(2BC). "pop" a target from array ? Used for "attack roundup target" only, cf 0x4DBEF1.
		// 0x2D9 seems to be the last one
	};

}
