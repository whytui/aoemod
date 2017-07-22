#include "../include/CustomCiv.h"


namespace ROCKNROR {
	namespace CUSTOMCIV {
;


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


// Initialize each player data for current (or new) game
// Sets various info such as civId, techTreeId, isActive
void CustomCivHandler::InitForCurrentGame() {
	this->ResetPlayers();
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) {
		return;
	}
	for (int playerId = 1; playerId < global->playerTotalCount; playerId++) {
		STRUCT_PLAYER *player = global->GetPlayerStruct(playerId);
		if (!player || !player->IsCheckSumValid()) { continue; }
		this->InitValidPlayer(playerId, player->civilizationId, player->techTreeId);
	}
}


bool CustomCivHandler::CreateFakeRandomCivsForAllPlayers() {
	this->InitForCurrentGame();

	// TEST
	auto pi1 = &this->playersInfo[1];
	if (pi1) {
		pi1->CollectInfoFromExistingTechTree();
	}


	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!settings || !settings->IsCheckSumValid() || !global || !global->IsCheckSumValid()) {
		return false;
	}
	if (!global->technologiesInfo) { return false; }
	int playerCount = global->playerTotalCount - 1; // do not include gaia here.

	long int initialTechDefCount = ROCKNROR::crInfo.empiresDatTechDefCount;
	if (initialTechDefCount <= 0) { return false; } // maybe empires.dat has not been read yet.
	AOE_STRUCTURES::ReallocTechDefArray(initialTechDefCount + 8);

	for (int playerId = 1; playerId <= playerCount; playerId++) {
		int techTreeIndex = initialTechDefCount + playerId - 1;
		// Clean previous effects
		CleanEffectsForTechDef(global->technologiesInfo->GetTechDef(techTreeIndex));
		STRUCT_PLAYER *player = global->GetPlayerStruct(playerId);
		if (!player || !player->IsCheckSumValid()) { continue; }
		player->techTreeId = techTreeIndex; // used at game init to apply civilization bonus (malus), then not really used.
		this->playersInfo[playerId].myTechTreeId = techTreeIndex;

		//TEST
		STRUCT_TECH_DEF *techDef = global->technologiesInfo->GetTechDef(techTreeIndex);
		techDef->effectCount = 3;
		techDef->ptrEffects = (STRUCT_TECH_DEF_EFFECT*)AOEAllocZeroMemory(3, sizeof(STRUCT_TECH_DEF_EFFECT));
		techDef->ptrEffects[0].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT;
		techDef->ptrEffects[0].effectClass = AOE_CONST_FUNC::TribeAIGroupFootSoldier;
		techDef->ptrEffects[0].effectUnit = -1;
		techDef->ptrEffects[0].effectAttribute = TECH_UNIT_ATTRIBUTES::TUA_HP;
		techDef->ptrEffects[0].effectValue = ((float)playerId) / 10 + 1;

		techDef->ptrEffects[1].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD;
		//techDef->ptrEffects[1].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_DISABLE_RESEARCH;
		//techDef->ptrEffects[1].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_ENABLE_DISABLE_UNIT;
		//techDef->ptrEffects[1].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_MULT;
		techDef->ptrEffects[1].effectClass = AOE_CONST_FUNC::TribeAIGroupCivilian;
		techDef->ptrEffects[1].effectUnit = -1;
		techDef->ptrEffects[1].effectAttribute = TECH_UNIT_ATTRIBUTES::TUA_LOS;
		techDef->ptrEffects[1].effectValue = (float)playerId;

		techDef->ptrEffects[2].effectType = AOE_CONST_FUNC::TECH_DEF_EFFECTS::TDE_RESOURCE_MODIFIER_ADD_SET;
		techDef->ptrEffects[2].effectUnit = CST_RES_ORDER_TRIBUTE_SENT; // resource
		techDef->ptrEffects[2].effectClass = 0; // set ?
		techDef->ptrEffects[2].effectAttribute = -1; // unused?
		techDef->ptrEffects[2].effectValue = (float)playerId;
	}


	return true;
}


}
}
