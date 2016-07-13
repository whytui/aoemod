#include "../include/StrategyBuilder.h"

using namespace STRATEGY;

// Set default value for random factor (a %)
int StrategyBuilder::randomPercentFactor = 30;

const char *newline = "\r\n";



/*** General methods ***/



// Clears potential units list and free each underlying PotentialUnitInfo object.
void StrategyBuilder::FreePotentialElementsList() {
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		delete unitInfo;
	}
	this->potentialUnitsList.clear();
	for each (PotentialBuildingInfo *bldInfo in this->potentialBuildingsList)
	{
		delete bldInfo;
	}
	this->potentialBuildingsList.clear();
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		delete resInfo;
	}
	this->potentialBuildingsList.clear();
}


// Get the strategy element that correspond to an Age Id. researchId must correspond to an age upgrade !
// For stone age, returns the "fake" first strategy element (beginning of strategy)
// For other researches, returns NULL
ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *StrategyBuilder::GetAgeStrategyElement(short int researchId) {
	switch (researchId) {
	case CST_RSID_STONE_AGE:
		return &this->buildAI->fakeFirstStrategyElement;
	case CST_RSID_TOOL_AGE:
		return this->seToolAge;
	case CST_RSID_BRONZE_AGE:
		return this->seBronzeAge;
	case CST_RSID_IRON_AGE:
		return this->seIronAge;
	default:
		return NULL;
	}
}


// Add a unit to selection and updates some internal variables accordingly
// Return total unit count in selection
int StrategyBuilder::AddUnitIntoToSelection(PotentialUnitInfo *unitInfo) {
	unitInfo->isSelected = true;
	actuallySelectedUnits.push_back(unitInfo);
	if (unitInfo->isBoat) {
		this->selectedWaterUnitsCount++;
	} else {
		this->selectedLandUnitsCount++;
	}
	for (int i = 0; i < 4; i++) {
		this->totalTrainUnitCosts[i] += unitInfo->trainCosts[i];
	}
	return this->actuallySelectedUnits.size();
}

// Add a research to strategy just before supplied insertion point. Updates "isInStrategy" field.
// Returns actual number of element that were added
int StrategyBuilder::AddResearchToStrategy(PotentialResearchInfo *resInfo, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *insertionPoint) {
	if (resInfo->isInStrategy) { return 0; }
	int res = 0;
	bool forceNotInsert = (resInfo->researchId >= CST_RSID_STONE_AGE) && (resInfo->researchId <= CST_RSID_IRON_AGE); // +104 for republic age
	// Do not insert ages as they are added in dedicated method.
	if (!forceNotInsert) {
		if (resInfo->researchDef->researchLocation >= 0) { // shadow research without a location must not be actually added to strategy
			AddUnitInStrategy_before(this->buildAI, insertionPoint, -1, resInfo->researchDef->researchLocation, TAIUnitClass::AIUCTech, resInfo->researchId, player,
				GetResearchLocalizedName(resInfo->researchId).c_str());
			res++;
		}
	}
	resInfo->isInStrategy = true;
	return res;
}

// Add a building to strategy just before supplied insertion point. Updates underlying fields (added count...)
// Returns actual number of element that were added
int StrategyBuilder::AddBuildingToStrategy(PotentialBuildingInfo *bldInfo, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *insertionPoint) {
	assert(bldInfo->desiredCount > bldInfo->addedInStrategyCount);
	assert(insertionPoint != NULL);
	assert((bldInfo->unitDef != NULL) && (bldInfo->unitDefId >= 0));
	int res = 0;
	if ((bldInfo->unitDef == NULL) || (bldInfo->unitDefId < 0) || (insertionPoint == NULL)) { return res; }
	if (bldInfo->desiredCount <= bldInfo->addedInStrategyCount) { return res; }
	if (AddUnitInStrategy_before(this->buildAI, insertionPoint, -1, -1, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding,
		bldInfo->unitDefId, this->player, bldInfo->unitDef->ptrUnitName)) {
		res++;
		// Update building info underlying fields
		bldInfo->addedInStrategyCount++;
		if (bldInfo->firstAddedInStrategy == NULL) {
			bldInfo->firstAddedInStrategy = insertionPoint->previous; // save link to strategy element
		}
		if ((this->seMarket == NULL) && (bldInfo->unitDefId == CST_UNITID_MARKET)) {
			this->seMarket = insertionPoint->previous;
		}
		if ((this->seGovCenter == NULL) && (bldInfo->unitDefId == CST_UNITID_GOVERNMENT_CENTER)) {
			this->seGovCenter = insertionPoint->previous;
		}
	}
	return res;
}

// Returns a pointer to the PotentialResearchInfo object for a research, or NULL if not found.
PotentialResearchInfo *StrategyBuilder::GetResearchInfo(short int researchId) {
	for each (PotentialResearchInfo *curResInfo in this->potentialResearchesList) {
		if (curResInfo->researchId == researchId) { return curResInfo; }
	}
	return NULL;
}
// Returns a pointer to the PotentialResearchInfo object for a research, or NULL if not found.
PotentialResearchInfo *StrategyBuilder::GetResearchInfo(ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef) {
	for each (PotentialResearchInfo *curResInfo in this->potentialResearchesList) {
		if (curResInfo->researchDef == resDef) { return curResInfo; }
	}
	return NULL;
}
// Add building to potential buildings list and initializes underlying info.
// Returns true if actually added
bool StrategyBuilder::AddPotentialResearchInfoToList(short int researchId) {
	ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef = this->player->GetResearchDef(researchId);
	if (!resDef || !this->global || !this->player) { return false; }
	// Check availability (tech tree)
	ROR_STRUCTURES_10C::STRUCT_TECH_DEF *techTreeDef = this->global->GetTechDef(this->player->techTreeId);
	if (techTreeDef) {
		if (DoesTechDisableResearch(techTreeDef, researchId)) {
			this->log += "Trying to add a disabled research to potential researches list (";
			this->log += std::to_string(researchId);
			this->log += ") => ignored";
			this->log += newline;
			return false; // not available = do not add
		}
	}
	PotentialResearchInfo *resInfo = this->GetResearchInfo(resDef);
	if (resInfo != NULL) {
		return false;
	}
	resInfo = new PotentialResearchInfo();
	resInfo->researchDef = resDef;
	resInfo->researchId = researchId;
	resInfo->hasOptionalRequirements = ResearchHasOptionalRequirements(resDef);
	resInfo->directRequirementsAreSatisfied = !resInfo->hasOptionalRequirements;
	potentialResearchesList.push_back(resInfo);
	resInfo->totalCosts = 0;
	if (resDef->costUsed1 > 0) { resInfo->totalCosts += resDef->costAmount1; }
	if (resDef->costUsed2 > 0) { resInfo->totalCosts += resDef->costAmount2; }
	if (resDef->costUsed3 > 0) { resInfo->totalCosts += resDef->costAmount3; }

	ROR_STRUCTURES_10C::STRUCT_TECH_DEF *techDef = global->GetTechDef(resDef->technologyId);
	resInfo->techDef = techDef;
	// Copy & handle requirements in member fields
	int currentSlotId = 0;
	for (int i = 0; i < 4; i++) {
		short int curReqResId = resDef->requiredResearchId[i];
		if (curReqResId >= 0) {
			resInfo->missingRequiredResearches[currentSlotId] = curReqResId;
			currentSlotId++;
			resInfo->researchesThatMustBePutBeforeMe.insert(curReqResId);
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *tmpUnitDef = FindBuildingDefThatEnablesResearch(this->player, curReqResId);
			if (tmpUnitDef && tmpUnitDef->IsCheckSumValidForAUnitClass()) {
				resInfo->unitsThatMustBePutBeforeMe.insert(tmpUnitDef->DAT_ID1);
				if (resInfo->directRequirementsAreSatisfied) {
					// We "validated" this research, but we have to make sure requirements are satisfied => those from "building's initiate research" aren't yet.
					this->AddPotentialBuildingInfoToList(tmpUnitDef->DAT_ID1);
				}
			}
		}
	}
	// Search for required age
	resInfo->requiredAge = FindResearchRequiredAge(this->player, resInfo->researchId);
	// Ages are added to strategy at the very beginning of the process, so we can already retrieve "my age" strategy element
	if (resInfo->requiredAge > 0) {
		resInfo->requiredAgeResearch = FindFirstElementInStrategy(&this->buildAI->fakeFirstStrategyElement, AIUCCritical, resInfo->requiredAge);
		if (resInfo->requiredAgeResearch == NULL) {
			resInfo->requiredAgeResearch = FindFirstElementInStrategy(&this->buildAI->fakeFirstStrategyElement, AIUCTech, resInfo->requiredAge);
		}
	}
	return true;
}

// Returns a pointer to the PotentialBuildingInfo object for a research, or NULL if not found.
PotentialBuildingInfo *StrategyBuilder::GetBuildingInfo(short int unitDefId) {
	for each (PotentialBuildingInfo *curBldInfo in this->potentialBuildingsList) {
		if (curBldInfo->unitDefId == unitDefId) { return curBldInfo; }
	}
	return NULL;
}
// Returns a pointer to the PotentialBuildingInfo object for a research, or NULL if not found.
PotentialBuildingInfo *StrategyBuilder::GetBuildingInfo(ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDef) {
	for each (PotentialBuildingInfo *curBldInfo in this->potentialBuildingsList) {
		if (curBldInfo->unitDef == unitDef) { return curBldInfo; }
	}
	return NULL;
}
// Add building to potential buildings list and initilizes underlying info.
// Returns true if actually added
bool StrategyBuilder::AddPotentialBuildingInfoToList(short int unitDefId) {
	if (unitDefId < 0) { return false; }
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDef = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *)player->GetUnitDefBase(unitDefId);
	if (unitDef && unitDef->IsTypeValid()) {
		return AddPotentialBuildingInfoToList(unitDef);
	}
	return false;
}
bool StrategyBuilder::AddPotentialBuildingInfoToList(ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDef) {
	if (!unitDef || !unitDef->IsTypeValid()) { return false; }
	PotentialBuildingInfo *bldInfo = this->GetBuildingInfo(unitDef->DAT_ID1);
	if (bldInfo != NULL) {
		return false;
	}
	bldInfo = new PotentialBuildingInfo();
	bldInfo->unitDef = unitDef;
	bldInfo->unitDefId = unitDef->DAT_ID1;
	bldInfo->enabledByResearchId = FindResearchThatEnableUnit(this->player, unitDef->DAT_ID1, 0);
	bldInfo->desiredCount = 1;
	ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *parentResDef = player->GetResearchDef(bldInfo->enabledByResearchId);
	if (parentResDef && (bldInfo->enabledByResearchId >= 0)) {
		bldInfo->enabledInAge = GetAgeResearchFromDirectRequirement(parentResDef);
	} else {
		bldInfo->enabledInAge = -1;
	}
	// Is it a high-priority building ?
	if (unitDef->initiatesResearch >= 0) {
		ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef = this->player->GetResearchDef(unitDef->initiatesResearch);
		if (resDef && (resDef->technologyId >= 0)) {
			ROR_STRUCTURES_10C::STRUCT_TECH_DEF *techDef = global->GetTechDef(resDef->technologyId);
			if (techDef) {
				if (DoesTechEnableUnit(techDef, CST_UNITID_FARM)) {
					bldInfo->highPriority = true; // The building that enables farming (market in standard game) must be built ASAP.
				}
			}
		}
	}
	if (unitDef->DAT_ID1 == CST_UNITID_FORUM) {
		bldInfo->addedInStrategyCount = 1; // A TC is added automatically at strategy initialization, with age researches
	}
	bool added = (this->potentialBuildingsList.insert(bldInfo).second);

	// What if age comes from a recursive requirement ? TODO
	return added;
}


// Updates this->mustBeBeforeThisElem and this->mustBeAfterThisElem according to known dependencies on other unit/researches
// Previous values of mustBeBeforeThisElem  and mustBeAfterThisElem are reset (lost)
void PotentialResearchInfo::ComputeStratElemPositionConstraints(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI) {
	if (!buildAI) { return; }
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirst = &buildAI->fakeFirstStrategyElement;
	this->mustBeBeforeThisElem = NULL;
	this->mustBeAfterThisElem = NULL;

	// Standard calculation of mustBeAfterThisElem and mustBeBeforeThisElem
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *curElem = buildAI->fakeFirstStrategyElement.next;
	while (curElem && (curElem != fakeFirst) && (this->mustBeBeforeThisElem == NULL)) {
		if (this->mustBeBeforeThisElem == NULL) {
			// "mustBeBeforeThisElem" is the first encountered item that match a "xxxThatMustBePutAfterMe" item
			bool foundInList = false;
			if ((curElem->elementType == TAIUnitClass::AIUCBuilding) || (curElem->elementType == TAIUnitClass::AIUCLivingUnit)) {
				foundInList = this->unitsThatMustBePutAfterMe.find((short int)curElem->unitDAT_ID) != this->unitsThatMustBePutAfterMe.end();
			}
			if ((curElem->elementType == TAIUnitClass::AIUCTech) || (curElem->elementType == TAIUnitClass::AIUCCritical)) {
				foundInList = this->researchesThatMustBePutAfterMe.find((short int)curElem->unitDAT_ID) != this->researchesThatMustBePutAfterMe.end();
			}
			if (foundInList) {
				this->mustBeBeforeThisElem = curElem;
			}
		}
		// "mustBeAfterThisElem" is the last encountered item that match a "before me" item. Once mustBeBeforeThisElem is found, we should NOT find any more of these.
		bool foundInList = false;
		if ((curElem->elementType == TAIUnitClass::AIUCBuilding) || (curElem->elementType == TAIUnitClass::AIUCLivingUnit)) {
			foundInList = this->unitsThatMustBePutBeforeMe.find((short int)curElem->unitDAT_ID) != this->unitsThatMustBePutBeforeMe.end();
		}
		if ((curElem->elementType == TAIUnitClass::AIUCTech) || (curElem->elementType == TAIUnitClass::AIUCCritical)) {
			foundInList = this->researchesThatMustBePutBeforeMe.find((short int)curElem->unitDAT_ID) != this->researchesThatMustBePutBeforeMe.end();
		}
		if (foundInList) {
			this->mustBeAfterThisElem = curElem;
		}
		curElem = curElem->next;
	}

	// Special cases: override mustBeBeforeThisElem and mustBeAfterThisElem values
	if (this->forcePlaceForFirstImpactedUnit || this->forcePutAsEarlyAsPossible) {
		ROR_STRUCTURES_10C::STRUCT_PLAYER *player = buildAI->mainAI->ptrStructPlayer;
		assert(player && player->IsCheckSumValid());
		if (!player || !player->IsCheckSumValid()) { return; }
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *bestFirstElem = this->mustBeAfterThisElem; // use previous calculations for initial position
		if (bestFirstElem) {
			bestFirstElem = bestFirstElem->next;
		}
		int maxValueForMaxElemCount = 7;
		if (this->forcePutAsEarlyAsPossible) {
			maxValueForMaxElemCount = 5; // Reduce random part (do not put tech too far from calculated point)
		}
		if (this->forcePlaceForFirstImpactedUnit) {
			// Find the first impacted unit in strategy
			for each (short int unitDefId in this->impactedUnitDefIds)
			{
				STRUCT_UNITDEF_BASE *unitDefBase = player->GetUnitDefBase(unitDefId);
				TAIUnitClass unitClass = TAIUnitClass::AIUCNone;
				if (unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass() && unitDefBase->DerivesFromLiving()) {
					if (unitDefBase->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) {
						unitClass = TAIUnitClass::AIUCBuilding;
					}
					if (unitDefBase->unitType == GLOBAL_UNIT_TYPES::GUT_LIVING_UNIT) {
						unitClass = TAIUnitClass::AIUCLivingUnit;
					}
				}
				ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *firstElemThisUnit = FindFirstElementInStrategy(fakeFirst, unitClass, unitDefId);
				if (firstElemThisUnit && firstElemThisUnit->IsCheckSumValid()) {
					if ((bestFirstElem == NULL) || GetFirstElementOf(buildAI, bestFirstElem, firstElemThisUnit) == firstElemThisUnit) {
						bestFirstElem = firstElemThisUnit;
					}
				}
			}
		}
		if (bestFirstElem != NULL) {
			// Do not insert right here. Let AI train some units before starting upgrades.
			int elemCount = 0;
			int maxElemCount = randomizer.GetRandomValue_normal_moreFlat(2, maxValueForMaxElemCount); // Do not add further than x "elems" away from base calculated point
			ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *curElem = bestFirstElem;
			while (curElem && (elemCount < maxElemCount)) { // Just move a bit further
				if (curElem->elementType == TAIUnitClass::AIUCCritical) { elemCount = maxElemCount; } // Force "don't go further" / probably an age upgrade
				elemCount++;
				curElem = curElem->next;
			}
			// We could check research is not too early compared to its age...
			if (this->requiredAgeResearch) {
				if (GetFirstElementOf(buildAI, curElem, this->requiredAgeResearch) == this->requiredAgeResearch) {
					// Required age research IS before insertion point, which is good :)
					if (curElem) {
						this->mustBeBeforeThisElem = curElem->next;
						this->mustBeAfterThisElem = curElem->previous;
						return; // Position has been determined, no need to run the treatments below.
					}
				} else {
					assert(false && "Insertion point is prior to required age, this should never happen !");
				}
			}
		}
	}
}



/*** Units selection methods ***/


