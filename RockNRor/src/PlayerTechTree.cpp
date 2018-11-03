#include "../include/PlayerTechTree.h"

// \r is required to display correctly in ROR editable text components
#define NEWLINE "\r\n"

namespace ROCKNROR {
namespace STRATEGY {
;


// Fill eligibleDisableXxx, disabledResearchIds, disabledUnitDefIds collections (etc) according to player's tech tree (myTechTreeId).
void CustomPlayerInfo::CollectInfoFromExistingTechTree() {
	if (!this->isActive || (this->myTechTreeId < 0)) { return; }
	if (!ROCKNROR::crInfo.techTreeAnalyzer.IsReady()) { return; }
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid() || !global->civilizationDefinitions || (global->civCount < 2)) { return; }
	if (!global->technologiesInfo || !global->technologiesInfo->IsCheckSumValid()) { return; }
	STRUCT_CIVILIZATION_DEF *civDef1 = global->civilizationDefinitions[1];
	STRUCT_TECH_DEF *techTreeDef = global->technologiesInfo->GetTechDef((short int)this->myTechTreeId);
	if (!techTreeDef) { return; }

	std::set<long int> disabledResearchIds;
	std::set<long int> disabledUnitDefIds;

	for (int effectId = 0; effectId < techTreeDef->effectCount; effectId++) {
		STRUCT_TECH_DEF_EFFECT *effect = &techTreeDef->ptrEffects[effectId];
		// Does this effect disables a research ?
		if (effect->effectType == TECH_DEF_EFFECTS::TDE_DISABLE_RESEARCH) {
			disabledResearchIds.insert((long int)effect->effectValue); // effect value represents research id.
		}
		// Does this effect disables a unit (not used in tech trees in standard empires.dat) ?
		if ((effect->effectType == TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT) && (effect->effectClass == 0)) {
			disabledUnitDefIds.insert((long int)effect->effectUnit);
		}
	}

	// Add researches that become impossible because of "disabledResearchIds"
	std::set<long int> tmpListOfIds;
	for each (long int researchId in disabledResearchIds) {
		tmpListOfIds.insert(researchId);
	}
	for each (long int researchId in tmpListOfIds)
	{
		ROCKNROR::STRATEGY::TTDetailedResearchDef *detail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(researchId);
		if (!detail || !detail->active) { continue; }
		for each (ROCKNROR::STRATEGY::TTDetailedResearchDef *childResearch in detail->allChildResearches) {
			long int childId = childResearch->GetResearchDefId();
			if (childId >= 0) {
				disabledResearchIds.insert(childId); // maybe it was already there: "insert" handles unicity in std::set
			}
		}
	}

	// Units that are enabled in "disabledResearchIds" researches <=> DISABLED by tech tree: compute them and update disabledUnitDefIds
	for each (long int researchId in disabledResearchIds)
	{
		ROCKNROR::STRATEGY::TTDetailedResearchDef *detail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(researchId);
		if (!detail || !detail->active) { continue; }
		for each (ROCKNROR::STRATEGY::TTDetailedTrainableUnitDef *detail in detail->enableTrainables) {
			disabledUnitDefIds.insert(detail->unitDefId);
		}
		for each (ROCKNROR::STRATEGY::TTDetailedBuildingDef *bld in detail->enableBuildings) {
			disabledUnitDefIds.insert(bld->unitDefId);
		}
		for each (std::pair<long int, long int> pairUpgrade in detail->upgradedUnitDefId) {
			disabledUnitDefIds.insert(pairUpgrade.second);
			// Children should be handled by research dependencies, so we shouldn't need to handle them here
		}
	}

	// Now use collected info to organize known researches/units to enabled/disabled collections
	for (int resDefId = 0; resDefId < ROCKNROR::crInfo.techTreeAnalyzer.GetResearchCount(); resDefId++) {
		auto it = std::find(disabledResearchIds.begin(), disabledResearchIds.end(), resDefId);
		if (it == disabledResearchIds.end()) {
			// Not found: research is OK (not disabled by tech tree)
			auto detail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resDefId);
			if (detail && detail->active) {
				if (detail->researchDef->IsShadowResearch()) {
					this->enabledResearchesInfo.insert(ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resDefId));
				}
				this->enabledResearchesInfoAll.insert(ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resDefId));
			}
		} else {
			this->disabledResearchesInfo.insert(ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resDefId));
		}
	}

	for (int unitDefId = 0; unitDefId < ROCKNROR::crInfo.techTreeAnalyzer.GetUnitDefCount(); unitDefId++) {
		auto it = std::find(disabledUnitDefIds.begin(), disabledUnitDefIds.end(), unitDefId);
		bool isNeverEnabled = false;
		STRUCT_UNITDEF_BASE *unitDef = civDef1->GetUnitDef(unitDefId);
		if (unitDef && unitDef->DerivesFromTrainable()) {
			if (unitDef->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) {
				ROCKNROR::STRATEGY::TTDetailedBuildingDef *bldDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(unitDefId);
				if (!bldDetail || !bldDetail->isAvailableAfterAnalysis) { isNeverEnabled = true; }
			} else {
				assert(unitDef->unitType == GLOBAL_UNIT_TYPES::GUT_TRAINABLE);
				ROCKNROR::STRATEGY::TTDetailedTrainableUnitDef *unitDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(unitDefId);
				if (!unitDetail || !unitDetail->isAvailableAfterAnalysis) { isNeverEnabled = true; }
			}
		}

		// if isNeverEnabled, then use a dedicated collection ? (heroes/scenario/unused units) Or even ignore them !

		if (unitDef && unitDef->DerivesFromTrainable()) {
			if (!isNeverEnabled && (it == disabledUnitDefIds.end())) {
				// Not found: unitDef is OK (not disabled by tech tree)
				if (unitDef->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) {
					this->enabledBuildingsInfo.insert(ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(unitDefId));
				} else {
					assert(unitDef->unitType == GLOBAL_UNIT_TYPES::GUT_TRAINABLE);
					this->enabledTrainableUnitInfos.insert(ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(unitDefId));
				}
			} else {
				if (unitDef->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) {
					this->disabledBuildingsInfo.insert(ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(unitDefId));
				} else {
					assert(unitDef->unitType == GLOBAL_UNIT_TYPES::GUT_TRAINABLE);
					this->disabledTrainableUnitInfos.insert(ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(unitDefId));
				}
			}
		}
	}
}


// Returns true if the living unit is disabled in player tech tree.
// Returns false if the unit is NOT a living unit (returns false for buildings !)
bool CustomPlayerInfo::IsTrainableUnitDisabled(long int unitDefId) const {
	auto search = std::find_if(this->disabledTrainableUnitInfos.cbegin(), this->disabledTrainableUnitInfos.cend(),
		[unitDefId](TTDetailedUnitDef *curUnitInfo){ return curUnitInfo->unitDefId == unitDefId; }
	);
	return (search != this->disabledTrainableUnitInfos.cend());
}

// Returns true if the building is disabled in player tech tree.
// Returns false if the unit is NOT a building
bool CustomPlayerInfo::IsBuildingUnitDisabled(long int unitDefId) const {
	auto search = std::find_if(this->disabledBuildingsInfo.cbegin(), this->disabledBuildingsInfo.cend(),
		[unitDefId](TTDetailedBuildingDef *curUnitInfo){ return curUnitInfo->unitDefId == unitDefId; }
	);
	return (search != this->disabledBuildingsInfo.cend());
}

// Returns true if the unit (living unit or building) is disabled in player tech tree
bool CustomPlayerInfo::IsUnitDisabled(long int unitDefId) const {
	return this->IsTrainableUnitDisabled(unitDefId) ||
		this->IsBuildingUnitDisabled(unitDefId);
}

// Returns true if the research is disabled in player tech tree
bool CustomPlayerInfo::IsResearchDisabled(long int resDefId) const {
	auto search = std::find_if(this->disabledResearchesInfo.cbegin(), this->disabledResearchesInfo.cend(),
		[resDefId](TTDetailedResearchDef *curResInfo){ return curResInfo->GetResearchDefId() == resDefId; }
	);
	return (search != this->disabledResearchesInfo.cend());
}


// Adds an asterisk to unit names, for unit that benefit from a civ bonus
void CustomPlayerInfo::MarkCivBonusUnitNames() {
	if ((this->myPlayerId < 1) || (this->myTechTreeId < 0)) { return; }
	if (!ROCKNROR::crInfo.configInfo.markUnitsWithCivBonus) { return; }
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	STRUCT_TECH_DEF *techTreeDef = global->technologiesInfo->GetTechDef((short int)this->myTechTreeId);
	if (!techTreeDef) { return; }
	AOE_TECHNOLOGIES::TechnologyFilterBase *t = new AOE_TECHNOLOGIES::TechFilterNoTechTreeNoAge();
	AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(this->myPlayerId);
	if (!player) { return; }

	for (int unitDefId = 0; unitDefId < player->structDefUnitArraySize; unitDefId++) {
		if (this->IsUnitDisabled(unitDefId)) { continue; }
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = player->GetUnitDefBase(unitDefId);
		if (!unitDef) { continue; }
		if (DoesTechAffectUnit(techTreeDef, unitDef, t)) {
			std::string actualUnitName = GetLanguageDllText(unitDef->languageDLLID_Name);
			if (!actualUnitName.empty()) {
				int len = actualUnitName.length(); // text length (excluding ending \0)
				if (actualUnitName[len - 1] != '*') {
					AOEFree(unitDef->ptrUnitName);
					unitDef->ptrUnitName = (char*)AOEAlloc(len + 2);
					strncpy_s(unitDef->ptrUnitName, len + 2, actualUnitName.c_str(), len + 1);
					unitDef->ptrUnitName[len] = '*';
					unitDef->ptrUnitName[len + 1] = 0;
					unitDef->languageDLLID_Name = 0;
				}
			}
		}
	}
}


// Returns the final "disable probability" for this research.
// Returns <=0 if the unit must not be disable OR must not be disabled directly (handled by "unit line")
double TTCreatorResearchInfo::GetDisableProbability() const {
	if (this->hasBeenDisabled) { return -1; }
	// this->rawDisableProbability is in [0;1], so we can easily adjust it to reduce standard deviation
	double result = this->rawDisableProbability - 0.5; // now in [-0.5;0.5]
	result *= ROCKNROR::STRATEGY::TT_CONFIG::RES_PROBA_IMPACT_ON_RANDOM; // interval is reduced (constant is in [0;1])
	result += 0.5; // back to [0;1] interval (practically, smaller interval)
	return result;
}


// Returns the final "disable probability" for this unit.
// Returns <=0 if the unit must not be disable OR must not be disabled directly (handled by "unit line")
double TTCreatorUnitInfo::GetDisableProbability() const {
	if (!this->unitDetail->IsValid()) { return -1.; }
	if (this->hasADisabledChild || this->hasBeenDisabled) { return -1.; }
	if (this->unitDefId != this->unitDetail->baseUnitId) { return -1.; } // Only treat base units => 1 evaluation for each unitline

	if ((this->rootUnitDisablePolicy == TTCreatorBaseUnitDisableBehavior::TTBUDisableNever) &&
		(this->unitDetail->possibleUpgradedUnitIDs.size() == 0)) {
		return 0.;
	} else {
		// this->rawDisableProbability is in [0;1], so we can easily adjust it to reduce standard deviation
		double result = this->rawDisableProbability - 0.5; // now in [-0.5;0.5]
		result *= ROCKNROR::STRATEGY::TT_CONFIG::RES_PROBA_IMPACT_ON_RANDOM; // interval is reduced (constant is in [0;1])
		result += 0.5; // back to [0;1] interval (practically, smaller interval)
		return result;
	}
}


