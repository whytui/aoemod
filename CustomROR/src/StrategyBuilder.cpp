#include "../include/StrategyBuilder.h"

struct StrategyUnitInfo {
	StrategyUnitInfo(short int unitDefId) {
		this->unitDefId = unitDefId;
		this->countWithUnlimitedRetrains = 0;
		this->countWithLimitedRetrains = 0;
		this->totalCount = 0;
	}
	short int unitDefId;
	int countWithUnlimitedRetrains;
	int countWithLimitedRetrains;
	int totalCount;
};

class StrategyUnitsInfo {
public:
	~StrategyUnitsInfo() {
		for each (StrategyUnitInfo *unitInfo in this->unitsInfo)
		{
			delete unitInfo;
		}
		this->unitsInfo.clear();
	}
	StrategyUnitInfo *AddUnitInfo(short int unitDefId) {
		StrategyUnitInfo *unitInfo = new StrategyUnitInfo(unitDefId);
		this->unitsInfo.push_back(unitInfo);
		return unitInfo;
	}
	StrategyUnitInfo *GetUnitInfo(short int unitDefId) {
		for each (StrategyUnitInfo *unitInfo in this->unitsInfo)
		{
			if (unitInfo->unitDefId == unitDefId) { return unitInfo; }
		}
		return NULL;
	}
	StrategyUnitInfo *GetOrAddUnitInfo(short int unitDefId) {
		StrategyUnitInfo *u = GetUnitInfo(unitDefId);
		if (u) { return u; }
		return this->AddUnitInfo(unitDefId);
	}
	// collectMilitaryUnits: if true, collect only military units, otherwise, collect only villager units
	void CollectUnitInfosFromStrategy(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, bool collectMilitaryUnits) {
		if (!buildAI || !buildAI->IsCheckSumValid()) { return; }
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &buildAI->fakeFirstStrategyElement;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *currentElem = fakeFirstElem->next;
		while (currentElem && (currentElem != fakeFirstElem)) {
			bool isVillager = IsVillager_includingShips((unsigned short int)currentElem->unitDAT_ID);
			if ((currentElem->elementType == AIUCLivingUnit) &&
				((isVillager && !collectMilitaryUnits) || (!isVillager && collectMilitaryUnits))) {
				StrategyUnitInfo *uInfo = this->GetOrAddUnitInfo((short int)currentElem->unitDAT_ID);
				uInfo->totalCount++;
				if (currentElem->retrains == -1) {
					uInfo->countWithUnlimitedRetrains++;
				} else {
					uInfo->countWithLimitedRetrains++;
				}
			}
			currentElem = currentElem->next;
		}
	}
	std::vector<StrategyUnitInfo*> unitsInfo;
};



