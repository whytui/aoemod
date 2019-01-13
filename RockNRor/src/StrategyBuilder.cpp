#include "../include/StrategyBuilder.h"

using namespace ROCKNROR::STRATEGY;

// Set default value for random factor (a %). Re-initialized in the beginning of each generation
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
	this->potentialResearchesList.clear();
}


// Get the strategy element that correspond to an Age Id. researchId must correspond to an age upgrade !
// For stone age, returns the "fake" first strategy element (beginning of strategy)
// For other researches, returns NULL
AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *StrategyBuilder::GetAgeStrategyElement(short int researchId) {
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


// Safely get a SNNumber value. As game has not started yet, we should not use tacAI numbers ! They are not up to date yet.
long int StrategyBuilder::GetSNNumberValue(AOE_CONST_FUNC::SN_NUMBERS snNumber) {
	if (!this->ai || (snNumber < 0) || (snNumber > SN_NUMBERS::SN_UNKNOWN225)) { return -1; }
	return this->ai->structStrategyAI.SNNumber[snNumber];
}


// Add a unit to selection (actuallySelectedUnits) and updates some internal variables accordingly
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
int StrategyBuilder::AddResearchToStrategy(PotentialResearchInfo *resInfo, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *insertionPoint) {
	assert(insertionPoint);
	assert(resInfo->researchDef != NULL);
#ifdef _DEBUG
	if (!PLAYER::IsResearchEnabledForPlayer(this->player, resInfo->researchId)) {
		ROCKNROR::SYSTEM::StopExecution(_T("STRATEGY builder: unavailable research"), true, true);
	}
#endif
	if (resInfo->isInStrategy || !insertionPoint || !resInfo->researchDef) { return 0; }
	int res = 0;
	// TODO : use technology filter (tech tree/ages)
	bool forceNotInsert = (resInfo->researchId >= CST_RSID_STONE_AGE) && (resInfo->researchId <= CST_RSID_IRON_AGE); // +104 for republic age
	// Do not insert ages as they are added in dedicated method.
	if (!forceNotInsert) {
		if (!resInfo->researchDef->IsShadowResearch()) { // shadow research without a location must not be actually added to strategy
			AddUnitInStrategy_before(this->buildAI, insertionPoint, -1, resInfo->researchDef->researchLocation, TAIUnitClass::AIUCTech, resInfo->researchId, player,
				GetResearchLocalizedName(resInfo->researchId).c_str());
			res++;
			resInfo->actualStrategyElement = insertionPoint->previous;
		}
	}
	resInfo->isInStrategy = true;
	return res;
}


// Add a building to strategy just before supplied insertion point. Updates underlying fields (added count...)
// Returns actual number of element that were added
int StrategyBuilder::AddBuildingToStrategy(PotentialBuildingInfo *bldInfo, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *insertionPoint) {
	assert(bldInfo->desiredCount > bldInfo->addedInStrategyCount);
	assert(insertionPoint != NULL);
	assert((bldInfo->unitDef != NULL) && (bldInfo->unitDefId >= 0));
	int res = 0;
	if ((bldInfo->unitDef == NULL) || (bldInfo->unitDefId < 0) || (insertionPoint == NULL)) { return res; }
	if (bldInfo->desiredCount <= bldInfo->addedInStrategyCount) { return res; }
	if (AddUnitInStrategy_before(this->buildAI, insertionPoint, -1, -1, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding,
		bldInfo->unitDefId, this->player, bldInfo->unitDef->ptrUnitName)) {
		res++;
		this->UpdateBuildingInfoAfterAddInStrategy(bldInfo, insertionPoint->previous);
	}
	return res;
}

// Update a building information object when 1 building has been added to strategy
void StrategyBuilder::UpdateBuildingInfoAfterAddInStrategy(PotentialBuildingInfo *bldInfo, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *newElem) {
	// Update building info underlying fields
	bldInfo->addedInStrategyCount++;
	if (bldInfo->firstAddedInStrategy == NULL) {
		if (newElem == NULL) {
			newElem = FindFirstElementInStrategy(&this->buildAI->fakeFirstStrategyElement, TAIUnitClass::AIUCBuilding, bldInfo->unitDefId);
		}
		bldInfo->firstAddedInStrategy = newElem; // save link to strategy element
	}
	if ((this->seMarket == NULL) && (bldInfo->unitDefId == CST_UNITID_MARKET)) {
		this->seMarket = newElem;
	}
	if ((this->seGovCenter == NULL) && (bldInfo->unitDefId == CST_UNITID_GOVERNMENT_CENTER)) {
		this->seGovCenter = newElem;
	}
}

// Returns a pointer to the PotentialResearchInfo object for a research, or NULL if not found.
PotentialResearchInfo *StrategyBuilder::GetResearchInfo(short int researchId) const {
	for each (PotentialResearchInfo *curResInfo in this->potentialResearchesList) {
		if (curResInfo->researchId == researchId) { return curResInfo; }
	}
	return NULL;
}
// Returns a pointer to the PotentialResearchInfo object for a research, or NULL if not found.
PotentialResearchInfo *StrategyBuilder::GetResearchInfo(AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef) const {
	for each (PotentialResearchInfo *curResInfo in this->potentialResearchesList) {
		if (curResInfo->researchDef == resDef) { return curResInfo; }
	}
	return NULL;
}

// Add research to potential researches list and initializes underlying info.
// Returns NULL if not added (error), or pointer to object if successful OR ALREADY existing
// Adds all requirements too (researches, buildings)
PotentialResearchInfo *StrategyBuilder::AddPotentialResearchInfoToList(short int researchId, bool markForAdd) {
	AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = this->player->GetResearchDef(researchId);
	if (!resDef || !this->global || !this->player) { return NULL; }
	// Check availability (tech tree)
	if (!this->IsResearchInTechTree(researchId)) {
		this->log += "Trying to add a disabled research to potential researches list (";
		this->log += std::to_string(researchId);
		this->log += ") => ignored";
		this->log += newline;
		return NULL; // not available = do not add
	}
	PotentialResearchInfo *resInfo = this->GetResearchInfo(resDef);
	if (resInfo != NULL) {
		if (markForAdd) {
			resInfo->markedForAdd = true;
		}
		return resInfo; // already in list: do not add
	}
	TTDetailedResearchDef *resDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(researchId);
	// Do not exclude shadow research, let us analyze their requirements. We won't add shadow res. in strategy anyway.

	resInfo = new PotentialResearchInfo();
	resInfo->researchDef = resDef;
	resInfo->researchId = researchId;
	resInfo->markedForAdd = markForAdd;
	resInfo->detailedResInfo = resDtl;
	if (resDtl && resDtl->active) {
		resInfo->langName = resDtl->langName;
	} else {
		if (resDef->researchName) {
			resInfo->langName = resDef->researchName;
		}
		if (resDef->languageDLLName >= 0) {
			resInfo->langName = GetLanguageDllText(resDef->languageDLLName);
		}
	}
	resInfo->hasOptionalRequirements = ResearchHasOptionalRequirements(resDef);
	resInfo->directRequirementsAreSatisfied = !resInfo->hasOptionalRequirements;
	potentialResearchesList.push_back(resInfo);
	resInfo->totalCosts = 0;
	if (resDef->costUsed1 > 0) { resInfo->totalCosts += resDef->costAmount1; }
	if (resDef->costUsed2 > 0) { resInfo->totalCosts += resDef->costAmount2; }
	if (resDef->costUsed3 > 0) { resInfo->totalCosts += resDef->costAmount3; }


	// *********************************
	// Add research's own requirements !
	if (!resDtl->hasOptionalRequirements && !resDtl->IsAgeResearch()) { // Don't handle age requirements like this, especially because of optional requirements !
		for each(TTDetailedResearchDef *resDtlReqDtl in resDtl->allRequirementsExcludingAges) {
			if (resDtlReqDtl) {
				PotentialResearchInfo *resDtlReqPotentialInfo = this->GetResearchInfo(resDtlReqDtl->researchDef);
				if (resDtlReqDtl->hasOptionalRequirements || resDtlReqDtl->IsAgeResearch()) {
					continue; // Don't handle age requirements like this, especially because of optional requirements !
				}
				if (resDtlReqPotentialInfo == NULL) {
					// Need to add a requirement -> recursive call
					this->AddPotentialResearchInfoToList((short int)resDtlReqDtl->GetResearchDefId(), markForAdd);
				} else {
#ifdef _DEBUG
					if (!resDtlReqPotentialInfo->markedForAdd) {
#endif
						resDtlReqPotentialInfo->markedForAdd = true;
#ifdef _DEBUG
					}
#endif
				}
			}
		}
	}

	// Check that building that enable "this" research is available and its requirements too
	for each (TTDetailedBuildingDef *bld in resDtl->triggeredByBuilding) {
		if (!bld->IsValid() || (bld->possibleAncestorUnitIDs.size() > 0)) {
			continue; // Ignore invalid and "non-root" buildings 
		}
		auto search = std::find_if(this->potentialBuildingsList.cbegin(), this->potentialBuildingsList.cend(),
			[bld](PotentialBuildingInfo *info){ return bld->baseUnitId == info->unitDefId; });
		if (search == this->potentialBuildingsList.cend()) {
			if (resDtl->hasOptionalRequirements || resDtl->IsAgeResearch()) {
				continue; // Don't handle age requirements like this, especially because of optional requirements !
			}
			// This building is not known yet in potential buildings list
			this->AddPotentialBuildingInfoToList(bld->unitDef, markForAdd); // does not handle any dependency
			if (bld->researchIdsThatEnableMe.size() > 0) {
				short int bldReqResId = (short int)*bld->researchIdsThatEnableMe.begin();
				this->AddPotentialResearchInfoToList(bldReqResId, markForAdd); // This will add the necessary requirements
			}
		}
	}
	// *********************************


	AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = global->GetTechDef(resDef->technologyId);
	resInfo->techDef = techDef;
	// Copy & handle requirements in member fields
	int currentSlotId = 0;
	for (int i = 0; i < 4; i++) {
		short int curReqResId = resDef->requiredResearchId[i];
		if (curReqResId >= 0) {
			resInfo->missingRequiredResearches[currentSlotId] = curReqResId;
			currentSlotId++;
			resInfo->researchesThatMustBePutBeforeMe.insert(curReqResId);
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *tmpUnitDef = FindBuildingDefThatEnablesResearch(this->player, curReqResId);
			if (tmpUnitDef && tmpUnitDef->IsCheckSumValidForAUnitClass()) {
				resInfo->unitsThatMustBePutBeforeMe.insert(tmpUnitDef->DAT_ID1);
			}
		}
	}
	// Search for required age
	resInfo->requiredAge = FindResearchRequiredAge(this->player, resInfo->researchId);
	// Ages are added to strategy at the very beginning of the process, so we can already retrieve "my age" strategy element
	if (resInfo->requiredAge >= 0) {
		resInfo->requiredAgeResearch = FindFirstElementInStrategy(&this->buildAI->fakeFirstStrategyElement, AIUCCritical, resInfo->requiredAge);
		if (resInfo->requiredAgeResearch == NULL) {
			resInfo->requiredAgeResearch = FindFirstElementInStrategy(&this->buildAI->fakeFirstStrategyElement, AIUCTech, resInfo->requiredAge);
		}
	}
	// Update list of impacted units
	AOE_TECHNOLOGIES::TechFilterExcludeTechsWithDrawbacks filter;
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		TTDetailedUnitDef *unitDetails = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedUnitDef(unitInfo->unitDefId);
		TTDetailedResearchDef *resDetails = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resInfo->researchId);
		if (!unitDetails || !unitDetails->IsValid() || !resDetails || !resDetails->active) { continue; }
		auto it = std::find(unitDetails->affectedByResearches.begin(), unitDetails->affectedByResearches.end(), resDetails);
		if (it != unitDetails->affectedByResearches.end()) {
			TTDetailedResearchDef *r = *it;
			if ((!settings->isDeathMatch && !r->CanExcludeInRandomMapAI()) ||
				(settings->isDeathMatch && !r->CanExcludeInDeathMatchAI())) {
				resInfo->impactedUnitDefIds.insert(unitInfo->unitDefId);
			}
		}
	}
	for each (PotentialBuildingInfo *bldInfo in this->potentialBuildingsList)
	{
		TTDetailedUnitDef *unitDetails = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedUnitDef(bldInfo->unitDefId);
		TTDetailedResearchDef *resDetails = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resInfo->researchId);
		if (!unitDetails || !unitDetails->IsValid() || !resDetails || !resDetails->active) { continue; }
		auto it = std::find(unitDetails->affectedByResearches.begin(), unitDetails->affectedByResearches.end(), resDetails);
		if (it != unitDetails->affectedByResearches.end()) {
			TTDetailedResearchDef *r = *it;
			if ((!settings->isDeathMatch && !r->CanExcludeInRandomMapAI()) ||
				(settings->isDeathMatch && !r->CanExcludeInDeathMatchAI())) {
				resInfo->impactedUnitDefIds.insert(bldInfo->unitDefId);
			}
		}
	}
	// Special treatments
	if (resInfo->techDef) {
		for (int i = 0; i < resInfo->techDef->effectCount; i++) {
			if (resInfo->techDef->ptrEffects[i].IsAttributeModifier() &&
				(resInfo->techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_POPULATION_COST)) {
				// Logistics (standard game): in random games, set lowest priority
				if (!this->settings->isDeathMatch) {
					resInfo->forcePutAfterOtherResearches = true;
				}
			}
		}
	}

	return resInfo;
}

