#include "../include/RockNRorInstance.h"



//#pragma warning(push)
//#pragma warning(disable: 4731) // Allow modifying EBP in assembler code... Required for compliance with some AOE methods. Be careful !
//#pragma warning(disable: 4733) // Allow modifying FS[0]... Required for compliance with some AOE methods. Be careful !

namespace ROCKNROR {

RockNRorInstance::RockNRorInstance() {
	this->gameVersionChecked = false;
}


void RockNRorInstance::DispatchToCustomCode(REG_BACKUP *REG_values) {
	if (!this->gameVersionChecked) {
		this->gameVersionChecked = true;
		if (!ROCKNROR::crCommand.CheckGameVersion()) {
			MessageBoxA(0, "Warning: game version does not match " MOD_NAME " version (" GAMEVERSION_TEXT 
				"). The game will be highly unstable.", "ROR API", MB_ICONWARNING);
		}
	}
#ifdef _DEBUG
	assert(REG_values != NULL);
	long int thisStartTime = AOE_METHODS::TimeGetTime();
#endif
	if ((REG_values->return_address < AOE_OFFSETS::ADDR_EXE_MIN) || (REG_values->return_address > AOE_OFFSETS::ADDR_EXE_MAX)) {
		std::string msg = "Received a ROR_API call with invalid address (";
		msg += GetHexStringAddress(REG_values->return_address);
		msg += "). Skipping.";
		traceMessageHandler.WriteMessage(msg);
		return; // invalid address. Bad calling information - bad call stack.
	}

	switch (REG_values->return_address) {
	//case 0x443713:
	case AOE_ROR_API_FIRST_CALL_RETURN_ADDRESS:
		// Inits
		this->OneShotInit();
		break;

	case 0x0041AEFA:
		this->WMCloseMessageEntryPoint(REG_values);
		break;
	case 0x00502A3D: // new game start/restart, including scenario
	case 0x00503054: // game start/restart from a saved game
		this->ActionOnGameStart(REG_values);
		break;
	case 0x0051D50E: // "standard" new game init
		this->InitBeforeGameStart(REG_values, false);
		break;
	case 0x00502E2A: // new game init for saved game
		this->InitBeforeGameStart(REG_values, true);
		break;
	case 0x0050693C:
		this->InitScenarioInfoTextData(REG_values);
		break;
	case 0x0051E0E1:
		this->EntryPoint_OnBeforeSaveGame(REG_values);
		break;
	case 0x435F74:
		this->ReadTextFromChat(REG_values);
		break;
	case 0x4B24F4:
		this->ComputeConversionResistance(REG_values);
		break;
	case 0x04B2578:
		this->OnSuccessfulConversion(REG_values);
		break;
	case 0x004B8BE5:
		this->OnWonderConstructionCompleted(REG_values);
		break;
	case 0x4D221D:
		this->AuditOnDoStrategyElementBuilding(REG_values);
		break;
	case 0x004AC92F:
		this->FixBuildingStratElemUnitID(REG_values);
		break;
	case 0x00408F92:
		this->AfterAddElementInStrategy(REG_values);
		break;
	case 0x004B8C74:
		this->OverloadIsStratElemUnitAlive_ResetElement(REG_values);
		break;
	case 0x004B7D89:
		this->FixAutoBuildWarships_addStratElem(REG_values);
		break;
	case 0x004B7E23:
		this->FixAutoBuildHouse_countHouse(REG_values);
		break;
	case 0x004B7E66:
		this->FixAutoBuildHouse_maxPopSignedCharLimitation(REG_values);
		break;
	case 0x004B801C:
		this->AfterAddDynamicStratElems(REG_values);
		break;
	case 0x04E2300:
		this->ManagePanicMode(REG_values);
		break;
	case 0x04D7AE6:
		this->TacAIOnUnitAttacked(REG_values);
		break;
	case 0x004C253D:
		this->ManageCityPlanHouseDistanceFromBuildings(REG_values);
		break;
	case 0x004BE8DD:
		this->ManageCityMapLikeComputationCall1(REG_values);
		break;
	case 0x004C1CB4:
		this->ManageCityMapLikeComputationCall2(REG_values);
		break;
	case 0x004C2386:
		this->ManageCityMapLikeValueAroundBushes(REG_values);
		break;
	case 0x004C27D6:
		this->ManageCityMapLikeValueFarmPlacement(REG_values);
		break;
	case 0x004DC088:
		this->OverloadSetInProgress(REG_values);
		break;
	case 0x004B8FC7:
		this->OverloadResetInProgress(REG_values);
		break;
	case 0x004F2B4F:
		this->OverloadResetUnitInAI(REG_values);
		break;
	case 0x00460519:
		this->GlobalOnButtonClick(REG_values);
		break;
	case 0x0041A6C7:
		this->GameAndEditor_ManageKeyPress(REG_values);
		break;
	case 0x00519FE3:
		this->OnGameRightClickUpInGameCheckActionType(REG_values);
		break;
	case 0x0051A508:
		this->OnGameRightClickUpEvent(REG_values);
		break;
	case 0x00462E65:
		this->OnTextBoxKeyPress(REG_values);
		break;
	case 0x004D098E:
		this->ManageTacAIUpdate(REG_values);
		break;
	case 0x4D1B81:
		this->ManageDefeatedAIPlayerTacAIUpdate(REG_values);
		break;
	case 0x004ED189:
		this->OnLivingUnitCreation(REG_values);
		break;
	case 0x0411D5D:
		this->ManageAttackActivityChange1(REG_values);
		break;
	case 0x0411F2B:
		this->ManageAttackActivityChange_convert(REG_values);
		break;
	// These 2 have exactly the same behaviour: use a common method (all that is specific (like JMP addresses) is in ROR modified code, not here)
	case 0x04E37B4:
	case 0x04D829E:
		this->ManageTowerPanicMode_militaryUnits(REG_values); // To rename...
		break;
	case 0x004E39DC:
		this->ManageTowerPanicMode_villagers(REG_values);
		break;
	case 0x004E5006:
		ManageBuildingStatus_canConvertUnit(REG_values);
		break;
	case 0x00433E7C:
		this->DisplayOptionButtonInMenu(REG_values);
		break;
	case 0x0043424D:
		this->ManageOptionButtonClickInMenu(REG_values);
		break;
	case 0x00460992:
		this->DialogPanelDestructor(REG_values);
		break;
	case 0x51D91A:
		this->ManageGameTimerSkips(REG_values);
		break;
	case 0x51D9EC:
		this->CollectTimerStats(REG_values);
		break;
	case 0x0484CBF:
		this->ManageChangeGameSpeed(REG_values);
		break;
	case 0x0050B6C7:
		this->ManageRelicsCount(REG_values);
		break;
	case 0x0050B70C:
		this->ManageRuinsCount(REG_values);
		break;
	case 0x00503417:
		this->ManageCivIdControlAfterGameSettingsSelection(REG_values);
		break;
	case 0x005034AF:
		this->ManageGetPlayerNameDLLStringIdOffset(REG_values);
		break;
	case 0x0050386A:
		this->ManageGetPlayerNameDLLStringIdOffset2(REG_values);
		break;
	case 0x004F36F1:
		this->ManageAIFileSelectionForPlayer(REG_values);
		break;
	case 0x0048829F:
		this->ManageCivsInGameSettingsCombo(REG_values);
		break;
	case 0x00490961:
		this->ManageCivsInEditorCombo(REG_values);
		break;
	case 0x004FF852:
		this->GetInGameCustomCivName(REG_values);
		break;
	case 0x0050A574: // Both have almost same custom code and behaviour (we can still use returnAddress to distinguish the 2 cases)
	case 0x0050A5B9: // Both have almost same custom code and behaviour (we can still use returnAddress to distinguish the 2 cases)
		this->CheckPlayerCreationAtGameStartup(REG_values);
		break;
	case 0x0048D143:
		this->PickRandomCivForPlayer(REG_values);
		break;
	case 0x00501985:
		this->OnGameSettingsNotifyEvent(REG_values);
		break;
	case 0x00507F7A:
		this->OnGameInitDisableResearchesEvent(REG_values);
		break;
	case 0x0050BB35:
		this->OnGameInitAfterSetInitialAge(REG_values);
		break;
	case 0x00507FCB:
		this->OnScenarioInitPlayerBadInitialAgeApplication(REG_values);
		break;
	case 0x0045E8D7:
		this->ManageOnPlayerRemoveUnit(REG_values);
		break;
	case 0x004F2AAB:
		this->ManageOnPlayerAddUnit(REG_values);
		break;
	case 0x00472C2F:
		this->MapGen_elevationProportionCalc(REG_values);
		break;
	case 0x00046F5AD:
		this->MapGen_applyElevation(REG_values);
		break;
	case 0x004EFF14:
		this->PlayerCreateUnit_manageStatus(REG_values);
		break;
	case 0x00499143:
		this->AfterScenarioEditorCreation(REG_values);
		break;
	case 0x004926B5:
		this->ScenarioEditorChangeSelectedTerrain(REG_values);
		break;
	case 0x00451DD0:
		this->OnComboboxTransferCaptureToPreviousObject(REG_values);
		break;
	case 0x004F16BB:
		this->CheckPopulationCostWithLogistics(REG_values);
		break;
#ifdef GAMEVERSION_AOE10b
	case 0x0048EC06:
#endif
#ifdef GAMEVERSION_AOE10c
	case 0x004FC076:
#endif
#ifdef GAMEVERSION_ROR10b
	case 0x00503746:
#endif
#ifdef GAMEVERSION_ROR10c
	case 0x00501216:
#endif
		this->OnBeforeLoadEmpires_DAT(REG_values);
		break;
	case 0x0051C903:
		this->OnAfterLoadEmpires_DAT(REG_values);
		break;
	case 0x0051A3B9:
		this->EditorCheckForUnitPlacement(REG_values);
		break;
	case 0x0426D0F:
		this->HumanSpecific_onCapturableUnitSeen(REG_values);
		break;
	case 0x051A700:
		this->EntryPoint_UIUnitSelection(REG_values);
		break;
	case 0x042C335:
		this->RORDebugLogHandler(REG_values);
		break;
	case 0x040C245:
		this->CollectAOEDebugLogsInGame(REG_values);
		break;
	case 0x044DFC6:
	case 0x044E02F:
		this->GathererPathFindingReturnToDeposit(REG_values); // Applicable for both 2 calls
		break;
	case 0x004A78C8:
		this->ShowUnitShortcutNumbers(REG_values);
		break;
	case 0x00494C7F:
		this->InitPlayersCivInScenarioEditor(REG_values);
		break;
	case 0x004978B6:
		this->FixUnsupportedRomanTileSetInEditorIcons(REG_values);
		break;
	case 0x00481782:
		this->OnGameMainUiInitTilesetRelatedGraphics(REG_values);
		break;
	case 0x004F87FD:
		this->OnDisplayBuildingIconInUnitInfoZone(REG_values);
		break;
	case 0x004A440A:
		this->OnEditorSetBuildingIconInUnitInfoZone(REG_values);
		break;
	case 0x004824AF:
		this->DisplayCommandBarGetSlpInfoForBuilding(REG_values);
		break;
	case 0x004FAA36:
		this->WriteF11PopInfoText(REG_values);
		break;
	case 0x004C41BB:
		this->FixGetUnitStructInTargetSelectionLoop(REG_values);
		break;
	case 0x0041498B:
		this->FixUnitIdBugStuckAttackNoTarget(REG_values);
		break;
	case 0x00412F34:
		this->SetActivityTargetUnitIdBug(REG_values);
		break;
	case 0x004E64BD:
		this->FixActivityTargetUnitIdBug_retreatAfterShooting(REG_values);
		break;
	case 0x004E479B:
		this->FixActivityTargetUnitIdBug_case1F4(REG_values);
		break;
	case 0x0050CB6F:
		this->FixKillXCrashOnUnknownPlayer(REG_values);
		break;
	case 0x00483490:
		this->EntryPointOnAfterShowUnitCommandButtons(REG_values);
		break;
	case 0x00481415:
		this->EntryPointOnGameCommandButtonClick(REG_values);
		break;
	case 0x00483715:
		this->ROR_GetButtonInternalIndexFromDatBtnId(REG_values);
		break;
	case 0x004EBB16:
		this->FixPlayerNoTechTree_applyTech(REG_values);
		break;
	case 0x410E3C:
	case 0x411257:
		this->EntryPointAutoSearchTargetUnit(REG_values);
		break;
	case 0x4F8D92:
		this->EntryPointOnBuildingAttackInfoDisplay(REG_values);
		break;
#ifdef GAMEVERSION_AOE10b
	case 0x4E6D5C:
#endif
#ifdef GAMEVERSION_AOE10c
	case 0x417209:
#endif
#ifdef GAMEVERSION_ROR10b
	case 0x501AB5:
#endif
#ifdef GAMEVERSION_ROR10c
	case 0x004FF535:
#endif
		this->EntryPointOnGetLocalizedString(REG_values);
		break;
	case 0x004ADB99:
		this->AllowMultiUnitTypeInQueue(REG_values);
		break;
	case 0x004268FF:
		this->EntryPointOnAttackableUnitKilled(REG_values);
		break;
	case 0x004F6F9E:
		this->EntryPointOnHoverOnUnit(REG_values);
		break;
	case 0x005133A8:
		this->EntryPointShowInGameDefaultCursor_noUnitUnderMouse(REG_values);
		break;
	case 0x00411D46:
		this->EntryPointAfterActivityStop(REG_values);
		break;
	case 0x0040ACDF:
		this->EntryPointGetMostDislikedPlayerId(REG_values);
		break;
	case 0x004BFFCC:
		this->EntryPointInfAIGroupFindMainTarget(REG_values);
		break;
	case 0x004D3AC1:
		this->EntryPointTacAIHandleActiveGroupsBeforeLoop(REG_values);
		break;
	case 0x004D3B1D:
		this->EntryPointTacAIHandleActiveGroups(REG_values);
		break;
	case 0x004AFBE5:
		this->EntryPointBeforeUnitCreateActivity(REG_values);
		break;
	case 0x00501647:
		this->OverrideShowF5DebugInfo(REG_values);
		break;
	case 0x004AFB79:
		this->UnitDefProvidesRenewableResource(REG_values);
		break;
	case 0x004F8F0C:
		this->EntryPointDisplayBuildingInfoResource(REG_values);
		break;
	case 0x004F9B85:
		this->EntryPointRefreshTradeGoodsInUnitInfoZone(REG_values);
		break;
	case 0x004C1AC5:
		this->EntryPointInfAISearchTradeTargetElem(REG_values);
		break;
	case 0x004B6FAF:
		this->AddRelevantResourceValueWhenTrading(REG_values);
		break;
	case 0x004AFB95:
		this->ShouldPreserveOwnedResourceWhenKilledBy(REG_values);
		break;
	/*case 0x004E4721:
		this->VillagerActivityProcessNotify(REG_values);
		break;*/
	case 0x004BDFC0:
		this->IsTargetableResourceCallForInfAI(REG_values);
		break;
	case 0x4BBA15:
	case 0x4BDD3C:
	case 0x4C624C:
		this->SeeUnitIsArtefactOrResourceOrFlagOrCreatable(REG_values);
		break;
	/*case 0x4138A3:
		this->ActivityBaseProcessNotify(REG_values);
		break;*/
	case 0x413182:
		this->ActivityNearbyUnitDetectionDelayUpdate(REG_values);
		break;
	case 0x00426C91:
		this->OnAttackableUnitUpdateVisibility(REG_values);
		break;
	case 0x4B6F6D:
	case 0x4F909E:
		this->ShouldNotApplyPalmyraTradingBonus(REG_values);
		break;
	case 0x4D1BBA:
		this->TacAIHandleOneUpdateByType(REG_values);
		break;
	case 0x4B1AF7:
		this->ActionBuildUpdateFarmHack(REG_values);
		break;
	case 0x4BD7D6:
		this->OnAddInUnitMemoryList(REG_values);
		break;
	case 0x4199C5:
		this->IsGamePaused(REG_values);
		break;
	case 0x42A961:
		this->ExecMoveCmdUsingFormation(REG_values);
		break;
	default:
		break;
	}
#ifdef _DEBUG
	TemporaryEntryPoints(REG_values);
	long int thisEndTime = AOE_METHODS::TimeGetTime();
	long int timeSpent = thisEndTime - thisStartTime;
	long int prevValue = ROCKNROR::crInfo.longestTimes_ms[REG_values->return_address]; // returns 0 when not set yet (which is good)
	if (timeSpent > prevValue) {
		ROCKNROR::crInfo.longestTimes_ms[REG_values->return_address] = timeSpent;
	}
	int n = ROCKNROR::crInfo.executionCounts[REG_values->return_address]; // returns 0 when not set yet (which is good)
	ROCKNROR::crInfo.executionCounts[REG_values->return_address] = n + 1;
#endif;
}


#ifdef _DEBUG

// This procedure is only for testing purpose.
void RockNRorInstance::TemporaryEntryPoints(REG_BACKUP *REG_values) {
	if (REG_values->return_address == 0x46BA04) {
		unsigned long int *p = (unsigned long int *)0x7BFAE8;
		*p = REG_values->EAX_val; // Original instruction in 0046B9FF
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		long int bufSize = GetIntValueFromRORStack(REG_values, 0x1C); // arg3
		unsigned char *buf = (unsigned char *)REG_values->EDI_val - bufSize; // This calculation is always correct (getting arg2 in stack isn't)
		unsigned long int callerAddr = GetIntValueFromRORStack(REG_values, 0x10) - 5;
		//unsigned char *buf = (unsigned char *) GetIntValueFromRORStack(REG_values, 0x18); // arg2 is WRONG: may have been overwritten (like arg1)
		CR_DEBUG::WriteDebugLogForDeserializedData(callerAddr, buf, bufSize);
	}
}
#endif;



// Initializations done only once, at startup.
// ROR UI has not been initialized yet, DRS files are not loaded, etc.
void RockNRorInstance::OneShotInit() {
	traceMessageHandler.WriteMessageNoNotification(localizationHandler.GetTranslation(CRLANG_ID_DEBUG_INIT, "Debug message system initialized."));
	ROCKNROR::crInfo.configInfo.ReadXMLConfigFile(MOD_NAME "\\RockNRor.xml");
	ROCKNROR::crInfo.configInfo.ReadCivXMLConfigFile(MOD_NAME "\\RockNRor_civs.xml");
	ROCKNROR::crInfo.configInfo.ReadTilesetXMLConfigFile(MOD_NAME "\\RockNRor_tilesets.xml");
	ROCKNROR::crCommand.ResetLogFile();

	// Note: CheckEnabledFeatures writes to log file
	if (!ROCKNROR::crCommand.CheckEnabledFeatures() || !ROCKNROR::VIRTUAL_METHOD_HOOKS::InstallVirtualMethodsPatches()) {
		const char *msg = localizationHandler.GetTranslation(CRLANG_ID_WARN_MISSING_FEATURE, "WARNING: Some features are not enabled in game executable. See " MOD_NAME "\\" MOD_NAME ".log file.");
		if (ROCKNROR::crInfo.configInfo.showAlertOnMissingFeature) {
			MessageBoxA(0, msg, "ROR API", MB_ICONWARNING);
		}
		traceMessageHandler.WriteMessageNoNotification(msg);
	}
	
	// Custom treatments for initialization
	ROCKNROR::crCommand.OneShotInit();
}


// From 0x041AEF5
// To cancel the close operation, change return address to 0x41AF29 and set EAX to 0 BEFORE calling DS:[EAX+0x12C].
void RockNRorInstance::WMCloseMessageEntryPoint(REG_BACKUP *REG_values) {
	bool preventGameFromExiting = false;
	// Do custom treatments NOW or never ;)

	if (preventGameFromExiting) {
		REG_values->EAX_val = 0;
		ChangeReturnAddress(REG_values, 0x41AF29);
		return;
	}

	// Do this only if we choose to actually exit program !
	ROCKNROR::crInfo.rnrUIHelper->DestroyAllScreens(); // Make sure to close all custom screens before ROR tries to (important to keep our pointers correct)

	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		unsigned long int myEAX = REG_values->EAX_val;
		unsigned long int myECX = REG_values->ECX_val;
		assert(myEAX != NULL);
		assert(myECX != NULL);
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = (AOE_STRUCTURES::STRUCT_GAME_SETTINGS *)myECX;
		assert(settings->IsCheckSumValid());
		_asm {
			MOV EAX, myEAX;
			MOV ECX, myECX;
			CALL DWORD PTR DS:[EAX+0x12C];
			MOV myEAX, EAX; // result is used in ROR code after return...
		}
		REG_values->EAX_val = myEAX;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
}


// "From" 0x435F74
void RockNRorInstance::ReadTextFromChat(REG_BACKUP *REG_values) {
	// Execute the code we removed in original code (overwritten to call ROR_API)
	// 0x435F6F : CALL 0x480760
	// The called function needs ECX to be set
	// EAX = @text (remove push EAX from original code, it will be done here)
	long int myEAX = REG_values->EAX_val;
	long int myECX = REG_values->ECX_val;
	char *txt = (char *)myEAX;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		long int callAddr = 0x00480760;
		_asm {
			MOV EAX, myEAX;
			PUSH EAX;
			MOV ECX, myECX;
			CALL callAddr;
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	// Now do our own treatments
	if (IsMultiplayer() || ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }
	ROCKNROR::crCommand.HandleChatCommand(txt);
}


// Do some action each time a game starts, including "load game", scenario, campaign and also MP games.
// Warning: for scenarios, if there is an introduction screen, this method is called at that moment
// (game might not be displayed yet)
void RockNRorInstance::ActionOnGameStart(REG_BACKUP *REG_values) {
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		ROCKNROR::crCommand.OnGameStart();
	}
}


// This is called when launching a new game, for all game types: DM / RM /scenario / campaign / MP.
// When loading a saved game, it is called from another address (0x502E25) than standard call (0x51D509)
// When calling this method, game data has not been loaded yet.
// This method should only do data reset and similar stuff.
void RockNRorInstance::InitBeforeGameStart(REG_BACKUP *REG_values, bool isSavedGame) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		if (!isSavedGame) {
			REG_values->ECX_val = (long int)*ROR_gameSettings; // MOV ECX,DWORD PTR DS:[<gameSettings>]
		} else {
			REG_values->ECX_val = *(long int*)(REG_values->EBP_val + 0x3F4); //MOV ECX,DWORD PTR SS:[EBP+3F4]
		}
	}

	ROCKNROR::crCommand.OnNewGame(isSavedGame);
}

// This is called while scenarioInfo structure is read from a file (cf 0x506937)
// Scenario texts (including players strategies) havealready been read, but not other information, be careful.
// WARNING: this method can be called without a game to be started (in scenario menus, to preview scenario info and set options)
// WARNING: at this point, global->scenarioInfo link has NOT bee set ! Use ESI value instead.
void RockNRorInstance::InitScenarioInfoTextData(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		unsigned long int *p = (unsigned long int*)REG_values->ESI_val;
		ror_api_assert(REG_values, p != NULL);
		*p = 0x0054A3C8;
	}

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scenarioInfo = (AOE_STRUCTURES::STRUCT_SCENARIO_INFO *)REG_values->ESI_val;
		ror_api_assert(REG_values, scenarioInfo && scenarioInfo->IsCheckSumValid());

		ROCKNROR::TRIGGER::InitScenarioInfoTextData(scenarioInfo);
	}
}


// From 0x04F16BB
// Original (overwritten) code does a TEST AH, 1 + JNZ 004F16E4. We do this with a call to ChangeReturnAddress.
void RockNRorInstance::CheckPopulationCostWithLogistics(REG_BACKUP *REG_values) {
	long int resourceId = REG_values->ECX_val;
	//short int *costPointer = (short int *)REG_values->EDX_val;
	AOE_STRUCTURES::STRUCT_COST *currentCost = (AOE_STRUCTURES::STRUCT_COST *)(REG_values->EDX_val - 2);
	long int currentCostIndex = REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)(REG_values->EDX_val - 0x14A - (6 * currentCostIndex));
	ror_api_assert(REG_values, unitDef && unitDef->IsCheckSumValidForAUnitClass());
	float *myResources = (float *)REG_values->EDI_val;
	bool resourceValueIsOk = false;
	long testValue = REG_values->EAX_val & 0x0000FF00; // AH
	testValue = testValue >> 8;
	if (!(testValue & 1)) { // Corresponds to the TEST AH, 1...
		resourceValueIsOk = true;
	}
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		if (resourceValueIsOk) {
			ChangeReturnAddress(REG_values, 0x4F16E4);
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	// Custom treatments
	if (!ROCKNROR::crInfo.configInfo.fixLogisticsNoHouseBug) { return; }

	bool trainUnitWasAskedByHuman = false;
	// A bit dirty but there is no better way to do this. It seems "human-asked" train unit actions always come from the same method, let's take advantage of this:
	unsigned long int *pRORMethodReturnAddress = (unsigned long int *)(REG_values->ESP_val + 0x10);
	if ((*pRORMethodReturnAddress >= 0x4ADAB0) && (*pRORMethodReturnAddress <= 0x4ADD47)) {
		trainUnitWasAskedByHuman = true;
	}

	// When human player clicked to train unit, let him do: this will enqueue unit.
	// If there is no population headroom, it won't actually train but wait (queue unit). This *normal* behaviour works well.
	if (trainUnitWasAskedByHuman) {
		return;
	}

	// With logistics, barracks' units get a costValue=0 for populationHeadroom, because it's an int and the real value should be 0.5.
	// This causes erroneous calculations: AI can train infantry when its population headroom==0 (which should not be possible)
	// Is our unit impacted by logistics (value=0 for pop headroom cost) ?
	if ((currentCost->costAmount == 0) && (currentCost->costType == CST_RES_ORDER_POPULATION_HEADROOM) && (unitDef->unitType == GUT_TRAINABLE)) {
		if (
			// We test resource storages to know if this unit type "uses" population: we don't want to impact any unit that would use 0 population
			((unitDef->resourceStorageType_1 == CST_RES_ORDER_POPULATION_HEADROOM) && (unitDef->resourceStorageAmount_1 < 0)) ||
			((unitDef->resourceStorageType_2 == CST_RES_ORDER_POPULATION_HEADROOM) && (unitDef->resourceStorageAmount_3 < 0)) ||
			((unitDef->resourceStorageType_3 == CST_RES_ORDER_POPULATION_HEADROOM) && (unitDef->resourceStorageAmount_3 < 0))
			) {
			// Check current population headroom is >0 (possibly 0.5) before allowing starting to train a living unit that requires population headroom
			if (myResources[CST_RES_ORDER_POPULATION_HEADROOM] <= 0) {
				// We are in the specific case when AI trains infantry whereas it should not be allowed. Stop it by returning "false" in the check cost function;
				ChangeReturnAddress(REG_values, 0x4F1724);
			}
		}

	}
}


