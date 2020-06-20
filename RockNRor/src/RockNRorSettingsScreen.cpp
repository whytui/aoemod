#include "../include/RockNRorSettingsScreen.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace UI {
;


RockNRorSettingsScreen::RockNRorSettingsScreen() : RnrScreenBase("RockNRor settings") {
	this->SetFullscreen();
	this->SetBackgroundTheme(AOE_CONST_DRS::AoeScreenTheme::GameSettingsTheme);
	this->ResetClassPointers();
	this->needToApplyChanges = false;
	this->startYforNextPage = 0;
}


RockNRorSettingsScreen::~RockNRorSettingsScreen() {
	if (needToApplyChanges) {
		ROCKNROR::PATCHER::SetMaxPopulationGetterInSPGames(ROCKNROR::crInfo.configInfo.singlePlayerMaxPopulation);
	}
}


// Reset various pointers for this class level (to override)
void RockNRorSettingsScreen::ResetClassPointers() {
	__super::ResetClassPointers();
	this->btnOK = NULL;
	this->btnPageDown = NULL;
	this->chkGenRandomTechTreeRMGames = NULL;
	this->chkRPGGameModeInRandomGames = NULL;
	this->chkRPGGameModeInScenario = NULL;
	this->chkGenStrategyInRM = NULL;
	this->chkGenStrategyInDM = NULL;
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
	this->btnChangeAvgCivBonusCount = NULL;
	this->btnRelicsCount = NULL;
	this->btnRuinsCount = NULL;
	this->cbxMaxAgeInRandomGames = NULL;
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
	const unsigned long int ACTUAL_START_Y = 20;
	unsigned long int currentPosY = ACTUAL_START_Y;
	unsigned long int skippedY = 0;
	unsigned long int initialSkippedY = this->startYforNextPage;
	bool searchForStartYPos = (this->startYforNextPage > 0); // True if we're currently NOT creating objects until we reach correct vertical pos
	STRUCT_UI_LABEL *fooLabel = NULL;
	unsigned long int screenSizeX = this->GetScreenSizeX();
	bool use2columns = (screenSizeX > 1600);
	unsigned long int startOfColumn2 = defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal + 100;

	// Title
	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_ROCKNROR_SETTINGS_TITLE, "RockNRor settings"),
			this->GetCenteredPositionX(titleSizeX), currentPosY, titleSizeX, titleSizeY,
			AOE_FONTS::AOE_FONT_VERY_BIG_LABEL_1);
	}
	currentPosY += titleSizeY + defaultSpaceVertical;

	// Change resolution
	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_CHANGE_SCREEN_RESOLUTION, "Change screen resolution"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddButton(&this->btnResolution1, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RESOLUTION_SMALL, "Small"),
			defaultMarginLeft + defaultSpaceHorizontal,
			currentPosY, buttonSizeX, checkboxSizeY);
		this->AddButton(&this->btnResolution2, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RESOLUTION_MEDIUM, "Medium"),
			defaultMarginLeft + defaultSpaceHorizontal * 2 + buttonSizeX,
			currentPosY, buttonSizeX, checkboxSizeY);
		this->AddButton(&this->btnResolution3, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RESOLUTION_HIGH, "High"),
			defaultMarginLeft + defaultSpaceHorizontal * 3 + buttonSizeX * 2,
			currentPosY, buttonSizeX, checkboxSizeY);
	}
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
	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_INTRO_TEXT, "Please find the full list of options in RockNRor\\RockNRor.xml file. You'll need to restart the game if you modify this file."),
			defaultMarginLeft, currentPosY, this->GetScreenSizeX() - defaultMarginLeft * 2, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	// Section: RockNRor mods
	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_MODS_SECTION, "RockNRor mods"),
			defaultMarginLeft, currentPosY, sectionTitleSizeX, sectionTitleSizeY,
			AOE_FONTS::AOE_FONT_BIG_LABEL);
	}
	currentPosY += sectionTitleSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_GEN_RANDOM_TECH_TREES_RM, "Generate random tech trees/bonuses in Random Map games"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkGenRandomTechTreeRMGames, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	if (!use2columns) {
		currentPosY += checkboxSizeY + defaultSpaceVertical;
		if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
			return;
		}
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddButton(&this->btnChangeAvgCivBonusCount, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_GEN_RANDOM_TECH_TREES_BONUS_COUNT, "Set average number of civ. bonus"),
			use2columns ? startOfColumn2 : defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}
	
	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RPG_MODE_RANDOM_GAMES, "RPG mode - Age Of Might And Magic - in random games"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkRPGGameModeInRandomGames, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RPG_MODE_SCENARIO, "RPG mode - Age Of Might And Magic - in scenario games"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkRPGGameModeInScenario, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_GEN_STRATEGY_RM, "Generate random strategies in Random Map games"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkGenStrategyInRM, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_GEN_STRATEGY_DM, "Generate random strategies in Deathmatch Games"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkGenStrategyInDM, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_NO_WALLS_RANDOM_GAMES, "No walls in Random Games/Deathmatch games"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkNoWallsInRMDM, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_NO_NEUTRAL_DIPL_RANDOM_GAMES, "No neutral diplomacy in Random Maps/Deathmatch games"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkNoNeutralDiplomacy, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_NO_WALL_RANDOM_GAMES, "No dock RM/DM on land maps where AI does not handle docks"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkNoDockInLandMaps, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_SHARE_ALLY_EXPLORATION_IN_RANDOM_GAMES, "Always share ally exploration in random games"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkAlwaysShareExplorationInRandomGames, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddButton(&this->btnChangeSPMaxPopulation, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_SP_MAX_POP_LINK, "Single player games maximum population"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY);
	}
	if (!use2columns) {
		currentPosY += checkboxSizeY + defaultSpaceVertical;
		if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
			return;
		}
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddButton(&this->btnRelicsCount, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RANDOM_GAMES_RELICS_COUNT_LINK, "Set relics count in random games"),
			use2columns ? startOfColumn2 : defaultMarginLeft, currentPosY, checkboxLabelSizeX / 2, checkboxSizeY);
		this->AddButton(&this->btnRuinsCount, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RANDOM_GAMES_RUINS_COUNT_LINK, "Set ruins count in random games"),
			(use2columns ? startOfColumn2 : defaultMarginLeft) + checkboxLabelSizeX / 2 + defaultSpaceHorizontal, currentPosY, checkboxLabelSizeX / 2, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RANDOM_GAMES_MAX_AGE, "Max. age in random games"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddComboBox(&this->cbxMaxAgeInRandomGames, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxLabelSizeX, checkboxSizeY, checkboxLabelSizeX, checkboxSizeY);
		this->cbxMaxAgeInRandomGames->Clear(); // because combobox has 1 empty line by default: remove it
		AOE_METHODS::UI_BASE::AddEntryInCombo(this->cbxMaxAgeInRandomGames, -1, "No limit (default)");
		AOE_METHODS::UI_BASE::AddEntryInCombo(this->cbxMaxAgeInRandomGames, AOE_CONST_FUNC::CST_RSID_STONE_AGE,
			GetLanguageDllText(LANG_ID_STONE_AGE).c_str());
		AOE_METHODS::UI_BASE::AddEntryInCombo(this->cbxMaxAgeInRandomGames, AOE_CONST_FUNC::CST_RSID_TOOL_AGE,
			GetLanguageDllText(LANG_ID_TOOL_AGE).c_str());
		AOE_METHODS::UI_BASE::AddEntryInCombo(this->cbxMaxAgeInRandomGames, AOE_CONST_FUNC::CST_RSID_BRONZE_AGE,
			GetLanguageDllText(LANG_ID_BRONZE_AGE).c_str());
		AOE_METHODS::UI_BASE::AddEntryInCombo(this->cbxMaxAgeInRandomGames, AOE_CONST_FUNC::CST_RSID_IRON_AGE,
			GetLanguageDllText(LANG_ID_IRON_AGE).c_str());
	}

	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	// Section: RockNRor AI options
	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RNR_AI_OPTIONS_SECTION, "RockNRor AI options"),
			defaultMarginLeft, currentPosY, sectionTitleSizeX, sectionTitleSizeY,
			AOE_FONTS::AOE_FONT_BIG_LABEL);
	}
	currentPosY += sectionTitleSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_IMPROVE_AI, "Allow RockNRor to improve AI"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkImproveAI, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_IMPROVE_CITY_PLAN_AI, "Allow RockNRor to improve AI city plan handling"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkImproveCityPlan, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

	if (searchForStartYPos && (currentPosY >= startYforNextPage)) {
		skippedY = this->startYforNextPage + currentPosY;
		searchForStartYPos = false;
		currentPosY = ACTUAL_START_Y;
	}
	if (!searchForStartYPos) {
		this->AddLabel(&fooLabel, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_FIX_LOGISTICS_BUG, "Fix logistics bug (AI no longer trains units without houses)"),
			defaultMarginLeft, currentPosY, checkboxLabelSizeX, checkboxSizeY,
			AOE_FONTS::AOE_FONT_STANDARD_TEXT);
		this->AddCheckBox(&this->chkFixLogisticsBug, defaultMarginLeft + checkboxLabelSizeX + defaultSpaceHorizontal,
			currentPosY, checkboxSizeX, checkboxSizeY);
	}
	currentPosY += checkboxSizeY + defaultSpaceVertical;
	if (!searchForStartYPos && this->CheckBottomOfScreen(currentPosY, initialSkippedY)) {
		return;
	}