// Returns a pointer to the PotentialBuildingInfo object for a research, or NULL if not found.
PotentialBuildingInfo *StrategyBuilder::GetBuildingInfo(short int unitDefId) const {
	for each (PotentialBuildingInfo *curBldInfo in this->potentialBuildingsList) {
		if (curBldInfo->unitDefId == unitDefId) { return curBldInfo; }
	}
	return NULL;
}
// Returns a pointer to the PotentialBuildingInfo object for a research, or NULL if not found.
PotentialBuildingInfo *StrategyBuilder::GetBuildingInfo(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef) const {
	for each (PotentialBuildingInfo *curBldInfo in this->potentialBuildingsList) {
		if (curBldInfo->unitDef == unitDef) { return curBldInfo; }
	}
	return NULL;
}
// Add building to potential buildings list and initilizes underlying info.
// Returns true if actually added
bool StrategyBuilder::AddPotentialBuildingInfoToList(short int unitDefId, bool markForAdd) {
	if (unitDefId < 0) { return false; }
	AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)player->GetUnitDefBase(unitDefId);
	if (unitDef && unitDef->IsTypeValid()) {
		return this->AddPotentialBuildingInfoToList(unitDef, markForAdd);
	}
	return false;
}
bool StrategyBuilder::AddPotentialBuildingInfoToList(AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef, bool markForAdd) {
	if (!unitDef || !unitDef->IsTypeValid()) { return false; }
	PotentialBuildingInfo *bldInfo = this->GetBuildingInfo(unitDef->DAT_ID1);
	if (bldInfo != NULL) {
		if (markForAdd) { bldInfo->markedForAdd = true; }
		return false;
	}
	bldInfo = new PotentialBuildingInfo();
	bldInfo->unitDef = unitDef;
	bldInfo->unitDefId = unitDef->DAT_ID1;
	bldInfo->langName = unitDef->ptrUnitName;
	bldInfo->unitDefDetailedInfo = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(unitDef->DAT_ID1);
	assert(bldInfo->unitDefDetailedInfo && bldInfo->unitDefDetailedInfo->IsValid());
	if (bldInfo->unitDefDetailedInfo && bldInfo->unitDefDetailedInfo->IsValid()) {
		bldInfo->langName = bldInfo->unitDefDetailedInfo->langName;
	}
	if (bldInfo->unitDefDetailedInfo->researchIdsThatEnableMe.size() > 0) {
		bldInfo->enabledByResearchId = (short int) *bldInfo->unitDefDetailedInfo->researchIdsThatEnableMe.begin();//FindResearchThatEnableUnit(this->player, unitDef->DAT_ID1, 0);
	}
	bldInfo->desiredCount = 1;
	bldInfo->markedForAdd = markForAdd;
	AOE_STRUCTURES::STRUCT_RESEARCH_DEF *parentResDef = player->GetResearchDef(bldInfo->enabledByResearchId);
	if (parentResDef && (bldInfo->enabledByResearchId >= 0)) {
		bldInfo->enabledInAge = GetAgeResearchFromDirectRequirement(parentResDef);
	} else {
		bldInfo->enabledInAge = -1;
	}

	if (bldInfo->unitDefDetailedInfo && (bldInfo->unitDefDetailedInfo->researchIdsThatEnableMe.size() > 0)) {
		bldInfo->enabledByResearchId = (short int)*bldInfo->unitDefDetailedInfo->researchIdsThatEnableMe.begin();
		// By the way, save all requirements to requiredResearchesForBaseUnit. Do not add "enabledByResearchId" to "requiredResearchesForBaseUnit"
		TTDetailedResearchDef *reqDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(bldInfo->enabledByResearchId);
		if (reqDtl && reqDtl->active) {
			for each(TTDetailedResearchDef *r in reqDtl->allRequirementsExcludingAges) {
				if (!r->IsShadowResearch() && !r->IsAgeResearch()) { // ignore ages and shadow reearches...
					bldInfo->requiredResearchesForBaseUnit.insert((short int)r->GetResearchDefId());
				}
			}
		}
	}

	// Is it a high-priority building ?
	if (unitDef->initiatesResearch >= 0) {
		AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = this->player->GetResearchDef(unitDef->initiatesResearch);
		if (resDef && (resDef->technologyId >= 0)) {
			AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = global->GetTechDef(resDef->technologyId);
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


// Returns a pointer to the PotentialUnitInfo for a unit
PotentialUnitInfo *StrategyBuilder::GetUnitInfo(short int unitDefId) const {
	for each (PotentialUnitInfo *curUnitInfo in this->potentialUnitsList) {
		if (curUnitInfo->unitDefId == unitDefId) { return curUnitInfo; }
	}
	return NULL;
}


// Returns true if a research is available in tech tree
bool StrategyBuilder::IsResearchInTechTree(short int researchId) {
	return !this->playerTechTreeInfo->IsResearchDisabled(researchId);
}

// Updates this->mustBeBeforeThisElem and this->mustBeAfterThisElem according to known dependencies on other unit/researches
// Previous values of mustBeBeforeThisElem  and mustBeAfterThisElem are reset (lost)
void PotentialResearchInfo::ComputeStratElemPositionConstraints(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI) {
	if (!buildAI) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = buildAI->mainAI->ptrStructPlayer;
	if (!player) { return; }
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeFirst = &buildAI->fakeFirstStrategyElement;
	this->mustBeBeforeThisElem = NULL;
	this->mustBeAfterThisElem = NULL;

	// Standard calculation of mustBeAfterThisElem and mustBeBeforeThisElem
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = buildAI->fakeFirstStrategyElement.next;
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
		assert(player && player->IsCheckSumValid());
		if (!player || !player->IsCheckSumValid()) { return; }
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *bestFirstElem = this->mustBeAfterThisElem; // use previous calculations for initial position
		if (bestFirstElem && (bestFirstElem->next != &buildAI->fakeFirstStrategyElement)) {
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
				if (unitDefBase && unitDefBase->IsCheckSumValidForAUnitClass() && unitDefBase->DerivesFromTrainable()) {
					if (unitDefBase->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) {
						unitClass = TAIUnitClass::AIUCBuilding;
					}
					if (unitDefBase->unitType == GLOBAL_UNIT_TYPES::GUT_TRAINABLE) {
						unitClass = TAIUnitClass::AIUCLivingUnit;
					}
					if (unitDefBase->unitAIType == TribeAIGroupCivilian) { continue; } // ignore villager so we won't force the research BEFORE villagers ! (nonsense)
				}
				AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *firstElemThisUnit = FindFirstElementInStrategy(fakeFirst, unitClass, unitDefId);
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
			AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = bestFirstElem;
			// Just move a bit further - except if reaching strategy end !
			while (curElem && (elemCount < maxElemCount) && (curElem->next != &buildAI->fakeFirstStrategyElement)) {
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
	if (this->forcePutAfterOtherResearches) {
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = this->mustBeBeforeThisElem;
		if (!curElem) {
			curElem = buildAI->fakeFirstStrategyElement.previous;
		}
		bool foundStratElem = false;
		while (curElem && !foundStratElem && (curElem != &buildAI->fakeFirstStrategyElement) &&
			(curElem != this->mustBeAfterThisElem)) {
			if ((curElem->elementType == TAIUnitClass::AIUCTech) || (curElem->elementType == TAIUnitClass::AIUCCritical)) {
				short int resDefId = (short int)curElem->unitDAT_ID;
				AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = player->GetResearchDef(resDefId);
				if (resDef) {
					AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = player->ptrGlobalStruct->GetTechDef(resDef->technologyId);
					if (techDef) {
						for each (short int unitDefId in this->impactedUnitDefIds)
						{
							foundStratElem |= ROCKNROR::crInfo.techTreeAnalyzer.DoesResearchAffectUnit(resDefId, unitDefId);
						}
					}
				}
			}	
			curElem = curElem->previous;
		}
		if (foundStratElem) {
			curElem = curElem->next;
			this->mustBeAfterThisElem = curElem;
		}
	}
}



/*** Units selection methods ***/


// Fills unitInfos with ALL available military units from tech tree (only "root" units are listed).
// Towers are ignored (not added to list). Boats are ignored on non-water maps.
// *** Make sure to delete all PotentialUnitInfo from list when you're finished with the list ***
void StrategyBuilder::CollectPotentialUnitsInfo(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid() || !player->ptrStructDefUnitTable || !player->ptrResearchesStruct) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	bool isWaterMap = IsDockRelevantForMap(settings->mapTypeChoice);
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return; }
	if (!global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid()) { return; }
	this->player = player;

	if (!ROCKNROR::crInfo.techTreeAnalyzer.IsReady()) {
		this->log += "ERROR: Tech tree analyzer is not ready !";
		this->log += newline;
		ROCKNROR::SYSTEM::StopExecution(_T("ERROR: Tech tree analyzer is not ready !"), true, true);
		return;
	}
	
	for (int unitDefId = 0; unitDefId < ROCKNROR::crInfo.techTreeAnalyzer.GetUnitDefCount(); unitDefId++) {
		ROCKNROR::STRATEGY::TTDetailedTrainableUnitDef *unitDetailTrainable = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(unitDefId);
		if (!unitDetailTrainable || !unitDetailTrainable->IsValid() || unitDetailTrainable->IsHeroOrScenarioUnit()) { continue; }
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = unitDetailTrainable->GetUnitDef();
		if (!unitDef || !unitDef->DerivesFromTrainable()) { continue; }
		if (unitDef->unitAIType == TribeAIGroupBuilding) { continue; } // Excludes towers and other buildings
		
		// Intentionally exclude "upgrades": we only get the list of "root" units
		if (unitDetailTrainable->possibleAncestorUnitIDs.size() > 0) { continue; }

		// Is unit available in my tech tree ?
		if (this->playerTechTreeInfo->IsTrainableUnitDisabled(unitDefId)) {
			continue;
		}
		/*auto searchDisableUnit = std::find_if(this->playerTechTreeInfo->disabledTrainableUnitInfos.cbegin(),
			this->playerTechTreeInfo->disabledTrainableUnitInfos.cend(), [unitDef](TTDetailedUnitDef *curDtl){ return curDtl->unitDefId == unitDef->DAT_ID1; });
		if (searchDisableUnit != this->playerTechTreeInfo->disabledTrainableUnitInfos.cend()) {
			continue; // unit is disabled (unavailable in tech tree)
		}*/

		// Find best available upgrade
		bool hasUnavailableUpgrade = false;
		TTDetailedTrainableUnitDef *bestUpgradedDetail = unitDetailTrainable; // default: best upgrade = self
		for each (auto curUpgradedId in unitDetailTrainable->possibleUpgradedUnitIDs)
		{
			// possibleUpgradedUnitIDs are ordered: each loop should provide a "better" upgrade than previous one
			auto searchDisableUpgrade = std::find_if(this->playerTechTreeInfo->disabledTrainableUnitInfos.cbegin(),
				this->playerTechTreeInfo->disabledTrainableUnitInfos.cend(), [curUpgradedId](TTDetailedUnitDef *curDtl)
			{ return curDtl->unitDefId == curUpgradedId; }); // finds an element if curUpgradedId is unavailable
			if (searchDisableUpgrade != this->playerTechTreeInfo->disabledTrainableUnitInfos.cend()) {
				hasUnavailableUpgrade = true; // Found a disabled upgrade of our unit
				break; // If this upgrade is disabled, next ones will too
			}
			bestUpgradedDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(curUpgradedId);
			assert(bestUpgradedDetail && bestUpgradedDetail->IsValid());
		}

		PotentialUnitInfo *unitInfo = new PotentialUnitInfo();
		unitInfo->ageResearchId = unitDetailTrainable->requiredAge;
		unitInfo->unitDefId = (short int)unitDetailTrainable->baseUnitId;
		unitInfo->baseUnitDefLiving = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE*) player->GetUnitDefBase((short int)unitDetailTrainable->baseUnitId);
		unitInfo->isBoat = IsWaterUnit(unitDef->unitAIType);
		unitInfo->enabledByResearchId = -1;
		if (unitDetailTrainable->researchIdsThatEnableMe.size() > 0) {
			unitInfo->enabledByResearchId = (short int)*unitDetailTrainable->researchIdsThatEnableMe.begin();
			// By the way, save all requirements to requiredResearchesForBaseUnit. Do not add "enabledByResearchId" to "requiredResearchesForBaseUnit"
			TTDetailedResearchDef *reqDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(unitInfo->enabledByResearchId);
			if (reqDtl && reqDtl->active) {
				for each(TTDetailedResearchDef *r in reqDtl->allRequirementsExcludingAges) {
					if (!r->IsShadowResearch() && !r->IsAgeResearch()) { // ignore ages and shadow reearches...
						unitInfo->requiredResearchesForBaseUnit.insert((short int)r->GetResearchDefId());
					}
				}
			}
		}
		unitInfo->strongestUpgradeUnitEnabledByResearchId = -1;
		if (bestUpgradedDetail->researchIdsThatEnableMe.size() > 0) {
			unitInfo->strongestUpgradeUnitEnabledByResearchId = (short int)*bestUpgradedDetail->researchIdsThatEnableMe.begin();
			// Find required researches for best upgraded unit
			TTDetailedResearchDef *reqUpgrDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(unitInfo->strongestUpgradeUnitEnabledByResearchId);
			if (reqUpgrDtl && reqUpgrDtl->active) {
				for each(TTDetailedResearchDef *r in reqUpgrDtl->allRequirementsExcludingAges) {
					if (!r->IsShadowResearch() && !r->IsAgeResearch()) { // ignore ages and shadow reearches...
						unitInfo->requiredResearchesForStrongestUpgrade.insert((short int)r->GetResearchDefId());
					}
				}
			}
		}

		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *bestUnitDef = bestUpgradedDetail->GetUnitDef();
		unitInfo->hitPoints = bestUnitDef->totalHitPoints;
		unitInfo->displayedAttack = bestUnitDef->displayedAttack;
		unitInfo->range = bestUnitDef->maxRange;
		unitInfo->isMelee = !IsRangedUnit(unitDef); // melee=not priest/not ranged(projectile), not st francis(melee with range)
		unitInfo->speedBase = unitDef->speed; // base speed value
		unitInfo->speedUpgraded = bestUnitDef->speed; // updated speed value
		unitInfo->upgradedUnitDefLiving = bestUnitDef;
		unitInfo->strongestUpgradeUnitDefId = (short int)bestUpgradedDetail->unitDefId;
		unitInfo->unitAIType = unitDef->unitAIType;
		unitInfo->unitName = bestUpgradedDetail->internalName.c_str();
		unitInfo->langName = bestUpgradedDetail->langName;
		unitInfo->conversionResistance = this->crInfo->GetConversionResistance(player->civilizationId, unitDef);
		unitInfo->hasUnavailableUpgrade = hasUnavailableUpgrade;
		unitInfo->unavailableRelatedResearchDetail.clear();
		unitInfo->availableRelatedResearchDetail.clear();
		unitInfo->isSuperUnit = bestUpgradedDetail->isSuperUnit;
		
		if (bestUpgradedDetail->GetUnitDef()->HasBlastDamage()) {
			unitInfo->displayedAttack = (unitInfo->displayedAttack * 115) / 100; // Give a "fake" attack bonus for units with blast damage (15% ?)
		}

		// (un)available researches
		for each (TTDetailedResearchDef *curRes in unitDetailTrainable->affectedByResearches)
		{
			if (!curRes->active || curRes->isAiUnsupported) { continue; }
			long int curResId = curRes->GetResearchDefId();
			auto searchDisableRes = std::find_if(this->playerTechTreeInfo->disabledResearchesInfo.cbegin(),
				this->playerTechTreeInfo->disabledResearchesInfo.cend(), [curResId](TTDetailedResearchDef *curDtl){ return curDtl->GetResearchDefId() == curResId; });
			if (searchDisableRes != this->playerTechTreeInfo->disabledResearchesInfo.cend()) {
				unitInfo->unavailableRelatedResearchDetail.insert(curRes);
			} else {
				unitInfo->availableRelatedResearchDetail.insert(curRes);
				// Take the occasion to apply effect on "best unit", for some specific attributes (speed...)
				for (int i = 0; (curRes->techDef && i < curRes->techDef->effectCount); i++) {
					if (curRes->techDef->ptrEffects[i].IsAttributeModifier()) {
						bool affectsMe = ((curRes->techDef->ptrEffects[i].effectUnit == unitDef->DAT_ID1) ||
							(curRes->techDef->ptrEffects[i].effectClass == unitDef->unitAIType));
						if (affectsMe && (curRes->techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_SPEED)) {
							float value = curRes->techDef->ptrEffects[i].GetValue();
							if (curRes->techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) {
								unitInfo->speedUpgraded += value;
							}
							if (curRes->techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) {
								unitInfo->speedUpgraded = value;
							}
							if (curRes->techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT) {
								unitInfo->speedUpgraded = unitInfo->speedUpgraded * value;
							}
						}
					}
				}
			}
		}

		// Analyze costs
		unitInfo->costsGold = false;
		for (int i = 0; i < 3; i++) {
			if ((unitDef->costs[i].costType >= AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_FOOD) &&
				(unitDef->costs[i].costType <= AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_GOLD) &&
				(unitDef->costs[i].costPaid)) {
				unitInfo->totalResourceCost += unitDef->costs[i].costAmount;
				if (unitDef->costs[i].costType == AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_GOLD) {
					unitInfo->costsGold = true;
				}
				unitInfo->trainCosts[unitDef->costs[i].costType] = unitDef->costs[i].costAmount;
			}
		}
		unitInfo->weightedCost = 0;
		for (int i = 0; i < 4; i++) {
			unitInfo->weightedCost += GetWeightedCostValue(unitInfo->trainCosts[i], (RESOURCE_TYPES)i);
		}
		// Has special attack bonus ?
		unitInfo->hasSpecificAttackBonus = false;
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

		// Retrieve player tech tree
		assert(player->techTreeId < global->technologiesInfo->technologyCount);
		STRUCT_TECH_DEF *techDefTechTree = NULL;
		bool currentUnitHasTechTreeBonus = false;
		if (!ROCKNROR::crInfo.myGameObjects.currentGameHasAllTechs && (player->techTreeId >= 0)) { // a player *might* not have a tech tree.
			techDefTechTree = global->technologiesInfo->GetTechDef(player->techTreeId);
			static AOE_TECHNOLOGIES::TechFilterExcludeDrawbacksAndDistributedEffects filter;
			bool techTreeAffectsCurrentUnit = (techDefTechTree != NULL) && DoesTechAffectUnit(techDefTechTree, unitDef, &filter);
			bool techTreeHasNegativeEffectOnUnit = HasTechNegativeSideEffect(techDefTechTree);
			if (techTreeAffectsCurrentUnit && !techTreeHasNegativeEffectOnUnit) {
				for (int i = 0; i < techDefTechTree->effectCount; i++) {
					if (techDefTechTree->ptrEffects[i].IsDisableUnit(unitDef->DAT_ID1)) {
						assert(false && "unexpected disable unit"); // If unit is disabled, this should have been detected before and we wouldn't be here
					}
				}
				currentUnitHasTechTreeBonus = true; // If not a "disable unit" effect, consider it is a positive effect
			}
		}
		unitInfo->hasCivBonus = currentUnitHasTechTreeBonus;

		unitInfo->isNonTowerMilitary = IsNonTowerMilitaryUnit(unitDef->unitAIType);

#ifdef _DEBUG
		if ((unitInfo->enabledByResearchId >= 0) && (!PLAYER::IsResearchEnabledForPlayer(this->player, unitInfo->enabledByResearchId))) {
			ROCKNROR::SYSTEM::StopExecution(_T("STRATEGY builder: unavailable research"), true, true);
		}
#endif

		// Save unit infos in list
		this->potentialUnitsList.push_back(unitInfo);
	}
}


// Compute all strength/weakness fields for all units in potential units list.
void StrategyBuilder::ComputeStrengthsForPotentialUnits() {
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		for (int i = 0; i < MC_COUNT; i++) {
			unitInfo->strengthVs[i] = 0;
			unitInfo->weaknessVs[i] = 0;
		}
		if (!unitInfo->isNonTowerMilitary) {
			continue;
		}

		// availableRelatedResearchesProportion is a % value (0-100)
		int availableRelatedResearchesProportion = 100;
		if (unitInfo->unavailableRelatedResearchDetail.size() + unitInfo->availableRelatedResearchDetail.size() != 0) {
			availableRelatedResearchesProportion = (unitInfo->availableRelatedResearchDetail.size() * 100) / (unitInfo->unavailableRelatedResearchDetail.size() + unitInfo->availableRelatedResearchDetail.size());
		}
		// A combination of speed and hit points
		float adjustedSpeed = unitInfo->speedUpgraded - 0.4f;
		if (adjustedSpeed < 0.4) { adjustedSpeed = 0.4f; } // This gives more importance to speed (more penalty for slow units): lower difference between speed values = more difference after multiplication
		float speedHitPointsFactor = adjustedSpeed * (float)unitInfo->hitPoints;
		// Examples: elephant=(0.4*600)=240. legion=(0.8*160)=128, axe=(.8*50)=40, cataphract=(1.6*180)=288, scythe=(1.6*137)=219. heavy_cat=(150*0.4)=60
		if (speedHitPointsFactor > 300.0f) { speedHitPointsFactor = 300.0f; }
		if (speedHitPointsFactor < 30.0f) { speedHitPointsFactor = 30.0f; }
		speedHitPointsFactor = speedHitPointsFactor / 3; // get a % value 1-100

		bool hasInefficientProjectile = false;
		if (UnitHasProjectile(unitInfo->upgradedUnitDefLiving)) {
			AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE *projectile = (AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE *)
				this->player->GetUnitDefBase(unitInfo->upgradedUnitDefLiving->projectileUnitId);
			if (projectile) {
				if (GetProjectileType(projectile) == UNIT_PROJECTILE_TYPE::UPT_CATAPULT_STONE) {
					hasInefficientProjectile = true;
				}
			}
		}
		float attackPoints = unitInfo->displayedAttack;
		if (unitInfo->isMelee) {
			attackPoints -= 2; // Melee may hardly catch their targets, targets have armors, etc. +This allow to consider lower attack value is 3.
			if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider) {
				attackPoints -= 4; // To compensate the fact it receives no attack upgrade from storage pit techs.
			}
		}
		if (attackPoints < 1) { attackPoints = 1; }
		int damageScore = 0; // A value 1-4 representing unit attack score. 4 is good, 1 is bad.
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
		unitInfo->damageScore = damageScore;

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
		if (hasInefficientProjectile) {
			unitInfo->strengthVs[MC_FAST_RANGED] -= 5;
			unitInfo->weaknessVs[MC_FAST_RANGED] += 5;
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
			if (hasInefficientProjectile) {
				unitInfo->strengthVs[MC_MELEE] -= 15;
				unitInfo->weaknessVs[MC_MELEE] += 15;
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
		int availableRelatedResearchesProportion = (unitInfo->availableRelatedResearchDetail.size() * 100) / (unitInfo->unavailableRelatedResearchDetail.size() + unitInfo->availableRelatedResearchDetail.size());
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
	UNIT_SPEED_CATEGORY speedCategory = GetUnitSpeedCategory(unitSpeed);
	switch (speedCategory) {
	case AOE_CONST_FUNC::USC_SLOW:
		basePenaltyValue = 20;
		break;
	case AOE_CONST_FUNC::USC_NORMAL:
		basePenaltyValue = 10;
		break;
	case AOE_CONST_FUNC::USC_FAST:
		basePenaltyValue = 2; // Fast units: can defend correctly against priests.
		break;
	default:
		basePenaltyValue = 10;
		break;
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
	if (!unitInfo->isMelee && ((unitSpeed == AOE_CONST_FUNC::USC_FAST) || (unitSpeed == AOE_CONST_FUNC::USC_NORMAL)) && (unitInfo->range > 6)) {
		unitInfo->strengthVs[MC_PRIEST] += 10; // Range units are slightly better vs priests than melee
	} else {
		if (unitInfo->range >= 10) { // siege weapons, actually (archers have 7 max, we don't count range upgrades here)
			unitInfo->strengthVs[MC_PRIEST] += 20;
		}
	}
	if (unitInfo->isMelee && ((unitSpeed == AOE_CONST_FUNC::USC_FAST) || (unitSpeed == AOE_CONST_FUNC::USC_NORMAL))) {
		unitInfo->strengthVs[MC_PRIEST] += 15;
		if (unitInfo->isMelee && (unitSpeed == AOE_CONST_FUNC::USC_FAST)) {
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
	if (unitInfo->upgradedUnitDefLiving && unitInfo->upgradedUnitDefLiving->IsCheckSumValidForAUnitClass() &&
		unitInfo->upgradedUnitDefLiving->DerivesFromTrainable()) {
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
		// in fact, few units have armors (not counting storage pit upgrades). So, few units match this:
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
	int proportion = 100;
	if (unitInfo->availableRelatedResearchDetail.size() + unitInfo->unavailableRelatedResearchDetail.size() != 0) {
		proportion = (unitInfo->availableRelatedResearchDetail.size()* 100) / (unitInfo->availableRelatedResearchDetail.size() + unitInfo->unavailableRelatedResearchDetail.size());
	}
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
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupHero: // Heroes only ?!
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
		unitInfo->globalScoreEarlyAge = 0;
		if (!unitInfo->isNonTowerMilitary) {
			continue;
		}
		if (unitInfo->damageScore == 1) {
			unitInfo->globalScoreEarlyAge = 20;
		}
		if (unitInfo->damageScore == 2) {
			unitInfo->globalScore = 30;
			unitInfo->globalScoreEarlyAge = 50;
		}
		if (unitInfo->damageScore == 3) {
			unitInfo->globalScore = 55;
			unitInfo->globalScoreEarlyAge = 70;
		}
		if (unitInfo->damageScore == 4) {
			unitInfo->globalScore = 75;
			unitInfo->globalScoreEarlyAge = 90;
		}
		if (unitInfo->isSuperUnit) {
			unitInfo->globalScore = 90;
			// Early age: no impact
		}
		if (unitInfo->hasCivBonus) {
			unitInfo->globalScore += 25; // note: we can go > 100 at this point, not an issue
			unitInfo->globalScoreEarlyAge += 35; // even more useful in early ages
		}
		if (unitInfo->hasUnavailableUpgrade) {
			unitInfo->globalScore = (unitInfo->globalScore * 85) / 100; // -15% if unit upgrades are missing
			// Early age: no impact
		}
		int numberOfAvailableUpgrades = unitInfo->upgradesUnitDefId.size();
		// TODO: example: assyrian cat is super unit (all unit upgrades) but missing 2 techs/5 => not selected
		int proportion = 100;
		if (unitInfo->availableRelatedResearchDetail.size() + unitInfo->unavailableRelatedResearchDetail.size() != 0) {
			proportion = (unitInfo->availableRelatedResearchDetail.size() * 100) / (unitInfo->availableRelatedResearchDetail.size() + unitInfo->unavailableRelatedResearchDetail.size());
		}
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

		// Range units (siege): decrease score for slow/inefficient projectiles (catapults !)
		if (unitInfo->baseUnitDefLiving && (UnitHasProjectile(unitInfo->baseUnitDefLiving))) {
			AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE *proj = (AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE *)this->player->GetUnitDefBase(unitInfo->baseUnitDefLiving->projectileUnitId);
			UNIT_PROJECTILE_TYPE pType = GetProjectileType(proj);
			if (pType == UNIT_PROJECTILE_TYPE::UPT_CATAPULT_STONE) {
				unitInfo->globalScore = (unitInfo->globalScore * 90 / 100);
				unitInfo->globalScoreEarlyAge = (unitInfo->globalScoreEarlyAge * 80 / 100);
			}
			if (unitInfo->baseUnitDefLiving->accuracyPercent < 100) { // not in standard game
				unitInfo->globalScore = (unitInfo->globalScore * 90 / 100);
				unitInfo->globalScoreEarlyAge = (unitInfo->globalScoreEarlyAge * 90 / 100);
			}
		}

		// Adjust (a bit) according to costs: cheaper units are less strong (lower score), this is a bit unfair. Ex legion underestimated, heavy cats overestimated.
		int costProportion = (unitInfo->weightedCost * 100) / highestWeightedCost; // 0 to 100 value representing 1 unit cost.
		costProportion = costProportion / 10; // we limit "cost" impact to maximum 10% of global score. low value=low cost=better score
		costProportion = 100 - costProportion; // Get values in 90%-100% interval, where 100% is the better score. Very approximate due to int type, but the whole thing IS approximate.
		unitInfo->globalScore = unitInfo->globalScore * costProportion / 100;
		unitInfo->globalScoreEarlyAge = unitInfo->globalScoreEarlyAge * costProportion / 100;

		// Adjustments for deathmatch
		if (this->settings->isDeathMatch) {
			// Exclude tool age units like scouts
			TTDetailedTrainableUnitDef *upgradeDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(unitInfo->strongestUpgradeUnitDefId);
			if (upgradeDtl && (upgradeDtl->requiredAge < AOE_CONST_FUNC::CST_RSID_BRONZE_AGE)) {
				unitInfo->globalScore = 0;
			}
		}

		if (unitInfo->globalScore < 0) { unitInfo->globalScore = 0; }
		if (unitInfo->globalScore > 100) { unitInfo->globalScore = 100; }
		if (unitInfo->globalScoreEarlyAge < 0) { unitInfo->globalScoreEarlyAge = 0; }
		if (unitInfo->globalScoreEarlyAge > 100) { unitInfo->globalScoreEarlyAge = 100; }

		// Add a random part
		int random = randomizer.GetRandomValue_normal_moreFlat(0, randomPercentFactor * 2);
		random = random + 100 - randomPercentFactor; // in [100-randomImpact, 100+randomImpact] (used with a multiplication, it gives a resulting randomImpact% adjustment + or -)
		unitInfo->globalScore = (unitInfo->globalScore * random) / 100;

		if (unitInfo->globalScore > 100) { unitInfo->globalScore = 100; }

		random = randomizer.GetRandomValue_normal_moreFlat(0, randomPercentFactor * 2);
		random = random + 100 - randomPercentFactor; // in [100-randomImpact, 100+randomImpact] (used with a multiplication, it gives a resulting randomImpact% adjustment + or -)
		unitInfo->globalScoreEarlyAge = (unitInfo->globalScoreEarlyAge * random) / 100;

		if (unitInfo->globalScoreEarlyAge > 100) { unitInfo->globalScoreEarlyAge = 100; }
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


// Recompute unit info bonuses that depend on comparison with other units: rare strengths, balanced cost
// Does not recompute unit strengths (but computes it using the correct strength values)
// Does not recompute global score
// upgradedUnit: if true, consider upgraded unit. if false, consider base unit
#pragma WARNING("No very relevant for boats, do a specific method ?")
void StrategyBuilder::RecomputeComparisonBonuses(std::list<PotentialUnitInfo*> selectedUnits, bool waterUnit, bool upgradedUnit,
	short int maxAgeResearchId) {
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
		if ((!unitInfo->isSelected) && unitInfo->isNonTowerMilitary && (unitInfo->ageResearchId <= maxAgeResearchId)) {
			unitInfo->bonusForUsedResourceTypes = this->GetCostScoreRegardingCurrentSelection(unitInfo);
		}
	}
	if (maxAgeResearchId == CST_RSID_TOOL_AGE) {
		// Allow only 1 melee and 1 range (max) in tool age (reset scores to prevent adding such units)
		bool hasMeleeInTool = false;
		bool hasRangedInTool = false;
		for each (PotentialUnitInfo *unitInfo in selectedUnits)
		{
			if (unitInfo->isSelected && unitInfo->isNonTowerMilitary && (unitInfo->ageResearchId <= CST_RSID_TOOL_AGE)) {
				if (unitInfo->isMelee) {
					hasMeleeInTool = true;
				} else {
					hasRangedInTool = true;
				}
			}
		}
		if (hasMeleeInTool || hasRangedInTool) {
			for each (PotentialUnitInfo *unitInfo in selectedUnits) {
				if (unitInfo->isNonTowerMilitary && !unitInfo->isSelected && (unitInfo->ageResearchId <= maxAgeResearchId)) {
					if (hasMeleeInTool && unitInfo->isMelee) {
						unitInfo->bonusForRareStrength = 0;
						unitInfo->bonusForUsedResourceTypes = 0;
						unitInfo->globalScoreEarlyAge = 0;
						this->log += "Disabled unit because of tool age similarity: ";
						this->log += localizationHandler.GetTranslation(unitInfo->baseUnitDefLiving->languageDLLID_Name, unitInfo->unitName);
						this->log += newline;
					}
				}
			}
		}
	}
}

// Get the additional farm production amount for a research and underlying required researches
float StrategyBuilder::GetFarmProductionBonusRecursive(PotentialResearchInfo *resInfo, float supposedCurrentProduction) const {
	if (resInfo == NULL) { return 0; }
	float currentProduction = supposedCurrentProduction;
	for each (short int reqResId in resInfo->researchesThatMustBePutBeforeMe)
	{
		PotentialResearchInfo *reqResInfo = this->GetResearchInfo(reqResId);
		if (reqResInfo) {
			currentProduction += this->GetFarmProductionBonusRecursive(reqResInfo, currentProduction); // Underlying dependencies
			currentProduction += this->GetFarmProductionBonus(reqResInfo->researchId, currentProduction); // this required research farm bonus
		}
	}
	return currentProduction - supposedCurrentProduction;
}
// Get the additional farm production amount for a research (not recursive)
float StrategyBuilder::GetFarmProductionBonus(short int researchId, float supposedCurrentProduction) const {
	if (researchId < 0) { return 0; }
	PotentialResearchInfo *resInfo = this->GetResearchInfo(researchId);
	if (resInfo == NULL) { return 0; }
	return this->GetFarmProductionBonusForTech(resInfo->researchDef->technologyId, supposedCurrentProduction);
}
// Get the additional farm production amount for a technology (not recursive)
float StrategyBuilder::GetFarmProductionBonusForTech(short int techId, float supposedCurrentProduction) const {
	if (techId < 0) { return 0; }
	AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = this->global->GetTechDef(techId);
	for (int i = 0; (techDef != NULL) && (i < techDef->effectCount); i++) {
		if ((techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET) &&
			(techDef->ptrEffects[i].effectUnit == CST_RES_ORDER_FARM_FOOD_AMOUNT)) {
			return techDef->ptrEffects[i].effectValue;
		}
		if ((techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_MULT) &&
			(techDef->ptrEffects[i].effectUnit == CST_RES_ORDER_FARM_FOOD_AMOUNT)) {
			return supposedCurrentProduction - (supposedCurrentProduction * techDef->ptrEffects[i].effectValue);
		}
	}
	return 0;
}

// Update unitInstanceScoreForOptionalResearch for remaining (not selected) optional researches
void StrategyBuilder::ComputeScoresForRemainingOptionalResearches() {
	std::list<PotentialResearchInfo*> remainingResearches; // This list is a shortcut to all remaining optional researches (filtering other ones)
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		// Filter: exclude research already added/marked for add, shadow researches, etc
		if (resInfo->researchDef && !resInfo->researchDef->IsShadowResearch() && !resInfo->isInStrategy && !resInfo->markedForAdd) {
			for each (short int impactedUnitDefId in resInfo->impactedUnitDefIds) {
				for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits) // does not include civilians
				{
					if (unitInfo->unitDefId == impactedUnitDefId) {
						resInfo->unitInstanceScoreForOptionalResearch += (int)unitInfo->addedCount;
					}
				}
				PotentialUnitInfo *civilianInfo = this->GetUnitInfo(impactedUnitDefId);
				if (civilianInfo && (civilianInfo->unitAIType == TribeAIGroupCivilian)) {
#pragma TODO("Take into consideration desired units(needs mostly gold?food?) and a player personality (aggressive/economy?)")
#pragma TODO("Have type of gather resource in detailed info to have a more relevant repartition than 25% each")
					//vInfo->unitDefDetailedInfo->
					resInfo->unitInstanceScoreForOptionalResearch += (this->villagerCount_alwaysRetrain * 25) / 100;
				}
				// Units that are not in "actuallySelectedUnits": villagers, farms...
				if (impactedUnitDefId == CST_UNITID_VILLAGER) {
					resInfo->unitInstanceScoreForOptionalResearch += this->villagerCount_alwaysRetrain;
				}
				if (impactedUnitDefId == CST_UNITID_FARM) {
					PotentialBuildingInfo *farmInfo = this->GetBuildingInfo(CST_UNITID_FARM);
					if (farmInfo) {
						if (this->createFarms) {
							resInfo->unitInstanceScoreForOptionalResearch += farmInfo->desiredCount;
						} else {
							resInfo->unitInstanceScoreForOptionalResearch += this->GetSNNumberValue(SNMaxFarms);
						}
					}
				}
			}
			// Transform into a % value
			resInfo->unitInstanceScoreForOptionalResearch = (resInfo->unitInstanceScoreForOptionalResearch * 100) / this->maxPopulation;
			resInfo->unitInstanceScoreForOptionalResearch *= 2; // To have more impact, btw a tech rarely affects more than half the population
			if (resInfo->unitInstanceScoreForOptionalResearch > 100) {
				resInfo->unitInstanceScoreForOptionalResearch = 100;
			}
			if (resInfo->unitInstanceScoreForOptionalResearch < 0) {
				resInfo->unitInstanceScoreForOptionalResearch = 0;
			}
			AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = this->global->GetTechDef(resInfo->researchDef->technologyId);
			for (int i = 0; (techDef != NULL) && (i < techDef->effectCount); i++) {
				if (techDef->ptrEffects[i].IsAttributeModifier()) {
					if ((techDef->ptrEffects[i].effectClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) &&
						(techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_HP)) {
						if (this->settings && this->settings->IsCheckSumValid()) {
							// Add HP to all buildings (the only attribute that really improves buildings)
							int valueToAdd = 20; // easy levels: will probably not be enough to add architecture
							if (this->settings->rgeGameOptions.difficultyLevel == 2) {
								valueToAdd = randomizer.GetRandomValue_normal_moderate(10, 60); // will sometimes do...
							}
							if (this->settings->rgeGameOptions.difficultyLevel == 1) {
								valueToAdd = randomizer.GetRandomValue_normal_moderate(35, 75); // will generally do...
							}
							if (this->settings->rgeGameOptions.difficultyLevel == 0) {
								valueToAdd = randomizer.GetRandomValue_normal_moderate(45, 70); // random part, but almost always enough to actually add architecture
							}
							resInfo->unitInstanceScoreForOptionalResearch += valueToAdd;
						}
					}
					if (techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_SPEED) {
						// Make sure this tech effect applies on "my" units
						AOE_TECHNOLOGIES::TechFilterExcludeTechsWithDrawbacks filter; // Used to exclude technologies like jihad, martyrdom...
						if (!filter.IgnoreEffect(&techDef->ptrEffects[i])) {
							bool affectsVillagers = (techDef->ptrEffects[i].effectClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian);
							bool affectsMyUnits = false;
							// Try on "selected" units
							for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
							{
								if (techDef->ptrEffects[i].effectUnit == unitInfo->unitDefId) {
									affectsMyUnits = true;
									break;
								}
							}
							if (affectsMyUnits) {
								resInfo->unitInstanceScoreForOptionalResearch += 20; // Increasing units speed is always precious
							}
							if (affectsVillagers) {
								resInfo->unitInstanceScoreForOptionalResearch += 90; // Increasing villagers speed is always relevant (except jihad, beware !) => wheel, if not already added
							}
						}
					}
					if (techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_POPULATION_COST) {
						// Set low priority to logistics in RM (standard game)
						if (!this->settings->isDeathMatch) {
							resInfo->unitInstanceScoreForOptionalResearch -= 10;
						}
					}
					if ((techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_RANGE) &&
						(techDef->ptrEffects[i].effectClass == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon)) {
						resInfo->unitInstanceScoreForOptionalResearch += 30; // Adding range to siege units is very useful
					}
				}
				if (AOE_TECHNOLOGIES::TechnologyFilterBase::IsResourceModifier(&techDef->ptrEffects[i])) {
					if (techDef->ptrEffects[i].effectUnit == CST_RES_ORDER_FARM_FOOD_AMOUNT) {
						resInfo->unitInstanceScoreForOptionalResearch += 20; // Farming is the base of economy
						float currentProduction = this->player->GetResourceValue(CST_RES_ORDER_FARM_FOOD_AMOUNT);
						// Take civ farming bonus into account : NO: already taken into account in my resources !
						/*float civFarmBonus = this->GetFarmProductionBonusForTech(this->player->techTreeId, currentProduction);
						if (civFarmBonus != 0) {
							currentProduction += civFarmBonus;
						}*/
						// Count previous farm production upgrades
						currentProduction += this->GetFarmProductionBonusRecursive(resInfo, currentProduction);
						float bonusFactor = 1; // a "percentage" to apply but in 0-1 (in fact, >1 because used as a multiplier)
						if (currentProduction > 0) {
							bonusFactor = techDef->ptrEffects[i].effectValue / currentProduction;
							float additionalFactor = 1;
							if (bonusFactor > 0.15f) {
								additionalFactor += 0.5f; // Give more weight to the part above 15%
								if (bonusFactor > 0.25f) {
									additionalFactor += 0.5f; // Give more weight to the part above 25%
								}
							}
							bonusFactor *= additionalFactor;
							bonusFactor += 1; // use as a multiplier (15%=0.15 becomes 115%=1.15)
						}
						// Malus for late availability
						switch (resInfo->requiredAge) {
						case CST_RSID_IRON_AGE:
							bonusFactor *= 0.85f;
							resInfo->forcePutAsEarlyAsPossible = true;
							break;
						case CST_RSID_BRONZE_AGE:
							bonusFactor *= 0.95f;
							resInfo->researchesThatMustBePutAfterMe.insert(CST_RSID_IRON_AGE); // If selected, try to insert it early enough (so that more farms take advantage of it)
							break;
						case CST_RSID_TOOL_AGE:
						case CST_RSID_STONE_AGE:
						case -1:
							resInfo->researchesThatMustBePutAfterMe.insert(CST_RSID_BRONZE_AGE); // If selected, try to insert it early enough (so that more farms take advantage of it)
							break;
						}
						float newScoreWithBonusFactor = ((float)resInfo->unitInstanceScoreForOptionalResearch * (bonusFactor));
						// Randomly decrease score (for more aggressive strategies)
						newScoreWithBonusFactor *= ((float)randomizer.GetRandomValue_normal_moderate(90, 100)) / 100;
						resInfo->unitInstanceScoreForOptionalResearch = (int)newScoreWithBonusFactor;
					}
					if (techDef->ptrEffects[i].effectUnit == CST_RES_ORDER_GOLD_MINING_PODUCTIVITY) {
						resInfo->unitInstanceScoreForOptionalResearch += 15; // A good boost for gold production !
					}
				}
			}
			// Costs...
			RESEARCH_COST_CATEGORY researchCostCat = GetResearchCostCategory(resInfo->totalCosts);
			switch (researchCostCat) {
			case AOE_CONST_FUNC::RCC_VERY_CHEAP:
				resInfo->unitInstanceScoreForOptionalResearch = (resInfo->unitInstanceScoreForOptionalResearch * 115) / 100;
				break;
			case AOE_CONST_FUNC::RCC_CHEAP:
				break;
			case AOE_CONST_FUNC::RCC_MODERATE:
				resInfo->unitInstanceScoreForOptionalResearch = (resInfo->unitInstanceScoreForOptionalResearch * 90) / 100;
				break;
			case AOE_CONST_FUNC::RCC_EXPENSIVE:
				resInfo->unitInstanceScoreForOptionalResearch = (resInfo->unitInstanceScoreForOptionalResearch * 80) / 100;
				break;
			case AOE_CONST_FUNC::RCC_VERY_EXPENSIVE:
				resInfo->unitInstanceScoreForOptionalResearch = (resInfo->unitInstanceScoreForOptionalResearch * 80) / 100;
				break;
			default:
				break;
			}

			// Apply a random factor
			const int randomImpact = 15;
			int randomFactor = randomizer.GetRandomValue_normal_moreFlat(100 - randomImpact, 100 + randomImpact);
			resInfo->unitInstanceScoreForOptionalResearch = (resInfo->unitInstanceScoreForOptionalResearch * randomFactor) / 100;
			// Fill remainingResearches for further use
			remainingResearches.push_back(resInfo);
		}
		if (resInfo->unitInstanceScoreForOptionalResearch > 100) {
			resInfo->unitInstanceScoreForOptionalResearch = 100;
		}
		if (resInfo->unitInstanceScoreForOptionalResearch < 0) {
			resInfo->unitInstanceScoreForOptionalResearch = 0;
		}
	}

	// Take care of dependencies (loop on *filtered* list)
	// A requirement must have a least the same score as "child" research
	for each (PotentialResearchInfo *currentResInfo in remainingResearches)
	{
		for each (PotentialResearchInfo *requiredResInfo in remainingResearches)
		{
			for each (short int researchId in currentResInfo->researchesThatMustBePutBeforeMe)
			{
				if (researchId == requiredResInfo->researchId) {
					if (currentResInfo->unitInstanceScoreForOptionalResearch > requiredResInfo->unitInstanceScoreForOptionalResearch) {
						requiredResInfo->unitInstanceScoreForOptionalResearch = currentResInfo->unitInstanceScoreForOptionalResearch;
					}
				}
			}
		}
		if (currentResInfo->unitInstanceScoreForOptionalResearch > 100) {
			currentResInfo->unitInstanceScoreForOptionalResearch = 100;
		}
		if (currentResInfo->unitInstanceScoreForOptionalResearch < 0) {
			currentResInfo->unitInstanceScoreForOptionalResearch = 0;
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
		this->RecomputeComparisonBonuses(preSelectedUnits, waterUnits, true, CST_RSID_IRON_AGE);

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
#pragma TODO("attack bonus: compare to selected units abilities / weaknesses ?")
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

	if (this->settings->isDeathMatch) {
		return; // ignore optional units in DM
	}

	// add "bonus" units to fill weaknesses, ex stone thrower vs towers, camel vs cavalry...
	MILITARY_CATEGORY firstAddedCategory = MC_NONE;
	for (int step = 1; step <= 2; step++) {
		int mediumStrength = currentCumulatedStrength / MC_COUNT;
		int worstStrength = currentTotalStrengths[currentWorstStrengthIndex];
		int proportion = 50;
		if (mediumStrength != 0) {
			proportion = (worstStrength * 100) / mediumStrength;
		}
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
// Only consider water units (waterUnits=true) or only consider land units (waterUnits=false)
void StrategyBuilder::SelectStrategyUnitsForLandOrWater(bool waterUnits) {
#ifdef _DEBUG
	/*for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList) {
		if (!unitInfo->isSelected) {
			if ((unitInfo->unitDefId == CST_UNITID_SHORT_SWORDSMAN) ||
				(unitInfo->unitDefId == CST_UNITID_BALLISTA)) {
				this->AddUnitIntoToSelection(unitInfo);
			}
		}
	}
	return;*/
#endif

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
			this->log += "ERROR: Could not select any military unit for strategy";
			this->log += newline;
			return;
		}
	}

	// Choose and add 'main' units to selection (add to actuallySelectedUnits, set "isSelected" to true)
	this->SelectStrategyUnitsFromPreSelection(preSelectedUnits, waterUnits);

	// Add units with special bonus like camel/chariot if it helps for a specific need: done in SelectStrategyUnitsFromPreSelection
}


// Search for a unit that can be added to strategy as "optional" (without full upgrades) to fight a detected weakness.
// waterUnits: if true, manage water units only. If false, manage land units only
void StrategyBuilder::AddOptionalUnitAgainstWeakness(MILITARY_CATEGORY weaknessCategory, bool waterUnits) {
	int bestScore = 0;
	PotentialUnitInfo *bestUnit = NULL;

	// Recompute strengths, for *base* units (not counting upgrades here)
	this->RecomputeComparisonBonuses(this->potentialUnitsList, waterUnits, false, CST_RSID_IRON_AGE);

	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		if (!unitInfo->isNonTowerMilitary) {
			continue;
		}
		// Ignore already selected units (and those NOT matching the land/water parameter)
		if (!unitInfo->isSelected && (unitInfo->isBoat == waterUnits)) {

			// Recompute costs bonus according to current selected units
			unitInfo->bonusForUsedResourceTypes = this->GetCostScoreRegardingCurrentSelection(unitInfo);
			int tmpScore = unitInfo->strengthVs[weaknessCategory];
			// Some hardcoded (but still generic) rules
			if ((weaknessCategory == MILITARY_CATEGORY::MC_TOWER) && (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon)) {
				if (unitInfo->upgradedUnitDefLiving->HasBlastDamage()) {
					tmpScore = (tmpScore * 115) / 100; // catapult-like siege
				} else {
					tmpScore = (tmpScore * 105) / 100; // ballista-like siege vs towers
				}
			}
#pragma TODO("Should recompute scores using BASE units (without costly upgrades)")
			// Important: units may have a low score (<10) and % couldnt apply well: check this
			// TODO take care of relative cost ! Create a dedicated method GetRelativeCostScore with existing code above (in another method)
			// Units available without cost (or low research cost)
			bool lowCostActivation = false;
			if (unitInfo->enabledByResearchId == -1) {
				lowCostActivation = true;
			} else {
				AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = AOE_METHODS::PLAYER::GetResearchDef(this->player, unitInfo->enabledByResearchId);
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

// Select the main (military) units are to be added in strategy, chosen in potentialUnitsList
// This only interacts with potentialUnitsList (may set isSelected=true) and feeds actuallySelectedUnits with "main" non-tower military units
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
	traceMessageHandler.WriteMessageNoNotification("Strategy decision:");
	this->log += "Strategy decision:";
	this->log += newline;
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits) {
		std::string msg = "Unit id=";
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unit = NULL;
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
		this->log += msg;
		this->log += newline;
	}
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList) {
		if (!unitInfo->isSelected && unitInfo->isNonTowerMilitary) {
			std::string msg = "[Not selected] Unit id=";
			AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unit = NULL;
			if (unitInfo->isOptionalUnit) {
				msg += std::to_string(unitInfo->unitDefId);
				unit = unitInfo->baseUnitDefLiving;
			} else {
				msg += std::to_string(unitInfo->strongestUpgradeUnitDefId);
				unit = unitInfo->upgradedUnitDefLiving;
			}
			msg += std::string(" (") + GetLanguageDllText(unit->languageDLLID_Name) + std::string(")");
			msg += " - score=";
			msg += std::to_string(unitInfo->globalScore);
			msg += ", early=";
			msg += std::to_string(unitInfo->globalScoreEarlyAge);
			this->log += msg;
			this->log += newline;
		}
	}
#endif
}

// age = age to take care (ignore units that are only available in later ages)
// hasAlreadyUnits = true if provided age already has some military units
// Returns number of added units (0/1). Added units are marked for add (and WILL actually be written in strategy).
int StrategyBuilder::AddOneMilitaryUnitForEarlyAge(short int age, bool hasAlreadyUnits) {
	// Recalculate bonus for rare strength taking into account specific limitation on age.
	this->RecomputeComparisonBonuses(this->potentialUnitsList, false, false, age);
	std::set<AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES> selectedUnitsClassesAfterThisAge; // list of "main units" AI types, without duplicate
	std::set<AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES> selectedUnitsClassesInThisAge; // list of "main units" AI types, without duplicate
	// Just collect lists of units in/after this age
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits) {
		if (unitInfo->ageResearchId > age) {
			selectedUnitsClassesAfterThisAge.insert(unitInfo->unitAIType);
		} else {
			selectedUnitsClassesInThisAge.insert(unitInfo->unitAIType);
		}
	}
	// Compute bonusForTechsSimilarity
	int countWithPerfectSimilarity = 0;
	int unitDefIdWithPerfectSimilarity = -1;
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList) {
		if (!unitInfo->isSelected && !unitInfo->isBoat && unitInfo->isNonTowerMilitary && (unitInfo->ageResearchId <= age)) {
			unitInfo->bonusForTechsSimilarity = 0; // reset
			for each (AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES refAIType in selectedUnitsClassesAfterThisAge)
			{
				if (AreSimilarClasses(unitInfo->unitAIType, refAIType)) {
					unitInfo->bonusForTechsSimilarity += 30;
				}
				bool attack, defense, hitPoints;
				ShareCommonTechs(unitInfo->unitAIType, refAIType, attack, defense, hitPoints); // collect info...
				int tmpScore = (attack ? 1 : 0) + (defense ? 1 : 0) + (hitPoints ? 1 : 0);
				if (tmpScore > 2) { tmpScore = 2; }
				unitInfo->bonusForTechsSimilarity += tmpScore * 15;
			}
			for each (AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES refAIType in selectedUnitsClassesInThisAge)
			{
				// Do not pick units with similar classes with existing ones : here similarity is a (big) flaw
				if (AreSimilarClasses(unitInfo->unitAIType, refAIType)) {
					unitInfo->bonusForTechsSimilarity = -100; // Make we don't pick similar units for early ages. No use
				}
			}
			// Bonus for no-research units (no cost to have it available)
			AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = player->GetResearchDef(unitInfo->enabledByResearchId);
			bool nullCost = (resDef == NULL);
			if (resDef) {
				if (((resDef->costUsed1 == 0) || (resDef->costAmount1 == 0)) &&
					((resDef->costUsed2 == 0) || (resDef->costAmount2 == 0)) &&
					((resDef->costUsed3 == 0) || (resDef->costAmount3 == 0))) {
					nullCost = true;
				}
			}
			// TODO: unit has upgrades in "its" age ? If yes, it is a drawback (it means: needs additional cost OR is not so strong ?)
			if (!nullCost) {
				unitInfo->bonusForTechsSimilarity = (unitInfo->bonusForTechsSimilarity * 85) / 100; // -15% if research has a cost
			}
			// civ bonus...
			if (unitInfo->hasCivBonus) {
				unitInfo->bonusForTechsSimilarity = (unitInfo->bonusForTechsSimilarity * 115) / 100; // +15% for civ bonus
			}
			for each (PotentialResearchInfo *researchInfo in this->potentialResearchesList)
			{
				if (researchInfo->markedForAdd && 
					(researchInfo->impactedUnitDefIds.find(unitInfo->unitDefId) != researchInfo->impactedUnitDefIds.end())) {
					if (GetNewUnitIdIfTechUpgradesUnit(researchInfo->techDef, unitInfo->unitDefId) > -1) {
						// There already is upgrades for 'this' unit in strategy, this is an excellent point
						unitInfo->bonusForTechsSimilarity = 100;
						countWithPerfectSimilarity++;
						unitDefIdWithPerfectSimilarity = unitInfo->unitDefId;
					}
				}
			}

			if (unitInfo->bonusForTechsSimilarity > 100) { unitInfo->bonusForTechsSimilarity = 100; }
			if (unitInfo->bonusForTechsSimilarity < 0) { unitInfo->bonusForTechsSimilarity = 0; }
		}
	}

	// Choose a unit that has both combination of: global score, similarity with main units
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList) {
		if (!unitInfo->isBoat && !unitInfo->isSelected && unitInfo->isNonTowerMilitary && (unitInfo->ageResearchId <= age)) {
			if (countWithPerfectSimilarity == 1) {
				// We found (only) one unit with upgrades that are already in strategy: select it and bypass other criteria.
				unitInfo->scoreForEarlyAge = (unitInfo->unitDefId == unitDefIdWithPerfectSimilarity) ? 100.0f : 0.0f;
			} else {
				// Standard case
				float globalScoreNonZero = (float)(unitInfo->globalScoreEarlyAge);
				if (globalScoreNonZero <= 0) { globalScoreNonZero = 0; }
				int tmpBonusForTechsSimilarity = unitInfo->bonusForTechsSimilarity / 2; // max 50% impact on global score
				unitInfo->scoreForEarlyAge = (globalScoreNonZero * (100 + tmpBonusForTechsSimilarity)) / 100;
				if (hasAlreadyUnits) {
					float rareStrengthFactor = ((float)unitInfo->bonusForRareStrength);
					rareStrengthFactor = rareStrengthFactor / 2; // Max 50% impact
					unitInfo->scoreForEarlyAge = unitInfo->scoreForEarlyAge * (100 + rareStrengthFactor) / 100;
				}
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
		this->log += "[";
		this->log += std::to_string(age);
		this->log += "] Early age unit selection: ";
		this->log += bestUnit->GetBaseUnitName();
		this->log += " earlyscore=";
		this->log += std::to_string((int)bestUnit->scoreForEarlyAge);
		this->log += newline;
		this->CollectResearchInfoForUnit(bestUnit, false, true); // Add requirements for this unit (only requirements)
		return 1; // return unitInfo instead ?
	}
	return 0;
}

// Add (and mark for add) some military to selection, to fill gaps in strategy in early ages (if any)
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
		for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
		{
			if (!unitInfo->isSelected && unitInfo->isNonTowerMilitary && (unitInfo->ageResearchId <= currentAge)) {
				this->log += "Early age not-selected unit: ";
				this->log += unitInfo->GetBaseUnitName();
				this->log += " earlyscore=";
				this->log += std::to_string((int)unitInfo->scoreForEarlyAge);
				this->log += newline;
			}
		}
	}
}

// Selects optional researches to add to strategy
// All villagers and military units must have already been added to strategy.
// This method only updates flags, does not actually add anything to strategy
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
		if (resInfo->researchDef && !resInfo->researchDef->IsShadowResearch()) { // small optim: exclude shadow researches
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

	// Recompute unit scores on remaining researches, counting ALL (selected) units (not restricted to early units)
	this->ComputeScoresForRemainingOptionalResearches();

	std::list<PotentialResearchInfo*> remainingOptionalResearches; // A shortcut to (filtered) remaining researches
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList) {
		if (!resInfo->isInStrategy && !resInfo->markedForAdd && !resInfo->forcePlaceForFirstImpactedUnit &&
			resInfo->researchDef && (resInfo->researchDef->researchLocation > -1)) {
			remainingOptionalResearches.push_back(resInfo);
		}
	}

	// Mark for add the researches that have an excellent score
	for each (PotentialResearchInfo *resInfo in remainingOptionalResearches) {
		if (resInfo->unitInstanceScoreForOptionalResearch > 85) {
			bool hasVillager = false;
			bool hasNonVillager = false;
			for each (short int unitDefId in resInfo->impactedUnitDefIds)
			{
				if (unitDefId == CST_UNITID_VILLAGER) {
					hasVillager = true;
				} else {
					hasNonVillager = true;
				}
			}
			if (resInfo->unitInstanceScoreForOptionalResearch < 95) {
				resInfo->markedForAdd = true; // Add without priority
			} else {
				if (hasVillager) {
					resInfo->forcePutAsEarlyAsPossible = true;
				} else {
					if (hasNonVillager) {
						resInfo->forcePlaceForFirstImpactedUnit = true;
					} else {
						assert(false && "Research should have impacted units");
					}
				}
			}
			this->log += "Optional research was selected(+): ";
			this->log += GetResearchLocalizedName(resInfo->researchId);
			this->log += " with score=";
			this->log += std::to_string(resInfo->unitInstanceScoreForOptionalResearch);
			this->log += newline;
		}
	}
	// Update "remaining" list (remove "marked for add/force" ones)
	auto it = remove_if(remainingOptionalResearches.begin(), remainingOptionalResearches.end(),
		[](PotentialResearchInfo *resInfo) {
		return resInfo->isInStrategy || resInfo->markedForAdd || resInfo->forcePlaceForFirstImpactedUnit || resInfo->forcePutAsEarlyAsPossible;
	});
	remainingOptionalResearches.erase(it, remainingOptionalResearches.end());

	// Choose some optional researches with a good score...
	if (remainingOptionalResearches.empty()) { return; }
	int totalScore = 0;
	int maxScore = 0;
	int minScore = 100;
	for each (PotentialResearchInfo *resInfo in remainingOptionalResearches)
	{
		totalScore += resInfo->unitInstanceScoreForOptionalResearch;
		if (resInfo->unitInstanceScoreForOptionalResearch < minScore) {
			minScore = resInfo->unitInstanceScoreForOptionalResearch;
		}
		if (resInfo->unitInstanceScoreForOptionalResearch > maxScore) {
			maxScore = resInfo->unitInstanceScoreForOptionalResearch;
		}
	}
	int medianScore = totalScore / remainingOptionalResearches.size();
	assert(maxScore >= medianScore);
	assert(minScore <= medianScore);
	int smallestDiff = min(maxScore - medianScore, medianScore - minScore);
	if (smallestDiff < 0) { return; } // Error case
	int adjustedMedianScore = randomizer.GetRandomValue_normal_moderate(medianScore - (smallestDiff / 2), medianScore + smallestDiff);
	for each (PotentialResearchInfo *resInfo in remainingOptionalResearches)
	{
		if (resInfo->unitInstanceScoreForOptionalResearch > adjustedMedianScore) {
			resInfo->markedForAdd = true;
			this->log += "Optional research was selected: ";
			this->log += GetResearchLocalizedName(resInfo->researchId);
			this->log += " with score=";
			this->log += std::to_string(resInfo->unitInstanceScoreForOptionalResearch);
			this->log += newline;
		}
	}
	// Update "remaining" list (remove "marked for add/force" ones)
	it = remove_if(remainingOptionalResearches.begin(), remainingOptionalResearches.end(),
		[](PotentialResearchInfo *resInfo) {
		return resInfo->isInStrategy || resInfo->markedForAdd || resInfo->forcePlaceForFirstImpactedUnit || resInfo->forcePutAsEarlyAsPossible;
	});
	remainingOptionalResearches.erase(it, remainingOptionalResearches.end());
	for each (PotentialResearchInfo *resInfo in remainingOptionalResearches)
	{
		this->log += "Optional research was ignored: ";
		this->log += GetResearchLocalizedName(resInfo->researchId);
		this->log += " with score=";
		this->log += std::to_string(resInfo->unitInstanceScoreForOptionalResearch);
		this->log += newline;
	}
}


// Collect all researches info for villagers/economy (does NOT mark them for add : optional researches)
void StrategyBuilder::AddResearchesForEconomy() {
	for (int unitDefId = 0; unitDefId < this->player->structDefUnitArraySize; unitDefId++) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = this->player->GetUnitDefBase(unitDefId);
		if (unitDef && unitDef->IsCheckSumValidForAUnitClass() && unitDef->DerivesFromCommandable()) {
			AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *unitDefBird = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)unitDef;
			PotentialBuildingInfo *bldInfo = this->GetBuildingInfo(unitDefId);
			PotentialUnitInfo *unitInfo = this->GetUnitInfo(unitDefId);
			if (unitDefBird->unitDefinitionSwitchGroupId && (unitDefBird->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian)) {
				this->CollectResearchInfoForUnit(unitInfo, true, false);
			}
			if (unitDefId == CST_UNITID_FARM) {
				this->CollectResearchInfoForUnit(bldInfo, true, false); // find all potential farm upgrades, but don't mark for add all of them ! Already done previously but withOUT all upgrades
			}
			if (this->isWaterMap) {
				if ((unitDefBird->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupFishingBoat) ||
					(unitDefBird->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupTransportBoat)) {
					// Trade boat...
					this->CollectResearchInfoForUnit(unitInfo, true, false);
				}
			}
		}
	}
}


// Add user-defined "forced" researches (always develop... if possible)
void StrategyBuilder::AddUserDefinedForcedResearches() {
	std::list<short int> *listToUse = NULL;
	if (settings->isDeathMatch) {
		listToUse = &ROCKNROR::crInfo.configInfo.dmAIForcedResearches;
	} else {
		listToUse = &ROCKNROR::crInfo.configInfo.rmAIForcedResearches;
	}
	for each (short int curResId in *listToUse)
	{
		this->AddUserDefinedForcedResearches(curResId);
	}
}


// Add one user-defined "forced" researches (always develop... if possible)
void StrategyBuilder::AddUserDefinedForcedResearches(short int oneResearchId) {
	ROCKNROR::STRATEGY::TTDetailedResearchDef *resDefDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(oneResearchId);
	if (!resDefDtl || !resDefDtl->active) {
		return;
	}
	// Handle water-map techs
	if (!this->isWaterMap) {
		if (resDefDtl->researchDef->researchLocation == CST_UNITID_DOCK) {
			this->log += "Ignored a \"forced\" research because this map is a no-water map: id=";
			this->log += std::to_string(oneResearchId);
			this->log += newline;
			return; // Ignore "water map" techs on no-water maps.
		}
	}

	if (this->playerTechTreeInfo->IsResearchDisabled(oneResearchId)) {
		return; // unavailable research: ignore (can't force add to strategy)
	}

	PotentialResearchInfo *resInfo = this->GetResearchInfo(oneResearchId);
	if (resInfo) {
		resInfo->markedForAdd = true;
		resInfo->forcePutAsEarlyAsPossible = true;
	} else {
		if (this->IsResearchInTechTree(oneResearchId)) {
			resInfo = this->AddPotentialResearchInfoToList(oneResearchId, true);
			if (resInfo) { resInfo->forcePutAsEarlyAsPossible = true; }
		}
	}
	if (!resInfo) {
		return;
	}
	// We need to add requirements too
	for each (ROCKNROR::STRATEGY::TTDetailedResearchDef *reqRes in resDefDtl->allRequirementsExcludingAges)
	{
		if (reqRes->active && !reqRes->IsShadowResearch()) {
			short int reqResId = (short int)reqRes->GetResearchDefId();
			auto tmp = this->AddPotentialResearchInfoToList(reqResId, true);
			if (!tmp) {
#pragma TODO("Normally we can remove this code. Make sure this works (optional gets marked for add")
				PotentialResearchInfo *tmpResInfo = this->GetResearchInfo(reqResId);
				if (tmpResInfo) {
					// If it was already in list, make sure the requirement is marked for add (at this point it my be an "optional" and could not be actually included in strategy).
					tmpResInfo->markedForAdd = true;
				}
			}
		}
	}
	resInfo->markedForAdd = true;
}


// Returns a vector of all non-disabled researches that affect the provided unit definition (ID)
// useFilter: if true, exclude researches with drawbacks for current game type (DM/RM)
std::vector<short int> StrategyBuilder::GetAllResearchesThatAffectUnit(short int unitDefId, bool useFilter) {
	return ROCKNROR::crInfo.techTreeAnalyzer.GetAllResearchesThatAffectUnit(this->player, unitDefId, useFilter);
}


// Add tower upgrades to internal objects (and mark them as priority items)
// Does not add upgrades that slow projectiles down (ballista tower)
// Only adds unit upgrades (sentry, watch tower) + "enable unit" (watch tower) researches, not others researches.
// Added researches (unit upgrades) are automatically marked for add (will actually be included in strategy)
void StrategyBuilder::AddTowerResearches() {
	const short int TOWER_BASE_ID = CST_UNITID_WATCH_TOWER;
	const short int TOWER_BASE_RESEARCH_ID = CST_RSID_WATCH_TOWER;

	short int enableWatchTower = FindResearchThatEnableUnit(this->player, TOWER_BASE_ID, -1);
	if ((enableWatchTower < 0) || !this->IsResearchInTechTree(enableWatchTower)) {
		return;
	}

	PotentialResearchInfo *watchTowerInfo = this->AddPotentialResearchInfoToList(TOWER_BASE_RESEARCH_ID, true);
	watchTowerInfo->forcePutAsEarlyAsPossible = true;
	bool includeAllUpgrades = (this->settings->isDeathMatch != 0);
	this->AddPotentialBuildingInfoToList(TOWER_BASE_ID, false); // Do not mark for add yet
	PotentialBuildingInfo *towerInfo = this->GetBuildingInfo(TOWER_BASE_ID);
	if (towerInfo) {
		this->CollectResearchInfoForUnit(towerInfo, includeAllUpgrades, true);
	}

	if (includeAllUpgrades) {
		// Just ensure we mark the concerned "potential research info" as "impacting watch tower".
		for (int researchId = 0; researchId < this->player->ptrResearchesStruct->researchCount; researchId++) {
			TTDetailedResearchDef *resInfo = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(researchId);
			if (resInfo && resInfo->active && resInfo->DoesAffectUnit(TOWER_BASE_ID)) {
				PotentialResearchInfo *p = this->GetResearchInfo(researchId);
				if (p && p->markedForAdd) {
					p->impactedUnitDefIds.insert(TOWER_BASE_ID);
					auto search = std::find_if(resInfo->upgradedUnitDefId.cbegin(), resInfo->upgradedUnitDefId.cend(),
						[TOWER_BASE_ID](std::pair<long int, long int> pairUpgrade){ return pairUpgrade.first == TOWER_BASE_ID; });
					if (search != resInfo->upgradedUnitDefId.cend()) {
						p->forcePutAsEarlyAsPossible = true; // Force tower upgrades to be researched quickly
					}
				}
			}
		}
		// DM: no need to continue, we already added all useful techs
		return;
	}

	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *watchTower = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)this->player->GetUnitDefBase(CST_UNITID_WATCH_TOWER);
	if (!watchTower || !watchTower->IsCheckSumValidForAUnitClass() || !watchTower->DerivesFromAttackable()) { return; }
	AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE *projectileDefInitial = (AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE *)this->player->GetUnitDefBase(watchTower->projectileUnitId);

	std::vector<short int> allResearchesForUnit = this->GetAllResearchesThatAffectUnit(CST_UNITID_WATCH_TOWER, true);

	// Add the available upgrades (except ballista tower, detected thanks to projectile speed), ignore other techs
	// Could use CanExcludeInRandomMapAI() ?
	for each (short int researchId in allResearchesForUnit)
	{
		bool newProjectileIsMuchSlower = false;
		bool newReloadTimeIsMuchSlower = false;
		bool isUpgradeUnit = false;
		AOE_STRUCTURES::STRUCT_RESEARCH_DEF *resDef = this->player->GetResearchDef(researchId);
		AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *destBld = NULL;
		if (resDef && (resDef->technologyId >= 0) && (resDef->researchLocation >= 0)) {
			AOE_STRUCTURES::STRUCT_TECH_DEF *techDef = this->global->GetTechDef(resDef->technologyId);
			if (techDef) {
				short int destUnitId = GetNewUnitIdIfTechUpgradesUnit(techDef, CST_UNITID_WATCH_TOWER);
				destBld = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)this->player->GetUnitDefBase(destUnitId);
				isUpgradeUnit = (destUnitId > -1) && (destBld != NULL);
			}
		}
		if (destBld && destBld->IsCheckSumValidForAUnitClass() && destBld->DerivesFromAttackable()) {
			AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE *projectileDefAfter = (AOE_STRUCTURES::STRUCT_UNITDEF_PROJECTILE *)this->player->GetUnitDefBase(destBld->projectileUnitId);
			if (projectileDefAfter && projectileDefAfter->IsCheckSumValid()) {
				float speedAfter = projectileDefAfter->speed;
				float speedInitial = projectileDefInitial->speed;
				newProjectileIsMuchSlower = (speedAfter < speedInitial * 0.9);
				newReloadTimeIsMuchSlower = (destBld->reloadTime1 > watchTower->reloadTime1 * 1.1); // Warning: high reload time is BAD !
			}
		}
		if (isUpgradeUnit && !newProjectileIsMuchSlower && !newReloadTimeIsMuchSlower) {
			PotentialResearchInfo *resInfo = this->AddPotentialResearchInfoToList(researchId, true);
			if (resInfo && this->settings && this->settings->IsCheckSumValid() && (this->settings->rgeGameOptions.difficultyLevel < 2) &&
				this->IsResearchInTechTree(resInfo->researchId)) { // this check is not necessary: if not available, it won't be in allResearchesForUnit
				resInfo->forcePutAsEarlyAsPossible = true; // Force tower upgrades to be researched quickly
			}
		}
		// For researches that already exist in our list, mark the fact it impacts towers
		PotentialResearchInfo *resInfo = this->GetResearchInfo(researchId);
		if (resInfo) {
			resInfo->impactedUnitDefIds.insert(CST_UNITID_WATCH_TOWER);
		}
	}
}


