#include "../include/GameEventHandler.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace GAME_EVENTS {
;

// Returns true if event is handled and ROR code must NOT be executed
// Returns false if ROR code should be executed normally (default case)
bool PlayerNotifyEvent(STRUCT_PLAYER *player, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT notifyEvent) {
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return false; }

	// TODO: determines the conditions under which units are detected. Not gaia units ? status=2...

	if (notifyEvent.eventId == AOE_CONST_INTERNAL::EVENT_PLAYER_SEE_UNIT) {
		// TODO: overwrite the "insert in infai" treatment

		long int myUnitId = notifyEvent.targetUnitId;
		long int seenUnitId = notifyEvent.genericParam4;
		//long int argPlayerId = notifyEvent.actorUnitId;
		//long int myPlayerId = player->playerId;
		STRUCT_UNIT_BASE *seenUnit = GetUnitStruct(seenUnitId);
		STRUCT_UNIT_BASE *myUnit = GetUnitStruct(myUnitId);
		if (!seenUnit || !myUnit || !seenUnit->IsCheckSumValidForAUnitClass() || !myUnit->IsCheckSumValidForAUnitClass()) {
			return false;
		}
		COMBAT::OnSeeNearbyUnit(player, myUnit, seenUnit);
	}

	return false; // default: let ROR code be run
}


// UnitActivity.ProcessNotify event.
// outExecStandardCode is an output bool: if set to true, ROR standard code will be executed afterwards. If false, it will be skipped.
long int ActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int arg2, bool &outExecStandardCode) {
	outExecStandardCode = true;

	/*std::string msg1 = "ActivityProcessNotify ";
	msg1 += GetHexStringAddress(notifyEvent->activityId, 3);
	AOE_METHODS::UI_BASE::CallWriteText(msg1.c_str());*/

	// Custom treatments
#ifndef _DEBUG
	return;
#endif;
	static std::set<AOE_CONST_INTERNAL::GAME_EVENT_TYPE> test;
	AOE_CONST_INTERNAL::GAME_EVENT_TYPE notificationTaskId = notifyEvent->eventId;
	test.insert(notificationTaskId);
	// TODO: CST_ATI_NOTIFY_TOO_CLOSE_TO_SHOOT : search another target or move away ?
	/*if (argNotificationId == 0x20D) {
	AOE_METHODS::CallWriteCenteredText("20D");
	}*/
	if (!activity || !activity->ptrUnit || !activity->ptrUnit->ptrStructPlayer) { return 3; } // value to return ??
	std::string msg = "p#";
	msg += std::to_string(activity->ptrUnit->ptrStructPlayer->playerId);
	msg += " u#";
	msg += std::to_string(activity->ptrUnit->unitInstanceId);
	msg += " ";
	if (activity->ptrUnit->unitDefinition) {
		msg += activity->ptrUnit->unitDefinition->ptrUnitName;
	}
	msg += " ";
	msg += GetHexStringAddress(notificationTaskId, 3);
	//msg += std::to_string((int)notificationTaskId);

	switch (notificationTaskId) {
	case GAME_EVENT_TYPE::EVENT_TOO_FAR_TO_SHOOT: // See 0x4143B7
		break;
	case GAME_EVENT_TYPE::EVENT_UNKNOWN_1FC_NEED_MOVE_AGAIN: // Target moved ? The "target is no longer visible" is a sub-case of this. See 4E3FB8(for predator)
		/*AOE_METHODS::CallWriteCenteredText(msg.c_str());
		AOE_METHODS::PLAYER::CopyScreenPosition(GetControlledPlayerStruct_Settings(), activity->ptrUnit->ptrStructPlayer);
		AOE_METHODS::PLAYER::ChangeControlledPlayer(activity->ptrUnit->ptrStructPlayer->playerId, false);
		SelectOneUnit(GetControlledPlayerStruct_Settings(), activity->ptrUnit, true);
		AOE_METHODS::SetGamePause(true);*/
		break;
	case GAME_EVENT_TYPE::EVENT_BEING_ATTACKED: // 1f4 -> TODO: if attacker is under "min range": just move away if I am idle, ignore event otherwise
	case GAME_EVENT_TYPE::EVENT_ACTION_FAILED: // 1F9
	case GAME_EVENT_TYPE::EVENT_ACTION_COMPLETED:
	case GAME_EVENT_TYPE::EVENT_ACTION_INVALIDATED:
	case GAME_EVENT_TYPE::EVENT_SHOULD_MOVE_BACK_AFTER_SHOOTING: // 200
	case GAME_EVENT_TYPE::EVENT_MOVEMENT_FINISHED_UNSURE: // target gatherable unit is depleted? Movement finished, including "exploration basic move" ?
	case GAME_EVENT_TYPE::EVENT_SHOULD_ESCAPE_ATTACK: // 20f
	case GAME_EVENT_TYPE::EVENT_TOO_CLOSE_TO_SHOOT: // 1FE
	case GAME_EVENT_TYPE::EVENT_UNIT_CAPTURED: // 20B
	case GAME_EVENT_TYPE::EVENT_RELEASE_BEING_WORKED_ON: // >0x258 but still a notification !
		// Usual cases...
		break;
	case GAME_EVENT_TYPE::EVENT_UNSURE_ATTACK: //0x258 (should not be a notification?) There is code for this... dead (wrong?obsolete?)code ?
		msg += " (task_attack)";
		AOE_METHODS::CallWriteCenteredText(msg.c_str());
		AOE_METHODS::PLAYER::CopyScreenPosition(GetControlledPlayerStruct_Settings(), activity->ptrUnit->ptrStructPlayer);
		AOE_METHODS::PLAYER::ChangeControlledPlayer(activity->ptrUnit->ptrStructPlayer->playerId, false);
		SelectOneUnit(GetControlledPlayerStruct_Settings(), activity->ptrUnit, true);
		AOE_METHODS::SetGamePause(true);
		break;
	default:
		msg += " (unknown)";
		AOE_METHODS::CallWriteCenteredText(msg.c_str());
		AOE_METHODS::PLAYER::CopyScreenPosition(GetControlledPlayerStruct_Settings(), activity->ptrUnit->ptrStructPlayer);
		AOE_METHODS::PLAYER::ChangeControlledPlayer(activity->ptrUnit->ptrStructPlayer->playerId, false);
		SelectOneUnit(GetControlledPlayerStruct_Settings(), activity->ptrUnit, true);
		AOE_METHODS::SetGamePause(true);
		break;
	}
	return 2;
}


}
}
