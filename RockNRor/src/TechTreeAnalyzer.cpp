#include "../include/TechTreeAnalyzer.h"
#pragma once

// \r is required to display correctly in ROR editable text components
#define NEWLINE "\r\n"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace STRATEGY {
;


// Copy all direct/indirect requirements from "other" to this->allRequirementsExcludingAges
void DetailedResearchDef::AddAllRequirementsFrom(DetailedResearchDef *other) {
	if (!other) { return; }
	for each (DetailedResearchDef *requirement in other->allRequirementsExcludingAges)
	{
		this->allRequirementsExcludingAges.insert(requirement);
	}
	for each (DetailedResearchDef *requirement in other->directRequirements)
	{
		this->allRequirementsExcludingAges.insert(requirement);
	}
}


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
		DetailedResearchDef *detail = this->GetDetailedResearchDef(resId);
		assert(detail != NULL);
		if (resDef) {
			detail->internalName = string((resDef->researchName != NULL) ? resDef->researchName : "");
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
void TechTreeAnalyzer::FindBuildingThatTriggerResearches() {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return; }
	if (global->civCount < 2) { return; }
	STRUCT_CIVILIZATION_DEF *civDef1 = global->civilizationDefinitions[1];
	
	for (int unitDefId = 0; unitDefId < civDef1->civUnitDefCount; unitDefId++) {
		STRUCT_UNITDEF_BUILDING *unitDef = (STRUCT_UNITDEF_BUILDING*)civDef1->GetUnitDef(unitDefId);
		if (unitDef && (unitDef->unitType == GUT_BUILDING) && unitDef->IsCheckSumValid()) {
			DetailedResearchDef *detail = this->GetDetailedResearchDef(unitDef->initiatesResearch);
			if (detail) {
				this->InitGetDetailedBuildingDef(unitDef); // required to create internal data about this building
				detail->triggeredByBuilding.insert(this->GetDetailedBuildingDef(unitDefId));
			}
		}
	}
}