// Handle farm building/research info/desired count
void StrategyBuilder::HandleFarmsInfo() {
	bool dontAddFarms = false;
	
	if (!this->createFarms) {
		this->log += "Do not add farms, count on SNAutoBuildFarms";
		this->log += newline;
		dontAddFarms = true;
	}
	if (this->settings->isDeathMatch) {
		this->log += "Deathmatch: do not add farms";
		this->log += newline;
		dontAddFarms = true;
	}
	if (dontAddFarms) {
		this->AddPotentialBuildingInfoToList(CST_UNITID_FARM, false); // do not mark for add in this case, but we need the building info to exist in the list
		PotentialBuildingInfo *marketInfo = this->GetBuildingInfo(CST_UNITID_MARKET);
		this->CollectResearchInfoForUnit(marketInfo, false, false); // Add market anyway
		return;
	}
	// Always add wheel => now done automatically (and NOT hardcoded !) in optional researches part

	// Make sure farm requirements are met
	this->AddPotentialBuildingInfoToList(CST_UNITID_FARM, true); // Add if not already present
	PotentialBuildingInfo *farmInfo = this->GetBuildingInfo(CST_UNITID_FARM);
	this->CollectResearchInfoForUnit(farmInfo, false, false); // Will add market...
	// Set desired number of farms
	bool normalOrEasy = (this->settings->rgeGameOptions.difficultyLevel >= 2);
	if (farmInfo) {
		if (this->isWaterMap) {
			farmInfo->desiredCount = normalOrEasy ? 4 : 5;
		} else {
			farmInfo->desiredCount = normalOrEasy ? 5 : 7;
		}
		farmInfo->highPriority = true;
		int additionalPop = this->maxPopulation - 50;
		if (additionalPop > 0) {
			farmInfo->desiredCount += (additionalPop / 10); // 1 additional farm for each additional "10 population" group
			const int maxAllowedFarms = this->isWaterMap ? 16: 20; // Max number of farms
			if (farmInfo->desiredCount > maxAllowedFarms) { farmInfo->desiredCount = maxAllowedFarms; }
		}
		this->log += "Desired number of farms: ";
		this->log += std::to_string(farmInfo->desiredCount);
		this->log += newline;
		farmInfo->markedForAdd = true;
	}
}


