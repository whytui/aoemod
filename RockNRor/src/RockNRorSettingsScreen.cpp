#include "../include/RockNRorSettingsScreen.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace UI {
;

// Reset various pointers for this class level (to override)
void RockNRorSettingsScreen::ResetClassPointers() {
	__super::ResetClassPointers();
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
}


// Create screen content: buttons, etc. Called by CreateScreen(...) method.
void RockNRorSettingsScreen::CreateScreenComponents() {
	const unsigned long int titleSizeX = 350;
	const unsigned long int titleSizeY = 30;
	const unsigned long int defaultSpaceHorizontal = 20;
	const unsigned long int defaultSpaceVertical = 10;
	const unsigned long int defaultMarginLeft = 40;
	const unsigned long int sectionTitleSizeX = 400;
	const unsigned long int sectionTitleSizeY = 24;
	const unsigned long int checkboxSizeX = 24;
	const unsigned long int checkboxSizeY = 24;
	const unsigned long int checkboxLabelSizeX = 640;
	unsigned long int currentPosY = 20;
	STRUCT_UI_LABEL *fooLabel = NULL;

	if (this->GetScreenSizeX() <= checkboxLabelSizeX + defaultMarginLeft) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_NEED_HIGHER_RESOLUTION, "Please set a higher resolution to open this screen !"),
			defaultMarginLeft, currentPosY, this->GetScreenSizeX() - defaultMarginLeft, titleSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		currentPosY += titleSizeY + defaultSpaceVertical;
		this->AddButton(&this->btnOK, localizationHandler.GetTranslation(CRLANG_ID_CLOSE, "Close"), this->GetLeftCenteredPositionX(80),
			this->GetScreenSizeY() - 30, 80, 22, 0);
		return;
	}

	// Title
	this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_ROCKNROR_SETTINGS_TITLE, "RockNRor settings"),
		this->GetCenteredPositionX(titleSizeX), currentPosY, titleSizeX, titleSizeY,
		AOE_FONTS::AOE_FONT_VERY_BIG_LABEL_1);
	currentPosY += titleSizeY + defaultSpaceVertical;

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

#pragma TODO("Max population")
//#pragma TODO("Initial resources (?)")
#pragma TODO("Relic/ruins count")

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
	// Change resolution

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
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool RockNRorSettingsScreen::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnOK) {
		this->CloseScreen(false);
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

