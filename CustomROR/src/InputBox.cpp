#include "../include/InputBox.h"


InputBox::InputBox() {
	this->_ResetPointers();
}


void InputBox::_ResetPointers() {
	__super::_ResetPointers();
	this->edtInput = NULL;
	this->lblTitle = NULL;
	this->edtDescription = NULL;
}

void InputBox::OnBeforeClose(bool isCancel) {}


// This class needs parameters to create content. Call this after calling OpenPopup().
void InputBox::AddPopupContent(const char *title, const char *desc, const char *initialInputValue, long int maxLength, bool readOnly) {
	this->readOnly = readOnly;
	if (maxLength < 1) {
		maxLength = 0x8000 - 2;
	}
	this->maxLength = maxLength;
	int labelSizeX = strlen(title) * 12;
	int labelPosX = ((this->hSize - labelSizeX) / 2) + 1;
	if (labelPosX < 1) { labelPosX = 1; }
	int textSizeY = this->vSize - (24*2) - 56;
	if (textSizeY < 1) { textSizeY = 10; }
	this->AddLabel(popup, &this->lblTitle, title, labelPosX, 10, labelSizeX, 28, AOE_FONTS::AOE_FONT_BIG_LABEL);
	if (*desc != 0) {
		this->AddTextBox(popup, &this->edtDescription, desc, 10000, 20, 40, this->hSize - 40, textSizeY, true, true, false);
	}
	this->AddTextBox(popup, &this->edtInput, initialInputValue, maxLength, 60, textSizeY + 44, this->hSize - 120, 20, readOnly, true, false);
}