// Handle desired units count if logistics is part of included researches.
void StrategyBuilder::HandleLogistics() {
	PotentialResearchInfo *logisticsInfo = this->GetResearchInfo(CST_RSID_LOGISTICS);
	if (!logisticsInfo || !logisticsInfo->markedForAdd) {
		return;
	}
	if (this->isWaterMap) {} // How to handle the fact that ROR will add boats ?
	int totalPop = this->villagerCount_alwaysRetrain;
	std::list<PotentialUnitInfo*> involvedUnits;
	for each (auto oneUnit in this->actuallySelectedUnits) {
		if ((oneUnit->unitAIType == AOE_CONST_FUNC::TribeAIGroupSlinger) || (oneUnit->unitAIType == AOE_CONST_FUNC::TribeAIGroupFootSoldier)) {
			totalPop += (int)(oneUnit->desiredCount / 2);
			if (!oneUnit->isOptionalUnit || !oneUnit->earlyAgeUnit) {
				involvedUnits.push_back(oneUnit); // we'll only add "main" military units
			}
		} else {
			totalPop += (int)oneUnit->desiredCount;
		}
	}
	int remainingRoom = totalPop - this->maxPopulation;
	if (remainingRoom <= 0) { return; }
	if (involvedUnits.size() <= 0) { return; }
	int countPerUnit = remainingRoom / involvedUnits.size();
	this->log += "Logistics => add ";
	this->log += std::to_string(remainingRoom);
	this->log += " units from ";
	this->log += std::to_string(involvedUnits.size());
	this->log += " kinds";
	this->log += newline;
	assert(remainingRoom < 500);
	while (remainingRoom > 0) {
		for each (PotentialUnitInfo *curUnit in involvedUnits)
		{
			curUnit->desiredCount++;
			remainingRoom--;
		}
	}
}