// 0x4B24F4
void RockNRorInstance::ComputeConversionResistance(REG_BACKUP *REG_values) {
	// Collect information
	AOE_STRUCTURES::STRUCT_ACTION_CONVERSION *convAction = (AOE_STRUCTURES::STRUCT_ACTION_CONVERSION *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *target = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	ror_api_assert(REG_values, convAction->IsCheckSumValid());
	ror_api_assert(REG_values, target->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, target == convAction->targetUnit);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actor = convAction->actor;
	long int myESP = REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_PLAYER *targetPlayer = target->ptrStructPlayer;
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = actor->ptrStructPlayer;
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *targetUnitDef = target->unitDefinition;
	
	bool allTechs = (actorPlayer && (actorPlayer->techTreeId < 0));
	allTechs |= ROCKNROR::crInfo.myGameObjects.doNotApplyHardcodedCivBonus;
	char civIdToUse = allTechs ? 0 : targetPlayer->civilizationId; // If all techs, do NOT apply hardcoded civ bonuses

	// Compute resistance
	float resistance = ROCKNROR::crInfo.GetConversionResistance(civIdToUse, targetUnitDef->unitAIType);

	// Save computed resistance to dedicated stack record
	_asm {
		MOV EAX, resistance;
		MOV ECX, myESP;
		MOV DS:[ECX], EAX;
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;

	// For monitoring
	ROCKNROR::crInfo.passiveConversionAttemptsCount[targetPlayer->playerId]++;
	ROCKNROR::crInfo.activeConversionAttemptsCount[actorPlayer->playerId]++;
}


// 0x04B2578 = address when randomness decided a conversion is successful = beginning of actual unit conversion process.
void RockNRorInstance::OnSuccessfulConversion(REG_BACKUP *REG_values) {
	// Do overwritten instructions
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EBX_val = (long int)(*ROR_gameSettings);
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Custom treatments
	AOE_STRUCTURES::STRUCT_ACTION_CONVERSION *convAction = (AOE_STRUCTURES::STRUCT_ACTION_CONVERSION *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	ror_api_assert(REG_values, convAction->IsCheckSumValid());
	ror_api_assert(REG_values, targetUnit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, targetUnit == convAction->targetUnit);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actor = convAction->actor;
	ror_api_assert(REG_values, actor != NULL);
	ror_api_assert(REG_values, actor->IsCheckSumValidForAUnitClass());
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = actor->ptrStructPlayer; // Player that HAS converted a unit
	ror_api_assert(REG_values, actorPlayer != NULL);
	ror_api_assert(REG_values, actorPlayer->IsCheckSumValid());

	// Manage functional impacts / fixes about unit conversion
	ROCKNROR::crCommand.OnUnitChangeOwner_fixes((AOE_STRUCTURES::STRUCT_UNIT_BASE*)targetUnit, actorPlayer);

	// Conversions Monitoring:
	AOE_STRUCTURES::STRUCT_PLAYER *targetPlayer = targetUnit->ptrStructPlayer; // The victim (if unit has been converted)
	assert(targetPlayer != NULL);
	assert(targetPlayer->IsCheckSumValid());
	ROCKNROR::crInfo.passiveConversionSuccessfulAttemptsCount[targetPlayer->playerId]++;
	ROCKNROR::crInfo.activeConversionSuccessfulAttemptsCount[actorPlayer->playerId]++;

}


// 0x4F2AAB
// This is called from player.AddUnit method
void RockNRorInstance::ManageOnPlayerAddUnit(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_PLAYER *player = (AOE_STRUCTURES::STRUCT_PLAYER *)REG_values->ECX_val;
	if (!player) { return; }
	ror_api_assert(REG_values, player->IsCheckSumValid());
	if (!player->IsCheckSumValid()) {
		return;
	}
	long int isTempUnit = REG_values->EAX_val; // = arg3
	// +8: to compensate the 2 PUSH (EBX, ESI) that are done before calling ROR_API
	// This way, ESP[0]=ROR_return_address, ESP[i]=arg[i] (0 < i < 4)
	long int *myESP = (long int *)(REG_values->ESP_val + 8);
	long int isNotCreatable = myESP[2];
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)myESP[1];
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// Original overwritten code to do now: MOV ESI,ECX and MOV ECX,DWORD PTR SS:[ESP+10] (get arg2)
		REG_values->ESI_val = REG_values->ECX_val;
		REG_values->ECX_val = isNotCreatable; // arg2
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Now custom treatments
	ROCKNROR::crCommand.OnPlayerAddUnitCustomTreatments(player, unit, (isTempUnit!=0), (isNotCreatable!=0));
}


// 0x472C25
// Customize "pseudo-random" value that provides elevated terrain proportion in generated map.
// In fact the number is not random, it only depends on map seed due to pseudo-random computation algorithm.
// Warning: game code does a "*5" multiplication for Hills map type afterwards.
void RockNRorInstance::MapGen_elevationProportionCalc(REG_BACKUP *REG_values) {
	long int elevationBaseCalcValue;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		_asm {
			MOV EAX, 0x52605D;
			CALL EAX;
			MOV elevationBaseCalcValue, EAX;
		}
		REG_values->EAX_val = elevationBaseCalcValue;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// Custom treatments
	if (!ROCKNROR::crInfo.configInfo.useMapGenerationCustomElevationCalculation) { return; }
	long int *myECX = (long int *)REG_values->EBX_val;
	long int mapType = myECX[0x34 / 4];
	assert(mapType < AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_MAP_TYPES_COUNT);
	assert(mapType >= 0);
	if ((mapType < 0) || (mapType >= AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_MAP_TYPES_COUNT)) { return; }
	float tmpValue = ((float)elevationBaseCalcValue) * ROCKNROR::crInfo.configInfo.mapGenerationCustomElevationFactor[mapType];
	if (mapType == AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_HILLS) {
		tmpValue = tmpValue / 5; // Because of hardcoded *5 in game code that will be applied afterwards.
	}
	REG_values->EAX_val = (long int)tmpValue;
}


// 0046F5A8
// Replace an ugly buggy method to add compatibility with non-square maps.
// Moreover, it is possible this improves elevation management because there are some "uncertain" algorithms with X positions.
void RockNRorInstance::MapGen_applyElevation(REG_BACKUP *REG_values) {
	if (REG_values->fixesForGameEXECompatibilityAreDone) {
		return;
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Collect (and check) arguments
	long int posY = REG_values->EDX_val;
	long int posX = REG_values->EAX_val;
	//long int dist = *(long int*)(REG_values->ESP_val + 0x0C);
	long int dist = GetIntValueFromRORStack(REG_values, 0x0C);
	ror_api_assert(REG_values, dist > 0);
	ror_api_assert(REG_values, posX >= 0);
	ror_api_assert(REG_values, posY >= 0);
	if ((dist <= 0) || (posX < 0) || (posY < 0)) { return; }
	AOE_STRUCTURES::STRUCT_RMM_ELEVATION_GENERATOR *elevInfo = (AOE_STRUCTURES::STRUCT_RMM_ELEVATION_GENERATOR*) REG_values->ECX_val;
	ROCKNROR::crCommand.Fixed_MapGen_applyElevation(posX, posY, dist, elevInfo);
}


// 0x0045E8D7
// This is called from player.RemoveUnit method. Note: player.RemoveUnit is called BY unit.destructor.
// Warning: this method is called in many situations, game might NOT be running.
void RockNRorInstance::ManageOnPlayerRemoveUnit(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_PLAYER *player = (AOE_STRUCTURES::STRUCT_PLAYER *)REG_values->ECX_val;
	if (!player) { return; }
	ror_api_assert(REG_values, player->IsCheckSumValid());
	if (!player->IsCheckSumValid()) {
		return;
	}
	// +8: to compensate the 2 PUSH (ESI, EDI) we already did before calling ROR_API.
	// This way, ESP[0]=ROR_return_address, ESP[i]=arg[i] (0 < i < 4)
	long int *myESP = (long int *)(REG_values->ESP_val + 8);
	long int isNotCreatable = myESP[2];
	long int isTempUnit = myESP[3];
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)myESP[1];
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// Original overwritten code to do now: MOV EAX,DWORD PTR SS:[ESP+8] and MOV ESI,ECX
		REG_values->ESI_val = REG_values->ECX_val;
		REG_values->EAX_val = isNotCreatable;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	ROCKNROR::crCommand.OnPlayerRemoveUnit(player, unit, isTempUnit != 0, isNotCreatable != 0);
}


// Fills strategy element's unitId when construction begins (while in progress)
// In method buildingUnit.initialize (0x4AC8A0)
void RockNRorInstance::FixBuildingStratElemUnitID(REG_BACKUP *REG_values) {
	// TODO: separate compatibility code from "functional" code, and use ROCKNROR::crInfo.configInfo.doNotApplyFixes parameter.
	AOE_STRUCTURES::STRUCT_PLAYER *p = (AOE_STRUCTURES::STRUCT_PLAYER*) REG_values->EDI_val;
	ror_api_assert(REG_values, p != NULL);
	AOE_STRUCTURES::STRUCT_AI *ai = p->ptrAIStruct;
	if (ai) {
		AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = &ai->structBuildAI;
		ror_api_assert(REG_values, buildAI != NULL);
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->ESI_val;
		ror_api_assert(REG_values, unit != NULL);
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unit->unitDefinition;
		ror_api_assert(REG_values, unitDef != NULL);
		long int unitId = unit->unitInstanceId;
		char unitStatus = unit->unitStatus;

		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *p_fakeStratElem = &buildAI->fakeFirstStrategyElement;
		ror_api_assert(REG_values, p_fakeStratElem != NULL);
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *p_currentStratElem = p_fakeStratElem->next;
		short int unitDATID1 = unitDef->DAT_ID1;

		while (p_currentStratElem != p_fakeStratElem) {
			ror_api_assert(REG_values, p_currentStratElem != NULL);
			if ((p_currentStratElem->unitInstanceId == -1) &&
				(p_currentStratElem->elementType == AIUCBuilding) &&
				(p_currentStratElem->inProgressCount == 1) &&
				(p_currentStratElem->unitDAT_ID == unitDATID1) &&
				(unitStatus == 0) // unit status=0 (being built)
				) {
				p_currentStratElem->unitInstanceId = unitId; // Assign unit instance ID
			}
			p_currentStratElem = p_currentStratElem->next;
		}
	}
	// and do overwritten original code...
	long int myECX = REG_values->ECX_val;
	long int myEDX = REG_values->EDX_val;
	_asm {
		MOV ECX, myECX
		MOV EDX, myEDX
		MOV DX,WORD PTR DS:[ECX+0x4A]
		MOV ECX,DWORD PTR DS:[ECX+0x3C]
		MOV myECX, ECX
		MOV myEDX, EDX
	}
	REG_values->ECX_val = myECX;
	REG_values->EDX_val = myEDX;
}


// From 0x4B8C74 (in buildAI.resetStratElemForUnitId)
// Test if strategy element is alive for reset purpose (like original code). In addition, resets unitInstanceId if the strategy element is "in progress" (to be detected by next custom tactical update).
// For the algorithm to work well, requires also "FixUnitIdForInProgressBuilding", "ManageTacticalAIUpdate"
// Performance info: The loop first tests strategy element's unitInstanceId, so ROR_API is only called once in the loop, which is a good thing.
void RockNRorInstance::OverloadIsStratElemUnitAlive_ResetElement(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *currentElement = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *) REG_values->ECX_val;
	ror_api_assert(REG_values, currentElement != NULL);
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = (AOE_STRUCTURES::STRUCT_BUILD_AI *) REG_values->EDI_val;
	ror_api_assert(REG_values, buildAI != NULL);
	ror_api_assert(REG_values, buildAI->IsCheckSumValid());

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// Run technical fixes (even in improve AI is disabled)
		ROCKNROR::STRATEGY::CheckStratElemAliveForReset(buildAI, currentElement);
	}

	// This corresponds to original code we overloaded to call ROR_API: return alive_count. If >0, the strategy element will be reset.
	REG_values->EAX_val = currentElement->aliveCount;
}


// 0x00408F92
// Called at the end of buildAI.AddElementsToStrategy(...) ROR method.
// Be careful as it can be called by buildAI.addDynamicStrategyElements(...) which also triggers a ROR_API entry point (afterwards)
void RockNRorInstance::AfterAddElementInStrategy(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// Do overwritten ROR code:
		REG_values->EAX_val = 1;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Custom treatments

	// Get arguments + run checks
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = (AOE_STRUCTURES::STRUCT_BUILD_AI *) REG_values->EBP_val;
	// posToAdd (EDI) is the element after which the method adds elements.
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *posToAdd = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *)REG_values->EDI_val;
	assert(buildAI != NULL);
	assert(buildAI->IsCheckSumValid());
	assert(posToAdd != NULL);
	assert(posToAdd->IsCheckSumValid());
	if (!buildAI || !buildAI->IsCheckSumValid()) { return; }
	if (!posToAdd || !posToAdd->IsCheckSumValid()) { return; }
	long int *args = (long int *)(REG_values->ESP_val + 0x64);
	// WARNING: do not use arg1, it is invalid because it is overwritten to be used as a local var by ROR method.
	long int unitDATID = args[2];
	long int nbToCreate = args[3]; // Yes, this method can create several at once, be careful
	long int actorDATID = args[4]; // check if it is correct or overwritten in method?
	long int retrains = args[5];
	long int positionCounter = args[6]; // position in strategy list, NOT elemId !
	
	ROCKNROR::crCommand.AfterAddElementInStrategy(buildAI, posToAdd, nbToCreate);
}


// 0x004B7D84
// Fix a bug in buildAI.addDynamicStrategyElements()
// Warships are never added because the code calls the wrong method (add research instead of add unit)
void RockNRorInstance::FixAutoBuildWarships_addStratElem(REG_BACKUP *REG_values) {
	if (REG_values->fixesForGameEXECompatibilityAreDone) {
		return;
	}
	// TODO: separate compatibility code from "functional" code and use ROCKNROR::crInfo.configInfo.doNotApplyFixes

	long int insertPosition = REG_values->EDI_val;
	long int unitDAT_ID = 0x13; // Also hardcoded in EXE
	long int retrains = -1;
	unsigned long int myESI = REG_values->ESI_val;
	long int numberOfItemsToAdd = REG_values->EBP_val; // Warning: this is a loop, so this value decreases at each loop
	long int strategyElementsNumberBetweenTwoAdds = REG_values->EBX_val;
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = (AOE_STRUCTURES::STRUCT_BUILD_AI *) myESI;
	assert(buildAI != NULL);
	assert(buildAI->IsCheckSumValid());
	//AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	//assert(settings != NULL);
	//if (!settings || !buildAI) { return; }
	if (!buildAI) { return; }

	//if ((!settings->isCampaign) && (!settings->isScenario) && (!settings->isMultiplayer) && (insertPosition < 8)) {
	if (insertPosition < 8) {
		// insertPosition seems to be very low. Training warships at the very beginning of strategy might get AI stuck because it's not available yet AND it won't create villagers/other required items
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = buildAI->fakeFirstStrategyElement.previous;
		long int minPosition = -1;
		long int maxPosition = -1; // Counter of last element
		if (stratElem) {
			maxPosition = stratElem->elemId;
		}
		// Try first the best starting location to add warships
		if (minPosition < 0) { // If possible, wait market has been built (so that farms are unlocked first)
			minPosition = STRATEGY::FindElementInStrategy(buildAI->mainAI->ptrStructPlayer, AOE_CONST_FUNC::AIUCBuilding, CST_UNITID_MARKET);
		}
		if (minPosition < 0) { // Try tool age...
			minPosition = STRATEGY::FindElementInStrategy(buildAI->mainAI->ptrStructPlayer, AOE_CONST_FUNC::AIUCCritical, CST_RSID_TOOL_AGE);
		}
		if (minPosition < 0) { // Try tool age (if non-critical)
			minPosition = STRATEGY::FindElementInStrategy(buildAI->mainAI->ptrStructPlayer, AOE_CONST_FUNC::AIUCTech, CST_RSID_TOOL_AGE);
		}
		if (minPosition < 0) { // Dock... ?
			minPosition = STRATEGY::FindElementInStrategy(buildAI->mainAI->ptrStructPlayer, AOE_CONST_FUNC::AIUCBuilding, CST_UNITID_DOCK);
		}

		// If local variables are consistent, change start / end positions to add warships AND distance between them in strategy.
		if ((minPosition < maxPosition) && (maxPosition > -1) && (minPosition > -1)) {
			if (minPosition + 5 < maxPosition) { minPosition = minPosition + 5; } // insert after the reference, and leave some other items between (warships are not urgent at this strategy point)

			insertPosition = minPosition;
			strategyElementsNumberBetweenTwoAdds = (maxPosition - minPosition) / numberOfItemsToAdd;

			// Update registers for next loops !
			REG_values->EDI_val = insertPosition; // Update value (for game loop)
			//REG_values->EBP_val = numberOfItemsToAdd; // unchanged
			REG_values->EBX_val = strategyElementsNumberBetweenTwoAdds;
		}
	}

	
	_asm {
		PUSH insertPosition;
		PUSH retrains;
		PUSH unitDAT_ID;
		MOV ECX, myESI;
		MOV EAX, 0x004B9050; // buildAI.addLivingElementInStrategy(DATID, retrains, insertPosition)
		CALL EAX;
		// Result (EAX) is unused
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;

}


// 0x004B7E1E
// Returns strategy element DAT_ID. Returns Town center ID instead of house ID because only TC is supported in algorithm (count the 4-pop housage).
void RockNRorInstance::FixAutoBuildHouse_countHouse(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *) REG_values->ECX_val;
	REG_values->EAX_val = stratElem->unitDAT_ID;
	
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// Workaround for houses: return TC's DAT_ID so that it will count a 4-population housage.
		if (stratElem->unitDAT_ID == CST_UNITID_HOUSE) {
			REG_values->EAX_val = CST_UNITID_FORUM;
		}
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
}


// From 004B7E5E. (Always) Changes return address to 0x4B7E7E (continue loop - normal case) or 0x4B7E8C (exit loop)
void RockNRorInstance::FixAutoBuildHouse_maxPopSignedCharLimitation(REG_BACKUP *REG_values) {
	// Get context information
	long int hostedPopulationAtCurrentPoint = REG_values->EBX_val;
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = (AOE_STRUCTURES::STRUCT_BUILD_AI *)REG_values->ESI_val;
	ror_api_assert(REG_values, (buildAI != NULL) && buildAI->IsCheckSumValid());
	ror_api_assert(REG_values, (buildAI->mainAI != NULL) && buildAI->mainAI->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = &buildAI->mainAI->structTacAI;
	ror_api_assert(REG_values, tacAI->IsCheckSumValid());
	long int houseOverage = tacAI->SNNumber[SNHouseOverage]; // cf 0x4B7E72
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (hostedPopulationAtCurrentPoint > houseOverage + ROCKNROR::crInfo.configInfo.singlePlayerMaxPopulation) {
		// Exit loop (enough houses)
		// This can only happen if initial strategy contains (enough or too many) houses. "Normal" exit loop is when end of strategy is reached.
		ChangeReturnAddress(REG_values, 0x4B7E8C);
		return;
	} else {
		// Continue loop (not enough houses)
		ChangeReturnAddress(REG_values, 0x4B7E7E); // Skip "old" bugged comparison (not supporting higher populations than 127)
		return;
	}
}


// 0x004B8017
// This is called at the end of buildAI.addDynamicStrategyElements(). (which adds houses, docks, boats, setgather%)
// This allow modifying/fixing what has just been added dynamically
void RockNRorInstance::AfterAddDynamicStratElems(REG_BACKUP *REG_values) {
	unsigned long int myESP = REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = (AOE_STRUCTURES::STRUCT_BUILD_AI *) REG_values->ESI_val;
	unsigned long int myECX;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		_asm {
			MOV ECX, DWORD PTR SS:[ESP+0xAC];
			MOV myECX, ECX;
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	// Custom treatments: run various fixes on added strategy elements
	STRATEGY::FixAutoBuildStrategyElements(buildAI);
}


// From 0051E0DC. Called just before starting to actually save a game to a gmx file.
void RockNRorInstance::EntryPoint_OnBeforeSaveGame(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->ECX_val = (long int)*ROR_gameSettings;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	char *filename = (char*)REG_values->EAX_val;
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = (AOE_STRUCTURES::STRUCT_GAME_GLOBAL *)REG_values->EDI_val;
	ror_api_assert(REG_values, global != NULL);
	ror_api_assert(REG_values, global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }
	
	// Custom code
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Update triggers data before saving, 
	// so that when loading game we will have relevant trigger information (which have already been executed, etc)
	ROCKNROR::TRIGGER::WriteTriggersFromInternalToGameData(true);
}


// This method replaces the "panic mode" management (add units in strategy when attacked and weak)
// From 0x4E22B0=tacAI.DoPanicModeIfNeeded(enemyPlayerId), called *every time a unit is attacked* (from tacAI.reactToEvent(myUnitId, arg2_unitId, eventId(201 here), arg4, arg5, arg6))
// WARNING: apparently, when a *building* is killed (final attack that makes HP<1), this is not triggered. However killing living units DOES trigger this.
// WARNING: This method may change return address (only when forcing usage of ROR panic mode algorithm - not recommended)
// MAKE SURE TacAIOnUnitAttacked entry point is active too
void RockNRorInstance::ManagePanicMode(REG_BACKUP *REG_values) {
	// Collect context information/variables
	long int timeSinceLastPanicMode_s = REG_values->EDX_val; // in seconds
	long int currentGameTime_ms = REG_values->ESI_val; // in ms
	AOE_STRUCTURES::STRUCT_AI *mainAI = (AOE_STRUCTURES::STRUCT_AI *)REG_values->ECX_val;
	assert(mainAI != NULL);
	assert(mainAI->IsCheckSumValid());
	if (!mainAI || (!mainAI->IsCheckSumValid())) { return; }
	short int enemyPlayerId = (short int)GetIntValueFromRORStack(REG_values, 0x98);

	REG_values->fixesForGameEXECompatibilityAreDone = true;

	if (ROCKNROR::ShouldUseOriginalPanicModeMethod() || ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// if it returns true, it means we want to force usage of ROR *original* (crappy) panic mode code.
		// So let's do it by forcing return address (skip the JMP 004E254F)
		// However, we first do the check on "last execution" (this test has been overriden by binary change, and is not done anymore in ROR code).
		if (timeSinceLastPanicMode_s > 180) { // 0xB4
			// Delay has been reached, panic mode method can be run
			ChangeReturnAddress(REG_values, 0x4E2307);
		}
		// Delay till next execution is not finished. Do nothing right now;
	}
	// Other cases : use RockNRor (recommended)
	// There is nothing to do here, there is another entry point just after (calls TacAIOnUnitAttacked)
}


// From 0x4D7ADD. tacAI.reactToEvent(...) for event 0x201.
// This is called each time one of "my" units is attacked (even by gaia).
// MAKE SURE ManagePanicMode entry point is active too
void RockNRorInstance::TacAIOnUnitAttacked(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *myUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EBP_val;
	long int myUnitId = REG_values->EBX_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	long int enemyPlayerId = REG_values->ECX_val;
	AOE_STRUCTURES::STRUCT_PLAYER *enemyPlayer = (AOE_STRUCTURES::STRUCT_PLAYER *)REG_values->EAX_val;
	// Checks
	ror_api_assert(REG_values, tacAI && tacAI->IsCheckSumValid());
	ror_api_assert(REG_values, myUnit && myUnit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, enemyUnit && enemyUnit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, enemyPlayer && enemyPlayer->IsCheckSumValid());
	ror_api_assert(REG_values, enemyUnit->ptrStructPlayer == enemyPlayer);
	ror_api_assert(REG_values, enemyPlayer->playerId == enemyPlayerId);
	ror_api_assert(REG_values, myUnit->unitInstanceId == myUnitId);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EDX_val = tacAI->attacksByPlayerCount[enemyPlayerId]; // 0x4D7AE1
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Custom treatments

	// ROR original method for panic mode is deactivated: call RockNRor treatments.
	// Make sure to remain consistent about this !
	bool usedRorPanicModeMethod = ROCKNROR::ShouldUseOriginalPanicModeMethod();

	// Run custom AI treatments for "being attacked" event
	// ...and runs custom panic mode treatments *if usedRorPanicModeMethod=true*.
	if (ROCKNROR::IsImproveAIEnabled(tacAI->commonAIObject.playerId)) {
		CUSTOM_AI::customAIHandler.OnUnitAttacked(tacAI, myUnit, enemyUnit, usedRorPanicModeMethod);
	}
}


// From 004C2538
// This ROR method (0x4C1EF0) initializes map build like data to 0x14 
// and disables (0xFF) tiles that are occupied by buildings (and some nearby tiles)
// This replaces a small portion of code that is only executed for building new houses (DATID=0x46)
// This method is NO LONGER required. ManageCityMapLikeComputationCall1 will handle houses better than this.
void RockNRorInstance::ManageCityPlanHouseDistanceFromBuildings(REG_BACKUP *REG_values) {
	if (REG_values->fixesForGameEXECompatibilityAreDone) {
		return;
	}
	// Note: not checking ROCKNROR::crInfo.configInfo.doNotApplyFixes here
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *otherBuilding = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) REG_values->EDI_val;
	ror_api_assert(REG_values, otherBuilding != NULL);
	long int myESP = REG_values->ESP_val;
	// At this point EBP and EAX both are otherBuilding's Y position. We have the choice to use one of them or re-get from EDI. Doesn't change a lot.
	long int *minPosY = (long int *)(myESP + 0x28); // ESP+28 is minPosY
	long int *minPosX = (long int *)(myESP + 0x2C); // ESP+2C is minPosX
	long int *maxPosY = (long int *)(myESP + 0x30); // ESP+30 is maxPosY
	long int maxPosXValue; // Will be written in ESP+34, but later
	// MinPosX will be written in stack from (returned) EAX value in ROR's code.

	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *defOtherBuilding = otherBuilding->unitDefinition;
	ror_api_assert(REG_values, defOtherBuilding != NULL);
	// Get relevant distance value according to "reference building" type (DAT_ID)
	float distanceValue = 3; // From original code
	/*float distanceValue = ROCKNROR::crInfo.configInfo.cityPlanHouseDistanceFromOtherBld;
	if (defOtherBuilding->DAT_ID1 == CST_UNITID_FORUM) {
		distanceValue = ROCKNROR::crInfo.configInfo.cityPlanHouseDistanceFromTownCenter;
	}
	if ((defOtherBuilding->DAT_ID1 == CST_UNITID_GRANARY) ||
		(defOtherBuilding->DAT_ID1 == CST_UNITID_STORAGE_PIT)) {
		distanceValue = ROCKNROR::crInfo.configInfo.cityPlanHouseDistanceFromStorageBld;
	}*/

	// Do like original code but with a custom value (distanceValue)
	*minPosY = (long int)(otherBuilding->positionY - distanceValue);
	*minPosX = (long int)(otherBuilding->positionX - distanceValue);
	*maxPosY = (long int)(otherBuilding->positionY + distanceValue);
	maxPosXValue = (long int)(otherBuilding->positionX + distanceValue);// maxPosX is written later in stack using EAX value.

	// Registers are used again after we return, make sure they have the correct value
	// EBP : posY(min), EBX=posX(max), EAX=posX(max)
	REG_values->EAX_val = maxPosXValue;
	REG_values->EBX_val = (long int)(*minPosX);
	REG_values->EBP_val = (long int)(*minPosY);
}

	

// From 0x4BE8D8 call
// Takes other building into account in temp map like info for city plan evaluation
void RockNRorInstance::ManageCityMapLikeComputationCall1(REG_BACKUP *REG_values) {
	long int *myESP = (long int *) REG_values->ESP_val;
	long int myECX = GetIntValueFromRORStack(REG_values, 0x1C);
	long int placementType = REG_values->EBX_val; // arg1
	long int DAT_ID = REG_values->EDX_val; // arg2
	long int *TCPositionYX = (long int *)GetIntValueFromRORStack(REG_values, 0x0);
	long int *TownLowestPositionYX = (long int *)GetIntValueFromRORStack(REG_values, 0x4);
	long int *TownHighestPositionYX = (long int*)GetIntValueFromRORStack(REG_values, 0x8);
	
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// Do original code's call to fill temp map info
		REG_values->ECX_val = myECX; // Replaces "MOV ECX,DWORD PTR SS:[ESP+1C]" from original code 
		_asm {
			MOV ECX, myECX;
			PUSH TownHighestPositionYX;
			PUSH TownLowestPositionYX;
			PUSH TCPositionYX;
			PUSH DAT_ID;
			PUSH placementType;
			MOV EAX, 0x004C1EF0; // infAI.fillTempMapLikeLocationForNewBuilding(placementType,DAT_ID,ptrPosTC,ptrPosTC-maxTownSize,ptrPosTC+maxTownSize)
			CALL EAX;
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	//AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = *(AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT **) (myESP + 0x1BC / 4);
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *)GetIntValueFromRORStack(REG_values, 0x1BC);
	ror_api_assert(REG_values, stratElem != NULL);
	ror_api_assert(REG_values, stratElem->IsCheckSumValid());
	ror_api_assert(REG_values, stratElem->unitDAT_ID == DAT_ID);
	//AOE_STRUCTURES::STRUCT_INF_AI *infAI = *(AOE_STRUCTURES::STRUCT_INF_AI **) (myESP + 7);
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *) GetIntValueFromRORStack(REG_values, 0x1C);
	ror_api_assert(REG_values, infAI != NULL);

	CITY_PLAN::ManageCityPlanOtherBuildingsImpact(infAI, stratElem, (AOE_STRUCTURES::STRUCT_POSITION_INFO*)TCPositionYX);
}


