
#pragma once

#include <AOE_empires_dat.h>
#include <AOE_const_internal.h>
#include <AOE_struct_managed_array.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{
	// External dependencies
	class STRUCT_UNIT_BASE;


	// Not very well known. A "potential" target for a unit ? See 0x414B00
	// Size = 0x24.
	class STRUCT_UNIT_ACTIVITY_TARGET_ELEM {
	public:
		long int actorUnitId;
		long int internalId;
		long int unknown_08;
		long int targetUnitId;
		// 0x10
		long int targetPlayerId;
		float posY; // target position
		float posX; // target position
		float posZ; // target position
		// 0x20
		float maxRange; // Actor's maximum range.
	};

	// Size = 0x18. No dedicated constructor? See 0x414D90 for init/adding to array. "NotifyEvent"
	// Values remain in fields even when unit is idle.
	class STRUCT_UNIT_ACTIVITY_QUEUE {
	public:
		long int targetUnitId; // +0. Sometimes = actor?
		long int actorUnitId; // +4. My unit id ?
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS activityId; // +8. Queued activity ID. Or event id ??? GAME_EVENT_TYPES?
		long int genericParam4; // +C. Can be different things ? Seen targetUnitId, activityId(...
		long int genericParam5; // +10. Seen currentHP value.
		long int genericParam6; // +14. Seen unitDefMaxHP value (int).
	};
	static_assert(sizeof(STRUCT_UNIT_ACTIVITY_QUEUE) == 0x18, "STRUCT_UNIT_ACTIVITY_QUEUE size");


