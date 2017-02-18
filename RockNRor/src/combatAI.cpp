#include "../include/combatAI.h"

using namespace AOE_STRUCTURES;


namespace AOE_METHODS {

	// Estimates the total time to kill a group's units at target position, considering enemy units known from infAI elem list
	float GetTimeToKillGroupUnitsAtTargetPosition(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *targetUnit) {
		if (!infAI || !infAI->IsCheckSumValid()) { return 0; }
		if (!unitGroup || !unitGroup->IsCheckSumValid()) { return 0; }
		if (!targetUnit || !targetUnit->IsCheckSumValidForAUnitClass()) { return 0; }
		unsigned long int callAddr = 0x4C6400;
		float result;
		_asm {
			MOV ECX, infAI;
			PUSH targetUnit;
			PUSH unitGroup;
			CALL callAddr;
			FSTP DS:[result]; // REQUIRED to compensate the FLD from called method (for float stack consistency)
		}
		return result;
	}

}


namespace COMBAT {
;


// Custom treatment to decide if a potential target unit should be ignored
// Overload standard rules for catapults(ignores building if HP=1) and "target=wall" cases.
// Default result=false (on error cases)
bool AutoSearchTargetShouldIgnoreUnit(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *potentialTargetUnit) {
	assert(ROCKNROR::crInfo.configInfo.useEnhancedRulesForAutoAttackTargetSelection); // should have been checked before calling this
	if (!activity || !potentialTargetUnit || !potentialTargetUnit->IsCheckSumValidForAUnitClass()) {
		return false; // error case
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *targetUnitDefBase = potentialTargetUnit->unitDefinition;
	if (!targetUnitDefBase || !targetUnitDefBase->IsCheckSumValidForAUnitClass()) {
		return false; // error case
	}
	AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *actorUnit = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *)activity->ptrUnit;
	if (!actorUnit || !actorUnit->IsCheckSumValidForAUnitClass()) {
		return false; // We expect ACTOR unit to derive from type50 ! (should be living or building)
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *actorUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)actorUnit->unitDefinition;
	if (!actorUnitDef || !actorUnitDef->IsCheckSumValidForAUnitClass()) {
		return false; // We expect ACTOR unit to derive from type50 ! (should be living or building)
	}

	bool canHurtOtherUnits = (actorUnitDef->blastLevel != AOE_CONST_FUNC::BLAST_LEVELS::CST_BL_DAMAGE_TARGET_ONLY) &&
		(actorUnitDef->blastRadius > 0);
	bool isMelee = (actorUnitDef->maxRange == 0);
	bool isCatapult = canHurtOtherUnits && (actorUnitDef->unitAIType == TribeAIGroupSiegeWeapon);
	// Note: catapult trireme, juggernaught, cleopatra are warships (22), not TribeAIGroupSiegeWeapon.
	bool targetIsWall = (targetUnitDefBase->unitAIType == TribeAIGroupWall);
	bool targetIsTower = IsTower(targetUnitDefBase);
	bool targetIsMilitary = IsNonTowerMilitaryUnit(targetUnitDefBase->unitAIType);

	bool applyWeirdRuleOnCatsWith1HPBuilding = true; // True if we want the same (weird) rule as original ROR code on catapults and buildings with HP=1
	if (applyWeirdRuleOnCatsWith1HPBuilding && isCatapult && (potentialTargetUnit->remainingHitPoints <= 1)) {
		return true; // Ignore unit. Not sure why there is that check !
	}


	// Custom rules
	bool hasRuleAtUnitLevel = false;
	AutoAttackPolicy *policy = isMelee ? &ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastMeleeUnits :
		&ROCKNROR::crInfo.configInfo.autoAttackOptionForBlastRangedUnits;
	// If there is a config at unit level, use it instead of global parameter.
	UnitCustomInfo *unitInfo = ROCKNROR::crInfo.myGameObjects.FindUnitCustomInfo(actorUnit->unitInstanceId);
	if (unitInfo && unitInfo->autoAttackPolicyIsSet) {
		policy = &unitInfo->autoAttackPolicy;
		hasRuleAtUnitLevel = true;
	}

	bool targetIsVillager = IsVillager_includingShips(targetUnitDefBase->DAT_ID1);
	bool targetIsNonTowerBuilding = (targetUnitDefBase->unitAIType == TribeAIGroupBuilding) && (!IsTower(targetUnitDefBase));

	bool policyTellsToIgnore =
		(!policy->attackVillagers && targetIsVillager) ||
		(!policy->attackNonTowerBuildings && targetIsNonTowerBuilding) ||
		(!policy->attackTowers && targetIsTower) ||
		(!policy->attackMilitary && targetIsMilitary) ||
		(!policy->attackWalls && targetIsWall);

	if ((canHurtOtherUnits || hasRuleAtUnitLevel) && policyTellsToIgnore) {
		return true;
	}

	return false;
}


// Returns true if a unit should change target to new one (default), false if it should keep attacking current one.
// This choice is very important, unit must NOT change its mind on each call because it is called too often and unit just attacks none of its "attackers"
// Which is a common issue in original game (example: 2-priest trick to safely convert an enemy)
// For not managed cases: Default behaviour is return true (change target) because it allows original code to be executed.
// The goal is to detect the more relevant situations where we can return false (force attacking current unit: do not switch).
// The improved algorithm is only used if ImproveAI config is ON.
bool ShouldChangeTarget(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity, long int targetUnitId) {
	assert(COMBAT_CONST::distanceToConsiderVeryClose > 0);

	// Error cases: let ROR code handle them.
	if (!activity) { return true; }
	if (targetUnitId < 0) { return true; } // important check because we call GetUnitStruct on this.
	//assert(activity->IsCheckSumValid());

	AOE_STRUCTURES::STRUCT_UNIT_BASE *actorUnit = activity->ptrUnit;
	AOE_STRUCTURES::STRUCT_UNIT_BASE *newTargetUnit = GetUnitStruct(targetUnitId);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *oldTargetUnit = GetUnitStruct(activity->targetUnitId);
	if (!actorUnit || !newTargetUnit /*|| !oldTargetUnit*/) { return true; }
	if (!oldTargetUnit) {
		// activity target unit can be reset by pending treatments. Find if action HAS a valid target. We may want to keep this target.
		AOE_STRUCTURES::STRUCT_ACTION_BASE *action = AOE_STRUCTURES::GetUnitAction(actorUnit); // May return NULL
		if (!action || (action->targetUnitId < 0)) {
			return true;
		}
		oldTargetUnit = GetUnitStruct(action->targetUnitId);
		activity->targetUnitId = action->targetUnitId; // A bit dirty. Fixes the activity.targetUnitId==-1 case.
	}
	if (!oldTargetUnit) { return true; } // We really could not find the target, it seems there is none: change target !

	assert(newTargetUnit->IsCheckSumValidForAUnitClass());
	assert(actorUnit->IsCheckSumValidForAUnitClass());
	if (!newTargetUnit->IsCheckSumValidForAUnitClass() || !actorUnit->IsCheckSumValidForAUnitClass()) {
		return true; // invalid data. Let original code be executed.
	}
	if (oldTargetUnit == newTargetUnit) {
		return false;
	} // same unit: do not change (=no stupid hesitation)

	// Checks for invalid/dead targets
	if ((oldTargetUnit->remainingHitPoints <= 0) || (oldTargetUnit->unitType < GLOBAL_UNIT_TYPES::GUT_TRAINABLE) ||
		(oldTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_TREE) || (oldTargetUnit->unitStatus > 2)) {
		return true; // switch
	}
	if ((newTargetUnit->remainingHitPoints <= 0) || (newTargetUnit->unitType < GLOBAL_UNIT_TYPES::GUT_TRAINABLE) ||
		(newTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_TREE) || (newTargetUnit->unitStatus > 2)) {
		return false; // Keep. Not sure if this case can happen. Maybe when a projectile from a dead unit hits me afterwards ?
	}
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return true; }
	assert((global->gameMapInfo != NULL) && (global->gameMapInfo->IsCheckSumValid()));
	long mapSizeX = global->gameMapInfo->mapArraySizeX;
	long mapSizeY = global->gameMapInfo->mapArraySizeY;
	assert((long)newTargetUnit->positionX <= mapSizeX);
	assert((long)actorUnit->positionX <= mapSizeX);
	assert((long)oldTargetUnit->positionX <= mapSizeX);
	assert((long)newTargetUnit->positionY <= mapSizeY);
	assert((long)actorUnit->positionY <= mapSizeY);
	assert((long)oldTargetUnit->positionY <= mapSizeY);
	assert(newTargetUnit->positionX >= 0);
	assert(newTargetUnit->positionY >= 0);
	assert(actorUnit->positionX >= 0);
	assert(actorUnit->positionY >= 0);
	assert(oldTargetUnit->positionX >= 0);
	assert(oldTargetUnit->positionY >= 0);
	if (((long)newTargetUnit->positionX > mapSizeX) ||
		((long)newTargetUnit->positionY > mapSizeY) ||
		((long)actorUnit->positionX > mapSizeX) ||
		((long)actorUnit->positionY > mapSizeY) ||
		((long)oldTargetUnit->positionX > mapSizeX) ||
		((long)oldTargetUnit->positionX > mapSizeY) ||
		((long)newTargetUnit->positionX < 0) ||
		((long)newTargetUnit->positionY < 0) ||
		((long)actorUnit->positionX < 0) ||
		((long)actorUnit->positionY < 0) ||
		((long)oldTargetUnit->positionX < 0) ||
		((long)oldTargetUnit->positionY < 0)
		) {
		return true;
	}

