#include "../include/MapCopyPopup.h"


namespace ROCKNROR {
namespace UI {
;


// Reset various pointers for this class level (to override)
void MapCopyPopup::ResetClassPointers() {
	__super::ResetClassPointers();
	this->btnOK = NULL;
	this->btnCopyAllMap = NULL;
	this->btnCopyZone = NULL;
	this->btnPaste = NULL;
	this->edtCopyFromX = NULL;
	this->edtCopyFromY = NULL;
	this->edtCopyToX = NULL;
	this->edtCopyToY = NULL;
	this->edtPasteToX = NULL;
	this->edtPasteToY = NULL;
	this->chkIncludeUnits = NULL;
	this->edtCurrentBufferSize = NULL;
	this->edtResultMessage = NULL;
}


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void MapCopyPopup::CreateScreenComponents() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	bool isInEditor = settings && settings->IsCheckSumValid() && (settings->currentUIStatus == GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR);

	const long int btnSize = 0xAC;
	AOE_STRUCTURES::STRUCT_UI_LABEL *unused;
	long int currentVPos = 5;
	char bufX[20];
	char bufY[20];
	long int posX = this->initialPosX;
	long int posY = this->initialPosY;
	if (posX < 0) { posX = 0; }
	if (posY < 0) { posY = 0; }
	sprintf_s(bufX, "%ld", posX);
	sprintf_s(bufY, "%ld", posY);
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_COPY_PASTE_MAP_ZONES_TITLE, "Copy/Paste map zones"), 100, currentVPos, 300, 20, AOE_FONT_BIG_LABEL);
	currentVPos += 35;
	// Copy pos
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_COPY_FROM_MAP_LABEL, "Copy from (x/y of bottom-left corner)"), 10, currentVPos, 250, 20, AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(&this->edtCopyFromX, bufX, 3, 280, currentVPos, 80, 20, false, false, true);
	this->AddTextBox(&this->edtCopyFromY, bufY, 3, 380, currentVPos, 80, 20, false, false, true);
	currentVPos += 30;
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_COPY_TO_MAP_LABEL, "Copy to (x/y of top-right corner)"), 10, currentVPos, 250, 20, AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(&this->edtCopyToX, bufX, 3, 280, currentVPos, 80, 20, false, false, true);
	this->AddTextBox(&this->edtCopyToY, bufY, 3, 380, currentVPos, 80, 20, false, false, true);
	currentVPos += 30;
	// Paste pos
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_PASTE_TO_MAP_LABEL, "Paste to position (x/y of bottom-left corner)"), 10, currentVPos, 250, 20, AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(&this->edtPasteToX, bufX, 3, 280, currentVPos, 80, 20, false, false, true);
	this->AddTextBox(&this->edtPasteToY, bufY, 3, 380, currentVPos, 80, 20, false, false, true);
	currentVPos += 30;
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_ID_COPY_MAP_UNITS, "Copy units"), 10, currentVPos, 250, 20, AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkIncludeUnits, 280, currentVPos, 24, 24);
	currentVPos += 30;
	std::string bufferInfoText = this->GetBufferSizeText();
	this->AddTextBox(&this->edtCurrentBufferSize, bufferInfoText.c_str(), 200, 30, currentVPos, 450, 26, true, true, false);

	currentVPos += 60;
	this->AddButton(&this->btnCopyAllMap, localizationHandler.GetTranslation(CRLANG_ID_COPY_ALL_MAP, "Copy all map"), 30, currentVPos, btnSize, 30, 0);
	this->AddButton(&this->btnCopyZone, localizationHandler.GetTranslation(CRLANG_ID_COPY_MAP_ZONE, "Copy zone"), 30 + btnSize + 10, currentVPos, btnSize, 30, 0);
	this->AddButton(&this->btnPaste, localizationHandler.GetTranslation(CRLANG_ID_PASTE_MAP_ZONE, "Paste"), 30 + btnSize * 2 + 20, currentVPos, btnSize, 30, 0);
	if (!isInEditor && this->btnPaste) {
		this->btnPaste->editable = 0;
		this->btnPaste->readOnly = 1;
	}

	currentVPos += 60;
	this->AddTextBox(&this->edtResultMessage, "", 200, 30, currentVPos, 450, 30, true, true, false);

	// OK button
	this->AddButton(&this->btnOK, localizationHandler.GetTranslation(LANG_ID_OK, "OK"), this->GetLeftCenteredPositionX(80),
		this->GetScreenSizeY() - 30, 80, 22, 0);
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool MapCopyPopup::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (sender && sender->ptrParentObject) {
		AOE_METHODS::UI_BASE::SetFocus(sender->ptrParentObject, sender); // To validate input text.
	}
	const char *successText = localizationHandler.GetTranslation(CRLANG_ID_SUCCESS, "success");
	if (sender == this->btnCopyZone) {
		if (!this->edtCopyFromX || !this->edtCopyFromY || !this->edtCopyToX || !this->edtCopyToY || !this->edtPasteToY ||
			!this->edtPasteToX || !this->chkIncludeUnits) {
			return true;
		}
		long int minX = StrToInt(this->edtCopyFromX->pTypedText, -1);
		long int minY = StrToInt(this->edtCopyFromY->pTypedText, -1);
		long int maxX = StrToInt(this->edtCopyToX->pTypedText, -1);
		long int maxY = StrToInt(this->edtCopyToY->pTypedText, -1);
		if ((minX < 0) || (minY < 0) || (maxX < 0) || (maxY < 0)) { return true; }
		bool result = MAP::mapCopierInstance.CopyMapZone(minX, minY, maxX, maxY, this->chkIncludeUnits->IsChecked());
		if (this->edtResultMessage->pTypedText != NULL) {
			this->edtResultMessage->SetText(result ? successText : MAP::mapCopierInstance.GetLastError().c_str());
		}
		if (this->edtCurrentBufferSize && this->edtCurrentBufferSize->pTypedText) {
			this->edtCurrentBufferSize->SetText(this->GetBufferSizeText().c_str());
		}
		return true;
	}
	if (sender == this->btnCopyAllMap) {
		if (this->chkIncludeUnits != NULL) {
			AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
			if (global && global->IsCheckSumValid() && global->gameMapInfo && global->gameMapInfo->IsCheckSumValid()) {
				bool result = MAP::mapCopierInstance.CopyMapZone(0, 0, global->gameMapInfo->mapArraySizeX - 1, global->gameMapInfo->mapArraySizeY - 1,
					this->chkIncludeUnits->IsChecked());
				if (this->edtResultMessage->pTypedText != NULL) {
					this->edtResultMessage->SetText(result ? successText : MAP::mapCopierInstance.GetLastError().c_str());
				}
			}
		}
		if (this->edtCurrentBufferSize && this->edtCurrentBufferSize->pTypedText) {
			this->edtCurrentBufferSize->SetText(this->GetBufferSizeText().c_str());
		}
		return true;
	}
	if (sender == this->btnPaste) {
		if (!this->edtPasteToX || !this->edtPasteToY) { return true; }
		long int x = StrToInt(this->edtPasteToX->pTypedText, -1);
		long int y = StrToInt(this->edtPasteToY->pTypedText, -1);
		bool result = MAP::mapCopierInstance.PasteMapZone(x, y);
		if (this->edtResultMessage && (this->edtResultMessage->pTypedText != NULL)) {
			this->edtResultMessage->SetText(result ? successText : MAP::mapCopierInstance.GetLastError().c_str());
		}
		return true;
	}
	
	if (sender == this->btnOK) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool MapCopyPopup::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}


std::string MapCopyPopup::GetBufferSizeText() {
	std::string bufferInfoText = localizationHandler.GetTranslation(CRLANG_ID_MAP_COPY_CURRENT_BUFFER_SIZE, "Current buffer: size=");
	bufferInfoText += std::to_string(MAP::mapCopierInstance.GetCopiedSizeX());
	bufferInfoText += ", ";
	bufferInfoText += std::to_string(MAP::mapCopierInstance.GetCopiedSizeX());
	bufferInfoText += ". ";
	bufferInfoText += localizationHandler.GetTranslation(CRLANG_ID_MAP_COPY_CURRENT_BUFFER_HAS_UNITS, "Contains units");
	bufferInfoText += ": ";
	bufferInfoText += MAP::mapCopierInstance.BufferContainsUnits() ? localizationHandler.GetTranslation(CRLANG_ID_YES, "yes") :
		localizationHandler.GetTranslation(CRLANG_ID_NO, "no");
	return bufferInfoText;
}


}
}
