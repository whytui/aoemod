#include "../include/cityPlan.h"

namespace CITY_PLAN {


// Computes (existing) building influence zone for farm placement map like values computation.
// existingBuilding must be a building (current building from loop, to take into account in map like values)
// Updates existingBldInfluenceZone with the influence distance we want to use for provided building (positions near building will be preferred)
// Updates skipThisBuilding to true if the building must be ignored for farm position computation. If true, existingBldInfluenceZone returned value should be ignored.
// Improvements depend on configuration (useEnhancedFeature and improveAILevel) + game difficulty level
void FixCityPlanFarmPlacement(AOE_STRUCTURES::STRUCT_UNIT_BASE *existingBuilding, long int &existingBldInfluenceZone, bool &skipThisBuilding) {
	skipThisBuilding = false;
	existingBldInfluenceZone = 3;
	if (!existingBuilding || !existingBuilding->IsCheckSumValidForAUnitClass()) { return; }

	// Default behavior / values
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef_base = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)existingBuilding->unitDefinition;
	assert(unitDef_base && unitDef_base->IsCheckSumValidForAUnitClass());
	if (!unitDef_base && !unitDef_base->IsCheckSumValidForAUnitClass()) { return; }

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());

	// Set some default values
	existingBldInfluenceZone = (int)unitDef_base->lineOfSight; // Default (from ROR code: use existing bld LOS)
	int difficultyLevel = settings->rgeGameOptions.difficultyLevel;
	bool useEnhancedFeature = CUSTOMROR::crInfo.configInfo.cityPlanLikeValuesEnhancement;
	bool isRelevantDropSite = (unitDef_base->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_GRANARY) ||
		(unitDef_base->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_FORUM); // Note: forager's storage sites are defined in empires.dat (Drop site in AGE3) => granary (68) and TC (109)

	if (!useEnhancedFeature) {
		return; // Feature is not enabled: exit and use defaults.
	}

	if (existingBuilding->ptrStructPlayer && existingBuilding->ptrStructPlayer->IsCheckSumValid() &&
		!CUSTOMROR::IsImproveAIEnabled(existingBuilding->ptrStructPlayer->playerId)) {
		return; // Do not improve if config says no AI improvement
	}

	// Improved farming is so good for AI we must apply this carefully, according to AI level, so AI doesn't get too strong in easy levels.
	if (difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASY) {
		return; // Easy (3) and easiest (4): use default behavior (exit and use defaults)
	}

	// Medium difficulty : just exclude some buildings (hard to find a compromise between no fix at all and very important fix)
	if (difficultyLevel == AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_MEDIUM) {
		// Do not add "like" value near storage pit because it disturbs other gatherers
		// Do not add "like" value near towers because they almost always are far from TC
		if ((unitDef_base->DAT_ID1 == AOE_CONST_FUNC::CST_UNITID_STORAGE_PIT) ||
			IsTower(unitDef_base)) {
			skipThisBuilding = true;
		}
		if (!isRelevantDropSite) {
			// Keep default influence zone for normal buildings (just decrease it by 1, to get a slightly better result)
			if (existingBldInfluenceZone > 3) {
				existingBldInfluenceZone--;
			}
		} else {
			// TC or granary: extend influence to get a slightly better result
			if (existingBldInfluenceZone < 5) {
				existingBldInfluenceZone = 5;
			}
			existingBldInfluenceZone++;
		}
		// No more improvements in medium level.
		return;
	}

	// HERE: feature is ON and we are in hard or hardest level.

	// Ignore all buildings that are not farming drop sites, there is no reason to build farms near them !!!
	if (!isRelevantDropSite) {
		skipThisBuilding = true;
		return;
	}

	// Loop (existing) building is a relevant drop site: set a correct influence zone
	// This influence zone must be high enough to cover most of the city (so we're sure to find a value if not much space is left)
	// ROR code will divide influence zone into 3 zones with progressive priorities (closest=best score)
	existingBldInfluenceZone = 11;
}