// Create all base strategy elements (ages + buildings=barracks,market,govSiege + wheel if available)
// Does not add villagers
void StrategyBuilder::CreateBasicStrategyElements(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI) {
	if (!buildAI || !buildAI->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(buildAI->commonAIObject.playerId);
	if (!player || !player->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *fakeFirstElem = &buildAI->fakeFirstStrategyElement;
	
	// Adding before "fake first" will add in last position in strategy.
	// Add Town Center
	AddUnitInStrategy_before(buildAI, fakeFirstElem, -1, -1, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding, CST_UNITID_FORUM, player, "TownCenter");
	// Barracks are always needed
	AddUnitInStrategy_before(buildAI, fakeFirstElem, -1, -1, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding, CST_UNITID_BARRACKS, player, "Barracks");
	// Tool Age
	if (!IsResearchRelevantForStrategy(player, CST_RSID_TOOL_AGE)) {
		string msg = "This civ seems NOT to have tool age. Player #";
		msg += to_string(buildAI->commonAIObject.playerId);
		msg += " : ";
		msg += buildAI->commonAIObject.playerName;
		traceMessageHandler.WriteMessageNoNotification(msg);
		return;
	}
	AddUnitInStrategy_before(buildAI, fakeFirstElem, -1, CST_UNITID_FORUM, AOE_CONST_FUNC::TAIUnitClass::AIUCCritical, CST_RSID_TOOL_AGE, player, "*Tool Age*");
	AddUnitInStrategy_before(buildAI, fakeFirstElem, -1, -1, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding, CST_UNITID_MARKET, player, "Market");
	// Bronze Age
	if (!IsResearchRelevantForStrategy(player, CST_RSID_BRONZE_AGE)) {
		string msg = "This civ seems NOT to have bronze age. Player #";
		msg += to_string(buildAI->commonAIObject.playerId);
		msg += " : ";
		msg += buildAI->commonAIObject.playerName;
		traceMessageHandler.WriteMessageNoNotification(msg);
		return;
	}
	AddUnitInStrategy_before(buildAI, fakeFirstElem, -1, CST_UNITID_FORUM, AOE_CONST_FUNC::TAIUnitClass::AIUCCritical, CST_RSID_BRONZE_AGE, player, "*Bronze Age*");
	AddUnitInStrategy_before(buildAI, fakeFirstElem, -1, -1, AOE_CONST_FUNC::TAIUnitClass::AIUCBuilding, CST_UNITID_GOVERNMENT_CENTER, player, "Government center");
	if (IsResearchRelevantForStrategy(player, CST_RSID_WHEEL)) { // requires research status to be set
		AddUnitInStrategy_before(buildAI, fakeFirstElem, -1, CST_UNITID_MARKET, AOE_CONST_FUNC::TAIUnitClass::AIUCTech, CST_RSID_WHEEL, player, "Wheel");
	}
	// Iron Age
	if (!IsResearchRelevantForStrategy(player, CST_RSID_IRON_AGE)) {
		string msg = "This civ seems NOT to have iron age. Player #";
		msg += to_string(buildAI->commonAIObject.playerId);
		msg += " : ";
		msg += buildAI->commonAIObject.playerName;
		traceMessageHandler.WriteMessageNoNotification(msg);
		return;
	}
	AddUnitInStrategy_before(buildAI, fakeFirstElem, -1, CST_UNITID_FORUM, AOE_CONST_FUNC::TAIUnitClass::AIUCCritical, CST_RSID_IRON_AGE, player, "*Iron Age*");
}

// Create a brand new dynamic strategy for player.
void StrategyBuilder::CreateStrategyFromScratch(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI) {
	if (!buildAI || !buildAI->IsCheckSumValid() || !buildAI->mainAI || !buildAI->mainAI->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_PLAYER *player = GetPlayerStruct(buildAI->commonAIObject.playerId);
	if (!player || !player->IsCheckSumValid()) { return; }
	ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL *global = player->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return; }
	// Clear previous strategy
	ClearStrategy(buildAI);
	strcpy_s(buildAI->strategyFileName, 0x3F, "Dynamic CustomROR strategy");
	// Initialize strategy with ages (no villager yet)
	this->CreateBasicStrategyElements(buildAI);


	// WARNING : take care if map is naval (=> villager count, land military units count + all non-war boats thing)
	// + max pop criterion


	if (buildAI->mainAI->structTradeAI.needGameStartAIInit) {
		// Warning: automatic element insertions WILL be triggered
	} else {
		// TODO : trigger dynamic element insertions ?
	}
}


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
			unitInfo->upgradedUnitDefLiving = unitDefLiving; // Default: base unit. We'll update this later if necessary
			unitInfo->unitDefId = unitDefLiving->DAT_ID1;

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
			unitInfo->speed = bestUpgradedUnit->speed; // // updated below if some researches update the speed value
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

			// Analysze costs
			for (int i = 0; i < 3; i++) {
				if ((unitDefLiving->costs[i].costType >= AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_FOOD) &&
					(unitDefLiving->costs[i].costType <= AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_GOLD) &&
					(unitDefLiving->costs[i].costPaid)) {
					unitInfo->totalResourceCost += unitDefLiving->costs[i].costAmount;
					if (unitDefLiving->costs[i].costType == AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_GOLD) {
						unitInfo->costsGold = true;
					}
				}
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
							/*short int upgradeTargetUnitDefId = techDef->ptrEffects[i].UpgradeUnitGetTargetUnit();
							if ((upgradeTargetUnitDefId >= 0) && (techDef->ptrEffects[i].effectUnit == unitDefLiving->DAT_ID1)) {
								// We found an upgrade for our unit
								unitInfo->upgradesUnitDefId.push_back(upgradeTargetUnitDefId);
							}
							if (upgradeTargetUnitDefId == unitDefLiving->DAT_ID1) {
								// We found the base unit of our unit (our unit is an upgrade for some other unit)
								unitInfo->baseUnitDefId = techDef->ptrEffects[i].effectUnit;
							}*/
							bool isAttrModifier = ((techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) ||
								(techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) ||
								(techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT));
							bool affectsMe = ((techDef->ptrEffects[i].effectUnit == unitDefLiving->DAT_ID1) ||
								(techDef->ptrEffects[i].effectClass == unitDefLiving->unitAIType));
							if (isAttrModifier && affectsMe && (techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_SPEED)) {
								float value = techDef->ptrEffects[i].GetValue();
								if (techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) {
									unitInfo->speed += value;
								}
								if (techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) {
									unitInfo->speed = value;
								}
								if (techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT) {
									unitInfo->speed = unitInfo->speed * value;
								}
							}
						}
					}
				}
			}
			// Save unit infos in list
			this->potentialUnitsList.push_back(unitInfo);
		}
	}
}