	// Collect info on the 2 units to attack (and "me")
	AOE_STRUCTURES::STRUCT_PLAYER *actorPlayer = actorUnit->ptrStructPlayer;
	assert(actorPlayer != NULL);
	assert(actorPlayer->IsCheckSumValid());
	float *resources = (float *)actorPlayer->ptrResourceValues;
	assert(resources != NULL);
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *actorUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE*)actorUnit->unitDefinition;
	assert(actorUnitDef != NULL);
	assert(actorUnitDef->IsCheckSumValidForAUnitClass());
	assert(actorUnitDef->DerivesFromAttackable());
	if (!actorUnitDef || !actorUnitDef->DerivesFromAttackable()) { return true; } // error case
	bool actorIsPriest = (actorUnitDef->DAT_ID1 == CST_UNITID_PRIEST);
	bool actorIsArcher = (actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher) ||
		(actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher) ||
		(actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantArcher) ||
		(actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher); // True if actor unit is any type of archer (not counting slingers, towers)
	// Also consider war boats as archer (excluding fire galley and "catapult" ones)
	if ((actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupWarBoat) &&
		(actorUnitDef->DAT_ID1 != CST_UNITID_CATAPULT_TRIREME) &&
		(actorUnitDef->DAT_ID1 != CST_UNITID_JUGGERNAUGHT) &&
		(actorUnitDef->DAT_ID1 != CST_UNITID_FIRE_GALLEY)
		) {
		actorIsArcher = true;
	}
	assert(newTargetUnit->unitDefinition != NULL);
	assert(oldTargetUnit->unitDefinition != NULL);
	if ((newTargetUnit->unitDefinition == NULL) || (oldTargetUnit->unitDefinition == NULL)) {
		return true; // error case
	}

	// Is AI Improvement feature ON ?
	if (!ROCKNROR::IsImproveAIEnabled(actorPlayer->playerId)) {
		return true; // AI improvement is disabled: let ROR original code do its normal behavior.
	}
	// Do not improve in easy levels
	if (GetGameSettingsPtr() && (GetGameSettingsPtr()->rgeGameOptions.difficultyLevel >= AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL::GDL_EASY)) { return true; }

	// A target is mine or allied
	if (actorUnit->ptrStructPlayer->ptrDiplomacyStances[oldTargetUnit->ptrStructPlayer->playerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY) {
		return true; // change to new because old target is allied (may happen after a conversion or maybe diplomacy change)
	}
	if (actorUnit->ptrStructPlayer->ptrDiplomacyStances[newTargetUnit->ptrStructPlayer->playerId] == AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY) {
		return false; // keep old because new target is allied (can this happen ? Not sure)
	}

	bool newTargetIsVillager = (newTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian);// IsVillager(newTargetUnit->unitDefinition->DAT_ID1);
	bool actionTargetIsVillager = (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupCivilian); // IsVillager(oldTargetUnit->unitDefinition->DAT_ID1);
	bool newTargetIsPriest = (newTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_PRIEST);
	bool actionTargetIsPriest = (oldTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_PRIEST);
	bool newTargetIsTower = IsTower(newTargetUnit->unitDefinition);
	bool actionTargetIsTower = IsTower(oldTargetUnit->unitDefinition);
	bool newTargetIsWall = (newTargetUnit->unitDefinition->unitAIType == TribeAIGroupWall);
	bool actionTargetIsWall = (oldTargetUnit->unitDefinition->unitAIType == TribeAIGroupWall);
	// Position/distances
	float myPosX = actorUnit->positionX;
	float myPosY = actorUnit->positionY;
	float distanceX = (newTargetUnit->positionX - myPosX);
	float distanceY = (newTargetUnit->positionY - myPosY);
	float squareDistanceNewTarget = (distanceX * distanceX) + (distanceY * distanceY);
	distanceX = (oldTargetUnit->positionX - myPosX);
	distanceY = (oldTargetUnit->positionY - myPosY);
	float squareDistanceOldTarget = (distanceX * distanceX) + (distanceY * distanceY);
	bool isOldTargetPosKnown = IsUnitPositionKnown(actorPlayer, oldTargetUnit);
	// The value to test distances, taking into account unit range.
	float squareVeryCloseTestDistance = (actorUnitDef->maxRange + COMBAT_CONST::distanceToConsiderVeryClose);
	squareVeryCloseTestDistance = squareVeryCloseTestDistance * squareVeryCloseTestDistance;

	bool almostKilled_actionTarget = (actorUnitDef->displayedAttack > oldTargetUnit->remainingHitPoints); // Very approximative but fast ;)
	bool almostKilled_newTarget = (actorUnitDef->displayedAttack > newTargetUnit->remainingHitPoints); // Very approximative but fast ;)
	bool alreadyVeryClose_oldTarget = (squareDistanceOldTarget <= squareVeryCloseTestDistance); // If true, consider I don't need moving to attack
	bool alreadyVeryClose_newTarget = (squareDistanceNewTarget <= squareVeryCloseTestDistance); // If true, consider I don't need moving to attack

	// VERY FIRST priority decision: fog-hidden enemy: must change target or this would be cheating !!!
	if (!isOldTargetPosKnown) {
		return true; // Change target
	}

	// Special: actor is priest ("almost killed" values don't have any importance here). Against tower, consider "melee" distance, not priest's range
	if (actorIsPriest) {
		// Additional information
		bool canConvertTowers = (resources[RESOURCE_TYPES::CST_RES_ORDER_CAN_CONVERT_BUILDING] > 0);
		bool alreadyVeryClose_oldTarget_melee = (squareDistanceOldTarget <= COMBAT_CONST::distanceToConsiderVeryClose); // For priests (that have both range and "melee" (vs buildings) conversion)
		bool alreadyVeryClose_newTarget_melee = (squareDistanceOldTarget <= COMBAT_CONST::distanceToConsiderVeryClose);
		bool newTargetIsChariot = (newTargetUnit->unitDefinition->unitAIType == TribeAIGroupChariot) ||
			(newTargetUnit->unitDefinition->unitAIType == TribeAIGroupChariotArcher);
		bool actionTargetIsChariot = (oldTargetUnit->unitDefinition->unitAIType == TribeAIGroupChariot) ||
			(oldTargetUnit->unitDefinition->unitAIType == TribeAIGroupChariotArcher);

		// Top priority: priests can't convert TC, wonder, walls or unfinished buildings => next 2 rules
		if ((newTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) && (newTargetUnit->unitStatus < 2)) {
			return false; // Force keep if "new" target is a not-fully built building.
		}
		if (newTargetIsWall || (newTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) || (newTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_WONDER)) {
			return false; // Force keep if "new" target can't be converted.
		}
		if ((oldTargetUnit->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) && (oldTargetUnit->unitStatus < 2)) {
			return true; // Change if current target is a not-fully built building
		}
		if (actionTargetIsWall || (oldTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_FORUM) || (oldTargetUnit->unitDefinition->DAT_ID1 == CST_UNITID_WONDER)) {
			return true;  // Change if current target can't be converted
		}

		// Priest that are very close to towers should try to convert them before the towers kills them
		if (alreadyVeryClose_newTarget_melee && newTargetIsTower && canConvertTowers) {
			return true; // Change to "new" target = tower
		}
		if (alreadyVeryClose_oldTarget_melee && actionTargetIsTower && canConvertTowers) {
			// Especially, note that towers have less resistance than enemy priests. If a tower is near, it is better to convert it.
			return false; // Force keep
		}
		if (actionTargetIsPriest && canConvertTowers) {
			return false; // Force keep - do not hesitate against priest (except for close towers but it has already been tested before)
		}
		if (newTargetIsPriest && canConvertTowers) {
			return true; // do not hesitate against priest (except for close towers but it has already been tested before)
		}
		// Avoid trying to convert chariots...
		if (newTargetIsChariot && !actionTargetIsChariot) {
			return false; // Force keep action target which is not a chariot
		}
		if (!newTargetIsChariot && actionTargetIsChariot) {
			return true; // Switch to "new" target which is not a chariot
		}
		// Manage case: Only one of the 2 targets is in range
		if (!alreadyVeryClose_newTarget && alreadyVeryClose_oldTarget) {
			return false; // Force keep
		}
		if (alreadyVeryClose_newTarget && !alreadyVeryClose_oldTarget) {
			return true; // Switch
		}
		// Old target is a building not at range, other is a living unit: switch
		if (!alreadyVeryClose_oldTarget_melee &&
			(oldTargetUnit->unitDefinition->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING) &&
			(newTargetUnit->unitDefinition->unitType != GLOBAL_UNIT_TYPES::GUT_BUILDING)
			) {
			return true;
		}
		// TO DO: add other rules to accept some target changes when useful.

		// Default: priests are weak, they must avoid changing target while converting.
		return false; // Force keep
	}

	// "New" target is priest, close and almost killed: switch !
	if (newTargetIsPriest && almostKilled_newTarget && alreadyVeryClose_newTarget) {
		return true;
	}
	// Action target is priest, close and almost killed: keep !
	if (actionTargetIsPriest && almostKilled_actionTarget && alreadyVeryClose_oldTarget) {
		return false; // Force keep
	}
	// Only target is a priest
	if (newTargetIsPriest && !actionTargetIsPriest) {
		return true; // Change
	}
	// Both are priests: attack one that is in range, priority goes to current one.
	if (newTargetIsPriest && actionTargetIsPriest) {
		if (alreadyVeryClose_oldTarget) { return false; } // Keep
		if (alreadyVeryClose_newTarget) { return true; } // Change
	}
	// Other cases where action target is a priest : keep current target (don't hesitate against priests !)
	if (actionTargetIsPriest) {
		return false; // Force keep
	}

	// Now deal with cases with no priest
	// "Easy" case: "New" target is close and almost killed: switch !
	if (almostKilled_newTarget && alreadyVeryClose_newTarget) {
		return true;
	}
	// "Easy" case: Action target is close and almost killed: keep !
	if (almostKilled_actionTarget && alreadyVeryClose_oldTarget) {
		return false; // Force keep
	}

	// I am an archer: if possible, avoid attacking towers (or even buildings) because I'm not good for that
	if (actorIsArcher) {
		bool actionTargetIsBuilding = (oldTargetUnit->unitDefinition->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING);
		bool targetIsBuilding = (newTargetUnit->unitDefinition->unitType == GLOBAL_UNIT_TYPES::GUT_BUILDING);

		// Top-Priority exception: try to kill towers with only 1 HP (already done by "easy case" above?)
		if (newTargetIsTower && (newTargetUnit->remainingHitPoints < 2)) {
			return true; // Change to "1 HP" tower to instant kill it !
		}
		if (actionTargetIsTower && (oldTargetUnit->remainingHitPoints < 2)) {
			return false; // Keep target="1 HP" tower to instant kill it !
		}

		if (targetIsBuilding && !actionTargetIsBuilding) {
			return false; // Force keep
		}
		if (!targetIsBuilding && actionTargetIsBuilding) {
			return true; // current target is a building, should be better to change to non-building target !
		}
	}

	// When attacking some "priority" units (that are in my range), force keep: siege weapons, some archers
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSiegeWeapon)) {
		return false; // force keep
	}
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupChariotArcher)) {
		return false; // force keep
	}
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupHorseArcher)) {
		return false; // force keep
	}
	if (alreadyVeryClose_oldTarget && (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupArcher)) {
		return false; // force keep
	}
	// Slingers: attack them, but NOT if I am an archer
	if (alreadyVeryClose_oldTarget && !actorIsArcher && (oldTargetUnit->unitDefinition->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupSlinger)) {
		return false; // force keep
	}
	// Enemy tower: keep if I am war elephant
	if (alreadyVeryClose_oldTarget && (actorUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupElephantRider) && (IsTower(oldTargetUnit->unitDefinition))) {
		return false; // force keep
	}


	// If only 1 of the 2 targets has me as its target, attack this one
	AOE_STRUCTURES::STRUCT_ACTION_BASE *newTargetUnitAction = NULL;
	AOE_STRUCTURES::STRUCT_ACTION_BASE *actionTargetUnitAction = NULL;
	STRUCT_UNIT_COMMANDABLE *newTargetUnitBird = (STRUCT_UNIT_COMMANDABLE *)newTargetUnit;
	STRUCT_UNIT_COMMANDABLE *oldTargetUnitBird = (STRUCT_UNIT_COMMANDABLE *)oldTargetUnit;
	if (newTargetUnitBird->DerivesFromCommandable() && (newTargetUnitBird->ptrActionInformation) && (newTargetUnitBird->ptrActionInformation->ptrActionLink) &&
		(newTargetUnitBird->ptrActionInformation->ptrActionLink->actionStruct)) {
		newTargetUnitAction = (AOE_STRUCTURES::STRUCT_ACTION_BASE *)newTargetUnitBird->ptrActionInformation->ptrActionLink->actionStruct;
	}
	if (oldTargetUnitBird->DerivesFromCommandable() && (oldTargetUnitBird->ptrActionInformation) && (oldTargetUnitBird->ptrActionInformation->ptrActionLink) &&
		(oldTargetUnitBird->ptrActionInformation->ptrActionLink->actionStruct)) {
		actionTargetUnitAction = (AOE_STRUCTURES::STRUCT_ACTION_BASE *)oldTargetUnitBird->ptrActionInformation->ptrActionLink->actionStruct;
	}
	bool newTargetAttacksMe = newTargetUnitAction && (
		(newTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_UNKNOWN_7) ||
		(newTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_ATTACK) ||
		(newTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_CONVERT)
		) &&
		(newTargetUnitAction->targetUnit == actorUnit);
	bool actionTargetAttacksMe = actionTargetUnitAction && (
		(actionTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_UNKNOWN_7) ||
		(actionTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_ATTACK) ||
		(actionTargetUnitAction->actionTypeID == AOE_CONST_FUNC::UNIT_ACTION_ID::CST_IAI_CONVERT)
		) &&
		(actionTargetUnitAction->targetUnit == actorUnit);

	if (newTargetAttacksMe && !actionTargetAttacksMe) {
		return true; // change
	}
	if (!newTargetAttacksMe && actionTargetAttacksMe) {
		return false; // force keep
	}

	// Same enemy unit AI type: just compare HP (they are similar units)
	if (newTargetUnit->unitDefinition->unitAIType == oldTargetUnit->unitDefinition->unitAIType) {
		if (newTargetUnit->remainingHitPoints > oldTargetUnit->remainingHitPoints) {
			return false; // force keep if both units have the same unit type and current one has LESS HP (weaker: keep).
		} // Else: let other tests be run (just in case they find some reason to choose), if needed it will go to this function's ending "return true;"
	}

	// None is attacking me ?
	if (!newTargetAttacksMe && !actionTargetAttacksMe) {
		if (newTargetIsVillager && !actionTargetIsVillager) {
			return false; // Current is NOT a villager, attack it (keep)
		}
		if (!newTargetIsVillager && actionTargetIsVillager) {
			return true; // New is NOT a villager, attack it (change)
		}
	}

	// TO DO
	// Try a circular list (~10 ?) to log all last attacker unitIDs + game time of last attack.
	// If a time is quite old (several seconds), remove from list
	// Each time we come here, update in the list or add if not present.
	// So we can know how many units are attacking the unit. If there are too many, do a "force keep" for one of them
	return true; // Default: change action (let the original code decide)
}


