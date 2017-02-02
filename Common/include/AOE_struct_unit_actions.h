
#pragma once

#include <assert.h>
#include <AOE_empires_dat.h>
#include <AOE_const_internal.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>

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


#define CHECKSUM_ACTION_BASE 0x005429D8
	// Size=0x40 for parent class D8 29 54 00 (RGE_Action)
	// constructor=406EA0(actor, arg2) / 407050. + 406E40?(fromFile)
	// Note: (some) Actions are created from game commands in 0x4B4600 = unitActionInfo.createActionForCommand(unitDefCommand, arg2, posY, posX, posZ)
	// EDX+28 = action.update()?
	// EDX+5C = action.setStatus?(actionStatus)
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
		STRUCT_UNIT_BASE *targetUnit2; // +14. Secondary target pointer (used for missiles only ?)
		long int targetUnitId; // +18
		long int targetUnitId2; // +1C. A secondary target ? See 0x4073F0=action.setTargetUnit2(arg1). Used for missiles (only ?)
		// 0x20
		float targetUnitPositionY;
		float targetUnitPositionX;
		float targetUnitPositionZ;
		float timer; // +2C. For some actions (attack), it is execution time? For farms, it is "remaining food" (to be added to unit.resValue).
		// 0x30
		STRUCT_UNIT_COMMAND_DEF *command; // +30. Not always used, nullable. For gatherer, it is always set.
		STRUCT_UNIT_ACTION_INFO *requiredActionInfo; // +34. SubAction ? Link with unit/"actionLink"/action. Allows chaining actions ! This is NOT unit->actionInfo !
		unsigned long int pGraphics; // ptr to graphics structure, consistent with unit+10 ? w_lumber, etc
		char unknown_3C; // Flag 0/1, about graphics (about need to refresh graphics ?)
		char unknown_3D[3]; // Unused. Probably.
		// 0x40: not in BASE class ; it has different type/role according to child classes. (seen float, word...)
	};

#define CHECKSUM_ACTION_MOVE 0x005426DC
	// [id 1] Size = 0x44
	// Constructor 0x4052D0: actionMove.constructor(actor, target, maxTargetDistance, pGraphic), 0x4051C0
	class STRUCT_ACTION_MOVE : public STRUCT_ACTION_BASE { // DC 26 54 00
	public:
		// 0x40
		float maxDistanceFromTarget; // The maximum distance from target where we accept to stop movement ? "range_in"

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_ACTION_MOVE); }
		void Constructor(STRUCT_UNIT_BASE *actor, STRUCT_UNIT_BASE *target, float maxTargetDistance, unsigned long int pGraphic) {
			assert(actor && actor->IsCheckSumValidForAUnitClass());
			const unsigned long int addr = 0x4052D0;
			_asm {
				MOV ECX, this;
				PUSH pGraphic;
				PUSH maxTargetDistance;
				PUSH target;
				PUSH actor;
				CALL addr;
			}
		}
	};
	static_assert(sizeof(STRUCT_ACTION_MOVE) == 0x44, "STRUCT_ACTION_MOVE size");

	// [id 3] Size 0x40
	// Constructor = 0x402440
#define CHECKSUM_ACTION_GARRISON_ENTER 0x005424D4
	class STRUCT_ACTION_GARRISON_ENTER : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_GARRISON_ENTER; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_GARRISON_ENTER; }
	};
	static_assert(sizeof(STRUCT_ACTION_GARRISON_ENTER) == 0x40, "STRUCT_ACTION_GARRISON_ENTER size");


	// [id 4] Size 0x40
	// Constructor = 0x402B50
#define CHECKSUM_ACTION_EXPLORE 0x00542538
	class STRUCT_ACTION_EXPLORE : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_EXPLORE; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_EXPLORE; }
	};
	static_assert(sizeof(STRUCT_ACTION_EXPLORE) == 0x40, "STRUCT_ACTION_EXPLORE size");


	// [id 5] Size = 0x44. checksum=98 25 54 00.
	// Constructor= 0x402EF0, 0x402E80(fromFile), 0x402F80