#pragma TODO("AI dislike level")



	// Interface
	// auto rebuild farms, autoattack?, auto repair TC, game speed, invisible tree

	// Scenario editor

	// OK button
	this->AddOkButton();

	// Back to top (if needed)
	if (initialSkippedY > 0) {
		// Add the "next page" button in bottom right corner
		this->AddButton(&this->btnBackToTop, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_BACK_TO_TOP, "Back to top"),
			this->GetScreenSizeX() - 200, this->GetScreenSizeY() - 30, 80, 22, 0);
	}

	this->InitInputs();
}


void RockNRorSettingsScreen::InitInputs() {
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkGenRandomTechTreeRMGames, ROCKNROR::crInfo.configInfo.randomTechTreeForRMGames);
	//AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkGenRandomTechTreeDMGames, ROCKNROR::crInfo.configInfo.randomTechTreeForDMGames);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkGenStrategyInRM, ROCKNROR::crInfo.configInfo.generateStrategyForRM);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkGenStrategyInDM, ROCKNROR::crInfo.configInfo.generateStrategyForDM);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkNoDockInLandMaps, ROCKNROR::crInfo.configInfo.noDockInMostlyLandMaps);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkNoNeutralDiplomacy, ROCKNROR::crInfo.configInfo.noNeutralInitialDiplomacy);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkNoWallsInRMDM, ROCKNROR::crInfo.configInfo.noWalls);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkRPGGameModeInRandomGames, ROCKNROR::crInfo.configInfo.enableRPGModeInRandomGames);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkRPGGameModeInScenario, ROCKNROR::crInfo.configInfo.enableRPGModeInScenario);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkAlwaysShareExplorationInRandomGames, ROCKNROR::crInfo.configInfo.allyExplorationIsAlwaysShared);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkImproveAI, ROCKNROR::crInfo.configInfo.improveAILevel > 0);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkImproveCityPlan, ROCKNROR::crInfo.configInfo.cityPlanLikeValuesEnhancement);
	AOE_METHODS::UI_BASE::CheckBox_SetChecked(this->chkFixLogisticsBug, ROCKNROR::crInfo.configInfo.fixLogisticsNoHouseBug);

	int maxAgeSelectedIndex = 0;
	switch (ROCKNROR::crInfo.configInfo.maxAgeRM_DM) {
	case AOE_CONST_FUNC::CST_RSID_STONE_AGE:
	case AOE_CONST_FUNC::CST_RSID_TOOL_AGE:
	case AOE_CONST_FUNC::CST_RSID_BRONZE_AGE:
	case AOE_CONST_FUNC::CST_RSID_IRON_AGE:
		maxAgeSelectedIndex = ROCKNROR::crInfo.configInfo.maxAgeRM_DM - AOE_CONST_FUNC::CST_RSID_STONE_AGE + 1;
	}
	this->cbxMaxAgeInRandomGames->SetSelectedIndex(maxAgeSelectedIndex);
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
	if (checkbox == this->chkGenStrategyInDM) {
		ROCKNROR::crInfo.configInfo.generateStrategyForDM = checkbox->IsChecked();
		return;
	}
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


