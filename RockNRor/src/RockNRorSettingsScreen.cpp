#include "../include/RockNRorSettingsScreen.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace UI {
;


RockNRorSettingsScreen::RockNRorSettingsScreen() : RnrScreenBase("RockNRor settings") {
	this->SetFullscreen();
	this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::GameSettingsTheme);
	this->ResetClassPointers();
}


RockNRorSettingsScreen::~RockNRorSettingsScreen() {
	if (needToApplyChanges) {
		SetMaxPopulationGetterInSPGames(ROCKNROR::crInfo.configInfo.singlePlayerMaxPopulation);
	}
}


// Reset various pointers for this class level (to override)
void RockNRorSettingsScreen::ResetClassPointers() {
	__super::ResetClassPointers();
	this->needToApplyChanges = false;
	this->btnOK = NULL;
	this->chkGenRandomTechTreeRMGames = NULL;
	this->chkRPGGameModeInRandomGames = NULL;
	this->chkRPGGameModeInScenario = NULL;
	this->chkGenStrategyInRM = NULL;
	//this->chkGenStrategyInDM = NULL;
	this->chkNoWallsInRMDM = NULL;
	this->chkNoNeutralDiplomacy = NULL;
	this->chkNoDockInLandMaps = NULL;
	this->chkAlwaysShareExplorationInRandomGames = NULL;
	this->chkImproveAI = NULL;
	this->chkImproveCityPlan = NULL;
	this->chkFixLogisticsBug = NULL;
	this->btnResolution1 = NULL;
	this->btnResolution2 = NULL;
	this->btnResolution3 = NULL;
	this->btnChangeSPMaxPopulation = NULL;
	this->btnRelicsCount = NULL;
	this->btnRuinsCount = NULL;
}


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void RockNRorSettingsScreen::CreateScreenComponents() {
	const unsigned long int titleSizeX = 350;
	const unsigned long int titleSizeY = 30;
	const unsigned long int defaultSpaceHorizontal = 20;
	const unsigned long int defaultSpaceVertical = 10;
	const unsigned long int defaultMarginLeft = 40;
	const unsigned long int sectionTitleSizeX = 500;
	const unsigned long int sectionTitleSizeY = 24;
	const unsigned long int checkboxSizeX = 24;
	const unsigned long int checkboxSizeY = 24;
	const unsigned long int checkboxLabelSizeX = 640;
	const unsigned long int buttonSizeX = 100;
	unsigned long int currentPosY = 20;
	STRUCT_UI_LABEL *fooLabel = NULL;

	// Title
	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_ROCKNROR_SETTINGS_TITLE, "RockNRor settings"),
		this->GetCenteredPositionX(titleSizeX), currentPosY, titleSizeX, titleSizeY,
		AOE_FONTS::AOE_FONT_VERY_BIG_LABEL_1);
	currentPosY += titleSizeY + defaultSpaceVertical;

	// Change resolution
	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_CHANGE_SCREEN_RESOLUTION, "Change screen resolution"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	this->AddButton(&this->btnResolution1, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RESOLUTION_SMALL, "Small"),
		defaultMarginLeft + defaultSpaceHorizontal,
		currentPosY, buttonSizeX, checkboxSizeY);
	this->AddButton(&this->btnResolution2, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RESOLUTION_MEDIUM, "Medium"),
		defaultMarginLeft + defaultSpaceHorizontal * 2 + buttonSizeX,
		currentPosY, buttonSizeX, checkboxSizeY);
	this->AddButton(&this->btnResolution3, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RESOLUTION_HIGH, "High"),
		defaultMarginLeft + defaultSpaceHorizontal * 3 + buttonSizeX * 2,
		currentPosY, buttonSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	// Is the screen large enough ?
	if (this->GetScreenSizeX() <= checkboxLabelSizeX + defaultMarginLeft) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_NEED_HIGHER_RESOLUTION, "Please set a higher resolution to open this screen !"),
			defaultMarginLeft, currentPosY, this->GetScreenSizeX() - defaultMarginLeft, titleSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		currentPosY += titleSizeY + defaultSpaceVertical;
		this->AddButton(&this->btnOK, localizationHandler.GetTranslation(CRLANG_ID_CLOSE, "Close"), this->GetLeftCenteredPositionX(80),
			this->GetScreenSizeY() - 30, 80, 22, 0);
		return;
	}

	// Introduction
	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_INTRO_TEXT, "Please find the full list of options in RockNRor\\RockNRor.xml file. You'll need to restart the game if you modify this file."),
		defaultMarginLeft, currentPosY, this->GetScreenSizeX() - defaultMarginLeft * 2, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	// Section: RockNRor mods
	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_MODS_SECTION, "RockNRor mods"),
		defaultMarginLeft, currentPosY, sectionTitleSizeX, sectionTitleSizeY,
		AOE_FONTS::AOE_FONT_BIG_LABEL);
	currentPosY += sectionTitleSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_GEN_RANDOM_TECH_TREES_RM, "Generate random tech trees/bonuses in Random Map games"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkGenRandomTechTreeRMGames, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RPG_MODE_RANDOM_GAMES, "RPG mode - Age Of Might And Magic - in random games"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkRPGGameModeInRandomGames, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RPG_MODE_SCENARIO, "RPG mode - Age Of Might And Magic - in scenario games"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkRPGGameModeInScenario, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_GEN_STRATEGY_RM, "Generate random strategies in Random Map games"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkGenStrategyInRM, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_GEN_STRATEGY_DM, "Generate random strategies in Deathmatch Games"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	/*this->AddCheckBox(&this->chkGenStrategyInDM, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);*/
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_NO_WALLS_RANDOM_GAMES, "No walls in Random Games/Deathmatch games"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkNoWallsInRMDM, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_NO_NEUTRAL_DIPL_RANDOM_GAMES, "No neutral diplomacy in Random Maps/Deathmatch games"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkNoNeutralDiplomacy, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_NO_WALL_RANDOM_GAMES, "No dock RM/DM on land maps where AI does not handle docks"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkNoDockInLandMaps, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_SHARE_ALLY_EXPLORATION_IN_RANDOM_GAMES, "Always share ally exploration in random games"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkAlwaysShareExplorationInRandomGames, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddButton(&this->btnChangeSPMaxPopulation, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_SP_MAX_POP_LINK, "Single player games maximum population"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddButton(&this->btnRelicsCount, localizationHandler.GetTranslation(0, "Set relics count in random games"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX / 2, checkboxSizeY);
	this->AddButton(&this->btnRuinsCount, localizationHandler.GetTranslation(0, "Set ruins count in random games"),
		defaultMarginLeft + checkboxLabelSizeX / 2 + defaultSpaceHorizontal, currentPosY, checkboxLabelSizeX / 2, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	// Section: RockNRor AI options
	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RNR_AI_OPTIONS_SECTION, "RockNRor AI options"),
		defaultMarginLeft, currentPosY, sectionTitleSizeX, sectionTitleSizeY,
		AOE_FONTS::AOE_FONT_BIG_LABEL);
	currentPosY += sectionTitleSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_IMPROVE_AI, "Allow RockNRor to improve AI"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkImproveAI, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_IMPROVE_CITY_PLAN_AI, "Allow RockNRor to improve AI city plan handling"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkImproveCityPlan, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_FIX_LOGISTICS_BUG, "Fix logistics bug (AI no longer trains units without houses)"),
		defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
		AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	this->AddCheckBox(&this->chkFixLogisticsBug, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
		currentPosY, checkboxSizeX, checkboxSizeY);
	currentPosY += checkboxSizeY + defaultSpaceVertical;

