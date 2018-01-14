#include "../include/GameEventHandler.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace GAME_EVENTS {
;

// This method is called for each player.NotifyEvent(...) call = it receives all event notifications at player level.
// Returns true if event is handled and ROR code must NOT be executed
// Returns false if ROR code should be executed normally (default case)
bool PlayerNotifyEvent(STRUCT_PLAYER *player, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT notifyEvent) {
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return false; }

	if (notifyEvent.eventId == AOE_CONST_INTERNAL::EVENT_PLAYER_SEE_UNIT) {
		long int myUnitId = notifyEvent.callerUnitId;
		long int seenUnitId = notifyEvent.genericParam4;
		// long int argPlayerId = notifyEvent.actorUnitId; // Not sure of the role of this value
		PlayerNotifySeeUnit(player, myUnitId, seenUnitId);
	}

	/*if (notifyEvent.eventId == AOE_CONST_INTERNAL::EVENT_TOO_CLOSE_TO_SHOOT) {
		// Handled in ActivityProcessNotify
	}*/

	/*if (notifyEvent.eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_CANNOT_REPAIR_NO_RESOURCES) {
		// This event is NOT sent here (which is bad because AI players don't get it)
	}*/

	return false; // default: let ROR code be run
}


// UnitActivity.ProcessNotify event for ALL activity classes (base+children)
// outExecStandardCode is an output bool: if set to true, ROR standard code will be executed afterwards. If false, it will be skipped.
ACTIVITY_EVENT_HANDLER_RESULT ActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int uTime, bool &outExecStandardCode) {
	outExecStandardCode = true;
	if (!notifyEvent || !activity || !activity->IsCheckSumValid()) {
		return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION;
	}

	if (activity->checksum == CHECKSUM_UNIT_ACTIVITY_CIVILIAN) {
		return CivilianActivityProcessNotify(activity, notifyEvent, uTime, outExecStandardCode);
	}

	if (notifyEvent->eventId == AOE_CONST_INTERNAL::EVENT_TOO_CLOSE_TO_SHOOT) {
		// TODO
	}

	if (notifyEvent->eventId == AOE_CONST_INTERNAL::EVENT_BECOME_TOO_FAR_TO_SHOOT) {
	}
	if (notifyEvent->eventId == AOE_CONST_INTERNAL::EVENT_NO_DROP_SITE) {
		// If a drop site is being built, go help in construction (if reachable)
		// Otherwise, try to build one ?
	}


	/*std::string msg1 = "ActivityProcessNotify ";
	msg1 += GetHexStringAddress(notifyEvent->activityId, 3);
	AOE_METHODS::UI_BASE::CallWriteText(msg1.c_str());*/

	// Custom treatments
#ifndef _DEBUG
	return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION;
#endif;
	static std::set<AOE_CONST_INTERNAL::GAME_EVENT_TYPE> test;
	AOE_CONST_INTERNAL::GAME_EVENT_TYPE notificationTaskId = notifyEvent->eventId;
	test.insert(notificationTaskId);

	if (!activity || !activity->ptrUnit || !activity->ptrUnit->ptrStructPlayer) {
		return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION;
	}
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
	case GAME_EVENT_TYPE::EVENT_BECOME_TOO_FAR_TO_SHOOT: // See 0x4143B7
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
	case GAME_EVENT_TYPE::EVENT_ACTION_COMPLETED_SEARCH: // 202
	case GAME_EVENT_TYPE::EVENT_SHOULD_ESCAPE_ATTACK: // 20f
	case GAME_EVENT_TYPE::EVENT_TOO_CLOSE_TO_SHOOT: // 1FE
	case GAME_EVENT_TYPE::EVENT_UNIT_CAPTURED: // 20B
	case GAME_EVENT_TYPE::EVENT_RELEASE_BEING_WORKED_ON: // >0x258 but still a notification !
	case GAME_EVENT_TYPE::EVENT_NO_DROP_SITE:
		// Usual cases...
		break;
	case GAME_EVENT_TYPE::EVENT_INVALID_VALUE__ATTACK: //0x258 (should not be a notification?) There is code for this... dead (wrong?obsolete?)code ?
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
	return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION;
}