#define CHECKSUM_ACTION_GATHER_WITHOUT_ATTACK 0x00542598
	class STRUCT_ACTION_GATHER_WITHOUT_ATTACK : public STRUCT_ACTION_BASE {
	public:
		short int targetUnitDefId; // +40. DATID1 of target.
		short int unused_42;
		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_ACTION_GATHER_WITHOUT_ATTACK); }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_GATHER_NO_ATTACK; }
	};
	static_assert(sizeof(STRUCT_ACTION_GATHER_WITHOUT_ATTACK) == 0x44, "STRUCT_ACTION_GATHER_WITHOUT_ATTACK size");


	// [id 0x08] Size = 0x54 - Constructor = 0x4048F0 (fromFile), 0x404990
	// Projectile units' actions (move until target or trajectory end is reached)
#define CHECKSUM_ACTION_PROJECTILE 0x0054267C
	class STRUCT_ACTION_PROJECTILE : public STRUCT_ACTION_BASE {
	public:
		char unknown_40[0x54 - 0x40];
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_PROJECTILE; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_MISSILE; }
	};
	static_assert(sizeof(STRUCT_ACTION_PROJECTILE) == 0x54, "STRUCT_ACTION_PROJECTILE size");


#define CHECKSUM_ACTION_ATTACK 0x005423F8
	// [id 9] Size 0x5C
	// Constructor 0x401150 = actionAttack.construct(pUnitActor, pUnitTarget, arg3/4/5, blastRadius, minRange, projectileUnit), 0x401000
	// This action corresponds to giving 1 shot, then action status comes to 1 and a new attack action is created (to confirm)
	class STRUCT_ACTION_ATTACK : public STRUCT_ACTION_BASE { // F8 23 54 00
	public:
		// 0x40
		//short int targetDATID;
		//short int unknown_42;
		unsigned long int unknown_40; // Pointer to graphics ? "move_sprite_in"?
		unsigned long int unknown_44; // "fight_sprite_in" ?
		unsigned long int unknown_48; // "wait_sprite_in" ?
		float unknown_4C; // max distance from target I can attack from ? // "min_range_in"?
		// 0x50
		// missile_id_in (short)? then fire_missile_at_frame_in (short) ?
		long int unknown_50; // Dword
		short int unknown_54;
		short int unknown_56;
		char unknown_58; // set to 0 after attack effect is applied? 401FB7
		char unknown_59; // +59. Init=1 in 4011F1 or 0 in 4011F7
		char unknown_5A; // a counter (decremented...)?
		char unknown_5B; // +5B. unused ?
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_ATTACK; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_ATTACK; }
	};
	static_assert(sizeof(STRUCT_ACTION_ATTACK) == 0x5C, "STRUCT_ACTION_ATTACK size");


#define CHECKSUM_ACTION_FLY_BIRD 0x00542474
	// [id 0x0A] Size 0x40
	// Constructor = 0x4020B0(fromFile)
	class STRUCT_ACTION_FLY_BIRD : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_FLY_BIRD; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_FLY; }
	};
	static_assert(sizeof(STRUCT_ACTION_FLY_BIRD) == 0x40, "STRUCT_ACTION_FLY_BIRD size");


#define CHECKSUM_ACTION_TRANSPORT 0x00542970
	// [id 0x0C] Size 0x40
	// Constructor = 0x4067A0, 0x4067D0
	class STRUCT_ACTION_TRANSPORT : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_TRANSPORT; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_TRANSPORT; }
	};
	static_assert(sizeof(STRUCT_ACTION_TRANSPORT) == 0x40, "STRUCT_ACTION_TRANSPORT size");


#define CHECKSUM_ACTION_PRODUCE_FARM_FOOD 0x0054261C
	// [id 0x15] Size = 0x40. Checksum = 1C 26 54 00. This child class does not really have anything specific compared to base class.
	// Constructor 0x404720, 0x404750.
	// "Farm is depleted" event is detected/triggered at 0x40482D.
	// Warning: farm "total" food is this->resourceValue (+2C) + farmUnit->resourceValue. See GetFarmCurrentTotalFood method.
	class STRUCT_ACTION_PRODUCE_FARM_FOOD : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_PRODUCE_FARM_FOOD; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_MAKE; }
	};

	// [id 0x65] Size 0x40
	// Constructor = 0x4B1390
	// Derives from "0x005429D8" , ccor 0x406E40, 0x4B13C0
