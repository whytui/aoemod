#include "../include/unitTargeting.h"


using namespace AOE_CONST_FUNC;
using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

// Global static objects
UnitTargeting unitTargetingHandler;


// Reset all underlying info (useful at game start, etc)
void UnitTargeting::ResetAllInfo() {
	for (int i = 0; i < _countof(this->lastChosenTargetUnitId);i++) {
		this->lastChosenTargetUnitId[i] = -1;
		this->lastTargetChangeGameTime[i] = 0;
		this->lastTargetSelectionGameTime[i] = 0;
		this->priorityLocation[i].posX = -1;
		this->priorityLocation[i].posY = -1;
		this->groupIsInMyTown[i] = false;
	}
}


// Initializes internal information for a new search for a given player
void UnitTargeting::InitNewSearch(long int playerId) {
	assert((playerId >= 0) && (playerId < _countof(this->lastChosenTargetUnitId)));
	this->priorityLocation[playerId].posX = -1;
	this->priorityLocation[playerId].posY = -1;
	this->groupIsInMyTown[playerId] = false;
}


// Reset all values. InProgress is set to false (0)
void UnitTargeting::ResetTargetInfo(STRUCT_TAC_AI_TARGET_INFO *targetInfo) {
	assert(targetInfo != NULL);
	if (!targetInfo) { return; }
	targetInfo->targetUnitId = -1;
	targetInfo->targetEvaluation = -1.f;
	targetInfo->targetInfAIUnitElemListIndex = -1;
	targetInfo->currentSearchInfAIUnitElemListIndex = 0;
	targetInfo->unknown_10 = -1;
	targetInfo->unknown_14 = -1;
	targetInfo->currentSearchIsBuildings = 0; // start with living units
	targetInfo->buildingTargetUnitId = -1;
	targetInfo->buildingTargetEvaluation = -1.f;
	targetInfo->buildingTargetInfAIUnitElemListIndex = -1;
	targetInfo->targetSearchInProgress = 1;
}


// Set target in unitGroup, in internal data, in targetInfo, in infAIElem.
// Private method: we do not check input pointers here (be careful)
void UnitTargeting::SetTarget(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo,
	STRUCT_UNIT_MEMORY *targetUnitMemory, long int leaderTerrainZoneId, float targetEvaluation) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	// Trick to get index without having to loop on all elements
	int index = (targetUnitMemory - infAI->unitMemoryList);
	assert(index >= 0);
	assert(index < infAI->unitMemoryListSize);
	unitGroup->targetUnitIdArrayUsedElemCount = 0;
	unitGroup->AddTargetUnitIdToArray(targetUnitMemory->unitId);
	unitGroup->targetPlayerId = targetUnitMemory->playerId;
	unitGroup->terrainZoneId = leaderTerrainZoneId;
	targetInfo->targetUnitId = targetUnitMemory->unitId;
	targetInfo->targetEvaluation = targetEvaluation;
	targetInfo->targetInfAIUnitElemListIndex = index;
	targetInfo->targetSearchInProgress = 0;
	targetUnitMemory->attackAttempts++; // TODO/WARNING: also done by caller but only in 0x4D4376
	long int myPlayerId = infAI->commonAIObject.playerId;
	if (this->lastChosenTargetUnitId[myPlayerId] != targetUnitMemory->unitId) {
		this->lastChosenTargetUnitId[myPlayerId] = targetUnitMemory->unitId;
		this->lastTargetChangeGameTime[myPlayerId] = global->currentGameTime;
	}
	this->lastTargetSelectionGameTime[myPlayerId] = global->currentGameTime;
}


// Returns the enemy wonder with higher attack priority. Does not return wonders from allied/neutral players
// playerId can be a joker (if -1)
// Returns NULL if not found
STRUCT_UNIT_MEMORY *UnitTargeting::FindWonderToAttackUnitMemoryPtr(STRUCT_INF_AI *infAI, long int playerId) {
	assert(infAI && infAI->IsCheckSumValid());
	if (!infAI || !infAI->IsCheckSumValid()) { return NULL; }
	assert((playerId >= -1) && (playerId <= 8));
	STRUCT_UNIT_MEMORY *result;
	unsigned long int addr = 0x4C5980;
	_asm{
		MOV ECX, infAI;
		PUSH playerId;
		CALL addr;
		MOV result, EAX;
	}
	return result;
}


// If current target is still a valid target - and its position is known -, return its pointer in InfAI elem list.
// Returns NULL otherwise
STRUCT_UNIT_MEMORY *UnitTargeting::GetUnitMemoryForCurrentTargetIfStillEligible(STRUCT_INF_AI *infAI, long int targetPlayerId,
	STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }

	STRUCT_UNIT_BASE *groupLeader = global->GetUnitFromId(unitGroup->commanderUnitId);
	if (!groupLeader || !groupLeader->IsCheckSumValidForAUnitClass() || !groupLeader->ptrStructPlayer) { return NULL; }
	char leaderTerrainZoneId = groupLeader->GetMyTerrainZoneId();

	if ((targetPlayerId != unitGroup->targetPlayerId) || (unitGroup->targetUnitIdArrayUsedElemCount <= 0) ||
		((char)unitGroup->terrainZoneId != leaderTerrainZoneId)) {
		return NULL;
	}

	for (int i = unitGroup->targetUnitIdArrayUsedElemCount; i >= 0; i--) {
		long int loopTargetId = unitGroup->GetTargetUnitIdFromArray(i);
		STRUCT_UNIT_BASE *loopTargetUnit = NULL;
		if (loopTargetId >= 0) { // Missing check in original code
			loopTargetUnit = global->GetUnitFromId(loopTargetId);
		}
		if (loopTargetUnit && loopTargetUnit->IsCheckSumValidForAUnitClass() && loopTargetUnit->ptrStructPlayer &&
			loopTargetUnit->ptrStructPlayer->IsCheckSumValid() && 
			IsUnitPositionKnown(infAI->ptrMainAI->player, loopTargetUnit)) {
			long int loopTargetPlayerId = loopTargetUnit->ptrStructPlayer->playerId;
			if ((loopTargetPlayerId > 0) && (loopTargetUnit->unitStatus <= AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY)) {
				long int canMoveToTarget = AOE_METHODS::UNIT::CanMoveToTarget(groupLeader, loopTargetId,
					AOE_METHODS::UNIT::GetMaxRange(groupLeader), true);
				if (canMoveToTarget) {
					// Additional conditions/checks ?
					// check game time since when we have the same target ?

					unitGroup->targetUnitIdArrayUsedElemCount = 0; // Reset (no need to erase values from array)
					unitGroup->AddTargetUnitIdToArray(loopTargetId);
					if ((targetInfo->targetUnitId == loopTargetId) && (targetInfo->targetInfAIUnitElemListIndex >= 0)) {
						// Optimization; if we already have this pointer, just use it (don't loop on all elem list)
						return &infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex];
					}

					long int indexInList = unitExtensionHandler.GetIndexForUnitInInfAIUnitMemory(loopTargetId, infAI->commonAIObject.playerId);
					if (indexInList >= 0) {
#ifdef _DEBUG
						assert(infAI->unitMemoryList[indexInList].unitId == loopTargetId);
#endif
						return &infAI->unitMemoryList[indexInList];
					} else {
						return NULL;
					}
				}
			}
		}
	}
	return NULL;
}


