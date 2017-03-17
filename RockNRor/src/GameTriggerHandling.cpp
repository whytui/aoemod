#include "../include/GameTriggerHandling.h"

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace TRIGGER {
;



// Just execute a "standard" trigger. this only does the action part, does no check and does not manage other impacts.
// Do not call this directly ! (private)
void Trigger_JustDoAction(CR_TRIGGERS::crTrigger *trigger) {
	if (!trigger) { return; }
	if (!trigger->IsValid()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global != NULL);
	if (!global) { return; }
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_WRITE_CHAT) {
		AOE_METHODS::UI_BASE::CallWriteText(trigger->GetParameterValue(CR_TRIGGERS::KW_MESSAGE, ""));
	}
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_WRITE_CENTERED_MESSAGE) {
		AOE_METHODS::UI_BASE::CallWriteCenteredText(trigger->GetParameterValue(CR_TRIGGERS::KW_MESSAGE, ""));
	}
	long int actionPlayerId = -1;
	long int actionResourceId = -1;
	float actionResourceValue = 0;
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_RESOURCE) {
		// Not MP-compatible (to confirm)
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		actionResourceId = trigger->GetParameterValue(CR_TRIGGERS::KW_RESOURCE_ID, -1);
		actionResourceValue = trigger->GetParameterValue(CR_TRIGGERS::KW_RESOURCE_VALUE, 0.0);
		if ((actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount)) { return; }
		if ((actionResourceId < 0) || (actionResourceId > AOE_CONST_FUNC::CST_RES_ORDER_ALL_RELICS)) { return; }
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (player) {
			// Using game commands make this trigger MP-compatible
			float oldvalue = player->GetResourceValue((AOE_CONST_FUNC::RESOURCE_TYPES)actionResourceId);
			GAME_COMMANDS::CreateCmd_AddResource((short int)actionPlayerId, (short int)actionResourceId, actionResourceValue - oldvalue);
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ADD_RESOURCE) {
		// Not MP-compatible (to confirm)
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		actionResourceId = trigger->GetParameterValue(CR_TRIGGERS::KW_RESOURCE_ID, -1);
		actionResourceValue = trigger->GetParameterValue(CR_TRIGGERS::KW_RESOURCE_VALUE, 0.0);
		if ((actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount)) { return; }
		if ((actionResourceId < 0) || (actionResourceId >= AOE_CONST_FUNC::CST_RES_COUNT)) { return; }
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (player) {
			// Using game commands make this trigger MP-compatible
			GAME_COMMANDS::CreateCmd_AddResource((short int)actionPlayerId, (short int)actionResourceId, actionResourceValue);
		}
	}

	if ((trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_DISABLE_RESEARCH) ||
		(trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ENABLE_RESEARCH) ||
		(trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_APPLY_RESEARCH)
		) {
		// Not MP-compatible (to confirm)
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		long int actionResearchId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_RESEARCH_ID, -1);
		// This feature is not available for invalid players, nor for gaia player.
		if ((actionPlayerId <= 0) || (actionPlayerId >= global->playerTotalCount) || (actionResearchId < 0)) { return; }
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (player) {
			AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *pr = player->ptrResearchesStruct;
			if (!pr || !pr->researchStatusesArray || (pr->researchCount <= actionResearchId)) { return; }
			if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_DISABLE_RESEARCH) {
				// We allow to disable researches only if: they are not being researched and not already researched.
				if (pr->researchStatusesArray[actionResearchId].currentStatus < AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_BEING_RESEARCHED) {
					pr->researchStatusesArray[actionResearchId].currentStatus = AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_DISABLED;
				}
			}
			if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ENABLE_RESEARCH) {
				// We allow to enable researches only if: they are disabled or not available yet (waiting for requirement)
				if (pr->researchStatusesArray[actionResearchId].currentStatus < AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE) {
					pr->researchStatusesArray[actionResearchId].currentStatus = AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_AVAILABLE;
				}
			}
			if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_APPLY_RESEARCH) {
				// We allow to "apply" researches only if: they are not being researched and not already researched.
				if (pr->researchStatusesArray[actionResearchId].currentStatus < AOE_CONST_FUNC::RESEARCH_STATUSES::CST_RESEARCH_STATUS_BEING_RESEARCHED) {
					ApplyResearchForPlayer(player, actionResearchId);
				}
			}
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_APPLY_TECH) {
		// Not MP-compatible (to confirm)
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		long int actionTechId = trigger->GetParameterValue(CR_TRIGGERS::KW_TECHNOLOGY_ID, -1);
		if ((actionPlayerId >= global->playerTotalCount) || (actionTechId < 0)) { return; }
		if (actionPlayerId < -1) { return; } // invalid and not a joker (-1)
		// Set min and max playerid: by default they are the same (apply on only 1 player)
		long int minPlayerId = actionPlayerId;
		long int maxPlayerId = actionPlayerId;
		if (actionPlayerId == -1) { // -1 is a joker: apply on all players
			minPlayerId = 0;
			maxPlayerId = global->playerTotalCount - 1;
		}
		for (int i = minPlayerId; i <= maxPlayerId; i++) { // This for does only 1 loop in standard case (if NOT using a joker)
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(i);
			if (player) {
				ApplyTechnologyForPlayer(player, actionTechId);
			}
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_DIPL_CHANGE) {
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		long int targetPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_TARGET_PLAYER_ID, -1);
		long int diplomacyValue = trigger->GetParameterValue(CR_TRIGGERS::KW_DIPLOMACY_VALUE, -1);
		long int isMutual = trigger->GetParameterValue(CR_TRIGGERS::KW_MUTUAL, 0); // default = false (0)
		if ((actionPlayerId >= global->playerTotalCount) || (actionPlayerId < 0)) { return; }
		if ((targetPlayerId >= global->playerTotalCount) || (targetPlayerId < 0)) { return; }
		if (actionPlayerId == targetPlayerId) { return; }
		if ((diplomacyValue < 0) || (diplomacyValue > 3)) { return; }
		if (diplomacyValue == 2) { diplomacyValue = 3; } // 2 is unused, let's use it as "enemy" like 3.
		GAME_COMMANDS::CreateCmd_ChangeDiplomacy((short int)actionPlayerId, (short int)targetPlayerId, (PLAYER_DIPLOMACY_STANCES)diplomacyValue);
		if (isMutual) {
			GAME_COMMANDS::CreateCmd_ChangeDiplomacy((short int)targetPlayerId, (short int)actionPlayerId, (PLAYER_DIPLOMACY_STANCES)diplomacyValue);
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_DISABLE_TRIGGER) {
		if (ROCKNROR::crInfo.triggerSet) {
			char *targetTriggerName = trigger->GetParameterValue(CR_TRIGGERS::KW_TARGET_TRIGGER_NAME, "");
			CR_TRIGGERS::crTrigger *otherTrigger = ROCKNROR::crInfo.triggerSet->GetTrigger(targetTriggerName);
			if (otherTrigger) {
				otherTrigger->enabled = false;
			}
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_TERRAIN) {
		// Not MP-compatible (to confirm)
		long int minPosX, minPosY, maxPosX, maxPosY, elevationLevel, terrainId;
		minPosX = trigger->GetParameterValue(CR_TRIGGERS::KW_MIN_POS_X, -1);
		minPosY = trigger->GetParameterValue(CR_TRIGGERS::KW_MIN_POS_Y, -1);
		maxPosX = trigger->GetParameterValue(CR_TRIGGERS::KW_MAX_POS_X, -1);
		maxPosY = trigger->GetParameterValue(CR_TRIGGERS::KW_MAX_POS_Y, -1);
		elevationLevel = trigger->GetParameterValue(CR_TRIGGERS::KW_ELEVATION_LEVEL, -1);
		terrainId = trigger->GetParameterValue(CR_TRIGGERS::KW_TERRAIN_ID, -1);
		// Test invalid coordinates or missing information cases
		if ((minPosX < 0) || (minPosY < 0) || (minPosX > 255) || (minPosY > 255) ||
			(maxPosX < 0) || (maxPosY < 0) || (maxPosX > 255) || (maxPosY > 255) ||
			(minPosX > maxPosX) || (minPosY > maxPosY) ||
			((elevationLevel < 0) && (terrainId) < 0)) {
			return;
		}
		AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		assert(gameMapInfo && gameMapInfo->IsCheckSumValid());
		if (!gameMapInfo || !gameMapInfo->IsCheckSumValid()) { return; }
		if ((minPosX >= gameMapInfo->mapArraySizeX) || (minPosY >= gameMapInfo->mapArraySizeY) ||
			(maxPosX >= gameMapInfo->mapArraySizeX) || (maxPosX >= gameMapInfo->mapArraySizeX)
			) {
			return;
		}
		for (int x = minPosX; x <= maxPosX; x++) {
			for (int y = minPosY; y <= maxPosY; y++) {
				AOE_STRUCTURES::STRUCT_GAME_MAP_TILE_INFO *tile = gameMapInfo->GetTileInfo(x, y);
				if (tile) {
					// Note: tile's "Set" methods are both secure (check the value is in bounds)
					if (elevationLevel >= 0) { tile->terrainData.SetAltitude((char)elevationLevel); } // TO DO: this does not work well in-game + side effects
					if (terrainId >= 0) { tile->terrainData.SetTerrainId((char)terrainId); } // Does not update minimap, does not refresh map immediatly
				}
			}
		}
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_MODIFY_UNIT) {
		// Not MP-compatible (to confirm)
		long int actionUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_ID, -1);
		if (actionUnitId < 0) { return; }
		float unitHP_add = trigger->GetParameterValue(CR_TRIGGERS::KW_ADD_HP, (float)0);
		float unitHP_set = trigger->GetParameterValue(CR_TRIGGERS::KW_SET_HP, (float)-1);
		float resourceAmount_add = trigger->GetParameterValue(CR_TRIGGERS::KW_ADD_RESOURCE, (float)0);
		float resourceAmount_set = trigger->GetParameterValue(CR_TRIGGERS::KW_SET_RESOURCE, (float)-1);
		char orientation = (char)trigger->GetParameterValue(CR_TRIGGERS::KW_UNIT_ORIENTATION_INDEX, -1);
		char graphicsAge = (char)trigger->GetParameterValue(CR_TRIGGERS::KW_UNIT_GRAPHICS_AGE, -1);
		float posX = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_X, (float)-1);
		float posY = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_Y, (float)-1);
		long int newOwnerId = trigger->GetParameterValue(CR_TRIGGERS::KW_OWNER_PLAYER_ID, -1);

		AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(actionUnitId);
		if (!targetUnit || !targetUnit->IsCheckSumValidForAUnitClass()) { return; }
		if (unitHP_set >= 0) {
			targetUnit->remainingHitPoints = unitHP_set;
		}
		if (unitHP_add != 0) {
			targetUnit->remainingHitPoints += unitHP_add;
		}
		if (resourceAmount_set >= 0) {
			targetUnit->resourceValue = resourceAmount_set;
		}
		if (resourceAmount_add != 0) {
			targetUnit->resourceValue += resourceAmount_add;
		}
		//if ((graphicsAge >= 0) && (graphicsAge <= 2)) { // TO DO (does not work) => use orientation ?
		//targetUnit->currentGraphicsDamageIndex = graphicsAge;
		//}
		if ((orientation >= 0) && (orientation <= 7)) {
			targetUnit->orientationIndex = orientation;
		}
		if ((posX >= 0) && (posY >= 0)) { // This has important side effect, not recommended !!! TO DO.
			AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
			if ((posX < gameMapInfo->mapArraySizeX) && (posY < gameMapInfo->mapArraySizeY)) {
				targetUnit->positionX = posX;
				targetUnit->positionY = posY;
			}
		}
		// Last but not least: change unit owner !
		if ((newOwnerId >= 0) && (newOwnerId < global->playerTotalCount)) {
			AOE_METHODS::UNIT::ChangeUnitOwner(targetUnit, GetPlayerStruct(newOwnerId));
		}
	}

	// Spawn a unit
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ADD_UNIT_INSTANCE) {
		// Not MP-compatible (to confirm)
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -2);
		long int actionUnitDefId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_DEF_ID, -1);
		float posX = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_X, (float)-1);
		float posY = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_Y, (float)-1);
		long int useInitialBuildingStatus = trigger->GetParameterValue(CR_TRIGGERS::KW_USE_INITIAL_BUILDING_STATUS, 0);
		if ((actionUnitDefId < 0) || (actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount) || (posX < 0) || (posY < 0)) {
			return;
		}
		AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		assert(gameMapInfo && gameMapInfo->IsCheckSumValid());
		if ((posX >= gameMapInfo->mapArraySizeX) || (posY >= gameMapInfo->mapArraySizeY)) { return; }
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(actionPlayerId);
		if (!player || !player->IsCheckSumValid()) { return; }
		if (actionUnitDefId >= player->structDefUnitArraySize) { return; }
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = player->ptrStructDefUnitTable[actionUnitDefId];
		if (!unitDef || !unitDef->IsCheckSumValidForAUnitClass()) { return; }

		bool result = false;
		if (useInitialBuildingStatus) {
			if (unitDef->unitType != GLOBAL_UNIT_TYPES::GUT_BUILDING) {
				traceMessageHandler.WriteMessage("ERROR: 'use initial building status' option can only be used with building units.");
				return;
			}
			// To create non-constructed buildings:
			result = GAME_COMMANDS::CreateCmd_Build_withoutBuilder(player->playerId, unitDef->DAT_ID1, posX, posY + 5);
		} else {
			result = GAME_COMMANDS::CreateCmd_CompleteBuild(player->playerId, unitDef->DAT_ID1, posX, posY);
		}
		if (!result) {
			traceMessageHandler.WriteMessage("ERROR: Unit instance creation failed.");
			return;
		}
	}

	// Make unique (create dedicated unit definition)
	// Only supported for living units and buildings.
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_MAKE_UNIQUE) {
		// Not MP-compatible (to confirm)
		long int actionUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_ID, -1);
		if (actionUnitId < 0) { return; }
		char *newUnitName = trigger->GetParameterValue(CR_TRIGGERS::KW_UNIT_NAME, "");
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(actionUnitId);
		if (!unitBase || !unitBase->IsCheckSumValidForAUnitClass()) { return; }
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = unitBase->unitDefinition;
		if (!unitDefBase || !unitDefBase->IsCheckSumValidForAUnitClass()) { return; }
		// Only living units & buildings are allowed
		if (!unitDefBase->DerivesFromTrainable()) {
			traceMessageHandler.WriteMessage("ERROR: 'Make unique' triggers only support living units and buildings.");
			return;
		}
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *unitDefLiving = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unitDefBase;
		if (!unitDefLiving || !unitDefLiving->DerivesFromTrainable()) { return; }

		AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *unitLiving = (AOE_STRUCTURES::STRUCT_UNIT_TRAINABLE *)unitBase;
		if (!unitLiving || !unitLiving->DerivesFromTrainable()) { return; }

		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefToFree = NULL; // Only used for units that have a temporary unitDef (converted units)
		if (unitLiving->hasDedicatedUnitDef) {
			unitDefToFree = unitDefBase; // previous unitDef was (also) a temporary one. We will free it.
		}

		// Save HP % before modification to preserve it (example: if maxHP changes from 20 to 40, if old unit had 10/20 HP, new one should have 20/40 !)
		float HPProportion = unitLiving->remainingHitPoints / ((float)unitDefLiving->totalHitPoints);

		// Create the new (temporary) unit definition.
		AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *newUnitDefBuilding = NULL;
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *newUnitDefLiving = NULL;
		if (unitLiving->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) {
			// Our unit is a building
			//unitDefBuilding = (AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)unitDefBase;
			assert(((AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)unitDefBase)->IsCheckSumValid());
			newUnitDefBuilding = CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING>((AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *)unitDefBase);
			newUnitDefLiving = (AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE*)newUnitDefBuilding;
		} else {
			// We actually have a living unit
			newUnitDefLiving = CopyUnitDefToNew<AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE>((AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *)unitDefBase);
		}

		// Here newUnitDefLiving is ALWAYS our new unitDef (cast as living=parent class if needed)
		// newUnitDefBuilding is only non-NULL if our unit is actually a building. It can be used if we modify building-specific attributes.

		// Apply supplied properties to new unit def.
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_TOTAL_HP)) {
			newUnitDefLiving->totalHitPoints = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_TOTAL_HP, 1);
			// Fix actual Hit points
			unitLiving->remainingHitPoints = newUnitDefLiving->totalHitPoints * HPProportion;
		}
		bool updateLOS = false;
		float suppliedRange = -1;
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_RANGE)) {
			updateLOS = true;
			suppliedRange = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_RANGE, -1);
			if (suppliedRange >= 0) {
				//newUnitDefLiving->lineOfSight = suppliedRange; // See below. LOS can't be changed this way.
				float upgradeRelatedRange = newUnitDefLiving->maxRange - newUnitDefLiving->displayedRange;
				if (upgradeRelatedRange < 0) { upgradeRelatedRange = 0; }
				newUnitDefLiving->displayedRange = suppliedRange - upgradeRelatedRange; // the figure before "+". For a "7+1" range, this corresponds to 7.
				newUnitDefLiving->maxRange = suppliedRange; // Total range. For a "7+1" range, this corresponds to 8.
			}
		}

		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_SPEED)) {
			newUnitDefLiving->speed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_SPEED, newUnitDefLiving->speed);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_ROTATION_SPEED)) {
			newUnitDefLiving->rotationSpeed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_ROTATION_SPEED, newUnitDefLiving->rotationSpeed);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_WORK_RATE)) {
			newUnitDefLiving->workRate = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_WORK_RATE, newUnitDefLiving->workRate);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_ACCURACY_PERCENT)) {
			newUnitDefLiving->accuracyPercent = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_ACCURACY_PERCENT, (long int)newUnitDefLiving->accuracyPercent);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_DISPLAYED_ARMOR)) {
			newUnitDefLiving->displayedArmor = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_DISPLAYED_ARMOR, (long int)newUnitDefLiving->displayedArmor);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_DISPLAYED_ATTACK)) {
			newUnitDefLiving->displayedAttack = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_DISPLAYED_ATTACK, (long int)newUnitDefLiving->displayedAttack);
		}
		if (trigger->IsParameterDefined(CR_TRIGGERS::KW_VISIBLE_IN_FOG)) {
			newUnitDefLiving->visibleInFog = (char)trigger->GetParameterValue(CR_TRIGGERS::KW_VISIBLE_IN_FOG, (long int)newUnitDefLiving->visibleInFog);
			if (newUnitDefLiving->visibleInFog != 0) {
				newUnitDefLiving->visibleInFog = 1; // just in case.
			}
		}

		// Now associate new unit definition to our unit
		unitLiving->unitDefinition = newUnitDefLiving;
		unitLiving->hasDedicatedUnitDef = 1; // don't forget to set this flag so that ROR correctly frees everything later.
		// Force use provided name (if provided)
		if (*newUnitName != 0) {
			if (newUnitDefLiving->ptrUnitName) {
				AOEFree(newUnitDefLiving->ptrUnitName);
			}
			char *newAllocatedName = (char *)AOEAlloc(0x30);
			strcpy_s(newAllocatedName, 0x30, newUnitName);
			newUnitDefLiving->ptrUnitName = newAllocatedName;
			newUnitDefLiving->languageDLLID_Name = 0;
			newUnitDefLiving->languageDLLID_Creation = 0;
		}

		// Free previous unitdef if unit already had hasDedicatedUnitDef==1
		if (unitDefToFree) {
			AOEFree(unitDefToFree);
		}

		// Handle visibility
		if (updateLOS && (suppliedRange >= 0)) {
			AOE_STRUCTURES::ChangeLOSForUniqueUnit(unitLiving, suppliedRange);
		}
	}

	// A big one: create unit def (actually, duplicate + customize)
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ADD_UNIT_DEF) {
		// Unit definitions MUST always match for *all* players, or the game may crash.
		//actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		long int sourceUnitDefId = trigger->GetParameterValue(CR_TRIGGERS::KW_FROM_UNIT_DEF_ID, -2);
		char *newUnitName = trigger->GetParameterValue(CR_TRIGGERS::KW_UNIT_NAME, "");
		if ((sourceUnitDefId < 0) /*|| (actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount)*/) {
			return;
		}

		// A first loop to run checks
		int commonArraySize = -1;
		for (int loopPlayerId = 0; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(loopPlayerId);
			if (!player || !player->IsCheckSumValid()) { return; }
			// Make sure that for all players source UnitDefId exists
			if (sourceUnitDefId >= player->structDefUnitArraySize) { return; }
			// Make sure source unitDef exists for all players (remember: it can exist and be disabled, like gaia units for other civs)
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *sourceUnitDef = player->ptrStructDefUnitTable[sourceUnitDefId];
			if (!sourceUnitDef || !sourceUnitDef->IsCheckSumValidForAUnitClass()) { return; }
			// Make sure maximum unitDefId is the same for all players (synchronized)
			if (commonArraySize == -1) {
				commonArraySize = player->structDefUnitArraySize; // first loop: initialize.
			} else {
				// after first loop: make sure we always get the same value
				if (player->structDefUnitArraySize != commonArraySize) {
					traceMessageHandler.WriteMessage("Player unit definitions are not synchronized for all players, cannot add.");
					return;
				}
			}
		}
		assert(commonArraySize >= 0);
		if (commonArraySize < 0) {
			traceMessageHandler.WriteMessage("ERROR: commonArraySize<0. How is this possible ?");
			return;
		}
		int newUnitDefId = commonArraySize;

		// A second loop to do the job
		for (int loopPlayerId = 0; loopPlayerId < global->playerTotalCount; loopPlayerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(loopPlayerId);
			assert(player && player->IsCheckSumValid()); // should not happen, already checked
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *sourceUnitDefBase = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE*) player->ptrStructDefUnitTable[sourceUnitDefId];
			assert(sourceUnitDefBase && sourceUnitDefBase->IsCheckSumValidForAUnitClass()); // should not happen, already checked
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *newUnitDefBase = CopyUnitDefToNewUsingGoodClass(sourceUnitDefBase);
			if (!newUnitDefBase) {
				traceMessageHandler.WriteMessage("ERROR: Failed to copy unit definition");
				return;
			}
			assert(newUnitDefId == player->structDefUnitArraySize); // Note: structDefUnitArraySize has not been updated yet
			assert(newUnitDefBase && newUnitDefBase->IsCheckSumValidForAUnitClass());
			assert(newUnitDefBase->unitType == sourceUnitDefBase->unitType);
			// Add in player unit definition table
			short int tmpAddedUnitDefId = AddUnitDefToPlayer(player, newUnitDefBase);
			assert(newUnitDefId == player->structDefUnitArraySize - 1); // Note: structDefUnitArraySize has been updated now
			if (tmpAddedUnitDefId == -1) {
				assert(false);
				traceMessageHandler.WriteMessage("An error occurred while adding new unit definition");
				return;
			}
			if (newUnitDefId != tmpAddedUnitDefId) {
				assert(false);
				traceMessageHandler.WriteMessage("Inconsistent unitDefID while adding new unit definition");
				return;
			}

			// Force use provided name (if provided)
			if (*newUnitName != 0) {
				if (newUnitDefBase->ptrUnitName) {
					AOEFree(newUnitDefBase->ptrUnitName);
				}
				char *newAllocatedName = (char *)AOEAlloc(0x30);
				strcpy_s(newAllocatedName, 0x30, newUnitName);
				newUnitDefBase->ptrUnitName = newAllocatedName;
				newUnitDefBase->languageDLLID_Name = 0;
				newUnitDefBase->languageDLLID_Creation = 0;
			}

			// Apply custom modifications from trigger data
			// We manipulate here objects from different possible classes. Make sure we work on members that DO exist in our object instance !
			bool isType50OrChild = newUnitDefBase->DerivesFromAttackable();
			bool isFlagOrChild = newUnitDefBase->DerivesFromFlag();
			bool isMovableOrChild = newUnitDefBase->DerivesFromMovable();
			bool isCommandableOrChild = newUnitDefBase->DerivesFromCommandable();
			AOE_STRUCTURES::STRUCT_UNITDEF_FLAG *newUnitDef_flag = NULL;
			AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE *newUnitDef_movable = NULL;
			AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *newUnitDef_commandable = NULL;
			AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *newUnitDef_type50 = NULL;
			if (isFlagOrChild) { newUnitDef_flag = (AOE_STRUCTURES::STRUCT_UNITDEF_FLAG *)newUnitDefBase; }
			if (isMovableOrChild) { newUnitDef_movable = (AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE *)newUnitDefBase; }
			if (isCommandableOrChild) { newUnitDef_commandable = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)newUnitDefBase; }
			if (isType50OrChild) { newUnitDef_type50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)newUnitDefBase; }

			// Apply supplied properties to new unit def.
			if (trigger->IsParameterDefined(CR_TRIGGERS::KW_TOTAL_HP)) {
				newUnitDefBase->totalHitPoints = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_TOTAL_HP, 1);
			}
			if (trigger->IsParameterDefined(CR_TRIGGERS::KW_RANGE)) {
				float suppliedRange = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_RANGE, -1);
				if (suppliedRange >= 0) {
					newUnitDefBase->lineOfSight = suppliedRange;
					if (isType50OrChild && newUnitDef_type50) {
						float upgradeRelatedRange = newUnitDef_type50->maxRange - newUnitDef_type50->displayedRange;
						if (upgradeRelatedRange < 0) { upgradeRelatedRange = 0; }
						newUnitDef_type50->displayedRange = suppliedRange - upgradeRelatedRange; // the figure before "+". For a "7+1" range, this corresponds to 7.
						newUnitDef_type50->maxRange = suppliedRange; // Total range. For a "7+1" range, this corresponds to 8.
					}
				}
			}

			if (isFlagOrChild && newUnitDef_flag && (trigger->IsParameterDefined(CR_TRIGGERS::KW_SPEED))) {
				newUnitDef_flag->speed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_SPEED, newUnitDef_flag->speed);
			}
			if (isMovableOrChild && newUnitDef_movable && (trigger->IsParameterDefined(CR_TRIGGERS::KW_ROTATION_SPEED))) {
				newUnitDef_movable->rotationSpeed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_ROTATION_SPEED, newUnitDef_movable->rotationSpeed);
			}
			if (isCommandableOrChild && newUnitDef_commandable && (trigger->IsParameterDefined(CR_TRIGGERS::KW_WORK_RATE))) {
				newUnitDef_commandable->workRate = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_WORK_RATE, newUnitDef_commandable->workRate);
			}
			if (isType50OrChild && newUnitDef_type50 && (trigger->IsParameterDefined(CR_TRIGGERS::KW_ACCURACY_PERCENT))) {
				newUnitDef_type50->accuracyPercent = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_ACCURACY_PERCENT, (long int)newUnitDef_type50->accuracyPercent);
			}
			if (isType50OrChild && newUnitDef_type50 && (trigger->IsParameterDefined(CR_TRIGGERS::KW_DISPLAYED_ARMOR))) {
				newUnitDef_type50->displayedArmor = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_DISPLAYED_ARMOR, (long int)newUnitDef_type50->displayedArmor);
			}
			if (isType50OrChild && newUnitDef_type50 && (trigger->IsParameterDefined(CR_TRIGGERS::KW_DISPLAYED_ATTACK))) {
				newUnitDef_type50->displayedAttack = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_DISPLAYED_ATTACK, (long int)newUnitDef_type50->displayedAttack);
			}
			if (trigger->IsParameterDefined(CR_TRIGGERS::KW_VISIBLE_IN_FOG)) {
				newUnitDefBase->visibleInFog = (char)trigger->GetParameterValue(CR_TRIGGERS::KW_VISIBLE_IN_FOG, (long int)newUnitDefBase->visibleInFog);
				if (newUnitDefBase->visibleInFog != 0) {
					newUnitDefBase->visibleInFog = 1; // just in case.
				}
			}
		}

		// Update other triggers with new unitDefId
		CR_TRIGGERS::crTrigger *currentTrg = ROCKNROR::crInfo.triggerSet->IteratorFirst();
		while (currentTrg) {
			// Search for enabled triggers for same player, that want to spawn a unit...
			if (currentTrg->enabled && (currentTrg->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_ADD_UNIT_INSTANCE)) {
				// Find if the spawn unit action uses "unit_name" as reference
				char *otherTrgUnitName = currentTrg->GetParameterValue(CR_TRIGGERS::KW_UNIT_NAME, "");
				if (strcmp(newUnitName, otherTrgUnitName) == 0) {
					// Found a trigger which is based on the unit name we just created: set the unitDefId value.
					currentTrg->SetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_DEF_ID, newUnitDefId);
				}
			}
			currentTrg = ROCKNROR::crInfo.triggerSet->IteratorNext();
		}

		trigger->enabled = false; // Creating unit def is not repeatable. Force disable.
	}

	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_MODIFY_UNIT_DEF) {
		actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1); // -1 means all players
		long int sourceUnitDefId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_DEF_ID, -2);
		if ((sourceUnitDefId < 0) || (actionPlayerId < -1) || (actionPlayerId >= global->playerTotalCount)) { return; }

		// A first loop to run checks
		int commonArraySize = -1;
		int minPlayerId = actionPlayerId; // Default: no actual loop (only 1 player)
		int maxPlayerId = actionPlayerId;
		if (actionPlayerId == -1) { // joker: actual loop on all players
			minPlayerId = 0;
			maxPlayerId = global->playerTotalCount - 1;
		}
		for (int loopPlayerId = minPlayerId; loopPlayerId <= maxPlayerId; loopPlayerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(loopPlayerId);
			if (!player || !player->IsCheckSumValid()) { return; }
			// Make sure that for all players source UnitDefId exists
			if (sourceUnitDefId >= player->structDefUnitArraySize) { return; }
			// Make sure source unitDef exists for all players (remember: it can exist and be disabled, like gaia units for other civs)
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *sourceUnitDef = player->ptrStructDefUnitTable[sourceUnitDefId];
			if (!sourceUnitDef || !sourceUnitDef->IsCheckSumValidForAUnitClass()) { return; }
			// Make sure maximum unitDefId is the same for all players (synchronized)
			if (commonArraySize == -1) {
				commonArraySize = player->structDefUnitArraySize; // first loop: initialize.
			} else {
				// after first loop: make sure we always get the same value
				if (player->structDefUnitArraySize != commonArraySize) {
					traceMessageHandler.WriteMessage("Player unit definitions are not synchronized for all players, cannot add.");
					return;
				}
			}
		}
		assert(commonArraySize >= 0);
		if (commonArraySize < 0) {
			traceMessageHandler.WriteMessage("ERROR: commonArraySize<0. How is this possible ?");
			return;
		}

		char *newUnitName = trigger->GetParameterValue(CR_TRIGGERS::KW_UNIT_NAME, "");

		// A second loop to do the job
		for (int loopPlayerId = minPlayerId; loopPlayerId <= maxPlayerId; loopPlayerId++) {
			AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(loopPlayerId);
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = player->ptrStructDefUnitTable[sourceUnitDefId];
			// Force use provided name (if provided)
			if (*newUnitName != 0) {
				if (unitDefBase->ptrUnitName) {
					AOEFree(unitDefBase->ptrUnitName);
				}
				char *newAllocatedName = (char *)AOEAlloc(0x30);
				strcpy_s(newAllocatedName, 0x30, newUnitName);
				unitDefBase->ptrUnitName = newAllocatedName;
				unitDefBase->languageDLLID_Name = 0;
				unitDefBase->languageDLLID_Creation = 0;
			}

			// Apply custom modifications from trigger data
			// We manipulate here objects from different possible classes. Make sure we work on members that DO exist in our object instance !
			bool isType50OrChild = unitDefBase->DerivesFromAttackable();
			bool isFlagOrChild = unitDefBase->DerivesFromFlag();
			bool isMovableOrChild = unitDefBase->DerivesFromMovable();
			bool isCommandableOrChild = unitDefBase->DerivesFromCommandable();
			AOE_STRUCTURES::STRUCT_UNITDEF_FLAG *newUnitDef_flag = NULL;
			AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE *newUnitDef_movable = NULL;
			AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *newUnitDef_commandable = NULL;
			AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *newUnitDef_type50 = NULL;
			if (isFlagOrChild) { newUnitDef_flag = (AOE_STRUCTURES::STRUCT_UNITDEF_FLAG *)unitDefBase; }
			if (isMovableOrChild) { newUnitDef_movable = (AOE_STRUCTURES::STRUCT_UNITDEF_MOVABLE *)unitDefBase; }
			if (isCommandableOrChild) { newUnitDef_commandable = (AOE_STRUCTURES::STRUCT_UNITDEF_COMMANDABLE *)unitDefBase; }
			if (isType50OrChild) { newUnitDef_type50 = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)unitDefBase; }

			// Apply supplied properties to new unit def.
			if (trigger->IsParameterDefined(CR_TRIGGERS::KW_TOTAL_HP)) {
				float oldTotalHitPoints = (float)unitDefBase->totalHitPoints;
				unitDefBase->totalHitPoints = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_TOTAL_HP, 1);
				// Here we modify an existing unit definition: there might be existing unit instances. We have to update them accordingly (like after a HP upgrade from some tech)
				if (player->ptrCreatableUnitsListLink && player->ptrCreatableUnitsListLink->IsCheckSumValid()) {
					AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *curElem = player->ptrCreatableUnitsListLink->lastListElement;
					while (curElem) {
						if (curElem->unit && curElem->unit->IsCheckSumValidForAUnitClass() && curElem->unit->unitDefinition &&
							curElem->unit->unitDefinition->IsCheckSumValidForAUnitClass() &&
							(curElem->unit->unitDefinition->DAT_ID1 == unitDefBase->DAT_ID1)) {
							float hpProp = curElem->unit->remainingHitPoints / oldTotalHitPoints;
							curElem->unit->remainingHitPoints = hpProp * (float)unitDefBase->totalHitPoints;
						}
						curElem = curElem->previousElement;
					}
				}
			}
			if (trigger->IsParameterDefined(CR_TRIGGERS::KW_RANGE)) {
				float suppliedRange = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_RANGE, -1);
				if (suppliedRange >= 0) {
					unitDefBase->lineOfSight = suppliedRange;
					if (isType50OrChild && newUnitDef_type50) {
						float upgradeRelatedRange = newUnitDef_type50->maxRange - newUnitDef_type50->displayedRange;
						if (upgradeRelatedRange < 0) { upgradeRelatedRange = 0; }
						newUnitDef_type50->displayedRange = suppliedRange - upgradeRelatedRange; // the figure before "+". For a "7+1" range, this corresponds to 7.
						newUnitDef_type50->maxRange = suppliedRange; // Total range. For a "7+1" range, this corresponds to 8.
					}
				}
			}
			if (isFlagOrChild && newUnitDef_flag && (trigger->IsParameterDefined(CR_TRIGGERS::KW_SPEED))) {
				newUnitDef_flag->speed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_SPEED, newUnitDef_flag->speed);
			}
			if (isMovableOrChild && newUnitDef_movable && (trigger->IsParameterDefined(CR_TRIGGERS::KW_ROTATION_SPEED))) {
				newUnitDef_movable->rotationSpeed = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_ROTATION_SPEED, newUnitDef_movable->rotationSpeed);
			}
			if (isCommandableOrChild && newUnitDef_commandable && (trigger->IsParameterDefined(CR_TRIGGERS::KW_WORK_RATE))) {
				newUnitDef_commandable->workRate = trigger->GetParameterValue<float>(CR_TRIGGERS::KW_WORK_RATE, newUnitDef_commandable->workRate);
			}
			if (isType50OrChild && newUnitDef_type50 && (trigger->IsParameterDefined(CR_TRIGGERS::KW_ACCURACY_PERCENT))) {
				newUnitDef_type50->accuracyPercent = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_ACCURACY_PERCENT, (long int)newUnitDef_type50->accuracyPercent);
			}
			if (isType50OrChild && newUnitDef_type50 && (trigger->IsParameterDefined(CR_TRIGGERS::KW_DISPLAYED_ARMOR))) {
				newUnitDef_type50->displayedArmor = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_DISPLAYED_ARMOR, (long int)newUnitDef_type50->displayedArmor);
			}
			if (isType50OrChild && newUnitDef_type50 && (trigger->IsParameterDefined(CR_TRIGGERS::KW_DISPLAYED_ATTACK))) {
				newUnitDef_type50->displayedAttack = (short int)trigger->GetParameterValue(CR_TRIGGERS::KW_DISPLAYED_ATTACK, (long int)newUnitDef_type50->displayedAttack);
			}
			if (trigger->IsParameterDefined(CR_TRIGGERS::KW_VISIBLE_IN_FOG)) {
				unitDefBase->visibleInFog = (char)trigger->GetParameterValue(CR_TRIGGERS::KW_VISIBLE_IN_FOG, (long int)unitDefBase->visibleInFog);
				if (unitDefBase->visibleInFog != 0) {
					unitDefBase->visibleInFog = 1; // just in case.
				}
			}
		}
	}

	// Give a command to a unit
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_UNIT_TARGET) {
		long int actionUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_ID, -1);
		long int targetUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_TARGET_UNIT_ID, -1);
		if ((actionUnitId < 0) || (targetUnitId < 0)) { return; }
		TellUnitToInteractWithTarget((AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE*)GetUnitStruct(actionUnitId), (AOE_STRUCTURES::STRUCT_UNIT_BASE*)GetUnitStruct(targetUnitId));
	}

	// Move a unit
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_UNIT_DESTINATION) {
		// Not MP-compatible (to confirm)
		long int actionUnitId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_UNIT_ID, -1);
		float posX = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_X, (float)-1);
		float posY = trigger->GetParameterValue(CR_TRIGGERS::KW_POS_Y, (float)-1);
		if ((actionUnitId < 0) || (posX < 0) || (posY < 0)) {
			return;
		}
		AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *gameMapInfo = global->gameMapInfo;
		assert(gameMapInfo && gameMapInfo->IsCheckSumValid());
		if ((posX >= gameMapInfo->mapArraySizeX) || (posY >= gameMapInfo->mapArraySizeY)) { return; }
		MoveUnitToTargetOrPosition((AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE*)GetUnitStruct(actionUnitId), NULL, posX, posY);
	}

	// Make gaia units human-capturable (or not)
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_GAIA_CAPTURABLE) {
		// Not MP-compatible (to confirm)
		int valueToSet = trigger->GetParameterValue(CR_TRIGGERS::KW_ENABLE, -1);
		if ((valueToSet < 0) || (valueToSet > 1)) { // only 0 and 1 are accepted
			return;
		}
		AOE_STRUCTURES::STRUCT_PLAYER *gaia = GetPlayerStruct(0);
		assert(gaia && gaia->IsCheckSumValid());
		if (!gaia || !gaia->IsCheckSumValid()) {
			return;
		}
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_LINK *creatablesLink = gaia->ptrCreatableUnitsListLink;
		if (!creatablesLink || !creatablesLink->IsCheckSumValid()) {
			return;
		}
		if (creatablesLink->listElemCount <= 0) {
			return;
		}
		AOE_STRUCTURES::STRUCT_PER_TYPE_UNIT_LIST_ELEMENT *currentElem = creatablesLink->lastListElement;
		int counter = creatablesLink->listElemCount;
		while ((currentElem != NULL) && (counter >= 1)) {
			AOE_STRUCTURES::STRUCT_UNIT_BASE *unitBase = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)currentElem->unit;
			if (unitBase && unitBase->IsCheckSumValidForAUnitClass()) {
				AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDefBase = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)unitBase->unitDefinition;
				if (unitDefBase && unitDefBase &&
					((unitDefBase->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_BUILDING) || (unitDefBase->unitType == AOE_CONST_FUNC::GLOBAL_UNIT_TYPES::GUT_TRAINABLE)) &&
					(unitDefBase->unitAIType != AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES::TribeAIGroupArtefact)
					) {
					// Unit is a building or a living unit, so it is capturable
					// Very important : exclude artefacts to avoid human players from capturing artefacts more easily than AI players

					// We can convert into type50 because we checked unit type is 70 or 80
					AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *unit50 = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)unitBase;
					unit50->stillToBeDiscoveredByHuman = valueToSet;
				}
			}

			currentElem = currentElem->previousElement;
			counter--;
		}
	}

	// Change "viewed" playerId
	if (trigger->triggerActionType == CR_TRIGGERS::TRIGGER_ACTION_TYPES::TYPE_SET_VIEWED_PLAYER) {
		long int actionPlayerId = trigger->GetParameterValue(CR_TRIGGERS::KW_ACTION_PLAYER_ID, -1);
		if ((actionPlayerId < 0) || (actionPlayerId >= global->playerTotalCount)) {
			return;
		}
		AOE_METHODS::PLAYER::ChangeControlledPlayer(actionPlayerId, false);
	}
}


