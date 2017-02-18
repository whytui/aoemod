#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "CustomPopupBase.h"
#include "RockNRorCommon.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "CustomPopupSystem.h"


class SimpleEditTextPopup : public CustomPopupBase {
public:
	SimpleEditTextPopup();
	void _ResetPointers() override;
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void AddPopupContent(const char *title, const char *initialValue, long int maxLength, char *outputBuffer, bool readOnly);
	void OnBeforeClose(bool isCancel) override;

	// Opens the custom "simple edit text" popup in editor
	// When closed, the popup updates outputBuffer if it is provided (not NULL)
	// Returns true if OK.
	static bool OpenCustomTextEditPopup(const char *title, char *initialValue, long int sizeX, long int sizeY,
		long int outputBufferSize = 0, char *outputBuffer = NULL, bool readOnly = false, bool withCancelBtn = false);

	// Open a popup with RockNRor (debug) messages
	static bool OpenTraceMessagePopup();

private:
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtText;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblTitle;
	long int maxLength;
	char *bufferToWrite;
};

