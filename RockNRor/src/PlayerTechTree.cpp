#include "../include/PlayerTechTree.h"

// \r is required to display correctly in ROR editable text components
#define NEWLINE "\r\n"

namespace ROCKNROR {
namespace STRATEGY {
;


// Fill eligibleDisableXxx collections according to player's tech tree (myTechTreeId).
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
	
	// DISABLE RESEARCH (may cascade on other researches + units)
	this->CalcResearchesDisableScore();
	this->CreateDisableResearchesEffects();

	// DISABLE UNITS/upgrades (via research)
	this->CalcUnitsDisableScore();
	this->CreateDisableUnitsEffects();

	// CIV BONUS
	this->CalcRandomCivBonus();

	this->AddEffectsToTechDef();

	// TODO: protect at least 1 (2?) super unit in tech tree
	// Ex: 65 techs bronze/iron
	// range disable : 15-26
	// egypt 16 disable unit/upgrade, 5 disable tech
	// greek 15 disable unit/upgrade, 5 disable tech
	// assy 13 disable unit/upgrade, 11 disable tech
	// bab 13 disable unit/upgrade, 6 disable tech
	// yama 14 disable unit/upgrade, 8 disable tech mainly temple + towers/wall not counted
	// rome 11 disable unit/upgrade, 4 disable tech ! (2 temple, 1 market 1 gov)
	// palmy 9 disable unit/upgrade, 13 disable tech (bad temple, market...)
	// carth 11 disable unit/upgrade, 8 disable tech (temple, sp...)
	// cho 17 disable unit/upgrade, 9 disable tech (sp, ov, dock...)

	// Do not allow disable building at this point (except iron age towers)
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
	bronzeIronResearchLinesCount -= 3; // Exclude bronze/iron age, wheel TODO should be more geenric
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

