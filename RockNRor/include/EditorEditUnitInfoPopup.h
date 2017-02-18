#pragma once
#include <assert.h>
#include <string.h>
#include <set>
#include <mystrings.h>
#include <UI_components\AOE_struct_ui_combobox.h>
#include "CustomPopupBase.h"
#include "RockNRorCommon.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "CustomPopupSystem.h"
#include "unitHandling.h"


class EditorEditUnitInfoPopup : public CustomPopupBase {
public:
	EditorEditUnitInfoPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) override;
	void OnBeforeClose(bool isCancel) override;
	void OnAfterClose(bool isCancel) override;

	// Opens the custom "edit unit" in editor
	// Returns true if OK.
	static bool OpenCustomEditorEditUnitPopup();

private:
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit;
	std::set<AOE_STRUCTURES::STRUCT_UNIT_BASE *>allSelectedUnits;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtStatus;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lbl_s0;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lbl_s2;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lbl_s4;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkbox_s0;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkbox_s2;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkbox_s4;
	int initialOwner; // Player ID that owns the unit when popup is opened
	AOE_STRUCTURES::STRUCT_UI_COMBOBOX *cbxPlayerOwner;
};

