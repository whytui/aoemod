#include "../include/EditMapSizeXYPopup.h"


EditMapSizeXYPopup::EditMapSizeXYPopup() {
	this->_ResetPointers();
	this->sizeX = 0;
	this->sizeY = 0;
}


void EditMapSizeXYPopup::_ResetPointers() {
	__super::_ResetPointers();
	this->edtSizeX = NULL;
	this->edtSizeY = NULL;
}


void EditMapSizeXYPopup::_AddPopupContent() {
	this->sizeX = 0;
	this->sizeY = 0;

	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	// By default, try to reuse existing map dimensions.
	if (global && global->IsCheckSumValid() && global->gameMapInfo && global->gameMapInfo->IsCheckSumValid()) {
		this->sizeX = global->gameMapInfo->mapArraySizeX;
		this->sizeY = global->gameMapInfo->mapArraySizeY;
	}
	if (this->sizeX > 255) { this->sizeX = 255; }
	if (this->sizeY > 255) { this->sizeY = 255; }

	// Popup is open. Add components
	long int btnhPos = (hSize - 0xAC) / 2;
	AOE_STRUCTURES::STRUCT_UI_LABEL *unused;
	char bufX[5];
	char bufY[5];
	sprintf_s(bufX, "%ld", this->sizeX);
	sprintf_s(bufY, "%ld", this->sizeY);
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_ID_TYPE_X_Y_MAP_SIZE, "Please type X and Y map size"), 30, 5, 280, 20, AOE_FONT_BIG_LABEL);
	this->AddTextBox(popup, &this->edtSizeX, bufX, 3, 120, 40, 80, 20, false, false, true);
	this->AddTextBox(popup, &this->edtSizeY, bufY, 3, 120, 60, 80, 20, false, false, true);
}


void EditMapSizeXYPopup::OnAfterClose(bool isCancel) {
	if (isCancel) { return; }
	char *textX = AOE_METHODS::AOE_GetEditText(this->edtSizeX);
	char *textY = AOE_METHODS::AOE_GetEditText(this->edtSizeY);
	long int x = atoi(textX);
	long int y = atoi(textY);
	if ((x <= 0) || (y <= 0) || (x > 255) || (y > 255)) { return; }
	CUSTOMROR::crCommand.ScenarioEditor_customGenerateMap(x, y);
}
