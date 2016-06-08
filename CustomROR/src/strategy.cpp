#include "../include/strategy.h"



// Returns true if the research is present in tech tree but not researched yet (nor being researched)
bool IsResearchRelevantForStrategy(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, short int research_id) {
	AOE_CONST_FUNC::RESEARCH_STATUSES status = GetResearchStatus(player, research_id);
	return (status == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_WAITING_REQUIREMENT) ||
		(status == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE);
}


// Returns 1st element position (>=0) if (at least) 1 matching element exists in strategy. -1=no such element
long int FindElementPosInStrategy(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, AOE_CONST_FUNC::TAIUnitClass elementType, short int DAT_ID) {
	if (!player) { return -1; }
	ROR_STRUCTURES_10C::STRUCT_AI *mainAI = player->GetAIStruct();
	if (!mainAI) { return -1; }
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeStratElem = &mainAI->structBuildAI.fakeFirstStrategyElement;
	assert(fakeStratElem != NULL); // How could it be ?
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeStratElem->next;
	while ((currentStratElem != NULL) && (currentStratElem != fakeStratElem)) {
		if ((currentStratElem->elementType == elementType) && (currentStratElem->unitDAT_ID == DAT_ID)) {
			return currentStratElem->counter;
		}
		currentStratElem = currentStratElem->next;
	}
	return -1;
}




// This fixes dynamically added houses, boats + boat techs, docks, setgatherpercentage
void FixAutoBuildStrategyElements(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI) {
	if (!buildAI) { return; }
	assert(buildAI->IsCheckSumValid());
	if (!buildAI->IsCheckSumValid()) { return; }

	// Some locals to save important elements
	//ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *toolAge = NULL;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *bronzeAge = NULL;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *ironAge = NULL;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *mediumWarShip = NULL;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *trireme = NULL;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *heavyTransport = NULL;

	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentStratElem = buildAI->fakeFirstStrategyElement.next;
	// Loop on strategy elements to find what's to fix
	while (currentStratElem && (currentStratElem != &buildAI->fakeFirstStrategyElement)) {
		// Dynamically-added medium war ship research has a wrong name. Fix it
		if ((currentStratElem->elementType == AOE_CONST_FUNC::AIUCTech) && (currentStratElem->unitDAT_ID == CST_RSID_MEDIUM_WAR_SHIP) &&
			(strcmp(currentStratElem->unitName, "Soldier-Archer2")) == 0) {
			strcpy_s(currentStratElem->unitName, 30, "MediumWarShip_Added");
		}
		// Also fix other research names
		if ((currentStratElem->elementType == AOE_CONST_FUNC::AIUCTech) &&
			(strcmp(currentStratElem->unitName, "AddedBuildListItem")) == 0) {
			if (currentStratElem->unitDAT_ID == CST_RSID_TRIREME) {
				strcpy_s(currentStratElem->unitName, 30, "Trireme_Added");
			}
			if (currentStratElem->unitDAT_ID == CST_RSID_HEAVY_TRANSPORT) {
				strcpy_s(currentStratElem->unitName, 30, "HeavyTransport_Added");
			}
		}

		// save some informatoin: Research/critical researches (no distinction here)
		if ((currentStratElem->elementType == AOE_CONST_FUNC::AIUCTech) || (currentStratElem->elementType == AOE_CONST_FUNC::AIUCCritical)) {
			/*if (currentStratElem->unitDAT_ID == CST_RSID_TOOL_AGE) {
			toolAge = currentStratElem;
			}*/
			if (currentStratElem->unitDAT_ID == CST_RSID_BRONZE_AGE) {
				bronzeAge = currentStratElem;
			}
			if (currentStratElem->unitDAT_ID == CST_RSID_IRON_AGE) {
				ironAge = currentStratElem;
			}
			if (currentStratElem->unitDAT_ID == CST_RSID_MEDIUM_WAR_SHIP) {
				mediumWarShip = currentStratElem;
			}
			if (currentStratElem->unitDAT_ID == CST_RSID_TRIREME) {
				trireme = currentStratElem;
			}
			if (currentStratElem->unitDAT_ID == CST_RSID_HEAVY_TRANSPORT) {
				heavyTransport = currentStratElem;
			}
		}
		currentStratElem = currentStratElem->next;
	}

	// Move elements if some dependencies are not respected
	MoveStrategyElement_after_ifWrongOrder(buildAI, bronzeAge, mediumWarShip); // move mediumWarShip if needed
	MoveStrategyElement_after_ifWrongOrder(buildAI, ironAge, trireme); // ...
	MoveStrategyElement_after_ifWrongOrder(buildAI, mediumWarShip, trireme);
	MoveStrategyElement_after_ifWrongOrder(buildAI, ironAge, heavyTransport);

	assert(buildAI->mainAI != NULL);
	if (buildAI->mainAI != NULL) {
		// Useful to associate houses, docks and possible boats (in scenario) to existing units so that AI doesn't re-creates them twice.
		UpdateStrategyWithUnreferencedExistingUnits(buildAI->mainAI->ptrStructPlayer);
	}
}


// Sets a research status to "alive" in strategy ; useful in some cases that are not managed automatically
void MarkResearchAsDoneInStrategy(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, short int DATID) {
	if (!buildAI) { return; }
	assert(buildAI->IsCheckSumValid());
	if (!buildAI->IsCheckSumValid()) { return; }

	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentStratElem = buildAI->fakeFirstStrategyElement.next;
	while ((currentStratElem != NULL) && (currentStratElem != &buildAI->fakeFirstStrategyElement)) {
		if ((currentStratElem->elementType == TAIUnitClass::AIUCTech) && (currentStratElem->unitDAT_ID == DATID)) {
			currentStratElem->inProgressCount = 0;
			currentStratElem->aliveCount = 1;
		}
		currentStratElem = currentStratElem->next;
	}
}



// Update strategy to add existing units so AI won't train/build it again (to call after enabling AI on a player)
// Warning: when we just enabled a player's AI, its AI initialization has not occurred yet.
// For example, houses (or farms) will not exist in Strategy. You may have to call this again when init has just been done.
// The method will do nothing if player is NULL or if its AI structure is NULL.
void UpdateStrategyWithUnreferencedExistingUnits(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	if (!player) { return; }
	ROR_STRUCTURES_10C::STRUCT_AI *mainAI = player->GetAIStruct();
	if (!mainAI) { return; }
	// Call generic method
	UpdateStrategyWithUnreferencedExistingUnits(&mainAI->structBuildAI, -1, TAIUnitClass::AIUCNone);
}


