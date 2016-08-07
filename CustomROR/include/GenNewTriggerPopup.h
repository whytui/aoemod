#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
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
private:
	AOE_STRUCTURES::STRUCT_UI_COMBOBOX *cbxEventType;
	AOE_STRUCTURES::STRUCT_UI_COMBOBOX *cbxActionType;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtTriggerText;
};
