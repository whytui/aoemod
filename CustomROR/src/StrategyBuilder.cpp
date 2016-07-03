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
void StrategyBuilder::CollectPotentialUnitsInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, std::list<PotentialUnitInfo*> &unitInfos) {
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
			unitInfo->unitDefId = unitDefLiving->DAT_ID1;
			unitInfo->hitPoints = unitDefLiving->totalHitPoints;
			unitInfo->range = unitDefLiving->maxRange;
			unitInfo->isMelee = (unitInfo->range < 2);
			unitInfo->speed = unitDefLiving->speed; // // updated below if some researches update the speed value
			unitInfo->unitAIType = unitDefLiving->unitAIType;
			unitInfo->hasCivBonus = currentUnitHasTechTreeBonus;
			unitInfo->enabledByResearchId = researchIdThatEnablesUnit;
			unitInfo->ageResearchId = ageResearchIdThatEnablesUnit;
			unitInfo->unitName = unitDefLiving->ptrUnitName;
			unitInfo->hasUnavailableUpgrade = false;
			unitInfo->displayedAttack = unitDefLiving->displayedAttack;
			unitInfo->availableRelatedResearchesCount = 0; // updated below
			unitInfo->unavailableRelatedResearchesCount = 0; // updated below
			if (unitDefLiving->blastRadius > 0) {
				unitInfo->displayedAttack = (unitInfo->displayedAttack * 115) / 100; // Give a "fake" attack bonus for units with blast damage (15% ?)
			}
			unitInfo->conversionResistance = this->crInfo->GetConversionResistance(player->civilizationId, unitDefLiving->unitAIType);

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
							short int upgradeTargetUnitDefId = techDef->ptrEffects[i].UpgradeUnitGetTargetUnit();
							if ((upgradeTargetUnitDefId >= 0) && (techDef->ptrEffects[i].effectUnit == unitDefLiving->DAT_ID1)) {
								// We found an upgrade for our unit
								unitInfo->upgradesUnitDefId.push_back(upgradeTargetUnitDefId);
							}
							if (upgradeTargetUnitDefId == unitDefLiving->DAT_ID1) {
								// We found the base unit of our unit (our unit is an upgrade for some other unit)
								unitInfo->baseUnitDefId = techDef->ptrEffects[i].effectUnit;
							}
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
			unitInfos.push_back(unitInfo);
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
	std::list<PotentialUnitInfo*> potentialUnits;

	this->CollectPotentialUnitsInfo(player, potentialUnits);


	// Free allocated items
	for each (PotentialUnitInfo *unitInfo in potentialUnits)
	{
		delete unitInfo;
	}
	return shrPtr;
}