// UnitActivity.ProcessNotify event for ALL activity classes (base+children)
// outExecStandardCode is an output bool: if set to true, ROR standard code will be executed afterwards. If false, it will be skipped.
ACTIVITY_EVENT_HANDLER_RESULT CivilianActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int uTime, bool &outExecStandardCode) {
	outExecStandardCode = true;
	if (!notifyEvent || !activity || !activity->IsCheckSumValid()) {
		return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION;
	}
	// This is restricted to "AI improvements ON" configuration.
	if (!activity || !activity->IsCheckSumValid() || !notifyEvent || !activity->ptrUnit ||
		!activity->ptrUnit->ptrStructPlayer || !IsImproveAIEnabled(activity->ptrUnit->ptrStructPlayer->playerId)) {
		return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION;
	}

	if (notifyEvent->eventId == GAME_EVENT_TYPE::EVENT_SHOULD_MOVE_BACK_AFTER_SHOOTING) {
		// Retreat after shooting is not handled in ROR original code for civilians. It should (for hunters) !
		if (COMBAT::HunterMoveBackAfterShooting(activity, notifyEvent)) {
			outExecStandardCode = false; // Event has been processed with an action. We don't want to run ROR handler.
			return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_HANDLED_WITH_AN_ACTION;
		} else {
			// Normal execution (actually, ROR won't do much with this event)
			return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION;
		}
	}
	if (notifyEvent->eventId == GAME_EVENT_TYPE::EVENT_BEING_ATTACKED) {
		if (activity->currentTaskId == ACTIVITY_TASK_ID::CST_ATI_TASK_MOVE) {
			// Villager should not stop moving (to strike back to animal) when he's not ready to shoot
			for (int i = 0; i < activity->orderQueueUsedElemCount; i++) {
				if ((activity->orderQueue[i].targetUnitId == notifyEvent->callerUnitId) &&
					(activity->orderQueue[i].orderId == UNIT_AI_ORDER::CST_ORDER_GATHER_ATTACK)) {
					if (!AOE_METHODS::UNIT::IsReadyToAttack(activity->ptrUnit)) {
						outExecStandardCode = false; // We don't want to run ROR handler in this case.
						return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_HANDLED_WITH_AN_ACTION; // Ignore the notification because a pending order already concerns this "attacker" unit and I am not ready yet to strike back.
					}
				}
			}
		}
	}
	if (notifyEvent->eventId == GAME_EVENT_TYPE::EVENT_ACTION_INVALIDATED) {
		if (activity->currentTaskId == ACTIVITY_TASK_ID::CST_ATI_TASK_REPAIR) {
			STRUCT_UNIT_ATTACKABLE *villager = (STRUCT_UNIT_ATTACKABLE *)activity->ptrUnit;
			STRUCT_PLAYER *player = villager->ptrStructPlayer;
			if (!villager->DerivesFromAttackable()) { return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION; }
			STRUCT_ACTION_BASE *action = GetUnitAction(villager);
			if (!action || !action->targetUnit || (action->targetUnit->unitDefinition == NULL) ||
				(action->targetUnit->checksum != CHECKSUM_UNIT_BUILDING)) {
				return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION;
			}
			STRUCT_UNITDEF_BUILDING *bldDef = (STRUCT_UNITDEF_BUILDING*)action->targetUnit->unitDefinition;
			for (int i = 0; i < 3; i++) {
				RESOURCE_TYPES reqResourceId = bldDef->costs[i].costType;
				if ((reqResourceId >= 0) && (reqResourceId <= 3) && (bldDef->costs[i].costPaid != 0) && (bldDef->costs[i].costAmount > 0)) {
					// This resource is used to repair.
					float remainingRes = player->GetResourceValue(reqResourceId);
					if (remainingRes < 0.1) {
						// This resource is missing. Go find some ! (TODO: go gather the correct resource if possible)
						// At least, stop trying to repair (AI villagers tend to get stuck). Seems not so bad for now (AI will give another task)
						// The tacAI.Update method must ensure not to retry if some resource is missing.
						if (GAME_COMMANDS::CreateCmd_Stop(villager->unitInstanceId)) {
							return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_HANDLED_WITH_AN_ACTION; // Event has been processed with an action. We don't want to run ROR handler.
						}
					}
				}
			}
		}
	}
	if ((notifyEvent->eventId == GAME_EVENT_TYPE::EVENT_ACTION_COMPLETED) || (notifyEvent->eventId == GAME_EVENT_TYPE::EVENT_ACTION_COMPLETED_SEARCH)) {
		ACTIVITY_TASK_ID eventTaskId = (ACTIVITY_TASK_ID)notifyEvent->genericParam4;
		if (eventTaskId == ACTIVITY_TASK_ID::CST_ATI_TASK_BUILD) {
		//if (activity->currentTaskId == ACTIVITY_TASK_ID::CST_ATI_TASK_BUILD) {
			STRUCT_UNIT_ATTACKABLE *villager = (STRUCT_UNIT_ATTACKABLE *)activity->ptrUnit;
			long int buildingId = activity->targetUnitId;
			STRUCT_UNIT_BASE *buildingUnit = GetUnitStruct(buildingId);
			bool wasBuildingFarm = (buildingUnit && buildingUnit->IsCheckSumValidForAUnitClass() && buildingUnit->unitDefinition &&
				(buildingUnit->unitDefinition->DAT_ID1 == CST_UNITID_FARM));
			if (wasBuildingFarm) {
				// Remark: if several villagers were building this farm, CanBuilderSwitchToFarmer won't detect them if "gather" commands have not been handled yet (which generally is the case)
				if (AOE_STRUCTURES::CanBuilderSwitchToFarmer(villager, buildingUnit)) {
					// This corresponds to the hack in 0x4B1A91
					// Not really an issue here because it's always visible (cf builder), but technically CreateCmd_RightClick is not fully MP-compliant
					GAME_COMMANDS::CreateCmd_RightClick(villager->unitInstanceId, buildingId, buildingUnit->positionX, buildingUnit->positionY);
				}
			}
		}
	}
	return ACTIVITY_EVENT_HANDLER_RESULT::EVT_RES_EVENT_PROCESSED_NO_ACTION;
}


