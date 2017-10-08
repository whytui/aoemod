#pragma once
#include "../include/EditorScenarioInfoPopup.h"
#include "../include/GenNewTriggerPopup.h"




namespace ROCKNROR {
namespace UI {
;


// Reset various pointers for this class level (to override)
void EditorScenarioInfoPopup::ResetClassPointers() {
	__super::ResetClassPointers();
	this->btnOK = NULL;
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
}


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void EditorScenarioInfoPopup::CreateScreenComponents() {
	STRUCT_UI_EASY_PANEL *popup = this->GetAoeScreenObject();
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	long int enableLengthenCombatMode = 0;
	if (global && global->IsCheckSumValid()) {
		AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = global->scenarioInformation;
		if (scInfo && scInfo->IsCheckSumValid()) {
			enableLengthenCombatMode = scInfo->enableLengthenCombatMode;
		}
	}

	const long int btnSize = 0xAC;
	long int btnTotalSpace = this->GetScreenSizeX() - (btnSize * 2);
	long int btnSimpleSpace = btnTotalSpace / 3; // btn count + 1 more space on the right
	long int btnhPos1 = btnSimpleSpace; // x position for buttons
	long int btnhPos2 = btnSize + btnSimpleSpace * 2; // x position for buttons
	this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_NONE;
	this->playerId = -1;
	AOE_STRUCTURES::STRUCT_UI_LABEL *unused;
	this->AddLabel(&this->lblTitle, localizationHandler.GetTranslation(CRLANG_IDCUSTOM_EDITOR_MENU, "Custom editor menu"), btnhPos1 - 5, 10, 190, 30, AOE_FONTS::AOE_FONT_BIG_LABEL);
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_IDPLAYER_ID, "PlayerId :"), btnhPos1, 40, 0x60, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddTextBox(&this->edtPlayerId, "2", 1, btnhPos1 + btnSize - 0x22, 42, 0x22, 20, false, false, true);
	this->AddButton(&this->btnAI, LANG_ID_STRATEGY, btnhPos1, 80, btnSize, 30, 0);
	this->AddButton(&this->btnPER, LANG_ID_PERSONALITY, btnhPos1, 120, btnSize, 30, 0);
	this->AddButton(&this->btnTriggers, localizationHandler.GetTranslation(CRLANG_IDTRIGGERS, "Triggers"), btnhPos1, 160, btnSize, 30, 0);
	this->AddButton(&this->btnTerrainEdit, localizationHandler.GetTranslation(CRLANG_IDTERRAIN_EDIT, "Terrain edit"), btnhPos1, 200, btnSize, 30, 0);
	this->AddButton(&this->btnVictoryCondition, localizationHandler.GetTranslation(CRLANG_IDVICTORY_CONDITIONS, "Victory conditions"), btnhPos2, 80, btnSize, 30, 0);
	long int chkSize = 30;
	long int hSpace = 15;
	long int lblhSize = 160;
	long int lblhPos = hSpace;
	long int lblhPosCol2 = (this->GetScreenSizeX() - lblhSize - chkSize - hSpace * 2); // x position for checkboxes col2
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_IDUNIT_OVERLAPPING, "Allow unit overlapping"), lblhPos, 240, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkAllowUnitOverlapping, lblhPos + lblhSize + hSpace, 240, chkSize, chkSize);
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_IDBUILD_ON_ELEVATION, "Allow buildings on elevation"), lblhPos, 280, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkDisableHillModeCheck, lblhPos + lblhSize + hSpace, 280, chkSize, chkSize);
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_IDDISABLE_TERRAIN_RESTRICTION, "Disable terrain restrictions"), lblhPos, 320, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkDisableTerrainRestrictions, lblhPos + lblhSize + hSpace, 320, chkSize, chkSize);
	// Checkboxes column 2
	this->AddLabel(&unused, localizationHandler.GetTranslation(CRLANG_IDLENGTHEN_COMBAT_MODE, "Lengthen combat mode"), lblhPosCol2, 240, lblhSize, 30, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkLengthenCombatMode, lblhPosCol2 + lblhSize + hSpace, 240, chkSize, chkSize);
	
	// Checkboxes initial values
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkAllowUnitOverlapping, ROCKNROR::crInfo.configInfo.editor_allowUnitOverlapping);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkDisableHillModeCheck, ROCKNROR::crInfo.configInfo.editor_disableHillModeChecks);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkDisableTerrainRestrictions, ROCKNROR::crInfo.configInfo.editor_disableTerrainRestrictions);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkLengthenCombatMode, enableLengthenCombatMode != 0);

	// OK button
	this->AddButton(&this->btnOK, localizationHandler.GetTranslation(LANG_ID_OK, "OK"), this->GetLeftCenteredPositionX(80),
		this->GetScreenSizeY() - 30, 80, 22, 0);
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool EditorScenarioInfoPopup::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (!sender || !sender->IsCheckSumValidForAChildClass()) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid() || (settings->ptrGlobalStruct == NULL)) { return false; }
	if (!GetGameGlobalStructPtr()) { return false; }
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = GetGameGlobalStructPtr()->scenarioInformation;
	assert(scInfo);
	if (!scInfo) { return false; }
	assert(this->edtPlayerId != NULL);

	bool doCloseCustomPopup = false;

	if (this->edtPlayerId == NULL) { return false; }
	if (sender->ptrParentObject) {
		AOE_METHODS::UI_BASE::SetFocus(sender->ptrParentObject, sender); // To validate input text.
	}
	if (this->edtPlayerId->pTypedText != NULL) {
		this->playerId = (this->edtPlayerId->pTypedText[0] - '0');
		if ((this->playerId > 8) || (this->playerId < 0)) {
			this->playerId = -1;
			return false; // do not close popup
		}
	}
	if (sender == this->btnOK) {
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_NONE;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnAI) {
		if (scInfo->strategyFileSize[this->playerId - 1] <= 0) { return false; }
		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_AI;

		/*if ((this->playerId < 1) || (this->playerId > 8)) { return; }
		if (settings->ptrGlobalStruct && settings->ptrGlobalStruct->scenarioInformation &&
			(settings->ptrGlobalStruct->scenarioInformation->strategyFileSize[this->playerId - 1] > 0)) {
			SimpleEditTextPopup *nextPopup = new SimpleEditTextPopup();
			nextPopup->OpenPopup(700, 580, false);
			nextPopup->AddPopupContent(localizationHandler.GetTranslation(CRLANG_ID_GET_STRATEGY, "Get Strategy (read only)"), settings->ptrGlobalStruct->scenarioInformation->strategyFileContent[this->playerId - 1],
				settings->ptrGlobalStruct->scenarioInformation->strategyFileSize[this->playerId - 1], NULL, false);
			this->nextPopup = nextPopup;
		}*/

		doCloseCustomPopup = true;
	}
	if (sender == this->btnPER) {
		if (scInfo->personalityFileSize[this->playerId - 1] <= 0) { return false; }

		/*if ((this->playerId < 1) || (this->playerId > 8)) { return; }
		if (settings->ptrGlobalStruct && settings->ptrGlobalStruct->scenarioInformation &&
			(settings->ptrGlobalStruct->scenarioInformation->personalityFileSize[this->playerId - 1] > 0)) {
			SimpleEditTextPopup *nextPopup = new SimpleEditTextPopup();
			nextPopup->OpenPopup(700, 580, false);
			nextPopup->AddPopupContent(localizationHandler.GetTranslation(CRLANG_ID_GET_PERSONALITY, "Get Personality (read only)"), settings->ptrGlobalStruct->scenarioInformation->personalityFileContent[this->playerId - 1],
				settings->ptrGlobalStruct->scenarioInformation->personalityFileSize[this->playerId - 1], NULL, false);
			this->nextPopup = nextPopup;
		}*/

		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_PER;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnTriggers) {

		/*
		if (!this->nextPopup) {
			STRUCT_ANY_UI *currentScreen = AOE_METHODS::GetCurrentScreen();
			if (currentScreen) {
				GenNewTriggerPopup *nextPopup = new GenNewTriggerPopup();
				nextPopup->OpenPopup(currentScreen->sizeX - 2, currentScreen->sizeY - 2, false, AOE_CONST_DRS::LightOrangeTheme);
				this->nextPopup = nextPopup;
			}
		}*/

		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_TRIGGER;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnTerrainEdit) {
		/*EditTerrainPopup *nextPopup = new EditTerrainPopup();
		nextPopup->OpenPopup(500, 300, true);
		this->nextPopup = nextPopup;*/

		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_TERRAIN_EDIT;
		doCloseCustomPopup = true;
	}
	if (sender == this->btnVictoryCondition) {
		/*InputBox_int<long int> *nextPopup = new InputBox_int<long int>();
		nextPopup->OpenPopup(450, 200, true);
		nextPopup->AddPopupContent(localizationHandler.GetTranslation(CRLANG_ID_GOLD_VICT_CONDITION, "Gold victory condition"),
			localizationHandler.GetTranslation(CRLANG_ID_GOLD_VICT_CONDITION_DETAILS, "If you want to set a victory condition on gold amount, type a non-zero value.\nYou need to select \"custom\" in general victory conditions tab."),
			settings->ptrGlobalStruct->scenarioInformation->generalVictory_goldAmount, 0, 999999, &settings->ptrGlobalStruct->scenarioInformation->generalVictory_goldAmount, false);
		this->nextPopup = nextPopup;*/

		this->popupToOpen = SC_INFO_POPUP_TO_OPEN::PTO_VICTORY_CONDITION;
		doCloseCustomPopup = true;
	}

	if (sender == this->chkAllowUnitOverlapping) {
		ROCKNROR::crInfo.configInfo.editor_allowUnitOverlapping = this->chkAllowUnitOverlapping->IsChecked();
		doCloseCustomPopup = false;
	}
	if (sender == this->chkDisableHillModeCheck) {
		ROCKNROR::crInfo.configInfo.editor_disableHillModeChecks = this->chkDisableHillModeCheck->IsChecked();
		doCloseCustomPopup = false;
	}
	if (sender == this->chkDisableTerrainRestrictions) {
		ROCKNROR::crInfo.configInfo.editor_disableTerrainRestrictions = this->chkDisableTerrainRestrictions->IsChecked();
		doCloseCustomPopup = false;
	}
	if (sender == this->chkLengthenCombatMode) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		if (global && global->IsCheckSumValid()) {
			AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = global->scenarioInformation;
			if (scInfo && scInfo->IsCheckSumValid()) {
				scInfo->enableLengthenCombatMode = this->chkLengthenCombatMode->IsChecked();
			}
		}
		doCloseCustomPopup = false;
	}

	if (doCloseCustomPopup) {
		this->CloseScreen(false);
		return true;
	}
	return false; // Not one of our buttons; let ROR code be executed normally
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool EditorScenarioInfoPopup::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}




}
}