#define CHECKSUM_ACTION_BUILD 0x005486BC
	class STRUCT_ACTION_BUILD : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_BUILD; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_BUILD; }
	};
	static_assert(sizeof(STRUCT_ACTION_BUILD) == 0x40, "STRUCT_ACTION_BUILD size");


#define CHECKSUM_ACTION_MAKE_OBJECT 0x005488C4
	// [id 0x66] Size 0x50
	// Constructor 04B4F10, 0x4B4E30
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

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_MAKE_OBJECT; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_MAKE_OBJECT; }
	};
	static_assert(sizeof(STRUCT_ACTION_MAKE_OBJECT) == 0x50, "STRUCT_ACTION_MAKE_OBJECT size");


	// [id 0x67] Size 0x48
	// Constructor 0x4B60B0, 0x4B6010
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
	static_assert(sizeof(STRUCT_ACTION_MAKE_TECH) == 0x48, "STRUCT_ACTION_MAKE_TECH size");


#define CHECKSUM_ACTION_CONVERSION 0x0054871C
	// [id 0x68] Size 0x48
	// Constructor = 0x4B1CB0, 0x4B1C30
	class STRUCT_ACTION_CONVERSION : public STRUCT_ACTION_BASE {
	public:
		//unsigned long int checksum; // 1C 87 54 00
		// 0x40 = targetDATID (word) ?really unsure?
		char unknown_040[0x48 - 0x40];

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_CONVERSION; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_CONVERT; } // really unsure :(
	};

	// [id 0x69] Size 0x40
	// Constructor = 0x4B2C00, 0x4B2BC0
#define CHECKSUM_ACTION_HEAL 0x005487E8
	class STRUCT_ACTION_HEAL : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTION_HEAL; }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_HEAL; } // really unsure :(
	};


	// [id 0x6A] Size = 0x44.
	// Constructor=0x4B55C0, 0x4B5650, 0x4B5550
#define CHECKSUM_ACTION_REPAIR 0x00548924
	class STRUCT_ACTION_REPAIR : public STRUCT_ACTION_BASE {
	public:
		unsigned long int unknown_40; // +40. Default 0
		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_ACTION_REPAIR); }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_REPAIR; }
	};
	static_assert(sizeof(STRUCT_ACTION_REPAIR) == 0x44, "STRUCT_ACTION_REPAIR size");


	// [id 0x6B] Size = 0x40. constructor = 0x4B0DF0, 0x4B0E20, 0x4B0DC0. checksum=D8 29 54 00 (same structure as action base)
	class STRUCT_ACTION_ARTEFACT_CAPTURE : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_ACTION_BASE); }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_ARTIFACT_CAPTURE; }
	};
	static_assert(sizeof(STRUCT_ACTION_ARTEFACT_CAPTURE) == 0x40, "STRUCT_ACTION_ARTEFACT_CAPTURE size");


#define CHECKSUM_ACTION_DISCOVERY_ARTEFACT 0x00548788
	// [id 0x6C] Size = 0x44. checksum=88 87 54 00
	// Constructor = 0x4B27D0
	class STRUCT_ACTION_DISCOVERY_ARTEFACT : public STRUCT_ACTION_BASE {
	public:
		char *discoveredByPlayerTable; // +44. Pointer to array of n bytes (depending on total player count).
		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_ACTION_DISCOVERY_ARTEFACT); }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_DISCOVERY_CAPTURE; }
	};
	static_assert(sizeof(STRUCT_ACTION_DISCOVERY_ARTEFACT) == 0x44, "STRUCT_ACTION_DISCOVERY_ARTEFACT size");


	// [id 0x6E] Size = 0x44. checksum=48 88 54 00.
	// Constructor=0x4B32C0 = actionGatherWithAttack.constructor(actor, defendCmd, targetUnit), 0x4B3250
	// (+34) action->actionInfo has a next action=attack tree (or animal) until it is ready to be gathered ?
	// When target status <=2, the "attack phase" is used (0x4B361E), otherwise gathering phase is done (0x4B363B)
