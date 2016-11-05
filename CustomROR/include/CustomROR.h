#pragma once

#include <string.h> // strcmp
#include <stdio.h>
#include <ROR_API_pub.h>
#include <AOE_const_functional.h>
#include <AOE_const_language.h>
#include <AOE_offsets.h>
#include <AOE_struct_unit_def.h>
#include <ROR_API_assert.h>
#include <customResolution.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_game_settings.h>
#include <UI\AOE_struct_ui_in_game_options.h>
#include <AOE_struct_ui_main_info.h>

#include <Windows.h> // for messagebox
#include "crCommon.h"
#include "CustomRORInfo.h"
#include "crConfig.h"
#include "crCommands.h"
#include "crMainInterface.h"
#include "UI_utilities.h"
#include "interface.h"
#include "EXESelfEditor.h"
#include "drsHandler.h"
#include "crLocalization.h"
#include "CustomPopupBase.h"
#include "InGameCustomRorOptionsPopup.h"
#include "cityPlan.h"
#include "unitTargeting.h"
#include "TileSetHandler.h"
#include "combatAI.h"


namespace CUSTOMROR {

	/* This class is the interface object between game code (through ROR_API call) and CustomROR plugin code.
	In theory, methods from this class should only do the basic treatments that allow plugging in our code:
	- Do the equivalent code to what has been modified in game EXE (some instructions may have been removed so that we can add a CALL <ROR_API>)
	- More globally, manage all compatibility issues between ROR context, it normal execution and our custom code: update registry values, change return address, etc
	- Get context information from registry values / stack and transform it into known C++ objects (cf AOE_STRUCTURES for example).
	- Call custom methods to add features/fixes, etc: generally in crCommand class.

	Methods from this class should not do functional treatments, just call entry points/methods from other (more functional) classes.
	*/

	class CustomRORInstance {
	public:
		CustomRORInstance();

		// Entry point : Game code -> ROR_API -> CustomRORInstance.DispatchToCustomCode(...) -> customROR treatments
		void DispatchToCustomCode(REG_BACKUP *REG_values);
#ifdef _DEBUG
		void TemporaryEntryPoints(REG_BACKUP *REG_values);
#endif

		// Methods for ROR

		// Initializations done only once
		void OneShotInit();

