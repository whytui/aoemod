#pragma once
#include <UI_components\AOE_struct_ui_textbox.h>
#include "../include/SimpleEditText.h"




// Opens the custom "simple edit text" popup in editor
// outputBuffer is a pointer where the typed text will be copied when the popup is closed. Make sure its allocated size is >=maxLength
// Returns true if OK.
bool SimpleEditTextPopup::OpenCustomTextEditPopup(const char *title, char *initialValue, long int sizeX, long int sizeY,
	long int maxLength, char *outputBuffer, bool readOnly, bool withCancelBtn) {
	SimpleEditTextPopup *p = CUSTOMROR::customPopupSystem.OpenCustomGamePopup<SimpleEditTextPopup>(sizeX, sizeY, withCancelBtn);
	if (p) {
		p->AddPopupContent(title, initialValue, maxLength, outputBuffer, readOnly);
	}
	return (p != NULL);
}


// Open a popup with CustomROR (debug) messages
bool SimpleEditTextPopup::OpenTraceMessagePopup() {
	bool reverseOrder = CUSTOMROR::crInfo.configInfo.showLogsInReverseOrder;
	// Double spaces are intended (to compensate weird-looking font)
	char *title = "Show  debug  messages";
	if (reverseOrder) {
		title = "Show  debug  messages (reverse order)";
	}
	return SimpleEditTextPopup::OpenCustomTextEditPopup(title, (char*)traceMessageHandler.GetAllMessages(reverseOrder).c_str(),
		780, 590,
		0, NULL, false /* NOT read only so that scrollbar is active*/, false);
}



SimpleEditTextPopup::SimpleEditTextPopup() {
	this->_ResetPointers();
}


void SimpleEditTextPopup::_ResetPointers() {
	__super::_ResetPointers();
	this->edtText = NULL;
	this->lblTitle = NULL;
	this->bufferToWrite = NULL;
}


// This class needs parameters to create content. Call this after calling OpenPopup().
void SimpleEditTextPopup::AddPopupContent(const char *title, const char *initialValue, long int maxLength, char *outputBuffer, bool readOnly) {
	this->bufferToWrite = outputBuffer;
	if (maxLength < 1) {
		maxLength =  AOE_STRUCTURES::STRUCT_UI_TEXTBOX::GetMaxSupportedBufferSize();
	}
	this->maxLength = maxLength;
	int labelSizeX = strlen(title) * 12;
	int labelPosX = ((this->hSize - labelSizeX) / 2) + 1;
	if (labelPosX < 1) { labelPosX = 1; }
	this->AddLabel(popup, &this->lblTitle, title, labelPosX, 10, labelSizeX, 24, AOE_FONTS::AOE_FONT_BIG_LABEL);
	this->AddTextBox(popup, &this->edtText, initialValue, maxLength, 10, 30, this->hSize - 20, this->vSize - 80, readOnly, true, false);
}


void SimpleEditTextPopup::OnBeforeClose(bool isCancel) {
	if (!isCancel && this->edtText) {
		AOE_METHODS::UI_BASE::SetFocus(this->edtText->ptrParentObject, NULL); // "validate" typed value.
		// Write in provided buffer, if not NULL.
		if ((this->maxLength > 0) && (this->bufferToWrite != NULL)) {
			strcpy_s(this->bufferToWrite, this->maxLength - 1, this->edtText->pTypedText);
		}
	}
}

