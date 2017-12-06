#include "../include/TechTreeAnalyzer.h"

// \r is required to display correctly in ROR editable text components
#define NEWLINE "\r\n"

using namespace AOE_STRUCTURES;
using namespace AOE_CONST_FUNC;

namespace ROCKNROR {
namespace STRATEGY {
;


// Fill detailedResearches info from global game structure (from DAT info)
// Does not evaluate indirect dependencies, etc
// requiredAge is set only if there is a direct dependency
bool TechTreeAnalyzer::ReadRawDataFromDat() {
	this->FreeArrays(); // clean previous data, if any
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid() || !global->technologiesInfo || 
		!global->technologiesInfo->IsCheckSumValid()) { return false; }
	assert(global->researchDefInfo != NULL);
	if (!global->researchDefInfo) { return false; }
	if (global->civCount < 2) { return false; }
	STRUCT_CIVILIZATION_DEF *civDef1 = global->civilizationDefinitions[1];

	this->researchCount = global->researchDefInfo->researchCount;
	this->unitDefCount = civDef1->civUnitDefCount;
	this->technologyCount = global->technologiesInfo->technologyCount;

	// Collect tech tree IDs
	this->techTreeTechIDs.clear();
	for (int i = 0; i < global->civCount; i++) {
		STRUCT_CIVILIZATION_DEF *civDef = global->civilizationDefinitions[i];
		this->techTreeTechIDs.insert(civDef->techTreeId);
	}

	if ((this->researchCount <= 0) || (this->unitDefCount <= 0)) { return false; }
	this->AllocArrays();
	for (int resId = 0; resId < this->researchCount; resId++) {
		STRUCT_RESEARCH_DEF *resDef = global->researchDefInfo->GetResearchDef(resId);
		TTDetailedResearchDef *detail = this->GetDetailedResearchDef(resId);
		assert(detail != NULL);
		if (resDef) {
			detail->internalName = std::string((resDef->researchName != NULL) ? resDef->researchName : "");
			detail->langName = GetResearchLocalizedName((short int)detail->GetResearchDefId());
			if (detail->langName.empty()) {
				detail->langName = detail->internalName;
			}
			detail->researchDef = resDef;
			detail->techDef = global->GetTechDef(resDef->technologyId);
			detail->allRequirementsAreKnown = false;
			detail->active = true;
			if (detail->techDef != NULL) { // some shadow researches have effect=-1 (they are useful for dependencies)
				bool hasValidEffect = false;
				for (int effectIndex = 0; effectIndex < detail->techDef->effectCount; effectIndex++) {
					hasValidEffect = detail->techDef->ptrEffects[effectIndex].HasValidEffect();

					// Special: is AI-supported effect
					if (detail->techDef->ptrEffects[effectIndex].IsResourceModifier()) {
						if ((detail->techDef->ptrEffects[effectIndex].effectUnit == CST_RES_ORDER_PRIEST_SACRIFICE) ||
							(detail->techDef->ptrEffects[effectIndex].effectUnit == CST_RES_ORDER_SHARED_EXPLORATION)) {
							detail->isAiUnsupported = true;
						}
					}
				}
				detail->hasValidEffect = hasValidEffect;
				// Special: has negative side effect (jihad, etc)
				if (HasTechNegativeSideEffect(detail->techDef)) {
					detail->hasNegativeSideEffect = true;
				}
			}
			
			if (resDef->minRequiredResearchesCount > 0) {
				int valuedReqResearchCount = 0;
				for (int i = 0; i < 4; i++) {
					if (resDef->requiredResearchId[i] >= 0) {
						valuedReqResearchCount++;
						short int reqResId = resDef->requiredResearchId[i];
						STRUCT_RESEARCH_DEF *reqResDef = global->researchDefInfo->GetResearchDef(reqResId);
						if (reqResDef) {
							// Add to direct requirements (if id is valid)
							detail->directRequirements.insert(this->GetDetailedResearchDef(reqResId));
						}
						switch (reqResId) {
						case CST_RSID_STONE_AGE:
						case CST_RSID_TOOL_AGE:
						case CST_RSID_BRONZE_AGE:
						case CST_RSID_IRON_AGE:
							detail->requiredAge = reqResId;
						default:
							break;
						}
					}
				}
				if (resDef->minRequiredResearchesCount < valuedReqResearchCount) {
					detail->hasOptionalRequirements = true;
				}
			}
		}
	}
	return true;
}


// Update detailedResearches info to find buildings that trigger researches
// Finds (and create in this->detailedBuildings) all buildings that trigger a research.
// Also feeds this->detailedTrainables
void TechTreeAnalyzer::FindBuildingThatTriggerResearches() {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return; }
	if (global->civCount < 2) { return; }
	STRUCT_CIVILIZATION_DEF *civDef1 = global->civilizationDefinitions[1];
	assert(civDef1 && (this->unitDefCount == civDef1->civUnitDefCount));

	for (int unitDefId = 0; unitDefId < civDef1->civUnitDefCount; unitDefId++) {
		STRUCT_UNITDEF_TRAINABLE *unitDefTrainable = (STRUCT_UNITDEF_TRAINABLE*)civDef1->GetUnitDef(unitDefId);
		STRUCT_UNITDEF_BUILDING *unitDefBld = (STRUCT_UNITDEF_BUILDING*)civDef1->GetUnitDef(unitDefId);
		if (unitDefTrainable && (unitDefTrainable->unitType == GUT_TRAINABLE) && unitDefTrainable->IsCheckSumValid()) {
			this->InitGetDetailedTrainableUnitDef(unitDefTrainable); // required to create internal data about this building
		}
		if (unitDefBld && (unitDefBld->unitType == GUT_BUILDING) && unitDefBld->IsCheckSumValid()) {
			TTDetailedResearchDef *detail = this->GetDetailedResearchDef(unitDefBld->initiatesResearch);
			this->InitGetDetailedBuildingDef(unitDefBld); // required to create internal data about this building
			if (detail) {
				detail->triggeredByBuilding.insert(this->GetDetailedBuildingDef(unitDefId));
			}
		}
	}
}


