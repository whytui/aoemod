
#pragma once

#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include <UI_components\AOE_struct_ui_button_with_number.h>
#include <UI\AOE_struct_ui_in_game_main.h>
#include "crCommon.h"
#include "CustomRORInfo.h"
#include "AOEPrimitives_units.h"
#include "buttonBarConst.h"

namespace BUTTONBAR {


// Hide a button from buttonbar. cmdButtonId should be in [0,11]
void HideCommandButton(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, int cmdButtonId);

// Force refresh of game button bar
void ForceRefresh(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI);
void ForceRefresh();

// Refresh status for custom auto-attack policy buttons
void RefreshCustomAutoAttackButtons(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, const AutoAttackPolicy *attackPolicy);


// Called at the end of showUnitCommandButtons
void AfterShowUnitCommandButtons(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI);


// Add relevant buttons in command bar for "trainable" unit (type=70). Excludes all other unit types, even child classes (buildings)
void AddButtonsForLivingUnit(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);


// Add relevant buttons in command bar for "building" unit (type=80). Excludes all other unit types.
void AddButtonsForBuildingUnit(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, AOE_STRUCTURES::STRUCT_UNIT_BUILDING *unit);


// Update the whole button bar when user clicks "defend unit/zone"
void SetButtonBarForDefendUnitOrZone(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit);


}