// Execute a trigger (considering requirements are OK)
void ExecuteTriggerAction(CR_TRIGGERS::crTrigger *trigger) {
	if (!trigger) { return; }
	if (IsMultiplayer()) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global != NULL);
	if (!global) { return; }
	if (!trigger->enabled) { return; }

	// Mark trigger as disabled now it has been executed (except repeatable triggers)
	if (trigger->triggerEventType != CR_TRIGGERS::EVENT_TIMER) {
		trigger->enabled = false;
	} else {
		long int timerInterval = trigger->GetParameterValue(CR_TRIGGERS::KW_TIMER_INTERVAL, 0);
		long int timerTimeValue = trigger->GetParameterValue(CR_TRIGGERS::KW_TIMER_VALUE, 0);
		if (timerInterval < 1) {
			trigger->enabled = false; // invalid timer interval OR one-shot only timer.
		} else {
			timerTimeValue += timerInterval;
			trigger->SetParameterValue(CR_TRIGGERS::KW_TIMER_VALUE, timerTimeValue);
		}
	}

	// Common for all triggers types: manage trigger to run "before".
	if (!trigger->triggerNameToRunBefore.empty() && ROCKNROR::crInfo.triggerSet) {
		CR_TRIGGERS::crTrigger *trBefore = ROCKNROR::crInfo.triggerSet->GetTrigger(trigger->triggerNameToRunBefore);
		// Do not allow calling repeatable triggers: those triggers may remain enabled (because they are repeatable), which may lead to infinite dependencies.
		if (trBefore && (trBefore->enabled) && (!trBefore->IsRepeatable())) {
			ExecuteTriggerAction(trBefore);
		}
	}

	// Use a dedicated routine to execute action.
	ROCKNROR::TRIGGER::Trigger_JustDoAction(trigger);

	// Common for all triggers types: manage trigger to run "after".
	if (!trigger->triggerNameToRunAfter.empty() && ROCKNROR::crInfo.triggerSet) {
		CR_TRIGGERS::crTrigger *trAfter = ROCKNROR::crInfo.triggerSet->GetTrigger(trigger->triggerNameToRunAfter);
		// Do not allow calling repeatable triggers: those triggers may remain enabled (because they are repeatable), which may lead to infinite dependencies.
		if (trAfter && (trAfter->enabled) && (!trAfter->IsRepeatable())) {
			ExecuteTriggerAction(trAfter);
		}
	}
}


