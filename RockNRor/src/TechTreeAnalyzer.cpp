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
	if (!global || !global->IsCheckSumValid()) { return false; }
	assert(global->researchDefInfo != NULL);
	if (!global->researchDefInfo) { return false; }
	if (global->civCount < 2) { return false; }
	STRUCT_CIVILIZATION_DEF *civDef1 = global->civilizationDefinitions[1];

	this->researchCount = global->researchDefInfo->researchCount;
	this->unitDefCount = civDef1->civUnitDefCount;

	if ((this->researchCount <= 0) || (this->unitDefCount <= 0)) { return false; }
	this->AllocArrays();
	for (int resId = 0; resId < this->researchCount; resId++) {
		STRUCT_RESEARCH_DEF *resDef = global->researchDefInfo->GetResearchDef(resId);
		TTDetailedResearchDef *detail = this->GetDetailedResearchDef(resId);
		assert(detail != NULL);
		if (resDef) {
			detail->internalName = std::string((resDef->researchName != NULL) ? resDef->researchName : "");
			detail->researchDef = resDef;
			detail->techDef = global->GetTechDef(resDef->technologyId);
			detail->allRequirementsAreKnown = false;
			detail->active = true;
			if (detail->techDef != NULL) { // some shadow researches have effect=-1 (they are useful for dependencies)
				bool hasValidEffect = false;
				for (int effectIndex = 0; (effectIndex < detail->techDef->effectCount) && !hasValidEffect; effectIndex++) {
					hasValidEffect = detail->techDef->ptrEffects[effectIndex].HasValidEffect();
				}
				detail->hasValidEffect = hasValidEffect;
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
// Also fills possibleUpgradedUnitIDs / possibleParentUnitIDs
void TechTreeAnalyzer::FindResearchesThatEnableKnownBuildings() {
	for (int resDefId = 0; resDefId < this->researchCount; resDefId++) {
		TTDetailedResearchDef *detailRes = this->detailedResearches[resDefId];
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
					detailBuilding->possibleAncestorUnitIDs.insert(parentUnitId);

					TTDetailedBuildingDef *detailBuildingParent = this->GetDetailedBuildingDef(parentUnitId);
					if ((detailBuildingParent != NULL) && (detailBuildingParent->unitDef != NULL)) {
						detailBuildingParent->possibleUpgradedUnitIDs.insert(upgradedUnitId);
					}
				}
				TTDetailedTrainableUnitDef *detailTrainable = this->GetDetailedTrainableUnitDef(upgradedUnitId);
				if ((detailTrainable != NULL) && (detailTrainable->unitDef != NULL)) {
					detailTrainable->researchIdsThatEnableMe.insert(resDefId);
					detailRes->enableTrainables.insert(detailTrainable);
					detailTrainable->possibleAncestorUnitIDs.insert(parentUnitId);
					detailRes->upgradedUnitDefId.insert(std::pair<long int, long int>(parentUnitId, upgradedUnitId));

					TTDetailedTrainableUnitDef *detailTrainableParent = this->GetDetailedTrainableUnitDef(parentUnitId);
					if ((detailTrainableParent != NULL) && (detailTrainableParent->unitDef != NULL)) {
						detailTrainableParent->possibleUpgradedUnitIDs.insert(upgradedUnitId);
					}
				}
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
			this->myLog.append(initiateResearchDetail->internalName);
			this->myLog.append(") is triggered by building '");
			this->myLog.append(detailBld->internalName);
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
			this->myLog.append(initiateResearchDetail->internalName);
			this->myLog.append(" : all requirements are known (from building #");
			this->myLog.append(std::to_string(detailBld->unitDefId));
			this->myLog.append(")" NEWLINE);
		}
	}
	if (detailBld->initiatesResearch == -1) {
		this->myLog.append("Building #");
		this->myLog.append(std::to_string(detailBld->unitDefId));
		this->myLog.append(" / ");
		this->myLog.append(detailBld->internalName);
		this->myLog.append(" is now available." NEWLINE);
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
	this->myLog.append(detailTrainable->internalName);
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
			this->myLog.append(detail->internalName);
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
		this->myLog.append(detail->internalName);
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
void TechTreeAnalyzer::UpdateChildResearchDependencies() {
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
}


// Update baseUnitDefId for all known buildings/trainable units to identify root units
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


// Analyze tech tree and fill internal data
bool TechTreeAnalyzer::AnalyzeTechTree() {
	if (this->analyzeComplete) { return true; }
	this->myLog.append("Analyze technology tree started" NEWLINE);
	if (!this->ReadRawDataFromDat()) {
		this->myLog.append("Stop: could not get empires.dat data" NEWLINE);
		return false;
	}
	// If we reach this we are sure that global struct is initialized, and that DAT info has been loaded already.
	this->FindBuildingThatTriggerResearches();
	this->FindResearchesThatEnableKnownBuildings();

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
	
	// Debugging
	for (int researchId = 0; researchId < this->researchCount; researchId++) {
		TTDetailedResearchDef *detail = this->GetDetailedResearchDef(researchId);
		if (!detail || !detail->active) { continue; }
		if (!detail->allRequirementsAreKnown) { continue; }
		this->myLog.append("Research #");
		this->myLog.append(std::to_string(researchId));
		this->myLog.append(" / ");
		this->myLog.append(detail->internalName);
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


}
}