// From 0x4C1CAF call
// Does nothing but normal behaviour yet...
void RockNRorInstance::ManageCityMapLikeComputationCall2(REG_BACKUP *REG_values) {
	long int *myESP = (long int *)REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *) REG_values->EBP_val;
	long int *TCPositionYX = *((long int **)myESP + 1);
	long int *TownLowestPositionYX = *((long int **)myESP + 2);
	long int *TownHighestPositionYX = *((long int **)myESP + 3);

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// Sometimes AI has not correctly been initialized (when game was saved too quickly after game start ?).
		// Bad data cause a crash in the call below, we can avoid this by returning without doing the call.
		if ((infAI->XMapSize < 0) || (infAI->YMapSize < 0)) {
			ror_api_assert(REG_values, infAI->XMapSize >= 0); // For debugging purpose. Not an critical error.
			ror_api_assert(REG_values, infAI->YMapSize >= 0); // For debugging purpose. Not an critical error.
			std::string msg = "Missing mapSize data in InfAI for player #";
			msg += std::to_string(infAI->commonAIObject.playerId);
			msg += "(";
			msg += infAI->commonAIObject.playerName;
			msg += ").";
			traceMessageHandler.WriteMessage(msg);
			AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
			if (global && global->IsCheckSumValid()) {
				AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
				if (mapInfo && mapInfo->IsCheckSumValid()) {
					infAI->XMapSize = mapInfo->mapArraySizeX;
					infAI->YMapSize = mapInfo->mapArraySizeY;
					traceMessageHandler.WriteMessage("RockNRor just fixed missing map dimensions in AI data.");
				}
			}
			return;
		}
	}

	// Do original code's call to fill temp map info
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		_asm {
			MOV ECX, infAI; // in original code: MOV ECX,EBP
			PUSH TownHighestPositionYX;
			PUSH TownLowestPositionYX;
			PUSH TCPositionYX;
			PUSH -1; // arg2=DAT_ID
			PUSH 0; // arg1=placementType
			MOV EAX, 0x004C1EF0;
			CALL EAX;
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
}


// This manages the like value to add in a bush's neighborhood when trying to build a granary.
void RockNRorInstance::ManageCityMapLikeValueAroundBushes(REG_BACKUP *REG_values) {
	long int *likeValueToAdd = ((long int *)REG_values->ESP_val);

	// Original overwritten code "MOV EAX,DWORD PTR SS:[ESP+0x34]"
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		long int *savedMinPosX = (long int *)(REG_values->ESP_val + 0x38); // 0x38 because there is now 1 more PUSH in stack (with custom code)
		REG_values->EAX_val = *savedMinPosX;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	// Set the "like value". A hardcoded value has been push'ed, now update with the real value we want to use
	*likeValueToAdd = ROCKNROR::crInfo.configInfo.cityPlanBerryBushWeightForGranary;
}


// From 004C27D1 : this code is dedicated to farms placement mode.
// Changing return address to 0x4C2A86 will make the loop continue, ignoring current (existing) building
// REG_values->EAX_val (both input and output in this method) is influence distance for current existing building. We can set it to a custom value if we want to.
void RockNRorInstance::ManageCityMapLikeValueFarmPlacement(REG_BACKUP *REG_values) {
	long int placementMode = REG_values->EBX_val;
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *) REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unitExistingBld_base = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefExistingBld_base = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)REG_values->ECX_val;
	long int RORBldLoopIndex = REG_values->EBP_val;
	// Checks
	ror_api_assert(REG_values, placementMode == 8);
	ror_api_assert(REG_values, infAI && infAI->IsCheckSumValid());
	ror_api_assert(REG_values, unitExistingBld_base && unitExistingBld_base->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, unitDefExistingBld_base && unitDefExistingBld_base->IsCheckSumValidForAUnitClass());

	// This replaces "overwritten" original ROR code (for compatibility)
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EDX_val = *(long int*)(REG_values->ESP_val + 0x28);;
		REG_values->EDI_val = REG_values->EAX_val;
	}

	long int existingBldInfluenceZone = REG_values->EAX_val;
	bool skipThisBuilding = false;
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		CITY_PLAN::FixCityPlanFarmPlacement(unitExistingBld_base, existingBldInfluenceZone, skipThisBuilding);
		REG_values->EAX_val = existingBldInfluenceZone;
	}

	if (skipThisBuilding) {
		ChangeReturnAddress(REG_values, 0x4C2A86);
		return;
	}
}


void RockNRorInstance::OverloadSetInProgress(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *) REG_values->EBX_val;
	ror_api_assert(REG_values, stratElem != NULL);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actor = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) REG_values->EDI_val;
	ror_api_assert(REG_values, actor != NULL);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		stratElem->inProgressCount = 1; // What original (replaced) code used to do
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	if (stratElem->elementType != AOE_CONST_FUNC::TAIUnitClass::AIUCLivingUnit) { return; }
	if (!actor->unitDefinition || (actor->unitDefinition->unitType != AOE_CONST_FUNC::GUT_BUILDING)) { return; }
	// Here: actor is a building and strategy element is a unit to train. Set temporarily unitInstanceId to actor's id or we can't find where it is being trained.
	stratElem->unitInstanceId = actor->unitInstanceId;
}


void RockNRorInstance::OverloadResetInProgress(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *) REG_values->ECX_val;
	ror_api_assert(REG_values, stratElem != NULL);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		stratElem->inProgressCount = 0; // What original (replaced) code used to do
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	stratElem->unitInstanceId = -1; // Added: reset instanceId (RockNRor uses it while in progress to retrieve actor)
}


// This overloads a part of the unit destructor that removes references to deleted unit from strategy and infAI.
// The goal is to add a loop to cancel "in progress" units that are trained in the building, if any.
void RockNRorInstance::OverloadResetUnitInAI(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_AI *mainAI = (AOE_STRUCTURES::STRUCT_AI *) REG_values->ECX_val;
	ror_api_assert(REG_values, mainAI != NULL); // mainAI must not be NULL because of TEST ECX,ECX on 4F2B35
	long int unitId_toReset = REG_values->EAX_val;
	long int myESP = REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit_toReset = *(AOE_STRUCTURES::STRUCT_UNIT_BASE **) (myESP + 0x0C);
	ror_api_assert(REG_values, unit_toReset != NULL);
	// What original (replaced) code used to do:
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		_asm {
			MOV ECX, mainAI;
			MOV EAX, unitId_toReset;
			MOV EDX,DWORD PTR DS:[ECX];
			PUSH EAX;
			CALL DS:[EDX+0x44];
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	// TODO: move in strategyUpdater
	// Custom treatments: if unit is a building, search for a unit being trained in it and cancel "in progress" flag
	if (unit_toReset->unitInstanceId != unitId_toReset) { return; } // Should not happen
	if (!unit_toReset->unitDefinition || unit_toReset->unitDefinition->unitType != AOE_CONST_FUNC::GUT_BUILDING) { return; } // Quit if not a building
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &mainAI->structBuildAI.fakeFirstStrategyElement;
	ror_api_assert(REG_values, fakeFirstElem != NULL);
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = fakeFirstElem->next;
	ror_api_assert(REG_values, stratElem != NULL);
	while (stratElem && stratElem != fakeFirstElem) {
		if ((stratElem->elementType == AOE_CONST_FUNC::AIUCLivingUnit) &&
			(stratElem->inProgressCount > 0) &&
			(stratElem->unitInstanceId == unitId_toReset)
		) {
			stratElem->unitInstanceId = -1;
			stratElem->inProgressCount = 0;
		}
		stratElem = stratElem->next;
	}
}


// From 0x4B8BDA
// This is called when a wonder construction is finished.
// ROR code sets "my wonder is built" flag, but they forgot to reset "I am building my wonder" and AI gets kinda stuck.
void RockNRorInstance::OnWonderConstructionCompleted(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_AI *mainAI = (AOE_STRUCTURES::STRUCT_AI *)REG_values->ECX_val;
	ror_api_assert(REG_values, mainAI && mainAI->IsCheckSumValid());
	if (!mainAI || !mainAI->IsCheckSumValid()) { return; }
	mainAI->structTacAI.myWonderIsBuiltFlag = 1;
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	// Always do the fix whatever config says (it's more a technical fix than an AI fix) - except in MP
	if (!IsMultiplayer() && !ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		mainAI->structTacAI.myWonderBeingBuiltFlag = 0; // This instruction is missing in original game code.
	}
}


// From 0x4D2218
// A strategy element (type=building) has been found and is going to be executed.
// We can add additional checks and possibly prevent the construction from being done.
// To prevent the construction, change return address to 004D236C.
// Otherwise, do nothing special and the execution will continue normally.
void RockNRorInstance::AuditOnDoStrategyElementBuilding(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *)REG_values->EAX_val;
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ESI_val;
	ror_api_assert(REG_values, tacAI && tacAI->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// This "if" is the original instruction we overwrote. (if EAX is NULL then JMP...)
		if (REG_values->EAX_val == 0) {
			ChangeReturnAddress(REG_values, 0x004D236C);
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	bool doNotRunConstruction = false; // Default: let construction be triggered...
	if (!stratElem) { return; } // Do not continue if strategy element was not found (nothing to build)
	ror_api_assert(REG_values, stratElem && stratElem->IsCheckSumValid());

	// Custom treatments
	ror_api_assert(REG_values, stratElem->elementType == AIUCBuilding);

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		doNotRunConstruction = STRATEGY::ShouldNotTriggerConstruction(tacAI, stratElem);
	}

	// Do not modify below
	if (doNotRunConstruction) {
		ChangeReturnAddress(REG_values, 0x004D236C);
	}
}


// 0x04604C9
// Intercepts ALL button OnClick events, including bitbuttons, checkboxes, dropdowns (just the "open to show list" click)
// Note: Change return address to 0x460568 to disable standard treatments for this event.
void RockNRorInstance::GlobalOnButtonClick(REG_BACKUP *REG_values) {
	ror_api_assert(REG_values, REG_values->ESI_val != 0);
	AOE_STRUCTURES::STRUCT_UI_BUTTON *button = (AOE_STRUCTURES::STRUCT_UI_BUTTON *)REG_values->ESI_val;
	ror_api_assert(REG_values, button && button->IsCheckSumValidForAChildClass());

	// Compatibility with EXE
	REG_values->EAX_val = (unsigned long int)button->currentState; // +1F6
	REG_values->fixesForGameEXECompatibilityAreDone = true;

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	// Custom treatments
	if (ROCKNROR::crMainInterface.Global_OnButtonClick(button)) {
		ChangeReturnAddress(REG_values, 0x460568); // goes to end of ROR method
	}
	static bool done = false;
	if (!done) {
		// TODO: find the appropriate place so that it is called only once: need an entry point just after window is created.
		ROCKNROR::crMainInterface.ChangeWindowTitle();
		done = true;
	}
}


// Manage the KeyPress event - 0x0041A6C7
// This is also called in "scenario info" in-game screen. NOT in diplomacy popup, menu, achievements, etc
// If returned REG_values->EAX_val is == 0 then calling method will not be executed (will RETN immediatly). Can be used to disable some treatments.
void RockNRorInstance::GameAndEditor_ManageKeyPress(REG_BACKUP *REG_values) {
	long int myESP = REG_values->ESP_val;
	long int pressedKey = *(long int *) (myESP + 0x14); // 0x12:ALT 0x73:F4
	long int myECX = REG_values->ECX_val;
	long int myEAX;
	long int myEDI;
	long int tmpChecksum = 0;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		_asm {
			// Original (replaced) code
			MOV EDI, myECX;
			MOV EAX, DWORD PTR DS:[EDI+0x14];
			MOV myEAX, EAX;
			MOV myEDI, EDI;
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = myEAX;
		REG_values->EDI_val = myEDI;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	unsigned long int **ptrCurrentUI = (unsigned long int **) AOE_OFFSETS::ADDR_VAR_ACTIVE_UI_STRUCT;
	assert(ptrCurrentUI != NULL); // it's a constant, it can't be NULL !
	if (!*ptrCurrentUI) { return; } // Check ROR's variable value is not NULL, or exit in that case (not an error from this code's point of view).

	// Additional info
	short int ctrl = GetKeyState(VK_CONTROL);
	bool ctrlIsON = (ctrl < 0);
	short int shift = GetKeyState(VK_SHIFT);
	bool shiftIsON = (shift < 0);
	bool altIsOn = (GetKeyState(VK_MENU) < 0); // is ALT key pressed ?

	// Call our dedicated event manager.
	if (ROCKNROR::crMainInterface.GameAndEditor_OnKeyPress(pressedKey, ctrlIsON, shiftIsON, altIsOn)) {
		REG_values->EAX_val = 0;
		ChangeReturnAddress(REG_values, 0x41A7C5); // Will NEG EAX (still 0) and then ignore the key press event
		return;
	}
}



// From 0x519FDC. May change return address to 0x51A0F4 (to replace original JNZ)
// This is called in-game, when a right-click is performed.
void RockNRorInstance::OnGameRightClickUpInGameCheckActionType(REG_BACKUP *REG_values) {
	MOUSE_ACTION_TYPES mouseActionType = (MOUSE_ACTION_TYPES)REG_values->EAX_val;
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (mouseActionType != MOUSE_ACTION_TYPES::CST_MAT_NORMAL) {
		// Mouse action types other than NORMAL (0): use default behaviour.
		ChangeReturnAddress(REG_values, 0x51A0F4); // Cf JNZ in 0x519FDE
	}
	if (mouseActionType >= 0) {
		return; // No custom treatments for standard mouse action types. Note there are other entry points in sub-calls. (OnGameRightClickUpEvent, etc)
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// If we choose to handle custom mouse action types here, then ChangeReturnAddress for mouseActionType<0 too (like original code)
	long int mousePosX = GetIntValueFromRORStack(REG_values, 0x28);
	long int mousePosY = GetIntValueFromRORStack(REG_values, 0x2C);
	float posX, posY;
	GetGamePositionUnderMouse(&posX, &posY);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unitUnderMouse = AOE_METHODS::GetUnitAtMousePosition(mousePosX, mousePosY, INTERACTION_MODES::CST_IM_RESOURCES, false);
	ROCKNROR::crCommand.OnInGameRightClickCustomAction(posX, posY, unitUnderMouse);
}


// Plugged on 0x51A508's call
// Triggered when player releases right click in game screen, for default mouse action type only.
void RockNRorInstance::OnGameRightClickUpEvent(REG_BACKUP *REG_values) {
	unsigned long int myESP = REG_values->ESP_val;
	const long int CST_OnGameRightClickUpEvent_IGNORE_CLICK = 0x0051A63D;
	const long int CST_OnGameRightClickUpEvent_HANDLE_CLICK = 0x0051A50B;
	AOE_STRUCTURES::STRUCT_UNIT_BASE **rorNewUnitList = *((AOE_STRUCTURES::STRUCT_UNIT_BASE ***)(myESP + 0x14)); // selected units, valid for action
	short int numberOfUnitsInList = *((short*)(myESP + 0x12));
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// Manage overwritten instruction
		long int findSelectedUnitsPreviousResult = (REG_values->EAX_val & 0xFF); // only low byte (AL) is relevant !
		if (findSelectedUnitsPreviousResult == 0) {
			REG_values->EAX_val = CST_OnGameRightClickUpEvent_IGNORE_CLICK; // Corresponds to (effective) JE 0x0051A63D in original code
		} else {
			REG_values->EAX_val = CST_OnGameRightClickUpEvent_HANDLE_CLICK; // Continue (no real jump)
		}
		// IMPORTANT warning: if we choose to jump away while REG_values->EAX_val == 1, we HAVE to free the list.
		// We're lucky, the "CALL 005260F8" tests if argument is NULL. SO we can ALWAYS free the list here,
		// So in all cases the list is freed and there is no error (if we jump back to normal location, it will attempt to free(0) and just do nothing.
		if (findSelectedUnitsPreviousResult && (rorNewUnitList != NULL)) {
			_asm {
				MOV ECX, rorNewUnitList;
				PUSH ECX;
				MOV EAX, 0x005260F8; // Free
				CALL EAX;
				ADD ESP, 4;
				// Update stack variable so that game code won't call free() with the same address (but 0 instead)
				MOV EAX, myESP;
				XOR ECX, ECX;
				MOV DS:[EAX+0x14], ECX;
			}
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// Now we correctly plugged our method to game EXE. In all cases it's OK, the list is freed and won't be again (in ROR code)
	// It is possible to change the "return address" (JMP, in fact) by setting REG_values->EAX_val. Do not change other values.

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Now manage custom treatments
	AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *UIGameMain = (AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *) REG_values->ESI_val;
	if (!UIGameMain || !UIGameMain->IsCheckSumValid()) { return; }
	ror_api_assert(REG_values, UIGameMain != NULL);
	AOE_STRUCTURES::STRUCT_PLAYER *controlledPlayer = UIGameMain->controlledPlayer;
	if (controlledPlayer == NULL) { return; } // I suppose it shouldn't happen but I don't know all behaviours for this variable
	unsigned long int unitCount = controlledPlayer->selectedUnitCount;
	if (unitCount == 0) {
		return;
	}


	long int mousePosX = *(long int *)(myESP + 0x34);
	long int mousePosY = *(long int *)(myESP + 0x38);
	if (ROCKNROR::crMainInterface.ApplyRightClickReleaseOnSelectedUnits(UIGameMain, controlledPlayer, mousePosX, mousePosY)) {
		REG_values->EAX_val = CST_OnGameRightClickUpEvent_IGNORE_CLICK; // Force to ignore click in ROR code
	}
}


// Manages key press on +/- in game to change game speed.
// Also fixes the "6" key press bug on french keyboard
void RockNRorInstance::ManageChangeGameSpeed(REG_BACKUP *REG_values) {
	long int myESP = REG_values->ESP_val;
	// Read arguments/local variables
	float *gameSpeed = (float *)(myESP + 8);
	long int *bool_increaseSpeed = (long int *)(myESP + 0x110);
	SHORT keyStateResult;

	if (!*bool_increaseSpeed) {
		keyStateResult = GetKeyState(0x36); // "-6" key
		keyStateResult = keyStateResult & 0xF0;
		if (keyStateResult) { return; }
	}
	

	// if custom game speeds are not enabled, use standard calculation
	if (!ROCKNROR::crInfo.configInfo.useImprovedGameSpeeds || ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// Reimplement standard algorithm (use only 1.0, 1.5, 2.0 hardcoded values)
		if (*bool_increaseSpeed) {
			*gameSpeed = *gameSpeed + 0.5f;
			if (*gameSpeed > 2) { *gameSpeed = 2; }
		} else {
			*gameSpeed = *gameSpeed - 0.5f;
			if (*gameSpeed < 1) { *gameSpeed = 1; }
		}
		return;
	}

	// Calculate new speed
	if (*bool_increaseSpeed) {
		*gameSpeed = (*gameSpeed) * ROCKNROR::crInfo.configInfo.improvedGameSpeedFactor;
	} else {
		*gameSpeed = (*gameSpeed) / ROCKNROR::crInfo.configInfo.improvedGameSpeedFactor;
	}
	// Fix value when ~1 (fix rounding issues)
	if (abs(*gameSpeed - 1) <= 0.1) { *gameSpeed = 1; }
	// Fix null speed
	if (abs(*gameSpeed) < 0.05) {
		*gameSpeed = 0.5;
		traceMessageHandler.WriteMessage("Fix NULL speed, set to 0.5x instead.");
	}
}


// 004D0989.
// Entry point in the "tactical AI update" event that occurs regularly (every x milliseconds) for each AI player.
// For the strategy algorithms to work well, requires also "FixUnitIdForInProgressBuilding", "FixResetStratElemForUnitId"
// May change return address to 0x4D0EED to skip the tacAI from this method (but still call underlying tacAI.update()). 
// ... Note that underlying tacAI.update is also optimized in ManageDefeatedAIPlayerTacAIUpdate(...)
// May change return address to 0x4D0EFE to skip the whole tacAI update process (dead players...)
// ... => in this case, we must change managed AI player manually to avoid being stuck
void RockNRorInstance::ManageTacAIUpdate(REG_BACKUP *REG_values) {
	ror_api_assert(REG_values, REG_values->ECX_val == REG_values->ESI_val);
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ESI_val;
	ror_api_assert(REG_values, tacAI && tacAI->IsCheckSumValid());
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	REG_values->EDX_val = (unsigned long int) tacAI->ptrMainAI;
	// Note: ManageTacAIUpdate takes into account ROCKNROR::crInfo.configInfo.doNotApplyFixes.
	bool isDeadPlayer = !ROCKNROR::crCommand.ManageTacAIUpdate(tacAI->ptrMainAI);

	if (isDeadPlayer) {
		ChangeReturnAddress(REG_values, 0x4D0EFE);
		AOE_METHODS::PLAYER::GlobalSetNextManagedAIPlayer();

		//ChangeReturnAddress(REG_values, 0x4D0EED); // Other solution that does not require calling GlobalSetNextManagedAIPlayer(). But more treatments are executed.
	}
}


// From 004D1B7B. Avoid calling AI updates on defeated players.
// Change return address to 0x4D24E8 if AI update must be skipped (and set ECX=mainAI, in this case)
// The code in 0x4D24E8 will change "currently managed AI player" so AIs won't get stuck.
// Warning: for performance reasons, this method should NOT do too much treatments !
void RockNRorInstance::ManageDefeatedAIPlayerTacAIUpdate(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ESI_val;
	ror_api_assert(REG_values, tacAI && tacAI->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = (unsigned long int) tacAI->ptrMainAI;
	}
	// Changes nothing in standard case (ECX will be overwritten in 0x4D1B82)
	// In custom case (skip - JMP to 0x4D24E8), it is necessary that ECX=mainAI
	REG_values->ECX_val = (unsigned long int) tacAI->ptrMainAI;

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	ror_api_assert(REG_values, tacAI->ptrMainAI && tacAI->ptrMainAI->IsCheckSumValid());
	if (!tacAI->ptrMainAI || !tacAI->ptrMainAI->IsCheckSumValid()) { return; }

	AOE_STRUCTURES::STRUCT_PLAYER *player = tacAI->ptrMainAI->ptrStructPlayer;
	ror_api_assert(REG_values, player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return; }
	// Custom case: skip AI updates for defeated players (also for players that won - game would be ended in a such case)
	if (player->aliveStatus != 0) {
		ChangeReturnAddress(REG_values, 0x04D24E8);
	}
}

// This is called just before calling empires.dat loading ([global]+0xB0), called from 0x501211.
void RockNRorInstance::OnBeforeLoadEmpires_DAT(REG_BACKUP *REG_values) {
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		REG_values->ECX_val = REG_values->ECX_val + 0x1F8;
		return;
	}
	AOE_STRUCTURES::STRUCT_COMMAND_LINE_INFO *cmdLineInfo = (AOE_STRUCTURES::STRUCT_COMMAND_LINE_INFO *)REG_values->ECX_val;
	const char *empiresDatRelativeFileName = ROCKNROR::crCommand.GetCustomEmpiresDatRelativeFileName(cmdLineInfo);
#ifdef GAMEVERSION_AOE10b
	REG_values->EAX_val = 
#else
	REG_values->ECX_val =
#endif
		(unsigned long int) empiresDatRelativeFileName;
}


// This is called just after empires.dat is loaded.
void RockNRorInstance::OnAfterLoadEmpires_DAT(REG_BACKUP *REG_values) {
	REG_values->fixesForGameEXECompatibilityAreDone = true; // There is nothing special to do for this change to be compatible with ROR execution.
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		ROCKNROR::crCommand.OnAfterLoadEmpires_DAT();
	}
}


// From 0x51A3BA
// This is called when user clicks to add a unit in scenario editor (NOT called when moving mouse in game zone).
void RockNRorInstance::EditorCheckForUnitPlacement(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *) REG_values->ESI_val;
	long int callResult = 0;
	REG_values->ECX_val = REG_values->ESI_val; // in fact it is not necessary because we do the CALL ourselves
	REG_values->fixesForGameEXECompatibilityAreDone = true;

	ror_api_assert(REG_values, unitDef);
	ror_api_assert(REG_values, unitDef->IsCheckSumValidForAUnitClass());
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
	ror_api_assert(REG_values, player);
	ror_api_assert(REG_values, player->IsCheckSumValid());

	long int *argsAsInt = (long int *)REG_values->ESP_val;
	float *argsAsFloat = (float *)REG_values->ESP_val;

	float posY = argsAsFloat[0];
	float posX = argsAsFloat[1];
	unsigned long int arg4 = argsAsInt[2];
	long int checkVisibility = argsAsInt[3];
	long int checkHillMode = argsAsInt[4] & 0xFF; // keeping only relevant byte is not necessary, but more practical for us.
	long int arg7 = argsAsInt[5];
	long int editorMode = argsAsInt[6]; // should be 1
	long int checkAirModeAndHPBar = argsAsInt[7];
	long int checkConflictingUnits = argsAsInt[8];
	bool ignoreTerrainRestrictionErrors = false;

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// Let crCommand object customize the parameters according to configuration and user choices...
		ROCKNROR::crCommand.ApplyCustomizationOnEditorAddUnit(checkVisibility, checkHillMode, editorMode, checkAirModeAndHPBar, checkConflictingUnits, ignoreTerrainRestrictionErrors);
	}

	// CALL defUnit.GetErrorForUnitCreationAtLocation(player, posY, posX, arg4, checkVisibility, hillMode, arg7, editorMode, checkAirModeAndHPBar?, checkConflictingUnits)
	_asm {
		PUSH checkConflictingUnits;
		PUSH checkAirModeAndHPBar;
		PUSH editorMode;
		PUSH arg7;
		PUSH checkHillMode;
		PUSH checkVisibility;
		PUSH arg4;
		PUSH posX;
		PUSH posY;
		PUSH player;
		MOV ECX, unitDef;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX+0x20];
		AND EAX, 0xFF;
		MOV callResult, EAX;
	}
	REG_values->EAX_val = callResult;
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		if (ignoreTerrainRestrictionErrors && (callResult == AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION::CST_EUC_BAD_TERRAIN_RESTRICTION)) {
			REG_values->EAX_val = AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION::CST_EUC_OK;
		}
	}
}