		void WMCloseMessageEntryPoint(REG_BACKUP *REG_values);
		void ReadTextFromChat(REG_BACKUP *REG_values);
		void ActionOnGameStart(REG_BACKUP *REG_values);
		void InitBeforeGameStart(REG_BACKUP *REG_values, bool isSavedGame);
		void InitScenarioInfoTextData(REG_BACKUP *REG_values);
		void EntryPoint_OnBeforeSaveGame(REG_BACKUP *REG_values);
		void ManagePanicMode(REG_BACKUP *REG_values);
		void ManageCityPlanHouseDistanceFromBuildings(REG_BACKUP *REG_values);
		void ManageCityMapLikeComputationCall1(REG_BACKUP *REG_values);
		void ManageCityMapLikeComputationCall2(REG_BACKUP *REG_values);
		void ManageCityMapLikeValueAroundBushes(REG_BACKUP *REG_values);
		void ManageCityMapLikeValueFarmPlacement(REG_BACKUP *REG_values);
		void OverloadSetInProgress(REG_BACKUP *REG_values);
		void OverloadResetInProgress(REG_BACKUP *REG_values);
		void OverloadResetUnitInAI(REG_BACKUP *REG_values);
		void OnWonderConstructionCompleted(REG_BACKUP *REG_values);
		void AuditOnDoStrategyElementBuilding(REG_BACKUP *REG_values);
		void AfterAddElementInStrategy(REG_BACKUP *REG_values);
		void FixAutoBuildWarships_addStratElem(REG_BACKUP *REG_values);
		void FixAutoBuildHouse_countHouse(REG_BACKUP *REG_values);
		void FixAutoBuildHouse_maxPopSignedCharLimitation(REG_BACKUP *REG_values);
		void AfterAddDynamicStratElems(REG_BACKUP *REG_values);
		void GameAndEditor_ManageKeyPress(REG_BACKUP *REG_values);
		void GlobalOnButtonClick(REG_BACKUP *REG_values);
		void ManageOnDialogUserEvent(REG_BACKUP *REG_values);
		void ManageChangeGameSpeed(REG_BACKUP *REG_values);
		void ManageTacAIUpdate(REG_BACKUP *REG_values);
		void ManageDefeatedAIPlayerTacAIUpdate(REG_BACKUP *REG_values);
		void ManageAttackActionChange(REG_BACKUP *REG_values); // Obsolete
		void ManageAttackActivityChange1(REG_BACKUP *REG_values);
		void ManageAttackActivityChange_convert(REG_BACKUP *REG_values);
		void ManageTowerPanicMode_militaryUnits(REG_BACKUP *REG_values);
		void ManageTowerPanicMode_villagers(REG_BACKUP *REG_values);
		void ManageBuildingStatus_canConvertUnit(REG_BACKUP *REG_values);
		void ManageGameTimerSkips(REG_BACKUP *REG_values);
		void OnGameRightClickUpInGameCheckActionType(REG_BACKUP *REG_values);
		void OnGameRightClickUpEvent(REG_BACKUP *REG_values);
		void OnGameRightClickUpRedCrossDisplay(REG_BACKUP *REG_values);
		void OnLivingUnitCreation(REG_BACKUP *REG_values);
		void OnGameSettingsNotifyEvent(REG_BACKUP *REG_values);
		void OnGameInitDisableResearchesEvent(REG_BACKUP *REG_values);
		void OnScenarioInitPlayerBadInitialAgeApplication(REG_BACKUP *REG_values);
		void OnGameInitAfterSetInitialAge(REG_BACKUP *REG_values);
		void OnTextBoxKeyPress(REG_BACKUP *REG_values);
		void PlayerCreateUnit_manageStatus(REG_BACKUP *REG_values);
		void CheckPopulationCostWithLogistics(REG_BACKUP *REG_values);
		void ComputeConversionResistance(REG_BACKUP *REG_values);
		void OnSuccessfulConversion(REG_BACKUP *REG_values);
		void ManageOnPlayerAddUnit(REG_BACKUP *REG_values);
		void ManageOnPlayerRemoveUnit(REG_BACKUP *REG_values);
		void FixBuildingStratElemUnitID(REG_BACKUP *REG_values);
		void OverloadIsStratElemUnitAlive_ResetElement(REG_BACKUP *REG_values);
		//void DisableWeakUnitsFromStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI);
		void OnBeforeLoadEmpires_DAT(REG_BACKUP *REG_values);
		void OnAfterLoadEmpires_DAT(REG_BACKUP *REG_values);
		void EditorCheckForUnitPlacement(REG_BACKUP *REG_values);
		void HumanSpecific_onCapturableUnitSeen(REG_BACKUP *REG_values);
		void EntryPoint_UIUnitSelection(REG_BACKUP *REG_values);
		void RORDebugLogHandler(REG_BACKUP *REG_values);
		void CollectAOEDebugLogsInGame(REG_BACKUP *REG_values);
		void GathererPathFindingReturnToDeposit(REG_BACKUP *REG_values);
		void ShowUnitShortcutNumbers(REG_BACKUP *REG_values);
		void InitPlayersCivInScenarioEditor(REG_BACKUP *REG_values);
		void FixUnsupportedRomanTileSetInEditorIcons(REG_BACKUP *REG_values);
		void OnGameMainUiInitTilesetRelatedGraphics(REG_BACKUP *REG_values);
		void OnDisplayBuildingIconInUnitInfoZone(REG_BACKUP *REG_values);
		void OnEditorSetBuildingIconInUnitInfoZone(REG_BACKUP *REG_values);
		void WriteF11PopInfoText(REG_BACKUP *REG_values);
		void FixGetUnitStructInTargetSelectionLoop(REG_BACKUP *REG_values);
		void FixUnitIdBugStuckAttackNoTarget(REG_BACKUP *REG_values);
		void SetActivityTargetUnitIdBug(REG_BACKUP *REG_values);
		void FixActivityTargetUnitIdBug_retreatAfterShooting(REG_BACKUP *REG_values);
		void FixActivityTargetUnitIdBug_case1F4(REG_BACKUP *REG_values);
		void FixKillXCrashOnUnknownPlayer(REG_BACKUP *REG_values);
		void EntryPointOnAfterShowUnitCommandButtons(REG_BACKUP *REG_values);
		void EntryPointOnGameCommandButtonClick(REG_BACKUP *REG_values);
		void ROR_GetButtonInternalIndexFromDatBtnId(REG_BACKUP *REG_values);
		void FixPlayerNoTechTree_applyTech(REG_BACKUP *REG_values);
		void EntryPointAutoSearchTargetUnit(REG_BACKUP *REG_values);
		void EntryPointOnBuildingInfoDisplay(REG_BACKUP *REG_values);
		void EntryPointOnGetLocalizedString(REG_BACKUP *REG_values);
		void AllowMultiUnitTypeInQueue(REG_BACKUP *REG_values);
		void EntryPointOnAttackableUnitKilled(REG_BACKUP *REG_values);
		void EntryPointOnHoverOnUnit(REG_BACKUP *REG_values);
		void EntryPointShowInGameDefaultCursor_noUnitUnderMouse(REG_BACKUP *REG_values);
		void EntryPointAfterActivityStop(REG_BACKUP *REG_values);
		void EntryPointGetMostDislikedPlayerId(REG_BACKUP *REG_values);
		void EntryPointInfAIGroupFindMainTarget(REG_BACKUP *REG_values);

