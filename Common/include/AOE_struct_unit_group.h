
#pragma once

#include <assert.h>
#include <AOE_empires_dat.h>
#include <AOE_const_internal.h>


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	const long int STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT = 40; // 0x28
	const long int STRUCT_UNIT_GROUP_TARGET_SLOTS_COUNT = 20;

	// Size = 0x330. Constructor()=0x4CC630
	// tacAI.createGroup(useSequence)=0x4E0400 ; tacAI.removeUnitGroup(unitGroupId)=0x4E04B0
	// tacAI.RemoveAllGroups(unitGroupType)=0x4E0520, -1=joker
	// Organized as a loop chained list. See TacAI (first elem is included in TacAI).
	// 0x4DC670=tacAI.SetupUnitGroups()
	// 0x4CE950=unitGroupElem.areUnitsWithinGrpRange(tacAI, mainAI). Returns true if all units are <groupSpacing or artefactreturndistance
	// 0x4E07F0 = tacAI.searchNearestGroupFromPosition(groupType, distance, pPosYX)
	class STRUCT_UNIT_GROUP {
	public:
		unsigned long int checksum; // E0 8C 54 00
		STRUCT_UNIT_GROUP *next;
		STRUCT_UNIT_GROUP *previous;
		long int unitGroupId; // A unique ID. Set in 0x4CCC50.
		// 0x10
		long int isTasked; // +10. Set to 0 when task changes or when idle, 1 when "init" for task has been done ??
		AOE_CONST_INTERNAL::UNIT_GROUP_TYPES unitGroupType; // +14. Set in 0x4CCC70.
		long int taskSubTypeId; // +18. -1=default. 0,4 = capture?
		long int myUnitsIdArray[STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT]; // +1C. 40 elements, can be non-consecutive (ignore -1 values).
		// 0xBC
		long int myUnitsHPArray[STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT]; // +BC. 40 elements. Also used as myUnitsIdArray+0xA0.
		long int unitCount; // +15C. Must correspond to valid unitIds count in unitIdArray. Warning: 4CCC90 method requires LucTieuPhung's fix to avoid breaking this assertion.
		// 0x160
		long int unitMaximumCount; // Unsure
		long int commanderUnitId; // +164. group leader
		long int initialTotalGroupHP; // +168. Total group HP when task was started (used to calculate percent health retreat, etc ?)
		long int initialUnitCount; // +16C.  Total group unit count when task was started (set when resetOrg=1)
		// 0x170
		float unknown_170_posY;
		float unknown_174_posX;
		float unknown_178_posZ;
		unsigned long int unknown_17C; // unknown type
		// 0x180
		AOE_CONST_INTERNAL::UNIT_GROUP_TASK_IDS currentTask; // TaskId, see 4CD339. Values in 0-0x15
		long int targetUnitId; // +184. including defended unit case ?
		long int targetDAT_ID; // +188. unitDefId of target unit.
		float targetPosY;
		// 0x190
		float targetPosX;
		float targetPosZ;
		unsigned long int unknown_198; // seems int. DWORD?
		float posY; // not sure what this is exactly, NOT group "supposed" position ? Leader pos ?
		// 0x1A0
		float posX;
		float posZ;
		long int unknown_1A8;
		float retreatPosY; // +1AC. 0x4CD65E. Only for retreat ?
		// 0x1B0
		float retreatPosX;
		float retreatPosZ;
		float unknown_1B8; // type: unsure
		float unknown_1BC; // type: unsure
		// 0x1C0
		long int unknown_1C0;
		long int transportUnitGroupId; // +1C4. Unit group ID of the transport that does transport "me".
		unsigned long int unknown_1C8;
		long int lastTaskingTime_ms; // +1CC. Last game time of "task active soldier" execution for the group ? Compared to SNTacticalUpdateFrequency (default 3 seconds)
		// 0x1D0
		long int attackPlayId; // attackId that is being played. -1=Non-Play-based attack
		char unknown_1D4; // +1D4. Number of elements in +1D8 ?
		char unknown_1D5;
		short int unknown_1D6; // check type (2 bytes ?)
		float *unknown_1D8; // +1D8. Array of struct size=0x10, +0/+4=float pos? INCLUDED array ? Related to attacking ? Intermediate movement steps to go to target ?
		char unknown_1DC[0x2C8 - 0x1DC];
		char regroupAttempts; // +2C8. Unsure. When reaches 10, the group is considered regrouped ? 0x4CD42F, 0x4D37B4
		char unknown_2C9;
		char unknown_2CA; // ?
		char unknown_2CB; // ?
		long int targetUnitIdArrayUsedElemCount; // +2CC. Index of first UNused element in targetUnitIdArray (=> values 0-19). See 0x4CEBB6
		long int targetUnitIdArray[STRUCT_UNIT_GROUP_TARGET_SLOTS_COUNT]; // +2D0. Array of unitIDs. Total size 0x50 bytes (0x14*4). Get=0x4CEB90
		long int targetPlayerId; // +320. Related to targetUnitIdArray unit's playerId (last inserted? All the same player?)
		long int terrainZoneId; // +324. TerrainZoneId (to identify the island/lake/sea the group is in)
		long int unknown_328_gameTime; // Last tasking time (milliseconds). 10 seconds min between roundup attacks (0x4CDAFC)
		unsigned long int unknown_gameTime_ms; // +0x32C. To confirm
		// End (0x330)
		bool IsCheckSumValid() const { return this->checksum == 0x00548CE0; }
		// Safely get a unit id from targets array. Limited to 20 slots !
		long int GetTargetUnitIdFromArray(long int index) const {
			assert(index < STRUCT_UNIT_GROUP_TARGET_SLOTS_COUNT); // Overflow !
			assert(this->targetUnitIdArrayUsedElemCount <= STRUCT_UNIT_GROUP_TARGET_SLOTS_COUNT);
			if ((index < 0) || (index >= this->targetUnitIdArrayUsedElemCount) || (index >= STRUCT_UNIT_GROUP_TARGET_SLOTS_COUNT)) {
				return -1;
			}
			return this->targetUnitIdArray[index];
		}
		// Safely adds a unit id to targets array. Limited to 20 slots !
		bool AddTargetUnitIdToArray(long int unitId) {
			assert(this->targetUnitIdArrayUsedElemCount < STRUCT_UNIT_GROUP_TARGET_SLOTS_COUNT);
			if (this->targetUnitIdArrayUsedElemCount >= STRUCT_UNIT_GROUP_TARGET_SLOTS_COUNT) {
				return false; // overflow: can't add
			}
			this->targetUnitIdArray[this->targetUnitIdArrayUsedElemCount] = unitId;
			this->targetUnitIdArrayUsedElemCount++;
			return true;
		}
		// Safely gets a unit ID from myUnitsIdArray. Limited to 40 slots.
		long int GetMyUnitId(long int index) const {
			assert(index < STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT); // Overflow !
			if ((index < 0) || (index >= STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT)) { return -1; }
			return this->myUnitsIdArray[index];
		}
		// Safely gets a unit HP from myUnitsHPArray. Limited to 40 slots.
		long int GetMyUnitHP(long int index) const {
			assert(index < STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT); // Overflow !
			if ((index < 0) || (index >= STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT)) { return 0; }
			return this->myUnitsHPArray[index];
		}
	};


}
