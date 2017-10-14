#include "../include/InputBox.h"


namespace ROCKNROR {
namespace UI {
;


// Reset various pointers for this class level (to override)
void InputBox::ResetClassPointers() {
	__super::ResetClassPointers();
	this->btnOK = NULL;
	this->btnCancel = NULL;
	this->edtInput = NULL;
	this->lblTitle = NULL;
	this->edtDescription = NULL;
}


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void InputBox::CreateScreenComponents() {
	if (maxLength < 1) {
		maxLength = 0x8000 - 2;
	}
	int labelSizeX = strlen(this->title.c_str()) * 12;
	int labelPosX = ((this->GetScreenSizeX() - labelSizeX) / 2) + 1;
	if (labelPosX < 1) { labelPosX = 1; }
	int textSizeY = this->GetScreenSizeY() - (24 * 2) - 56;
	if (textSizeY < 1) { textSizeY = 10; }
	this->AddLabel(&this->lblTitle, this->title.c_str(), labelPosX, 10, labelSizeX, 28, AOE_FONTS::AOE_FONT_BIG_LABEL);
	if (!this->description.empty()) {
		this->AddTextBox(&this->edtDescription, this->description.c_str(), 10000, 20, 40, this->GetScreenSizeX() - 40, textSizeY, true, true, false);
	}
	this->AddTextBox(&this->edtInput, this->initialInputValue.c_str(), maxLength, 60, textSizeY + 44, this->GetScreenSizeX() - 120, 20, readOnly, false, false);

	// OK/cancel buttons
	this->AddButton(&this->btnOK, localizationHandler.GetTranslation(LANG_ID_OK, "OK"), this->GetLeftCenteredPositionX(172),
		this->GetScreenSizeY() - 30, 80, 22, 0);
	this->AddButton(&this->btnCancel, localizationHandler.GetTranslation(LANG_ID_CANCEL, "Cancel"), this->GetLeftCenteredPositionX(172) + 86,
		this->GetScreenSizeY() - 30, 80, 22, 0);
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool InputBox::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if ((sender == this->btnOK) || (sender == this->btnCancel)) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool InputBox::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}



}
}

