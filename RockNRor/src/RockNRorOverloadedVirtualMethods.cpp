#include "../include/RockNRorOverloadedVirtualMethods.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
;

namespace OVERLOADED_VIRTUAL_METHODS {

	// Index=class checksum, value=original address for ActivityProcessNotify
	std::map<unsigned long int, unsigned long int> activityProcessNotifyCheckSumAndOriginalAddress;


	// Return value is an unknown enum. 2=ok, processed.
	long int __stdcall ActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int arg2) {
		assert(activity && activity->IsCheckSumValid() && notifyEvent);
		if (!activity || !activity->IsCheckSumValid() || !notifyEvent) { return 3; } // TODO: return which value ? Meaning?
		
		bool runStandardMethod = true;
		int result = ROCKNROR::crCommand.ActivityProcessNotify(activity, notifyEvent, arg2, runStandardMethod); // updates runStandardMethod

		unsigned long int originalCallAddr = activityProcessNotifyCheckSumAndOriginalAddress[activity->checksum];

		if (runStandardMethod && (originalCallAddr != 0)) {
			_asm {
				MOV ECX, activity;
				MOV EDX, DS:[ECX];
				PUSH arg2;
				PUSH notifyEvent;
				// DO NOT CALL DS:[EDX+0xCC] because we changed it !
				CALL originalCallAddr;
				MOV result, EAX;
			}
		}
		return result;
	}


	DECLARE_VIRTUAL_METHOD_HANDLER(ActivityProcessNotify)




	// Patches ROR process (.rdata section) to connect overloaded virtual methods
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