// Update this->detailedBuildings with researches that enable the buildings (if not available at start)
// Also fills possibleUpgradedUnitIDs / possibleParentUnitIDs for links with base unit (only)
void TechTreeAnalyzer::FindResearchesThatEnableUnits() {
	for (int resDefId = 0; resDefId < this->researchCount; resDefId++) {
		TTDetailedResearchDef *detailRes = this->detailedResearches[resDefId];

		// Take the occasion to feed TTDetailedBuildingDef->researchesDevelopedHere
		if (!detailRes->researchDef) { continue; }
		if (detailRes->researchDef->researchLocation >= 0) {
			TTDetailedBuildingDef *trainLocDetail = this->GetDetailedBuildingDef(detailRes->researchDef->researchLocation);
			if (trainLocDetail && trainLocDetail->IsValid() && !detailRes->IsShadowResearch()) {
				trainLocDetail->researchesDevelopedHere.insert(resDefId);
			}
		}

		STRUCT_TECH_DEF *techDef = detailRes->techDef;
		if (!techDef) { 
			continue;
		}
		for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
			STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
			// Case 1 : Enable unit
			if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (techEffect->effectClass == 1)/*enable*/) {
				long int enabledUnitId = techEffect->effectUnit;
				TTDetailedBuildingDef *detailBuilding = this->GetDetailedBuildingDef(enabledUnitId);
				if ((detailBuilding != NULL) && (detailBuilding->unitDef != NULL)) {
					detailBuilding->researchIdsThatEnableMe.insert(resDefId);
					detailRes->enableBuildings.insert(detailBuilding);
				}
				TTDetailedTrainableUnitDef *detailTrainable = this->GetDetailedTrainableUnitDef(enabledUnitId);
				if ((detailTrainable != NULL) && (detailTrainable->unitDef != NULL)) {
					detailTrainable->researchIdsThatEnableMe.insert(resDefId);
					detailRes->enableTrainables.insert(detailTrainable);
				}
			}
			
			// Case 2 : Upgrade some unit TO THIS unit
			if (techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) {
				long int upgradedUnitId = techEffect->effectClass;
				long int parentUnitId = techEffect->effectUnit;
				TTDetailedBuildingDef *detailBuilding = this->GetDetailedBuildingDef(upgradedUnitId);
				if ((detailBuilding != NULL) && (detailBuilding->unitDef != NULL)) {
					detailBuilding->researchIdsThatEnableMe.insert(resDefId);
					detailRes->enableBuildings.insert(detailBuilding);
					if (std::find(detailBuilding->possibleAncestorUnitIDs.cbegin(), detailBuilding->possibleAncestorUnitIDs.cend(), parentUnitId) == detailBuilding->possibleAncestorUnitIDs.cend()) {
						detailBuilding->possibleAncestorUnitIDs.push_back(parentUnitId);
					}

					TTDetailedBuildingDef *detailBuildingParent = this->GetDetailedBuildingDef(parentUnitId);
					if ((detailBuildingParent != NULL) && (detailBuildingParent->unitDef != NULL)) {
						if (std::find(detailBuildingParent->possibleUpgradedUnitIDs.cbegin(), 
							detailBuildingParent->possibleUpgradedUnitIDs.cend(), upgradedUnitId) == detailBuildingParent->possibleUpgradedUnitIDs.end()) {
							detailBuildingParent->possibleUpgradedUnitIDs.push_back(upgradedUnitId);
						}
					}
				}
				TTDetailedTrainableUnitDef *detailTrainable = this->GetDetailedTrainableUnitDef(upgradedUnitId);
				if ((detailTrainable != NULL) && (detailTrainable->unitDef != NULL)) {
					detailTrainable->researchIdsThatEnableMe.insert(resDefId);
					detailRes->enableTrainables.insert(detailTrainable);
					if (std::find(detailTrainable->possibleAncestorUnitIDs.cbegin(), detailTrainable->possibleAncestorUnitIDs.cend(), parentUnitId) == detailTrainable->possibleAncestorUnitIDs.cend()) {
						detailTrainable->possibleAncestorUnitIDs.push_back(parentUnitId);
					}
					detailRes->upgradedUnitDefId.insert(std::pair<long int, long int>(parentUnitId, upgradedUnitId));

					TTDetailedTrainableUnitDef *detailTrainableParent = this->GetDetailedTrainableUnitDef(parentUnitId);
					if ((detailTrainableParent != NULL) && (detailTrainableParent->unitDef != NULL)) {
						if (std::find(detailTrainableParent->possibleUpgradedUnitIDs.cbegin(), detailTrainableParent->possibleUpgradedUnitIDs.cend(), upgradedUnitId) == detailTrainableParent->possibleUpgradedUnitIDs.cend()) {
							detailTrainableParent->possibleUpgradedUnitIDs.push_back(upgradedUnitId);
						}
					}
				}
			}
		}
	}

	// We'll sort possibleUpgradedUnitIDs and possibleAncestorUnitIDs later
}


// this->detailedBuildings with units (trainable) that are trained there
void TechTreeAnalyzer::CollectTrainLocations() {
	for (int unitDefId = 0; unitDefId < this->unitDefCount; unitDefId++) {
		TTDetailedTrainableUnitDef *trainableDetail = this->GetDetailedTrainableUnitDef(unitDefId);
		if (trainableDetail && trainableDetail->IsValid() && (trainableDetail->GetTrainLocation() >= 0)) {
			long int trainLocation = trainableDetail->GetTrainLocation();
			TTDetailedBuildingDef *bldDetail = this->GetDetailedBuildingDef(trainLocation);
			if (bldDetail && bldDetail->IsValid() && !trainableDetail->IsHeroOrScenarioUnit()) {
				bldDetail->unitsTrainedHere.insert(trainableDetail->unitDef);
			}
		}
	}
}


void TechTreeAnalyzer::MarkBuildingAsAvailable(long int unitDefId) {
	TTDetailedBuildingDef *detailBld = this->detailedBuildings[unitDefId];
	if (!detailBld || !detailBld->unitDef) { return; }
	if (detailBld->isAvailableAfterAnalysis) { return; } // Already marked as available.
	detailBld->isAvailableAfterAnalysis = true;
	// Building just became ready : simulate the "initiate research" effect (if there is an "initiate research")
	TTDetailedResearchDef *initiateResearchDetail = this->GetDetailedResearchDef(detailBld->initiatesResearch); // may return NULL as ID may be -1
	if (initiateResearchDetail && initiateResearchDetail->active) {
		if (initiateResearchDetail->directRequirements.size() > 0) {
			this->myLog.append("[INFO] Research #");
			this->myLog.append(std::to_string(initiateResearchDetail->GetResearchDefId()));
			this->myLog.append(" (");
			this->myLog.append(initiateResearchDetail->langName);
			this->myLog.append(") is triggered by building '");
			this->myLog.append(detailBld->langName);
			this->myLog.append("' but has requirements. This is normal for building enablers (also enabled by bld of same kind)." NEWLINE);
		}

		// Note: this can happen more than once if several buildings trigger the same research (e.g. TC_std(109) and TC_upgrade(71))
		if (!initiateResearchDetail->allRequirementsAreKnown) {
			initiateResearchDetail->allRequirementsAreKnown = true;

			int i = detailBld->researchIdsThatEnableMe.size();
			if (i > 1) {
				this->myLog.append("Warning: building #");
				this->myLog.append(std::to_string(detailBld->unitDefId));
				this->myLog.append(" has more than one enabling research" NEWLINE);
			}
			for each (long int parentResDefId in detailBld->researchIdsThatEnableMe)
			{
				TTDetailedResearchDef *parentResDetail = this->GetDetailedResearchDef(parentResDefId);
				if (parentResDetail && parentResDetail->active) {
					initiateResearchDetail->directRequirements.insert(parentResDetail); // research that enable building is considered DIRECT requirement for building's initiate research.
					initiateResearchDetail->AddAllRequirementsFrom(parentResDetail);
				}
			}

			this->myLog.append("Research #");
			this->myLog.append(std::to_string(initiateResearchDetail->GetResearchDefId()));
			this->myLog.append(" / ");
			this->myLog.append(initiateResearchDetail->langName);
			this->myLog.append(" : all requirements are known (from building #");
			this->myLog.append(std::to_string(detailBld->unitDefId));
			this->myLog.append(")" NEWLINE);
		}
	}
	if (detailBld->initiatesResearch == -1) {
		this->myLog.append("Building #");
		this->myLog.append(std::to_string(detailBld->unitDefId));
		this->myLog.append(" / ");
		this->myLog.append(detailBld->langName);
		this->myLog.append(" is now available." NEWLINE);
	}

	for each (long int reqResId in detailBld->researchIdsThatEnableMe) {
		TTDetailedResearchDef *reqRes = this->GetDetailedResearchDef(reqResId);
		if (reqRes && reqRes->active && (detailBld->requiredAge < reqRes->requiredAge)) {
			detailBld->requiredAge = reqRes->requiredAge;
		}
	}
	if (detailBld->requiredAge == -1) {
		detailBld->requiredAge = AOE_CONST_FUNC::CST_RSID_STONE_AGE;
	}
}


