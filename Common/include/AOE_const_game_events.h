
#pragma once


/* This file contains various constants that are used in game EXE
*/

namespace AOE_CONST_INTERNAL
{


	enum GAME_EVENT_SOUNDS : long int {
		CST_GES_ERROR_IMPOSSIBLE = 3,
		CST_GES_RESEARCH_COMPLETE = 5,
		CST_GES_PLAYER_DEATH = 6, // Any player's death (not specific to user-controlled)
		CST_GES_UNKNOWN_7_DISCONNECTED = 7,
		CST_GES_WONDER_STARTED = 8,
		CST_GES_WONDER_FINISHED = 9,
		CST_GES_WONDER_DESTROYED = 0x0A,
		CST_GES_ALL_ARTEFACTS_CAPTURED = 0x0B, // Relics or ruins
		CST_GES_UNIT_HAS_BEEN_CONVERTED = 0x0D,
		CST_GES_AGE_RESEARCHED = 0x0F,
		CST_GES_FARM_DEPLETED = 0x10
	};


	// Those IDs are used in various methods that deal with game events
	// - gameSettings.NotifyEvent(eventId, playerId, DATID, posY, posX) = [EDX+0x40]=0x501980. Arguments may have different roles. WARNING, here *arg1* is event ID !!!
	// - player.notifyEvent(unitId, arg2, eventId, arg4, arg5, arg6) (0x4F3350 / EDX+0xE8). Always calls player.handleEventInAI (if AI struct exists)
	// - player.handleEventInAI(unitId, arg2, eventId, arg4, arg5, arg6) (0x4F34C0)
	// - unit.notify(actorUnitId?, impactedUnitId?, notifyTaskId, generic_4, generic_5, generic_6) (0x426DB0 / EDX+0x148): dispatch to activity.addToNotifyQueue or player.notify
	// - activity.processNotify(struct NotifyEvent *, unsigned long) (0x413890 / EDX+0xCC)
	// - activity.notifyCommander(arg1, arg2, arg3, generic_4, generic_5, generic_6?). (0x410A20 / EDX+0x18)
	// - tacAI.reactToEvent(unitId, arg2, eventId, arg4, arg5, arg6) (0x4D7880 / EDX+0x40) : treatments only for events 0x72,0x74,0x201
	// 1-99 (0x63) : basic events/errors at game settings level
	// 100-499 (0x64-0x1F3): global events/errors (game settings level + player level for 0x72+ ?)
	// 500+ (0x1F4): More specific events (mostly handled at unitAI level, possibly player level ?)
	enum GAME_EVENT_TYPE : long int {
		EVENT_INVALID = -1, // For RockNRor internal usage
		EVENT_CANT_UNLOAD_NO_ROOM = 01,
		EVENT_CANT_UNLOAD_TOO_FAR = 02,
		EVENT_CANT_TRADE_WITH_ENEMY = 03,
		EVENT_UNKNOWN_04_PLAYER_DISCONNECTED = 04,
		EVENT_UNKNOWN_05_PLAYER_DISCONNECTED_ABANDONED = 05,
		EVENT_PLAYER_LOST = 06,

		// gameSettings.NotifyEvent: arg2=actorPlayerId, arg3=targetPlayerId, arg4=resourceId, arg5=resourceType
		EVENT_TRIBUTE_PAID = 07,

		EVENT_DIPLOMACY_CHANGED = 8,

		

		// A research just finished
		EVENT_RESEARCH_COMPLETE = 0x64,

		EVENT_CANCELED_RESEARCH = 0x65,
		EVENT_UNIT_SPAWNED = 0x66,
		EVENT_CANCEL_TRAIN_UNIT = 0x67,

		// When the trained unit can't spawn because there is no free tile for this (rarely happens in normal games !)
		EVENT_CANNOT_SPAWN_NO_ROOM = 0x68,

		// A construction just finished
		EVENT_BUILDING_COMPLETE = 0x69,

		EVENT_UNKNOWN_6A = 0x6A,

		// Reparing is no longer possible because resources are depleted.
		// See also EVENT_NOT_ENOUGH_RESOURCES for train/build missing resources errors
		EVENT_CANNOT_REPAIR_NO_RESOURCES = 0x6B,

