#include "../include/RockNRorVirtualMethodHooks.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
;

namespace VIRTUAL_METHOD_HOOKS {

	// Maps to store ORIGINAL call addresses. Index=class checksum, value=original method call address
	std::map<unsigned long int, unsigned long int> activityProcessNotifyCheckSumAndOriginalAddress;
	std::map<unsigned long int, unsigned long int> playerProcessNotifyCheckSumAndOriginalAddress;


	// Return value is an unknown enum. 2=ok, processed.
	long int __stdcall ActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int arg2) {
		assert(activity && activity->IsCheckSumValid() && notifyEvent);
		if (!activity || !activity->IsCheckSumValid() || !notifyEvent) { return 3; } // TODO: return which value ? Meaning?
		
		unsigned long int originalCallAddr = activityProcessNotifyCheckSumAndOriginalAddress[activity->checksum];
		bool runStandardMethod = true;

		// Custom treatments
		int result = ROCKNROR::crCommand.ActivityProcessNotify(activity, notifyEvent, arg2, runStandardMethod); // updates runStandardMethod

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
		return result;
	}


	// Returns void. This hook handles player's notifications.
	void __stdcall PlayerProcessNotify(STRUCT_PLAYER *player, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT notifyEvent) {
		assert(player && player->IsCheckSumValid());
		if (!player || !player->IsCheckSumValid()) { return; }
		unsigned long int originalCallAddr = playerProcessNotifyCheckSumAndOriginalAddress[player->checksum];
		bool runStandardMethod = true;

		// Custom treatments
		// runStandardMethod = ...

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
	}




	// Technical declaration for Hook methods (creates small asm hook methods to dispatch to specific methods

	DECLARE_VIRTUAL_METHOD_HANDLER(ActivityProcessNotify)
	DECLARE_VIRTUAL_METHOD_HANDLER(PlayerProcessNotify)




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
