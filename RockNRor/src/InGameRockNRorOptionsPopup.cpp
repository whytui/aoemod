#include "../include/InGameRockNRorOptionsPopup.h"


namespace ROCKNROR {
namespace UI {
;


// Reset various pointers for this class level (to override)
void InGameRockNRorOptionsPopup::ResetClassPointers() {
	__super::ResetClassPointers();
	this->btnOK = NULL;
	this->btnCancel = NULL;
	this->autoRebuildFarmConfig = NULL;
	this->btnTechTreeInfo = NULL;
	this->btnMapCopy = NULL;
	this->btnRnrGlobalOptions = NULL;
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


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void InGameRockNRorOptionsPopup::CreateScreenComponents() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }

	this->autoRebuildFarmConfig = ::ROCKNROR::crInfo.configInfo.GetAutoRebuildFarmConfig(settings->rgeGameOptions.isScenario || settings->isCampaign, settings->isDeathMatch);
	if (!this->autoRebuildFarmConfig) { return; } // ERROR !
	char customOptionHumanPenaltyTextBuffer[12];
	char customOptionGameSpeedFactorTextBuffer[12];
	char maxFarmsTextBuffer[12];
	char maxFoodTextBuffer[12];
	char minWoodTextBuffer[12];
	sprintf_s(customOptionHumanPenaltyTextBuffer, "%ld", ::ROCKNROR::crInfo.configInfo.dislike_humanPlayer);
	sprintf_s(customOptionGameSpeedFactorTextBuffer, "%d", (int)(::ROCKNROR::crInfo.configInfo.improvedGameSpeedFactor * (float)100));
	sprintf_s(maxFarmsTextBuffer, "%d", this->autoRebuildFarmConfig->autoRebuildFarms_maxFarms);
	sprintf_s(maxFoodTextBuffer, "%d", this->autoRebuildFarmConfig->autoRebuildFarms_maxFood);
	sprintf_s(minWoodTextBuffer, "%d", this->autoRebuildFarmConfig->autoRebuildFarms_minWood);


	this->AddLabel(&this->customOptionHeaderLabelVar,
		localizationHandler.GetTranslation(CRLANG_ID_ROCKNROR_OPTIONS_POPUP_TITLE, "RockNRor Options"), 0xD0, 0x0A, 0x100, 0x1E, AOE_FONTS::AOE_FONT_BIG_LABEL); // Title

	// human dislike penalty
	this->AddTextBox(&this->customOptionHumanPenaltyTextVar, customOptionHumanPenaltyTextBuffer, 2, 0x120, 0x34, 0x30, 0x16, false, false);
	this->AddLabel(&this->customOptionHumanPenaltyLabelVar, localizationHandler.GetTranslation(CRLANG_ID_DISLIKE_PENALTY, txtHumanPenalty), 0x10, 0x30, 0x100, 0x1E);
	// Game speeds config
	this->AddTextBox(&this->customOptionGameSpeedTextVar, customOptionGameSpeedFactorTextBuffer, 4, 0x120, 0x4C, 0x30, 0x16, false, false);
	this->AddLabel(&this->customOptionGameSpeedLabelVar, localizationHandler.GetTranslation(CRLANG_ID_GAME_SPEED_CHANGE_FACTOR, txtGameSpeedFactor), 0x10, 0x48, 0x100, 0x1E);
	// Farms autorebuild
	this->AddLabel(&this->lblAutoRebuildFarms, localizationHandler.GetTranslation(CRLANG_ID_AUTO_REBUILD_FARMS, txtAutoRebuildFarms), 0x10, 0x64, 0x100, 0x1E);
	this->AddCheckBox(&this->chkAutoRebuildFarms, 0x120, 0x64, 0x1E, 0x1E);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkAutoRebuildFarms, autoRebuildFarmConfig->enableAutoRebuildFarms);
	this->AddLabel(&this->lblAutoRebuildFarmsMaxFood, localizationHandler.GetTranslation(CRLANG_ID_AUTO_REBUILD_FARMS_MAX_FOOD, txtAutoRebuildFarmsMaxFood), 0x10, 0x80, 0x100, 0x1E);
	this->AddLabel(&this->lblAutoRebuildFarmsMinWood, localizationHandler.GetTranslation(CRLANG_ID_AUTO_REBUILD_FARMS_MIN_WOOD, txtAutoRebuildFarmsMinWood), 0x10, 0x98, 0x100, 0x1E);
	this->AddLabel(&this->lblAutoRebuildFarmsMaxFarms, localizationHandler.GetTranslation(CRLANG_ID_AUTO_REBUILD_FARMS_MAX_NUMBER, txtAutoRebuildFarmsMaxNumber), 0x10, 0xB0, 0x100, 0x1E);
	this->AddTextBox(&this->edtAutoRebuildFarmsMaxFood, maxFoodTextBuffer, 5, 0x120, 0x84, 0x34, 0x16, false, false);
	this->AddTextBox(&this->edtAutoRebuildFarmsMinWood, minWoodTextBuffer, 5, 0x120, 0x9C, 0x34, 0x16, false, false);
	this->AddTextBox(&this->edtAutoRebuildFarmsMaxFarms, maxFarmsTextBuffer, 2, 0x120, 0xB4, 0x34, 0x16, false, false);

