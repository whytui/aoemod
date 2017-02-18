#pragma once
#include <assert.h>
#include <string.h>
#include <mystrings.h>
#include "CustomPopupBase.h"
#include "RockNRorCommon.h"
#include "RockNRorLocalization.h"
#include "AOE_const_language.h"
#include "CustomPopupSystem.h"


class EditTerrainPopup : public CustomPopupBase {
public:
	EditTerrainPopup();
	void _ResetPointers() override;
	void _AddPopupContent() override;
	void OnBeforeClose(bool isCancel) override;
	bool isForTriggers;

	// Opens the custom "edit terrain" popup in editor
	// Returns true if OK.
	static bool OpenCustomTerrainEditPopup();

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

