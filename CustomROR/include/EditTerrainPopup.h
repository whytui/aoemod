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


class EditTerrainPopup : public CustomPopupBase {
public:
	EditTerrainPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	void OnBeforeClose(bool isCancel) override;
	bool isForTriggers;
private:
	long int mapSizeX, mapSizeY;
	char terrainId;
	char elevation;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtMinPosX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtMinPosY;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtMaxPosX;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtMaxPosY;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtTerrainId;
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *edtElevation;
	AOE_STRUCTURES::STRUCT_UI_BUTTON *chkSoften;
};

