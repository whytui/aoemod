#pragma once
#include <assert.h>
#include <string.h>
#include <AOE_structures.h>
#include <mystrings.h>
#include "CustomPopupBase.h"
#include "crCommon.h"
#include "crCommands.h"
#include "crLocalization.h"
#include "AOE_const_language.h"



class SimpleEditTextPopup : public CustomPopupBase {
public:
	SimpleEditTextPopup();
	void _ResetPointers() override;
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void AddPopupContent(const char *title, const char *initialValue, long int maxLength, char *outputBuffer, bool readOnly);
	void OnBeforeClose(bool isCancel) override;
	bool isForTriggers;
private:
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtText;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblTitle;
	long int maxLength;
	char *bufferToWrite;
};

