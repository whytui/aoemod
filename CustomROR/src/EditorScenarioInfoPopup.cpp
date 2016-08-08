#include "../include/EditorScenarioInfoPopup.h"


// Scenario editor main custom popup

EditorScenarioInfoPopup::EditorScenarioInfoPopup() {
	this->_ResetPointers();
	this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_NONE;
}

void EditorScenarioInfoPopup::_ResetPointers() {
	__super::_ResetPointers();
	this->btnAI = NULL;
	this->btnPER = NULL;
	this->btnTerrainEdit = NULL;
	this->btnTriggers = NULL;
	this->btnVictoryCondition = NULL;
	this->edtPlayerId = NULL;
	this->lblTitle = NULL;
	this->chkAllowUnitOverlapping = NULL;
	this->chkDisableHillModeCheck = NULL;
	this->chkDisableTerrainRestrictions = NULL;
	this->chkLengthenCombatMode = NULL;
	this->varToUpdate_allowUnitOverlapping = NULL;
	this->varToUpdate_disableHillModeChecks = NULL;
	this->varToUpdate_disableTerrainRestrictionChecks = NULL;
	this->varToUpdate_lengthenCombatMode = NULL;
}

// Provide the pointer to the boolean that will be updated according to user choice.
void EditorScenarioInfoPopup::SetVarToUpdate_allowUnitOverlapping(bool *varToUpdate) {
	this->varToUpdate_allowUnitOverlapping = varToUpdate;
	if (this->chkAllowUnitOverlapping && varToUpdate) {
		AOE_CheckBox_SetChecked(this->chkAllowUnitOverlapping, *varToUpdate);
	}
}
void EditorScenarioInfoPopup::SetVarToUpdate_disableHillModeChecks(bool *varToUpdate) {
	this->varToUpdate_disableHillModeChecks = varToUpdate;
	if (this->chkDisableHillModeCheck && varToUpdate) {
		AOE_CheckBox_SetChecked(this->chkDisableHillModeCheck, *varToUpdate);
	}
}
void EditorScenarioInfoPopup::SetVarToUpdate_disableTerrainRestrictionChecks(bool *varToUpdate) {
	this->varToUpdate_disableTerrainRestrictionChecks = varToUpdate;
	if (this->chkDisableTerrainRestrictions && varToUpdate) {
		AOE_CheckBox_SetChecked(this->chkDisableTerrainRestrictions, *varToUpdate);
	}
}
void EditorScenarioInfoPopup::SetVarToUpdate_lengthenCombatMode(long int *varToUpdate) {
	this->varToUpdate_lengthenCombatMode = varToUpdate;
	if (this->chkLengthenCombatMode && varToUpdate) {
		AOE_CheckBox_SetChecked(this->chkLengthenCombatMode, (*varToUpdate != 0));
	}
}