/*** Strategy writing methods ***/


// Get the very global information about strategy generation (number of villagers, etc)
void StrategyBuilder::CollectGlobalStrategyGenerationInfo(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	this->maxPopulation = this->crInfo->configInfo.singlePlayerMaxPopulation;
	if (this->maxPopulation <= 0) {
		this->maxPopulation = settings->maxPopulation;
	}
	this->isWaterMap = IsDockRelevantForMap(settings->mapTypeChoice);
	this->createFarms = true;
	if (StrategyBuilder::RM_NO_STATIC_FARM_IF_SN_AUTO_BUILD_FARMS && player->ptrAIStruct && (this->GetSNNumberValue(SNAutoBuildFarms) == 1)) {
		// When "auto build farms" is ON, do not include them in strategy. Let them be added automatically in-game
		this->createFarms = false;
	}

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

	int fixedVillagerCount = 5; // default for DM
	int limitedRetrainsVillagerCount = 4; // default for DM
	if (!this->settings->isDeathMatch) {
		fixedVillagerCount = randomizer.GetRandomValue_normal_moderate(minFixedVillagers, minFixedVillagers + maxFixedVillagersRandomPart);
		limitedRetrainsVillagerCount = randomizer.GetRandomValue_normal_moderate(minLimitedRetrainsVillagers, minLimitedRetrainsVillagers + maxLimitedRetrainsVillagersRandomPart);
	}
	if (fixedVillagerCount + limitedRetrainsVillagerCount > maxTotalVillagers) { limitedRetrainsVillagerCount = maxTotalVillagers; }

	this->villagerCount_alwaysRetrain = fixedVillagerCount;
	this->villagerCount_limitedRetrains = limitedRetrainsVillagerCount;
	if (this->maxPopulation > 50) {
		int additionalPop = this->maxPopulation - 50;
		if (!this->settings->isDeathMatch) {
			// A third (33%) of additional population will be villager. A third of those will have limited retrains
			this->villagerCount_alwaysRetrain += (2 * additionalPop) / 9;
			this->villagerCount_limitedRetrains += additionalPop / 9;
		} else {
			this->villagerCount_limitedRetrains += additionalPop / 20; // Don't add too many villagers, and only limited retrain ones
		}
	}
	this->log += "Villager count: ";
	this->log += std::to_string(this->villagerCount_alwaysRetrain);
	this->log += "+";
	this->log += std::to_string(this->villagerCount_limitedRetrains);
	this->log += newline;

	// "civilian" Boats : TODO. ROR can handle it meanwhile
	if (this->isWaterMap) {

	}

	// Select the "main" military units to train (including ships - excluding towers)
#pragma TODO("How about DM here ? Use a different selection process ?")
	this->CollectPotentialUnitsInfo(player);
	this->ComputeStrengthsForPotentialUnits();
	this->SelectStrategyUnits();
}


