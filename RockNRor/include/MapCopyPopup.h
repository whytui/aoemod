#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "CustomPopupBase.h"
#include "AOE_map.h"
#include "crCommon.h"
#include "crCommands.h"
#include "crLocalization.h"
#include "AOE_const_language.h"


class MapCopyPopup : public CustomPopupBase {
public:
	MapCopyPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;

	// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
	bool OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) override;
private:
	long int initialPosX, initialPosY; // Coordinates under mouse when popup was opened
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCopyZone;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnCopyAllMap;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *btnPaste;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtCopyFromX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtCopyFromY;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtCopyToX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtCopyToY;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtPasteToX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtPasteToY;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkIncludeUnits;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtCurrentBufferSize;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtResultMessage;

	std::string GetBufferSizeText();
};
