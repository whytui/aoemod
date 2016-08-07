#include "../include/CustomROR.h"


//#pragma warning(push)
//#pragma warning(disable: 4731) // Allow modifying EBP in assembler code... Required for compliance with some AOE methods. Be careful !
//#pragma warning(disable: 4733) // Allow modifying FS[0]... Required for compliance with some AOE methods. Be careful !

CustomRORInstance::CustomRORInstance() {}


void CustomRORInstance::DispatchToCustomCode(REG_BACKUP *REG_values) {
	assert(REG_values != NULL);
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
	case 0x004B801C:
		this->AfterAddDynamicStratElems(REG_values);
		break;
	case 0x04E2300:
		this->ManagePanicMode(REG_values);
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
	case 0x004604C9:
		this->GlobalOnButtonClick(REG_values);
		break;
	case 0x0041A6C7:
		this->GameAndEditor_ManageKeyPress(REG_values);
		break;
	case 0x004348DE:
		this->ManageOnDialogUserEvent(REG_values);
		break;
	case 0x0051A508:
		this->OnGameRightClickUpEvent(REG_values);
		break;
	case 0x00462E65:
		this->OnTextBoxKeyPress(REG_values);
		break;
	case 0x051A5DA:
		this->OnGameRightClickUpRedCrossDisplay(REG_values);
		break;
	case 0x004D09C2:
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
	case 0x0043136D:
		this->ManageKeyPressInOptions(REG_values);
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
		this->EntryPointOnBuildingInfoDisplay(REG_values);
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
	default:
		break;
	}
#ifdef _DEBUG
	TemporaryEntryPoints(REG_values);
#endif;
}


#ifdef _DEBUG

// This procedure is only for testing purpose.
void CustomRORInstance::TemporaryEntryPoints(REG_BACKUP *REG_values) {
	if (REG_values->return_address == 0x46BA04) {
		unsigned long int *p = (unsigned long int *)0x7BFAE8;
		*p = REG_values->EAX_val; // Original instruction in 0046B9FF
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		long int bufSize = GetIntValueFromRORStack(REG_values, 0x1C); // arg3
		unsigned char *buf = (unsigned char *)REG_values->EDI_val - bufSize; // This calculation is always correct (getting arg2 in stack isn't)
		unsigned long int callerAddr = GetIntValueFromRORStack(REG_values, 0x10) - 5;
		//unsigned char *buf = (unsigned char *) GetIntValueFromRORStack(REG_values, 0x18); // arg2 is wRONG: may have been overwritten (like arg1)
		WriteDebugLogForDeserializedData(callerAddr, buf, bufSize);
	}
}
#endif;


// Initializations done only once, at startup.
// ROR UI has not been initialized yet, DRS files are not loaded, etc.
void CustomRORInstance::OneShotInit() {
	traceMessageHandler.WriteMessageNoNotification(localizationHandler.GetTranslation(CRLANG_ID_DEBUG_INIT, "Debug message system initialized."));
	this->crInfo.configInfo.ReadXMLConfigFile("CustomROR\\customROR.xml");
	this->crInfo.configInfo.ReadCivXMLConfigFile("CustomROR\\customROR_civs.xml");
	this->crCommand.crInfo = &this->crInfo;
	this->crMainInterface.crCommand = &this->crCommand;
	// Do not use crCommand before this line !

	// Note: CheckEnabledFeatures writes to log file
	if (!this->crCommand.CheckEnabledFeatures()) {
		const char *msg = localizationHandler.GetTranslation(CRLANG_ID_WARN_MISSING_FEATURE, "WARNING: Some features are not enabled in game executable. See CustomROR\\CustomROR.log file.");
		if (this->crInfo.configInfo.showAlertOnMissingFeature) {
			MessageBoxA(0, msg, "ROR API", MB_ICONWARNING);
		}
		traceMessageHandler.WriteMessageNoNotification(msg);
	}
	
	// Custom treatments for initialization
	this->crCommand.OneShotInit();
}


// From 0x041AEF5
// To cancel the close operation, change return address to 0x41AF29 and set EAX to 0 BEFORE calling DS:[EAX+0x12C].
void CustomRORInstance::WMCloseMessageEntryPoint(REG_BACKUP *REG_values) {
	bool preventGameFromExiting = false;
	// Do custom treatments NOW or never ;)
	preventGameFromExiting = preventGameFromExiting || this->crMainInterface.FixGamePopupIssuesBeforeGameClose();

	if (preventGameFromExiting) {
		REG_values->EAX_val = 0;
		ChangeReturnAddress(REG_values, 0x41AF29);
		return;
	}

	// Do this only if we choose to actually exit program !
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		unsigned long int myEAX = REG_values->EAX_val;
		unsigned long int myECX = REG_values->ECX_val;
		assert(myEAX != NULL);
		assert(myECX != NULL);
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = (AOE_STRUCTURES::STRUCT_GAME_SETTINGS *)myECX;
		assert(settings->IsCheckSumValid());
		_asm {
			MOV EAX, myEAX
			MOV ECX, myECX
			CALL DWORD PTR DS:[EAX+0x12C]
			MOV myEAX, EAX // result is used in ROR code after return...
		}
		REG_values->EAX_val = myEAX;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

}


// "From" 0x435F74
void CustomRORInstance::ReadTextFromChat(REG_BACKUP *REG_values) {
	// Execute the code we removed in original code (overwritten to call ROR_API)
	// 00435F6F   . E8 ECA70400        CALL 1920x120.00480760
	// The called function needs ECX to be set
	// EAX = @text (remove push EAX from original code, it will be done here)
	long int myEAX = REG_values->EAX_val;
	long int myECX = REG_values->ECX_val;
	char *txt = (char *)myEAX;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		long int callAddr = 0x00480760;
		_asm {
			MOV EAX, myEAX
			PUSH EAX
			MOV ECX, myECX
			CALL callAddr
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	// Now do our own treatments
	if (IsMultiplayer()) { return; }
	this->crCommand.HandleChatCommand(txt);
}


// Do some action each time a game starts, including "load game", scenario, campaign and also MP games.
// Warning: for scenarios, if there is an introduction screen, this method is called at that moment
// (game might not be displayed yet)
void CustomRORInstance::ActionOnGameStart(REG_BACKUP *REG_values) {
	this->crCommand.OnGameStart();
}


// This is called when launching a new game, for all game types: DM / RM /scenario / campaign / MP.
// When loading a saved game, it is called from another address (0x502E25) than standard call (0x51D509)
// When calling this method, game data has not been loaded yet.
// This method should only do data reset and similar stuff.
void CustomRORInstance::InitBeforeGameStart(REG_BACKUP *REG_values, bool isSavedGame) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		if (!isSavedGame) {
			REG_values->ECX_val = (long int)*ROR_gameSettings; // MOV ECX,DWORD PTR DS:[<gameSettings>]
		} else {
			REG_values->ECX_val = *(long int*)(REG_values->EBP_val + 0x3F4); //MOV ECX,DWORD PTR SS:[EBP+3F4]
		}
	}

	this->crCommand.OnNewGame(isSavedGame);
}

// This is called while scenarioInfo structure is read from a file (cf 0x506937)
// Scenario texts (including players strategies) havealready been read, but not other information, be careful.
// WARNING: this method can be called without a game to be started (in scenario menus, to preview scenario info and set options)
// WARNING: at this point, global->scenarioInfo link has NOT bee set ! Use ESI value instead.
void CustomRORInstance::InitScenarioInfoTextData(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		unsigned long int *p = (unsigned long int*)REG_values->ESI_val;
		ror_api_assert(REG_values, p != NULL);
		*p = 0x0054A3C8;
	}

	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scenarioInfo = (AOE_STRUCTURES::STRUCT_SCENARIO_INFO *)REG_values->ESI_val;
	ror_api_assert(REG_values, scenarioInfo && scenarioInfo->IsCheckSumValid());

	this->crCommand.InitScenarioInfoTextData(scenarioInfo);
}