// Fills unitInfos with all available military units from tech tree.
// Towers are ignored (not added to list). Boats are ignored on non-water maps.
// *** Make sure to delete all PotentialUnitInfo from list when you're finished with the list ***
void StrategyBuilder::CollectPotentialUnitsInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid() || !player->ptrStructDefUnitTable || !player->ptrResearchesStruct) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	bool isWaterMap = IsDockRelevantForMap(settings->mapTypeChoice);
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return; }
	if (!global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid()) { return; }
	// Retrieve player tech tree
	assert(player->techTreeId < global->technologiesInfo->technologyCount);
	STRUCT_TECH_DEF *techDefTechTree = NULL;
	if (player->techTreeId >= 0) { // a player *might* not have a tech tree.
		techDefTechTree = &global->technologiesInfo->ptrTechDefArray[player->techTreeId];
		if (techDefTechTree->effectCount <= 0) { techDefTechTree = NULL; }
	}
	this->player = player;

	// Store list of available research (not disabled in tech tree) to avoid having to recompute it more than once
	std::list<short int> allAvailableResearches;
	std::list<short int> allDisabledResearches; // analog: full list of tech tree-disable researches
	for (int curRsId = 0; curRsId < player->ptrResearchesStruct->researchCount; curRsId++) {
		STRUCT_RESEARCH_DEF *curResDef = player->GetResearchDef(curRsId);
		bool researchIsDisabled = false;
		for (int i = 0; (techDefTechTree != NULL) && (i < techDefTechTree->effectCount) && !researchIsDisabled; i++) {
			if ((techDefTechTree->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_DISABLE_RESEARCH) &&
				(techDefTechTree->ptrEffects[i].GetValue() == curRsId)) {
				researchIsDisabled = true;
			}
		}
		if (researchIsDisabled) {
			allDisabledResearches.push_back(curRsId);
		} else {
			allAvailableResearches.push_back(curRsId);
		}
	}

	for (int curUnitDefID = 0; curUnitDefID < player->structDefUnitArraySize; curUnitDefID++) {
		STRUCT_UNITDEF_BASE *unitDefBase = player->GetUnitDefBase(curUnitDefID);
		bool validUnit = (unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass());
		validUnit = validUnit && unitDefBase->DerivesFromLiving();
		validUnit = validUnit && IsNonTowerMilitaryUnit(unitDefBase->unitAIType);
		STRUCT_UNITDEF_LIVING *unitDefLiving = (STRUCT_UNITDEF_LIVING *)unitDefBase;
		assert(!validUnit || unitDefLiving->IsCheckSumValid());
		validUnit = validUnit && (unitDefLiving->towerMode == 0) && (unitDefLiving->speed > 0) &&
			(unitDefLiving->trainButton > 0) && (unitDefLiving->trainLocation >= 0); // both exclude some heroes/cheats/non-standard units, but not all of them


		bool availableForPlayer = validUnit && (unitDefLiving->availableForPlayer != 0); // Warning: this excludes units that are enabled by researches
		short int researchIdThatEnablesUnit = -1; // Store the researchId that enables current unit... If any
		short int ageResearchIdThatEnablesUnit = -1; // Age (identified by research id) where unit can become available

		// Search for a research (available in my tech tree) that enables unit
		if (!availableForPlayer && validUnit) {
			short int enableUnitResearchId = FindResearchThatEnableUnit(player, unitDefBase->DAT_ID1, 0);
			while ((enableUnitResearchId >= 0) && !availableForPlayer) {
				STRUCT_RESEARCH_DEF *enableUnitResearchDef = player->GetResearchDef(enableUnitResearchId);

				if (enableUnitResearchDef && (enableUnitResearchDef->technologyId >= 0) && (enableUnitResearchDef->technologyId < global->technologiesInfo->technologyCount)) {
					// Is this research disabled by my tech tree ?
					auto it = std::find_if(allDisabledResearches.begin(), allDisabledResearches.end(),
						[enableUnitResearchId](short int availableResId) { return enableUnitResearchId == availableResId; }
					);
					bool researchIsDisabled = (it != allDisabledResearches.end()); // If found, then the research IS disabled
					STRUCT_TECH_DEF *enableUnitTechDef = &global->technologiesInfo->ptrTechDefArray[enableUnitResearchDef->technologyId];
					if (enableUnitTechDef && !researchIsDisabled) { // if research object is found AND available in my tech tree (not disabled)
						for (int i = 0; i < enableUnitTechDef->effectCount; i++) {
							if (enableUnitTechDef->ptrEffects[i].IsEnableUnit(unitDefBase->DAT_ID1)) {
								availableForPlayer = true;
								researchIdThatEnablesUnit = enableUnitResearchId;
								// By the way, find which age is required. If we're lucky, it's a direct requirement in research
								ageResearchIdThatEnablesUnit = GetAgeResearchFromDirectRequirement(enableUnitResearchDef);
								// If we're unlucky, requirement is missing in empires.dat (example: priest). Find train location (ex:temple)'s required age from research that enabled is
								if ((ageResearchIdThatEnablesUnit < 0) && (unitDefLiving->trainLocation >= 0)) {
									short int bldEnabler = FindResearchThatEnableUnit(player, unitDefLiving->trainLocation, 0);
									STRUCT_RESEARCH_DEF *bldEnablerResearchDef = player->GetResearchDef(bldEnabler);
									ageResearchIdThatEnablesUnit = GetAgeResearchFromDirectRequirement(bldEnablerResearchDef);
								}
							}
						}
					}
				}
				// Several researches might enable a same unit (for example if tech trees enable some units). Not standard, but possible.
				enableUnitResearchId = FindResearchThatEnableUnit(player, unitDefBase->DAT_ID1, enableUnitResearchId + 1); // Search next
			}
		}
		validUnit = validUnit && availableForPlayer;

		bool techTreeAffectsCurrentUnit = (techDefTechTree != NULL) && DoesTechAffectUnit(techDefTechTree, unitDefBase);
		bool currentUnitHasTechTreeBonus = false;
		if (validUnit && techTreeAffectsCurrentUnit) {
			for (int i = 0; i < techDefTechTree->effectCount; i++) {
				if (techDefTechTree->ptrEffects[i].IsDisableUnit(unitDefBase->DAT_ID1)) {
					validUnit = false;
				}
				if (validUnit && techDefTechTree->ptrEffects[i].HasValidEffect()) {
					// We consider valid effects in tech tree (that are NOT disable unit) are always positive effects
					// What could we do otherwise ? Ignore the unit ? Add flag in PotentialUnitInfo ?
					currentUnitHasTechTreeBonus = true;
				}
			}
		}
		if (validUnit) {
			PotentialUnitInfo *unitInfo = new PotentialUnitInfo();
			unitInfo->hasUnavailableUpgrade = false; // updated below
			unitInfo->availableRelatedResearchesCount = 0; // updated below
			unitInfo->unavailableRelatedResearchesCount = 0; // updated below
			unitInfo->baseUnitDefLiving = unitDefLiving;
			unitInfo->upgradedUnitDefLiving = unitDefLiving; // Default: base unit. We'll update this later if necessary
			unitInfo->unitDefId = unitDefLiving->DAT_ID1;
			unitInfo->isBoat = IsWaterUnit(unitDefLiving->unitAIType);

			// (just) Get list of (available) upgraded units + base unit
			for each (short int curResearchId in allAvailableResearches)
			{
				STRUCT_RESEARCH_DEF *curResearchDef = player->GetResearchDef(curResearchId);
				short int techId = -1;
				STRUCT_TECH_DEF *techDef = NULL;
				if (curResearchDef) {
					techId = curResearchDef->technologyId;
					if ((techId >= 0) && (techId < global->technologiesInfo->technologyCount)) {
						techDef = &global->technologiesInfo->ptrTechDefArray[techId];
					}
				}
				if (techDef && techDef->ptrEffects) {
					if (DoesTechAffectUnit(techDef, unitDefLiving)) {
						for (int i = 0; i < techDef->effectCount; i++) {
							short int upgradeTargetUnitDefId = techDef->ptrEffects[i].UpgradeUnitGetTargetUnit();
							if ((upgradeTargetUnitDefId >= 0) && (techDef->ptrEffects[i].effectUnit == unitDefLiving->DAT_ID1)) {
								// We found an upgrade for our unit
								unitInfo->upgradesUnitDefId.push_back(upgradeTargetUnitDefId);
								int totalCost = 0;
								if (curResearchDef->costUsed1) { totalCost += curResearchDef->costAmount1; }
								if (curResearchDef->costUsed2) { totalCost += curResearchDef->costAmount2; }
								if (curResearchDef->costUsed3) { totalCost += curResearchDef->costAmount3; }
								if (totalCost >= 1000) { unitInfo->isSuperUnit = true; } // Is there any better criteria ? not sure
							}
							if (upgradeTargetUnitDefId == unitDefLiving->DAT_ID1) {
								// We found the base unit of our unit (our unit is an upgrade for some other unit)
								unitInfo->baseUnitDefId = techDef->ptrEffects[i].effectUnit;
							}
						}
					}
				}
			}
			// Determine best upgraded unit (now we have the list of available unit upgrades)
			short int bestUnitDefId = unitDefLiving->DAT_ID1;
			short int bestHP = unitDefLiving->totalHitPoints;
			float bestRange = unitDefLiving->displayedRange;
			short int bestAttack = unitDefLiving->displayedAttack;
			short int bestArmor = unitDefLiving->displayedArmor;
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *bestUpgradedUnit = unitDefLiving;
			float bestSpeed = unitDefLiving->speed;
			for each (short int upgradedUnitDefId in unitInfo->upgradesUnitDefId)
			{
				ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *upgradedDef = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *)GetUnitDefStruct(player, upgradedUnitDefId);
				bool currentIsBetter = false;
				if (upgradedDef && upgradedDef->IsCheckSumValid()) {
					if ((upgradedDef->totalHitPoints > bestHP) || (upgradedDef->displayedAttack > bestAttack) ||
						(upgradedDef->displayedArmor > bestArmor) || (upgradedDef->displayedRange > bestRange)) {
						currentIsBetter = true;
					} else {
						// If previous criterion are all equals, try speed (we're careful, some upgrade could make a unit stronger but a bit slower => use this as last resort)
						currentIsBetter = (upgradedDef->speed > bestSpeed);
					}
					if (currentIsBetter) {
						bestAttack = upgradedDef->displayedAttack;
						bestArmor = upgradedDef->displayedArmor;
						bestHP = upgradedDef->totalHitPoints;
						bestRange = upgradedDef->displayedRange;
						bestSpeed = upgradedDef->speed;
						bestUnitDefId = upgradedUnitDefId;
						bestUpgradedUnit = upgradedDef;
					}
				}
			}
			// Value various info with upgraded unit data (not base unit).
			unitInfo->strongestUpgradeUnitDefId = bestUnitDefId;
			unitInfo->upgradedUnitDefLiving = bestUpgradedUnit;
			unitInfo->hitPoints = bestUpgradedUnit->totalHitPoints;
			unitInfo->range = bestUpgradedUnit->maxRange;
			unitInfo->isMelee = (unitInfo->range < 2);
			unitInfo->speedBase = unitInfo->baseUnitDefLiving->speed; // base speed value
			unitInfo->speedUpgraded = bestUpgradedUnit->speed; // updated speed value
			unitInfo->unitAIType = bestUpgradedUnit->unitAIType;
			unitInfo->hasCivBonus = currentUnitHasTechTreeBonus;
			unitInfo->enabledByResearchId = researchIdThatEnablesUnit;
			unitInfo->ageResearchId = ageResearchIdThatEnablesUnit;
			unitInfo->unitName = bestUpgradedUnit->ptrUnitName;
			unitInfo->displayedAttack = bestUpgradedUnit->displayedAttack;
			if (bestUpgradedUnit->blastRadius > 0) {
				unitInfo->displayedAttack = (unitInfo->displayedAttack * 115) / 100; // Give a "fake" attack bonus for units with blast damage (15% ?)
			}
			unitInfo->conversionResistance = this->crInfo->GetConversionResistance(player->civilizationId, bestUpgradedUnit->unitAIType);

			// Analyze costs
			for (int i = 0; i < 3; i++) {
				if ((unitDefLiving->costs[i].costType >= AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_FOOD) &&
					(unitDefLiving->costs[i].costType <= AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_GOLD) &&
					(unitDefLiving->costs[i].costPaid)) {
					unitInfo->totalResourceCost += unitDefLiving->costs[i].costAmount;
					if (unitDefLiving->costs[i].costType == AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_GOLD) {
						unitInfo->costsGold = true;
					}
					unitInfo->trainCosts[unitDefLiving->costs[i].costType] = unitDefLiving->costs[i].costAmount;
				}
			}
			unitInfo->weightedCost = 0;
			for (int i = 0; i < 4; i++) {
				unitInfo->weightedCost += GetWeightedCostValue(unitInfo->trainCosts[i], (RESOURCE_TYPES)i);
			}

			// Collect research info for this unit (available/unavailable, unit upgrades info, speed updates).
			for each (short int curResearchId in allDisabledResearches)
			{
				STRUCT_RESEARCH_DEF *curResearchDef = player->GetResearchDef(curResearchId);
				short int techId = -1;
				STRUCT_TECH_DEF *techDef = NULL;
				if (curResearchDef) {
					techId = curResearchDef->technologyId;
					if ((techId >= 0) && (techId < global->technologiesInfo->technologyCount)) {
						techDef = &global->technologiesInfo->ptrTechDefArray[techId];
					}
				}
				if (techDef && techDef->ptrEffects) {
					if (DoesTechAffectUnit(techDef, unitDefLiving)) {
						unitInfo->unavailableRelatedResearchesCount++;
						for (int i = 0; i < techDef->effectCount; i++) {
							short int upgradeTargetUnitDefId = techDef->ptrEffects[i].UpgradeUnitGetTargetUnit();
							if ((upgradeTargetUnitDefId >= 0) && (techDef->ptrEffects[i].effectUnit == unitDefLiving->DAT_ID1)) {
								// We found an upgrade for our unit (... which is disabled by tech tree)
								unitInfo->hasUnavailableUpgrade = true;
							}
						}
					}
				}
			}
			// Collect some info on techs that improve the unit (speed...)
			for each (short int curResearchId in allAvailableResearches)
			{
				STRUCT_RESEARCH_DEF *curResearchDef = player->GetResearchDef(curResearchId);
				short int techId = -1;
				STRUCT_TECH_DEF *techDef = NULL;
				if (curResearchDef) {
					techId = curResearchDef->technologyId;
					if ((techId >= 0) && (techId < global->technologiesInfo->technologyCount)) {
						techDef = &global->technologiesInfo->ptrTechDefArray[techId];
					}
				}
				if (techDef && techDef->ptrEffects) {
					if (DoesTechAffectUnit(techDef, unitDefLiving)) {
						unitInfo->availableRelatedResearchesCount++;
						for (int i = 0; i < techDef->effectCount; i++) {
							bool isAttrModifier = ((techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) ||
								(techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) ||
								(techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT));
							bool affectsMe = ((techDef->ptrEffects[i].effectUnit == unitDefLiving->DAT_ID1) ||
								(techDef->ptrEffects[i].effectClass == unitDefLiving->unitAIType));
							if (isAttrModifier && affectsMe && (techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_SPEED)) {
								float value = techDef->ptrEffects[i].GetValue();
								if (techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) {
									unitInfo->speedUpgraded += value;
								}
								if (techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) {
									unitInfo->speedUpgraded = value;
								}
								if (techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT) {
									unitInfo->speedUpgraded = unitInfo->speedUpgraded * value;
								}
							}
						}
					}
				}
			}
			// Has special bonus ?
			if (unitInfo->upgradedUnitDefLiving && unitInfo->upgradedUnitDefLiving->ptrAttacksList) {
				for (int i = 0; i < unitInfo->upgradedUnitDefLiving->attacksCount; i++) {
					if ((unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CAMEL_ON_CAVALRY) ||
						(unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_SLINGER_ON_ARCHERS) ||
						(unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CAVALRY_ON_INFANTRY) ||
						(unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CHARIOTS_ON_PRIESTS)
						) {
						unitInfo->hasSpecificAttackBonus = true;
					}
				}
			}
			// Save unit infos in list
			this->potentialUnitsList.push_back(unitInfo);
		}
	}
}