		if ((crResInfo->disableProbability < 0) && (armorClass == ATTACK_CLASS::CST_AC_BASE_PIERCE)) {
			if (crResInfo->researchDetail->researchLinePredecessors.size() == 0) {
				crResInfo->disableProbability = TT_CONFIG::RES_PROBA_STANDARD_RESEARCH;
				crResInfo->disableWeight = TT_CONFIG::RES_WEIGHT_STANDARD_RESEARCH;
			} else {
				crResInfo->disableProbability = TT_CONFIG::RES_PROBA_SPECIALIZED_RESEARCH;
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
		if (crResInfo->techImpactsReligion && (crResInfo->disableProbability < 0)) {
			// TODO: higher proba for jihad, martyrdom ?
			crResInfo->disableProbability = religionProba;
			crResInfo->disableWeight = weightForReligion;
			if (crResInfo->researchDetail->requiredAge < AOE_CONST_FUNC::CST_RSID_IRON_AGE) {
				crResInfo->disableWeight = crResInfo->disableWeight * religionBronzeFactor;
				crResInfo->disableProbability = crResInfo->disableProbability * (2 - religionBronzeFactor);
			}
			if (crResInfo->disableWeight > 1) { crResInfo->disableWeight = 1; }
			if (crResInfo->disableProbability > 1) { crResInfo->disableProbability = 1; }
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
		if ((crResInfo->disableProbability < 0) && isMarketOrGov) {
			// Protect key techs like wheel, architecture, woodworking+range => reduce their probabilities
			if (crResInfo->researchDetail->IsWheel()) {
				crResInfo->disableProbability = TT_CONFIG::RES_PROBA_WHEEL;
				crResInfo->disableWeight = TT_CONFIG::RES_WEIGHT_WHEEL;
				continue;
			}
			if (crResInfo->researchDetail->IsArchitecture()) {
				crResInfo->disableProbability = TT_CONFIG::RES_PROBA_COMMON_USEFUL_RESEARCH;
				crResInfo->disableWeight = TT_CONFIG::RES_WEIGHT_COMMON_USEFUL_RESEARCH;
				continue;
			}
			if (crResInfo->researchDetail->IsWoodWorkingAndRange()) {
				crResInfo->disableProbability = TT_CONFIG::RES_PROBA_COMMON_USEFUL_RESEARCH;
				crResInfo->disableWeight = TT_CONFIG::RES_WEIGHT_COMMON_USEFUL_RESEARCH;
				continue;
			}

			crResInfo->disableProbability = randomProbaForEconomy;
			crResInfo->disableWeight = weightForEconomy;
			if (crResInfo->researchDetail->requiredAge < AOE_CONST_FUNC::CST_RSID_IRON_AGE) {
				crResInfo->disableWeight = crResInfo->disableWeight * economyBronzeFactor;
				crResInfo->disableProbability = crResInfo->disableProbability * (2 - economyBronzeFactor);
			}
			if (crResInfo->disableWeight > 1) { crResInfo->disableWeight = 1; }
			if (crResInfo->disableProbability > 1) { crResInfo->disableProbability = 1; }
		}
	}

	// Set probability for remaining researches
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo)
	{
		if (crResInfo->disableProbability < 0) {
			crResInfo->disableProbability = TT_CONFIG::RES_PROBA_STANDARD_RESEARCH;
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
				crUnitInfo->disableProbability = TT_CONFIG::RES_PROBA_MIN;
				crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_STANDARD_UNIT;
			} else {
				crUnitInfo->rootUnitDisablePolicy = TTCreatorBaseUnitDisableBehavior::TTBUDisableVeryRare;
				crUnitInfo->disableProbability = TT_CONFIG::RES_PROBA_VERY_RARE;
				crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_HIGH_IMPACT_UNIT;
			}
			continue;
		}
		// Special: iron age "root" boats (note: pre-iron war boats can't be disabled, cf CollectUnitInfo) => fire galley, cat.trireme
		if ((crUnitInfo->unitDetail->unitClass == AOE_CONST_FUNC::TribeAIGroupWarBoat) &&
			(crUnitInfo->unitDetail->requiredAge == AOE_CONST_FUNC::CST_RSID_IRON_AGE)) {
			crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_STANDARD_UNIT;
			crUnitInfo->disableProbability = TT_CONFIG::RES_PROBA_STANDARD_UNIT; // Useful for other upgrades (catapult trireme)
			// Will force to choose among iron-age root units of same class (warboats here)
			crUnitInfo->rootUnitDisablePolicy = TTCreatorBaseUnitDisableBehavior::TTBUDisableMostlyExclusiveSameClassIron;
			continue;
		}

		// Special: stone thrower and siege
		if (crUnitInfo->unitDetail->unitClass == AOE_CONST_FUNC::TribeAIGroupSiegeWeapon) {
			crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_HIGH_IMPACT_UNIT;
			if (crUnitInfo->unitDefId == AOE_CONST_FUNC::CST_UNITID_STONE_THROWER) {
				crUnitInfo->rootUnitDisablePolicy = TTCreatorBaseUnitDisableBehavior::TTBUDisableVeryRare;
				crUnitInfo->disableProbability = TT_CONFIG::RES_PROBA_STANDARD_UNIT; // Useful for other upgrades (catapults)
			} else {
				crUnitInfo->disableProbability = TT_CONFIG::RES_PROBA_SPECIALIZED_UNIT;
			}
		}

		// Special: unit that is only unit in given train location (may depend on mods, in standard game: hoplite)
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
				crUnitInfo->disableProbability = TT_CONFIG::RES_PROBA_SPECIALIZED_UNIT;
				crUnitInfo->rootUnitDisablePolicy = TTCreatorBaseUnitDisableBehavior::TTBUDisableVeryRare;
				crUnitInfo->disableWeight = TT_CONFIG::RES_WEIGHT_HIGH_IMPACT_UNIT;
			}
		}
	}

	// Set probability for remaining units
	for each (TTCreatorUnitInfo *crUnitInfo in this->allCreatorUnitInfo)
	{
		if (crUnitInfo->disableProbability < 0) {
			crUnitInfo->disableProbability = TT_CONFIG::RES_PROBA_STANDARD_UNIT;
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
		crResInfo->disableWeight *= (1 + directEnableTrainableCount*0.05); // Add 10% weight per dependency

		int indirectEnableTrainableCount = 0;
		std::for_each(crResInfo->researchDetail->allChildResearches.begin(),
			crResInfo->researchDetail->allChildResearches.end(),
			[&indirectEnableTrainableCount](TTDetailedResearchDef *childDetail){
			indirectEnableTrainableCount += childDetail->enableTrainables.size();
		}
		);
		crResInfo->disableWeight *= (1 + directEnableTrainableCount*0.02); // Add 5% weight per indirect dependency

		// Effects on unit (attributes, etc). Add weight if many units (take care of unit classes !)
		// TODO
#pragma message("TODO: effects on units : update weight")
	}
}