		// Some player started the construction of a wonder
		EVENT_WONDER_START_BUILDING = 0x6C,

		// Some player finished constructing a wonder
		EVENT_WONDER_FINISHED = 0x6D,

		// Some player's wonder was just destroyed
		EVENT_WONDER_DESTROYED = 0x6E,

		// Some player just collected all relics
		EVENT_ALL_RELICS_CAPTURED = 0x72,

		// Some player just lost control over all relics
		EVENT_ALL_RELICS_LOST_CONTROL = 0x73,

		// Some player just collected all ruins
		EVENT_ALL_RUINS_CAPTURED = 0x74,
		
		// Some player just lost control over all ruins
		EVENT_ALL_RUINS_LOST_CONTROL = 0x75,

		// A conversion order failed because the priest does not have 100% faith
		EVENT_CANNOT_CONVERT_NO_FAITH = 0x76,

		// A priest conversion order failed because monotheism is not researched
		EVENT_CANNOT_CONVERT_NEED_MONOTHEISM_PRIEST = 0x77,

		// A building conversion order failed because monotheism is not researched
		EVENT_CANNOT_CONVERT_NEED_MONOTHEISM_BUILDING = 0x78,

		// A conversion order failed because TC can't be converted.
		EVENT_CANNOT_CONVERT_TC = 0x79,

		// A conversion order failed because wonders can't be converted.
		EVENT_CANNOT_CONVERT_WONDER = 0x7A,

		// A unit was just converted from a player to another. Current player may be "actor" or "victim".
		// NotifyEvent: arg2=actor playerId, arg3=victim playerId, arg4=posY, arg5=posX
		EVENT_SUCCESSFULLY_CONVERTED_UNIT = 0x7B,

		// Added or Removed a unit in building train queue
		EVENT_ADD_REMOVE_IN_TRAIN_QUEUE = 0x7C,

		// need houses, food/wood or maxPop reached (to build or train, NOT for repair)
		EVENT_NOT_ENOUGH_RESOURCES = 0x7D,

		// When a farm is depleted and dies
		// Sound 0x10
		EVENT_FARM_DEPLETED = 0x7E,
		

		
		// Notify being attacked. Triggers a player.NotifyEvent with event 0x201
		// genericParam4=attackerUnitId, genericParam5=? (not a unitid) genericParam6=? ; targetUnitId=attacker, actorUnitId=me
		EVENT_BEING_ATTACKED = 0x1F4,

		EVENT_UNUSED_1F5 = 0x1F5, // Not used at all.
		EVENT_UNUSED_1F6 = 0x1F6, // Not used at all.
		EVENT_UNUSED_1F7 = 0x1F7, // Not used at all.
		EVENT_UNUSED_1F8 = 0x1F8, // Not used at all.
		EVENT_ACTION_FAILED = 0x1F9, // Triggered in 0x405786, ...
		EVENT_ACTION_COMPLETED = 0x1FA, // Notify activity completed (normal completion). arg4=activityId
		EVENT_ACTION_INVALIDATED = 0x1FB, // 0x410999

		// Source: 0x40580C in action.update().
		// Action re-triggered.
		// Cause=action's movement completed but target is still too far (target moved)
		// The "target is no longer visible" is a sub-case of this. See 0x4E3FB8(for predator)
		EVENT_UNKNOWN_1FC_NEED_MOVE_AGAIN = 0x1FC,

		// Source: 0x401C7B. See 0x4143B7.
		// Target becomes too far to be shot at ? (may still be visible, but out of reach)
		EVENT_TOO_FAR_TO_SHOOT = 0x1FD,
		
		// When distance is too low to attack (for siege...). Set in 0x401BCF,0x401E6F only. See 0x41426A.
		EVENT_TOO_CLOSE_TO_SHOOT = 0x1FE,
		
		// To confirm. See 0x4143B7 (part of the "default" switch cases). Used in early versions only ? Never triggered.
		EVENT_UNIT_SAW_ENEMY_UNIT = 0x1FF,

