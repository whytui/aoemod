#include "../include/EditTerrainPopup.h"


namespace ROCKNROR {
namespace UI {
;


// Reset various pointers for this class level (to override)
void EditTerrainPopup::ResetClassPointers() {
	__super::ResetClassPointers();
	this->btnOK = NULL;
	this->btnCancel = NULL;
	this->edtMaxPosX = NULL;
	this->edtMaxPosY = NULL;
	this->edtMinPosX = NULL;
	this->edtMinPosY = NULL;
	this->edtElevation = NULL;
	this->edtTerrainId = NULL;
	this->chkSoften = NULL;
}


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void EditTerrainPopup::CreateScreenComponents() {
	this->mapSizeX = -1;
	this->mapSizeY = -1;
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (global && global->IsCheckSumValid() && global->gameMapInfo && global->gameMapInfo->IsCheckSumValid()) {
		this->mapSizeX = global->gameMapInfo->mapArraySizeX;
		this->mapSizeY = global->gameMapInfo->mapArraySizeY;
	}
	char bufText[60];
	sprintf_s(bufText, "Map Size X,Y = %ld, %ld", this->mapSizeX, this->mapSizeY);

	// Popup is open. Add components
	long int btnhPos = (this->GetScreenSizeX() - 130) / 2;
	AOE_STRUCTURES::STRUCT_UI_LABEL *unused;
	long int currentVPos = 5;
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_IDTERRAIN_EDIT, "Edit terrain"), btnhPos, currentVPos, 180, 20, AOE_FONT_BIG_LABEL);
	currentVPos += 35;
	long int VPosForInputs = currentVPos;
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_POSX_MINMAX, "Pos X (min/max)"), 10, currentVPos, 120, 20, AOE_FONT_STANDARD_TEXT);
	currentVPos += 30;
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_POSY_MINMAX, "Pos Y (min/max)"), 10, currentVPos, 120, 20, AOE_FONT_STANDARD_TEXT);
	currentVPos += 30;
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_TERRAINID, "Terrain Id (0-31)"), 10, currentVPos, 120, 20, AOE_FONT_STANDARD_TEXT);
	currentVPos += 30;
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_ELEVATION, "Elevation (0-7)"), 10, currentVPos, 120, 20, AOE_FONT_STANDARD_TEXT);
	currentVPos += 30;
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_SOFTEN_TERRAIN_OPTION, "Soften modified terrain borders"), 10, currentVPos + 6, 350, 20, AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkSoften, 240, currentVPos, 30, 30);
	currentVPos += 34;
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_TIP_TERRAIN_LEAVE_EMPTY, "Tip: leave terrain/elevation empty to preserve current values."), 10, currentVPos, 450, 20, AOE_FONT_STANDARD_TEXT);
	currentVPos += 30;
	// Table of inputs
	this->AddTextBox(&this->edtMinPosX, "0", 3, 140, VPosForInputs, 80, 20, false, false, true);
	this->AddTextBox(&this->edtMinPosY, "0", 3, 140, VPosForInputs + 30, 80, 20, false, false, true);
	this->AddTextBox(&this->edtMaxPosX, "0", 3, 240, VPosForInputs, 80, 20, false, false, true);
	this->AddTextBox(&this->edtMaxPosY, "0", 3, 240, VPosForInputs + 30, 80, 20, false, false, true);
	this->AddTextBox(&this->edtTerrainId, "", 2, 140, VPosForInputs + 60, 80, 20, false, false, true);
	this->AddTextBox(&this->edtElevation, "", 1, 140, VPosForInputs + 90, 80, 20, false, false, true);
	this->AddLabel(&unused, bufText, 10, currentVPos, 350, 20, AOE_FONT_STANDARD_TEXT);
	currentVPos += 30;

	// OK Cancel buttons
	this->AddButton(&this->btnOK, localizationHandler.GetTranslation(LANG_ID_OK, "OK"), this->GetLeftCenteredPositionX(172),
		this->GetScreenSizeY() - 30, 80, 22, 0);
	this->AddButton(&this->btnCancel, localizationHandler.GetTranslation(LANG_ID_CANCEL, "Cancel"), this->GetLeftCenteredPositionX(172) + 86,
		this->GetScreenSizeY() - 30, 80, 22, 0);
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool EditTerrainPopup::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnCancel) {
		this->CloseScreen(false);
		return true;
	}
	if (sender == this->btnOK) {
		this->Validate();
		this->CloseScreen(false);
		return true;
	}
	return false;
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool EditTerrainPopup::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}


void EditTerrainPopup::Validate() {
	assert(this->edtElevation);
	if (!this->edtElevation) { return; }
	AOE_METHODS::UI_BASE::SetFocus(this->edtElevation->ptrParentObject, NULL);
	if ((this->mapSizeX < 0) || (this->mapSizeY < 0)) { return; }
	bool elevationEmpty = ((this->edtElevation->pTypedText == NULL) || (this->edtElevation->pTypedText[0] == '\0'));
	bool terrainEmpty = ((this->edtTerrainId->pTypedText == NULL) || (this->edtTerrainId->pTypedText[0] == '\0'));
	bool minXEmpty = ((this->edtMinPosX->pTypedText == NULL) || (this->edtMinPosX->pTypedText[0] == '\0'));
	bool minYEmpty = ((this->edtMinPosY->pTypedText == NULL) || (this->edtMinPosY->pTypedText[0] == '\0'));
	bool maxXEmpty = ((this->edtMaxPosX->pTypedText == NULL) || (this->edtMaxPosX->pTypedText[0] == '\0'));
	bool maxYEmpty = ((this->edtMaxPosY->pTypedText == NULL) || (this->edtMaxPosY->pTypedText[0] == '\0'));
	if (terrainEmpty && elevationEmpty) { return; } // Only 1 of {terrain, elevation} is required
	if (minXEmpty || maxXEmpty || minYEmpty || maxYEmpty) { return; } // coordinates are all requires
	long int minX, minY, maxX, maxY;
	minX = atoi(this->edtMinPosX->pTypedText);
	minY = atoi(this->edtMinPosY->pTypedText);
	maxX = atoi(this->edtMaxPosX->pTypedText);
	maxY = atoi(this->edtMaxPosY->pTypedText);
	char terrainId = atoi(this->edtTerrainId->pTypedText);
	char elevation = atoi(this->edtElevation->pTypedText);
	if ((minX > maxX) || (minY > maxY) || (maxX > this->mapSizeX) || (maxY > this->mapSizeY)) {
		return;
	}
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert((global != NULL) && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }
	assert((global->gameMapInfo != NULL) && (global->gameMapInfo->IsCheckSumValid()));
	if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	assert(mapInfo->mapArraySizeX >= this->mapSizeX);
	assert(mapInfo->mapArraySizeY >= this->mapSizeY);
	if ((mapInfo->mapArraySizeX < this->mapSizeX) || (mapInfo->mapArraySizeY < this->mapSizeY)) { return; }

	// TODO: move treatments into this->crCommand
	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO *tile = mapInfo->GetTileInfo(x, y);
			if (tile) {
				if (!elevationEmpty) { tile->terrainData.SetAltitude(elevation); } // checks elevation is 0-7
				if (!terrainEmpty) { tile->terrainData.SetTerrainId(terrainId); } // checks terrainId is 0-31
			}
		}
	}
	if (this->chkSoften->IsChecked()) {
		AOE_METHODS::SoftenAltitudeDifferences(mapInfo, minX, minY, maxX, maxY);
		AOE_METHODS::SoftenTerrainDifferences(mapInfo, minX, minY, maxX, maxY);
	}
}


}
}
