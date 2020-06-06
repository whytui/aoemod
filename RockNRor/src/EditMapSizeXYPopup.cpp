#include "../include/EditMapSizeXYPopup.h"


namespace ROCKNROR {
namespace UI {
;


// Reset various pointers for this class level (to override)
void EditMapSizeXYPopup::ResetClassPointers() {
	__super::ResetClassPointers();
	this->btnOK = NULL;
	this->edtSizeX = NULL;
	this->edtSizeY = NULL;
}


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void EditMapSizeXYPopup::CreateScreenComponents() {
	this->sizeX = 0;
	this->sizeY = 0;

	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	// By default, try to reuse existing map dimensions.
	if (global && global->IsCheckSumValid() && global->gameMapInfo && global->gameMapInfo->IsCheckSumValid()) {
		this->sizeX = global->gameMapInfo->mapArraySizeX;
		this->sizeY = global->gameMapInfo->mapArraySizeY;
	}
	if (this->sizeX >= AOE_MAX_ALLOWED_MAP_SIZE) { this->sizeX = AOE_MAX_ALLOWED_MAP_SIZE - 1; }
	if (this->sizeY >= AOE_MAX_ALLOWED_MAP_SIZE) { this->sizeY = AOE_MAX_ALLOWED_MAP_SIZE - 1; }

	// Popup is open. Add components
	long int btnhPos = (this->GetScreenSizeX() - 0xAC) / 2;
	AOE_STRUCTURES::STRUCT_UI_LABEL *unused;
	char bufX[6];
	char bufY[6];
	sprintf_s(bufX, "%ld", this->sizeX);
	sprintf_s(bufY, "%ld", this->sizeY);
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_TYPE_X_Y_MAP_SIZE, "Please type X and Y map size"), 30, 5, 280, 20, AOE_FONT_BIG_LABEL);
	int maxTextLength = 3;
	if (AOE_MAX_ALLOWED_MAP_SIZE >= 1000) { maxTextLength = 4; }
	this->AddTextBox(&this->edtSizeX, bufX, maxTextLength, 120, 40, 80, 20, false, false, true);
	this->AddTextBox(&this->edtSizeY, bufY, maxTextLength, 120, 60, 80, 20, false, false, true);

	// OK button
	this->AddButton(&this->btnOK, localizationHandler.GetTranslation(LANG_ID_OK, "OK"), this->GetLeftCenteredPositionX(80),
		this->GetScreenSizeY() - 30, 80, 22, 0);
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool EditMapSizeXYPopup::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnOK) {
		this->Validate();
		this->CloseScreen(false);
		return true;
	}
	return false;
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool EditMapSizeXYPopup::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}


void EditMapSizeXYPopup::Validate() {
	char *textX = AOE_METHODS::UI_BASE::GetEditText(this->edtSizeX);
	char *textY = AOE_METHODS::UI_BASE::GetEditText(this->edtSizeY);
	long int x = atoi(textX);
	long int y = atoi(textY);
	if ((x <= 0) || (y <= 0) || (x >= AOE_MAX_ALLOWED_MAP_SIZE) || (y >= AOE_MAX_ALLOWED_MAP_SIZE)) { return; }
	// TODO: move this method in a dedicated file (class) !
	ROCKNROR::crCommand.ScenarioEditor_customGenerateMap(x, y);
}



}
}