// Create a random tech tree (list of effects) on provided TechDef (should be initially empty).
void TechTreeCreator::CreateRandomTechTree(STRUCT_TECH_DEF *techDef) {
	if (!techDef || (techDef->effectCount > 0)) { return; }
	if (!ROCKNROR::crInfo.techTreeAnalyzer.IsReady()) { return; }

	this->SetConfigFromStatistics();

	this->techDefForTechTree = techDef;
	this->arrayResearchCount = ROCKNROR::crInfo.techTreeAnalyzer.GetResearchCount();
	this->arrayUnitDefCount = ROCKNROR::crInfo.techTreeAnalyzer.GetUnitDefCount();

	// Get non-shadow researches
	this->CollectResearchInfo();
	int researchCountBronze = this->eligibleDisableResearchIdBronze.size();
	int researchCountIron = this->eligibleDisableResearchIdIron.size();

	// Get eligible units that may be disabled (or receive bonus). Note: some unit excluded here may receive bonus via "unit class" bonuses.
	this->CollectUnitInfo();
	int eligibleDisableUnitDefIdBronzeCount = this->eligibleDisableUnitDefIdBronze.size();
	int eligibleDisableUnitDefIdIronCount = this->eligibleDisableUnitDefIdIron.size();


	// Init base probabilities, also create research/unit internal info objects.
	this->SetResearchBaseProbabilities();
	this->SetUnitBaseProbabilities();
	this->UpdateResearchProbabilitiesWithImpacts();
	
	// DISABLE RESEARCH (may cascade on other researches + units)
	this->CreateDisableResearchesEffects();

	// DISABLE UNITS/upgrades (via research)
	this->CreateDisableUnitsEffects();

	this->CalcTotalDisabledWeight();

	if (simulationMode) {
		return;
	}

	// CIV BONUS
	this->CalcRandomCivBonus();

	this->AddEffectsToTechDef();

	// TODO: protect at least 1 (2?) super unit in tech tree
	// Do not allow disable building at this point (except iron age towers) + temple ?
}



// Get a TTCreatorResearchInfo from this->allCreatorResearchInfo for given ID. NULL if not found
TTCreatorResearchInfo *TechTreeCreator::GetCrResearchInfo(long int researchId) const {
	auto it = std::find_if(this->allCreatorResearchInfo.begin(), this->allCreatorResearchInfo.end(),
		[researchId](TTCreatorResearchInfo *lambdaInfo){return lambdaInfo->researchId == researchId; }
	);
	if (it == this->allCreatorResearchInfo.end()) {
		return NULL;
	}
	return *it;
}


// Get a TTCreatorUnitInfo from this->allCreatorUnitInfo for given ID. NULL if not found
TTCreatorUnitInfo *TechTreeCreator::GetCrUnitInfo(long int unitDefId) const {
	auto it = std::find_if(this->allCreatorUnitInfo.begin(), this->allCreatorUnitInfo.end(),
		[unitDefId](TTCreatorUnitInfo *lambdaInfo){return lambdaInfo->unitDefId == unitDefId; }
	);
	if (it == this->allCreatorUnitInfo.end()) {
		return NULL;
	}
	return *it;
}


void TechTreeCreator::SetConfigFromStatistics() {
	if (!ROCKNROR::crInfo.techTreeAnalyzer.IsReady()) { return; }
	// Units
	int bronzeIronTrainableLinesCount = ROCKNROR::crInfo.techTreeAnalyzer.statistics.rootTrainablesCountByAge[2] + ROCKNROR::crInfo.techTreeAnalyzer.statistics.rootTrainablesCountByAge[3];
	bronzeIronTrainableLinesCount++; // For slinger as we allow disabling it
	int minDisableUnitLines = (bronzeIronTrainableLinesCount * TT_CONFIG::CALC_MIN_DISABLE_UNITLINE_COUNT_PERCENT) / 100;
	int maxDisableUnitLines = (bronzeIronTrainableLinesCount * TT_CONFIG::CALC_MAX_DISABLE_UNITLINE_COUNT_PERCENT) / 100;
	if (bronzeIronTrainableLinesCount < 10) {
		minDisableUnitLines = min(3, bronzeIronTrainableLinesCount - 2);
		maxDisableUnitLines = max(bronzeIronTrainableLinesCount - 3, 0);
	}
	
	// Researches
	// *** WARNING : at this point we disable researches individually, not using research lines !!! ***
	int bronzeIronResearchLinesCount = ROCKNROR::crInfo.techTreeAnalyzer.statistics.rootNonShadowResearchesCountByAge[2] + ROCKNROR::crInfo.techTreeAnalyzer.statistics.rootNonShadowResearchesCountByAge[3];
	bronzeIronResearchLinesCount -= 3; // Exclude bronze/iron age, wheel TODO should be more generic
	bronzeIronResearchLinesCount -= 2; // Exclude walls TODO should be more geenric
	int minDisableResearchLines = (bronzeIronResearchLinesCount * TT_CONFIG::CALC_MIN_DISABLE_RESEARCH_COUNT_PERCENT) / 100;
	int maxDisableResearchLines = (bronzeIronResearchLinesCount * TT_CONFIG::CALC_MAX_DISABLE_RESEARCH_COUNT_PERCENT) / 100;
	if (bronzeIronResearchLinesCount < 10) {
		minDisableResearchLines = min(2, bronzeIronResearchLinesCount - 3);
		maxDisableResearchLines = max(bronzeIronResearchLinesCount - 4, 0);
	}

	TT_CONFIG::MIN_DISABLE_UNITLINE_COUNT = minDisableUnitLines;
	TT_CONFIG::MAX_DISABLE_UNITLINE_COUNT = maxDisableUnitLines;
	TT_CONFIG::MIN_DISABLE_RESEARCH_COUNT = minDisableResearchLines;
	TT_CONFIG::MAX_DISABLE_RESEARCH_COUNT = maxDisableResearchLines;
}


void TechTreeCreator::CollectResearchInfo() {
	for (int resDefId = 0; resDefId < this->arrayResearchCount; resDefId++) {
		TTDetailedResearchDef *detail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resDefId);
		if (!detail || !detail->active || detail->IsShadowResearch()) { continue; }
		if (!detail->techDef || !detail->researchDef) { continue; }
		if (!detail->hasValidEffect) { continue; }
		if (detail->requiredAge < CST_RSID_BRONZE_AGE) { continue; } // pre-bronze researches can't be disabled by tech tree
		
		// Exclude research that enable/upgrade units (we handle those in "disable unit" stuff)
		bool doesEnableOrUpgradeUnit = false;
		for (int effectId = 0; effectId < detail->techDef->effectCount; effectId++) {
			switch (detail->techDef->ptrEffects[effectId].effectType) {
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT:
				if (detail->techDef->ptrEffects[effectId].effectClass == 1) {
					doesEnableOrUpgradeUnit = true;
				} else {
					// Weird: a non-tech tree tech that disables some unit ? Maybe to replace it with something else, but why don't use upgrade ?
				}
				break;
			case AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_UPGRADE_UNIT:
				doesEnableOrUpgradeUnit = true;
				break;
			default:break;
			}
		}
		if (doesEnableOrUpgradeUnit) {
			continue;
		}
		
		AOE_CONST_FUNC::ATTACK_CLASS armorClass = AOE_STRUCTURES::RESEARCH::DoesTechImproveSomeArmor(detail->techDef);
		AOE_CONST_FUNC::ATTACK_CLASS attackClass = AOE_STRUCTURES::RESEARCH::DoesTechImproveSomeAttack(detail->techDef);
		if ((armorClass == ATTACK_CLASS::CST_AC_BASE_MELEE) && (detail->requiredAge < AOE_CONST_FUNC::CST_RSID_IRON_AGE) &&
			(detail->researchDef->researchLocation == AOE_CONST_FUNC::CST_UNITID_STORAGE_PIT)) {
			continue; // Never disable pre-iron *melee* armor researches in storage pit (however shields can be disabled)
		}
		if ((attackClass == ATTACK_CLASS::CST_AC_BASE_MELEE) && (detail->requiredAge < AOE_CONST_FUNC::CST_RSID_IRON_AGE) &&
			(detail->researchDef->researchLocation == AOE_CONST_FUNC::CST_UNITID_STORAGE_PIT)) {
			continue; // Never disable pre-iron attack researches in storage pit
		}

		if ((detail->GetResearchDefId() >= AOE_CONST_FUNC::CST_RSID_STONE_AGE) &&
			(detail->GetResearchDefId() <= AOE_CONST_FUNC::CST_RSID_IRON_AGE)) {
			continue; // Never disable ages (!!!)
		}

		if (AOE_STRUCTURES::RESEARCH::DoesTechRevealAlly(detail->techDef)) {
			continue; // Never disable writing
		}

		if (detail->requiredAge == AOE_CONST_FUNC::CST_RSID_BRONZE_AGE) {
			this->eligibleDisableResearchIdBronze.insert(resDefId);
		}
		if (detail->requiredAge == AOE_CONST_FUNC::CST_RSID_IRON_AGE) {
			this->eligibleDisableResearchIdIron.insert(resDefId);
		}
	}
}


void TechTreeCreator::CollectUnitInfo() {
	for (int unitDefId = 0; unitDefId < this->arrayUnitDefCount; unitDefId++) {
		// Remark: excluding buildings here
		TTDetailedTrainableUnitDef *detail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(unitDefId);
		if (!detail || detail->IsHeroOrScenarioUnit() || !detail->unitDef) { continue; }
		if (detail->unitDef->unitAIType == AOE_CONST_FUNC::TribeAIGroupCivilian) {
			continue;
		}
		bool isPreIron = (detail->requiredAge < AOE_CONST_FUNC::CST_RSID_IRON_AGE);
		bool isRootUnit = detail->baseUnitId == detail->unitDefId;
		isRootUnit = (detail->baseUnitId == detail->possibleAncestorUnitIDs.size() == 0);
		if (isRootUnit &&
			((detail->unitDef->unitAIType == AOE_CONST_FUNC::TribeAIGroupTradeBoat) ||
			(detail->unitDef->unitAIType == AOE_CONST_FUNC::TribeAIGroupTransportBoat) ||
			(detail->unitDef->unitAIType == AOE_CONST_FUNC::TribeAIGroupFishingBoat))) {
			continue; // never disable *root* trade/transport/fishing units
		}
		if (isPreIron &&
			(detail->unitDef->unitAIType == AOE_CONST_FUNC::TribeAIGroupWarBoat)) {
			continue; // never disable pre-iron war ships
		}
		if (detail->requiredAge <= AOE_CONST_FUNC::CST_RSID_TOOL_AGE) {
			// Only slinger can be disabled in tool age
			if (detail->unitDef->unitAIType != AOE_CONST_FUNC::TribeAIGroupSlinger) {
				continue;
			}
		}
		// TODO add other custom rules (units that can't be disabled)

		if (detail->requiredAge <= AOE_CONST_FUNC::CST_RSID_BRONZE_AGE) {
			this->eligibleDisableUnitDefIdBronze.insert(unitDefId); // include tool age units (only slinger !)
		}
		if (detail->requiredAge == AOE_CONST_FUNC::CST_RSID_IRON_AGE) {
			this->eligibleDisableUnitDefIdIron.insert(unitDefId);
		}
	}
}