// Returns false if we should prevent unit from moving back (to maxrange) after shooting. Default result=true
bool ShouldRetreatAfterShooting(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity) {
	assert(activity);
	if (!activity || !activity->ptrUnit) { return true; }
	AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE *actorUnitType50 = (AOE_STRUCTURES::STRUCT_UNIT_ATTACKABLE*)activity->ptrUnit;
	if (!actorUnitType50 || !actorUnitType50->IsCheckSumValidForAUnitClass() || !actorUnitType50->DerivesFromAttackable()) { return true; }
	if (!actorUnitType50->ptrStructPlayer || !actorUnitType50->ptrStructPlayer->IsCheckSumValid()) { return true; }
	// This feature is an AI improvement. Do it only if config says so.
	if (!ROCKNROR::IsImproveAIEnabled(actorUnitType50->ptrStructPlayer->playerId)) {
		return true;
	}
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *actorUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)actorUnitType50->unitDefinition;
	if (!actorUnitDef || !actorUnitDef->IsCheckSumValidForAUnitClass() || !actorUnitDef->DerivesFromAttackable()) { return true; }
	if (!actorUnitDef->DerivesFromAttackable()) { return true; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(activity->targetUnitId);
	if (!targetUnit || !targetUnit->IsCheckSumValidForAUnitClass()) { return true; }
	AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *targetUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_ATTACKABLE *)targetUnit->unitDefinition;
	if (targetUnitDef->DerivesFromAttackable()) {
		if ((targetUnitDef->projectileUnitId >= 0) && (targetUnitDef->maxRange > actorUnitDef->maxRange)) {
			// Force NOT retreat after shooting, because enemy has a better range than me !
			return false;
		}
	}
	return true; // Default behaviour
}