void TechTreeAnalyzer::MarkTrainableUnitAsAvailable(long int unitDefId) {
	TTDetailedTrainableUnitDef *detailTrainable = this->GetDetailedTrainableUnitDef(unitDefId);
	if (!detailTrainable || !detailTrainable->unitDef) { return; }
	if (detailTrainable->isAvailableAfterAnalysis) { return; } // Already marked as available.
	detailTrainable->isAvailableAfterAnalysis = true;

	for each (long int reqResId in detailTrainable->researchIdsThatEnableMe) {
		TTDetailedResearchDef *reqRes = this->GetDetailedResearchDef(reqResId);
		if (reqRes && reqRes->active && (detailTrainable->requiredAge < reqRes->requiredAge)) {
			detailTrainable->requiredAge = reqRes->requiredAge;
		}
	}
	if (detailTrainable->requiredAge == -1) {
		detailTrainable->requiredAge = AOE_CONST_FUNC::CST_RSID_STONE_AGE;
	}

	this->myLog.append("Unit #");
	this->myLog.append(std::to_string(detailTrainable->unitDefId));
	this->myLog.append(" / ");
	this->myLog.append(detailTrainable->langName);
	this->myLog.append(" is now available." NEWLINE);
}


// Update required researches for researches whose direct dependencies are all satisfied (all required researches are known)
// Returns the number of researches that are now "satisfied"
int TechTreeAnalyzer::EvaluateRequiredResearchesSimple() {
	int updatedResearches = 0;
	this->myLog.append("New evaluation of research triggered by available buildings." NEWLINE);
	for (int unitDefId = 0; unitDefId < this->unitDefCount; unitDefId++) {
		TTDetailedBuildingDef *detailBld = this->detailedBuildings[unitDefId];
		if (detailBld && detailBld->unitDef) {
			bool buildingIsReady = detailBld->isAvailableImmediately;
			if (!buildingIsReady) {
				auto it = std::find_if(detailBld->researchIdsThatEnableMe.begin(), detailBld->researchIdsThatEnableMe.end(),
					[this](long int lambdaDefId) {
					TTDetailedResearchDef *detailResDef = this->GetDetailedResearchDef(lambdaDefId);
					return (detailResDef != NULL) && detailResDef->allRequirementsAreKnown;
				});
				if (it != detailBld->researchIdsThatEnableMe.end()) {
					// Found at least 1 "ready" research that enables the building
					buildingIsReady = true;
				}
			}

			if (buildingIsReady && !detailBld->isAvailableAfterAnalysis) {
				this->MarkBuildingAsAvailable(unitDefId);
			}
		}
		TTDetailedTrainableUnitDef *detailTrainable = this->GetDetailedTrainableUnitDef(unitDefId);
		if (detailTrainable && detailTrainable->unitDef) {
			bool trainableIsReady = detailTrainable->isAvailableImmediately;
			if (!trainableIsReady) {
				auto it = std::find_if(detailTrainable->researchIdsThatEnableMe.begin(), detailTrainable->researchIdsThatEnableMe.end(),
					[this](long int lambdaDefId) {
					TTDetailedResearchDef *detailResDef = this->GetDetailedResearchDef(lambdaDefId);
					return (detailResDef != NULL) && detailResDef->allRequirementsAreKnown;
				});
				if (it != detailTrainable->researchIdsThatEnableMe.end()) {
					// Found at least 1 "ready" research that enables the building
					trainableIsReady = true;
				}
			}
			if (trainableIsReady && !detailTrainable->isAvailableAfterAnalysis) {
				this->MarkTrainableUnitAsAvailable(unitDefId);
			}
		}

	}

	this->myLog.append("New evaluation of trivial research dependencies." NEWLINE);
	for (int researchId = 0; researchId < this->researchCount; researchId++) {
		TTDetailedResearchDef *detail = this->GetDetailedResearchDef(researchId);
		if (!detail || !detail->active) { continue; }
		if (detail->allRequirementsAreKnown) { continue; }
		bool allReqSatisfied = true;
		for each (TTDetailedResearchDef *reqDetail in detail->directRequirements)
		{
			allReqSatisfied &= reqDetail->allRequirementsAreKnown;
		}
		// Note: research is automatically triggered if "allSatisfied" && (researchTime == 0)
		// But research CAN be developed (by player) if "allSatisfied" && (researchTime != 0) && (researchLocation >= 0) && (building is available)
		// Otherwise, the research needs to be triggered by a building ?
		bool allowedResearch = (detail->researchDef->researchTime == 0);
		long int researchLocation = detail->researchDef->researchLocation;
		TTDetailedBuildingDef *researchLocDetail = this->GetDetailedBuildingDef(researchLocation);
		std::string additionalLogInfo = "";
		if (researchLocDetail && researchLocDetail->isAvailableAfterAnalysis) {
			allowedResearch = true;
			if (allReqSatisfied) {
				additionalLogInfo = " thanks to building #";
				additionalLogInfo.append(std::to_string(researchLocation));
			}
		}
		// Can we consider the research as done ?
		// allReqSatisfied=OK for required researches ; allowedResearch=OK regarding researchLocation/time
		if (allReqSatisfied && allowedResearch) {
			updatedResearches++;
			detail->allRequirementsAreKnown = true;
			for each (TTDetailedResearchDef *reqDetail in detail->directRequirements)
			{
				detail->allRequirementsExcludingAges.insert(reqDetail);
				detail->AddAllRequirementsFrom(reqDetail); // inherit required research's requirements
			}
			this->myLog.append("Research #");
			this->myLog.append(std::to_string(detail->GetResearchDefId()));
			this->myLog.append(" / ");
			this->myLog.append(detail->langName);
			this->myLog.append(" : all requirements are known =>");
			for each (TTDetailedResearchDef *reqDetail in detail->allRequirementsExcludingAges)
			{
				this->myLog.append(" ");
				this->myLog.append(std::to_string(reqDetail->GetResearchDefId()));
			}
			this->myLog.append(additionalLogInfo);
			this->myLog.append(NEWLINE);
		}
	}

	return updatedResearches;
}