// Update strategy to add existing units so AI won't train/build it again. Check will only be run on matching strategy elements.
// This overload is a bit faster because it filters unit types.
// You can use DAT_ID=-1 and elemType=AIUCNone as jokers (to apply on all strategy elements)
// We consider it is a technical fix, so it is always done, even if AI improvement is disabled.
void UpdateStrategyWithUnreferencedExistingUnits(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, long int DAT_ID, TAIUnitClass elemType) {
	if ((elemType != TAIUnitClass::AIUCNone) && (elemType != TAIUnitClass::AIUCBuilding) && (elemType != TAIUnitClass::AIUCLivingUnit)) { return; } // only relevant for creatable
	if (!buildAI) { return; }

	ROR_STRUCTURES_10C::STRUCT_AI *mainAI = buildAI->mainAI;
	if (!mainAI) { return; }
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = mainAI->ptrStructPlayer;
	if (!player) { return; }
	ROR_STRUCTURES_10C::STRUCT_TAC_AI *tacAI = &mainAI->structTacAI;
	ROR_STRUCTURES_10C::STRUCT_PER_TYPE_UNIT_LIST_LINK *creatableListLink = player->ptrCreatableUnitsListLink;
	assert(creatableListLink != NULL);
	if (creatableListLink == NULL) { return; }
	ROR_STRUCTURES_10C::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *creatableListElem = creatableListLink->lastListElement;
	std::vector<ROR_STRUCTURES_10C::STRUCT_UNIT*> notInStrategyUnitsList;
	int indexInList = 0;
	// Fill availableUnitsList with player's (creatable) units list that match DAT_ID
	while ((creatableListElem != NULL) && (indexInList < creatableListLink->listElemCount)) {
		ROR_STRUCTURES_10C::STRUCT_UNIT *currentUnit = creatableListElem->unit;
		if (currentUnit && (currentUnit->unitInstanceId >= 0) && (currentUnit->ptrStructDefUnit) && // Exclude temporary units (smoke, dead units) with negative id
			(currentUnit->ptrStructDefUnit->unitType >= AOE_CONST_FUNC::GUT_LIVING_UNIT) && // living + building (+trees, but non-gaia players don't have any)
			((currentUnit->ptrStructDefUnit->DAT_ID1 == DAT_ID) || (DAT_ID == -1))) {
			ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = currentUnit->ptrStructDefUnit;
			notInStrategyUnitsList.push_back(currentUnit);
		}
		creatableListElem = creatableListElem->previousElement;
		indexInList++;
	}

	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeStratElem = &mainAI->structBuildAI.fakeFirstStrategyElement;
	assert(fakeStratElem != NULL); // this should/could not happen !
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentElem = fakeStratElem;
	if (fakeStratElem) { currentElem = fakeStratElem->next; }

	// Loop on strategy elements. Remove from availableUnitsList the units that are already "used" in a strategy element
	// (only keep "unreferenced" ones)
	while ((currentElem != NULL) && (currentElem != fakeStratElem)) {
		// Performance: We only take care of strategy elements that match provided criteria
		if (((currentElem->elementType == elemType) || (elemType == AIUCNone)) &&
			((currentElem->unitDAT_ID == DAT_ID) || (DAT_ID == -1))) {
			std::vector<ROR_STRUCTURES_10C::STRUCT_UNIT*>::iterator it = notInStrategyUnitsList.begin();
			while (it != notInStrategyUnitsList.end()) {
				if (currentElem->unitInstanceId == (*it)->unitInstanceId) {
					it = notInStrategyUnitsList.erase(it); // Remove from list the units that are already "used" by a strategy element
				} else {
					it++;
				}
			}
		}
		currentElem = currentElem->next;
	}

	// New loop on strategy elements to "use" free units and mark "not done" elements to "done" with free units' ID
	if (fakeStratElem) { currentElem = fakeStratElem->next; }
	while ((currentElem != NULL) && (currentElem != fakeStratElem) && (!notInStrategyUnitsList.empty())) {
		// Performance: We only take care of strategy elements that match provided criteria
		if (((currentElem->elementType == elemType) || (elemType == AIUCNone)) &&
			((currentElem->unitDAT_ID == DAT_ID) || (DAT_ID == -1))) {
			if ((currentElem->aliveCount == 0) && (currentElem->inProgressCount == 0) && (currentElem->unitInstanceId <= 0) &&
				((currentElem->retrains == -1) || (currentElem->totalCount < currentElem->retrains)) &&
				((currentElem->elementType == AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding) || (currentElem->elementType == AOE_CONST_FUNC::TAIUnitClass::AIUCLivingUnit))
				) {
				// This unit is not being created, not alive, and retrains condition allows creating some => use an available existing unit if we have one
				// Search for a matching available unit
				std::vector<ROR_STRUCTURES_10C::STRUCT_UNIT*>::iterator it = notInStrategyUnitsList.begin();
				bool found = false;
				while (!found && (it != notInStrategyUnitsList.end())) {
					ROR_STRUCTURES_10C::STRUCT_UNIT* currentAvailableUnit = *it;
					assert(currentAvailableUnit != NULL);
					assert(currentAvailableUnit->ptrStructDefUnit != NULL);
					if ((currentAvailableUnit->ptrStructDefUnit->DAT_ID1 == currentElem->unitDAT_ID) ||
						//(currentAvailableUnit->ptrStructDefUnit->DAT_ID2 == currentElem->unitDAT_ID) ||
						// Previous line does not work because sc editor use "DATID2" value for both DATID1&2 when placing 
						// The wrong unit in a scenario (house1 instead of house, etc)
						((currentElem->unitDAT_ID == AOE_CONST_FUNC::CST_UNITID_VILLAGER) && IsVillager(currentAvailableUnit->ptrStructDefUnit->DAT_ID1))
						) {
						found = true;
						currentElem->unitInstanceId = currentAvailableUnit->unitInstanceId; // Associate strategy element to existing unit
						if ((currentAvailableUnit->unitType == AOE_CONST_FUNC::GUT_BUILDING) && (currentAvailableUnit->unitStatus < 2)) {
							// Special case: in-progress buildings (not fully built)
							currentElem->aliveCount = 0;
							currentElem->inProgressCount = 1;
						} else {
							// Standard case
							currentElem->totalCount++;
							currentElem->aliveCount = 1;
							currentElem->inProgressCount = 0;
						}
						// In-progress living units (being trained) : will be ignored, but the impact is less important than being-built buildings.
						// AI will train them again. Units die more easily/quickly than buildings ;) TO DO later if I have time to.
						it = notInStrategyUnitsList.erase(it); // Now this unit is referenced in strategy, remove it from list.
					} else {
						it++;
					}
				}
			}
		}
		currentElem = currentElem->next; // Loop: next strategy element
	}
}