// Manage triggers execution for a specific event
void ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES evt) {
	CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
	memset(&evtInfo, -1, sizeof(evtInfo));
	ROCKNROR::TRIGGER::ExecuteTriggersForEvent(evt, evtInfo);
}


// Manage triggers execution for a specific event
void ExecuteTriggersForEvent(CR_TRIGGERS::TRIGGER_EVENT_TYPES evt, CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER &evtInfo) {
	// Make sure we never execute trigger when game is not running
	// Also disabled in multiplayer games.
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) || settings->rgeGameOptions.isMultiPlayer) {
		return;
	}

	// Make sure difficulty level is always set
	if (evtInfo.difficultyLevel < 0) {
		AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
		assert(settings);
		if (!settings) { return; }
		evtInfo.difficultyLevel = settings->rgeGameOptions.difficultyLevel;
	}
	// Make sure current game time is always set
	if (evtInfo.currentGameTime_s < 0) {
		AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
		assert(global);
		if (!global) { return; }
		evtInfo.currentGameTime_s = global->currentGameTime / 1000;
	}
	// Loop on triggers and execute those that are ready
	if (ROCKNROR::crInfo.triggerSet && (!ROCKNROR::crInfo.triggerSet->IsEmpty())) {
		CR_TRIGGERS::crTrigger *currentTrigger = ROCKNROR::crInfo.triggerSet->IteratorFirst();
		while (currentTrigger) {
			if (currentTrigger->CanExecuteNow(evt, evtInfo)) {
				ROCKNROR::TRIGGER::ExecuteTriggerAction(currentTrigger);
			}
			currentTrigger = ROCKNROR::crInfo.triggerSet->IteratorNext();
		}
	}
}


