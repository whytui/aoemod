
#pragma once

#include <AOE_empires_dat.h>
#include <AOE_const_internal.h>
#include <AOE_struct_managed_array.h>
#include <AOE_struct_map_base_common.h>

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


	// Size = 0x24. "OrderEvent". See 0x414B00
	class STRUCT_UNIT_ACTIVITY_ORDER_EVENT {
	public:
		long int actorUnitId;
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS activityId;
		long int unknown_08; // related to activity+2C. A value 0-100 (or -1) ? See 0x41375C
		long int targetUnitId;
		// 0x10
		long int targetPlayerId; // Sometimes a BYTE (seen 0x000000FF) ?
		float posY; // target position
		float posX; // target position
		float posZ; // target position
		// 0x20
		float maxRange; // Actor's maximum range.
	};
	static_assert(sizeof(STRUCT_UNIT_ACTIVITY_ORDER_EVENT) == 0x24, "STRUCT_UNIT_ACTIVITY_ORDER_EVENT size");


	// Size = 0x18. No dedicated constructor? See 0x414D90 for init/adding to array. "NotifyEvent"
	// Values remain in fields even when unit is idle: check "nextActivityQueueUsedElems".
	// For activityId=0x1F4: genericParam4=attackerUnitId, genericParam5=? (not a unitid) genericParam6=? ; targetUnitId=attacker, actorUnitId=me
	// For activityId=0x200: genericParam4=activityId to restore afterwards (attack...), genericParam5=targetUnitId ?
	// For activityId=0x202: genericParam4=activityId(explore...)
	class STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT {
	public:
		long int targetUnitId; // +0. Sometimes = actor? "Other" unit id ?
		long int actorUnitId; // +4. My unit id ? (always "me" ?)
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS activityId; // +8. Notified activity ID. GAME_EVENT_TYPES?
		long int genericParam4; // +C. Can be different things ? Seen targetUnitId, activityId(next?)...
		long int genericParam5; // +10. Seen currentHP value.
		long int genericParam6; // +14. Seen unitDefMaxHP value (int).
	};
	static_assert(sizeof(STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT) == 0x18, "STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT size");


	// Size=0x8. Is it STRUCT_NEARBY_UNIT_INFO ??? Maybe not as +6/+7 fields seem to be unused here ?
	class STRUCT_UNIT_ACTIVITY_UNKNOWN_12C_ELEM {
	public:
		long int unitId;
		char unknown_04; // +04. -1=invalid or N/A
		char unknown_05; // +05. -1=invalid or N/A
		char unknown_06; // Unused ?
		char unknown_07; // Unused ?
	};
	static_assert(sizeof(STRUCT_UNIT_ACTIVITY_UNKNOWN_12C_ELEM) == 0x8, "STRUCT_UNIT_ACTIVITY_UNKNOWN_12C_ELEM size");
	// Size=0x1C8. Constructor=0x40D2F0 (see other methods just after)
	class STRUCT_UNIT_ACTIVITY_UNKNOWN_12C {
	public:
		STRUCT_UNIT_ACTIVITY_UNKNOWN_12C_ELEM unknown_00[50]; // +00.
		unsigned long int unknown_190;
		unsigned long int unknown_194;
		unsigned long int unknown_198; // A struct size=0x0C, ccor(init?)=0x46A7E0
		unsigned long int unknown_19C;
		unsigned long int unknown_1A0; // end of struct...
		unsigned long int unknown_1A4[5]; // +1A4. Cf 0x40D390. (index can be 0-4)
		char unknown_1B8; // +1B8. Default=0xFD=253
		char unused_1B9[3];
		unsigned long int unknown_1BC;
		unsigned long int unknown_1C0;
		unsigned long int unknown_1C4;
	};
	static_assert(sizeof(STRUCT_UNIT_ACTIVITY_UNKNOWN_12C) == 0x1C8, "STRUCT_UNIT_ACTIVITY_UNKNOWN_12C size");