#pragma TODO("AI dislike level")



	// Interface
	// auto rebuild farms, autoattack?, auto repair TC, game speed, invisible tree

	// Scenario editor

	// OK button
	this->AddButton(&this->btnOK, localizationHandler.GetTranslation(CRLANG_ID_CLOSE, "Close"), this->GetLeftCenteredPositionX(80),
		this->GetScreenSizeY() - 30, 80, 22, 0);

	this->InitInputs();
}


void RockNRorSettingsScreen::InitInputs() {
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkGenRandomTechTreeRMGames, ROCKNROR::crInfo.configInfo.randomTechTreeForRMGames);
	//AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkGenRandomTechTreeDMGames, ROCKNROR::crInfo.configInfo.randomTechTreeForDMGames);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkGenStrategyInRM, ROCKNROR::crInfo.configInfo.generateStrategyForRM);
	//AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkGenStrategyInDM, ROCKNROR::crInfo.configInfo.generateStrategyForDM);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkNoDockInLandMaps, ROCKNROR::crInfo.configInfo.noDockInMostlyLandMaps);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkNoNeutralDiplomacy, ROCKNROR::crInfo.configInfo.noNeutralInitialDiplomacy);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkNoWallsInRMDM, ROCKNROR::crInfo.configInfo.noWalls);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkRPGGameModeInRandomGames, ROCKNROR::crInfo.configInfo.enableRPGModeInRandomGames);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkRPGGameModeInScenario, ROCKNROR::crInfo.configInfo.enableRPGModeInScenario);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkAlwaysShareExplorationInRandomGames, ROCKNROR::crInfo.configInfo.allyExplorationIsAlwaysShared);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkImproveAI, ROCKNROR::crInfo.configInfo.improveAILevel > 0);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkImproveCityPlan, ROCKNROR::crInfo.configInfo.cityPlanLikeValuesEnhancement);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkFixLogisticsBug, ROCKNROR::crInfo.configInfo.fixLogisticsNoHouseBug);
}