// From 0x051A700
void RockNRorInstance::EntryPoint_UIUnitSelection(REG_BACKUP *REG_values) {
	REG_values->fixesForGameEXECompatibilityAreDone = true; // No action required in this case.
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return; }
	
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Custom treatments
	if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) {
		unsigned long int *pArgInteractionMode = (unsigned long int *) (REG_values->ESP_val + 0x20);
		// For some specific actions, we force selection to allow every type of unit (select/modify unit actions)
		if ((settings->mouseActionType == AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_NORMAL) ||
			(settings->mouseActionType == AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_EDITOR_CHANGE_ORIENTATION) ||
			(settings->mouseActionType == AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES::CST_MAT_EDITOR_MOVE_UNIT)
			) {
			*pArgInteractionMode = 0; // Will allow selecting/moving/changing orientation on all units
		}
	}
}


// From 0x426D08
// Specific code to handle gaia units conversion when found by human player.
// Fix some special cases (artefacts).
// Change return address to 0x426D19 if we want NOT to change unit owner.
void RockNRorInstance::HumanSpecific_onCapturableUnitSeen(REG_BACKUP *REG_values) {
	long int actorPlayerId = REG_values->EDI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *beingSeenUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) REG_values->ESI_val;
	ror_api_assert(REG_values, actorPlayerId >= 0 && actorPlayerId <= 8);
	ror_api_assert(REG_values, beingSeenUnit != NULL);
	ror_api_assert(REG_values, beingSeenUnit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, beingSeenUnit->DerivesFromAttackable());
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = NULL;
	if (!beingSeenUnit || !beingSeenUnit->IsCheckSumValidForAUnitClass() || !beingSeenUnit->DerivesFromAttackable() || 
		(actorPlayerId < 0) || (actorPlayerId > 8)) { return; }
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		actorPlayer = GetPlayerStruct((short int)actorPlayerId);
		REG_values->ECX_val = (unsigned long int)actorPlayer;
	}
	bool doNotCaptureUnit = false;

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// Custom treatments here
		doNotCaptureUnit = ROCKNROR::crCommand.HumanSpecific_onCapturableUnitSeen(beingSeenUnit, actorPlayer);
	}

	// Do not modify below.
	if (doNotCaptureUnit) {
		ChangeReturnAddress(REG_values, 0x426D19);
	}
}


// From 0x50B6C2
void RockNRorInstance::ManageRelicsCount(REG_BACKUP *REG_values) {
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		REG_values->EAX_val = 5;
	} else {
		REG_values->EAX_val = ROCKNROR::crInfo.configInfo.randomMapRelicsCount;
	}
}


void RockNRorInstance::ManageRuinsCount(REG_BACKUP *REG_values) {
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		REG_values->EAX_val = 5;
	} else {
		REG_values->EAX_val = ROCKNROR::crInfo.configInfo.randomMapRuinsCount;
	}
}


void RockNRorInstance::ManageCivIdControlAfterGameSettingsSelection(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		return;
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		ChangeReturnAddress(REG_values, 0x00503451);
	}

	if ((REG_values->EAX_val < 0) || (REG_values->EAX_val > ROCKNROR::crInfo.configInfo.civCount)) {
		// Bad civilization, jump to the code that randomly chooses one (normal behaviour with "random" option)
		ChangeReturnAddress(REG_values, 0x00503423);
	} else {
		// Valid civilization, continue with it.
		ChangeReturnAddress(REG_values, 0x00503451);
	}
}


// Manage first call to gameSettings.getDLLCivNameIdOffset(civId)
// For custom civs, returns 0 so the call we be made again later.
void RockNRorInstance::ManageGetPlayerNameDLLStringIdOffset(REG_BACKUP *REG_values) {
	// TODO: not taking into account ROCKNROR::crInfo.configInfo.doNotApplyFixes
	long int civId = REG_values->EAX_val;
	long int myECX = REG_values->ECX_val; // game settings addr
	long int *stackPtr = (long int *)(REG_values->ESP_val);
	long int civNameDLLStringIdOffset;
	if ((civId > 0) && (civId <= CIVILIZATIONS::CST_CIVID_STANDARD_MAX)) {
		// Just do the normal code: call gameSettings.getDLLCivNameIdOffset(civId) and continue...
		_asm {
			PUSH civId;
			MOV EDX, 0x00505F90;
			MOV ECX, myECX;
			CALL EDX; // gameSettings.getDLLCivNameIdOffset(civId)
			MOV civNameDLLStringIdOffset, EAX;
		}
		*stackPtr = civNameDLLStringIdOffset;
		return;
	}
	//long int playerIdMinus1 = REG_values->ESI_val;

	*stackPtr = 0; // Cannot store the value we need in a byte (0<x<255). Returning NULL makes so it is called again in 0x00503865

}


// Manages 2nd call to gameSettings.getDLLPlayerNameIdOffset(civId)
// For custom civs, returns a specific name taken from standard DLL strings.
// WARNING: this changes ROR return address
void RockNRorInstance::ManageGetPlayerNameDLLStringIdOffset2(REG_BACKUP *REG_values) {
	long int civId = REG_values->EAX_val;
	long int myECX = REG_values->ECX_val; // game settings addr
	long int myEDX = REG_values->EDX_val;
	long int myESP = REG_values->ESP_val;
	long int *stackPtr = (long int *)(myESP); // initially this stack location contains civId (just PUSHed EAX).
	long int civNameDLLStringIdOffset;
	char playerId = (char)*(stackPtr + (0x38 / 4));

	if ((civId > 0) && (civId <= CIVILIZATIONS::CST_CIVID_STANDARD_MAX)) {
		// Just do the normal code: call gameSettings.getDLLPlayerNameIdOffsetForCiv(civId) and continue...
		_asm {
			PUSH civId;
			MOV EDX, 0x00505F90;
			MOV ECX, myECX;
			CALL EDX; // gameSettings.getDLLPlayerNameIdOffsetForCiv(civId)
			ADD EAX, LANG_ID_PLAYER_NAME_BY_CIV_BASE;
			MOV civNameDLLStringIdOffset, EAX;
			MOV EDX, myEDX;
		}
		// Write in game settings that we used that DLL name ID...
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *gameSettings = GetGameSettingsPtr();
		gameSettings->civPlayerNameIsUsed[civNameDLLStringIdOffset - 9] = 1;
		gameSettings->playerNameCivDLLID_Offset[playerId] = (char)civNameDLLStringIdOffset - 9;

		ChangeReturnAddress(REG_values, 0x503874);
		*stackPtr = civNameDLLStringIdOffset; // See comment below about stackPtr
		return;
	}
	// Custom civ: we don't have names for them in language(x).dll

	// Try to use player names provided in configuration file.
	CivilizationInfo *c = ROCKNROR::crInfo.configInfo.GetCivInfo(civId);
	if (c) {
		if (playerId > 8) { playerId = 8; } // Would be nonsense but this will avoid writing out of nameIndexIsUsedByPlayer bounds
		if (playerId < 0) { playerId = 0; } // Same thing
		std::string s = c->ChooseRandomPlayerName(ROCKNROR::crInfo.nameIndexIsUsedByPlayer[civId], playerId);
		if (!s.empty()) {
			char **buffer = (char **)(myESP + 4); // arg2 that was pushed
			long int *bufferSize = (long int *)(myESP + 8); // arg3 that was pushed
			strcpy_s(*buffer, *bufferSize, s.c_str()); // Writes our player name in buffer.
			// Do NOT change return addr (customized ROR code will POP 3 dwords and JMP to skip unwanted call (we already have our string)
			// Do NOT write on *stackPtr because it will be POPed and value will be unused (with ADD ESP, 0x0C - simulate a RETN 0x0C)
			return;
		}
	}

	// No name is available. Use "player1" "player2" etc from language.dll according to player id.

	// Don't PUSH the value again. As this call does not POP the argument, it is directly used as arg for next call.
	*stackPtr = LANG_ID_PLAYERX_BASE + playerId;
	ChangeReturnAddress(REG_values, 0x503874);
}


// Chooses a convenient strategy (ai file) for given player according to configuration/map type for custom civs.
// Fills game's buffer with an existing AI filename for given player. If nothing is found (or if file doesnt exist), a NULL string is used.
// This method must NOT fill the buffer with an incorrect file name or the game will freeze.
// Warning: this method may change ROR return address
void RockNRorInstance::ManageAIFileSelectionForPlayer(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_PLAYER *p = (AOE_STRUCTURES::STRUCT_PLAYER *) REG_values->EBX_val;
	if (!p) { return; }
	if (!p->ptrAIStruct) {
		return; // customized ROR code will JMP to end of "player.chooseAIFileName" function
	}
	
	// Custom civ
	char **stackPtr = (char **) (REG_values->ESP_val + 0x320); // 0x318 for sub esp and 8 for the 2 PUSH (cf ROR code).
	char *buffer1 = stackPtr[1];
	char *buffer2 = stackPtr[2];

	strcpy_s(buffer2, 22, "RandomGameSpecialized");
	if (!ROCKNROR::crCommand.ManageAIFileSelectionForPlayer(p->civilizationId, buffer1)) {
		ChangeReturnAddress(REG_values, 0x004F36F8); // continue "player.chooseAIFileName" function normally
	}

	// Return to normal address (leave REG_values->return_address unchanged).
}


void RockNRorInstance::ManageCivsInGameSettingsCombo(REG_BACKUP *REG_values) {
	long int myESI = REG_values->ESI_val;
	ror_api_assert(REG_values, myESI != NULL);
	if (!myESI) { return; }
	AOE_STRUCTURES::STRUCT_ANY_UI *ptrCombo;
	_asm {
		MOV ESI, myESI;
		MOV ECX, DS:[ESI];
		MOV ptrCombo, ECX;
	}
	for (int i = 1; i <= CIVILIZATIONS::CST_CIVID_STANDARD_MAX; i++) {
		long int DLL_ID = i + 0x27F6;
		if (i > 0x0C) {
			DLL_ID += 3; // There's a gap after civID=0x0C (choson - last non-expansion civ)
		}
		if (i == 0x0F) { DLL_ID = 0x2809; } // Stupid inversion between palmyra and macedonia
		if (i == 0x10) { DLL_ID = 0x2808; } // Stupid inversion between palmyra and macedonia
		AOE_METHODS::UI_BASE::AddEntryInComboUsingDLLID(ptrCombo, i, DLL_ID);
	}

	// Do we already have a global struct ? If Yes, check civ number in empires.dat (if not matching our config, it may crash)
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	if (globalStruct != NULL) {
		if (globalStruct->civCount < ROCKNROR::crInfo.configInfo.civCount) {
			// This situation is quite bad. empires.dat contains less civs than XML config.
			// We can only detect this when global struct already exists.
			// On first time we run a game, settings are displayed BEFORE the global struct is created (and before empires.dat is loaded)
			ROCKNROR::crInfo.configInfo.civCount = globalStruct->civCount;
			const char *msg = localizationHandler.GetTranslation(CRLANG_ID_MISSING_CIVS_IN_EMPIRES_DAT, "ERROR: there are more civs in RockNRor_civs.xml than in empires.dat. This may cause game crashes !");
			traceMessageHandler.WriteMessage(msg);
			FILE *fileLog = NULL;
			int logFileRes;
			logFileRes = fopen_s(&fileLog, MOD_NAME "\\" MOD_NAME ".log", "a + "); // appends (do not overwrite)
			if (logFileRes == 0) {
				fprintf_s(fileLog, msg);
				fprintf_s(fileLog, "\n");
			}
			fclose(fileLog);
		}
	}

	// Manage custom civs
	for (int civid = CIVILIZATIONS::CST_CIVID_STANDARD_MAX + 1; civid <= ROCKNROR::crInfo.configInfo.civCount; civid++) {
		CivilizationInfo *civInfo = ROCKNROR::crInfo.configInfo.GetCivInfo(civid);
		if (civInfo) {
			AOE_METHODS::UI_BASE::AddEntryInCombo(ptrCombo, civid, civInfo->GetCivName().c_str());
		} else {
			AOE_METHODS::UI_BASE::AddEntryInCombo(ptrCombo, civid, localizationHandler.GetTranslation(CRLANG_ID_DEFAULT_CIV_NAME, "Custom civilization"));
		}
	}
	

	// Add random entry
	AOE_METHODS::UI_BASE::AddEntryInComboUsingDLLID(ptrCombo, ROCKNROR::crInfo.configInfo.civCount + 1, 0x280A);
}


// Manages filling the civilization combobox in scenario editor UI. Overloads default behaviour.
void RockNRorInstance::ManageCivsInEditorCombo(REG_BACKUP *REG_values) {
	long int myEDI = REG_values->EDI_val;
	ror_api_assert(REG_values, myEDI != NULL);
	if (!myEDI) { return; }
	AOE_STRUCTURES::STRUCT_ANY_UI *ptrCombo;
	_asm {
		MOV EDI, myEDI
		MOV ECX, DS:[EDI]
		MOV ptrCombo, ECX
	}
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		for (int i = 1; i <= CIVILIZATIONS::CST_CIVID_STANDARD_MAX; i++) {
			long int DLL_ID = i + 0x27F6;
			if (i > 0x0C) {
				DLL_ID += 3; // There's a gap after civID=0x0C (choson - last non-expansion civ)
			}
			if (i == 0x0F) { DLL_ID = 0x2809; } // Stupid inversion between palmyra and macedonia
			if (i == 0x10) { DLL_ID = 0x2808; } // Stupid inversion between palmyra and macedonia
			AOE_METHODS::UI_BASE::AddEntryInComboUsingDLLID(ptrCombo, 0, DLL_ID);
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// Manage custom civs
	for (int civId = CIVILIZATIONS::CST_CIVID_STANDARD_MAX + 1; civId <= ROCKNROR::crInfo.configInfo.civCount; civId++) {
		CivilizationInfo *civInfo = ROCKNROR::crInfo.configInfo.GetCivInfo(civId);
		if (civInfo) {
			AOE_METHODS::UI_BASE::AddEntryInCombo(ptrCombo, 0, civInfo->GetCivName().c_str());
		} else {
			AOE_METHODS::UI_BASE::AddEntryInCombo(ptrCombo, 0, localizationHandler.GetTranslation(CRLANG_ID_DEFAULT_CIV_NAME, "Custom civilization"));
		}
	}

	// Do NOT add random entry in scenario editor
}


// Overloads "default" case in in-game GetCivName method.
void RockNRorInstance::GetInGameCustomCivName(REG_BACKUP *REG_values) {
	long int civId = REG_values->EDX_val;
	long int bufferSize = REG_values->EDI_val;
	char *buffer = (char *)REG_values->ESI_val;

	CivilizationInfo *civInfo = ROCKNROR::crInfo.configInfo.GetCivInfo(civId);
	if ((civId > ROCKNROR::crInfo.configInfo.civCount) || (civId < 1) || (!civInfo)) {
		// Unknown civ id (or not defined in RockNRor config). Use original (overwritten code)
		// Note: civ name is NOT displayed when controlling player 0 (gaia)
		long int myEAX = REG_values->EAX_val;
		long int gameSettings = REG_values->ECX_val;
		_asm {
			MOV EAX, myEAX;
			MOV ECX, gameSettings;
			PUSH bufferSize;
			PUSH buffer;
			PUSH 0x2804;
			CALL DWORD PTR DS:[EAX+0x20];
		}
		return;
	}
	// Here civInfo is not NULL, we can use its data.
	ror_api_assert(REG_values, civInfo != NULL); // good check just in case the code above is modified...
	strcpy_s(buffer, bufferSize, civInfo->GetCivName().c_str());
}


// From 0x50A56F and 0x50A5B2
void RockNRorInstance::CheckPlayerCreationAtGameStartup(REG_BACKUP *REG_values) {
	bool isHumanPlayer = (REG_values->return_address == 0x0050A5B9);
	unsigned long int myESI = REG_values->ESI_val;
	unsigned long int myEBX = REG_values->EBX_val;
	unsigned long int myEDX = REG_values->EDX_val;
	unsigned char civId = *(unsigned char*)(myESI + myEBX + 0x251);
	if (civId > ROCKNROR::crInfo.configInfo.civCount) { civId = 1; } // Force value to avoid game crash
	myEDX = myEDX & 0xFFFFFF00; // Remove "DL" part (low byte)
	myEDX = myEDX | civId; // Replace with civId
	REG_values->EDX_val = myEDX;

	// Fix for multiplayer games. Never create AI struct for human players to avoid sync errors.
	if (isHumanPlayer && IsMultiplayer()) {
		ChangeReturnAddress(REG_values, 0x50A576);
	}
}


// From 0048D13C. Randomly picks civilizations for each player, at game settings screen init.
// May change return address to 0x48D156
void RockNRorInstance::PickRandomCivForPlayer(REG_BACKUP *REG_values) {
	long int curPlayerIndex = REG_values->ESI_val; // = playerId-1 (beware)
	
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		long int pseudoRandom = AOE_METHODS::GetAndReCalcPseudoRandomValue();
		REG_values->EAX_val = pseudoRandom;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	
	if (!ROCKNROR::crInfo.configInfo.allowPickingCustomCivsInRandomInit || ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Custom treatments
	long int pseudoRandom = AOE_METHODS::GetAndReCalcPseudoRandomValue();
	long int choice = pseudoRandom % ROCKNROR::crInfo.configInfo.civCount; // civCount does not include "gaia" civ.
	choice++; // Because civ0 is "gaia" civ, we must ignore it. Now choice is in [1..n] interval.
	REG_values->EAX_val = choice;
	REG_values->ECX_val = (unsigned long int)GetGameSettingsPtr();
	ChangeReturnAddress(REG_values, 0x48D156);
}


#ifdef _DEBUG
// Make some checks to prevent irrelevant target unit change and stupid hesitations (like when attacked by 2 priests)
// Return (custom) code is used in customized code in game EXE to decide where to jump.
// 1 = Target unit is REALLY the same as before. Continue with normal behaviour for this case => 00426EE1 (no jump)
//     Also return 1 to keep current target (in that case, force EDI = <old_target_ptr>)
// 2 = We prefer to change target unit. Use normal behaviour (let the original code decide) for this case in 00426F28 (cf original JNZ)
// 3 = OBSOLETE (DO NOT USE) Custom jump to 00426FD1 to exit successfully the method, without changing target. Sometimes CAUSES stuck units bug !
void RockNRorInstance::ManageAttackActionChange(REG_BACKUP *REG_values) {
	assert(false && "ManageAttackActionChange is obsolete"); // This method is OBSOLETE
	const float distanceToConsiderVeryClose = 1.5; // Please leave this > 0.
	REG_values->EAX_val = 2; // Default: change action (let the original code decide)
	// Get context info
	AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) REG_values->EDI_val; // "new" possible target
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_ACTION_INFO *actorActionInfo = (AOE_STRUCTURES::STRUCT_UNIT_ACTION_INFO *) REG_values->ECX_val;

	if (!actorActionInfo || !actorActionInfo->ptrActionLink || !targetUnit || !actorUnit) {
		return;
	}
	ror_api_assert(REG_values, targetUnit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, actorUnit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, actorActionInfo->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_ACTION_LINK *link = actorActionInfo->ptrActionLink;
	ror_api_assert(REG_values, link != NULL);
	AOE_STRUCTURES::STRUCT_ACTION_ATTACK *action = (AOE_STRUCTURES::STRUCT_ACTION_ATTACK *) link->actionStruct;
	ror_api_assert(REG_values, action != NULL);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actionTargetUnit = action->targetUnit; // 00426EE5 original instruction...
	if (!actionTargetUnit) {
		return; // Change action: there is currently no target !
	}
	bool sameTargetUnit = (actionTargetUnit == targetUnit); // Cf cmp in original code 00426EE8
	if (sameTargetUnit) {
		REG_values->EAX_val = 1; // Same unit as before, continue without a jump (custom case 1).
		return;
	}

	// Run algorithm to decide if unit should keep target or change
	bool shouldChange = ROCKNROR::COMBAT::ShouldChangeTarget(actorUnit->currentActivity, targetUnit->unitInstanceId);
	if (!shouldChange) { REG_values->EDI_val = (long)actionTargetUnit; REG_values->EAX_val = 2; }
}
#endif


// From 00411D58 (activity.setAttackUnit(unitId, force)). For military units + villagers (but not villagers against tree/animals)
// For compatibility with original code, if EBP != 0 then change return address to 0x411D64 (equivalent to the JNZ we overwrote).
// It is possible to overwrite arg1 in stack to force another unitId (ESP+014) (ROR method code has not read it yet).
// Warning: unitId in stack (ESP+014) should NOT be -1 (game bug). If this occurs, force change return address to 0x411DC7 (return 0 in ROR code)
void RockNRorInstance::ManageAttackActivityChange1(REG_BACKUP *REG_values) {
	long int arg_flag = REG_values->EBP_val;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->ESI_val = REG_values->ECX_val; // MOV esi, ecx
		if (arg_flag) {
			ChangeReturnAddress(REG_values, 0x411D64);
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ECX_val;
	long int *arg_targetUnitId = (long int*)(REG_values->ESP_val + 0x14); // Get arg1 (pointer, we may want to update it)

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Custom Treatments.
	if ((*arg_targetUnitId != -1) && !ROCKNROR::COMBAT::ShouldChangeTarget(activity, *arg_targetUnitId)) {
		*arg_targetUnitId = activity->targetUnitId; // Force current target to keep it (do not change target).
	}
	if (*arg_targetUnitId == -1) {
		// This case is not correctly managed by the game. It searches unit -1 (sometimes finds a doppleganger which is irrelevant).
		// Force "return false" in ROR method
		ChangeReturnAddress(REG_values, 0x411DC7);
		return;
	}
}


// From 00411F26. For converting priests
// For compatibility with original code, if EAX != 0 then change return address to 0x411F35 (equivalent to the JNZ we overwrote).
// It is possible to overwrite arg1 in stack to force another unitId (ESP+014) (ROR method code has not read it yet).
void RockNRorInstance::ManageAttackActivityChange_convert(REG_BACKUP *REG_values) {
	long int arg_flag = REG_values->EAX_val;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->ESI_val = REG_values->ECX_val; // MOV esi, ecx
		if (arg_flag) {
			ChangeReturnAddress(REG_values, 0x411F35);
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ECX_val;
	long int *arg_targetUnitId = (long int*)(REG_values->ESP_val + 0x0C); // Get arg1 (pointer, we may want to update it)
	
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Custom Treatments.
	if (*arg_targetUnitId == -1) { return; } // Let normal code manage this...

	if (!ROCKNROR::COMBAT::ShouldChangeTarget(activity, *arg_targetUnitId)) {
		*arg_targetUnitId = activity->targetUnitId; // Force current target to keep it (do not change target).
	}
}


// Called from 004E37AF (manage attack tower in my town) + 004D8299 (reaction when being attacked - can be other than tower, in/out my town...)
// We are lucky, in both cases,  EBX/EDI/ECX have the same role.
// Manage military units reaction when AI has an enemy tower in his town (or when attacked). Especially for priests that are stuck !
// The return value for REG_values->EAX_val is crucial:
// 0 = The unit will NOT change task (NEW case). Will jump to 0x04E39BC / 0x04D86D0 (see ROR modified code).
// 1 = Change unit action to attack tower, to use when (activity.taskId != 0x258) (standard case). Will jump to 004E389A / 004D8388 just like in original code (see ROR modified code).
// 2 = Change unit action to attack tower, to use when (activity.taskId == 0x258) (standard case). No jump, continue with normal code.
// The JUMP addresses are in ROR modified code (we use some TEST EAX / CMP AL,1 + JE in modified code), that's why we can use this method for different CALL addresses.
void RockNRorInstance::ManageTowerPanicMode_militaryUnits(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ECX_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *myUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EBX_val;
	//AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ESI_val;
	REG_values->EAX_val = 1; // Default
	ror_api_assert(REG_values, activity != NULL); // This has been verified in ROR code just before calling ROR_API
	ror_api_assert(REG_values, enemyUnit != NULL);
	ror_api_assert(REG_values, myUnit != NULL);
	//ror_api_assert(REG_values, tacAI != NULL);
	if (!activity || !enemyUnit || !myUnit) { return; }
	ror_api_assert(REG_values, enemyUnit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, myUnit->IsCheckSumValidForAUnitClass());
	//ror_api_assert(REG_values, tacAI->IsCheckSumValid());
	bool is_attack_activity = false;
	bool forceKeepCurrentActivity = false; // Default: do NOT skip ! (attack the tower/enemy)
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		if (activity->currentTaskId == AOE_CONST_INTERNAL::ACTIVITY_TASK_ID::CST_ATI_TASK_ATTACK) {
			is_attack_activity = true;
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// Info on context
	bool contextIsTowerInMyTown = (REG_values->return_address == 0x04E37B4);
	bool contextIsReactionToAttack = (REG_values->return_address == 0x04D829E);

	// Custom treatments: compute "forceKeepCurrentActivity" variable in specific cases we want to improve.
	// Warning: don't forget there is 2 possible contexts (tower in my town OR I was just attacked by enemyUnit)

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		if (contextIsTowerInMyTown) {
			if (!ROCKNROR::crCommand.ShouldAttackTower_towerPanic((STRUCT_UNIT_COMMANDABLE*)myUnit, enemyUnit)) {
				forceKeepCurrentActivity = true;
			}
		}
		if (contextIsReactionToAttack) {
			forceKeepCurrentActivity = !ROCKNROR::COMBAT::ShouldChangeTarget(activity, enemyUnit->unitInstanceId);
		}
	}


	// *** DO NOT MODIFY BELOW *** Insert custom treatments before and do not do a "return" to let this code be executed.
	// Update forceKeepCurrentActivity variable to impact game behaviour according to your computations.
	if (forceKeepCurrentActivity) {
		REG_values->EAX_val = 0; // Keep current activity/Action (NEW case). Works for all taskId (== or != 0x258).
	} else {
		// We want the unit to attack the tower.
		if (is_attack_activity) {
			REG_values->EAX_val = 2; // taskId is 0x258 => case 2 : no jump
		} else {
			REG_values->EAX_val = 1; // other than 0x258: case 1 : jump
		}
	}
}


/* Called from 004E39D1. Replace ROR's loop to attack the tower with villagers so we can use uour own criteria.
This does not take into account ROCKNROR::crInfo.configInfo.doNotApplyFixes
*/
void RockNRorInstance::ManageTowerPanicMode_villagers(REG_BACKUP *REG_values) {
	// ROR code context variables
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyTower = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	long int *pAssignedUnitsCount = (long int *)(REG_values->ESP_val + 0x14);
	AOE_STRUCTURES::STRUCT_POSITION_INFO *pTCPositionInfo = (AOE_STRUCTURES::STRUCT_POSITION_INFO*)(REG_values->ESP_val + 0x20);

	REG_values->fixesForGameEXECompatibilityAreDone = true;

	//bool forceKeepCurrentActivity = false; // Default: change and attack tower
	assert(enemyTower != NULL);
	if (!enemyTower || !enemyTower->IsCheckSumValidForAUnitClass() || !tacAI->IsCheckSumValid()) {
		return; // Error case. Do nothing
	}

	assert(pAssignedUnitsCount >= 0);
	if (*pAssignedUnitsCount >= 6) { // Same as JGE in original code: do not assign more than 6 units at once
		return;
	}
	if (*pAssignedUnitsCount < 0) { // Same as JGE in original code: do not assign more than 6 units at once
		return;
	}

	// Will handle loop on villager and task them to attack the tower (if conditions for that are met).
	ROCKNROR::crCommand.towerPanic_LoopOnVillagers(tacAI, enemyTower, pAssignedUnitsCount, pTCPositionInfo);
}


// Called from 0x4E5001
// ROR's function canConvertUnit has a bug, it returns true when enemy unit is an unfinished building (status == 0).
void RockNRorInstance::ManageBuildingStatus_canConvertUnit(REG_BACKUP *REG_values) {
	long int unitId = REG_values->EAX_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = GetUnitStruct(unitId);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EAX_val = (unsigned long int)targetUnit;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Custom treatment
	if (targetUnit && (targetUnit->unitStatus < 2)) {
		// It is not necessary to test unit type (is a building?) because living units status is always>=2 during game.
		REG_values->EAX_val = 0; // Force canConvertUnit result to false (unit is an unfinished building and can't be converted)
	}
}


// This method is used to slow down (skip timer occurences) for modern computers that are too fast.
// The game does not supports too fast timer occurences.
// In such cases, UI is still working but game time appears to be (almost) frozen.
void RockNRorInstance::ManageGameTimerSkips(REG_BACKUP *REG_values) {
	long int myECX = REG_values->ECX_val;
	long int myEDX = REG_values->EDX_val;
	long int myESI = REG_values->ESI_val;
	long int myESP = REG_values->ESP_val;
	_asm {
		MOV ESI, myESI;
		MOV ECX, DWORD PTR DS:[ESI+0x4];
		MOV EDX, DWORD PTR DS:[ESI+0x58];
		MOV myECX, ECX;
		MOV myEDX, EDX;
	}
	REG_values->ECX_val = myECX;
	REG_values->EDX_val = myEDX;
	REG_values->fixesForGameEXECompatibilityAreDone = true;

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	if (++ROCKNROR::crInfo.gameTimerSlowDownCounter > ROCKNROR::crInfo.configInfo.gameTimerSlowDownFactor) {
		ROCKNROR::crInfo.gameTimerSlowDownCounter = 0;
	}
	else {
		ChangeReturnAddress(REG_values, 0x51D940);
	}

	// TO DO: Not the best place to do that...
	// Can we find a better place to dedicate RockNRor timer things ?
	ROCKNROR::crCommand.OnGameTimer();

}


// Collects information about last execution times and recomputes slow down factor if option is enabled.
// Avoid doing too much treatments here.
// 0x51D9EC
void RockNRorInstance::CollectTimerStats(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *g = (AOE_STRUCTURES::STRUCT_GAME_GLOBAL *) REG_values->ESI_val;
	ror_api_assert(REG_values, g && g->IsCheckSumValid());

	long int interval_ms = REG_values->EAX_val;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = REG_values->EDI_val + interval_ms;
		g->currentGameTime = REG_values->EAX_val;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	
	if (ROCKNROR::crInfo.CollectedTimerIntervalsIndex >= CST_TIMER_STATS_ARRAY_SIZE) { ROCKNROR::crInfo.CollectedTimerIntervalsIndex = 0; }
	ROCKNROR::crInfo.CollectedTimerIntervals_ms[ROCKNROR::crInfo.CollectedTimerIntervalsIndex++] = interval_ms;

	// Auto compute slow down factor if option is enabled and only once every CST_TIMER_STATS_ARRAY_SIZE
	if (ROCKNROR::crInfo.configInfo.gameTimerSlowDownAutoFix && (ROCKNROR::crInfo.CollectedTimerIntervalsIndex == CST_TIMER_STATS_ARRAY_SIZE)) {
		ROCKNROR::crCommand.AutoFixGameTimer();
	}
}



// This overloads the original code that displays Option button in game menu.
// Reads ROCKNROR::crInfo.configInfo.showRockNRorMenu Parameter to display standard menu OR customized menu
void RockNRorInstance::DisplayOptionButtonInMenu(REG_BACKUP *REG_values) {
	long int myESI = REG_values->ESI_val;
	long int myEDI = REG_values->EDI_val;
	long int myEBP = REG_values->EBP_val;
	long int myEAX = 1;
	unsigned long int **pOptionsBtn; // init in asm code below
	_asm {
		MOV ESI, myESI;
		MOV EDI, myEDI;
		LEA EAX, DWORD PTR DS:[ESI+EDI*4+0x490];
		MOV pOptionsBtn, EAX;
	}
	bool showRockNRorMenu = ROCKNROR::crInfo.configInfo.showRockNRorMenu && !ROCKNROR::crInfo.configInfo.doNotApplyFixes;

	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		myEDI++;
		REG_values->fixesForGameEXECompatibilityAreDone = true;

		// Standard Options button
		long int xSize = showRockNRorMenu ? 0xA0 : 0x168;
		myEAX = AOE_METHODS::UI_BASE::AddButton((AOE_STRUCTURES::STRUCT_UI_EASY_PANEL*) myESI, (AOE_STRUCTURES::STRUCT_UI_BUTTON**) pOptionsBtn,
			LANG_ID_GAME_OPTIONS, 0x14, myEBP, xSize, 0x1E, 5, AOE_FONTS::AOE_FONT_BIG_LABEL);
	}
	
	// Limit to single player, causes crash on multi.
	assert(ROCKNROR::crInfo.customGameMenuOptionsBtnVar == NULL);
	long int dllIdToUse = LANG_ID_OPTIONS;
	if (localizationHandler.StringExists(CRLANG_ID_ROCKNROR)) {
		dllIdToUse = CRLANG_ID_ROCKNROR;
	}
	if (myEAX && showRockNRorMenu && !IsMultiplayer()) {
		myEAX = AOE_METHODS::UI_BASE::AddButton((AOE_STRUCTURES::STRUCT_UI_EASY_PANEL*)myESI, 
			&ROCKNROR::crInfo.customGameMenuOptionsBtnVar, 
			dllIdToUse, 0xD0, myEBP, 0xAC, 0x1E,
			AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_CUSTOM_OPTIONS, AOE_FONTS::AOE_FONT_BIG_LABEL);
	}
	REG_values->EDI_val = myEDI; // Important for navigating with arrows in menu.
	REG_values->EAX_val = myEAX; // Return code
}


// From 0x0043424D
// WARNING: this is NOT called when those buttons are clicked: quit game, load, save, help
// Manage onclick on our custom button in menu.
// Opens a custom popup (created like game's option menu)
void RockNRorInstance::ManageOptionButtonClickInMenu(REG_BACKUP *REG_values) {
	long int myEAX = REG_values->EAX_val;
	long int myESP = REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_ANY_UI *previousPopup = (AOE_STRUCTURES::STRUCT_ANY_UI *)REG_values->ESI_val;

	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (myEAX == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_OPTIONS) { return; } // Corresponds to original code (click on original options menu)

	ChangeReturnAddress(REG_values, 0x004342AF); // Corresponds to the JNZ we removed in original code

	if (myEAX != AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_CUSTOM_OPTIONS) { return; }

	if (!ROCKNROR::crInfo.configInfo.showRockNRorMenu) { return; } // In theory this should be useless because we shouldn't come here if it is disabled
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }

	ChangeReturnAddress(REG_values, 0x004342A7);

	// Now manage the case when the clicked button is our custom button...
	if (!AOE_METHODS::UI_BASE::CloseScreenAndDestroy("Menu Dialog")) {
		return;
	}
	AOE_METHODS::UI_BASE::SetCurrentPanel("Game Screen", 0);
	ROCKNROR::UI::InGameRockNRorOptionsPopup *popup = new ROCKNROR::UI::InGameRockNRorOptionsPopup();
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (settings && settings->IsCheckSumValid() && settings->ptrGameUIStruct && settings->ptrGameUIStruct->IsCheckSumValid()) {
		popup->SetBackgroundTheme((AOE_CONST_DRS::AoeScreenTheme)settings->ptrGameUIStruct->themeSlpId);
		if (settings->previousPauseIsUserPause) { // we use this flag because we are currently in "game menu" context
			// If game is already paused, then do not unpause when the popup is closed
			popup->afterCloseGamePausePolicy = ROCKNROR::UI::RnrScreenBase::AfterClosePausePolicy::NONE;
		}
	}
	popup->CreateScreen(AOE_METHODS::UI_BASE::GetCurrentScreen());
}


// From 0x460989. Does not change return address
// DialogPanel.destructor() => called for all dialogPanel children, at destruction time.
void RockNRorInstance::DialogPanelDestructor(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_DIALOG_BASE *dialog = (AOE_STRUCTURES::STRUCT_UI_DIALOG_BASE *)REG_values->ESI_val;
	ror_api_assert(REG_values, dialog && dialog->IsCheckSumValidForAChildClass());
	unsigned long int previousChecksum = dialog->checksum; // useful to identify the dialog type
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		dialog->checksum = CHECKSUM_UI_DIALOG_BASE; // Original instruction
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }

	// Make sure to always free the custom "rocknror options" button when closing a dialog. 
	// In theory, this can only happen when closing ingame menu.
	// As the menu dialog destructor WILL be called, we are sure to free the button in all possible cases.
	if (ROCKNROR::crInfo.customGameMenuOptionsBtnVar != NULL) {
		CallAOEDeleteDialog((unsigned long int **)&ROCKNROR::crInfo.customGameMenuOptionsBtnVar);
		assert(ROCKNROR::crInfo.customGameMenuOptionsBtnVar == NULL);
	}
}


// From 0x499134
// This is called at the end of scenario editor UI creation.
void RockNRorInstance::AfterScenarioEditorCreation(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)REG_values->ESI_val;
	ror_api_assert(REG_values, scEditor != NULL);
	ror_api_assert(REG_values, scEditor->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->ECX_val = *(long int*)(REG_values->ESP_val + 0x1CC);
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }
	// Custom actions
	ROCKNROR::crCommand.CustomScenarioEditorUICreation(scEditor);
}