void TechTreeAnalyzer::EvaluateRequiredResearches() {
	int updatedCount = 1;
	while (updatedCount > 0) {
		updatedCount = this->EvaluateRequiredResearchesSimple();
	}
	this->myLog.append("No more researches to evaluate." NEWLINE);
}


// Fill "unreachableResearches" from detailed research info (requirements must have been computed)
int TechTreeAnalyzer::CollectUnreachableResearches() {
	int count = 0;
	for (int researchId = 0; researchId < this->researchCount; researchId++) {
		TTDetailedResearchDef *detail = this->GetDetailedResearchDef(researchId);
		if (!detail || !detail->active) { continue; }
		if (detail->allRequirementsAreKnown) { continue; }
		this->unreachableResearches.insert(detail);
		count++;
		this->myLog.append("Warning: unreachable research #");
		this->myLog.append(std::to_string(researchId));
		this->myLog.append(" / ");
		this->myLog.append(detail->langName);
		this->myLog.append(" dep=> [");
		for each (auto req in detail->directRequirements)
		{
			this->myLog.append(" ");
			this->myLog.append(std::to_string(req->GetResearchDefId()));
		}
		this->myLog.append("]");
		this->myLog.append(NEWLINE);
	}
	return count;
}


// Update allChildResearches for each research info
// Also updates "researchLine" predecessors/successors
void TechTreeAnalyzer::UpdateChildResearchDependencies() {
	// Update child researches
	for (int resDefId = 0; resDefId < this->researchCount; resDefId++) {
		TTDetailedResearchDef *detail = this->GetDetailedResearchDef(resDefId);
		if (!detail || !detail->active) { continue; }

		for (int childId = 0; childId < this->researchCount; childId++) {
			TTDetailedResearchDef *child = this->GetDetailedResearchDef(childId);
			if (child && child->active && (child != detail)) {
				auto x = std::find(child->allRequirementsExcludingAges.begin(), child->allRequirementsExcludingAges.end(), detail);
				if (x != child->allRequirementsExcludingAges.end()) {
					// Found
					detail->allChildResearches.insert(child);
				}
			}
		}
	}
	// Handle "research lines" = series of researches on a same location/button
	for (int res1Id = 0; res1Id < this->researchCount; res1Id++) {
		TTDetailedResearchDef *res1dtl = this->GetDetailedResearchDef(res1Id);
		if (!res1dtl || !res1dtl->active || res1dtl->IsShadowResearch()) { continue; }
		for (int res2Id = 0; res2Id < this->researchCount; res2Id++) {
			TTDetailedResearchDef *res2dtl = this->GetDetailedResearchDef(res2Id);
			if (res2dtl && res2dtl->active && (res2dtl != res1dtl) && !res2dtl->IsShadowResearch()) {
				STRUCT_RESEARCH_DEF *res1 = res1dtl->researchDef;
				STRUCT_RESEARCH_DEF *res2 = res2dtl->researchDef;
				if (res1 && res2 && (res1->researchLocation == res2->researchLocation) && (res1->researchLocation >= 0) &&
					(res1->buttonId == res2->buttonId)) {
					// We got 2 (different) researches, non-shadow, in same building & buttonId => same lineage
					auto it = std::find(res1dtl->allRequirements.begin(), res1dtl->allRequirements.end(), res2dtl);
					//auto it = std::find_if(res1dtl->allRequirements.begin(), res1dtl->allRequirements.end(),
						//[res2dtl](TTDetailedResearchDef *lambdaDtl) {return lambdaDtl == res2dtl; });
					if (it == res1dtl->allRequirements.end()) {
						// Res2 not found in res1's requirements => res1 comes BEFORE res2
						if (std::find(res1dtl->researchLineSuccessors.cbegin(), res1dtl->researchLineSuccessors.cend(), res2dtl) == res1dtl->researchLineSuccessors.end()) {
							res1dtl->researchLineSuccessors.push_back(res2dtl);
						}
						if (std::find(res2dtl->researchLinePredecessors.cbegin(), res2dtl->researchLinePredecessors.cend(), res1dtl) == res2dtl->researchLinePredecessors.end()) {
							res2dtl->researchLinePredecessors.push_back(res1dtl);
						}
					} else {
						// Res2 found in res1's requirements => res2 comes BEFORE res1
						if (std::find(res2dtl->researchLineSuccessors.cbegin(), res2dtl->researchLineSuccessors.cend(), res1dtl) == res2dtl->researchLineSuccessors.end()) {
							res2dtl->researchLineSuccessors.push_back(res1dtl);
						}
						if (std::find(res1dtl->researchLinePredecessors.cbegin(), res1dtl->researchLinePredecessors.cend(), res2dtl) == res1dtl->researchLinePredecessors.end()) {
							res1dtl->researchLinePredecessors.push_back(res2dtl);
						}
					}
				}
			}
		}
	}
	// Sort research lines, now the lists are fully fed
	for (int resId = 0; resId < this->researchCount; resId++) {
		TTDetailedResearchDef *resDtl = this->GetDetailedResearchDef(resId);
		if (!resDtl || !resDtl->active || resDtl->IsShadowResearch()) { continue; }
		resDtl->researchLineSuccessors.sort([](TTDetailedResearchDef *left, TTDetailedResearchDef *right) {
			return *left < *right;
		});
		resDtl->researchLinePredecessors.sort([](TTDetailedResearchDef *left, TTDetailedResearchDef *right) {
			return *left < *right;
		});
	}
}