// Computes the damage dealt by a group on a unit, cf 0x4C62F0.
float GetGroupDamageOnUnit(STRUCT_INF_AI *infAI, STRUCT_UNIT_GROUP *unitGroup, STRUCT_UNIT_BASE *targetUnit) {
	assert(infAI && infAI->IsCheckSumValid() && unitGroup && unitGroup->IsCheckSumValid() &&
		targetUnit && targetUnit->IsCheckSumValidForAUnitClass());
	if (!infAI || !infAI->IsCheckSumValid() || !unitGroup || !unitGroup->IsCheckSumValid() ||
		!targetUnit || !targetUnit->IsCheckSumValidForAUnitClass()) {
		return 0;
	}
	if (unitGroup->unitCount <= 0) {
		return 0;
	}
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) {
		return 0;
	}
	assert(unitGroup->unitCount <= 40);
	float totalDamagePerSecond = 0;
	for (int i = 0; i < unitGroup->unitCount; i++) {
		long int myUnitId = unitGroup->GetMyUnitId(i);
		STRUCT_UNIT_BASE *myUnit = NULL;
		if (myUnitId > -1) {
			myUnit = global->GetUnitFromId(myUnitId);
		}
		if (myUnit && myUnit->IsCheckSumValidForAUnitClass()) {
			float myDamage = AOE_METHODS::UNIT::CalcDamage(myUnit, targetUnit);
			float myReloadTime = AOE_METHODS::UNIT::GetReloadTime1(myUnit);
			totalDamagePerSecond += (myDamage / myReloadTime);
		}
	}
	return totalDamagePerSecond;
}


}