		// Map generation
		void ManageRelicsCount(REG_BACKUP *REG_values);
		void ManageRuinsCount(REG_BACKUP *REG_values);
		void MapGen_elevationProportionCalc(REG_BACKUP *REG_values);
		void MapGen_applyElevation(REG_BACKUP *REG_values);

		// Civilizations management
		void ManageCivIdControlAfterGameSettingsSelection(REG_BACKUP *REG_values);
		void ManageGetPlayerNameDLLStringIdOffset(REG_BACKUP *REG_values);
		void ManageGetPlayerNameDLLStringIdOffset2(REG_BACKUP *REG_values);
		void ManageAIFileSelectionForPlayer(REG_BACKUP *REG_values);
		void ManageCivsInGameSettingsCombo(REG_BACKUP *REG_values);
		void ManageCivsInEditorCombo(REG_BACKUP *REG_values);
		void GetInGameCustomCivName(REG_BACKUP *REG_values);
		void CheckPlayerCreationAtGameStartup(REG_BACKUP *REG_values);

		// Manage Game Timer considerations
		void CollectTimerStats(REG_BACKUP *REG_values);


		// UI
		//void ShowOKDialog(char *message, char *title, long int hSize, long int vSize);
		void DisplayOptionButtonInMenu(REG_BACKUP *REG_values);
		void ManageOptionButtonClickInMenu(REG_BACKUP *REG_values);
		void ManageKeyPressInOptions(REG_BACKUP *REG_values);
		void AfterScenarioEditorCreation(REG_BACKUP *REG_values);
		void ScenarioEditorChangeSelectedTerrain(REG_BACKUP *REG_values);
		void OnComboboxTransferCaptureToPreviousObject(REG_BACKUP *REG_values);
	};

	// Main object
	static CustomRORInstance objCustomRORInstance;
}

// Technical interface method for ROR_API. Do not modify.
extern "C" void __declspec(dllexport) DispatchToCustomCode(REG_BACKUP *REG_values) {
	CUSTOMROR::objCustomRORInstance.DispatchToCustomCode(REG_values);
}
