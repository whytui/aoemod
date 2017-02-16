#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "CustomPopupBase.h"
#include "crCommon.h"
#include "crCommands.h"
#include "crLocalization.h"
#include "AOE_const_language.h"



// A common base class to implement input boxes.
class InputBox : public CustomPopupBase {
public:
	InputBox();
	void _ResetPointers() override;
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void AddPopupContent(const char *title, const char *desc, const char *initialInputValue, long int maxLength, bool readOnly);
	void OnBeforeClose(bool isCancel) override;
protected:
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtInput;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblTitle;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtDescription;
	long int maxLength;
	bool readOnly;
};


// Generic input box for integers. For float, please use InputBox_float instead (and specify maxLength).
// Invalid values are replaced by 0.
template<typename inttype> class InputBox_int : public InputBox {
public:
	InputBox_int() { this->_ResetPointers(); }
	void _ResetPointers() override {
		this->bufferToWrite = NULL;
		this->minValue = 0;
		this->maxValue = 0;
		this->forcedMaxLength = 0;
	}
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void AddPopupContent(const char *title, const char *text, inttype initialValue, inttype minValue, inttype maxValue, inttype *outputBuffer, bool readOnly) {
		this->bufferToWrite = outputBuffer;
		long int maxLength = 0;
		std::string sFoo;
		if (this->forcedMaxLength > 0) { maxLength = this->forcedMaxLength; } else {
			// Calculate max length
			sFoo = to_string(minValue);
			maxLength = sFoo.size();
			sFoo = to_string(maxValue);
			long int otherSize = sFoo.size();
			if (otherSize > maxLength) { maxLength = otherSize; }
		}
		sFoo = to_string(initialValue);
		InputBox::AddPopupContent(title, text, sFoo.c_str(), maxLength, readOnly);
	}
	void OnBeforeClose(bool isCancel) override {
		if (isCancel || !this->edtInput || this->readOnly || !this->bufferToWrite) { return; }
		AOE_METHODS::UI_BASE::SetFocus(this->edtInput->ptrParentObject, NULL); // "validate" typed value.
		inttype value = atoi(this->edtInput->pTypedText); // atoi returns 0 if invalid, we use that behaviour too
		*this->bufferToWrite = value;
	}
protected:
	long int forcedMaxLength;
private:
	inttype minValue, maxValue;
	inttype *bufferToWrite;
};

// A specific implementation for float (use atof).
void InputBox_int<float>::OnBeforeClose(bool isCancel) {
	if (isCancel || !this->edtInput || this->readOnly || !this->bufferToWrite) { return; }
	AOE_METHODS::UI_BASE::SetFocus(this->edtInput->ptrParentObject, NULL); // "validate" typed value.
	float value = (float)atof(this->edtInput->pTypedText); // atof returns 0 if invalid, we use that behaviour too
	*this->bufferToWrite = value;
}

// Please use this derived class for float messagebox, and specify input text max length.
class InputBox_float : public InputBox_int<float> {
	void AddPopupContent(char *title, char *text, float initialValue, float minValue, float maxValue, long int maxLength, float *outputBuffer, bool readOnly) {
		if (maxLength < 1) { maxLength = 1; }
		this->forcedMaxLength = maxLength;
		InputBox_int::AddPopupContent(title, text, initialValue, minValue, maxValue, outputBuffer, readOnly);
	}
};

