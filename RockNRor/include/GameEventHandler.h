#pragma once

#include <string>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_activity.h>
#include <AOE_struct_player.h>
#include "RORVirtualMethodHelper.h"
#include "mainStructuresHandling.h"
#include "RockNRorCommand.h"
#include "AOEPrimitives_gameCommand.h"
#include "AOEPrimitives_units.h"


namespace ROCKNROR {
namespace GAME_EVENTS {
;

// This method is called for each player.NotifyEvent(...) call = it receives all event notifications at player level.
// Returns true if event is handled and ROR code must NOT be executed
// Returns false if ROR code should be executed normally (default case)
bool PlayerNotifyEvent(STRUCT_PLAYER *player, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT notifyEvent);


// UnitActivity.ProcessNotify event for ALL activity classes (base+children)
// outExecStandardCode is an output bool: if set to true, ROR standard code will be executed afterwards. If false, it will be skipped.
ACTIVITY_EVENT_HANDLER_RESULT ActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int uTime, bool &outExecStandardCode);


// UnitActivity.ProcessNotify event for ALL activity classes (base+children)
// outExecStandardCode is an output bool: if set to true, ROR standard code will be executed afterwards. If false, it will be skipped.
ACTIVITY_EVENT_HANDLER_RESULT CivilianActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int uTime, bool &outExecStandardCode);


// Handler for EVENT_PLAYER_SEE_UNIT event, which is triggered from activity.update() execution every x milliseconds.
// Returns true if event is handled and ROR code must NOT be executed
// Returns false if ROR code should be executed normally (default case)
// Note: the units that are detected via this event are limited (cf 0x413108)
// - unit.status <=2 (the temp results contains other units(?), but this is filtered in 0x41312B before sending EVENT_PLAYER_SEE_UNIT.
// - neutral and enemy units only (so, not gaia !)
// - unit.definition.unitAIType is one of the "important" AItypes defined in actorUnit.activity.importantAITypes
// Which means, according to the unit that "sees", some units may be ignored ! Especially all gaia units.
// Remark: See also RockNRorInstance::OnAttackableUnitUpdateVisibility = entry point that adds/updates infAI elements for type50+ units.
bool PlayerNotifySeeUnit(STRUCT_PLAYER *player, long int myUnitId, long int seenUnitId);


}
}
