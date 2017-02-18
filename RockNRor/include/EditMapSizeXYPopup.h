#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "CustomPopupBase.h"
#include "RockNRorCommon.h"
#include "RockNRorCommand.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "CustomPopupSystem.h"


class EditMapSizeXYPopup : public CustomPopupBase {
public:
	EditMapSizeXYPopup();
	void _ResetPointers() override;
	// This class needs parameters to create content. Call this after calling OpenPopup().
	void _AddPopupContent() override;
	void OnAfterClose(bool isCancel) override;
	bool isForTriggers;

	// Opens the custom "edit map size" popup in editor
	// Returns true if OK.
	static bool OpenEditMapSizePopup();

private:
	long int sizeX;
	long int sizeY;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtSizeX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtSizeY;
};

