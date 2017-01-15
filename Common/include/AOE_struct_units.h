
#pragma once

#include <AOE_struct_sprites.h>
#include <AOE_struct_graphics.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_map_tile_info.h>
#include <AOE_struct_unit_activity.h>
//#include <AOE_struct_unit_actions.h>
#include <AOE_struct_map_base_common.h>
#include <AOE_struct_map_visibility_info.h>
#include <AOE_struct_managed_array.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {
	// External dependencies
	class STRUCT_PLAYER;
	class STRUCT_ACTION_BASE;
	class STRUCT_UNIT_ACTION_INFO;


	// Size=4
	class STRUCT_HUMAN_TRAIN_QUEUE_INFO {
	public:
		short int DATID; // Only relevant when unitCount > 0
		short int unitCount;
	};


	// Size 0x10
	// Represents a list of units of the same kind (typically, createable units, etc)
	class STRUCT_PER_TYPE_UNIT_LIST_ELEMENT {
	public:
		STRUCT_UNIT_BASE *unit;
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *previousElement;
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *nextElement;
		unsigned long int unused_0C; // SEEMS to be unused
	};

	// Player's unit-per-type list. Intermediate structure to a list of all units of a given type (example: createables)
	// Size 0x0C. Constructor=0x450FF0(0x00544AD4). "Tribe_Object_List"
	class STRUCT_PER_TYPE_UNIT_LIST_LINK {
	public:
		unsigned long int checksum; // 70 9B 54 00 or D4 4A 54 00
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *lastListElement; // NULL if empty. To loop, use "previous" pointer at each iteration and stop when NULL is found.
		short int listElemCount; // +08
		short int unused_0A;

		bool IsCheckSumValid() { return (this->checksum == 0x00549B70) || (this->checksum == 0x00544AD4); }
	};
	static_assert(sizeof(STRUCT_PER_TYPE_UNIT_LIST_LINK) == 0x0C, "STRUCT_PER_TYPE_UNIT_LIST_LINK size");


	// Size = 0x34. Name=InfluenceMap or "Path" ? Constructor=0x457FE0 ?
	// (free=0x4580A0). An interesting method: 0x4581B0=moveInfo.replaceByNewDestination(STRUCT_POSITION_INFO*)
	// About movement/path finding
	class STRUCT_UNIT_MOVEMENT_INFO {
	public:
		unsigned long int unknown_00;
		unsigned long int unknown_04; // targetPosY (float) ?
		unsigned long int unknown_08; // targetPosX (float) ?
		unsigned long int unknown_0C; // targetPosZ (float) ?
		// 0x10
		unsigned long int unknown_10; // unknown type/size
		unsigned long int unknown_14;
		unsigned long int unknown_18;
		unsigned long int unknown_1C;
		// 0x20
		unsigned long int unknown_20; // unknown type/size
		STRUCT_WAYPOINT *intermediatePositions; // +24
		long int intermediatePositionsArraySize; // +28
		long int intermediatePositionsUsedElementsCount; // +2C
		// 0x30
		long int intermediatePositionsCurrentIndex; // +30. To confirm. See 4583D0
	};



	// Eye candy (type10) / common (base) class for all units (unit instances). Name=RGE_Static_Object
	// A8 7D 54 00. Size=0x88 (constructor 0x04A64B0)
	// 0x4CD2D0 = unitGroupElem.task(tacAI, mainAI, unitGrpTaskId, resetOrg, arg5=oCA)
	// Methods:
	// +0x38 = unit.setGraphics(pGraphics)
	// +0x44 = unit.convertToPlayer(ptrPlayer)
	// +x05C = unit.setStatus(newStatus). Contains a hack to revive relics/ruins because of relics in a dying transport boat.
	// +0x6C = unit.killAndHandleSacrifice(). Does not delete units if they have resources ! (trees, mines, etc)
	// +0x74 = unit.CalcDamageFrom(attacksCount, pAttacksList, f_altitudeFactor, actorPlayer, actorUnit). Ex 0x426910.
	// +0x84 = unit.setResourceValue?(f_value, isAdd, checkResourceCapacity)
	// 0x+9C = unit.createMoveToAction(targetUnit, fposY, fposX, fposZ)
	// +0xD8 = unit.addVisibilityTo(playerToImpact, arg2, distance?)
	// +0xDC = unit.removeVisibilityFor(playerToImpact, arg2, distance?)
	// +0xEC = unit.setIsUnderAttack(bool)
	// +0xF0 = unit.GetAttackAltitudeFactor(targetUnit). WARNING: Returns in ST (FLD xxx), not EAX. Ex 0x4AEC00.
	// +0xFC = unit.FLD_speed()
	// +0x100 = unit.FLD_reloadTime1()
	// +0x104 = unit.calcDamageFrom(unit)?. WARNING: Returns in ST (FLD xxx), not EAX. Ex 0x427AC0.
	// +0x10C = unit.FLD_maxRange(). Loads 0 for units that do not derive from type50 (attackable)
	// +0x14C = unit.attackPosition(fposY, fposX, arg3, arg4)
	// +0x150 = unit.setAttackAction?(targetUnitId, force)
	// +0x154 = unit.??(fposY, fposX, arg3, arg4, force?) create action move ?
	// +0x164 = unit.goGather(targetUnitId, force?) returns 1 on success
	// +0x17C = unit.Explore(posY, posX, arg3)
	// +0x194 = unit.CanAttackUnit???(targetUnitId, fRange, arg3, arg4, arg5, arg6)
	// +0x198 = CanMoveTo(posY,posX,posZ,fMaxRange,targetUnitId,pArg6_float,arg7_size?,targetPlayerId,someUnitClass) ? returns 1 if ok. someUnitClass=1B=walls
	// +0x1A0 = MoveToTarget?(targetUnitId,fMaxRange,arg3,arg4,arg5,arg6,arg7) ? arg6=value for UNKNOWN_MAP_DATA_F04C+0x11DCDC arg4=(0=use0x6A1CC0, 1=use0x583BC8) arg5=unitGroup??
	// +0x1A4 = DoMove?(posY,posX,posZ,fMaxRange,targetUnitId, pArg6_float,arg7_size,arg8,arg9,targetPlayerId,someUnitClass)
	// +0x1CC = unit.xxx(pPosYXZ_target, fRange, targetUnitId) for intelligent attack ?
	// +0x1E4 = unit.createSpriteList(ptrUnit)
	// +0x208 = unit.assignAction(action). ex: 0x406490.
	// +0x210 = unit.idIdle() to confirm. ex: 0x406610.
	// +0x244 = unit.kill(). ex: 0x4ED6D0. Not for all unit types !
	// +0x254 = unit.createUnitActivity(). ("combatUnit.initUnitAI") Only called for living units only (types 70/80). Does not free previous activity, if any !
	class STRUCT_UNIT_BASE {
	public:
		unsigned long int checksum;
		long int unitInstanceId; // Can be negative (temporary units like smoke). Can be something else (or unused?) (seen for flare, type=10)
		STRUCT_UNITDEF_BASE *unitDefinition; // +8. Unit definition (model). Living units can have a dedicated unit definition.
		STRUCT_PLAYER *ptrStructPlayer;
		// 0x10
		STRUCT_GRAPHICS *pCurrentGraphics; // +10
		unsigned long int unknown_014;
		STRUCT_ACTIVE_SPRITE_LIST *spriteList; // ptr to struct 00 30 54 00 - size=0x0C Related to graphics
		STRUCT_GAME_MAP_TILE_INFO *myTile; // +1C. To confirm. +5=byte=altitude&terrain bits, see TERRAIN_BYTE class
		// 0x20
		STRUCT_UNIT_BASE *transporterUnit; // Transport boat the unit is in
		STRUCT_PER_TYPE_UNIT_LIST_LINK *transportedUnits; // +24. List of units in the transport (if "this" is a transport)
		short int unknown_028;
		short int unknown_02A; // for graphics ?
		short int unknown_02C; // for graphics ?
		short int unknown_02E; // for graphics ?
		// 0x30
		float remainingHitPoints;
		char currentGraphicsDamageIndex; // +34. According to remaining HP ? A percentage value (0-100)?
		char orientationIndex; // +35. Also used as building step (<25%, <50%, <75%, >75%). For types 30+, step orientationAngle(+9C), not this. See 0x44BBD0(unit+[0xD4]).
		char mouseSelectionStatus; // +36. 1=selected, 2=right-click-target. Is it a Mask, not an enum !
		char shortcutNumber; // 0-10, 10+=regrouped units ; 0=none, 10=0x0A="0" key.
		float positionY; // (+38) bottom-left to upper-right
		float positionX; // (+3C) top-left to bottom-right
		// 0x40
		float positionZ; // Read only, because it's updated automatically according to map position(X,Y)
		float resourceValue; // See resourceValue. Faith (0-100) for priests, food for hunter...
		AOE_CONST_INTERNAL::GAME_UNIT_STATUS unitStatus; // 0=being_built, 2=ready, at least for buildings... Values=0,1,2,3,5,7,8=max? 5=waitingForResourceDepletion.
		char isNotCreatable; // +49. 1 if the unit is a "not-creatable" unit, like cliffs, etc. Always 1 in parent constructors, set to 0 in "creatable-level" constructor.
		char isDoppleGanger; // +4A. Is temp unit ? Not 100% sure.
		char unknown_04B;
		short int resourceTypeId; // A unit can hold only 1 resource. =0x22=34 for priest.
		char unitType; // +4E. Warning: this value is often WRONG. See unitDef value instead. Only 1-byte here, whereas it is 4-bytes in unitDef struct. 70=living, etc. See GLOBAL_UNIT_TYPES
		char unitCountThatAreTargetingMe; // +4F. Number of (other)units that have "me" as target (repairman, gatherer, attacker...). See 4AADB0. Warning: the count is not reliable: when moving to it, the other unit counts TWICE (and counts for 1 when actually doing the action)
		// 0x50
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *ptrElemInPlayerUnitList; // +50. Link to corresponding reference in player's creatable units list. Can be NULL in saved games (if not "used" yet ?)
		STRUCT_MANAGED_ARRAY unknown_054; // +54 a ptr. about movement ? objectCollisionList ? Units in same Formation ? A list of unitID.
		STRUCT_MANAGED_ARRAY unitIDsInMyGroup; // +64. list of IDs of the units in same group as me. A limit to 25 in 0x4ABEC8 (beware stack overflow)
		STRUCT_UNIT_ACTIVITY *currentActivity; // +74. Called "UnitAI" in ROR code. Warning: some unit don't have one (e.g. lion tame)
		long int groupLeaderUnitId; // +78. Unit Id of my group's leader. -1 when unitIDsInMyGroup is empty.
		long int terrainZoneInfoIndex; // +7C. Index of terrainZoneInfo in terrainZoneInfoLink array. -1 means not initialized yet.
		// 0x80
		float unknown_080;
		char unknown_084; // related to movement ? "isMapInfoUpToDateForThisUnit" ?
		char unknown_085; // related to movement ? Previous value of +84 ?
		char isBeingAttacked; // +86. For map blinking (+other treatments?)? This is constantly reset and re-computed in "unit.timerUpdate(?)" 0x426BF0 treatments.
		char unknown_087;

		bool IsCheckSumValid() const { return (this->checksum == 0x00547DA8); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_EYE_CANDY); }
		// Returns true if checksum is valid for this class OR a child class
		bool IsCheckSumValidForAUnitClass() const {
			return (this->checksum == 0x00547DA8) || // eye candy (including cliffs...)
				(this->checksum == 0x00542E14) || // Flag
				(this->checksum == 0x005441BC) || // Doppleganger
				(this->checksum == 0x00544838) || // Movable (Dead/fish)
				(this->checksum == 0x00542748) || // Commandable (Birds)
				(this->checksum == 0x0054324C) || // Type50
				(this->checksum == 0x005445E8) || // Projectiles
				(this->checksum == 0x0054820C) || // living
				(this->checksum == 0x00547FA0) || // Building
				(this->checksum == 0x00548474) // Tree
				;
		}
		// Returns true if the unit is a flag (20) or a child class (all but eye candy and trees)
		bool DerivesFromFlag() const {
			return (this->IsCheckSumValidForAUnitClass() && (this->checksum != 0x00547DA8) && (this->checksum != 0x00548474)); // all but 10 and 90
		}
		// Returns true if the unit is movable (30 - deadfish in AGE3) or a child class
		bool DerivesFromMovable() const {
			return this->DerivesFromCommandable() || (this->checksum == 0x00544838);
		}
		// Returns true if the unit is a commandable (40 - bird in AGE3) or a child class
		bool DerivesFromCommandable() const {
			return this->DerivesFromAttackable() || (this->checksum == 0x00542748);
		}
		// Returns true if the unit is a living unit or a child class (building)
		bool DerivesFromTrainable() const {
			return (this->checksum == 0x0054820C) || // living
				(this->checksum == 0x00547FA0); // Building
		}
		// Returns true if the unit is type50 or one of its child classes (projectile, living/building).
		bool DerivesFromAttackable() const {
			return (this->checksum == 0x0054324C) || // Type50
				(this->checksum == 0x005445E8) || // Projectiles
				(this->checksum == 0x0054820C) || // living
				(this->checksum == 0x00547FA0); // Building
		}
		// Deletes a units by calling destructor. You should provide freeMemory=true
		// AOE destructor calls all necessary underlying updates of removing the unit (map, player, AI...)
		// Unit will disappear without animation.
		void AOE_destructor(bool freeMemory) {
			long int doFree = freeMemory ? 1 : 0;
			unsigned long int myaddr = (unsigned long int)this;
			_asm {
				MOV ECX, myaddr;
				MOV EDX, DS:[ECX];
				PUSH doFree;
				CALL DS : [EDX];
			}
		}

		// Returns the ID of underlying terrain zone (identifies the continent/island or lake/sea where I currently belong)
		// This allows to distinguish locations/units I can travel to (for example, a villager can only walk to its island - unless there are shallows)
		// Accessible destinations are tiles with the same terrain zone ID
		// Warning: the returned terrainZoneId has only a meaning for THIS unit (or units with same terrain accessibility)
		// This method is NOT const. If unit underlying zoneInfo index has not been computed yet, if will be done here.
		char GetMyTerrainZoneId() {
			unsigned long int addr = 0x4ABA60;
			char result;
			_asm {
				MOV ECX, this;
				CALL addr;
				MOV result, AL; // Warning: returned value is a byte (EAX high bytes are undetermined)
			}
			return result;
		}

		// Returns the ID of terrain zone at a specific location (from MY terrain restriction point of view)
		// This method is NOT const. If unit underlying zoneInfo index has not been computed yet, if will be done here.
		char GetTerrainZoneIdAtPos(long int x, long int y) {
			unsigned long int addr = 0x4AB9E0;
			char result;
			_asm {
				MOV ECX, this;
				PUSH x;
				PUSH y;
				CALL addr;
				MOV result, AL; // Warning: returned value is a byte (EAX high bytes are undetermined)
			}
			return result;
		}

		// Get unit max range, returns 0 for types that do not derive from Type50 (attackable).
		// This is analog to "EDX+0x10C" call
		float GetMaxRange() const {
			if (!this->DerivesFromAttackable()) {
				return 0.f;
			}
			return ((STRUCT_UNITDEF_ATTACKABLE*)this->unitDefinition)->maxRange;
		}

		// Returns true if unit can move to target. Still not well known
		long int CanMoveTo(long int targetUnitId, float maxRange, long int arg3, long int arg4, long int arg5, long int arg6) {
			long int result;
			_asm {
				MOV ECX, this;
				MOV EDX, DS:[ECX];
				PUSH arg6;
				PUSH arg5;
				PUSH arg4;
				PUSH arg3;
				PUSH maxRange;
				PUSH targetUnitId;
				CALL DS:[EDX + 0x194];
				MOV result, EAX;
			}
			return result;
		}
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE) == 0x88, "STRUCT_UNIT_BASE size");



	// 14 2E 54 00 = flag (type20). Size=0x8C. constructor=0x0415020 - derives from type10
	class STRUCT_UNIT_FLAG : public STRUCT_UNIT_BASE {
	public:
		float currentMovementSpeed; // +88

		bool IsCheckSumValid() { return (this->checksum == 0x00542E14); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_FLAGS); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_FLAG) == 0x8C, "STRUCT_UNIT_FLAG size");


	// BC 41 54 00 = doppleganger (type25). Size=0xB0 (constructor=0x0436030) - derives from type20
	class STRUCT_UNIT_DOPPLEGANGER : public STRUCT_UNIT_FLAG {
	public:
		unsigned long int unknown_08C;
		// 0x90
		unsigned long int unknown_90_color; // ptr to struct 0C 32 54 00 (+4=colorName). +28=sub-struct
		char unknown_94[0xA4 - 0x94];
		unsigned long int unknown_A4; // bool ?
		char unknown_A8; // value 1-3 ?
		char unknown_A9; // Seen 0xDF ?
		char unknown_AA[0xB0 - 0xAA];

		bool IsCheckSumValid() { return (this->checksum == 0x005441BC); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_DOPPLEGANGER); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_DOPPLEGANGER) == 0xB0, "STRUCT_UNIT_DOPPLEGANGER size");


	// 38 48 54 00 = Movable (type30 - deadfish in AGE3). Size=0x180 (constructor=0x044AD30) - Derives from type20
	class STRUCT_UNIT_MOVABLE : public STRUCT_UNIT_FLAG {
	public:
		unsigned long int unknown_08C;
		// 0x90
		float velocityY; // +90. Used for movement computation ? Used for missiles.
		float velocityX; // +94. Used for movement computation ? Used for missiles.
		float velocityZ; // +98.
		float orientationAngle; // +9C. unit orientation angle. Updating this impacts unit.orientationIndex (+0x35). Angle [0, 2*PI[. 0=heading to northEast (increasing Y). Warning: walking graphics need to have 8 angles to allow valid movement.
		unsigned long int unknown_0A0; // +A0. "turn_towards_time" ?
		STRUCT_UNIT_MOVEMENT_INFO movementInfo; // +A4. Movement info / path finding.
		STRUCT_UNIT_MOVEMENT_INFO temp_AI_movementInfo; // +D8. Only used in AI treatments(?) to make checks before assigning tasks ? Used when unknown_154_tempFlag_calculatingPath=1
		float unknown_10C; // +10C.
		// 0x110
		float move_initialPosX; // wrong ?
		float move_initialPosZ; // wrong ?
		unsigned long int unknown_118;
		float move_targetPosY; // updated from action struct on action updates. Set in 0x44BE90
		// 0x120
		float move_targetPosX;
		float move_targetPosZ;
		unsigned long int unknown_128;
		unsigned long int unknown_12C;
		// 0x130
		unsigned long int unknown_130;
		float move_distanceFromTargetToReach; // +134. The maximum distance to target to reach before movement is considered done. "Action range"
		long int move_targetUnitId; // +138. Set in 0x44BED0
		float targetSizeRadiusY; // +13C. Set in 0x44BEE0
		// 0x140
		float targetSizeRadiusX;
		long int unknown_144; // =targetPositionsArrayUsedElements+0xF ?
		char unknown_148[0x154 - 0x148];
		char unknown_154_tempFlag_calculatingPath; // 1 while in treatment (computing a move path ?). If 1, use temp_AI_movementInfo instead of movementInfo ?
		char unknown_155; // Related to action, consistent with action+0xC ? (used when action+C=0 or no action?)
		short int unknown_156; // or 2 bytes ?
		long int unknown_158_posY1;
		long int unknown_15C_posX1;
		long int unknown_160_posY2;
		long int unknown_164_posX2;
		STRUCT_UNIT_TARGET_POS *targetPositionsArray; // +168. ptr to list of 3-bytes elements posYXZ, related to movement. See 44EB70 (add pos in array)
		long int targetPositionsArrayUsedElements; // size of unknown_168_ptrList
		// 0x170
		unsigned long int targetPositionsArrayTotalSize; // allocated number of elements (elemSize=3, warning, it is unusual).
		char isInMovement; // +174. Not sure
		char unknown_175;
		char unknown_176; // ?
		char unknown_177; // ?
		unsigned long int unknown_178;
		char unknown_17C;// orientation index ?
		char unknown_17D; // orientation index ?
		char unknown_17E[2];

		bool IsCheckSumValid() { return (this->checksum == 0x00544838); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_MOVABLE); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_MOVABLE) == 0x180, "STRUCT_UNIT_MOVABLE size");


	// 48 27 54 00 = commandable (type40 - bird in AGE3). Size=0x18C (constructor=0x04058A0) - Derives from type30
	class STRUCT_UNIT_COMMANDABLE : public STRUCT_UNIT_MOVABLE {
	public:
		// 0x180
		unsigned long int unknown_180;
		STRUCT_UNIT_ACTION_INFO *ptrActionInformation; // +184. Useful to retrieve unit's action.
		long int rightClickActionCounter_unsure; // +188. Incremented each time a right-click action is done by player ?

		bool IsCheckSumValid() { return (this->checksum == 0x00542748); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_COMMANDABLE); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE) == 0x18C, "STRUCT_UNIT_COMMANDABLE size");


	// 4C 32 54 00 = attackable (type50). Size=0x1BC (constructor=00425F30) - Derives from type40
	class STRUCT_UNIT_ATTACKABLE : public STRUCT_UNIT_COMMANDABLE {
	public:
		STRUCT_NEARBY_UNIT_INFO *myVisibilityToOtherPlayers_unsure[9]; // +18C. Provides visibility info from other player's point of view ?
		// 0x1B0
		STRUCT_MAP_VISIBILITY_INFO unknownVisibility_1B0; // Same "nature" object as +0x1E4. A mask for map visibility (visible for ...)?
		float unknown_1B4; // Time to next xxx?
		char stillToBeDiscoveredByHuman; // +1B8. 1 for gaia units that have not been discovered yet (for gaia units capture system). Only non-AI players can capture gaia units.
		char unknown_1B9[3]; // unused?

		bool IsCheckSumValid() const { return (this->checksum == 0x0054324C); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_ATTACKABLE); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE) == 0x1BC, "STRUCT_UNIT_ATTACKABLE size");


	// E8 45 54 00 = projectile (type60). Size=1C4 (constructor=0x0443D30) - Derives from type50
	class STRUCT_UNIT_PROJECTILE : public STRUCT_UNIT_ATTACKABLE {
	public:
		char unknown_1BC; // byte
		char unknown_1BD;
		short int unknown_1BE;
		// 0x1C0
		unsigned long int unknown_1C0;

		bool IsCheckSumValid() const { return (this->checksum == 0x005445E8); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_PROJECTILE); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE) == 0x1C4, "STRUCT_UNIT_PROJECTILE size");


	// 0C 82 54 00 = trainable (type70 - living in AGE3). Size=0x1C0 - Derives from type50
	// Constructor=0x4AE2D0=livingUnit.constructor(UnitDef, player, posY, posX, posZ?,arg6)
	// Constructor=0x4AE380 (internalFileId, global, isLeafClass), uses 0x4AE5C0=livingUnit.readFromFile(internalFileId, global)
	class STRUCT_UNIT_TRAINABLE : public STRUCT_UNIT_ATTACKABLE {
	public:
		char hasDedicatedUnitDef; // +1BC. If true, this->unitDef is a "temporary" copy created when unit was converted, which means it must be freed when unit dies (or changes, for villager mode). See 0x4AE435 for delete.
		char unknown_1BD;
		short int unknown_1BE;

		bool IsCheckSumValid() const { return (this->checksum == 0x0054820C); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TRAINABLE); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE) == 0x1C0, "STRUCT_UNIT_TRAINABLE size");


	// A0 7F 54 00 = building (type80). Size=0x1FC - Derives from type70
	// Constructor=0x4AC3F0=bldUnit.constructor(7 args)
	// Constructor=0x4AC550=bldUnit.constructor(defUnit, player, posY, posX, posZ, arg6)
	// Constructor=0x4AC670=bldUnit.constructor(internalFileId, global, isLeafClass)
	class STRUCT_UNIT_BUILDING : public STRUCT_UNIT_TRAINABLE {
	public:
		// 0x1C0
		//short int *ptrHumanTrainQueueInformation; // [+00]=Word=unit_DAT_ID, [+02]=trainQueueCount, if 0 then DATID has no meaning.
		STRUCT_HUMAN_TRAIN_QUEUE_INFO *ptrHumanTrainQueueInformation; // +1C0. Only relevant for human-triggered "train unit".
		short int unknown_1C4; // Max number of different units in queue ?
		short int trainUnitQueueCurrentElemCount; // +1C6. Number of different queued unit types. Warning: it is >=1 only when TRAINING a unit that was "human-asked" ? This flag does NOT mean "busy" !
		unsigned long int unknown_1C8;
		STRUCT_UNIT_ACTION_INFO *trainUnitActionInfo; // +1CC. Only for makeobject actions (not research).
		// 0x1D0
		char queueActionsCounter; // Increments when I click to train/enqueue a unit or cancel it. (if I cancel 3 queued units, it decreases by 3)
		char unknown_1D1;
		short int unknown_1D2; // see 004ADE3C, 4AE1C3. Values -1, 4 ?
		float constructionProgress;
		long int strategyElementId; // +1D8. Strategy element ID (building's) : used to update strategy element when construction ends.
		char tileset; // +1DC.
		char unknown_1DD; // value = 0 or 100 (not sure)? Switched when construction ends?
		short int unknown_1DE;
		// 0x1E0
		unsigned long int unknown_1E0;
		STRUCT_MAP_VISIBILITY_INFO unknown_1E4; // Do not modify (automatically refreshed)
		STRUCT_MAP_VISIBILITY_INFO unknown_1E8; // Do not modify (automatically refreshed)
		STRUCT_MAP_VISIBILITY_INFO unknown_1EC; // Do not modify (automatically refreshed)
		// 0x1F0
		unsigned long int unknown_1F0;
		unsigned long int unknown_1F4;
		unsigned long int unknown_1F8;

		// Returns true if checksum is valid FOR A BUILDING only.
		bool IsCheckSumValid() const { return (this->checksum == 0x00547FA0); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING) == 0x1FC, "STRUCT_UNIT_BUILDING size");


	// 74 84 54 00 = tree (type90). Size=0x88 (constructor=0x04B0D20) - Derives from type10
	class STRUCT_UNIT_TREE : public STRUCT_UNIT_BASE {
	public:
		bool IsCheckSumValid() const { return (this->checksum == 0x00548474); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TREE); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_TREE) == 0x88, "STRUCT_UNIT_TREE size");


	static long int GetUnitStructRealSize(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
		if (unit->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE);
		if (((STRUCT_UNIT_COMMANDABLE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE);
		if (((STRUCT_UNIT_FLAG*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_FLAG);
		if (((STRUCT_UNIT_MOVABLE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_MOVABLE);
		if (((STRUCT_UNIT_DOPPLEGANGER*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DOPPLEGANGER);
		if (((STRUCT_UNIT_TRAINABLE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE);
		if (((STRUCT_UNIT_PROJECTILE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE);
		if (((STRUCT_UNIT_TREE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_TREE);
		if (((STRUCT_UNIT_ATTACKABLE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE);
		if (((STRUCT_UNIT_BUILDING*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING);
		// unit->unitType is often erroneous ! If not found, try again using only checksum.
		if (unit->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE);
		if (((STRUCT_UNIT_COMMANDABLE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE);
		if (((STRUCT_UNIT_FLAG*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_FLAG);
		if (((STRUCT_UNIT_MOVABLE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_MOVABLE);
		if (((STRUCT_UNIT_DOPPLEGANGER*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DOPPLEGANGER);
		if (((STRUCT_UNIT_TRAINABLE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE);
		if (((STRUCT_UNIT_PROJECTILE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE);
		if (((STRUCT_UNIT_TREE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_TREE);
		if (((STRUCT_UNIT_ATTACKABLE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE);
		if (((STRUCT_UNIT_BUILDING*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING);
		return 0;
	}

}
