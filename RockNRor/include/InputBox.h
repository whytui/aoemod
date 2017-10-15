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

class InputBox : public RnrScreenBase {
public:
	InputBox(const char *title, const char *desc, const char *initialInputValue, long int maxLength, bool readOnly) : RnrScreenBase("input box") {
		this->SetWindowed(100, 100, 450, 200); // will always work (default values)
		this->SetCenteredForSize(450, 200); // May fail if game settings can't be retrieved
		this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::FullBlackTheme);
		this->ResetClassPointers();
		this->readOnly = readOnly;
		this->maxLength = maxLength;
		if (initialInputValue) { this->initialInputValue = initialInputValue; }
		this->title = title;
		this->description = desc;
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

protected:
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnOK;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCancel;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtInput;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblTitle;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtDescription;
	long int maxLength;
	bool readOnly;
	std::string initialInputValue;
	std::string title;
	std::string description;
};



// Generic input box for integers. For float, please use InputBox_float instead (and specify maxLength).
// Invalid values are replaced by 0.
template<typename inttype> class InputBox_int : public InputBox {
public:
	InputBox_int(const char *title, const char *text, inttype initialValue, inttype minValue, inttype maxValue, inttype *outputBuffer, bool readOnly)
		: InputBox(title, text, 0, 0, readOnly)
	{
		this->bufferToWrite = NULL;
		this->minValue = minValue;
		this->maxValue = maxValue;
		this->forcedMaxLength = 0;
		this->bufferToWrite = outputBuffer;
		std::string sFoo;
		if (this->forcedMaxLength > 0) { this->maxLength = this->forcedMaxLength; } else {
			// Calculate max length
			sFoo = to_string(minValue);
			this->maxLength = sFoo.size();
			sFoo = to_string(maxValue);
			long int otherSize = sFoo.size();
			if (otherSize > this->maxLength) { this->maxLength = otherSize; }
		}
		this->initialInputValue = std::to_string(initialValue);
	}


	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(STRUCT_UI_BUTTON *sender) override {
		if (sender == this->btnCancel) {
			this->CloseScreen(false);
			return true;
		}
		if (sender == this->btnOK) {
			if (this->ValidateInput()) {
				this->CloseScreen(false);
				return true;
			}
		}
		return false;
	}

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) override {
		if ((keyCode == VK_RETURN) && (uiObj != this->btnCancel)) {
			if (this->ValidateInput()) {
				this->CloseScreen(false);
				return true;
			}
		}
		return __super::OnKeyDown(uiObj, keyCode, repeatCount, ALT, CTRL, SHIFT);
	}

	bool ValidateInput() {
		if (!this->edtInput || this->readOnly || !this->bufferToWrite) { return false; }
		AOE_METHODS::UI_BASE::SetFocus(this->edtInput->ptrParentObject, NULL); // "validate" typed value.
		inttype value = atoi(this->edtInput->pTypedText); // atoi returns 0 if invalid, we use that behaviour too
		if (value < this->minValue) {
			value = this->minValue;
		}
		if (value > this->maxValue) {
			value = this->maxValue;
		}
		*this->bufferToWrite = value;
		return true;
	}


protected:
	long int forcedMaxLength;
	inttype minValue, maxValue;
	inttype *bufferToWrite;
};


// A specific implementation for float (use atof).
bool InputBox_int<float>::ValidateInput() {
	if (!this->edtInput || this->readOnly || !this->bufferToWrite) { return false; }
	AOE_METHODS::UI_BASE::SetFocus(this->edtInput->ptrParentObject, NULL); // "validate" typed value.
	float value = (float)atof(this->edtInput->pTypedText); // atof returns 0 if invalid, we use that behaviour too
	if (value < this->minValue) {
		value = this->minValue;
	}
	if (value > this->maxValue) {
		value = this->maxValue;
	}
	*this->bufferToWrite = value;
	return true;
}


// Please use this derived class for float messagebox, and specify input text max length.
class InputBox_float : public InputBox_int<float> {
	InputBox_float(const char *title, const char *text, float initialValue, float minValue, float maxValue, float *outputBuffer, bool readOnly)
		: InputBox_int(title, text, initialValue, minValue, maxValue, outputBuffer, readOnly) {
		if (maxLength < 1) { maxLength = 1; }
		this->forcedMaxLength = maxLength;
	}
};



}
}