// Tries to add an existing unit to player's strategy (cf buildAI pointer)
// Returns true if a matching (not in progress/done) strategy element has been found and updated (set to done + associate with unitId)
// Note: unit may NOT belong to same player because it is used in successful conversion code, before unit is actually transfered to new player.
bool UpdateStrategyWithExistingUnit(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, ROR_STRUCTURES_10C::STRUCT_UNIT *unit) {
	if ((buildAI == NULL) || (unit == NULL)) { return false; }
	assert(buildAI->IsCheckSumValid());
	assert(unit->IsCheckSumValid());
	if (!(buildAI->IsCheckSumValid()) || (!unit->IsCheckSumValid())) { return false; }

	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef = unit->ptrStructDefUnit;
	if (!unitDef) { return false; }
	short int baseDATID = unitDef->DAT_ID1;
	if (IsVillager(baseDATID)) { baseDATID = CST_UNITID_MAN; } // We don't want forager or repairmen here, just VILLAGERS to match strategy elements !
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstStratElem = &buildAI->fakeFirstStrategyElement;
	if (fakeFirstStratElem == NULL) { return false; } // Just a security
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeFirstStratElem->next;

	while (currentStratElem && (currentStratElem != fakeFirstStratElem)) {
		TAIUnitClass unitStrategyClass = GetUnitStrategyElemClass(unitDef);
		if ((currentStratElem->unitDAT_ID == baseDATID) && (currentStratElem->inProgressCount == 0) &&
			(currentStratElem->aliveCount == 0) && (currentStratElem->elementType == unitStrategyClass) &&
			((currentStratElem->retrains == -1) || (currentStratElem->totalCount < currentStratElem->retrains))) {
			currentStratElem->aliveCount = 1;
			currentStratElem->inProgressCount = 0; // Not required, just some kind of double-check.
			currentStratElem->unitInstanceId = unit->unitInstanceId;
			currentStratElem->totalCount++; // There is a real question here, do we consider the unit has been trained/built ?
			return true;
		}
		currentStratElem = currentStratElem->next;
	}
	return false;
}


// Calls RoR original code to add an element in strategy (AFTER the element in positionToInsert).
// Consecutive inserts after a same element are reversed (of course) !
// If you call this to add a research (type 1 or 4), you need to update the unit name afterwards because ROR method is bugged.
// DO NOT use it to insert researches, please use AddUnitInStrategy_before
bool AddUnitInStrategy(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, long int positionToInsert, long int retrains, long int actor, long int unitType, long int unitDATID, ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	assert(buildAI && buildAI->IsCheckSumValid());
	assert(player && player->IsCheckSumValid());
	if (!buildAI || !buildAI->IsCheckSumValid() || !player || !player->IsCheckSumValid()) { return false; }
	long int result;
	_asm {
		MOV ECX, buildAI // Required to call method
			PUSH positionToInsert
			PUSH retrains
			PUSH actor
			PUSH unitType
			PUSH DWORD PTR 1 // nb to create
			PUSH unitDATID
			push player
			MOV EAX, 0x00408D90
			CALL EAX
			MOV result, EAX
	}
	// Warning: update name afterward for researches because it adds a stupid unit name (even if unitDATID is a researchId)
	return (result != 0);
}


// Custom method to insert a line in strategy. This tries to be a bit faster and avoids "unit" Name bug for researches
// Inserts before 'nextElem' strategy element. Consecutive inserts order is preserved if we insert before the same element.
// If you want to insert researches in strategy, use this overload.
bool AddUnitInStrategy_before(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *nextElem, long int retrains, long int actor,
	long int unitType, long int unitDATID, ROR_STRUCTURES_10C::STRUCT_PLAYER *player, const char *name) {
	assert(buildAI && buildAI->IsCheckSumValid());
	assert(player && player->IsCheckSumValid());
	if (!buildAI || !buildAI->IsCheckSumValid() || !player || !player->IsCheckSumValid()) { return false; }
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elem = (ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *)AOEAlloc(0xB0);
	if (!elem) {
		return false;
	}
	long int counter = buildAI->strategyElementSequence;

	_asm {
		PUSH 0 // arg14 - for +9C
		PUSH DWORD PTR -1 // arg13 - for +98
		PUSH DWORD PTR -1 // arg12 - for +94
		PUSH DWORD PTR -1 // arg11 - for +90
		PUSH DWORD PTR 0 // arg10 - for +64
		PUSH 0x40400000 // arg9 related to size radius
		PUSH 0x40400000 // arg8 related to size radius
		PUSH name // arg7
		PUSH actor // arg6 = actor
		PUSH 0 // arg5 - for +7C
		PUSH DWORD PTR 1 // arg4 - for +78 - always 1 ?
		PUSH counter // arg3 = counter
		PUSH unitDATID // arg2
		PUSH unitType // arg1
		MOV ECX, elem
		MOV EAX, 0x407980
		CALL EAX
		// returns EAX= ptr new strat elem
	}
	buildAI->strategyElementSequence++; // To increment after having been used.
	// Other inits
	elem->retrains = retrains;
	// Actually insert new element into chained list.
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *previousElem = nextElem->previous;
	previousElem->next = elem;
	nextElem->previous = elem;
	elem->previous = previousElem;
	elem->next = nextElem;
	return true;
}


// Use it to move a strategy element to any other location in strategy list.
// afterThisElem if the element after which we move elemToMove
bool MoveStrategyElement_after(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elemToMove,
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *afterThisElem) {
	// Some consistency checks
	if (!buildAI || !elemToMove || !afterThisElem) { return false; }
	assert(buildAI->IsCheckSumValid());
	assert(elemToMove->IsCheckSumValid());
	assert(afterThisElem->IsCheckSumValid());
	if (!buildAI->IsCheckSumValid() || !elemToMove->IsCheckSumValid() || !afterThisElem->IsCheckSumValid()) { return false; }
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeElem = &buildAI->fakeFirstStrategyElement;
	if (elemToMove == fakeElem) { return false; } // don't move fake empty element !
	if (elemToMove == afterThisElem) { return false; } // don't move after himself !

	// Get neighbor elements
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *oldPrevious = elemToMove->previous;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *oldNext = elemToMove->next;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *newPrevious = afterThisElem;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *newNext = afterThisElem->next;
	assert(oldPrevious != NULL);
	assert(oldNext != NULL);
	assert(newPrevious != NULL);
	assert(newNext != NULL);
	if (!oldPrevious || !oldNext || !newPrevious || !newNext) { return false; }

	// Detach elemToMove from the list
	oldPrevious->next = oldNext;
	oldNext->previous = oldPrevious;

	// Now attach to new position (neighbor elements)
	newPrevious->next = elemToMove;
	newNext->previous = elemToMove;
	// Now attach to new position (elemToMove's links)
	elemToMove->next = newNext;
	elemToMove->previous = newPrevious;
	return true;
}