// From 0x4926B0
void RockNRorInstance::ScenarioEditorChangeSelectedTerrain(REG_BACKUP *REG_values) {
	long int arg1 = *(long int *)(REG_values->ESP_val + 4);
	long int selectedIndex = arg1 & 0xFFFF;
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)REG_values->ECX_val;
	ror_api_assert(REG_values, scEditor && scEditor->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EDX_val = arg1;
		REG_values->EAX_val = selectedIndex;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	
	// Custom treatments
	if (!ROCKNROR::crInfo.configInfo.showHiddenTerrainsInEditor || ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }
	long int customTerrainIdToUse = -1;
	customTerrainIdToUse = ROCKNROR::crCommand.GetTerrainIdForSelectedTerrainIndex(scEditor, selectedIndex);

	if (customTerrainIdToUse > -1) {
		// Force terrainId to use / skip hardcoded switch from ROR original code
		REG_values->EAX_val = customTerrainIdToUse;
		ChangeReturnAddress(REG_values, 0x4926FD);
	}
}


// From 0x451DCA
// This is executed when a combobox' list loses focus (and hides), ONLY when the combobox has a "previous" capture UI object (unsure about exact role).
void RockNRorInstance::OnComboboxTransferCaptureToPreviousObject(REG_BACKUP *REG_values) {
	ror_api_assert(REG_values, REG_values->EDI_val == ADDR_VAR_PANEL_SYSTEM);
	//AOE_STRUCTURES::STRUCT_UI_MAIN_INFO *uiMainInfo = (AOE_STRUCTURES::STRUCT_UI_PANEL_SYSTEM *)REG_values->EDI_val;
	// EDI + 8 should point to combobox
	// EDI + C should point to current (main) screen.
	AOE_STRUCTURES::STRUCT_ANY_UI *objToGiveCapture = (AOE_STRUCTURES::STRUCT_ANY_UI *)REG_values->ESI_val;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		unsigned long int p = REG_values->EDI_val;
		const unsigned long int addr = 0x451CC0;
		_asm {
			MOV ECX, p;
			PUSH 0;
			CALL addr;
		}
	}
}


// 0x4ED189
// Called each time a living unit is created.
// Don't forget to check player (gaia?), unit type...
// Warning: this is called by *scenario editor*, *game init* and *in-game unit spawn*
// NOT called when loading a saved game...
void RockNRorInstance::OnLivingUnitCreation(REG_BACKUP *REG_values) {
	// Now get informations and check them
	AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unit = (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE*)REG_values->EAX_val;
	ror_api_assert(REG_values, unit != NULL); // TO DO: Could just return ? Leave that for testing purpose for now...
	AOE_STRUCTURES::STRUCT_PLAYER *player = unit->ptrStructPlayer;
	ror_api_assert(REG_values, player != NULL);
	ror_api_assert(REG_values, unit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, unit->DerivesFromTrainable());
	ror_api_assert(REG_values, player->IsCheckSumValid());
	
	ror_api_assert(REG_values, GetGameSettingsPtr() != NULL);
	bool isInGame = (GetGameSettingsPtr()->currentUIStatus == 4); // Use this information if needed;

	long int *myESP = (long int *)REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_ACTION_MAKE_OBJECT *actionStruct = NULL;
	if (myESP[5] == 0x4B541F) {
		// Unit creation comes from an action struct execution: get its information
		// In that case, at the (calling) method beginning, ESI was=actionStruct and has been pushed in stack (and is just about to be pop'ed).
		// So we can get it in ESP position in the stack.
		actionStruct = (AOE_STRUCTURES::STRUCT_ACTION_MAKE_OBJECT *) (myESP[0]);
	}	
	if (actionStruct) {
		if (!actionStruct->IsCheckSumValid() || (actionStruct->actionTypeID != AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_MAKE_OBJECT)) {
			assert(false && "action could not be retrieved");
			actionStruct = NULL; // The value we read is not an "MakeObject" actionstruct, ignore it.
		}
	}

	// Get information about UI status: is this editor, game loading, playing... ?
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	ror_api_assert(REG_values, settings != NULL);
	ror_api_assert(REG_values, settings->IsCheckSumValid());
	AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS currentStatus = settings->currentUIStatus;
	
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Now do custom code
	ROCKNROR::crCommand.OnLivingUnitCreation(currentStatus, unit, actionStruct);
}


// From 0x501980
// In game code, this method has treatments for events 7E(farm depleted) 7C(trainQueue) 7D(missing house) 64 (researchDone) 66,69 (spawn/build complete)
// 68,6B,1-3(errors) 4-6 (player lost/disconnect...) 7 (tribute) 8 (dipl change) 6C 6D 6E(wonders) 72-75(relics/ruins events) 76-7B (convert events).
// May change return address to 0x502816 to disable ROR treatments
void RockNRorInstance::OnGameSettingsNotifyEvent(REG_BACKUP *REG_values) {
	unsigned long int returnAddressDiableTreatments = 0x502816;
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = (AOE_STRUCTURES::STRUCT_GAME_SETTINGS *)REG_values->ECX_val;
	ror_api_assert(REG_values, settings == GetGameSettingsPtr());
	long int *myESP = (long int *)REG_values->ESP_val;
	ror_api_assert(REG_values, myESP != NULL);
	long int eventId = myESP[1];
	long int playerId = myESP[2];
	long int arg3 = myESP[3];
	long int arg4 = myESP[4];
	long int arg5 = myESP[5];
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EAX_val = eventId - 1; // Overwritten code in original code. MOV EAX,DWORD PTR SS:[ESP+4] and DEC EAX
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	AOE_CONST_INTERNAL::GAME_EVENT_TYPE eventType = (AOE_CONST_INTERNAL::GAME_EVENT_TYPE)eventId;

	// Custom code
	if (ROCKNROR::crCommand.EntryPoint_GameSettingsNotifyEvent(eventType, (short int)playerId, arg3, arg4, arg5)) {
		// The method returns true when original treatments must be skipped
		ChangeReturnAddress(REG_values, returnAddressDiableTreatments);
	}
}


// From 0x0507F75 (scenarioInfo.applyDisabledResearches(player))
// This is only executed for scenarios, not DM/RM !
void RockNRorInstance::OnGameInitDisableResearchesEvent(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *playerResearchInfo = (AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *)REG_values->ECX_val;
	ror_api_assert(REG_values, playerResearchInfo != NULL);
	if (REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// Custom treatments
		ROCKNROR::TRIGGER::OnGameInitDisableResearchesEvent(playerResearchInfo);
	}

	// Do not modify below. Make sure this is always executed (do not RETURN before)
	const unsigned long int updateResearchStatuses = 0x4EB38D;
	_asm {
		MOV ECX, playerResearchInfo;
		CALL updateResearchStatuses;
	}
}


// From 0x507FC5
// This is called in scenarioInfo.applyDisabledResearches(player) method.
void RockNRorInstance::OnScenarioInitPlayerBadInitialAgeApplication(REG_BACKUP *REG_values) {
	// There is nothing to do for compatibility
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	// Custom treatment is to disable call to player.applyStartingAge. This is done by overriding it by a call to ROR_API.
	//long int playerStartingAge = REG_values->EAX_val;
}


// From 0x50BB2A
// This is called in global.newGameInit method, after settings "global" initial age.
// This is called before settings various player resources / and score information.
// We use it to fix scenario player-specific starting age (technology tree has already been applied, here) in scenario.
// It can be used as an entry point too.
// Please note this is call for ALL game types (NOT for load game though)
void RockNRorInstance::OnGameInitAfterSetInitialAge(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	ror_api_assert(REG_values, settings && settings->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->ECX_val = (unsigned long int) settings;
	}
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = (AOE_STRUCTURES::STRUCT_GAME_GLOBAL *) REG_values->EBP_val;
	ror_api_assert(REG_values, global && global->IsCheckSumValid());
	long int playerTotalCount = global->playerTotalCount;
	long int currentPlayerId = REG_values->ESI_val;
	ror_api_assert(REG_values, currentPlayerId >= 0);
	ror_api_assert(REG_values, currentPlayerId < playerTotalCount);

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }
	// Custom treatments
	ROCKNROR::crCommand.OnGameInitAfterApplyingTechTrees(currentPlayerId);
}


// 0x0462E60. An important entry point as textboxes "keypress" events can't be trapped using standard methods
void RockNRorInstance::OnTextBoxKeyPress(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *textbox = (AOE_STRUCTURES::STRUCT_UI_TEXTBOX *)REG_values->ECX_val;
	ror_api_assert(REG_values, textbox != NULL);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EAX_val = *((unsigned long int*)0x7BFAB8);
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	long int keyChar = *((unsigned long int*)(REG_values->ESP_val + 0x04)); // arg1
	// (In custom popups)(why restrict?), RETURN in a textbox loses focus (so it is taken into account)
	if (/*(ROCKNROR::crInfo.HasOpenedCustomGamePopup()) &&*/ (keyChar == VK_RETURN) && (textbox->IsCheckSumValid()) &&
		(textbox->inputType != 7)) { // excludes multiline textboxes
		AOE_METHODS::UI_BASE::SetFocus(textbox->ptrParentObject, NULL);
	}
	// Send notification to custom screen - if any - as standard "keypress" events won't be raised.
	ROCKNROR::UI::RnrScreenBase *rnrScreen = ROCKNROR::crInfo.rnrUIHelper->GetCurrentRnrScreen();
	if (rnrScreen) {
		rnrScreen->OnKeyDown(textbox, keyChar, 1, 0, 0, 0);
		ROCKNROR::crInfo.rnrUIHelper->PurgeClosedScreens();
		rnrScreen = NULL; // No longer use rnrScreen ! (in case it's been closed/deleted)
	}
}


// From 004EFF0F - in player.createUnit(f_posY, f_posX, f_posZ?, DAT_ID, status, f_orientation)
// Called to create units from a scenario file (scenario editor or play scenario)
// NOT called when loading a saved game, never called in-game, never called when creating units in scenario editor.
void RockNRorInstance::PlayerCreateUnit_manageStatus(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	ror_api_assert(REG_values, unit != NULL);
	REG_values->fixesForGameEXECompatibilityAreDone = true; // there is nothing to do for compatibility (CALL ROR_API is added instead of NOPs)
	
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	char iDesiredStatus = *((char*)REG_values->ESP_val + 0x24); // get arg5 (byte)
	AOE_CONST_INTERNAL::GAME_UNIT_STATUS desiredStatus = (AOE_CONST_INTERNAL::GAME_UNIT_STATUS)iDesiredStatus;

	// Custom code
	ror_api_assert(REG_values, unit->IsCheckSumValidForAUnitClass());
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !unit->IsCheckSumValidForAUnitClass() || !unit->unitDefinition) { return; }
	// Only modify behaviour in editor.
	// In real game, we must NOT do such updates because unit would have an invalid state (it is more complex than just changing unitStatus field)

	if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) {
		// Editor only: use shortcut numbers to provide visual custom information (see unit with custom status)
		if (desiredStatus < AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY) {
			unit->shortcutNumber = 1;
		}
		if (desiredStatus > AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY) {
			unit->shortcutNumber = 3;
		}

		// For editor only: buildings with a status >2 are not displayed correctly (because game's code tests "status==2").
		// In this very precise case, force the call that was not done (because of the CMP BL,2 condition).
		// This needs to be done BEFORE we change unit.unitStatus (the call needs status to be =0).
		if ((desiredStatus > AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY) && (unit->unitDefinition->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING)) {
			AOE_METHODS::UNIT::BuildingUnitDoConstruct((STRUCT_UNIT_BUILDING*)unit, 10000.f);
		}
		unit->unitStatus = desiredStatus;
	}
}


// From 0x42C330
// This method does nothing (RETN). In ROR's code, it corresponds to a log method that is only compiled in DEBUG mode.
// ROR method signature is f(void *obj, char *text, args...). obj is unused.
// WARNING: (ROR) Compiler optimization is really a pain here, some calls to this method are NOT RELEVANT / not related to logs.
// We must return (do nothing) for those calls.
void RockNRorInstance::RORDebugLogHandler(REG_BACKUP *REG_values) {
	// If disabled in config, exit and do nothing (default - corresponds to standard behaviour)
	if (ROCKNROR::crInfo.configInfo.collectRORDebugLogs <= 0) { return; }
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }

#ifndef GAMEVERSION_ROR10c
	return;
#endif
	unsigned long int returnAddress = *(unsigned long int*)REG_values->ESP_val;

	if ((returnAddress < AOE_OFFSETS::ADDR_EXE_MIN + 5) || (returnAddress > AOE_OFFSETS::ADDR_EXE_MAX)) {
		return; // invalid address
	}
	// Verify that ROR EXE caller is NOT a class call (NOT like CALL DS:[xxx+yyy]).
	// Only hardcoded calls like CALL 0x42C330 are actual calls to log method (including some false positives)
	if (*(char *)(returnAddress - 5) != (char)0xE8) { return; }

	// Exclude some "hardcoded" calls that are not log calls but destructors or other (empty) methods. (exclude "false positives")
	unsigned long int notLogCalls[] = { 0x41E9A7, 0x425A8D, 0x428253, 0x428B10, 0x428B2A, 
		0x47B7B8, 0x481473, 0x487F1F, 0x491C9D, 0x4D2EBE, 0x4D2F2B, 0x4E7D15};
	for (int i = 0; i < sizeof(notLogCalls); i++) {
		if (returnAddress - 5 == notLogCalls[i]) {
			return;
		}
	}

	// If we arrive here, then it IS a call to log method.
	unsigned long int textStackAddress = REG_values->ESP_val + 8;
	if ((returnAddress - 5 >= 0x4CD2D0) && (returnAddress - 5 <= 0x4CE8E6)) {
		textStackAddress += 4; // One additional argument (before) for those calls (must be a different method)
	}

	if (*(char*)textStackAddress == 0) {
		//assert(false && "Bad argument"); // ?
		return;
	}

	ROCKNROR::crCommand.HandleRORDebugLogCall(textStackAddress);
}


// From 0x40C240. Corresponds to a trace method (disabled), signature is Method(pObject, message, [args]...) (printf-style)
// Warning: do not use arg1 as it can be several different objects (often AI struct, but not always)
void RockNRorInstance::CollectAOEDebugLogsInGame(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = *(long int*)(0x554740); // For MOV EAX,DWORD PTR DS:[554740]
	}
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	if (ROCKNROR::crInfo.configInfo.collectRORDebugLogs < 2) {
		return;
	}
	//AOE_STRUCTURES::STRUCT_AI *ai = *(AOE_STRUCTURES::STRUCT_AI **)(REG_values->ESP_val + 4);
	//ror_api_assert(REG_values, ai && ai->IsCheckSumValid());
	char *message = *(char**)(REG_values->ESP_val + 8); // Method's arg2

	unsigned long int textStackAddress = REG_values->ESP_val + 8;
	ROCKNROR::crCommand.HandleRORDebugLogCall(textStackAddress);
}


// From 0x044E02F, compatible with a call from 0x044DFC6 too.
// Called from ROR method that tests if a villager can find a way to a resource (0x044DFC6) and a way back from resource to deposit building (0x044E02F)
// There is a call to "calcPathForMove" that sometimes fails for no apparent reason, especially on "back" movement calculation.
// The fact that we simulate a movement where unit position <> start position causes the method to fail
// Here we just make a workaround to help path finding method to succeed, so that most relevant gathering target can be selected
void RockNRorInstance::GathererPathFindingReturnToDeposit(REG_BACKUP *REG_values) {
	// As this method is compatible with 2 calls, we set 2 bools to know in which context we're running.
	bool firstCall = (REG_values->return_address == 0x044DFC6); // first call = way from current pos to resource
	bool secondCall = (REG_values->return_address == 0x044E02F); // second call = way back from resource to storage

	long int *pathFindingArgs = (long int *)(REG_values->ESP_val - 4); // please use args[1] to args[15]
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = NULL;
	if (firstCall) {
		targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) REG_values->EDI_val;
	} else {
		targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *) GetUnitStruct(pathFindingArgs[7]);
	}
	assert(actorUnit && actorUnit->IsCheckSumValidForAUnitClass());
	assert(targetUnit && targetUnit->IsCheckSumValidForAUnitClass());
	if (!actorUnit || !targetUnit) { return; }

	AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *actorAsType50 = NULL;
	if (actorUnit->DerivesFromAttackable()) { actorAsType50 = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)actorUnit; }
	AOE_STRUCTURES::STRUCT_PATH *movInfo = NULL;
	assert(actorAsType50 != NULL);

	// This series of checks is for DEBUG compilation mode only
#ifdef _DEBUG
	if (secondCall && actorAsType50) {
		movInfo = &actorAsType50->temp_AI_movementInfo;
		assert((unsigned long int)movInfo == REG_values->EDI_val);
		assert(((float)pathFindingArgs[1]) - movInfo->intermediatePositions[movInfo->intermediatePositionsUsedElementsCount - 1].posY < 1);
		assert(((float)pathFindingArgs[2]) - movInfo->intermediatePositions[movInfo->intermediatePositionsUsedElementsCount - 1].posX < 1);
	}
	if (firstCall) {
		assert(((float)pathFindingArgs[1]) - actorUnit->positionY < 1);
		assert(((float)pathFindingArgs[2]) - actorUnit->positionX < 1);
	}
	if (targetUnit) {
		assert(((float)pathFindingArgs[3]) - targetUnit->positionY < 1);
		assert(((float)pathFindingArgs[4]) - targetUnit->positionX < 1);
		assert(pathFindingArgs[7] == targetUnit->unitInstanceId);
	}
	assert(pathFindingArgs[5] == REG_values->ESI_val);
#endif

	if (firstCall) {
		// At this point we don't have(need) a fix for first call (it doesnt seem to fail too much)
		REG_values->EAX_val = AOE_METHODS::CallFindPathForUnit(pathFindingArgs);
		return;
	}
	// For second call, there are lots of failure due to start position <> unit position.
	// Use a trick to prevent that.
	if (secondCall && actorAsType50) {
		// Note: this takes into account ROCKNROR::crInfo.configInfo.doNotApplyFixes
		REG_values->EAX_val = ROCKNROR::crCommand.GathererCheckPathFinding(actorAsType50, pathFindingArgs);
	}
}


