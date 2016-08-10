#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include <UI_components\AOE_struct_ui_button.h>
#include "CustomPopupBase.h"
#include "crCommon.h"
#include "crCommands.h"
#include "crLocalization.h"
#include "AOE_const_language.h"


class GenNewTriggerPopup : public CustomPopupBase {
public:
	GenNewTriggerPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	void OnBeforeClose(bool isCancel) override;
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) override;
private:
	AOE_STRUCTURES::STRUCT_UI_COMBOBOX *cbxEventType;
	AOE_STRUCTURES::STRUCT_UI_COMBOBOX *cbxActionType;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtTriggerText;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnGenerate;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkBeforeTrigger;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAfterTrigger;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkDiffLevelRestriction;
};