// If elem_shouldBeAfter is NOT after elem_shouldBeBefore, then move elem_shouldBeAfter just after elem_shouldBeBefore.
// Note: only elem_shouldBeAfter (3rd argument) can be MOVED by this method.
// Use this to fix dependency issues, a correct call example is elem_shouldBeBefore=barracks, elem_shouldBeAfter=clubman
// Return true if an element has been moved, false otherwise
bool MoveStrategyElement_after_ifWrongOrder(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI,
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elem_shouldBeBefore,
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elem_shouldBeAfter) {
	// Some consistency checks
	if (!buildAI || !elem_shouldBeBefore || !elem_shouldBeAfter) { return false; }
	assert(buildAI->IsCheckSumValid());
	assert(elem_shouldBeBefore->IsCheckSumValid());
	assert(elem_shouldBeAfter->IsCheckSumValid());
	if (!buildAI->IsCheckSumValid() || !elem_shouldBeBefore->IsCheckSumValid() || !elem_shouldBeAfter->IsCheckSumValid()) { return false; }
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeElem = &buildAI->fakeFirstStrategyElement;
	if (elem_shouldBeBefore == fakeElem) { return false; } // fake empty element is irrelevant in this method
	if (elem_shouldBeAfter == fakeElem) { return false; } // fake empty element is irrelevant in this method
	if (elem_shouldBeBefore == elem_shouldBeAfter) { return false; } // don't move after himself !

	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentElem = elem_shouldBeAfter->next;
	// Loop from elem_shouldBeAfter to strategy end. If we find elem_shouldBeBefore on our way, then it means order is incorrect => move elem_shouldBeAfter after elem_shouldBeBefore
	while ((currentElem) && (currentElem != fakeElem)) {
		if (currentElem == elem_shouldBeBefore) { // What ? elem1 is after elem2 ? Fix that !
			MoveStrategyElement_after(buildAI, elem_shouldBeAfter /* to move*/, elem_shouldBeBefore /*destination(after...)*/);
			return true; // We're done, no need to finish the loop
		}
		currentElem = currentElem->next;
	}
	return false;
}


// An overload that applies on the first matching element (for both "shouldBeBefore"/"shouldBeAfter"), starting from strategy beginning.
// Example: if used with house and man (there are several of each in strategy), first house will be used as shouldBeBefore, first man as shouldBeAfter
// You can use TAIUnitClass::AIUCNone as a joker that represents (only) BUILDING OR LIVING unit
// Return true if an element has been moved, false otherwise
bool MoveStrategyElement_after_ifWrongOrder(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI,
	long int DATID_shouldBeBefore,
	TAIUnitClass elemType_shouldBeBefore,
	long int DATID_shouldBeAfter,
	TAIUnitClass elemType_shouldBeAfter) {
	if (!buildAI) { return false; }
	assert(buildAI->IsCheckSumValid());
	if (!buildAI->IsCheckSumValid()) { return false; }
	// No jokers for DATID
	if ((DATID_shouldBeBefore < 0) || (DATID_shouldBeAfter < 0)) { return false; }

	// Search for matching elements
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeElem = &buildAI->fakeFirstStrategyElement;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentElem = fakeElem->next;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elem_shouldBeBefore = NULL;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elem_shouldBeAfter = NULL;
	while ((currentElem) && (currentElem != fakeElem) &&
		((elem_shouldBeBefore == NULL) || (elem_shouldBeAfter == NULL)) // Exit loop when both have been found
		) {
		// Useful for joker filter (joker = OK for buildings and living units only)
		bool currentElemIsCreatable = (currentElem->elementType == TAIUnitClass::AIUCBuilding) || (currentElem->elementType == TAIUnitClass::AIUCLivingUnit);
		if ((currentElem->unitDAT_ID == DATID_shouldBeBefore) && (elem_shouldBeBefore == NULL) &&
			((currentElem->elementType == elemType_shouldBeBefore) || ((elemType_shouldBeBefore == TAIUnitClass::AIUCNone) && (currentElemIsCreatable)))
			) {
			elem_shouldBeBefore = currentElem;
		}
		if ((currentElem->unitDAT_ID == DATID_shouldBeAfter) && (elem_shouldBeAfter == NULL) &&
			((currentElem->elementType == elemType_shouldBeAfter) || ((elemType_shouldBeAfter == TAIUnitClass::AIUCNone) && (currentElemIsCreatable)))
			) {
			elem_shouldBeAfter = currentElem;
		}
		currentElem = currentElem->next;
	}

	if ((elem_shouldBeBefore != NULL) && (elem_shouldBeAfter != NULL)) {
		// Do the job if we found both strategy elements
		return MoveStrategyElement_after_ifWrongOrder(buildAI, elem_shouldBeBefore, elem_shouldBeAfter);
	}

	return false;
}


