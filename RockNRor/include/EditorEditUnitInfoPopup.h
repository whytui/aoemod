#pragma once
#include <assert.h>
#include <string.h>
#include <set>
#include <mystrings.h>
#include <UI_components\AOE_struct_ui_combobox.h>
#include "RockNRorCommon.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "unitHandling.h"
#include "RnrScreenBase.h"


namespace ROCKNROR {
namespace UI {
;

class EditorEditUnitInfoPopup : public RnrScreenBase {
public:
	EditorEditUnitInfoPopup() : RnrScreenBase("editor unit info") {
		this->SetWindowed(100, 100, 500, 400); // will always work (default values)
		this->SetCenteredForSize(500, 400); // May fail if game settings can't be retrieved
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::ScenarioEditorTheme);
		this->ResetClassPointers();
	}

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(STRUCT_UI_BUTTON *sender) override;

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) override;

protected:
	// Reset various pointers for this class level (to override)
	void ResetClassPointers() override;

	// Create screen content: buttons, etc. Called by CreateScreen(...) method.
	void CreateScreenComponents() override;

private:
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit;
	std::set<AOE_STRUCTURES::STRUCT_UNIT_BASE *>allSelectedUnits;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtStatus;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lbl_s0;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lbl_s2;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lbl_s4;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkbox_s0;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkbox_s2;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkbox_s4;
	int initialOwner; // Player ID that owns the unit when popup is opened
	AOE_STRUCTURES::STRUCT_UI_COMBOBOX *cbxPlayerOwner;

	// Save changes
	void Validate();
	void RestoreCursor();
};


}
}