#define CHECKSUM_UNIT_ACTIVITY_BASE 0x00542D10 // Base class (not used directly?). Constructor=0x40EDF0.
#define CHECKSUM_UNIT_ACTIVITY_PREY_ANIMAL 0x00548D7C // "prey animal" TribeHuntedAnimalUnitAI (class 9). Constructor 0x4E3B50
#define CHECKSUM_UNIT_ACTIVITY_LION 0x0054901C // Constructor=0x4E4170. Specificity is to wait for killed target to decay before attacking another one + attacks randomly ?
#define CHECKSUM_UNIT_ACTIVITY_GAIA_ELEPHANT 0x00548F3C // TribeElephantUnitAI. Hardcoded to class=0A, unitDefId=0x30. Constructor 0x4E4040.
#define CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL 0x00548E5C // Constructor=0x4E3E50. All non-lion, non-elephant predator animals.
#define CHECKSUM_UNIT_ACTIVITY_NON_DISCOVERY_ARTEFACT 0x00549644 // Constructor 0x4E6900. All artefacts that are NOT discovery (excludes unitDefId 10 and 99, which no longer exists in empires.dat)
#define CHECKSUM_UNIT_ACTIVITY_CIVILIAN 0x005490FC // For class=4. constructor=0x4E44D0
#define CHECKSUM_UNIT_ACTIVITY_PRIEST 0x005491DC // Constructor 0x4E4E00. Class 18 (0x12)
#define CHECKSUM_UNIT_ACTIVITY_MILITARY 0x00549564 // Constructor 0x4E60F0. Archers, melee, war ships, (non-priest)heroes/cheat units. Classes 0,6,12,13,17,19,22,23,24,25,26,28 and >33 if classes are added.
#define CHECKSUM_UNIT_ACTIVITY_TOWER 0x00549724 // Constructor 0x4E6D60. For hardcoded unitDefIDs. EXCLUDES Lazor tower (=military)
#define CHECKSUM_UNIT_ACTIVITY_BUILDING 0x00549804 // For class 3 (excluding towers). constructor=0x4E6F40
#define CHECKSUM_UNIT_ACTIVITY_TRADE_SHIP 0x005492C4 // Constructor=0x4E56F0. Class=2. Not really compatible with "generic" trade units (if using other units than trade boat/dock)
#define CHECKSUM_UNIT_ACTIVITY_FISHING_SHIP 0x00549484 // Constructor=0x4E5DC0. Class=0x15
#define CHECKSUM_UNIT_ACTIVITY_TRANSPORT_SHIP 0x005493A4 // Constructor=0x4E5B40. Class=0x14
	// Unit artificial intelligence (not reserved to AI players !). "UnitAIModule"
	// In standard game, only living units have this object, but not all (lion_trained doesn't have one)
	// However, unitActivity is destroyed in unit base class, so it is possible to add unit activity to ANY unit.
	// Size=0x134 for ALL child classes. Constructor=0x40EDF0 (base). 0x4AFBE0=unit.createUnitActivity()
	// +0x00 = activity.destructor(do_free)
	// +0x04 = activity.save(internalFileId)
	// +0x08 = activity.load(internalFileId)
	// +0x0C = activity.GetOwnerPlayer()
	// +0x10 = activity.order(?, taskId, targetUnitId, targetPlayerId, posY, posX, posZ, maxRange, arg9, arg10, arg11)
	// +0x14 = activity.notify(actorUnitId, targetUnitId, internalId, targetUnitClass, currentHP, maxHP) = add to notify queue.
	// +0x18 = activity.notifyCommander(arg1, arg2, arg3, generic_4, generic_5, generic_6?). Eg. 0x410A20
	// +0x1C = activity.notifyCommander(NotifyEvent*). Eg. 0x4109F0 => calls [EDX+0x18]
	// +0x20 = activity.prepareTmpMapInfo?(arg1) : collects info about nearby units (cf ADDR_ELEMCOUNT_TEMP_NEARBY_UNITS_PER_DIPLVALUE)
	// +0x2C = activity.isRetryableOrder(internalId) : true for non-interruptible activities (repair, heal, convert, attack, defend/capture+0x264)
	// +0x30 = activity.autoChooseTargetAtReach?(checkWallsIfCurrentTargetIsWall?, checkCalcPath?, arg3) BestUnitToAttack??
	// ?int UnitAIModule::mostDangerousEnemy(float *)
	// ?int UnitAIModule::weakestEnemy(float *)
	// ?int UnitAIModule::closestAttacker(float *)
	// ?int UnitAIModule::closestObject(int,int,int,float *)
	// ?int UnitAIModule::closestUndiscoveredTile(int *,int *,int)
	// +0x40 = activity.findGatherTarget?(targetAIType, arg2, arg3, arg4, arg5)
	// +0x44 = activity.xxx(resourceGatherType, arg2)
	// +0x48 = ?int UnitAIModule::closestUndiscoveredTile(int *,int *,int)
	// +0x4C = activity.logDebug???
	// +0x50 = activity.canAttackUnit(ptrUnit). Or can convert, for priest activity.
	// +0x54 = 
	// +0x58 = activity.stop(bool "use+28_order?))
	// +0x5C = activity.setAttackOrGatherAttackTarget(targetUnitId, force)
	// +0x60 = activity.setAttackTarget(targetUnitId)
	// +0x64 = activity.setGatherWithAttackTarget(targetId, force) for hunters, lumberjacks... ("huntObject")
	// +0x68 = activity.setGatherNoAttackTarget(targetId, force) for miners, forager, farmers...
	// +0x6C = activity.convertTarget(unitId, force)
	// +0x70 = activity.healTarget(targetUnitId, force)
	// +0x74 = activity.setRepairAction(targetUnitId, force)
	// +0x78 = activity.setBuildAction(targetUnitId, force)
	// +0x7C = activity.tradeWithObject(targetUnitId, force)
	// +0x80 = activity.explore(int posY, int posX, posZ)
	// +0x84 = activity.enterObject(targetUnitId, force)
	// +0x88 = activity.transport(arg1, arg2, arg3)
	// +0x90 = activity.moveTo(fposY, fposX, arg3, arg4, force)
	// +0x94 = activity.moveTo(targetUnitId, f_maxRange, force?)
	// +0x98 = activity.moveTo(int,int)
	// +0x9C = activity.evasiveMoveTo(arg1, arg2, arg3, force). Eg 0x412650
	// +0xA0 = activity.?(force)
	// +0xA4 = activity.runAwayFromAttackers(force)
	// +0xA8 = activity.followObject(targetUnitId, f_dist, force)
	// +0xAC = activity.defendObject(targetUnitId, f_dist, force)
	// +0xB0 = activity.defendPosition(float, float, float, force)
	// +0xB4 = activity.  transport load ?
	// +0xB8 = activity. for 2D3
	// +0xBC = activity.isImportantObject(AIType)
	// +0xC0 = activity.GetResourceGatherType(AIType)
	// +0xC4 = activity.canConvert(targetUnitId) ?
	// +0xC8 = activity.processOrder(orderEvent, arg2)
	// +0xCC = activity.ProcessNotify(notifyEvent, arg2). Returns some enum (3,4,5=do nothing special?,6=triggered an activity?..) "ProcessNotify" ? Ex:priest=0x4E5370, military=0x4E62D0, base=0x413890. Each class has a specific overload (except tower).
	// +0xD4 = activity.ProcessIdle(arg1)? 0x4145A0 (base proc, used by many children), priest=0x4E54E0. For example, auto-targeting of idle units.
	// +0xD8 = activity.ProcessMisc(). ex. 0x414600. Find target? Returns some enum
	// +0xDC = activity.processRetryableOrder() [Last for all except priest activity]. Returns some enum
	// PriestActivity+0xE0
	// PriestActivity+0xE4 [Last]
	class STRUCT_UNIT_ACTIVITY {
	public:
		unsigned long int checksum;
		STRUCT_UNIT_BASE *ptrUnit; // +4. actor unit.
		long int unknown_008; // default -1 ?
		GLOBAL_UNIT_AI_TYPES unitAIType; // +0C. unit AI type (on 4 bytes, actually).
	private:
		short int unused_unitAIType; // +0E. Because unitAIType is 4-bytes here. Do not write on these bytes !
	public:
		long int orderQueueUsedElemCount; // +10. Number of "used" elements in array.
		long int orderQueueSize; // +14. Allocated elements
		STRUCT_UNIT_ACTIVITY_ORDER_EVENT *orderQueue; // +18. Stacked activities ? THIS is used to get back to work after being attacked !
		long int notifyQueueUsedElemCount; // +1C. Actually used elems in Notify queue
		long int notifyQueueArraySize; // +20. Allocated array size (number of elements) for Notify queue.
		STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyQueue; // +24. See 0x414D90(add)
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS orderTaskId; // +28. The current *explicit* order. If -1, then unit reacts to attack, see 414600.
		long int unknown_2C; // +2C. 0x64 in 4DA4C9,4D841F. cf targetsInfoArray+08. CMP to enemySightedRespDist in 0x4D85B7. Updated in 0x40F8B1. Special values -1,100. Normal values 0-99. Distance to current target ? Reset to -1 when activity stops. Priority ?
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS currentActionId; // +30. Current activity type.
		long int targetUnitId; // +34. Current target unit instance ID.
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES targetUnitType; // +38. Target AI type (3=building...).
		float targetPosY; // +3C
		float targetPosX; // +40
		float targetPosZ; // +44
		float maxDistance; // +48. "Desired target distance". Default 2 ?
		long int unitIdToDefend; // +4C. Unit ID to capture or defend ?
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS previous_whenAttackedInternalId; // +50. Backup for +28. "lastOrder?"
		AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS previousActionId; // +54. Backup for currentActionId. set in 40F6D0 method, 411D00. "lastAction"
		long int previousTargetUnitId; // +58. Previous target class
		 AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES previousTargetUnitType; // +5C. Type=GLOBAL_UNIT_AI_TYPES but as a dword.
		// 0x60
		// Recomputed in each activity update (very often!)?
		// Unis IDs are added on "activity.processNotify" for being attacked.
		// Unit IDs are removed on "updateActivity" (?), cf loop in 0x40F8E2: for each unit: if unit.activity.targetId != myUnitId then remove from list.
		// Warning: Units that do NOT have an activity (tame lion...) are not well listed here (it's quite a bug in the game, units are supposed to all have an activity... I think)
		STRUCT_MANAGED_ARRAY unitIDsThatAttackMe;
		STRUCT_WAYPOINT waypointQueue[8]; // +70.
		// 0xF0
		unsigned long int unknown_0F0; // wayPointQueueSize ???
		unsigned long int lastUpdateTime; // +F4. To confirm
		unsigned long int unknown_0F8; // +F8.
		long int unknown_0FC; // int, consistent with +0F8 and global.playerVar ? Init=random+(unknown_100*3/4)
		// 0x100
		long int unknown_100_baseForRandomSeed; // init 0xBB8=3000. Base for random seed calculation. The highest it is, the lowest are chances of "reacting" ? Lion=6000(low reaction %), gazelle=4000(high)
		unsigned long int unknown_104_gameTime; // +104. Some game time (ms) in the *future* ? (next xxx) ? Or defenseBuffer ?? Or idleTimeout?
		unsigned long int unknown_108; // int, consistent with +10C. Default 0 (NULL?) Time since last autoFindTarget?
		unsigned long int unknown_10C; // int, consistent with +108. Default 0x3E8=1000? Delay between 2 autoFindTarget ? 0x40FBA3
		// 0x110
		float currentPosY;
		float currentPosX;
		float currentPosZ; // To confirm
		unsigned long int unknown_11C;
		// 0x120
		float unitLineOfSight; // init 1. To confirm (read from unitDef.lineOfSight?)
		long int *listOfImportantUnitAITypes; // +124. List size/content is hardcoded, depends on activity type.
		long int listOfImportantUnitAITypesArraySize; // +128. number of elements in +124.
		STRUCT_UNIT_ACTIVITY_UNKNOWN_12C *unknown_12C; // +12C. Can be NULL.
		// 0x130
		char unknown_130_switchBackFromHunter_unsure; // A flag ? 410C00. 4E49C3: set when activity changed to hunt to defend against an animal?
		char unknown_131; // unused ?
		char unknown_132; // unused ?
		char unknown_133; // unused ?
		// Returns true if checksum is one of the valid possible Activity structure checksums
		bool IsCheckSumValid() const { 
			return (this->checksum == CHECKSUM_UNIT_ACTIVITY_BASE) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_BUILDING) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_CIVILIAN) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_FISHING_SHIP) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_GAIA_ELEPHANT) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_LION) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_MILITARY) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_NON_DISCOVERY_ARTEFACT) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_PREY_ANIMAL) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_PRIEST) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_TOWER) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_TRADE_SHIP) ||
				(this->checksum == CHECKSUM_UNIT_ACTIVITY_TRANSPORT_SHIP);
		}
		// Returns address of constructor(arg1=pUnit, arg2=?=generally 0x0A)
		static unsigned long int ConstructorAddress(unsigned long int classChecksum) {
			switch (classChecksum) {
			case CHECKSUM_UNIT_ACTIVITY_BUILDING:
				return 0x4E6F40;
			case CHECKSUM_UNIT_ACTIVITY_CIVILIAN:
				return 0x4E44D0;
			case CHECKSUM_UNIT_ACTIVITY_FISHING_SHIP:
				return 0x4E5DC0;
			case CHECKSUM_UNIT_ACTIVITY_GAIA_ELEPHANT:
				return 0x4E4040;
			case CHECKSUM_UNIT_ACTIVITY_LION:
				return 0x4E4170;
			case CHECKSUM_UNIT_ACTIVITY_MILITARY:
				return 0x4E60F0;
			case CHECKSUM_UNIT_ACTIVITY_NON_DISCOVERY_ARTEFACT:
				return 0x4E6900;
			case CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL:
				return 0x4E3E50;
			case CHECKSUM_UNIT_ACTIVITY_PREY_ANIMAL:
				return 0x4E3B50;
			case CHECKSUM_UNIT_ACTIVITY_PRIEST:
				return 0x4E4E00;
			case CHECKSUM_UNIT_ACTIVITY_TOWER:
				return 0x4E6D60;
			case CHECKSUM_UNIT_ACTIVITY_TRADE_SHIP:
				return 0x4E56F0;
			case CHECKSUM_UNIT_ACTIVITY_TRANSPORT_SHIP:
				return 0x4E5B40;
			case CHECKSUM_UNIT_ACTIVITY_BASE:
				return 0x40EDF0;
			default:
				return NULL;
			}
		}
	};

}

