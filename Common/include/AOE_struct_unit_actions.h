
#pragma once

#include <AOE_empires_dat.h>
#include <AOE_const_internal.h>
#include <AOE_struct_unit_def.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// External dependencies
	class STRUCT_ACTION_BASE;
	class STRUCT_UNIT_BASE;
	
	// Pre-declaration for circular dependencies
	class STRUCT_UNIT_ACTION_INFO;


	// Size=0x40 for parent class D8 29 54 00 (*seems* to be the common parent)
	// constructor=406EA0 / 407050.
	class STRUCT_ACTION_BASE {
	public:
		unsigned long int checksum;
		AOE_CONST_FUNC::UNIT_ACTION_ID actionTypeID;
		short int unknown_006;
		STRUCT_UNIT_BASE *actor;
		AOE_CONST_INTERNAL::ACTION_STATUS actionStatus; // +0xC (byte)
		char unused[3];
		// 0x10
		STRUCT_UNIT_BASE *targetUnit;
		unsigned long int unknown_014; // actor unit pointer ???
		long int targetUnitId; // +18
		long int unknown_unitId; // +1C. A secondary target ?
		// 0x20
		float targetUnitPositionY;
		float targetUnitPositionX;
		float targetUnitPositionZ;
		float unsure_resourceValue; // For some actions (attack), it is execution time? For farms, it is "remaining food" (to be added to unit.resValue).
		// 0x30
		STRUCT_UNIT_COMMAND_DEF *command; // +30. Not always used, nullable. For gatherer, it is always set.
		STRUCT_UNIT_ACTION_INFO *requiredActionInfo; // +34. SubAction ? Link with unit/"actionLink"/action. Allows chaining actions ! This is NOT unit->actionInfo !
		unsigned long int pGraphics; // ptr to graphics structure, consistent with unit+10 ? w_lumber, etc
		char unknown_3C; // Flag 0/1, about graphics (about need to refresh graphics ?)
		char unknown_3D[3]; // Unused. Probably.
		// 0x40: not in BASE class ; it has different type/role according to child classes. (seen float, word...)
	};

	// Size = 0x44
	// Constructor 4052D0: actionMove.constructor(actor, target, maxTargetDistance, pGraphic)
	class STRUCT_ACTION_MOVE : public STRUCT_ACTION_BASE { // DC 26 54 00
	public:
		// 0x40
		float maxDistanceFromTarget; // The maximum distance from target where we accept to stop movement ?
	};
	static_assert(sizeof(STRUCT_ACTION_MOVE) == 0x44, "STRUCT_ACTION_MOVE size");

	// Size 0x5C
	// Constructor 0x401150 = actionAttack.construct(pUnitActor, pUnitTarget, arg3/4/5, blastRadius, minRange, projectileUnit)
	class STRUCT_ACTION_ATTACK : public STRUCT_ACTION_BASE { // F8 23 54 00
	public:
		// 0x40
		//short int targetDATID;
		//short int unknown_42;
		unsigned long int unknown_40; // Pointer to graphics ?
		unsigned long int unknown_44;
		unsigned long int unknown_48;
		float unknown_4C; // max distance from target I can attack from ?
		// 0x50
		long int unknown_50; // Dword
		short int unknown_54;
		short int unknown_56;
		char unknown_58; // set to 0 after attack effect is applied? 401FB7
		char unknown_59; // +59. Init=1 in 4011F1 or 0 in 4011F7
		char unknown_5A; // a counter (decremented...)?
		char unknown_5B;
		bool IsCheckSumValid() { return this->checksum == 0x005423F8; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_ATTACK_9; } // really unsure :(
	};
	static_assert(sizeof(STRUCT_ACTION_ATTACK) == 0x5C, "STRUCT_ACTION_ATTACK size");

	// Size 0x48
	// Constructor = 4B1CB0
	class STRUCT_ACTION_CONVERSION : public STRUCT_ACTION_BASE {
	public:
		//unsigned long int checksum; // 1C 87 54 00
		// 0x40 = targetDATID (word) ?really unsure?
		char unknown_040[0x48 - 0x40];

		bool IsCheckSumValid() { return this->checksum == 0x0054871C; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_CONVERT; } // really unsure :(
	};

	// Size 0x50
	// Constructor 04B4F10
	class STRUCT_ACTION_MAKE_OBJECT : public STRUCT_ACTION_BASE { // Train unit...
	public:
		//unsigned long int checksum; // C4 88 54 00
		// 0x40
		short int targetUnitDAT_ID; // being trained unit dat_id
		short int unknown_042;
		float currentTrainTime; // +44. To confirm
		long int strategyElementId; // +48. value from command +0x0C ?
		char unknown_04C;
		char unknown_04D[3]; // unused ?

		bool IsCheckSumValid() { return this->checksum == 0x005488C4; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_MAKE_OBJECT; }
	};

	// Size 0x48
	// Constructor 0x4B60B0
	class STRUCT_ACTION_MAKE_TECH : public STRUCT_ACTION_BASE {
	public:
		//unsigned long int checksum; // 90 89 54 00 ?
		// 0x40
		short int researchID;
		short int unknown_42; // Unused... probably
		long int strategyElementId; // +44

		bool IsCheckSumValid() { return this->checksum == 0x00548990; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_MAKE_TECH; }
	};

	// Size = 0x44. checksum=88 87 54 00
	// Expected commandType = 0x6C (discovery)
	class STRUCT_ACTION_DISCOVERY_ARTEFACT : public STRUCT_ACTION_BASE {
	public:
		char *discoveredByPlayerTable; // +44. Pointer to array of n bytes (depending on total player count).
		bool IsCheckSumValid() { return (this->checksum == 0x00548788); }
	};

	// Size = 0x44. checksum=48 88 54 00.
	// Constructor=0x4B32C0 = actionGatherWithAttack.constructor(actor, defendCmd, targetUnit)
	// (+34) action->actionInfo has a next action=attack tree (or animal) until it is ready to be gathered ?