// Creates base strategy elements (ages + initial Town center)
// Does not add villagers or any other item
void StrategyBuilder::CreateTCAndAgesStrategyElements() {
	if (!this->buildAI || !this->buildAI->IsCheckSumValid()) { return; }
	if (!player || !player->IsCheckSumValid()) { return; }
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &this->buildAI->fakeFirstStrategyElement;

	TAIUnitClass researchAiTypeToUse = TAIUnitClass::AIUCCritical;
	if (this->settings->isDeathMatch) {
		// In DM, AI will evolve faster without the constraint of "waiting for age before looking further".
		researchAiTypeToUse = TAIUnitClass::AIUCTech;
	}
	
	// Adding before "fake first" will add in last position in strategy.
	// Add Town Center
	if (AddUnitInStrategy_before(this->buildAI, fakeFirstElem, -1, -1, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding, CST_UNITID_FORUM, this->player, "TownCenter")) {
		this->seTownCenter = fakeFirstElem->previous;
	}
	
	// Tool Age
	if ((GetResearchStatus(player, CST_RSID_TOOL_AGE) == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED)) {
		string msg = "This civ seems NOT to have tool age. Player #";
		msg += to_string(this->buildAI->commonAIObject.playerId);
		msg += " : ";
		msg += this->buildAI->commonAIObject.playerName;
		traceMessageHandler.WriteMessageNoNotification(msg);
		this->log += msg;
		this->log += newline;
		return;
	}
	if (AddUnitInStrategy_before(this->buildAI, fakeFirstElem, -1, CST_UNITID_FORUM, researchAiTypeToUse, CST_RSID_TOOL_AGE, player, "*Tool Age*")) {
		this->seToolAge = fakeFirstElem->previous;
	}
	// Bronze Age
	if ((GetResearchStatus(player, CST_RSID_BRONZE_AGE) == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED)) {
		string msg = "This civ seems NOT to have bronze age. Player #";
		msg += to_string(this->buildAI->commonAIObject.playerId);
		msg += " : ";
		msg += this->buildAI->commonAIObject.playerName;
		traceMessageHandler.WriteMessageNoNotification(msg);
		this->log += msg;
		this->log += newline;
		return;
	}
	if (AddUnitInStrategy_before(buildAI, fakeFirstElem, -1, CST_UNITID_FORUM, researchAiTypeToUse, CST_RSID_BRONZE_AGE, this->player, "*Bronze Age*")) {
		this->seBronzeAge = fakeFirstElem->previous;
	}
	// Iron Age
	if ((GetResearchStatus(player, CST_RSID_IRON_AGE) == AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED)) {
		string msg = "This civ seems NOT to have iron age. Player #";
		msg += to_string(buildAI->commonAIObject.playerId);
		msg += " : ";
		msg += buildAI->commonAIObject.playerName;
		traceMessageHandler.WriteMessageNoNotification(msg);
		this->log += msg;
		this->log += newline;
		return;
	}
	if (AddUnitInStrategy_before(this->buildAI, fakeFirstElem, -1, CST_UNITID_FORUM, researchAiTypeToUse, CST_RSID_IRON_AGE, player, "*Iron Age*")) {
		this->seIronAge = fakeFirstElem->previous;
	}
}