void EditorScenarioInfoPopup::_AddPopupContent() {
	const long int btnSize = 0xAC;
	long int btnTotalSpace = hSize - (btnSize * 2);
	long int btnSimpleSpace = btnTotalSpace / 3; // btn count + 1 more space on the right
	long int btnhPos1 = btnSimpleSpace; // x position for buttons
	long int btnhPos2 = btnSize + btnSimpleSpace * 2; // x position for buttons
	this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_NONE;
	this->playerId = -1;
	AOE_STRUCTURES::STRUCT_UI_LABEL *unused;
	this->AddLabel(popup, &this->lblTitle, localizationHandler.GetTranslation(CRLANG_IDCUSTOM_EDITOR_MENU, "Custom editor menu"), btnhPos1 - 5, 10, 190, 30, AOE_FONTS::AOE_FONT_BIG_LABEL);
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_IDPLAYER_ID, "PlayerId :"), btnhPos1, 40, 0x60, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(popup, &this->edtPlayerId, "2", 1, btnhPos1 + btnSize - 0x22, 42, 0x22, 20, false, false, true);
	this->AddButton(popup, &this->btnAI, LANG_ID_STRATEGY, btnhPos1, 80, btnSize, 30, 0);
	this->AddButton(popup, &this->btnPER, LANG_ID_PERSONALITY, btnhPos1, 120, btnSize, 30, 0);
	this->AddButton(popup, &this->btnTriggers, localizationHandler.GetTranslation(CRLANG_IDTRIGGERS, "Triggers"), btnhPos1, 160, btnSize, 30, 0);
	this->AddButton(popup, &this->btnTerrainEdit, localizationHandler.GetTranslation(CRLANG_IDTERRAIN_EDIT, "Terrain edit"), btnhPos1, 200, btnSize, 30, 0);
	this->AddButton(popup, &this->btnVictoryCondition, localizationHandler.GetTranslation(CRLANG_IDVICTORY_CONDITIONS, "Victory conditions"), btnhPos2, 80, btnSize, 30, 0);
	long int chkSize = 30;
	long int hSpace = 15;
	long int lblhSize = 160;
	long int lblhPos = hSpace;
	long int lblhPosCol2 = (hSize - lblhSize - chkSize - hSpace * 2); // x position for checkboxes col2
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_IDUNIT_OVERLAPPING, "Allow unit overlapping"), lblhPos, 240, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(popup, &this->chkAllowUnitOverlapping, lblhPos + lblhSize + hSpace, 240, chkSize, chkSize);
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_IDBUILD_ON_ELEVATION, "Allow buildings on elevation"), lblhPos, 280, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(popup, &this->chkDisableHillModeCheck, lblhPos + lblhSize + hSpace, 280, chkSize, chkSize);
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_IDDISABLE_TERRAIN_RESTRICTION, "Disable terrain restrictions"), lblhPos, 320, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(popup, &this->chkDisableTerrainRestrictions, lblhPos + lblhSize + hSpace, 320, chkSize, chkSize);
	// Checkboxes column 2
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_IDLENGTHEN_COMBAT_MODE, "Lengthen combat mode"), lblhPosCol2, 240, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(popup, &this->chkLengthenCombatMode, lblhPosCol2 + lblhSize + hSpace, 240, chkSize, chkSize);
}


bool EditorScenarioInfoPopup::OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) {
	if (!sender || !sender->IsCheckSumValidForAChildClass()) { return false; }
	bool doCloseCustomPopup = false;
	if (!GetGameGlobalStructPtr()) { return false; }
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = GetGameGlobalStructPtr()->scenarioInformation;
	assert(this->edtPlayerId != NULL);

	if (this->edtPlayerId == NULL) { return false; }
	if (sender->ptrParentObject) {
		AOE_SetFocus(sender->ptrParentObject, sender); // To validate input text.
	}
	if (this->edtPlayerId->pTypedText != NULL) {
		this->playerId = (this->edtPlayerId->pTypedText[0] - '0');
		if ((this->playerId > 8) || (this->playerId < 0)) {
			this->playerId = -1;
			return false; // do not close popup
		}
	}
	if (sender == this->btnAI) {
		if (scInfo->strategyFileSize[this->playerId - 1] <= 0) { return false; }
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_AI;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnPER) {
		if (scInfo->personalityFileSize[this->playerId - 1] <= 0) { return false; }
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_PER;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnTriggers) {
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_TRIGGER;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnTerrainEdit) {
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_TERRAIN_EDIT;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnVictoryCondition) {
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_VICTORY_CONDITION;
		doCloseCustomPopup = true;
	}
	if (sender == this->chkAllowUnitOverlapping) {
		if (this->varToUpdate_allowUnitOverlapping) {
			*this->varToUpdate_allowUnitOverlapping = this->chkAllowUnitOverlapping->IsChecked();
		}
		doCloseCustomPopup = false;
	}
	if (sender == this->chkDisableHillModeCheck) {
		if (this->varToUpdate_disableHillModeChecks) {
			*this->varToUpdate_disableHillModeChecks = this->chkDisableHillModeCheck->IsChecked();
		}
		doCloseCustomPopup = false;
	}
	if (sender == this->chkDisableTerrainRestrictions) {
		if (this->varToUpdate_disableTerrainRestrictionChecks) {
			*this->varToUpdate_disableTerrainRestrictionChecks = this->chkDisableTerrainRestrictions->IsChecked();
		}
		doCloseCustomPopup = false;
	}
	if (sender == this->chkLengthenCombatMode) {
		if (this->varToUpdate_lengthenCombatMode) {
			*this->varToUpdate_lengthenCombatMode = this->chkLengthenCombatMode->IsChecked();
		}
		doCloseCustomPopup = false;
	}

	if (doCloseCustomPopup) {
		this->ClosePopup(false);
		return true;
	}
	return false;
}