STRUCT_UNIT_MEMORY *UnitTargeting::ContinueFindGroupMainTargetInProgress(STRUCT_INF_AI *infAI, long int targetPlayerId,
	STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue) {
	// exclude villager explorers (run too fast) ?
	// TODO: if too much time spent, return NULL (keep inprogress=1)

	long int priorityPosX = this->priorityLocation[infAI->commonAIObject.playerId].posX;
	long int priorityPosY = this->priorityLocation[infAI->commonAIObject.playerId].posY;
	bool usePriorityPos = true;
	if ((priorityPosX < 0) || (priorityPosY < 0)) {
		if (!this->groupIsInMyTown[infAI->commonAIObject.playerId]) {
			// Group is already on a military campaign: search targets near current position. Don't cross the whole map each time we change target ! (which is default behaviour !)
			priorityPosX = (long int)unitGroup->currentPosX;
			priorityPosY = (long int)unitGroup->currentPosY;
		} else {
			// Group is in town: no restriction on target position. Allow attacking enemy towns, which may be far from my town.
			priorityPosX = -1;
			priorityPosY = -1;
			usePriorityPos = false;
		}
	}

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(settings && settings->IsCheckSumValid());
	assert(global && global->IsCheckSumValid());
	STRUCT_UNIT_BASE *groupLeader = global->GetUnitFromId(unitGroup->commanderUnitId);
	bool leaderIsRanged = false;
	long int losToUse = 6;
	if (groupLeader->DerivesFromAttackable()) {
		STRUCT_UNITDEF_ATTACKABLE *unitDef = (STRUCT_UNITDEF_ATTACKABLE*)groupLeader->unitDefinition;
		leaderIsRanged = (unitDef->unitAIType != TribeAIGroupPriest) && IsRangedUnit(unitDef); // exclude priest because of building conversion ?
		if (leaderIsRanged) {
			losToUse = (long int)unitDef->maxRange;
		}
	}
	
	char leaderTerrainZoneId = groupLeader->GetMyTerrainZoneId();
	STRUCT_UNIT_MEMORY *militaryTargetsByDistance[3]; // Military targets. index 0=close=priority, index2=far
	STRUCT_UNIT_MEMORY *buildingTargetsByDistance[3]; // Building targets. index 0=close=priority, index2=far
	STRUCT_UNIT_MEMORY *otherLivingTargetsByDistance[3]; // Civilians, etc. index 0=close=priority, index2=far
	for (int i = 0; i < 3; i++) { // init
		buildingTargetsByDistance[i] = militaryTargetsByDistance[i] = otherLivingTargetsByDistance[i] = NULL;
	}

	for (int i = 0; i < infAI->unitMemoryListSize; i++) {
		if (infAI->unitMemoryList[i].playerId == targetPlayerId) {
			GLOBAL_UNIT_AI_TYPES curAIType = infAI->unitMemoryList[i].unitClass;
			int distanceLevel = 0;
			STRUCT_UNIT_MEMORY **targetToUpdate = &militaryTargetsByDistance[0];
			if (usePriorityPos) {
				long int diffX = infAI->unitMemoryList[i].posX - priorityPosX;
				long int diffY = infAI->unitMemoryList[i].posY - priorityPosY;
				long int diffsqr = diffX * diffX + diffY * diffY;
				if (diffsqr <= losToUse * losToUse) {
					distanceLevel = 0;
				} else {
					if (diffsqr > AI_CONST::townSizeSquare) { // enemy further than "town size": consider it very far, with very low priority (if using a priority position)
						distanceLevel = 2;
					} else {
						distanceLevel = 1;
					}
				}
			}
			bool curIsMilitaryUnitOrTower = ((infAI->unitMemoryList[i].attack > 0) && (infAI->unitMemoryList[i].unitClass != TribeAIGroupCivilian));
			bool curIsBuilding = !curIsMilitaryUnitOrTower && (infAI->unitMemoryList[i].unitClass != TribeAIGroupBuilding);
			if (curIsMilitaryUnitOrTower) {
				targetToUpdate = &militaryTargetsByDistance[distanceLevel];
			} else {
				if (curIsBuilding) {
					targetToUpdate = &buildingTargetsByDistance[distanceLevel];
				} else {
					targetToUpdate = &otherLivingTargetsByDistance[distanceLevel];
				}
			}

			if ((infAI->unitMemoryList[i].unitClass != TribeAIGroupArtefact) &&
				(GetUnitStruct(infAI->unitMemoryList[i].unitId) != NULL)) {
				bool curElemIsBetter = (*targetToUpdate == NULL);
				if (*targetToUpdate != NULL) {
					curElemIsBetter = ((*targetToUpdate)->attack <= 0) && curIsMilitaryUnitOrTower;
				}

				*targetToUpdate = &infAI->unitMemoryList[i];
			}
		}
	}

	targetInfo->targetSearchInProgress = 0;
	// Select top-priority target from "grid" of targets
	if (settings->isDeathMatch) {
		// Distance is a sufficient criterion. Of course, at similar distance, try to attack living units, but avoid running through map too much
		for (int i = 0; i < 3; i++) {
			if (militaryTargetsByDistance[i]) {
				this->SetTarget(infAI, unitGroup, targetInfo, militaryTargetsByDistance[i], leaderTerrainZoneId, 1);
				return militaryTargetsByDistance[i];
			}
			if (otherLivingTargetsByDistance[i]) {
				this->SetTarget(infAI, unitGroup, targetInfo, otherLivingTargetsByDistance[i], leaderTerrainZoneId, 1);
				return otherLivingTargetsByDistance[i];
			}
			if (buildingTargetsByDistance[i]) {
				this->SetTarget(infAI, unitGroup, targetInfo, buildingTargetsByDistance[i], leaderTerrainZoneId, 1);
				return buildingTargetsByDistance[i];
			}
		}
	} else {
		// Scenario, random games: avoid attacking buildings as main target when there are living units to attack: it would waste my time, maybe I won't even manage to destroy the building...
		// TODO: decide if I must avoid or attack towers according to group composition
		for (int i = 0; i < 2; i++) {
			if (militaryTargetsByDistance[i]) {
				this->SetTarget(infAI, unitGroup, targetInfo, militaryTargetsByDistance[i], leaderTerrainZoneId, 1);
				return militaryTargetsByDistance[i];
			}
			if (otherLivingTargetsByDistance[i]) {
				this->SetTarget(infAI, unitGroup, targetInfo, otherLivingTargetsByDistance[i], leaderTerrainZoneId, 1);
				return otherLivingTargetsByDistance[i];
			}
		}
		// Nearby and middle-distance units have lower priority in this case
		for (int i = 0; i < 2; i++) {
			if (buildingTargetsByDistance[i]) {
				this->SetTarget(infAI, unitGroup, targetInfo, buildingTargetsByDistance[i], leaderTerrainZoneId, 1);
				return buildingTargetsByDistance[i];
			}
		}
		// Last: "very far" units
		if (militaryTargetsByDistance[2]) {
			this->SetTarget(infAI, unitGroup, targetInfo, militaryTargetsByDistance[2], leaderTerrainZoneId, 1);
			return militaryTargetsByDistance[2];
		}
		if (otherLivingTargetsByDistance[2]) {
			this->SetTarget(infAI, unitGroup, targetInfo, otherLivingTargetsByDistance[2], leaderTerrainZoneId, 1);
			return otherLivingTargetsByDistance[2];
		}
		// Last of all: far buildings
		if (buildingTargetsByDistance[2]) {
			this->SetTarget(infAI, unitGroup, targetInfo, buildingTargetsByDistance[2], leaderTerrainZoneId, 1);
			return buildingTargetsByDistance[2];
		}
	}
	return NULL;
}


// If target player has all relics/ruins, finds the location to attack to capture one.
// Returns true if a target location has been set (other than -1).
bool UnitTargeting::SetPriorityTargetLocation(STRUCT_INF_AI *infAI, long int targetPlayerId,
	STRUCT_UNIT_BASE *groupLeader, STRUCT_TAC_AI_TARGET_INFO *targetInfo) {
	assert(targetInfo != NULL);
	if (!targetInfo) { return false; }
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	STRUCT_AI *mainAI = infAI->ptrMainAI;
	assert(mainAI && mainAI->IsCheckSumValid());
	STRUCT_PLAYER *player = mainAI->player;
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return false; }
	int myPlayerId = player->playerId;
	this->priorityLocation[myPlayerId].posX = -1;
	this->priorityLocation[myPlayerId].posY = -1;
	STRUCT_PLAYER *targetPlayer = global->GetPlayerStruct(targetPlayerId);
	if (!targetPlayer || !targetPlayer->IsCheckSumValid()) { return false; }
	bool hasStandardVictoryCondition = (global->generalVictoryCondition == GAME_GLOBAL_GENERAL_VICTORY_CONDITION::GGVC_STANDARD);
	bool targetHasAllRelics = (targetPlayer->GetResourceValue(CST_RES_ORDER_ALL_RELICS) != 0);
	bool targetHasAllRuins = (targetPlayer->GetResourceValue(CST_RES_ORDER_ALL_RUINS) != 0);
	if (player->remainingTimeToAllRelicsVictory < 0) {
		targetHasAllRelics = false;
	}
	if (player->remainingTimeToAllRuinsVictory < 0) {
		targetHasAllRuins = false;
	}
	long int timeSinceLastTargetSelection_ms = global->currentGameTime - this->lastTargetSelectionGameTime[myPlayerId];
	bool allowForceUpdateTargetOnAllRelics = (timeSinceLastTargetSelection_ms > TARGETING_CONST::timeAboveWhichFarTargetFromArtefactsIsRemoved);
	if (hasStandardVictoryCondition && allowForceUpdateTargetOnAllRelics && (targetHasAllRelics || targetHasAllRuins)) {
		bool relicsAreMoreImportantThanRuins = targetHasAllRelics;
		if (targetHasAllRelics && targetHasAllRuins) {
			relicsAreMoreImportantThanRuins = (player->remainingTimeToAllRelicsVictory < player->remainingTimeToAllRuinsVictory);
		}
		// Find visibleArtefacts
		std::list<STRUCT_UNIT_BASE*>targetRelics;
		std::list<STRUCT_UNIT_BASE*>targetRuins;
		if (targetPlayer->ptrCreatableUnitsListLink && targetPlayer->ptrCreatableUnitsListLink->IsCheckSumValid()) {
			int totalElemCount = targetPlayer->ptrCreatableUnitsListLink->listElemCount;
			STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *curElem = targetPlayer->ptrCreatableUnitsListLink->lastListElement;
			int curElemIndex = 0;
			while ((curElemIndex < totalElemCount) && (curElem != NULL)) {
				STRUCT_UNIT_BASE *curUnit = curElem->unit;
				if (curUnit && curUnit->IsCheckSumValidForAUnitClass() && curUnit->unitDefinition && curUnit->unitDefinition->IsCheckSumValidForAUnitClass()) {
					if (curUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact) {
						if (PLAYER::IsExploredForPlayer(player, (long int)curUnit->positionX, (long int)curUnit->positionY)) {
							short int unitDefId = curUnit->unitDefinition->DAT_ID1;
							bool isRelic = (unitDefId == CST_UNITID_RELIC);
							bool ignoreIt = false;
							if (!isRelic && (unitDefId != CST_UNITID_RUIN) && (unitDefId != CST_UNITID_RUIN2)) {
								// Custom unit (unknown).
								if (curUnit->unitDefinition->DerivesFromFlag() &&
									((STRUCT_UNITDEF_FLAG*)curUnit->unitDefinition)->speed > 0) {
									isRelic = true; // Can move: relic
								}
							}
							if (isRelic) {
								if (AOE_METHODS::UNIT::IsUnitIdle(curUnit)) {
									targetRelics.push_back(curUnit);
								}
							} else {
								targetRuins.push_back(curUnit);
							}
						}
					}
				}
				curElem = curElem->previousElement;
				curElemIndex++;
			}
		}
		// Now work on targetArtefacts
		long int bestSqrDistFromGroup = 256 * 256;
		STRUCT_UNIT_BASE *bestArtefactTarget = NULL;
		std::list<STRUCT_UNIT_BASE *> *firstList = &targetRelics;
		std::list<STRUCT_UNIT_BASE *> *secondList = &targetRuins;
		if (!relicsAreMoreImportantThanRuins) {
			firstList = &targetRuins;
			secondList = &targetRelics;
		}
		for each (STRUCT_UNIT_BASE *curArtefact in *firstList)
		{
			long int diffX = (long int)(curArtefact->positionX - groupLeader->positionX);
			long int diffY = (long int)(curArtefact->positionY - groupLeader->positionY);
			long int curSqrDist = (diffX * diffX) + (diffY * diffY);
			if ((bestArtefactTarget == NULL) || (curSqrDist < bestSqrDistFromGroup)) {
				bestArtefactTarget = curArtefact;
				bestSqrDistFromGroup = curSqrDist;
			}
		}
		if (bestArtefactTarget == NULL) {
			for each (STRUCT_UNIT_BASE *curArtefact in *secondList)
			{
				long int diffX = (long int)(curArtefact->positionX - groupLeader->positionX);
				long int diffY = (long int)(curArtefact->positionY - groupLeader->positionY);
				long int curSqrDist = (diffX * diffX) + (diffY * diffY);
				if ((bestArtefactTarget == NULL) || (curSqrDist < bestSqrDistFromGroup)) {
					bestArtefactTarget = curArtefact;
					bestSqrDistFromGroup = curSqrDist;
				}
			}
		}
		// bestArtefactTarget (if non NULL) now determines the location to attack/capture
		if (bestArtefactTarget != NULL) {
			this->priorityLocation[myPlayerId].posX = (long int)bestArtefactTarget->positionX;
			this->priorityLocation[myPlayerId].posY = (long int)bestArtefactTarget->positionY;
			if (targetInfo->targetInfAIUnitElemListIndex >= 0) {
				if (infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex].playerId == targetPlayerId) {
					long int diffX = infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex].posX - this->priorityLocation[myPlayerId].posX;
					long int diffY = infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex].posY - this->priorityLocation[myPlayerId].posY;
					if ((diffX*diffX) + (diffY*diffY) > TARGETING_CONST::squareDistanceAboveWhichRecomputeTargetOnAllArtefacts) {
						targetInfo->targetUnitId = -1;
						targetInfo->targetEvaluation = -1.f;
						targetInfo->targetInfAIUnitElemListIndex = -1;
					}
				}
			}
			return true;
		}
	}
	return false;
}