// Create villager strategy elements, including fishing ships
void StrategyBuilder::CreateVillagerStrategyElements() {
	if (!this->buildAI || !this->buildAI->IsCheckSumValid()) { return; }
	if (!player || !player->IsCheckSumValid()) { return; }
	assert(this->seToolAge != NULL);
	assert(this->seBronzeAge != NULL);
	assert(this->seIronAge != NULL);
	if (!this->seToolAge || !this->seBronzeAge || !this->seIronAge) { return; }

	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &this->buildAI->fakeFirstStrategyElement;
	int totalVillagersCount = this->villagerCount_alwaysRetrain + this->villagerCount_limitedRetrains;
	assert(this->villagerCount_alwaysRetrain + this->villagerCount_limitedRetrains > 0);

#pragma TODO("boats")
#pragma TODO("add a random part in repartition")
	int currentCount_fixed = 0;
	int currentCount_retrains = 0;

	// *** Stone age *** : 11+2 (land), 9+3 (water)
	int stoneAgeVillagerCount_fixed = this->villagerCount_alwaysRetrain; // default (DM)
	int stoneAgeVillagerCount_retrains = 0; // default (DM)
	if (!this->settings->isDeathMatch) {
		stoneAgeVillagerCount_fixed = 11;
		stoneAgeVillagerCount_retrains = 2;
		if (this->villagerCount_alwaysRetrain < 15) {
			stoneAgeVillagerCount_fixed = 9;
			stoneAgeVillagerCount_retrains = 3;
		}
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
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *insertionPoint = &this->buildAI->fakeFirstStrategyElement; // insert just before this
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *minInsertionPoint = NULL; // do not insert before this.
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
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *beforeInsertionPoint = insertionPoint->previous;
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
				AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = beforeInsertionPoint->next; // should be 1st inserted element
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
			AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = this->buildAI->fakeFirstStrategyElement.next;
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
			case TribeAIGroupHero:
			case TribeAIGroupPhalanx:
			case TribeAIGroupMountedSoldier:
			case TribeAIGroupArcher:
			case TribeAIGroupChariotArcher:
			case TribeAIGroupElephantArcher:
			case TribeAIGroupHorseArcher:
				unitInfo->scoreForUnitCount = proportionForOneMainUnit * 1.15f; // bonus for melee and base archer unit count (need more numbers)
				break;
			case TribeAIGroupPriest:
				unitInfo->scoreForUnitCount = proportionForOneMainUnit * 0.50f; // Priests: train less than other units (expensive, and too many of them is fruitless)
				break;
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
			if (unitInfo->desiredCount < 0) {
				unitInfo->desiredCount = 0;
			}
			this->log += "Desired count for ";
			this->log += unitInfo->GetUpgradedUnitName();
			this->log += "=";
			this->log += std::to_string(unitInfo->desiredCount);
			this->log += newline;
		}
	}

	// A bit dirty: if using logistics, add units to use extra room in population
	this->HandleLogistics();

	// For the moment, insert all in iron age
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *insertionPoint = &this->buildAI->fakeFirstStrategyElement;

	// Fill orderedUnitsToAdd list with ordered units (1 by 1)
	std::list<PotentialUnitInfo*> orderedUnitsToAdd;
	bool found = true;
	while (found) {
		PotentialUnitInfo *currentBest = NULL;
		float lowestProportion = 100;
		for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
		{
			if (!unitInfo->earlyAgeUnit && (unitInfo->desiredCount > 0)) {
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

#pragma TODO("unfinished")
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
		if (unitInfo->enabledByResearchId >= 0) {
			tmpResIdAsVector.push_back(unitInfo->enabledByResearchId);
		}
#pragma TODO("Use collected requirements, remove that call !")
		//unitInfo->requiredResearchesForBaseUnit if unitInfo->earlyAgeUnit or unitInfo->isOptionalUnit ?
		//unitInfo->requiredResearchesForStrongestUpgrade		
		std::vector<short int> requiredResearches = GetValidOrderedResearchesListWithDependencies(this->player, tmpResIdAsVector);
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
				this->AddPotentialResearchInfoToList(resId, true);
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
					AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *insertionPoint = this->buildAI->fakeFirstStrategyElement.next;
					if (resInfo->mustBeAfterThisElem != NULL) {
						insertionPoint = resInfo->mustBeAfterThisElem->next;
					}
					int usefulness = resInfo->impactedUnitDefIds.size();
					if ((usefulness <= 1) && (resInfo->mustBeBeforeThisElem != NULL)) {
						insertionPoint = resInfo->mustBeBeforeThisElem;
					}
					if ((usefulness >= 2) && (usefulness <= 3)) {
						int elemCountInAllowedInterval = 0;
						AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = resInfo->mustBeAfterThisElem->next;
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
	// First add researches with a specific location (forcePlaceForFirstImpactedUnit)
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		if (resInfo->markedForAdd && (resInfo->forcePlaceForFirstImpactedUnit || resInfo->forcePutAsEarlyAsPossible) && !resInfo->isInStrategy) {
			// If research has already been placed, move it ? should not be necessary because ComputeStratElemPositionConstraints should have taken this into account ?
			resInfo->ComputeStratElemPositionConstraints(this->buildAI);
			if (resInfo->mustBeAfterThisElem != NULL) {
				this->AddResearchToStrategy(resInfo, resInfo->mustBeAfterThisElem->next);
			}
		}
	}

	// Add "unit upgrade" researches whose cost is quite low, compared to unit itself
	// => add as early as possible (if not, we will add them later anyway, don't worry)
#pragma TODO("We could do this analysis BEFORE starting adding strategy elems.")
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList) {
		if (resInfo->markedForAdd && !resInfo->isInStrategy && (resInfo->researchDef->researchLocation >= 0) && resInfo->techDef &&
			!resInfo->forcePutAfterOtherResearches) {
			// Do not handle "affects units" here, because it may add many "cheap" researches, which is bad (example: temple techs)
			// bool researchUpgradesUnits = false;
			int maxUnitCost = 0; // Theoritically, only 1 unit is concerned by unit upgrade here !...
			for each (short int unitDefId in resInfo->impactedUnitDefIds) {
				AOE_TECHNOLOGIES::TechFilterExcludeDrawbacksAndDistributedEffects filter;
				if (GetNewUnitIdIfTechUpgradesUnit(resInfo->techDef, unitDefId) >= 0) {
					AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)this->player->GetUnitDefBase(unitDefId);
					if (unitDef && unitDef->DerivesFromTrainable()) {
						int totalUnitCost = 0;
						for (int i = 0; i < 3; i++) {
							if (unitDef->costs[i].costPaid) {
								totalUnitCost += unitDef->costs[i].costAmount;
							}
						}
						if (totalUnitCost > maxUnitCost) {
							maxUnitCost = totalUnitCost;
						}
					}
				}
			}
			int costForComparison = maxUnitCost * 4;
			if (resInfo->totalCosts <= costForComparison) {
				// Place this research as early as possible
				//resInfo->forcePutAsEarlyAsPossible = true; // for the record...
				resInfo->ComputeStratElemPositionConstraints(this->buildAI);
				if (resInfo->mustBeAfterThisElem && resInfo->mustBeAfterThisElem->next) {
					// Put the research after some impacted units
					AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = curElem = resInfo->mustBeAfterThisElem->next;
					int count = 1;
					const int unitCountBeforeResearch = 3;
					int unitCountBeforeResearchWithRandom = unitCountBeforeResearch + randomizer.GetRandomValue_normal_moderate(-4, 4);
					if (unitCountBeforeResearchWithRandom < 0) { unitCountBeforeResearchWithRandom = -unitCountBeforeResearchWithRandom; } // absolute value (positive)
					while (curElem && (curElem != &this->buildAI->fakeFirstStrategyElement) && (count < unitCountBeforeResearchWithRandom)) {
						if (curElem->elementType == TAIUnitClass::AIUCLivingUnit) {
							auto it = std::find_if(resInfo->impactedUnitDefIds.begin(), resInfo->impactedUnitDefIds.end(),
								[curElem](short int curId) { return curId == curElem->unitDAT_ID; }
							);
							if (it != resInfo->impactedUnitDefIds.end()) {
								count++;
							}
						}
						curElem = curElem->next;
					}
					this->AddResearchToStrategy(resInfo, curElem);
					this->log += GetResearchLocalizedName(resInfo->researchId);
					this->log += " has been placed using higher priority because of its low (relative) cost. id=";
					this->log += std::to_string(resInfo->researchId);
					this->log += newline;
				}
			}
		}
	}

	// Add researches that improve units, where research cost is low compared to unit itself
	// LIMITATION: only if 1 or 2 researches. Avoid adding if too many of such researches (cf temple techs)
	// Need to get list of impacted units (for global list of such researches?) and make a decision...
	// => add quite early
	// TODO


	// Find the strategy location where population reaches "popLimitToPutResearches"
	int popLimitToPutResearches = 50; // the population before which ALL researches must be placed
	if (this->settings->isDeathMatch) {
		popLimitToPutResearches = 30;
	}
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeElem = &this->buildAI->fakeFirstStrategyElement;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *endLimitForResearchesElem = fakeElem->next;
	int retrainablePop = 0;
	int posAfterIron = 0; // number of strategy elements read since iron age
	bool seenIron = false;
	while (endLimitForResearchesElem && (endLimitForResearchesElem != fakeElem) && (retrainablePop < popLimitToPutResearches)) {
		if ((endLimitForResearchesElem->elementType == AIUCLivingUnit) && (endLimitForResearchesElem->retrains < 0)) {
			retrainablePop++;
		}
		if (seenIron) {
			posAfterIron++;
		}
		if (((endLimitForResearchesElem->elementType == AIUCCritical) || (endLimitForResearchesElem->elementType == AIUCTech)) &&
			(endLimitForResearchesElem->unitDAT_ID == CST_RSID_IRON_AGE)) {
			seenIron = true;
			assert(endLimitForResearchesElem == this->seIronAge);
		}
		endLimitForResearchesElem = endLimitForResearchesElem->next;
	}
	if (!seenIron) {
		// Invalid case: too much population before iron ! We can't force researches before iron !
		endLimitForResearchesElem = fakeElem;
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
	// TODO: set an order for insertion, use priorities...
	// TODO: exclude resInfo->forcePutAfterOtherResearches entries and do a dedicated loop afterwards ?
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
				if (resInfo->detailedResInfo->IsShadowResearch()) {
					resInfo->isInStrategy = true;
					addedResearchesThisLoop++;
					continue;
				}
				if (requirementsAreReady) {
					resInfo->ComputeStratElemPositionConstraints(this->buildAI); // update dependencies on strategy elements
					// Add to strategy at correct location
					AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *insertionPoint = this->buildAI->fakeFirstStrategyElement.next;
					if (this->seIronAge != NULL) {
						insertionPoint = this->seIronAge;
					}
					int maxRandomSteps = 25;
					if (resInfo->mustBeAfterThisElem != NULL) {
						insertionPoint = resInfo->mustBeAfterThisElem->next;
						maxRandomSteps -= 10;
					}
					if ((endLimitForResearchesElem != fakeElem) && (endLimitForResearchesElem == this->seIronAge)) {
						maxRandomSteps = posAfterIron;
					}
					int moveFurther = randomizer.GetRandomValue(1, maxRandomSteps);
					while (insertionPoint && insertionPoint->next && (moveFurther > 0)) {
						if (insertionPoint->elemId == -1) { break; } // Reached empty element (end of strategy)
						if (insertionPoint == resInfo->mustBeBeforeThisElem) { break; } //  Ensure dependencies are respected
						if (insertionPoint == endLimitForResearchesElem) { break; } // Ensure going no further than a specified population
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
	this->log += std::to_string(totalAddedResearches);
	this->log += "/";
	this->log += std::to_string(totalResearchesToUse);
	this->log += " identified non-critical researches added";
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
			(bldInfo->addedInStrategyCount > 0)) {
			continue;
		}
		// Some buildings with hardcoded behaviour
		if ((bldInfo->unitDefId == CST_UNITID_GRANARY) || (bldInfo->unitDefId == CST_UNITID_STORAGE_PIT)) {
			// Read SN number from strategy AI because it is valued and correct even "before" game start. TacAI's SN number are copied later.
			if (this->ai->structStrategyAI.SNNumber[SNAutoBuildDropsites] > 0) {
				//this->ai->structTacAI.granaryAddedToStrategy
				//this->ai->structTacAI.storagePitAddedToStrategy
				bldInfo->addedInStrategyCount++; // Anticipate the one that will be added at game start after a few seconds
				continue;
			}
		}
		if (bldInfo->unitDefId == CST_UNITID_FARM) {
			continue; // Do not insert farms here. We need all standard buildings to be added first (so farm requirements are detected correctly)
		}
		if (!this->isWaterMap && (bldInfo->unitDefId == CST_UNITID_DOCK)) {
			this->log += "Warning: do not add dock in strategy as this is not a water map";
		}

		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &this->buildAI->fakeFirstStrategyElement;
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = this->GetAgeStrategyElement(bldInfo->enabledInAge);
		if (!curElem) {
			curElem = &this->buildAI->fakeFirstStrategyElement;
		}
		curElem = curElem->next;
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *insertionPoint = NULL;
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


// Add strategy elements for farms
void StrategyBuilder::CreateFarmStrategyElements() {
	PotentialBuildingInfo *farmInfo = this->GetBuildingInfo(CST_UNITID_FARM);
	if (!farmInfo || (farmInfo->desiredCount <= 0)) { return; }
	if (!this->createFarms) { return; }

	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *myAgeElem = this->GetAgeStrategyElement(farmInfo->enabledInAge);
	if (myAgeElem == NULL) {
		myAgeElem = this->seToolAge;
	}
	int stratElemCountInAge = 0;
	bool foundNextAge = false;
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *nextAgeElem = myAgeElem->next;
	while (nextAgeElem && (nextAgeElem != &this->buildAI->fakeFirstStrategyElement) && !foundNextAge) {
		foundNextAge = ((nextAgeElem->elementType == TAIUnitClass::AIUCCritical) || (nextAgeElem->elementType == TAIUnitClass::AIUCTech)) &&
			(nextAgeElem->unitDAT_ID >= CST_RSID_STONE_AGE) && (nextAgeElem->unitDAT_ID <= CST_RSID_IRON_AGE); // +104 for republic age
		if (!foundNextAge) {
			stratElemCountInAge++;
			nextAgeElem = nextAgeElem->next;
		}
	}
	if (!foundNextAge && (stratElemCountInAge > 25)) { stratElemCountInAge = 25; } // Case: farm is enabled in iron (not a standard case)

	// Add farms in current age, with flat repartition
	int addedCount = AddStrategyElements(myAgeElem, stratElemCountInAge, farmInfo->desiredCount, -1, -1, TAIUnitClass::AIUCBuilding, CST_UNITID_FARM,
		this->player, "Farm");
	for (int i = 0; i < addedCount; i++) {
		this->UpdateBuildingInfoAfterAddInStrategy(farmInfo, NULL);
	}
	if (randomizer.GetRandomPercentageValue() <= 50) {
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *firstFarm = FindFirstElementInStrategy(myAgeElem, TAIUnitClass::AIUCBuilding, farmInfo->unitDefId);
		if (firstFarm && firstFarm->IsCheckSumValid()) {
			firstFarm->retrains = 6; // Limited retrains to get (a bit) less farms in late game
		}
	}
}

// Add strategy elements for towers (buildings - not researches)
void StrategyBuilder::CreateTowerBuildingsStrategyElements() {
	this->AddPotentialBuildingInfoToList(CST_UNITID_WATCH_TOWER, true);
	PotentialBuildingInfo *towerInfo = this->GetBuildingInfo(CST_UNITID_WATCH_TOWER);
	if (!towerInfo) { return; }
	// TODO : compute number of towers (use a random part)
	// Do not set too many towers (more can be added dynamically during game)
	towerInfo->desiredCount = 8;
	
	short startAge = towerInfo->enabledInAge;
	if (startAge < CST_RSID_TOOL_AGE) {
		startAge = CST_RSID_TOOL_AGE;
	}
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *myAgeElem = this->GetAgeStrategyElement(startAge);
	if (myAgeElem == NULL) {
		myAgeElem = this->seToolAge;
	}

	while (myAgeElem && (myAgeElem->unitDAT_ID <= CST_RSID_IRON_AGE) && (towerInfo->addedInStrategyCount < towerInfo->desiredCount) &&
		((myAgeElem->elementType == TAIUnitClass::AIUCCritical) || (myAgeElem->elementType == TAIUnitClass::AIUCTech))) {
		int desiredCountInThisAge = towerInfo->desiredCount - towerInfo->addedInStrategyCount;
		switch (myAgeElem->unitDAT_ID) {
		// +case 104 for republic age
		case CST_RSID_IRON_AGE:
			break;
		case CST_RSID_BRONZE_AGE:
			if (desiredCountInThisAge > 4) {
				desiredCountInThisAge = 4; // Max 4 towers in bronze age
			}
			break;
		default: // tool/stone
			if (desiredCountInThisAge > 2) {
				desiredCountInThisAge = 2; // Max 2 towers in tool age
			}
			break;
		}

		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *mustBeAfterThisElem = myAgeElem;
		if (towerInfo->firstAddedInStrategy) {
			if (GetFirstElementOf(this->buildAI, towerInfo->firstAddedInStrategy, mustBeAfterThisElem) == mustBeAfterThisElem) {
				mustBeAfterThisElem = towerInfo->firstAddedInStrategy; // mustBeAfterThisElem is the "latest" element
			}
		}
		PotentialResearchInfo *enableResearchInfo = this->GetResearchInfo(towerInfo->enabledByResearchId);
		if (enableResearchInfo->actualStrategyElement) {
			if (GetFirstElementOf(this->buildAI, enableResearchInfo->actualStrategyElement, mustBeAfterThisElem) == mustBeAfterThisElem) {
				mustBeAfterThisElem = enableResearchInfo->actualStrategyElement; // mustBeAfterThisElem is the "latest" element
			}
		}

		int stratElemUntilNextAge = 0;
		bool foundNextAge = false;
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *nextAgeElem = mustBeAfterThisElem->next;
		while (nextAgeElem && (nextAgeElem != &this->buildAI->fakeFirstStrategyElement) && !foundNextAge) {
			foundNextAge = ((nextAgeElem->elementType == TAIUnitClass::AIUCCritical) || (nextAgeElem->elementType == TAIUnitClass::AIUCTech)) &&
				(nextAgeElem->unitDAT_ID >= CST_RSID_STONE_AGE) && (nextAgeElem->unitDAT_ID <= CST_RSID_IRON_AGE); // +104 for republic age
			if (!foundNextAge) {
				stratElemUntilNextAge++;
				nextAgeElem = nextAgeElem->next;
			}
		}
		// Add 2 towers to current age
		int addedCountInThisAge = AddStrategyElements(mustBeAfterThisElem, stratElemUntilNextAge, desiredCountInThisAge, -1, -1,
			TAIUnitClass::AIUCBuilding, CST_UNITID_WATCH_TOWER, this->player, "Tower");
		for (int i = 0; i < addedCountInThisAge; i++) {
			this->UpdateBuildingInfoAfterAddInStrategy(towerInfo, NULL);
		}
		// Move to next age
		short int nextAge = (short int)myAgeElem->unitDAT_ID + 1;
		if (nextAge > CST_RSID_IRON_AGE) {
			myAgeElem = NULL; // Will exit loop
		} else {
			myAgeElem = this->GetAgeStrategyElement(nextAge);
		}
	}
}

// Create secondary (optional) occurrences of buildings. E.g. 2nd TC, or additional military buildings to train army faster and do researches while another building is training units
int StrategyBuilder::CreateSecondaryBuildingStrategyElements() {
	// Backup TC: add after all "required" buildings and after gov center (not allowed before) & after iron age too
	PotentialBuildingInfo *tcInfo = this->GetBuildingInfo(CST_UNITID_FORUM);
	if (tcInfo && (tcInfo->addedInStrategyCount <= 1) && (tcInfo->desiredCount > 1)) {
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = this->seIronAge;
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
			this->AddBuildingToStrategy(tcInfo, curElem);
		}
	}

	// Farms
	this->CreateFarmStrategyElements();

	// "Military" buildings
	std::set<PotentialBuildingInfo*> bldList; // buildings that need to be built more than once
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
	{
		if (unitInfo->baseUnitDefLiving && unitInfo->isSelected && !unitInfo->isOptionalUnit &&
			((unitInfo->addedCount > 7) || (unitInfo->isSuperUnit))) {
			short int bldId = unitInfo->baseUnitDefLiving->trainLocation;
			PotentialBuildingInfo *bldInfo = this->GetBuildingInfo(bldId);
			if (bldInfo) {
				assert(bldInfo->desiredCount > 0);
				bldList.insert(bldInfo); // add without duplicate
				bldInfo->unitsToBeTrained += (int)unitInfo->desiredCount;
			}
		}
	}
	for each (PotentialBuildingInfo *bldInfo in bldList) {
		int unitsPerBuilding = randomizer.GetRandomValue_normal_moderate(6, 10); // with a small random part here
		bldInfo->desiredCount = bldInfo->unitsToBeTrained / unitsPerBuilding;
		if (bldInfo->desiredCount < 2) { bldInfo->desiredCount = 2; }
		if (bldInfo->desiredCount > 5) { bldInfo->desiredCount = 5; }
		this->log += "Add more buildings for ";
		if (bldInfo->unitDef) {
			this->log += bldInfo->unitDef->ptrUnitName;
		}
		this->log += ", total=";
		this->log += std::to_string(bldInfo->desiredCount);
		this->log += " (";
		this->log += std::to_string(bldInfo->unitsToBeTrained);
		this->log += " units)";
		this->log += newline;
		assert(bldInfo->firstAddedInStrategy != NULL);
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *curElem = bldInfo->firstAddedInStrategy;
		if (curElem) {
			int usageOfThisBuildingCount = 0;
			while (curElem && (bldInfo->addedInStrategyCount < bldInfo->desiredCount) &&
				(curElem != &this->buildAI->fakeFirstStrategyElement) && (curElem->elemId >= 0)) {
				if ((curElem->actor == bldInfo->unitDefId) && (curElem->retrains == -1) &&
					((curElem->elementType == TAIUnitClass::AIUCLivingUnit) || (curElem->elementType == TAIUnitClass::AIUCTech))) {
					usageOfThisBuildingCount++;
				}
				if (usageOfThisBuildingCount == unitsPerBuilding) {
					usageOfThisBuildingCount = 0; // reset
					this->AddBuildingToStrategy(bldInfo, curElem);
				}
				curElem = curElem->next;
			}
		}
	}

	return 0;
}


// Returns true if the game is a scenario where we don't want to generate a dybnamic strategy
bool StrategyBuilder::DoesScenarioExcludeStrategyGeneration() {
	if (!this->settings || !this->global) {
		return true;
	}
	if (this->settings->isCampaign || this->settings->rgeGameOptions.isScenario) {
		// Campaign/Scenario (possible for players with no strategy)
		// If the scenarios contains "disable researches/buildings", then abort and let standard behaviour occur
		// We don't want to deal with weird cases where almost no tech/unit is available
		if (this->global->scenarioInformation) {
			int playerIndex = this->player->playerId - 1;

			char *str = this->global->scenarioInformation->strategyFileNames[playerIndex];
			if ((str[0] == 'N') && (str[1] == 'O') && (str[2] == 'N') && (str[3] == 'E')) {
				// The scenario explicitly does NOT want a strategy for that player.
				return true;
			}

			for (int j = 0; j < AOE_CONST_INTERNAL::SC_ED_DISABLE_TECHNOLOGY_INDICES::CST_DTI_COUNT; j++) {
				if (!this->global->scenarioInformation->allowedTechnologyFlags[playerIndex][j]) {
					// We found a research/building that is NOT allowed = limitation on available tech tree = don't mess with strategy
					return true;
				}
			}
		}
	}
	return false; // Standard case
}


// Create a brand new dynamic strategy for player.
// Returns true if a strategy was generated
bool StrategyBuilder::CreateStrategyFromScratch() {
	if (!this->buildAI || !this->buildAI->IsCheckSumValid() || !this->buildAI->mainAI || !this->buildAI->mainAI->IsCheckSumValid()) { return false; }
	if (!this->player || !this->player->IsCheckSumValid()) { return false; }
	if (!this->global || !this->global->IsCheckSumValid() || !this->settings) { return false; }
	assert(this->playerTechTreeInfo && this->playerTechTreeInfo->isActive);
	// Clear previous strategy
	ROCKNROR::STRATEGY::ClearStrategy(this->buildAI);
	strcpy_s(this->buildAI->strategyFileName, 0x3F, "Dynamic RockNRor strategy");
	this->log += "Start Strategy creation (2) for p#";
	this->log += std::to_string(this->player->playerId);
	this->log += " = ";
	this->log += this->player->playerName_length16max;
	this->log += newline;

	if (this->DoesScenarioExcludeStrategyGeneration()) {
		return false; // This is a scenario and we should not generate a strategy for current player
	}

	if (this->settings->isDeathMatch) {
		StrategyBuilder::randomPercentFactor = 25;
	} else {
		StrategyBuilder::randomPercentFactor = 30;
	}

	// WARNING : take care if map is naval (=> villager count, land military units count + all non-war boats thing)
	// + max pop criterion
	// Feed potential unit info (all possible units), mark "main" chosen ones as selected (only non-tower military units)
	this->CollectGlobalStrategyGenerationInfo(player);

	// Initialize strategy with TC + ages (no villager yet)
	this->CreateTCAndAgesStrategyElements();
	// Add villagers
	this->CreateVillagerStrategyElements();

	// Select researches to add in strategy
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
	{
		this->CollectResearchInfoForUnit(unitInfo, !unitInfo->isOptionalUnit, true);
	}
	// Add towers, farms, mark them for add
	this->AddTowerResearches(); // Do not use standard algorithm for towers (because of ballista tower specificity.
	this->HandleFarmsInfo();
	
	if (this->settings->isDeathMatch) {
		// Update building count for main military units
		for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits)
		{
			if (!unitInfo->baseUnitDefLiving || unitInfo->isOptionalUnit) { continue; }
			short int bldId = unitInfo->baseUnitDefLiving->trainLocation;
			PotentialBuildingInfo *bldInfo = this->GetBuildingInfo(bldId);
			assert(bldInfo && bldInfo->markedForAdd);
			if (!bldInfo) { continue; }
			// Warning: more than one unit can be trained in same location
			bldInfo->desiredCount *= 3;
			if (bldInfo->desiredCount > 6) {
				bldInfo->desiredCount = 6;
			}
		}
	}

	// Just make sure ages exist and are marked for add/marked as created, for consistency
	for (short int ageResId = AOE_CONST_FUNC::CST_RSID_STONE_AGE; ageResId <= AOE_CONST_FUNC::CST_RSID_IRON_AGE; ageResId++) {
		PotentialResearchInfo *ageResInfo = this->AddPotentialResearchInfoToList(ageResId, true); // add if not already existing
		if (ageResInfo) {
			ageResInfo->markedForAdd = true;
			ageResInfo->isInStrategy = true;
			switch (ageResId) {
			case AOE_CONST_FUNC::CST_RSID_TOOL_AGE:
				ageResInfo->actualStrategyElement = this->seToolAge;
				break;
			case AOE_CONST_FUNC::CST_RSID_BRONZE_AGE:
				ageResInfo->actualStrategyElement = this->seBronzeAge;
				break;
			case AOE_CONST_FUNC::CST_RSID_IRON_AGE:
				ageResInfo->actualStrategyElement = this->seIronAge;
				break;
			default:
				break;
			}
		}
	}

#ifdef _DEBUG
	// Debug check : at this point, all added researches are required and should be marked for add (ignore shadow researches)
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		if (!resInfo->markedForAdd && resInfo->detailedResInfo && !resInfo->detailedResInfo->IsShadowResearch()) {
			this->log += resInfo->langName;
			this->log += " should be marked for add";
			this->log += newline;
		}
	}
#endif

	// Add to potentials all the optional researches for economy (NOT marked for add at this point)
	this->AddResearchesForEconomy();

	// Add user-defined "forced" researches (always developed unless unavailable) and mark them for add
	this->AddUserDefinedForcedResearches();

	if (!this->settings->isDeathMatch) {
		// Finalize exact list of trained units => add units and mark them for add (+ collects/mark for add required researches, if any)
		this->AddMilitaryUnitsForEarlyAges();
	}

	// Unit selection is finished. Check the buildings : collects and marks for add missing required buildings, if any
	this->UpdateRequiredBuildingsFromValidatedResearches();

	// Update internal info in "potential buildings"
	this->UpdateMissingResearchRequirementsInResearchInfo();

	// Add backup TC in internal info
	PotentialBuildingInfo *tcInfo = this->GetBuildingInfo(CST_UNITID_FORUM);
	if (tcInfo && (tcInfo->desiredCount == 1)) {
		tcInfo->desiredCount++;  // Add a backup TC
		tcInfo->highPriority = true; // Even in RM ?
	}

	// If required buildings have not been added to strategy yet, do it now: mark for add the building and also updates "potential research" requirement ready flags.
	this->AddMissingBuildings();

	// Check on requirements
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		if (!resInfo->directRequirementsAreSatisfied) {
			std::string msg = std::string("WARNING: some requirements are not satisfied/") + GetResearchLocalizedName(resInfo->researchId);
			traceMessageHandler.WriteMessage(msg);
			this->log += msg;
			this->log += newline;
#ifdef _DEBUG
			ROCKNROR::SYSTEM::StopExecution(_T("Issue with requirements"), true, true); // temporary
#endif
		}
	}


	// Actually create strategy items

	// Add (and organize) items to strategy : military units (main ones)
	this->CreateMainMilitaryUnitsElements();
	if (!this->settings->isDeathMatch) {
		// Add optional military units (early ages units)
		this->CreateEarlyMilitaryUnitsElements();
	}

	// Choose additional (cheap & useful) researches for "retrains" units - optional
	// Choose additional (cheap & useful) researches for villagers/economy
	this->ChooseOptionalResearches();

	// Add military units requirements (only necessary techs)
	this->CreateMilitaryRequiredResearchesStrategyElements();
	// Add other researches
	this->CreateOtherResearchesStrategyElements();

	// Add buildings to strategy (first building of each kind)
	this->CreateFirstBuildingsStrategyElements();
	this->CreateSecondaryBuildingStrategyElements();
	this->CreateTowerBuildingsStrategyElements();

	if (buildAI->mainAI->needGameStartAIInit) {
		// Warning: automatic element insertions WILL be triggered
	} else {
		// TODO : trigger dynamic element insertions ? Houses, (dock+boats?), setGather%... farms?
	}

	// If game is running, search matching units for strategy elements ?

	// Ending check
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		if (resInfo->markedForAdd && !resInfo->isInStrategy && resInfo->researchDef && (resInfo->researchDef->researchLocation >= 0)) {
			this->log += "Warning: Not added: research=";
			this->log += GetResearchLocalizedName(resInfo->researchId);
			this->log += newline;
		}
	}
	for each (PotentialBuildingInfo *bldInfo in this->potentialBuildingsList)
	{
		if ((bldInfo->unitDefId == CST_UNITID_FARM) && (!this->createFarms)) {
			continue;
		}
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
	return true;
}