// Compute all score fields for all units in potential units list.
void StrategyBuilder::ComputeScoresForPotentialUnits() {
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		for (int i = 0; i < MC_COUNT; i++) {
			unitInfo->strengthVs[i] = 0;
			unitInfo->weaknessVs[i] = 0;
		}

		// availableRelatedResearchesProportion is a % value (0-100)
		int availableRelatedResearchesProportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->unavailableRelatedResearchesCount + unitInfo->availableRelatedResearchesCount);
		// A combination of speed and hit points
		float adjustedSpeed = unitInfo->speedUpgraded - 0.4f;
		if (adjustedSpeed < 0.4) { adjustedSpeed = 0.4f; } // This gives more importance to speed (more penalty for slow units): lower difference between speed values = more difference after multiplication
		float speedHitPointsFactor = adjustedSpeed * (float)unitInfo->hitPoints;
		// Examples: elephant=(0.4*600)=240. legion=(0.8*160)=128, axe=(.8*50)=40, cataphract=(1.6*180)=288, scythe=(1.6*137)=219. heavy_cat=(150*0.4)=60
		if (speedHitPointsFactor > 300.0f) { speedHitPointsFactor = 300.0f; }
		if (speedHitPointsFactor < 30.0f) { speedHitPointsFactor = 30.0f; }
		speedHitPointsFactor = speedHitPointsFactor / 3; // get a % value 1-100

		float attackPoints = unitInfo->displayedAttack;
		if (unitInfo->isMelee) {
			attackPoints -= 2; // Melee may hardly catch their targets, targets have armors, etc. +This allow to consider lower attack value is 3.
			if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider) {
				attackPoints -= 4; // To compensate the fact it receives no attack upgrade from storage pit techs.
			}
		}
		if (attackPoints < 1) { attackPoints = 1; }
		float damageScore = 0; // A value 1-4 representing unit attack score. 4 is good, 1 is bad.
		if (attackPoints < 4) {
			damageScore = 1; // Very weak: melee<6, archers<4
		}
		if ((attackPoints >= 4) && (attackPoints < 6)) {
			if (unitInfo->isMelee) {
				damageScore = 2; // Weak: melee=6-7
			} else {
				damageScore = 3; // Medium : archers=4-5
			}
		}
		if ((attackPoints >= 6) && (attackPoints < 10)) {
			if (unitInfo->isMelee) {
				damageScore = 3; // Medium: melee=8-11, archers=6-9
			} else {
				damageScore = 4; // Good: archers=6-9
			}
		}
		if (damageScore <= 0) { damageScore = 4; } // >= 10: good
		if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
			if (availableRelatedResearchesProportion >= 80) {
				damageScore = 4; // Force "good" for priests with almost all upgrades
			} else {
				damageScore = 3; // medium for basic priests
			}
		}
		unitInfo->damageScore = (int)damageScore;

		// Compute priest-related values
		this->ComputeScoresVsPriests(unitInfo);

		// Compute towers-relative values
		this->ComputeScoresVsTower(unitInfo);
		if (damageScore <= 1) {
			unitInfo->strengthVs[MC_TOWER] = (unitInfo->strengthVs[MC_TOWER] * 33) / 100;
		}
		if (damageScore == 2) {
			unitInfo->strengthVs[MC_TOWER] = (unitInfo->strengthVs[MC_TOWER] * 66) / 100;
		}

		// Compute siege-related values
		float baseValue = speedHitPointsFactor;
		if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon) {
			if (unitInfo->upgradedUnitDefLiving->blastRadius >= 0.8) { // Catapult-like (blast damage = good vs siege)
				baseValue = baseValue * 3.0f; // Big bonus for blast-siege weapon against siege weapons
				if (!unitInfo->hasUnavailableUpgrade && (availableRelatedResearchesProportion >= 80)) {
					baseValue = baseValue * 1.25f; // Slightly improved bonus if fully upgraded
				}
			} else { // low blast zone or NO blast damage : stone thrower, ballista, or equivalent custom unit
				if (unitInfo->hasUnavailableUpgrade || (availableRelatedResearchesProportion <= 80)) {
					// Moreover, unit is not fully upgraded
					baseValue = baseValue * 0.5f; // Reduced bonus
					unitInfo->weaknessVs[MC_SIEGE] += 10;
				} else {
					// Well upgraded unit: not so bad. Do not apply a bonus nor a malus
				}
			}
		} else { // Other than siege
			if (!unitInfo->isMelee) {
				if (unitInfo->unitAIType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
					baseValue = baseValue * 0.33f; // archers : attack not efficient vs siege
				}
				unitInfo->weaknessVs[MC_SIEGE] += 25; // archers/priests : weak vs siege (easily killed)
			} else {
				// Melee : credit for blast damage !
				if (unitInfo->upgradedUnitDefLiving->blastRadius >= 0.8) {
					baseValue = baseValue * 1.25f; // not a too big bonus, blast damage might be restricted to fully-upgraded unit (scythe...), which is rarely reached in RM.
				}
			}
		}
		if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
			baseValue = baseValue * 2.0f; // Because priests get a very, very low score with criteria above.
		}
		baseValue = baseValue * (((damageScore) / 4) + 0.75f); // ((damageScore) / 4) + 0.75 can be 1, 1.25, 1.5, 1.75.
		if (damageScore == 1) {
			baseValue = baseValue * 0.75f; // An additional penalty for very weak units
		}
		if (unitInfo->totalResourceCost <= 100) {
			baseValue = baseValue * 1.2f;
			if (unitInfo->totalResourceCost <= 50) {
				baseValue = baseValue * 1.5f; // Cheap units can be produced in higher quantities (cumulated bonus with previous one)
			}
		}

		if (baseValue > 100.0f) { baseValue = 100.0f; }
		if (baseValue < 0.0f) { baseValue = 0.0f; }

		unitInfo->strengthVs[MC_SIEGE] += (int)baseValue;
		if (speedHitPointsFactor < 40) {
			unitInfo->weaknessVs[MC_SIEGE] += 10;
			if (speedHitPointsFactor < 20) {
				unitInfo->weaknessVs[MC_SIEGE] += 10;
			}
		}

		// Compute values vs fast ranged units (like chariot/cavalry archer)
		unitInfo->strengthVs[MC_FAST_RANGED] = 10;
		unitInfo->weaknessVs[MC_FAST_RANGED] = 10;
		switch (unitInfo->unitAIType) {
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher:
			unitInfo->strengthVs[MC_FAST_RANGED] = 100;
			unitInfo->weaknessVs[MC_FAST_RANGED] = 10;
			break;
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher:
			unitInfo->strengthVs[MC_FAST_RANGED] = 35;
			unitInfo->weaknessVs[MC_FAST_RANGED] = 40;
			break;
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon:
			unitInfo->strengthVs[MC_FAST_RANGED] = 40;
			unitInfo->weaknessVs[MC_FAST_RANGED] = 10;
			break;
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger:
			unitInfo->strengthVs[MC_FAST_RANGED] = 80;
			unitInfo->weaknessVs[MC_FAST_RANGED] = 5;
			break;
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest:
			unitInfo->strengthVs[MC_FAST_RANGED] = 5;
			unitInfo->weaknessVs[MC_FAST_RANGED] = 95;
			break;
		default:
			break;
		}
		if (unitInfo->isMelee) {
			unitInfo->strengthVs[MC_FAST_RANGED] = 40; // default for units faster than normal, but not really fast
			unitInfo->weaknessVs[MC_FAST_RANGED] = 40; // default for units faster than normal, but not really fast
			if (unitInfo->speedUpgraded < 1.4) {
				unitInfo->strengthVs[MC_FAST_RANGED] = 20;
				if (unitInfo->speedUpgraded < 1.1) {
					unitInfo->strengthVs[MC_FAST_RANGED] -= 10;
				}
				unitInfo->weaknessVs[MC_FAST_RANGED] = 80;
			}
			if (unitInfo->speedUpgraded >= 2) {
				unitInfo->strengthVs[MC_FAST_RANGED] = 80;
				unitInfo->weaknessVs[MC_FAST_RANGED] = 10;
			}
		}
		// Take into account attack bonuses vs mounted archers OR vs archers
		if (unitInfo->upgradedUnitDefLiving && unitInfo->upgradedUnitDefLiving->ptrAttacksList) {
			for (int i = 0; i < unitInfo->upgradedUnitDefLiving->attacksCount; i++) {
				if ((unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CAMEL_ON_CAVALRY) ||
					(unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_SLINGER_ON_ARCHERS)) {
					unitInfo->strengthVs[MC_FAST_RANGED] += 30;
				}
			}
		}
		if (damageScore <= 1) {
			unitInfo->strengthVs[MC_FAST_RANGED] = (unitInfo->strengthVs[MC_FAST_RANGED] * 20) / 100;
		}
		if (damageScore == 2) {
			unitInfo->strengthVs[MC_FAST_RANGED] = (unitInfo->strengthVs[MC_FAST_RANGED] * 66) / 100;
		}
		if (unitInfo->totalResourceCost <= 90) {
			unitInfo->strengthVs[MC_FAST_RANGED] = (unitInfo->strengthVs[MC_FAST_RANGED] * 110) / 100;
			if (unitInfo->totalResourceCost <= 50) {
				unitInfo->strengthVs[MC_FAST_RANGED] = (unitInfo->strengthVs[MC_FAST_RANGED] * 125) / 100;
			}
		}
		if (unitInfo->strengthVs[MC_FAST_RANGED] > 100) { unitInfo->strengthVs[MC_FAST_RANGED] = 100; }


		// Compute values vs melee units
		unitInfo->strengthVs[MC_MELEE] = 0;
		unitInfo->weaknessVs[MC_MELEE] = 0;
		if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
			unitInfo->strengthVs[MC_MELEE] = 45;
			unitInfo->weaknessVs[MC_MELEE] = 20;
			if (availableRelatedResearchesProportion >= 80) {
				unitInfo->strengthVs[MC_MELEE] += 20;
			}
		} else {
			if (unitInfo->isMelee) {
				unitInfo->strengthVs[MC_MELEE] = 50;
				unitInfo->weaknessVs[MC_MELEE] = 10;
				if ((unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupMountedSoldier) ||
					(unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot)) {
					// Cavalry, camel, scout + chariot&scythe
					unitInfo->strengthVs[MC_MELEE] -= 10;
					unitInfo->weaknessVs[MC_MELEE] += 20; // Speed is no use in melee vs melee, it just means the cost paid for speed is useless and waste
				}
				if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupFootSoldier) {
					// clubman + swordsman families. Quite adapted, can train a lot of them (low cost).
					unitInfo->strengthVs[MC_MELEE] += 15;
				}
				if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPhalanx) {
					// Hoplite + upgrades
					unitInfo->strengthVs[MC_MELEE] += 50; // Hoplites are excellent in melee combat
				}
				if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider) {
					// War elephants (NOT archer)
					unitInfo->strengthVs[MC_MELEE] += 20; // war elephants "like" melee combat, especially due to their blast damage.
				}
			} else {
				// Ranged (non-priest)
				switch (unitInfo->unitAIType) {
				case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger:
					unitInfo->strengthVs[MC_MELEE] = 0;
					unitInfo->weaknessVs[MC_MELEE] = 100;
					break;
				case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon:
					unitInfo->strengthVs[MC_MELEE] = 5; // base value (not counting upgrades, etc, see below)
					unitInfo->weaknessVs[MC_MELEE] = 10;
					if (!unitInfo->hasUnavailableUpgrade) {
						unitInfo->strengthVs[MC_MELEE] += 10;
					} else {
						unitInfo->weaknessVs[MC_MELEE] += 15; // siege weapons with few upgrades are easy targets for melee units
					}
					if (availableRelatedResearchesProportion >= 80) {
						unitInfo->strengthVs[MC_MELEE] += 10;
					} else {
						unitInfo->weaknessVs[MC_MELEE] += 15; // siege weapons with few upgrades are easy targets for melee units
					}
					break;
				default:
					// Standard archer types
					unitInfo->strengthVs[MC_MELEE] = 40;
					unitInfo->weaknessVs[MC_MELEE] = 10;
					break;
				}
			}
			// Here: all but priests
			if (unitInfo->upgradedUnitDefLiving && unitInfo->upgradedUnitDefLiving->ptrAttacksList) {
				for (int i = 0; i < unitInfo->upgradedUnitDefLiving->attacksCount; i++) {
					if (unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CAVALRY_ON_INFANTRY) {
						unitInfo->strengthVs[MC_MELEE] += 10; // Note: The attack bonus only applies to barracks infantry, so don't add too much strength
					}
					if (unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CAMEL_ON_CAVALRY) {
						unitInfo->weaknessVs[MC_MELEE] += 10; // Camels are good vs cavalry, but weak against all other melee units
					}
				}
			}
			if (damageScore <= 2) {
				unitInfo->strengthVs[MC_MELEE] -= 30;
			}
			if (damageScore == 4) {
				unitInfo->strengthVs[MC_MELEE] += 10;
			}
		}
		if (unitInfo->strengthVs[MC_MELEE] > 100) { unitInfo->strengthVs[MC_MELEE] = 100; }
	} // end loop on units
}

// Compute score fields for priests in unitInfo object.
void StrategyBuilder::ComputeScoresVsPriests(PotentialUnitInfo *unitInfo) {
	unitInfo->weaknessVs[MC_PRIEST] = 0;
	unitInfo->strengthVs[MC_PRIEST] = 20; // Default base strength value
	if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
		// availableRelatedResearchesProportion is a % value (0-100)
		int availableRelatedResearchesProportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->unavailableRelatedResearchesCount + unitInfo->availableRelatedResearchesCount);
		int scoreForPriestResearches = (availableRelatedResearchesProportion < 80) ? 35 : 95;
		// Well-developed priests are good against priests (at least, allows converting back my units OR converting enemy units - the goal is not necessarily to convert enemy priests)
		unitInfo->strengthVs[MC_PRIEST] = scoreForPriestResearches;
		unitInfo->weaknessVs[MC_PRIEST] = 100 - scoreForPriestResearches;
		return;
	}
	float unitSpeed = unitInfo->speedUpgraded; // upgradedUnit ? unitInfo->speedUpgraded : unitInfo->speedBase;

	// Conversion resistances: both a defensive & offensive criteria
	if (unitInfo->conversionResistance < 1) { // Easier to convert (does not exist in standard game)
		unitInfo->weaknessVs[MC_PRIEST] += 20;
		unitInfo->strengthVs[MC_PRIEST] -= 20;
	}
	if (unitInfo->conversionResistance > 4) {
		unitInfo->weaknessVs[MC_PRIEST] -= 100; // Very hard to convert - almost insensitive to conversion !
		unitInfo->strengthVs[MC_PRIEST] += 100;
	}
	if ((unitInfo->conversionResistance > 2) && (unitInfo->conversionResistance <= 4)) {
		unitInfo->weaknessVs[MC_PRIEST] -= 40; // Hard to convert
		unitInfo->strengthVs[MC_PRIEST] += 40;
	}
	if ((unitInfo->conversionResistance > 1) && (unitInfo->conversionResistance <= 2)) {
		unitInfo->weaknessVs[MC_PRIEST] -= 20; // A bit hard to convert
		unitInfo->strengthVs[MC_PRIEST] += 20;
	}
	// Slow units are weak against priests
	// Note on standard speeds: 0.8-0.9 slow (siege, elephants, base hopite&priests), 1.1-1.2 normal(villagers, infantry,archers), 2 fast
	// Boats: 1.35-1.4, 1.75, 2, 2.5(upgraded trade ship). War ships are all in 1.35-1.75
	int basePenaltyValue = 10;
	if (unitSpeed < 1) {
		basePenaltyValue = 20;
	}
	if ((unitSpeed >= 1.0f) && (unitSpeed < 1.5f)) {
		basePenaltyValue = 10;
	}
	if (unitSpeed >= 1.5f) {
		basePenaltyValue = 2; // Fast units: can defend correctly against priests.
	}
	// Costly units are a weakness against priests (especially elephants, but not only)
	if (unitInfo->totalResourceCost > 160) {
		basePenaltyValue *= 3;
		unitInfo->strengthVs[MC_PRIEST] -= 10;
	}
	if ((unitInfo->totalResourceCost > 100) && (unitInfo->totalResourceCost <= 160)) {
		basePenaltyValue = basePenaltyValue * 2;
	}
	if ((unitInfo->totalResourceCost > 50) && (unitInfo->totalResourceCost <= 100)) {
		basePenaltyValue = (basePenaltyValue * 150) / 100;
		unitInfo->strengthVs[MC_PRIEST] += 10;
	}
	if ((unitInfo->totalResourceCost <= 50)) {
		unitInfo->strengthVs[MC_PRIEST] += 15; // low cost = attack in higher numbers
	}
	if (unitInfo->costsGold) {
		basePenaltyValue = (basePenaltyValue * 125) / 100; // Slightly heavier penalty if costs gold. Eg swordsmen vs clubmen, cavalry vs scouts
	}
	if (unitInfo->isMelee) {
		basePenaltyValue = (basePenaltyValue * 150) / 100;
	}
	if (basePenaltyValue > 100) { basePenaltyValue = 100; }

	unitInfo->weaknessVs[MC_PRIEST] += basePenaltyValue;

	// Strength vs priests
	if (!unitInfo->isMelee && (unitSpeed >= 1) && (unitInfo->range > 6)) {
		unitInfo->strengthVs[MC_PRIEST] += 10; // Range units are slightly better vs priests than melee
	} else {
		if (unitInfo->range >= 10) { // siege weapons, actually (archers have 7 max, we don't count range upgrades here)
			unitInfo->strengthVs[MC_PRIEST] += 20;
		}
	}
	if (unitInfo->isMelee && (unitSpeed >= 1.0f)) {
		unitInfo->strengthVs[MC_PRIEST] += 15;
		if (unitInfo->isMelee && (unitSpeed >= 1.5f)) {
			unitInfo->strengthVs[MC_PRIEST] += 20; // even more for fast units
		}
	} else {
		unitInfo->strengthVs[MC_PRIEST] -= 5;
	}
	if (unitInfo->displayedAttack >= 25) {
		unitInfo->strengthVs[MC_PRIEST] += 5; // Can "one-shot" priests (not upgraded)
		if (unitInfo->displayedAttack >= 50) {
			unitInfo->strengthVs[MC_PRIEST] += 10; // Can "one-shot" priests (upgraded). Total +10 (+ranged unit bonus, possibly)
		}
	}
	if (unitInfo->upgradedUnitDefLiving && unitInfo->upgradedUnitDefLiving->IsCheckSumValid()) {
		for (int i = 0; i < unitInfo->upgradedUnitDefLiving->attacksCount; i++) {
			if (unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CHARIOTS_ON_PRIESTS) {
				unitInfo->strengthVs[MC_PRIEST] += 50;
			}
		}
	}
	if (unitInfo->hitPoints > 80) {
		unitInfo->strengthVs[MC_PRIEST] -= 5;
		if (unitInfo->hitPoints > 120) {
			unitInfo->strengthVs[MC_PRIEST] -= 5;
			if (unitInfo->hitPoints > 180) {
				unitInfo->strengthVs[MC_PRIEST] -= 10;
			}
		}
	}
	if (unitInfo->upgradedUnitDefLiving && (unitInfo->upgradedUnitDefLiving->displayedArmor > 0)) {
		// in fact, few units have armors (not counting storage pit upgrades). So, fex units match this:
		if (unitInfo->upgradedUnitDefLiving->displayedArmor >= 3) {
			unitInfo->strengthVs[MC_PRIEST] -= 10;
		}
		if (unitInfo->upgradedUnitDefLiving->displayedArmor == 2) {
			unitInfo->strengthVs[MC_PRIEST] -= 5;
		}
	}

	switch (unitInfo->damageScore) {
	case 1:
		unitInfo->strengthVs[MC_PRIEST] -= 15;
	case 2:
		unitInfo->strengthVs[MC_PRIEST] -= 5;
	case 4:
		unitInfo->strengthVs[MC_PRIEST] += 10;
	default:
		break;
	}
	if (unitInfo->strengthVs[MC_PRIEST] > 100) { unitInfo->strengthVs[MC_PRIEST] = 100; }
}

// Compute score fields for towers in unitInfo object.
void StrategyBuilder::ComputeScoresVsTower(PotentialUnitInfo *unitInfo) {
	unitInfo->strengthVs[MC_TOWER] = 0;
	int proportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->availableRelatedResearchesCount + unitInfo->unavailableRelatedResearchesCount);
	switch (unitInfo->unitAIType) {
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon:
		unitInfo->strengthVs[MC_TOWER] += 60;
		if (!unitInfo->hasUnavailableUpgrade) {
			unitInfo->strengthVs[MC_TOWER] += 20;
		}
		if (proportion >= 80) {
			unitInfo->strengthVs[MC_TOWER] += 15;
		}
		break;
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher:
		unitInfo->strengthVs[MC_TOWER] += 5;
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest:
		unitInfo->strengthVs[MC_TOWER] += 1;
		break;
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger:
		unitInfo->strengthVs[MC_TOWER] += 30; // cool in tool/bronze, weak later
		break;
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPhalanx:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupHeavyFootSoldier: // Heroes only ?!
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider:
		unitInfo->strengthVs[MC_TOWER] += 20; // cumulate with below melee classes
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupFootSoldier:
		unitInfo->strengthVs[MC_TOWER] += 30; // cumulate with below melee classes
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupMountedSoldier:
		unitInfo->strengthVs[MC_TOWER] += 10; // cumulate with below melee classes
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot:
		unitInfo->strengthVs[MC_TOWER] += 30;
		break;
	default:
		unitInfo->strengthVs[MC_TOWER] += 10; // Default
		break;
	}
}


