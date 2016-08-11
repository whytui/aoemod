#pragma once
#include <UI_components\AOE_struct_ui_textbox.h>
#include "../include/SimpleEditText.h"


SimpleEditTextPopup::SimpleEditTextPopup() {
	this->_ResetPointers();
}


void SimpleEditTextPopup::_ResetPointers() {
	__super::_ResetPointers();
	this->isForTriggers = false;
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
		AOE_SetFocus(this->edtText->ptrParentObject, NULL); // "validate" typed value.
		if (this->maxLength > 0) {

			if (this->isForTriggers) {
				// Use dedicated API to update trigger information
				this->crCommand->WriteTriggersInGameData(this->edtText->pTypedText);
			} else {
				// Standard case: write in provided buffer, if not NULL.
				if (this->bufferToWrite != NULL) {
					strcpy_s(this->bufferToWrite, this->maxLength - 1,
						this->edtText->pTypedText);
				}
			}
		}
	}
}

