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
void CreateRandomTechTree(STRUCT_TECH_DEF *techDef) {
	if (!techDef || (techDef->effectCount > 0)) { return; }
	if (!ROCKNROR::crInfo.techTreeAnalyzer.IsReady()) { return; }

	long int arrayResearchCount = ROCKNROR::crInfo.techTreeAnalyzer.GetResearchCount();
	long int arrayUnitDefCount = ROCKNROR::crInfo.techTreeAnalyzer.GetUnitDefCount();

	// Get non-shadow researches
	std::set<long int> eligibleDisableResearchIdBronze;
	std::set<long int> eligibleDisableResearchIdIron;
	for (int resDefId = 0; resDefId < arrayResearchCount; resDefId++) {
		TTDetailedResearchDef *detail = ROCKNROR::crInfo.techTreeAnalyzer.GetDetailedResearchDef(resDefId);
		if (!detail || !detail->active || detail->IsShadowResearch()) { continue; }
		if (!detail->techDef) { continue; }
		if (!detail->hasValidEffect) { continue; }
		if (detail->requiredAge < CST_RSID_BRONZE_AGE) { continue; } // pre-bronze researches can't be disabled by tech tree
		// Exclude research that enable/upgrade units
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
		// TODO: add custom filters, eg pre-iron storage pit researches never disabled except shields, etc
		if (detail->requiredAge == AOE_CONST_FUNC::CST_RSID_BRONZE_AGE) {
			eligibleDisableResearchIdBronze.insert(resDefId);
		}
		if (detail->requiredAge == AOE_CONST_FUNC::CST_RSID_IRON_AGE) {
			eligibleDisableResearchIdIron.insert(resDefId);
		}
	}
	int researchCountBronze = eligibleDisableResearchIdBronze.size();
	int researchCountIron = eligibleDisableResearchIdIron.size();

	// Get eligible units that may be disabled (or receive bonus). Note: some unit excluded here may receive bonus via "unit class" bonuses.
	std::set<long int> eligibleDisableUnitDefIdBronze;
	std::set<long int> eligibleDisableUnitDefIdIron;
	for (int unitDefId = 0; unitDefId < arrayUnitDefCount; unitDefId++) {
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
			eligibleDisableUnitDefIdBronze.insert(unitDefId); // include tool age units (only slinger !)
		}
		if (detail->requiredAge == AOE_CONST_FUNC::CST_RSID_IRON_AGE) {
			eligibleDisableUnitDefIdIron.insert(unitDefId);
		}
	}
	int eligibleDisableUnitDefIdBronzeCount = eligibleDisableUnitDefIdBronze.size();
	int eligibleDisableUnitDefIdIronCount = eligibleDisableUnitDefIdIron.size();


	// main parts:
	// 1) disable some researches (consequence: may cascade other researches + disable some units)
	// 2) disable some units/upgrade
	// 3) add some bonuses

	int bonusCount = randomizer.GetRandomValue_normal_moderate(2, 4);


	// Ex: 65 techs bronze/iron
	// egypt 16 disable unit/upgrade, 5 disable tech
	// greek 15 disable unit/upgrade, 5 disable tech
	// assy 13 disable unit/upgrade, 11 disable tech
	// bab 13 disable unit/upgrade, 6 disable tech
	// yama 14 disable unit/upgrade, 8 disable tech mainly temple + towers/wall not counted

	// disable units: make a list of "base ids", choose an id in it, then choose an "upgrade level" to disable. So big families (infantry...) don't get higher chances of being disabled(?)
	// Do not allow disable building at this point (except iron age towers)
	// Set a probability of being disabled + a score (weight of being disabled) ?
}


}
}