STRUCT_UNIT_MEMORY* UnitTargeting::FindTargetUnitNearPriorityLocation(STRUCT_INF_AI *infAI, long int targetPlayerId) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	STRUCT_GAME_MAP_INFO *mapInfo = global->gameMapInfo;
	assert(mapInfo && mapInfo->IsCheckSumValid());
	if (!mapInfo) { return NULL; }
	long int myPlayerId = infAI->commonAIObject.playerId;
	STRUCT_PLAYER *targetPlayer = global->GetPlayerStruct(targetPlayerId);
	if (!targetPlayer) { return NULL; }

	long int centerPosX = this->priorityLocation[myPlayerId].posX;
	long int centerPosY = this->priorityLocation[myPlayerId].posY;

	// Draw squares around target position, getting more and more far, until we find an eligible target
	for (int curDist = 0; curDist <= TARGETING_CONST::maxDistToSearchUnitsNearTargetLocation; curDist++) {
		// Start at top-left
		long int basePosX = centerPosX - curDist;
		long int basePosY = centerPosY - curDist;
#pragma message("All 4 subroutines: Can we actually set artefact as target ? Would make the whole thing very simple")
		for (int xOffset = 0; xOffset < curDist * 2; xOffset++) {
			// Search in parallel top and bottom horizonal lines
			STRUCT_GAME_MAP_TILE_INFO *curTile = mapInfo->GetTileInfo(basePosX + xOffset, centerPosY - curDist);
			STRUCT_UNIT_BASE_LIST *curListElem = curTile->unitsOnThisTile;
			// "Top" tile
			for (int i = 0; i < curTile->unitsOnThisTileCount + 1; i++) {
				if (curListElem) {
					STRUCT_UNIT_BASE *curUnit = curListElem->unit;
					if (curUnit && curUnit->IsCheckSumValidForAUnitClass() && (curUnit->ptrStructPlayer == targetPlayer) &&
						(curUnit->unitStatus <= GAME_UNIT_STATUS::GUS_2_READY) && (curUnit->remainingHitPoints > 0) &&
						curUnit->unitDefinition && 
						(curUnit->unitDefinition->unitAIType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact)) {
						return ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(curUnit->unitInstanceId, infAI);
						//return AOE_METHODS::LISTS::FindInfAIUnitElemInList(infAI, curUnit->unitInstanceId);
					}
					curListElem = curListElem->next;
				}
			}
			// "Bottom" tile
			curTile = mapInfo->GetTileInfo(basePosX + xOffset, centerPosY + curDist);
			for (int i = 0; i < curTile->unitsOnThisTileCount; i++) {
				if (curListElem) {
					STRUCT_UNIT_BASE *curUnit = curListElem->unit;
					if (curUnit && curUnit->IsCheckSumValidForAUnitClass() && (curUnit->ptrStructPlayer == targetPlayer) &&
						(curUnit->unitStatus <= GAME_UNIT_STATUS::GUS_2_READY) && (curUnit->remainingHitPoints > 0) &&
						curUnit->unitDefinition && (curUnit->unitDefinition->unitAIType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact)) {
						return ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(curUnit->unitInstanceId, infAI);
						//return AOE_METHODS::LISTS::FindInfAIUnitElemInList(infAI, curUnit->unitInstanceId);
					}
					curListElem = curListElem->next;
				}
			}
		}
		for (int yOffset = 1; yOffset < curDist * 2 - 1; yOffset++) {
			// Search in parallel left and right vertical lines
			STRUCT_GAME_MAP_TILE_INFO *curTile = mapInfo->GetTileInfo(centerPosX - curDist, basePosY + yOffset);
			STRUCT_UNIT_BASE_LIST *curListElem = curTile->unitsOnThisTile;
			// "Top" tile
			for (int i = 0; i < curTile->unitsOnThisTileCount; i++) {
				if (curListElem) {
					STRUCT_UNIT_BASE *curUnit = curListElem->unit;
					if (curUnit && curUnit->IsCheckSumValidForAUnitClass() && (curUnit->ptrStructPlayer == targetPlayer) &&
						(curUnit->unitStatus <= GAME_UNIT_STATUS::GUS_2_READY) && (curUnit->remainingHitPoints > 0) &&
						curUnit->unitDefinition && (curUnit->unitDefinition->unitAIType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact)) {
						return ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(curUnit->unitInstanceId, infAI); 
						//return AOE_METHODS::LISTS::FindInfAIUnitElemInList(infAI, curUnit->unitInstanceId);
					}
					curListElem = curListElem->next;
				}
			}
			// "Bottom" tile
			curTile = mapInfo->GetTileInfo(centerPosX + curDist, basePosY + yOffset);
			for (int i = 0; i < curTile->unitsOnThisTileCount; i++) {
				if (curListElem) {
					STRUCT_UNIT_BASE *curUnit = curListElem->unit;
					if (curUnit && curUnit->IsCheckSumValidForAUnitClass() && (curUnit->ptrStructPlayer == targetPlayer) &&
						(curUnit->unitStatus <= GAME_UNIT_STATUS::GUS_2_READY) && (curUnit->remainingHitPoints > 0) &&
						curUnit->unitDefinition && (curUnit->unitDefinition->unitAIType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact)) {
						return ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(curUnit->unitInstanceId, infAI); 
						//return AOE_METHODS::LISTS::FindInfAIUnitElemInList(infAI, curUnit->unitInstanceId);
					}
					curListElem = curListElem->next;
				}
			}
		}
	}
	return NULL;
}


