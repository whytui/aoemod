#include "../include/GenNewTriggerPopup.h"


GenNewTriggerPopup::GenNewTriggerPopup() {
	this->_ResetPointers();
}


void GenNewTriggerPopup::_ResetPointers() {
	__super::_ResetPointers();
	this->cbxActionType = NULL;
	this->cbxEventType = NULL;
	this->edtSampleTriggerText = NULL;
	this->edtTriggerText = NULL;
	this->btnGenerate = NULL;
	this->chkBeforeTrigger = NULL;
	this->chkAfterTrigger = NULL;
	this->chkDiffLevelRestriction = NULL;
	this->btnSaveTriggers = NULL;
	this->btnReloadTriggers = NULL;
}


void GenNewTriggerPopup::_AddPopupContent() {
	// Left part = edit actual triggers.
	// Right part = generate sample triggers
	int splitScreenXPos = this->hSize / 2;
	int btnWidth = 120;
	AOE_STRUCTURES::STRUCT_UI_LABEL *unused;
	
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_ID_EDIT_TRIGGERS, "Edit triggers"), 60, 10, 300, 20, AOE_FONT_BIG_LABEL);
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_ID_GEN_TRIGGER_SAMPLES, "Generate trigger samples"), splitScreenXPos + 40, 10, 300, 20, AOE_FONT_BIG_LABEL);

	// Trigger edition
	this->AddTextBox(this->popup, &this->edtTriggerText, "", 4000, 10, 50, splitScreenXPos - 20, this->vSize - 90, false, true, false);
	this->AddButton(this->popup, &this->btnSaveTriggers, localizationHandler.GetTranslation(CRLANG_ID_SAVE, "Save"), 30, this->vSize - 38, btnWidth, 30);
	this->AddButton(this->popup, &this->btnReloadTriggers, localizationHandler.GetTranslation(CRLANG_ID_RELOAD, "Reload"), btnWidth + 50, this->vSize - 38, btnWidth, 30);

	// Sample triggers part
	int cbxSizeX = 160;
	int cbxSizeY = 20;
	if (splitScreenXPos <= 350) {
		cbxSizeX = 120;
	}
	int comboPosX = splitScreenXPos + 20;
	int comboPosY = 50;
	bool success = this->AddComboBox(popup, &this->cbxEventType, comboPosX, comboPosY, cbxSizeX, cbxSizeY, cbxSizeX, cbxSizeY, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	if (!success) { return; }
	success = this->AddComboBox(popup, &this->cbxActionType, comboPosX + cbxSizeX + 40, comboPosY, cbxSizeX, cbxSizeY, cbxSizeX, cbxSizeY, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	if (!success) { return; }
	this->cbxEventType->Clear();
	this->cbxActionType->Clear();
	this->cbxEventType->SetBorderAroundText(true);
	this->cbxActionType->SetBorderAroundText(true);
	// How to force use of scroll bar on comboboxes to avoid going out of screen ? Seems to be an AOE UI framework bug.
	
	int chkPosX = this->hSize - 10 - 24;
	this->AddCheckBox(popup, &this->chkBeforeTrigger, chkPosX, 90, 24, 24);
	this->AddCheckBox(popup, &this->chkAfterTrigger, chkPosX, 130, 24, 24);
	this->AddCheckBox(popup, &this->chkDiffLevelRestriction, chkPosX, 170, 24, 24);
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_ID_TRG_RUN_BEFORE, "Run another trigger before execution"), comboPosX, 90, 250, 20, AOE_FONT_SMALL_TEXT_6);
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_ID_TRG_RUN_AFTER, "Run another trigger after execution"), comboPosX, 130, 250, 20, AOE_FONT_SMALL_TEXT_6);
	this->AddLabel(popup, &unused, localizationHandler.GetTranslation(CRLANG_ID_TRG_DIFF_LEVEL_CONDITION, "Difficulty level condition"), comboPosX, 170, 250, 20, AOE_FONT_SMALL_TEXT_6);

	int xSpaceAroundButton = splitScreenXPos - btnWidth;
	this->AddButton(popup, &this->btnGenerate, localizationHandler.GetTranslation(CRLANG_ID_GENERATE_TRIGGER_SAMPLE, "Generate"), splitScreenXPos + (xSpaceAroundButton / 2), 200, btnWidth, 30);
	int remainingHeight = this->vSize - 40 - 250; // button: 30 + 20 for space
	int sampleBoxSizeY = 300;
	int sampleBoxPosY = this->vSize - 40 - sampleBoxSizeY;
	this->AddTextBox(popup, &this->edtSampleTriggerText, localizationHandler.GetTranslation(CRLANG_ID_TRG_REFER_TO_CUSTOMIZEAOE_FOR_DOC, "Please run CustomizeAOE.exe to get full documentation on triggers."), 1000, splitScreenXPos + 20, 250, splitScreenXPos - 40, remainingHeight, false, true, false);

	// Prepare events and actions lists.
	for (CR_TRIGGERS::TRIGGER_EVENT_TYPES e = CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_NONE;
		e < CR_TRIGGERS::TRIGGER_EVENTS_TYPES_COUNT; e = CR_TRIGGERS::TRIGGER_EVENT_TYPES(e + 1)) {
		if (e == CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_NONE) {
			AOE_METHODS::UI_BASE::AddEntryInCombo(this->cbxEventType, 0, localizationHandler.GetTranslation(CRLANG_ID_TRG_CHOOSE_EVENT, "Choose an event"));
		} else {
			AOE_METHODS::UI_BASE::AddEntryInCombo(this->cbxEventType, (long int)e, CR_TRIGGERS::TriggerEventTypeToText(e));
		}
	}
	for (CR_TRIGGERS::TRIGGER_ACTION_TYPES a = CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_NONE;
		a < CR_TRIGGERS::TRIGGER_ACTION_TYPES_COUNT; a = CR_TRIGGERS::TRIGGER_ACTION_TYPES(a + 1)) {
		if (a == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_NONE) {
			AOE_METHODS::UI_BASE::AddEntryInCombo(this->cbxActionType, 0, localizationHandler.GetTranslation(CRLANG_ID_TRG_CHOOSE_ACTION, "Choose an action"));
		} else {
			AOE_METHODS::UI_BASE::AddEntryInCombo(this->cbxActionType, (long int)a, CR_TRIGGERS::TriggerActionTypeToText(a));
		}
	}
	this->LoadTriggerTextFromGameInfo();
}