// Set initial (base) probabilities for all researches. Does not take care of dependencies/impact on other researches/units.
// Creates the TTCreatorResearchInfo objects into allCreatorResearchInfo list.
void TechTreeCreator::SetResearchBaseProbabilities() {
	// INIT: create object for each eligible research.
	for each (long int resDefId in this->eligibleDisableResearchIdBronze) {
		TTCreatorResearchInfo *ttcResearchInfo = new TTCreatorResearchInfo();
		ttcResearchInfo->researchId = resDefId;
		ttcResearchInfo->researchDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resDefId);
		if (ttcResearchInfo->researchDetail) {
			ttcResearchInfo->internalName = ttcResearchInfo->researchDetail->internalName;
			ttcResearchInfo->langName = ttcResearchInfo->researchDetail->langName;
		}
		// ttcResearchInfo->researchDetail should always be valid: all IDs from this->eligible* collections should be valid
		this->allCreatorResearchInfo.push_back(ttcResearchInfo);
	}
	for each (long int resDefId in this->eligibleDisableResearchIdIron) {
		TTCreatorResearchInfo *ttcResearchInfo = new TTCreatorResearchInfo();
		ttcResearchInfo->researchId = resDefId;
		ttcResearchInfo->researchDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resDefId);
		if (ttcResearchInfo->researchDetail) {
			ttcResearchInfo->internalName = ttcResearchInfo->researchDetail->internalName;
			ttcResearchInfo->langName = ttcResearchInfo->researchDetail->langName;
		}
		// ttcResearchInfo->researchDetail should always be valid: all IDs from this->eligible* collections should be valid
		this->allCreatorResearchInfo.push_back(ttcResearchInfo);
	}
	

	// Collect general info and handle some specific cases
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo)
	{
		assert(crResInfo->researchDetail != NULL);
		crResInfo->researchLocation = crResInfo->researchDetail->researchDef->researchLocation;
		AOE_CONST_FUNC::ATTACK_CLASS armorClass = AOE_STRUCTURES::RESEARCH::DoesTechImproveSomeArmor(crResInfo->researchDetail->techDef);
		AOE_CONST_FUNC::ATTACK_CLASS attackClass = AOE_STRUCTURES::RESEARCH::DoesTechImproveSomeAttack(crResInfo->researchDetail->techDef);
		crResInfo->techImpactsReligion = AOE_STRUCTURES::RESEARCH::DoesTechImpactReligion(crResInfo->researchDetail->techDef);

		if ((crResInfo->rawDisableProbability < 0) && (armorClass == ATTACK_CLASS::CST_AC_BASE_PIERCE)) {
			if (crResInfo->researchDetail->researchLinePredecessors.size() == 0) {
				crResInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_STANDARD_RESEARCH;
				crResInfo->disableWeight = TT_CONFIG::RES_WEIGHT_STANDARD_RESEARCH;
			} else {
				crResInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_SPECIALIZED_RESEARCH;
				crResInfo->disableWeight = TT_CONFIG::RES_WEIGHT_STANDARD_RESEARCH;
			}
		}
	}


	// Set probability for friendly researches (temple...)
	// Temple: choose "religion level" : low/moderate/high. All temple techs proba. will be chosen in the same probability range, according to this level.
	//  level 0 (min): being disable is VERY likely to happen for those techs ; level 5 (max): player is likely to have access to all techs (good temple civ)
	this->religionLevel = randomizer.GetRandomValue(0, 5);
	double religionProba= TT_CONFIG::RES_PROBA_SPECIALIZED_RESEARCH;
	double weightForReligion = TT_CONFIG::RES_WEIGHT_STANDARD_RESEARCH;
	double religionBronzeFactor = 1; // applied to probas for bronze age religion techs. >1 means less disable proba (protect bronze age techs)
	religionProba = 0.05 + (5 - this->religionLevel) * 0.15; // max 0.05+0.75=0.8 for min religion level
	religionBronzeFactor = 1 + 0.03 *this->religionLevel; // max 1.15 "protection %" for bronze techs

	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo)
	{
		if (crResInfo->techImpactsReligion && (crResInfo->rawDisableProbability < 0)) {
			crResInfo->rawDisableProbability = religionProba;
			crResInfo->disableWeight = weightForReligion;
			if (crResInfo->researchDetail->requiredAge < AOE_CONST_FUNC::CST_RSID_IRON_AGE) {
				crResInfo->disableWeight = crResInfo->disableWeight * religionBronzeFactor;
				crResInfo->rawDisableProbability = crResInfo->rawDisableProbability * (2 - religionBronzeFactor);
			}
			// Higher proba for jihad, martyrdom
			if (crResInfo->researchDetail->hasNegativeSideEffect || crResInfo->researchDetail->isAiUnsupported) {
				crResInfo->rawDisableProbability += 0.05;
			}

			if (crResInfo->disableWeight > 1) { crResInfo->disableWeight = 1; }
			if (crResInfo->rawDisableProbability > 1) { crResInfo->rawDisableProbability = 1; }
		}
	}

	// Government center + market techs
	this->economyLevel = randomizer.GetRandomValue(0, 3);
	double randomProbaForEconomy = TT_CONFIG::RES_PROBA_STANDARD_RESEARCH;
	double weightForEconomy = TT_CONFIG::RES_WEIGHT_STANDARD_RESEARCH;
	double economyBronzeFactor = 1.25;
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo)
	{
		bool isMarketOrGov = (crResInfo->researchLocation == AOE_CONST_FUNC::CST_UNITID_MARKET) ||
			(crResInfo->researchLocation == AOE_CONST_FUNC::CST_UNITID_GOVERNMENT_CENTER);
		if ((crResInfo->rawDisableProbability < 0) && isMarketOrGov) {
			// Protect key techs like wheel, architecture, woodworking+range => reduce their probabilities
			if (crResInfo->researchDetail->IsWheel()) {
				crResInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_WHEEL;
				crResInfo->disableWeight = TT_CONFIG::RES_WEIGHT_WHEEL;
				continue;
			}
			if (crResInfo->researchDetail->IsArchitecture()) {
				crResInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_COMMON_USEFUL_RESEARCH;
				crResInfo->disableWeight = TT_CONFIG::RES_WEIGHT_COMMON_USEFUL_RESEARCH;
				continue;
			}
			if (crResInfo->researchDetail->IsWoodWorkingAndRange()) {
				crResInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_COMMON_USEFUL_RESEARCH;
				crResInfo->disableWeight = TT_CONFIG::RES_WEIGHT_COMMON_USEFUL_RESEARCH;
				continue;
			}

			crResInfo->rawDisableProbability = randomProbaForEconomy;
			crResInfo->disableWeight = weightForEconomy;
			if (crResInfo->researchDetail->requiredAge < AOE_CONST_FUNC::CST_RSID_IRON_AGE) {
				crResInfo->disableWeight = crResInfo->disableWeight * economyBronzeFactor;
				crResInfo->rawDisableProbability = crResInfo->rawDisableProbability * (2 - economyBronzeFactor);
			}
			if (crResInfo->disableWeight > 1) { crResInfo->disableWeight = 1; }
			if (crResInfo->rawDisableProbability > 1) { crResInfo->rawDisableProbability = 1; }
		}
	}

	// Set probability for remaining researches
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo)
	{
		if (crResInfo->rawDisableProbability < 0) {
			crResInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_STANDARD_RESEARCH;
			crResInfo->disableWeight = TT_CONFIG::RES_WEIGHT_STANDARD_RESEARCH;
		}
	}
}


// Set initial (base) probabilities for all units. Does not take care of dependencies/impact on other researches/units.
// Creates the TTCreatorUnitInfo objects into allCreatorUnitInfo list.
void TechTreeCreator::SetUnitBaseProbabilities() {
	// INIT: create object for each eligible unit.
	for each (long int resUnitId in this->eligibleDisableUnitDefIdBronze) {
		TTCreatorUnitInfo *ttcUnitInfo = new TTCreatorUnitInfo();
		ttcUnitInfo->unitDefId = resUnitId;
		TTDetailedTrainableUnitDef *trainableDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(resUnitId);
		if (trainableDetail) {
			ttcUnitInfo->unitDetail = trainableDetail;
			ttcUnitInfo->trainLocation = trainableDetail->GetTrainLocation();
			ttcUnitInfo->internalName = trainableDetail->internalName;
			ttcUnitInfo->langName = trainableDetail->langName;
			for each (long int srcResDefId in trainableDetail->researchIdsThatEnableMe)
			{
				if (ttcUnitInfo->sourceResearchId == -1) {
					ttcUnitInfo->sourceResearchId = srcResDefId;
				} else {
					assert(false && "non-unique source research id for unit");
				}
			}
		} else {
			TTDetailedBuildingDef *ttcBldInfo = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(resUnitId);
			ttcUnitInfo->unitDetail = ttcBldInfo;
			if (ttcBldInfo) {
				ttcUnitInfo->internalName = ttcBldInfo->internalName;
				ttcUnitInfo->langName = ttcBldInfo->langName;
			}
		} 
		assert(ttcUnitInfo->unitDetail);
		this->allCreatorUnitInfo.push_back(ttcUnitInfo);
	}
	for each (long int resUnitId in this->eligibleDisableUnitDefIdIron) {
		TTCreatorUnitInfo *ttcUnitInfo = new TTCreatorUnitInfo();
		ttcUnitInfo->unitDefId = resUnitId;
		TTDetailedTrainableUnitDef *trainableDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(resUnitId);
		if (trainableDetail) {
			ttcUnitInfo->unitDetail = trainableDetail;
			ttcUnitInfo->trainLocation = trainableDetail->GetTrainLocation();
			ttcUnitInfo->internalName = trainableDetail->internalName;
			ttcUnitInfo->langName = trainableDetail->langName;
			for each (long int srcResDefId in trainableDetail->researchIdsThatEnableMe)
			{
				if (ttcUnitInfo->sourceResearchId == -1) {
					ttcUnitInfo->sourceResearchId = srcResDefId;
				} else {
					assert(false && "non-unique source research id for unit");
				}
			}
		} else {
			TTDetailedBuildingDef *ttcBldInfo = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(resUnitId);
			ttcUnitInfo->unitDetail = ttcBldInfo;
			if (ttcBldInfo) {
				ttcUnitInfo->internalName = ttcBldInfo->internalName;
				ttcUnitInfo->langName = ttcBldInfo->langName;
			}
		}
		this->allCreatorUnitInfo.push_back(ttcUnitInfo);
	}

	// We run 1 evaluation for each "unit line" (same base id) : e.g. common evaluation for clubman+axeman
	for each (TTCreatorUnitInfo *crUnitInfo in this->allCreatorUnitInfo)
	{
		if (!crUnitInfo->unitDetail->IsValid()) { continue; }
		if (crUnitInfo->unitDefId != crUnitInfo->unitDetail->baseUnitId) { continue; } // Only treat base units => 1 evaluation for each unitline

		// By default, all child units have default weight
		for each (long int childId in crUnitInfo->unitDetail->possibleUpgradedUnitIDs)
		{
			TTCreatorUnitInfo *child = this->GetCrUnitInfo(childId);
			if (!child) { continue; }
			child->disableWeight = TT_CONFIG::RES_PROBA_STANDARD_UNIT;
		}

		// Special: priest. Disable probability is extremely low, and only possible if religion level is already poor.
		if (crUnitInfo->unitDetail->unitClass == AOE_CONST_FUNC::TribeAIGroupPriest) {
			if (this->religionLevel > 0) {
				crUnitInfo->rootUnitDisablePolicy = TTCreatorBaseUnitDisableBehavior::TTBUDisableNever;
				crUnitInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_MIN;
				crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_HIGH_IMPACT_UNIT;
			} else {
				crUnitInfo->rootUnitDisablePolicy = TTCreatorBaseUnitDisableBehavior::TTBUDisableVeryRare;
				crUnitInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_VERY_RARE;
				crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_HIGH_IMPACT_UNIT;
			}
			continue;
		}

		// Special: iron age "root" boats (note: pre-iron war boats can't be disabled, cf CollectUnitInfo) => fire galley, cat.trireme
		if ((crUnitInfo->unitDetail->unitClass == AOE_CONST_FUNC::TribeAIGroupWarBoat) &&
			(crUnitInfo->unitDetail->requiredAge == AOE_CONST_FUNC::CST_RSID_IRON_AGE)) {
			crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_STANDARD_UNIT;
			crUnitInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_STANDARD_UNIT; // Useful for other upgrades (catapult trireme)
			// Will force to choose among iron-age root units of same class (warboats here)
			crUnitInfo->rootUnitDisablePolicy = TTCreatorBaseUnitDisableBehavior::TTBUDisableMostlyExclusiveSameClassIron;
			continue;
		}

		// Special: stone thrower and siege
		if (crUnitInfo->unitDetail->unitClass == AOE_CONST_FUNC::TribeAIGroupSiegeWeapon) {
			if (crUnitInfo->unitDefId == AOE_CONST_FUNC::CST_UNITID_STONE_THROWER) {
				crUnitInfo->rootUnitDisablePolicy = TTCreatorBaseUnitDisableBehavior::TTBUDisableVeryRare;
				crUnitInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_STANDARD_UNIT; // Useful for other upgrades (catapults)
				crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_HIGH_IMPACT_UNIT;
			} else {
				crUnitInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_SPECIALIZED_UNIT;
				crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_STANDARD_UNIT;
			}
		}

		// Special: chariots(melee+archers) availability has a strong impact on civ (no-gold, precious against priests...)
		if ((crUnitInfo->unitDetail->unitClass == AOE_CONST_FUNC::TribeAIGroupChariot) ||
			(crUnitInfo->unitDetail->unitClass == AOE_CONST_FUNC::TribeAIGroupChariotArcher)) {
			crUnitInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_STANDARD_UNIT;
			crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_CHARIOTS;
		}

		// Special: unit that is only unit in given train location (may depend on mods, in standard game: hoplite)
		if (crUnitInfo->rawDisableProbability < 0) { // exclude already handled units !
			TTDetailedBuildingDef *bld = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(crUnitInfo->trainLocation);
			assert(bld && bld->IsValid());
			if (bld && bld->IsValid()) {
				long int uniqueRootId = -1;
				bool uniqueRootUnitId = true;
				for each (auto oneUnit in bld->unitsTrainedHere)
				{
					TTDetailedTrainableUnitDef *oneUnitDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(oneUnit->DAT_ID1);
					if (uniqueRootId < 0) {
						uniqueRootId = oneUnitDetail->baseUnitId;
					} else {
						uniqueRootUnitId &= (uniqueRootId == oneUnitDetail->baseUnitId);
					}
				}
				// TODO: count "military" researches only ? Only those that apply to THE unit ? TODO EXCLUDE unit upgrades !
				//int researchCountThere = bld->researchesDevelopedHere.size();
				if (uniqueRootUnitId /*&& (researchCountThere == 0)*/) {
					crUnitInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_SPECIALIZED_UNIT;
					crUnitInfo->rootUnitDisablePolicy = TTCreatorBaseUnitDisableBehavior::TTBUDisableVeryRare;
					crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_HIGH_IMPACT_UNIT;
				}
			}
		}

		ROCKNROR::STRATEGY::TTDetailedTrainableUnitDef *unitDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(crUnitInfo->unitDefId);
		if (unitDetail && unitDetail->isSuperUnit) {
			crUnitInfo->disableWeight += TT_CONFIG::RES_ADDITIONTAL_WEIGHT_SUPER_UNIT;
		}
		if (crUnitInfo->disableWeight > TT_CONFIG::RES_WEIGHT_MAX) {
			crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_MAX;
		}
	}

	// Set probability for remaining units
	for each (TTCreatorUnitInfo *crUnitInfo in this->allCreatorUnitInfo)
	{
		if (crUnitInfo->rawDisableProbability < 0) {
			crUnitInfo->rawDisableProbability = TT_CONFIG::RES_PROBA_STANDARD_UNIT;
			crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_STANDARD_UNIT;
		}
	}
}