// Updates temp map like data for choosing a new building location according to existing buildings
// Improvements depend on configuration (useEnhancedFeature and improveAILevel) + game difficulty level
void ManageCityPlanOtherBuildingsImpact(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem, AOE_STRUCTURES::STRUCT_POSITION_INFO *TCPosition) {
	if (!infAI || !stratElem || !TCPosition) { return; }
	assert(infAI->IsCheckSumValid());
	assert(stratElem->IsCheckSumValid());
	if (!infAI->IsCheckSumValid() || !stratElem->IsCheckSumValid()) { return; }
	long int TC_x = TCPosition->posX;
	long int TC_y = TCPosition->posY;
	assert(TC_x >= 0);
	assert(TC_y >= 0);

	// Stop here is feature is not enabled in configuration
	if (!CUSTOMROR::crInfo.configInfo.cityPlanLikeValuesEnhancement) { return; }

	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	if (!settings || !settings->IsCheckSumValid()) { return; }
	if (!CUSTOMROR::IsImproveAIEnabled(infAI->commonAIObject.playerId) || (settings->rgeGameOptions.difficultyLevel >= GAME_DIFFICULTY_LEVEL::GDL_EASY)) {
		return;
	}

	// Collect useful context information : player, strategy element to build...
	AOE_STRUCTURES::STRUCT_AI *ai = infAI->ptrMainAI;
	assert(ai && ai->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_PLAYER *player = ai->ptrStructPlayer;
	assert(player != NULL);
	assert(player->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *globalStruct = GetGameGlobalStructPtr();
	assert(globalStruct != NULL);
	assert(globalStruct->IsCheckSumValid());

	AOE_STRUCTURES::STRUCT_MAP_TILE_VALUES *mapInfosStruct = ROR_pTempMapBuildLikeInfos;
	assert(mapInfosStruct != NULL);
	assert(mapInfosStruct->IsCheckSumValid());

	// Note about positions:
	// (tileX,tileY) tile positions are integer. So tile area is game (float) positions tileX<=x<=tileX+1 (same for Y).
	// For 2*2 building: buildingPos=(5,10) (for example) => tiles are (4,9) (4,10) (5,9) (5,10)
	// For 3*3 building: buildingPos=(5.5,10.5) (for example) => tiles are (4,9)->(6,9) (4,10)->(6,10) (4,11)->(6,11)
#ifdef _DEBUG
	static bool expBMP = false;
	if (expBMP) {
		_BITMAP::BitmapExporter::ExportDataColumnsAsBitmapUsingPalette("D:\\ctypln1.bmp", mapInfosStruct->arraySizeX, mapInfosStruct->arraySizeY, mapInfosStruct->ptrColsPtr, -1, NULL, 0);
	}
#endif

	bool hasHouse = false; // True if at least 1 house already exists
	bool hasTC = false; // True if at least 1 Town center already exists

	// 1) Loop on my buildings just to collect info
	for (long int index = 0; index < infAI->buildingUnits.usedElements; index++) {
		short int unitId = (short int)infAI->buildingUnits.unitIdArray[index];
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = globalStruct->ptrUnitPointersList[unitId];
		if (unit) {
			assert(unit->IsCheckSumValidForAUnitClass());
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *defUnit = unit->unitDefinition;
			if (defUnit) {
				assert(defUnit->IsCheckSumValidForAUnitClass());
				if (defUnit->DAT_ID1 == CST_UNITID_HOUSE) {
					// Mark if we found a house (later we will have special treatment for 1st house to build it faster)
					hasHouse = true;
				}
				if (defUnit->DAT_ID1 == CST_UNITID_FORUM) {
					// Mark if we found a TC
					hasTC = true;
				}
			}
		}
	}

	bool isFirstHouse = (!hasHouse && (stratElem->unitDAT_ID == CST_UNITID_HOUSE));
	bool isFarm = (stratElem->unitDAT_ID == CST_UNITID_FARM);

	// 2) Active loop on my buildings
	for (long int index = 0; index < infAI->buildingUnits.usedElements; index++) {
		short int unitId = (short int)infAI->buildingUnits.unitIdArray[index];
		AOE_STRUCTURES::STRUCT_UNIT_BUILDING *unit = (AOE_STRUCTURES::STRUCT_UNIT_BUILDING *)globalStruct->ptrUnitPointersList[unitId];
		if (unit) {
			assert(unit->IsCheckSumValid()); // IS a building (no other unit type)
			AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *defUnit = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)unit->unitDefinition;
			if (defUnit) {
				assert(defUnit->IsCheckSumValid()); // IS a building definition (no other unit type)
				// Make sure buildings are not all side by side with no space: if 1 side is "all blocked", make sure we don't build on opposite side.
				// 2 consecutive buildings in a row will be possible, but not more.
				int unitX = (int)unit->positionX;
				int unitY = (int)unit->positionY;
				char protectOppositeSidesValue = 1; // a very low like value, tiles won't be used unless AI is forced to (really no other room for new buildings?)
				if (stratElem->unitDAT_ID == CST_UNITID_HOUSE) {
					// It seems AI only cares about DISABLED tiles when placing houses ? Setting to 0 or 1 won't prevent from building there
					// Too bad because this call's goal is not to disable, but to set a very low priority :-(
					protectOppositeSidesValue = -1;
				}
				mapInfosStruct->ProtectBuildingOppositeSides(unitX, unitY, defUnit->sizeRadiusY, protectOppositeSidesValue);

				// Storage buildings: protect (more) their neighborhood, they need to be easily accessible by gatherers (or new villagers)
				// Moreover, TC and granary need space to build farms nearby

				// substract this value to X/Y positions that are > to center position to fix rounding.
				int unitRadiusDecreaseForHighPositions = 1 - (((int)(defUnit->sizeRadiusY * 2)) % 2); // 0 for values like 1.5 or 2.5, 1 for round values
				bool loopUnitIsDropSite = false;
				long int protectDistance = 0;
				long int protectValueLargeZone = -8;
				long int protectValueSmallZone = -10;
				if (defUnit->DAT_ID1 == CST_UNITID_FORUM) {
					loopUnitIsDropSite = true;
					protectDistance = 10;
					protectValueLargeZone = -9;
					protectValueSmallZone = -18;
				}
				if (defUnit->DAT_ID1 == CST_UNITID_GRANARY) {
					loopUnitIsDropSite = true;
					protectDistance = 8;
					protectValueLargeZone = -6;
					protectValueSmallZone = -10;
				}
				if (defUnit->DAT_ID1 == CST_UNITID_STORAGE_PIT) {
					loopUnitIsDropSite = true;
					protectDistance = 5;
					protectValueLargeZone = -6;
					protectValueSmallZone = -10;
				}
				if (loopUnitIsDropSite && isFirstHouse) {
					// Be less restrictive for first house to get a faster start
					protectDistance = 5;
					protectValueLargeZone = -4;
					protectValueSmallZone = -8;
				}
				// Protect drop sites
				if (loopUnitIsDropSite && !isFarm) {
					mapInfosStruct->AddMapLikeValueRectangle(unitX - protectDistance, unitY - protectDistance,
						unitX + protectDistance - unitRadiusDecreaseForHighPositions, unitY + protectDistance - unitRadiusDecreaseForHighPositions,
						protectValueLargeZone);
					protectDistance = (protectDistance * 2) / 3;
					// Even more in near positions
					mapInfosStruct->AddMapLikeValueRectangle(unitX - protectDistance, unitY - protectDistance,
						unitX + protectDistance - unitRadiusDecreaseForHighPositions, unitY + protectDistance - unitRadiusDecreaseForHighPositions,
						protectValueSmallZone);
				}
				// Farms : we don't need to add a bonus near TC/granary, ROR code will do it (see ManageCityMapLikeValueFarmPlacement(...) !)
			}
		}

		// 3) Is there enemy units around that could destroy the building before I build it ?
		assert(infAI->unitElemList != NULL);
		if (infAI->unitElemList && (settings->rgeGameOptions.difficultyLevel <= GAME_DIFFICULTY_LEVEL::GDL_HARD)) {
			// Hard/Hardest levels only
			long int mapSizeX = infAI->XMapSize;
			long int mapSizeY = infAI->YMapSize;
			if (globalStruct->gameMapInfo && globalStruct->gameMapInfo->IsCheckSumValid()) { // This source is more reliable than infAI
				mapSizeX = globalStruct->gameMapInfo->mapArraySizeX;
				mapSizeY = globalStruct->gameMapInfo->mapArraySizeY;
			}
			for (int i = 0; i < infAI->unitElemListSize; i++) {
				int unitPlayerId = infAI->unitElemList[i].playerId;
				int posX = infAI->unitElemList[i].posX;
				int posY = infAI->unitElemList[i].posY;
				if ((unitPlayerId > 0) && (unitPlayerId <= globalStruct->playerTotalCount) &&
					(player->diplomacyVSPlayers[unitPlayerId] >= PLAYER_DIPLOMACY_VALUES::CST_PDV_NEUTRAL) && // Neutral or enemy
					((posX >= 0) && (posX < mapSizeX) && (posY >= 0) && (posY < mapSizeY)) && // Valid position
					(infAI->unitElemList[i].unitId >= 0) && (unitPlayerId != player->playerId)) {
					AOE_STRUCTURES::STRUCT_PLAYER *unitPlayer = globalStruct->GetPlayerStruct(infAI->unitElemList[i].playerId);
					AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = unitPlayer->GetUnitDefBase(infAI->unitElemList[i].unitDATID);
					bool isTower = IsTower(unitDef);
					bool posIsVisible = PLAYER::IsFogVisibleForPlayer(player, posX, posY);
					bool isAggressive = isTower || IsNonTowerMilitaryUnit(unitDef->unitAIType);
					if (isAggressive && (posIsVisible || (unitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) || (unitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupUnused_Tower))) {
						AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *unitAttackable = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)globalStruct->GetUnitFromId(infAI->unitElemList[i].unitId);
						assert(!unitAttackable || unitAttackable->IsCheckSumValidForAUnitClass());
						AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDefAtk = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDef;
						if (unitAttackable && unitAttackable->DerivesFromAttackable()) {
							AOE_STRUCTURES::STRUCT_ACTION_BASE *action = AOE_STRUCTURES::GetUnitAction(unitAttackable);
							bool hasBlastDamage = unitDefAtk->HasBlastDamage();
							bool isBusy = false; // true if unit has already a combat task (if I can see that information)
							if (action && posIsVisible) { // do not cheat: if not visible, we can't know if unit is idle/busy, and with which kind of task
								isBusy = (action->actionTypeID == UNIT_ACTION_ID::CST_IAI_UNKNOWN_7) ||
									(action->actionTypeID == UNIT_ACTION_ID::CST_IAI_ATTACK) ||
									(action->actionTypeID == UNIT_ACTION_ID::CST_IAI_CONVERT);
							}
							if (!isBusy || hasBlastDamage) {
								// Unit is idle, or maybe moving, guarding a location, etc: it may be ready to attack any new construction
								assert(unitDef->DerivesFromAttackable());
								AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *unitDefAtt = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE*)unitDef;
								int unitRadiusDecreaseForHighPositions = 1 - (((int)(unitDef->sizeRadiusY * 2)) % 2); // 0 for values like 1.5 or 2.5, 1 for round values
								int distance = (int)unitDefAtt->maxRange;
								if (distance < 2) { distance = 2; }
								if (distance > 18) { distance = 18; }
								// Avoid building near this enemy unit
								mapInfosStruct->AddMapLikeValueRectangle(posX - distance, posY - distance,
									posX + distance - unitRadiusDecreaseForHighPositions, posY + distance - unitRadiusDecreaseForHighPositions,
									-10);
							}
						}
					}
				}
			}
		}

		// 4) Take into account previous failures ?
		
	}


#ifdef _DEBUG
	if (expBMP) {
		_BITMAP::BitmapExporter::ExportDataColumnsAsBitmapUsingPalette("D:\\ctypln2.bmp", mapInfosStruct->arraySizeX, mapInfosStruct->arraySizeY, mapInfosStruct->ptrColsPtr, -1, NULL, 0);
	}
#endif
}



}