void GenNewTriggerPopup::OnBeforeClose(bool isCancel) {
	AOE_METHODS::UI_BASE::SetFocus(this->popup, this->btnGenerate);
	if (isCancel) { return; }
}

// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool GenNewTriggerPopup::OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) {
	if (!sender) { return false; }
	if (this->btnSaveTriggers) {
		// Make sure to lose focus on input fields to validate user entry.
		AOE_METHODS::UI_BASE::SetFocus(this->btnSaveTriggers->ptrParentObject, NULL);
	}
	if (sender == this->btnReloadTriggers) {
		this->LoadTriggerTextFromGameInfo();
		return true;
	}
	if (sender == this->btnSaveTriggers) {
		this->SaveTriggerTextToGameInfo();
		return true;
	}
	if (sender == this->btnGenerate) {
		CR_TRIGGERS::TRIGGER_EVENT_TYPES evtId = (CR_TRIGGERS::TRIGGER_EVENT_TYPES) this->cbxEventType->GetSelectedIndex();
		CR_TRIGGERS::TRIGGER_ACTION_TYPES actId = (CR_TRIGGERS::TRIGGER_ACTION_TYPES) this->cbxActionType->GetSelectedIndex();
		if (evtId < 0) { evtId = CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_NONE; }
		if (actId < 0) { actId = CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_NONE; }
		std::string trgText = "TRIGGER\r\n";
		trgText += CR_TRIGGERS::GetTriggerParamKeyword(CR_TRIGGERS::KW_NAME);
		trgText += "=<this trigger name>\r\n";
		if (this->chkBeforeTrigger->IsChecked()) {
			trgText += CR_TRIGGERS::GetTriggerParamKeyword(CR_TRIGGERS::KW_RUN_BEFORE);
			trgText += "=<other trigger name>\r\n";
		}
		if (this->chkAfterTrigger->IsChecked()) {
			trgText += CR_TRIGGERS::GetTriggerParamKeyword(CR_TRIGGERS::KW_RUN_AFTER);
			trgText += "=<other trigger name>\r\n";
		}
		if (this->chkDiffLevelRestriction->IsChecked()) {
			trgText += CR_TRIGGERS::GetTriggerParamKeyword(CR_TRIGGERS::KW_DIFF_LEVEL);
			trgText += "=<[0-4] value, 0 is hardest.>\r\n";
		}
		trgText += CR_TRIGGERS::GenerateEventTriggerString(evtId, false);
		trgText += "\r\n";
		trgText += CR_TRIGGERS::GenerateActionTriggerString(actId, false);
		trgText += "\r\n";

		this->edtSampleTriggerText->SetText(trgText.c_str());
		return true;
	}
	return false;
}


void GenNewTriggerPopup::LoadTriggerTextFromGameInfo() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid() || (settings->ptrGlobalStruct == NULL)) { return; }
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *sc_info = settings->ptrGlobalStruct->scenarioInformation;
	if (!sc_info || !sc_info->IsCheckSumValid()) { return; }
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) { return; }

	this->edtTriggerText->SetText(GetTriggerDataPtr(sc_info));
}


void GenNewTriggerPopup::SaveTriggerTextToGameInfo() {
	// Use dedicated API to update trigger information
	CUSTOMROR::TRIGGER::WriteTriggersInGameData(this->edtTriggerText->pTypedText);
}
