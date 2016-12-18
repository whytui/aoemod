#include "../include/mainCustomAI.h"


namespace CUSTOM_AI {
;

// Common AI handler
CustomAIHandler customAIHandler;


CustomPlayerAI::CustomPlayerAI() {
	// Do not call reset on other objects from constructor
	this->ResetOwnFields();
}


// Reset all information (game (re)start, etc)
void CustomPlayerAI::ResetAllInfo() {
	this->ResetOwnFields();
	for (int i = 0; i < 9; i++) {
		STRATEGY::strategyUpdater[i].ResetAllInfo();
	}
	this->militaryAIInfo.ResetAllInfo();
	this->unitGroupAI.ResetAllInfo();
	this->unitGroupAI.militaryAIInfo = &this->militaryAIInfo;
	this->economyAI.ResetAllInfo();
}

// Reset own fields (game (re)start, etc)
void CustomPlayerAI::ResetOwnFields() {
	this->mainAI = NULL;
	this->myPlayer = NULL;
	this->myPlayerId = -1;
	this->isValidPlayer = false;
	this->isPlayerAlive = false;
	this->lastStrategyAnalysisTime = 0;
}


// Game start init
void CustomPlayerAI::Init(STRUCT_GAME_GLOBAL *global, long int playerId) {
	assert(global != NULL);
	if (!global) { return; }
	this->myPlayerId = playerId;
	this->myPlayer = global->GetPlayerStruct(playerId);
	if (this->myPlayer) {
		this->mainAI = this->myPlayer->ptrAIStruct;
		if (this->mainAI && this->mainAI->IsCheckSumValid()) {
			this->isValidPlayer = true;
		}
		this->isPlayerAlive = (myPlayer->aliveStatus == 0);
	}
	this->lastStrategyAnalysisTime = 0;
}


// Returns true if player is enabled, alive, has AI structure
bool CustomPlayerAI::IsValidAIPlayer() {
	return this->isValidPlayer && this->isPlayerAlive && (this->myPlayerId >= 0) && (this->mainAI != NULL) &&
		(this->myPlayer != NULL);
}


// Runs strategy update if needed.
// Returns true if treatements were executed, false if not (not enough time has passed since last execution)
bool CustomPlayerAI::RunStrategyUpdate(long int currentGameTime) {
	if (!this->IsValidAIPlayer()) { return false; }
	long int updateDelay_ms = CUSTOMROR::crInfo.configInfo.tacticalAIUpdateDelay * 1000;
	if (this->isValidPlayer && this->isPlayerAlive &&
		(currentGameTime - this->lastStrategyAnalysisTime > updateDelay_ms)) {
		STRATEGY::AnalyzeStrategy(&this->mainAI->structBuildAI);
		return true;
	}
	return false;
}


// One-shot strategy analysis, only run at game beginning. Not even re-run when loading a saved game.
void CustomPlayerAI::RunInitialStrategyAnalysis() {
	if (!this->IsValidAIPlayer()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *gameSettings = GetGameSettingsPtr();
	assert(gameSettings && gameSettings->IsCheckSumValid());
	if (!gameSettings || !gameSettings->IsCheckSumValid()) { return; }
	if (gameSettings->difficultyLevel <= GAME_DIFFICULTY_LEVEL::GDL_HARD) {
		// Hard/hardest: Search for techs that can be added to strategy and would improve my military units
		STRATEGY::AddUsefulMilitaryTechsToStrategy(this->myPlayer);
	}
	if (gameSettings->difficultyLevel <= GAME_DIFFICULTY_LEVEL::GDL_EASY) {
		// All levels except easiest
		STRATEGY::AdaptStrategyToMaxPopulation(this->myPlayer);
	}
}


// Triggered when a relic/ruin control was transfered to another player (not necessarily an enemy)
void CustomPlayerAI::OnArtefactControlLoss(AOE_STRUCTURES::STRUCT_UNIT_BASE *myUnit, AOE_STRUCTURES::STRUCT_PLAYER *otherPlayer) {

}


// Triggered when one of my units has been converted
void CustomPlayerAI::OnUnitConverted(AOE_STRUCTURES::STRUCT_UNIT_BASE *myUnit, AOE_STRUCTURES::STRUCT_PLAYER *enemyPlayer) {
	if (!enemyPlayer || !enemyPlayer->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }
	TimeIntervalAttackRecord *interval = this->militaryAIInfo.recentAttacksByPlayer[enemyPlayer->playerId].GetIntervalForGameTime(global->currentGameTime);
	if (interval) {
		interval->convertedCount++;
	}
}


// Triggered each time an AI player's unit is attacked (possibly by a gaia unit)
// rorOriginalPanicModMethodHasBeenRun indicates if ROR's panic mode method has been run. If so, panic mode treatments should not be run here.
void CustomPlayerAI::OnUnitAttacked(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *myUnit,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyUnit, bool rorOriginalPanicModeMethodHasBeenRun) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	assert(tacAI && myUnit && enemyUnit && enemyUnit->unitDefinition); // already checked by caller
	AOE_STRUCTURES::STRUCT_PLAYER *enemyPlayer = enemyUnit->ptrStructPlayer;
	assert(enemyPlayer && enemyPlayer->IsCheckSumValid());
	assert(this->mainAI == tacAI->ptrMainAI);
	if (!tacAI) {
		tacAI = &this->mainAI->structTacAI; // this should never happen
	}
	if (!enemyPlayer) { return; }
	long int enemyPlayerId = enemyPlayer->playerId;
	assert((enemyPlayerId >= 0) && (enemyPlayerId < 9) && (enemyPlayerId < global->playerTotalCount));
	if ((enemyPlayerId < 0) || (enemyPlayerId > 8)) { return; } // Could cause overflows
	AOE_STRUCTURES::STRUCT_UNIT_BASE *myTC = AOE_MainAI_findUnit(this->mainAI, CST_UNITID_FORUM);

	// Record the attack. The analog treatment in ROR code is in 0x4D7AF0 (update TacAI.attacksByPlayerCount[enemyPlayerId]).
	// Note: we also record gaia attacks (why wouldn't we?)
	this->militaryAIInfo.SaveEnemyAttackInHistory(enemyPlayerId, global->currentGameTime, enemyUnit, myTC);
	
	AOE_STRUCTURES::STRUCT_INF_AI *infAI = &this->mainAI->structInfAI;
	if (infAI && infAI->IsCheckSumValid()) {
		// Make sure attacker IS in my unit elem list (infAI).
		// Until I figure out how this list is valued (and why it is so... wrong!), let's use this hack to make sure attackers are found in panic mode analysis.
		AddUpdateInfAIElemList(infAI, enemyUnit);
	}

	if (myUnit->remainingHitPoints < 1) {
		// Unit has just been killed
		TimeIntervalAttackRecord *interval = this->militaryAIInfo.recentAttacksByPlayer[enemyPlayerId].GetIntervalForGameTime(global->currentGameTime);
		if (interval) {
			interval->deathsCount++;
		}
	}

	// Handle panic mode (if eligible)
	if (!rorOriginalPanicModeMethodHasBeenRun && (enemyPlayerId > 0)) {
		// Use an approximate filter on position to reduce the number of calls to panic mode method.
		// More precise checks are run there, this is just an optimization !
		bool panicModeIsEligible = CUSTOM_AI::CustomAIMilitaryInfo::IsPanicModeEligible(myTC, myUnit, enemyUnit);
		long int timeSinceLastPanicMode_ms = (global->currentGameTime - tacAI->lastPanicModeStrategyUpdateTime);
		panicModeIsEligible &= (timeSinceLastPanicMode_ms >= (CUSTOMROR::crInfo.configInfo.panicModeDelay * 1000));
		if (panicModeIsEligible) {
			STRATEGY::ManagePanicMode(tacAI->ptrMainAI, enemyPlayerId, &this->militaryAIInfo);
		}
	}

	// Have nearby unit groups react to the attack if idle/attacking a secondary target
	if (enemyPlayerId > 0) {
		STRUCT_UNIT_GROUP *fakeGrp = &tacAI->fakeFirstUnitGroupElem;
		STRUCT_UNIT_GROUP *curGrp = fakeGrp->next;
		while (curGrp && (curGrp != fakeGrp)) {
			STRUCT_UNIT_BASE *curLeader = global->GetUnitFromId(curGrp->commanderUnitId);
			if (curLeader && curLeader->IsCheckSumValidForAUnitClass()) {
				bool canChangeTarget = false;
				STRUCT_UNIT_BASE *curGroupTarget = global->GetUnitFromId(curGrp->targetUnitId);
				if (!curGroupTarget) { canChangeTarget = true; } else {
					if (curGroupTarget->unitDefinition) {
						if (!UnitDefCanAttack(curGroupTarget->unitDefinition) && (curGroupTarget->unitDefinition->DAT_ID1 != CST_UNITID_WONDER)) {
							canChangeTarget = true;
						}
					}
				}
				if (canChangeTarget) {
					long int sqrdist = (long int)((curLeader->positionX - enemyUnit->positionX) * (curLeader->positionX - enemyUnit->positionX) +
						(curLeader->positionY - enemyUnit->positionY) * (curLeader->positionY - enemyUnit->positionY));
					if (sqrdist < COMBAT::COMBAT_CONST::distanceAlwaysTaskIdleMilitaryUnitsOnAttack * COMBAT::COMBAT_CONST::distanceAlwaysTaskIdleMilitaryUnitsOnAttack) { // TODO: constant
						// Do *not* task units (could conflict with other treatments, get units stuck, etc) but set group's target
						this->unitGroupAI.SetUnitGroupTarget(curGrp, enemyUnit);

						// Task idle units ?
						int remainingUnits = curGrp->unitCount;
						for (int i = 0; i < STRUCT_UNIT_GROUP_UNIT_SLOTS_COUNT; i++) {
							STRUCT_UNIT_BASE *curUnit = global->GetUnitFromId(curGrp->GetMyUnitId(i));
							if (curUnit && curUnit->IsCheckSumValidForAUnitClass()) {
								remainingUnits--;
								if (AOE_METHODS::IsUnitIdle(curUnit) && curUnit->DerivesFromCommandable()) {
									MoveAndAttackTarget(tacAI, (STRUCT_UNIT_COMMANDABLE*)curUnit, enemyUnit);
								}
							}
						}
						
					}
				}
			}

			curGrp = curGrp->next;
		}
	}
}


CustomAIHandler::CustomAIHandler() {
	// Do not call reset on other objects from constructor
	this->ResetOwnFields();
}

void CustomAIHandler::ResetOwnFields() {
	this->currentGameTotalPlayerCount = 0;
}

// To be executed each time a game starts/is loaded
// This method is in charge of resetting all AI-related CustomROR structures
void CustomAIHandler::ResetAllInfo() {
	this->ResetOwnFields();
	for (int i = 0; i < _countof(this->playerAITable); i++) {
		this->playerAITable[i].ResetAllInfo();
	}
	CUSTOM_AI::unitTargetingHandler.ResetAllInfo();
	CUSTOM_AI::playerTargetingHandler.ResetAllInfo();
}


// To be executed at game start. Required for all other treatments to work.
void CustomAIHandler::GameStartInit() {
	assert(this->currentGameTotalPlayerCount == 0); // If not, ResetAllInfo has NOT been called at game init, which is bad.
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }
	this->currentGameTotalPlayerCount = global->playerTotalCount;
	for (int i = 1; i < this->currentGameTotalPlayerCount; i++) {
		this->playerAITable[i].Init(global, i);
		CUSTOM_AI::playerTargetingHandler.GetPlayerInfo(i)->militaryAIInfo = &this->playerAITable[i].militaryAIInfo;
	}

}