// Add units to fit maximum population (especially if >50)
void AdaptStrategyToMaxPopulation(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	if (!player || !player->ptrAIStruct) { return; }
	assert(player->GetAIStruct() != NULL);
	ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI = &player->GetAIStruct()->structBuildAI;
	if (!buildAI) { return; }
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstStratElem = &buildAI->fakeFirstStrategyElement;
	assert(fakeFirstStratElem != NULL);
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elemToInsertBefore = fakeFirstStratElem;
	if (fakeFirstStratElem->previous == NULL) { return; }
	// If strategy last element is a wonder, insert before it.
	if ((fakeFirstStratElem->previous->unitDAT_ID == CST_UNITID_WONDER) && (fakeFirstStratElem->previous->elementType == AIUCBuilding)) {
		elemToInsertBefore = fakeFirstStratElem->previous;
	}
	if (fakeFirstStratElem == NULL) { return; } // Just a security
	int currentPopulation = 0; // Does not count limited-retrains units
	int strongUnitCount = 0; // Number of different military unit (DATID) we found in strategy. Only keep the strongest.
#define STRONG_UNIT_TYPE_COUNT 4
	int strongerUnits[STRONG_UNIT_TYPE_COUNT];
	for (int i = 0; i < STRONG_UNIT_TYPE_COUNT; i++) {
		strongerUnits[i] = -1;
	}
	int maxPopulation = (int) player->GetResourceValue(CST_RES_ORDER_POULATION_LIMIT);

	// MIN_ENDING_TOWER_COUNT_TO_FORCE_INSERT = number of consecutive "ending" towers in strategy above which we force inserting unit BEFORE those towers.
#define MIN_ENDING_TOWER_COUNT_TO_FORCE_INSERT 4
	// For strategies that have a lot of towers in end of strategy (deathmatch...), keep the first one of the ending tower series.
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeFirstStratElem->previous;
	bool noTowerFoundYet = true;
	long int consecutiveTowerCount = 0;
	bool canExitLoop = false;
	while (currentStratElem && (currentStratElem != fakeFirstStratElem) && !canExitLoop) {
		if (IsTower((unsigned short int)currentStratElem->unitDAT_ID)) {
			noTowerFoundYet = false;
			consecutiveTowerCount++;
		} else {
			if (!noTowerFoundYet) {
				// We already found towers, but this element isn't : series of ending towers is finished.
				canExitLoop = true;
			}
		}
		if (!canExitLoop) { // do not change current element when exiting loop.
			currentStratElem = currentStratElem->previous;
		}
	}
	if (consecutiveTowerCount > MIN_ENDING_TOWER_COUNT_TO_FORCE_INSERT) {
		assert(currentStratElem != NULL);
		if (currentStratElem == NULL) { return; } // ERROR !
		elemToInsertBefore = currentStratElem->next;
		// Leave some towers before our added units
		for (int i = 0; i < MIN_ENDING_TOWER_COUNT_TO_FORCE_INSERT - 1; i++) {
			elemToInsertBefore = elemToInsertBefore->next;
		}
	}

	// Go back some strategy elements to get a position where add an "anticipated" house: 
	// This helps strategy not being stuck (with some extra population housage).
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elemToMoveFirstAddedHouse = elemToInsertBefore;
	int counterToMoveFirstAddedHouse = 0;
	while (elemToMoveFirstAddedHouse && elemToMoveFirstAddedHouse->previous && (counterToMoveFirstAddedHouse < 4) &&
		(elemToMoveFirstAddedHouse->previous != fakeFirstStratElem)) {
		counterToMoveFirstAddedHouse++;
		elemToMoveFirstAddedHouse = elemToMoveFirstAddedHouse->previous;
	}

	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT **defUnitTable = player->ptrStructDefUnitTable;

	// Do only 1 (reverse) loop on strategy and collect all necessary information.
	// We suppose that a reverse loop will find first the strongest (military) units !
	currentStratElem = fakeFirstStratElem->previous;
	while (currentStratElem && currentStratElem != fakeFirstStratElem) {
		if ((currentStratElem->elementType == AIUCLivingUnit) && (currentStratElem->retrains == -1)) {
			currentPopulation++;
			if ((currentStratElem->unitDAT_ID != CST_UNITID_MAN) &&
				(currentStratElem->unitDAT_ID != CST_UNITID_MAN2) &&
				(currentStratElem->unitDAT_ID != CST_UNITID_FISHING_BOAT) &&
				(currentStratElem->unitDAT_ID != CST_UNITID_TRADE_BOAT) &&
				(currentStratElem->unitDAT_ID != CST_UNITID_TRANSPORT_BOAT)
				) {
				// We have found a military unit
				int matchingStrongUnit = -1;
				short int index = 0;
				// Search if we already know this DATID in our table
				while ((matchingStrongUnit == -1) && (index < STRONG_UNIT_TYPE_COUNT)) {
					assert(index < STRONG_UNIT_TYPE_COUNT);
					if (strongerUnits[index] == currentStratElem->unitDAT_ID) {
						matchingStrongUnit = index;
					}
					index++;
				}
				// Add if not already collected...
				// And if table is not already full (this will ignore last met unit types, that's why the loop is reverse).
				if ((matchingStrongUnit == -1) && (strongUnitCount < STRONG_UNIT_TYPE_COUNT)) {
					assert(strongUnitCount < STRONG_UNIT_TYPE_COUNT);
					strongerUnits[strongUnitCount++] = currentStratElem->unitDAT_ID;
				}
			}
		}
		currentStratElem = currentStratElem->previous;
	}

	int populationToAdd = maxPopulation - currentPopulation;
	if (populationToAdd <= 0) { return; }
	assert(elemToInsertBefore != NULL);
	std::string msg = "Adapt p#";
	msg += std::to_string(player->playerId);
	msg += " to max population: insertion start point uID=";
	msg += std::to_string(elemToInsertBefore->previous ? elemToInsertBefore->previous->counter : elemToInsertBefore->counter);
	msg += ". Estimated number of units to add: ";
	msg += std::to_string(populationToAdd);
	msg += " units.";
	traceMessageHandler.WriteMessageNoNotification(msg);

	if (GetCustomRorMaxPopulationBeginStratElem(buildAI) == NULL) {
		// Add a fake element so we can find where is starting point of "added elements" in further calls / other methods.
		AddUnitInStrategy_before(buildAI, elemToInsertBefore, -1, -1, AIUCTech,
			39 /*storage pit, always (immediately) researched = no impact*/, player, CST_CUSTOMROR_FAKE_STRATELEM_MAXPOP_BEGIN);
		elemToInsertBefore->previous->aliveCount = 1; // Make sure it does not impact AI.
		elemToInsertBefore->previous->retrains = 1; // Make sure it does not impact AI.
	}

	if (strongUnitCount <= 0) { return; }
	if (strongUnitCount == 1) {
		for (int i = 0; i < populationToAdd; i++) {
			traceMessageHandler.WriteMessageNoNotification("There is only 1 unit type to add in strategy.");
			short int DAT_ID = strongerUnits[0];
			char buf[30];
			ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *defUnit = defUnitTable[DAT_ID];
			//if (!defUnit) { return; }
			assert(defUnit != NULL);
			strcpy_s(buf, GetUnitName(DAT_ID));
			AddUnitInStrategy_before(buildAI, elemToInsertBefore, -1, defUnit->trainLocation,
				AIUCLivingUnit, DAT_ID, player, buf);
			return;
		}
	}
	// What should we actually add in strategy ?
	assert(STRONG_UNIT_TYPE_COUNT > 2);
	// secondary unit types: "disable" if very weak / non-upgradable units
	for (int index = strongUnitCount - 1; index >= 3; index--) {
		if (GetUnitWeight(strongerUnits[index]) < 100) {
			strongerUnits[index] = -1;
			strongUnitCount = index;
		}
	}
	// Note: we keep at least Top 2 unit types, strongUnitCount >= 2 here.

	// Calculate numbers of each unit type to add. Iterate an create a little more than 1/x (first units in array will be created more than next ones)
	// Adding 1 or 2 extra units in strategy is not a problem.
	int thisUnitCountToAdd[STRONG_UNIT_TYPE_COUNT];
	for (int index = 0; index < STRONG_UNIT_TYPE_COUNT; index++) {
		assert(index < STRONG_UNIT_TYPE_COUNT);
		thisUnitCountToAdd[index] = 0;
	}
	int totalCountIWillAdd = 0;
	for (int index = 0; index < strongUnitCount; index++) {
		assert(index < STRONG_UNIT_TYPE_COUNT);
		thisUnitCountToAdd[index] = ((populationToAdd - totalCountIWillAdd) * ((100 / (strongUnitCount - index)) + 10)) / 100 + 1;
		totalCountIWillAdd += thisUnitCountToAdd[index];
	}

	// alternately add all units
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *lastAddedHouse = NULL;
	int currentIndex = 0;
	int countToNextHouse = 0;
	while (totalCountIWillAdd > 0) {
		int loopCount = 0;
		assert(currentIndex < STRONG_UNIT_TYPE_COUNT);
		while ((thisUnitCountToAdd[currentIndex] <= 0) && (loopCount < strongUnitCount)) {
			currentIndex++;
			if (currentIndex >= strongUnitCount) { currentIndex = 0; }
			loopCount++;
		}
		if (loopCount == strongUnitCount) {
			totalCountIWillAdd = 0; // Security against infinite loops
		} else {
			// Add a house every 4 units
			if (countToNextHouse <= 0) {
				AddUnitInStrategy_before(buildAI, elemToInsertBefore, -1, -1,
					AIUCBuilding, CST_UNITID_HOUSE, player, "House_customROR");
				countToNextHouse = 4;
				lastAddedHouse = elemToInsertBefore->previous;
			}
			// Add unit
			short int DAT_ID = strongerUnits[currentIndex];
			char buf[30];
			ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *defUnit = defUnitTable[DAT_ID];
			//if (!defUnit) { return; }
			assert(defUnit != NULL);
			strcpy_s(buf, GetUnitName(DAT_ID));
			AddUnitInStrategy_before(buildAI, elemToInsertBefore, -1, defUnit->trainLocation,
				AIUCLivingUnit, DAT_ID, player, buf);

			assert(currentIndex < STRONG_UNIT_TYPE_COUNT);
			thisUnitCountToAdd[currentIndex]--;
			totalCountIWillAdd--;
			countToNextHouse--;
			currentIndex++;
			if (currentIndex >= strongUnitCount) { currentIndex = 0; }
		}
	}

	MoveStrategyElement_after(buildAI, lastAddedHouse, elemToMoveFirstAddedHouse->previous);

	// TO DO: add buildings to train...
	// TO DO keep room for automatically added boats ? (when current pop < 50 in initial strat)
	// Add some villies ?
}


