#include "../include/GenNewTriggerPopup.h"


GenNewTriggerPopup::GenNewTriggerPopup() {
	this->_ResetPointers();
}


void GenNewTriggerPopup::_ResetPointers() {
	__super::_ResetPointers();
	this->cbxActionType = NULL;
	this->cbxEventType = NULL;
	this->edtTriggerText = NULL;
}


void GenNewTriggerPopup::_AddPopupContent() {

	// Popup is open. Add components
	//long int btnhPos = (hSize - 130) / 2;
	//AOE_STRUCTURES::STRUCT_UI_LABEL *unused;
	
	bool success = AOE_AddComboBox(popup, &this->cbxActionType, 20, 40, 80, 80, 80, 80, AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddObjectInContentList(this->cbxActionType);
	// TO DO : fix Global_OnButtonClick in crMainitf
	AOE_AddEntryInCombo(this->cbxActionType, 0, "test1");
	AOE_AddEntryInCombo(this->cbxActionType, 1, "test2");
	this->AddTextBox(popup, &this->edtTriggerText, "0", 3, 140, 40, 80, 20, false, false, true);
	//this->AddLabel(popup, &unused, bufText, 10, 190, 350, 20, AOE_FONT_STANDARD_TEXT);
}
