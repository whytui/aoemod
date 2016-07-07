#include "../include/StrategyBuilder.h"

using namespace STRATEGY;


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
			unitInfo->baseUnitDefLiving = unitDefLiving;
			unitInfo->upgradedUnitDefLiving = unitDefLiving; // Default: base unit. We'll update this later if necessary
			unitInfo->unitDefId = unitDefLiving->DAT_ID1;
			unitInfo->isBoat = IsWaterUnit(unitDefLiving->unitAIType);

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
								int totalCost = 0;
								if (curResearchDef->costUsed1) { totalCost += curResearchDef->costAmount1; }
								if (curResearchDef->costUsed2) { totalCost += curResearchDef->costAmount2; }
								if (curResearchDef->costUsed3) { totalCost += curResearchDef->costAmount3; }
								if (totalCost >= 1000) { unitInfo->isSuperUnit = true; } // Is there any better criteria ? not sure
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
			unitInfo->speedBase = unitInfo->baseUnitDefLiving->speed; // base speed value
			unitInfo->speedUpgraded = bestUpgradedUnit->speed; // updated speed value
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

			// Analyze costs
			for (int i = 0; i < 3; i++) {
				if ((unitDefLiving->costs[i].costType >= AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_FOOD) &&
					(unitDefLiving->costs[i].costType <= AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_GOLD) &&
					(unitDefLiving->costs[i].costPaid)) {
					unitInfo->totalResourceCost += unitDefLiving->costs[i].costAmount;
					if (unitDefLiving->costs[i].costType == AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_GOLD) {
						unitInfo->costsGold = true;
					}
					unitInfo->trainCosts[unitDefLiving->costs[i].costType] = unitDefLiving->costs[i].costAmount;
				}
			}
			unitInfo->weightedCost = 0;
			for (int i = 0; i < 4; i++) {
				unitInfo->weightedCost += GetWeightedCostValue(unitInfo->trainCosts[i], (RESOURCE_TYPES)i);
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
			// Collect some info on techs that improve the unit (speed...)
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
							bool isAttrModifier = ((techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) ||
								(techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) ||
								(techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT));
							bool affectsMe = ((techDef->ptrEffects[i].effectUnit == unitDefLiving->DAT_ID1) ||
								(techDef->ptrEffects[i].effectClass == unitDefLiving->unitAIType));
							if (isAttrModifier && affectsMe && (techDef->ptrEffects[i].effectAttribute == TECH_UNIT_ATTRIBUTES::TUA_SPEED)) {
								float value = techDef->ptrEffects[i].GetValue();
								if (techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_ADD) {
									unitInfo->speedUpgraded += value;
								}
								if (techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_SET) {
									unitInfo->speedUpgraded = value;
								}
								if (techDef->ptrEffects[i].effectType == TECH_DEF_EFFECTS::TDE_ATTRIBUTE_MODIFIER_MULT) {
									unitInfo->speedUpgraded = unitInfo->speedUpgraded * value;
								}
							}
						}
					}
				}
			}
			// Has special bonus ?
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
	this->SelectStrategyUnits();


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
		for (int i = 0; i < MC_COUNT; i++) {
			unitInfo->strengthVs[i] = 0;
			unitInfo->weaknessVs[i] = 0;
		}
		
		// availableRelatedResearchesProportion is a % value (0-100)
		int availableRelatedResearchesProportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->unavailableRelatedResearchesCount + unitInfo->availableRelatedResearchesCount);
		// A combination of speed and hit points
		float adjustedSpeed = unitInfo->speedUpgraded - 0.4f;
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
		unitInfo->damageScore = (int)damageScore;

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
			unitInfo->strengthVs[MC_MELEE] = 30;
			unitInfo->weaknessVs[MC_MELEE] = 20;
			if (availableRelatedResearchesProportion >= 80) {
				unitInfo->strengthVs[MC_MELEE] += 20;
			}
		} else {
			if (unitInfo->isMelee) {
				unitInfo->strengthVs[MC_MELEE] = 50;
				unitInfo->weaknessVs[MC_MELEE] = 10;
				if ((unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupMountedSoldier) ||
					(unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariot)){
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
		int availableRelatedResearchesProportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->unavailableRelatedResearchesCount + unitInfo->availableRelatedResearchesCount);
		int scoreForPriestResearches = (availableRelatedResearchesProportion < 80) ? 30 : 70;
		// Well-developed priests are good against priests (at least, allows converting back my units OR converting enemy units)
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
	if (unitSpeed < 1) {
		basePenaltyValue = 20;
	}
	if ((unitSpeed >= 1.0f) && (unitSpeed < 1.5f)) {
		basePenaltyValue = 10;
	}
	if (unitSpeed >= 1.5f) {
		basePenaltyValue = 2; // Fast units: can defend correctly against priests.
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
	if (!unitInfo->isMelee && (unitSpeed >= 1) && (unitInfo->range > 6)) {
		unitInfo->strengthVs[MC_PRIEST] += 10; // Range units are slightly better vs priests than melee
	} else {
		if (unitInfo->range >= 10) { // siege weapons, actually (archers have 7 max, we don't count range upgrades here)
			unitInfo->strengthVs[MC_PRIEST] += 20;
		}
	}
	if (unitInfo->isMelee && (unitSpeed >= 1.0f)) {
		unitInfo->strengthVs[MC_PRIEST] += 15;
		if (unitInfo->isMelee && (unitSpeed >= 1.5f)) {
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
	if (unitInfo->upgradedUnitDefLiving && unitInfo->upgradedUnitDefLiving->IsCheckSumValid()) {
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
		// in fact, few units have armors (not counting storage pit upgrades). So, fex units match this:
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
	int proportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->availableRelatedResearchesCount + unitInfo->unavailableRelatedResearchesCount);
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
	case GLOBAL_UNIT_AI_TYPES::TribeAIGroupHeavyFootSoldier: // Heroes only ?!
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
		if (unitInfo->damageScore == 2) {
			unitInfo->globalScore = 30;
		}
		if (unitInfo->damageScore == 3) {
			unitInfo->globalScore = 55;
		}
		if (unitInfo->damageScore == 4) {
			unitInfo->globalScore = 75;
		}
		if (unitInfo->isSuperUnit) {
			unitInfo->globalScore = 90;
		}
		if (unitInfo->hasCivBonus) {
			unitInfo->globalScore += 25; // note: we can go > 100 at this point, not an issue
		}
		if (unitInfo->hasUnavailableUpgrade) {
			unitInfo->globalScore = (unitInfo->globalScore * 85) / 100; // -15% if unit upgrades are missing
		}
		int numberOfAvailableUpgrades = unitInfo->upgradesUnitDefId.size();
		// TODO: example: assyrian cat is super unit (all unit upgrades) but missing 2 techs/5 => not selected
		int proportion = (unitInfo->availableRelatedResearchesCount * 100) / (unitInfo->availableRelatedResearchesCount + unitInfo->unavailableRelatedResearchesCount);
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

		// Adjust (a bit) according to costs: cheaper units are less strong (lower score), this is a bit unfair. Ex legion underestimated, heavy cats overestimated.
		int costProportion = (unitInfo->weightedCost * 100) / highestWeightedCost; // 0 to 100 value representing 1 unit cost.
		costProportion = costProportion / 10; // we limit "cost" impact to maximum 10% of global score. low value=low cost=better score
		costProportion = 100 - costProportion; // Get values in 90%-100% interval, where 100% is the better score. Very approximate due to int type, but the whole thing IS approximate.
		unitInfo->globalScore = unitInfo->globalScore * costProportion / 100;

		if (unitInfo->globalScore < 0) { unitInfo->globalScore = 0; }
		if (unitInfo->globalScore > 100) { unitInfo->globalScore = 100; }

		// Add a random part
		const int randomImpact = 30; // max + or - % value change from random part.
		int random = randomizer.GetRandomValue_normal_moreFlat(0, randomImpact * 2);
		random = random + 100 - randomImpact; // in [100-randomImpact, 100+randomImpact] (used with a multiplication, it gives a resulting randomImpact% adjustment + or -)
		unitInfo->globalScore = (unitInfo->globalScore * random) / 100;

		if (unitInfo->globalScore > 100) { unitInfo->globalScore = 100; }
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


// Recompute unit info bonuses that depend on comparison with other units
// upgradedUnit: if true, consider upgraded unit. if false, consider base unit
#pragma message("No very relevant for boats, do a specific method ?")
void StrategyBuilder::RecomputeComparisonBonuses(std::list<PotentialUnitInfo*> selectedUnits, bool waterUnit, bool upgradedUnit) {
	int totalStrengthVs[MC_COUNT];
	int tempStrengthVs[MC_COUNT];
	for (int i = 0; i < MC_COUNT; i++) { totalStrengthVs[i] = 0; tempStrengthVs[i] = 0; }
	for each (PotentialUnitInfo *unitInfo in selectedUnits)
	{
		if (unitInfo->isBoat == waterUnit) { // Do NOT exclude already selected units here
			totalStrengthVs[MC_PRIEST] += unitInfo->strengthVs[MC_PRIEST];
			totalStrengthVs[MC_FAST_RANGED] += unitInfo->strengthVs[MC_FAST_RANGED];
			totalStrengthVs[MC_TOWER] += unitInfo->strengthVs[MC_TOWER];
			totalStrengthVs[MC_MELEE] += unitInfo->strengthVs[MC_MELEE];
			totalStrengthVs[MC_SIEGE] += unitInfo->strengthVs[MC_SIEGE];
		}
	}
	for (int i = 0; i < MC_COUNT; i++) { tempStrengthVs[i] = totalStrengthVs[i]; }

	// If some category has few points, try to give priority to the unit that has the most points there...
	int categoriesWeaknessOrder[MC_COUNT];
	for (int i = 0; i < MC_COUNT; i++) { categoriesWeaknessOrder[i] = MC_NONE; }

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

	// Give bonus to units that are good against units that bother me (I don't have many units good against such units)
	int bonus = 100;
	int weaknessIndex = 0;
	for each (PotentialUnitInfo *unitInfo in selectedUnits)
	{
		if (unitInfo->isBoat == waterUnit) { // do not exclude already selected units here
			if (categoriesWeaknessOrder[weaknessIndex] == MC_NONE) { weaknessIndex++; } // should not happen as NONE is -1 value.
			int currentCategory = categoriesWeaknessOrder[weaknessIndex];
			int meanStrengthValue = (totalStrengthVs[currentCategory] / selectedUnits.size()); // medium strength Vs this category among all selected units from provided list
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
		if (!unitInfo->isSelected) {
			unitInfo->bonusForUsedResourceTypes = this->GetCostScoreRegardingCurrentSelection(unitInfo);
		}
	}
}


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
		this->RecomputeComparisonBonuses(preSelectedUnits, waterUnits, true);

		long int bestScore = 0;
		PotentialUnitInfo *bestUnit = NULL;
		const long int rareStrengthImpact = 30;
		const long int resourceRepartitionImpact = 15;
		const long int newRandomImpact = 8;
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
						if (unitInfo->hasCivBonus || unitInfo->hasSpecificAttackBonus) {
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

	// add "bonus" units to fill weaknesses, ex stone thrower vs towers, camel vs cavalry...
	MILITARY_CATEGORY firstAddedCategory = MC_NONE;
	for (int step = 1; step <= 2; step++) {
		int mediumStrength = currentCumulatedStrength / MC_COUNT;
		int worstStrength = currentTotalStrengths[currentWorstStrengthIndex];
		int proportion = (worstStrength * 100) / mediumStrength;
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
			if ( (i != firstAddedCategory) && (currentTotalStrengths[i] < currentWorstStrengthValue)) {
				currentWorstStrengthIndex = (MILITARY_CATEGORY)i;
				currentWorstStrengthValue = currentTotalStrengths[i];
			}
		}
	}
}


// Select which units are to be added in strategy, based on potentialUnitsList
// Requires that this->potentialUnitsList has already been filled
void StrategyBuilder::SelectStrategyUnitsForLandOrWater(bool waterUnits) {
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
			return;
		}
	}

	this->SelectStrategyUnitsFromPreSelection(preSelectedUnits, waterUnits);

	// Consider adding units with special bonus like camel/chariot if it helps for a specific need

	// Take care of early ages : add archers, axemen, slingers or scout (according to already available techs). TODO later (not in this method) ?
}

// Select which units are to be added in strategy, based on potentialUnitsList
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
	traceMessageHandler.WriteMessage("Strategy decision:");
	for each (PotentialUnitInfo *unitInfo in this->actuallySelectedUnits) {
		std::string msg = "Unit id=";
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *unit = NULL;
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
	}
#endif
}


// Add a unit to selection and updates some internal variables accordingly
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

// Search for a unit that can be added to strategy as "optional" (without full upgrades) to fight a detected weakness.
// waterUnits: if true, manage water units only. If false, manage land units only
void StrategyBuilder::AddOptionalUnitAgainstWeakness(MILITARY_CATEGORY weaknessCategory, bool waterUnits) {
	int bestScore = 0;
	PotentialUnitInfo *bestUnit = NULL;
	
	// Recompute strengths, for *base* units (not counting upgrades here)
	this->RecomputeComparisonBonuses(this->potentialUnitsList, waterUnits, false);

	for each (PotentialUnitInfo *unitInfo in this->potentialUnitsList)
	{
		// Ignore already selected units (and those NOT matching the land/water parameter)
		if (!unitInfo->isSelected && (unitInfo->isBoat == waterUnits)) {
			
			// Recompute costs bonus according to current selected units
			unitInfo->bonusForUsedResourceTypes = this->GetCostScoreRegardingCurrentSelection(unitInfo);
			int tmpScore = unitInfo->strengthVs[weaknessCategory];
			// Some hardcoded (but still generic) rules
			if ((weaknessCategory == MILITARY_CATEGORY::MC_TOWER) && (unitInfo->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon)) {
				if (unitInfo->upgradedUnitDefLiving->blastRadius > 0) {
					tmpScore = (tmpScore * 115) / 100; // catapult-like siege
				} else {
					tmpScore = (tmpScore * 105) / 100; // ballista-like siege vs towers
				}
			}
#pragma message("Should recompute scores using BASE units (without costly upgrades)")
			// Important: units may have a low score (<10) and % couldnt apply well: check this
			// TODO take care of relative cost ! Create a dedicated method GetRelativeCostScore with existing code above (in another method)
			// Units available without cost (or low research cost)
			bool lowCostActivation = false;
			if (unitInfo->enabledByResearchId == -1) {
				lowCostActivation = true;
			} else {
				ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *resDef = GetResearchDef(this->player, unitInfo->enabledByResearchId);
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
	}
}


// Returns true if the two classes are equals or very similar (infantry with phalanx, archer classes...)
static bool STRATEGY::AreSimilarClasses(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2) {
	if (class1 == class2) { return true; }
	int id1 = 0;
	switch (class1) {
		// Archer types
	case AOE_CONST_FUNC::TribeAIGroupArcher:
	case AOE_CONST_FUNC::TribeAIGroupChariotArcher:
	case AOE_CONST_FUNC::TribeAIGroupElephantArcher:
	case AOE_CONST_FUNC::TribeAIGroupHorseArcher:
		id1 = 1;
		break;
		// slow/middle-speed infantry
	case AOE_CONST_FUNC::TribeAIGroupFootSoldier:
	case AOE_CONST_FUNC::TribeAIGroupHeavyFootSoldier:
	case AOE_CONST_FUNC::TribeAIGroupPhalanx:
	case AOE_CONST_FUNC::TribeAIGroupElephantRider:
		id1 = 2;
		break;
		// Cavalry (fast melee)
	case AOE_CONST_FUNC::TribeAIGroupMountedSoldier:
	case AOE_CONST_FUNC::TribeAIGroupChariot:
		id1 = 3;
		break;
		// Other types : can't be regouped
	case AOE_CONST_FUNC::TribeAIGroupSiegeWeapon:
	case AOE_CONST_FUNC::TribeAIGroupPriest:
	case AOE_CONST_FUNC::TribeAIGroupWarBoat:
	case AOE_CONST_FUNC::TribeAIGroupSlinger:
	default:
		return false;
	}
	switch (class2) {
		// Archer types
	case AOE_CONST_FUNC::TribeAIGroupArcher:
	case AOE_CONST_FUNC::TribeAIGroupChariotArcher:
	case AOE_CONST_FUNC::TribeAIGroupElephantArcher:
	case AOE_CONST_FUNC::TribeAIGroupHorseArcher:
		return (id1 == 1);
		break;
		// slow/middle-speed infantry
	case AOE_CONST_FUNC::TribeAIGroupFootSoldier:
	case AOE_CONST_FUNC::TribeAIGroupHeavyFootSoldier:
	case AOE_CONST_FUNC::TribeAIGroupPhalanx:
	case AOE_CONST_FUNC::TribeAIGroupElephantRider:
		return (id1 == 2);
		break;
		// Cavalry (fast melee)
	case AOE_CONST_FUNC::TribeAIGroupMountedSoldier:
	case AOE_CONST_FUNC::TribeAIGroupChariot:
		return (id1 == 3);
		break;
	case AOE_CONST_FUNC::TribeAIGroupSiegeWeapon:
	case AOE_CONST_FUNC::TribeAIGroupPriest:
	case AOE_CONST_FUNC::TribeAIGroupWarBoat:
	case AOE_CONST_FUNC::TribeAIGroupSlinger:
	default:
		return false;
	}
}

