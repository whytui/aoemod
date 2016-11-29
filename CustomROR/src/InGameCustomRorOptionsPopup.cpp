#include "../include/InGameCustomRorOptionsPopup.h"


InGameCustomRorOptionsPopup::InGameCustomRorOptionsPopup() {
	this->_ResetPointers();
}


void InGameCustomRorOptionsPopup::_ResetPointers() {
	__super::_ResetPointers();
	this->openTechTreeInfo = false;
	this->autoRebuildFarmConfig = NULL;
	this->btnTechTreeInfo = NULL;
	this->btnMapCopy = NULL;
	this->customOptionButtonVar = NULL;
	this->customOptionFreeTextAnswerVar = NULL;
	this->customOptionFreeTextLabelVar = NULL;
	this->customOptionFreeTextVar = NULL;
	this->customOptionGameSpeedLabelVar = NULL;
	this->customOptionGameSpeedTextVar = NULL;
	this->customOptionHeaderLabelVar = NULL;
	this->customOptionHumanPenaltyLabelVar = NULL;
	this->customOptionHumanPenaltyTextVar = NULL;
	this->chkAutoRebuildFarms = NULL;
	this->lblAutoRebuildFarms = NULL;
	this->edtAutoRebuildFarmsMaxFood = NULL;
	this->lblAutoRebuildFarmsMaxFood = NULL;
	this->edtAutoRebuildFarmsMinWood = NULL;
	this->lblAutoRebuildFarmsMinWood = NULL;
	this->edtAutoRebuildFarmsMaxFarms = NULL;
	this->lblAutoRebuildFarmsMaxFarms = NULL;
}