// Compute global scores using many criteria (unit cost, damage, civ bonus, super unit, (un/)available techs...), using a random part.
// Does not compare units/unit scores to each other at this point (all scores are independent)
void StrategyBuilder::ComputeGlobalScores() {
	int highestWeightedCost = 0;
	// Inits
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		if (!unitInfo->isBoat) {
			if (unitInfo->weightedCost > highestWeightedCost) { highestWeightedCost = unitInfo->weightedCost; }
		}
	}
	// First computation of "global" scores
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		if (unitInfo->isBoat) {
			continue;
		}
		unitInfo->globalScore = 0;
		if (unitInfo->damageScore == 2) {
			unitInfo->globalScore = 30;
		}
		if (unitInfo->damageScore == 3) {
			unitInfo->globalScore = 55;
		}
		if (unitInfo->damageScore == 4) {
			unitInfo->globalScore = 75;
		}
		if (unitInfo->isSuperUnit) {
			unitInfo->globalScore = 90;
		}
		if (unitInfo->hasCivBonus) {
			unitInfo->globalScore += 25; // note: we can go > 100 at this point, not an issue
		}
		if (unitInfo->hasUnavailableUpgrade) {
			unitInfo->globalScore = (unitInfo->globalScore * 85) / 100; // -15% if unit upgrades are missing
		}
		int numberOfAvailableUpgrades = unitInfo->upgradesUnitDefId.size();
		// TODO: example: assyrian cat is super unit (all unit upgrades) but missing 2 techs/5 => not selected
		int proportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->availableRelatedResearchesCount + unitInfo->unavailableRelatedResearchesCount);
		// Reduce proportion impact:
		int missingTechProportionWeight = 70; // Importance (a % value) of missing techs.
		if (unitInfo->isSuperUnit) {
			missingTechProportionWeight = 50;
		}
		int invertedProportion = (100 - proportion);
		invertedProportion = (invertedProportion * (100 - missingTechProportionWeight)) / 100; // *50% or 30% (so, 50% or 70% on proportion itself)
		proportion = 100 - invertedProportion;
		unitInfo->globalScore = (unitInfo->globalScore * proportion) / 100; // Apply directly proportion (a % of available techs) to score
		if (!unitInfo->costsGold) {
			unitInfo->globalScore = (unitInfo->globalScore * 110) / 100; // +10% if no gold is required
		}
		int diff = 3; // best availability (if no age specified, unit is available from stone age)
		if ((unitInfo->ageResearchId >= CST_RSID_STONE_AGE) && (unitInfo->ageResearchId <= CST_RSID_IRON_AGE)) {
			diff = CST_RSID_IRON_AGE - unitInfo->ageResearchId; // 0-3
		}
		unitInfo->globalScore = (unitInfo->globalScore * (100 + diff * 2)) / 100; // Add up to 6% bonus for early availability

		// Adjust (a bit) according to costs: cheaper units are less strong (lower score), this is a bit unfair. Ex legion underestimated, heavy cats overestimated.
		int costProportion = (unitInfo->weightedCost * 100) / highestWeightedCost; // 0 to 100 value representing 1 unit cost.
		costProportion = costProportion / 10; // we limit "cost" impact to maximum 10% of global score. low value=low cost=better score
		costProportion = 100 - costProportion; // Get values in 90%-100% interval, where 100% is the better score. Very approximate due to int type, but the whole thing IS approximate.
		unitInfo->globalScore = unitInfo->globalScore * costProportion / 100;

		if (unitInfo->globalScore < 0) { unitInfo->globalScore = 0; }
		if (unitInfo->globalScore > 100) { unitInfo->globalScore = 100; }

		// Add a random part
		int random = randomizer.GetRandomValue_normal_moreFlat(0, randomPercentFactor * 2);
		random = random + 100 - randomPercentFactor; // in [100-randomImpact, 100+randomImpact] (used with a multiplication, it gives a resulting randomImpact% adjustment + or -)
		unitInfo->globalScore = (unitInfo->globalScore * random) / 100;

		if (unitInfo->globalScore > 100) { unitInfo->globalScore = 100; }
	}
}


// Get unit info's score for costs, according to currently (actually) selected units.
int StrategyBuilder::GetCostScoreRegardingCurrentSelection(PotentialUnitInfo *unitInfo) {
	assert(!unitInfo->isSelected);
	bool noData = true;
	for (int i = 0; i < 4; i++) {
		if (this->totalTrainUnitCosts[i] != 0) { noData = false; }
	}
	if (noData) {
		return 50; // same value for all units when no unit is actually selected
	}

	const int USED_RESOURCE_TYPES = 3;
	int cumulatedWeightedCosts[4];
	int unitWeightedCosts[4];
	int totalCumulatedCost = 0;
	int totalUnitCost = 0;
	for (int i = 0; i < 4; i++) {
		cumulatedWeightedCosts[i] = GetWeightedCostValue(this->totalTrainUnitCosts[i], (RESOURCE_TYPES)i);
		unitWeightedCosts[i] = GetWeightedCostValue(unitInfo->trainCosts[i], (RESOURCE_TYPES)i);
		if (i != RESOURCE_TYPES::CST_RES_ORDER_STONE) {
			// Ignore stone costs as almost no units use it (only slinger in standard game): would lead to wrong calculations
			totalCumulatedCost += cumulatedWeightedCosts[i];
			totalUnitCost += unitWeightedCosts[i];
		}
	}
	for (int i = 0; i < 4; i++) {
		if (i != RESOURCE_TYPES::CST_RES_ORDER_STONE) {
			cumulatedWeightedCosts[i] = (cumulatedWeightedCosts[i] * 100) / totalCumulatedCost;
			unitWeightedCosts[i] = (unitWeightedCosts[i] * 100) / totalUnitCost;
		}
	}
	// Now cumulatedWeightedCosts and unitWeightedCosts are percentage (sum for all resources ~=100)
	int diffToMedian = 0; // difference (%) to median for cumulated costs (unit+current total) for each resource type
	for (int i = 0; i < 4; i++) {
		if (i != RESOURCE_TYPES::CST_RES_ORDER_STONE) {
			const int medianValue = 100 / USED_RESOURCE_TYPES;
			int cumulated2Values = cumulatedWeightedCosts[i] + unitWeightedCosts[i]; // 0-200
			int diffToMedianForThisResource = (cumulated2Values - (medianValue * 2)) / 2; // Result is 0-100
			if (diffToMedianForThisResource < 0) { diffToMedianForThisResource = -diffToMedianForThisResource; } // Always positive (absolute value)
			diffToMedian += diffToMedianForThisResource;
		}
	}
	int highestPossibleDiffToMedian = (min(100 - (100 / USED_RESOURCE_TYPES), (100 / USED_RESOURCE_TYPES)) * 4) / 3; // Worst case: 100/0/0 makes 67+33+33 diffs

	diffToMedian = diffToMedian / USED_RESOURCE_TYPES; // Get a % value. Higher value = bad score (bad resource repartition)
	int adjustedPercentage = (diffToMedian * 100) / highestPossibleDiffToMedian;
	int scoreForResourceRepartition = 100 - adjustedPercentage; // High value = bonus

	if (scoreForResourceRepartition > 100) { scoreForResourceRepartition = 100; }
	if (scoreForResourceRepartition < 0) { scoreForResourceRepartition = 0; }
	return scoreForResourceRepartition;
}


// Recompute unit info bonuses that depend on comparison with other units
// Does not recompute unit strengths !
// upgradedUnit: if true, consider upgraded unit. if false, consider base unit
#pragma message("Remove this overload")
void StrategyBuilder::RecomputeComparisonBonuses(std::list<PotentialUnitInfo*> selectedUnits, bool waterUnit, bool upgradedUnit) {
	int totalStrengthVsWithSelected[MC_COUNT];
	int totalStrengthVsWithoutSelected[MC_COUNT];
	int numberWithoutSelected = 0; // unit count that are NOT selected
	int tempStrengthVs[MC_COUNT];
	for (int i = 0; i < MC_COUNT; i++) { totalStrengthVsWithSelected[i] = 0; totalStrengthVsWithoutSelected[i] = 0; tempStrengthVs[i] = 0; }
	// Compute total strength per category
	for each (PotentialUnitInfo *unitInfo in selectedUnits)
	{
		if (unitInfo->isBoat == waterUnit) {
			totalStrengthVsWithSelected[MC_PRIEST] += unitInfo->strengthVs[MC_PRIEST];
			totalStrengthVsWithSelected[MC_FAST_RANGED] += unitInfo->strengthVs[MC_FAST_RANGED];
			totalStrengthVsWithSelected[MC_TOWER] += unitInfo->strengthVs[MC_TOWER];
			totalStrengthVsWithSelected[MC_MELEE] += unitInfo->strengthVs[MC_MELEE];
			totalStrengthVsWithSelected[MC_SIEGE] += unitInfo->strengthVs[MC_SIEGE];
			if (!unitInfo->isSelected) {
				numberWithoutSelected++;
				totalStrengthVsWithoutSelected[MC_PRIEST] += unitInfo->strengthVs[MC_PRIEST];
				totalStrengthVsWithoutSelected[MC_FAST_RANGED] += unitInfo->strengthVs[MC_FAST_RANGED];
				totalStrengthVsWithoutSelected[MC_TOWER] += unitInfo->strengthVs[MC_TOWER];
				totalStrengthVsWithoutSelected[MC_MELEE] += unitInfo->strengthVs[MC_MELEE];
				totalStrengthVsWithoutSelected[MC_SIEGE] += unitInfo->strengthVs[MC_SIEGE];
			}
		}
	}
	for (int i = 0; i < MC_COUNT; i++) { tempStrengthVs[i] = totalStrengthVsWithSelected[i]; }

	// If some category has few points, try to give priority to the unit that has the most points there...
	int categoriesWeaknessOrder[MC_COUNT];
	for (int i = 0; i < MC_COUNT; i++) { categoriesWeaknessOrder[i] = MC_NONE; }
	// Sort strength by increasing value
	for (int i = 0; i < MC_COUNT; i++) {
		int lowestValue = 999999;
		int currentLowestCategory = i;
		for (int j = 0; j < MC_COUNT; j++) {
			if ((tempStrengthVs[j] < tempStrengthVs[i])) {
				lowestValue = tempStrengthVs[j];
				currentLowestCategory = j;
			}
		}
		// Found the lowest value among [i;MC_COUNT-1] categories
		categoriesWeaknessOrder[i] = currentLowestCategory; // Set i'th element and never touch it again.
		tempStrengthVs[currentLowestCategory] = 999999; // Force not using it anymore
	}

	// Give bonus to units that are good against units that bother me (I don't have many units good against such units)
	int bonus = 100;
	int weaknessIndex = 0;
	for each (PotentialUnitInfo *unitInfo in selectedUnits)
	{
		if ((unitInfo->isBoat == waterUnit) && (!unitInfo->isSelected)) {
			if (categoriesWeaknessOrder[weaknessIndex] == MC_NONE) { weaknessIndex++; } // should not happen as NONE is -1 value.
			int currentCategory = categoriesWeaknessOrder[weaknessIndex];
			//int meanStrengthValue = (totalStrengthVsWithSelected[currentCategory] / selectedUnits.size()); // medium strength Vs this category among all selected units from provided list
			int meanStrengthValue = (totalStrengthVsWithoutSelected[currentCategory] / numberWithoutSelected); // medium strength Vs this category among all selected units from provided list
			if (unitInfo->strengthVs[currentCategory] > meanStrengthValue) {
				// This unit has a quite good strength against a military category that troubles me (I don't have many good units against it)
				unitInfo->bonusForRareStrength = bonus;
				bonus -= 30;
				weaknessIndex++; // same thing on next critical weakness
			}
			if (bonus <= 0) { break; }
		}
	}

	// Bonus about costs repartition
	for each (PotentialUnitInfo *unitInfo in selectedUnits)
	{
		if (!unitInfo->isSelected) {
			unitInfo->bonusForUsedResourceTypes = this->GetCostScoreRegardingCurrentSelection(unitInfo);
		}
	}
}

// Recompute unit info bonuses that depend on comparison with other units: rare strengths, balanced cost
// Does not recompute unit strengths (but computes it using the correct strength values)
// upgradedUnit: if true, consider upgraded unit. if false, consider base unit
#pragma message("No very relevant for boats, do a specific method ?")
void StrategyBuilder::RecomputeComparisonBonuses(std::list<PotentialUnitInfo*> selectedUnits, bool waterUnit, bool upgradedUnit, short int maxAgeResearchId) {
	int totalStrengthVsWithSelected[MC_COUNT];
	int totalStrengthVsWithoutSelected[MC_COUNT];
	int numberWithoutSelected = 0; // unit count that are NOT selected
	int tempStrengthVs[MC_COUNT];
	for (int i = 0; i < MC_COUNT; i++) { totalStrengthVsWithSelected[i] = 0; totalStrengthVsWithoutSelected[i] = 0; tempStrengthVs[i] = 0; }
	// Compute total strength per category. Ignore units not available at given age
	for each (PotentialUnitInfo *unitInfo in selectedUnits)
	{
		if ((unitInfo->isBoat == waterUnit) && (unitInfo->ageResearchId <= maxAgeResearchId)) {
			totalStrengthVsWithSelected[MC_PRIEST] += unitInfo->strengthVs[MC_PRIEST];
			totalStrengthVsWithSelected[MC_FAST_RANGED] += unitInfo->strengthVs[MC_FAST_RANGED];
			totalStrengthVsWithSelected[MC_TOWER] += unitInfo->strengthVs[MC_TOWER];
			totalStrengthVsWithSelected[MC_MELEE] += unitInfo->strengthVs[MC_MELEE];
			totalStrengthVsWithSelected[MC_SIEGE] += unitInfo->strengthVs[MC_SIEGE];
			if (!unitInfo->isSelected) {
				numberWithoutSelected++;
				totalStrengthVsWithoutSelected[MC_PRIEST] += unitInfo->strengthVs[MC_PRIEST];
				totalStrengthVsWithoutSelected[MC_FAST_RANGED] += unitInfo->strengthVs[MC_FAST_RANGED];
				totalStrengthVsWithoutSelected[MC_TOWER] += unitInfo->strengthVs[MC_TOWER];
				totalStrengthVsWithoutSelected[MC_MELEE] += unitInfo->strengthVs[MC_MELEE];
				totalStrengthVsWithoutSelected[MC_SIEGE] += unitInfo->strengthVs[MC_SIEGE];
			}
		}
	}
	for (int i = 0; i < MC_COUNT; i++) { tempStrengthVs[i] = totalStrengthVsWithSelected[i]; }

	// If some category has few points, try to give priority to the unit that has the most points there...
	int categoriesWeaknessOrder[MC_COUNT];
	for (int i = 0; i < MC_COUNT; i++) { categoriesWeaknessOrder[i] = MC_NONE; }
	// Sort strength by increasing value
	for (int i = 0; i < MC_COUNT; i++) {
		int lowestValue = 999999;
		int currentLowestCategory = i;
		for (int j = 0; j < MC_COUNT; j++) {
			if ((tempStrengthVs[j] < tempStrengthVs[i])) {
				lowestValue = tempStrengthVs[j];
				currentLowestCategory = j;
			}
		}
		// Found the lowest value among [i;MC_COUNT-1] categories
		categoriesWeaknessOrder[i] = currentLowestCategory; // Set i'th element and never touch it again.
		tempStrengthVs[currentLowestCategory] = 999999; // Force not using it anymore
	}

	// Give bonus to units that are good against units that bother me (I don't have many good units against such units)
	int bonus = 100;
	int weaknessIndex = 0;
	for each (PotentialUnitInfo *unitInfo in selectedUnits)
	{
		if ((unitInfo->isBoat == waterUnit) && (unitInfo->ageResearchId <= maxAgeResearchId) && (!unitInfo->isSelected)) {
			if (categoriesWeaknessOrder[weaknessIndex] == MC_NONE) { weaknessIndex++; } // should not happen as NONE is -1 value.
			int currentCategory = categoriesWeaknessOrder[weaknessIndex];
			//int meanStrengthValue = (totalStrengthVsWithSelected[currentCategory] / selectedUnits.size()); // medium strength Vs this category among all selected units from provided list
			int meanStrengthValue = (totalStrengthVsWithoutSelected[currentCategory] / numberWithoutSelected); // medium strength Vs this category among all selected units from provided list
			if (unitInfo->strengthVs[currentCategory] > meanStrengthValue) {
				// This unit has a quite good strength against a military category that troubles me (I don't have many good units against it)
				unitInfo->bonusForRareStrength = bonus;
				bonus -= 30;
				weaknessIndex++; // same thing on next critical weakness
			}
			if (bonus <= 0) { break; }
		}
	}

	// Bonus about costs repartition
	for each (PotentialUnitInfo *unitInfo in selectedUnits)
	{
		if ((!unitInfo->isSelected) && (unitInfo->ageResearchId <= maxAgeResearchId)) {
			unitInfo->bonusForUsedResourceTypes = this->GetCostScoreRegardingCurrentSelection(unitInfo);
		}
	}
}


