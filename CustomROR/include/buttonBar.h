
#pragma once

#include <AOE_struct_unit_def.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include <UI_components\AOE_struct_ui_button_with_number.h>
#include <UI\AOE_struct_ui_in_game_main.h>
#include "crCommon.h"
#include "CustomRORInfo.h"
#include "AOEPrimitives_units.h"
#include "buttonBarCommon.h"

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


// Add a command button in unit-commands zone (under game zone).
// buttonIndex: 0-4 = first row, 6-10=second row, 5 and 11 are "special" right buttons (11=unselect/cancel, generally)
// DATID can be a unitDefId (train), researchId (do_research)... Set it to 0 when not relevant.
// isDisabled : set it to true to get a read only button (no possible click)
// creationText can be left NULL to display a text using unit/research/etc's LanguageDLLID.
// iconSlpInfo can be left NULL unless we want to use specific SLP resource for icon.
// Technically, this just updates the button (no button object is created).
// Refresh may NOT be performed if underlying info (command id, DATID) are unchanged, which may lead to wrong updates
bool AddInGameCommandButton(long int buttonIndex, AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID UICmdId,
	long int DATID, bool isDisabled, const char *creationText, AOE_STRUCTURES::STRUCT_SLP_INFO *iconSlpInfo,
	bool forceUpdate);


}