// Export RockNRor's internal data triggers to Trigger text format.
std::string GetTriggerTextFromInternalData() {
	if (!ROCKNROR::crInfo.triggerSet) { return ""; }
	return ROCKNROR::crInfo.triggerSet->GetStringForScenarioInfo(false);
}


void ReloadTriggersFromGameData(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scenarioInfo) {
	if (ROCKNROR::crInfo.triggerSet) {
		delete ROCKNROR::crInfo.triggerSet;
		ROCKNROR::crInfo.triggerSet = NULL;
	}
	ROCKNROR::crInfo.triggerSet = new CR_TRIGGERS::crTriggerSet();
	if (!scenarioInfo || (!scenarioInfo->IsCheckSumValid())) { return; }

	ROCKNROR::crInfo.triggerSet->ReadFromFileContent(GetTriggerDataPtr(scenarioInfo));
}

void ReloadTriggersFromGameData() {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global) { return; }
	if (!global->scenarioInformation) { return; }
	ROCKNROR::TRIGGER::ReloadTriggersFromGameData(global->scenarioInformation);
}


// Write trigger to game data, using RockNRor internal data.
bool WriteTriggersFromInternalToGameData(bool onlyEnabledTriggers) {
	if (!ROCKNROR::crInfo.triggerSet) { return false; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->scenarioInformation) { return false; }
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *sc_info = global->scenarioInformation; // Guaranteed non-NULL by previous "if"
	assert(sc_info != NULL);
	if (!sc_info) { return false; }
	std::string tmpStr = ROCKNROR::crInfo.triggerSet->GetStringForScenarioInfo(onlyEnabledTriggers);
	char *trgData = GetTriggerDataPtr(sc_info);

	// Free previous value, if any
	if (trgData != NULL) {
		AOEFree(trgData);
		SetTriggerDataSize(sc_info, 0);
	}
	// Write triggers data in scenario information
	return ROCKNROR::TRIGGER::WriteTriggersInGameData((char *)tmpStr.c_str());
}