#define CHECKSUM_ACTION_GATHER_WITH_ATTACK 0x00548848
	class STRUCT_ACTION_GATHER_WITH_ATTACK : public STRUCT_ACTION_BASE {
	public:
		long int targetUnitDefId; // +40. Set as a dword in 4B3334
		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_ACTION_GATHER_WITH_ATTACK); }
	};
	static_assert(sizeof(STRUCT_ACTION_GATHER_WITH_ATTACK) == 0x44, "STRUCT_ACTION_GATHER_WITH_ATTACK size");

	// Size = 0x44. checksum=98 25 54 00.
	// Constructor= 0x402EF0
#define CHECKSUM_ACTION_GATHER_WITHOUT_ATTACK 0x00542598
	class STRUCT_ACTION_GATHER_WITHOUT_ATTACK : public STRUCT_ACTION_BASE {
	public:
		short int targetUnitDefId; // +40. DATID1 of target.
		short int unused_42;
		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_ACTION_GATHER_WITHOUT_ATTACK); }
	};
	static_assert(sizeof(STRUCT_ACTION_GATHER_WITHOUT_ATTACK) == 0x44, "STRUCT_ACTION_GATHER_WITHOUT_ATTACK size");


#define CHECKSUM_ACTION_PRODUCE_FARM_FOOD 0x0054261C
	// Size = 0x40. Checksum = 1C 26 54 00. This child class does not really have anything specific compared to base class.
	// "Farm is depleted" event is detected/triggered at 0x40482D.
	// Warning: farm "total" food is this->resourceValue (+2C) + farmUnit->resourceValue. See GetFarmCurrentTotalFood method.
	class STRUCT_ACTION_PRODUCE_FARM_FOOD : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_PRODUCE_FARM_FOOD; }
	};


	// Size = 0x54 - Constructor=0x404990
	// Projectiles' actions (move until target or trajectory end is reached)
#define CHECKSUM_ACTION_PROJECTILE 0x0054267C
	class STRUCT_ACTION_PROJECTILE : STRUCT_ACTION_BASE {
	public:
		char unknown_40[0x54 - 0x40];
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_PROJECTILE; }
	};
	static_assert(sizeof(STRUCT_ACTION_PROJECTILE) == 0x54, "STRUCT_ACTION_PROJECTILE size");



	// Not very well known. Size = 8 ?
	class STRUCT_ACTION_LINK {
	public:
		STRUCT_ACTION_BASE *actionStruct; // +0. Pointer to an action struct (various types possible) = current action.
		STRUCT_ACTION_LINK *nextActionLink; // +4. Use case: Action to restore at move end.
	};

	// Accessed via unit+0x184 pointer or from action itself
	// Size = 0x0C.
	// Warning: there are 2 usages ("from unit" or "from action")
	// Example: action cut tree has an action.actionLink = "attack tree" (before it can be gathered)
	// Chain example: action hunt elephant => subaction attack elephant => subaction move (close enough) to elephant
	// Note: actions & action chains do NOT handle "resume working" after being attacked, see activity.targetsArray.
	class STRUCT_UNIT_ACTION_INFO {
	public:
		unsigned long int checksum; // A8 88 54 00 (from unit) or 00 26 54 00 (from action)
		STRUCT_UNIT_BASE *ptrUnit;
		STRUCT_ACTION_LINK *ptrActionLink;

		bool IsCheckSumValid() {
			return (this->checksum == 0x005488A8) ||
				(this->checksum == 0x00542600);
		}
	};


}
