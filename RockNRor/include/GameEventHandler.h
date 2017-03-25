#pragma once

#include <string>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_unit_activity.h>
#include <AOE_struct_player.h>
#include "RORVirtualMethodHelper.h"
#include "mainStructuresHandling.h"
#include "RockNRorCommand.h"


namespace ROCKNROR {
namespace GAME_EVENTS {
;

// This method is called for each player.NotifyEvent(...) call = it receives all event notifications at player level.
// Returns true if event is handled and ROR code must NOT be executed
// Returns false if ROR code should be executed normally (default case)
bool PlayerNotifyEvent(STRUCT_PLAYER *player, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT notifyEvent);


// UnitActivity.ProcessNotify event.
// outExecStandardCode is an output bool: if set to true, ROR standard code will be executed afterwards. If false, it will be skipped.
long int ActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int arg2, bool &outExecStandardCode);


// Handler for EVENT_PLAYER_SEE_UNIT event.
// Returns true if event is handled and ROR code must NOT be executed
// Returns false if ROR code should be executed normally (default case)
bool PlayerNotifySeeUnit(STRUCT_PLAYER *player, long int myUnitId, long int seenUnitId);


}
}
