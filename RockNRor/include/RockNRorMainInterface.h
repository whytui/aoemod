#pragma once

#include <AOE_struct_game_settings.h>
#include <UI\AOE_struct_ui_in_game_menu.h>
#include <UI\AOE_struct_ui_diplomacy_dialog.h>
#include <UI\AOE_struct_ui_scenario_editor_menu.h>
#include <UI\AOE_struct_ui_mp_setup_screen.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_units.h>
#include "RockNRorInfo.h"
#include "RockNRorCommand.h"
#include "EditMapSizeXYPopup.h"
#include "EditorScenarioInfoPopup.h"
#include "InGameRockNRorOptionsPopup.h"
#include "EditorEditUnitInfoPopup.h"
#include "InGameUnitPropertiesPopup.h"
#include "MapCopyPopup.h"
#include "traceMessage.h"
#include "unitShortcuts.h"
#include "crDebugOutputPrimitives.h"
#include "playerHandling.h"

namespace ROCKNROR {


/* This class manages main interactions between ROR (via RockNRorInstance class) and RockNRor commands
 * Especially, UI events are managed here as our UI objects need to access RockNRorCommand object.
*/
class RockNRorMainInterface {
public:
	RockNRorMainInterface();
	~RockNRorMainInterface();


	// Manage key press in game screen and editor. (when UI components/screens themselves did not handle the event ?)
	// Returns true if event has been handled AND we don't want original code to try to handle it.
	// Warning: UI components may catch and handle events before so this method is NOT called. See crCommand.OnGameCommandButtonClick.
	bool GameAndEditor_OnKeyPress(long int pressedKey, bool CTRL, bool SHIFT, bool ALT);

	// Called for ALL button clicks in the game.
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool Global_OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *object);

	// Returns true if custom map generation has been executed. False if we want to let original game code apply.
	bool ScenarioEditor_callMyGenerateMapIfRelevant();

	// Manage right button release action on selected units for given player
	// Returns true if a red cross sign should be displayed (a relevant action occurred)
	bool ApplyRightClickReleaseOnSelectedUnits(AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *UIGameZone,
		AOE_STRUCTURES::STRUCT_PLAYER *player, long int mousePosX, long int mousePosY);

	// Change ROR's window title to add our mod name.
	bool ChangeWindowTitle();

private:
	bool windowTitleChanged;
};


// Global unique object
extern RockNRorMainInterface crMainInterface;
}