std::shared_ptr<StrategyGenerationInfo> StrategyBuilder::GetStrategyGenerationInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
	if (!player || !player->IsCheckSumValid()) { return NULL; }
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return NULL; }
	std::shared_ptr<StrategyGenerationInfo> shrPtr(make_shared<StrategyGenerationInfo>(player));
	StrategyGenerationInfo *genInfo = shrPtr.get();
	genInfo->maxPopulation = settings->maxPopulation;
	genInfo->isWaterMap = IsDockRelevantForMap(settings->mapTypeChoice);

	// *** Compute unit numbers ***
	// Land maps: fixed villagers count around 14-20  ; retrains around 6-10  ; total 18-28
	// Water maps: fixed villagers count around 10-16  ; retrain around 5-9  ; total 16-24 . original AI: often 10+6 or 14+6
	const long int maxTotalVillagers = genInfo->isWaterMap ? 22 : 26;
	const long int minFixedVillagers = genInfo->isWaterMap ? 10 : 14;
	const long int minLimitedRetrainsVillagers = genInfo->isWaterMap ? 5 : 6;
	assert(minFixedVillagers + minLimitedRetrainsVillagers < maxTotalVillagers);
	const long int maxFixedVillagersRandomPart = 6; // random interval size
	const long int maxLimitedRetrainsVillagersRandomPart = genInfo->isWaterMap ? 4 : 6; // random interval size

	int fixedVillagerCount = randomizer.GetRandomValue_normal_moderate(minFixedVillagers, minFixedVillagers + maxFixedVillagersRandomPart);
	int limitedRetrainsVillagerCount = randomizer.GetRandomValue_normal_moderate(minLimitedRetrainsVillagers, minLimitedRetrainsVillagers + maxLimitedRetrainsVillagersRandomPart);
	if (fixedVillagerCount + limitedRetrainsVillagerCount > maxTotalVillagers) { limitedRetrainsVillagerCount = maxTotalVillagers; }

	genInfo->villagerCount_alwaysRetrain = fixedVillagerCount;
	genInfo->villagerCount_limitedRetrains = limitedRetrainsVillagerCount;
	// TODO: more villagers if max population > 50 ?

	// Boats : TODO
	if (genInfo->isWaterMap) {

	}

	// Select the military units to train

	this->CollectPotentialUnitsInfo(player);
	this->ComputeScoresForPotentialUnits();



	return shrPtr;
}

