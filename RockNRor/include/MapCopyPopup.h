#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "AOE_map.h"
#include "RockNRorCommon.h"
#include "RockNRorCommand.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "RnrScreenBase.h"



namespace ROCKNROR {
namespace UI {
;


class MapCopyPopup : public RnrScreenBase {
public:
	MapCopyPopup() : RnrScreenBase("map copy popup") {
		this->SetWindowed(100, 100, 500, 450); // will always work (default values)
		this->SetCenteredForSize(600, 450); // May fail if game settings can't be retrieved
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::FullBlackTheme);
		this->ResetClassPointers();
		float x, y;
		GetGamePositionUnderMouse(&x, &y);
		this->initialPosX = (long int)x;
		this->initialPosY = (long int)y;
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
	long int initialPosX, initialPosY; // Coordinates under mouse when popup was opened
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCopyZone;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCopyAllMap;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnPaste;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtCopyFromX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtCopyFromY;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtCopyToX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtCopyToY;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtPasteToX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtPasteToY;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkIncludeUnits;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtCurrentBufferSize;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtResultMessage;

	std::string GetBufferSizeText();
};


}
}