// Write trigger to game data, using provided text trigger data.
bool WriteTriggersInGameData(char *triggerText) {
	if (!triggerText) { triggerText = ""; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->scenarioInformation) { return false; }
	AOE_STRUCTURES::STRUCT_SCENARIO_INFO *sc_info = global->scenarioInformation; // Guaranteed non-NULL by previous "if"
	assert(sc_info != NULL);
	if (sc_info == NULL) { return false; }

	bool hasEndTag = TriggerTextContainsENDTagAtBeginning(triggerText);

	long int size = strlen(triggerText); // can be 0.
	long int sizeToAlloc = size + 1;
	if (!hasEndTag) { sizeToAlloc += SpecialKeyword_getLength(CR_TRIGGERS::KW_END_LINE); }
	char *strToSave = (char *)AOEAlloc(sizeToAlloc);
	if (!hasEndTag) {
		strcpy_s(strToSave, SpecialKeyword_getLength(CR_TRIGGERS::KW_END_LINE) + 1, GetTriggerSpecialKeyword(CR_TRIGGERS::KW_END_LINE)); // +1 for (temporary) \0
		strcpy_s(strToSave + SpecialKeyword_getLength(CR_TRIGGERS::KW_END_LINE), size + 1, triggerText); // Concatenate (overwrite "previous" ending \0)
	} else {
		strcpy_s(strToSave, sizeToAlloc, triggerText);
	}
	SetTriggerDataSize(sc_info, sizeToAlloc);
	SetTriggerDataPtr(sc_info, strToSave);
	return true;
}