// From 0x4A78C0 : in display unit method, where shortcut numbers are managed.
// - JUMP to 0x4A792C (default) = do NOT show shortcut (managed in ROR modified code = no need to change return address)
// - JUMP to 0x4A78DB = show standard shortcut(only for 1 - 9; will crash otherwise)
// To show custom shortcut, do the CALL 0x516390 in RockNRor then JUMP to 0x4A792C.
void RockNRorInstance::ShowUnitShortcutNumbers(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EBP_val;

	const unsigned long int RA_showStandardShortcut = 0x4A78DB; // Return address for "standard shortcut" code.
	const unsigned long int RA_showCustomShortcut = 0x4A78F5; // Return address for "custom shortcut" code.

	REG_values->fixesForGameEXECompatibilityAreDone = true;

	// Here: unit has a custom shortcut OR shortcut 10 (not displayed in standard game)
	long int stack20 = GetIntValueFromRORStack(REG_values, 0x20);
	stack20 = stack20 & 0xFFFF; // It is a word (MOVSX)
	long int stack1C = GetIntValueFromRORStack(REG_values, 0x1C);
	stack1C = stack1C & 0xFFFF; // It is a word (MOVSX)
	long int stack10 = GetIntValueFromRORStack(REG_values, 0x10);
	// This call takes into account ROCKNROR::crInfo.configInfo.doNotApplyFixes flag.
	if (ROCKNROR::crCommand.DisplayCustomUnitShortcutSymbol(unitBase, stack1C, stack20, stack10)) {
		ChangeReturnAddress(REG_values, RA_showStandardShortcut); // standard behaviour if call returned true
	}
}


// From 0x494C76
// Change return address to 0x494CE5 (we replace loop by custom code)
void RockNRorInstance::InitPlayersCivInScenarioEditor(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditorUIMain = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)REG_values->ECX_val;
	char *filename = (char*)GetIntValueFromRORStack(REG_values, 0x7EC); // ROR method's arg1
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		long int *p = (long int *)(REG_values->ESP_val + 0x18);
		*p = REG_values->EAX_val;
		ChangeReturnAddress(REG_values, 0x494CE5);
	}

	ror_api_assert(REG_values, scEditorUIMain);
	ror_api_assert(REG_values, scEditorUIMain->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = scEditorUIMain->global;
	ror_api_assert(REG_values, global);
	ror_api_assert(REG_values, global->IsCheckSumValid());
	short int civCount = global->civCount;
	char curCivId = 1;
	const long int playerNameDllBaseId = 0x2865; // Use civNameDllBaseId + civId...

	short int *myEBP = (short int*)REG_values->EBP_val;
	long int *myEBX = (long int*)REG_values->EBX_val;
	char *myEDI = (char*)REG_values->EDI_val;
	char *myESP = (char*)REG_values->ESP_val;

	char civIdList[8] = { 1, 2, 3, 4, 5, 6, 7, 8 }; // Default values, for default behavior
	bool useFix = !ROCKNROR::crInfo.configInfo.doNotApplyFixes; // *true* unless the "no fix" flag is set

	// Custom part : overload default civilization IDs (1-8) with scenario's ones when loading an existing scenario...
	if (useFix && filename && (*filename != 0)) {
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		ror_api_assert(REG_values, settings);
		ror_api_assert(REG_values, settings->IsCheckSumValid());
		AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = NULL;
		// Get scenario information (read scenario file)
		_asm {
			MOV EDX, 0x417670;
			PUSH 0;
			PUSH filename;
			MOV ECX, settings;
			CALL EDX;
			MOV scInfo, EAX;
		}

		if (scInfo) {
			ror_api_assert(REG_values, scInfo->IsCheckSumValid());
			if (scInfo->IsCheckSumValid()) {
				for (int i = 0; i < 8; i++) {
					civIdList[i] = (char)scInfo->playerCivilizationId[i];
				}
			} else {
				traceMessageHandler.WriteMessage("Scenario info object is invalid");
			}

			// Free scenario information
			_asm {
				MOV ECX, scInfo;
				MOV EDX, DS:[ECX];
				PUSH 1;
				CALL DS:[EDX + 4];
			}
		}
	}

	for (int playerId = 0; playerId < 8; playerId++) {
		curCivId = civIdList[playerId];
		if (curCivId >= civCount) { curCivId = 1; } // 0x494C91->CA1

		GetLanguageDllText(playerNameDllBaseId + playerId, myEDI, 0x41, ""); // 0x494CA9->CB6

		*(myESP + 0x265 + playerId) = 0; // 0x494CBF. 1-byte update (char*).
		*(myESP + 0x26E + playerId) = curCivId; // 0x494CC7. 1-byte update (char*).
		*myEBP = -1; // 0x494CCE
		*myEBX = playerId; // 0x494CD4

		myEBP++; // (+2 bytes !)
		myEBX++; // (+4 bytes !)
		myEDI += 0x41;
	}

	REG_values->EBP_val = (unsigned long int)myEBP;
	REG_values->EBX_val = (unsigned long int)myEBX;
	REG_values->EDI_val = (unsigned long int)myEDI;
	REG_values->ESI_val = 8;
}


// From 0x4978AF. The ROR-added code to support roman tileset in icons in editor does not work at all. Fix it.
void RockNRorInstance::FixUnsupportedRomanTileSetInEditorIcons(REG_BACKUP *REG_values) {
	bool useStandardBehavior = ROCKNROR::crInfo.configInfo.doNotApplyFixes; // *false* unless the "no fix" flag is set.
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (useStandardBehavior) {
		REG_values->EDI_val = 1; // This is exactly what original code does.
		return;
	}

	// Custom code
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditorUI = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN*)REG_values->ESI_val;
	ror_api_assert(REG_values, scEditorUI && scEditorUI->IsCheckSumValid());
	ror_api_assert(REG_values, REG_values->EBX_val == 4);
	ror_api_assert(REG_values, REG_values->EDI_val == 4);
	// Overload icon loading for roman tileset because ROR code is total crap (does not load the correct SLP, does not update the correct pointer in editor structure.
	char shpName[] = "btnbldg4.shp";
	AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo = (AOE_STRUCTURES::STRUCT_SLP_INFO *)AOEAlloc(0x20);
	if (slpInfo) {
		SetIntValueToRORStack(REG_values, 0x1D4, 1); // set local var = 1 (step for exception manager?)
		SetIntValueToRORStack(REG_values, 0x10, (unsigned long int)slpInfo); // save pointer (although it seems to be unused)
		long int slpId = AOE_CONST_DRS::SLPID_TILESET_BLD_ICONS_ROR;
		AOE_METHODS::InitSlpInfoFromDrs(slpInfo, slpId, shpName);
		REG_values->EAX_val = (unsigned long int)slpInfo;
	} else {
		REG_values->EAX_val = 0;
	}
	ChangeReturnAddress(REG_values, 0x497900); // After call to slpinfo constructor in ROR code...
}


// From 0x48177A
// Changes return address to 0x481E0A if custom treatments are used or to 0x481792 to use standard AOE code.
void RockNRorInstance::OnGameMainUiInitTilesetRelatedGraphics(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *) REG_values->ESI_val;
	ror_api_assert(REG_values, gameMainUI && gameMainUI->IsCheckSumValid());
	long int sizeX_714 = REG_values->EDX_val;
	ror_api_assert(REG_values, sizeX_714 == gameMainUI->sizeX);
	long int tileset = REG_values->EAX_val;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EBX_val = tileset;
		if (REG_values->EBX_val >= TILESET::allTilesetsInfo.tilesetCount) {
			REG_values->EBX_val = 0; // 0x481790
		}
		gameMainUI->tileset = tileset;
		gameMainUI->tilesetRelatedGraphicsSizeX = sizeX_714;
		ChangeReturnAddress(REG_values, 0x481792);
	}
	bool useCustomTreatments = TILESET::allTilesetsInfo.usesCustomCivs; // Use standard code when no tileset customization is configured

	if (!useCustomTreatments || ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	TILESET::tilesetHandler.InitGameMainUITilesetDependentGraphics(gameMainUI, tileset);
	
	ChangeReturnAddress(REG_values, 0x481F26); // after init of tileset-dependent graphics (overriden part)
}


// From 0x4F87F6. Change return address to 0x4F8801 for custom tilesets.
void RockNRorInstance::OnDisplayBuildingIconInUnitInfoZone(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_BUILDING *unitBld = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)REG_values->ECX_val;
	// Remark: game code is a bit hazardous. It supposes that command attribute==2 makes that unit object IS a building, which may be wrong depending on empires.dat.
	long int unitTileSet = 0;
	if (unitBld->IsCheckSumValid()) {
		// We try to be more secure here. Access the field only if object IS a valid building object (avoid overflow).
		unitTileSet = unitBld->tileset;
	}
	REG_values->EDX_val = unitTileSet; // Always set EDX, this can't do any harm (even if not supposed to be useful for custom tilesets)
	ror_api_assert(REG_values, unitBld->IsCheckSumValidForAUnitClass());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	if ((unitTileSet < 0) || (unitTileSet >= TILESET::allTilesetsInfo.tilesetCount)) {
		traceMessageHandler.WriteMessage(std::string("Error: tileset ") + std::to_string(unitTileSet) + std::string(" is invalid"));
		unitTileSet = 0;
	}
	bool useCustomTreatments = TILESET::allTilesetsInfo.usesCustomCivs; // Use standard code when no tileset customization is configured

	if (!useCustomTreatments || !TILESET::allTilesetsInfo.IsCustomized(unitTileSet) || ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return; // Do not change return address. We're finished here (EDX has been set correctly).
	}
	// We have a custom tileset
	AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo = TILESET::allTilesetsInfo.GetBuildingIconsSlpInfoForTileSet(unitTileSet);
	if (slpInfo) {
		REG_values->EDI_val = (unsigned long int)slpInfo;
		ChangeReturnAddress(REG_values, 0x4F8801); // Skip the MOV EDI,DWORD PTR DS:[EDI+EDX*4]
		return;
	}
	// Not found ? use default icons instead to avoid crash or UI issues
	traceMessageHandler.WriteMessage(std::string("Error: could not get building icons for tileset ") + std::to_string(unitTileSet));
	REG_values->EDX_val = 0;
	// ... And exit in "standard" behaviour to use tileset 0 (do not change return address)
}


// From 0x4A4402. In editorUnitInfoZone.setUnitDef(unitDef, player). Ensure SLPInfo is correct even for custom tilesets
void RockNRorInstance::OnEditorSetBuildingIconInUnitInfoZone(REG_BACKUP *REG_values) {
	long int currentTileset = REG_values->EDX_val;
	AOE_STRUCTURES::STRUCT_UI_EDITOR_UNIT_INFO_ZONE *editorUnitInfoZone = (AOE_STRUCTURES::STRUCT_UI_EDITOR_UNIT_INFO_ZONE*)REG_values->EBP_val;
	AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)REG_values->EAX_val;
	AOE_STRUCTURES::STRUCT_SLP_INFO **iconsForBuildings = (AOE_STRUCTURES::STRUCT_SLP_INFO **)REG_values->ECX_val;
	ror_api_assert(REG_values, unitDef && unitDef->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, editorUnitInfoZone && editorUnitInfoZone->IsCheckSumValid());
	ror_api_assert(REG_values, iconsForBuildings != NULL);
	// Remark: game code is a bit hazardous. It supposes that command attribute==2 makes that unit object IS a building, which may be wrong depending on empires.dat.
	REG_values->EDX_val = 0;
	if (unitDef->IsCheckSumValid()) {
		// We try to be more secure here. Access the field only if object IS a valid building object (avoid overflow).
		REG_values->EDX_val = unitDef->graphicsAngle; // 0x4A4405
	}
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if ((currentTileset < 0) || (currentTileset >= TILESET::allTilesetsInfo.tilesetCount)) {
		traceMessageHandler.WriteMessage(std::string("Error: tileset ") + std::to_string(currentTileset) + std::string(" is invalid"));
		currentTileset = 0;
	}
	bool useCustomTreatments = TILESET::allTilesetsInfo.usesCustomCivs; // Use standard code when no tileset customization is configured

	if (!useCustomTreatments || !TILESET::allTilesetsInfo.IsCustomized(currentTileset) || ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return; // Standard case: correctly handled by ROR code
	}
	
	// Custom tilesets: all we have to do is replace EDI by the correct pointer
	AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo = TILESET::allTilesetsInfo.GetBuildingIconsSlpInfoForTileSet(currentTileset);
	if (slpInfo) {
		REG_values->EDI_val = (unsigned long int)slpInfo;
		return;
	}
	// Not found ? use default icons instead to avoid crash or UI issues
	traceMessageHandler.WriteMessage(std::string("Error: could not get building icons for tileset ") + std::to_string(currentTileset));
	REG_values->EDI_val = (unsigned long int) iconsForBuildings[0];
}


// From 0x4824A9: display command bar icons (building icons for build menu)
void RockNRorInstance::DisplayCommandBarGetSlpInfoForBuilding(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *)REG_values->ESI_val;
	ror_api_assert(REG_values, gameMainUI && gameMainUI->IsCheckSumValid());
	long int tilesetId_fixed = REG_values->EDX_val; // ROR code forced this value to 0 if >5.
	ror_api_assert(REG_values, tilesetId_fixed >= 0);
	REG_values->fixesForGameEXECompatibilityAreDone = true;

	bool useStandardCode = !TILESET::allTilesetsInfo.usesCustomCivs ||
		ROCKNROR::crInfo.configInfo.doNotApplyFixes ||
		(!TILESET::allTilesetsInfo.IsCustomized(tilesetId_fixed) && (tilesetId_fixed < 5));

	if (useStandardCode) {
		if (tilesetId_fixed == 5) {
			tilesetId_fixed = 0; // Missing check in original game code ! All other values are secured
		}
		REG_values->EAX_val = (unsigned long int)gameMainUI->iconsForBuildings[tilesetId_fixed];
		// TOOD: A return is missing here ?
	}

	// Custom treatments: get the REAL (custom) tileset from player structure instead of ROR local variable (which may have been forced to 0)
	// Getting the info directly from player structure avoids us to overload the code in 0x482308.
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetControlledPlayerStruct_Settings();
	if (player && player->IsCheckSumValid()) {
		tilesetId_fixed = player->tileSet;
	}

	AOE_STRUCTURES::STRUCT_SLP_INFO *slpInfo = TILESET::allTilesetsInfo.GetBuildingIconsSlpInfoForTileSet(tilesetId_fixed);
	if (!slpInfo) {
		slpInfo = TILESET::allTilesetsInfo.GetBuildingIconsSlpInfoForTileSet(0); // Safe mode: use first tileset
	}
	REG_values->EAX_val = (unsigned long int)slpInfo;
}


// From 0x4FAA30. TODO: use ROCKNROR::crInfo.configInfo.doNotApplyFixes ?
void RockNRorInstance::WriteF11PopInfoText(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_F11_POP_LABEL *f11panel = (AOE_STRUCTURES::STRUCT_UI_F11_POP_LABEL *)REG_values->ESI_val;
	ror_api_assert(REG_values, f11panel && f11panel->IsCheckSumValid());
	
	char *bufferToWrite = (char*) GetIntValueFromRORStack(REG_values, 0); // arg1=buffer
	char *format = (char *)GetIntValueFromRORStack(REG_values, 4);
	char *localizedText = (char *)GetIntValueFromRORStack(REG_values, 8);
	long int currentPop = GetIntValueFromRORStack(REG_values, 0xC); // arg4
	long int houseMaxPop = GetIntValueFromRORStack(REG_values, 0x10); // arg5
	ROCKNROR::crCommand.WriteF11PopInfoText(f11panel, bufferToWrite, format, localizedText, currentPop, houseMaxPop);
}

// From 0x4C41B3 (in infAI.FindEnemyUnitIdWithinRange(ptrMyReferenceUnit, maxDistance, DATID, DATID, DATID, DATID))
// Change return address to 0x4C41C2 when unitId is invalid (-1) to force ignoring it.
// Indeed, the GetUnitPtr(-1) call might find a unit (eg a doppleganger)
void RockNRorInstance::FixGetUnitStructInTargetSelectionLoop(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *)REG_values->ESI_val;
	long int currentLoopOffset = REG_values->EDI_val;
	long int currentUnitId = REG_values->ECX_val;
	long int loopIndex = GetIntValueFromRORStack(REG_values, 0x10);
	ror_api_assert(REG_values, currentLoopOffset == loopIndex*0x24);
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes && (currentUnitId == -1)) { // This corresponds to bugged case in standard game ==> main ADDED fix.
		REG_values->EAX_val = NULL; // simulates that the GetUnitPtr(...) did not found a valid unit
		ChangeReturnAddress(REG_values, 0x4C41C2);
		return;
	}
	REG_values->ECX_val = (unsigned long int) infAI->ptrMainAI; // Required for call 0x40BAB0
	REG_values->EAX_val = currentUnitId; // modified ROR code PUSHes EAX, not ECX.

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }

	// Optional custom treatments

	// This was commented out because such updates are now handle on unit visibility changes in unitExtensions (restricted to type50+ though)
	// Moreover, this place was not especially relevant for infAI elem updates...
	/*if ((currentUnitId >= 0) && ROCKNROR::IsImproveAIEnabled(infAI->commonAIObject.playerId)) {
		AOE_STRUCTURES::STRUCT_UNIT_MEMORY *unitListElemBase = infAI->unitElemList;
		if (!unitListElemBase || (loopIndex >= infAI->unitElemListSize)) { return; }
		AOE_STRUCTURES::STRUCT_UNIT_MEMORY *currentUnitListElem = &unitListElemBase[loopIndex];
		ROCKNROR::crCommand.OnFindEnemyUnitIdWithinRangeLoop(infAI, currentUnitListElem);
	}*/
}


// From 0x414980
// Change return address to 0x414991 when unitId is invalid (-1) to force ignoring it.
// Indeed, the GetUnitPtr(-1) call might find a unit (eg a doppleganger)
// This method fixes the case when some unit (let's say an archer) keeps shooting at nothing, at a specific position on the map.
void RockNRorInstance::FixUnitIdBugStuckAttackNoTarget(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	ror_api_assert(REG_values, activity && isAValidRORChecksum(activity->checksum));
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	REG_values->EAX_val = activity->targetUnitId;
	REG_values->ECX_val = REG_values->ESI_val;
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; } // not recommended !
	if (activity->targetUnitId == -1) {
		REG_values->EAX_val = NULL; // simulates that the GetUnitPtr(...) did not found a valid unit
		ChangeReturnAddress(REG_values, 0x414991);
		return;
	}
}


// From 0x412F2C
void RockNRorInstance::SetActivityTargetUnitIdBug(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EAX_val;
	long int unitIdToSearch = GetIntValueFromRORStack(REG_values, 0);
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (unitIdToSearch == -1) {
		REG_values->EAX_val = NULL;
	} else {
		REG_values->EAX_val = (unsigned long int) GetUnitStruct(unitIdToSearch);
	}
}


// From 0x4E64B2. May change return address to 0x4E64C7
// Method = retreat after shooting.
void RockNRorInstance::FixActivityTargetUnitIdBug_retreatAfterShooting(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EAX_val;
	ror_api_assert(REG_values, actorUnit && actorUnit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, activity && isAValidRORChecksum(activity->checksum));
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	long int targetUnitId = activity->targetUnitId;
	// Compatibility treatments
	REG_values->EDX_val = targetUnitId;
	REG_values->EAX_val = (unsigned long int)actorUnit->ptrStructPlayer;
	if (targetUnitId == -1) {
		// Here is the fix
		REG_values->EAX_val = 0; // Unit not found
		ChangeReturnAddress(REG_values, 0x4E64C7); // jump AFTER the call that get unit struct. The next test will see NULL value and exit method.
		return; // No target = no need to continue with custom treatments
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	// Custom treatments
	if (!ROCKNROR::COMBAT::ShouldRetreatAfterShooting(activity)) {
		REG_values->EAX_val = 0; // Unit not found
		ChangeReturnAddress(REG_values, 0x4E64C7); // jump AFTER the call that get unit struct. The next test will see NULL value and exit method.
	}
}


// From 0x4E4796.
// Get unit struct for "param" unitId and activity->targetUnitId to respectively EDI and EAX.
// FIX: if unitId is -1, the unit pointer must be NULL.
void RockNRorInstance::FixActivityTargetUnitIdBug_case1F4(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	ror_api_assert(REG_values, activity && isAValidRORChecksum(activity->checksum));
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	long int *pUnitId = (long int*)REG_values->EBX_val;
	ror_api_assert(REG_values, pUnitId != NULL);
	long int unitIdFromStack = GetIntValueFromRORStack(REG_values, 0);
	ror_api_assert(REG_values, *pUnitId == unitIdFromStack);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *paramUnit = NULL;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *activityTargetUnit = NULL;
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) { // Not recommended NOT to apply fix here !
		if (unitIdFromStack != -1) {
			// FIXED First "get unit struct" (0x4E4796)
			paramUnit = GetUnitStruct(unitIdFromStack);
		}
		if (activity->targetUnitId != -1) {
			// FIXED Second "get unit struct" (0x4E47A7)
			activityTargetUnit = GetUnitStruct(activity->targetUnitId);
		}
	}
	// Update registry with correct values (NULL if IDs are -1)
	REG_values->EDI_val = (unsigned long int)paramUnit;
	REG_values->EAX_val = (unsigned long int)activityTargetUnit;
	SetIntValueToRORStack(REG_values, 0x20, (unsigned long int)paramUnit); // cf MOV DWORD PTR SS:[ESP+20],EDI
}


// From 0x50CB64.
// kill* cheat codes crash when player does not exist. This changes return address when player is invalid to force return.
void RockNRorInstance::FixKillXCrashOnUnknownPlayer(REG_BACKUP *REG_values) {
	long int playerIdToKill = REG_values->EDI_val;
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = (AOE_STRUCTURES::STRUCT_GAME_GLOBAL *)REG_values->ESI_val;
	ror_api_assert(REG_values, global && global->IsCheckSumValid());
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) { // not recommended NOT to do this fix !!
		if ((playerIdToKill < 1) || (playerIdToKill >= global->playerTotalCount)) {
			// Invalid player : needs a fix
			ChangeReturnAddress(REG_values, 0x50CB76);
			REG_values->ESI_val = 0; // This will make game EXE jump and exit method properly.
			traceMessageHandler.WriteMessage("kill* cheat code was used with an invalid player. RockNRor prevented the game from crashing.");
			return;
		}
	}
	// Normal case: just get player structure (like original overwritten code)
	REG_values->ECX_val = (unsigned long int)global->ptrPlayerStructPtrTable[playerIdToKill]; // THIS instruction is not secured in game EXE.
}


// From 0x483489. end of ShowUnitComandButtons (in-game screen - begin=0x482290)
void RockNRorInstance::EntryPointOnAfterShowUnitCommandButtons(REG_BACKUP *REG_values) {
	ror_api_assert(REG_values, REG_values->ECX_val == REG_values->ESI_val);
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *)REG_values->ECX_val;
	ror_api_assert(REG_values, gameMainUI && gameMainUI->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		const unsigned long int addr = 0x4834A0;
		_asm {
			MOV ECX, gameMainUI;
			CALL addr;
		}
	}
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	// Custom treatments:
	ROCKNROR::crCommand.AfterShowUnitCommandButtons(gameMainUI);
}


// From 0x481410 - UIGameMain.DoButtonAction(commandId, infoValue, arg3)
// Forcing return address to 0x4815BE allows to skip ROR treatments (this button click will be ignored by ROR)
// WARNING: This method may receive "OnKeyPress" events before the "OnKeyPress" method, for example pressing ESC when a unit is selected (ESC = button shortcut)
// In such situations, the "GameAndEditor_ManageKeyPress" method never receives the event because THIS code already handled it.
void RockNRorInstance::EntryPointOnGameCommandButtonClick(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *)REG_values->ECX_val;
	ror_api_assert(REG_values, gameMainUI && gameMainUI->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EAX_val = *(long int *)AOE_OFFSETS::ADDR_GAME_ACTIONS_ALLOWED;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	bool forceIgnoreThisEvent = false; // Modify this flag if you wish.
	long int arg1 = GetIntValueFromRORStack(REG_values, 4);
	long int arg2 = GetIntValueFromRORStack(REG_values, 8);
	long int arg3 = GetIntValueFromRORStack(REG_values, 0x0C);
	AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID eventCommandId = (AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID)arg1;

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// Do custom treatments here
		forceIgnoreThisEvent = ROCKNROR::crCommand.OnGameCommandButtonClick(gameMainUI, eventCommandId, arg2);
	}

	// Do not modify below.
	if (forceIgnoreThisEvent) {
		ChangeReturnAddress(REG_values, 0x4815BE);
	}
}


// From 0x483710. Fix the transformation of empires.dat button IDs to internal indexes to support more than 1 page
void RockNRorInstance::ROR_GetButtonInternalIndexFromDatBtnId(REG_BACKUP *REG_values) {
	long int rawButtonId = GetIntValueFromRORStack(REG_values, 4);
	long int result = BUTTONBAR::GetButtonInternalIndexFromDatBtnId((char)rawButtonId);
	if (result < 0) {
		result = 0; // Make sure always to return a valid buttonId in ROR code ! 0 is default value in original method.
	}
	REG_values->EAX_val = result;
	REG_values->fixesForGameEXECompatibilityAreDone = true;
}


// From 0x4EBB10. add missing check on techId (check it is >=0)
void RockNRorInstance::FixPlayerNoTechTree_applyTech(REG_BACKUP *REG_values) {
	long int positiveTechId = GetIntValueFromRORStack(REG_values, 8); // Get arg1
	positiveTechId = positiveTechId & 0xFFFF; // Because arg1 is a WORD ! (short int)
	short int rawTechId = (short int)positiveTechId; // rawTechId can be -1
	AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *tdi = (AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *)REG_values->ECX_val;
	ror_api_assert(REG_values, tdi && tdi->IsCheckSumValid());
	if (positiveTechId < 0) {
		positiveTechId = tdi->technologyCount; // this will force detection of invalid techid.
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	REG_values->EAX_val = positiveTechId; // MOVSX EAX,WORD PTR SS:[ESP+8] in 0x4EBB11.

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		STRUCT_PLAYER *player = (STRUCT_PLAYER *) GetIntValueFromRORStack(REG_values, 0x0C); // get arg2
		ROCKNROR::TECHEFFECT::ApplyUnsupportedEffects(player, tdi, (short int)rawTechId);
	}
}