// Returns true if current strategy status allows triggering a wonder construction
bool IsStrategyCompleteForWonder(ROR_STRUCTURES_10C::STRUCT_AI *ai) {
	if (!ai || !ai->IsCheckSumValid()) { return false; }
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &ai->structBuildAI.fakeFirstStrategyElement;
	assert(fakeFirstElem->IsCheckSumValid());
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = ai->ptrStructPlayer;
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return false; }

	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentElem = fakeFirstElem->next;
	long int missingVillagers = 0;
	long int populationHeadroom = (long int)player->GetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_POPULATION_HEADROOM);
	long int remainingPopToMaximum = (long int)(player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_POULATION_LIMIT) - player->GetResourceValue(RESOURCE_TYPES::CST_RES_ORDER_CURRENT_POPULATION));
	if (remainingPopToMaximum < populationHeadroom) {
		populationHeadroom = remainingPopToMaximum; // reached or almost reached maximum population
	}

	// Loop on strategy elements to see if there are missing "prerequisites" (in fact, we want almost all strategy elements to be done to build a wonder)
	while ((currentElem != fakeFirstElem) && (currentElem != NULL) && 
		(!IsCustomRorPopulationBeginStratElem(currentElem))) { // Ignore elements that are located AFTER customROR-added elements (to fit max population)
		if (currentElem->unitDAT_ID == CST_UNITID_WONDER) {
			return true; // reached wonder element, ignore next ones (if any)
		}

		bool ignoreThisElement = (currentElem->retrains >= 0) || // Limited retrains: ignore
			(currentElem->elementType == AIUCStrategyCmd) ||
			(currentElem->elementType == AIUCEnableAttack) ||
			(currentElem->elementType == AIUCLoadNewStrategy) ||
			(currentElem->elementType == AIUCNone);

		if ((currentElem->aliveCount == 0) && !ignoreThisElement) {
			if (currentElem->elementType == AIUCCritical) {
				return false; // A critical step has not been fulfilled
			}
			// Rules for researches
			if (currentElem->elementType == AIUCTech) {
				// A research has not been done.
				// Just try to see if it's not available in tech tree (then ignore it) = this may happen when strategy is not adapted to civ - this case is possible
				if ((currentElem->inProgressCount == 0) && !IsResearchRelevantForStrategy(player, (short)currentElem->unitDAT_ID)) {
					std::string s = "Strategy error: research is not in tech tree: p#";
					s += to_string(player->playerId);
					s += ", research=";
					s += currentElem->unitName;
					traceMessageHandler.WriteMessage(s); // TO DO : remove notification
				} else {
					long int actorDATID = -1;
					ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *actor = (ROR_STRUCTURES_10C::STRUCT_UNIT_BASE *)GetUnitStruct(currentElem->actor);
					if (actor) {
						ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *actorDef = actor->GetUnitDefinition();
						if (actorDef && actorDef->IsCheckSumValidForAUnitClass()) {
							actorDATID = actorDef->DAT_ID1;
						}
					}
					// Exception: Do not block for dock techs (in case we have a no water map?)
					// +auto-inserted techs ?
					if (actorDATID != CST_UNITID_DOCK) {
						// This research (from strategy element) CAN be done in tech tree but is not researched yet.
						return false;
					}
				}
			}

			// Rules for buildings
			if (currentElem->elementType == AIUCBuilding) {
				// Explicitely require ALL houses / TC to be fully built.
				if ((currentElem->unitDAT_ID == CST_UNITID_HOUSE) || (currentElem->unitDAT_ID == CST_UNITID_FORUM)) {
					return false;
				}

				// We allow some "useless" buildings NOT to be built: farms, gov siege, market
				bool isUnnecessaryBld = (currentElem->unitDAT_ID != CST_UNITID_FARM) ||
					// SP / granary are dynamically added: there might be several of them, but we don't need them to build a wonder
					(currentElem->unitDAT_ID != CST_UNITID_GRANARY) ||
					(currentElem->unitDAT_ID != CST_UNITID_STORAGE_PIT) ||
					(currentElem->unitDAT_ID != CST_UNITID_DOCK) || // Can be added dynamically, don't take the risk to block wonder for this.
					(currentElem->unitDAT_ID != CST_UNITID_GOVERNMENT_SIEGE) ||
					(currentElem->unitDAT_ID != CST_UNITID_MARKET);
				if (!isUnnecessaryBld) {
					// Add other criteria ?
					return false; // A "necessary" building is missing
				}
			}

			// Rules for living units - only if there is room for new units.
			// If no more headroom because of missing houses, "building" rules will ensure we return false
			// If no more headroom because we reached population max, don't block wonder construction for this.
			if ((currentElem->elementType == AIUCLivingUnit) && (populationHeadroom > 0)) {
				bool isVillager = IsVillager_includingShips((unsigned short)currentElem->unitDAT_ID);
				if (isVillager) {
					missingVillagers++;
					if (missingVillagers > 4) { // we tolerate some villagers to be spawned afterwards, but not too many
						return false;
					}
				}
				// Military unit: tolerate if in progress, but not if it has not been started yet.
				if (!isVillager && (currentElem->inProgressCount == 0)) {
					return false;
				}
			}
		}

		currentElem = currentElem->next;
	}

	// Here : we could test some "extra" bonus researches (if some useful military upgrade is available, or some others like coinage...)
	// and add them before building a wonder (return false if something was added) ??

	return true; // Success: let's build a wonder !
}


