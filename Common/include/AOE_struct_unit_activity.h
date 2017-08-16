
#pragma once

#include <AOE_empires_dat.h>
#include <AOE_const_internal.h>
#include <AOE_const_game_events.h>
#include <AOE_struct_managed_array.h>
#include <AOE_struct_map_base_common.h>
#include <AOE_struct_AI_play_status.h>

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
		AOE_CONST_INTERNAL::UNIT_AI_ORDER orderId;
		long int priority; // +08. Related to activity+2C. A value 0-100 (or -1) ? See 0x41375C
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
	// See GAME_EVENT_TYPE comments for genericParam roles
	class STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT {
	public:
		long int callerUnitId; // +0. (old name=targetUnitId)
		long int recipientUnitId; // +4. My unit id ? (always "me" ?)
		AOE_CONST_INTERNAL::GAME_EVENT_TYPE eventId; // +8. Notified activity ID. GAME_EVENT_TYPES?
		long int genericParam4; // +C. Can be different things ? Seen targetUnitId, activityId(next?)...
		long int genericParam5; // +10. Seen currentHP value.
		long int genericParam6; // +14. Seen unitDefMaxHP value (int).
	};
	static_assert(sizeof(STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT) == 0x18, "STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT size");



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
	// +0x20 = activity.CollectNearbyImportantUnitsInTempResults() fills ADDR_ELEMCOUNT_TEMP_NEARBY_UNITS_PER_DIPLVALUE, ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE
	// +0x2C = activity.isRetryableOrder(internalId) : true for non-interruptible activities (repair, heal, convert, attack, defend/capture+0x264) WRONG? activity.taskRequiresLiveTarget(internalId)
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
	// +0x80 = activity.explore(int posY, int posX, posZ) -> task 0x25D
	// +0x84 = activity.enterObject(targetUnitId, force)
	// +0x88 = activity.transport(arg1, arg2, arg3)
	// +0x8C = int UnitAIModule::transportObject(float,float,float,int) ?
	// +0x90 = activity.moveTo(fposY, fposX, arg3, arg4, force) moveAwayFrom ?
	// +0x94 = activity.moveTo(targetUnitId, f_maxRange, force?)
	// +0x98 = activity.moveTo(int,int)
	// +0x9C = activity.evasiveMoveTo(arg1, arg2, arg3, force). Eg 0x412650
	// +0xA0 = activity.?(force)
	// +0xA4 = activity.runAwayFromAttackers(force)
	// +0xA8 = activity.followObject(targetUnitId, f_dist, force)
	// +0xAC = activity.defendObject(targetUnitId, f_dist, force)
	// +0xB0 = activity.defendPosition(float, float, float, force)
	// +0xB4 = activity.  transport load ? for 2D2
	// +0xB8 = activity. for 2D3
	// +0xBC = activity.isImportantObject(AIType)
	// +0xC0 = activity.GetResourceGatherType(AIType)
	// +0xC4 = activity.canConvert(targetUnitId) ?
	// +0xC8 = activity.processOrder(orderEvent, arg2)
	// +0xCC = activity.ProcessNotify(notifyEvent, arg2). Returns some enum (3,4,5=do nothing special?,6=triggered an activity?..) "ProcessNotify" ? Ex:priest=0x4E5370, military=0x4E62D0, base=0x413890. Each class has a specific overload (except tower).
	// +0xD0 = activity.ProcessNotifyCommander(notifyEvent?). 0x4144C0.
	// +0xD4 = activity.ProcessIdle(arg1)? 0x4145A0 (base proc, used by many children), priest=0x4E54E0. For example, auto-targeting of idle units. "activity.manageAutoAttackNearbyUnit(arg1)". Contains hardcoded catapult IDs
	// +0xD8 = activity.ProcessMisc(). ex. 0x414600. Find target? Returns some enum
	// +0xDC = activity.processRetryableOrder() [Last for all except priest activity]. Returns ACTIVITY_EVENT_HANDLER_RESULT? eg 0x414A50
	// PriestActivity+0xE0
	// PriestActivity+0xE4 [Last]
	class STRUCT_UNIT_ACTIVITY {
	public:
		unsigned long int checksum;
		STRUCT_UNIT_BASE *ptrUnit; // +4. actor unit.
		long int mood; // +08. Default -1 ?
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType; // +0C. unit AI type (on 4 bytes, actually).
	private:
		short int unused_unitAIType; // +0E. Because unitAIType is 4-bytes here. Do not write on these bytes !
	public:
		long int orderQueueUsedElemCount; // +10. Number of "used" elements in array.
		long int orderQueueSize; // +14. Allocated elements
		STRUCT_UNIT_ACTIVITY_ORDER_EVENT *orderQueue; // +18. Stacked activities ? THIS is used to get back to work after being attacked !
		long int notifyQueueUsedElemCount; // +1C. Actually used elems in Notify queue
		long int notifyQueueArraySize; // +20. Allocated array size (number of elements) for Notify queue.
		STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyQueue; // +24. See 0x414D90(add)
		AOE_CONST_INTERNAL::UNIT_AI_ORDER orderId; // +28. The current *explicit* order. If -1, then unit reacts to attack, see 0x414600.
		long int currentOrderPriority; // +2C. 0x64 in 4DA4C9,4D841F. CMP to enemySightedRespDist in 0x4D85B7. Updated in 0x40F8B1. Special values -1,100. Normal values 0-99. Reset to -1 when activity stops.
		AOE_CONST_INTERNAL::ACTIVITY_TASK_ID currentTaskId; // +30. Current activity type.
		long int targetUnitId; // +34. Current target unit instance ID.
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES targetUnitType; // +38. Target AI type (3=building...).
		float targetPosY; // +3C
		float targetPosX; // +40
		float targetPosZ; // +44
		float desiredTargetDistance; // +48. "Desired target distance". Default 2 ?
		long int unitIdToDefend; // +4C. Unit ID to capture or defend ?
		AOE_CONST_INTERNAL::UNIT_AI_ORDER previousOrderId; // +50. Backup for +28. "lastOrder?"
		AOE_CONST_INTERNAL::ACTIVITY_TASK_ID previousActionId; // +54. Backup for currentActionId. set in 40F6D0 method, 411D00. "lastAction"
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
		unsigned long int wayPointQueueSize; // +F0. Max 8.
		unsigned long int lastUpdateTime; // +F4.
		unsigned long int idleTimer; // +F8.
		long int adjustedIdleTimeout; // +FC. int, consistent with +0F8 and global.playerVar ? Init=random+(unknown_100*3/4) in 0x413010.
		long int idleTimeout; // +100. init 0xBB8=3000. Base for random seed calculation. The highest it is, the lowest are chances of "reacting" ? Lion=6000(low reaction time), gazelle=4000(high)
		unsigned long int secondaryTimer; // +104. Some game time (ms) in the *future* ? (next xxx) ? Or defenseBuffer ?? Or idleTimeout?
		unsigned long int timeSinceLastDetectSeeUnits_ms; // +108. Milliseconds. Time since last autoFindTarget was executed.
		unsigned long int delayBetweenDetectSeeUnits_ms; // +10C. Delay until next "detect nearby units". Default 0x3E8=1000, reset (each time) in 0x413183 using a random part(4000ms+random[0-999]). Used in 0x40FBA3.
		// 0x110
		float currentPosY;
		float currentPosX;
		float currentPosZ; // To confirm
		unsigned long int unknown_11C;
		// 0x120
		float defenseBuffer; // init 1. To confirm (read from unitDef.lineOfSight?). (Old name=unitLineOfSight)
		long int *listOfImportantUnitAITypes; // +124. List size/content is hardcoded, depends on activity type. This filters the unit classes to include when searching for nearby units.
		long int listOfImportantUnitAITypesArraySize; // +128. number of elements in +124.
		STRUCT_AI_PLAY_STATUS *aiPlayStatus; // +12C. Can be NULL.
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