// Update this->detailedBuildings with researches that enable the buildings (if not available at start)
// Also fills possibleUpgradedUnitIDs / possibleParentUnitIDs
void TechTreeAnalyzer::FindResearchesThatEnableKnownBuildings() {
	for (int resDefId = 0; resDefId < this->researchCount; resDefId++) {
		DetailedResearchDef *detailRes = this->detailedResearches[resDefId];
		STRUCT_TECH_DEF *techDef = detailRes->techDef;
		if (!techDef) { 
			continue;
		}
		for (int effectIndex = 0; effectIndex < techDef->effectCount; effectIndex++) {
			STRUCT_TECH_DEF_EFFECT *techEffect = &techDef->ptrEffects[effectIndex];
			// Case 1 : Enable unit
			if ((techEffect->effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (techEffect->effectClass == 1)/*enable*/) {
				long int enabledUnitId = techEffect->effectUnit;
				DetailedBuildingDef *detailBuilding = this->GetDetailedBuildingDef(enabledUnitId);
				if ((detailBuilding != NULL) && (detailBuilding->unitDef != NULL)) {
					detailBuilding->researchIdsThatEnableMe.insert(resDefId);
				}
			}
			
			// Case 2 : Upgrade some unit TO THIS unit
			if (techEffect->effectType == TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT) {
				long int upgradedUnitId = techEffect->effectClass;
				long int parentUnitId = techEffect->effectUnit;
				DetailedBuildingDef *detailBuilding = this->GetDetailedBuildingDef(upgradedUnitId);
				if ((detailBuilding != NULL) && (detailBuilding->unitDef != NULL)) {
					detailBuilding->researchIdsThatEnableMe.insert(resDefId);
					detailBuilding->possibleAncestorUnitIDs.insert(parentUnitId);
				}

				DetailedBuildingDef *detailBuildingParent = this->GetDetailedBuildingDef(parentUnitId);
				if ((detailBuildingParent != NULL) && (detailBuildingParent->unitDef != NULL)) {
					detailBuildingParent->possibleUpgradedUnitIDs.insert(upgradedUnitId);
				}
			}
		}
	}
}


void TechTreeAnalyzer::EvaluateRequiredAge() {
	for (int i = 0; i < this->researchCount; i++) {

	}
}


// Update required researches for researches whose direct dependencies are all satisfied (all required researches are known)
// Returns the number of researches that are now "satisfied"
int TechTreeAnalyzer::EvaluateRequiredResearchesSimple() {
	int updatedResearches = 0;
	this->myLog.append("New evaluation of research triggered by available buildings." NEWLINE);
	for (int bldDefId = 0; bldDefId < this->unitDefCount; bldDefId++) {
		DetailedBuildingDef *detailBld = this->detailedBuildings[bldDefId];
		if (detailBld && detailBld->unitDef) {
			bool buildingIsReady = detailBld->isAvailableImmediately;
			if (!buildingIsReady) {
				auto it = std::find_if(detailBld->researchIdsThatEnableMe.begin(), detailBld->researchIdsThatEnableMe.end(),
					[this](long int lambdaDefId) {
					DetailedResearchDef *detailResDef = this->GetDetailedResearchDef(lambdaDefId);
					return (detailResDef != NULL) && detailResDef->allRequirementsAreKnown;
				});
				if (it != detailBld->researchIdsThatEnableMe.end()) {
					// Found at least 1 "ready" research that enables the building
					buildingIsReady = true;
				}
			}

			if (buildingIsReady && !detailBld->isAvailableInCurrentState) {
				detailBld->isAvailableInCurrentState = true;
				// Building just became ready : simulate the "initiate research" effect (if there is an "initiate research")
				DetailedResearchDef *initiateResearchDetail = this->GetDetailedResearchDef(detailBld->initiatesResearch); // may return NULL as ID may be -1
				if (initiateResearchDetail && initiateResearchDetail->active) {
					if (initiateResearchDetail->directRequirements.size() > 0) {
						this->myLog.append("Warning: research #");
						this->myLog.append(std::to_string(initiateResearchDetail->GetResearchDefId()));
						this->myLog.append(" is triggered by building but has requirements" NEWLINE);
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
							DetailedResearchDef *parentResDetail = this->GetDetailedResearchDef(parentResDefId);
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
			}
		}
	}

	this->myLog.append("New evaluation of trivial research dependencies." NEWLINE);
	for (int researchId = 0; researchId < this->researchCount; researchId++) {
		DetailedResearchDef *detail = this->GetDetailedResearchDef(researchId);
		if (!detail || !detail->active) { continue; }
		if (detail->allRequirementsAreKnown) { continue; }
		bool allReqSatisfied = true;
		for each (DetailedResearchDef *reqDetail in detail->directRequirements)
		{
			allReqSatisfied &= reqDetail->allRequirementsAreKnown;
		}
		// Note: research is automatically triggered if "allSatisfied" && (researchTime == 0)
		// But research CAN be developed (by player) if "allSatisfied" && (researchTime != 0) && (researchLocation >= 0) && (building is available)
		// Otherwise, the research needs to be triggered by a building ?
		bool allowedResearch = (detail->researchDef->researchTime == 0);
		long int researchLocation = detail->researchDef->researchLocation;
		DetailedBuildingDef *researchLocDetail = this->GetDetailedBuildingDef(researchLocation);
		std::string additionalLogInfo = "";
		if (researchLocDetail && researchLocDetail->isAvailableInCurrentState) {
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
			for each (DetailedResearchDef *reqDetail in detail->directRequirements)
			{
				detail->allRequirementsExcludingAges.insert(reqDetail);
				detail->AddAllRequirementsFrom(reqDetail); // inherit required research's requirements
			}
			this->myLog.append("Research #");
			this->myLog.append(std::to_string(detail->GetResearchDefId()));
			this->myLog.append(" / ");
			this->myLog.append(detail->internalName);
			this->myLog.append(" : all requirements are known =>");
			for each (DetailedResearchDef *reqDetail in detail->allRequirementsExcludingAges)
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

	DetailedBuildingDef *townCenterDetail = this->GetDetailedBuildingDef(CST_UNITID_FORUM);
	if (townCenterDetail) {
		this->myLog.append("Adding Town Center (#109) to available buildings to match game behavior." NEWLINE);
		townCenterDetail->isAvailableImmediately = true; // Hack that corresponds... To the game hack (TC can be build at any time - if not already built - event when not enabled yet by research 115)
	}

	this->EvaluateRequiredResearches();

	// Debug: log missing research (there should be none ; with standard empires.dat, there IS none !)
	for (int researchId = 0; researchId < this->researchCount; researchId++) {
		DetailedResearchDef *detail = this->GetDetailedResearchDef(researchId);
		if (!detail || !detail->active) { continue; }
		if (detail->allRequirementsAreKnown) { continue; }
		this->myLog.append("Remaining research #");
		this->myLog.append(std::to_string(researchId));
		this->myLog.append(" / ");
		this->myLog.append(detail->internalName);
		this->myLog.append(NEWLINE);
	}
	
	// TEST
	for (int researchId = 0; researchId < this->researchCount; researchId++) {
		DetailedResearchDef *detail = this->GetDetailedResearchDef(researchId);
		if (!detail || !detail->active) { continue; }
		if (!detail->allRequirementsAreKnown) { continue; }
		this->myLog.append("Research #");
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
		for each (auto req in detail->allRequirementsExcludingAges)
		{
			this->myLog.append(" ");
			this->myLog.append(std::to_string(req->GetResearchDefId()));
		}
		this->myLog.append(NEWLINE);
	}


	return true;
}


}
}