// Handler for EVENT_PLAYER_SEE_UNIT event, which is triggered from activity.update() execution every x milliseconds.
// Returns true if event is handled and ROR code must NOT be executed
// Returns false if ROR code should be executed normally (default case)
// Note: the units that are detected via this event are limited (cf 0x413108)
// - unit.status <=2 (the temp results contains other units(?), but this is filtered in 0x41312B before sending EVENT_PLAYER_SEE_UNIT.
// - neutral and enemy units only (so, not gaia !)
// - unit.definition.unitAIType is one of the "important" AItypes defined in actorUnit.activity.importantAITypes
// Which means, according to the unit that "sees", some units may be ignored ! Especially all gaia units.
// Remark: See also RockNRorInstance::OnAttackableUnitUpdateVisibility = entry point that adds/updates infAI elements for type50+ units.
bool PlayerNotifySeeUnit(STRUCT_PLAYER *player, long int myUnitId, long int seenUnitId) {
	// Remark: here the "temp results" arrays contain nearby units (caller is looping on it) ; but you should NOT use it ;)
	/*int n = GetElemCountInUnitListTempSearchResult(PLAYER_DIPLOMACY_VALUES::CST_PDV_ENEMY);
	STRUCT_NEARBY_UNIT_INFO *e = GetUnitListTempSearchResult(PLAYER_DIPLOMACY_VALUES::CST_PDV_ENEMY, (resultIndex));*/

	if (!IsImproveAIEnabled(player->playerId)) {
		return false; // Let standard ROR code be executed. Don't use optimizations/improvements
	}
	// Note: this does not required AI to be active, just AI structures to exist.
	// It is useful (*vital*) to maintain infAI data if AI is temporarily disabled

	STRUCT_UNIT_BASE *seenUnit = GetUnitStruct(seenUnitId);
	STRUCT_UNIT_BASE *myUnit = GetUnitStruct(myUnitId);
	if (!seenUnit || !myUnit || !seenUnit->IsCheckSumValidForAUnitClass() || !myUnit->IsCheckSumValidForAUnitClass()) {
		return false;
	}
	bool result = false; // Default (for non-type50+ units): Let standard code be executed...
	if (seenUnit->unitDefinition && seenUnit->DerivesFromAttackable()) {
		// Prevent ROR from updating infAI list: unitExtensions+visibility change hooks handle this (and are better)
		// Use quick-update instead (retrieve infAI elem index from cache)
		ROCKNROR::unitExtensionHandler.AddUpdateUnitInInfAILists(seenUnit, player->playerId);
		result = true;
	} else {
		// Non-type50 are not handled by visibility change hooks (as those units do NOT have the visibility back-pointer updates).
		// Let standard code be executed...
		// result = false;
	}
	COMBAT::OnSeeNearbyUnit(player, myUnit, seenUnit);
	return result;
}


}
}