// Update baseUnitDefId for all known buildings/trainable units to identify root units
// Also update all dependencies between units from a same treeline (eg. swordsman-broad-long-legion)
void TechTreeAnalyzer::UpdateUnitsBaseId() {
	for (int bldDefId = 0; bldDefId < this->unitDefCount; bldDefId++) {
		TTDetailedUnitDef *detail = this->GetDetailedBuildingDef(bldDefId);
		bool isBuilding = (detail != NULL);
		if (!isBuilding) {
			detail = this->GetDetailedTrainableUnitDef(bldDefId);
		}

		if (detail && detail->IsValid()) {
			// To avoid recursive stuff, we consider that root ancestor is THE possible ancestor that has NO ancestor itself (there should be only one)
			// This may fail if some mod uses separate ways to reach some unit ! (but in that case, anyway, detailBld->baseUnitId would have no real meaning).
			for each (auto ancestorId in detail->possibleAncestorUnitIDs)
			{
				TTDetailedUnitDef *ancestor;
				if (isBuilding) { // Bug in C++ syntax (!): can't use a ? b : c syntax here
					ancestor = this->GetDetailedBuildingDef(ancestorId);
				} else {
					ancestor = this->GetDetailedTrainableUnitDef(ancestorId);
				}

				//TTDetailedBuildingDef *ancestor = this->detailedBuildings[ancestorId];
				if (ancestor && ancestor->IsValid() && (ancestor->possibleAncestorUnitIDs.size() == 0)) {
					detail->baseUnitId = ancestorId;
				}
			}
		}
	}


	// Update links for the whole tree line (only base unit has all correct links at this point, because techs always links baseUnit to upgrades)
	for (int unitDefId = 0; unitDefId < this->unitDefCount; unitDefId++) {
		TTDetailedUnitDef *detail = this->GetDetailedTrainableUnitDef(unitDefId);
		if (!detail) {
			detail = this->GetDetailedBuildingDef(unitDefId);
		}
		if (!detail || !detail->IsValid()) { continue; }
		long int myUnitDefId = detail->unitDefId;
		long int myBaseId = detail->baseUnitId;
		if (myBaseId != myUnitDefId) { continue; } // We're only interested in base units here
		if (detail->possibleUpgradedUnitIDs.size() == 0) { continue; } // No treeline here

		// Build a list of all children (no need for base unit, all links are already correctly handled)
		std::set<TTDetailedUnitDef*> allTreeLine;
		for each (long int treeLineUnitId in detail->possibleUpgradedUnitIDs) {
			TTDetailedUnitDef *treeLineUnitDtl = this->GetDetailedTrainableUnitDef(treeLineUnitId);
			if (!treeLineUnitDtl) {
				treeLineUnitDtl = this->GetDetailedBuildingDef(treeLineUnitId);
			}
			if (treeLineUnitDtl && treeLineUnitDtl->IsValid()) {
				allTreeLine.insert(treeLineUnitDtl);
			}
		}

		for each (long int treeLineUnitId1 in detail->possibleUpgradedUnitIDs) {
			TTDetailedUnitDef *treeLineUnitDtl1 = this->GetDetailedTrainableUnitDef(treeLineUnitId1);
			if (!treeLineUnitDtl1) {
				treeLineUnitDtl1 = this->GetDetailedBuildingDef(treeLineUnitId1);
			}
			if (!treeLineUnitDtl1 || !treeLineUnitDtl1->IsValid()) { continue; } // Should not happen

			long int unit1srcResId = -1; // research that enables "me"
			for each (long int loopId in treeLineUnitDtl1->researchIdsThatEnableMe)
			{
				unit1srcResId = loopId; // there should be 1 loop max in theory
			}
			TTDetailedResearchDef *unit1srcRes = this->GetDetailedResearchDef(unit1srcResId);

			for each (TTDetailedUnitDef* treeLineUnitDtl2 in allTreeLine)
			{
				if (treeLineUnitDtl2->unitDefId == treeLineUnitId1) { continue; } // same as me: skip
				if (treeLineUnitDtl2->unitDefId < treeLineUnitId1) { continue; } // so we run this only once for a combination instead of {a,b} and {b,a}
				// Determine if other is my child or my parent
				long int unit2srcResId = -1;
				for each (long int otherSrcResIdLoop in treeLineUnitDtl2->researchIdsThatEnableMe)
				{
					unit2srcResId = otherSrcResIdLoop; // there should be 1 loop max in theory
				}
				TTDetailedResearchDef *unit2srcRes = this->GetDetailedResearchDef(unit2srcResId);
				if (unit1srcRes && unit2srcRes && unit1srcRes->active && unit2srcRes->active) {
					auto it = std::find(unit1srcRes->allRequirementsExcludingAges.begin(),
						unit1srcRes->allRequirementsExcludingAges.end(),
						unit2srcRes);
					if (it != unit1srcRes->allRequirementsExcludingAges.end()) {
						// found: u2 is a requirement (parent) of u1
						if (std::find(treeLineUnitDtl1->possibleAncestorUnitIDs.cbegin(), treeLineUnitDtl1->possibleAncestorUnitIDs.cend(), treeLineUnitDtl2->unitDefId) == treeLineUnitDtl1->possibleAncestorUnitIDs.cend()) {
							treeLineUnitDtl1->possibleAncestorUnitIDs.push_back(treeLineUnitDtl2->unitDefId);
						}
						if (std::find(treeLineUnitDtl2->possibleUpgradedUnitIDs.cbegin(), treeLineUnitDtl2->possibleUpgradedUnitIDs.cend(), treeLineUnitId1) == treeLineUnitDtl2->possibleUpgradedUnitIDs.cend()) {
							treeLineUnitDtl2->possibleUpgradedUnitIDs.push_back(treeLineUnitId1);
						}
					} else {
						// Make sure u1 is a requirement (parent) of u2
						auto it2 = std::find(unit2srcRes->allRequirementsExcludingAges.begin(),
							unit2srcRes->allRequirementsExcludingAges.end(),
							unit1srcRes);
						if (it2 != unit2srcRes->allRequirementsExcludingAges.end()) {
							// Found: u2 is a requirement (parent) of u1
							if (std::find(treeLineUnitDtl1->possibleUpgradedUnitIDs.cbegin(), treeLineUnitDtl1->possibleUpgradedUnitIDs.cend(), treeLineUnitDtl2->unitDefId) == treeLineUnitDtl1->possibleUpgradedUnitIDs.cend()) {
								treeLineUnitDtl1->possibleUpgradedUnitIDs.push_back(treeLineUnitDtl2->unitDefId);
							}
							if (std::find(treeLineUnitDtl2->possibleAncestorUnitIDs.cbegin(), treeLineUnitDtl2->possibleAncestorUnitIDs.cend(), treeLineUnitId1) == treeLineUnitDtl2->possibleAncestorUnitIDs.cend()) {
								treeLineUnitDtl2->possibleAncestorUnitIDs.push_back(treeLineUnitId1);
							}

						} else {
							assert(false && "units should not be unrelated here");
						}
					}
				}
			}
		}
	}

	// Now sort possibleUpgradedUnitIDs and possibleAncestorUnitIDs for unit detailed info
	// Note: we just fixed baseUnitId values so the requirements for "<" operator are met.
	for (int unitDefId = 0; unitDefId < this->unitDefCount; unitDefId++) {
		TTDetailedUnitDef *unitDetail = this->GetDetailedUnitDef(unitDefId);
		if (!unitDetail || !unitDetail->IsValid()) { continue; }
		unitDetail->possibleAncestorUnitIDs.sort([&](long int left, long int right) {
			auto l = this->GetDetailedUnitDef(left);
			auto r = this->GetDetailedUnitDef(right);
			if (!l) { return true; }
			if (!r) { return false; }
			return *l < *r;
		});
		unitDetail->possibleUpgradedUnitIDs.sort([&](long int left, long int right) {
			auto l = this->GetDetailedUnitDef(left);
			auto r = this->GetDetailedUnitDef(right);
			if (!l) { return true; }
			if (!r) { return false; }
			return *l < *r;
		});
	}
}