#define CHECKSUM_UNIT_ACTIVITY_BASE 0x00542D10 // Base class (not used directly?)
#define CHECKSUM_UNIT_ACTIVITY_GAZELLE 0x00548D7C // "prey animal" TribeHuntedAnimalUnitAI (class 9)
#define CHECKSUM_UNIT_ACTIVITY_LION 0x0054901C // Constructor=0x4E4170
#define CHECKSUM_UNIT_ACTIVITY_GAIA_ELEPHANT 0x00548F3C // TribeElephantUnitAI. Hardcoded to class=0A, unitDefId=0x30
#define CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL 0x00548E5C // Constructor=0x4E3E50. All non-lion, non-elephant predator animals.
#define CHECKSUM_UNIT_ACTIVITY_NON_DISCOVERY_ARTEFACT 0x00549644 // Constructor 0x4E6900. All artefacts that are NOT discovery (excludes unitDefId 10 and 99, which no longer exists in empires.dat)
#define CHECKSUM_UNIT_ACTIVITY_PRIEST 0x005491DC // Constructor 0x4E4E00. Class 18 (0x12)
#define CHECKSUM_UNIT_ACTIVITY_MILITARY 0x00549564 // Constructor 0x4E60F0. Archers, melee, war ships, (non-priest)heroes/cheat units
#define CHECKSUM_UNIT_ACTIVITY_TOWER 0x00549724 // Constructor 0x4E6D60. For hardcoded unitDefIDs. EXCLUDES Lazor tower (=military)
#define CHECKSUM_UNIT_ACTIVITY_CIVILIAN 0x005490FC // For class=4. constructor=0x4E44D0
#define CHECKSUM_UNIT_ACTIVITY_BUILDING 0x00549804 // For class 3 (excluding towers). constructor=0x4E6F40
#define CHECKSUM_UNIT_ACTIVITY_TRADE_SHIP 0x005492C4 // Constructor=0x4E56F0. Class=2
#define CHECKSUM_UNIT_ACTIVITY_FISHING_SHIP 0x00549484 // Constructor=0x4E5DC0. Class=0x15
#define CHECKSUM_UNIT_ACTIVITY_TRANSPORT_SHIP 0x005493A4 // Constructor=0x4E5B40. Class=0x14

	// Unit artificial intelligence (not reserved to AI players !). "UnitAIModule"
	// Size=0x134 for ALL child classes. Constructor=0x40EDF0. 0x4AFBE0=createUnitActivity(...)
	// 0x413890 = UnitActivity.processNotify(struct NotifyEvent *,unsigned long). NotifyEvent's size=0x18. Common to all child classes ?
	// NotifyEvent+C = eventId (is this same enum as gameSettings/player events?)
	// [EDX+0x18]=activity.notifyEvent?(arg1, arg2, eventId, arg4, arg5, arg6)
	// [EDX+0x1C]=activity.CallNotifyEvent(arg1, arg2, arg3..?) - get arg1=ptr
	// [EDX+0x20]=activity.prepareTmpMapInfo?(arg1) : collects info about nearby units (cf ADDR_ELEMCOUNT_TEMP_NEARBY_UNITS_PER_DIPLVALUE)
	// [EDX+0x2C]=activity.isXxx(internalId)? : true for non-interruptible activities? (repair, heal,convert,attack,defend/capture+0x264)
	// [EDX+0x30]=activity.autoChooseTargetAtReach?(checkWallsIfCurrentTargetIsWall?, checkCalcPath?, arg3)
	// [EDX+0x40]=activity.findGatherTarget?(targetAIType, arg2, arg3, arg4, arg5)
	// [EDX+0x44]=activity.xxx(resourceGatherType, arg2)
	// [EDX+0x50]=activity.isNotArtefact(ptrUnit)
	// [EDX+0x58]=activity.stop(bool "use+28_task?))
	// [EDX+0x5C]=activity.setAttackOrGatherTarget(targetUnitId, force)
	// [EDX+0x60]=activity.setAttackTarget(targetUnitId)
	// [EDX+0x64]=activity.setGatherWithAttackTarget(targetId, force) for hunters, lumberjacks...
	// [EDX+0x68]=activity.setGatherNoAttackTarget(targetId, force) for miners, forager, farmers...
	// [EDX+0x6C]=activity.convertTarget(unitId, force)
	// [EDX+0x70]=activity.setHealAction?(targetUnitId, force) to confirm
	// [EDX+0x74]=activity.setRepairAction(targetUnitId, force) ?
	// [EDX+0x78]=activity.setBuildAction(targetUnitId, force)
	// [EDX+0x88]=
	// [EDX+0x90]=activity.xxx(f_posY, posX, posZ) (used by +2D4?)
	// [EDX+0x9C]=activity.fleeToRandomPlace(arg1, arg2, arg3, force)??? Eg 0x412650
	// [EDX+0xA4]=activity.fleeFromAttacker(force)
	// [EDX+0xA8]=activity.related to 26F
	// [EDX+0xB4]=activity.  transport load ?
	// [EDX+0xB8]=activity. for 2D3
	// [EDX+0xC0]=activity.GetResourceGatherType(AIType)
	// [EDX+0xC4]=activity.canConvert(targetUnitId) ?
	// [EDX+0xC8]=activity.setTargetFromArray?(targetsInfoArray, arg2)
	// [EDX+0xCC]=activity.dequeue(activityQueue) ? Returns some enum (3,4..)
	// [EDX+0xD8]=activity.chooseTargetForCurrentTask()? ? ex. 0x414600.
	// [EDX+0xE8]=when being attacked?
	class STRUCT_UNIT_ACTIVITY {
	public:
		// Known checksums: 
		// 1C 90 54 00 (predator animals?), 7C 8D 54 00 (gazelle), 3C 8F 54 00 (elephant), 04 98 54 00 (buildings?), 
		// FC 90 54 00 (villager?gatherer?), DC 91 54 00 (priest), 64 95 54 00 (military?), 24 97 54 00 (towers?), 44 96 54 00
		// 5C 8E 54 00 (dead alligator?)
		unsigned long int checksum;
		STRUCT_UNIT_BASE *ptrUnit; // +4. actor unit.
		unsigned long int unknown_008; // default -1 ?
		GLOBAL_UNIT_AI_TYPES unitAIType; // +0C. unit AI type (on 4 bytes ?)
		short int unused_unitAIType; // +0E. Because unitAIType is 4-bytes here (apparently)
		// 0x10
		long int targetsInfoArrayUsedElements; // Number of "used" elements in array.
		unsigned long int targetsInfoArrayTotalSize; // Allocated elements
		STRUCT_UNIT_ACTIVITY_TARGET_ELEM *targetsInfoArray; // +18. Stacked activities ? THIS is used to get back to work after being attacked !
		long int nextActivityQueueUsedElems; // +1C. Actually used elems in +24 array
		// 0x20
		long int nextActivityQueueArraySize; // +20. Allocated array size (number of elements) for +24.
		STRUCT_UNIT_ACTIVITY_QUEUE *nextActivitiesQueue_unsure; // TO DO. ElemSize = 0x18. See 414D90
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS internalId_whenAttacked; // taskId. Auto-tasking ID ? Used when idle or attacked? If -1, then unit reacts to attack ? See 414600. Related to +30 value +0x64. "Reaction task to interruptions?" Priority ?
		long int order; // 0x64 in 4DA4C9. cf targetsInfoArray+08. Maybe WRONG ? order could be +28 ?
		// 0x30
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS currentActionId; // +30. Current activity type.
		long int targetUnitId; // +34. Current target unit instance ID.
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES targetUnitType; // +38. Target AI type (3=building...).
		float targetPosY;
		// 0x40
		float targetPosX;
		float targetPosZ;
		float maxDistance; // +48. "Desired target distance". Default 2 ?
		long int unitIdToDefend; // +4C. Unit ID to capture or defend ?
		// 0x50
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS previous_whenAttackedInternalId; // +50. Backup for +28
		long int previousActionId; // Backup for currentActionId. set in 40F6D0 method, 411D00
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES previousTargetUnitId; // +58. Previous target class
		long int previousTargetUnitType; // +5C. Type=GLOBAL_UNIT_AI_TYPES but as a dword.
		// 0x60
		// Recomputed in each activity update (very often!)?
		// Unis IDs are added on "activity.processNotify" for being attacked.
		// Unit IDs are removed on "updateActivity" (?), cf loop in 40F8E2: for each unit: if unit.activity.targetId != myUnitId then remove from list.
		// Warning: Units that do NOT have an activity (tame lion...) are not well listed here (it's quite a bug in the game, units are supposed to all have an activity... I think)
		STRUCT_MANAGED_ARRAY unitIDsThatAttackMe;
		// 0x70
		float unknown_070_posY;
		float unknown_074_posX;
		long int unknown_078; // a unitId. Commander??? TODO: test it
		unsigned long int unknown_07C; // unknown type
		// 0x80
		float unknown_080_posY;
		float unknown_084_posX;
		unsigned long int unknown_088; // unknown type
		unsigned long int unknown_08C; // unknown type
		// 0x90
		float unknown_090_posY;
		float unknown_094_posX;
		unsigned long int unknown_098; // unknown type
		unsigned long int unknown_09C; // unknown type
		// 0xA0
		float unknown_0A0_posY;
		float unknown_0A4_posX;
		unsigned long int unknown_0A8; // unknown type
		unsigned long int unknown_0AC; // unknown type
		// 0xB0
		float unknown_0B0_posY;
		float unknown_0B4_posX;
		unsigned long int unknown_0B8; // unknown type
		unsigned long int unknown_0BC; // unknown type
		// 0xC0
		float unknown_0C0_posY;
		float unknown_0C4_posX;
		unsigned long int unknown_0C8; // unknown type
		unsigned long int unknown_0CC; // unknown type
		// 0xD0
		float unknown_0D0_posY;
		float unknown_0D4_posX;
		unsigned long int unknown_0D8; // unknown type
		unsigned long int unknown_0DC; // unknown type
		// 0xE0
		float unknown_0E0_posY;
		float unknown_0E4_posX;
		unsigned long int unknown_0E8; // unknown type
		unsigned long int unknown_0EC; // unknown type
		// 0xF0
		unsigned long int unknown_0F0;
		unsigned long int unknown_0F4; // int, consistent with +0FC and global.playerVar ?
		unsigned long int unknown_0F8; // int, consistent with +0FC and global.playerVar ?
		long int unknown_0FC; // int, consistent with +0F8 and global.playerVar ? Init=random+(unknown_100*3/4)
		// 0x100
		long int unknown_100; // init 0xBB8=3000. Base for random seed calculation. The highest it is, the lowest are chances of "reacting". Lion=6000(low reaction %), gazelle=4000(high)
		long int unknown_104_gameTime; // +104. Some game time (ms) in the *future* ? (next xxx) ?
		unsigned long int unknown_108; // int, consistent with +10C
		unsigned long int unknown_10C; // int, consistent with +108
		// 0x110
		float currentPosY;
		float currentPosX;
		float currentPosZ; // To confirm
		unsigned long int unknown_11C;
		// 0x120
		float unitLineOfSight; // init 1. To confirm (read from unitDef.lineOfSight?)
		long int *unknownListOfUnitAITypes; // +124. List of unit classes I can interact with ????? List size/content is hardcoded, depends on activity type.
		long int playerNearbyUnitsunknownListOfUnitAITypesArraySize; // +128. number of elements in +124.
		unsigned long int unknown_12C;
		// 0x130
		char unknown_130; // A flag ? 410C00. 4E49C3: set when activity changed to hunt to defend against an animal?
		char unknown_131; // unused ?
		char unknown_132; // unused ?
		char unknown_133; // unused ?
	};

}

