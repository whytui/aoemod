#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include <UI_components\AOE_struct_ui_button.h>
#include <UI_components\AOE_struct_ui_label.h>
#include <UI_components\AOE_struct_ui_textbox.h>
#include "RockNRorCommon.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "RnrScreenBase.h"



namespace ROCKNROR {
namespace UI {
;

class SimpleEditTextPopup : public RnrScreenBase {
public:
	SimpleEditTextPopup(const char *title, const char *initialValue, long int maxLength, char *outputBuffer, bool readOnly, bool withCancelBtn) : RnrScreenBase("simple edit text") {
		this->SetWindowed(100, 100, 500, 300); // will always work (default values)
		this->SetCenteredForSize(500, 300); // May fail if game settings can't be retrieved
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::FullBlackTheme);
		this->ResetClassPointers();
		this->title = title;
		this->initialValue = initialValue;
		this->readOnly = readOnly;
		this->bufferToWrite = outputBuffer;
		this->maxLength = maxLength;
		this->withCancelBtn = withCancelBtn;
		if (this->maxLength < 1) {
			this->maxLength = AOE_STRUCTURES::STRUCT_UI_TEXTBOX::GetMaxSupportedBufferSize();
		}
	}

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(STRUCT_UI_BUTTON *sender) override;

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) override;

	// Opens the custom "simple edit text" popup in editor
	// When closed, the popup updates outputBuffer if it is provided (not NULL)
	// Returns true if OK.
	static bool OpenCustomTextEditPopup(const char *title, char *initialValue, long int sizeX, long int sizeY,
		long int outputBufferSize = 0, char *outputBuffer = NULL, bool readOnly = false, bool withCancelBtn = false);

	// Open a popup with RockNRor (debug) messages
	static bool OpenTraceMessagePopup();

protected:
	// Reset various pointers for this class level (to override)
	void ResetClassPointers() override;

	// Create screen content: buttons, etc. Called by CreateScreen(...) method.
	void CreateScreenComponents() override;

private:
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCancel;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtText;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblTitle;
	std::string title;
	std::string initialValue;
	bool readOnly;
	bool withCancelBtn;
	long int maxLength;
	char *bufferToWrite;
};

}
}