// Find out which trainable units are "super units" (cataphracts, armored elephants, massive catapults, etc)
// Priest canNOT be super units here.
void TechTreeAnalyzer::DetectSuperUnits() {
	for (int bldDefId = 0; bldDefId < this->unitDefCount; bldDefId++) {
		TTDetailedTrainableUnitDef *detail = this->GetDetailedTrainableUnitDef(bldDefId);
		if (!detail || !detail->IsValid()) { continue; }
		if (detail->possibleUpgradedUnitIDs.size() > 0) { continue; } // Super units don't have more upgrades
		if (detail->IsHeroOrScenarioUnit()) { continue; }
		if (detail->requiredAge < AOE_CONST_FUNC::CST_RSID_IRON_AGE) { continue; } // Super units are iron age units (excludes priest)
		for each (long int enableResId in detail->researchIdsThatEnableMe) {
			// There should be only one, actually
			TTDetailedResearchDef *enableResDetail = this->GetDetailedResearchDef(enableResId);
			if (enableResDetail && enableResDetail->active) {
				long int totalCost = 0;
				if (enableResDetail->researchDef->costUsed1 != 0) {
					totalCost += enableResDetail->researchDef->costAmount1;
				}
				if (enableResDetail->researchDef->costUsed2 != 0) {
					totalCost += enableResDetail->researchDef->costAmount2;
				}
				if (enableResDetail->researchDef->costUsed3 != 0) {
					totalCost += enableResDetail->researchDef->costAmount3;
				}
				if (totalCost >= STRATEGY_CONST::MIN_COST_FOR_SUPER_UNIT_UPGRADE) {
					detail->isSuperUnit = true;
				}
			}
		}
	}
}


void TechTreeAnalyzer::UpdateUnitClassesData() {
	for (int unitClassIndex = 0; unitClassIndex < this->unitClassCount; unitClassIndex++) {
		GLOBAL_UNIT_AI_TYPES curUnitClassId = (GLOBAL_UNIT_AI_TYPES)unitClassIndex;
		TTDetailedUnitClass *unitClassDetail = this->GetDetailedUnitClass(unitClassIndex);
		if (!unitClassDetail) { continue; }
		unitClassDetail->Init(curUnitClassId);
		// Buildings
		for (int unitDefId = 0; unitDefId < this->unitDefCount; unitDefId++) {
			TTDetailedBuildingDef *bldDetail = this->GetDetailedBuildingDef(unitDefId);
			if (!bldDetail || !bldDetail->IsValid()) { continue; }
			if (bldDetail->unitClass == curUnitClassId) {
				unitClassDetail->allClassUnits.insert(bldDetail);
				assert((curUnitClassId == TribeAIGroupBuilding) || (curUnitClassId == TribeAIGroupWall));
			}
		}
		// Trainables
		for (int unitDefId = 0; unitDefId < this->unitDefCount; unitDefId++) {
			TTDetailedTrainableUnitDef *unitDetail = this->GetDetailedTrainableUnitDef(unitDefId);
			if (!unitDetail || !unitDetail->IsValid()) { continue; }
			if (unitDetail->unitClass == curUnitClassId) {
				unitClassDetail->allClassUnits.insert(unitDetail);
				assert((curUnitClassId != TribeAIGroupBuilding) && (curUnitClassId != TribeAIGroupWall));
			}
			if (unitDetail->isSuperUnit) {
				unitClassDetail->includesSuperUnit = true;
			}
		}
		// Common (uses previously collected data)
		for each (TTDetailedUnitDef *unitDetail in unitClassDetail->allClassUnits)
		{
			if (!unitDetail->IsHeroOrScenarioUnit()) {
				// Save train location... if unique among class units (ignoring hero units because train location is both irrelevant and unreliable)
				bool nonUniqueTrainLocation = false; 
				long int curTrainLocation = unitDetail->GetTrainLocation();
				if ((curTrainLocation >= 0) && !unitDetail->IsHeroOrScenarioUnit()) {
					nonUniqueTrainLocation |= ((unitClassDetail->trainLocationIfUnique >= 0) && (unitClassDetail->trainLocationIfUnique != curTrainLocation));
					if (!nonUniqueTrainLocation) {
						unitClassDetail->trainLocationIfUnique = curTrainLocation;
					}
				}
				// Non-hero units: save in a specific collection
				unitClassDetail->allClassUnitsNoHero.insert(unitDetail);
				if (unitDetail->baseUnitId == unitDetail->unitDefId) {
					unitClassDetail->rootUnits.insert(unitDetail);
				}
			}
			if (unitDetail->requiredAge <= CST_RSID_STONE_AGE) { unitClassDetail->stoneAgeUnitCount++; }
			if (unitDetail->requiredAge == CST_RSID_TOOL_AGE) { unitClassDetail->toolAgeUnitCount++; }
			if (unitDetail->requiredAge == CST_RSID_BRONZE_AGE) { unitClassDetail->bronzeAgeUnitCount++; }
			if (unitDetail->requiredAge == CST_RSID_IRON_AGE) { unitClassDetail->ironAgeUnitCount++; }
		}
	}
}




// Update links between researches and units that are affected by them
void TechTreeAnalyzer::UpdateResearchAffectingUnitsLinks() {
	AOE_TECHNOLOGIES::TechnologyFilterBase filter; // TechnologyFilterBase does not filter out any tech
	for (int resId = 0; resId < this->researchCount; resId++) {
		TTDetailedResearchDef *resInfo = this->GetDetailedResearchDef(resId);
		if (!resInfo || !resInfo->active || !resInfo->techDef) { continue; }
		for (int unitDefId = 0; unitDefId < this->unitDefCount; unitDefId++) {
			TTDetailedUnitDef *unitDetail = this->GetDetailedTrainableUnitDef(unitDefId);
			if (!unitDetail) {
				unitDetail = this->GetDetailedBuildingDef(unitDefId);
			}
			if (unitDetail && unitDetail->IsValid()) {
				if (DoesTechAffectUnit(resInfo->techDef, unitDetail->GetUnitDef(), &filter)) {
					unitDetail->affectedByResearches.insert(resInfo);
					resInfo->affectsUnits.insert(unitDetail);
				}
			}
		}
	}
}