// From 0x410E31 / 0x41124C : auto search target unit method.
void RockNRorInstance::EntryPointAutoSearchTargetUnit(REG_BACKUP *REG_values) {
	bool isFirstLoop = true;
	unsigned long int returnAddress_ignoreThisTarget = 0;
	unsigned long int returnAddress_thisTargetIsAcceptable = 0;
	unsigned long int returnAddress_applyStandardRules_arg1IsFalse = 0;
	// Some setup according to source call
	switch (REG_values->return_address) {
	case 0x410E3C:
		isFirstLoop = true;
		returnAddress_ignoreThisTarget = 0x410F7E;
		returnAddress_thisTargetIsAcceptable = 0x410E97;
		returnAddress_applyStandardRules_arg1IsFalse = 0x410E47;
		break;
	case 0x411257:
		isFirstLoop = false;
		returnAddress_ignoreThisTarget = 0x41147C;
		returnAddress_thisTargetIsAcceptable = 0x4112B2;
		returnAddress_applyStandardRules_arg1IsFalse = 0x411262;
		break;
	default:
		traceMessageHandler.WriteMessage("Invalid source call address in EntryPointAutoSearchTargetUnit");
		return;
	}
	long int arg1 = GetIntValueFromRORStack(REG_values, 0x48);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = arg1; // Not really necessary, but cleaner.
		if (arg1 == 0) {
			// Default behaviour: if arg1 is false, jump (cf JE in original code 410E3D and 411258)
			ChangeReturnAddress(REG_values, returnAddress_applyStandardRules_arg1IsFalse);
			// In our custom treatments, we can change return address again and override this if we want
		}
	}

	// Return here if we don't want custom rules. We will get standard behaviour.
	// Standard behaviour is: ignore buildings with HP=1 for catapults, ignore walls unless arg1==1 and current target IS a wall too.
	if (!ROCKNROR::crInfo.configInfo.useEnhancedRulesForAutoAttackTargetSelection || ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Custom treatments. Apply custom rules to ignore/accept target unit (overloads standard rules for catapults and walls)

	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *potentialTargetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	ror_api_assert(REG_values, activity && isAValidRORChecksum(activity->checksum));
	ror_api_assert(REG_values, potentialTargetUnit && potentialTargetUnit->IsCheckSumValidForAUnitClass());
	if (potentialTargetUnit && potentialTargetUnit->IsCheckSumValidForAUnitClass()) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = potentialTargetUnit->unitDefinition;
		if (unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass()) {
			if (unitDefBase->unitAIType == TribeAIGroupWall) {
				return; // Potential target is a wall: use standard checks/behaviour.
			}
		}
	}
	bool ignoreThisTarget = ROCKNROR::COMBAT::AutoSearchTargetShouldIgnoreUnit(activity, potentialTargetUnit);


	// Do not modify below.
	if (ignoreThisTarget) {
		ChangeReturnAddress(REG_values, returnAddress_ignoreThisTarget);
	} else {
		ChangeReturnAddress(REG_values, returnAddress_thisTargetIsAcceptable);
	}
}


// From 004F8D87. Called only for buildings that have an attack value (towers !)
void RockNRorInstance::EntryPointOnBuildingAttackInfoDisplay(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone = (AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *)REG_values->EBP_val;
	ror_api_assert(REG_values, unitInfoZone && unitInfoZone->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->ESI_val++;
	}
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	// Custom code
	// This updates REG_values->ESI_val if necessary.
	ROCKNROR::crCommand.DisplayCustomBuildingAttackAttributesInUnitInfo(unitInfoZone, REG_values->ESI_val);
}


// From 0x4FF530 = GetLanguageDllString(stringId, buffer, size)
void RockNRorInstance::EntryPointOnGetLocalizedString(REG_BACKUP *REG_values) {
	unsigned long int *p = (unsigned long int *) ((ADDR_VAR_HINST_LANGUAGEX_DLL != 0) ? ADDR_VAR_HINST_LANGUAGEX_DLL : ADDR_VAR_HINST_LANGUAGE_DLL);
#ifdef GAMEVERSION_AOE10b
	unsigned long int returnAddress = 0x4E6D7D; // different code here (don't skip the 3 POP)
#endif
#ifdef GAMEVERSION_AOE10c
	unsigned long int returnAddress = 0x41722D;
#endif
#ifdef GAMEVERSION_ROR10b
	unsigned long int returnAddress = 0x501AE3;
#endif
#ifdef GAMEVERSION_ROR10c
	unsigned long int returnAddress = 0x4FF563;
#endif
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
#if defined(GAMEVERSION_AOE10c)
		REG_values->ECX_val = *p;
#endif
#if defined(GAMEVERSION_ROR10b) || defined(GAMEVERSION_ROR10c)
		REG_values->EAX_val = *p;
#endif
#ifdef GAMEVERSION_AOE10b // different code in this version
		REG_values->EDI_val = GetIntValueFromRORStack(REG_values, 0x18);
		REG_values->EAX_val = GetIntValueFromRORStack(REG_values, 0x10);
#endif
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	bool useCustomLocalization = !ROCKNROR::crInfo.configInfo.doNotApplyFixes; // *true* unless "no fix" option is set
	if (useCustomLocalization) {
#ifdef GAMEVERSION_AOE10b // different code in this version
		long int stringDllId = REG_values->EAX_val;
		char *buffer = (char*)REG_values->ESI_val;
		long int bufferSize =REG_values->EDI_val;
#else
		long int stringDllId = GetIntValueFromRORStack(REG_values, 4);
		char *buffer = (char*)GetIntValueFromRORStack(REG_values, 8);
		long int bufferSize = GetIntValueFromRORStack(REG_values, 0x0C);
#endif
		buffer[bufferSize - 1] = 0; // Just in case.
		bool successfullyFoundString = ROCKNROR::GetLocalizedString(stringDllId, buffer, bufferSize);
		if (successfullyFoundString) {
			ChangeReturnAddress(REG_values, returnAddress); // (RETN 0x0C instruction) Prevent ROR from searching in language(x).dll
		}
	}
}


// From 0x4ADB91. Normal code tests if number of queue items is > 0. If so, current queue is cancelled before adding a new item in queue.
void RockNRorInstance::AllowMultiUnitTypeInQueue(REG_BACKUP *REG_values) {
	short int elemCount = (REG_values->EAX_val & 0xFFFF); // AX / it is a word
	bool forceKeepCurrentQueue = ROCKNROR::crInfo.configInfo.allowMultiQueueing; // Game default = false
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		forceKeepCurrentQueue = false;
	}
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		if (elemCount <= 0) {
			forceKeepCurrentQueue = true; // original instruction (equivalent)
		}
	}

	if (forceKeepCurrentQueue) {
		ChangeReturnAddress(REG_values, 0x04ADBB6); // preserve queue
	}
}


// From 0x4268F8. Occurs when a units receives damage from an attack and dies (HP<1 ; death did not occur yet, HP was just updated)
void RockNRorInstance::EntryPointOnAttackableUnitKilled(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)REG_values->ESI_val;
	ror_api_assert(REG_values, targetUnit != NULL);
	ror_api_assert(REG_values, targetUnit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, targetUnit->DerivesFromAttackable());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		targetUnit->remainingHitPoints = 0; // Original instruction in 0x4268FA
	}

	// Get context information
	long int attacksCount = GetIntValueFromRORStack(REG_values, 0x0C);
	AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK *pAttacksList = (AOE_STRUCTURES::STRUCT_ARMOR_OR_ATTACK *)GetIntValueFromRORStack(REG_values, 0x10);
	float altitudeFactor = GetFloatValueFromRORStack(REG_values, 0x14);
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = (AOE_STRUCTURES::STRUCT_PLAYER *)GetIntValueFromRORStack(REG_values, 0x18);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetIntValueFromRORStack(REG_values, 0x1C);
	ror_api_assert(REG_values, !actorPlayer || actorPlayer->IsCheckSumValid()); // if provided, actor must be valid
	ror_api_assert(REG_values, !actorUnit || actorUnit->IsCheckSumValidForAUnitClass()); // if provided, actor must be valid

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }
	// Custom treatments
	ROCKNROR::UNIT::OnAttackableUnitKilled(targetUnit, actorUnit);
}


// From 0x4F6F96 : player.HoverOnUnit(...).
// May change return address to 0x4F7177 (force return NULL in ROR's method) or 0x4F7179 (return with EAX's current value)
void RockNRorInstance::EntryPointOnHoverOnUnit(REG_BACKUP *REG_values) {
	// Get context and local variables from ROR method.
	GAME_CURSOR *pOutputCursor = (GAME_CURSOR*)REG_values->EBP_val;
	long int *pOutputDllId = (long int *)REG_values->EBX_val;
	UNIT_INTERACTION_ID foundInteraction = (UNIT_INTERACTION_ID)REG_values->EAX_val;
	long int foundHintDllId = REG_values->ESI_val;
	STRUCT_UNIT_BASE *unit = (STRUCT_UNIT_BASE *)GetIntValueFromRORStack(REG_values, 0x18);
	STRUCT_PLAYER *controlledPlayer = (STRUCT_PLAYER *)GetIntValueFromRORStack(REG_values, 0x10);
	long int unitPlayerId = GetIntValueFromRORStack(REG_values, 0x14);
	GLOBAL_UNIT_AI_TYPES unitAIType = (GLOBAL_UNIT_AI_TYPES)GetIntValueFromRORStack(REG_values, 0x30);
	long int unitDefId = GetIntValueFromRORStack(REG_values, 0x20);
	long int totalHP = GetIntValueFromRORStack(REG_values, 0x1C);
	if (unit) { ror_api_assert(REG_values, unit->IsCheckSumValidForAUnitClass()); }
	ror_api_assert(REG_values, controlledPlayer && controlledPlayer->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		if (foundInteraction < 0) {
			ChangeReturnAddress(REG_values, 0x4F7177);
		}
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}
	// Custom treatments
	GAME_CURSOR forcedCursor = (GAME_CURSOR)-1;
	if (ROCKNROR::crCommand.OnHoverOnUnit(unit, controlledPlayer, unitPlayerId, foundInteraction, foundHintDllId, forcedCursor)) {
		REG_values->ESI_val = foundHintDllId;
		REG_values->EAX_val = foundInteraction;
	}
	if (forcedCursor >= 0) {
		REG_values->EAX_val = 1;
		*pOutputCursor = forcedCursor;
		*pOutputDllId = foundHintDllId;
		ChangeReturnAddress(REG_values, 0x4F7179); // Stop treatments and force usage of this cursor.
	}
}


// From 0x51339B
void RockNRorInstance::EntryPointShowInGameDefaultCursor_noUnitUnderMouse(REG_BACKUP *REG_values) {
	GAME_CURSOR cursorToSet = GAME_CURSOR::GC_NORMAL; // Game default behaviour
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		// Compatibility is done below (call to SetGameCursor)
	}

	// Custom treatments (update cursorToSet if necessary)
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (settings) {
		ror_api_assert(REG_values, settings->IsCheckSumValid());
		if (settings->mouseActionType < 0) {
			cursorToSet = BUTTONBAR::BUTTONBAR_CONST::GetCursorForCustomActionType(settings->mouseActionType);
		}
	}

	AOE_METHODS::SetGameCursor(cursorToSet);
}


// From 0x411D3A. Called on unitActivity.stop(bool)
void RockNRorInstance::EntryPointAfterActivityStop(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *unitActivity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	ror_api_assert(REG_values, unitActivity != NULL); // control checksums ? Not sure I have the whole list
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = 1;
	}
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }
	// Custom treatments
	ROCKNROR::crCommand.OnUnitActivityStop(unitActivity);
}


// From 0x40ACDA: diplAI.GetMostDislikedPlayerId(askTributeAmount, askTributePlayerId, bAttackWinningPlayer, attackWinningPlayerFactor)
// This may change return address to 0x40AD8E to bypass standard treatments.
// The returned value (via EDX) is used to ask tribute(?) and is also set in tacAI.targetPlayers = impacts which player "I" will attack.
void RockNRorInstance::EntryPointGetMostDislikedPlayerId(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_DIPLOMACY_AI *diplAI = (AOE_STRUCTURES::STRUCT_DIPLOMACY_AI *)REG_values->EDI_val;
	AOE_STRUCTURES::STRUCT_PLAYER *player = (AOE_STRUCTURES::STRUCT_PLAYER *)REG_values->EAX_val;
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = (AOE_STRUCTURES::STRUCT_GAME_GLOBAL *)REG_values->ECX_val;
	ror_api_assert(REG_values, diplAI && diplAI->IsCheckSumValid());
	ror_api_assert(REG_values, player && player->IsCheckSumValid());
	ror_api_assert(REG_values, global && global->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		if (global->playerTotalCount <= 1) {
			ChangeReturnAddress(REG_values, 0x40AD8E);
		}
	}
	long int snAskTributeAmount = GetIntValueFromRORStack(REG_values, 0x14);
	long int snAskTributePlayerId = GetIntValueFromRORStack(REG_values, 0x18);
	long int attackWinningPlayerFlag = GetIntValueFromRORStack(REG_values, 0x1C);
	long int attackWinningPlayerFactor = GetIntValueFromRORStack(REG_values, 0x20);
	bool skipStandardTreatment = false; // Set it to true to bypass standard ROR calculations of target playerId
	long int mostDislikedPlayerId = -1; // Modify it to impact returned value and set target playerId

	// Custom treatments
	if (ROCKNROR::IsImproveAIEnabled(player->playerId) && !ROCKNROR::crInfo.configInfo.doNotApplyFixes &&
		!CUSTOM_AI::playerTargetingHandler.ForceUseStandardRorTargetPlayerSelection()) {
		// Override the calculation of target player Id (most disliked)
		mostDislikedPlayerId = CUSTOM_AI::playerTargetingHandler.GetMostDislikedPlayer(player, diplAI, snAskTributeAmount, snAskTributePlayerId,
			(attackWinningPlayerFlag != 0), attackWinningPlayerFactor);
		skipStandardTreatment = true;
	}

	// Do not modify below
	if (skipStandardTreatment) {
		REG_values->EDX_val = mostDislikedPlayerId; // no impact if we do NOT change return address.
		ChangeReturnAddress(REG_values, 0x40AD8E); // Exit and return EDX' value (default -1)
	}
}


// From 0x4BFFC5. infAI.findAttackTarget(targetPlayerId, unitGroup, pTacAI_targetInfo, timeGetTimeValue)
// Can change return value to 0x4C0E3C if we chose to override ROR method (and NOT execute it)
// This method searches for a target when AI player has an idle military situation (?) => output is *pTacAI_targetInfo
// Callers are 0x4D4348, 0x4D53F9, 0x4D5BF2 (common parent method=0x4D3AB0=tacAITaskActiveGroups)
void RockNRorInstance::EntryPointInfAIGroupFindMainTarget(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *)REG_values->ESI_val;
	ror_api_assert(REG_values, infAI && infAI->IsCheckSumValid());
	long int targetPlayerId = GetIntValueFromRORStack(REG_values, 0x88); // arg1
	AOE_STRUCTURES::STRUCT_UNIT_GROUP *unitGroup = (AOE_STRUCTURES::STRUCT_UNIT_GROUP *)
		GetIntValueFromRORStack(REG_values, 0x8C); // arg2
	ror_api_assert(REG_values, (unsigned long int)unitGroup == REG_values->EBP_val);
	ror_api_assert(REG_values, REG_values->EBP_val == REG_values->ECX_val);
	ror_api_assert(REG_values, unitGroup && unitGroup->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_TAC_AI_TARGET_INFO *targetInfo = (AOE_STRUCTURES::STRUCT_TAC_AI_TARGET_INFO *)
		GetIntValueFromRORStack(REG_values, 0x90); // arg3
	long int argTimeGetTimeValue = GetIntValueFromRORStack(REG_values, 0x94); // arg4
	ror_api_assert(REG_values, (targetPlayerId >= -1) && (targetPlayerId <= 8)); // Very tolerant. I doubt -1 and 0 are valid
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = unitGroup->commanderUnitId; // same as replaced instruction 0x4BFFC7
	}
	bool noCustomTreatment = false; // set true for debugging (to force DO NOT use custom code)
	if (noCustomTreatment || !ROCKNROR::IsImproveAIEnabled(infAI->commonAIObject.playerId) || ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

#ifndef _DEBUG
	return; // NOT ready yet for release version
#endif

	// Here, custom treatments are enabled
	ChangeReturnAddress(REG_values, 0x4C0E3C);
	AOE_STRUCTURES::STRUCT_UNIT_MEMORY *resultInfAIUnitElem = NULL;

	resultInfAIUnitElem = CUSTOM_AI::unitTargetingHandler.TestFindGroupMainTarget(infAI, targetPlayerId, unitGroup, targetInfo, argTimeGetTimeValue);
	//resultInfAIUnitElem = ROCKNROR::FindGroupMainTarget(infAI, targetPlayerId, unitGroup, targetInfo, argTimeGetTimeValue);

	REG_values->EAX_val = (unsigned long int)resultInfAIUnitElem;
}


// From 0x4D3ABA. tacAI.TaskActiveSoldiers(timeGetTime, allowedtime) before doing anything else
// Can change return address to 0x4D6039 to exit procedure
void RockNRorInstance::EntryPointTacAIHandleActiveGroupsBeforeLoop(REG_BACKUP *REG_values) {
	ror_api_assert(REG_values, REG_values->ECX_val == REG_values->EDI_val);
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ECX_val;
	ror_api_assert(REG_values, (tacAI != NULL) && tacAI->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = tacAI->currentlyProcessedUnitGroupId;
	}
	long int processStartTimeGetTimeValue = GetIntValueFromRORStack(REG_values, 0xE4); // NOT game time ! Cf AOE_METHODS::TimeGetTime()
	long int allowedTime = GetIntValueFromRORStack(REG_values, 0xE8);

	// Custom treatments
	bool forceExitProcedure = false; // Game default

	long int playerId = tacAI->commonAIObject.playerId;
	assert(playerId >= 0);
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		forceExitProcedure = !CUSTOM_AI::customAIHandler.GetCustomPlayerAI(playerId)->unitGroupAI.OnTaskActiveGroupsBegin(tacAI, processStartTimeGetTimeValue, allowedTime);
	}

	// Do not modify below.
	if (forceExitProcedure) {
		ChangeReturnAddress(REG_values, 0x4D6039);
		return;
	}
}


// From 0x4D3B16. tacAI.TaskActiveSoldiers(timeGetTime, allowedtime) at begin of "for each group" loop
// Can change return address to 0x4D6024 to ignore current group
// EntryPointTacAIHandleActiveGroupsBeforeLoop is ALWAYS (once) called before this
void RockNRorInstance::EntryPointTacAIHandleActiveGroups(REG_BACKUP *REG_values) {
	ror_api_assert(REG_values, REG_values->EBP_val == 0);
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->EDI_val;
	AOE_STRUCTURES::STRUCT_UNIT_GROUP *unitGroup = (AOE_STRUCTURES::STRUCT_UNIT_GROUP *)REG_values->ESI_val;
	ror_api_assert(REG_values, (tacAI != NULL) && tacAI->IsCheckSumValid());
	ror_api_assert(REG_values, (unitGroup == NULL) || unitGroup->IsCheckSumValid());
	const unsigned long int skipToNextGroupAddress = 0x4D6024;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		if (unitGroup == NULL) {
			ChangeReturnAddress(REG_values, skipToNextGroupAddress);
		}
	}
	// Standard behavior : NULL group => next loop
	if ((unitGroup == NULL) || (!unitGroup->IsCheckSumValid())) {
		return;
	}
	// Standard behavior : skip some group types & inactives
	if ((unitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_FISHING_SHIP) ||
		(unitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_TRADE_SHIP) ||
		(unitGroup->unitGroupType == UNIT_GROUP_TYPES::CST_UGT_TRANSPORT_BOAT) ||
		(unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_NOT_SET) ||
		(unitGroup->currentTask == UNIT_GROUP_TASK_IDS::CST_UGT_IDLE)) {
		ChangeReturnAddress(REG_values, skipToNextGroupAddress);
		return;
	}

	bool skipStandardTreatments = false; // Default
	// Custom treatments
	long int playerId = tacAI->commonAIObject.playerId;
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes && ROCKNROR::IsImproveAIEnabled(playerId) && CUSTOM_AI::customAIHandler.IsAliveAI(playerId)) {
		skipStandardTreatments = CUSTOM_AI::customAIHandler.GetCustomPlayerAI(playerId)->unitGroupAI.TaskActiveUnitGroup(tacAI, unitGroup);
	}

	// Do not modify below
	if (skipStandardTreatments) {
		ChangeReturnAddress(REG_values, skipToNextGroupAddress);
	}
}


// From 0x4AFBDF=unit.createActivity. This is executed before the method actually starts.
// Change return address to 0x4AFC6E to return and skip ROR treatments.
void RockNRorInstance::EntryPointBeforeUnitCreateActivity(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		// Fix the 2 "PUSHs" values. Previous EAX and EDX values are unused, we use them to PUSH the correct values in ROR code.
		REG_values->EAX_val = -1; // For 0x4AFBE0
		REG_values->EDX_val = 0x53474A; // For 0x4AFBE2
	}
	// Custom treatments
	bool skipRORTreatments = false;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->ECX_val;
	ror_api_assert(REG_values, unit && unit->IsCheckSumValidForAUnitClass());
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) {
		return;
	}
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// The method does not check for existing pointer (and does not free it if necessary). Do it now.
		// Remark: this is not a bug in standard game as this situation never occurs. However, this is a necessary precaution if we want to add custom unitActivity structures.
		if (unit->currentActivity) {
			AOEFree(unit->currentActivity);
			unit->currentActivity = NULL;
		}

		skipRORTreatments = !ROCKNROR::UNIT::OnUnitCreateActivityStruct(unit);
	}

	if (skipRORTreatments || (unit->currentActivity != NULL)) {
		// Make sure to always skip ROR code if currentActivity exists.
		ChangeReturnAddress(REG_values, 0x4AFC6E);
	}
}


// From 0x501640. In gameSettings.showTimings(). (show F5 debug info in game screen)
// Does not change return address. It is possible to disable standard behaviour: just set ECX=NULL.
void RockNRorInstance::OverrideShowF5DebugInfo(REG_BACKUP *REG_values) {
	STRUCT_GAME_SETTINGS *settings = (STRUCT_GAME_SETTINGS *)REG_values->EAX_val;
	ror_api_assert(REG_values, settings && settings->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->ECX_val = (unsigned long int)settings->ptrGameUIStruct;
	}
	bool disableStandardDebugDisplay = false; // Game default

	// Custom code
	if (settings->ptrGameUIStruct && !ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		disableStandardDebugDisplay = ROCKNROR::crCommand.HandleShowDebugGameInfo(settings);
	}

	// Do not modify below.
	if (disableStandardDebugDisplay) {
		REG_values->ECX_val = NULL;
	}
}


// From unit.canTradeWith(unitDefId) in 0x4AFB70 (unit+0x138). Called from 0x40386E(farming), 0x4B6CDF(trading), etc
// Set EAX = 0x2D (CST_UNITID_DOCK) to have ROR function return true, any other value = return false (can't trade with)
void RockNRorInstance::UnitDefProvidesRenewableResource(REG_BACKUP *REG_values) {
	long int targetUnitDefId = REG_values->EAX_val;
	bool resultCanTradeWith = (targetUnitDefId == CST_UNITID_FARM) /* Yes ! */ ||
		(targetUnitDefId == CST_UNITID_DOCK); // This init is game default.
	REG_values->fixesForGameEXECompatibilityAreDone = true;

	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->ECX_val;
	ror_api_assert(REG_values, actorUnit && actorUnit->IsCheckSumValidForAUnitClass());

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		resultCanTradeWith = AOE_STRUCTURES::CanGetRenewableResourceFrom(actorUnit, targetUnitDefId);
	}

	if (resultCanTradeWith) {
		REG_values->EAX_val = CST_UNITID_DOCK;
	}
}


// From 0x4F8F04.
// Always changes return address to 0x4F902A. We overload the necessary "add resource info line" for this portion of ROR code.
// Note: displaying farm food has already been handled (same for "action in progress), we can exclude these cases to avoid showing both info.
void RockNRorInstance::EntryPointDisplayBuildingInfoResource(REG_BACKUP *REG_values) {
	// Change return address to 0x4F902A = do not show resource info using ROR code.
	ChangeReturnAddress(REG_values, 0x4F902A);
	// Collect context information
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	ror_api_assert(REG_values, unit && unit->IsCheckSumValidForAUnitClass());
	AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone = (AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *)REG_values->EBP_val;
	ror_api_assert(REG_values, unitInfoZone && unitInfoZone->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *unitPlayer = unit->ptrStructPlayer;
	ror_api_assert(REG_values, unitPlayer && unitPlayer->IsCheckSumValid());
	REG_values->ECX_val = (unsigned long int)unitPlayer; // Instruction for EXE compatibility
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	
	if (!unit->unitDefinition || !unit->unitDefinition->IsCheckSumValidForAUnitClass()) { return; }
	UNIT_ACTION_ID currentAction = (UNIT_ACTION_ID)GetIntValueFromRORStack(REG_values, 0x1C); // Get "local_1C" = action type.
	bool isMine = (unitPlayer == unitInfoZone->controlledPlayer);
	bool inProgressInfoAlreadyDisplayed = (currentAction == CST_IAI_MAKE_OBJECT) || (currentAction == CST_IAI_MAKE_TECH);

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }
	if (!ROCKNROR::crInfo.configInfo.useImprovedButtonBar && inProgressInfoAlreadyDisplayed) {
		// If buttonbar improvement is OFF, do not show both progress and trade goods (original behaviour, for dock)
		return;
	}

	// Custom code (original code would set unitOffersTradeGoods=(unit == DOCK))
	bool unitOffersTradeGoods = AOE_STRUCTURES::UnitOffersTrading(unit);
	
	if (!isMine && !unitOffersTradeGoods) {
		// Never show resource info on non-owned units, if resource is NOT trade goods.
		return;
	}
	if (isMine && (unit->unitDefinition->DAT_ID1 == CST_UNITID_FARM) && !unitOffersTradeGoods) {
		// Farm food amount has already been written. Return except if there are trade goods to display.
		return;
	}

	if (!unitOffersTradeGoods && (unit->resourceValue <= 0)) {
		// We don't want to show resource value (there is none !) and this is not a "trade building".
		return;
	}

	// Overload the "add line" operation to handle correctly all cases (including trading)
	long int iconId = -1;
	long int amountToDisplay = (long int)unit->resourceValue;
	// This switch could be in a primitive somewhere...
	switch (unit->resourceTypeId) {
	case RESOURCE_TYPES::CST_RES_ORDER_STONE:
		iconId = AOE_CONST_DRS::RT_ICON_STONE; break;
	case RESOURCE_TYPES::CST_RES_ORDER_GOLD:
		iconId = AOE_CONST_DRS::RT_ICON_GOLD; break;
	case RESOURCE_TYPES::CST_RES_ORDER_FOOD:
	case RESOURCE_TYPES::CST_RES_ORDER_MEAT_STORAGE:
	case RESOURCE_TYPES::CST_RES_ORDER_BERRY_STORAGE:
	case RESOURCE_TYPES::CST_RES_ORDER_FISH_STORAGE:
		iconId = AOE_CONST_DRS::UIZ_ICON_FOOD; break; // Food icon
	case RESOURCE_TYPES::CST_RES_ORDER_WOOD:
		iconId = AOE_CONST_DRS::UIZ_ICON_WOOD; break;
	case RESOURCE_TYPES::CST_RES_ORDER_TRADE_GOODS:
		iconId = AOE_CONST_DRS::UIZ_ICON_TRADE;
		amountToDisplay = (long int)unitPlayer->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_TRADE_GOODS);
		break;
	default:
		break;
	}
	if (iconId >= 0) {
		// Note: this updates ESI value (current line index), which is expected.
		AOE_METHODS::UnitInfoZoneAddAttributeLine(unitInfoZone, iconId, 0, amountToDisplay, amountToDisplay, REG_values->ESI_val);
	}
}


// From 0x4F9B7B, in unitInfoZone.refreshIfNeeded().
// Change return address to 0x4F9C7B if unit is a dock (standard code) or a trade building (custom fix)
void RockNRorInstance::EntryPointRefreshTradeGoodsInUnitInfoZone(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->ECX_val;
	ror_api_assert(REG_values, unit && unit->IsCheckSumValidForAUnitClass());
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)REG_values->EBX_val;
	ror_api_assert(REG_values, unitDef && unitDef->IsCheckSumValidForAUnitClass());
	AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone = (AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *)REG_values->ESI_val;
	ror_api_assert(REG_values, unitInfoZone && unitInfoZone->IsCheckSumValid());
	bool refreshNeeded = false;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		refreshNeeded = (unitDef && unitDef->DAT_ID1 == CST_UNITID_DOCK); // Game default
	}

	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// Custom code
		if (AOE_STRUCTURES::UnitDefOffersTrading(unitDef)) {
			refreshNeeded = true;
		}
	}

	// Do not modify below
	if (refreshNeeded) {
		ChangeReturnAddress(REG_values, 0x4F9C7B); // Force refresh
	}
}