#define CHECKSUM_ACTION_GATHER_WITH_ATTACK 0x00548848
	class STRUCT_ACTION_GATHER_WITH_ATTACK : public STRUCT_ACTION_BASE {
	public:
		long int targetUnitDefId; // +40. Set as a dword in 4B3334
		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_ACTION_GATHER_WITH_ATTACK); }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_GATHER_WITH_ATTACK; }
	};
	static_assert(sizeof(STRUCT_ACTION_GATHER_WITH_ATTACK) == 0x44, "STRUCT_ACTION_GATHER_WITH_ATTACK size");


	// [id 0x6F] Size = 0x48.
	// Constructor=0x4B6520, 0x4B65B0, 0x4B64A0
#define CHECKSUM_ACTION_TRADE 0x005489F0
	class STRUCT_ACTION_TRADE : public STRUCT_ACTION_BASE {
	public:
		float unknown_40; // +40
		float unknown_44; // +44
		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_ACTION_TRADE); }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_TRADE; }
	};
	static_assert(sizeof(STRUCT_ACTION_TRADE) == 0x48, "STRUCT_ACTION_TRADE size");


	// [id 0x78] Size = 0x40. checksum=68 8A 54 00. Constructor 0x4B7220 ?
	// Expected commandType = 0x78 (generate wonder victory)
	// This action sets the timer for victory (according to victory conditions though) and sets visibility to all players
	// "Timer" field starts at 0 and counts the "up time" (since wonder is fully built)
#define CHECKSUM_ACTION_GENERATE_WONDER_VICTORY 0x00548A68
	class STRUCT_ACTION_GENERATE_WONDER_VICTORY : public STRUCT_ACTION_BASE {
	public:
		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_ACTION_GENERATE_WONDER_VICTORY); }
		AOE_CONST_FUNC::UNIT_ACTION_ID GetExpectedInternalActionId() { return AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_GENERATE_WONDER_VICTORY; }
	};
	static_assert(sizeof(STRUCT_ACTION_GENERATE_WONDER_VICTORY) == 0x40, "STRUCT_ACTION_GENERATE_WONDER_VICTORY size");




	// Not very well known. Size = 8 ?
	class STRUCT_ACTION_LINK {
	public:
		STRUCT_ACTION_BASE *actionStruct; // +0. Pointer to an action struct (various types possible) = current action.
		STRUCT_ACTION_LINK *nextActionLink; // +4. Use case: Action to restore at move end.
	};

	// Accessed via unit+0x184 pointer or from action itself
	// Size = 0x0C. Constructor=0x4B4180. Name=TRIBE_Action_List ?
	// Warning: there are 2 usages ("from unit" or "from action")
	// Example: action cut tree has an action.actionLink = "attack tree" (before it can be gathered)
	// Chain example: action hunt elephant => subaction attack elephant => subaction move (close enough) to elephant
	// Note: actions & action chains do NOT handle "resume working" after being attacked, see activity.targetsArray.
	class STRUCT_UNIT_ACTION_INFO {
	public:
		unsigned long int checksum; // A8 88 54 00 (from unit) or 00 26 54 00 (from action)
		STRUCT_UNIT_BASE *ptrUnit;
		STRUCT_ACTION_LINK *ptrActionLink;

		bool IsCheckSumValid() const {
			return (this->checksum == 0x005488A8) ||
				(this->checksum == 0x00542600);
		}

		void AssignAction(STRUCT_ACTION_BASE *action) {
			const unsigned long int addr = 0x404400;
			_asm {
				MOV ECX, this;
				PUSH action;
				CALL addr;
			}
		}
	};
	static_assert(sizeof(STRUCT_UNIT_ACTION_INFO) == 0x0C, "STRUCT_UNIT_ACTION_INFO size");

}