// Calculate additional statistics, one tech tree has been analysed
void TechTreeAnalyzer::CalculateStatistics() {
	this->statistics.Reset();

	for (int researchId = 0; researchId < this->researchCount; researchId++) {
		TTDetailedResearchDef *detail = this->GetDetailedResearchDef(researchId);
		if (!detail || !detail->active) { continue; }
		if (!detail->allRequirementsAreKnown) {
			this->statistics.inaccessibleResearchesCount++;
		}
		if (!detail->IsShadowResearch() && detail->allRequirementsAreKnown) {
			int ageIndex = detail->requiredAge - AOE_CONST_FUNC::CST_RSID_STONE_AGE; // TODO: WRONG for buildings
			if (ageIndex < 0) { ageIndex = 0; }
			if (ageIndex >= AGES_COUNT) { ageIndex = AGES_COUNT - 1; }
			this->statistics.nonShadowResearchesCount++;
			this->statistics.nonShadowResearchesCountByAge[ageIndex]++;
			if (detail->researchLinePredecessors.size() == 0) {
				// No predecessors = we have a "root" research (regarding our research lineage concept)
				this->statistics.rootNonShadowResearchesCount++;
				this->statistics.rootNonShadowResearchesCountByAge[ageIndex]++;
				if (detail->researchLineSuccessors.size() == 0) {
					this->statistics.nonShadowResearchesWithoutLineageCount++; // this research is lonely (no lineage). Eg wheel, heavy transport...
					this->statistics.nonShadowResearchesWithoutLineageCountByAge[ageIndex]++;
				}
			}
		}
	}

	for (int unitDefId = 0; unitDefId < this->unitDefCount; unitDefId++) {
		bool isBuilding = false;
		TTDetailedUnitDef *unitDetail = this->GetDetailedTrainableUnitDef(unitDefId);
		if (!unitDetail) {
			unitDetail = this->GetDetailedBuildingDef(unitDefId);
			if (unitDetail) { isBuilding = true; }
		}
		if (unitDetail && unitDetail->IsValid()) {
			int ageIndex = unitDetail->requiredAge - AOE_CONST_FUNC::CST_RSID_STONE_AGE; // TODO: WRONG for buildings
			if (ageIndex < 0) { ageIndex = 0; }
			if (ageIndex >= AGES_COUNT) { ageIndex = AGES_COUNT - 1; }
			if (unitDetail->IsHeroOrScenarioUnit()) {
				this->statistics.heroesCount++;
			} else {
				// Non-heroes
				if (unitDetail->baseUnitId == unitDefId) { // root unit
					if (isBuilding) {
						this->statistics.rootBuildingUnitsCount++;
						this->statistics.rootBuildingsCountByAge[ageIndex]++;
					} else {
						this->statistics.rootTrainableUnitsCount++;
						this->statistics.rootTrainablesCountByAge[ageIndex]++;
					}
				}
				if (isBuilding) {
					this->statistics.allBuildingsCountByAge[ageIndex]++;
					this->statistics.allBuildingUnitsCount++;
				} else {
					this->statistics.allTrainablesCountByAge[ageIndex]++;
					this->statistics.allTrainableUnitsCount++;
				}
				if (unitDetail->unitClass == TribeAIGroupCivilian) {
					this->statistics.villagerCount++;
				}
			}
		}
	}
	this->myLog.append("*** Researches ***" NEWLINE);
	this->myLog.append("- Inaccessible researches count = ");
	this->myLog.append(std::to_string(this->statistics.inaccessibleResearchesCount));
	this->myLog.append(NEWLINE "- Researches : number of valid non-shadow researches = ");
	this->myLog.append(std::to_string(this->statistics.nonShadowResearchesCount));
	this->myLog.append(NEWLINE "- ... By age : ");
	for (int i = 0; i < AGES_COUNT; i++) {
		this->myLog.append(std::to_string(this->statistics.nonShadowResearchesCountByAge[i]));
		if (i < AGES_COUNT - 1) {
			this->myLog.append(", ");
		}
	}
	this->myLog.append(NEWLINE "- Researches : number of research lineages (including lonely researches) = ");
	this->myLog.append(std::to_string(this->statistics.rootNonShadowResearchesCount));
	this->myLog.append(NEWLINE "- ... By age : ");
	for (int i = 0; i < AGES_COUNT; i++) {
		this->myLog.append(std::to_string(this->statistics.rootNonShadowResearchesCountByAge[i]));
		if (i < AGES_COUNT - 1) {
			this->myLog.append(", ");
		}
	}
	this->myLog.append(NEWLINE "- Researches : number of researches without predecessor/successor = ");
	this->myLog.append(std::to_string(this->statistics.nonShadowResearchesWithoutLineageCount));
	this->myLog.append(NEWLINE "- ... By age : ");
	for (int i = 0; i < AGES_COUNT; i++) {
		this->myLog.append(std::to_string(this->statistics.nonShadowResearchesWithoutLineageCountByAge[i]));
		if (i < AGES_COUNT - 1) {
			this->myLog.append(", ");
		}
	}

	this->myLog.append(NEWLINE "*** Units ***");
	this->myLog.append(NEWLINE "- Total number of hero units = ");
	this->myLog.append(std::to_string(this->statistics.heroesCount));
	this->myLog.append(" (heroes are excluded in next figures)" NEWLINE "- Total number of villager units = ");
	this->myLog.append(std::to_string(this->statistics.villagerCount));
	this->myLog.append(NEWLINE "- Total number of trainable units = ");
	this->myLog.append(std::to_string(this->statistics.allTrainableUnitsCount));
	this->myLog.append(NEWLINE "- ... By age : ");
	for (int i = 0; i < AGES_COUNT; i++) {
		this->myLog.append(std::to_string(this->statistics.allTrainablesCountByAge[i]));
		if (i < AGES_COUNT - 1) {
			this->myLog.append(", ");
		}
	}
	this->myLog.append(NEWLINE "- Number of unit lineages = ");
	this->myLog.append(std::to_string(this->statistics.rootTrainableUnitsCount));
	this->myLog.append(NEWLINE "- ... By age : ");
	for (int i = 0; i < AGES_COUNT; i++) {
		this->myLog.append(std::to_string(this->statistics.rootTrainablesCountByAge[i]));
		if (i < AGES_COUNT - 1) {
			this->myLog.append(", ");
		}
	}
	this->myLog.append(NEWLINE "- Total number of building units = ");
	this->myLog.append(std::to_string(this->statistics.allBuildingUnitsCount));
	this->myLog.append(NEWLINE "- ... By age : ");
	for (int i = 0; i < AGES_COUNT; i++) {
		this->myLog.append(std::to_string(this->statistics.allBuildingsCountByAge[i]));
		if (i < AGES_COUNT - 1) {
			this->myLog.append(", ");
		}
	}
	this->myLog.append(NEWLINE "- Number of building lineages = ");
	this->myLog.append(std::to_string(this->statistics.rootBuildingUnitsCount));
	this->myLog.append(NEWLINE "- ... By age : ");
	for (int i = 0; i < AGES_COUNT; i++) {
		this->myLog.append(std::to_string(this->statistics.rootBuildingsCountByAge[i]));
		if (i < AGES_COUNT - 1) {
			this->myLog.append(", ");
		}
	}
	this->myLog.append(NEWLINE "End of statistics" NEWLINE);
}