// Adds in strategy all available researches that improve provided unit
// Returns the number of inserted researches in strategy
int AddResearchesInStrategyForUnit(ROR_STRUCTURES_10C::STRUCT_AI *ai, short int unitDefId) {
	if (!ai || !ai->IsCheckSumValid()) { return 0; }
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = ai->ptrStructPlayer;
	if (!player || !player->IsCheckSumValid()) { return 0; }
	if (!player->ptrResearchesStruct || !player->ptrResearchesStruct->ptrResearchDefInfo ||
		!player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray) {
		return 0;
	}
	if ((unitDefId < 0) || (unitDefId >= player->structDefUnitArraySize)) { return 0;}
	std::vector<short int> researchesForUnit = FindResearchesThatAffectUnit(player, unitDefId, true);
	std::vector<short int> allResearchesForUnit = GetValidOrderedResearchesListWithDependencies(player, researchesForUnit);
	// Important note: for "shadow" researches with optional requirements (buildings for ages, etc), requirements are not analyzed yet.
	// We'll have to manage them ourselves.
	/*std::set<short int> researchesWithoutLocation;
	for each (short int researchId in allResearchesForUnit)
	{
		ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef = &player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[researchId];
		if (resDef->researchLocation < 0) {
			researchesWithoutLocation.insert(researchId); // Shadow researches (some may correspond to required buildings, take care not to ignore them)
		}
	}
	for each (short int researchId in researchesWithoutLocation)
	{
		auto it = remove_if(allResearchesForUnit.begin(), allResearchesForUnit.end(), [researchId](short int i) {return i == researchId; });
		allResearchesForUnit.erase(it, allResearchesForUnit.end());
	}*/
	
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstElement = &ai->structBuildAI.fakeFirstStrategyElement;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentElem = fakeFirstElement->previous;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elemToInsert = NULL;
	// Search for wonder, if any
	while (currentElem && (currentElem != fakeFirstElement) && !elemToInsert) {
		if ((currentElem->unitDAT_ID == CST_UNITID_WONDER) && (currentElem->elementType == AOE_CONST_FUNC::AIUCBuilding)) {
			elemToInsert = currentElem;
		}
		currentElem = currentElem->previous; // reverse loop because wonder is generally at the end !
	}
	if (!elemToInsert) {
		elemToInsert = fakeFirstElement;
	}
	// elemToInsert = position to insert at stategy end OR just before wonder, if any.

	int addedItems = 0;
	// allResearchesForUnit is ordered by dependency, so we're adding the requirements first and strategy order is OK.
	// However inserted elements are not optimized for development speed and there is no priority other than dependencies.
	for each (short int researchId in allResearchesForUnit)
	{
		if ((researchId >= 0) && (researchId < player->ptrResearchesStruct->ptrResearchDefInfo->researchCount)) {
			ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef = &player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray[researchId];

			if (ResearchHasOptionalRequirements(resDef)) {
				// Handle optional requirements. This DOES have importance. eg. in DM, yamato build a temple just to go iron !
				// TODO
			} else {
				char nameBuffer[0x50]; // stratelem.name size is 0x40
				char namePrefix[] = "CustomROR_";
				strcpy_s(nameBuffer, namePrefix);
				if ((resDef->researchLocation == -1) || (resDef->buttonId == 0)) {
					// Shadow research (automatically researched): do not add in strategy
					// However, it may correspond to a required building (temple for fanaticism..)
					ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDefBuilding = FindBuildingDefThatEnablesResearch(player, researchId);
					if (unitDefBuilding && unitDefBuilding->IsCheckSumValid()) {
						// Make sure this building is built in strategy
						if (FindElementPosInStrategy(player, TAIUnitClass::AIUCBuilding, unitDefBuilding->DAT_ID1) == -1) {
							strcpy_s(nameBuffer + sizeof(namePrefix) - 1, sizeof(nameBuffer) - sizeof(namePrefix) + 1, unitDefBuilding->ptrUnitName);
							addedItems++;
							AddUnitInStrategy_before(&ai->structBuildAI, elemToInsert, -1, -1 /*villager*/,
								TAIUnitClass::AIUCBuilding, unitDefBuilding->DAT_ID1, player, nameBuffer);
						}
					}
				} else {
					// Standard research
					ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDefBuilding = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *)player->GetUnitDefBase(resDef->researchLocation);
					if (unitDefBuilding && unitDefBuilding->IsCheckSumValid()) {
						// Add "action" building if there is none in strategy
						if (FindElementPosInStrategy(player, TAIUnitClass::AIUCBuilding, unitDefBuilding->DAT_ID1) == -1) {
							strcpy_s(nameBuffer + sizeof(namePrefix) - 1, sizeof(nameBuffer) - sizeof(namePrefix) + 1, unitDefBuilding->ptrUnitName);
							addedItems++;
							AddUnitInStrategy_before(&ai->structBuildAI, elemToInsert, -1, -1 /*villager*/,
								TAIUnitClass::AIUCBuilding, unitDefBuilding->DAT_ID1, player, nameBuffer);
						}
					}

					if (FindElementPosInStrategy(player, AOE_CONST_FUNC::TAIUnitClass::AIUCTech, researchId) == -1) {
						strcpy_s(nameBuffer + sizeof(namePrefix) - 1, sizeof(nameBuffer) - sizeof(namePrefix) + 1, resDef->researchName);
						addedItems++;
						AddUnitInStrategy_before(&ai->structBuildAI, elemToInsert, -1, resDef->researchLocation,
							TAIUnitClass::AIUCTech, researchId, player, nameBuffer);
					}
				}
			}
		}
	}
	return addedItems;
}