// Searches researches that impact a specific living unit and add them to internal list of potential researches.
// Searches recursively required researches EXCEPT for "optional" requirements. No decision is made here.
// allUpgrades: if true, all related upgrades will be added. Otherwise, only requirements will be added.
// Returns a list of research IDs that were actually added to list (some others might already have been in list before)
// You may need to call UpdateRequiredBuildingsFromValidatedResearches, UpdateMissingResearchRequirements and AddMissingBuildings() afterwards
void StrategyBuilder::CollectResearchInfoForUnit(PotentialUnitInfo *unitInfo, bool allUpgrades, bool markForSelection) {
	if (!unitInfo || !ai || !ai->IsCheckSumValid()) { return; }
	if (!player || !player->IsCheckSumValid()) { return; }
	if (!player->ptrResearchesStruct || !player->ptrResearchesStruct->ptrResearchDefInfo) {
		return;
	}

	long int unitDefIdToUse = unitInfo->strongestUpgradeUnitDefId;
	std::set<short int> *allRequiredResearches = NULL;
	if (unitInfo->isOptionalUnit) {
		// just consider base unit (not upgraded unit)
		unitDefIdToUse = unitInfo->unitDefId;
		PotentialResearchInfo *newPotentialRes = this->AddPotentialResearchInfoToList(unitInfo->enabledByResearchId, markForSelection);
		allRequiredResearches = &unitInfo->requiredResearchesForBaseUnit;
	} else {
		this->AddPotentialResearchInfoToList(unitInfo->strongestUpgradeUnitEnabledByResearchId, markForSelection);
		allRequiredResearches = &unitInfo->requiredResearchesForStrongestUpgrade;
	}

	// Add all requirements
	for each (short int oneReqResDefId in *allRequiredResearches)
	{
		TTDetailedResearchDef *oneResDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(oneReqResDefId);
		if (!oneResDtl || !oneResDtl->active || oneResDtl->IsShadowResearch()) { continue; }
		if (oneResDtl ->hasOptionalRequirements || oneResDtl->IsAgeResearch()) {
			continue; // Don't handle age requirements like this, especially because of optional requirements !
		}
		for each (TTDetailedBuildingDef *reqBldDtl in oneResDtl->triggeredByBuilding) {
			if (reqBldDtl->possibleAncestorUnitIDs.size() > 0) { continue; } // Ignore non-root buildings
			// We have a required building.
			this->AddPotentialBuildingInfoToList(reqBldDtl->unitDef, markForSelection);
			// Make sure building is enabled, this will add underlying requirements.
			for each (short int tmpId in reqBldDtl->researchIdsThatEnableMe) {
				this->AddPotentialResearchInfoToList(tmpId, markForSelection);
			}
		}

		if (!this->AddPotentialResearchInfoToList(oneReqResDefId, markForSelection)) {
			this->log += "Warning: could not add research ";
			this->log += oneResDtl->langName;
			this->log += newline;
		}
	}

	if (allUpgrades) {
		for each (TTDetailedResearchDef *resDtl in unitInfo->availableRelatedResearchDetail) {
			if (resDtl->IsShadowResearch()) { continue; }
			if (this->settings->isDeathMatch && resDtl->CanExcludeInDeathMatchAI()) {
				continue; // DM exclusions: AI unsupported (martyrdom...)
			}
			if (!this->settings->isDeathMatch && resDtl->CanExcludeInRandomMapAI()) {
				continue; // RM exclusions : jihad, ballista tower... (all with negative effects except warboats = for trireme)
			}
			// This will add the potential research and the necessary building + requirements
			if (!this->AddPotentialResearchInfoToList((short int)resDtl->GetResearchDefId(), markForSelection)) {
				this->log += "Warning: could not add ";
				this->log += resDtl->langName;
				this->log += newline;
			}
		}
	}
	// To return a number, we could count the number of potential researches/buildings before and after ?
}

// Searches researches that impact a specific building and add them to internal list of potential researches.
// Searches recursively required researches EXCEPT for "optional" requirements. No decision is made here.
// allUpgrades: if true, all related upgrades will be added. Otherwise, only requirements will be added.
// Returns a list of research IDs that were actually added to list (some others might already have been in list before)
// You may need to call UpdateRequiredBuildingsFromValidatedResearches, UpdateMissingResearchRequirements and AddMissingBuildings() afterwards
void StrategyBuilder::CollectResearchInfoForUnit(PotentialBuildingInfo *bldInfo, bool allUpgrades, bool markForSelection) {
	if (!bldInfo || !ai || !ai->IsCheckSumValid()) { return; }
	if (!player || !player->IsCheckSumValid()) { return; }
	if (!player->ptrResearchesStruct || !player->ptrResearchesStruct->ptrResearchDefInfo) {
		return;
	}

	//long int unitDefIdToUse = unitInfo->strongestUpgradeUnitDefId;
	TTDetailedBuildingDef *bldDtl = bldInfo->unitDefDetailedInfo;
	assert(bldDtl && bldDtl->IsValid());
	if (!bldDtl || !bldDtl->IsValid()) { return; }

	PotentialResearchInfo *newPotentialRes = NULL;
	if (bldInfo->unitDefDetailedInfo->researchIdsThatEnableMe.size() > 0) {
		newPotentialRes = this->AddPotentialResearchInfoToList((short int)*bldDtl->researchIdsThatEnableMe.begin(), markForSelection);
	}
	if (newPotentialRes == NULL) {
		// No required research
		return;
	}
	
	// Add all requirements
	for (auto it = bldInfo->requiredResearchesForBaseUnit.cbegin(); it != bldInfo->requiredResearchesForBaseUnit.cend(); it++)
	//for each (short int oneReqResDefId in bldInfo->requiredResearchesForBaseUnit)
	{
		short int oneReqResDefId = *it;
		TTDetailedResearchDef *oneResDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(oneReqResDefId);
		if (!oneResDtl || !oneResDtl->active || oneResDtl->IsShadowResearch()) { continue; }
		if (oneResDtl->hasOptionalRequirements || oneResDtl->IsAgeResearch()) {
			continue; // Don't handle age requirements like this, especially because of optional requirements !
		}
		for each (TTDetailedBuildingDef *reqBldDtl in oneResDtl->triggeredByBuilding) {
			if (reqBldDtl->possibleAncestorUnitIDs.size() > 0) { continue; } // Ignore non-root buildings
			// We have a required building.
			this->AddPotentialBuildingInfoToList(reqBldDtl->unitDef, markForSelection);
			// Make sure building is enabled, this will add underlying requirements.
			for each (short int tmpId in reqBldDtl->researchIdsThatEnableMe) {
				this->AddPotentialResearchInfoToList(tmpId, markForSelection);
			}
		}

		if (!this->AddPotentialResearchInfoToList(oneReqResDefId, markForSelection)) {
			this->log += "Warning: could not add research ";
			this->log += oneResDtl->langName;
			this->log += newline;
		}
	}

	if (allUpgrades) {
		for each (TTDetailedResearchDef *resDtl in bldDtl->affectedByResearches) {
			if (!this->IsResearchInTechTree((short int)resDtl->GetResearchDefId())) {
				continue;
			}
		//for each (TTDetailedResearchDef *resDtl in bldInfo->availableRelatedResearchDetail) {
			if (resDtl->IsShadowResearch()) { continue; }
			if (this->settings->isDeathMatch && resDtl->CanExcludeInDeathMatchAI()) {
				continue; // DM exclusions: AI unsupported (martyrdom...)
			}
			if (!this->settings->isDeathMatch && resDtl->CanExcludeInRandomMapAI()) {
				continue; // RM exclusions : jihad, ballista tower... (all with negative effects except warboats = for trireme)
			}
			// This will add the potential research and the necessary building + requirements
			if (!this->AddPotentialResearchInfoToList((short int)resDtl->GetResearchDefId(), markForSelection)) {
				this->log += "Warning: could not add ";
				this->log += resDtl->langName;
				this->log += newline;
			}
		}
	}
	// To return a number, we could count the number of potential researches/buildings before and after ?
}


// Adds (and marks for add) all necessary buildings for "validated" researches to buildings ID list.
// Returns number of building IDs added to internal list
int StrategyBuilder::UpdateRequiredBuildingsFromValidatedResearches() {
	int addedElements = 0;
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		if (resInfo->markedForAdd && resInfo->researchDef) {
			short int resLocationId = resInfo->researchDef->researchLocation;
			// Research's location
			if ((resInfo->researchDef->researchLocation >= 0) &&
				//(!IsUnitDisabledInTechTree(this->player->playerId, resInfo->researchDef->researchLocation))
				!this->playerTechTreeInfo->IsBuildingUnitDisabled(resLocationId)) {
				// This only adds if not already in list
				if (this->AddPotentialBuildingInfoToList(resInfo->researchDef->researchLocation, true)) {
					addedElements++;
				}
			}
			// Building that enables research
			AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *parentBld = FindBuildingDefThatEnablesResearch(this->player, resInfo->researchId);
			if (parentBld) {
				if (this->AddPotentialBuildingInfoToList(parentBld->DAT_ID1, true)) {
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
void StrategyBuilder::UpdateMissingResearchRequirementsInResearchInfo() {
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

// Add missing buildings - if any - that block some research requirements, also mark them for add
// WRONG(removed): Potential research info "requirements" flags are updated to take into account the new building (in case it unlocks some dependencies).
void StrategyBuilder::AddMissingBuildings() {
#pragma WARNING("Dirty handling of missing requirements. We should store info about all shadow research?")
	TTDetailedResearchDef *ironDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(AOE_CONST_FUNC::CST_RSID_IRON_AGE);
	auto it = std::find_if(ironDtl->allRequirements.cbegin(), ironDtl->allRequirements.cend(), 
		[](TTDetailedResearchDef *dtl){ return dtl->hasOptionalRequirements; }
	);
	for (; it != ironDtl->allRequirements.cend(); it++) {
		// We are looping on all iron age requirements that include optional requirements (the *** shadow researches that allow choosing between several buildings)
		TTDetailedResearchDef *shadow = *it;
		if (!shadow || !shadow->active || !shadow->researchDef) { continue; }
		int requiredCount = shadow->researchDef->minRequiredResearchesCount;
		int foundCount = 0;
		for (int i = 0; i < 4; i++) {
			short int curReqId = shadow->researchDef->requiredResearchId[i];
			TTDetailedResearchDef *curDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(curReqId);
			
			bool isInStrategy = false;
			if (curDtl) {
				auto searchSPGranary = std::find_if(curDtl->triggeredByBuilding.begin(), curDtl->triggeredByBuilding.end(),
					[](TTDetailedBuildingDef *bDtl){ return (bDtl != NULL) && (bDtl->unitDefId == CST_UNITID_STORAGE_PIT) || (bDtl->unitDefId == CST_UNITID_GRANARY); }
				);
				isInStrategy = (searchSPGranary != curDtl->triggeredByBuilding.end()); // SP/granary will be added by AI. Consider they're OK.
			}
			PotentialResearchInfo *potentialRes = this->GetResearchInfo(curReqId);
			if (potentialRes && potentialRes->markedForAdd) {
				isInStrategy = true;
			} else {
				if ((curReqId >= AOE_CONST_FUNC::CST_RSID_STONE_AGE) && (curReqId <= AOE_CONST_FUNC::CST_RSID_IRON_AGE)) {
					isInStrategy = true; // ages : we always include them in strategy !
				}
			}
			if (isInStrategy) {
				foundCount++;
			}
		}
		if (foundCount < requiredCount) {
			this->log += "Need to add stuff for shadow research #";
			this->log += std::to_string(shadow->GetResearchDefId());
			this->log += newline;

			std::list<AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING*> missingBuildingsDef;
			missingBuildingsDef.clear();
			foundCount = 0;
			for each (auto v in shadow->directRequirements)
			{
				for each (auto vv in v->triggeredByBuilding)
				{
					if (vv->possibleAncestorUnitIDs.size() > 0) {
						continue; // ignore non-base buildings
					}
					AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING*)vv->GetUnitDef();
					if (unitDef->IsCheckSumValid()) {
						PotentialBuildingInfo *potentialBld = this->GetBuildingInfo(unitDef);
						if (potentialBld) {
							if (potentialBld->markedForAdd) {
								foundCount++;
							} else {
								potentialBld->markedForAdd = true;
								foundCount++;
							}
						} else {
							missingBuildingsDef.push_back(unitDef);
						}
					}
					if (foundCount >= requiredCount) { break; }
				}
			}
			while ((foundCount < requiredCount) && (missingBuildingsDef.size() > 0)) {
				AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *bldDef = missingBuildingsDef.front();
				missingBuildingsDef.pop_front();
				if (!this->playerTechTreeInfo->IsBuildingUnitDisabled(bldDef->DAT_ID1)) {
				//if (!IsUnitDisabledInTechTree(this->player->playerId, bldDef->DAT_ID1)) {
					this->log.append("Added building #" + std::to_string(bldDef->DAT_ID1) + " (" + bldDef->ptrUnitName + ") to list due to missing requirements.\n");
					this->AddPotentialBuildingInfoToList(bldDef, true);
					foundCount++;
				}
			}

		}
	}


	/*assert(this->global && this->player);
	for each (PotentialResearchInfo *resInfo in this->potentialResearchesList)
	{
		std::list<AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING*> missingBuildingsDef;
		missingBuildingsDef.clear();
		if (!resInfo->directRequirementsAreSatisfied) {
			for (int i = 0; i < 4; i++) {
				if ((resInfo->missingRequiredResearches[i] >= 0) && (this->IsResearchInTechTree(resInfo->missingRequiredResearches[i]))) {
					AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *bldDef = FindBuildingDefThatEnablesResearch(this->player, resInfo->missingRequiredResearches[i]);
					if (bldDef && bldDef->IsCheckSumValid()) {
						bool found = false;
						// We found a building that satisfies a requirement. Is this building (already) in my list
						// (if member info had been recomputed, it should not happen here because resInfo->missingRequiredResearches[i] would be -1)
						for each (PotentialBuildingInfo *bldInfo in this->potentialBuildingsList)
						{
							if (bldInfo->unitDefId == bldDef->DAT_ID1) {
								if (!bldInfo->markedForAdd) {
									CR_DEBUG::StopExecution(_T("Possible bug: building is known but not marked for add => considered as present in strategy !"), true, true);
								}
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
			for each (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *bldDef in missingBuildingsDef)
			{
				// This loop is here to give priority to temple, if relevant
				if (bldDef->DAT_ID1 == CST_UNITID_TEMPLE) {
					useTemple = true;
				}
			}
			if (useTemple) {
				auto it = std::remove_if(missingBuildingsDef.begin(), missingBuildingsDef.end(),
					[](AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *bldDef) { return bldDef->DAT_ID1 == CST_UNITID_TEMPLE; }
				);
				missingBuildingsDef.erase(it, missingBuildingsDef.end());
			}

			int loopCount = 0;
			while (!resInfo->directRequirementsAreSatisfied && (loopCount < 4) && (useTemple || !missingBuildingsDef.empty())) {
				AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *bldDef = NULL;
				if (useTemple) {
					bldDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)this->player->GetUnitDefBase(CST_UNITID_TEMPLE);
					useTemple = false;
				} else {
					bldDef = missingBuildingsDef.front();
					missingBuildingsDef.pop_front(); // remove the item we just read
				}
				if (!IsUnitDisabledInTechTree(this->player->playerId, bldDef->DAT_ID1)) {
					this->log.append("Added building #" + std::to_string(bldDef->DAT_ID1) + " (" + bldDef->ptrUnitName + ") to list due to missing requirements.\n");
					this->AddPotentialBuildingInfoToList(bldDef, true);
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
	}*/
}