void InGameCustomRorOptionsPopup::_AddPopupContent() {
	if (this->IsClosed() || (this->popup == NULL)) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }

	this->autoRebuildFarmConfig = CUSTOMROR::crInfo.configInfo.GetAutoRebuildFarmConfig(settings->isScenario || settings->isCampaign, settings->isDeathMatch);
	if (!this->autoRebuildFarmConfig) { return; } // ERROR !
	char customOptionHumanPenaltyTextBuffer[12];
	char customOptionGameSpeedFactorTextBuffer[12];
	char maxFarmsTextBuffer[12];
	char maxFoodTextBuffer[12];
	char minWoodTextBuffer[12];
	sprintf_s(customOptionHumanPenaltyTextBuffer, "%ld", CUSTOMROR::crInfo.configInfo.dislike_humanPlayer);
	sprintf_s(customOptionGameSpeedFactorTextBuffer, "%d", (int)(CUSTOMROR::crInfo.configInfo.improvedGameSpeedFactor * (float)100));
	sprintf_s(maxFarmsTextBuffer, "%d", this->autoRebuildFarmConfig->autoRebuildFarms_maxFarms);
	sprintf_s(maxFoodTextBuffer, "%d", this->autoRebuildFarmConfig->autoRebuildFarms_maxFood);
	sprintf_s(minWoodTextBuffer, "%d", this->autoRebuildFarmConfig->autoRebuildFarms_minWood);


	//this->customGamePopupButtonVar
	this->AddLabel(this->popup, &this->customOptionHeaderLabelVar, 
		localizationHandler.GetTranslation(CRLANG_ID_CUSTOMROR_OPTIONS_POPUP_TITLE, "CustomROR Options"), 0xD0, 0x0A, 0x100, 0x1E, AOE_FONTS::AOE_FONT_BIG_LABEL); // Title
	
	// human dislike penalty
	this->AddTextBox(this->popup, &this->customOptionHumanPenaltyTextVar, customOptionHumanPenaltyTextBuffer, 2, 0x120, 0x34, 0x30, 0x16, false, false);
	this->AddLabel(this->popup, &this->customOptionHumanPenaltyLabelVar, localizationHandler.GetTranslation(CRLANG_ID_DISLIKE_PENALTY, txtHumanPenalty), 0x10, 0x30, 0x100, 0x1E);
	// Game speeds config
	this->AddTextBox(this->popup, &this->customOptionGameSpeedTextVar, customOptionGameSpeedFactorTextBuffer, 4, 0x120, 0x4C, 0x30, 0x16, false, false);
	this->AddLabel(this->popup, &this->customOptionGameSpeedLabelVar, localizationHandler.GetTranslation(CRLANG_ID_GAME_SPEED_CHANGE_FACTOR, txtGameSpeedFactor), 0x10, 0x48, 0x100, 0x1E);
	// Farms autorebuild
	this->AddLabel(this->popup, &this->lblAutoRebuildFarms, localizationHandler.GetTranslation(CRLANG_ID_AUTO_REBUILD_FARMS, txtAutoRebuildFarms), 0x10, 0x64, 0x100, 0x1E);
	this->AddCheckBox(this->popup, &this->chkAutoRebuildFarms, 0x120, 0x64, 0x1E, 0x1E);
	AOE_METHODS::AOE_CheckBox_SetChecked(this->chkAutoRebuildFarms, autoRebuildFarmConfig->enableAutoRebuildFarms);
	this->AddLabel(this->popup, &this->lblAutoRebuildFarmsMaxFood, localizationHandler.GetTranslation(CRLANG_ID_AUTO_REBUILD_FARMS_MAX_FOOD, txtAutoRebuildFarmsMaxFood), 0x10, 0x80, 0x100, 0x1E);
	this->AddLabel(this->popup, &this->lblAutoRebuildFarmsMinWood, localizationHandler.GetTranslation(CRLANG_ID_AUTO_REBUILD_FARMS_MIN_WOOD, txtAutoRebuildFarmsMinWood), 0x10, 0x98, 0x100, 0x1E);
	this->AddLabel(this->popup, &this->lblAutoRebuildFarmsMaxFarms, localizationHandler.GetTranslation(CRLANG_ID_AUTO_REBUILD_FARMS_MAX_NUMBER, txtAutoRebuildFarmsMaxNumber), 0x10, 0xB0, 0x100, 0x1E);
	this->AddTextBox(this->popup, &this->edtAutoRebuildFarmsMaxFood, maxFoodTextBuffer, 5, 0x120, 0x84, 0x34, 0x16, false, false);
	this->AddTextBox(this->popup, &this->edtAutoRebuildFarmsMinWood, minWoodTextBuffer, 5, 0x120, 0x9C, 0x34, 0x16, false, false);
	this->AddTextBox(this->popup, &this->edtAutoRebuildFarmsMaxFarms, maxFarmsTextBuffer, 2, 0x120, 0xB4, 0x34, 0x16, false, false);
	
	// Free text zone
	this->AddLabel(this->popup, &this->customOptionFreeTextLabelVar, localizationHandler.GetTranslation(CRLANG_ID_OTHER_COMMANDS_LABEL, "Other commands (enter to validate)"), 0x10, 0x120, 0x100, 0x1E);
	this->AddTextBox(this->popup, &this->customOptionFreeTextVar, "", 100, 0x100, 0x120, 0xB0, 0x16, false, false);
	this->AddTextBox(this->popup, &this->customOptionFreeTextAnswerVar, "", 100, 0x1C0, 0x120, 0x80, 0x16, true, false);
	
	this->AddButton(this->popup, &this->btnTechTreeInfo, localizationHandler.GetTranslation(CRLANG_ID_TECH_TREE, "Tech tree info"), 0x170, 0x34, 0xAC, 0x1E);
	this->AddButton(this->popup, &this->btnMapCopy, localizationHandler.GetTranslation(-1, "Map copy"), 0x170, 0x50, 0xAC, 0x1E);
}

// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool InGameCustomRorOptionsPopup::OnButtonClick(AOE_STRUCTURES::STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnTechTreeInfo) {
		this->openTechTreeInfo = true;
		this->ClosePopup(false);
		return true;
	}
	if (sender == this->btnMapCopy) {
		this->openMapCopyPopup = true;
		this->ClosePopup(false);
		return true;
	}
	return false;
}