// From 0x04F16BB
// Original (overwritten) code does a TEST AH, 1 + JNZ 004F16E4. We do this with a call to ChangeReturnAddress.
void CustomRORInstance::CheckPopulationCostWithLogistics(REG_BACKUP *REG_values) {
	long int resourceId = REG_values->ECX_val;
	//short int *costPointer = (short int *)REG_values->EDX_val;
	AOE_STRUCTURES::STRUCT_COST *currentCost = (AOE_STRUCTURES::STRUCT_COST *)(REG_values->EDX_val - 2);
	long int currentCostIndex = REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_DEF_UNIT *unitDef = (AOE_STRUCTURES::STRUCT_DEF_UNIT *)(REG_values->EDX_val - 0x14A - (6 * currentCostIndex));
	ror_api_assert(REG_values, unitDef && unitDef->IsCheckSumValid());
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
	if (!this->crInfo.configInfo.fixLogisticsNoHouseBug) { return; }

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
	if ((currentCost->costAmount == 0) && (currentCost->costType == CST_RES_ORDER_POPULATION_HEADROOM) && (unitDef->unitType == GUT_LIVING_UNIT)) {
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
void CustomRORInstance::ComputeConversionResistance(REG_BACKUP *REG_values) {
	// Collect information
	AOE_STRUCTURES::STRUCT_ACTION_CONVERSION *convAction = (AOE_STRUCTURES::STRUCT_ACTION_CONVERSION *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT *target = (AOE_STRUCTURES::STRUCT_UNIT *)REG_values->EDI_val;
	ror_api_assert(REG_values, convAction->IsCheckSumValid());
	ror_api_assert(REG_values, target->IsCheckSumValid());
	ror_api_assert(REG_values, target == convAction->targetUnit);
	AOE_STRUCTURES::STRUCT_UNIT *actor = convAction->actor;
	long int myESP = REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_PLAYER *targetPlayer = target->ptrStructPlayer;
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = actor->ptrStructPlayer;
	AOE_STRUCTURES::STRUCT_DEF_UNIT *targetUnitDef = target->ptrStructDefUnit;
	// Compute resistance
	float resistance = this->crInfo.GetConversionResistance(targetPlayer->civilizationId, targetUnitDef->unitAIType);

	// Save computed resistance to dedicated stack record
	_asm {
		MOV EAX, resistance
		MOV ECX, myESP
		MOV DS:[ECX], EAX
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;

	// For monitoring
	this->crInfo.passiveConversionAttemptsCount[targetPlayer->playerId]++;
	this->crInfo.activeConversionAttemptsCount[actorPlayer->playerId]++;
}


// 0x04B2578 = address when randomness decided a conversion is successful = beginning of actual unit conversion process.
void CustomRORInstance::OnSuccessfulConversion(REG_BACKUP *REG_values) {
	// Do overwritten instructions
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EBX_val = (long int)(*ROR_gameSettings);
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	// Custom treatments
	AOE_STRUCTURES::STRUCT_ACTION_CONVERSION *convAction = (AOE_STRUCTURES::STRUCT_ACTION_CONVERSION *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT *)REG_values->EDI_val;
	ror_api_assert(REG_values, convAction->IsCheckSumValid());
	ror_api_assert(REG_values, targetUnit->IsCheckSumValid());
	ror_api_assert(REG_values, targetUnit == convAction->targetUnit);
	AOE_STRUCTURES::STRUCT_UNIT *actor = convAction->actor;
	ror_api_assert(REG_values, actor != NULL);
	ror_api_assert(REG_values, actor->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = actor->ptrStructPlayer; // Player that HAS converted a unit
	ror_api_assert(REG_values, actorPlayer != NULL);
	ror_api_assert(REG_values, actorPlayer->IsCheckSumValid());

	// Manage functional impacts / fixes about unit conversion
	this->crCommand.OnUnitChangeOwner_fixes(targetUnit, actorPlayer);

	// Conversions Monitoring:
	AOE_STRUCTURES::STRUCT_PLAYER *targetPlayer = targetUnit->ptrStructPlayer; // The victim (if unit has been converted)
	assert(targetPlayer != NULL);
	assert(targetPlayer->IsCheckSumValid());
	this->crInfo.passiveConversionSuccessfulAttemptsCount[targetPlayer->playerId]++;
	this->crInfo.activeConversionSuccessfulAttemptsCount[actorPlayer->playerId]++;

}


// 0x4F2AAB
// This is called from player.AddUnit method
void CustomRORInstance::ManageOnPlayerAddUnit(REG_BACKUP *REG_values) {
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
	AOE_STRUCTURES::STRUCT_UNIT *unit = (AOE_STRUCTURES::STRUCT_UNIT *)myESP[1];
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// Original overwritten code to do now: MOV ESI,ECX and MOV ECX,DWORD PTR SS:[ESP+10] (get arg2)
		REG_values->ESI_val = REG_values->ECX_val;
		REG_values->ECX_val = isNotCreatable; // arg2
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// Now custom treatments
	this->crCommand.OnPlayerAddUnitCustomTreatments(player, unit, (isTempUnit!=0), (isNotCreatable!=0));
}


// 0x472C25
// Customize "pseudo-random" value that provides elevated terrain proportion in generated map.
// In fact the number is not random, it only depends on map seed due to pseudo-random computation algorithm.
// Warning: game code does a "*5" multiplication for Hills map type afterwards.
void CustomRORInstance::MapGen_elevationProportionCalc(REG_BACKUP *REG_values) {
	long int elevationBaseCalcValue;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		_asm {
			MOV EAX, 0x52605D
			CALL EAX
			MOV elevationBaseCalcValue, EAX
		}
		REG_values->EAX_val = elevationBaseCalcValue;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// Custom treatments
	if (!this->crInfo.configInfo.useMapGenerationCustomElevationCalculation) { return; }
	long int *myECX = (long int *)REG_values->EBX_val;
	long int mapType = myECX[0x34 / 4];
	assert(mapType < AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_MAP_TYPES_COUNT);
	assert(mapType >= 0);
	if ((mapType < 0) || (mapType >= AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_MAP_TYPES_COUNT)) { return; }
	float tmpValue = ((float)elevationBaseCalcValue) * this->crInfo.configInfo.mapGenerationCustomElevationFactor[mapType];
	if (mapType == AOE_CONST_FUNC::MAP_TYPE_INDEX::MTI_HILLS) {
		tmpValue = tmpValue / 5; // Because of hardcoded *5 in game code that will be applied afterwards.
	}
	REG_values->EAX_val = (long int)tmpValue;
}


// 0046F5A8
// Replace an ugly buggy method to add compatibility with non-square maps.
// Moreover, it is possible this improves elevation management because there are some "uncertain" algorithms with X positions.
void CustomRORInstance::MapGen_applyElevation(REG_BACKUP *REG_values) {
	if (REG_values->fixesForGameEXECompatibilityAreDone) {
		return;
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	// Collect (and check) arguments
	long int posY = REG_values->EDX_val;
	long int posX = REG_values->EAX_val;
	//long int dist = *(long int*)(REG_values->ESP_val + 0x0C);
	long int dist = GetIntValueFromRORStack(REG_values, 0x0C);
	ror_api_assert(REG_values, dist > 0);
	ror_api_assert(REG_values, posX >= 0);
	ror_api_assert(REG_values, posY >= 0);
	if ((dist <= 0) || (posX < 0) || (posY < 0)) { return; }
	AOE_STRUCTURES::STRUCT_MAPGEN_ELEVATION_INFO *elevInfo = (AOE_STRUCTURES::STRUCT_MAPGEN_ELEVATION_INFO*) REG_values->ECX_val;
	this->crCommand.Fixed_MapGen_applyElevation(posX, posY, dist, elevInfo);
}


// 0x0045E8D7
// This is called from player.RemoveUnit method. Note: player.RemoveUnit is called BY unit.destructor.
// Warning: this method is called in many situations, game might NOT be running.
void CustomRORInstance::ManageOnPlayerRemoveUnit(REG_BACKUP *REG_values) {
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
	AOE_STRUCTURES::STRUCT_UNIT *unit = (AOE_STRUCTURES::STRUCT_UNIT *)myESP[1];
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// Original overwritten code to do now: MOV EAX,DWORD PTR SS:[ESP+8] and MOV ESI,ECX
		REG_values->ESI_val = REG_values->ECX_val;
		REG_values->EAX_val = isNotCreatable;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	this->crCommand.OnPlayerRemoveUnit(player, unit, isTempUnit != 0, isNotCreatable != 0);
}


// Fills strategy element's unitId when construction begins (while in progress)
void CustomRORInstance::FixBuildingStratElemUnitID(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_PLAYER *p = (AOE_STRUCTURES::STRUCT_PLAYER*) REG_values->EDI_val;
	ror_api_assert(REG_values, p != NULL);
	AOE_STRUCTURES::STRUCT_AI *ai = p->ptrAIStruct;
	if (ai) {
		AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = &ai->structBuildAI;
		ror_api_assert(REG_values, buildAI != NULL);
		AOE_STRUCTURES::STRUCT_UNIT *unit = (AOE_STRUCTURES::STRUCT_UNIT *)REG_values->ESI_val;
		ror_api_assert(REG_values, unit != NULL);
		AOE_STRUCTURES::STRUCT_DEF_UNIT *unitDef = unit->ptrStructDefUnit;
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


// From 0x4B8C74
// Test if strategy element is alive for reset purpose (like original code). In addition, resets unitInstanceId if the strategy element is "in progress" (to be detected by next custom tactical update).
// For the algorithm to work well, requires also "FixUnitIdForInProgressBuilding", "ManageTacticalAIUpdate"
// Performance info: The loop first tests strategy element's unitInstanceId, so ROR_API is only called once in the loop, which is a good thing.
void CustomRORInstance::OverloadIsStratElemUnitAlive_ResetElement(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *currentElement = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *) REG_values->ECX_val;
	ror_api_assert(REG_values, currentElement != NULL);
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = (AOE_STRUCTURES::STRUCT_BUILD_AI *) REG_values->EDI_val;
	ror_api_assert(REG_values, buildAI != NULL);
	ror_api_assert(REG_values, buildAI->IsCheckSumValid());

	// Now manage the case when "in-progress" building is destroyed: do not reset it immediatly because it would be re-triggered, destroyed again by same enemy (while HP=1) and so on
	// We just reset unitInstanceId to -1 to show that the unit no longer exists. Next strategy update will detect it and reset it "asynchronously".
	if ((currentElement->inProgressCount > 0) && (currentElement->elementType == AIUCBuilding)) {
		currentElement->unitInstanceId = -1;
		if (currentElement->unitDAT_ID == CST_UNITID_WONDER) {
			assert(buildAI->mainAI && buildAI->mainAI->IsCheckSumValid());
			if ((buildAI->mainAI) && (buildAI->mainAI->IsCheckSumValid())) {
				// Make sure "wonder is being built flag" is reset;
				buildAI->mainAI->structTacAI.myWonderBeingBuiltFlag = 0;
			}
		}
		REG_values->EAX_val = 0; // if in progress, it can't be alive
	} else {
		// This corresponds to original code we overloaded to call ROR_API: return alive_count. If >0, the strategy element will be reset.
		REG_values->EAX_val = currentElement->aliveCount;
	}
}


// 0x00408F92
// Called at the end of buildAI.AddElementsToStrategy(...) ROR method.
// Be careful as it can be called by buildAI.addDynamicStrategyElements(...) which also triggers a ROR_API entry point (afterwards)
void CustomRORInstance::AfterAddElementInStrategy(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// Do overwritten ROR code:
		REG_values->EAX_val = 1;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
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
	
	this->crCommand.AfterAddElementInStrategy(buildAI, posToAdd, nbToCreate);
}


// 0x004B7D84
// Fix a bug in buildAI.addDynamicStrategyElements()
// Warships are never added because the code calls the wrong method (add research instead of add unit)
void CustomRORInstance::FixAutoBuildWarships_addStratElem(REG_BACKUP *REG_values) {
	if (REG_values->fixesForGameEXECompatibilityAreDone) {
		return;
	}

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
			minPosition = FindElementInStrategy(buildAI->mainAI->ptrStructPlayer, AOE_CONST_FUNC::AIUCBuilding, CST_UNITID_MARKET);
		}
		if (minPosition < 0) { // Try tool age...
			minPosition = FindElementInStrategy(buildAI->mainAI->ptrStructPlayer, AOE_CONST_FUNC::AIUCCritical, CST_RSID_TOOL_AGE);
		}
		if (minPosition < 0) { // Try tool age (if non-critical)
			minPosition = FindElementInStrategy(buildAI->mainAI->ptrStructPlayer, AOE_CONST_FUNC::AIUCTech, CST_RSID_TOOL_AGE);
		}
		if (minPosition < 0) { // Dock... ?
			minPosition = FindElementInStrategy(buildAI->mainAI->ptrStructPlayer, AOE_CONST_FUNC::AIUCBuilding, CST_UNITID_DOCK);
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
		PUSH insertPosition
		PUSH retrains
		PUSH unitDAT_ID
		MOV ECX, myESI
		MOV EAX, 0x004B9050 // buildAI.addLivingElementInStrategy(DATID, retrains, insertPosition)
		CALL EAX
		// Result (EAX) is unused
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;

}


// 0x004B7E1E
// Returns strategy element DAT_ID. Returns Town center ID instead of house ID because only TC is supported in algorithm (count the 4-pop housage).
void CustomRORInstance::FixAutoBuildHouse_countHouse(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *) REG_values->ECX_val;
	REG_values->EAX_val = stratElem->unitDAT_ID;
	// Workaround for houses: return TC's DAT_ID so that it will count a 4-population housage.
	if (stratElem->unitDAT_ID == CST_UNITID_HOUSE) {
		REG_values->EAX_val = CST_UNITID_FORUM;
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
}


// 0x004B8017
// This is called at the end of buildAI.addDynamicStrategyElements(). (which adds houses, docks, boats, setgather%)
// This allow modifying/fixing what has just been added dynamically
void CustomRORInstance::AfterAddDynamicStratElems(REG_BACKUP *REG_values) {
	unsigned long int myESP = REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI = (AOE_STRUCTURES::STRUCT_BUILD_AI *) REG_values->ESI_val;
	unsigned long int myECX;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		_asm {
			MOV ECX, DWORD PTR SS:[ESP+0xAC]
			MOV myECX, ECX
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	// Custom treatments: run various fixes on added strategy elements
	FixAutoBuildStrategyElements(buildAI);
}


// From 0051E0DC. Called just before starting to actually save a game to a gmx file.
void CustomRORInstance::EntryPoint_OnBeforeSaveGame(REG_BACKUP *REG_values) {
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

	// Update triggers data before saving, 
	// so that when loading game we will have relevant trigger information (which have already been executed, etc)
	this->crCommand.WriteTriggersFromInternalToGameData(true);
}


// This method replaces the "panic mode" management (add units in strategy when attacked and weak)
// WARNING: This method may change return address (only when forcing usage of ROR panic mode algorithm - not recommended)
void CustomRORInstance::ManagePanicMode(REG_BACKUP *REG_values) {
	// Collect context information/variables
	long int timeSinceLastPanicMode = REG_values->EDX_val; // in seconds
	long int currentGameTime_ms = REG_values->ESI_val; // in ms
	AOE_STRUCTURES::STRUCT_AI *mainAI = (AOE_STRUCTURES::STRUCT_AI *)REG_values->ECX_val;
	assert(mainAI != NULL);
	assert(mainAI->IsCheckSumValid());
	if (!mainAI || (!mainAI->IsCheckSumValid())) { return; }
	long int myESP = REG_values->ESP_val;
	//short int enemyPlayerId = (short int) *(long int *)(myESP + 0x98);
	short int enemyPlayerId = (short int)GetIntValueFromRORStack(REG_values, 0x98);

	if (!this->crCommand.RunManagePanicMode_isUsageOfRORCodeWanted(mainAI, enemyPlayerId, timeSinceLastPanicMode, currentGameTime_ms)) {
		return;
	}
	// if previous function returns true, it means we want to force usage of ROR original panic mode code.
	// So let's do it by forcing return address (skip the JMP 004E254F)
	ChangeReturnAddress(REG_values, 0x4E2307);
}


// From 004C2538
// This ROR method (0x4C1EF0) initializes map build like data to 0x14 
// and disables (0xFF) tiles that are occupied by buildings (and some nearby tiles)
// This replaces a small portion of code that is only executed for building new houses (DATID=0x46)
// This method is NO LONGER required. ManageCityMapLikeComputationCall1 will handle houses better than this.
void CustomRORInstance::ManageCityPlanHouseDistanceFromBuildings(REG_BACKUP *REG_values) {
	if (REG_values->fixesForGameEXECompatibilityAreDone) {
		return;
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	AOE_STRUCTURES::STRUCT_UNIT *otherBuilding = (AOE_STRUCTURES::STRUCT_UNIT *) REG_values->EDI_val;
	ror_api_assert(REG_values, otherBuilding != NULL);
	long int myESP = REG_values->ESP_val;
	// At this point EBP and EAX both are otherBuilding's Y position. We have the choice to use one of them or re-get from EDI. Doesn't change a lot.
	long int *minPosY = (long int *)(myESP + 0x28); // ESP+28 is minPosY
	long int *minPosX = (long int *)(myESP + 0x2C); // ESP+2C is minPosX
	long int *maxPosY = (long int *)(myESP + 0x30); // ESP+30 is maxPosY
	long int maxPosXValue; // Will be written in ESP+34, but later
	// MinPosX will be written in stack from (returned) EAX value in ROR's code.

	AOE_STRUCTURES::STRUCT_DEF_UNIT *defOtherBuilding = otherBuilding->ptrStructDefUnit;
	ror_api_assert(REG_values, defOtherBuilding != NULL);
	// Get relevant distance value according to "reference building" type (DAT_ID)
	float distanceValue = 3; // From original code
	/*float distanceValue = this->crInfo.configInfo.cityPlanHouseDistanceFromOtherBld;
	if (defOtherBuilding->DAT_ID1 == CST_UNITID_FORUM) {
		distanceValue = this->crInfo.configInfo.cityPlanHouseDistanceFromTownCenter;
	}
	if ((defOtherBuilding->DAT_ID1 == CST_UNITID_GRANARY) ||
		(defOtherBuilding->DAT_ID1 == CST_UNITID_STORAGE_PIT)) {
		distanceValue = this->crInfo.configInfo.cityPlanHouseDistanceFromStorageBld;
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

	

// From 004BE8D8 call
// Takes other building into account in temp map like info for city plan evaluation
void CustomRORInstance::ManageCityMapLikeComputationCall1(REG_BACKUP *REG_values) {
	long int *myESP = (long int *) REG_values->ESP_val;
	//long int myECX = *(myESP + (0x1C/4));
	long int myECX = GetIntValueFromRORStack(REG_values, 0x1C);
	long int placementType = REG_values->EBX_val; // arg1
	long int DAT_ID = REG_values->EDX_val; // arg2
	//long int *TCPositionYX = *((long int **)myESP + 0);
	long int *TCPositionYX = (long int *)GetIntValueFromRORStack(REG_values, 0x0);
	//long int *TownLowestPositionYX = *((long int **)myESP + 1);
	long int *TownLowestPositionYX = (long int *)GetIntValueFromRORStack(REG_values, 0x4);
	//long int *TownHighestPositionYX = *((long int **)myESP + 2);
	long int *TownHighestPositionYX = (long int*)GetIntValueFromRORStack(REG_values, 0x8);
	
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// Do original code's call to fill temp map info
		REG_values->ECX_val = myECX; // Replaces "MOV ECX,DWORD PTR SS:[ESP+1C]" from original code 
		_asm {
			MOV ECX, myECX
			PUSH TownHighestPositionYX
			PUSH TownLowestPositionYX
			PUSH TCPositionYX
			PUSH DAT_ID
			PUSH placementType
			MOV EAX, 0x004C1EF0 // infAI.fillTempMapLikeLocationForNewBuilding(placementType,DAT_ID,ptrPosTC,ptrPosTC-maxTownSize,ptrPosTC+maxTownSize)
			CALL EAX
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	//AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = *(AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT **) (myESP + 0x1BC / 4);
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *)GetIntValueFromRORStack(REG_values, 0x1BC);
	ror_api_assert(REG_values, stratElem != NULL);
	ror_api_assert(REG_values, stratElem->IsCheckSumValid());
	ror_api_assert(REG_values, stratElem->unitDAT_ID == DAT_ID);
	//AOE_STRUCTURES::STRUCT_INF_AI *infAI = *(AOE_STRUCTURES::STRUCT_INF_AI **) (myESP + 7);
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *) GetIntValueFromRORStack(REG_values, 0x1C);
	ror_api_assert(REG_values, infAI != NULL);

	this->crCommand.ManageCityPlanOtherBuildingsImpact(infAI, stratElem, (AOE_STRUCTURES::STRUCT_POSITION_INFO*)TCPositionYX);
}


// From 004C1CAF call
// Does nothing but normal behaviour yet...
void CustomRORInstance::ManageCityMapLikeComputationCall2(REG_BACKUP *REG_values) {
	long int *myESP = (long int *)REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *) REG_values->EBP_val;
	long int *TCPositionYX = *((long int **)myESP + 1);
	long int *TownLowestPositionYX = *((long int **)myESP + 2);
	long int *TownHighestPositionYX = *((long int **)myESP + 3);

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
				traceMessageHandler.WriteMessage("CustomROR just fixed missing map dimensions in AI data.");
			}
		}
		return;
	}

	// Do original code's call to fill temp map info
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		_asm {
			MOV ECX, infAI // in original code: MOV ECX,EBP
				PUSH TownHighestPositionYX
				PUSH TownLowestPositionYX
				PUSH TCPositionYX
				PUSH - 1 // arg2=DAT_ID
				PUSH 0 // arg1=placementType
				MOV EAX, 0x004C1EF0
				CALL EAX
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
}


// This manages the like value to add in a bush's neighborhood when trying to build a granary.
void CustomRORInstance::ManageCityMapLikeValueAroundBushes(REG_BACKUP *REG_values) {
	long int *likeValueToAdd = ((long int *)REG_values->ESP_val);

	// Original overwritten code "MOV EAX,DWORD PTR SS:[ESP+0x34]"
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		long int *savedMinPosX = (long int *)(REG_values->ESP_val + 0x38); // 0x38 because there is now 1 more PUSH in stack (with custom code)
		REG_values->EAX_val = *savedMinPosX;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	// Set the "like value". A hardcoded value has been push'ed, now update with the real value we want to use
	*likeValueToAdd = this->crInfo.configInfo.cityPlanBerryBushWeightForGranary;
}


// From 004C27D1 : this code is dedicated to farms placement mode.
// Changing return address to 0x4C2A86 will make the loop continue, ignoring current (existing) building
// REG_values->EAX_val (both input and output in this method) is influence distance for current existing building. We can set it to a custom value if we want to.
void CustomRORInstance::ManageCityMapLikeValueFarmPlacement(REG_BACKUP *REG_values) {
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
	this->crCommand.FixCityPlanFarmPlacement(unitExistingBld_base, existingBldInfluenceZone, skipThisBuilding);
	REG_values->EAX_val = existingBldInfluenceZone;

	if (skipThisBuilding) {
		ChangeReturnAddress(REG_values, 0x4C2A86);
		return;
	}
}


void CustomRORInstance::OverloadSetInProgress(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *) REG_values->EBX_val;
	ror_api_assert(REG_values, stratElem != NULL);
	AOE_STRUCTURES::STRUCT_UNIT *actor = (AOE_STRUCTURES::STRUCT_UNIT *) REG_values->EDI_val;
	ror_api_assert(REG_values, actor != NULL);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		stratElem->inProgressCount = 1; // What original (replaced) code used to do
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	if (stratElem->elementType != AOE_CONST_FUNC::TAIUnitClass::AIUCLivingUnit) { return; }
	if (!actor->ptrStructDefUnit || (actor->ptrStructDefUnit->unitType != AOE_CONST_FUNC::GUT_BUILDING)) { return; }
	// Here: actor is a building and strategy element is a unit to train. Set temporarily unitInstanceId to actor's id or we can't find where it is being trained.
	stratElem->unitInstanceId = actor->unitInstanceId;
}


void CustomRORInstance::OverloadResetInProgress(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem = (AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *) REG_values->ECX_val;
	ror_api_assert(REG_values, stratElem != NULL);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		stratElem->inProgressCount = 0; // What original (replaced) code used to do
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	stratElem->unitInstanceId = -1; // Added: reset instanceId (CustomROR uses it while in progress to retrieve actor)
}


// This overloads a part of the unit destructor that removes references to deleted unit from strategy and infAI.
// The goal is to add a loop to cancel "in progress" units that are trained in the building, if any.
void CustomRORInstance::OverloadResetUnitInAI(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_AI *mainAI = (AOE_STRUCTURES::STRUCT_AI *) REG_values->ECX_val;
	ror_api_assert(REG_values, mainAI != NULL); // mainAI must not be NULL because of TEST ECX,ECX on 4F2B35
	long int unitId_toReset = REG_values->EAX_val;
	long int myESP = REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_UNIT *unit_toReset = *(AOE_STRUCTURES::STRUCT_UNIT **) (myESP + 0x0C);
	ror_api_assert(REG_values, unit_toReset != NULL);
	// What original (replaced) code used to do:
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		_asm {
			MOV ECX, mainAI
			MOV EAX, unitId_toReset
			MOV EDX,DWORD PTR DS:[ECX]
			PUSH EAX
			CALL DS:[EDX+0x44]
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	// Custom treatments: if unit is a building, search for a unit being trained in it and cancel "in progress" flag
	if (unit_toReset->unitInstanceId != unitId_toReset) { return; } // Should not happen
	if (!unit_toReset->ptrStructDefUnit || unit_toReset->ptrStructDefUnit->unitType != AOE_CONST_FUNC::GUT_BUILDING) { return; } // Quit if not a building
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


// From 004B8BDA
// This is called when a wonder construction is finished.
// ROR code sets "my wonder is built" flag, but they forgot to reset "I am building my wonder" and AI gets kinda stuck.
void CustomRORInstance::OnWonderConstructionCompleted(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_AI *mainAI = (AOE_STRUCTURES::STRUCT_AI *)REG_values->ECX_val;
	ror_api_assert(REG_values, mainAI && mainAI->IsCheckSumValid());
	if (!mainAI || !mainAI->IsCheckSumValid()) { return; }
	mainAI->structTacAI.myWonderIsBuiltFlag = 1;
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	// Always do the fix whatever config says (it's more a technical fix than an AI fix) - except in MP
	if (!IsMultiplayer()) {
		mainAI->structTacAI.myWonderBeingBuiltFlag = 0; // This instruction is missing in original game code.
	}
}


// From 004D2218
// A strategy element (type=building) has been found and is going to be executed.
// We can add additional checks and possibly prevent the construction from being done.
// To prevent the construction, change return address to 004D236C.
// Otherwise, do nothing special and the execution will continue normally.
void CustomRORInstance::AuditOnDoStrategyElementBuilding(REG_BACKUP *REG_values) {
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

	doNotRunConstruction = this->crCommand.ShouldNotTriggerConstruction(tacAI, stratElem);

	// Do not modify below
	if (doNotRunConstruction) {
		ChangeReturnAddress(REG_values, 0x004D236C);
	}
}


// 0x04604C9
// Intercepts ALL button OnClick events, including bitbuttons, checkboxes, dropdowns (just the "open to show list" click)
// Note: Change return address to 0x460568 to disable standard treatments for this event.
void CustomRORInstance::GlobalOnButtonClick(REG_BACKUP *REG_values) {
	ror_api_assert(REG_values, REG_values->ECX_val != 0);
	unsigned long int objAddr = REG_values->ECX_val;
	unsigned long int evtStatus;
	// Note: at this moment, ECX and ESI are the same. Let's use ECX.
	_asm {
		MOV ECX, objAddr
		MOV EAX, DS:[ECX + 0x13C]
		MOV evtStatus, EAX
	}
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EAX_val = evtStatus;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	// Custom treatments
	if (this->crMainInterface.Global_OnButtonClick(objAddr)) {
		ChangeReturnAddress(REG_values, 0x460568);
	}
}


// Manage the KeyPress event - 0x0041A6C7
// This is also called in "scenario info" in-game screen. NOT in diplomacy popup, menu, achievements, etc
// If returned REG_values->EAX_val is == 0 then calling method will not be executed (will RETN immediatly). Can be used to disable some treatments.
void CustomRORInstance::GameAndEditor_ManageKeyPress(REG_BACKUP *REG_values) {
	long int myESP = REG_values->ESP_val;
	long int pressedKey = *(long int *) (myESP + 0x14); // 0x12: ALT 0x73:F4
	long int myECX = REG_values->ECX_val;
	long int myEAX;
	long int myEDI;
	long int tmpChecksum = 0;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		_asm {
			// Original (replaced) code
			MOV EDI, myECX
			MOV EAX, DWORD PTR DS:[EDI+0x14]
			MOV myEAX, EAX
			MOV myEDI, EDI
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	REG_values->EAX_val = myEAX;
	REG_values->EDI_val = myEDI;

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
	if (this->crMainInterface.GameAndEditor_OnKeyPress(pressedKey, ctrlIsON, shiftIsON, altIsOn)) {
		REG_values->EAX_val = 0;
		ChangeReturnAddress(REG_values, 0x41A7C5); // Will NEG EAX (still 0) and then ignore the key press event
		return;
	}
}


// Call from 0x04348D9
// At beginning of function dialog.OnUserEvent(...)
// Note: to disable "normal" treatments from dialog.OnUserEvent(...), change return address to 0x43498C and set REG_values->EAX_val to 1 (or 0)
void CustomRORInstance::ManageOnDialogUserEvent(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_POPUP_DIALOG *ptrDialog = (AOE_STRUCTURES::STRUCT_UI_POPUP_DIALOG*) REG_values->ECX_val;
	ror_api_assert(REG_values, ptrDialog != NULL);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		// Do the code we overwrote to call ROR_API
		REG_values->ESI_val = REG_values->ECX_val; // MOV ESI, ECX
		REG_values->ECX_val = (long) ptrDialog->ptrParentObject; // MOV ECX,DWORD PTR DS:[ESI+40]
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	if (!ptrDialog->IsCheckSumValid()) {
		return; // Seems to be called for another UI object. This is unexpected (but possible?), return before doing any harm.
	}
	// Get (some) function arguments that have already been read into registers
	unsigned long int ptrSender = REG_values->EAX_val;
	long int evtStatus = REG_values->EDX_val; // 1="real" button click

	// Now do custom treatments
	if ((unsigned long int *) ptrDialog == this->crInfo.customYesNoDialogVar) {
		ChangeReturnAddress(REG_values, 0x43498C); // Disable ROR's treatments for this call to dialog.OnUserEvent(...)
		REG_values->EAX_val = 1;
		if (evtStatus == 1) { // button has actually been clicked
			long int returnValue = this->crCommand.CloseCustomDialogMessage(ptrDialog, ptrSender);
			// Manage user choice
			// ...
		}
	}

}


// Plugged on 51A203's call
// Triggered when player releases right click in game screen
void CustomRORInstance::OnGameRightClickUpEvent(REG_BACKUP *REG_values) {
	unsigned long int myESP = REG_values->ESP_val;
	const long int CST_OnGameRightClickUpEvent_IGNORE_CLICK = 0x0051A63D;
	const long int CST_OnGameRightClickUpEvent_HANDLE_CLICK = 0x0051A50B;
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
		if (findSelectedUnitsPreviousResult) {
			_asm {
				MOV EAX, myESP
				MOV ECX, DS:[EAX+0x14]
				TEST ECX, ECX
				JE nullValue // this really isnt supposed to be... But we try to have strong code.
				PUSH ECX
				MOV EAX, 0x005260F8 // Free
				CALL EAX
				ADD ESP, 4
				// Update stack variable so that game code won't call free() with the same address (but 0 instead)
				MOV EAX, myESP
				XOR ECX, ECX
				MOV DS:[EAX+0x14], ECX
			nullValue:
			}
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// Now we correctly plugged our method to game EXE. In all cases it's OK, the list is freed and won't be again (in ROR code)
	// It is possible to change the "return address" (JMP, in fact) by setting REG_values->EAX_val. Do not change other values.


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
	if (this->crMainInterface.ApplyRightClickReleaseOnSelectedUnits(UIGameMain, controlledPlayer, mousePosX, mousePosY)) {
		REG_values->EAX_val = CST_OnGameRightClickUpEvent_HANDLE_CLICK; // Continue (no real jump)
		short int *customLocalVar = (short int *)(REG_values->ESP_val + 0x12); // This local var is not used anymore in original code: we can use it for ourselves
		*customLocalVar = 1;
	}
}


// Entry point that allow forcing the display of "right click" red cross sign when normal game would not show it.
// In ROR, it is part of the same method that calls OnGameRightClickUpEvent.
// See customLocalVar (stack variable in ESP+0x12) that can be used by both methods to save information between the 2 entry points.
void CustomRORInstance::OnGameRightClickUpRedCrossDisplay(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EAX_val = REG_values->EAX_val & 0xFF;
		REG_values->EDI_val = REG_values->EAX_val;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// This local var is not used anymore in original code: we can use it for ourselves. See OnGameRightClickUpEvent.
	short int *customLocalVar = (short int *)(REG_values->ESP_val + 0x12);
	if (*customLocalVar == 1) {
		REG_values->EAX_val = 1;
		REG_values->EDI_val = 1;
	}
}


// Manages key press on +/- in game to change game speed.
// Also fixes the "6" key press bug on french keyboard
void CustomRORInstance::ManageChangeGameSpeed(REG_BACKUP *REG_values) {
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
	if (!this->crInfo.configInfo.useImprovedGameSpeeds) {
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
		*gameSpeed = (*gameSpeed) * this->crInfo.configInfo.improvedGameSpeedFactor;
	} else {
		*gameSpeed = (*gameSpeed) / this->crInfo.configInfo.improvedGameSpeedFactor;
	}
	// Fix value when ~1 (fix rounding issues)
	if (abs(*gameSpeed - 1) <= 0.1) { *gameSpeed = 1; }
	// Fix null speed
	if (abs(*gameSpeed) < 0.05) {
		*gameSpeed = 0.5;
		traceMessageHandler.WriteMessage("Fix NULL speed, set to 0.5x instead.");
	}
}


// Overloads the "tactical AI update" event that occurs regularly for each AI player.
// For the algorithm to work well, requires also "FixUnitIdForInProgressBuilding", "FixResetStratElemForUnitId"
void CustomRORInstance::ManageTacAIUpdate(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_AI *ai = (AOE_STRUCTURES::STRUCT_AI *)REG_values->EDX_val;
	if ((ai == NULL) || (tacAI == NULL)) { return; }

	this->crCommand.ManageTacAIUpdate(ai);
}


// From 004D1B7B. Avoid calling AI updates on defeated players.
// Change return address to 0x4D24E8 if AI update must be skipped (and set ECX=mainAI, in this case)
// The code in 0x4D24E8 will change "currently managed AI player" so AIs won't get stuck.
// Warning: for performance reasons, this method should NOT do too much treatments !
void CustomRORInstance::ManageDefeatedAIPlayerTacAIUpdate(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ESI_val;
	ror_api_assert(REG_values, tacAI && tacAI->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = (unsigned long int) tacAI->ptrMainAI;
	}
	// Changes nothing in standard case (ECX will be overwritten in 0x4D1B82)
	// In custom case (skip - JMP to 0x4D24E8), it is necessary that ECX=mainAI
	REG_values->ECX_val = (unsigned long int) tacAI->ptrMainAI;

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
void CustomRORInstance::OnBeforeLoadEmpires_DAT(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_COMMAND_LINE_INFO *cmdLineInfo = (AOE_STRUCTURES::STRUCT_COMMAND_LINE_INFO *)REG_values->ECX_val;
	const char *empiresDatRelativeFileName = this->crCommand.GetCustomEmpiresDatRelativeFileName(cmdLineInfo);
#ifdef GAMEVERSION_AOE10b
	REG_values->EAX_val = 
#else
	REG_values->ECX_val =
#endif
		(unsigned long int) empiresDatRelativeFileName;
	REG_values->fixesForGameEXECompatibilityAreDone = true;
}


// This is called just after empires.dat is loaded.
void CustomRORInstance::OnAfterLoadEmpires_DAT(REG_BACKUP *REG_values) {
	REG_values->fixesForGameEXECompatibilityAreDone = true; // There is nothing special to do for this change to be compatible with ROR execution.
	this->crCommand.OnAfterLoadEmpires_DAT();
}


// From 0051A3BA
// This is called when user clicks to add a unit in scenario editor (NOT called when moving mouse in game zone).
void CustomRORInstance::EditorCheckForUnitPlacement(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_DEF_UNIT *unitDef = (AOE_STRUCTURES::STRUCT_DEF_UNIT *) REG_values->ESI_val;
	long int callResult = 0;
	REG_values->ECX_val = REG_values->ESI_val; // in fact it is not necessary because we do the CALL ourselves
	REG_values->fixesForGameEXECompatibilityAreDone = true;

	ror_api_assert(REG_values, unitDef);
	ror_api_assert(REG_values, unitDef->IsCheckSumValid());
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

	// Let crCommand object customize the parameters according to configuration and user choices...
	this->crCommand.ApplyCustomizationOnEditorAddUnit(checkVisibility, checkHillMode, editorMode, checkAirModeAndHPBar, checkConflictingUnits, ignoreTerrainRestrictionErrors);

	// CALL defUnit.GetErrorForUnitCreationAtLocation(player, posY, posX, arg4, checkVisibility, hillMode, arg7, editorMode, checkAirModeAndHPBar?, checkConflictingUnits)
	_asm {
		PUSH checkConflictingUnits
		PUSH checkAirModeAndHPBar
		PUSH editorMode
		PUSH arg7
		PUSH checkHillMode
		PUSH checkVisibility
		PUSH arg4
		PUSH posX
		PUSH posY
		PUSH player
		MOV ECX, unitDef
		MOV EDX, DS:[ECX]
		CALL DS:[EDX+0x20]
		AND EAX, 0xFF
		MOV callResult, EAX
	}
	REG_values->EAX_val = callResult;
	if (ignoreTerrainRestrictionErrors && (callResult == AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION::CST_EUC_BAD_TERRAIN_RESTRICTION)) {
		REG_values->EAX_val = AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION::CST_EUC_OK;
	}
}


// From 0x051A700
void CustomRORInstance::EntryPoint_UIUnitSelection(REG_BACKUP *REG_values) {
	REG_values->fixesForGameEXECompatibilityAreDone = true; // No action required in this case.
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return; }
	
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


// From 00426D08
// Specific code to handle gaia units conversion when found by human player.
// Fix some special cases (artefacts).
// Change return address to 00426D19 if we want NOT to change unit owner.
void CustomRORInstance::HumanSpecific_onCapturableUnitSeen(REG_BACKUP *REG_values) {
	long int actorPlayerId = REG_values->EDI_val;
	AOE_STRUCTURES::STRUCT_UNIT *beingSeenUnit = (AOE_STRUCTURES::STRUCT_UNIT *) REG_values->ESI_val;
	ror_api_assert(REG_values, actorPlayerId >= 0 && actorPlayerId <= 8);
	ror_api_assert(REG_values, beingSeenUnit != NULL);
	ror_api_assert(REG_values, beingSeenUnit->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = NULL;
	if (!beingSeenUnit || !beingSeenUnit->IsCheckSumValid() || (actorPlayerId < 0) || (actorPlayerId > 8)) { return; }
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		actorPlayer = GetPlayerStruct((short int)actorPlayerId);
		REG_values->ECX_val = (unsigned long int)actorPlayer;
	}
	bool doNotCaptureUnit = false;

	// Custom treatments here
	doNotCaptureUnit = this->crCommand.HumanSpecific_onCapturableUnitSeen(beingSeenUnit, actorPlayer);

	// Do not modify below.
	if (doNotCaptureUnit) {
		ChangeReturnAddress(REG_values, 0x426D19);
	}
}


void CustomRORInstance::ManageRelicsCount(REG_BACKUP *REG_values) {
	REG_values->EAX_val = this->crInfo.configInfo.randomMapRelicsCount;
}


void CustomRORInstance::ManageRuinsCount(REG_BACKUP *REG_values) {
	REG_values->EAX_val = this->crInfo.configInfo.randomMapRuinsCount;
}


void CustomRORInstance::ManageCivIdControlAfterGameSettingsSelection(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		return;
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if ((REG_values->EAX_val < 0) || (REG_values->EAX_val > this->crInfo.configInfo.civCount) ) {
		// Bad civilization, jump to the code that randomly chooses one (normal behaviour with "random" option)
		ChangeReturnAddress(REG_values, 0x00503423);
	} else {
		// Valid civilization, continue with it.
		ChangeReturnAddress(REG_values, 0x00503451);
	}
}


// Manage first call to gameSettings.getDLLCivNameIdOffset(civId)
// For custom civs, returns 0 so the call we be made again later.
void CustomRORInstance::ManageGetPlayerNameDLLStringIdOffset(REG_BACKUP *REG_values) {
	long int civId = REG_values->EAX_val;
	long int myECX = REG_values->ECX_val; // game settings addr
	long int *stackPtr = (long int *)(REG_values->ESP_val);
	long int civNameDLLStringIdOffset;
	if ((civId > 0) && (civId <= CIVILIZATIONS::CST_CIVID_STANDARD_MAX)) {
		// Just do the normal code: call gameSettings.getDLLCivNameIdOffset(civId) and continue...
		_asm {
			PUSH civId
			MOV EDX, 0x00505F90
			MOV ECX, myECX
			CALL EDX // gameSettings.getDLLCivNameIdOffset(civId)
			MOV civNameDLLStringIdOffset, EAX
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
void CustomRORInstance::ManageGetPlayerNameDLLStringIdOffset2(REG_BACKUP *REG_values) {
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
			PUSH civId
			MOV EDX, 0x00505F90
			MOV ECX, myECX
			CALL EDX // gameSettings.getDLLPlayerNameIdOffsetForCiv(civId)
			ADD EAX, LANG_ID_PLAYER_NAME_BY_CIV_BASE
			MOV civNameDLLStringIdOffset, EAX
			MOV EDX, myEDX
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
	CivilizationInfo *c = this->crInfo.configInfo.GetCivInfo(civId);
	if (c) {
		if (playerId > 8) { playerId = 8; } // Would be nonsense but this will avoid writing out of nameIndexIsUsedByPlayer bounds
		if (playerId < 0) { playerId = 0; } // Same thing
		std::string s = c->ChooseRandomPlayerName(this->crInfo.nameIndexIsUsedByPlayer[civId], playerId);
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
	return;

}


// Chooses a convenient strategy (ai file) for given player according to configuration/map type for custom civs.
// Fills game's buffer with an existing AI filename for given player. If nothing is found (or if file doesnt exist), a NULL string is used.
// This method must NOT fill the buffer with an incorrect file name or the game will freeze.
// Warning: this method may change ROR return address
void CustomRORInstance::ManageAIFileSelectionForPlayer(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_PLAYER *p = (AOE_STRUCTURES::STRUCT_PLAYER *) REG_values->EBX_val;
	if (!p) { return; }
	if (!p->ptrAIStruct) {
		return; // customize ROR code will JMP to end of "player.chooseAIFileName" function
	}
	
	// Custom civ
	char **stackPtr = (char **) (REG_values->ESP_val + 0x320); // 0x318 for sub esp and 8 for the 2 PUSH (cf ROR code).
	char *buffer1 = stackPtr[1];
	char *buffer2 = stackPtr[2];

	strcpy_s(buffer2, 22, "RandomGameSpecialized");
	if (!this->crCommand.ManageAIFileSelectionForPlayer(p->civilizationId, buffer1)) {
		ChangeReturnAddress(REG_values, 0x004F36F8); // continue "player.chooseAIFileName" function normally
	}

	// Return to normal address (leave REG_values->return_address unchanged).
}


void CustomRORInstance::ManageCivsInGameSettingsCombo(REG_BACKUP *REG_values) {
	long int myESI = REG_values->ESI_val;
	ror_api_assert(REG_values, myESI != NULL);
	if (!myESI) { return; }
	AOE_STRUCTURES::STRUCT_ANY_UI *ptrCombo;
	_asm {
		MOV ESI, myESI
		MOV ECX, DS:[ESI]
		MOV ptrCombo, ECX
	}
	for (int i = 1; i <= CIVILIZATIONS::CST_CIVID_STANDARD_MAX; i++) {
		long int DLL_ID = i + 0x27F6;
		if (i > 0x0C) {
			DLL_ID += 3; // There's a gap after civID=0x0C (choson - last non-expansion civ)
		}
		if (i == 0x0F) { DLL_ID = 0x2809; } // Stupid inversion between palmyra and macedonia
		if (i == 0x10) { DLL_ID = 0x2808; } // Stupid inversion between palmyra and macedonia
		AOE_AddEntryInComboUsingDLLID(ptrCombo, i, DLL_ID);
	}

	// Do we already have a global struct ? If Yes, check civ number in empires.dat (if not matching our config, it may crash)
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	if (globalStruct != NULL) {
		if (globalStruct->civCount < this->crInfo.configInfo.civCount) {
			// This situation is quite bad. empires.dat contains less civs than XML config.
			// We can only detect this when global struct already exists.
			// On first time we run a game, settings are displayed BEFORE the global struct is created (and before empires.dat is loaded)
			this->crInfo.configInfo.civCount = globalStruct->civCount;
			const char *msg = localizationHandler.GetTranslation(CRLANG_ID_MISSING_CIVS_IN_EMPIRES_DAT, "ERROR: there are more civs in customROR_civs.xml than in empires.dat. This may cause game crashes !");
			traceMessageHandler.WriteMessage(msg);
			FILE *fileLog = NULL;
			int logFileRes;
			logFileRes = fopen_s(&fileLog, "CustomROR\\CustomROR.log", "a+"); // appends (do not overwrite)
			if (logFileRes == 0) {
				fprintf_s(fileLog, msg);
				fprintf_s(fileLog, "\n");
			}
			fclose(fileLog);
		}
	}

	// Manage custom civs
	for (int civid = CIVILIZATIONS::CST_CIVID_STANDARD_MAX + 1; civid <= this->crInfo.configInfo.civCount; civid++) {
		CivilizationInfo *civInfo = this->crInfo.configInfo.GetCivInfo(civid);
		if (civInfo) {
			AOE_AddEntryInCombo(ptrCombo, civid, civInfo->GetCivName().c_str());
		} else {
			AOE_AddEntryInCombo(ptrCombo, civid, localizationHandler.GetTranslation(CRLANG_ID_DEFAULT_CIV_NAME, "Custom civilization"));
		}
	}
	

	// Add random entry
	AOE_AddEntryInComboUsingDLLID(ptrCombo, this->crInfo.configInfo.civCount + 1, 0x280A);
}


// Manages filling the civilization combobox in scenario editor UI. Overloads default behaviour.
void CustomRORInstance::ManageCivsInEditorCombo(REG_BACKUP *REG_values) {
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
			AOE_AddEntryInComboUsingDLLID(ptrCombo, 0, DLL_ID);
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// Manage custom civs
	for (int civId = CIVILIZATIONS::CST_CIVID_STANDARD_MAX + 1; civId <= this->crInfo.configInfo.civCount; civId++) {
		CivilizationInfo *civInfo = this->crInfo.configInfo.GetCivInfo(civId);
		if (civInfo) {
			AOE_AddEntryInCombo(ptrCombo, 0, civInfo->GetCivName().c_str());
		} else {
			AOE_AddEntryInCombo(ptrCombo, 0, localizationHandler.GetTranslation(CRLANG_ID_DEFAULT_CIV_NAME, "Custom civilization"));
		}
	}

	// Do NOT add random entry in scenario editor
}


// Overloads "default" case in in-game GetCivName method.
void CustomRORInstance::GetInGameCustomCivName(REG_BACKUP *REG_values) {
	long int civId = REG_values->EDX_val;
	long int bufferSize = REG_values->EDI_val;
	char *buffer = (char *)REG_values->ESI_val;

	CivilizationInfo *civInfo = this->crInfo.configInfo.GetCivInfo(civId);
	if ((civId > this->crInfo.configInfo.civCount) || (civId < 1) || (!civInfo)) {
		// Unknown civ id (or not defined in customROR config). Use original (overwritten code)
		// Note: civ name is NOT displayed when controlling player 0 (gaia)
		long int myEAX = REG_values->EAX_val;
		long int gameSettings = REG_values->ECX_val;
		_asm {
			MOV EAX, myEAX
			MOV ECX, gameSettings
			PUSH bufferSize
			PUSH buffer
			PUSH 0x2804
			CALL DWORD PTR DS:[EAX+0x20]
		}
		return;
	}
	// Here civInfo is not NULL, we can use its data.
	ror_api_assert(REG_values, civInfo != NULL); // good check just in case the code above is modified...
	strcpy_s(buffer, bufferSize, civInfo->GetCivName().c_str());
}


// From 0x50A56F and 0x50A5B2
void CustomRORInstance::CheckPlayerCreationAtGameStartup(REG_BACKUP *REG_values) {
	bool isHumanPlayer = (REG_values->return_address == 0x0050A5B9);
	unsigned long int myESI = REG_values->ESI_val;
	unsigned long int myEBX = REG_values->EBX_val;
	unsigned long int myEDX = REG_values->EDX_val;
	unsigned char civId = *(unsigned char*)(myESI + myEBX + 0x251);
	if (civId > this->crInfo.configInfo.civCount) { civId = 1; } // Force value to avoid game crash
	myEDX = myEDX & 0xFFFFFF00; // Remove "DL" part (low byte)
	myEDX = myEDX | civId; // Replace with civId
	REG_values->EDX_val = myEDX;

	// Fix for multiplayer games. Never create AI struct for human players to avoid sync errors.
	if (isHumanPlayer && IsMultiplayer()) {
		ChangeReturnAddress(REG_values, 0x50A576);
	}
}


#ifdef _DEBUG
// Make some checks to prevent irrelevant target unit change and stupid hesitations (like when attacked by 2 priests)
// Return (custom) code is used in customized code in game EXE to decide where to jump.
// 1 = Target unit is REALLY the same as before. Continue with normal behaviour for this case => 00426EE1 (no jump)
//     Also return 1 to keep current target (in that case, force EDI = <old_target_ptr>)
// 2 = We prefer to change target unit. Use normal behaviour (let the original code decide) for this case in 00426F28 (cf original JNZ)
// 3 = OBSOLETE (DO NOT USE) Custom jump to 00426FD1 to exit successfully the method, without changing target. Sometimes CAUSES stuck units bug !
void CustomRORInstance::ManageAttackActionChange(REG_BACKUP *REG_values) {
	assert(false && "ManageAttackActionChange is obsolete"); // This method is OBSOLETE
	const float distanceToConsiderVeryClose = 1.5; // Please leave this > 0.
	REG_values->EAX_val = 2; // Default: change action (let the original code decide)
	// Get context info
	AOE_STRUCTURES::STRUCT_UNIT *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT *) REG_values->EDI_val; // "new" possible target
	AOE_STRUCTURES::STRUCT_UNIT *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT *) REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_ACTION_INFO *actorActionInfo = (AOE_STRUCTURES::STRUCT_UNIT_ACTION_INFO *) REG_values->ECX_val;

	if (!actorActionInfo || !actorActionInfo->ptrActionLink || !targetUnit || !actorUnit) {
		return;
	}
	ror_api_assert(REG_values, targetUnit->IsCheckSumValid());
	ror_api_assert(REG_values, actorUnit->IsCheckSumValid());
	ror_api_assert(REG_values, actorActionInfo->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_ACTION_LINK *link = actorActionInfo->ptrActionLink;
	ror_api_assert(REG_values, link != NULL);
	AOE_STRUCTURES::STRUCT_ACTION_ATTACK *action = (AOE_STRUCTURES::STRUCT_ACTION_ATTACK *) link->actionStruct;
	ror_api_assert(REG_values, action != NULL);
	AOE_STRUCTURES::STRUCT_UNIT *actionTargetUnit = action->targetUnit; // 00426EE5 original instruction...
	if (!actionTargetUnit) {
		return; // Change action: there is currently no target !
	}
	bool sameTargetUnit = (actionTargetUnit == targetUnit); // Cf cmp in original code 00426EE8
	if (sameTargetUnit) {
		REG_values->EAX_val = 1; // Same unit as before, continue without a jump (custom case 1).
		return;
	}

	// Run algorithm to decide if unit should keep target or change
	bool shouldChange = this->crCommand.ShouldChangeTarget(actorUnit->currentActivity, targetUnit->unitInstanceId);
	if (!shouldChange) { REG_values->EDI_val = (long)actionTargetUnit; REG_values->EAX_val = 2; }
}
#endif


// From 00411D58 (activity.setAttackUnit(unitId, force)). For military units + villagers (but not villagers against tree/animals)
// For compatibility with original code, if EBP != 0 then change return address to 0x411D64 (equivalent to the JNZ we overwrote).
// It is possible to overwrite arg1 in stack to force another unitId (ESP+014) (ROR method code has not read it yet).
// Warning: unitId in stack (ESP+014) should NOT be -1 (game bug). If this occurs, force change return address to 0x411DC7 (return 0 in ROR code)
void CustomRORInstance::ManageAttackActivityChange1(REG_BACKUP *REG_values) {
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

	// Custom Treatments.
	if ((*arg_targetUnitId != -1) && !this->crCommand.ShouldChangeTarget(activity, *arg_targetUnitId)) {
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
void CustomRORInstance::ManageAttackActivityChange_convert(REG_BACKUP *REG_values) {
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
	// Custom Treatments.
	if (*arg_targetUnitId == -1) { return; } // Let normal code manage this...

	if (!this->crCommand.ShouldChangeTarget(activity, *arg_targetUnitId)) {
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
void CustomRORInstance::ManageTowerPanicMode_militaryUnits(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ECX_val;
	AOE_STRUCTURES::STRUCT_UNIT *enemyUnit = (AOE_STRUCTURES::STRUCT_UNIT *)REG_values->EDI_val;
	AOE_STRUCTURES::STRUCT_UNIT *myUnit = (AOE_STRUCTURES::STRUCT_UNIT *)REG_values->EBX_val;
	//AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ESI_val;
	REG_values->EAX_val = 1; // Default
	ror_api_assert(REG_values, activity != NULL); // This has been verified in ROR code just before calling ROR_API
	ror_api_assert(REG_values, enemyUnit != NULL);
	ror_api_assert(REG_values, myUnit != NULL);
	//ror_api_assert(REG_values, tacAI != NULL);
	if (!activity || !enemyUnit || !myUnit) { return; }
	ror_api_assert(REG_values, enemyUnit->IsCheckSumValid());
	ror_api_assert(REG_values, myUnit->IsCheckSumValid());
	//ror_api_assert(REG_values, tacAI->IsCheckSumValid());
	bool is_attack_activity = false;
	bool forceKeepCurrentActivity = false; // Default: do NOT skip ! (attack the tower/enemy)
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		if (activity->currentActionId == AOE_CONST_INTERNAL::ACTIVITY_TASK_IDS::CST_ATI_ATTACK) {
			is_attack_activity = true;
		}
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// Info on context
	bool contextIsTowerInMyTown = (REG_values->return_address == 0x04E37B4);
	bool contextIsReactionToAttack = (REG_values->return_address == 0x04D829E);

	// Custom treatments: compute "forceKeepCurrentActivity" variable in specific cases we want to improve.
	// Warning: don't forget there is 2 possible contexts (tower in my town OR I was just attacked by enemyUnit)

	if (contextIsTowerInMyTown) {
		if (!this->crCommand.ShouldAttackTower_towerPanic(myUnit, enemyUnit)) {
			forceKeepCurrentActivity = true;
		}
	}
	if (contextIsReactionToAttack) {
		forceKeepCurrentActivity = !this->crCommand.ShouldChangeTarget(activity, enemyUnit->unitInstanceId);
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
*/
void CustomRORInstance::ManageTowerPanicMode_villagers(REG_BACKUP *REG_values) {
	// ROR code context variables
	AOE_STRUCTURES::STRUCT_TAC_AI *tacAI = (AOE_STRUCTURES::STRUCT_TAC_AI *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT *enemyTower = (AOE_STRUCTURES::STRUCT_UNIT *)REG_values->EDI_val;
	long int *pAssignedUnitsCount = (long int *)(REG_values->ESP_val + 0x14);
	AOE_STRUCTURES::STRUCT_POSITION_INFO *pTCPositionInfo = (AOE_STRUCTURES::STRUCT_POSITION_INFO*)(REG_values->ESP_val + 0x20);

	REG_values->fixesForGameEXECompatibilityAreDone = true;

	//bool forceKeepCurrentActivity = false; // Default: change and attack tower
	assert(enemyTower != NULL);
	if (!enemyTower || !enemyTower->IsCheckSumValid() || !tacAI->IsCheckSumValid()) {
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
	this->crCommand.towerPanic_LoopOnVillagers(tacAI, enemyTower, pAssignedUnitsCount, pTCPositionInfo);
}


// Called from 0x4E5001
// ROR's function canConvertUnit has a bug, it returns true when enemy unit is an unfinished building (status == 0).
void CustomRORInstance::ManageBuildingStatus_canConvertUnit(REG_BACKUP *REG_values) {
	long int unitId = REG_values->EAX_val;
	AOE_STRUCTURES::STRUCT_UNIT *targetUnit = GetUnitStruct(unitId);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EAX_val = (unsigned long int)targetUnit;
		REG_values->fixesForGameEXECompatibilityAreDone = true;
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
void CustomRORInstance::ManageGameTimerSkips(REG_BACKUP *REG_values) {
	long int myECX = REG_values->ECX_val;
	long int myEDX = REG_values->EDX_val;
	long int myESI = REG_values->ESI_val;
	long int myESP = REG_values->ESP_val;
	_asm {
		MOV ESI, myESI
		MOV ECX, DWORD PTR DS:[ESI+0x4]
		MOV EDX, DWORD PTR DS:[ESI+0x58]
		MOV myECX, ECX
		MOV myEDX, EDX
	}
	REG_values->ECX_val = myECX;
	REG_values->EDX_val = myEDX;

	if (++this->crInfo.gameTimerSlowDownCounter > this->crInfo.configInfo.gameTimerSlowDownFactor) {
		this->crInfo.gameTimerSlowDownCounter = 0;
	}
	else {
		ChangeReturnAddress(REG_values, 0x51D940);
	}

	// TO DO: Not the best place to do that...
	// Can we find a better place to dedicate customROR timer things ?
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global != NULL);
	if (!global) { return; }
	long int currentGameTime = global->currentGameTime / 1000;
	if ((this->crInfo.configInfo.dislikeComputeInterval > 0) &&
		(currentGameTime >= (this->crInfo.LastDislikeValuesComputationTime_second + this->crInfo.configInfo.dislikeComputeInterval))) {
		this->crInfo.LastDislikeValuesComputationTime_second = currentGameTime;
		this->crCommand.ComputeDislikeValues();
	}

	// Manage triggers
	if (this->crInfo.triggersLastCheckTime_s + 1 <= currentGameTime) {
		this->crInfo.triggersLastCheckTime_s = currentGameTime;

		// Timer trigger
		CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
		memset(&evtInfo, -1, sizeof(evtInfo));
		evtInfo.currentGameTime_s = currentGameTime;
		this->crCommand.ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_TIMER, evtInfo);

		// "Passive" triggers (we need to test their criteria regulary)

		// Trigger type: Reached some resource value...?
		memset(&evtInfo, -1, sizeof(evtInfo)); // Reset event info
		for (int iPlayerId = 1; iPlayerId < global->playerTotalCount; iPlayerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(iPlayerId);
			if (player && player->IsCheckSumValid()) {
				// We set a limitation for performance: only the 4 main resources are supported. We could just remove the limitation in this "for" loop.
				for (int currentResourceId = AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_FOOD;
					currentResourceId < AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT; currentResourceId++) {
					float value = player->GetResourceValue((AOE_CONST_FUNC::RESOURCE_TYPES)currentResourceId);
					evtInfo.currentGameTime_s = currentGameTime;
					evtInfo.playerId = iPlayerId;
					evtInfo.resourceId = currentResourceId;
					evtInfo.resourceValue = value;
					this->crCommand.ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_RESOURCE_VALUE_MORE_THAN, evtInfo);
					this->crCommand.ExecuteTriggersForEvent(CR_TRIGGERS::EVENT_RESOURCE_VALUE_LESS_THAN, evtInfo);
				}
			}
		}
	}

	// Other customROR "timer" treatments: do them only once every second maximum (for performance)
	if (this->crInfo.lastCustomRORTimeExecution_gameTime_s + 1 <= currentGameTime) {
		this->crInfo.lastCustomRORTimeExecution_gameTime_s = currentGameTime;
	} else {
		// No enough time has run since last execution.
		return;
	}

	// If there are pending messages, show them
	if (traceMessageHandler.HasUnreadMessages() && this->crInfo.configInfo.showCustomRORNotifications) {
		traceMessageHandler.MarkAsRead(true);
		this->crMainInterface.OpenTraceMessagePopup();
	}
}


// Collects information about last execution times and recomputes slow down factor if option is enabled.
// Avoid doing too much treatments here.
// 0x51D9EC
void CustomRORInstance::CollectTimerStats(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *g = (AOE_STRUCTURES::STRUCT_GAME_GLOBAL *) REG_values->ESI_val;
	ror_api_assert(REG_values, g && g->IsCheckSumValid());

	long int interval_ms = REG_values->EAX_val;
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = REG_values->EDI_val + interval_ms;
		g->currentGameTime = REG_values->EAX_val;
	}
	
	if (this->crInfo.CollectedTimerIntervalsIndex >= CST_TIMER_STATS_ARRAY_SIZE) { this->crInfo.CollectedTimerIntervalsIndex = 0; }
	this->crInfo.CollectedTimerIntervals_ms[this->crInfo.CollectedTimerIntervalsIndex++] = interval_ms;

	// Auto compute slow down factor if option is enabled and only once every CST_TIMER_STATS_ARRAY_SIZE
	if (this->crInfo.configInfo.gameTimerSlowDownAutoFix && (this->crInfo.CollectedTimerIntervalsIndex == CST_TIMER_STATS_ARRAY_SIZE)) {
		this->crCommand.AutoFixGameTimer();
	}
}



// This overloads the original code that displays Option button in game menu.
// Reads this->crInfo.configInfo.showCustomRORMenu Parameter to display standard menu OR customized menu
void CustomRORInstance::DisplayOptionButtonInMenu(REG_BACKUP *REG_values) {
	long int myESI = REG_values->ESI_val;
	long int myEDI = REG_values->EDI_val;
	long int myEBP = REG_values->EBP_val;
	long int myEAX = 1;
	unsigned long int **pOptionsBtn; // init in asm code below
	_asm {
		MOV ESI, myESI
		MOV EDI, myEDI
		LEA EAX, DWORD PTR DS:[ESI+EDI*4+0x490]
		MOV pOptionsBtn, EAX
	}
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		myEDI++;
		REG_values->fixesForGameEXECompatibilityAreDone = true;

		// Standard Options button
		long int xSize = this->crInfo.configInfo.showCustomRORMenu ? 0xA0 : 0x168;
		myEAX = AOE_AddButton((AOE_STRUCTURES::STRUCT_ANY_UI*) myESI, (AOE_STRUCTURES::STRUCT_UI_BUTTON**) pOptionsBtn,
			LANG_ID_GAME_OPTIONS, 0x14, myEBP, xSize, 0x1E, 5, AOE_FONTS::AOE_FONT_BIG_LABEL);
	}
	
	// Limit to single player, causes crash on multi.
	assert(this->crInfo.customGameMenuOptionsBtnVar == NULL);
	long int dllIdToUse = LANG_ID_OPTIONS;
	if (localizationHandler.StringExists(CRLANG_ID_CUSTOMROR)) {
		dllIdToUse = CRLANG_ID_CUSTOMROR;
	}
	if (myEAX && this->crInfo.configInfo.showCustomRORMenu && !IsMultiplayer()) {
		myEAX = AOE_AddButton((AOE_STRUCTURES::STRUCT_ANY_UI*)myESI,
			(AOE_STRUCTURES::STRUCT_UI_BUTTON**)&this->crInfo.customGameMenuOptionsBtnVar, 
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
void CustomRORInstance::ManageOptionButtonClickInMenu(REG_BACKUP *REG_values) {
	long int myEAX = REG_values->EAX_val;
	long int myESP = REG_values->ESP_val;
	AOE_STRUCTURES::STRUCT_ANY_UI *previousPopup = (AOE_STRUCTURES::STRUCT_ANY_UI *)REG_values->ESI_val;

	if (this->crInfo.configInfo.showCustomRORMenu) {
		// Before returning, make sure we always "free" the "custom options" button (from game menu).
		this->crMainInterface.FreeInGameCustomOptionsButton();
		// If it is another button than "custom options", we can free custom options button.
		// Otherwise, custom popup will manage this (we already added the button in objects to free).
		if (myEAX != AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_CUSTOM_OPTIONS) {
			this->crInfo.ForceClearCustomMenuObjects();
		}
	}

	if (myEAX == AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_MENU_OPTIONS) { return; } // Corresponds to original code (click on original options menu)

	ChangeReturnAddress(REG_values, 0x004342AF); // Corresponds to the JNZ we removed in original code

	if (myEAX != AOE_CONST_INTERNAL::GAME_SCREEN_BUTTON_IDS::CST_GSBI_CUSTOM_OPTIONS) { return; }

	if (!this->crInfo.configInfo.showCustomRORMenu) { return; } // In theory this should be useless because we shouldn't come here if it is disabled

	// Now manage the case when the clicked button is our custom button...
	this->crMainInterface.CreateGameCustomRorOptionsPopup(previousPopup);
	ChangeReturnAddress(REG_values, 0x004342A7);
}


// Intercepts key press (click) events in options / custom options menu.
// For normal options menu, just does as in original game (gives back execution to standard code)
// Custom treatments for "our" options popup...
// 0x0043136D
void CustomRORInstance::ManageKeyPressInOptions(REG_BACKUP *REG_values) {
	unsigned long int senderAddr = REG_values->EAX_val;
	unsigned long int myEDI = REG_values->EDI_val;
	unsigned long int myESP = REG_values->ESP_val;
	long int myESI = REG_values->ESI_val;

	//AOE_STRUCTURES::STRUCT_ANY_UI *parent = (AOE_STRUCTURES::STRUCT_ANY_UI *)myESI;
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_OPTIONS *parentAsGameOptions = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_OPTIONS *)myESI;
	if (!parentAsGameOptions->IsCheckSumValid()) {
		return; // Unknown situation: we have unexpected data.
	}
	if (senderAddr == (long int)parentAsGameOptions->btnCancel) {
		return; // sender=cancel button (from original options menu), return normally and continue (original JNZ would not jump)
	}
	// Force set focus to parent (popup) to validate currently typed text, if any. No effect in normal options menu.
	AOE_SetFocus(parentAsGameOptions, NULL);

	// Note: myESP + 0x110 is return address of original code method... So arg1 is +0x114, arg2 0x118...
	unsigned long int arg2 = *((unsigned long int*) (myESP + 0x118)); // For textBox, 1 means ESC or click away

	unsigned long int UIObjHWnd = *((unsigned long int*) (senderAddr + 0xF4));
	SendMessageA((HWND)UIObjHWnd, EM_EMPTYUNDOBUFFER, 0, 0); // avoids bad behaviours when pressing ESC (restoring an old value when re-entering the text box !)
	//ShowWindow((HWND)UIObjHWnd, SW_SHOW);

	char *typedText = "";
	
	InGameCustomRorOptionsPopup *crOptionsPopup = NULL;
	bool isCustomROROptionsPopupOpen = false;
	if (this->crMainInterface.currentCustomPopup != NULL) {
		if (dynamic_cast<InGameCustomRorOptionsPopup*>(this->crMainInterface.currentCustomPopup) != NULL) {
			crOptionsPopup = (InGameCustomRorOptionsPopup*)this->crMainInterface.currentCustomPopup;
			isCustomROROptionsPopupOpen = true;
		} else {
			traceMessageHandler.WriteMessage("Internal ERROR: bad popup object type");
			return;
		}
	}
	if (!crOptionsPopup) {
		ChangeReturnAddress(REG_values, 0x0043134A); // continue "original" method (compares sender to other "standard" buttons)
		return;
	}
	
	if ((arg2 == 0) && (this->crInfo.configInfo.showCustomRORMenu) &&
		(senderAddr == (unsigned long int) crOptionsPopup->customOptionFreeTextVar) && (crOptionsPopup->customOptionFreeTextVar != NULL)) {
		typedText = crOptionsPopup->customOptionFreeTextVar->pTypedText;
		char *answer;
		unsigned long int h = crOptionsPopup->customOptionFreeTextAnswerVar->hWnd;
		/*bool result =*/ crCommand.ExecuteCommand(typedText, &answer);
		SendMessageA((HWND)h, WM_SETTEXT, 0, (LPARAM)answer);
		SendMessageA((HWND)h, EM_SETREADONLY, 1, 0);
		//SendMessageA((HWND)h, WM_SETFOCUS, 0, 0);
		ShowWindow((HWND)h, SW_SHOW);
		// No need to free answer, because it points to an internal buffer[...] and is re-used each time.
	}

	// Return if the event is NOT a click on CustomROR in-game custom options OK button.
	// In that case, continue "original" method (compares sender to other "standard" buttons)
	if ((!crOptionsPopup->customOptionButtonVar) || (senderAddr != (unsigned long int)crOptionsPopup->customOptionButtonVar) ||
		!this->crInfo.configInfo.showCustomRORMenu || (!isCustomROROptionsPopupOpen)) {
		ChangeReturnAddress(REG_values, 0x0043134A);
		return;
	}
	// Here: this->crInfo.configInfo.showCustomRORMenu is ON (previous if "returned" if not)
	// Note that we are sure that current popup is customROR custom game options, and the event is a click on OK.

	// Here: our custom options button has been clicked.
	if (myEDI != 1) { // EDI is 1 when we did a "full" click (press+release) on the button
		ChangeReturnAddress(REG_values, 0x0043134A);
		return;
	}

	// Now do our treatments: close custom options when OK has been clicked
	crOptionsPopup->ClosePopup(false);

	ChangeReturnAddress(REG_values, 0x0043141B); // Almost same as original options' cancel button
	// But DO NOT restore settings values like volume... As we didnt initialize those values, it would set irrelevant random values
}


// From 0x499134
// This is called at the end of scenario editor UI creation.
void CustomRORInstance::AfterScenarioEditorCreation(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditor = (AOE_STRUCTURES::STRUCT_UI_SCENARIO_EDITOR_MAIN *)REG_values->ESI_val;
	ror_api_assert(REG_values, scEditor != NULL);
	ror_api_assert(REG_values, scEditor->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->ECX_val = *(long int*)(REG_values->ESP_val + 0x1CC);
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	// Custom actions
	this->crCommand.CustomScenarioEditorUICreation(scEditor);
}


// From 004926B0
void CustomRORInstance::ScenarioEditorChangeSelectedTerrain(REG_BACKUP *REG_values) {
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
	if (!this->crInfo.configInfo.showHiddenTerrainsInEditor) { return; }
	long int customTerrainIdToUse = -1;
	customTerrainIdToUse = this->crCommand.GetTerrainIdForSelectedTerrainIndex(scEditor, selectedIndex);

	if (customTerrainIdToUse > -1) {
		// Force terrainId to use / skip hardcoded switch from ROR original code
		REG_values->EAX_val = customTerrainIdToUse;
		ChangeReturnAddress(REG_values, 0x4926FD);
	}
}


// 0x4ED189
// Called each time a living unit is created.
// Don't forget to check player (gaia?), unit type...
// Warning: this is also called by scenario editor and at game init !
void CustomRORInstance::OnLivingUnitCreation(REG_BACKUP *REG_values) {
	// Now get informations and check them
	AOE_STRUCTURES::STRUCT_UNIT *unit = (AOE_STRUCTURES::STRUCT_UNIT*)REG_values->EAX_val;
	ror_api_assert(REG_values, unit != NULL); // TO DO: Could just return ? Leave that for testing purpose for now...
	AOE_STRUCTURES::STRUCT_PLAYER *player = unit->ptrStructPlayer;
	ror_api_assert(REG_values, player != NULL);
	ror_api_assert(REG_values, unit->IsCheckSumValid());
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
			actionStruct = NULL; // The value we read is not an "MakeObject" actionstruct, ignore it.
		}
	}

	// Get information about UI status: is this editor, game loading, playing... ?
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = *ROR_gameSettings;
	ror_api_assert(REG_values, settings != NULL);
	ror_api_assert(REG_values, settings->IsCheckSumValid());
	AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS currentStatus = settings->currentUIStatus;
	
	// Now do custom code
	this->crCommand.OnLivingUnitCreation(currentStatus, unit, actionStruct);
}


// 0x00501980
// In game code, this method has treatments for events 7E(farm depleted) 7C(trainQueue) 7D(missing house) 64 (researchDone) 66,69 (spawn/build complete)
// 68,6B,1-3(errors) 4-6 (player lost/disconnect...) 7 (tribute) 8 (dipl change) 6C 6D 6E(wonders) 72-75(relics/ruins events) 76-7B (convert events).
void CustomRORInstance::OnGameSettingsNotifyEvent(REG_BACKUP *REG_values) {
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

	// Custom code
	this->crCommand.EntryPoint_GameSettingsNotifyEvent(eventId, (short int)playerId, arg3, arg4, arg5);
}


// From 0x0507F75 (scenarioInfo.applyDisabledResearches(player))
// This is only executed for scenarios, not DM/RM !
void CustomRORInstance::OnGameInitDisableResearchesEvent(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *playerResearchInfo = (AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *)REG_values->ECX_val;
	ror_api_assert(REG_values, playerResearchInfo != NULL);
	if (REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}

	// Custom treatments
	this->crCommand.OnGameInitDisableResearchesEvent(playerResearchInfo);

	// Do not modify below. Make sure this is always executed (do not RETURN before)
	const unsigned long int updateResearchStatuses = 0x4EB38D;
	_asm {
		MOV ECX, playerResearchInfo
		CALL updateResearchStatuses
	}
}


// From 00507FC5
// This is called in scenarioInfo.applyDisabledResearches(player) method.
void CustomRORInstance::OnScenarioInitPlayerBadInitialAgeApplication(REG_BACKUP *REG_values) {
	// There is nothing to do for compatibility
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	// Custom treatment is to disable call to player.applyStartingAge. This is done by overriding it by a call to ROR_API.
	//long int playerStartingAge = REG_values->EAX_val;
}


// From 0050BB2A
// This is called in global.newGameInit method, after settings "global" initial age.
// This is called before settings various player resources / and score information.
// We use it to fix scenario player-specific starting age (technology tree has already been applied, here) in scenario.
// It can be used as an entry point too.
// Please note this is call for ALL game types (NOT for load game though)
void CustomRORInstance::OnGameInitAfterSetInitialAge(REG_BACKUP *REG_values) {
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
	// Custom treatments
	this->crCommand.OnGameInitAfterApplyingTechTrees(currentPlayerId);
}


// 0x0462E60
void CustomRORInstance::OnTextBoxKeyPress(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_TEXTBOX *textbox = (AOE_STRUCTURES::STRUCT_UI_TEXTBOX *)REG_values->ECX_val;
	ror_api_assert(REG_values, textbox != NULL);
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->EAX_val = *((unsigned long int*)0x7BFAB8);
		REG_values->fixesForGameEXECompatibilityAreDone = true;
	}
	long int keyChar = *((unsigned long int*)(REG_values->ESP_val + 0x04)); // arg1
	// In custom popups, RETURN in a textbox loses focus (so it is taken into account)
	if ((this->crInfo.HasOpenedCustomGamePopup()) && (keyChar == VK_RETURN) && (textbox->IsCheckSumValid()) &&
		(textbox->inputType != 7)) {
		AOE_SetFocus(textbox->ptrParentObject, NULL);
	}
}


// From 004EFEB5
// Called to create units from a scenario file (scenario editor or play scenario)
// NOT called when loading a saved game, never called in-game, never called when creating units in scenario editor.
void CustomRORInstance::PlayerCreateUnit_manageStatus(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT *unit = (AOE_STRUCTURES::STRUCT_UNIT *)REG_values->EDI_val;
	ror_api_assert(REG_values, unit != NULL);
	
	char iDesiredStatus = *((char*)REG_values->ESP_val + 0x24); // get arg5 (byte)
	AOE_CONST_INTERNAL::UNIT_STATUS desiredStatus = (AOE_CONST_INTERNAL::UNIT_STATUS)iDesiredStatus;

	// Custom code
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !unit->IsCheckSumValid() || !unit->ptrStructDefUnit) { return; }
	// Only modify behaviour in editor.
	// In real game, we must NOT do such updates because unit would have an invalid state (it is more complex than just changing unitStatus field)

	if (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) {
		// Editor only: use shortcut numbers to provide visual custom information (see unit with custom status)
		if (desiredStatus < AOE_CONST_INTERNAL::UNIT_STATUS::CST_US_READY) {
			unit->shortcutNumber = 1;
		}
		if (desiredStatus > AOE_CONST_INTERNAL::UNIT_STATUS::CST_US_READY) {
			unit->shortcutNumber = 3;
		}

		// For editor only: buildings with a status >2 are not displayed correctly (because game's code tests "status==2").
		// In this very precise case, force the call that was not done (because of the CMP BL,2 condition).
		// This needs to be done BEFORE we change unit.unitStatus (the call needs status to be =0).
		if ((desiredStatus > AOE_CONST_INTERNAL::UNIT_STATUS::CST_US_READY) && (unit->ptrStructDefUnit->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING)) {
			float f = 10000;
			_asm {
				PUSH f
				MOV ECX, unit
				MOV EAX, 0x4AD7B0
				CALL EAX
			}
		}
		unit->unitStatus = desiredStatus;
	}
}


// From 0x42C330
// This method does nothing (RETN). In ROR's code, it corresponds to a log method that is only compiled in DEBUG mode.
// ROR method signature is f(void *obj, char *text, args...). obj is unused.
// WARNING: (ROR) Compiler optimization is really a pain here, some calls to this method are NOT RELEVANT / not related to logs.
// We must return (do nothing) for those calls.
void CustomRORInstance::RORDebugLogHandler(REG_BACKUP *REG_values) {
	// If disabled in config, exit and do nothing (default - corresponds to standard behaviour)
	if (this->crInfo.configInfo.collectRORDebugLogs <= 0) { return; }

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

	this->crCommand.HandleRORDebugLogCall(textStackAddress);
}


// From 0x40C240. Corresponds to a trace method (disabled), signature is Method(pObject, message, [args]...) (printf-style)
// Warning: do not use arg1 as it can be several different objects (often AI struct, but not always)
void CustomRORInstance::CollectAOEDebugLogsInGame(REG_BACKUP *REG_values) {
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->EAX_val = *(long int*)(0x554740); // For MOV EAX,DWORD PTR DS:[554740]
	}
	if (this->crInfo.configInfo.collectRORDebugLogs < 2) {
		return;
	}
	//AOE_STRUCTURES::STRUCT_AI *ai = *(AOE_STRUCTURES::STRUCT_AI **)(REG_values->ESP_val + 4);
	//ror_api_assert(REG_values, ai && ai->IsCheckSumValid());
	char *message = *(char**)(REG_values->ESP_val + 8); // Method's arg2

	unsigned long int textStackAddress = REG_values->ESP_val + 8;
	this->crCommand.HandleRORDebugLogCall(textStackAddress);
}


// From 0x044E02F, compatible with a call from 0x044DFC6 too.
// Called from ROR method that tests if a villager can find a way to a resource (0x044DFC6) and a way back from resource to deposit building (0x044E02F)
// There is a call to "calcPathForMove" that sometimes fails for no apparent reason, especially on "back" movement calculation.
// The fact that we simulate a movement where unit position <> start position causes the method to fail
// Here we just make a workaround to help path finding method to succeed, so that most relevant gathering target can be selected
void CustomRORInstance::GathererPathFindingReturnToDeposit(REG_BACKUP *REG_values) {
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

	AOE_STRUCTURES::STRUCT_UNIT_TYPE50 *actorAsType50 = NULL;
	if (actorUnit->DerivesFromType50()) { actorAsType50 = (AOE_STRUCTURES::STRUCT_UNIT_TYPE50 *)actorUnit; }
	AOE_STRUCTURES::STRUCT_UNIT_MOVEMENT_INFO *movInfo = NULL;
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
		REG_values->EAX_val = callFindPathForUnit(pathFindingArgs);
		return;
	}
	// For second call, there are lots of failure due to start position <> unit position.
	// Use a trick to prevent that.
	if (secondCall && actorAsType50) {
		REG_values->EAX_val = this->crCommand.GathererCheckPathFinding(actorAsType50, pathFindingArgs);
	}
}


// From 004A78C0 : in display unit method, where shortcut numbers are managed.
// - JUMP to 4A792C (default) = do NOT show shortcut (managed in ROR modified code = no need to change return address)
// - JUMP to 4A78DB = show standard shortcut(only for 1 - 9; will crash otherwise)
// To show custom shortcut, do the CALL 516390 in customROR then JUMP to 4A792C.
void CustomRORInstance::ShowUnitShortcutNumbers(REG_BACKUP *REG_values) {
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
	if (this->crCommand.DisplayCustomUnitShortcutSymbol(unitBase, stack1C, stack20, stack10)) {
		ChangeReturnAddress(REG_values, RA_showStandardShortcut); // standard behaviour if call returned true
	}
}


// From 0x494C76
// Change return address to 0x494CE5 (we replace loop by custom code)
void CustomRORInstance::InitPlayersCivInScenarioEditor(REG_BACKUP *REG_values) {
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
	bool useFix = true;

	// Custom part : overload default civilization IDs (1-8) with scenario's ones when loading an existing scenario...
	if (useFix && filename && (*filename != 0)) {
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		ror_api_assert(REG_values, settings);
		ror_api_assert(REG_values, settings->IsCheckSumValid());
		AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scInfo = NULL;
		// Get scenario information (read scenario file)
		_asm {
			MOV EDX, 0x417670
			PUSH 0
			PUSH filename
			MOV ECX, settings
			CALL EDX
			MOV scInfo, EAX
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
				MOV ECX, scInfo
				MOV EDX, DS:[ECX]
				PUSH 1
				CALL DS:[EDX + 4]
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
void CustomRORInstance::FixUnsupportedRomanTileSetInEditorIcons(REG_BACKUP *REG_values) {
	bool useStandardBehavior = false;
	if (REG_values->fixesForGameEXECompatibilityAreDone) {
		return;
	}
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
		long int slpId = CST_BLD_ICONS_ROMAN_TILESET_SLP_ID;
		InitSlpInfoFromDrs(slpInfo, slpId, shpName);
		REG_values->EAX_val = (unsigned long int)slpInfo;
	} else {
		REG_values->EAX_val = 0;
	}
	ChangeReturnAddress(REG_values, 0x497900); // After call to slpinfo constructor in ROR code...
}


// From 004FAA30
void CustomRORInstance::WriteF11PopInfoText(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_F11_POP_PANEL *f11panel = (AOE_STRUCTURES::STRUCT_UI_F11_POP_PANEL *)REG_values->ESI_val;
	ror_api_assert(REG_values, f11panel && f11panel->IsCheckSumValid());
	
	char *bufferToWrite = (char*) GetIntValueFromRORStack(REG_values, 0); // arg1=buffer
	char *format = (char *)GetIntValueFromRORStack(REG_values, 4);
	char *localizedText = (char *)GetIntValueFromRORStack(REG_values, 8);
	long int currentPop = GetIntValueFromRORStack(REG_values, 0xC); // arg4
	long int houseMaxPop = GetIntValueFromRORStack(REG_values, 0x10); // arg5
	this->crCommand.WriteF11PopInfoText(f11panel, bufferToWrite, format, localizedText, currentPop, houseMaxPop);
}

// From 004C41B3 (in infAI.FindEnemyUnitIdWithinRange(ptrMyReferenceUnit, maxDistance, DATID, DATID, DATID, DATID))
// Change return address to 0x4C41C2 when unitId is invalid (-1) to force ignoring it.
// Indeed, the GetUnitPtr(-1) call might find a unit (eg a doppleganger)
void CustomRORInstance::FixGetUnitStructInTargetSelectionLoop(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = (AOE_STRUCTURES::STRUCT_INF_AI *)REG_values->ESI_val;
	long int currentLoopOffset = REG_values->EDI_val;
	long int currentUnitId = REG_values->ECX_val;
	long int loopIndex = GetIntValueFromRORStack(REG_values, 0x10);
	ror_api_assert(REG_values, currentLoopOffset == loopIndex*0x24);
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (currentUnitId == -1) { // This corresponds to bugged case in standard game.
		REG_values->EAX_val = NULL; // simulates that the GetUnitPtr(...) did not found a valid unit
		ChangeReturnAddress(REG_values, 0x4C41C2);
		return;
	}
	REG_values->ECX_val = (unsigned long int) infAI->ptrMainAI; // Required for call 0x40BAB0
	REG_values->EAX_val = currentUnitId; // modified ROR code PUSHes EAX, not ECX.

	if ((currentUnitId >= 0) && this->crCommand.IsImproveAIEnabled(infAI->commonAIObject.playerId)) {
		AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *unitListElemBase = infAI->unitElemList;
		if (!unitListElemBase || (loopIndex >= infAI->unitElemListSize)) { return; }
		AOE_STRUCTURES::STRUCT_INF_AI_UNIT_LIST_ELEM *currentUnitListElem = &unitListElemBase[loopIndex];
		this->crCommand.OnFindEnemyUnitIdWithinRangeLoop(infAI, currentUnitListElem);
	}
}


// From 00414980
// Change return address to 0x414991 when unitId is invalid (-1) to force ignoring it.
// Indeed, the GetUnitPtr(-1) call might find a unit (eg a doppleganger)
// This method fixes the case when some unit (let's say an archer) keeps shooting at nothing, at a specific position on the map.
void CustomRORInstance::FixUnitIdBugStuckAttackNoTarget(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	ror_api_assert(REG_values, activity && isAValidRORChecksum(activity->checksum));
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	REG_values->EAX_val = activity->targetUnitId;
	REG_values->ECX_val = REG_values->ESI_val;
	if (activity->targetUnitId == -1) {
		REG_values->EAX_val = NULL; // simulates that the GetUnitPtr(...) did not found a valid unit
		ChangeReturnAddress(REG_values, 0x414991);
		return;
	}
}


// From 00412F2C
void CustomRORInstance::SetActivityTargetUnitIdBug(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT *)REG_values->EAX_val;
	long int unitIdToSearch = GetIntValueFromRORStack(REG_values, 0);
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if (unitIdToSearch == -1) {
		REG_values->EAX_val = NULL;
	} else {
		REG_values->EAX_val = (unsigned long int) GetUnitStruct(unitIdToSearch);
	}
}


// From 004E64B2. May change return address to 0x4E64C7
// Method = retreat after shooting.
void CustomRORInstance::FixActivityTargetUnitIdBug_retreatAfterShooting(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT *)REG_values->EAX_val;
	ror_api_assert(REG_values, actorUnit && actorUnit->IsCheckSumValid());
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
	// Custom treatments
	if (!this->crCommand.ShouldRetreatAfterShooting(activity)) {
		REG_values->EAX_val = 0; // Unit not found
		ChangeReturnAddress(REG_values, 0x4E64C7); // jump AFTER the call that get unit struct. The next test will see NULL value and exit method.
	}
}


// From 004E4796.
// Get unit struct for "param" unitId and activity->targetUnitId to respectively EDI and EAX.
// FIX: if unitId is -1, the unit pointer must be NULL.
void CustomRORInstance::FixActivityTargetUnitIdBug_case1F4(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	ror_api_assert(REG_values, activity && isAValidRORChecksum(activity->checksum));
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	long int *pUnitId = (long int*)REG_values->EBX_val;
	ror_api_assert(REG_values, pUnitId != NULL);
	long int unitIdFromStack = GetIntValueFromRORStack(REG_values, 0);
	ror_api_assert(REG_values, *pUnitId == unitIdFromStack);
	AOE_STRUCTURES::STRUCT_UNIT *paramUnit = NULL;
	AOE_STRUCTURES::STRUCT_UNIT *activityTargetUnit = NULL;
	if (unitIdFromStack != -1) {
		// FIXED First "get unit struct" (0x4E4796)
		paramUnit = GetUnitStruct(unitIdFromStack);
	}
	if (activity->targetUnitId != -1) {
		// FIXED Second "get unit struct" (0x4E47A7)
		activityTargetUnit = GetUnitStruct(activity->targetUnitId);
	}
	// Update registry with correct values (NULL if IDs are -1)
	REG_values->EDI_val = (unsigned long int)paramUnit;
	REG_values->EAX_val = (unsigned long int)activityTargetUnit;
	SetIntValueToRORStack(REG_values, 0x20, (unsigned long int)paramUnit); // cf MOV DWORD PTR SS:[ESP+20],EDI
}


// From 0050CB64.
// kill* cheat codes crash when player does not exist. This changes return address when player is invalid to force return.
void CustomRORInstance::FixKillXCrashOnUnknownPlayer(REG_BACKUP *REG_values) {
	long int playerIdToKill = REG_values->EDI_val;
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = (AOE_STRUCTURES::STRUCT_GAME_GLOBAL *)REG_values->ESI_val;
	ror_api_assert(REG_values, global && global->IsCheckSumValid());
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	if ((playerIdToKill < 1) || (playerIdToKill >= global->playerTotalCount)) {
		// Invalid player : needs a fix
		ChangeReturnAddress(REG_values, 0x50CB76);
		REG_values->ESI_val = 0; // This will make game EXE jump and exit method properly.
		traceMessageHandler.WriteMessage("kill* cheat code was used with an invalid player. CustomROR prevented the game from crashing.");
		return;
	}
	// Normal case: just get player structure (like original overwritten code)
	REG_values->ECX_val = (unsigned long int)global->ptrPlayerStructPtrTable[playerIdToKill]; // THIS instruction is not secured in game EXE.
}


// From 00483489. end of ShowUnitComandButtons (in-game screen)
void CustomRORInstance::EntryPointOnAfterShowUnitCommandButtons(REG_BACKUP *REG_values) {
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
	// Custom treatments:
	this->crCommand.AfterShowUnitCommandButtons(gameMainUI);
}


// From 00481410 - UIGameMain.DoButtonAction(commandId, infoValue, arg3)
// Forcing return address to 0x4815BE allows to skip ROR treatments (this button click will be ignored by ROR)
void CustomRORInstance::EntryPointOnGameCommandButtonClick(REG_BACKUP *REG_values) {
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

	// Do custom treatments here
	forceIgnoreThisEvent = this->crCommand.OnGameCommandButtonClick(gameMainUI, (AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID)arg1, arg2);

	// Do not modify below.
	if (forceIgnoreThisEvent) {
		ChangeReturnAddress(REG_values, 0x4815BE);
	}
}


// From 0x483710. Fix the transformation of empires.dat button IDs to internal indexes to support more than 1 page
void CustomRORInstance::ROR_GetButtonInternalIndexFromDatBtnId(REG_BACKUP *REG_values) {
	long int rawButtonId = GetIntValueFromRORStack(REG_values, 4);
	long int result = GetButtonInternalIndexFromDatBtnId((char)rawButtonId);
	if (result < 0) {
		result = 0; // Make sure always to return a valid buttonId in ROR code ! 0 is default value in original method.
	}
	REG_values->EAX_val = result;
	REG_values->fixesForGameEXECompatibilityAreDone = true;
}


// From 0x4EBB10. add missing check on techId (check it is >=0)
void CustomRORInstance::FixPlayerNoTechTree_applyTech(REG_BACKUP *REG_values) {
	long int techId = GetIntValueFromRORStack(REG_values, 8); // Get arg1
	techId = techId & 0xFFFF; // Because arg1 is a WORD ! (short int)
	AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *tdi = (AOE_STRUCTURES::STRUCT_TECH_DEF_INFO *)REG_values->ECX_val;
	ror_api_assert(REG_values, tdi && tdi->IsCheckSumValid());
	if (techId < 0) {
		techId = tdi->technologyCount; // this will force detection of invalid techid.
	}
	REG_values->fixesForGameEXECompatibilityAreDone = true;
	REG_values->EAX_val = techId; // MOVSX EAX,WORD PTR SS:[ESP+8] in 0x4EBB11.
}


// From 00410E31 / 0041124C : auto search target unit method.
void CustomRORInstance::EntryPointAutoSearchTargetUnit(REG_BACKUP *REG_values) {
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
	if (!this->crInfo.configInfo.useEnhancedRulesForAutoAttackTargetSelection) {
		return;
	}

	// Custom treatments. Apply custom rules to ignore/accept target unit (overloads standard rules for catapults and walls)

	AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity = (AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *)REG_values->ESI_val;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *potentialTargetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)REG_values->EDI_val;
	ror_api_assert(REG_values, activity && isAValidRORChecksum(activity->checksum));
	ror_api_assert(REG_values, potentialTargetUnit && potentialTargetUnit->IsCheckSumValidForAUnitClass());
	if (potentialTargetUnit && potentialTargetUnit->IsCheckSumValidForAUnitClass()) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = potentialTargetUnit->GetUnitDefinition();
		if (unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass()) {
			if (unitDefBase->unitAIType == TribeAIGroupWall) {
				return; // Potential target is a wall: use standard checks/behaviour.
			}
		}
	}
	bool ignoreThisTarget = this->crCommand.AutoSearchTargetShouldIgnoreUnit(activity, potentialTargetUnit);


	// Do not modify below.
	if (ignoreThisTarget) {
		ChangeReturnAddress(REG_values, returnAddress_ignoreThisTarget);
	} else {
		ChangeReturnAddress(REG_values, returnAddress_thisTargetIsAcceptable);
	}
}


// From 004F8D87
void CustomRORInstance::EntryPointOnBuildingInfoDisplay(REG_BACKUP *REG_values) {
	AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *unitInfoZone = (AOE_STRUCTURES::STRUCT_UI_UNIT_INFO_ZONE *)REG_values->EBP_val;
	ror_api_assert(REG_values, unitInfoZone && unitInfoZone->IsCheckSumValid());
	if (!REG_values->fixesForGameEXECompatibilityAreDone) {
		REG_values->fixesForGameEXECompatibilityAreDone = true;
		REG_values->ESI_val++;
	}
	// Custom code
	// This updates REG_values->ESI_val if necessary.
	this->crCommand.DisplayCustomBuildingAttributesInUnitInfo(unitInfoZone, REG_values->ESI_val);
}


// From 004FF530 = GetLanguageDllString(stringId, buffer, size)
void CustomRORInstance::EntryPointOnGetLocalizedString(REG_BACKUP *REG_values) {
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
	bool useCustomLocalization = true;
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
		bool successfullyFoundString = this->crCommand.GetLocalizedString(stringDllId, buffer, bufferSize);
		if (successfullyFoundString) {
			ChangeReturnAddress(REG_values, returnAddress); // (RETN 0x0C instruction) Prevent ROR from searching in language(x).dll
		}
	}
}


// From 004ADB91. Normal code tests if number of queue items is > 0. If so, current queue is cancelled before adding a new item in queue.
void CustomRORInstance::AllowMultiUnitTypeInQueue(REG_BACKUP *REG_values) {
	short int elemCount = (REG_values->EAX_val & 0xFFFF); // AX / it is a word
	bool forceKeepCurrentQueue = this->crInfo.configInfo.allowMultiQueueing; // Game default = false
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


//#pragma warning(pop)