		// Notify the unit should move back to its max range after shooting to a target. 0x4E646B. Generic_arg4=taskid(0x258)
		// genericParam4=activityId to restore afterwards (attack...), genericParam5=targetUnitId ?
		EVENT_SHOULD_MOVE_BACK_AFTER_SHOOTING = 0x200,

		// Player notification: some unit was attacked. See also 0x1F4
		EVENT_PLAYER_UNIT_ATTACKED = 0x201,

		// target gatherable unit is depleted? Movement finished, including "exploration basic move" ?
		// genericParam4=activityId(explore...)
		EVENT_MOVEMENT_FINISHED_UNSURE = 0x202,

		// Source 0x403392, 0x4B38C1, 0x4B6A43
		// Triggered when no drop site was found for a unit (with resource value to bring back)
		// Both arg1/2 = unitId of gatherer or trading unit.
		// Generic arg4=concerned task ID (trade, gather with/without attack)
		EVENT_NO_DROP_SITE = 0x203,

		// 204? 205?
		EVENT_TOO_FAR_FROM_LAND = 0x206, // to confirm [player notif]
		EVENT_CANNOT_UNLOAD = 0x207, // Not enough space to unload ? To confirm [player notif]
		EVENT_UNKNOWN_208 = 0x208, // for early versions only ? Seems to be unused here.

		// Related to notification when being attacked ? see 0x4E4769. When triggered, AI player adds 10 dislike for target player
		// generic_arg4=unitId (attacker, to which add dislike)
		EVENT_UNKNOWN_209 = 0x209,

		// Source 0x42ADFC
		// arg1=playerId that PAYS the tribute
		// arg2=playerId that receives the tribute ("target")
		// arg4=resource type
		// Notification is sent at player level (to target player cf arg2)
		EVENT_TRIBUTE_RECEIVED = 0x20A,

		// Unit captured (on actor=new owner side) : conversion+capture of artefacts/gaia units. 0x4A9FB7, 0x4AEBDD
		// arg1=arg2=captured unit id
		// arg4=player Id (new owner="actor")
		// Notification is sent to unit then falls back at player level
		EVENT_UNIT_CAPTURED = 0x20B,

		EVENT_UNUSED_20C = 0x20C, // Not used at all.

		// Source 0x413143 in activity.triggerSeeUnitEvents()? (under activity's timerUpdate)
		// This event is used whereas 1FF seems not.
		// Important: Only units with a "UnitAI" can "see" other units (to trigger the event and update infAI)
		// But ALL units with a UnitAI do run this detection (each "delayBetweenDetectSeeUnits" milliseconds).
		// Detection frequency is quite poor (4 seconds+a random part 0x413183), units may move 5 or even 10 tiles (fast units) between 2 detections.
		// Sent to player.notify and handled in player.handleEventInAI: adds unit in infAI list (this can be done a lot of times for the same unit, ok cause there won't be duplicates, but bad for perf)
		// arg1=unitId that "sees" arg2=playerId, arg4=other unit Id
		EVENT_PLAYER_SEE_UNIT = 0x20D,

		// Source=0x426651 (in send projectile code)
		// Notification triggered when someone shoots a projectile at "me". "escape attack" 0x4E62F3
		// arg1=actor unitId (shooter), arg2=target unitId(the one that needs to escape), arg4=actor unitId (shooter), arg5=currentHP, arg6=maxHP
		EVENT_SHOULD_ESCAPE_ATTACK = 0x20F,

		// This one is not an event (it is a task), but this value is found in several places, and seems to be dead code.
		EVENT_UNSURE_ATTACK = 0x258,

		// (699) Stop being targeted by another unit (any action: repair,attack,heal,...).
		// This occurs when the other unit (that targets "me") dies or changes target.
		// Used as a *notification*. Generic_arg4=otherUnitClass. Related to update of unit.unitCountThatAreTargetingMe
		// arg1=unitID that stops targeting, arg2=unitID that is no longer targeted by the other one.
		// Some units do NOT raise this event: trees, mines, etc
		EVENT_RELEASE_BEING_WORKED_ON = 0x2BB
		// 0x2BB is last (higher values are *orders*)
	};

}