void InGameCustomRorOptionsPopup::OnBeforeClose(bool isCancel) {
	char *typedText;
	if (this->customOptionHumanPenaltyTextVar) {
		typedText = this->customOptionHumanPenaltyTextVar->pTypedText;
		CUSTOMROR::crInfo.configInfo.dislike_humanPlayer = atoi(typedText); // does not raise. Returns 0 if invalid.
	}

	if (this->customOptionGameSpeedTextVar) {
		typedText = this->customOptionGameSpeedTextVar->pTypedText;
		float f = (float)(atof(typedText) / 100); // does not raise. Returns 0 if invalid.
		if (f > 1) {
			CUSTOMROR::crInfo.configInfo.improvedGameSpeedFactor = f;
		}
	}

	if (this->customOptionFreeTextAnswerVar) {
		unsigned long int h = this->customOptionFreeTextAnswerVar->hWnd;
		ShowWindow((HWND)h, SW_HIDE);
	}

	if (this->autoRebuildFarmConfig) {
		if (this->chkAutoRebuildFarms) {
			this->autoRebuildFarmConfig->enableAutoRebuildFarms = (this->chkAutoRebuildFarms->checked != 0);
		}
		if (this->edtAutoRebuildFarmsMaxFarms) {
			typedText = this->edtAutoRebuildFarmsMaxFarms->pTypedText;
			this->autoRebuildFarmConfig->autoRebuildFarms_maxFarms = atoi(typedText); // does not raise. Returns 0 if invalid.
		}
		if (this->edtAutoRebuildFarmsMaxFood) {
			typedText = this->edtAutoRebuildFarmsMaxFood->pTypedText;
			this->autoRebuildFarmConfig->autoRebuildFarms_maxFood = atoi(typedText); // does not raise. Returns 0 if invalid.
		}
		if (this->edtAutoRebuildFarmsMinWood) {
			typedText = this->edtAutoRebuildFarmsMinWood->pTypedText;
			this->autoRebuildFarmConfig->autoRebuildFarms_minWood = atoi(typedText); // does not raise. Returns 0 if invalid.
		}
	}
}


void InGameCustomRorOptionsPopup::OnAfterClose(bool isCancel) {
	if (this->openTechTreeInfo) {
		AOE_STRUCTURES::STRUCT_PLAYER *humanPlayer = GetControlledPlayerStruct_Settings();
		assert(humanPlayer && humanPlayer->IsCheckSumValid());

		SimpleEditTextPopup *tmpNextPopup = new SimpleEditTextPopup();
		tmpNextPopup->OpenPopup(700, 580, false);
		tmpNextPopup->AddPopupContent(localizationHandler.GetTranslation(CRLANG_ID_TECH_TREE_CAN_BE_RESEARCHED_TITLE, "Technology tree that can still be researched"),
			GetRemainingTechTreeText(humanPlayer).c_str(), 10000, NULL, false);
		this->nextPopup = tmpNextPopup;
	}
	if (this->openMapCopyPopup) {
		MapCopyPopup *tmpNextPopup = new MapCopyPopup();
		tmpNextPopup->OpenPopup(600, 450, false);
		this->nextPopup = tmpNextPopup;
	}
}


// Specific method to use instead of OpenPopup (special treatments for ingame customROR menu).
// previousPopup should be game menu popup object.
AOE_STRUCTURES::STRUCT_ANY_UI *InGameCustomRorOptionsPopup::CloseMenuAndOpenPopup(AOE_STRUCTURES::STRUCT_ANY_UI *previousPopup) {
	assert(previousPopup != NULL);
	if (!previousPopup) { return NULL; }
	AOE_STRUCTURES::STRUCT_ANY_UI *MainGameUIObj = previousPopup->ptrParentObject;
	this->popup = AOE_METHODS::AOE_CreateCustomOptionsPopupFromMenu(MainGameUIObj);
	if ((this->popup != NULL) && (!this->IsClosed())) {
		// WARNING: here we are adding manually the "standard"/"common" OK button.
		// Normally, this button is created by our popups API (and freed automatically).
		// So we MUST NOT call AddObjectInContentList on this->customOptionButtonVar ! Or the game will crash (trying to free it twice)
		AOE_METHODS::AOE_AddButton(this->popup, &this->customOptionButtonVar, LANG_ID_OK, 0xD8, 0x160, 0xAC, 0x1E); // OK button

		this->_AddPopupContent();
	}
	return this->popup;
}

