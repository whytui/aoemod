#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "RockNRorCommon.h"
#include "RockNRorCommand.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "RnrScreenBase.h"


namespace ROCKNROR {
namespace UI {
;


class EditMapSizeXYPopup : public RnrScreenBase {
public:
	EditMapSizeXYPopup() : RnrScreenBase("edit map sizexy") {
		this->SetWindowed(100, 100, 320, 130); // will always work (default values)
		this->SetCenteredForSize(320, 130); // May fail if game settings can't be retrieved
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::InGameOptionsTheme);
		this->sizeX = 0;
		this->sizeY = 0;
	}

	// Create screen content: buttons, etc. Called by CreateScreen(...) method.
	void CreateScreenComponents() override;

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	virtual bool OnButtonClick(STRUCT_ANY_UI *sender) override;

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) override;

protected:
	// Reset various pointers for this class level (to override)
	virtual void ResetClassPointers() override;

private:
	long int sizeX;
	long int sizeY;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtSizeX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtSizeY;

	void Validate();
};

}
}