// Find a strategy element for a specific actor building (instance)id. (strategy element must be in progress)
// This is only possible thanks to custom treatments that set unitInstanceId to actor unitId while training/researching is in progress.
// Unicity is not guaranteed (first found is returned), however it is not supposed to happen much.
// actorUnitId must be >=0
// Returns NULL if not found.
ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *GetStrategyElementForActorBuilding(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, long int actorUnitId) {
	if (!player || !player->IsCheckSumValid() || (actorUnitId < 0)) {
		return NULL;
	}
	ROR_STRUCTURES_10C::STRUCT_AI *ai = player->ptrAIStruct;
	if (!ai || !ai->IsCheckSumValid()) {
		return NULL;
	}
	ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI = &ai->structBuildAI;
	if (!buildAI->IsCheckSumValid()) {
		return NULL;
	}
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &ai->structBuildAI.fakeFirstStrategyElement;
	assert(fakeFirstElem->IsCheckSumValid());
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *curElem = fakeFirstElem->next;
	while (curElem && (curElem != fakeFirstElem)) {
		if ((curElem->inProgressCount == 1) && (curElem->unitInstanceId == actorUnitId)) {
			return curElem;
		}
		curElem = curElem->next;
	}
	return NULL;
}

// Resets a strategy element status (in progress, active)
void ResetStrategyElementStatus(ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elem) {
	if (!elem || !elem->IsCheckSumValid()) { return; }
	elem->aliveCount = 0;
	elem->unitInstanceId = -1;
	elem->inProgressCount = 0;
}


// Returns true if the strategy element is the fake one added by customROR when it updated strategy to adapt to maximum population.
bool IsCustomRorPopulationBeginStratElem(ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *stratElem) {
	return (stratElem->elementType == TAIUnitClass::AIUCTech) &&
		(strcmp(stratElem->unitName, CST_CUSTOMROR_FAKE_STRATELEM_MAXPOP_BEGIN) == 0);
}


// Returns the fake strategy element added by customROR when it updated strategy to adapt to maximum population.
// Returns NULL if not found.
ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *GetCustomRorMaxPopulationBeginStratElem(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI) {
	if (!buildAI || !buildAI->IsCheckSumValid()) {
		return NULL;
	}
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &buildAI->fakeFirstStrategyElement;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *curElem = fakeFirstElem->next;
	while (curElem && (curElem != fakeFirstElem)) {
		if (IsCustomRorPopulationBeginStratElem(curElem)) {
			return curElem;
		}
		curElem = curElem->next;
	}
	return NULL;
}

// Dumps strategy to text format, for debugging.
std::string ExportStrategyToText(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI) {
	std::string result = "";
	if (!buildAI || !buildAI->IsCheckSumValid()) {
		return NULL;
	}
	const char *newline = "\r\n"; // \r is required to display correctly in ROR editable text components
	char buffer[200];
	result = "Strategy for ";
	result += buildAI->commonAIObject.playerName;
	result += " (#";
	result += std::to_string(buildAI->commonAIObject.playerId);
	result += ") : ";
	result += buildAI->strategyFileName;
	result += newline;
	result += "Counter Ctgry DAT_ID Name                           Actor Unitid InProgress Alive Attempts #created Retrains"
#ifdef _DEBUG
	" ptr"
#endif
		;
	result += newline;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &buildAI->fakeFirstStrategyElement;
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentStratElem = fakeFirstElem->next;
	while (currentStratElem && (currentStratElem != fakeFirstElem)) {
		char category = ' ';
		switch (currentStratElem->elementType) {
		case TAIUnitClass::AIUCBuilding:
			if (currentStratElem->retrains > 0) {
				category = 'A';
			} else {
				category = 'B';
			}
			break;
		case TAIUnitClass::AIUCTech:
			category = 'R';
			break;
		case TAIUnitClass::AIUCCritical:
			category = 'C';
			break;
		case TAIUnitClass::AIUCLivingUnit:
			if (currentStratElem->retrains > 0) {
				category = 'T';
			} else {
				category = 'U';
			}
			break;
		case TAIUnitClass::AIUCEnableAttack:
			category = 'E';
			break;
		case TAIUnitClass::AIUCLoadNewStrategy:
			category = 'L';
			break;
		case TAIUnitClass::AIUCStrategyCmd:
			category = 'G';
			break;
		default:
			break;
		}
		sprintf_s(buffer, "%03ld     %1c     %-3ld    %-30s %-3ld   %-6ld %-2ld         %-2ld    %-4ld     %-4ld     %-3ld"
#ifdef _DEBUG
			"      0x%08lX"
#endif
			"%s",
			currentStratElem->counter, category, currentStratElem->unitDAT_ID,
			currentStratElem->unitName, currentStratElem->actor, currentStratElem->unitInstanceId, currentStratElem->inProgressCount,
			currentStratElem->aliveCount, currentStratElem->buildAttempts, currentStratElem->totalCount, currentStratElem->retrains, (long int)currentStratElem,
			newline
			);
		result += buffer;
		currentStratElem = currentStratElem->next;
	}
	return result;
}

