#include "../include/SimpleEditText.h"




namespace ROCKNROR {
namespace UI {
;


// Reset various pointers for this class level (to override)
void SimpleEditTextPopup::ResetClassPointers() {
	__super::ResetClassPointers();
	this->btnOK = NULL;
	this->btnCancel = NULL;
	this->edtText = NULL;
	this->lblTitle = NULL;
	this->bufferToWrite = NULL;
}


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void SimpleEditTextPopup::CreateScreenComponents() {
	int labelSizeX = strlen(this->title.c_str()) * 12;
	int labelPosX = ((this->GetScreenSizeX() - labelSizeX) / 2) + 1;
	if (labelPosX < 1) { labelPosX = 1; }
	this->AddLabel(&this->lblTitle, title.c_str(), labelPosX, 10, labelSizeX, 24, AOE_FONTS::AOE_FONT_BIG_LABEL);
	this->AddTextBox(&this->edtText, initialValue.c_str(), maxLength, 10, 30, this->GetScreenSizeX() - 20, this->GetScreenSizeY() - 80, readOnly, true, false);
	
	// OK/cancel buttons
	if (withCancelBtn) {
		this->AddButton(&this->btnOK, localizationHandler.GetTranslation(LANG_ID_OK, "OK"), this->GetLeftCenteredPositionX(172),
			this->GetScreenSizeY() - 30, 80, 22, 0);
		this->AddButton(&this->btnCancel, localizationHandler.GetTranslation(LANG_ID_CANCEL, "Cancel"), this->GetLeftCenteredPositionX(172) + 86,
			this->GetScreenSizeY() - 30, 80, 22, 0);
	} else {
		this->AddButton(&this->btnOK, localizationHandler.GetTranslation(CRLANG_ID_CLOSE, "Close"), this->GetLeftCenteredPositionX(80),
			this->GetScreenSizeY() - 30, 80, 22, 0);
	}
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool SimpleEditTextPopup::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnCancel) {
		this->CloseScreen(false);
		return true;
	}if (sender == this->btnOK) {
		if (this->edtText) {
			AOE_METHODS::UI_BASE::SetFocus(this->edtText->ptrParentObject, NULL); // "validate" typed value.
			// Write in provided buffer, if not NULL.
			if ((this->maxLength > 0) && (this->bufferToWrite != NULL)) {
				strcpy_s(this->bufferToWrite, this->maxLength - 1, this->edtText->pTypedText);
			}
		}
		this->CloseScreen(false);
		return true;
	}
	return false;
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool SimpleEditTextPopup::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}


// Opens the custom "simple edit text" popup in editor
// outputBuffer is a pointer where the typed text will be copied when the popup is closed. Make sure its allocated size is >=maxLength
// Returns true if OK.
bool SimpleEditTextPopup::OpenCustomTextEditPopup(const char *title, char *initialValue, long int sizeX, long int sizeY,
	long int maxLength, char *outputBuffer, bool readOnly, bool withCancelBtn) {
	SimpleEditTextPopup *p = new SimpleEditTextPopup(title, initialValue, maxLength, outputBuffer, readOnly, withCancelBtn);
	if ((sizeX > 0) && (sizeY > 0)) {
		p->SetCenteredForSize(sizeX, sizeY);
	} else {
		p->SetFullscreen();
		p->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::GameSettingsTheme);
	}
	return p->CreateScreen(GetCurrentScreen());
}


// Open a popup with RockNRor (debug) messages
bool SimpleEditTextPopup::OpenTraceMessagePopup() {
	bool reverseOrder = ROCKNROR::crInfo.configInfo.showLogsInReverseOrder;
	// Double spaces are intended (to compensate weird-looking font)
	char *title = "Show  debug  messages";
	if (reverseOrder) {
		title = "Show  debug  messages (reverse order)";
	}
	return SimpleEditTextPopup::OpenCustomTextEditPopup(title, (char*)traceMessageHandler.GetAllMessages(reverseOrder).c_str(),
		0, 0, // size=0 enables fullscreen
		0, NULL, false /* NOT read only so that scrollbar is active*/, false);
}


}
}


