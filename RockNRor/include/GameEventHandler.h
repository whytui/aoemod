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

// Returns true if event is handled and ROR code must NOT be executed
// Returns false if ROR code should be executed normally (default case)
bool PlayerNotifyEvent(STRUCT_PLAYER *player, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT notifyEvent);


// UnitActivity.ProcessNotify event.
// outExecStandardCode is an output bool: if set to true, ROR standard code will be executed afterwards. If false, it will be skipped.
long int ActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int arg2, bool &outExecStandardCode);


}
}