	// Free text zone
	this->AddLabel(&this->customOptionFreeTextLabelVar, localizationHandler.GetTranslation(CRLANG_ID_OTHER_COMMANDS_LABEL, "Other commands (enter to validate)"), 0x10, 0x120, 0x100, 0x1E);
	this->AddTextBox(&this->customOptionFreeTextVar, "", 100, 0x100, 0x120, 0xB0, 0x16, false, false);
	this->AddTextBox(&this->customOptionFreeTextAnswerVar, "", 100, 0x1C0, 0x120, 0x80, 0x16, true, false);

	this->AddButton(&this->btnRnrGlobalOptions, localizationHandler.GetTranslation(CRLANG_ID_ROCKNROR_GLOBAL_SETTINGS, "Global options"), 0x170, 0x34, 0xAC, 0x1E);
	this->AddButton(&this->btnTechTreeInfo, localizationHandler.GetTranslation(CRLANG_ID_TECH_TREE, "Tech tree info"), 0x170, 0x56, 0xAC, 0x1E);
	this->AddButton(&this->btnMapCopy, localizationHandler.GetTranslation(-1, "Map copy"), 0x170, 0x78, 0xAC, 0x1E);
	
	// OK Cancel buttons
	this->AddButton(&this->btnOK, localizationHandler.GetTranslation(LANG_ID_OK, "OK"), this->GetLeftCenteredPositionX(172),
		this->GetScreenSizeY() - 30, 80, 22, 0);
	this->AddButton(&this->btnCancel, localizationHandler.GetTranslation(LANG_ID_CANCEL, "Cancel"), this->GetLeftCenteredPositionX(172) + 86,
		this->GetScreenSizeY() - 30, 80, 22, 0);
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool InGameRockNRorOptionsPopup::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnTechTreeInfo) {
		AOE_STRUCTURES::STRUCT_PLAYER *humanPlayer = GetControlledPlayerStruct_Settings();
		assert(humanPlayer && humanPlayer->IsCheckSumValid());

		SimpleEditTextPopup *tmpNextPopup = new SimpleEditTextPopup(localizationHandler.GetTranslation(CRLANG_ID_TECH_TREE_CAN_BE_RESEARCHED_TITLE, "Technology tree that can still be researched"), 
			GetRemainingTechTreeText(humanPlayer).c_str(), 10000, NULL, false, false);
		tmpNextPopup->SetFullscreen();
		tmpNextPopup->SetBackgroundTheme(this->GetBackgroundSlpTheme());
		this->OpenOtherScreenAndCloseThisOne(tmpNextPopup, false);
		return true;
	}
	if (sender == this->btnMapCopy) {
		MapCopyPopup *tmpNextPopup = new MapCopyPopup();
		tmpNextPopup->SetBackgroundTheme(this->GetBackgroundSlpTheme());
		this->OpenOtherScreenAndCloseThisOne(tmpNextPopup, false);
		return true;
	}
	if (sender == this->btnRnrGlobalOptions) {
		ROCKNROR::UI::RockNRorSettingsScreen *tmpNextScreen = new ROCKNROR::UI::RockNRorSettingsScreen();
		this->OpenOtherScreenAndCloseThisOne(tmpNextScreen, false);
		return true;
	}
	if (sender == this->btnOK) {
		this->Validate();
		this->CloseScreen(false);
		return true;
	}
	if (sender == this->btnCancel) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool InGameRockNRorOptionsPopup::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	if (uiObj == this->customOptionFreeTextVar) {
		if (keyCode == VK_RETURN) {
			char *typedText = this->customOptionFreeTextVar->pTypedText;
			std::string answer;
			unsigned long int h = this->customOptionFreeTextAnswerVar->hWnd;
			/*bool result =*/ ROCKNROR::crCommand.ExecuteCommand(typedText, answer);
			SendMessageA((HWND)h, WM_SETTEXT, 0, (LPARAM)answer.c_str());
			SendMessageA((HWND)h, EM_SETREADONLY, 1, 0);
			//SendMessageA((HWND)h, WM_SETFOCUS, 0, 0);
			ShowWindow((HWND)h, SW_SHOW);
			if (answer.substr(0, 5) == "Close") {
				// dirty hack to close current popup, so that next popup will be based on normal screen (opening a fullscreen UI under a popup does not work very well)
				this->CloseScreen(false);
			}
		}
	}
	return false;
}


void InGameRockNRorOptionsPopup::Validate() {
	char *typedText;
	if (this->customOptionHumanPenaltyTextVar) {
		typedText = this->customOptionHumanPenaltyTextVar->pTypedText;
		::ROCKNROR::crInfo.configInfo.dislike_humanPlayer = atoi(typedText); // does not raise. Returns 0 if invalid.
	}

	if (this->customOptionGameSpeedTextVar) {
		typedText = this->customOptionGameSpeedTextVar->pTypedText;
		float f = (float)(atof(typedText) / 100); // does not raise. Returns 0 if invalid.
		if (f > 1) {
			::ROCKNROR::crInfo.configInfo.improvedGameSpeedFactor = f;
		}
	}

	if (this->customOptionFreeTextAnswerVar) {
		unsigned long int h = this->customOptionFreeTextAnswerVar->hWnd;
		ShowWindow((HWND)h, SW_HIDE);
	}

	if (this->autoRebuildFarmConfig) {
		if (this->chkAutoRebuildFarms) {
			this->autoRebuildFarmConfig->enableAutoRebuildFarms = (this->chkAutoRebuildFarms->currentState != 0);
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


}
}