void TechTreeCreator::UpdateResearchProbabilitiesWithImpacts() {
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo) {
		assert(crResInfo->researchDetail->enableBuildings.size() == 0);
		int nonShadowChildrenCount = std::count_if(crResInfo->researchDetail->allChildResearches.begin(), 
			crResInfo->researchDetail->allChildResearches.end(),
			[](TTDetailedResearchDef *childDetail){ return !childDetail->IsShadowResearch(); }
		);
		if (nonShadowChildrenCount > 10) { nonShadowChildrenCount = 10; }
		crResInfo->disableWeight *= (1 + nonShadowChildrenCount*0.05); // Add 5% weight per dependency

		// underlying "disable units": add a lot of weight as this part is supposed to deal with techs that do NOT impact units activation.
		int directEnableTrainableCount = crResInfo->researchDetail->enableTrainables.size();
		crResInfo->disableWeight *= (1 + directEnableTrainableCount*0.05); // Add 5% weight per dependency

		int indirectEnableTrainableCount = 0;
		std::for_each(crResInfo->researchDetail->allChildResearches.begin(),
			crResInfo->researchDetail->allChildResearches.end(),
			[&indirectEnableTrainableCount](TTDetailedResearchDef *childDetail){
			indirectEnableTrainableCount += childDetail->enableTrainables.size();
		}
		);
		crResInfo->disableWeight *= (1 + directEnableTrainableCount*0.02); // Add 2% weight per indirect dependency

		// Effects on unit (attributes, etc). Add weight if many units (take care of unit classes !)
		int affectedUnitImpact = crResInfo->researchDetail->affectsUnits.size() / 3;
		if (affectedUnitImpact > 8) { affectedUnitImpact = 8; } // More than 24 units affected, stop counting
		crResInfo->disableWeight *= (1 + affectedUnitImpact*0.01);

		if (crResInfo->disableWeight > 1) { crResInfo->disableWeight = 1; } // should not be necessary
	}
}


// Creates disable research effects according to disable scores that have been computed (on all researches)
void TechTreeCreator::CreateDisableResearchesEffects() {
	int preferredDisableResearchCount = randomizer.GetRandomValue_normal_moreFlat(TT_CONFIG::MIN_DISABLE_RESEARCH_COUNT, TT_CONFIG::MAX_DISABLE_RESEARCH_COUNT);
	int curDisableCount = 0;
	// disabled researches in temple count for maximum xxx in curDisableCount.
	// Prevents temple disable researches to count too much in number in disable researches (after all, they all only concern one single unit)
	int templeResearchesDoNotCountMoreThan = 3; 
	while (curDisableCount < preferredDisableResearchCount) {
		double bestScore = -1;
		// Randomly choose a research to disable
		TTCreatorResearchInfo *bestElem = randomizer.PickRandomElementWithWeight_ptrCollection<std::list, TTCreatorResearchInfo>
			(this->allCreatorResearchInfo, [](TTCreatorResearchInfo *curInfo) {
			return curInfo->GetDisableProbability();
		});
		if (bestElem == NULL) { break; }

		std::list<TTCreatorResearchInfo*> disabledChildren = this->CreateDisableEffectOnResearch(bestElem);
		int addedCount = 1; // Corresponds to "bestElem"
		
		if (bestElem->researchLocation == AOE_CONST_FUNC::CST_UNITID_TEMPLE) {
			if (templeResearchesDoNotCountMoreThan > 0) {
				templeResearchesDoNotCountMoreThan--;
			} else {
				addedCount--; // compensate: do not count this researches (too many temple researches have been disabled, do not count them anymore)
			}
		}
		// Count the underlying disabled researches too
		addedCount += disabledChildren.size();
		for each (TTCreatorResearchInfo *child in disabledChildren)
		{
			if (child->isFakeForDisableUnit) {
				addedCount--; // compensate: do not count this research (ignore shadow researches)
				continue; 
			}
			if (child->researchLocation == AOE_CONST_FUNC::CST_UNITID_TEMPLE) {
				if (templeResearchesDoNotCountMoreThan > 0) {
					templeResearchesDoNotCountMoreThan--;
				} else {
					addedCount--; // compensate: do not count this research (too many temple researches have been disabled, do not count them anymore)
				}
			}
		}
		curDisableCount += addedCount;
	}
}


// Creates disable unit effects according to disable scores that have been computed (on root units)
// If a unit(line) is selected for being disabled, then this method chooses at which level (which upgrade) the unitline becomes unavailable.
void TechTreeCreator::CreateDisableUnitsEffects() {
	int preferredDisableUnitCount = randomizer.GetRandomValue_normal_moreFlat(TT_CONFIG::MIN_DISABLE_UNITLINE_COUNT, TT_CONFIG::MAX_DISABLE_UNITLINE_COUNT);
	int curDisableCount = 0;
	while (curDisableCount < preferredDisableUnitCount) {
		double bestScore = -1;
		TTCreatorUnitInfo *rootUnitToDisable = NULL;

		// Randomly choose a unit to disable
		rootUnitToDisable = randomizer.PickRandomElementWithWeight_ptrCollection<std::list, TTCreatorUnitInfo>
			(this->allCreatorUnitInfo, [](TTCreatorUnitInfo *curInfo){
			return curInfo->GetDisableProbability();
		});
		if (rootUnitToDisable == NULL) { break; }

		// Here we have a unit to disable
		TTCreatorUnitInfo *unitToActuallyDisable = this->PickUnitUpgradeToDisableInUnitLine(rootUnitToDisable);

		std::list<TTCreatorResearchInfo*> disabledChildren = this->CreateDisableEffectOnUnit(unitToActuallyDisable);
		unitToActuallyDisable->hasBeenDisabledDirectly = true;
		rootUnitToDisable->hasADisabledChild = true;
		int addedCount = disabledChildren.size(); // disabledChildren includes the researchInfo for unitToActuallyDisable itself.

		curDisableCount += addedCount;
	}
}


// Calculates this->totalDisableWeight according to what has been disabled in tech tree (cf internal data)
void TechTreeCreator::CalcTotalDisabledWeight() {
	this->totalDisableWeight = 0;

	for each (TTCreatorUnitInfo *curUnit in this->allCreatorUnitInfo)
	{
		if (curUnit->hasBeenDisabled) {
			this->totalDisableWeight += curUnit->disableWeight;
		}
	}
	for each (TTCreatorResearchInfo *curResearch in this->allCreatorResearchInfo)
	{
		if (curResearch->researchDetail->IsShadowResearch() || curResearch->isFakeForDisableUnit) { continue; }
		if (curResearch->hasBeenDisabled) {
			this->totalDisableWeight += curResearch->disableWeight;
		}
	}
}


// Creates random civ bonuses
void TechTreeCreator::CalcRandomCivBonus() {
	this->disabledWeightProportion = this->totalDisableWeight;
	if (this->expectedAverageDisableWeight > 0) {
		this->disabledWeightProportion = this->totalDisableWeight / this->expectedAverageDisableWeight;
	}

	int avgCount = ROCKNROR::crInfo.configInfo.randomTechTreeDesiredAvgBonusCount;
	if (avgCount < 1) { avgCount = 1; }
	int maxBonusCount = avgCount + 2; // force to stop loop when that number of bonuses is reached
	this->bonusText.clear();
	double bonusWeight = 0;
	for (int i = 0; i < maxBonusCount; i++) {
		bonusWeight += this->CreateOneBonus();
		this->bonusCount++;
		double estimatedDesiredTotalBonusWeight = this->lastAverageBonusWeight * avgCount;
		this->bonusProportion = bonusWeight / estimatedDesiredTotalBonusWeight;
		if (this->bonusProportion > this->disabledWeightProportion) {
			break;
		}
	}
}