// This is dedicated to triggers handling on "GameSettingsNotifyEvent" event.
void ManageTriggersOnGameNotifyEvent(long int eventId, short int playerId, long int arg3, long int arg4, long int arg5) {
	CR_TRIGGERS::EVENT_INFO_FOR_TRIGGER evtInfo;
	CR_TRIGGERS::TRIGGER_EVENT_TYPES evtType = CR_TRIGGERS::EVENT_NONE;
	memset(&evtInfo, -1, sizeof(evtInfo));
	evtInfo.playerId = playerId;

	// Manage triggers
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_RESEARCH_COMPLETE) {
		evtInfo.researchId = arg3;
		evtInfo.posX = arg5;
		evtInfo.posY = arg4;
		evtType = CR_TRIGGERS::EVENT_RESEARCH_COMPLETED;
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_BUILDING_COMPLETE) {
		evtInfo.unitDefId = arg3;
		evtInfo.posX = arg5;
		evtInfo.posY = arg4;
	}
	/*if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_ALL_RELICS_CAPTURED) {
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_ALL_RELICS_LOST_CONTROL) {
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_ALL_RUINS_CAPTURED) {
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_ALL_RUINS_LOST_CONTROL) {
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_PLAYER_LOST) {
	}*/
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_UNIT_SPAWNED) {
		evtInfo.unitDefId = arg3;
		evtInfo.posX = arg5;
		evtInfo.posY = arg4;
	}
	// Warning, for all 3 "wonder" events, position is arg3/4, not 4/5
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_WONDER_DESTROYED) {
		evtInfo.posX = arg4;
		evtInfo.posY = arg3;
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_WONDER_FINISHED) {
		evtInfo.posX = arg4;
		evtInfo.posY = arg3;
	}
	if (eventId == AOE_CONST_INTERNAL::GAME_EVENT_TYPE::EVENT_WONDER_START_BUILDING) {
		evtInfo.posX = arg4;
		evtInfo.posY = arg3;
	}
	if (evtType != CR_TRIGGERS::TRIGGER_EVENT_TYPES::EVENT_NONE) {
		ROCKNROR::TRIGGER::ExecuteTriggersForEvent(evtType, evtInfo);
	}
}