// Make the selection of units to add to strategy based on a pre-selection list.
// waterUnits : true=deal with water units only, false=deal with land units only
void StrategyBuilder::SelectStrategyUnitsFromPreSelection(std::list<PotentialUnitInfo*> preSelectedUnits, bool waterUnits) {
	int currentUnitCount = this->actuallySelectedUnits.size();

	bool statisfiedWithCurrentSelection = false;
	int currentTotalStrengths[MC_COUNT];
	int currentCumulatedStrength;
	int categoriesCountWithLowStrength;
	MILITARY_CATEGORY currentWorstStrengthIndex = MC_NONE; // Category that has the lowest strength value

	while (!statisfiedWithCurrentSelection && (currentUnitCount < 3)) {
		currentCumulatedStrength = 0;
		categoriesCountWithLowStrength = 0;
		this->RecomputeComparisonBonuses(preSelectedUnits, waterUnits, true);

		long int bestScore = 0;
		PotentialUnitInfo *bestUnit = NULL;
		const long int rareStrengthImpact = 30;
		const long int resourceRepartitionImpact = 15;
		long int newRandomImpact = (randomPercentFactor * 100) / 25; // 25% of randomPercentFactor setting
		for each (PotentialUnitInfo *unitInfo in preSelectedUnits)
		{
			if ((!unitInfo->isSelected) && (unitInfo->isBoat == waterUnits)) {
				long int currentScore = unitInfo->globalScore;
				long int strengthFactor = ((unitInfo->bonusForRareStrength * rareStrengthImpact) / 100) + 100; // Get a value between 100% and 130% (if rareStrengthImpact=30) => for multiplication
				long int resourceFactor = ((unitInfo->bonusForUsedResourceTypes * resourceRepartitionImpact) / 100) + 100; // Get a value between 100% and 130% (if rareStrengthImpact=30) => for multiplication
				long int newRandomFactor = ((randomizer.GetRandomPercentageValue() * newRandomImpact) / 100) + 100; // Full random but low impact => 100%-108% multiplication
				// Avoid redundant units (eg. phalanx+legion)
				for each (PotentialUnitInfo *refUnitInfo in this->actuallySelectedUnits)
				{
					if (AreSimilarClasses(refUnitInfo->unitAIType, unitInfo->unitAIType)) {
						// Default: Big penalty (-25%) when a similar unit has already been added to strategy
						int percentageToApply = 75;
						if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon) {
							// Lighter penalty for siege weapons because they are quite compatible (can be combined efficiently)
							percentageToApply = 90;
						}
						if (unitInfo->hasCivBonus) {
							percentageToApply += 5; // Reduce (a bit) penalty for units that have some civ bonus
						}
						if (unitInfo->hasSpecificAttackBonus) {
							// Does attack bonus concern a category against which I need more strength ?
#pragma message("TODO attack bonus: compare to selected units abilities / weaknesses ?")
							percentageToApply += 5; // Reduce (a bit) penalty for units that have some special bonus
						}
						if (percentageToApply > 100) { percentageToApply = 100; }
						currentScore = (currentScore * percentageToApply) / 100;
					}
				}
				currentScore = currentScore * strengthFactor * resourceFactor * newRandomFactor / 1000000;

				if (bestScore < currentScore) {
					bestScore = currentScore;
					bestUnit = unitInfo;
				}
			}
		}
		if (bestUnit != NULL) {
			currentUnitCount = this->AddUnitIntoToSelection(bestUnit);
		} else {
			// What can we do here ?
			break;
		}

		// Decide if we want to continue adding units in strategy. Most standard strategy have 2, 3 or maybe 4 units (not counting boats)
		int currentWorstStrengthValue = 9999999;
		currentWorstStrengthIndex = (MILITARY_CATEGORY)0;
		for (int i = 0; i < MC_COUNT; i++) {
			currentTotalStrengths[i] = 0;
			for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
			{
				currentTotalStrengths[i] += unitInfo->strengthVs[i]; // total strength for this military category
			}
			currentCumulatedStrength += currentTotalStrengths[i]; // the very total strength amount
			if (currentTotalStrengths[i] < 30) {
				categoriesCountWithLowStrength++;
			}
			if (currentTotalStrengths[i] < currentWorstStrengthValue) {
				currentWorstStrengthIndex = (MILITARY_CATEGORY)i;
				currentWorstStrengthValue = currentTotalStrengths[i];
			}
		}

		// Test weaknesses and check there is no "too high" weaknesses (ex: elephant+cavalry is enough to break loop here)

		const int minStrengthValueOnAllCategoriesToStop = 120; // Takes at least 2 units to be satisfied, probably more
		if ((categoriesCountWithLowStrength < 1) || (this->actuallySelectedUnits.size() > 4) ||
			(currentCumulatedStrength > (minStrengthValueOnAllCategoriesToStop * MC_COUNT))) { // Mean-strength for all categories > minStrengthValueOnAllCategoriesToStop
			statisfiedWithCurrentSelection = true;
		}
	}

	// add "bonus" units to fill weaknesses, ex stone thrower vs towers, camel vs cavalry...
	MILITARY_CATEGORY firstAddedCategory = MC_NONE;
	for (int step = 1; step <= 2; step++) {
		int mediumStrength = currentCumulatedStrength / MC_COUNT;
		int worstStrength = currentTotalStrengths[currentWorstStrengthIndex];
		int proportion = (worstStrength * 100) / mediumStrength;
		if (!waterUnits) { // Not implemented for boats. Actually, not relevant.
			if ((proportion < 50) && (this->selectedLandUnitsCount < 4)) {
				this->AddOptionalUnitAgainstWeakness(currentWorstStrengthIndex, waterUnits);
				//this->RecomputeComparisonBonuses(...)
			}
		}
		// Compute next (different) weakness category
		int currentWorstStrengthValue = 9999999;
		currentWorstStrengthIndex = (MILITARY_CATEGORY)0;
		for (int i = 0; i < MC_COUNT; i++) {
			if ((i != firstAddedCategory) && (currentTotalStrengths[i] < currentWorstStrengthValue)) {
				currentWorstStrengthIndex = (MILITARY_CATEGORY)i;
				currentWorstStrengthValue = currentTotalStrengths[i];
			}
		}
	}
}


// Select which units are to be added in strategy, based on potentialUnitsList
// Requires that this->potentialUnitsList has already been filled
void StrategyBuilder::SelectStrategyUnitsForLandOrWater(bool waterUnits) {
	std::list<PotentialUnitInfo*> preSelectedUnits; // PotentialUnitInfo pointers are "shortcuts", do not free them here

	// First selection based on "global" scores = Fill preSelectedUnits
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		if (unitInfo->isBoat == waterUnits) {
			if (unitInfo->globalScore >= 45) {
				preSelectedUnits.push_back(unitInfo);
			}
		}
	}

	int totalStrengthVs[MC_COUNT];
	for (int i = 0; i < MC_COUNT; i++) { totalStrengthVs[i] = 0; }
	for each (PotentialUnitInfo *unitInfo in preSelectedUnits)
	{
		if (unitInfo->isBoat == waterUnits) {
			totalStrengthVs[MC_PRIEST] += unitInfo->strengthVs[MC_PRIEST];
			totalStrengthVs[MC_FAST_RANGED] += unitInfo->strengthVs[MC_FAST_RANGED];
			totalStrengthVs[MC_TOWER] += unitInfo->strengthVs[MC_TOWER];
			totalStrengthVs[MC_MELEE] += unitInfo->strengthVs[MC_MELEE];
			totalStrengthVs[MC_SIEGE] += unitInfo->strengthVs[MC_SIEGE];
		}
	}


	int bestScore = 0;
	PotentialUnitInfo *bestUnit = NULL;
	// If no unit is already selected, do not use the cost criteria. Just pick best unit according to global score.
	if (this->actuallySelectedUnits.size() == 0) {
		for each (PotentialUnitInfo *unitInfo in preSelectedUnits)
		{
			if (!unitInfo->isSelected && (unitInfo->isBoat == waterUnits) && (bestScore < unitInfo->globalScore)) {
				bestScore = unitInfo->globalScore;
				bestUnit = unitInfo;
			}
		}
		if (bestUnit != NULL) {
			this->AddUnitIntoToSelection(bestUnit);
		} else {
			assert(false && "Could not select any military unit for strategy");
			traceMessageHandler.WriteMessage("ERROR: Could not select any military unit for strategy");
			return;
		}
	}

	this->SelectStrategyUnitsFromPreSelection(preSelectedUnits, waterUnits);

	// Consider adding units with special bonus like camel/chariot if it helps for a specific need
#pragma message("TODO early ages")
	// Take care of early ages : add archers, axemen, slingers or scout (according to already available techs). TODO later (not in this method) ?
}

// Search for a unit that can be added to strategy as "optional" (without full upgrades) to fight a detected weakness.
// waterUnits: if true, manage water units only. If false, manage land units only
void StrategyBuilder::AddOptionalUnitAgainstWeakness(MILITARY_CATEGORY weaknessCategory, bool waterUnits) {
	int bestScore = 0;
	PotentialUnitInfo *bestUnit = NULL;

	// Recompute strengths, for *base* units (not counting upgrades here)
	this->RecomputeComparisonBonuses(this->potentialUnitsList, waterUnits, false);

	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		// Ignore already selected units (and those NOT matching the land/water parameter)
		if (!unitInfo->isSelected && (unitInfo->isBoat == waterUnits)) {

			// Recompute costs bonus according to current selected units
			unitInfo->bonusForUsedResourceTypes = this->GetCostScoreRegardingCurrentSelection(unitInfo);
			int tmpScore = unitInfo->strengthVs[weaknessCategory];
			// Some hardcoded (but still generic) rules
			if ((weaknessCategory == MILITARY_CATEGORY::MC_TOWER) && (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon)) {
				if (unitInfo->upgradedUnitDefLiving->blastRadius > 0) {
					tmpScore = (tmpScore * 115) / 100; // catapult-like siege
				} else {
					tmpScore = (tmpScore * 105) / 100; // ballista-like siege vs towers
				}
			}
#pragma message("Should recompute scores using BASE units (without costly upgrades)")
			// Important: units may have a low score (<10) and % couldnt apply well: check this
			// TODO take care of relative cost ! Create a dedicated method GetRelativeCostScore with existing code above (in another method)
			// Units available without cost (or low research cost)
			bool lowCostActivation = false;
			if (unitInfo->enabledByResearchId == -1) {
				lowCostActivation = true;
			} else {
				ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef = GetResearchDef(this->player, unitInfo->enabledByResearchId);
				if (resDef) {
					int totalCost = 0;
					if (resDef->costUsed1) { totalCost += resDef->costAmount1; }
					if (resDef->costUsed2) { totalCost += resDef->costAmount2; }
					if (resDef->costUsed3) { totalCost += resDef->costAmount3; }
					if (totalCost <= 180) {
						lowCostActivation = true;
					}
				}
			}
			if (lowCostActivation) {
				tmpScore = (tmpScore * 110) / 100; // no or low research cost = +10%
			}
			if (unitInfo->isSuperUnit) {
// Expensive (upgraded) units are less relevant here and won't be researched for optional unit. 
// Penalty also stands for the fact that, if we select it, unit will be weaker than its computer score (because of missing upgrades)
tmpScore = (tmpScore * 90) / 100;
			}
			if (!unitInfo->costsGold) {
				tmpScore = (tmpScore * 105) / 100; // gold-free units are cool complement units
			}
			if ((unitInfo->ageResearchId == -1) || (unitInfo->ageResearchId <= CST_RSID_BRONZE_AGE)) {
				tmpScore = (tmpScore * 105) / 100; // early availability: +5%
			}
			if (tmpScore > bestScore) {
				bestScore = tmpScore;
				bestUnit = unitInfo;
			}
		}
	}
	if (bestUnit != NULL) {
		this->AddUnitIntoToSelection(bestUnit);
		bestUnit->isOptionalUnit = true;
		bestUnit->earlyAgeUnit = false;
	}
}

// Select which units are to be added in strategy, based on potentialUnitsList
void StrategyBuilder::SelectStrategyUnits() {
	// Compute individual global scores (no comparison between units). Done once and for all
	this->ComputeGlobalScores();

	if (false) { // if water map : TODO
		// Select boats with 1st priority
		this->SelectStrategyUnitsForLandOrWater(true);
	}
	// Then land units : according to need for boats (or not), wood-costly units might be preferred or not.
	this->SelectStrategyUnitsForLandOrWater(false);

	// Debug log
#ifdef _DEBUG
	traceMessageHandler.WriteMessage("Strategy decision:");
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits) {
		std::string msg = "Unit id=";
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *unit = NULL;
		if (unitInfo->isOptionalUnit) {
			msg += std::to_string(unitInfo->unitDefId);
			unit = unitInfo->baseUnitDefLiving;
		} else {
			msg += std::to_string(unitInfo->strongestUpgradeUnitDefId);
			unit = unitInfo->upgradedUnitDefLiving;
		}
		msg += std::string(" (") + GetLanguageDllText(unit->languageDLLID_Name) + std::string(")");
		if (unitInfo->isOptionalUnit) {
			msg += " : OPTIONAL";
		}
		msg += " - score=";
		msg += std::to_string(unitInfo->globalScore);
		traceMessageHandler.WriteMessageNoNotification(msg);
	}
#endif
}

// age = age to take care (ignore units that are only available in later ages)
// hasAlreadyUnits = true if provided age already has some military units
// Returns number of added units (0/1)
int StrategyBuilder::AddOneMilitaryUnitForEarlyAge(short int age, bool hasAlreadyUnits) {
	// Recalculate bonus for rare strength taking into account specific limitation on age.
	this->RecomputeComparisonBonuses(this->potentialUnitsList, false, false, age);
	// Compute bonusForTechsSimilarity
	std::set<AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES> selectedUnitsClassesAfterThisAge; // list of "main units" AI types, without duplicate
	std::set<AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES> selectedUnitsClassesInThisAge; // list of "main units" AI types, without duplicate
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits) {
		if (unitInfo->ageResearchId > age) {
			selectedUnitsClassesAfterThisAge.insert(unitInfo->unitAIType);
		} else {
			selectedUnitsClassesInThisAge.insert(unitInfo->unitAIType);
		}
	}
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList) {
		if (!unitInfo->isSelected && !unitInfo->isBoat && (unitInfo->ageResearchId <= age)) {
			unitInfo->bonusForTechsSimilarity = 0; // reset
			for each (AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES refAIType in selectedUnitsClassesAfterThisAge)
			{
				if (AreSimilarClasses(unitInfo->unitAIType, refAIType)) {
					unitInfo->bonusForTechsSimilarity += 30;
				}
				bool attack, defense, hitPoints;
				ShareCommonTechs(unitInfo->unitAIType, refAIType, attack, defense, hitPoints);
				int tmpScore = (attack ? 1 : 0) + (defense ? 1 : 0) + (hitPoints ? 1 : 0);
				if (tmpScore > 2) { tmpScore = 2; }
				unitInfo->bonusForTechsSimilarity += tmpScore * 15;
			}
			for each (AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES refAIType in selectedUnitsClassesInThisAge)
			{
				// Do not pick units with similar classes with existing ones : here simimlarity is a (big) flaw
				if (AreSimilarClasses(unitInfo->unitAIType, refAIType)) {
					unitInfo->bonusForTechsSimilarity = -100; // Make we don't pick similar units for early ages. No use
				}
			}
			// Bonus for no-research units (no cost to have it available)
			ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef = player->GetResearchDef(unitInfo->enabledByResearchId);
			bool nullCost = (resDef == NULL);
			if (resDef) {
				if (((resDef->costUsed1 == 0) || (resDef->costAmount1 == 0)) &&
					((resDef->costUsed2 == 0) || (resDef->costAmount2 == 0)) &&
					((resDef->costUsed3 == 0) || (resDef->costAmount3 == 0))) {
					nullCost = true;
				}
			}
			if (!nullCost) {
				unitInfo->bonusForTechsSimilarity = (unitInfo->bonusForTechsSimilarity * 85) / 100; // -15% if research has a cost
			}
			// civ bonus...
			if (unitInfo->hasCivBonus) {
				unitInfo->bonusForTechsSimilarity = (unitInfo->bonusForTechsSimilarity * 115) / 100; // +15% for civ bonus
			}
			if (unitInfo->bonusForTechsSimilarity > 100) { unitInfo->bonusForTechsSimilarity = 100; }
			if (unitInfo->bonusForTechsSimilarity < 0) { unitInfo->bonusForTechsSimilarity = 0; }
		}
	}

	if (!hasAlreadyUnits) {
		// Choose a unit that has both combination of: global score, similarity with main units
		for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList) {
			if (!unitInfo->isBoat && !unitInfo->isSelected && (unitInfo->ageResearchId <= age)) {
				unitInfo->scoreForEarlyAge = ((float)unitInfo->globalScore) * (100 + unitInfo->bonusForTechsSimilarity) / 100;
			}
		}
	} else {
		// Choose a unit that has both combination of: rare strength bonus for this age, similarity with main units
		for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList) {
			if (!unitInfo->isBoat && !unitInfo->isSelected && (unitInfo->ageResearchId <= age)) {
				unitInfo->scoreForEarlyAge = ((float)unitInfo->bonusForRareStrength) * (100 + unitInfo->bonusForTechsSimilarity) / 100;
			}
		}
	}
	// Best unit selection
	float bestScore = 0;
	PotentialUnitInfo *bestUnit = NULL;
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList) {
		if (!unitInfo->isBoat && !unitInfo->isSelected && (unitInfo->ageResearchId <= age)) {
			if (unitInfo->scoreForEarlyAge > bestScore) {
				bestScore = unitInfo->scoreForEarlyAge;
				bestUnit = unitInfo;
			}
		}
	}
	if (bestUnit != NULL) {
		bestUnit->isOptionalUnit = true;
		bestUnit->earlyAgeUnit = true;
		this->AddUnitIntoToSelection(bestUnit);
		this->log += "Early age unit selection: ";
		this->log += bestUnit->unitName;
		this->log += newline;
		int addedStuff = this->CollectResearchInfoForUnit(bestUnit->unitDefId, false); // Add requirements for this unit (only requirements)
		return 1; // return unitInfo instead ?
	}
	return 0;
}

// Add some military to selection, to fille gaps in strategy in early ages (if any)
void StrategyBuilder::AddMilitaryUnitsForEarlyAges() {
	int strengths[4][MC_COUNT]; // First index=age 0-3
	// Note: ignore stone age, priority is to go Tool (and not much military at this age)
	for (int currentAge = CST_RSID_BRONZE_AGE; currentAge >= CST_RSID_TOOL_AGE; currentAge--) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < MC_COUNT; j++) {
				strengths[i][j] = 0;
			}
		}
		// For land units only. Get total strength at each age. A unit enabled in age 'x' is available in all next ages.
		for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
		{
			if (!unitInfo->isBoat) {
				assert(unitInfo->isSelected);
				short int ageIndex = unitInfo->ageResearchId - CST_RSID_STONE_AGE;
				if (ageIndex < 0) {
					ageIndex = 0;
				}
				for (int age = ageIndex; age < 4; age++) {
					for (int i = 0; i < MC_COUNT; i++) {
						strengths[age][i] += unitInfo->strengthVs[i];
					}
				}
			}
		}
		if (currentAge == CST_RSID_BRONZE_AGE) { // log only once
			for (int age = 0; age < 4; age++) {
				this->log += "Strengths [";
				this->log += std::to_string(age);
				this->log += "] = ";
				for (int i = 0; i < MC_COUNT; i++) {
					this->log += std::to_string(strengths[age][i]);
					this->log += ", ";
				}
				this->log += newline;
			}
		}

		int totalStrengthForCurrentAge = 0;
		for (int i = 0; i < MC_COUNT; i++) {
			totalStrengthForCurrentAge += strengths[currentAge][i];
		}
		this->AddOneMilitaryUnitForEarlyAge(currentAge, (totalStrengthForCurrentAge >= 10));
		// Try to add a second unit when there were none at beginning.
		if (totalStrengthForCurrentAge < 10) {
			this->AddOneMilitaryUnitForEarlyAge(currentAge, true);
		}
	}
}

