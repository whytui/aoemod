#include "../include/PlayerTechTree.h"

namespace ROCKNROR {
namespace STRATEGY {
;


// Fill enabledXxx/disabledXxx collections according to player's tech tree (myTechTreeId).
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


	// DISABLE RESEARCH
	this->SetResearchBaseProbabilities();
	this->CalcResearchesDisableScore();

	// main parts:
	// 1) disable some researches (consequence: may cascade other researches + disable some units)
	// 2) disable some units/upgrade
	// 3) add some bonuses

	int bonusCount = randomizer.GetRandomValue_normal_moderate(2, 4);


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

	// disable units: make a list of "base ids", choose an id in it, then choose an "upgrade level" to disable. So big families (infantry...) don't get higher chances of being disabled(?)
	// Do not allow disable building at this point (except iron age towers)
	// Set a probability of being disabled + a score (weight of being disabled) ?
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
		bool isRootUnit = detail->baseUnitId == detail->unitDefId; // WRONG !
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
void TechTreeCreator::SetResearchBaseProbabilities() {
	// INIT: create object for each eligible research.
	for each (long int resDefId in this->eligibleDisableResearchIdBronze) {
		TTCreatorResearchInfo *ttcResearchInfo = new TTCreatorResearchInfo();
		ttcResearchInfo->researchId = resDefId;
		ttcResearchInfo->researchDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resDefId);
		// ttcResearchInfo->researchDetail should always be valid: all IDs from this->eligible* collections should be valid
		this->allCreatorResearchInfo.push_back(ttcResearchInfo);
	}
	for each (long int resDefId in this->eligibleDisableResearchIdIron) {
		TTCreatorResearchInfo *ttcResearchInfo = new TTCreatorResearchInfo();
		ttcResearchInfo->researchId = resDefId;
		ttcResearchInfo->researchDetail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resDefId);
		// ttcResearchInfo->researchDetail should always be valid: all IDs from this->eligible* collections should be valid
		this->allCreatorResearchInfo.push_back(ttcResearchInfo);
	}
	
	// Collect general info
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo)
	{
		assert(crResInfo->researchDetail != NULL);
		crResInfo->researchLocation = crResInfo->researchDetail->researchDef->researchLocation;
		AOE_CONST_FUNC::ATTACK_CLASS armorClass = AOE_STRUCTURES::RESEARCH::DoesTechImproveSomeArmor(crResInfo->researchDetail->techDef);
		AOE_CONST_FUNC::ATTACK_CLASS attackClass = AOE_STRUCTURES::RESEARCH::DoesTechImproveSomeAttack(crResInfo->researchDetail->techDef);
		crResInfo->techImpactsReligion = AOE_STRUCTURES::RESEARCH::DoesTechImpactReligion(crResInfo->researchDetail->techDef);
	}

	// 1 - set probability for "hardcoded" groups:  eg. storage pit iron-armors (3) should remain together
	double randomProbaForArmors = RES_PROBA_STANDARD_RESEARCH;
	const double weightForArmors = RES_WEIGHT_STANDARD_RESEARCH;
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo)
	{
		TTDetailedResearchDef *detail = crResInfo->researchDetail;
		if ((crResInfo->updatedArmorClass == ATTACK_CLASS::CST_AC_BASE_MELEE) &&
			(detail->requiredAge == AOE_CONST_FUNC::CST_RSID_IRON_AGE) &&
			(detail->researchDef->researchLocation != AOE_CONST_FUNC::CST_UNITID_STORAGE_PIT)) {
			crResInfo->disableProbability = randomProbaForArmors;
			crResInfo->disableWeight = weightForArmors;
		}
	}


	// 2 - set probability for friendly researches (temple...)
	// Temple: choose "religion level" : low/moderate/high. All temple techs proba. will be chosen in the same probability range, according to this level.
	this->religionLevel = randomizer.GetRandomValue(0, 5);
	double minReligionProbaRange = RES_PROBA_MIN;
	double maxReligionProbaRange = RES_PROBA_MAX;
	double weightForReligion = RES_WEIGHT_STANDARD_RESEARCH;
	double religionBronzeFactor = 1;
	switch (this->religionLevel) {
	case 0: // level 0 (min): being disable is VERY likely to happen for those techs
		minReligionProbaRange = 0.7;
		maxReligionProbaRange = RES_PROBA_MAX;
		break;
	case 5: // level 5 (max): player is likely to have access to all techs (good temple civ)
		minReligionProbaRange = RES_PROBA_MIN;
		maxReligionProbaRange = 0.3;
		religionBronzeFactor = 1.1;
		break;
	case 1:
	case 2: // 1-2 (quite poor temple)
		minReligionProbaRange = 0.33;
		maxReligionProbaRange = 0.8;
		break;
	default: // 3-4 (quite good temple)
		minReligionProbaRange = 0.2;
		maxReligionProbaRange = 0.66;
		religionBronzeFactor = 1.05;
	}
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo)
	{
		if (crResInfo->techImpactsReligion && (crResInfo->disableProbability < 0)) {
			// TODO
			crResInfo->disableProbability = maxReligionProbaRange;
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
	double randomProbaForEconomy = RES_PROBA_STANDARD_RESEARCH;
	double weightForEconomy = RES_WEIGHT_STANDARD_RESEARCH;
	double economyBronzeFactor = 1.25;
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo)
	{
		bool isMarketOrGov = (crResInfo->researchLocation == AOE_CONST_FUNC::CST_UNITID_MARKET) ||
			(crResInfo->researchLocation == AOE_CONST_FUNC::CST_UNITID_GOVERNMENT_CENTER);
		if ((crResInfo->disableProbability < 0) && isMarketOrGov) {
			if (crResInfo->researchDetail->IsWheel()) {
				crResInfo->disableProbability = RES_PROBA_WHEEL;
				crResInfo->disableWeight = RES_WEIGHT_WHEEL;
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

	// 3 - set probability for remaining researches
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo)
	{
		if (crResInfo->disableProbability < 0) {
			crResInfo->disableProbability = RES_PROBA_STANDARD_RESEARCH;
			crResInfo->disableWeight = RES_WEIGHT_STANDARD_RESEARCH;
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


void TechTreeCreator::CalcResearchesDisableScore() {
	for each (TTCreatorResearchInfo *crResInfo in this->allCreatorResearchInfo) {
		int imyrand = randomizer.GetRandomNonZeroPercentageValue();
		double myrand = (double)imyrand;
		
		double tmpCompute = crResInfo->disableProbability * myrand * RES_PROBA_IMPACT_ON_RANDOM +
			myrand * (1 - RES_PROBA_IMPACT_ON_RANDOM); // Now tmpCompute is in 0-100

		crResInfo->disableScore = tmpCompute;

	}
}


}
}
