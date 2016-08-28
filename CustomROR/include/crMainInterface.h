#include <AOE_struct_game_settings.h>
#include <UI\AOE_struct_ui_in_game_menu.h>
#include <UI\AOE_struct_ui_diplomacy_dialog.h>
#include <UI\AOE_struct_ui_scenario_editor_menu.h>
#include <UI\AOE_struct_ui_mp_setup_screen.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_units.h>
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

/* This class manages main interactions between ROR (via CustomRORInstance class) and customROR commands
 * Especially, UI events are managed here as our UI objects need to access CustomRORCommand object.
*/
class CustomRORMainInterface {
public:
	CustomRORMainInterface();
	CustomRORMainInterface(CustomRORCommand *crCommand);
	~CustomRORMainInterface();

	CustomRORCommand *crCommand;
	CustomPopupBase *currentCustomPopup;


	// Ensure exiting game won't crash due to UI pointer issues related to custom popup (occurs in scenario editor)
	// Returns true if we want NOT the game to exit
	bool FixGamePopupIssuesBeforeGameClose();

	// Manage key press in game screen and editor.
	// Returns true if event has been handled AND we don't want original code to try to handle it.
	bool GameAndEditor_OnKeyPress(long int pressedKey, bool CTRL, bool SHIFT, bool ALT);

	// Create in-game customROR options screen. Returns false if failed and if return address must be changed.
	bool CreateGameCustomRorOptionsPopup(AOE_STRUCTURES::STRUCT_ANY_UI *previousPopup);

private:
	// Returns NULL if failed (you can't create a custom popup object if previous one is still opened).
	// If result is non-NULL, you can call [result]->OpenPopup() to actually open the popup.
	template<class popupType> popupType *CreateCustomPopupObject();
public:
	// Please use this API to open a custom game popup. Do not call crInfo's method directly.
	// Returns NULL if failed.
	// You can get AOE popup object with xxx->GetAOEPopupObject()
	template<class popupType> popupType *OpenCustomGamePopup(long int hSize, long int vSize, bool withCancelBtn = false);

	// Please use this API to close a custom game popup. Do not call crInfo's method directly.
	// set isCancel to true to indicate we don't want to save/apply changes.
	void CloseCustomGamePopup(bool isCancel = false);

	// Called for ALL button clicks in the game.
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool Global_OnButtonClick(unsigned long int objAddress);

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnCustomButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender);


	// Opens the custom "edit unit" in editor
	// Returns true if OK.
	bool OpenCustomEditorEditUnitPopup();

	// Opens the custom "simple edit text" popup in editor
	// When closed, the popup updates outputBuffer if it is provided (not NULL)
	// Returns true if OK.
	bool OpenCustomTextEditPopup(const char *title, char *initialValue, long int sizeX, long int sizeY,
		long int outputBufferSize = 0, char *outputBuffer = NULL, bool readOnly = false, bool withCancelBtn = false);

	// Opens the custom "scenario info" popup in editor
	// Returns true if OK.
	bool OpenCustomEditorScenarioInfoPopup();

	// Opens the custom "edit map size" popup in editor
	// Returns true if OK.
	bool OpenEditMapSizePopup();

	// Opens the custom "edit terrain" popup in editor
	// Returns true if OK.
	bool OpenCustomTerrainEditPopup();

	// Returns true if custom map generation has been executed. False if we want to let original game code apply.
	bool ScenarioEditor_callMyGenerateMapIfRelevant();

	// Manage right button release action on selected units for given player
	// Returns true if a red cross sign should be displayed (a relevant action occurred)
	bool ApplyRightClickReleaseOnSelectedUnits(AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *UIGameMain,
		AOE_STRUCTURES::STRUCT_PLAYER *player, long int mousePosX, long int mousePosY);

	// Open a popup with CustomROR (debug) messages
	bool OpenTraceMessagePopup();

	// Open the relevant "view/edit unit" popup for currently selected unit.
	// Returns true if successful.
	bool OpenInGameUnitPropertiesPopup();
	// Open the relevant "view/edit unit" popup for provided unit.
	// Returns true if successful.
	bool OpenInGameUnitPropertiesPopup(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

	// Use this to properly free custom button from options menu. This should (always) be called when ingame menu is closed.
	void FreeInGameCustomOptionsButton();
};