// Selects optional researches to add to strategy
// All villagers and military units must have already been added to strategy.
void StrategyBuilder::ChooseOptionalResearches() {

	std::list<PotentialUnitInfo*> unitsThatNeedMoreAttack;
	// Special: selected melee units with low attack (like scouts, clubmen)
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
	{
		if (unitInfo->isMelee && (unitInfo->baseUnitDefLiving->displayedAttack <= 4)) {
			// Set force use (first) attack bonus research OR use upgrade (like axe)
			unitsThatNeedMoreAttack.push_back(unitInfo);
		}
	}
	// First check in researches that are ALREADY validated if some already improves this unit...
	std::set<PotentialResearchInfo*> moreAttackResearches; // researches improving early-age units that ARE available at the proper age.
	// Loop on all known researches (validated AND optional) to collect info on the ones that might be interesting here
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		resInfo->unitInstanceScoreForOptionalResearch = 0;
		if (resInfo->researchDef && (resInfo->researchDef->researchLocation >= 0)) { // small optim: exclude shadow researches
			for each (short int impactedUnitDefId in resInfo->impactedUnitDefIds)
			{
				for each (PotentialUnitInfo *unitInfo in unitsThatNeedMoreAttack) {
					resInfo->unitInstanceScoreForOptionalResearch += (int)unitInfo->addedCount;
					// Ignore stone age : no research. Moreover, it makes some criteria fail (clubmen's age would be considered anterior to any research's age)
					short int unitInfoAge = unitInfo->ageResearchId;
					if (unitInfoAge < CST_RSID_TOOL_AGE) { unitInfoAge = CST_RSID_TOOL_AGE; }
					if ((impactedUnitDefId == unitInfo->unitDefId) &&
						(resInfo->requiredAge <= unitInfoAge) && // Filter on "available in unit's age"
						(resInfo->totalCosts <= 130)) { // Important: filter on cheap researches
						bool relevantResearch = false;
						for (int i = 0; ((i < resInfo->techDef->effectCount) && !relevantResearch); i++) {
							// Modifier attack for some units, including "current" unit. (A research that modifies attack amount is always positive)
							if (resInfo->techDef->ptrEffects[i].HasValidEffect() &&
								resInfo->techDef->ptrEffects[i].IsAttributeModifier() &&
								(resInfo->techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_ATTACK) &&
								((resInfo->techDef->ptrEffects[i].effectUnit == unitInfo->unitDefId) || (resInfo->techDef->ptrEffects[i].effectClass == unitInfo->unitAIType))
								) {
								relevantResearch = true;
							}
							// Upgrade "current unit" to another one
							if ((resInfo->techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) &&
								(resInfo->techDef->ptrEffects[i].effectUnit == unitInfo->unitDefId)
								) {
								relevantResearch = true;
							}
						}
						if (relevantResearch) {
							moreAttackResearches.insert(resInfo);
							if (resInfo->markedForAdd) {
								// We can already mark this research for early "placement": it is already "validated", and useful for early units.
								// This does NOT add researches to strategy, only moves them to an "earlier" location
								resInfo->forcePlaceForFirstImpactedUnit = true; // Research placement in strategy will put this near first related unit.
							}
						}
					}
				}
			}
		}
	}
	// In collected techs, is there any one that serves more than 1 unit ?
	for each (PotentialResearchInfo *resInfo in moreAttackResearches)
	{
		int impactedUnitsCount = resInfo->impactedUnitDefIds.size();
		if ((impactedUnitsCount > 1) && (!resInfo->isInStrategy && (!resInfo->forcePlaceForFirstImpactedUnit))) {
			resInfo->forcePlaceForFirstImpactedUnit = true;
		}
		int unitInstanceCountThatTriggersAddResearch = randomizer.GetRandomValue_normal_moderate(3, 8);
		// If enough unit "instances" are trained, 
		if (resInfo->unitInstanceScoreForOptionalResearch >= unitInstanceCountThatTriggersAddResearch) {
			resInfo->forcePlaceForFirstImpactedUnit = true;
		}
	}

	// Recompute unit scores for ALL (selected) units (not restricted to early units)
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		resInfo->unitInstanceScoreForOptionalResearch = 0;
		for each (short int impactedUnitDefId in resInfo->impactedUnitDefIds) {
			for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
			{
				if (unitInfo->unitDefId == impactedUnitDefId) {
					resInfo->unitInstanceScoreForOptionalResearch += (int)unitInfo->addedCount;
				}
			}
#pragma message("Villagers: we should add all related villager datids in researches->impactedUnitDefIds when adding research info. This way it would even be visible for early units see above")
			/*if (impactedUnitDefId == CST_UNITID_VILLAGER) {
				resInfo->unitInstanceScoreForOptionalResearch += this->villagerCount_alwaysRetrain;
				}
				*/
		}
		ROR_STRUCTURES_10C::STRUCT_TECH_DEF *techDef = this->global->GetTechDef(resInfo->researchDef->technologyId);
		for (int i = 0; (techDef != NULL) && (i < techDef->effectCount); i++) {
			if (techDef->ptrEffects[i].IsAttributeModifier()) {
				if ((techDef->ptrEffects[i].effectClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) &&
					(techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_HP)) {
					resInfo->unitInstanceScoreForOptionalResearch += 15; // Add HP to all buildings (the only attribute that really improves buildings)
				}
			}
		}
	}


	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		// Loop on researches we do not plan (yet) to include in strategy = optional researches
		if (!resInfo->isInStrategy && !resInfo->markedForAdd && !resInfo->forcePlaceForFirstImpactedUnit) {
			int impactedUnitsCount = resInfo->impactedUnitDefIds.size();
			// Costs ?
			// low-cost techs that improve >1 unit type: add (tool age storage pit techs...)
			// check Age availability ?

			// Special cases
			// TODO !!!!
		}
	}
}


// Adds non-military researches that should always be included, for example wheel - if available in tech tree.
void StrategyBuilder::AddMandatoryNonMilitaryResearches() {
	// Always add wheel (if available in tech tree - checked in AddPotentialResearchInfoToList)
	this->AddPotentialResearchInfoToList(CST_RSID_WHEEL); // Note: it may have already been added by some dependency
	PotentialResearchInfo *resInfo = this->GetResearchInfo(CST_RSID_WHEEL);
	if (resInfo) {
		// Force wheel to be developed before next age (if there IS a next age)
		if (resInfo->requiredAge < CST_RSID_IRON_AGE) {
			resInfo->researchesThatMustBePutAfterMe.insert(resInfo->requiredAge + 1);
		}
		resInfo->forcePutAsEarlyAsPossible = true;
	}

	// Make sure farm requirements are met
	short int enableFarmResearchId = FindResearchThatEnableUnit(this->player, CST_UNITID_FARM, 0);
	this->AddPotentialResearchInfoToList(enableFarmResearchId);
}



/*** Strategy writing methods ***/


// Get the very global information about strategy generation (number of villagers, etc)
void StrategyBuilder::CollectGlobalStrategyGenerationInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	this->maxPopulation = settings->maxPopulation;
	this->isWaterMap = IsDockRelevantForMap(settings->mapTypeChoice);

	// *** Compute unit numbers ***
	// Land maps: fixed villagers count around 14-20  ; retrains around 6-10  ; total 18-28
	// Water maps: fixed villagers count around 10-16  ; retrain around 5-9  ; total 16-24 . original AI: often 10+6 or 14+6
	const long int minTotalVillagers = this->isWaterMap ? 12 : 14; // Max villager count     = 22 (water) or 26 (land)
	const long int maxTotalVillagers = this->isWaterMap ? 22 : 26; // Max villager count     = 22 (water) or 26 (land)
	const long int minFixedVillagers = this->isWaterMap ? 10 : 14; // Min fixed vill count   = 10 (water) or 14 (land)
	const long int minLimitedRetrainsVillagers = this->isWaterMap ? 3 : 4; // min "retrains" = 3  (water) or  4 (land)
	assert(minFixedVillagers + minLimitedRetrainsVillagers < maxTotalVillagers);
	const long int maxFixedVillagersRandomPart = 6; // random interval size
	const long int maxLimitedRetrainsVillagersRandomPart = this->isWaterMap ? 4 : 6; // random interval size

	int fixedVillagerCount = randomizer.GetRandomValue_normal_moderate(minFixedVillagers, minFixedVillagers + maxFixedVillagersRandomPart);
	int limitedRetrainsVillagerCount = randomizer.GetRandomValue_normal_moderate(minLimitedRetrainsVillagers, minLimitedRetrainsVillagers + maxLimitedRetrainsVillagersRandomPart);
	if (fixedVillagerCount + limitedRetrainsVillagerCount > maxTotalVillagers) { limitedRetrainsVillagerCount = maxTotalVillagers; }

	this->villagerCount_alwaysRetrain = fixedVillagerCount;
	this->villagerCount_limitedRetrains = limitedRetrainsVillagerCount;
	// TODO: more villagers if max population > 50 ?

	// "civilian" Boats : TODO
	if (this->isWaterMap) {

	}

	// Select the military units to train (including ships)

	this->CollectPotentialUnitsInfo(player);
	this->ComputeScoresForPotentialUnits();
	this->SelectStrategyUnits();

}


