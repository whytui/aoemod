#pragma once
#include <assert.h>
#include <string.h>
#include <ROR_structures.h>
#include <mystrings.h>
#include "CustomPopupBase.h"
#include "crCommon.h"
#include "crCommands.h"
#include "crLocalization.h"
#include "AOE_const_language.h"



class EditorEditUnitInfoPopup : public CustomPopupBase {
public:
	EditorEditUnitInfoPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) override;
	void OnBeforeClose(bool isCancel) override;
	void OnAfterClose(bool isCancel) override;
private:
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtStatus;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lbl_s0;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lbl_s2;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lbl_s4;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkbox_s0;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkbox_s2;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkbox_s4;
};