// Create one civ bonus
double TechTreeCreator::CreateOneBonus() {
	this->lastAverageBonusWeight = 0;
	double result = 0;
	std::map<GLOBAL_UNIT_AI_TYPES, int> unitLinesByUnitClass; // number of non-disabled unit for each class
	for (int i = 0; i < GLOBAL_UNIT_AI_TYPES::TribeAIGroupStandardCount; i++) {
		unitLinesByUnitClass[(GLOBAL_UNIT_AI_TYPES)i] = 0;
	}

	// Get total number of available units for each class
	for (int unitClassIndex = 0; unitClassIndex < ROCKNROR::crInfo.techTreeAnalyzer.unitClassCount; unitClassIndex++) {
		TTDetailedUnitClass *unitClassInfo = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedUnitClass(unitClassIndex);
		if (!unitClassInfo || unitClassInfo->allClassUnitsNoHero.size() == 0) { continue; }

		for each (auto detail in unitClassInfo->allClassUnitsNoHero)
		{
			int curUnitDefId = detail->unitDefId;
			TTCreatorUnitInfo *crUnitInfo = this->GetCrUnitInfo(curUnitDefId);
			if (crUnitInfo && crUnitInfo->hasBeenDisabled) { continue; } // Note: crUnitInfo may be NULL for non-eligible units: e.g. clubman...
			if ((detail->baseUnitId == detail->unitDefId) && (detail->unitClass >= 0)) {
				unitLinesByUnitClass[detail->unitClass]++;
			}
		}
	}

	bool alreadyHasArcherBonus = (std::find_if(this->classesWithBonus.cbegin(), this->classesWithBonus.cend(),
		[](AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES classValue) { 
		return (classValue == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher) ||
			(classValue == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher) ||
			(classValue == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher) ||
			(classValue == AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher);
	}) != this->classesWithBonus.cend());

	std::map<GLOBAL_UNIT_AI_TYPES, BonusGenProperties> propsByUnitClass;
	// Add a class with specific "weight" and probability
	auto _addClass = [&](GLOBAL_UNIT_AI_TYPES unitClass, double weight, double proba){
		bool alreadyABonusThere = false;
		TTDetailedUnitClass *tmpUcDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedUnitClass(unitClass);
		if (tmpUcDetail && (tmpUcDetail->trainLocationIfUnique >= 0)) {
			alreadyABonusThere = std::find(this->trainLocationsWithBonus.cbegin(),
				this->trainLocationsWithBonus.cend(), tmpUcDetail->trainLocationIfUnique) != this->trainLocationsWithBonus.cend();
		}
		if (alreadyABonusThere) { proba -= 0.5; }
		if (proba < 0) { proba = 0; }
		propsByUnitClass.insert(BonusGenUCPPair(unitClass, { weight, proba}));
	};
	// propsByUnitClass.insert => Unit class, { weight, probabilityCoeff }
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher, 0.35, 0.9);
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding, 0.2, 0.95);
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot, 0.55, 0.97);
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher, 0.4, 0.9);
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian, 0.2, 1.08 );
	//_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupFishingBoat, 0.1, 0.70);// is water map
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher, 0.35, 0.99);
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider, 0.4, 0.98);
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupFootSoldier, 0.55, 1);
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher, 0.7, 1);
	// TODO: for TribeAIGroupMountedSoldier, treat unit by unit ?
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupMountedSoldier, 0.4, 1);
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupPhalanx, 0.5, 1);

	if (this->religionLevel > 3) {
		// Exclude bonus for priest for civs with bad temple. For religion levels 4-5 only.
		if (this->religionLevel == 4) {
			_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest, 0.65, 1.2); // we want a "not too high" proba, but don't forget this case is already filtered by "religion level=4"
		} else {
			// Temple bonus is likely to happen on very-good temple civs
			_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest, 0.65, 2.1); // Higher proba, to compensate the preliminary filter (only religion level=5 is allowed here)
		}
	}
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon, 0.7, 1);
	_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger, 0.2, 0.77);
	//_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupWarBoat, 0.3, 1); // is water map ?
	//_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupWall, 0.1, 1); // walls suck
	//_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupTradeBoat, 0.05, 0.7);
	//_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupTransportBoat, 0.05, 0.7);
	//_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupUnused_Tower, 0.2, 0.77); // would not work as units do not actually use this class
	// TribeAIGroupUnusedHealer => just to improve medecine ?
	//_addClass(GLOBAL_UNIT_AI_TYPES::TribeAIGroupUnused_Farm, 0.15, 0.75); // would not work as units do not actually use this class


	// TODO: modify probacoeff according to super unit availability ?
	// TODO: if possible decrease probability when there is already a bonus on a "same building" unit ?

	// Remove from list classes that already benefit from a civ bonus
	// Also remove classes with no unit (might have been disabled)
	for (auto iter = propsByUnitClass.begin(); iter != propsByUnitClass.end();) {
		GLOBAL_UNIT_AI_TYPES curClass = (*iter).first;
		auto ittmp = std::find(this->classesWithBonus.begin(), this->classesWithBonus.end(), curClass);
		bool toRemove = (ittmp != this->classesWithBonus.end()); // If already a bonus, remove
		toRemove |= (unitLinesByUnitClass[curClass] == 0); // If no impacted unit, remove too
		if (toRemove) {
			// Found: current unit class already got a bonus, exclude from "eligible" for this round
			propsByUnitClass.erase(iter++);
		} else {
			iter++;
		}
	}

	BonusGenUCPPair *bestElem = randomizer.PickRandomElementWithWeight_objectMap<GLOBAL_UNIT_AI_TYPES, BonusGenProperties>(
		propsByUnitClass, [](BonusGenUCPPair argPair){return argPair.second.probabilityCoeff; });

	GLOBAL_UNIT_AI_TYPES chosenClass = bestElem->first;
	double chosenWeight = bestElem->second.weight;
	// Remove chosen one from list
	for (auto iter = propsByUnitClass.begin(); iter != propsByUnitClass.end();) {
		if ((*iter).first == chosenClass) {
			propsByUnitClass.erase(iter++);
		} else {
			iter++;
		}
	}
	bool isRanged = (chosenClass == TribeAIGroupArcher) || (chosenClass == TribeAIGroupChariotArcher) ||
		(chosenClass == TribeAIGroupElephantArcher) || (chosenClass == TribeAIGroupHorseArcher) ||
		(chosenClass == TribeAIGroupPriest) || (chosenClass == TribeAIGroupSiegeWeapon) ||
		(chosenClass == TribeAIGroupSlinger) || (chosenClass == TribeAIGroupUnused_Tower) || (chosenClass == TribeAIGroupWarBoat); // Warning: wrong for filre galley

	bool isMelee = (chosenClass == TribeAIGroupChariot) || (chosenClass == TribeAIGroupElephantRider) ||
		(chosenClass == TribeAIGroupFootSoldier) || (chosenClass == TribeAIGroupMountedSoldier) ||
		(chosenClass == TribeAIGroupPhalanx); // true if chosen class represents military melee units

	bool isPriest = (chosenClass == TribeAIGroupPriest);
	bool isMilitary = (isRanged || isMelee || isPriest);
	

	std::map<TECH_UNIT_ATTRIBUTES, BonusGenProperties> propsByAttribute; // BonusGenProperties=weight, proba
	propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_HP, { 0.5, 1.1 }));

	if (chosenClass != TribeAIGroupCivilian) {
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_ADD_COST_AMOUNT, { 0.5, 1.1 }));
	}
	if (isMilitary) {
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_ARMOR, { 0.55, 0.9 }));
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_ATTACK, { 0.5, 1 }));
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_ATTACK_RELOAD_TIME, { 0.5, 0.85 }));
	}
	if (isRanged) {
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_RANGE, { 0.5, 1 })); // + LOS
	}
	// add TUA_INTELLIGENT_PROJECTILE ??
	// Note: avoid speed bonus on fast archers
	if ((chosenClass != TribeAIGroupBuilding) && (chosenClass != TribeAIGroupUnused_Tower) &&
		(chosenClass != TribeAIGroupUnused_Farm) && (chosenClass != TribeAIGroupWall) &&
		(chosenClass != TribeAIGroupHorseArcher) && (chosenClass != TribeAIGroupChariotArcher)) {
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_SPEED, { 0.6, 1 }));
	}
	if ((chosenClass == TribeAIGroupCivilian) || (chosenClass == TribeAIGroupFishingBoat)) {
		// Remark: for civilians, TUA_RESOURCE_CAPACITY bonus applies also on work rate.
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_RESOURCE_CAPACITY, { 0.35, 6 })); // High probability... Because resource capacity regroups several possible bonus
	}
	if (chosenClass == TribeAIGroupBuilding) {
		// Add specific cases for buildings (special bonus that will impact either farms or towers)
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_ARMOR, { 0.55, 0.88 })); // To test: but armor on towers could be a strong bonus in RM
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_ATTACK, { 0.5, 1 }));
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_ATTACK_RELOAD_TIME, { 0.5, 0.85 }));
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_RANGE, { 0.5, 1 })); // + LOS
		propsByAttribute.insert(BonusGenAttrPPair(TECH_UNIT_ATTRIBUTES::TUA_RESOURCE_CAPACITY, { 0.35, 0.92 }));
	}

	BonusGenAttrPPair *bestAttrElem = randomizer.PickRandomElementWithWeight_objectMap<TECH_UNIT_ATTRIBUTES, BonusGenProperties>(
		propsByAttribute, [](BonusGenAttrPPair argPair){return argPair.second.probabilityCoeff; });
	TECH_UNIT_ATTRIBUTES chosenAttr = bestAttrElem->first;
	double chosenAttrWeight = bestAttrElem->second.weight;
	propsByAttribute.clear();
	//const char *className = GetUnitClassName(chosenClass);
	this->classesWithBonus.insert(chosenClass);
	TTDetailedUnitClass *ucDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedUnitClass(chosenClass);
	if (ucDetail && (ucDetail->trainLocationIfUnique >= 0)) {
		this->trainLocationsWithBonus.insert(ucDetail->trainLocationIfUnique);
	}


	int minBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MIN_RATE_BASE;
	int maxBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MAX_RATE_BASE;
	if (chosenAttr == TUA_HP) {
		if ((chosenClass == TribeAIGroupBuilding) || (chosenClass == TribeAIGroupUnused_Tower) || 
			(chosenClass == TribeAIGroupUnused_Farm) || (chosenClass == TribeAIGroupWall)) {
			minBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MIN_RATE_BETTER;
			maxBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MAX_RATE_BASE;
		} else {
			if ((chosenClass == TribeAIGroupElephantArcher) || (chosenClass == TribeAIGroupElephantRider)) {
				minBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MIN_RATE_BASE;
				maxBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MAX_RATE_BASE; // no more than +25% HP for elephants, already a lot
			} else {
				minBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MIN_RATE_GOOD; // So that bonus is in [+25%, +40%]
				maxBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MAX_RATE_GOOD;
			}
		}
	}
	if (chosenAttr == TUA_SPEED) { // Increase speed: allow higher bonus (not too much for 'already fast' units though)
		if ((chosenClass == TribeAIGroupElephantArcher) ||
			(chosenClass == TribeAIGroupElephantRider) ||
			(chosenClass == TribeAIGroupPhalanx) ||
			(chosenClass == TribeAIGroupSiegeWeapon) ||
			(chosenClass == TribeAIGroupFootSoldier) ||
			(chosenClass == TribeAIGroupWarBoat)) {
			minBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MIN_RATE_BETTER;
			maxBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MAX_RATE_BETTER;
		}
	}
	if (chosenAttr == TUA_ADD_COST_AMOUNT) {
		if ((chosenClass == TribeAIGroupArcher) ||
			(chosenClass == TribeAIGroupCivilian) ||
			(chosenClass == TribeAIGroupFishingBoat) ||
			(chosenClass == TribeAIGroupFootSoldier) ||
			(chosenClass == TribeAIGroupSlinger) ||
			(chosenClass == TribeAIGroupBuilding) ||
			(chosenClass == TribeAIGroupUnused_Farm)
			) {
			// Buildings and low-cost units: limit the cost drop !
			maxBonusRate -= 5;
		} else {
			minBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MIN_RATE_BETTER;; // So that bonus is in [-15%, -25%]
		}
	}
	if ((chosenAttr == TUA_RESOURCE_CAPACITY) && (chosenClass == TribeAIGroupBuilding)) {
		minBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MIN_RATE_FARM_FOOD_AMOUNT;
		maxBonusRate = ROCKNROR::STRATEGY::TT_CONFIG::GEN_BONUS_MAX_RATE_FARM_FOOD_AMOUNT;
	}

	if (maxBonusRate < 0) { maxBonusRate = 0; }
	if (minBonusRate > maxBonusRate) { minBonusRate = maxBonusRate; }
	
	// additionalWeight is 0 in most cases. Used for very specific cases where bonus has a higher impact than expected
	double additionalWeight = this->CreateOneBonusEffect(chosenClass, chosenAttr, minBonusRate, maxBonusRate, unitLinesByUnitClass[chosenClass]);
	result = (chosenWeight + chosenAttrWeight) / 2 + additionalWeight;

	// Calculate the "average" weight so that caller can evaluate how strong the current set of bonuses is.

	double totalPossibleWeight = 0;
	double totalCount = 0;
	for (auto iter = propsByUnitClass.begin(); iter != propsByUnitClass.end(); iter++) {
		totalPossibleWeight += iter->second.weight;
		totalCount++;
	}
	for (auto iter = propsByAttribute.begin(); iter != propsByAttribute.end(); iter++) {
		totalPossibleWeight += iter->second.weight;
		totalCount++;
	}
	this->lastAverageBonusWeight = totalPossibleWeight / totalCount;

	return result;
}


