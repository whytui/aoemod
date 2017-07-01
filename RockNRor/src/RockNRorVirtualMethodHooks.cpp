#include "../include/RockNRorVirtualMethodHooks.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
;

namespace VIRTUAL_METHOD_HOOKS {

#ifdef _DEBUG
	// 'Private' objects for internal usage
	static unsigned long int execBeginTime_ms = 0;
	static int checkPerfCallConsistency = 0;
	static int skippedRecursiveCalls = 0;

	void RecordPerfBegin(unsigned long int callAddress) {
		if (checkPerfCallConsistency > 0) {
			skippedRecursiveCalls++; // we measure only 1 hook at once (there is already a pending perf record). Skip this "recursive" call
			return; // Ignore
		}
		execBeginTime_ms = AOE_METHODS::TimeGetTime();
		checkPerfCallConsistency++;
	}
	void RecordPerfEnd(unsigned long int callAddress) {
		if (skippedRecursiveCalls > 0) {
			skippedRecursiveCalls--;
			return;
		}
		long int endTime = AOE_METHODS::TimeGetTime();
		long int timeSpent = endTime - execBeginTime_ms;
		long int n = ROCKNROR::crInfo.executionCounts[callAddress]; // returns 0 when not set yet (which is good)
		ROCKNROR::crInfo.executionCounts[callAddress] = n + 1;
		long int prevValue = ROCKNROR::crInfo.longestTimes_ms[callAddress]; // returns 0 when not set yet (which is good)
		if (timeSpent > prevValue) {
			ROCKNROR::crInfo.longestTimes_ms[callAddress] = timeSpent;
		}
		checkPerfCallConsistency--;
		assert(checkPerfCallConsistency == 0);
	}
#endif

#ifdef _DEBUG
#define RECORD_PERF_BEGIN(addr) RecordPerfBegin(addr); // BEGIN statement to collect execution stats
#define RECORD_PERF_END(addr) RecordPerfEnd(addr); // END statement to collect execution stats
#else
#define RECORD_PERF_BEGIN(addr) ;
#define RECORD_PERF_END(addr) ;
#endif



	// Maps to store ORIGINAL call addresses. Index=class checksum, value=original method call address
	std::map<unsigned long int, unsigned long int> activityProcessNotifyCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> playerProcessNotifyCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress;


