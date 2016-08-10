#include "../include/GenNewTriggerPopup.h"


GenNewTriggerPopup::GenNewTriggerPopup() {
	this->_ResetPointers();
}


void GenNewTriggerPopup::_ResetPointers() {
	__super::_ResetPointers();
	this->cbxActionType = NULL;
	this->cbxEventType = NULL;
	this->edtTriggerText = NULL;
	this->btnGenerate = NULL;
	this->chkBeforeTrigger = NULL;
	this->chkAfterTrigger = NULL;
	this->chkDiffLevelRestriction = NULL;
}


void GenNewTriggerPopup::_AddPopupContent() {
	AOE_STRUCTURES::STRUCT_UI_LABEL *unused;
	
	this->AddLabel(popup, &unused, "Generate trigger samples", 100, 10, 300, 20, AOE_FONT_BIG_LABEL);

	int cbxSizeX = 160;
	int cbxSizeY = 20;
	int posX = 60;
	int posY = 50;
	bool success = this->AddComboBox(popup, &this->cbxEventType, posX, posY, cbxSizeX, cbxSizeY, cbxSizeX, cbxSizeY, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	if (!success) { return; }
	success = this->AddComboBox(popup, &this->cbxActionType, posX + cbxSizeX + 40, posY, cbxSizeX, cbxSizeY, cbxSizeX, cbxSizeY, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	if (!success) { return; }
	this->cbxEventType->Clear();
	this->cbxActionType->Clear();
	this->cbxEventType->SetBorderAroundText(true);
	this->cbxActionType->SetBorderAroundText(true);
	// How to force use of scroll bar on comboboxes to avoid going out of screen ?
	this->AddTextBox(popup, &this->edtTriggerText, NULL, 5000, 20, 240, 460, 60, false, true, false);
	//this->AddTextBox(popup, &this->edtTriggerText, "abcdef", 5000, 20, 240, 460, 60, false, true, false);
	
	this->AddCheckBox(popup, &this->chkBeforeTrigger, 420, 120, 24, 24);
	this->AddCheckBox(popup, &this->chkAfterTrigger, 420, 160, 24, 24);
	this->AddCheckBox(popup, &this->chkDiffLevelRestriction, 420, 200, 24, 24);
	this->AddLabel(popup, &unused, "Run another trigger before execution", 180, 120, 220, 20, AOE_FONT_SMALL_TEXT_6);
	this->AddLabel(popup, &unused, "Run another trigger after execution", 180, 160, 220, 20, AOE_FONT_SMALL_TEXT_6);
	this->AddLabel(popup, &unused, "Difficulty level condition", 180, 200, 150, 20, AOE_FONT_SMALL_TEXT_6);

	this->AddButton(popup, &this->btnGenerate, "Generate", 190, 320, 120, 30);

	// Prepare events and actions lists.
	for (CR_TRIGGERS::TRIGGER_EVENT_TYPES e = CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_NONE;
		e < CR_TRIGGERS::TRIGGER_EVENTS_TYPES_COUNT; e = CR_TRIGGERS::TRIGGER_EVENT_TYPES(e + 1)) {
		if (e == CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_NONE) {
			AOE_AddEntryInCombo(this->cbxEventType, 0, "Choose an event");
		} else {
			AOE_AddEntryInCombo(this->cbxEventType, (long int)e, CR_TRIGGERS::TriggerEventTypeToText(e));
		}
	}
	for (CR_TRIGGERS::TRIGGER_ACTION_TYPES a = CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_NONE;
		a < CR_TRIGGERS::TRIGGER_ACTION_TYPES_COUNT; a = CR_TRIGGERS::TRIGGER_ACTION_TYPES(a + 1)) {
		if (a == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_NONE) {
			AOE_AddEntryInCombo(this->cbxActionType, 0, "Choose an action");
		} else {
			AOE_AddEntryInCombo(this->cbxActionType, (long int)a, CR_TRIGGERS::TriggerActionTypeToText(a));
		}
	}
}

void GenNewTriggerPopup::OnBeforeClose(bool isCancel) {
	/*if (isCancel) { return; }
	AOE_SetFocus(this->popup, this->btnGenerate);*/
}

// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool GenNewTriggerPopup::OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) {
	if (!sender) { return false; }
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

		this->edtTriggerText->SetText(trgText.c_str());
		return true;
	}
	return false;
}
