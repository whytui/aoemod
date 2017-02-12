#pragma once
#include <AOE_struct_game_settings.h>
#include <UI\AOE_struct_ui_in_game_menu.h>
#include <UI\AOE_struct_ui_diplomacy_dialog.h>
#include <UI\AOE_struct_ui_scenario_editor_menu.h>
#include <UI\AOE_struct_ui_mp_setup_screen.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_units.h>
#include "CustomRORInfo.h"
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

namespace CUSTOMROR {
;

class CustomPopupSystem {
public:
	CustomPopupSystem();
	~CustomPopupSystem();

	CustomPopupBase *currentCustomPopup;

private:
	// Returns NULL if failed (you can't create a custom popup object if previous one is still opened).
	// If result is non-NULL, you can call [result]->OpenPopup() to actually open the popup.
	template<class popupType> popupType *CreateCustomPopupObject() {
		if (CUSTOMROR::crInfo.HasOpenedCustomGamePopup()) {
			return NULL;
		}
		if (this->currentCustomPopup) {
			// If previous popup object still exists but is marked as "closed", we can delete it.
			if (this->currentCustomPopup->IsClosed()) {
				delete this->currentCustomPopup;
				this->currentCustomPopup = NULL;
			} else {
				return NULL;
			}
		}
		this->currentCustomPopup = (CustomPopupBase*) new popupType();
		return (popupType*)this->currentCustomPopup;
	}

public:

	// Ensure exiting game won't crash due to UI pointer issues related to custom popup (occurs in scenario editor)
	// Returns true if we want NOT the game to exit
	bool FixGamePopupIssuesBeforeGameClose();


	// Please use this API to close a custom game popup. Do not call crInfo's method directly.
	// set isCancel to true to indicate we don't want to save/apply changes.
	void CloseCustomGamePopup(bool isCancel = false);


	// Please use this API to open a custom game popup. Do not call technical method directly.
	// Returns NULL if failed.
	// You can get AOE popup object with xxx->GetAOEPopupObject()
	template<class popupType> popupType *OpenCustomGamePopup(long int hSize, long int vSize, bool withCancelBtn) {
		popupType *result = CUSTOMROR::customPopupSystem.CreateCustomPopupObject<popupType>();
		if (result != NULL) {
			if (!CUSTOMROR::customPopupSystem.currentCustomPopup) { return result; } // Just a security
			if (CUSTOMROR::customPopupSystem.currentCustomPopup->OpenPopup(hSize, vSize, withCancelBtn) == NULL) {
				// Open popup failed. Don't return an invalid/incomplete object: free it and return NULL.
				delete result;
				result = NULL;
			}
		}
		return result;
	}

	// Use this to properly free custom button from options menu. This should (always) be called when ingame menu is closed.
	void FreeInGameCustomOptionsButton();


	// Create in-game customROR options screen. Returns false if failed and if return address must be changed.
	bool CreateGameCustomRorOptionsPopup(AOE_STRUCTURES::STRUCT_ANY_UI *previousPopup);

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnCustomButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender);


	// Triggered on a "onclick" on a UI component at provided address (objAddress)
	// This just detects if the component is the popup's OK or Cancel button, and handles popup closing event if so.
	// Returns true if the popup has been closed (and the event processed)
	bool OnButtonClickClosePopupIfNecessary(unsigned long int objAddress);


	/* ----- Open various specialized popups ----- */

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

	// Open a popup with CustomROR (debug) messages
	bool OpenTraceMessagePopup();

	// Open the relevant "view/edit unit" popup for currently selected unit.
	// Returns true if successful.
	bool OpenInGameUnitPropertiesPopup();

	// Open the relevant "view/edit unit" popup for provided unit.
	// Returns true if successful.
	bool OpenInGameUnitPropertiesPopup(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

};


extern CustomPopupSystem customPopupSystem;

}