// Use each research's probability to compute its "disable score"
void TechTreeCreator::CalcResearchesDisableScore() {
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo) {
		int imyrand = randomizer.GetRandomNonZeroPercentageValue();
		double myrand = (double)imyrand;
		
		double tmpCompute = crResInfo->disableProbability * myrand * TT_CONFIG::RES_PROBA_IMPACT_ON_RANDOM +
			myrand * (1 - TT_CONFIG::RES_PROBA_IMPACT_ON_RANDOM); // Now tmpCompute is in 0-100

		crResInfo->disableScore = tmpCompute;
	}
}


// Use each *root* unit's probability to compute its "disable score".
void TechTreeCreator::CalcUnitsDisableScore() {
	for each (TTCreatorUnitInfo *crUnitInfo in this->allCreatorUnitInfo) {
		if (!crUnitInfo->unitDetail->IsValid()) { continue; }
		if (crUnitInfo->unitDefId != crUnitInfo->unitDetail->baseUnitId) { continue; } // Only treat base units => 1 evaluation for each unitline

		// Random will decide if the unitline should be disabled (partially or fully)
		int imyrand = randomizer.GetRandomNonZeroPercentageValue();
		double myrand = (double)imyrand;

		double tmpCompute = crUnitInfo->disableProbability * myrand * TT_CONFIG::RES_PROBA_IMPACT_ON_RANDOM +
			myrand * (1 - TT_CONFIG::RES_PROBA_IMPACT_ON_RANDOM); // Now tmpCompute is in 0-100

		crUnitInfo->disableScore = tmpCompute;
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
		TTCreatorResearchInfo *bestElem = NULL;
		std::for_each(this->allCreatorResearchInfo.begin(), this->allCreatorResearchInfo.end(),
			[&bestElem, &bestScore](TTCreatorResearchInfo *curInfo) {
			if (!curInfo->hasBeenDisabled && (curInfo->disableScore > bestScore)) { bestScore = curInfo->disableScore; bestElem = curInfo; }
		}
		);
		if (bestScore < 0) { break; }

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
			if (child->researchLocation == AOE_CONST_FUNC::CST_UNITID_TEMPLE) {
				if (templeResearchesDoNotCountMoreThan > 0) {
					templeResearchesDoNotCountMoreThan--;
				} else {
					addedCount--; // compensate: do not count this researches (too many temple researches have been disabled, do not count them anymore)
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
		std::for_each(this->allCreatorUnitInfo.begin(), this->allCreatorUnitInfo.end(),
			[&rootUnitToDisable, &bestScore](TTCreatorUnitInfo *curInfo) {
			if (!curInfo->hasADisabledChild && (curInfo->disableScore > bestScore)) { bestScore = curInfo->disableScore; rootUnitToDisable = curInfo; }
		}
		);
		if (bestScore < 0) { break; }

		// Here we have a unit to disable
		TTCreatorUnitInfo *unitToActuallyDisable = this->PickUnitUpgradeToDisableInUnitLine(rootUnitToDisable);

		std::list<TTCreatorResearchInfo*> disabledChildren = this->CreateDisableEffectOnUnit(unitToActuallyDisable);
		unitToActuallyDisable->hasBeenDisabledDirectly = true;
		rootUnitToDisable->hasADisabledChild = true;
		int addedCount = disabledChildren.size(); // disabledChildren includes the researchInfo for unitToActuallyDisable itself.

		curDisableCount += addedCount;
	}
}


// Creates random civ bonuses
void TechTreeCreator::CalcRandomCivBonus() {
	this->totalDisableWeight = 0;
	double totalWeightAll = 0; // For ALL units and ALL researches, even not-disabled ones (ignore shadow and "fake" researches though)

	for each (TTCreatorUnitInfo *curUnit in this->allCreatorUnitInfo)
	{
		totalWeightAll += curUnit->disableWeight;
		if (curUnit->hasBeenDisabled) {
			this->totalDisableWeight += curUnit->disableWeight;
		}
	}

	for each (TTCreatorResearchInfo *curResearch in this->allCreatorResearchInfo)
	{
		if (curResearch->researchDetail->IsShadowResearch() || curResearch->isFakeForDisableUnit) { continue; }
		totalWeightAll += curResearch->disableWeight;
		if (curResearch->hasBeenDisabled) {
			this->totalDisableWeight += curResearch->disableWeight;
		}
	}

	// What is the "median" weight ?
	double disabledUnitMedian = ((double)TT_CONFIG::MAX_DISABLE_UNITLINE_COUNT) - ((double)TT_CONFIG::MIN_DISABLE_UNITLINE_COUNT);
	double disabledResearchMedian = ((double)TT_CONFIG::MAX_DISABLE_RESEARCH_COUNT) - ((double)TT_CONFIG::MIN_DISABLE_RESEARCH_COUNT);
	double weightMean = totalWeightAll / (disabledUnitMedian + disabledResearchMedian);

	this->relativeDisableWeight = this->totalDisableWeight - weightMean;
	this->computedMeanWeight = weightMean;

	// TODO fix and use weight evaluation
	int bonusCount = randomizer.GetRandomValue_normal_moderate(2, 4);
	this->bonusText.clear();
	double bonusWeight = 0;
	for (int i = 0; i < bonusCount; i++) {
		bonusWeight += this->CreateOneBonus();
		if (bonusWeight > this->totalDisableWeight) { 
			break; // TEST
		}
	}
}


// Create one civ bonus
double TechTreeCreator::CreateOneBonus() {
	double result = 0;
	std::map<GLOBAL_UNIT_AI_TYPES, int> unitLinesByUnitClass;
	for (int i = 0; i <= GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorse; i++) {
		unitLinesByUnitClass[(GLOBAL_UNIT_AI_TYPES)i] = 0;
	}

	// Get total number of units for each class
	for (int unitDefId = 0; unitDefId < this->arrayUnitDefCount; unitDefId++) {
		// Remark: excluding buildings here
		TTDetailedTrainableUnitDef *detail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedTrainableUnitDef(unitDefId);
		if (!detail || detail->IsHeroOrScenarioUnit() || !detail->unitDef) { continue; }
		TTCreatorUnitInfo *crUnitInfo = this->GetCrUnitInfo(unitDefId);
		if (crUnitInfo && crUnitInfo->hasBeenDisabled) { continue; }
		// crUnitInfo may be NULL for non-eligible units: e.g. clubman...
		if ((detail->baseUnitId == detail->unitDefId) && (detail->unitClass >= 0)) {
			unitLinesByUnitClass[detail->unitClass]++;
		}
	}

	
	std::map<GLOBAL_UNIT_AI_TYPES, double> weightByUnitClass;
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher, 0.35));
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding, 0.2));
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot, 0.55));
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher, 0.4));
	//weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian, 0.)); //?
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher, 0.35));
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider, 0.4));
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupFootSoldier, 0.55));
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher, 0.7));
	// TODO: for TribeAIGroupMountedSoldier, treat unit by unit ?
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupMountedSoldier, 0.4)); // cavalry+scout+camel
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupPhalanx, 0.5));
	if (this->religionLevel > 3) {
		// Exclude bonus for priest for civs with bad temple. For religion levels 4-5 only.
		weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest, 0.65));
	}
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon, 0.7));
	weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger, 0.2));
	//weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupWarBoat, 0.2)); // is water map ? 0 else
	//weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupWall, 0.)); // walls suck
	//weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupTradeBoat, 0.05));
	//weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupTransportBoat, 0.05));
	//weightByUnitClass.insert(std::pair<GLOBAL_UNIT_AI_TYPES, double>(GLOBAL_UNIT_AI_TYPES::TribeAIGroupUnused_Tower, 0.45)); // do not use as is
	
	// TribeAIGroupUnusedHealer => just to improve medecine ?
	// TribeAIGroupUnused_Farm => just to improve production ?

	// Remove from list classes that already benefit from a civ bonus
	// Also remove classes with no unit (might have been disabled)
	for (auto iter = weightByUnitClass.begin(); iter != weightByUnitClass.end();) {
		GLOBAL_UNIT_AI_TYPES curClass = (*iter).first;
		auto ittmp = std::find(this->classesWithBonus.begin(), this->classesWithBonus.end(), curClass);
		bool toRemove = (ittmp != this->classesWithBonus.end()); // If already a bonus, remove
		toRemove |= (unitLinesByUnitClass[curClass] == 0); // If no impacted unit, remove too
		if (toRemove) {
			// Found: current unit class already got a bonus, exclude from "eligible" for this round
			weightByUnitClass.erase(iter++);
		} else {
			iter++;
		}
	}

	int bestRandom = -1;
	std::pair<GLOBAL_UNIT_AI_TYPES, double> bestElem;
	for each (std::pair<GLOBAL_UNIT_AI_TYPES, double> elem in weightByUnitClass)
	{
		int curRnd = randomizer.GetRandomValue(0, 100000);
		if (curRnd > bestRandom) {
			bestRandom = curRnd;
			bestElem = elem;
		}
	}
	GLOBAL_UNIT_AI_TYPES chosenClass = bestElem.first;
	double chosenWeight = bestElem.second;
	// Remove chosen one from list
	for (auto iter = weightByUnitClass.begin(); iter != weightByUnitClass.end();) {
		if ((*iter).first == chosenClass) {
			weightByUnitClass.erase(iter++);
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
		(chosenClass == TribeAIGroupPhalanx);

	bool isPriest = (chosenClass == TribeAIGroupPriest);
	bool isMilitary = (isRanged || isMelee ||isPriest);


	std::map<TECH_UNIT_ATTRIBUTES, double> weightByAttribute;
	weightByAttribute.insert(std::pair<TECH_UNIT_ATTRIBUTES, double>(TECH_UNIT_ATTRIBUTES::TUA_HP, 0.5));
	// TODO: complex, must be unit-per unit
	//weightByAttribute.insert(std::pair<TECH_UNIT_ATTRIBUTES, double>(TECH_UNIT_ATTRIBUTES::TUA_ADD_COST_AMOUNT, 0.5));
	if (isMilitary) {
		weightByAttribute.insert(std::pair<TECH_UNIT_ATTRIBUTES, double>(TECH_UNIT_ATTRIBUTES::TUA_ARMOR, 0.55));
		weightByAttribute.insert(std::pair<TECH_UNIT_ATTRIBUTES, double>(TECH_UNIT_ATTRIBUTES::TUA_ATTACK, 0.5));
		weightByAttribute.insert(std::pair<TECH_UNIT_ATTRIBUTES, double>(TECH_UNIT_ATTRIBUTES::TUA_ATTACK_RELOAD_TIME, 0.5));
	}
	if (isRanged) {
		weightByAttribute.insert(std::pair<TECH_UNIT_ATTRIBUTES, double>(TECH_UNIT_ATTRIBUTES::TUA_RANGE, 0.5)); // + LOS
	}
	// add TUA_INTELLIGENT_PROJECTILE ??
	// Note: avoid speed bonus on fast archers
	if ((chosenClass != TribeAIGroupBuilding) && (chosenClass != TribeAIGroupUnused_Tower) &&
		(chosenClass != TribeAIGroupHorseArcher) && (chosenClass != TribeAIGroupChariotArcher)) {
		weightByAttribute.insert(std::pair<TECH_UNIT_ATTRIBUTES, double>(TECH_UNIT_ATTRIBUTES::TUA_SPEED, 0.6));
	}
	if ((chosenClass == TribeAIGroupCivilian) || (chosenClass == TribeAIGroupFishingBoat)) {
		weightByAttribute.insert(std::pair<TECH_UNIT_ATTRIBUTES, double>(TECH_UNIT_ATTRIBUTES::TUA_RESOURCE_CAPACITY, 0.4)); // for villagers/fishing ships + work rate ?
	}
	
	int bestAttrRandom = -1;
	std::pair<TECH_UNIT_ATTRIBUTES, double> bestAttrElem;
	for each (auto elem in weightByAttribute)
	{
		int curRnd = randomizer.GetRandomValue(0, 100000);
		if (curRnd > bestAttrRandom) {
			bestAttrRandom = curRnd;
			bestAttrElem = elem;
		}
	}
	TECH_UNIT_ATTRIBUTES chosenAttr = bestAttrElem.first;
	double chosenAttrWeight = bestAttrElem.second;
	weightByAttribute.clear();
	const char *className = GetUnitClassName(chosenClass);
	this->classesWithBonus.insert(chosenClass);

	float rndMultiplier = 1 + (((float)randomizer.GetRandomValue_normal_moderate(10, 25)) / 100.0f);

	// Create bonus

	if ((chosenAttr == TUA_HP) || (chosenAttr == TUA_SPEED)) {
		AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
		newEffect.effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT;
		newEffect.effectAttribute = chosenAttr;
		newEffect.effectClass = chosenClass;
		newEffect.effectUnit = -1;
		newEffect.effectValue = rndMultiplier;
		techTreeEffects.push_back(newEffect);
		this->bonusText += "Increase ";
		this->bonusText += GetTechUnitAttributeName(chosenAttr);
		this->bonusText += " by ";
		this->bonusText += std::to_string((int)(rndMultiplier*100) - 100);
		this->bonusText += "% for class ";
		this->bonusText += className;
		this->bonusText += NEWLINE;
	}

	if (chosenAttr == TUA_RANGE) { // Range and line of sight
		AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
		newEffect.effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD;
		newEffect.effectAttribute = TUA_RANGE;
		newEffect.effectClass = chosenClass;
		newEffect.effectUnit = -1;
		newEffect.effectValue = 1;
		if ((chosenClass == TribeAIGroupSiegeWeapon) || (chosenClass == TribeAIGroupPriest)) {
			if (rndMultiplier > 1.175) {
				newEffect.effectValue = 2;
				chosenAttrWeight += 0.1;
			}
		}
		techTreeEffects.push_back(newEffect);

		this->bonusText += "+";
		this->bonusText += std::to_string(((int)newEffect.effectValue));
		this->bonusText += " ";
		this->bonusText += GetTechUnitAttributeName(TUA_RANGE);
		this->bonusText += " to class ";
		this->bonusText += className;
		this->bonusText += NEWLINE;

		AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect2;
		newEffect2.effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD;
		newEffect2.effectAttribute = TUA_LOS;
		newEffect2.effectClass = chosenClass;
		newEffect2.effectUnit = -1;
		newEffect2.effectValue = 1;
		if ((chosenClass == TribeAIGroupSiegeWeapon) || (chosenClass == TribeAIGroupPriest) || (chosenClass == TribeAIGroupUnused_Tower)) {
			if (rndMultiplier > 1.175) {
				newEffect.effectValue = 2;
			}
		}
		techTreeEffects.push_back(newEffect2);
	}

	if (chosenAttr == TUA_ARMOR) {
		AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
		newEffect.effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD;
		newEffect.effectAttribute = chosenAttr;
		newEffect.effectClass = chosenClass;
		newEffect.effectUnit = -1;
		newEffect.SetAttackOrArmor(ATTACK_CLASS::CST_AC_BASE_MELEE, 1);
		assert(newEffect.GetAttackOrArmorType() == ATTACK_CLASS::CST_AC_BASE_MELEE);
		assert(newEffect.GetValue() == 1);
		techTreeEffects.push_back(newEffect);
		this->bonusText += "+1 ";
		this->bonusText += GetTechUnitAttributeName(chosenAttr);
		this->bonusText += " to class ";
		this->bonusText += className;
		this->bonusText += NEWLINE;
	}

	if (chosenAttr == TUA_ATTACK) {
		if (isPriest) {
			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
			newEffect.effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT;
			newEffect.effectAttribute = TUA_WORK_RATE;
			newEffect.effectClass = chosenClass;
			newEffect.effectUnit = -1;
			newEffect.effectValue = rndMultiplier;
			techTreeEffects.push_back(newEffect);
			this->bonusText += "Increase conversion efficiency by ";
			this->bonusText += std::to_string((int)(rndMultiplier * 100) - 100);
			this->bonusText += "% for class ";
			this->bonusText += className;
			this->bonusText += NEWLINE;
		} else {
			int attackValue = 1;
			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
			newEffect.effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD;
			newEffect.effectAttribute = chosenAttr;
			newEffect.effectClass = chosenClass;
			newEffect.effectUnit = -1;
			// TODO: if siege, choose a unit (in standard, ballista or cat). If so, set effectClass=-1 and effectUnit=...
			bool useMeleeAttack = isMelee;
			if (chosenClass == TribeAIGroupSiegeWeapon) {
				newEffect.effectClass = -1;
				newEffect.effectUnit = CST_UNITID_STONE_THROWER; // TODO (hardcoded)
				useMeleeAttack = true; // should guess from unitdef
				attackValue = 5; // +1 would be ridiculous for siege.
			}
			if (useMeleeAttack) {
				newEffect.SetAttackOrArmor(ATTACK_CLASS::CST_AC_BASE_MELEE, attackValue);
				assert(newEffect.GetAttackOrArmorType() == ATTACK_CLASS::CST_AC_BASE_MELEE);
				assert(newEffect.GetValue() == attackValue);
			} else {
				if (chosenClass != TribeAIGroupSiegeWeapon) {
					// TODO: add weight when improving attack for non-siege range unit
				}
				newEffect.SetAttackOrArmor(ATTACK_CLASS::CST_AC_BASE_PIERCE, attackValue);
				assert(newEffect.GetAttackOrArmorType() == ATTACK_CLASS::CST_AC_BASE_PIERCE);
				assert(newEffect.GetValue() == attackValue);
			}
			techTreeEffects.push_back(newEffect);
			this->bonusText += "+";
			this->bonusText += std::to_string(attackValue);
			this->bonusText += " ";
			this->bonusText += GetTechUnitAttributeName(chosenAttr);
			this->bonusText += " for class ";
			this->bonusText += className;
			this->bonusText += NEWLINE;
		}
	}

	if (chosenAttr == TUA_ATTACK_RELOAD_TIME) {
		if (!isPriest) {
			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
			newEffect.effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT;
			newEffect.effectAttribute = chosenAttr;
			newEffect.effectClass = chosenClass;
			newEffect.effectUnit = -1;
			newEffect.effectValue = 1 / rndMultiplier; // Diminish reload time to improve attack efficiency
			techTreeEffects.push_back(newEffect);
			this->bonusText += "Increase attack speed by ";
			this->bonusText += std::to_string((int)(rndMultiplier * 100) - 100);
			this->bonusText += "% for class ";
			this->bonusText += className;
			this->bonusText += NEWLINE;
		} else {
			// Priest recharging rate is a player resource
			AOE_STRUCTURES::STRUCT_TECH_DEF_EFFECT newEffect;
			newEffect.effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_MULT;
			newEffect.effectAttribute = -1;// unused
			newEffect.effectClass = -1; // unused
			newEffect.effectUnit = CST_RES_ORDER_FAITH_RECHARGING_RATE;
			newEffect.effectValue = rndMultiplier; 
			techTreeEffects.push_back(newEffect);
			this->bonusText += "Increase priest recharging speed by ";
			this->bonusText += std::to_string((int)(rndMultiplier * 100) - 100);
			this->bonusText += "% for class ";
			this->bonusText += className;
			this->bonusText += NEWLINE;
		}
	}

	if (chosenAttr == TUA_RESOURCE_CAPACITY) { // fishing ships, villager (pick one only !)
		// Improve work rate too
		// TODO
	}

	if (chosenAttr == TUA_ADD_COST_AMOUNT) {
		// TODO
	}

	result = (chosenWeight + chosenAttrWeight) / 2;
	return result;
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
#pragma message("TTBUDisableMostlyExclusiveSameClassIron not implemented")
		// TODO
	}

	// TODO: for long lineage >=3 (legion, cats, cavalry...), do not disable "only" last one when required researches are ok ?

	int countForRandomChoice = unitInfoThatCanBeDisabled.size();
	int randomLevelChoice = randomizer.GetRandomValue(1, countForRandomChoice);
	for (int i = 1; i < randomLevelChoice; i++) {
		assert(unitInfoThatCanBeDisabled.size() > 0);
		if (unitInfoThatCanBeDisabled.size() == 0) {
			return rootUnitInfo;
		}
		unitInfoThatCanBeDisabled.pop_front();
	}
	assert(unitInfoThatCanBeDisabled.size() > 0);
	if (unitInfoThatCanBeDisabled.size() == 0) {
		return rootUnitInfo;
	}
	return unitInfoThatCanBeDisabled.front();
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
		if (childDetail->IsShadowResearch()) { continue; }
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
				std::list<TTCreatorResearchInfo*> tmpRes = this->CreateDisableEffectOnUnit(enTrainable); // Can happen, for example legion (when fanaticism is disabled)
				for each (auto x in tmpRes)
				{
					result.push_back(x);
				}
			}
		} else {
			child = *it; // we know this research
		}
		if (child && !child->hasBeenDisabled) {
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
		srcResInfo->disableProbability = 0;
		srcResInfo->disableScore = 0;
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


std::string TechTreeCreator::GetDisabledUnitsText() const {
	std::string result = "";
	std::list<TTCreatorUnitInfo*> disabledUnits;
	auto lambdaIsDisabled = [](TTCreatorUnitInfo *lambdaInfo){return lambdaInfo->hasBeenDisabled; };
	auto it = std::find_if(this->allCreatorUnitInfo.begin(), this->allCreatorUnitInfo.end(), lambdaIsDisabled);
	while (it != allCreatorUnitInfo.end()) {
		TTCreatorUnitInfo *curUnit = *it;
		disabledUnits.push_back(curUnit);
		it = std::find_if(++it, this->allCreatorUnitInfo.end(), lambdaIsDisabled);
	}

	disabledUnits.sort([](TTCreatorUnitInfo *elem, TTCreatorUnitInfo *elem2) {
		if (elem->trainLocation != elem2->trainLocation) { return elem->trainLocation < elem2->trainLocation; }
		if (elem->unitDetail->baseUnitId != elem2->unitDetail->baseUnitId) { return elem->unitDetail->baseUnitId < elem2->unitDetail->baseUnitId; }
		if (elem->unitDetail->requiredAge != elem2->unitDetail->requiredAge) { return elem->unitDetail->requiredAge < elem2->unitDetail->requiredAge; }
		if (elem->unitDetail->possibleAncestorUnitIDs.size() != elem2->unitDetail->possibleAncestorUnitIDs.size()) { return elem->unitDetail->possibleAncestorUnitIDs.size() < elem2->unitDetail->possibleAncestorUnitIDs.size(); }
		return elem->unitDefId < elem2->unitDefId;
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
	result += NEWLINE;
#endif

	return result;
}

std::string TechTreeCreator::GetCivBonusText() const {
	std::string result = "";
#ifdef _DEBUG
	result += "Tech tree Weight=";
	result += std::to_string(this->totalDisableWeight);
	result += "   /   needForBonusWeight=";
	result += std::to_string(this->relativeDisableWeight);
	result += "   /   median=";
	result += std::to_string(this->computedMeanWeight);
	result += NEWLINE;
#endif
	
	result += this->bonusText;
	//result += NEWLINE;
	return result;
}


}
}
