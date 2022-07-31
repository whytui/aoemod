#include "../include/SearchUnitPopup.h"
#include "../include/UI_utilities.h"
#include "../include/playerHandling.h"


namespace ROCKNROR {
namespace UI {
;



// Reset various pointers for this class level (to override)
void SearchUnitPopup::ResetClassPointers() {
	__super::ResetClassPointers();
	this->lastResultUnitId = -1;
	this->btnOK = NULL;
	this->btnDoSearch = NULL;
	this->btnFindNext = NULL;
	this->lblTitle = NULL;
	this->edtDATId = NULL;
	this->edtUnitId = NULL;
	this->edtPosX = NULL;
	this->edtPosY = NULL;
	this->edtPosRadius = NULL;
	this->edtUnitClassId = NULL;
	this->edtPlayerId = NULL;
	this->edtResultInfo = NULL;
}


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void SearchUnitPopup::CreateScreenComponents() {
	if ((this->GetScreenStatus() != CREATED) || (this->GetAoeScreenObject() == NULL)) { return; }

	AOE_STRUCTURES::STRUCT_UI_LABEL *unusedLabel;
	long int currentVPos = 10;

	this->AddLabel(&this->lblTitle, localizationHandler.GetTranslation(CRLANG_ID_SEARCH_UNIT_BTN, "Search unit"),
		200, currentVPos, 200, 24, AOE_FONTS::AOE_FONT_BIG_LABEL);
	
	currentVPos += 50;

	this->AddLabel(&unusedLabel, localizationHandler.GetTranslation(CRLANG_ID_UNITID_DATID_LABEL, "Unit instance ID / Unit Definition (DAT) ID"),
		10, currentVPos, 260, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(&this->edtUnitId, "", 8, 300, currentVPos, 80, 20, false, false, true);
	this->AddTextBox(&this->edtDATId, "", 4, 400, currentVPos, 80, 20, false, false, true);

	currentVPos += 30;

	this->AddLabel(&unusedLabel, localizationHandler.GetTranslation(CRLANG_ID_UNIT_CLASS_ID_LABEL, "Unit Class ID"),
		10, currentVPos, 260, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(&this->edtUnitClassId, "", 2, 300, currentVPos, 80, 20, false, false, true);

	currentVPos += 40;

	this->AddLabel(&unusedLabel, localizationHandler.GetTranslation(CRLANG_ID_OWNER_PLAYER_ID_LABEL, "Owner player ID"),
		10, currentVPos, 260, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(&this->edtPlayerId, "", 1, 300, currentVPos, 80, 20, false, false, true);

	currentVPos += 40;

	this->AddLabel(&unusedLabel, localizationHandler.GetTranslation(CRLANG_ID_UNIT_APPROX_POSITION_LABEL, "Unit approximate position X/Y"),
		10, currentVPos, 260, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(&this->edtPosX, "", 3, 300, currentVPos, 80, 20, false, false, true);
	this->AddTextBox(&this->edtPosY, "", 3, 400, currentVPos, 80, 20, false, false, true);

	currentVPos += 30;
	
	this->AddLabel(&unusedLabel, localizationHandler.GetTranslation(CRLANG_ID_POSITION_RADIUS_LABEL, "Position search radius"),
		10, currentVPos, 260, 20, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(&this->edtPosRadius, "5", 3, 400, currentVPos, 80, 20, false, false, true);

	currentVPos += 40;

	this->AddButton(&this->btnDoSearch, localizationHandler.GetTranslation(CRLANG_ID_SEARCH_UNIT_BTN, "Search"), 140,
		currentVPos, 150, 22, 0);
	this->AddButton(&this->btnFindNext, localizationHandler.GetTranslation(CRLANG_ID_FIND_NEXT_BTN, "Find next"), 320,
		currentVPos, 150, 22, 0);
	currentVPos += 30;

	this->AddTextBox(&this->edtResultInfo, "", 255, 20, currentVPos, this->GetScreenSizeX() - 40, 20, true, false, false);
	currentVPos += 30;

	// OK button
	this->AddButton(&this->btnOK, localizationHandler.GetTranslation(LANG_ID_OK, "OK"), this->GetLeftCenteredPositionX(80),
		this->GetScreenSizeY() - 30, 80, 22, 0);

}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool SearchUnitPopup::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnOK) {
		this->CloseScreen(false);
		return true;
	}
	if (sender == this->btnDoSearch) {
		this->lastResultUnitId = INT_MIN;
		this->DoSearch();
		return true;
	}
	if (sender == this->btnFindNext) {
		this->DoSearch();
		return true;
	}
	
	return false; // Not one of our buttons; let ROR code be executed normally
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool SearchUnitPopup::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}


// Execute the unit search
void SearchUnitPopup::DoSearch() {
	char *textUnitId = AOE_METHODS::UI_BASE::GetEditText(this->edtUnitId);
	char *textDATID = AOE_METHODS::UI_BASE::GetEditText(this->edtDATId);
	char *textX = AOE_METHODS::UI_BASE::GetEditText(this->edtPosX);
	char *textY = AOE_METHODS::UI_BASE::GetEditText(this->edtPosY);
	char *textUnitClass = AOE_METHODS::UI_BASE::GetEditText(this->edtUnitClassId);
	char *textPlayerId = AOE_METHODS::UI_BASE::GetEditText(this->edtPlayerId);
	char *textPosRadius = AOE_METHODS::UI_BASE::GetEditText(this->edtPosRadius);
	long int unitId = atoi(textUnitId);
	long int datId = atoi(textDATID);
	long int x = atoi(textX);
	long int y = atoi(textY);
	long int posRadius = atoi(textPosRadius);
	long int unitClassId = atoi(textUnitClass);
	long int playerId = atoi(textPlayerId);
	if (*textUnitId == 0) { unitId = -1; }
	if (*textDATID == 0) { datId = -1; }
	if (*textX == 0) { x = -1; }
	if (*textY == 0) { y = -1; }
	if (posRadius <= 0) { posRadius = 1; }
	if (*textUnitClass == 0) { unitClassId = -1; }
	if (*textPlayerId == 0) { playerId = -1; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *result = this->FindUnit(playerId, unitId, datId, unitClassId, x, y, posRadius, this->lastResultUnitId + 1);
	this->HandleUnitFound(result);
}


AOE_STRUCTURES::STRUCT_UNIT_BASE *SearchUnitPopup::FindUnit(long int playerId, long int unitId, long int unitDATID,
	long int unitClassId, long int posX, long int posY, long int posRadius, long int startSearch) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return NULL; }

	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = NULL;
	if (unitId >= 0) {
		unit = global->GetUnitFromId(unitId);
		if (unit != NULL) { return unit; } // A search by instance ID cannot find multiple results anyway
	}

	if (startSearch < global->unitIdSeqForTempUnits) {
		startSearch = global->unitIdSeqForTempUnits + 1;
	}
	
	for (int i = startSearch; i < global->seqUnitId; i++) {
		unit = global->GetUnitFromIdAll(i);
		
		if ((unit == NULL) || (unit->unitDefinition == NULL) || (unit->ptrStructPlayer == NULL)) { continue; }
		if ((playerId >= 0) && (unit->ptrStructPlayer->playerId != playerId)) {
			continue;
		}
		if ((unitDATID >= 0) && (unit->unitDefinition->DAT_ID1 != unitDATID)) {
			continue;
		}
		if ((unitClassId >= 0) && (unit->unitDefinition->unitAIType != unitClassId)) {
			continue;
		}
		if (posX >= 0) {
			double a = abs((float)posX - unit->positionX);
			long int delta = (long int)a;
			if (delta > posRadius) {
				continue;
			}
		}
		if (posY >= 0) {
			double a = abs((float)posY - unit->positionY);
			long int delta = (long int)a;
			if (delta > posRadius) {
				continue;
			}
		}
		return unit;
	}

	return NULL;
}


void SearchUnitPopup::HandleUnitFound(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
	if (unit == NULL) {
		this->edtResultInfo->SetText(localizationHandler.GetTranslation(CRLANG_ID_NOT_FOUND, "Not found"));
		this->lastResultUnitId = INT_MIN;
		return;
	}
	this->lastResultUnitId = unit->unitInstanceId;
	std::string text = localizationHandler.GetTranslation(CRLANG_ID_FOUND_UNIT_TEXT_1, "Found unit");
	text += " #";
	text += std::to_string(unit->unitInstanceId);
	text += " ";
	text += localizationHandler.GetTranslation(CRLANG_ID_FOUND_UNIT_TEXT_2, "at position");
	text += " (";
	text += std::to_string(unit->positionX);
	text += ", ";
	text += std::to_string(unit->positionY);
	text += ") : ";

	char buffer[100];
	char *defaultValue = "";
	if (unit->unitDefinition->ptrUnitName != NULL) {
		defaultValue = unit->unitDefinition->ptrUnitName;
	}
	GetLanguageDllText(unit->unitDefinition->languageDLLID_Name, buffer, sizeof(buffer), defaultValue);
	text += buffer;

	this->edtResultInfo->SetText(text.c_str());

	AOE_STRUCTURES::PLAYER::SelectOneUnit(GetControlledPlayerStruct_Settings(), unit, true);

	// To allow game zone to actually move to the correct location, we need to make it active (and then activate our popup back)
	AOE_METHODS::UI_BASE::SetCurrentPanel(scenarioEditorScreenName, 0);
	AOE_METHODS::UI_BASE::SetCurrentPanel(this->GetScreenName().c_str(), 1);
	
}


}
}

