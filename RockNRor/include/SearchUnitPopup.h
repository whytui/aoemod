#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include <AOE_const_drs.h>
#include "interface.h"
#include "RnrScreenBase.h"
#include "SimpleEditText.h"
#include "InputBox.h"
#include "EditTerrainPopup.h"
#include "RockNRorCommon.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"



namespace ROCKNROR {
namespace UI {
;

class SearchUnitPopup : public RnrScreenBase {
public:
	SearchUnitPopup() : RnrScreenBase("search unit") {
		this->SetWindowed(100, 100, 600, 500); // will always work (default values)
		this->SetCenteredForSize(600, 500); // May fail if game settings can't be retrieved
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

	// Execute the unit search
	void DoSearch();

private:
	long int lastResultUnitId;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *foundUnit;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnDoSearch;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnFindNext;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblTitle;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtUnitId;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtDATId;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtPosX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtPosY;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtPosRadius;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtUnitClassId;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtPlayerId;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtResultInfo;

	AOE_STRUCTURES::STRUCT_UNIT_BASE *FindUnit(long int playerId, long int unitId, long int unitDATID, long int unitClassId,
		long int posX, long int posY, long int posRadius, long int startSearch);
	void HandleUnitFound(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);
};


}
}