// Clears potential units list and free each underlying PotentialUnitInfo object.
void StrategyBuilder::FreePotentialElementsList() {
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		delete unitInfo;
	}
	this->potentialUnitsList.clear();
}

// Compute all score fields for all units in potential units list.
void StrategyBuilder::ComputeScoresForPotentialUnits() {
	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		unitInfo->strengthVsMelee = 0;
		unitInfo->strengthVsFastRanged = 0;
		unitInfo->strengthVsPriests = 0;
		unitInfo->strengthVsSiege = 0;
		unitInfo->weaknessVsMelee = 0;
		unitInfo->weaknessVsFastRanged = 0;
		unitInfo->weaknessVsPriests = 0;
		unitInfo->weaknessVsSiege = 0;
		
		// availableRelatedResearchesProportion is a % value (0-100)
		int availableRelatedResearchesProportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->unavailableRelatedResearchesCount + unitInfo->availableRelatedResearchesCount);
		// A combination of speed and hit points
		float adjustedSpeed = unitInfo->speed - 0.4f;
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

		// Compute priest-related values
		this->ComputeScoresVsPriests(unitInfo, (int)damageScore);

		// Compute towers-relative values
		this->ComputeScoresVsTower(unitInfo);
		if (damageScore <= 1) {
			unitInfo->strengthVsTowers = (unitInfo->strengthVsTowers * 33) / 100;
		}
		if (damageScore == 2) {
			unitInfo->strengthVsTowers = (unitInfo->strengthVsTowers * 66) / 100;
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
					unitInfo->weaknessVsSiege += 10;
				} else {
					// Well upgraded unit: not so bad. Do not apply a bonus nor a malus
				}
			}
		} else { // Other than siege
			if (!unitInfo->isMelee) {
				if (unitInfo->unitAIType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
					baseValue = baseValue * 0.33f; // archers : attack not efficient vs siege
				}
				unitInfo->weaknessVsSiege += 25; // archers/priests : weak vs siege (easily killed)
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

		unitInfo->strengthVsSiege += (int)baseValue;
		if (speedHitPointsFactor < 40) {
			unitInfo->weaknessVsSiege += 10;
			if (speedHitPointsFactor < 20) {
				unitInfo->weaknessVsSiege += 10;
			}
		}

		// Compute values vs fast ranged units (like chariot/cavalry archer)
		unitInfo->strengthVsFastRanged = 10;
		unitInfo->weaknessVsFastRanged = 10;
		switch (unitInfo->unitAIType) {
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher:
			unitInfo->strengthVsFastRanged = 100;
			unitInfo->weaknessVsFastRanged = 10;
			break;
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher:
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher:
			unitInfo->strengthVsFastRanged = 40;
			unitInfo->weaknessVsFastRanged = 40;
			break;
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon:
			unitInfo->strengthVsFastRanged = 30;
			unitInfo->weaknessVsFastRanged = 10;
			break;
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger:
			unitInfo->strengthVsFastRanged = 90;
			unitInfo->weaknessVsFastRanged = 5;
			break;
		case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest:
			unitInfo->strengthVsFastRanged = 5;
			unitInfo->weaknessVsFastRanged = 95;
			break;
		default:
			break;
		}
		if (unitInfo->isMelee) {
			unitInfo->strengthVsFastRanged = 40; // default for units faster than normal, but not really fast
			unitInfo->weaknessVsFastRanged = 40; // default for units faster than normal, but not really fast
			if (unitInfo->speed < 1.4) {
				unitInfo->strengthVsFastRanged = 20;
				if (unitInfo->speed < 1.1) {
					unitInfo->strengthVsFastRanged -= 10;
				}
				unitInfo->weaknessVsFastRanged = 80;
			}
			if (unitInfo->speed >= 2) {
				unitInfo->strengthVsFastRanged = 80;
				unitInfo->weaknessVsFastRanged = 10;
			}
		}
		// Take into account attack bonuses vs mounted archers OR vs archers
		if (unitInfo->upgradedUnitDefLiving && unitInfo->upgradedUnitDefLiving->ptrAttacksList) {
			for (int i = 0; i < unitInfo->upgradedUnitDefLiving->attacksCount; i++) {
				if ((unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CAMEL_ON_CAVALRY) ||
					(unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_SLINGER_ON_ARCHERS)) {
					unitInfo->strengthVsFastRanged += 30;
				}
			}
		}
		if (damageScore <= 1) {
			unitInfo->strengthVsFastRanged = (unitInfo->strengthVsFastRanged * 20) / 100;
		}
		if (damageScore == 2) {
			unitInfo->strengthVsFastRanged = (unitInfo->strengthVsFastRanged * 66) / 100;
		}
		if (unitInfo->totalResourceCost <= 90) {
			unitInfo->strengthVsFastRanged = (unitInfo->strengthVsFastRanged * 110) / 100;
			if (unitInfo->totalResourceCost <= 50) {
				unitInfo->strengthVsFastRanged = (unitInfo->strengthVsFastRanged * 125) / 100;
			}
		}
		if (unitInfo->strengthVsFastRanged > 100) { unitInfo->strengthVsFastRanged = 100; }


		// Compute values vs melee units
		unitInfo->strengthVsMelee = 0;
		unitInfo->weaknessVsMelee = 0;
		if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
			unitInfo->strengthVsMelee = 30;
			unitInfo->weaknessVsMelee = 20;
			if (availableRelatedResearchesProportion >= 80) {
				unitInfo->strengthVsMelee += 20;
			}
		} else {
			if (unitInfo->isMelee) {
				unitInfo->strengthVsMelee = 50;
				unitInfo->weaknessVsMelee = 10;
				if ((unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupMountedSoldier) ||
					(unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot)){
					// Cavalry, camel, scout + chariot&scythe
					unitInfo->strengthVsMelee -= 10;
					unitInfo->weaknessVsMelee += 20; // Speed is no use in melee vs melee, it just means the cost paid for speed is useless and waste
				}
				if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupFootSoldier) {
					// clubman + swordsman families. Quite adapted, can train a lot of them (low cost).
					unitInfo->strengthVsMelee += 15;
				}
				if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPhalanx) {
					// Hoplite + upgrades
					unitInfo->strengthVsMelee += 50; // Hoplites are excellent in melee combat
				}
				if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider) {
					// War elephants (NOT archer)
					unitInfo->strengthVsMelee += 20; // war elephants "like" melee combat, especially due to their blast damage.
				}
			} else {
				// Ranged (non-priest)
				switch (unitInfo->unitAIType) {
				case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger:
					unitInfo->strengthVsMelee = 0;
					unitInfo->weaknessVsMelee = 100;
					break;
				case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon:
					unitInfo->strengthVsMelee = 20;
					unitInfo->weaknessVsMelee = 10;
					if (!unitInfo->hasUnavailableUpgrade) {
						unitInfo->strengthVsMelee += 10;
					} else {
						unitInfo->weaknessVsMelee += 15; // siege weapons with few upgrades are easy targets for melee units
					}
					if (availableRelatedResearchesProportion >= 80) {
						unitInfo->strengthVsMelee += 10;
					} else {
						unitInfo->weaknessVsMelee += 15; // siege weapons with few upgrades are easy targets for melee units
					}
					break;
				default:
					// Standard archer types
					unitInfo->strengthVsMelee = 40;
					unitInfo->weaknessVsMelee = 10;
					break;
				}
			}
			// Here: all but priests
			if (unitInfo->upgradedUnitDefLiving && unitInfo->upgradedUnitDefLiving->ptrAttacksList) {
				for (int i = 0; i < unitInfo->upgradedUnitDefLiving->attacksCount; i++) {
					if (unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CAVALRY_ON_INFANTRY) {
						unitInfo->strengthVsMelee += 10; // Note: The attack bonus only applies to barracks infantry, so don't add too much strength
					}
					if (unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CAMEL_ON_CAVALRY) {
						unitInfo->weaknessVsMelee += 10; // Camels are good vs cavalry, but weak against all other melee units
					}
				}
			}
			if (damageScore <= 2) {
				unitInfo->strengthVsMelee -= 30;
			}
			if (damageScore == 4) {
				unitInfo->strengthVsMelee += 10;
			}
		}
	} // end loop on units
}