void EditorScenarioInfoPopup::OnAfterClose(bool isCancel) {
	if (isCancel) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid() || (settings->ptrGlobalStruct == NULL)) { return; }
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *sc_info = settings->ptrGlobalStruct->scenarioInformation;
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) { return; }
	switch (this->popupToOpen) {
	case SC_INFO_POPUP_TO_OPEN::PTO_TRIGGER:
		if (!this->nextPopup) {
			SimpleEditTextPopup *nextPopup = new SimpleEditTextPopup();
			nextPopup->SetCRCommand(this->crCommand);
			nextPopup->OpenPopup(700, 580, true);
			nextPopup->AddPopupContent(localizationHandler.GetTranslation(CRLANG_ID_EDIT_TRIGGERS, "Edit triggers"), GetTriggerDataPtr(sc_info), 0, NULL, false);
			nextPopup->isForTriggers = true;
			this->nextPopup = nextPopup;
		}
		break;
	case SC_INFO_POPUP_TO_OPEN::PTO_AI: // AI
		if ((this->playerId < 1) || (this->playerId > 8)) { return; }
		if (settings->ptrGlobalStruct && settings->ptrGlobalStruct->scenarioInformation &&
			(settings->ptrGlobalStruct->scenarioInformation->strategyFileSize[this->playerId - 1] > 0)) {
			SimpleEditTextPopup *nextPopup = new SimpleEditTextPopup();
			nextPopup->SetCRCommand(this->crCommand);
			nextPopup->OpenPopup(700, 580, false);
			nextPopup->AddPopupContent(localizationHandler.GetTranslation(CRLANG_ID_GET_STRATEGY, "Get Strategy (read only)"), settings->ptrGlobalStruct->scenarioInformation->strategyFileContent[this->playerId - 1],
				settings->ptrGlobalStruct->scenarioInformation->strategyFileSize[this->playerId - 1], NULL, false);
			this->nextPopup = nextPopup;
		}
		break;
	case SC_INFO_POPUP_TO_OPEN::PTO_PER: // PER
		if ((this->playerId < 1) || (this->playerId > 8)) { return; }
		if (settings->ptrGlobalStruct && settings->ptrGlobalStruct->scenarioInformation &&
			(settings->ptrGlobalStruct->scenarioInformation->personalityFileSize[this->playerId - 1] > 0)) {
			SimpleEditTextPopup *nextPopup = new SimpleEditTextPopup();
			nextPopup->SetCRCommand(this->crCommand);
			nextPopup->OpenPopup(700, 580, false);
			nextPopup->AddPopupContent(localizationHandler.GetTranslation(CRLANG_ID_GET_PERSONALITY, "Get Personality (read only)"), settings->ptrGlobalStruct->scenarioInformation->personalityFileContent[this->playerId - 1],
				settings->ptrGlobalStruct->scenarioInformation->personalityFileSize[this->playerId - 1], NULL, false);
			this->nextPopup = nextPopup;
		}
		break;
	case SC_INFO_POPUP_TO_OPEN::PTO_TERRAIN_EDIT: // terrain edit
	{
		EditTerrainPopup *nextPopup = new EditTerrainPopup();
		nextPopup->SetCRCommand(this->crCommand);
		nextPopup->OpenPopup(500, 300, true);
		this->nextPopup = nextPopup;
		break;
	}
	case SC_INFO_POPUP_TO_OPEN::PTO_VICTORY_CONDITION:
	{
		InputBox_int<long int> *nextPopup = new InputBox_int<long int>();
		nextPopup->SetCRCommand(this->crCommand);
		nextPopup->OpenPopup(450, 200, true);
		nextPopup->AddPopupContent(localizationHandler.GetTranslation(CRLANG_ID_GOLD_VICT_CONDITION, "Gold victory condition"),
			localizationHandler.GetTranslation(CRLANG_ID_GOLD_VICT_CONDITION_DETAILS, "If you want to set a victory condition on gold amount, type a non-zero value.\nYou need to select \"custom\" in general victory conditions tab."),
			settings->ptrGlobalStruct->scenarioInformation->generalVictory_goldAmount, 0, 999999, &settings->ptrGlobalStruct->scenarioInformation->generalVictory_goldAmount, false);
		this->nextPopup = nextPopup;
		break;
	}
	default:
		break;
	}
}

