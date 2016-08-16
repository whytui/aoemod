
#pragma once

#include <AOE_struct_unit_def.h>
#include <AOE_struct_map_tile_info.h>
#include <AOE_struct_unit_activity.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_map_base_common.h>
#include <AOE_struct_map_visibility_info.h>

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
	// Size 0x0C
	class STRUCT_PER_TYPE_UNIT_LIST_LINK {
	public:
		unsigned long int checksum; // 70 9B 54 00 or D4 4A 54 00
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *lastListElement;
		short int listElemCount; // +08
		short int unused_0A;

		bool IsCheckSumValid() { return (this->checksum == 0x00549B70) || (this->checksum == 0x00544AD4); }
	};
	static_assert(sizeof(STRUCT_PER_TYPE_UNIT_LIST_LINK) == 0x0C, "STRUCT_PER_TYPE_UNIT_LIST_LINK size");


	// Size = 0x34
	// (free=0x4580A0). An interesting method: 0x4581B0=moveInfo.replaceByNewDestination(STRUCT_POSITION_INFO*)
	// About movement/path finding
	class STRUCT_UNIT_MOVEMENT_INFO {
	public:
		unsigned long int unknown_00;
		unsigned long int unknown_04;
		unsigned long int unknown_08;
		unsigned long int unknown_0C;
		// 0x10
		unsigned long int unknown_10; // unknown type/size
		unsigned long int unknown_14;
		unsigned long int unknown_18;
		unsigned long int unknown_1C;
		// 0x20
		unsigned long int unknown_20; // unknown type/size
		STRUCT_PATH_FINDING_INTERMEDIATE_STEP *intermediatePositions; // +24
		long int intermediatePositionsArraySize; // +28
		long int intermediatePositionsUsedElementsCount; // +2C
		// 0x30
		long int intermediatePositionsCurrentIndex; // +30. To confirm. See 4583D0
	};



	// Eye candy (type10) / common class for all units
	// A8 7D 54 00. Size=0x88 (constructor 0x04A64B0)
	class STRUCT_UNIT_BASE {
	public:
		unsigned long int checksum;
		long int unitInstanceId; // Can be negative (temporary units like smoke). Can be something else (or unused?) (seen for flare, type=10)
		STRUCT_UNITDEF_BASE *ptrStructDefUnit;
		STRUCT_PLAYER *ptrStructPlayer;
		// 0x10
		char *pCurrentGraphics; // Pointer to a structure about graphics...
		unsigned long int unknown_014;
		unsigned long int unknown_018; // ptr to struct 00 30 54 00 - size=0x0C Related to graphics
		STRUCT_GAME_MAP_TILE_INFO *myTile; // +1C. To confirm. +5=byte=altitude&terrain bits, see TERRAIN_BYTE class
		// 0x20
		STRUCT_UNIT_BASE *transporterUnit; // Transport boat the unit is in
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unknown_024; // +24.
		short int unknown_028;
		short int unknown_02A; // for graphics ?
		short int unknown_02C; // for graphics ?
		short int unknown_02E; // for graphics ?
		// 0x30
		float remainingHitPoints;
		char currentGraphicsDamageIndex; // +34. According to remaining HP ?
		char orientationIndex; // +35. Also used as building step (<25%, <50%, <75%, >75%). For types 30+, step orientationAngle(+9C), not this. See 0x44BBD0(unit+[0xD4]).
		char mouseSelectionStatus; // +36. 1=selected, 2=right-click-target. Is it a Mask, not an enum !
		char shortcutNumber; // 0-10, 10+=regrouped units ; 0=none, 10=0x0A="0" key.
		float positionY; // (+38) bottom-left to upper-right
		float positionX; // (+3C) top-left to bottom-right
		// 0x40
		float positionZ; // Read only, because it's updated automatically according to map position(X,Y)
		float resourceValue; // See resourceValue. Faith (0-100) for priests, food for hunter...
		char unitStatus; // 0=being_built, 2=ready, at least for buildings... To confirm exact roles. Values=0,1,2,3,5,7,8=max?
		char isNotCreatable; // +49. 1 if the unit is a "not-creatable" unit, like cliffs, etc. Always 1 in parent constructors, set to 0 in "creatable-level" constructor.
		char isDoppleGanger; // +4A. Is temp unit ? Not 100% sure.
		char unknown_04B;
		short int resourceTypeId; // A unit can hold only 1 resource. =0x22=34 for priest.
		char unitType; // +4E. Warning: this value is often WRONG. See unitDef value instead. Only 1-byte here, whereas it is 4-bytes in unitDef struct. 70=living, etc. See GLOBAL_UNIT_TYPES
		char unitCountThatAreTargettingMe; // Number of (other)units that have "me" as target. See 4AADB0.
		// 0x50
		STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *ptrElemInPlayerUnitList; // +50.
		unsigned long int unknown_054; // +54 a ptr. An array. Elemsize=4? about movement ?
		long int unknown_054_usedElemCount; // +58. (to confirm)
		long int unknown_054_maxElemCount; // +5C. (to confirm)
		// 0x60
		long int unknown_054_arraySize; // +60. Number of allocated elements in +54 (to confirm)
		char unknown_064[0x70 - 0x64];
		// +50 a ptr. ? +4,+8=dword. Related to playerUnitListStruct?
		// +54 a ptr. An array. Elemsize=4? about movement ?
		// +58: number of used elements in +54 ??
		// +60 size of array +54 ?
		// 0x70
		unsigned long int unknown_070; //same type as 74 ?
		STRUCT_UNIT_ACTIVITY *currentActivity; // +74
		short int unknown_078_unitInstanceId; // ?
		short int unknown_07A;
		unsigned long int unknown_07C; // Related to terrain restriction ????
		// 0x80
		unsigned long int unknown_080;
		char unknown_084; // related to movement ?
		char unknown_085; // related to movement ? Previous value of +84 ?
		char unknown_086_hasTarget; // ?
		char unknown_087;

		bool IsCheckSumValid() const { return (this->checksum == 0x00547DA8); }
		bool IsTypeValid() const { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_EYE_CANDY); }
		// Returns true if checksum is valid for this class OR a child class
		bool IsCheckSumValidForAUnitClass() const {
			return (this->checksum == 0x00547DA8) || // eye candy (including cliffs...)
				(this->checksum == 0x00542E14) || // Flag
				(this->checksum == 0x005441BC) || // Doppleganger
				(this->checksum == 0x00544838) || // Dead/fish (destroyable?)
				(this->checksum == 0x00542748) || // Birds
				(this->checksum == 0x0054324C) || // Type50
				(this->checksum == 0x005445E8) || // Projectiles
				(this->checksum == 0x0054820C) || // living
				(this->checksum == 0x00547FA0) || // Building
				(this->checksum == 0x00548474) // Tree
				;
		}
		// Obsolete getter
		STRUCT_UNITDEF_BASE *GetUnitDefinition() const { return this->ptrStructDefUnit; }
		// Returns true if the unit definition is a flag (20) or a child class (all but eye candy and trees)
		bool DerivesFromFlag() const {
			return (this->IsCheckSumValidForAUnitClass() && (this->checksum != 0x00547DA8) && (this->checksum != 0x00548474)); // all but 10 and 90
		}
		// Returns true if the unit definition is dead/fish (30) or a child class
		bool DerivesFromDead_fish() const {
			return this->DerivesFromBird() || (this->checksum == 0x00544838);
		}
		// Returns true if the unit definition is a bird (40) or a child class
		bool DerivesFromBird() const {
			return this->DerivesFromType50() || (this->checksum == 0x00542748);
		}
		// Returns true if the unit definition is a living unit or a child class (building)
		bool DerivesFromLiving() const {
			return (this->checksum == 0x0054820C) || // living
				(this->checksum == 0x00547FA0); // Building
		}
		// Returns true if the unit definition is type50 or one of its child classes (projectile, living/building).
		bool DerivesFromType50() const {
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
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE) == 0x88, "STRUCT_UNIT_BASE size");



	// 14 2E 54 00 = flag (type20). Size=0x8C. constructor=0x0415020 - derives from type10
	class STRUCT_UNIT_FLAG : public STRUCT_UNIT_BASE {
	public:
		float unknown_088;

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


	// 38 48 54 00 = dead/fish (type30). Size=0x180 (constructor=0x044AD30) - Derives from type20
	class STRUCT_UNIT_DEAD_FISH : public STRUCT_UNIT_FLAG {
	public:
		unsigned long int unknown_08C;
		// 0x90
		float unknown_090; // Used for movement computation ?
		float unknown_094; // Used for movement computation ?
		unsigned long int unknown_098;
		float orientationAngle; // +9C. unit orientation angle. Updating this impacts unit.orientationIndex (+0x35). Angle [0, 2*PI[. 0=heading to northEast (increasing Y)
		unsigned long int unknown_0A0;
		STRUCT_UNIT_MOVEMENT_INFO movementInfo; // +A4. Movement info / path finding.
		STRUCT_UNIT_MOVEMENT_INFO temp_AI_movementInfo; // +D8. Only used in AI treatments(?) to make checks before assigning tasks ? Used when unknown_154_tempFlag_calculatingPath=1
		float unknown_10C; // +10C.
		// 0x110
		float move_initialPosX; // wrong ?
		float move_initialPosZ; // wrong ?
		unsigned long int unknown_118;
		float move_targetPosY; // updated from action struct on action updates.
		// 0x120
		float move_targetPosX;
		float move_targetPosZ;
		unsigned long int unknown_128;
		unsigned long int unknown_12C;
		// 0x130
		unsigned long int unknown_130;
		float move_distanceFromTargetToReach; // The maximum distance to target to reach before movement is considered done.
		long int move_targetUnitId;
		float targetSizeRadiusY;
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
		char isInMovement; // Not sure
		char unknown_175;
		char unknown_176; // ?
		char unknown_177; // ?
		unsigned long int unknown_178;
		unsigned long int unknown_17C;

		bool IsCheckSumValid() { return (this->checksum == 0x00544838); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_DEAD_UNITS); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_DEAD_FISH) == 0x180, "STRUCT_UNIT_DEAD_FISH size");


	// 48 27 54 00 = bird (type40). Size=0x18C (constructor=0x04058A0) - Derives from type30
	class STRUCT_UNIT_BIRD : public STRUCT_UNIT_DEAD_FISH {
	public:
		// 0x180
		unsigned long int unknown_180;
		STRUCT_UNIT_ACTION_INFO *ptrActionInformation; // +184. Useful to retrieve unit's action.
		unsigned long int unknown_188;

		bool IsCheckSumValid() { return (this->checksum == 0x00542748); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BIRD); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD) == 0x18C, "STRUCT_UNIT_BIRD size");


	// 4C 32 54 00 = type50 (type50). Size=0x1BC? (constructor=00425F30) - Derives from type40
	class STRUCT_UNIT_TYPE50 : public STRUCT_UNIT_BIRD {
	public:
		unsigned long int unknown_18C_table; // table of 8 (ptr) elements "Y". Y+0=unitID,Y+5=numPlayerOwner,Y+6=byte(pos?),Y+7=byte(pos?)???
		// 0x190
		unsigned long int unknown_190;
		unsigned long int *unknown_194; // pointer to unit ID (myself ?)
		unsigned long int unknown_198;
		unsigned long int unknown_19C;
		unsigned long int unknown_1A0;
		unsigned long int unknown_1A4;
		unsigned long int unknown_1A8;
		unsigned long int unknown_1AC;
		// 0x1B0
		STRUCT_MAP_VISIBILITY_INFO unknownVisibility_1B0; // Same "nature" object as +0x1E4. A mask for map visibility (visible for ...)?
		float unknown_1B4; // Time to next xxx?
		char stillToBeDiscoveredByHuman; // 1 for gaia units that have not been discovered yet (for gaia units capture system)
		char unknown_1B9[3]; // ?

		bool IsCheckSumValid() { return (this->checksum == 0x0054324C); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TYPE50); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_TYPE50) == 0x1BC, "STRUCT_UNIT_TYPE50 size");


	// E8 45 54 00 = projectile (type60). Size=1C4 (constructor=0x0443D30) - Derives from type50
	class STRUCT_UNIT_PROJECTILE : public STRUCT_UNIT_TYPE50 {
	public:
		char unknown_1BC; // byte
		char unknown_1BD;
		short int unknown_1BE;
		// 0x1C0
		unsigned long int unknown_1C0;

		bool IsCheckSumValid() { return (this->checksum == 0x005445E8); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_PROJECTILE); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE) == 0x1C4, "STRUCT_UNIT_PROJECTILE size");


	// 0C 82 54 00 = living (type70). Size=0x1C0 (constructor=0x04AE2D0) - Derives from type50
	class STRUCT_UNIT_LIVING : public STRUCT_UNIT_TYPE50 {
	public:
		char hasDedicatedUnitDef; // +1BC. If true, this->unitDef is a "temporary" copy created when unit was converted, which means it must be freed when unit dies (or changes, for villager mode). See 4AE435 for delete.
		char unknown_1BD;
		short int unknown_1BE;

		bool IsCheckSumValid() { return (this->checksum == 0x0054820C); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_LIVING_UNIT); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_LIVING) == 0x1C0, "STRUCT_UNIT_LIVING size");


	// A0 7F 54 00 = building (type80). Size=0x1FC (constructor=0x04AC550) - Derives from type70
	class STRUCT_UNIT_BUILDING : public STRUCT_UNIT_LIVING {
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
		char unknown_1DC; // Consistent with player+0x54. An index for ?
		char unknown_1DD; // value = 0 or 100 ? Switched when construction ends?
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
		bool IsCheckSumValid() { return (this->checksum == 0x00547FA0); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING) == 0x1FC, "STRUCT_UNIT_BUILDING size");


	// 74 84 54 00 = tree (type90). Size=0x88 (constructor=0x04B0D20) - Derives from type10
	class STRUCT_UNIT_TREE : public STRUCT_UNIT_BASE {
	public:
		bool IsCheckSumValid() { return (this->checksum == 0x00548474); }
		bool IsTypeValid() { return this->IsCheckSumValid() && (this->unitType == (char)AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TREE); }
	};
	static_assert(sizeof(AOE_STRUCTURES::STRUCT_UNIT_TREE) == 0x88, "STRUCT_UNIT_TREE size");


	static long int GetUnitStructRealSize(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
		if (unit->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE);
		if (((STRUCT_UNIT_BIRD*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_FLAG*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_FLAG);
		if (((STRUCT_UNIT_DEAD_FISH*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DEAD_FISH);
		if (((STRUCT_UNIT_DOPPLEGANGER*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DOPPLEGANGER);
		if (((STRUCT_UNIT_LIVING*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_LIVING);
		if (((STRUCT_UNIT_PROJECTILE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE);
		if (((STRUCT_UNIT_TREE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_TYPE50*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_TYPE50);
		if (((STRUCT_UNIT_BUILDING*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING);
		// unit->unitType is often erroneous ! If not found, try again using only checksum.
		if (unit->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE);
		if (((STRUCT_UNIT_BIRD*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_FLAG*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_FLAG);
		if (((STRUCT_UNIT_DEAD_FISH*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DEAD_FISH);
		if (((STRUCT_UNIT_DOPPLEGANGER*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DOPPLEGANGER);
		if (((STRUCT_UNIT_LIVING*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_LIVING);
		if (((STRUCT_UNIT_PROJECTILE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE);
		if (((STRUCT_UNIT_TREE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_TYPE50*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_TYPE50);
		if (((STRUCT_UNIT_BUILDING*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING);
		return 0;
	}

}