// Update configuration from current comboboxes selection
void RockNRorSettingsScreen::UpdateConfigFromComboboxes() {
	if (this->cbxMaxAgeInRandomGames) {
		int result = this->cbxMaxAgeInRandomGames->selectedIndex; // Use combo's index, not list's (different if user has navigated to another entry and pressed ESC)
		if (result > 0) {
			result = AOE_CONST_FUNC::CST_RSID_STONE_AGE + result - 1;
		} else {
			result = -1;
		}
		ROCKNROR::crInfo.configInfo.maxAgeRM_DM = result;
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
	this->UpdateConfigFromComboboxes();
	this->CloseScreen(false);
	if (parent && parent->IsCheckSumValidForAChildClass()) {
		_asm {
			MOV ECX, parent;
			MOV EDX, [ECX];
			PUSH y;
			PUSH x;
			CALL[EDX + 0x50]; // handle size. Does not seem sufficient to update parent screen :( ... That's why standard game only proposes resolution change in game screen !
		}
	}
}


// Returns true if y is beyond Y screen size and saves y value into this->startYforNextPage
bool RockNRorSettingsScreen::CheckBottomOfScreen(unsigned long y, unsigned long initialSkippedY) {
	if (y + 30 >= this->GetScreenSizeY()) {
		this->startYforNextPage = initialSkippedY + y;

		// Add the "next page" button in bottom right corner
		this->AddButton(&this->btnPageDown, localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_NEXT_PAGE, "Next page"),
			this->GetScreenSizeX() - 100, this->GetScreenSizeY() - 30, 80, 22, 0);

		this->AddOkButton();

		return true;
	}
	return false;
}