// Compute score fields for priests in unitInfo object.
void StrategyBuilder::ComputeScoresVsPriests(PotentialUnitInfo *unitInfo, int damageScore) {
	unitInfo->weaknessVsPriests = 0;
	unitInfo->strengthVsPriests = 20; // Default base strength value
	if (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest) {
		// availableRelatedResearchesProportion is a % value (0-100)
		int availableRelatedResearchesProportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->unavailableRelatedResearchesCount + unitInfo->availableRelatedResearchesCount);
		int scoreForPriestResearches = (availableRelatedResearchesProportion < 80) ? 30 : 70;
		// TODO: test monotheism ?
		// Well-developed priests are good against priests (at least, allows converting back my units OR converting enemy units)
		unitInfo->strengthVsPriests = scoreForPriestResearches;
		unitInfo->weaknessVsPriests = 100 - scoreForPriestResearches;
		return;
	}

	// Conversion resistances: both a defensive & offensive criteria
	if (unitInfo->conversionResistance < 1) { // Easier to convert (does not exist in standard game)
		unitInfo->weaknessVsPriests += 20;
		unitInfo->strengthVsPriests -= 20;
	}
	if (unitInfo->conversionResistance > 4) {
		unitInfo->weaknessVsPriests -= 100; // Very hard to convert - almost insensitive to conversion !
		unitInfo->strengthVsPriests += 100;
	}
	if ((unitInfo->conversionResistance > 2) && (unitInfo->conversionResistance <= 4)) {
		unitInfo->weaknessVsPriests -= 40; // Hard to convert
		unitInfo->strengthVsPriests += 40;
	}
	if ((unitInfo->conversionResistance > 1) && (unitInfo->conversionResistance <= 2)) {
		unitInfo->weaknessVsPriests -= 20; // A bit hard to convert
		unitInfo->strengthVsPriests += 20;
	}
	// Slow units are weak against priests
	// Note on standard speeds: 0.8-0.9 slow (siege, elephants, base hopite&priests), 1.1-1.2 normal(villagers, infantry,archers), 2 fast
	// Boats: 1.35-1.4, 1.75, 2, 2.5(upgraded trade ship). War ships are all in 1.35-1.75
	int basePenaltyValue = 10;
	if (unitInfo->speed < 1) {
		basePenaltyValue = 20;
	}
	if ((unitInfo->speed >= 1.0f) && (unitInfo->speed < 1.5f)) {
		basePenaltyValue = 10;
	}
	if (unitInfo->speed >= 1.5f) {
		basePenaltyValue = 2; // Fast units: can defend correctly against priests.
	}
	// Costly units are a weakness against priests (especially elephants, but not only)
	if (unitInfo->totalResourceCost > 160) {
		basePenaltyValue *= 3;
		unitInfo->strengthVsPriests -= 10;
	}
	if ((unitInfo->totalResourceCost > 100) && (unitInfo->totalResourceCost <= 160)) {
		basePenaltyValue = basePenaltyValue * 2;
	}
	if ((unitInfo->totalResourceCost > 50) && (unitInfo->totalResourceCost <= 100)) {
		basePenaltyValue = (basePenaltyValue * 150) / 100;
		unitInfo->strengthVsPriests += 10;
	}
	if ((unitInfo->totalResourceCost <= 50)) {
		unitInfo->strengthVsPriests += 15; // low cost = attack in higher numbers
	}
	if (unitInfo->costsGold) {
		basePenaltyValue = (basePenaltyValue * 125) / 100; // Slightly heavier penalty if costs gold. Eg swordsmen vs clubmen, cavalry vs scouts
	}
	if (unitInfo->isMelee) {
		basePenaltyValue = (basePenaltyValue * 150) / 100;
	}
	if (basePenaltyValue > 100) { basePenaltyValue = 100; }

	unitInfo->weaknessVsPriests += basePenaltyValue;

	// Strength vs priests
	if (!unitInfo->isMelee && (unitInfo->speed >= 1) && (unitInfo->range > 6)) {
		unitInfo->strengthVsPriests += 10; // Range units are slightly better vs priests than melee
	} else {
		if (unitInfo->range >= 10) { // siege weapons, actually (archers have 7 max, we don't count range upgrades here)
			unitInfo->strengthVsPriests += 20;
		}
	}
	if (unitInfo->isMelee && (unitInfo->speed >= 1.0f)) {
		unitInfo->strengthVsPriests += 15;
		if (unitInfo->isMelee && (unitInfo->speed >= 1.5f)) {
			unitInfo->strengthVsPriests += 20; // even more for fast units
		}
	} else {
		unitInfo->strengthVsPriests -= 5;
	}
	if (unitInfo->displayedAttack >= 25) {
		unitInfo->strengthVsPriests += 5; // Can "one-shot" priests (not upgraded)
		if (unitInfo->displayedAttack >= 50) {
			unitInfo->strengthVsPriests += 10; // Can "one-shot" priests (upgraded). Total +10 (+ranged unit bonus, possibly)
		}
	}
	if (unitInfo->upgradedUnitDefLiving && unitInfo->upgradedUnitDefLiving->IsCheckSumValid()) {
		for (int i = 0; i < unitInfo->upgradedUnitDefLiving->attacksCount; i++) {
			if (unitInfo->upgradedUnitDefLiving->ptrAttacksList[i].classId == ATTACK_CLASS::CST_AC_CHARIOTS_ON_PRIESTS) {
				unitInfo->strengthVsPriests += 50;
			}
		}
	}
	
	switch (damageScore) {
	case 1:
		unitInfo->strengthVsPriests -= 15;
	case 2:
		unitInfo->strengthVsPriests -= 5;
	case 4:
		unitInfo->strengthVsPriests += 10;
	default:
		break;
	}
	if (unitInfo->strengthVsPriests > 100) { unitInfo->strengthVsPriests = 100; }
}

