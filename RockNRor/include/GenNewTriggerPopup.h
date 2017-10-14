#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include <UI_components\AOE_struct_ui_button.h>
#include "RockNRorCommon.h"
#include "RockNRorCommand.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "RnrScreenBase.h"


namespace ROCKNROR {
namespace UI {
;

class GenNewTriggerPopup : public RnrScreenBase {
public:
	GenNewTriggerPopup() : RnrScreenBase("gen new trigger") {
		this->SetFullscreen();
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::LightOrangeTheme);
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
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK;
	AOE_STRUCTURES::STRUCT_UI_COMBOBOX *cbxEventType;
	AOE_STRUCTURES::STRUCT_UI_COMBOBOX *cbxActionType;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtSampleTriggerText;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtTriggerText;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnGenerate;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkBeforeTrigger;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAfterTrigger;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkDiffLevelRestriction;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnSaveTriggers;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnReloadTriggers;

	void LoadTriggerTextFromGameInfo();
	void SaveTriggerTextToGameInfo();
};


}
}

