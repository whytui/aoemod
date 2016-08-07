#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "CustomPopupBase.h"
#include "SimpleEditText.h"
#include "crCommon.h"
#include "crCommands.h"
#include "crLocalization.h"
#include "AOE_const_language.h"


class InGameCustomRorOptionsPopup : public CustomPopupBase {
public:
	InGameCustomRorOptionsPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) override;
	void OnBeforeClose(bool isCancel) override;
	void OnAfterClose(bool isCancel) override;
	// Specific method to use instead of OpenPopup (special treatments for ingame customROR menu).
	// previousPopup should be game menu popup object.
	AOE_STRUCTURES::STRUCT_ANY_UI * CloseMenuAndOpenPopup(AOE_STRUCTURES::STRUCT_ANY_UI *previousPopup);
	// Popup content objects
	AOE_STRUCTURES::STRUCT_UI_BUTTON *customOptionButtonVar; // "OK" button of customROR's options popup. NULL means popup does not exist. See HasOpenedCustomGamePopup.
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *customOptionHumanPenaltyTextVar;
	AOE_STRUCTURES::STRUCT_UI_LABEL *customOptionHumanPenaltyLabelVar;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *customOptionGameSpeedTextVar;
	AOE_STRUCTURES::STRUCT_UI_LABEL *customOptionGameSpeedLabelVar;
	AOE_STRUCTURES::STRUCT_UI_LABEL *customOptionHeaderLabelVar;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *customOptionFreeTextVar;
	AOE_STRUCTURES::STRUCT_UI_LABEL *customOptionFreeTextLabelVar;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *customOptionFreeTextAnswerVar;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnTechTreeInfo;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkAutoRebuildFarms;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoRebuildFarms;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtAutoRebuildFarmsMaxFood;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoRebuildFarmsMaxFood;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtAutoRebuildFarmsMinWood;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoRebuildFarmsMinWood;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtAutoRebuildFarmsMaxFarms;
	AOE_STRUCTURES::STRUCT_UI_LABEL *lblAutoRebuildFarmsMaxFarms;

private:
	bool openTechTreeInfo;
};

