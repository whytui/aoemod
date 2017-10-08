#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include <AOE_const_drs.h>
#include "CustomPopupBase.h"
#include "SimpleEditText.h"
#include "InputBox.h"
#include "EditTerrainPopup.h"
#include "RockNRorCommon.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "CustomPopupSystem.h"


enum SC_INFO_POPUP_TO_OPEN { PTO_NONE = 0, PTO_TRIGGER, PTO_AI, PTO_PER, PTO_TERRAIN_EDIT, PTO_VICTORY_CONDITION, PTO_COUNT };


namespace ROCKNROR {
namespace UI {
;

class EditorScenarioInfoPopup : public RnrScreenBase {
public:
	EditorScenarioInfoPopup() : RnrScreenBase("editor unit info") {
		this->SetWindowed(100, 100, 500, 400); // will always work (default values)
		this->SetCenteredForSize(500, 400); // May fail if game settings can't be retrieved
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::InGameOptionsTheme);
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_NONE;
		this->ResetClassPointers();
	}

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	virtual bool OnButtonClick(STRUCT_UI_BUTTON *sender) override;

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) override;

protected:
	// Reset various pointers for this class level (to override)
	virtual void ResetClassPointers() override;

	// Create screen content: buttons, etc. Called by CreateScreen(...) method.
	void CreateScreenComponents() override;

private:
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblTitle;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtPlayerId;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnTriggers;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnAI;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnPER;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnTerrainEdit;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnVictoryCondition;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAllowUnitOverlapping;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkDisableHillModeCheck;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkDisableTerrainRestrictions;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkLengthenCombatMode;
	SC_INFO_POPUP_TO_OPEN popupToOpen;
	int playerId;

};


}
}