// Compute score fields for towers in unitInfo object.
void StrategyBuilder::ComputeScoresVsTower(PotentialUnitInfo *unitInfo) {
	unitInfo->strengthVsTowers = 0;
	int proportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->availableRelatedResearchesCount + unitInfo->unavailableRelatedResearchesCount);
	switch (unitInfo->unitAIType) {
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon:
		unitInfo->strengthVsTowers += 60;
		if (!unitInfo->hasUnavailableUpgrade) {
			unitInfo->strengthVsTowers += 20;
		}
		if (proportion >= 80) {
			unitInfo->strengthVsTowers += 15;
		}
		break;
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher:
		unitInfo->strengthVsTowers += 5;
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPriest:
		unitInfo->strengthVsTowers += 1;
		break;
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger:
		unitInfo->strengthVsTowers += 30; // cool in tool/bronze, weak later
		break;
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupPhalanx:
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupHeavyFootSoldier: // Heroes only ?!
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider:
		unitInfo->strengthVsTowers += 20; // cumulate with below melee classes
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupFootSoldier:
		unitInfo->strengthVsTowers += 30; // cumulate with below melee classes
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupMountedSoldier:
		unitInfo->strengthVsTowers += 10; // cumulate with below melee classes
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot:
		unitInfo->strengthVsTowers += 30;
		break;
	default:
		unitInfo->strengthVsTowers += 10; // Default
		break;
	}
}