void RockNRorSettingsScreen::UpdateConfigFromCheckbox(STRUCT_UI_BUTTON *checkbox) {
	if (checkbox == this->chkGenRandomTechTreeRMGames) {
		ROCKNROR::crInfo.configInfo.randomTechTreeForRMGames = checkbox->IsChecked();
		return;
	}
	/*if (checkbox == this->chkGenRandomTechTreeDMGames) {
		ROCKNROR::crInfo.configInfo.randomTechTreeForDMGames = checkbox->IsChecked();
		return;
	}*/
	if (checkbox == this->chkGenStrategyInRM) {
		ROCKNROR::crInfo.configInfo.generateStrategyForRM = checkbox->IsChecked();
		return;
	}
	/*if (checkbox == this->chkGenStrategyInDM) {
		ROCKNROR::crInfo.configInfo.generateStrategyForDM = checkbox->IsChecked();
		return;
	}*/
	if (checkbox == this->chkNoDockInLandMaps) {
		ROCKNROR::crInfo.configInfo.noDockInMostlyLandMaps = checkbox->IsChecked();
		return;
	}
	if (checkbox == this->chkNoNeutralDiplomacy) {
		ROCKNROR::crInfo.configInfo.noNeutralInitialDiplomacy = checkbox->IsChecked();
		return;
	}
	if (checkbox == this->chkNoWallsInRMDM) {
		ROCKNROR::crInfo.configInfo.noWalls = checkbox->IsChecked();
		return;
	}
	if (checkbox == this->chkRPGGameModeInRandomGames) {
		ROCKNROR::crInfo.configInfo.enableRPGModeInRandomGames = checkbox->IsChecked();
		return;
	}
	if (checkbox == this->chkRPGGameModeInScenario) {
		ROCKNROR::crInfo.configInfo.enableRPGModeInScenario = checkbox->IsChecked();
		return;
	}
	if (checkbox == this->chkAlwaysShareExplorationInRandomGames) {
		ROCKNROR::crInfo.configInfo.allyExplorationIsAlwaysShared = checkbox->IsChecked();
		return;
	}
	if (checkbox == this->chkImproveAI) {
		ROCKNROR::crInfo.configInfo.improveAILevel = checkbox->IsChecked() ? 1: 0;
		return;
	}
	if (checkbox == this->chkImproveCityPlan) {
		ROCKNROR::crInfo.configInfo.cityPlanLikeValuesEnhancement = checkbox->IsChecked();
		return;
	}
	if (checkbox == this->chkFixLogisticsBug) {
		ROCKNROR::crInfo.configInfo.fixLogisticsNoHouseBug = checkbox->IsChecked();
		return;
	}
}