// Manage disable (via trigger) units for villager, house, fishing ship, trade ship
// This method should be called at game start, after "initial" technologies have been applied, so we can override some (and force to disable again some units)
void ManageTriggerDisableUnitsForExceptions() {
	for (int playerId = 0; playerId < 9; playerId++) {
		AOE_STRUCTURES::STRUCT_PLAYER *player = GetPlayerStruct(playerId);
		if (player) {
			std::vector<short int>::const_iterator it;
			for (it = ROCKNROR::crInfo.unitDefToDisable[player->playerId].begin(); it != ROCKNROR::crInfo.unitDefToDisable[player->playerId].end(); it++) {
				short int DAT_ID = *it;
				if (
					(DAT_ID == CST_UNITID_FARM) || // See also ManageDisableUnitsForFarms - it can be enabled during the game
					// All 4 other units are ALWAYS enabled at game initialization (they have no pre-requisite)
					// so this method (called at startup) is always sufficient (those units won't be enabled again)
					(DAT_ID == CST_UNITID_HOUSE) ||
					(DAT_ID == CST_UNITID_VILLAGER) ||
					(DAT_ID == CST_UNITID_FISHING_BOAT) ||
					(DAT_ID == CST_UNITID_TRADE_BOAT)
					) {
					if ((DAT_ID < 0) || (DAT_ID >= player->structDefUnitArraySize)) { return; }
					AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)player->ptrStructDefUnitTable[DAT_ID];
					unitDef->availableForPlayer = 0;
				}
			}
		}
	}
}

