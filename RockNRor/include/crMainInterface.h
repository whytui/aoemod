#include <AOE_struct_game_settings.h>
#include <UI\AOE_struct_ui_in_game_menu.h>
#include <UI\AOE_struct_ui_diplomacy_dialog.h>
#include <UI\AOE_struct_ui_scenario_editor_menu.h>
#include <UI\AOE_struct_ui_mp_setup_screen.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_units.h>
#include "CustomRORInfo.h"
#include "crCommands.h"
#include "CustomPopupBase.h"
#include "EditMapSizeXYPopup.h"
#include "EditTerrainPopup.h"
#include "EditorScenarioInfoPopup.h"
#include "InGameCustomRorOptionsPopup.h"
#include "EditorEditUnitInfoPopup.h"
#include "InGameUnitPropertiesPopup.h"
#include "GenNewTriggerPopup.h"
#include "MapCopyPopup.h"
#include "traceMessage.h"
#include "unitShortcuts.h"
#include "crDebugOutputPrimitives.h"
#include "playerHandling.h"
#include "CustomPopupSystem.h"

namespace CUSTOMROR {


/* This class manages main interactions between ROR (via CustomRORInstance class) and RockNRor commands
 * Especially, UI events are managed here as our UI objects need to access CustomRORCommand object.
*/
class CustomRORMainInterface {
public:
	CustomRORMainInterface();
	~CustomRORMainInterface();


	// Manage key press in game screen and editor.
	// Returns true if event has been handled AND we don't want original code to try to handle it.
	bool GameAndEditor_OnKeyPress(long int pressedKey, bool CTRL, bool SHIFT, bool ALT);

	// Called for ALL button clicks in the game.
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool Global_OnButtonClick(unsigned long int objAddress);

	// Returns true if custom map generation has been executed. False if we want to let original game code apply.
	bool ScenarioEditor_callMyGenerateMapIfRelevant();

	// Manage right button release action on selected units for given player
	// Returns true if a red cross sign should be displayed (a relevant action occurred)
	bool ApplyRightClickReleaseOnSelectedUnits(AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *UIGameZone,
		AOE_STRUCTURES::STRUCT_PLAYER *player, long int mousePosX, long int mousePosY);

};


	// Global unique object
	extern CustomRORMainInterface crMainInterface;
}