	// Return value is an unknown enum. 2=ok, processed. (unitAI: EDX+0xCC call).
	long int __stdcall ActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int arg2) {
		unsigned long int originalCallAddr = activityProcessNotifyCheckSumAndOriginalAddress[activity->checksum];
		assert(activity && activity->IsCheckSumValid() && notifyEvent);
		RECORD_PERF_BEGIN(originalCallAddr);

		if (!activity || !activity->IsCheckSumValid() || !notifyEvent) {
			RECORD_PERF_END(originalCallAddr);
			return AOE_CONST_INTERNAL::ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION; // TODO: return which value ?
		}
		
		bool runStandardMethod = true;

		// Custom treatments
		int result = ROCKNROR::GAME_EVENTS::ActivityProcessNotify(activity, notifyEvent, arg2, runStandardMethod); // updates runStandardMethod

		if (runStandardMethod && (originalCallAddr != 0)) {
			_asm {
				MOV ECX, activity;
				MOV EDX, DS:[ECX];
				PUSH arg2;
				PUSH notifyEvent;
				// DO NOT CALL DS:[EDX+0xCC] because we changed the pointer !
				CALL originalCallAddr;
				MOV result, EAX;
			}
		}
		RECORD_PERF_END(originalCallAddr);
		return result;
	}


	// Returns void. This hook handles player's notifications (EDX+0xE8 call).
	void __stdcall PlayerProcessNotify(STRUCT_PLAYER *player, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT notifyEvent) {
		unsigned long int originalCallAddr = playerProcessNotifyCheckSumAndOriginalAddress[player->checksum];
		RECORD_PERF_BEGIN(originalCallAddr);
		assert(player && player->IsCheckSumValid());
		if (!player || !player->IsCheckSumValid()) {
			RECORD_PERF_END(originalCallAddr);
			return; 
		}
		bool runStandardMethod = true;

		// Custom treatments
		runStandardMethod = !ROCKNROR::GAME_EVENTS::PlayerNotifyEvent(player, notifyEvent);

		if (runStandardMethod) {
			unsigned long int arg1 = notifyEvent.targetUnitId;
			unsigned long int arg2 = notifyEvent.actorUnitId;
			unsigned long int arg3 = notifyEvent.eventId;
			unsigned long int arg4 = notifyEvent.genericParam4;
			unsigned long int arg5 = notifyEvent.genericParam5;
			unsigned long int arg6 = notifyEvent.genericParam6;
			_asm {
				MOV ECX, player;
				MOV EDX, DS:[ECX];
				PUSH arg6;
				PUSH arg5;
				PUSH arg4;
				PUSH arg3;
				PUSH arg2;
				PUSH arg1;
				// DO NOT CALL DS:[EDX+0xE8] because we changed the pointer !
				CALL originalCallAddr;
			}
		}
		RECORD_PERF_END(originalCallAddr);
	}


	// unit.addPositionToTargetPosArray(pDword_posYXZ, arg2) for ALL unit classes (base+children). Method offset=+1BC.
	// Returns 1 on success, 0 on failure
	long int __stdcall UnitAddPositionToTargetPosArray(STRUCT_UNIT_BASE *unit, STRUCT_UNIT_TARGET_POS *targetPos, long int arg2) {
		unsigned long int originalCallAddr = unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[unit->checksum];
		RECORD_PERF_BEGIN(originalCallAddr);
		bool runStandardMethod = true;
		long int result = 0;
		assert(unit && unit->IsCheckSumValidForAUnitClass());
		assert(targetPos != NULL);
		assert(unit->ptrStructPlayer != NULL);

		// Custom treatments
		// TODO check with largest map size if this can happen
		if ((targetPos->posX == 0xFF) || (targetPos->posY == 0xFF)) {
			std::string msg = "Error with targetPos in UnitAddPositionToTargetPosArray. Unit=";
			msg += std::to_string(unit->unitInstanceId);
			msg += ". @ret=";
			msg += GetHexStringAddress(GetLastVirtualMethodCallReturnAddress());
			traceMessageHandler.WriteMessageNoNotification(msg.c_str());
			if (unit->ptrStructPlayer) {
				AOE_STRUCTURES::STRUCT_PLAYER *player = AOE_METHODS::PLAYER::ChangeControlledPlayer(unit->ptrStructPlayer->playerId, false);
				assert(player && player->IsCheckSumValid());
				AOE_STRUCTURES::PLAYER::SelectOneUnit(player, unit, true);
			}
			AOE_METHODS::CallWriteText(msg.c_str());
			AOE_METHODS::SetGamePause(true); // seems better for stability.
			ROCKNROR::SYSTEM::StopExecution(_T("An error occurred. *FIRST* close this message, then attach a debugger or press ESC to continue."), true, true);
		}

		if (runStandardMethod) {
			unsigned long int arg1 = (unsigned int)targetPos;
			_asm {
				MOV ECX, unit;
				PUSH arg2;
				PUSH targetPos;
				CALL originalCallAddr;
				MOV result, EAX;
			}
		}

		RECORD_PERF_END(originalCallAddr);
		return result;
	}





	// Technical declarations for Hook methods (creates small asm hook methods to dispatch to specific methods

	DECLARE_VIRTUAL_METHOD_HANDLER(ActivityProcessNotify)
	DECLARE_VIRTUAL_METHOD_HANDLER(PlayerProcessNotify)
	DECLARE_VIRTUAL_METHOD_HANDLER(UnitAddPositionToTargetPosArray)



	// Patches ROR process (.rdata section) to connect overloaded virtual methods
	// Don't forget to declare RockNRor entry point methods as "__stdcall"
	bool InstallVirtualMethodsPatches() {
		if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
			return true;
		}
		if (ROCKNROR::crInfo.configInfo.doNotUpdateVirtualMethods) {
			CR_DEBUG::AppendTextToLogFile("Did not update virtual methods because it is disabled in configuration", true);
			return false;
		}
		if (EnableWritingInRData(true)) {
			// This can cause a crash if memory is not writeable. That's why we use EnableWritingInRData and also a flag to disable this.

			// Activity (unitAI)
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_BASE, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_BASE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_BUILDING, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_BUILDING]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_CIVILIAN, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_CIVILIAN]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_FISHING_SHIP, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_FISHING_SHIP]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_GAIA_ELEPHANT, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_GAIA_ELEPHANT]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_LION, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_LION]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_MILITARY, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_MILITARY]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_NON_DISCOVERY_ARTEFACT, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_NON_DISCOVERY_ARTEFACT]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_PREDATOR_ANIMAL]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_PREY_ANIMAL, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_PREY_ANIMAL]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_PRIEST, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_PRIEST]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_TOWER, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_TOWER]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_TRADE_SHIP, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_TRADE_SHIP]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ACTIVITY_TRANSPORT_SHIP, 0xCC, ActivityProcessNotify, activityProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_UNIT_ACTIVITY_TRANSPORT_SHIP]);

			// Player
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_PLAYER, 0xE8, PlayerProcessNotify, playerProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_PLAYER]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_RGE_PLAYER, 0xE8, PlayerProcessNotify, playerProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_RGE_PLAYER]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_GAIA_PLAYER, 0xE8, PlayerProcessNotify, playerProcessNotifyCheckSumAndOriginalAddress[CHECKSUM_GAIA_PLAYER]);

			// Unit
			//unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_BASE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_BASE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_ATTACKABLE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_ATTACKABLE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_BUILDING, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_BUILDING]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_COMMANDABLE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_COMMANDABLE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_DOPPLEGANGER, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_DOPPLEGANGER]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_FLAG, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_FLAG]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_MOVABLE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_MOVABLE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_PROJECTILE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_PROJECTILE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_TRAINABLE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_TRAINABLE]);
			INSTALL_VIRTUAL_METHOD_PATCH(CHECKSUM_UNIT_TREE, 0x1BC, UnitAddPositionToTargetPosArray, unitAddPositionToTargetPosArrayCheckSumAndOriginalAddress[CHECKSUM_UNIT_TREE]);

			CR_DEBUG::AppendTextToLogFile("Virtual methods APIs have been written", true);

			// Restore read-only access on .rdata section.
			EnableWritingInRData(false);
			return true;
		} else {
			CR_DEBUG::AppendTextToLogFile("Failed to enable writing in rdata", true);
			return false;
		}
	}

}
}