// Creates base strategy elements (ages + initial Town center)
// Does not add villagers or any other item
void StrategyBuilder::CreateTCAndAgesStrategyElements() {
	if (!this->buildAI || !this->buildAI->IsCheckSumValid()) { return; }
	if (!player || !player->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &this->buildAI->fakeFirstStrategyElement;
	
	// Adding before "fake first" will add in last position in strategy.
	// Add Town Center
	if (AddUnitInStrategy_before(this->buildAI, fakeFirstElem, -1, -1, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding, CST_UNITID_FORUM, this->player, "TownCenter")) {
		this->seTownCenter = fakeFirstElem->previous;
	}
	
	// Barracks are always needed
	// Tool Age
	if (!IsResearchRelevantForStrategy(player, CST_RSID_TOOL_AGE)) {
		string msg = "This civ seems NOT to have tool age. Player #";
		msg += to_string(this->buildAI->commonAIObject.playerId);
		msg += " : ";
		msg += this->buildAI->commonAIObject.playerName;
		traceMessageHandler.WriteMessageNoNotification(msg);
		return;
	}
	if (AddUnitInStrategy_before(this->buildAI, fakeFirstElem, -1, CST_UNITID_FORUM, AOE_CONST_FUNC::TAIUnitClass::AIUCCritical, CST_RSID_TOOL_AGE, player, "*Tool Age*")) {
		this->seToolAge = fakeFirstElem->previous;
	}
	// Bronze Age
	if (!IsResearchRelevantForStrategy(this->player, CST_RSID_BRONZE_AGE)) {
		string msg = "This civ seems NOT to have bronze age. Player #";
		msg += to_string(this->buildAI->commonAIObject.playerId);
		msg += " : ";
		msg += this->buildAI->commonAIObject.playerName;
		traceMessageHandler.WriteMessageNoNotification(msg);
		return;
	}
	if (AddUnitInStrategy_before(buildAI, fakeFirstElem, -1, CST_UNITID_FORUM, AOE_CONST_FUNC::TAIUnitClass::AIUCCritical, CST_RSID_BRONZE_AGE, this->player, "*Bronze Age*")) {
		this->seBronzeAge = fakeFirstElem->previous;
	}
	// Iron Age
	if (!IsResearchRelevantForStrategy(this->player, CST_RSID_IRON_AGE)) {
		string msg = "This civ seems NOT to have iron age. Player #";
		msg += to_string(buildAI->commonAIObject.playerId);
		msg += " : ";
		msg += buildAI->commonAIObject.playerName;
		traceMessageHandler.WriteMessageNoNotification(msg);
		return;
	}
	if (AddUnitInStrategy_before(this->buildAI, fakeFirstElem, -1, CST_UNITID_FORUM, AOE_CONST_FUNC::TAIUnitClass::AIUCCritical, CST_RSID_IRON_AGE, player, "*Iron Age*")) {
		this->seIronAge = fakeFirstElem->previous;
	}
}

// Create villager strategy elements, including fishing ships
void StrategyBuilder::CreateVillagerStrategyElements() {
	if (!this->buildAI || !this->buildAI->IsCheckSumValid()) { return; }
	if (!player || !player->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &this->buildAI->fakeFirstStrategyElement;
	int totalVillagersCount = this->villagerCount_alwaysRetrain + this->villagerCount_limitedRetrains;
	assert(this->villagerCount_alwaysRetrain + this->villagerCount_limitedRetrains > 0);

#pragma message("TODO: boats")
#pragma message("TODO: add a random part in repartition")
	int currentCount_fixed = 0;
	int currentCount_retrains = 0;

	// *** Stone age *** : 11+2 (land), 9+3 (water)
	int stoneAgeVillagerCount_fixed = 11;
	int stoneAgeVillagerCount_retrains = 2;
	if (this->villagerCount_alwaysRetrain < 15) {
		stoneAgeVillagerCount_fixed = 9;
		stoneAgeVillagerCount_retrains = 3;
	}
	if (stoneAgeVillagerCount_retrains >= this->villagerCount_limitedRetrains) {
		stoneAgeVillagerCount_retrains = this->villagerCount_limitedRetrains;
	}
	for (int i = 0; i < stoneAgeVillagerCount_fixed; i++) {
		AddUnitInStrategy_before(this->buildAI, this->seToolAge, -1, CST_UNITID_FORUM, TAIUnitClass::AIUCLivingUnit, CST_UNITID_VILLAGER, player, "Man");
		currentCount_fixed++;
	}
	for (int i = 0; i < stoneAgeVillagerCount_retrains; i++) {
		AddUnitInStrategy_before(this->buildAI, this->seToolAge, 2, CST_UNITID_FORUM, TAIUnitClass::AIUCLivingUnit, CST_UNITID_VILLAGER, player, "Man");
		currentCount_retrains++;
	}
	assert(currentCount_fixed == stoneAgeVillagerCount_fixed);
	assert(currentCount_retrains == stoneAgeVillagerCount_retrains);
	// *** Tool age ***
	// Total to add: half of remaining villagers, including all "fixed" ones (or almost)
	int totalToolAgeVillagers = totalVillagersCount - currentCount_fixed - currentCount_retrains;
	assert(totalToolAgeVillagers >= 0);
	totalToolAgeVillagers = (totalToolAgeVillagers + 1) / 2;
	if (totalToolAgeVillagers + currentCount_fixed + currentCount_retrains > totalVillagersCount) {
		totalToolAgeVillagers = totalVillagersCount;
	}
	// Now get repartition (fixed / retrains)
	int toolAgeVillagerCount_fixed = totalToolAgeVillagers;
	if (toolAgeVillagerCount_fixed + currentCount_fixed > this->villagerCount_alwaysRetrain) {
		toolAgeVillagerCount_fixed = this->villagerCount_alwaysRetrain - currentCount_fixed;
	}
	assert(toolAgeVillagerCount_fixed >= 0);
	int toolAgeVillagerCount_retrains = totalToolAgeVillagers - toolAgeVillagerCount_fixed;
	assert(toolAgeVillagerCount_retrains >= 0);
	if (toolAgeVillagerCount_retrains + currentCount_retrains > this->villagerCount_limitedRetrains) {
		assert(false && "this should not happen due to previous computations");
		toolAgeVillagerCount_retrains = this->villagerCount_limitedRetrains - currentCount_retrains;
	}
	for (int i = 0; i < toolAgeVillagerCount_fixed; i++) {
		AddUnitInStrategy_before(this->buildAI, this->seBronzeAge, -1, CST_UNITID_FORUM, TAIUnitClass::AIUCLivingUnit, CST_UNITID_VILLAGER, player, "Man");
		currentCount_fixed++;
	}
	for (int i = 0; i < toolAgeVillagerCount_retrains; i++) {
		AddUnitInStrategy_before(this->buildAI, this->seBronzeAge, 2, CST_UNITID_FORUM, TAIUnitClass::AIUCLivingUnit, CST_UNITID_VILLAGER, player, "Man");
		currentCount_retrains++;
	}

	// *** Bronze age *** : add all remaining villagers
	int bronzeAgeFixedCount = (this->villagerCount_alwaysRetrain - currentCount_fixed);
	for (int i = 0; i < bronzeAgeFixedCount; i++) {
		AddUnitInStrategy_before(this->buildAI, this->seIronAge, -1, CST_UNITID_FORUM, TAIUnitClass::AIUCLivingUnit, CST_UNITID_VILLAGER, player, "Man");
		currentCount_fixed++;
	}
	int bronzeAgeRetrainsCount = (this->villagerCount_limitedRetrains - currentCount_retrains);
	for (int i = 0; i < bronzeAgeRetrainsCount; i++) {
		// Only 1 retrain for bronze age limited retrains villagers
		AddUnitInStrategy_before(this->buildAI, this->seIronAge, 1, CST_UNITID_FORUM, TAIUnitClass::AIUCLivingUnit, CST_UNITID_VILLAGER, player, "Man");
		currentCount_retrains++;
	}
	assert(currentCount_fixed == this->villagerCount_alwaysRetrain);
	assert(currentCount_retrains == this->villagerCount_limitedRetrains);

	// Random part for retrains : switch retrains/not_retrains between tool and bronze villagers (random one, not necessarily last one)
}

// Create early ages military units strategy elements (limited retrains units)
void StrategyBuilder::CreateEarlyMilitaryUnitsElements() {
	// Stone age and tool are considered same here.
	for (short int currentAge = CST_RSID_TOOL_AGE; currentAge < CST_RSID_IRON_AGE; currentAge++) {
		std::set<PotentialUnitInfo*> unitsListForThisAge;
		unitsListForThisAge.clear();
		// Get different units to add
		for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
		{
			short int unitAge = unitInfo->ageResearchId;
			if (unitAge <= CST_RSID_TOOL_AGE) { unitAge = CST_RSID_TOOL_AGE; }
			// Some conditions may be redundant...
			if (unitInfo->isSelected && (unitAge == currentAge) && (unitInfo->isOptionalUnit) && (unitInfo->addedCount == 0) &&
				(unitInfo->earlyAgeUnit)) {
				unitsListForThisAge.insert(unitInfo);
			}
		}
		// Prepare number to add (using random factor)
		int totalUnitCount = randomizer.GetRandomValue_normal_moderate(2, 9); // in most cases, should be around 5-6
		int remainingUnitToAdd = totalUnitCount;
		// TODO take into account costs ? (scout is expensive, etc).. not sure
		// Prepare ordered list (with repartition)
		std::list<PotentialUnitInfo*> orderedUnitsToAdd;
		orderedUnitsToAdd.clear();
		while ((remainingUnitToAdd > 0) && (unitsListForThisAge.size() > 0)) {
			for each (PotentialUnitInfo *unitInfo in unitsListForThisAge)
			{
				orderedUnitsToAdd.push_back(unitInfo);
				remainingUnitToAdd--;
			}
		}
		// Find insertion point
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *insertionPoint = &this->buildAI->fakeFirstStrategyElement; // insert just before this
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *minInsertionPoint = NULL; // do not insert before this.
		switch (currentAge) {
		case CST_RSID_STONE_AGE:
		case CST_RSID_TOOL_AGE:
			insertionPoint = this->seBronzeAge;
			minInsertionPoint = this->seToolAge;
			break;
		case CST_RSID_BRONZE_AGE:
			insertionPoint = this->seIronAge;
			minInsertionPoint = this->seBronzeAge;
			break;
		case CST_RSID_IRON_AGE:
			minInsertionPoint = this->seIronAge;
			break;
		default:
			break;
		}
		// Use orderedUnitsToAdd to add elements in strategy in optimized order (with unit repartition)
		int loopCount = 0;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *beforeInsertionPoint = insertionPoint->previous;
		for each (PotentialUnitInfo *unitInfo in orderedUnitsToAdd)
		{
			long int retrains = 2;
			assert(unitInfo->earlyAgeUnit);
			AddUnitInStrategy_before(this->buildAI, insertionPoint, retrains, unitInfo->baseUnitDefLiving->trainLocation, TAIUnitClass::AIUCLivingUnit, unitInfo->unitDefId, player,
				(unitInfo->baseUnitDefLiving != NULL) ? unitInfo->baseUnitDefLiving->ptrUnitName : unitInfo->unitName);
			unitInfo->addedCount++;
			loopCount++;
			if (loopCount == 4) {
				// Move insertion point earlier so all units are not onsecutive
				int movementCount = 0; // = number of elements to reach "minInsertionPoint", backwards movement
				ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *curElem = beforeInsertionPoint->next; // should be 1st inserted element
				while ((movementCount < 100) && curElem && (curElem->previous != minInsertionPoint)) {
					curElem = curElem->previous;
					movementCount++;
				}
				if ((movementCount < 100) && (movementCount > 2)) {
					insertionPoint = beforeInsertionPoint->next;
					for (int i = 0; i < (movementCount / 2); i++) {
						insertionPoint = insertionPoint->previous;
					}
				}
			}
		}
		// Compute unitInfo->firstStratElem
		for each (PotentialUnitInfo *unitInfo in orderedUnitsToAdd)
		{
			ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *curElem = this->buildAI->fakeFirstStrategyElement.next;
			bool found = false;
			while (curElem && (curElem != &this->buildAI->fakeFirstStrategyElement) && !found) {
				if ((curElem->unitDAT_ID == unitInfo->unitDefId) && (curElem->elementType == TAIUnitClass::AIUCLivingUnit)) {
					found = true;
					unitInfo->firstStratElem = curElem;
				}
				curElem = curElem->next;
			}
		}
		orderedUnitsToAdd.clear();
	}
}


// Create main military units strategy elements
void StrategyBuilder::CreateMainMilitaryUnitsElements() {
	int optionalsCount = 0;
	int mainUnitsCount = 0;
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
	{
		if (!unitInfo->earlyAgeUnit) {
			if (unitInfo->isOptionalUnit) {
				optionalsCount++;
			} else {
				mainUnitsCount++;
			}
			unitInfo->scoreForUnitCount = 0;
		}
	}
	// Max 8 total "potential units"
	float maxPotentialUnitsNumber = 8;
	if (optionalsCount == 1) { maxPotentialUnitsNumber = 5; }
	if (optionalsCount == 0) { maxPotentialUnitsNumber = 0; }
	float maxOptionalsProportion = 8 * 100 / (float)this->maxPopulation; // a % value = max % accepted so we stay below 8 units

	// Work on unit repartition (number of each type)
	int totalUnitCount = optionalsCount + mainUnitsCount;
	if (totalTrainUnitCosts <= 0) { return; }
	float mainUnitsProportion = 85;
	if (100 - mainUnitsProportion > maxOptionalsProportion) {
		mainUnitsProportion = 100 - maxOptionalsProportion;
	}
	int remainingPopulation = this->maxPopulation - this->villagerCount_alwaysRetrain - fishingShipsCount_alwaysRetrain; // TODO: count other boats
	if (optionalsCount == 0) {
		mainUnitsProportion = 100;
	}
	float proportionForOneMainUnit = mainUnitsProportion / mainUnitsCount; // Basic repartition %
	float currentTotalPercentages = 0;
	// Loop: apply changes to proportions according to unit classes
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
	{
		if (!unitInfo->isOptionalUnit && !unitInfo->earlyAgeUnit) {
			switch (unitInfo->baseUnitDefLiving->unitAIType) {
			case TribeAIGroupChariot:
			case TribeAIGroupElephantRider:
			case TribeAIGroupFootSoldier:
			case TribeAIGroupHeavyFootSoldier:
			case TribeAIGroupPhalanx:
			case TribeAIGroupMountedSoldier:
			case TribeAIGroupArcher:
			case TribeAIGroupChariotArcher:
			case TribeAIGroupElephantArcher:
			case TribeAIGroupHorseArcher:
				unitInfo->scoreForUnitCount = proportionForOneMainUnit * 1.15f; // bonus for melee and base archer unit count (need more numbers)
				break;
			//case TribeAIGroupPriest:
			//case TribeAIGroupSiegeWeapon:
			//case TribeAIGroupSlinger:
			//case TribeAIGroupWarBoat:
			default:
				unitInfo->scoreForUnitCount = proportionForOneMainUnit;
				break;
			}
			currentTotalPercentages += unitInfo->scoreForUnitCount; // due to bonuses, total will be != 100%
		}
	}
	// Adjust % if sum is no longer matching mainUnitsProportion
	float percentageDiff = mainUnitsProportion - currentTotalPercentages; // can be negative (if we assigned too much % points)
	float adjustmentPerUnit = percentageDiff / mainUnitsCount;
	// Apply adjustment to get a correct sum of percentages + calculate desired count
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
	{
		if (!unitInfo->earlyAgeUnit) {
			if (!unitInfo->isOptionalUnit) {
				unitInfo->scoreForUnitCount += adjustmentPerUnit;
				if (unitInfo->scoreForUnitCount < 0) { unitInfo->scoreForUnitCount = 0; } // Should never happen
				if (unitInfo->scoreForUnitCount > 100) { unitInfo->scoreForUnitCount = 100; } // Should never happen
			}
			if (!unitInfo->isOptionalUnit) {
				unitInfo->desiredCount = ((unitInfo->scoreForUnitCount * remainingPopulation) / 100);
			} else {
				unitInfo->desiredCount = ((100 - mainUnitsProportion) * remainingPopulation / 100);
			}
			unitInfo->desiredCount = unitInfo->desiredCount;
			this->log += "Desired count for ";
			this->log += unitInfo->unitName;
			this->log += "=";
			this->log += std::to_string(unitInfo->desiredCount);
			this->log += newline;
		}
	}

	// For the moment, insert all in iron age
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *insertionPoint = &this->buildAI->fakeFirstStrategyElement;

	// Fill orderedUnitsToAdd list with ordered units (1 by 1)
	std::list<PotentialUnitInfo*> orderedUnitsToAdd;
	bool found = true;
	while (found) {
		PotentialUnitInfo *currentBest = NULL;
		float lowestProportion = 100;
		for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
		{
			if (!unitInfo->earlyAgeUnit) {
				float p = 100 * unitInfo->addedCount / unitInfo->desiredCount; // both are "int" value, but use float for % precision
				if ((unitInfo->isOptionalUnit) && (p < 100)) {
					p = p / 2; // Force optional units to be trained more at the beginning (there are less of them)
				}
				if (p < lowestProportion) {
					lowestProportion = p;
					currentBest = unitInfo;
				}
			}
		}
		found = (currentBest != NULL);
		if (found) {
			orderedUnitsToAdd.push_back(currentBest);
			currentBest->addedCount++;
		}
	}
	// Use orderedUnitsToAdd to add elements in strategy in optimized order (with unit repartition)
	for each (PotentialUnitInfo *unitInfo in orderedUnitsToAdd)
	{
		long int retrains = -1;
		if (unitInfo->earlyAgeUnit) { retrains = 2; } // should not happen here
		AddUnitInStrategy_before(this->buildAI, insertionPoint, retrains, unitInfo->baseUnitDefLiving->trainLocation, TAIUnitClass::AIUCLivingUnit, unitInfo->unitDefId, player,
			(unitInfo->baseUnitDefLiving != NULL) ? unitInfo->baseUnitDefLiving->ptrUnitName : unitInfo->unitName);
		// Note: unitInfo->addedCount has already been incremented
		if (unitInfo->firstStratElem == NULL) {
			unitInfo->firstStratElem = insertionPoint->previous; // First inserted will be first in strategy (insertion order is normal - not reversed)
		}
	}
	orderedUnitsToAdd.clear();

#pragma message("TODO: unfinished")
	// TODO: we inserted everything after iron age.
	// If units are available before, move some... and test if temp units are needed if only a part of units was moved?
}

// Add strategy elements for required researches for "main units"
void StrategyBuilder::CreateMilitaryRequiredResearchesStrategyElements() {
	// First of all, place unit-activating researches just before 1st occurrence of target unit.
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
	{
		short int resId = unitInfo->enabledByResearchId;
		PotentialResearchInfo *resInfo = this->GetResearchInfo(resId);
		if (resInfo && !resInfo->isInStrategy && resInfo->markedForAdd) {
			resInfo->unitsThatMustBePutAfterMe.insert(unitInfo->unitDefId); // just for the record...
			assert(unitInfo->firstStratElem != NULL);
			if (unitInfo->firstStratElem != NULL) {
				this->AddResearchToStrategy(resInfo, unitInfo->firstStratElem);
			}
		}
	}
	// Handle the researches that are NECESSARY (for base unit, do not consider upgrades here) before others
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
	{
		std::vector<short int> tmpResIdAsVector;
		tmpResIdAsVector.push_back(unitInfo->enabledByResearchId);
		std::vector<short int> requiredResearches = GetValidOrderedResearchesListWithDependencies(this->player, tmpResIdAsVector);
		// Copy to unitInfo's set once and for all... And apply some filtering
		for each (short int resId in requiredResearches)
		{
			STRUCT_RESEARCH_DEF *resDef = this->player->GetResearchDef(resId);
			if (resDef && (resId != CST_RSID_STONE_AGE) && (resId != CST_RSID_TOOL_AGE) && 
				(resId != CST_RSID_BRONZE_AGE) && (resId != CST_RSID_IRON_AGE) /*&& (resId != 104) for republic age*/) {
				if (resDef->researchLocation >= 0) { // ignore shadow techs : not to be put in strategy, and we won't get too many dependencies
					unitInfo->requiredResearchesForBaseUnit.insert(resId);
				}
			}
		}
		for each (short int reqResId in requiredResearches)
		{
			for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
			{
				if (resInfo->researchId == reqResId) {
					// Found current requirement in my "potential researches" list. Update its position constraints
					if (unitInfo->enabledByResearchId >= 0) {
						resInfo->researchesThatMustBePutAfterMe.insert(unitInfo->enabledByResearchId);
						resInfo->unitsThatMustBePutAfterMe.insert(unitInfo->unitDefId);
					}
				}
			}
		}
	}
	// Add researches to strategy (only the necessary ones)
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
	{
		for each (short int resId in unitInfo->requiredResearchesForBaseUnit)
		{
			// Get matching PotentialResearchInfo object
			PotentialResearchInfo *resInfo = this->GetResearchInfo(resId);
			if (!resInfo) {
				// Some dependency has not been correctly handled !
				this->AddPotentialResearchInfoToList(resId);
				resInfo = this->GetResearchInfo(resId);
				this->log += "Missing research in dependencies: ";
				this->log += std::to_string(resId);
				this->log += newline;
			}

			//for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
			//{
			if (resInfo && (resInfo->researchId == resId) && (!resInfo->isInStrategy) && (resInfo->markedForAdd) && resInfo->researchDef) {
					resInfo->ComputeStratElemPositionConstraints(this->buildAI); // update dependencies on strategy elements
					// Add to strategy at correct location
					ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *insertionPoint = this->buildAI->fakeFirstStrategyElement.next;
					if (resInfo->mustBeAfterThisElem != NULL) {
						insertionPoint = resInfo->mustBeAfterThisElem->next;
					}
					int usefulness = resInfo->impactedUnitDefIds.size();
					if (usefulness <= 1) {
						insertionPoint = resInfo->mustBeBeforeThisElem;
					}
					if ((usefulness >= 2) && (usefulness <= 3)) {
						int elemCountInAllowedInterval = 0;
						ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *curElem = resInfo->mustBeAfterThisElem->next;
						while (curElem && (curElem != &buildAI->fakeFirstStrategyElement) && (curElem != resInfo->mustBeBeforeThisElem)) {
							elemCountInAllowedInterval++;
							curElem = curElem->next;
						}
						if (elemCountInAllowedInterval > 2) {
							// put in medium position
							for (int i = 0; i < (elemCountInAllowedInterval/2); i++) {
								insertionPoint = insertionPoint->next;
							}
						}
					}
					// Other (useful for many units = high priority tech): insert as early as possible in strategy
					this->AddResearchToStrategy(resInfo, insertionPoint);
				}
			//}
		}
	}
}


// Add strategy elements other than "main units" required researches.
void StrategyBuilder::CreateOtherResearchesStrategyElements() {
	// Add also researches with a specific location (forcePlaceForFirstImpactedUnit)
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		if ((resInfo->forcePlaceForFirstImpactedUnit || resInfo->forcePutAsEarlyAsPossible) && !resInfo->isInStrategy) {
			// If research has already been placed, move it ? should not be necessary because ComputeStratElemPositionConstraints should have taken this into account ?
			resInfo->ComputeStratElemPositionConstraints(this->buildAI);
			if (resInfo->mustBeAfterThisElem != NULL) {
				this->AddResearchToStrategy(resInfo, resInfo->mustBeAfterThisElem->next);
			}
		}
	}

	// Other researches
	int totalResearchesToUse = 0; // Get total count of (remaining) researches to add
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		if (resInfo->markedForAdd && !resInfo->isInStrategy) {
			totalResearchesToUse++;
		}
	}
	// TEST - TODO: this is quite random and not much tested... Just a try.
	int totalAddedResearches = 0;
	int addedResearchesThisLoop = 1;
	while (addedResearchesThisLoop > 0) {
		addedResearchesThisLoop = 0;
		for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
		{
			if (resInfo->markedForAdd && !resInfo->isInStrategy) {
				bool requirementsAreReady = true;
				for each (short int reqResId in resInfo->researchesThatMustBePutBeforeMe)
				{
					PotentialResearchInfo *reqResInfo = this->GetResearchInfo(reqResId);
					if (reqResInfo) {
						requirementsAreReady = requirementsAreReady && (reqResInfo->isInStrategy || (reqResInfo->researchDef && reqResInfo->researchDef->researchLocation < 0));
					}
				}
				if (requirementsAreReady) {
					resInfo->ComputeStratElemPositionConstraints(this->buildAI); // update dependencies on strategy elements
					// Add to strategy at correct location
					ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *insertionPoint = this->buildAI->fakeFirstStrategyElement.next;
					if (resInfo->mustBeAfterThisElem != NULL) {
						insertionPoint = resInfo->mustBeAfterThisElem->next;
					}
					int moveFurther = randomizer.GetRandomValue(1, 24);
					while (insertionPoint && insertionPoint->next && (moveFurther > 0)) {
						if (insertionPoint->elemId == -1) { break; } // Reached empty element (end of strategy)
						if (insertionPoint == resInfo->mustBeBeforeThisElem) { break; } //  Ensure dependencies are respected
						insertionPoint = insertionPoint->next;
						moveFurther--;
					}
					if (insertionPoint) {
						this->AddResearchToStrategy(resInfo, insertionPoint);
						addedResearchesThisLoop++;
					}
				}
			}
		}
		totalAddedResearches += addedResearchesThisLoop;
	}
	// TODO: loop on previous block until we're making no more progress (or remaining=0) ?
	this->log += std::to_string(totalAddedResearches);
	this->log += "/";
	this->log += std::to_string(totalResearchesToUse);
	this->log += " non-critical researches added";
	this->log += newline;
}