// Manage "disable farm" when specified in triggers. Actually, prevent market construction from enabling farms.
// Farm is enabled by market construction, which may occur both "ingame" or at game loading (if a market exists, or if starting at bronze).
void ManageDisableUnitsForFarms(AOE_STRUCTURES::STRUCT_PLAYER *player) {
	std::vector<short int>::const_iterator it;
	for (it = ROCKNROR::crInfo.unitDefToDisable[player->playerId].begin(); it != ROCKNROR::crInfo.unitDefToDisable[player->playerId].end(); it++) {
		short int DAT_ID = *it;
		if (DAT_ID == CST_UNITID_FARM) {
			if ((DAT_ID < 0) || (DAT_ID >= player->structDefUnitArraySize)) { return; }
			AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)player->ptrStructDefUnitTable[DAT_ID];
			unitDef->availableForPlayer = 0;
			return;
		}
	}
}


// This is called while scenarioInfo structure is read from a file
// Scenario texts (including players strategies) have already been read, but not other information, be careful.
void InitScenarioInfoTextData(AOE_STRUCTURES::STRUCT_SCENARIO_INFO *scenarioInfo) {
	ROCKNROR::TRIGGER::ReloadTriggersFromGameData(scenarioInfo);

	for (int playerId = 0; playerId < 9; playerId++) {
		if (ROCKNROR::crInfo.triggerSet != NULL) {
			ROCKNROR::crInfo.FillUnitDefToDisableFromString(playerId, ROCKNROR::crInfo.triggerSet->disable_unitDefIDTextList[playerId].c_str());
			ROCKNROR::crInfo.FillResearchesToDisableFromString(playerId, ROCKNROR::crInfo.triggerSet->disable_researchIDTextList[playerId].c_str());
		}
	}
}


// This method handles the "disable unit" and "disable research" defined in custom triggers.
// This is called in "disable research" init at game start (for scenarios)
// This is only executed for scenarios, not DM/RM !
void OnGameInitDisableResearchesEvent(AOE_STRUCTURES::STRUCT_PLAYER_RESEARCH_INFO *playerResearchInfo) {
	assert(playerResearchInfo != NULL);
	if (!playerResearchInfo) { return; }
	AOE_STRUCTURES::STRUCT_PLAYER *player = playerResearchInfo->ptrPlayer;
	assert(player && player->IsCheckSumValid());
	if (!player || !player->IsCheckSumValid()) { return; }
	// Note: get global structure from player, because global variable might not be set yet.
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = player->GetGlobalStruct();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }

	// For current player, disable the units we have from trigger data.
	std::vector<short int>::const_iterator it;
	for (it = ROCKNROR::crInfo.unitDefToDisable[player->playerId].begin(); it != ROCKNROR::crInfo.unitDefToDisable[player->playerId].end(); it++) {
		short int DAT_ID = *it;
		if (PLAYER::DisableUnitForPlayer(player, DAT_ID)) {
			// success
		} else {
			// Exceptions: villager, house, farm, fishing ship, trade ship. DisableUnitForPlayer doesn't support them because of techs that do several things at once.
			// Those exceptions are managed in ManageTriggerDisableUnitsForExceptions and ManageDisableUnitsForFarms
		}
	}

	// Disable researches from trigger data
	for (it = ROCKNROR::crInfo.researchesToDisable[player->playerId].begin(); it != ROCKNROR::crInfo.researchesToDisable[player->playerId].end(); it++) {
		short int researchId = *it;
		AOE_METHODS::AOE_enableResearch(player, researchId, false);
	}
}


}

}