// From 0x4C1ABF. Replaces the whole function infAI.searchTradeTargetElem(actorUnitId), returns pointer to infAIElem = trade target.
void RockNRorInstance::EntryPointInfAISearchTradeTargetElem(REG_BACKUP *REG_values) {
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *)REG_values->ECX_val;
	long int actorUnitId = GetIntValueFromRORStack(REG_values, 4);

	// This call takes into account ROCKNROR::crInfo.configInfo.doNotApplyFixes
	AOE_STRUCTURES::STRUCT_UNIT_MEMORY *foundTradeTargetElem = ROCKNROR::UNIT::FindTradeTargetUnitMemoryElem(infAI, actorUnitId);

	// Do not modify below
	REG_values->EAX_val = (unsigned long int)foundTradeTargetElem;
}


// From 0x4B6FA8
void RockNRorInstance::AddRelevantResourceValueWhenTrading(REG_BACKUP *REG_values) {
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	float resourceValue = *((float*)&REG_values->EDX_val); // cast... without conversion
	unsigned long int checksumForCall = REG_values->EAX_val;
	AOE_STRUCTURES::STRUCT_PLAYER *player = (AOE_STRUCTURES::STRUCT_PLAYER *)REG_values->ECX_val;
	ror_api_assert(REG_values, player && player->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_ACTION_TRADE *actionTrade = (AOE_STRUCTURES::STRUCT_ACTION_TRADE *)REG_values->ESI_val;
	ror_api_assert(REG_values, actionTrade && actionTrade->IsCheckSumValid() && 
		(actionTrade->actionTypeID == actionTrade->GetExpectedInternalActionId()));
	long int outResourceId = CST_RES_ORDER_GOLD; // Game default (hardcoded).

	// Custom code
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		AOE_STRUCTURES::STRUCT_UNIT_BASE *actor = actionTrade->actor;
		AOE_STRUCTURES::STRUCT_UNIT_BASE *tradeTarget = actionTrade->targetUnit;
		long int tmp = AOE_STRUCTURES::GetTradeOutResourceId(actor, tradeTarget);
		if (tmp > -1) {
			outResourceId = tmp;
		}
	}

	// Do not modify below
	long int updateGoldCounter = (outResourceId == CST_RES_ORDER_GOLD);
	_asm {
		MOV ECX, player;
		MOV EAX, checksumForCall;
		PUSH updateGoldCounter;
		PUSH resourceValue;
		PUSH outResourceId;
		CALL DS:[EAX+0x78];
	}
}


// From 0x4AFB90: unit.PreserveOwnedResourceWhenKilledBy(unitClass)
// Returns (in REG_values->EAX) a bool, false if killed unit's owned resource should be reset, true otherwise (most cases)
void RockNRorInstance::ShouldPreserveOwnedResourceWhenKilledBy(REG_BACKUP *REG_values) {
	REG_values->EAX_val = 1; // Default result = true
	GLOBAL_UNIT_AI_TYPES attackerUnitClass = (GLOBAL_UNIT_AI_TYPES) GetIntValueFromRORStack(REG_values, 4);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *killedUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->ECX_val;
	ror_api_assert(REG_values, killedUnit && killedUnit->IsCheckSumValidForAUnitClass());
	if (!killedUnit || !killedUnit->IsCheckSumValidForAUnitClass()) { return; }
	ror_api_assert(REG_values, killedUnit->unitDefinition && killedUnit->unitDefinition->IsCheckSumValidForAUnitClass());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	bool result = true;
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		if ((killedUnit->unitDefinition->unitAIType == TribeAIGroupPreyAnimal) || (killedUnit->unitDefinition->unitAIType == TribeAIGroupPredatorAnimal)) {
			result = (attackerUnitClass == TribeAIGroupCivilian) ||
				(attackerUnitClass == TribeAIGroupOther_Dead_Projectile); /* Here is the bug in original code ! (unless there is a reason for this ???) */
		}
	} else {
		// Custom code
		result = AOE_STRUCTURES::ShouldKeepOwnedResourceWhenKilledByClass(killedUnit, attackerUnitClass);
	}

	// Do not modify below
	REG_values->EAX_val = result ? 1 : 0;
}


// From 0x4E471B: villagerActivity.ProcessNotify(TNotify, arg2).
// ROR code already handles 0x20F (escape projectile), 0x1F4 (being attacked), 0x1F9,1FA
// + see also 0x413890 (parent handler)
// It is possible to change return address to 0x4E4784 to return 4, 0x4E49CA to return 3, 0x4E4DAC to return EAX's value
// OBSOLETE and no longer used.
void RockNRorInstance::VillagerActivityProcessNotify(REG_BACKUP *REG_values) {
	AOE_CONST_INTERNAL::ACTIVITY_TASK_ID notifyTaskId = (AOE_CONST_INTERNAL::ACTIVITY_TASK_ID)REG_values->EAX_val;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EAX_val -= 0x1F4;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes || (ROCKNROR::crInfo.configInfo.improveAILevel == 0)) {
		return;
	}
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *unitActivity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ECX_val;
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notify = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *)REG_values->EBX_val;
	ror_api_assert(REG_values, notify && (notifyTaskId == notify->eventId));

	// Custom treatments (removed : using virtual method hooks now)
}


// From 0x4BDFB6, in infAI.addUnitInLists(unitStruct, flagNotInElemUnitList?)
// Standard behaviour is to set EAX with result from call 0x4BE1C0=infAI.isUnitClassTargetableAsResource(unitClassId)
void RockNRorInstance::IsTargetableResourceCallForInfAI(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	ror_api_assert(REG_values, targetUnit && targetUnit->IsCheckSumValidForAUnitClass());
	GLOBAL_UNIT_AI_TYPES unitClass = (GLOBAL_UNIT_AI_TYPES)REG_values->EBX_val;
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *)REG_values->ESI_val;
	ror_api_assert(REG_values, infAI && infAI->IsCheckSumValid());
	ror_api_assert(REG_values, (unitClass >= 0));
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes || !IsImproveAIEnabled(infAI->commonAIObject.playerId)) {
		// This corresponds to overwritten code in ROR executable.
		long int resBuffer;
		const unsigned long int callAddr = 0x4BE1C0;
		long int dword_unitClass = unitClass;
		_asm {
			MOV ECX, infAI;
			PUSH dword_unitClass;
			CALL callAddr;
			MOV resBuffer, EAX;
		}
		REG_values->EAX_val = resBuffer;
		return;
	}

	// Custom treatment
	bool isResource = EconomyAI::IsAITargetableResource(targetUnit);

	// Do not modify below
	REG_values->EAX_val = isResource ? 1 : 0;
}


// This method is shared for several entry points that are very similar.
// From 0x4BBA11 (see unit in manage infAI)
// From 0x4BDD2F (infAI.addUnitInLists(unitStruct, flagNotInElemUnitList?))
// From 0x4C6248 method to analyze, related to diplomacy change ?
// Note: this sets EAX = "bool" result for isArtefactOrTargetableResourceOrCreatable
// The primary goal of this entry point is to add units in gather list in special cases that are not handled natively (alligators, etc)
void RockNRorInstance::SeeUnitIsArtefactOrResourceOrFlagOrCreatable(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *)REG_values->ECX_val; // Common to all cases
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = NULL;
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = NULL;
	GLOBAL_UNIT_AI_TYPES unitClass = (GLOBAL_UNIT_AI_TYPES)-1;

	// Init depends on calling address
	if (REG_values->return_address == 0x4BBA15) {
		unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->ESI_val;
		unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)REG_values->EAX_val;
		unitClass = (GLOBAL_UNIT_AI_TYPES)REG_values->EBX_val;
		ror_api_assert(REG_values, REG_values->ECX_val == REG_values->EBP_val);
	}
	if (REG_values->return_address == 0x4BDD3C) {
		unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
		unitClass = (GLOBAL_UNIT_AI_TYPES)REG_values->EAX_val;
		if (unit) { unitDef = unit->unitDefinition; }
	}
	if (REG_values->return_address == 0x4C624C) {
		// This case needs to be tested
		unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EBX_val;
		unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)REG_values->EDX_val;
		unitClass = (GLOBAL_UNIT_AI_TYPES)REG_values->EBP_val;
	}
	
	ror_api_assert(REG_values, unit && unit->IsCheckSumValidForAUnitClass());
	ror_api_assert(REG_values, unitDef && unitDef->IsCheckSumValidForAUnitClass());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes || !IsImproveAIEnabled(infAI->commonAIObject.playerId)) {
		// Force original code
		long int res = 0;
		long int addr = 0x4BE100;
		long int dword_unitClass = unitClass;
		_asm {
			MOV ECX, infAI; // Not necessary, actually
			PUSH dword_unitClass;
			CALL addr;
			MOV res, EAX;
		}
		REG_values->EAX_val = res;
		return;
	}

	// Custom treatments
	bool resultIsArtefactOrTargetableResourceOrCreatable = EconomyAI::IsArtefactOrTargetableResourceOrCreatable(unit);

	// Do not modify below
	REG_values->EAX_val = resultIsArtefactOrTargetableResourceOrCreatable ? 1 : 0;
}


// From 0x41389B=activityBase.processNotify (begin=0x413890)
// Note: if child class already handles some notifications, this method won't be called.
/*void RockNRorInstance::ActivityBaseProcessNotify(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *)REG_values->EDI_val;
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	long int argNotificationId = REG_values->EAX_val;
	ror_api_assert(REG_values, REG_values->ECX_val == REG_values->ESI_val);
	ror_api_assert(REG_values, activity && activity->IsCheckSumValid());
	ror_api_assert(REG_values, notifyEvent != NULL);
	ror_api_assert(REG_values, notifyEvent->activityId == argNotificationId);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val -= 0x1F4;
	}
}
*/


// From 0x41317B. Overload the update of delay until next "nearby unit detection" for current unit (activity)
void RockNRorInstance::ActivityNearbyUnitDetectionDelayUpdate(REG_BACKUP *REG_values) {
	const long int defaultBaseDelayInOriginalCode = 4000; // Minimum milliseconds delay in standard AOE/ROR game. (0xFA0)
	long int randomPart = REG_values->EDX_val; // The random value between and 999 (milliseconds)
	ror_api_assert(REG_values, (randomPart >= 0) && (randomPart < 1000));
	if (REG_values->fixesForGameEXECompatibilityAreDone) {
		return; // if EDX has already been updated, do nothing.
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		REG_values->EDX_val = randomPart + defaultBaseDelayInOriginalCode;
	}

	// Custom treatments: allow using custom delays (detect more often)
	// Warning: this may greatly affect performance, especially because of adding/updating unit in infAI list (if not optimized bby RockNRor)
	if (ROCKNROR::crInfo.configInfo.unitAIDetectNearbyUnitsMinimumDelay < 1000) {
		randomPart = randomPart / 2; // Force random part in 0-499 when config's delay is low.
	}
	REG_values->EDX_val = randomPart + ROCKNROR::crInfo.configInfo.unitAIDetectNearbyUnitsMinimumDelay;
}


// From 0x426C86 : in unit50+.update(), when visibility to other players changed.
// Make sure that this method does not run too much treatments !!!
void RockNRorInstance::OnAttackableUnitUpdateVisibility(REG_BACKUP *REG_values) {
	STRUCT_UNIT_ATTACKABLE *unit = (STRUCT_UNIT_ATTACKABLE *)REG_values->ESI_val;
	long int posX = REG_values->EAX_val;
	long int posY = REG_values->EDI_val;
	STRUCT_MAP_VISIBILITY_INFO oldVisibilityInfo = *((STRUCT_MAP_VISIBILITY_INFO*)&REG_values->ECX_val);
	STRUCT_MAP_VISIBILITY_INFO newVisibilityInfo = *((STRUCT_MAP_VISIBILITY_INFO*)&REG_values->EBP_val);
#ifdef _DEBUG
	ror_api_assert(REG_values, unit && unit->IsCheckSumValidForAUnitClass() && unit->DerivesFromAttackable());
#endif
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EDX_val = (unsigned long int)(&unit->myVisibleInfoSetEntryForPlayers);
	}
	// Custom treatments
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) { return; }

	for (int iPlayerId = 0; iPlayerId < 9; iPlayerId++) {
		if (IsImproveAIEnabled(iPlayerId)) {
			unsigned short int thisPlayerBit = (1 << iPlayerId);
			bool isVisibleNow = (newVisibilityInfo.fogVisibilityMask & thisPlayerBit) != 0; // bit-and !!!
			bool wasVisibleBefore = (oldVisibilityInfo.fogVisibilityMask & thisPlayerBit) != 0; // bit-and !!!

			// TODO: take care of visibleInFog ? Maybe in unitExtensionHandler (could save the information?)

			if (isVisibleNow && !wasVisibleBefore) {
				ROCKNROR::unitExtensionHandler.AddUpdateUnitInInfAILists(unit, iPlayerId);
			}
			if (wasVisibleBefore && !isVisibleNow) {
				// TODO: what if not visible ? Leave it (we can check easily if tile is visible :)
				// BUT we still should clean infAI list(s) when unit is deleted ? That would be cheating ? Need to think about this
			}
		}
	}
}


// From 0x4B6F65, 0x4F9097. Test if player should benefit from palmyran trading bonus
// (*2 on evaluated distance that serves to estimate the amount of gold received)
// Return: set EAX=0 for default case : all techs OR not palmyra
// Return: set EAX=1 for palmyra if NOT all techs (originally, this case is hardcoded to "civ=palmyra", which is incorrect in all techs games)
void RockNRorInstance::ShouldNotApplyPalmyraTradingBonus(REG_BACKUP *REG_values) {
	bool isFromDockDisplay = (REG_values->return_address == 0x4F909E);
	
	AOE_STRUCTURES::STRUCT_PLAYER *player = (AOE_STRUCTURES::STRUCT_PLAYER *)(isFromDockDisplay ? REG_values->EAX_val : REG_values->ECX_val);
	assert(player && player->IsCheckSumValid());
	bool applyPalmyraBonus = false;

	if (!ROCKNROR::crInfo.myGameObjects.currentGameHasAllTechs && !ROCKNROR::crInfo.myGameObjects.doNotApplyHardcodedCivBonus) {
		applyPalmyraBonus = (player->civilizationId == CST_CIVID_PALMYRA);
	}
	REG_values->EAX_val = applyPalmyraBonus ? 0 : 1;
}


// From 0x4D1BB1 - tacAI.Update()
// May change return address to 0x4D236C or 0x4D2474 (to skip ROR treatments for current task)
void RockNRorInstance::TacAIHandleOneUpdateByType(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *) REG_values->ESI_val;
	ror_api_assert(REG_values, tacAI && tacAI->IsCheckSumValid());
	// safeguard
	if (tacAI->currentAIUpdateType > AI_UPDATE_TYPES::CST_AUT_BUILD_LIST_INSERTIONS) {
		tacAI->currentAIUpdateType = AI_UPDATE_TYPES::CST_AUT_BUILD_LIST_INSERTIONS;
		assert(false && "Bad value for currentAIUpdateType");
	}
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = tacAI->currentAIUpdateType;
	}
	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes || (!ROCKNROR::IsImproveAIEnabled(tacAI->commonAIObject.playerId))) {
		return;
	}
	long int allowedTime = REG_values->EDI_val;
	long int startProcessingTime = REG_values->EBP_val;
	bool runStandardTreatments = true;
	bool currentTaskIsCompleted = true;

	// Custom treatments : modify runStandardTreatments to execute or not ROR's standard procedure.

	runStandardTreatments = !CUSTOM_AI::EconomyAI::RunOneTacAIUpdateTask(tacAI, startProcessingTime, allowedTime);

	// DO NOT MODIFY BELOW
	if (!runStandardTreatments) {
		if (currentTaskIsCompleted) {
			SetIntValueToRORStack(REG_values, 0x10, 1); // set "thisActionTypeIsCompleted" default value in ROR proc context, cf instruction in 0x4D1BBE.
		}
		if (tacAI->currentAIUpdateType == AI_UPDATE_TYPES::CST_AUT_BUILD_LIST_INSERTIONS) {
			ChangeReturnAddress(REG_values, 0x4D2474); // Last ai update type has specific exiting code
		} else {
			ChangeReturnAddress(REG_values, 0x4D236C);
		}
	}
}


// From 0x4B1AF0 = actionBuild.update()
// This specifically overrides the hack when a farm construction finishes.
// In standard game, the build action's "time" field is set to -1 (should remain 0 always).
// In next actionBuild.update() execution, this flag (time==-1) is detected and builder operates a "right click" on farm to become a farmer
// Note: as this hack messes with builder's action, the notification for build action being completed is not triggered, which is a shame.
// Changes return address to...
// - 0x4B19FD to search for nearby constructions to build (status 3=search) = default treatment for most cases
// - 0x4B1B06 to set time=-1 to use farm hack (next call will simulate a "right click" to transform builder into a farmer
// - 0x4B1B0D to return 0 (do not search, do nothing special = will stop) : not a standard case
void RockNRorInstance::ActionBuildUpdateFarmHack(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_BASE *building = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EAX_val;
	AOE_STRUCTURES::STRUCT_ACTION_BUILD *actionBuild = (AOE_STRUCTURES::STRUCT_ACTION_BUILD *)REG_values->ESI_val;
	ror_api_assert(REG_values, building && building->IsCheckSumValidForAUnitClass() && building->unitDefinition);
	ror_api_assert(REG_values, actionBuild && actionBuild->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_UNIT_BASE *builder = actionBuild->actor;
	ror_api_assert(REG_values, builder && builder->IsCheckSumValidForAUnitClass() && builder->ptrStructPlayer);

	// Setup
	bool allowUseRorDirtyFarmHack = true; // Default behavior: allow using ROR's hack for farms
	bool useRorSearchOtherBuildingToConstruct = true; // Default behavior: for other buildings, go to "search"
	bool isFarm = building->unitDefinition && (building->unitDefinition->DAT_ID1 == CST_UNITID_FARM);

	REG_values->fixesForGameEXECompatibilityAreDone = true;
	ChangeReturnAddress(REG_values, 0x4B19FD); // Set to default behavior (non-farm)
	
	if (!ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// Custom treatments (if enabled)
		allowUseRorDirtyFarmHack = false;
		// If builder can turn into a farmer, then don't search for other constructions
		// Remark: if several villagers were building this farm, CanBuilderSwitchToFarmer won't detect them if "gather" commands have not been handled yet (which generally is the case)
		useRorSearchOtherBuildingToConstruct = !AOE_STRUCTURES::CanBuilderSwitchToFarmer(builder, building);
	}


	// DO NOT MODIFY BELOW
	if (isFarm && allowUseRorDirtyFarmHack) {
		// Set action time = -1, next call to update() will execute the hack code
		ChangeReturnAddress(REG_values, 0x4B1B06);
		return;
	}
	if (!useRorSearchOtherBuildingToConstruct) {
		// Custom case: do nothing (do NOT "search" for other nearby constructions to build)
		ChangeReturnAddress(REG_values, 0x4B1B0D);
	}
}


// From 0x4BD7CF : infAI.UpdateAddInUnitMEmory(unitStruct), call to infAI.UpdateAddInUnitMemory(unitID,(word)DATID, (word)unitClass, (int)posY, posX, posZ, playerId, HP, attackAttempts, arg10, float_attack?,float_reloadTime1, float_maxRange, unitStruct)
// Be careful, this method is also called BY rocknror, take care of recursion issues
void RockNRorInstance::OnAddInUnitMemoryList(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *)REG_values->ECX_val;
	ror_api_assert(REG_values, REG_values->EBX_val == REG_values->ECX_val);
	ror_api_assert(REG_values, infAI && infAI->IsCheckSumValid());

	long int unitId = GetIntValueFromRORStack(REG_values, 0);
	unsigned long int callAddress = 0x4BD7E0;
	unsigned long int callResult = 0;
	AOE_STRUCTURES::STRUCT_UNIT_MEMORY *updatedEntry = NULL;
	unsigned long int ptrArgs = REG_values->ESP_val;
	long int test0 = ((long int*)ptrArgs)[0];
	long int test13 = ((long int*)ptrArgs)[13];
	// Perform original call
	_asm {
		MOV EAX, 14; // 14 parameters
		MOV EDX, ptrArgs;
beginLoop:
		DEC EAX;
		MOV EDI, [EDX + EAX * 4];
		PUSH[EDX + EAX * 4];
		CMP EAX, 0;
		JG beginloop;
		MOV ECX, infAI;
		CALL callAddress;
		MOV callResult, EAX;
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	REG_values->EAX_val = callResult;
	updatedEntry = (AOE_STRUCTURES::STRUCT_UNIT_MEMORY *)callResult;

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes || (updatedEntry == NULL)) {
		return;
	}
	// Custom treatments
	long int playerId = infAI->commonAIObject.playerId;
#ifdef _DEBUG
	ror_api_assert(REG_values, unitId == updatedEntry->unitId);
	ror_api_assert(REG_values, (playerId >= 0) && (playerId < 9));
#endif
	if (ROCKNROR::IsImproveAIEnabled(playerId)) {
		// Make sure unitExtensions are informed that a new unitMemory entry has been added for that player
		ROCKNROR::unitExtensionHandler.UpdateUnitMemoryCacheForPlayer(updatedEntry, infAI);
	}
}


// from 0x4199BF (gameSettings.isGamePaused())
// May change return address to 0x4199CB (useful to force a "return true" statement).
void RockNRorInstance::IsGamePaused(REG_BACKUP *REG_values) {
	unsigned long int returnAdressToForceReturn = 0x4199CB;
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = (AOE_STRUCTURES::STRUCT_GAME_SETTINGS *)REG_values->ECX_val;
	ror_api_assert(REG_values, settings && settings->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		// Set EAX == 1 if NOT in editor (analog to original CMP status, 7 statement + following JNZ)
		if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) {
			REG_values->EAX_val = 0;
		} else {
			REG_values->EAX_val = 1;
		}
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		return;
	}

	// Custom code
	bool forceReturnTrue = false;
	bool forceReturnFalse = false;
	bool forceUseDefaultBehaviour = false; // set this to true to prevent customization / let ROR code decide
	bool isAutoScroll = false;
	unsigned long int callerAddr = GetIntValueFromRORStack(REG_values, 0) - 5;
	
#ifdef GAMEVERSION_ROR10c
	if ((callerAddr == 0x419A63) || // set game running
		(callerAddr == 0x41AC57) || // handle activate
		(callerAddr == 0x4801BD)) { // handle game update
		forceUseDefaultBehaviour = true;
	}
	if (callerAddr == 0x51874B) {
		isAutoScroll = true;
	}
	if ((callerAddr< ADDR_EXE_MIN) || (callerAddr > ADDR_EXE_MAX)) {
		// This is a call from RockNRor - or some external DLL
		// Actually, this case does not occur at this point
		forceUseDefaultBehaviour = true;
	}
#endif;

	if (!isAutoScroll && ROCKNROR::crInfo.configInfo.enableInputInPause) {
		forceReturnTrue = true;
	}
	if (isAutoScroll && ROCKNROR::crInfo.configInfo.enableScrollInPause) {
		forceReturnTrue = true;
	}

	// Do not modify below
	if (forceUseDefaultBehaviour) {
		return;
	}
	if (forceReturnTrue) {
		REG_values->EAX_val = 0;
	}
	if (forceReturnFalse) {
		REG_values->EAX_val = 1;
		ChangeReturnAddress(REG_values, returnAdressToForceReturn);
	}
}


// From 0x42A959 (cmdInfo.executeCommand_type3(pCmd) move)
// This overloads the test on "deltaY > 8" that decides to use formation move, or basic move
// This procedure MAY change return address:
// Change return address to 0x42AB97 to use 'forced' formation mode (units keep relative positions)
// Change return address to 0x42AB97 to use single destination mode (all units will go to exact same destination)
// Change return address to 0x42ACC4 to exit ROR procedure with no action (disable ROR treatments)
// Continue to use formation move (units will keep their relative positions)
void RockNRorInstance::ExecMoveCmdUsingFormation(REG_BACKUP *REG_values) {
	unsigned long int returnAdressForceFormation = 0x42A97B;
	unsigned long int returnAdressSingleDestinationMode = 0x42AB97;
	unsigned long int returnAdressDisableRORTreatments = 0x42ACC4;
	const float deltaYComparisonValue = 8.;

	long int unitCount = REG_values->EDI_val;
	AOE_STRUCTURES::COMMAND_MOVE *cmdMove = (AOE_STRUCTURES::COMMAND_MOVE*)REG_values->EBX_val;
	ror_api_assert(REG_values, (cmdMove != NULL) && cmdMove->IsCmdIdValid());
	// Get "local" variables
	float minPosY = GetFloatValueFromRORStack(REG_values, 0x10);
	float maxPosY = GetFloatValueFromRORStack(REG_values, 0x14);
	//float sumPosY = GetFloatValueFromRORStack(REG_values, 0x1C);
	//float sumPosX = GetFloatValueFromRORStack(REG_values, 0x20);
	float minPosX = GetFloatValueFromRORStack(REG_values, 0x2C);
	float maxPosX = GetFloatValueFromRORStack(REG_values, 0x28);

	// Default behavior (set useFormationMove according to "deltaY" value)

	bool disableRORTreatments = false;
	bool useFormationMove = (maxPosY - minPosY) < deltaYComparisonValue; // Don't know if it's < or <=

	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	if (ROCKNROR::crInfo.configInfo.doNotApplyFixes) {
		// This corresponds to the JE... command we replaced
		if (!useFormationMove) {
			ChangeReturnAddress(REG_values, returnAdressSingleDestinationMode);
		}
		return;
	}

	// Custom code. To use "standard" behavior, you can just return.
	if (cmdMove == NULL) { return; }

	switch (ROCKNROR::crInfo.configInfo.currentRightClickMoveFormation) {
	case ROCKNROR::CONFIG::RIGHTCLICK_FORMATION_TYPE::RCT_STANDARD:
		// Do nothing more, choice was calculated before with standard check
		break;
	case ROCKNROR::CONFIG::RIGHTCLICK_FORMATION_TYPE::RCT_ROCKNROR_IMPROVED:
		{
		// Click inside the "box" drawn by selected units = "regroup" = single target. Otherwise, use formation
			float x = cmdMove->posX;
			float y = cmdMove->posY;
			if ((minPosX < x) && (x < maxPosX) && (minPosY < y) && (y < maxPosY)) {
				useFormationMove = false;
			} else {
				useFormationMove = true;
			}
		}
		break;
	case ROCKNROR::CONFIG::RIGHTCLICK_FORMATION_TYPE::RCT_SINGLE_DESTINATION:
		useFormationMove = false;
		break;
	case ROCKNROR::CONFIG::RIGHTCLICK_FORMATION_TYPE::RCT_FORMATION:
		useFormationMove = true;
		break;
	default:
		break;
	}

	// DO NOT MODIFY BELOW - set return value according to our flags
	if (disableRORTreatments) {
		ChangeReturnAddress(REG_values, returnAdressDisableRORTreatments);
		return;
	}
	if (!useFormationMove) {
		ChangeReturnAddress(REG_values, returnAdressSingleDestinationMode);
	} else {
		ChangeReturnAddress(REG_values, returnAdressForceFormation);
	}
}


}

//#pragma warning(pop)