// Add the first building of each building kind to strategy. Returns number of added buildings
// Does not add an additional building if there is already one in strategy
// Always add in "parent" age strategy zone.
int StrategyBuilder::CreateFirstBuildingsStrategyElements() {
	int counter = 0;
	for each (PotentialBuildingInfo *bldInfo in this->potentialBuildingsList)
	{
		if (!bldInfo->unitDef || !bldInfo->unitDef->IsCheckSumValidForAUnitClass() || !bldInfo->unitDef->IsTypeValid() ||
			(bldInfo->addedInStrategyCount > 0)) { continue; }
		// Some buildings with hardcoded behaviour
		if ((bldInfo->unitDefId == CST_UNITID_GRANARY) || (bldInfo->unitDefId == CST_UNITID_STORAGE_PIT)) {
			// Read SN number from strategy AI because it is valued and correct even "before" game start. TacAI's SN number are copied later.
			if (this->ai->structStrategyAI.SNNumber[SNAutoBuildDropsites] > 0) {
				bldInfo->addedInStrategyCount++; // Anticipate the one that will be added at game start after a few seconds
				continue;
			}
		}
		if (!this->isWaterMap && (bldInfo->unitDefId == CST_UNITID_DOCK)) {
			this->log += "Warning: do not add dock in strategy as this is not a water map";
		}

		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &this->buildAI->fakeFirstStrategyElement;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *curElem = this->GetAgeStrategyElement(bldInfo->enabledInAge);
		if (!curElem) {
			curElem = &this->buildAI->fakeFirstStrategyElement;
		}
		curElem = curElem->next;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *insertionPoint = NULL;
		if (bldInfo->highPriority) {
			bldInfo->highPriority = false; // High priority is only for first placed building of this kind
			insertionPoint = curElem; // insertionPoint being set here, next loop will be ignored in this case
			// Just allow some villagers to be trained
			int allowedVillagers = 2;
			while (allowedVillagers > 0) {
				if ((insertionPoint->elementType == TAIUnitClass::AIUCLivingUnit) && (insertionPoint->unitDAT_ID == CST_UNITID_MAN)) {
					insertionPoint = insertionPoint->next;
				} else {
					allowedVillagers = 0; // Stop and insert here
				}
				allowedVillagers--;
			}
		}
		// Add a building just before the first location in strategy where it is needed - works for most dependencies except some, like market (to enable farms)
		while (curElem && (curElem != fakeFirstElem) && (insertionPoint == NULL)) {
			bool isResearch = (curElem->elementType == TAIUnitClass::AIUCTech) || (curElem->elementType == TAIUnitClass::AIUCCritical);
			bool actorFieldIsRelevant = isResearch || (curElem->elementType == TAIUnitClass::AIUCLivingUnit) || (curElem->elementType == TAIUnitClass::AIUCBuilding);
			if (actorFieldIsRelevant && (curElem->actor == bldInfo->unitDefId)) {
				insertionPoint = curElem;
			}
			if (isResearch && (curElem->unitDAT_ID >= CST_RSID_STONE_AGE) && (curElem->unitDAT_ID <= CST_RSID_IRON_AGE)) { // + 104 for republic age
				insertionPoint = curElem; // stop when reaching an age upgrade element. Building must be constructed in its "own" age
			}
			curElem = curElem->next;
		}
		if (insertionPoint) {
			counter += AddBuildingToStrategy(bldInfo, insertionPoint);
		}
	}
	return counter;
}

// Create secondary (optional) occurrences of buildings. E.g. 2nd TC, or additional military buildings to train army faster and do researches while another building is training units
int StrategyBuilder::CreateSecondaryBuildingStrategyElements() {
	// Backup TC: add after all "required" buildings and after gov center (not allowed before) & after iron age too
	PotentialBuildingInfo *tcInfo = this->GetBuildingInfo(CST_UNITID_FORUM);
	if (tcInfo && (tcInfo->addedInStrategyCount <= 1) && (tcInfo->desiredCount > 1)) {
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *curElem = this->seIronAge;
		if (GetFirstElementOf(this->buildAI, curElem, this->seGovCenter) == curElem) {
			curElem = this->seGovCenter; // Now curElem is the latest of the 2 elements in strategy
		}
		for each (PotentialBuildingInfo *bldInfo in this->potentialBuildingsList)
		{
			if (bldInfo->addedInStrategyCount > 0) {
				if (GetFirstElementOf(this->buildAI, curElem, bldInfo->firstAddedInStrategy) == curElem) {
					curElem = bldInfo->firstAddedInStrategy; // curElem was the earlier elem : change
				}
			}
		}
		// Move forward - with a random part
		int moveCount = randomizer.GetRandomValue_normal_moreFlat(5, 20);
		while (curElem && (curElem != &this->buildAI->fakeFirstStrategyElement) && (moveCount > 0)) {
			curElem = curElem->next;
			moveCount--;
		}
		if (curElem) {
			AddBuildingToStrategy(tcInfo, curElem);
		}
	}

	// TODO : other buildings
#pragma message("TODO : add other secondary buildings for 'main' units")
	return 0;
}


// Create a brand new dynamic strategy for player.
void StrategyBuilder::CreateStrategyFromScratch() {
	if (!this->buildAI || !this->buildAI->IsCheckSumValid() || !this->buildAI->mainAI || !this->buildAI->mainAI->IsCheckSumValid()) { return; }
	if (!this->player || !this->player->IsCheckSumValid()) { return; }
	if (!this->global || !this->global->IsCheckSumValid()) { return; }
	// Clear previous strategy
	ClearStrategy(this->buildAI);
	strcpy_s(this->buildAI->strategyFileName, 0x3F, "Dynamic CustomROR strategy");

	this->log += "Start Strategy creation for p#";
	this->log += std::to_string(this->player->playerId);
	this->log += " = ";
	this->log += this->player->playerName_length16max;
	this->log += newline;

	// WARNING : take care if map is naval (=> villager count, land military units count + all non-war boats thing)
	// + max pop criterion
	this->CollectGlobalStrategyGenerationInfo(player);

	// Initialize strategy with TC + ages (no villager yet)
	this->CreateTCAndAgesStrategyElements();
	// Add villagers
	this->CreateVillagerStrategyElements();

	// Select researches to add in strategy
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
	{
		this->CollectResearchInfoForUnit(unitInfo->unitDefId, !unitInfo->isOptionalUnit);
	}
	this->AddMandatoryNonMilitaryResearches();
	// Mark for add: all researches added previously are "validated" / "necessary"
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		resInfo->markedForAdd = true;
	}

	// Some "hardcoded" stuff (related to game basic behaviour)
#pragma message("villagers: does not work, you try on every villager id (use class&villager mode + farms amount")
	this->CollectResearchInfoForUnit(CST_UNITID_VILLAGER, false); // Get optional researches for economy (NOT marked for add at this point)
	this->CollectResearchInfoForUnit(CST_UNITID_WATCH_TOWER, false); // TODO: tower upgrades except ballista

	// Finalize exact list of trained units => add units
	this->AddMilitaryUnitsForEarlyAges();
	this->UpdateRequiredBuildingsFromValidatedResearches();
	this->UpdateMissingResearchRequirements();
	// Add backup TC in internal info
	PotentialBuildingInfo *tcInfo = this->GetBuildingInfo(CST_UNITID_FORUM);
	if (tcInfo && (tcInfo->desiredCount == 1)) {
		tcInfo->desiredCount++;  // Add a backup TC
	}

	// If required buildings have not been added to strategy yet, do it now...
	this->AddMissingBuildings();
	// Check on requirements
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		if (!resInfo->directRequirementsAreSatisfied) {
			std::string msg = std::string("WARNING: some requirements are not satisfied/") + GetResearchLocalizedName(resInfo->researchId);
			traceMessageHandler.WriteMessage(msg);
			this->log += msg;
		}
	}

	// Add (and organize) items to strategy : military units (main ones)
	this->CreateMainMilitaryUnitsElements();
	// Add optional military units (early ages units)
	this->CreateEarlyMilitaryUnitsElements();

	// Choose additional (cheap & useful) researches for "retrains" units - optional
	// Choose additional (cheap & useful) researches for villagers/economy
	this->ChooseOptionalResearches();

	// Add military units requirements (only necessary techs)
	this->CreateMilitaryRequiredResearchesStrategyElements();
	// Add other researches
	this->CreateOtherResearchesStrategyElements();

	// Add researches to strategy
	// Add buildings to strategy (first building of each kind)
	this->CreateFirstBuildingsStrategyElements();
	this->CreateSecondaryBuildingStrategyElements();

	if (buildAI->mainAI->structTradeAI.needGameStartAIInit) {
		// Warning: automatic element insertions WILL be triggered
	} else {
		// TODO : trigger dynamic element insertions ? Houses, (dock+boats?), setGather%... farms?
	}

	// If game is running, search matching units for strategy elements ?

	// TODO : more buildings for main units

	// Ending check
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		if (!resInfo->isInStrategy && resInfo->researchDef && (resInfo->researchDef->researchLocation >= 0)) {
			this->log += "Warning: Not added: research=";
			this->log += GetResearchLocalizedName(resInfo->researchId);
			this->log += newline;
		}
	}
	for each (PotentialBuildingInfo *bldInfo in this->potentialBuildingsList)
	{
		if (bldInfo->addedInStrategyCount == 0) {
			this->log += "Warning: Not added: building=";
			this->log += bldInfo->unitDef->ptrUnitName;
			this->log += newline;
		}
	}
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		if (unitInfo->isSelected && (unitInfo->firstStratElem == NULL)) {
			this->log += "Warning: Not added: unit=";
			this->log += unitInfo->unitName;
			this->log += newline;
		}
	}
#ifdef _DEBUG
	traceMessageHandler.WriteMessage(this->log);
#endif
}


// Searches researches that impact a specific unit and add them to internal list of potential researches.
// Searches recursively required researches EXCEPT for "optional" requirements. No decision is made here.
// allUpgrades: if true, all related upgrades will be added. Otherwise, only requirements will be added.
// Returns the number of actually added elements to list (some might already be in list before)
// You may need to call UpdateRequiredBuildingsFromValidatedResearches, UpdateMissingResearchRequirements and AddMissingBuildings() afterwards
int StrategyBuilder::CollectResearchInfoForUnit(short int unitDefId, bool allUpgrades) {
	if (!ai || !ai->IsCheckSumValid()) { return 0; }
	if (!player || !player->IsCheckSumValid()) { return 0; }
	if (!player->ptrResearchesStruct || !player->ptrResearchesStruct->ptrResearchDefInfo ||
		!player->ptrResearchesStruct->ptrResearchDefInfo->researchDefArray) {
		return 0;
	}
	if ((unitDefId < 0) || (unitDefId >= player->structDefUnitArraySize)) { return 0; }
	std::vector<short int> researchesForUnit;
	if (allUpgrades) {
		researchesForUnit = FindResearchesThatAffectUnit(player, unitDefId, true);
	} else {
		short int researchId = FindResearchThatEnableUnit(player, unitDefId, 0);
		if (researchId == -1) { return 0; }
		researchesForUnit.push_back(researchId);
	}
	// Consider unit's train location too !
#pragma message("TODO: Add this missing part in original method in strategy.cpp")
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_BASE *unitDefBase = player->GetUnitDefBase(unitDefId);
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *unitDefLiving = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *)unitDefBase;
	if (unitDefLiving && unitDefLiving->IsCheckSumValidForAUnitClass() && unitDefLiving->DerivesFromLiving()) {
		short int trainLocation = unitDefLiving->trainLocation;
		if (trainLocation == CST_UNITID_BUILDER) { trainLocation = -1; }
		if (trainLocation >= 0) {
			short int researchId = FindResearchThatEnableUnit(player, trainLocation, 0);
			if (researchId == -1) { return 0; }
			researchesForUnit.push_back(researchId);
		}
	}

	std::vector<short int> allResearchesForUnit = GetValidOrderedResearchesListWithDependencies(player, researchesForUnit);
	// Important note: for "shadow" researches with optional requirements (buildings for ages, etc), requirements are not analyzed yet.
	// We'll have to manage them ourselves.
	// Let's sort them out

	int addedElements = 0;
	for each (short int resDefId in allResearchesForUnit)
	{
		ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef = GetResearchDef(player, resDefId);
		if (resDef) {
			PotentialResearchInfo *resInfo = this->GetResearchInfo(resDefId);
			if (resInfo == NULL) {
				if (AddPotentialResearchInfoToList(resDefId)) {
					addedElements++;
					resInfo = this->GetResearchInfo(resDefId);
				}
			}
			if (resInfo) {
				resInfo->impactedUnitDefIds.insert(unitDefId);
			} else {
				assert(false && "ERROR: research info was not added for resId=");
				this->log += "ERROR: research info was not added for resId=";
				this->log += std::to_string(resDefId);
				this->log += newline;
			}
		}
	}
	if (!allUpgrades) {
		// Get All related researches, even optional ones (NOT for adding)
		std::vector<short int> allResearchesImpactingUnit = FindResearchesThatAffectUnit(player, unitDefId, true); // Get ALL related researches...
		for each (short int resDefId in allResearchesImpactingUnit)
		{
			PotentialResearchInfo *resInfo = this->GetResearchInfo(resDefId);
			if (resInfo) {
				resInfo->impactedUnitDefIds.insert(unitDefId); // just add reference to this unit if research already exists in list. Don't add anything here.
			}
		}
	}
	
	return addedElements;
}


// Adds all necessary buildings for "validated" researches to buildings ID list.
// Returns number of building IDs added to internal list
int StrategyBuilder::UpdateRequiredBuildingsFromValidatedResearches() {
	int addedElements = 0;
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		if (resInfo->markedForAdd && resInfo->researchDef) {
			// Research's location
			if ((resInfo->researchDef->researchLocation >= 0) &&
				(!IsUnitDisabledInTechTree(this->player->playerId, resInfo->researchDef->researchLocation))) {
				// This only adds if not already in list
				if (this->AddPotentialBuildingInfoToList(resInfo->researchDef->researchLocation)) {
					addedElements++;
				}
			}
			// Building that enables research
			ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *parentBld = FindBuildingDefThatEnablesResearch(this->player, resInfo->researchId);
			if (parentBld) {
				if (this->AddPotentialBuildingInfoToList(parentBld->DAT_ID1)) {
					addedElements++;
				}
			}
		}
	}
	return addedElements;
}

// Update all researches requirement statuses and set directRequirementsAreSatisfied if OK.
// Takes into account "confirmed" researches from list and also buildings ("initiates research").
// This does not add anything to our lists.
void StrategyBuilder::UpdateMissingResearchRequirements() {
	assert(player);
	if (!player) { return; }
	// Update using building's "initiate research" field (for buildings that actually ARE in my list)
	for each (PotentialBuildingInfo *bldInfo in this->potentialBuildingsList)
	{
		if (bldInfo->unitDef && bldInfo->unitDef->IsCheckSumValid()) {
			if (bldInfo->unitDef->initiatesResearch >= 0) {
				for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
				{
					for (int currentSlotId = 0; currentSlotId < 4; currentSlotId++) {
						if (resInfo->missingRequiredResearches[currentSlotId] >= 0) {
							if (resInfo->missingRequiredResearches[currentSlotId] == bldInfo->unitDef->initiatesResearch) {
								resInfo->missingRequiredResearches[currentSlotId] = -1; // This is now satisfied.
							}
						}
					}
				}
			}
		}
	}

	// Update research dependencies with other researches
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList) {
		//if (!resInfo->directRequirementsAreSatisfied) {
		for (int i = 0; i < 4; i++) {
			if (resInfo->missingRequiredResearches[i] >= 0) {
				for each (PotentialResearchInfo *confirmedResearch in this->potentialResearchesList) {
					if (confirmedResearch->markedForAdd) {
						if (resInfo->missingRequiredResearches[i] == confirmedResearch->researchId) {
							resInfo->missingRequiredResearches[i] = -1; // requirement is satisfied
						}
					}
				}
			}
		}
		//}
	}

	// Test if researches have remaining "missing" requirements. If not, update status
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList) {
		this->UpdatePotentialResearchStatusFromMissingRequirements(resInfo);
	}
}

// Set resInfo->directRequirementsAreSatisfied=true and resInfo->missingRequiredResearches[...]=-1 if (minimum) requirements are satisfied
void StrategyBuilder::UpdatePotentialResearchStatusFromMissingRequirements(PotentialResearchInfo *resInfo) {
	assert(resInfo->researchDef != NULL);
	if (resInfo->researchDef == NULL) { return; } // Should never happen
	int satisfiedCount = 0;
	for (int i = 0; i < 4; i++) {
		if ((resInfo->researchDef->requiredResearchId[i] >= 0) && (resInfo->missingRequiredResearches[i] == -1)) {
			satisfiedCount++;
		}
	}
	if (satisfiedCount >= resInfo->researchDef->minRequiredResearchesCount) {
		resInfo->directRequirementsAreSatisfied = true;
		for (int i = 0; i < 4; i++) {
			resInfo->missingRequiredResearches[i] = -1; // ignore the remaining (unnecessary) requirements
		}
	}
}

// Add missing buildings - if any - that block some research requirements
void StrategyBuilder::AddMissingBuildings() {
	assert(this->global && this->player);
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		std::list<ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING*> missingBuildingsDef;
		missingBuildingsDef.clear();
		if (!resInfo->directRequirementsAreSatisfied) {
			for (int i = 0; i < 4; i++) {
				if (resInfo->missingRequiredResearches[i] >= 0) {
					ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *bldDef = FindBuildingDefThatEnablesResearch(this->player, resInfo->missingRequiredResearches[i]);
					if (bldDef && bldDef->IsCheckSumValid()) {
						bool found = false;
						// We found a building that satisfies a requirement. Is this building (already) in my list
						// (if member info had been recomputed, it should not happen here because resInfo->missingRequiredResearches[i] would be -1)
						for each (PotentialBuildingInfo *bldInfo in this->potentialBuildingsList)
						{
							if (bldInfo->unitDefId == bldDef->DAT_ID1) {
								found = true;
							}
						}
						if (!found) {
							missingBuildingsDef.push_back(bldDef);
						}
					}
				}
			}

			// We got a list of buildings that satisfy missing requirements. Pick some until it is ok
			bool useTemple = false;
			for each (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *bldDef in missingBuildingsDef)
			{
				// This loop is here to give priority to temple, if relevant
				if (bldDef->DAT_ID1 == CST_UNITID_TEMPLE) {
					useTemple = true;
				}
			}
			if (useTemple) {
				auto it = std::remove_if(missingBuildingsDef.begin(), missingBuildingsDef.end(),
					[](ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *bldDef) { return bldDef->DAT_ID1 == CST_UNITID_TEMPLE; }
				);
				missingBuildingsDef.erase(it, missingBuildingsDef.end());
			}

			int loopCount = 0;
			while (!resInfo->directRequirementsAreSatisfied && (loopCount < 4) && (useTemple || !missingBuildingsDef.empty())) {
				ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *bldDef = NULL;
				if (useTemple) {
					bldDef = (ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *)this->player->GetUnitDefBase(CST_UNITID_TEMPLE);
					useTemple = false;
				} else {
					bldDef = missingBuildingsDef.front();
					missingBuildingsDef.pop_front(); // remove the item we just read
				}
				if (!IsUnitDisabledInTechTree(this->player->playerId, bldDef->DAT_ID1)) {
					this->AddPotentialBuildingInfoToList(bldDef);
					for (int i = 0; i < 4; i++) {
						if (resInfo->missingRequiredResearches[i] == bldDef->initiatesResearch) {
							resInfo->missingRequiredResearches[i] = -1; // Update requirement as we just satisfied it.
						}
					}
					this->UpdatePotentialResearchStatusFromMissingRequirements(resInfo); // maybe it's ok now.
				}
				loopCount++;
			}
		}
	}
}