// Frees all UI component objects, and re-create page content
void RockNRorSettingsScreen::ForceReloadPage() {
	AOE_METHODS::UI_BASE::SetFocus(this->GetAoeScreenObject(), NULL); // may avoid issues with comboboxes
	this->UpdateConfigFromComboboxes();
	this->FreeComponents();
	this->ResetClassPointers();
	this->CreateScreenComponents();
}


void RockNRorSettingsScreen::AddOkButton() {
	if (this->btnOK == NULL) {
		this->AddButton(&this->btnOK, localizationHandler.GetTranslation(CRLANG_ID_CLOSE, "Close"), this->GetLeftCenteredPositionX(80),
			this->GetScreenSizeY() - 30, 80, 22, 0);
	}
}


// Returns true if the event is handled and we don't want to handle anymore (disable ROR's additional treatments)
bool RockNRorSettingsScreen::OnButtonClick(STRUCT_UI_BUTTON *sender) {
	if (sender == this->btnOK) {
		this->UpdateConfigFromComboboxes();
		this->CloseScreen(false);
		return true;
	}
	if (sender == this->btnPageDown) {
		this->ForceReloadPage();
		return true;
	}
	if (sender == this->btnBackToTop) {
		this->startYforNextPage = 0;
		this->ForceReloadPage();
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
		if (ROCKNROR::PATCHER::GetHighestResolutionValues(x, y)) {
			this->ChangeResolution(x, y);
		}
		return true;
	}
	if (sender == this->btnChangeAvgCivBonusCount) {
		ROCKNROR::UI::InputBox_int<long int> *maxPopulationPopup = new ROCKNROR::UI::InputBox_int<long int>(
			localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_GEN_RANDOM_TECH_TREES_BONUS_COUNT, "Set average number of civ. bonus"),
			localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_GEN_RANDOM_TECH_TREES_BONUS_COUNT_DTL, "Set the desired average civilization bouns number for random games"),
			ROCKNROR::crInfo.configInfo.randomTechTreeDesiredAvgBonusCount,
			0, AOE_CONST_INTERNAL::TECH_LIMIT_MAXIMUM_POPULATION, &ROCKNROR::crInfo.configInfo.randomTechTreeDesiredAvgBonusCount, false);
		maxPopulationPopup->SetBackgroundTheme(this->GetBackgroundSlpTheme());
		maxPopulationPopup->CreateScreen(this->GetAoeScreenObject());
		this->needToApplyChanges = true;
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
			localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RANDOM_GAMES_RELICS_COUNT_TITLE, "Relics count"),
			localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_ENTER_VALUE_RANDOM_GAMES_RELICS_COUNT, "Enter the desired number of relics for random games. Please note that the game may randomly choose NOT to place relics at all."),
			ROCKNROR::crInfo.configInfo.randomMapRelicsCount,
			0, 999, &ROCKNROR::crInfo.configInfo.randomMapRelicsCount, false);
		maxPopulationPopup->SetBackgroundTheme(this->GetBackgroundSlpTheme());
		maxPopulationPopup->CreateScreen(this->GetAoeScreenObject());
		return true;
	}
	if (sender == this->btnRuinsCount) {
		ROCKNROR::UI::InputBox_int<long int> *maxPopulationPopup = new ROCKNROR::UI::InputBox_int<long int>(
			localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_RANDOM_GAMES_RUINS_COUNT_TITLE, "Ruins count"),
			localizationHandler.GetTranslation(CRLANG_ID_RNR_STTGS_ENTER_VALUE_RANDOM_GAMES_RUINS_COUNT, "Enter the desired number of ruins for random games. Please note that the game may randomly choose NOT to place ruins at all."),
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
		this->UpdateConfigFromComboboxes();
		this->CloseScreen(false);
		return true;
	}
	// VK_NEXT = page down
	if ((keyCode == VK_NEXT) && (this->btnPageDown != NULL)) {
		this->ForceReloadPage();
	}
	// VK_PRIOR = page up
	if ((keyCode == VK_PRIOR) && (this->btnBackToTop != NULL)) {
		this->startYforNextPage = 0;
		this->ForceReloadPage();
	}
	return false;
}



}
}