// Returns the weight to be added. 0 in most cases, !=0 for special cases
double TechTreeCreator::CreateOneBonusEffect(AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES bonusUnitClass,
	TECH_UNIT_ATTRIBUTES unitAttr, int minBonusRate, int maxBonusRate, int availableAffectedUnitLines) {
	double resultAdditionalWeight = 0.;
	const char *className = GetUnitClassName(bonusUnitClass);
	bool isPriest = (bonusUnitClass == TribeAIGroupPriest);
	bool isMelee = (bonusUnitClass == TribeAIGroupChariot) || (bonusUnitClass == TribeAIGroupElephantRider) ||
		(bonusUnitClass == TribeAIGroupFootSoldier) || (bonusUnitClass == TribeAIGroupMountedSoldier) ||
		(bonusUnitClass == TribeAIGroupPhalanx); // true if chosen class represents military melee units

	// A float "random" multiplier, e.g. 1.25 for a +25% bonus
	float rndMultiplier = 1 + (((float)randomizer.GetRandomValue_normal_moderate(minBonusRate, maxBonusRate)) / 100.0f);

	// Create bonus
	AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES applyToClass = bonusUnitClass; // dest tech effect class id
	short int applyToUnit = -1; // dest tech effect unit id

	// Cases when we apply to restricted units
	/*COMMENTED because unit upgrades would NOT get bonus !!! if (availableAffectedUnitLines == 1) {
		// apply to unit specifically. Avoids having bonus on "cavalry" class when only available unit is scout !
		TTDetailedUnitDef *theUnit = this->PickOneRandomRootUnitIdAmongUnitClass(bonusUnitClass, -1);
		applyToUnit = (short int)theUnit->unitDefId;
		applyToClass = AOE_CONST_FUNC::TribeAINone;
		// Could use TTDetailedUnitDef *rootUnitDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(applyToUnit);
		// + this->AddSameEffectForUnitDefIDs(&newEffect, rootUnitDtl->possibleUpgradedUnitIDs);
	}*/

	std::string applyToNameString = "class "; // localized name of affected class/unit
	applyToNameString += className;
	if (applyToUnit >= 0) {
		applyToNameString = "unit ";
		std::string name = GetUnitName(applyToUnit);
		if (!name.empty()) {
			applyToNameString += name;
		} else {
			applyToNameString += std::to_string(applyToUnit);
		}
	}

	if ((unitAttr == TUA_HP) || (unitAttr == TUA_SPEED)) {
		AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
		newEffect.SetAttributeMultiply(unitAttr, applyToClass, applyToUnit, rndMultiplier);
		techTreeEffects.push_back(newEffect);
		this->bonusText += "Increase ";
		this->bonusText += GetTechUnitAttributeName(unitAttr);
		this->bonusText += " by ";
		this->bonusText += std::to_string((int)(rndMultiplier * 100) - 100);
		this->bonusText += "% for ";
		this->bonusText += applyToNameString.c_str();
		this->bonusText += NEWLINE;
	}

	if (unitAttr == TUA_RANGE) { // Range and line of sight
		AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
		if (bonusUnitClass == TribeAIGroupBuilding) {
			applyToUnit = CST_UNITID_WATCH_TOWER;
			applyToClass = AOE_CONST_FUNC::TribeAINone;
			applyToNameString = "towers";
		}
		newEffect.SetAttributeAdd(TUA_RANGE, applyToClass, applyToUnit, 1.0f);
		if ((bonusUnitClass == TribeAIGroupSiegeWeapon) || (bonusUnitClass == TribeAIGroupPriest) || bonusUnitClass == TribeAIGroupBuilding) {
			if (rndMultiplier > 1.175) {
				newEffect.effectValue = 2;
				resultAdditionalWeight += 0.1;
			}
		}
		techTreeEffects.push_back(newEffect);

		// Apply effect on children (upgrades)
		if (applyToUnit >= 0) {
			this->AddSameEffectForUnitLineage(&newEffect, applyToUnit);
		}

		this->bonusText += "+";
		this->bonusText += std::to_string(((int)newEffect.effectValue));
		this->bonusText += " ";
		this->bonusText += GetTechUnitAttributeName(TUA_RANGE);
		this->bonusText += " for ";
		this->bonusText += applyToNameString.c_str();
		this->bonusText += NEWLINE;

		AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect2;
		newEffect2.SetAttributeAdd(TUA_LOS, applyToClass, applyToUnit, 1);
		if ((bonusUnitClass == TribeAIGroupSiegeWeapon) || (bonusUnitClass == TribeAIGroupPriest) || (bonusUnitClass == TribeAIGroupUnused_Tower)) {
			if (rndMultiplier > 1.175) {
				newEffect.effectValue = 2;
			}
		}
		techTreeEffects.push_back(newEffect2);
	}

	if (unitAttr == TUA_ARMOR) {
		ATTACK_CLASS armorToImprove = ATTACK_CLASS::CST_AC_BASE_MELEE;
		int tmpRnd = randomizer.GetRandomNonZeroPercentageValue();
		std::string armorTypeText = "melee";
		if (bonusUnitClass == TribeAIGroupBuilding) {
			applyToClass = AOE_CONST_FUNC::TribeAINone;
			applyToUnit = CST_UNITID_WATCH_TOWER;
			applyToNameString = "towers";
			// Remark: armor on tower should be ok (not too strong) because it is applied on enemy attack
			// before dividing by 5 (cf buildings terrain defense bonus) (to confirm)
		}
		if (tmpRnd > 85) {
			armorToImprove = ATTACK_CLASS::CST_AC_BASE_PIERCE;
			armorTypeText = "pierce";
		}
		AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
		newEffect.SetAttributeAdd(unitAttr, applyToClass, applyToUnit, 0); // need to set value using SetAttackOrArmorTypeValue
		newEffect.SetAttackOrArmorTypeValue(armorToImprove, 1);
		assert(newEffect.GetAttackOrArmorType() == armorToImprove);
		assert(newEffect.GetValue() == 1);
		techTreeEffects.push_back(newEffect);

		// Apply effect on children (upgrades)
		if (applyToUnit >= 0) {
			this->AddSameEffectForUnitLineage(&newEffect, applyToUnit);
		}

		this->bonusText += "+1 ";
		this->bonusText += GetTechUnitAttributeName(unitAttr);
		this->bonusText += " (";
		this->bonusText += armorTypeText;
		this->bonusText += ") for ";
		this->bonusText += applyToNameString.c_str();
		this->bonusText += NEWLINE;
	}

	if (unitAttr == TUA_ATTACK) {
		if (isPriest) {
			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
			newEffect.SetAttributeMultiply(TUA_WORK_RATE, applyToClass, applyToUnit, rndMultiplier);
			techTreeEffects.push_back(newEffect);
			this->bonusText += "Increase conversion efficiency by ";
			this->bonusText += std::to_string((int)(rndMultiplier * 100) - 100);
			this->bonusText += "% for ";
			this->bonusText += applyToNameString.c_str();
			this->bonusText += NEWLINE;
		} else {
			int attackValue = 1;
			bool useMeleeAttack = isMelee;

			if (bonusUnitClass == TribeAIGroupSiegeWeapon) {
				TTDetailedUnitDef *pickedUnitDtl = this->PickOneRandomRootUnitIdAmongUnitClass(bonusUnitClass, -1);
				applyToUnit = (short int)pickedUnitDtl->unitDefId;
				applyToClass = AOE_CONST_FUNC::TribeAINone;
				if (applyToUnit >= 0) {
					applyToNameString = "unit ";
					std::string name = GetUnitName(applyToUnit);
					if (!name.empty()) {
						applyToNameString += name;
					} else {
						applyToNameString += std::to_string(applyToUnit);
					}
				}
				attackValue = 5; // +1 would be ridiculous for siege.

				// Just find out if attack to use is melee
				TTDetailedTrainableUnitDef * asTrainable = (TTDetailedTrainableUnitDef *)pickedUnitDtl; // can cast as here we have siege unit (trainable)
				if (asTrainable->unitDef) {
					for (int i = 0; i < asTrainable->unitDef->attacksCount; i++) { // only 1, generally
						if (asTrainable->unitDef->ptrAttacksList[i].classId == AOE_CONST_FUNC::ATTACK_CLASS::CST_AC_BASE_MELEE) {
							useMeleeAttack = true;
						}
					}
				}
			} else {
				if (bonusUnitClass == TribeAIGroupBuilding) {
					applyToClass = AOE_CONST_FUNC::TribeAINone;
					applyToUnit = CST_UNITID_WATCH_TOWER;
					applyToNameString = "towers";
				} else if (!useMeleeAttack) {
					// TODO: add weight when improving attack for non-siege range unit
				}
			}

			// Apply effect on root unit
			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
			newEffect.SetAttributeAdd(unitAttr, applyToClass, applyToUnit, 0); // need to set value using SetAttackOrArmorTypeValue
			if (useMeleeAttack) {
				newEffect.SetAttackOrArmorTypeValue(ATTACK_CLASS::CST_AC_BASE_MELEE, attackValue);
				assert(newEffect.GetAttackOrArmorType() == ATTACK_CLASS::CST_AC_BASE_MELEE);
				assert(newEffect.GetValue() == attackValue);
			} else {
				newEffect.SetAttackOrArmorTypeValue(ATTACK_CLASS::CST_AC_BASE_PIERCE, attackValue);
				assert(newEffect.GetAttackOrArmorType() == ATTACK_CLASS::CST_AC_BASE_PIERCE);
				assert(newEffect.GetValue() == attackValue);
			}
			this->techTreeEffects.push_back(newEffect);
			// Apply effect on children (upgrades)
			if (applyToUnit >= 0) {
				this->AddSameEffectForUnitLineage(&newEffect, applyToUnit);
			}

			this->bonusText += "+";
			this->bonusText += std::to_string(attackValue);
			this->bonusText += " ";
			this->bonusText += GetTechUnitAttributeName(unitAttr);
			this->bonusText += " for ";
			this->bonusText += applyToNameString.c_str();
			this->bonusText += NEWLINE;
		}
	}

	if (unitAttr == TUA_ATTACK_RELOAD_TIME) {
		if (!isPriest) {
			if (bonusUnitClass == TribeAIGroupBuilding) {
				applyToClass = AOE_CONST_FUNC::TribeAINone;
				applyToUnit = CST_UNITID_WATCH_TOWER;
				applyToNameString = "towers";
			}
			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
			float fixedMult = (2.f - rndMultiplier); // Is calculated bonus is 25% (1.25), then use (2-1.25)=0.75 factor on reload time (-25%)
			newEffect.SetAttributeMultiply(unitAttr, applyToClass, applyToUnit, fixedMult); // Diminish reload time to improve attack efficiency
			techTreeEffects.push_back(newEffect);
			// Apply effect on children (upgrades)
			if (applyToUnit >= 0) {
				this->AddSameEffectForUnitLineage(&newEffect, applyToUnit);
			}
			this->bonusText += "Increase attack speed by ";
			this->bonusText += std::to_string((int)(rndMultiplier * 100) - 100);
			this->bonusText += "% for ";
			this->bonusText += applyToNameString.c_str();
			this->bonusText += NEWLINE;
		} else {
			// Priest recharging rate is a player resource
			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
			newEffect.SetResourceMultiply(CST_RES_ORDER_FAITH_RECHARGING_RATE, rndMultiplier);
			techTreeEffects.push_back(newEffect);
			this->bonusText += "Increase priest recharging speed by ";
			this->bonusText += std::to_string((int)(rndMultiplier * 100) - 100);
			this->bonusText += "% for ";
			this->bonusText += applyToNameString.c_str();
			this->bonusText += NEWLINE;
		}
	}

	if (unitAttr == TUA_RESOURCE_CAPACITY) { // fishing ships, villager (pick one only !), farms
		bool bonusHasBeenHandled = false;
		if (bonusUnitClass == TribeAIGroupTradeBoat) {
			applyToClass = TribeAIGroupTradeBoat;
		}
		if (bonusUnitClass == TribeAIGroupTransportBoat) {
			applyToClass = TribeAIGroupTransportBoat;
		}

		if (bonusUnitClass == TribeAIGroupFishingBoat) {
			applyToClass = TribeAIGroupFishingBoat;
		}
		if (bonusUnitClass == TribeAIGroupBuilding) {
			applyToClass = AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAINone;
			//applyToUnit = CST_UNITID_FARM;
			applyToUnit = -1;
			float baseFoodAmount = 250.f;
			// Compute as a "+" bonus to display/compute it like farm food amount techs (+75 for each tech in standard game)
			int bonusAbsoluteAdd = (int)(baseFoodAmount * (rndMultiplier - 1.0f));
			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
			newEffect.SetResourceAdd(CST_RES_ORDER_FARM_FOOD_AMOUNT, (float)bonusAbsoluteAdd);
			techTreeEffects.push_back(newEffect);
			this->bonusText += "Increase farm food amount (+";
			this->bonusText += std::to_string(bonusAbsoluteAdd);
			this->bonusText += ")";
			this->bonusText += NEWLINE;
			bonusHasBeenHandled = true;
		}
		if (bonusUnitClass == TribeAIGroupCivilian) {
			int villagerRnd = randomizer.GetRandomValue(1, 6);
			switch (villagerRnd) {
			case 1:applyToUnit = CST_UNITID_MINERGOLD; break;
			case 2:applyToUnit = CST_UNITID_MINERSTONE; break;
			case 3:applyToUnit = CST_UNITID_LUMBERJACK; break;
			case 4:applyToUnit = CST_UNITID_FORAGER; break;
			case 5:applyToUnit = CST_UNITID_FARMER; break;
			case 6:applyToUnit = CST_UNITID_HUNTER; break;
			}
			if (applyToUnit >= 0) {
				applyToClass = AOE_CONST_FUNC::TribeAINone;
				applyToNameString = "unit ";
				std::string name = GetUnitName(applyToUnit);
				if (!name.empty()) {
					applyToNameString += name;
				} else {
					applyToNameString += std::to_string(applyToUnit);
				}
			}
		}

		if (!bonusHasBeenHandled && ((applyToUnit >= 0) || (applyToClass >= 0))) {
			float tmpHalf = ((maxBonusRate - minBonusRate) / 2.f) + minBonusRate;
			float carryCapacityAdd = 2;
			if (rndMultiplier > tmpHalf) {
				carryCapacityAdd = 3;
			}

			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
			newEffect.SetAttributeAdd(TUA_RESOURCE_CAPACITY, applyToClass, applyToUnit, carryCapacityAdd);
			techTreeEffects.push_back(newEffect);

			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffectWR;
			newEffectWR.SetAttributeMultiply(TUA_WORK_RATE, applyToClass, applyToUnit, rndMultiplier);
			techTreeEffects.push_back(newEffectWR);

			// Apply effect on children (upgrades)
			if (applyToUnit >= 0) {
				this->AddSameEffectForUnitLineage(&newEffect, applyToUnit);
			}

			this->bonusText += "+";
			this->bonusText += std::to_string((int)carryCapacityAdd);
			this->bonusText += " carry amount for ";
			this->bonusText += applyToNameString.c_str();
			this->bonusText += NEWLINE;
		}
	}

	if (unitAttr == TUA_ADD_COST_AMOUNT) {
		AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
		float fixedMult = (2.f - rndMultiplier); // Is calculated bonus is 25% (1.25), then use (2-1.25)=0.75 factor on costs
		newEffect.SetAttributeMultiply(unitAttr, applyToClass, applyToUnit, fixedMult);
		techTreeEffects.push_back(newEffect);
		this->bonusText += "Decrease cost by ";
		this->bonusText += std::to_string((int)(rndMultiplier * 100) - 100);
		this->bonusText += "% for class ";
		this->bonusText += className;
		this->bonusText += NEWLINE;
	}
	return resultAdditionalWeight;
}