// (TEST) find a target for an active unit group, called from "task active groups" method.
// This method is used for "attack" phases, not really for defensive situations.
STRUCT_UNIT_MEMORY *UnitTargeting::TestFindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
	STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue) {
	// Collect info / Check parameters
	assert(targetInfo != NULL);
	if (!targetInfo) { return NULL; }
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!global || !global->IsCheckSumValid() || !settings || !settings->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	if ((targetPlayerId <= 0) || (targetPlayerId >= global->playerTotalCount)) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_AI *mainAI = infAI->ptrMainAI;
	assert(mainAI && mainAI->IsCheckSumValid());
	if (!mainAI || !mainAI->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_PLAYER *player = mainAI->player;
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_UNIT_BASE *groupLeader = global->GetUnitFromId(unitGroup->commanderUnitId);
	if (!groupLeader || !groupLeader->IsCheckSumValidForAUnitClass()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	if (!groupLeader->currentActivity) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_TAC_AI *tacAI = &infAI->ptrMainAI->structTacAI;
	assert(tacAI && tacAI->IsCheckSumValid());
	if (!tacAI || !tacAI->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }

	PLAYER_DIPLOMACY_STANCES diplVsTarget = groupLeader->ptrStructPlayer->ptrDiplomacyStances[targetPlayerId];
	if ((diplVsTarget != AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ENEMY) && (diplVsTarget != AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_NEUTRAL)) {
		assert(false && "Trying to find a target for a non-enemy player");
		targetInfo->targetSearchInProgress = 0; return NULL;
	}

	CUSTOM_AI::AIPlayerTargetingInfo *playerTargetingInfo = CUSTOM_AI::playerTargetingHandler.GetPlayerInfo(player->playerId);
	if (playerTargetingInfo && playerTargetingInfo->militaryAIInfo) {
		MILITARY_SITUATION militarySituation = playerTargetingInfo->militaryAIInfo->lastKnownMilitarySituation;
		long int evalGameTime_ms = playerTargetingInfo->militaryAIInfo->lastKnownMilitarySituationComputationGameTime;
		long int timeDiff_ms = global->currentGameTime - evalGameTime_ms;
		//if ((timeDiff_ms < 5000) && (militarySituation == MILITARY_SITUATION::MS_CRITICAL)) {
		if ((timeDiff_ms < 5000) && 
			((militarySituation == MILITARY_SITUATION::MS_CRITICAL) || (militarySituation == MILITARY_SITUATION::MS_WEAK))) {
			// Critical situation: do not attack. Weak also ?
			// Note: this method is only called from "task active groups": this won't prevent from defending. Custom "task active groups" code may also give a target.
			return NULL;
		}
	}

	// Already initialized: continue...
	if (targetInfo->targetSearchInProgress) {
		return this->ContinueFindGroupMainTargetInProgress(infAI, targetPlayerId, unitGroup, targetInfo, baseTimeGetTimeValue);
	} else {
		this->ResetTargetInfo(targetInfo);
	}

	this->InitNewSearch(player->playerId);

	// Group position is in town ?
	this->groupIsInMyTown[player->playerId] = false;
	STRUCT_UNIT_BASE *myMainCentralUnit = CUSTOM_AI::CustomAIMilitaryInfo::GetTownCenterOrUnitToDefend(player);
	if (myMainCentralUnit && myMainCentralUnit->IsCheckSumValidForAUnitClass()) {
		long int diffX = (long int)(groupLeader->positionX - myMainCentralUnit->positionX);
		long int diffY = (long int)(groupLeader->positionY - myMainCentralUnit->positionY);
		long int diff = diffX * diffX + diffY * diffY;
		if (diff < CUSTOM_AI::AI_CONST::townNeighborhoodSizeSquare) {
			this->groupIsInMyTown[player->playerId] = true;
		}
	}

	char leaderTerrainZoneId = groupLeader->GetMyTerrainZoneId();
	STRUCT_UNITDEF_BASE *groupLeaderDef = groupLeader->unitDefinition;
	assert(groupLeaderDef && groupLeaderDef->IsCheckSumValidForAUnitClass());
	bool isPriest = groupLeaderDef->unitAIType == TribeAIGroupPriest;
	bool isBoat = IsWaterUnit(groupLeaderDef->unitAIType);

	// Performance: keep current target if still alive and if a very small delay has passed since last target selection
	if ((global->currentGameTime - this->lastTargetSelectionGameTime[player->playerId] < TARGETING_CONST::neverRunTargetSelectionProcessBeforeThisDelay) &&
		(targetInfo->targetUnitId >= 0) && (targetInfo->targetInfAIUnitElemListIndex >= 0)) {
		STRUCT_UNIT_BASE *currentTarget = global->GetUnitFromId(targetInfo->targetUnitId);
		if (currentTarget && (currentTarget->unitStatus <= GAME_UNIT_STATUS::GUS_2_READY)) {
			// Do NOT update target selection time !
			return &infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex];
		}
	}

	// Highest priority rules: wonders, all artefacts...
	STRUCT_UNIT_MEMORY *enemyWonderInfo = this->FindWonderToAttackUnitMemoryPtr(infAI, -1);
	if (enemyWonderInfo) {
		// To get remaining time: settings->playerWondersVictoryDelays[targetPlayerId];
		this->SetTarget(infAI, unitGroup, targetInfo, enemyWonderInfo, leaderTerrainZoneId, TARGETING_CONST::evaluationForPriorityTargetWonder);
		return enemyWonderInfo;
	}

	// Target has all relics or all ruins: find location (into class member)
	if (this->SetPriorityTargetLocation(infAI, targetPlayerId, groupLeader, targetInfo)) {
		// Before looping on infAI elem list, try a quick look at target position: maybe there are units guarding this place !
		STRUCT_UNIT_MEMORY *targetNearArtefact = this->FindTargetUnitNearPriorityLocation(infAI, targetPlayerId);
		if (targetNearArtefact != NULL) {
			this->SetTarget(infAI, unitGroup, targetInfo, targetNearArtefact, leaderTerrainZoneId, 1); // Low score: we have no clue if unit is moving or gonna move away from our true target.
			return targetNearArtefact;
		}
	}

	STRUCT_PLAYER *targetPlayer = global->GetPlayerStruct(targetPlayerId);
	if (!targetPlayer || !targetPlayer->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }

	STRUCT_UNIT_MEMORY *currentTarget = this->GetUnitMemoryForCurrentTargetIfStillEligible(infAI, targetPlayerId, unitGroup, targetInfo, baseTimeGetTimeValue);
	if (currentTarget) {
		if ((currentTarget->attack > 0) || (currentTarget->unitClass != GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding)) {
			// if current target is not a building, keep it (villager, military units...
			return currentTarget;
		}
		// If current target is a building (non-tower), try to find another target
	}

	// Player global situation
	int villagerCount = tacAI->allVillagers.usedElements;
	int landMilitaryCount = tacAI->landMilitaryUnits.usedElements;
	int waterMilitaryCount = tacAI->warShips.usedElements;
	int grpCount = tacAI->unitGroupsCount;
	int myGrpUnitCount = unitGroup->unitCount;

	// Group composition
	STRUCT_UNIT_BASE *groupUnitsPtr[40]; // save pointers to unit group units
	memset(groupUnitsPtr, 0, sizeof(groupUnitsPtr));
	int curPos = 0;
	int foundCount = 0;
	int meleeCount = 0;
	int siegeCount = 0;
	int priestCount = 0;
	int archerCount = 0;
	int slingerCount = 0;
	int warBoatsCount = 0;
	while ((curPos < 40) && (foundCount < myGrpUnitCount)) {
		long int curUnitId = unitGroup->GetMyUnitId(curPos);
		STRUCT_UNIT_BASE *curUnit = NULL;
		if (curUnitId >= 0) {
			curUnit = global->GetUnitFromId(curUnitId);
		}
		if (curUnit && curUnit->IsCheckSumValidForAUnitClass() && curUnit->unitDefinition && curUnit->unitDefinition->IsCheckSumValidForAUnitClass()) {
			groupUnitsPtr[curPos] = curUnit;
			foundCount++;
			switch (curUnit->unitDefinition->unitAIType) {
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher:
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher:
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher:
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher:
				archerCount++;
				break;
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot:
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider:
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupFootSoldier:
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupHero:
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupMountedSoldier:
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPhalanx:
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupDomesticatedAnimal:
				meleeCount++;
				break;
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest:
				priestCount++;
				break;
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon:
				siegeCount++;
				break;
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger:
				slingerCount++;
				break;
			case GLOBAL_UNIT_AI_TYPES::TribeAIGroupWarBoat:
				warBoatsCount++;
				break;
			default:
				break;
			}
		}
		curPos++;
	}
	bool canAttackBuildings = (siegeCount + meleeCount + slingerCount > (myGrpUnitCount / 3));
	bool allowTargetingCivilian = !settings->isDeathMatch; // in deathmatch, attacking civilian is NOT a priority (at all)

	targetInfo->targetSearchInProgress = 1;
	return this->ContinueFindGroupMainTargetInProgress(infAI, targetPlayerId, unitGroup, targetInfo, baseTimeGetTimeValue);
}



// baseTimeGetTimeValue = TimeGetTime() value (ms) for total AI treatment process time calculation. We must not go over global->tmp_allowedTimeForAITreatment
// If successful, returns a pointer to the selected STRUCT_UNIT_MEMORY element.
// If successful, unitGroup targets array contains only 1 target (in most cases... to check in case when units block the way to main target)
// Returns false if found nothing OR if search could not be completed in time.
// Note: set targetInfo->targetSearchInProgress=0 when search is finished (even if failed)
STRUCT_UNIT_MEMORY *FindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
	STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue) {

	// Collect info / Check parameters
	assert(targetInfo != NULL);
	if (!targetInfo) { return NULL; }
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	if ((targetPlayerId <= 0) || (targetPlayerId >= global->playerTotalCount)) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_AI *mainAI = infAI->ptrMainAI;
	assert(mainAI && mainAI->IsCheckSumValid());
	if (!mainAI || !mainAI->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_PLAYER *player = mainAI->player; // "actor" player
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_UNIT_BASE *groupLeader = global->GetUnitFromId(unitGroup->commanderUnitId);
	if (!groupLeader || !groupLeader->IsCheckSumValidForAUnitClass()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	if (!groupLeader->currentActivity) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_TAC_AI *tacAI = &infAI->ptrMainAI->structTacAI;
	assert(tacAI && tacAI->IsCheckSumValid());
	if (!tacAI || !tacAI->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }

	char leaderTerrainZoneId = groupLeader->GetMyTerrainZoneId();
	STRUCT_UNITDEF_BASE *groupLeaderDef = groupLeader->unitDefinition;
	assert(groupLeaderDef && groupLeaderDef->IsCheckSumValidForAUnitClass());
	bool isPriest = groupLeaderDef->unitAIType == TribeAIGroupPriest;
	bool isBoat = IsWaterUnit(groupLeaderDef->unitAIType);

	if (!targetInfo->targetSearchInProgress) {
		// Initialize search
		if ((targetPlayerId == unitGroup->targetPlayerId) && (unitGroup->targetUnitIdArrayUsedElemCount >= 1) &&
			((char)unitGroup->terrainZoneId) == leaderTerrainZoneId) {
			long int lastTargetUnitId = unitGroup->GetTargetUnitIdFromArray(unitGroup->targetUnitIdArrayUsedElemCount - 1);
			STRUCT_UNIT_BASE *targetUnit = NULL;
			if (lastTargetUnitId >= 0) {
				targetUnit = global->GetUnitFromId(lastTargetUnitId);
			}
			if (targetUnit && targetUnit->IsCheckSumValidForAUnitClass()) {
				STRUCT_PLAYER *targetPlayer = targetUnit->ptrStructPlayer;
				assert(targetPlayer && targetPlayer->IsCheckSumValid());
				long int targetUnitPlayerId = targetPlayer->playerId;
				bool isEnemy = (targetUnitPlayerId > 0) &&
					(player->ptrDiplomacyStances[targetPlayerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ENEMY);
				if (isEnemy && (targetUnit->unitStatus <= AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY)) {
					long int canMoveToTarget = AOE_METHODS::UNIT::CanMoveToTarget(groupLeader, targetUnit->unitInstanceId,
						AOE_METHODS::UNIT::GetMaxRange(groupLeader), true);
					if (canMoveToTarget) {
						unitGroup->targetUnitIdArrayUsedElemCount = 0; // Reset (no need to erase values from array)
						unitGroup->AddTargetUnitIdToArray(targetUnit->unitInstanceId);
						return ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(targetUnit->unitInstanceId, infAI); 
						//return AOE_METHODS::LISTS::FindInfAIUnitElemInList(infAI, targetUnit->unitInstanceId);
					}
				}
			}
			// 0x4C013A. Strange conception: why not just do a (reverse) loop instead of duplicate code with code above ?
			for (int i = 0; i < unitGroup->targetUnitIdArrayUsedElemCount; i++) {
				long int loopTargetId = unitGroup->GetTargetUnitIdFromArray(i);
				STRUCT_UNIT_BASE *loopTargetUnit = NULL;
				if (loopTargetId >= 0) { // Missing check in original code
					loopTargetUnit = global->GetUnitFromId(loopTargetId);
				}
				if (loopTargetUnit && loopTargetUnit->IsCheckSumValidForAUnitClass() && loopTargetUnit->ptrStructPlayer &&
					loopTargetUnit->ptrStructPlayer->IsCheckSumValid()) {
					long int loopTargetPlayerId = loopTargetUnit->ptrStructPlayer->playerId;
					long int isLoopUnitEnemy = (loopTargetPlayerId > 0) &&
						(player->ptrDiplomacyStances[loopTargetPlayerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ENEMY);
					if (isLoopUnitEnemy && (loopTargetUnit->unitStatus <= AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY)) {
						long int canMoveToTarget = AOE_METHODS::UNIT::CanMoveToTarget(groupLeader, loopTargetId,
							AOE_METHODS::UNIT::GetMaxRange(groupLeader), true);
						if (canMoveToTarget) {
							return ROCKNROR::unitExtensionHandler.GetInfAIUnitMemory(loopTargetUnit->unitInstanceId, infAI);
							//return AOE_METHODS::LISTS::FindInfAIUnitElemInList(infAI, loopTargetUnit->unitInstanceId);
						}
					}
				}
			}
		}
		if (!targetInfo->targetSearchInProgress) { // always true: same condition as parent if
			STRUCT_UNIT_MEMORY *enemyWonderInfo = unitTargetingHandler.FindWonderToAttackUnitMemoryPtr(infAI, -1);
			if (enemyWonderInfo) {
				char wonderTerrainZoneId = groupLeader->GetTerrainZoneIdAtPos(enemyWonderInfo->posX, enemyWonderInfo->posY);
				if ((wonderTerrainZoneId == leaderTerrainZoneId) || isBoat) {
					long int canMoveToTarget = AOE_METHODS::UNIT::CanMoveToTarget(groupLeader, enemyWonderInfo->unitId,
						AOE_METHODS::UNIT::GetMaxRange(groupLeader), true);
					if (canMoveToTarget) {
						unitGroup->AddTargetUnitIdToArray(enemyWonderInfo->unitId);
						return enemyWonderInfo;
					}
					STRUCT_MANAGED_ARRAY tempList;
					// This call searches a path, even if enemy units block the way ? Unsure
					bool success = AOE_METHODS::UNIT::MoveToTarget_1A0(groupLeader, enemyWonderInfo->unitId, AOE_METHODS::UNIT::GetMaxRange(groupLeader),
						0, 1, targetPlayerId, 0x1B, &tempList);
					if (success) {
						if (tempList.usedElements > 0) {
							for (int i = 0; i < tempList.usedElements; i++) { // 0x4C0323
								if (i > tempList.arraySize) {
									// 0x4C032C. Dead code & unused ? How could usedElemCount be > allocCount ???
									assert(false);
									// I did not rewrote this portion of code, as I don't see the point...
								}
								long int currentUnitId = tempList.unitIdArray[i];
								STRUCT_UNIT_BASE *currentUnit = NULL;
								// ADDED: check unitId is not -1
								if (currentUnitId > -1) {
									currentUnit = global->GetUnitFromId(currentUnitId);
								}
								if (currentUnit && currentUnit->IsCheckSumValidForAUnitClass()) {
									if (i > tempList.arraySize - 1) {
										// 0x4C03A4. Dead code & unused ? How could usedElemCount be > allocCount ???
										assert(false);
										// I did not rewrote this portion of code, as I don't see the point...
									}
									// Add units that block access to wonder to targets list ??
									// Note: this is cheating, currentUnit may not be visible !
									AOE_METHODS::UNIT::AddUnitInInfAILists(infAI, currentUnitId);
									if (i > tempList.arraySize - 1) {
										// 0x4C0410. Dead code & unused ? How could usedElemCount be > allocCount ???
										assert(false);
										// I did not rewrote this portion of code, as I don't see the point...
									}
									unitGroup->AddTargetUnitIdToArray(currentUnitId);
								}
							}
						}
						unitGroup->AddTargetUnitIdToArray(enemyWonderInfo->unitId);
						unitGroup->targetPlayerId = enemyWonderInfo->playerId; // 0x4C049E
						unitGroup->terrainZoneId = leaderTerrainZoneId;
						if (tempList.unitIdArray) {
							AOEFree(tempList.unitIdArray);
							tempList.unitIdArray = NULL; // ADDED
							tempList.usedElements = tempList.arraySize = 0; // ADDED
						}
						return enemyWonderInfo;
					} else {
						// Move failed, even ignoring enemy units (not sure about this part)
						if (tempList.unitIdArray) {
							AOEFree(tempList.unitIdArray);
							tempList.unitIdArray = NULL; // ADDED
							tempList.usedElements = tempList.arraySize = 0; // ADDED
						}
					}
				} else {
					// Not sure about this. Seems illogical ?
					return enemyWonderInfo;
				}
			}
			// Search is not in progress (we are still in "prepare" phase), no wonder was found.
			// Init search (as no priority target was selected)
			targetInfo->targetUnitId = -1;
			targetInfo->targetEvaluation = -1.f;
			targetInfo->targetInfAIUnitElemListIndex = -1;
			targetInfo->currentSearchInfAIUnitElemListIndex = 0;
			targetInfo->unknown_10 = -1;
			targetInfo->unknown_14 = -1;
			targetInfo->currentSearchIsBuildings = 0; // start with living units
			targetInfo->buildingTargetUnitId = -1;
			targetInfo->buildingTargetEvaluation = -1.f;
			targetInfo->buildingTargetInfAIUnitElemListIndex = -1;
			targetInfo->targetSearchInProgress = 1;
		}
	}

	// main part
	if (targetInfo->currentSearchInfAIUnitElemListIndex < infAI->unitMemoryListSize) {
		// Loop from 0x4C053C to 0x4C0DBD. Loop on infAI.unitElems...
		for (int curIndex = targetInfo->currentSearchInfAIUnitElemListIndex; curIndex < infAI->unitMemoryListSize; curIndex++) {
			if (curIndex > targetInfo->currentSearchInfAIUnitElemListIndex) {
				// For all case EXCEPT first loop: check the time spent
				long int t = AOE_METHODS::TimeGetTime();
				if (t - baseTimeGetTimeValue >= global->tmp_allowedTimeForAITreatment) {
					targetInfo->currentSearchInfAIUnitElemListIndex = curIndex;
					return NULL; // Unfinished business. Note that "in progress" is still 1, to finish (continue) next time.
				}
			}
			STRUCT_UNIT_MEMORY *curElem = &infAI->unitMemoryList[curIndex];
			// curElem->playerId should be always correct (at least for type50+) thanks to RockNRor fixes (update on conversion if visible + updates when visibility changes)
			if (curElem->playerId != targetPlayerId) {
				continue;
			}
			if (curElem->unitId == -1) { continue; } // ADDED
			STRUCT_UNIT_BASE *curUnit = global->GetUnitFromId(curElem->unitId);
			if (!curUnit || !curUnit->IsCheckSumValidForAUnitClass() ||
				(curUnit->unitStatus > GAME_UNIT_STATUS::GUS_2_READY) || (curUnit->remainingHitPoints <= 0) ||
				(!curUnit->unitDefinition || !curUnit->unitDefinition->IsCheckSumValidForAUnitClass())) { // ADDED
				continue;
			}
			assert(curUnit->unitDefinition && curUnit->unitDefinition->IsCheckSumValidForAUnitClass());
			
			// 0x4C05D1
			if (!isBoat && IsWaterUnit(curUnit->unitDefinition->unitAIType)) {
				continue; // do not attack boats with non-water unit groups
			}
			// 0x4C05EA
			long int curUnitTerrainZoneId = groupLeader->GetTerrainZoneIdAtPos((long int)curUnit->positionX, (long int)curUnit->positionY);
			// 0x4C060D. Is the !isBoat condition correct here ?
			if (!isBoat && (curElem->unitDefId == CST_UNITID_DOCK) && (leaderTerrainZoneId != curUnitTerrainZoneId)) {
				continue;
			}
			// 0x4C062B
			// if (!targetInfo->currentSearchIsBuildings) {} // Duplicate if/else code !
			if (curElem->unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupWall) {
				continue;
			}

			bool useOriginalCodeWithSmallCheat = true;

			if (useOriginalCodeWithSmallCheat) {
				// 0x4C0669 = code that checks visibility with a bit of cheating (original code)
				long int diffX = curElem->posX - (long int)curUnit->positionX;
				long int diffY = curElem->posY - (long int)curUnit->positionY;
				if (diffX < 0) { diffX = -diffX; }
				if (diffY < 0) { diffY = -diffY; }
				bool consideredVisible = false;
				if ((diffX <= 5) && (diffY <= 5)) {
					consideredVisible = true; // ah!!! cheating
				} else {
					consideredVisible = PLAYER::IsFogVisibleForPlayer(player, (long int)curUnit->positionX, (long int)curUnit->positionY);
				}
				if (!consideredVisible) {
					continue;
				}
			} else {
				// Code that checks visibility without cheating :
				bool unitCantMove = (AOE_METHODS::UNIT::GetSpeed(curUnit) == 0);
				bool elemIsVisibleNow = (curUnit->unitDefinition->visibleInFog > 0) ||
					unitCantMove ||
					((curElem->posX >= 0) && (curElem->posY >= 0) && PLAYER::IsFogVisibleForPlayer(player, curElem->posX, curElem->posY));
				if (!elemIsVisibleNow) {
					continue; // removes cheating from original game !
				}
			}

			// 0x4C06D6
			if (isPriest && !AOE_METHODS::UNIT::CanConvert(groupLeader, curElem->unitId)) {
				continue;
			}
			// 0x4C0705 : begin calculations
			float groupDamage = ROCKNROR::COMBAT::GetGroupDamageOnUnit(infAI, unitGroup, curUnit); // Damage per second, actually
			if (groupDamage <= 0) { // ADDED. What happens in AOE code? e.g. group has only priests?
				continue;
			}
			float timeToKillEnemy = curUnit->remainingHitPoints / groupDamage;
			if (timeToKillEnemy <= 0) {
				continue; // 0x4C0740
			}
			// 0x4C0728
			float timeToKillMyUnitsAtTargetPos = AOE_METHODS::GetTimeToKillGroupUnitsAtTargetPosition(infAI, unitGroup, curUnit);
			if (timeToKillMyUnitsAtTargetPos <= 0) {
				continue; // 0x4C0755
			}
			float timeKillProportion = timeToKillMyUnitsAtTargetPos / timeToKillEnemy; // 0x4C075F->77C (local_50). High value = good
			float posDiffX = (((float)curElem->posX) - groupLeader->positionX);
			float posDiffY = (((float)curElem->posY) - groupLeader->positionY);
			float distanceCommanderToCurEnemy = sqrtf((posDiffX * posDiffX) + (posDiffY * posDiffY)); // local_30
			if (distanceCommanderToCurEnemy < 1) {
				distanceCommanderToCurEnemy = 1; // 0x4C07C6
			}
			float unknown_4C = (float)curElem->unknown_14; // TODO !!! 0x4C07E9 (note: ESP+50 because a PUSH 0x53 is pending)
			long int targetEvaluationRandomness = tacAI->SNNumber[SNTargetEvaluationRandomness]; // local40
			float randomnessEval = 0;
			if (targetEvaluationRandomness > 0) {
				// We could use AOE's pseudo-randomizer to be respect original code?
				randomnessEval = (float)(randomizer.GetRandomValue(0, 0xFFFF) % targetEvaluationRandomness);
			}
			long int targetEvaluationAttackAttempts = tacAI->SNNumber[SNTargetEvaluationAttackAttempts];
			float attemptsEval = (float)(targetEvaluationAttackAttempts * curElem->attackAttempts); // cf local_54
			float targetEvaluationContinent = 0;
			if (leaderTerrainZoneId == curUnitTerrainZoneId) { // Always true because of previous "if"s ?
				targetEvaluationContinent = (float)tacAI->SNNumber[SNTargetEvaluationContinent];
			}
			long int targetEvaluationInProgress = tacAI->SNNumber[SNTargetEvaluationInProgress];
			float targetInProgressEvaluation = 0; // local_38.
			if (targetEvaluationInProgress > 0) {
				long int alreadyTargetCount = 0; // cf local_2C
				for (int i = 0; i < unitGroup->unitCount; i++) {
					long int myGrpUnitId = unitGroup->GetMyUnitId(i);
					STRUCT_UNIT_BASE * myGrpUnit = NULL;
					if (myGrpUnitId >= 0) { // ADDED
						myGrpUnit = global->GetUnitFromId(myGrpUnitId);
					}
					if (myGrpUnit && myGrpUnit->IsCheckSumValidForAUnitClass() && myGrpUnit->currentActivity) {
						if (myGrpUnit->currentActivity->targetUnitId == curElem->unitId) {
							alreadyTargetCount++;
						}
					}
				}
				targetInProgressEvaluation = (float)(targetEvaluationInProgress * alreadyTargetCount);
			}

			float evalDistanceBetweenLoopElemAndUnknown24Elem = 0; // local_1C
			if ((targetInfo->currentSearchIsBuildings) && (targetInfo->buildingTargetUnitId != -1)) {
				long int unknown24_posX = infAI->unitMemoryList[targetInfo->buildingTargetInfAIUnitElemListIndex].posX;
				long int unknown24_posY = infAI->unitMemoryList[targetInfo->buildingTargetInfAIUnitElemListIndex].posY;
				long int elemsDiffX = curElem->posX - unknown24_posX;
				long int elemsDiffY = curElem->posY - unknown24_posY;
				float elemsDistance = (float)((elemsDiffX*elemsDiffX) + (elemsDiffY*elemsDiffY));
				elemsDistance = sqrtf(elemsDistance);
				if (elemsDistance < 1) {
					evalDistanceBetweenLoopElemAndUnknown24Elem = 5.f;
				} else {
					evalDistanceBetweenLoopElemAndUnknown24Elem = 25000.f / elemsDistance;
				}
			}
			bool canTrainUnits = (curUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) && (
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_BARRACKS) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_RANGE) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_SIEGE_WORKSHOP) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_DOCK) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_ACADEMY) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_STABLE) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_TEMPLE)
				);
			if (canTrainUnits) {
				if ((curUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) ||
					(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_DOCK)) {
					evalDistanceBetweenLoopElemAndUnknown24Elem += 200.f;
				} else {
					evalDistanceBetweenLoopElemAndUnknown24Elem += 300.f;
				}
			}
			long int targetEvaluationKills = tacAI->SNNumber[SNTargetEvaluationKills];
			long int targetEvaluationDistance = tacAI->SNNumber[SNTargetEvaluationDistance];
			long int targetEvaluationTimeKillRatio = tacAI->SNNumber[SNTargetEvaluationTimeKillRatio];
			float unknown_killRating = targetEvaluationKills * unknown_4C; // 0x4C0A32. in local_48_tmp
			assert(distanceCommanderToCurEnemy > 0);
			float distanceEval = ((float)targetEvaluationDistance) / ((float)distanceCommanderToCurEnemy);
			float killAndDistanceRating = distanceEval + unknown_killRating; // 0x4C0A5C. local_48_final

			float totalEvaluation = ((float)targetEvaluationTimeKillRatio) * timeKillProportion; // 0x4C0A70->A77
			totalEvaluation += killAndDistanceRating;
			totalEvaluation += evalDistanceBetweenLoopElemAndUnknown24Elem;
			totalEvaluation += targetInProgressEvaluation; // local_38
			totalEvaluation += targetEvaluationContinent; // local_3C
			totalEvaluation += attemptsEval; // local_54
			totalEvaluation += randomnessEval; // local_40
			if ((targetInfo->targetUnitId != -1) && (targetInfo->targetEvaluation >= totalEvaluation)) { // 0x4C0A99 : compare with previous best score
				continue;
			}
			if (!targetInfo->currentSearchIsBuildings && (AOE_METHODS::UNIT::GetSpeed(curUnit) <= 0)) {
				// A non-building search found a building (or non-moving) unit. Save it, but in "building" slot
				// Improvement: is it a tower ? Can it attack (attack OR convert) ?
				// Note: not checking terrainZone, movement...
				targetInfo->buildingTargetEvaluation = totalEvaluation;
				targetInfo->buildingTargetUnitId = curElem->unitId;
				targetInfo->buildingTargetInfAIUnitElemListIndex = targetInfo->currentSearchInfAIUnitElemListIndex; // NOT using curIndex. Not to skip some buildings in search ?
			}
			long int unknown_EBP = targetInfo->currentSearchIsBuildings ? -1 : 0x1B;
			STRUCT_MANAGED_ARRAY tempListForMove;
			tempListForMove.arraySize = 0;
			tempListForMove.maxElementCount = 0;
			tempListForMove.unitIdArray = NULL;
			tempListForMove.usedElements = 0;
			if (leaderTerrainZoneId == curUnitTerrainZoneId) {
				bool canMoveToTarget = AOE_METHODS::UNIT::CanMoveToTarget(groupLeader, curElem->unitId,
					AOE_METHODS::UNIT::GetMaxRange(groupLeader), true);
				if (!canMoveToTarget) {
					bool canMoveToTargetIfKillingEnemies = AOE_METHODS::UNIT::MoveToTarget_1A0(groupLeader, curElem->unitId,
						AOE_METHODS::UNIT::GetMaxRange(groupLeader), 0, 1, targetPlayerId, unknown_EBP, &tempListForMove);
				}
			}
			unitGroup->targetUnitIdArrayUsedElemCount = 0; // reset targets array.
			if (tempListForMove.usedElements > 0) {
				// Only if we used the "force" mode: add found units (that block the way?) in infAI lists & as group targets
				for (int i = 0; i < tempListForMove.usedElements; i++) {
					if (i >= tempListForMove.arraySize) {
						// 0x4C0BC6 - I don't see the point of this code
						// TODO
						assert(false && "How can usedElemCount be > arraySize ?");
					}
					// 0x4C0C17
					// Should check that unitId > -1 ?
					STRUCT_UNIT_BASE *tempListUnit = global->GetUnitFromId(tempListForMove.unitIdArray[i]);
					if (tempListUnit && tempListUnit->IsCheckSumValidForAUnitClass()) {
						// 0x4C0C32
						if (i >= tempListForMove.arraySize) {
							// 0x4C0C39 - I don't see the point of this code... Internship guy striked again !?
							// TODO
							//assert(false && "How can usedElemCount be > arraySize ?");
						}
						// 0x4C0C8C
						AOE_METHODS::UNIT::AddUnitInInfAILists(infAI, tempListForMove.unitIdArray[i]);
						if (i >= tempListForMove.arraySize) {
							// 0x4C0CA4 - I don't see the point of this code. WTF ? Maybe this was a macro, used a bit too much ?
							// TODO
							//assert(false && "How can usedElemCount be > arraySize ?");
						}
						// 0x4C0CF5. Add secondary target ?
						unitGroup->AddTargetUnitIdToArray(tempListForMove.unitIdArray[i]);
					}
				}
			}
			unitGroup->AddTargetUnitIdToArray(curElem->unitId);
			unitGroup->targetPlayerId = curElem->playerId;
			unitGroup->terrainZoneId = leaderTerrainZoneId;
			targetInfo->targetUnitId = curElem->unitId;
			targetInfo->targetEvaluation = totalEvaluation;
			targetInfo->targetInfAIUnitElemListIndex = curIndex;
			if (tempListForMove.unitIdArray != NULL) {
				AOEFree(tempListForMove.unitIdArray);
			}
			tempListForMove.arraySize = 0;
			tempListForMove.maxElementCount = 0;
			tempListForMove.unitIdArray = NULL;
			tempListForMove.usedElements = 0;
		}
	}
	if (targetInfo->targetUnitId == -1) { // not found
		// 0x4C0E13
		if (!targetInfo->currentSearchIsBuildings) {
			targetInfo->currentSearchIsBuildings = 1; // 0x4C0E1A
			targetInfo->currentSearchInfAIUnitElemListIndex = 0; // restart from beginning (search remains in progress). Exit this method, but next call will go on with buildings.
		} else {
			targetInfo->targetSearchInProgress = 0; // 0x4C0E27. Search completed but found nothing.
		}
		return NULL; // Not found
	}
	assert(targetInfo->targetInfAIUnitElemListIndex >= 0);
	assert(targetInfo->targetInfAIUnitElemListIndex < infAI->unitMemoryListSize);
	targetInfo->targetSearchInProgress = 0; // 0x4C0DF1
	infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex].attackAttempts++; // 0x4C0DEF. WARNING: also done by caller but only in 0x4D4376
