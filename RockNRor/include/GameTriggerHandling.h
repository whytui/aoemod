#pragma once
#include <string>
#include <AOE_struct_player.h>
#include <AOE_struct_units.h>
#include <triggerDefinition.h>
#include "RockNRorInfo.h"
#include "crTrigger.h"
#include "mainStructuresHandling.h"
#include "AOEPrimitives_UI_gameMain.h"
#include "AOEPrimitives_gameCommand.h"
#include "researches.h"
#include "unitHandling.h"
#include "GameTriggerTechInterfacing.h"


using namespace AOE_STRUCTURES;

namespace CUSTOMROR {
namespace TRIGGER {
;



// Just execute a "standard" trigger. this only does the action part, does not check and does not manage other impacts.
// Do not call directly.
void Trigger_JustDoAction(CR_TRIGGERS::crTrigger *trigger);

// Execute a trigger (considering requirements are OK)
void ExecuteTriggerAction(CR_TRIGGERS::crTrigger *trigger);


// Manage triggers execution for a specific event
void ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES evt);
void ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES evt, CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER &evtInfo);

// Export RockNRor's internal data triggers to Trigger text format.
std::string GetTriggerTextFromInternalData();


void ReloadTriggersFromGameData();
void ReloadTriggersFromGameData(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scenarioInfo);
// Write trigger to game data, using RockNRor internal data.
bool WriteTriggersFromInternalToGameData(bool onlyEnabledTriggers);
// Write trigger to game data, using provided text trigger data.
bool WriteTriggersInGameData(char *triggerText);

// This is dedicated to triggers handling on "GameSettingsNotifyEvent" event.
void ManageTriggersOnGameNotifyEvent(long int eventId, short int playerId, long int arg3, long int arg4, long int arg5);

// Manage disable (via trigger) units for villager, house, fishing ship, trade ship, farms
// This method should be called at game start, after "initial" technologies have been applied, so we can override some (and force to disable again some units)
void ManageTriggerDisableUnitsForExceptions();

// Manage "disable farm" when specified in triggers. Actually, prevent market construction from enabling farms.
// Farm is enabled by market construction, which may occur both "ingame" or at game loading (if a market exists, or if starting at bronze).
void ManageDisableUnitsForFarms(AOE_STRUCTURES::STRUCT_PLAYER *player);


// This is called while scenarioInfo structure is read from a file
// Scenario texts (including players strategies) have already been read, but not other information, be careful.
void InitScenarioInfoTextData(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scenarioInfo);

// This method handles the "disable unit" and "disable research" defined in custom triggers.
// This is called in "disable research" init at game start (for scenarios)
// This is only executed for scenarios, not DM/RM !
void OnGameInitDisableResearchesEvent(AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *playerResearchInfo);


}

}