// Analyze tech tree and fill internal data
bool TechTreeAnalyzer::AnalyzeTechTree() {
	if (this->analyzeComplete) { return true; }
	this->myLog.append("Analyze technology tree started" NEWLINE);
	if (!this->ReadRawDataFromDat()) {
		this->myLog.append("Stop: could not get empires.dat data" NEWLINE);
		return false;
	}
	// If we reach this we are sure that global struct is initialized, and that DAT info has been loaded already.

	// Create trainable & buildings detailed info objects
	this->FindBuildingThatTriggerResearches();
	this->FindResearchesThatEnableUnits();

	// In game, TC is always aither built or available in build menu (even... if unitDef is not available)
	// We need to do the equivalent here, or tech tree can't completely expand (can't reach tool age, because research location is unavailable).
	this->MarkBuildingAsAvailable(CST_UNITID_FORUM);

	this->EvaluateRequiredResearches();

	// Fill "unreachableResearches"
	this->CollectUnreachableResearches();

	// Update the "child" dependencies so each research know its children
	this->UpdateChildResearchDependencies();

	// Collect additional info
	this->UpdateUnitsBaseId();
	this->DetectSuperUnits();
	this->CollectTrainLocations(); // Requires dependencies/analysis to be complete because it uses IsHeroOrScenarioUnit() method.

	// Update unit-research links (researches *affecting* units)
	this->UpdateResearchAffectingUnitsLinks();

	// Debugging
#ifdef _DEBUG
	for (int researchId = 0; researchId < this->researchCount; researchId++) {
		TTDetailedResearchDef *detail = this->GetDetailedResearchDef(researchId);
		if (!detail || !detail->active) { continue; }
		if (!detail->allRequirementsAreKnown) { continue; }
		this->myLog.append("Research #");
		this->myLog.append(std::to_string(researchId));
		this->myLog.append(" / ");
		this->myLog.append(detail->langName);
		this->myLog.append(" {age ");
		this->myLog.append(std::to_string(detail->requiredAge));
		this->myLog.append("} dep=> [");
		for each (auto req in detail->directRequirements)
		{
			this->myLog.append(" ");
			this->myLog.append(std::to_string(req->GetResearchDefId()));
		}
		this->myLog.append("]");
		for each (auto req in detail->allRequirementsExcludingAges)
		{
			this->myLog.append(" ");
			this->myLog.append(std::to_string(req->GetResearchDefId()));
		}
		this->myLog.append(NEWLINE);
	}
#endif

	this->UpdateUnitClassesData();
	this->CalculateStatistics();
	this->analyzeComplete = true;
	return true;
}


// Please, please call this if TechDef pointers have changed in global->technologyInfos (when array has been re-allocated)
void TechTreeAnalyzer::RefreshTechDefPointers() {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	bool canUseNewPtr = (global && global->IsCheckSumValid() && global->technologiesInfo);
	for (int resDefId = 0; resDefId < this->researchCount; resDefId++) {
		TTDetailedResearchDef *detail = this->GetDetailedResearchDef(resDefId);
		if (detail && detail->techDef && detail->researchDef) {
			detail->techDef = NULL;
			if (canUseNewPtr) {
				detail->techDef = global->technologiesInfo->GetTechDef(detail->researchDef->technologyId);
			}
		}
	}
}


// Returns true if provided technologyId corresponds to a technology tree (excludes "dynamic" tech trees !)
bool TechTreeAnalyzer::IsTechTree(short int technologyId) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!this->IsReady() || !global) { return false; }
	for each (short int i in this->techTreeTechIDs)
	{
		if (i == technologyId) { return true; }
	}
	return false;
}



// Returns true if a research affects some unit
bool TechTreeAnalyzer::DoesResearchAffectUnit(short int researchId, short int unitDefId) {
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	TTDetailedUnitDef *unitDetails = this->GetDetailedUnitDef(unitDefId);
	TTDetailedResearchDef *resDetails = this->GetDetailedResearchDef(researchId);
	if (!unitDetails || !unitDetails->IsValid() || !resDetails || !resDetails->active) { return false; }
	auto it = std::find(unitDetails->affectedByResearches.begin(), unitDetails->affectedByResearches.end(), resDetails);
	if (it != unitDetails->affectedByResearches.end()) {
		TTDetailedResearchDef *r = *it;
		if ((!settings->isDeathMatch && !r->CanExcludeInRandomMapAI()) ||
			(settings->isDeathMatch && !r->CanExcludeInDeathMatchAI())) {
			return true;
		}
	}
	return false;
}


// Returns a vector of all non-disabled researches that affect the provided unit definition (ID)
// useFilter: if true, exclude researches with drawbacks for current game type (DM/RM)
std::vector<short int> TechTreeAnalyzer::GetAllResearchesThatAffectUnit(STRUCT_PLAYER *player, short int unitDefId, bool useFilter) {
	std::vector<short int> result;
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!player || !player->IsCheckSumValid() || !settings || !settings->IsCheckSumValid()) { return result; }
	std::vector<short int> allResearchesForUnit;
	TTDetailedUnitDef *details = this->GetDetailedUnitDef(unitDefId);
	if (!details || !details->IsValid()) { return allResearchesForUnit; }
	STRUCT_PLAYER_RESEARCH_INFO *rinfo = player->ptrResearchesStruct;
	if (!rinfo) { return allResearchesForUnit; }
	STRUCT_PLAYER_RESEARCH_STATUS *statuses = rinfo->researchStatusesArray;
	if (!statuses) { return allResearchesForUnit; }
	for each (TTDetailedResearchDef *resDetail in details->affectedByResearches) {
		if (!resDetail->active || resDetail->IsShadowResearch()) { continue; }
		// Ignore techs that are no use to AI & techs with drawbacks like jihad/ballista tower
		if (resDetail->CanExcludeInRandomMapAI()) { continue; }
		short int resDefId = (short int)resDetail->GetResearchDefId();
		if (statuses[resDefId].currentStatus != RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED) {
			allResearchesForUnit.push_back(resDefId);
		}
	}
	return allResearchesForUnit;
}


}
}