// Triggered each time an AI player's unit is attacked (possibly by a gaia unit)
// rorOriginalPanicModMethodHasBeenRun indicates if ROR's panic mode method has been run. If so, panic mode treatments should not be run here.
void CustomAIHandler::OnUnitAttacked(AOE_STRUCTURES::STRUCT_TAC_AI *tacAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *myUnit,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *enemyUnit, bool rorOriginalPanicModeMethodHasBeenRun) {
	assert(tacAI && tacAI->IsCheckSumValid());
	assert(myUnit && myUnit->IsCheckSumValidForAUnitClass());
	assert(enemyUnit && enemyUnit->IsCheckSumValidForAUnitClass());
	if (!tacAI || !tacAI->IsCheckSumValid() || !myUnit || !myUnit->IsCheckSumValidForAUnitClass() || !enemyUnit || !enemyUnit->IsCheckSumValidForAUnitClass()) {
		return;
	}
	long int myPlayerId = tacAI->commonAIObject.playerId;
	if (!this->IsAliveAI(myPlayerId)) { return; }

	// Dispatch event to player AI
	this->GetCustomPlayerAI(myPlayerId)->OnUnitAttacked(tacAI, myUnit, enemyUnit, rorOriginalPanicModeMethodHasBeenRun);
}


}