// Choose one of the upgrade of rootUnitInfo (or rootUnitInfo) that will be the first unit from the lineage to be unavailable.
TTCreatorUnitInfo *TechTreeCreator::PickUnitUpgradeToDisableInUnitLine(TTCreatorUnitInfo *rootUnitInfo) {
	int totalUpgradesCount = rootUnitInfo->unitDetail->possibleUpgradedUnitIDs.size() + 1; // including root unit
	if (totalUpgradesCount <= 1) {
		return rootUnitInfo;
	}

	// For unit with upgrades, we need to choose which upgrade becomes unavailable.
	
	// Special : check if all units are still enabled (maybe some required research has already become disabled)
	TTCreatorUnitInfo *firstDisabledUpgrade = NULL;
	for each (long int upgradeId in rootUnitInfo->unitDetail->possibleUpgradedUnitIDs)
	{
		auto it = std::find_if(this->allCreatorUnitInfo.begin(), this->allCreatorUnitInfo.end(),
			[upgradeId](TTCreatorUnitInfo *lambdaInfo){return lambdaInfo->unitDefId == upgradeId; }
		);
		if (it == this->allCreatorUnitInfo.end()) {
			// Not found
			continue;
		}
		TTCreatorUnitInfo *unitUpgradeInfo = *it;
		if (!unitUpgradeInfo || !unitUpgradeInfo->unitDetail->IsValid()) { continue; }
		if (unitUpgradeInfo->hasBeenDisabled) {
			if (!firstDisabledUpgrade) {
				firstDisabledUpgrade = unitUpgradeInfo;
			} else {
				// Keep the "earliest" disabled upgrade
				long int currentResultId = firstDisabledUpgrade->unitDefId;
				auto it = std::find_if(unitUpgradeInfo->unitDetail->possibleAncestorUnitIDs.begin(),
					unitUpgradeInfo->unitDetail->possibleAncestorUnitIDs.end(),
					[currentResultId](long int ancestorId){ return ancestorId == currentResultId; }
				);
				if (it == unitUpgradeInfo->unitDetail->possibleAncestorUnitIDs.end()) {
					// Not found: firstDisabledUpgrade is NOT an ancestor of unitUpgradeInfo => unitUpgradeInfo is the ancestor
					firstDisabledUpgrade = unitUpgradeInfo;
				}
			}
		}
	}
	if (firstDisabledUpgrade == rootUnitInfo) {
		return firstDisabledUpgrade; // No need to go further...
	}

	// Get the list of upgrades we still CAN disabled (not children of already disabled ones)
	std::list<TTCreatorUnitInfo*>unitInfoThatCanBeDisabled;
	if (firstDisabledUpgrade) {
		unitInfoThatCanBeDisabled.push_back(firstDisabledUpgrade);
	}
	for each (long int curId in rootUnitInfo->unitDetail->possibleUpgradedUnitIDs)
	{
		TTCreatorUnitInfo *curElem = this->GetCrUnitInfo(curId);
		if (!curElem) { continue; }

		if (firstDisabledUpgrade) {
			long int firstDisabledId = firstDisabledUpgrade->unitDefId;
			auto it2 = std::find_if(curElem->unitDetail->possibleAncestorUnitIDs.begin(),
				curElem->unitDetail->possibleAncestorUnitIDs.end(),
				[firstDisabledId](long int ancestorId){ return ancestorId == firstDisabledId; }
			);
			if (it2 == curElem->unitDetail->possibleAncestorUnitIDs.end()) {
				// Not found: firstDisabledUpgrade is NOT an ancestor of curElem => curElem is parent => eligible for "can be disabled" list
				if (curElem != firstDisabledUpgrade) {
					unitInfoThatCanBeDisabled.push_back(curElem);
				}
			}
		} else {
			unitInfoThatCanBeDisabled.push_back(curElem); // when all units are still available, they are all eligible for "can be disabled" list
		}
	}

	// Handle special cases described by "disable policy"
	bool protectBaseUnit = (rootUnitInfo->rootUnitDisablePolicy == TTCreatorBaseUnitDisableBehavior::TTBUDisableNever);
	if (rootUnitInfo->rootUnitDisablePolicy == TTCreatorBaseUnitDisableBehavior::TTBUDisableVeryRare) {
		if (randomizer.GetRandomNonZeroPercentageValue() >= 10) {
			protectBaseUnit = true;
		}
	}
	// Root unit has not been added to list yet. Do it now unless "protectBaseUnit" is set (in such case, root unit won't be eligible to be disabled).
	if (!protectBaseUnit) {
		unitInfoThatCanBeDisabled.push_back(rootUnitInfo);
	}

	if (rootUnitInfo->rootUnitDisablePolicy == TTCreatorBaseUnitDisableBehavior::TTBUDisableMostlyExclusiveSameClassIron) {
#pragma WARNING("TTBUDisableMostlyExclusiveSameClassIron not implemented")
	}

	// TODO: for long lineage >=3 (legion, cats, cavalry...), do not disable "only" last one when required researches are ok ?
	// Note: Even when only 2 "upgrade levels", almost all "last unit upgrades" depend on an "external" research, except composite bowman (who's NOT a super unit...), trireme. Oh, and axeman ;)
	int childrenCount = rootUnitInfo->unitDetail->possibleUpgradedUnitIDs.size();
	const int minNumberOfUpgradeLevelsForLastUnitRequirementCheck = 3; // counting root unit as an upgrade.
	if (childrenCount >= minNumberOfUpgradeLevelsForLastUnitRequirementCheck) {
		long int myLocation = rootUnitInfo->unitDetail->GetUnitDef()->trainLocation;
		// Find last upgrade
		int securityCounter = childrenCount;
		long int curChildId = *rootUnitInfo->unitDetail->possibleUpgradedUnitIDs.begin();
		TTCreatorUnitInfo *lastUpgradedUnitInfo = this->GetCrUnitInfo(curChildId);
		while (lastUpgradedUnitInfo && (securityCounter > 0)) {
			securityCounter--;
			if (lastUpgradedUnitInfo && (lastUpgradedUnitInfo->unitDetail->possibleUpgradedUnitIDs.size() > 0)) {
				curChildId = *lastUpgradedUnitInfo->unitDetail->possibleUpgradedUnitIDs.begin();
				lastUpgradedUnitInfo = this->GetCrUnitInfo(curChildId);
			}
		}
		if (lastUpgradedUnitInfo) {
			bool isIronAgeUpgrade = (lastUpgradedUnitInfo->unitDetail->requiredAge == AOE_CONST_FUNC::CST_RSID_IRON_AGE);
			long int enablerResId = *lastUpgradedUnitInfo->unitDetail->researchIdsThatEnableMe.begin();
			auto lastUpgradeResearchInfo = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(enablerResId);
			if (lastUpgradeResearchInfo && lastUpgradeResearchInfo->active && isIronAgeUpgrade) { // only for iron-age upgrades
				bool hasRequirementFromOtherBuilding = false;
				for each (auto req in lastUpgradeResearchInfo->directRequirements)  {
					if (!req->active) { continue; }
					if (req->IsAgeResearch()) { continue; } // Ignore age requirement
					if (req->IsWheel()) { continue; } // heavy chariot can be disabled while chariot is available
					if ((myLocation != req->researchDef->researchLocation) && (req->researchDef->researchLocation >= 0)) {
						hasRequirementFromOtherBuilding = true;
						break;
					}
				}
				if (hasRequirementFromOtherBuilding) {
					// Do not allow the last upgrade to be the *first* one being NON-available
					auto it = std::find(unitInfoThatCanBeDisabled.begin(), unitInfoThatCanBeDisabled.end(), lastUpgradedUnitInfo);
					if (it != unitInfoThatCanBeDisabled.end()) {
						unitInfoThatCanBeDisabled.erase(it++);
					}
				}
			}
		}

	}

	TTCreatorUnitInfo *chosen = randomizer.PickRandomElementWithWeight_ptrCollection<std::list, TTCreatorUnitInfo>(
		unitInfoThatCanBeDisabled, [](TTCreatorUnitInfo *u) { return u->rawDisableProbability; } );
	if (!chosen) {
		chosen = rootUnitInfo;
	}
	return chosen;
}


// Disable (cascade) child researches for parentResearch
// Returns the *child* disabled researches (not the parent research itself)
std::list<TTCreatorResearchInfo*> TechTreeCreator::CreateDisableEffectOnResearch(TTCreatorResearchInfo *resInfo) {
	// Disable myself
	STRUCT_TECH_DEF_EFFECT newEffect;
	newEffect.effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_DISABLE_RESEARCH;
	newEffect.effectAttribute = 0;
	newEffect.effectClass = 0;
	newEffect.effectUnit = 0;
	newEffect.effectValue = (float)resInfo->researchId;
	techTreeEffects.push_back(newEffect);
	resInfo->hasBeenDisabled = true;

	// Does provided research enable units (should not for allCreatorResearchInfo reseearches, as we filter out such researches)
	for each (TTDetailedTrainableUnitDef *trainableDetail in resInfo->researchDetail->enableTrainables) {
		assert(trainableDetail->IsValid());
		this->CreateDisableEffectOnUnit(trainableDetail);
	}

	std::list<TTCreatorResearchInfo*> result;
	// Handle children researches (from provided research) that get impossible (includes indirect children)
	for each (TTDetailedResearchDef *childDetail in resInfo->researchDetail->allChildResearches)
	{
		long int childDetailResId = childDetail->GetResearchDefId();
		if ((childDetailResId >= AOE_CONST_FUNC::CST_RSID_STONE_AGE) && (childDetailResId <= AOE_CONST_FUNC::CST_RSID_IRON_AGE)) {
			continue; // don't mess with ages
		}
		TTCreatorResearchInfo *child = NULL;
		auto it = std::find_if(this->allCreatorResearchInfo.begin(), this->allCreatorResearchInfo.end(), 
			[childDetail](TTCreatorResearchInfo *lambdaInfo){ return lambdaInfo->researchId == childDetail->GetResearchDefId(); }
			);
		if (it == this->allCreatorResearchInfo.end()) {
			// Not found: either the *child* research is a "unit disable" tech or it is a research we do NOT want to disable
			for each (TTDetailedTrainableUnitDef *enTrainable in childDetail->enableTrainables)
			{
				std::list<TTCreatorResearchInfo*> tmpRes = this->CreateDisableEffectOnUnit(enTrainable); // Can happen, for example legion (when fanaticism is disabled), chariot(wheel)...
				for each (auto x in tmpRes)
				{
					result.push_back(x);
				}
			}
		} else {
			child = *it; // we know this research
		}
		if (child && !child->hasBeenDisabled && !childDetail->IsShadowResearch()) {
			// This is done for all children
			STRUCT_TECH_DEF_EFFECT newEffect;
			newEffect.effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_DISABLE_RESEARCH;
			newEffect.effectAttribute = 0;
			newEffect.effectClass = 0;
			newEffect.effectUnit = 0;
			newEffect.effectValue = (float)child->researchId;
			techTreeEffects.push_back(newEffect);
			child->hasBeenDisabled = true;
			result.push_back(child);

			for each (TTDetailedTrainableUnitDef *trainableDetail in child->researchDetail->enableTrainables) {
				assert(trainableDetail->IsValid());
				// Note: unit's parent research is "child" and we just marked it as disabled, so this call won't disable any research recursively.
				this->CreateDisableEffectOnUnit(trainableDetail); // can happen, for example ..?
			}
		}
	}
	return result;
}