void RockNRorSettingsScreen::ChangeResolution(unsigned long int x, unsigned long int y) {
	if (AOE_METHODS::GetScreenFromName(gameScreenName) != NULL) {
		// Unstable. If game screen is open, use standard options menu to change resolution
		ROCKNROR::UI::SimpleEditTextPopup *message = new ROCKNROR::UI::SimpleEditTextPopup(
			localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_ERROR_MSG_TITLE, "Error"),
			localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RESOLUTION_USE_STD_OPTIONS, "Please use standard options to change resolution when game is running"),
			0, NULL, true, false);
		message->SetCenteredForSize(400, 250);
		message->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::RedTheme);
		this->OpenOtherScreenAndCloseThisOne(message, false);
		return;
	}
	STRUCT_UI_EASY_PANEL *myScreen = this->GetAoeScreenObject();
	STRUCT_UI_EASY_PANEL *parent = NULL;
	if (myScreen) {
		parent = (STRUCT_UI_EASY_PANEL*)myScreen->ptrParentObject;
	}
	AOE_METHODS::SetScreenResolution(x, y);
	this->CloseScreen(false);
	if (parent && parent->IsCheckSumValidForAChildClass()) {
		_asm {
			MOV ECX, parent;
			MOV EDX, [ECX];
			PUSH y;
			PUSH x;
			CALL[EDX + 0x50]; // handle size. Does not seem sufficient to update parent screen :(
		}
	}
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool RockNRorSettingsScreen::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnOK) {
		this->CloseScreen(false);
		return true;
	}
	if (sender == this->btnResolution1) {
		this->ChangeResolution(640, 480);

		return true;
	}
	if (sender == this->btnResolution2) {
		this->ChangeResolution(800, 600);
		return true;
	}
	if (sender == this->btnResolution3) {
		long int x, y;
		if (AOE_CONST_DRS::GetHighestResolutionValues(x, y)) {
			this->ChangeResolution(x, y);
		}
		return true;
	}
	if (sender == this->btnChangeSPMaxPopulation) {
		ROCKNROR::UI::InputBox_int<long int> *maxPopulationPopup = new ROCKNROR::UI::InputBox_int<long int>(
			localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_SP_MAXIMUM_POPULATION, "Maximum population"),
			localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_ENTER_VALUE_SP_MAX_POP, "Set a new value for Single Player games population limit"),
			ROCKNROR::crInfo.configInfo.singlePlayerMaxPopulation,
			0, AOE_CONST_INTERNAL::TECH_LIMIT_MAXIMUM_POPULATION, &ROCKNROR::crInfo.configInfo.singlePlayerMaxPopulation, false);
		maxPopulationPopup->SetBackgroundTheme(this->GetBackgroundSlpTheme());
		maxPopulationPopup->CreateScreen(this->GetAoeScreenObject());
		this->needToApplyChanges = true;
		return true;
	}
	if (sender == this->btnRelicsCount) {
		ROCKNROR::UI::InputBox_int<long int> *maxPopulationPopup = new ROCKNROR::UI::InputBox_int<long int>(
			localizationHandler.GetTranslation(0, "Relics count"),
			localizationHandler.GetTranslation(0, "Enter the desired number of relics for random games. Please note that the game may randomly choose NOT to place relics at all."),
			ROCKNROR::crInfo.configInfo.randomMapRelicsCount,
			0, 999, &ROCKNROR::crInfo.configInfo.randomMapRelicsCount, false);
		maxPopulationPopup->SetBackgroundTheme(this->GetBackgroundSlpTheme());
		maxPopulationPopup->CreateScreen(this->GetAoeScreenObject());
		return true;
	}
	if (sender == this->btnRuinsCount) {
		ROCKNROR::UI::InputBox_int<long int> *maxPopulationPopup = new ROCKNROR::UI::InputBox_int<long int>(
			localizationHandler.GetTranslation(0, "Ruins count"),
			localizationHandler.GetTranslation(0, "Enter the desired number of ruins for random games. Please note that the game may randomly choose NOT to place ruins at all."),
			ROCKNROR::crInfo.configInfo.randomMapRuinsCount,
			0, 999, &ROCKNROR::crInfo.configInfo.randomMapRuinsCount, false);
		maxPopulationPopup->SetBackgroundTheme(this->GetBackgroundSlpTheme());
		maxPopulationPopup->CreateScreen(this->GetAoeScreenObject());
		return true;
	}
	this->UpdateConfigFromCheckbox(sender);
	return false;
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool RockNRorSettingsScreen::OnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT) {
	if (keyCode == VK_ESCAPE) {
		this->CloseScreen(false);
		return true;
	}
	return false;
}



}
}