#ifdef _DEBUG
	// Just to make *really* sure this is always true. To remove later.
	assert(&infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex] == AOE_METHODS::LISTS::FindInfAIUnitMemoryElem(infAI, unitGroup->GetTargetUnitIdFromArray(0)));
#endif
	return &infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex];
}




// NOTE: this corresponds to original AOE method, kept for backup/information/debugging. Do not use this method
// baseTimeGetTimeValue = TimeGetTime() value (ms) for total AI treatment process time calculation. We must not go over global->tmp_allowedTimeForAITreatment
// If successful, returns a pointer to the selected STRUCT_UNIT_MEMORY element.
// If successful, unitGroup targets array contains only 1 target (in most cases... to check in case when units block the way to main target)
// Returns false if found nothing OR if search could not be completed in time.
// *********** DO NOT MODIFY THIS METHOD *****************
#ifdef _DEBUG
STRUCT_UNIT_MEMORY *LEGACY_FindGroupMainTarget(STRUCT_INF_AI *infAI, long int targetPlayerId,
	STRUCT_UNIT_GROUP *unitGroup, STRUCT_TAC_AI_TARGET_INFO *targetInfo, long int baseTimeGetTimeValue) {
	// Collect info / Check parameters
	if (!targetInfo) { return NULL; }
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	if ((targetPlayerId <= 0) || (targetPlayerId >= global->playerTotalCount)) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_AI *mainAI = infAI->ptrMainAI;
	assert(mainAI && mainAI->IsCheckSumValid());
	if (!mainAI || !mainAI->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_PLAYER *player = mainAI->player;
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }
	STRUCT_UNIT_BASE *groupLeader = global->GetUnitFromId(unitGroup->commanderUnitId);
	if (!groupLeader || !groupLeader->IsCheckSumValidForAUnitClass()) { targetInfo->targetSearchInProgress = 0; return NULL; } // 0x4BFFE2
	if (!groupLeader->currentActivity) { targetInfo->targetSearchInProgress = 0; return NULL; } // 0x4BFFF1
	STRUCT_TAC_AI *tacAI = &infAI->ptrMainAI->structTacAI;
	assert(tacAI && tacAI->IsCheckSumValid());
	if (!tacAI || !tacAI->IsCheckSumValid()) { targetInfo->targetSearchInProgress = 0; return NULL; }

	char leaderTerrainZoneId = groupLeader->GetMyTerrainZoneId(); // 0x4BFFF9
	STRUCT_UNITDEF_BASE *groupLeaderDef = groupLeader->unitDefinition;
	assert(groupLeaderDef && groupLeaderDef->IsCheckSumValidForAUnitClass());
	bool isPriest = groupLeaderDef->unitAIType == TribeAIGroupPriest;
	bool isBoat = IsWaterUnit(groupLeaderDef->unitAIType); // 0x4C001F

	// 0x4C003F
	if (!targetInfo->targetSearchInProgress) {
		// Initialize search ?
		if ((targetPlayerId == unitGroup->targetPlayerId) && // 0x4C0054
			(unitGroup->targetUnitIdArrayUsedElemCount >= 1) && // 0x4C0062
			((char)unitGroup->terrainZoneId) == leaderTerrainZoneId) { // 0x4C007A
			long int lastTargetUnitId = unitGroup->GetTargetUnitIdFromArray(unitGroup->targetUnitIdArrayUsedElemCount - 1);
			// There is a bug here in original code with unitId -1 !
			STRUCT_UNIT_BASE *targetUnit = NULL;
			if (lastTargetUnitId >= 0) {
				targetUnit = global->GetUnitFromId(lastTargetUnitId);
			}
			// 0x4C0095
			if (targetUnit && targetUnit->IsCheckSumValidForAUnitClass()) {
				STRUCT_PLAYER *targetPlayer = targetUnit->ptrStructPlayer;
				assert(targetPlayer && targetPlayer->IsCheckSumValid());
				long int targetUnitPlayerId = targetPlayer->playerId;
				bool isEnemy = (targetUnitPlayerId > 0) &&
					(player->ptrDiplomacyStances[targetPlayerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ENEMY);
				if (isEnemy && (targetUnit->unitStatus <= AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY)) {
					// 0x4C00C7-0x4C00FF
					long int canMoveToTarget = AOE_METHODS::UNIT::CanMoveToTarget(groupLeader, targetUnit->unitInstanceId,
						AOE_METHODS::UNIT::GetMaxRange(groupLeader), true);
					if (canMoveToTarget) {
						// 0x4C010A
						unitGroup->targetUnitIdArrayUsedElemCount = 0; // Reset (no need to erase values from array)
						unitGroup->AddTargetUnitIdToArray(targetUnit->unitInstanceId);
						return AOE_METHODS::LISTS::FindInfAIUnitMemoryElem(infAI, targetUnit->unitInstanceId); // 0x4C0135
					}
				}
			}
			// 0x4C013A. Strange conception: why not just do a (reverse) loop instead of duplicate code with code above ?
			for (int i = 0; i < unitGroup->targetUnitIdArrayUsedElemCount; i++) {
				long int loopTargetId = unitGroup->GetTargetUnitIdFromArray(i);
				STRUCT_UNIT_BASE *loopTargetUnit = NULL;
				if (loopTargetId >= 0) { // Missing check in original code
					loopTargetUnit = global->GetUnitFromId(loopTargetId); // 0x4C0164
				}
				if (loopTargetUnit && loopTargetUnit->IsCheckSumValidForAUnitClass() && loopTargetUnit->ptrStructPlayer &&
					loopTargetUnit->ptrStructPlayer->IsCheckSumValid()) {
					// 0x4C0171
					long int loopTargetPlayerId = loopTargetUnit->ptrStructPlayer->playerId;
					long int isLoopUnitEnemy = (loopTargetPlayerId > 0) &&
						(player->ptrDiplomacyStances[loopTargetPlayerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ENEMY);
					if (isLoopUnitEnemy && (loopTargetUnit->unitStatus <= AOE_CONST_INTERNAL::GAME_UNIT_STATUS::GUS_2_READY)) {
						// 0x4C0199
						long int canMoveToTarget = AOE_METHODS::UNIT::CanMoveToTarget(groupLeader, loopTargetId,
							AOE_METHODS::UNIT::GetMaxRange(groupLeader), true);
						if (canMoveToTarget) {
							return AOE_METHODS::LISTS::FindInfAIUnitMemoryElem(infAI, loopTargetUnit->unitInstanceId); // 0x4C0232 to 0x4C0246
						}
					}
				}
			}
		}
		// 0x4C01E8
		if (!targetInfo->targetSearchInProgress) { // always true: same condition as parent if
			STRUCT_UNIT_MEMORY *enemyWonderInfo = unitTargetingHandler.FindWonderToAttackUnitMemoryPtr(infAI, -1);
			if (enemyWonderInfo) {
				// 0x4C0208
				char wonderTerrainZoneId = groupLeader->GetTerrainZoneIdAtPos(enemyWonderInfo->posX, enemyWonderInfo->posY);
				if ((wonderTerrainZoneId == leaderTerrainZoneId) || isBoat) {
					// 0x4C024B
					long int canMoveToTarget = AOE_METHODS::UNIT::CanMoveToTarget(groupLeader, enemyWonderInfo->unitId,
						AOE_METHODS::UNIT::GetMaxRange(groupLeader), true); // 0x4C0272
					if (canMoveToTarget) {
						// 0x4C027D
						unitGroup->AddTargetUnitIdToArray(enemyWonderInfo->unitId);
						return enemyWonderInfo;
					}
					// 0x4C02B6
					STRUCT_MANAGED_ARRAY tempList;
					// This call searches a path, even if enemy units block the way ? Unsure
					bool success = AOE_METHODS::UNIT::MoveToTarget_1A0(groupLeader, enemyWonderInfo->unitId, AOE_METHODS::UNIT::GetMaxRange(groupLeader),
						0, 1, targetPlayerId, 0x1B, &tempList);
					if (success) {
						// 0x4C030F
						if (tempList.usedElements > 0) {
							for (int i = 0; i < tempList.usedElements; i++) { // 0x4C0323
								if (i > tempList.arraySize) {
									// 0x4C032C. Dead code & unused ? How could usedElemCount be > allocCount ???
									assert(false);
									// I did not rewrote this portion of code, as I don't see the point...
								}
								// 0x4C0380
								long int currentUnitId = tempList.unitIdArray[i];
								STRUCT_UNIT_BASE *currentUnit = NULL;
								// ADDED: check unitId is not -1
								if (currentUnitId > -1) {
									currentUnit = global->GetUnitFromId(currentUnitId);
								}
								if (currentUnit && currentUnit->IsCheckSumValidForAUnitClass()) {
									if (i > tempList.arraySize - 1) {
										// 0x4C03A4. Dead code & unused ? How could usedElemCount be > allocCount ???
										assert(false);
										// I did not rewrote this portion of code, as I don't see the point...
									}
									// 0x4C03F8
									// Add units that block access to wonder to targets list ??
									// Note: this is cheating, currentUnit may not be visible !
									AOE_METHODS::UNIT::AddUnitInInfAILists(infAI, currentUnitId);
									if (i > tempList.arraySize - 1) {
										// 0x4C0410. Dead code & unused ? How could usedElemCount be > allocCount ???
										assert(false);
										// I did not rewrote this portion of code, as I don't see the point...
									}
									// 0x4C0464
									unitGroup->AddTargetUnitIdToArray(currentUnitId);
								}
								// 0x4C0473: end of loop => back to 0x4C0323
							}
						}
						// 0x4C0480
						unitGroup->AddTargetUnitIdToArray(enemyWonderInfo->unitId);
						unitGroup->targetPlayerId = enemyWonderInfo->playerId; // 0x4C049E
						unitGroup->terrainZoneId = leaderTerrainZoneId;
						if (tempList.unitIdArray) {
							AOEFree(tempList.unitIdArray);
							tempList.unitIdArray = NULL; // ADDED
							tempList.usedElements = tempList.arraySize = 0; // ADDED
						}
						return enemyWonderInfo; // 0x4C04C6
					} else {
						// Move failed, even ignoring enemy units (not sure about this part)
						// 0x4C04CB
						if (tempList.unitIdArray) {
							AOEFree(tempList.unitIdArray);
							tempList.unitIdArray = NULL; // ADDED
							tempList.usedElements = tempList.arraySize = 0; // ADDED
						}
					}
				} else {
					// 0x4C0229
					// Not sure about this. Seems illogical ?
					return enemyWonderInfo;
				}
			}
			// 0x4C04E6 / 0x4C04E9 (same, 0x4C04E6 just sets EAX=-1 if not already done)
			// Search is not in progress (we are still in "prepare" phase), no wonder was found.
			// Init search (as no priority target was selected)
			targetInfo->targetUnitId = -1;
			targetInfo->targetEvaluation = -1.f;
			targetInfo->targetInfAIUnitElemListIndex = -1;
			targetInfo->currentSearchInfAIUnitElemListIndex = 0;
			targetInfo->unknown_10 = -1;
			targetInfo->unknown_14 = -1;
			targetInfo->currentSearchIsBuildings = 0; // start with living units
			targetInfo->buildingTargetUnitId = -1;
			targetInfo->buildingTargetEvaluation = -1.f;
			targetInfo->buildingTargetInfAIUnitElemListIndex = -1;
			targetInfo->targetSearchInProgress = 1;
		}
	}

	// 0x4C0517 : main part
	if (targetInfo->currentSearchInfAIUnitElemListIndex < infAI->unitMemoryListSize) {
		// Loop from 0x4C053C to 0x4C0DBD. Loop on infAI.unitElems...
		for (int curIndex = targetInfo->currentSearchInfAIUnitElemListIndex; curIndex < infAI->unitMemoryListSize; curIndex++) {
			if (curIndex > targetInfo->currentSearchInfAIUnitElemListIndex) {
				// For all case EXCEPT first loop: check the time spent
				long int t = AOE_METHODS::TimeGetTime();
				if (t - baseTimeGetTimeValue >= global->tmp_allowedTimeForAITreatment) {
					// 0x4C0E07
					targetInfo->currentSearchInfAIUnitElemListIndex = curIndex;
					return NULL; // Unfinished business. Note that "in progress" is still 1, to finish (continue) next time.
				}
			}
			// 0x4C0576
			STRUCT_UNIT_MEMORY *curElem = &infAI->unitMemoryList[curIndex];
//#pragma message("We should check that playerId is correct: may have changed due to a conversion. Beware fog-visibility too")
			if (curElem->playerId != targetPlayerId) {
				continue;
			}
			if (curElem->unitId == -1) { continue; } // ADDED
			STRUCT_UNIT_BASE *curUnit = global->GetUnitFromId(curElem->unitId);
			if (!curUnit || !curUnit->IsCheckSumValidForAUnitClass() ||
				(curUnit->unitStatus > GAME_UNIT_STATUS::GUS_2_READY) || (curUnit->remainingHitPoints <= 0) ||
				(!curUnit->unitDefinition || !curUnit->unitDefinition->IsCheckSumValidForAUnitClass())) { // ADDED
				continue;
			}
			assert(curUnit->unitDefinition && curUnit->unitDefinition->IsCheckSumValidForAUnitClass());
			// 0x4C05D1
			if (!isBoat && IsWaterUnit(curUnit->unitDefinition->unitAIType)) {
				continue; // do not attack boats with non-water unit groups
			}
			// 0x4C05EA
			// CHEATING : unit may not be visible
			long int curUnitTerrainZoneId = groupLeader->GetTerrainZoneIdAtPos((long int)curUnit->positionX, (long int)curUnit->positionY);
			// 0x4C060D. Is the !isBoat condition correct here ?
			if (!isBoat && (curElem->unitDefId == CST_UNITID_DOCK) && (leaderTerrainZoneId != curUnitTerrainZoneId)) {
				continue;
			}
			// 0x4C062B
			// if (!targetInfo->currentSearchIsBuildings) {} // Duplicate if/else code !
			if (curElem->unitClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupWall) {
				continue;
			}
			// 0x4C0669
			long int diffX = curElem->posX - (long int)curUnit->positionX;
			long int diffY = curElem->posY - (long int)curUnit->positionY;
			if (diffX < 0) { diffX = -diffX; }
			if (diffY < 0) { diffY = -diffY; }
			bool consideredVisible = false;
			if ((diffX <= 5) && (diffY <= 5)) {
				consideredVisible = true; // ah!!! cheating
			} else {
				consideredVisible = PLAYER::IsFogVisibleForPlayer(player, (long int)curUnit->positionX, (long int)curUnit->positionY);
			}
			if (!consideredVisible) {
				continue;
			}
			// 0x4C06D6
			if (isPriest && !AOE_METHODS::UNIT::CanConvert(groupLeader, curElem->unitId)) {
				continue;
			}
			// 0x4C0705 : begin calculations
			float groupDamage = ROCKNROR::COMBAT::GetGroupDamageOnUnit(infAI, unitGroup, curUnit); // Damage per second, actually
			if (groupDamage <= 0) { // ADDED. What happens in AOE code? e.g. group has only priests?
				continue;
			}
			float timeToKillEnemy = curUnit->remainingHitPoints / groupDamage;
			if (timeToKillEnemy <= 0) {
				continue; // 0x4C0740
			}
			// 0x4C0728
			float timeToKillMyUnitsAtTargetPos = AOE_METHODS::GetTimeToKillGroupUnitsAtTargetPosition(infAI, unitGroup, curUnit);
			if (timeToKillMyUnitsAtTargetPos <= 0) {
				continue; // 0x4C0755
			}
			float timeKillProportion = timeToKillMyUnitsAtTargetPos / timeToKillEnemy; // 0x4C075F->77C (local_50). High value = good
			float posDiffX = (((float)curElem->posX) - groupLeader->positionX);
			float posDiffY = (((float)curElem->posY) - groupLeader->positionY);
			float distanceCommanderToCurEnemy = sqrtf((posDiffX * posDiffX) + (posDiffY * posDiffY)); // local_30
			if (distanceCommanderToCurEnemy < 1) {
				distanceCommanderToCurEnemy = 1; // 0x4C07C6
			}
			float unknown_4C = (float)curElem->unknown_14; // TODO !!! 0x4C07E9 (note: ESP+50 because a PUSH 0x53 is pending)
			long int targetEvaluationRandomness = tacAI->SNNumber[SNTargetEvaluationRandomness]; // local40
			float randomnessEval = 0;
			if (targetEvaluationRandomness > 0) {
				// 0x4C07F6
				// We could use AOE's pseudo-randomizer to be respect original code?
				randomnessEval = (float)(randomizer.GetRandomValue(0, 0xFFFF) % targetEvaluationRandomness);
			}
			long int targetEvaluationAttackAttempts = tacAI->SNNumber[SNTargetEvaluationAttackAttempts];
			// 0x4C0821
			float attemptsEval = (float)(targetEvaluationAttackAttempts * curElem->attackAttempts); // cf local_54
			float targetEvaluationContinent = 0;
			if (leaderTerrainZoneId == curUnitTerrainZoneId) { // Always true because of previous "if"s ?
				targetEvaluationContinent = (float)tacAI->SNNumber[SNTargetEvaluationContinent];
			} // 0x4C0881
			long int targetEvaluationInProgress = tacAI->SNNumber[SNTargetEvaluationInProgress];
			float targetInProgressEvaluation = 0; // local_38.
			if (targetEvaluationInProgress > 0) {
				long int alreadyTargetCount = 0; // cf local_2C
				for (int i = 0; i < unitGroup->unitCount; i++) {
					// 0x4C08C7
					long int myGrpUnitId = unitGroup->GetMyUnitId(i);
					STRUCT_UNIT_BASE * myGrpUnit = NULL;
					if (myGrpUnitId >= 0) { // ADDED
						myGrpUnit = global->GetUnitFromId(myGrpUnitId);
					}
					if (myGrpUnit && myGrpUnit->IsCheckSumValidForAUnitClass() && myGrpUnit->currentActivity) {
						if (myGrpUnit->currentActivity->targetUnitId == curElem->unitId) {
							alreadyTargetCount++;
						}
					}
				}
				// 0x4C0919
				targetInProgressEvaluation = (float)(targetEvaluationInProgress * alreadyTargetCount);
			} // 0x4C093F

			float evalDistanceBetweenLoopElemAndUnknown24Elem = 0; // local_1C
			if ((targetInfo->currentSearchIsBuildings) && (targetInfo->buildingTargetUnitId != -1)) {
				// 0x4C095A
				long int unknown24_posX = infAI->unitMemoryList[targetInfo->buildingTargetInfAIUnitElemListIndex].posX;
				long int unknown24_posY = infAI->unitMemoryList[targetInfo->buildingTargetInfAIUnitElemListIndex].posY;
				long int elemsDiffX = curElem->posX - unknown24_posX;
				long int elemsDiffY = curElem->posY - unknown24_posY;
				float elemsDistance = (float)((elemsDiffX*elemsDiffX) + (elemsDiffY*elemsDiffY));
				elemsDistance = sqrtf(elemsDistance); // 0x4C09A2
				if (elemsDistance < 1) {
					evalDistanceBetweenLoopElemAndUnknown24Elem = 5.f;
				} else {
					evalDistanceBetweenLoopElemAndUnknown24Elem = 25000.f / elemsDistance;
				}
			}
			// 0x4C09CB
			bool canTrainUnits = (curUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) && (
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_BARRACKS) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_RANGE) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_SIEGE_WORKSHOP) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_DOCK) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_ACADEMY) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_STABLE) ||
				(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_TEMPLE)
				); // Cf call to 0x4BE250
			if (canTrainUnits) {
				if ((curUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) ||
					(curUnit->unitDefinition->DAT_ID1 == CST_UNITID_DOCK)) {
					evalDistanceBetweenLoopElemAndUnknown24Elem += 200.f; // 0x4C09FF
				} else {
					evalDistanceBetweenLoopElemAndUnknown24Elem += 300.f; // 0x4C09F3
				}
			} // 0x4C0A09
			long int targetEvaluationKills = tacAI->SNNumber[SNTargetEvaluationKills];
			long int targetEvaluationDistance = tacAI->SNNumber[SNTargetEvaluationDistance];
			long int targetEvaluationTimeKillRatio = tacAI->SNNumber[SNTargetEvaluationTimeKillRatio];
			float unknown_killRating = targetEvaluationKills * unknown_4C; // 0x4C0A32. in local_48_tmp
			assert(distanceCommanderToCurEnemy > 0);
			float distanceEval = ((float)targetEvaluationDistance) / ((float)distanceCommanderToCurEnemy);
			float killAndDistanceRating = distanceEval + unknown_killRating; // 0x4C0A5C. local_48_final

			float totalEvaluation = ((float)targetEvaluationTimeKillRatio) * timeKillProportion; // 0x4C0A70->A77
			totalEvaluation += killAndDistanceRating; // 0x4C0A7B
			totalEvaluation += evalDistanceBetweenLoopElemAndUnknown24Elem;
			totalEvaluation += targetInProgressEvaluation; // local_38
			totalEvaluation += targetEvaluationContinent; // local_3C
			totalEvaluation += attemptsEval; // local_54
			totalEvaluation += randomnessEval; // local_40
			if ((targetInfo->targetUnitId != -1) && (targetInfo->targetEvaluation >= totalEvaluation)) { // 0x4C0A99 : compare with previous best score
				continue;
			} // 0x4C0ABA
			if (!targetInfo->currentSearchIsBuildings && (AOE_METHODS::UNIT::GetSpeed(curUnit) <= 0)) {
				// A non-building search found a building (or non-moving) unit. Save it, but in "building" slot
				// Improvement: is it a tower ? Can it attack (attack OR convert) ?
				// Note: not checking terrainZone, movement...
				targetInfo->buildingTargetEvaluation = totalEvaluation;
				targetInfo->buildingTargetUnitId = curElem->unitId;
				targetInfo->buildingTargetInfAIUnitElemListIndex = targetInfo->currentSearchInfAIUnitElemListIndex; // NOT using curIndex. Not to skip some buildings in search ?
			} // 0x4C0AF8
			long int unknown_EBP = targetInfo->currentSearchIsBuildings ? -1 : 0x1B;
			STRUCT_MANAGED_ARRAY tempListForMove;
			tempListForMove.arraySize = 0;
			tempListForMove.maxElementCount = 0;
			tempListForMove.unitIdArray = NULL;
			tempListForMove.usedElements = 0;
			if (leaderTerrainZoneId == curUnitTerrainZoneId) {
				bool canMoveToTarget = AOE_METHODS::UNIT::CanMoveToTarget(groupLeader, curElem->unitId,
					AOE_METHODS::UNIT::GetMaxRange(groupLeader), true);
				if (!canMoveToTarget) {
					// 0x4C0B62
					bool canMoveToTargetIfKillingEnemies = AOE_METHODS::UNIT::MoveToTarget_1A0(groupLeader, curElem->unitId,
						AOE_METHODS::UNIT::GetMaxRange(groupLeader), 0, 1, targetPlayerId, unknown_EBP, &tempListForMove);
				}
			}
			// 0x4C0BA2
			unitGroup->targetUnitIdArrayUsedElemCount = 0; // reset targets array.
			if (tempListForMove.usedElements > 0) {
				// Only if we used the "force" mode: add found units (that block the way?) in infAI lists & as group targets
				// TODO 0x4C0BBD
				for (int i = 0; i < tempListForMove.usedElements; i++) {
					if (i >= tempListForMove.arraySize) {
						// 0x4C0BC6 - I don't see the point of this code
						// TODO
						assert(false && "How can usedElemCount be > arraySize ?");
					}
					// 0x4C0C17
					// Should check that unitId > -1 ?
					STRUCT_UNIT_BASE *tempListUnit = global->GetUnitFromId(tempListForMove.unitIdArray[i]);
					if (tempListUnit && tempListUnit->IsCheckSumValidForAUnitClass()) {
						// 0x4C0C32
						if (i >= tempListForMove.arraySize) {
							// 0x4C0C39 - I don't see the point of this code... Internship guy striked again !?
							// TODO
							//assert(false && "How can usedElemCount be > arraySize ?");
						}
						// 0x4C0C8C
						AOE_METHODS::UNIT::AddUnitInInfAILists(infAI, tempListForMove.unitIdArray[i]);
						if (i >= tempListForMove.arraySize) {
							// 0x4C0CA4 - I don't see the point of this code. WTF ? Maybe this was a macro, used a bit too much ?
							// TODO
							//assert(false && "How can usedElemCount be > arraySize ?");
						}
						// 0x4C0CF5. Add secondary target ?
						unitGroup->AddTargetUnitIdToArray(tempListForMove.unitIdArray[i]);
					}
				}
			} // 0x4C0D16
			unitGroup->AddTargetUnitIdToArray(curElem->unitId);
			unitGroup->targetPlayerId = curElem->playerId;
			unitGroup->terrainZoneId = leaderTerrainZoneId;
			targetInfo->targetUnitId = curElem->unitId;
			targetInfo->targetEvaluation = totalEvaluation;
			targetInfo->targetInfAIUnitElemListIndex = curIndex;
			if (tempListForMove.unitIdArray != NULL) {
				AOEFree(tempListForMove.unitIdArray); // 0x4C0D89
			}
			tempListForMove.arraySize = 0;
			tempListForMove.maxElementCount = 0;
			tempListForMove.unitIdArray = NULL;
			tempListForMove.usedElements = 0;
			// 0x4C0DA1 : end of one loop (next element)
		}
		// 0x4C0DC3
	}
	// 0x4C0DD1
	if (targetInfo->targetUnitId == -1) { // not found
		// 0x4C0E13
		if (!targetInfo->currentSearchIsBuildings) {
			targetInfo->currentSearchIsBuildings = 1; // 0x4C0E1A
			targetInfo->currentSearchInfAIUnitElemListIndex = 0; // restart from beginning (search remains in progress). Exit this method, but next call will go on with buildings.
		} else {
			targetInfo->targetSearchInProgress = 0; // 0x4C0E27. Search completed but found nothing.
		}
		return NULL; // Not found
	}
	// 0x4C0DD6
	assert(targetInfo->targetInfAIUnitElemListIndex >= 0);
	assert(targetInfo->targetInfAIUnitElemListIndex < infAI->unitMemoryListSize);
	targetInfo->targetSearchInProgress = 0; // 0x4C0DF1
	infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex].attackAttempts++; // 0x4C0DEF. WARNING: also done by caller but only in 0x4D4376
	// 0x4C0E00
	// Extremely NOT optimized (lol) ! Doing a whole loop to find something we already have = &infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex]
	STRUCT_UNIT_MEMORY *result = AOE_METHODS::LISTS::FindInfAIUnitMemoryElem(infAI, unitGroup->GetTargetUnitIdFromArray(0));
	assert(&infAI->unitMemoryList[targetInfo->targetInfAIUnitElemListIndex] == result); // TEST for optimization (TODO)
	return result;
}
#endif


}