// Returns the list of "recursive" TTCreatorResearchInfo that have been disabled, including the research that enables unitInfo.
std::list<TTCreatorResearchInfo*> TechTreeCreator::CreateDisableEffectOnUnit(TTCreatorUnitInfo *unitInfo) {
	std::list<TTCreatorResearchInfo*> result;
	if (!unitInfo) { return result; }
	if (unitInfo->hasBeenDisabled) { return result; } // already done
	unitInfo->hasBeenDisabled = true;

	// Make sure to disable child units (upgrades)
	for each (long int subId in unitInfo->unitDetail->possibleUpgradedUnitIDs) {
		auto it = std::find_if(this->allCreatorUnitInfo.begin(), this->allCreatorUnitInfo.end(),
			[subId](TTCreatorUnitInfo *lambdaInfo){return lambdaInfo->unitDefId == subId; }
		);
		if (it != this->allCreatorUnitInfo.end()) {
			// Found
			for each (auto x in this->CreateDisableEffectOnUnit(*it)) {
				result.push_back(x);
			}
		}
	}

	long int sourceResearchId = unitInfo->sourceResearchId;
	TTCreatorResearchInfo *srcResInfo = NULL;

	// Is the research that enables the unit known in allCreatorResearchInfo ?
	auto it = std::find_if(this->allCreatorResearchInfo.begin(), this->allCreatorResearchInfo.end(),
		[sourceResearchId](TTCreatorResearchInfo *lambdaInfo){return lambdaInfo->researchId == sourceResearchId; }
	);
	if (it != this->allCreatorResearchInfo.end()) {
		srcResInfo = *it;
	} else {
		// Create a fake one
		srcResInfo = new TTCreatorResearchInfo();
		srcResInfo->researchId = sourceResearchId;
		srcResInfo->researchDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(sourceResearchId);
		if (srcResInfo->researchDetail) {
			srcResInfo->internalName = srcResInfo->researchDetail->internalName;
			srcResInfo->langName = srcResInfo->researchDetail->langName;
		}
		srcResInfo->isFakeForDisableUnit = true;
		// newInfo->researchDetail should always be valid: all IDs from this->eligible* collections should be valid
		this->allCreatorResearchInfo.push_back(srcResInfo);
		srcResInfo->rawDisableProbability = 0;
		srcResInfo->disableWeight = 0;
	}
	if (srcResInfo && !srcResInfo->hasBeenDisabled) {
		result.push_back(srcResInfo);
		std::list<TTCreatorResearchInfo*> recurList = this->CreateDisableEffectOnResearch(srcResInfo);
		for each (TTCreatorResearchInfo *elem in recurList)
		{
			result.push_back(elem);
		}
	}

	return result;
}


// Returns the list of "recursive" TTCreatorResearchInfo that have been disabled (does not include unitInfo)
std::list<TTCreatorResearchInfo*> TechTreeCreator::CreateDisableEffectOnUnit(TTDetailedUnitDef *unitDetail) {
	std::list<TTCreatorResearchInfo*> result;
	if (!unitDetail) { return result; }
	long int unitDefId = unitDetail->unitDefId;
	auto it = std::find_if(this->allCreatorUnitInfo.begin(), this->allCreatorUnitInfo.end(),
		[unitDefId](TTCreatorUnitInfo *lambdaInfo){return lambdaInfo->unitDefId == unitDefId; }
	);
	if (it != this->allCreatorUnitInfo.end()) {
		// Found
		TTCreatorUnitInfo *unitElem = *it;
		return this->CreateDisableEffectOnUnit(unitElem);
	} else {
		assert(false && "non-allowed unit ?");
	}
	return result;
}


// Copy (add) effects from internal collection (techTreeEffects) to actual tech tree object (techDef)
void TechTreeCreator::AddEffectsToTechDef() {
	int effectCount = this->techTreeEffects.size();
	if (effectCount <= 0) { return; }
	this->techDefForTechTree->effectCount = effectCount;
	if (this->techDefForTechTree->ptrEffects) {
		AOEFree(this->techDefForTechTree->ptrEffects);
	}
	this->techDefForTechTree->ptrEffects = (STRUCT_TECH_DEF_EFFECT*)AOEAllocZeroMemory(effectCount, sizeof(STRUCT_TECH_DEF_EFFECT));

	int curIndex = 0;
	for each(STRUCT_TECH_DEF_EFFECT effect in this->techTreeEffects) {
		this->techDefForTechTree->ptrEffects[curIndex].effectType = effect.effectType;
		this->techDefForTechTree->ptrEffects[curIndex].effectAttribute = effect.effectAttribute;
		this->techDefForTechTree->ptrEffects[curIndex].effectClass = effect.effectClass;
		this->techDefForTechTree->ptrEffects[curIndex].effectUnit = effect.effectUnit;
		this->techDefForTechTree->ptrEffects[curIndex].effectValue = effect.effectValue;
		curIndex++;
	}
}


// Add an effect into internal collection (techTreeEffects) for EACH unitDefId provided in the parameter collection
// Useful to copy an effect from root unit to its upgrades
void TechTreeCreator::AddSameEffectForUnitDefIDs(AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT *srcEffect, const std::list<long int> &unitDefIDs) {
	if (!srcEffect) { return; }
	// Create a copy of source effect. Note that push_back will copy it in each loop (we're NOT passing pointers)
	AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT curEffect;
	curEffect.effectAttribute = srcEffect->effectAttribute;
	curEffect.effectClass = srcEffect->effectClass;
	curEffect.effectType = srcEffect->effectType;
	curEffect.effectValue = srcEffect->effectValue;
	for each (long int curUnitDefId in unitDefIDs)
	{
		curEffect.effectUnit = (short int) curUnitDefId; // The only thing that changes is unitDefId it applies to
		this->techTreeEffects.push_back(curEffect);
	}
}


// Add an effect into internal collection (techTreeEffects) for EACH unitDefId that belongs to same unit lineage
// Useful to copy an effect from root unit to its upgrades
// Unit can be either a "trainable" or a building
void TechTreeCreator::AddSameEffectForUnitLineage(AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT *srcEffect, short int unitDefId) {
	TTDetailedUnitDef *rootUnitDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(unitDefId);
	if (rootUnitDtl != nullptr) {
		this->AddSameEffectForUnitDefIDs(srcEffect, rootUnitDtl->possibleUpgradedUnitIDs);
		return;
	}
	TTDetailedBuildingDef *rootBldDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(unitDefId);
	if (rootBldDtl != nullptr) {
		this->AddSameEffectForUnitDefIDs(srcEffect, rootBldDtl->possibleUpgradedUnitIDs);
		return;
	}
}


// Randomly selects 1 non-disabled root unit in provided unit class. -1 is not found
// Use minimumRequiredAgeResearchId to filter on units >= provided age. E.g. CST_RSID_TOOL_AGE
TTDetailedUnitDef *TechTreeCreator::PickOneRandomRootUnitIdAmongUnitClass(GLOBAL_UNIT_AI_TYPES unitClass, int minimumRequiredAgeResearchId) {
	if (unitClass < 0) { return NULL; }
	TTDetailedUnitClass *ucDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedUnitClass(unitClass);
	if (!ucDetail || !ucDetail->IsValidClass()) { return NULL; }
	std::list<TTDetailedUnitDef*> eligibles;
	for each (auto x in ucDetail->rootUnits)
	{
		if (x->requiredAge < minimumRequiredAgeResearchId) { continue; }
		TTCreatorUnitInfo *crUnitInfo = this->GetCrUnitInfo(x->unitDefId);
		if (crUnitInfo && crUnitInfo->hasBeenDisabled) { continue; }
		eligibles.push_back(x);
	}
	TTDetailedUnitDef *result = NULL;
	int curBestScore = -1;
	for each (auto x in eligibles)
	{
		int curRnd = randomizer.GetRandomValue(1, 100000);
		if (curRnd > curBestScore) {
			curBestScore = curRnd;
			result = x;
		}
	}
	return result;
}



std::string TechTreeCreator::GetDisabledUnitsText() const {
	std::list<TTCreatorUnitInfo*> disabledUnits;
	auto lambdaIsDisabled = [](TTCreatorUnitInfo *lambdaInfo){return lambdaInfo->hasBeenDisabled; };
	auto it = std::find_if(this->allCreatorUnitInfo.begin(), this->allCreatorUnitInfo.end(), lambdaIsDisabled);
	while (it != allCreatorUnitInfo.end()) {
		TTCreatorUnitInfo *curUnit = *it;
		disabledUnits.push_back(curUnit);
		it = std::find_if(++it, this->allCreatorUnitInfo.end(), lambdaIsDisabled);
	}
	
	std::string result = "";
	disabledUnits.sort([](TTCreatorUnitInfo *elem, TTCreatorUnitInfo *elem2) {
		return (*elem->unitDetail < *elem2->unitDetail);
	});

	int curTrainLocation = -1;
	bool isFirst = true;
	for (auto it2 = disabledUnits.begin(); it2 != disabledUnits.end(); it2++) {
		TTCreatorUnitInfo *curElem = *it2;
		if (curElem->trainLocation != curTrainLocation) {
			if (!result.empty()) {
				result.append(NEWLINE);
			}
			result += "[";
			curTrainLocation = curElem->trainLocation;
			TTDetailedBuildingDef *locationDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(curTrainLocation);
			if (locationDtl && locationDtl->IsValid()) {
				result += locationDtl->langName;
			} else {
				result += "Unknown";
			}
			result += "] => ";
			isFirst = true;
		}
		if (!isFirst) {
			result += ", ";
		}
		isFirst = false;
		result += curElem->langName;
	}
	result += NEWLINE;
	return result;
}

std::string TechTreeCreator::GetDisabledResearchesText() const {
	std::string result = "";
	std::list<TTCreatorResearchInfo*> disabledResearches;
	auto lambdaIsDisabled = [](TTCreatorResearchInfo *lambdaInfo){
		return lambdaInfo->hasBeenDisabled && !lambdaInfo->isFakeForDisableUnit && !lambdaInfo->researchDetail->IsShadowResearch();
	};
	auto it = std::find_if(this->allCreatorResearchInfo.begin(), this->allCreatorResearchInfo.end(), lambdaIsDisabled);
	while (it != allCreatorResearchInfo.end()) {
		TTCreatorResearchInfo *curResearch = *it;
		disabledResearches.push_back(curResearch);
		it = std::find_if(++it, this->allCreatorResearchInfo.end(), lambdaIsDisabled);
	}

	disabledResearches.sort([](TTCreatorResearchInfo *elem, TTCreatorResearchInfo *elem2) {
		if (elem->researchLocation != elem2->researchLocation) { return elem->researchLocation < elem2->researchLocation; }
		if (elem->researchDetail->researchDef->buttonId != elem2->researchDetail->researchDef->buttonId) { return elem->researchDetail->researchDef->buttonId < elem2->researchDetail->researchDef->buttonId; }
		if (elem->researchDetail->requiredAge != elem2->researchDetail->requiredAge) { return elem->researchDetail->requiredAge < elem2->researchDetail->requiredAge; }
		if (elem->researchDetail->allRequirementsExcludingAges.size() != elem2->researchDetail->allRequirementsExcludingAges.size()) { return elem->researchDetail->allRequirementsExcludingAges.size() < elem2->researchDetail->allRequirementsExcludingAges.size(); }
		return elem->researchId < elem2->researchId;
	});

	int curResLocation = -1;
	bool isFirst = true;
	for (auto it2 = disabledResearches.begin(); it2 != disabledResearches.end(); it2++) {
		TTCreatorResearchInfo *curElem = *it2;
		if (curElem->researchLocation != curResLocation) {
			if (!result.empty()) {
				result.append(NEWLINE);
			}
			result += "[";
			curResLocation = curElem->researchLocation;
			TTDetailedBuildingDef *locationDtl = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedBuildingDef(curResLocation);
			if (locationDtl && locationDtl->IsValid()) {
				result += locationDtl->langName;
			} else {
				result += "Unknown";
			}
			result += "] => ";
			isFirst = true;
		}
		if (!isFirst) {
			result += ", ";
		}
		isFirst = false;
		result += curElem->langName;
	}
#ifdef _DEBUG
	result += NEWLINE "Religion level=";
	result += std::to_string(this->religionLevel);
#endif
	result += NEWLINE;

	return result;
}

std::string TechTreeCreator::GetCivBonusText() const {
	std::string result = "";
#ifdef _DEBUG
	result += "Tech tree totalDisableWeight=";
	result += std::to_string(this->totalDisableWeight);
	result += "   /   disabledWeightProportion=";
	result += std::to_string(this->disabledWeightProportion);
	result += "   /   bonusProportion=";
	result += std::to_string(this->bonusProportion);
	result += "   /   bonusCount=";
	result += std::to_string(this->bonusCount);
	result += NEWLINE;
#endif
	
	result += this->bonusText;
	//result += NEWLINE;
	return result;
}


std::list<std::string> TechTreeCreator::GetHumanBonusTextLines() const {
	std::list<std::string> result;
	std::string curLine = "";
	for (auto it = this->bonusText.begin(); it != this->bonusText.end(); it++) {
		char c = *it;
		if (c == '\n') {
			if (!curLine.empty()) {
				result.push_back(curLine);
			}
			curLine = "";
		} else {
			curLine += c;
		}
	}
	return result;
}


}
}
