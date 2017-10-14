#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "RockNRorCommon.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "RnrScreenBase.h"



namespace ROCKNROR {
namespace UI {
;

class EditTerrainPopup : public RnrScreenBase {
public:
	EditTerrainPopup() : RnrScreenBase("edit terrain") {
		this->SetWindowed(100, 100, 500, 300); // will always work (default values)
		this->SetCenteredForSize(500, 300); // May fail if game settings can't be retrieved
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::FullBlackTheme);
		this->ResetClassPointers();
		this->elevation = 0;
		this->terrainId = 0;
		this->mapSizeX = -1;
		this->mapSizeY = -1;
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
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCancel;
	long int mapSizeX, mapSizeY;
	char terrainId;
	char elevation;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtMinPosX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtMinPosY;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtMaxPosX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtMaxPosY;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtTerrainId;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtElevation;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkSoften;

	void Validate();
};


}
}


