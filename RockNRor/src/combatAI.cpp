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



namespace ROCKNROR {
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
		bool newTargetCanAttack = !newTargetIsVillager && UnitDefCanAttack(newTargetUnit->unitDefinition, false);
		bool oldTargetCanAttack = !actionTargetIsVillager && UnitDefCanAttack(oldTargetUnit->unitDefinition, false);
		if (newTargetIsVillager && !actionTargetIsVillager && oldTargetCanAttack) {
			return false; // Current is NOT a villager AND is miltary, attack it (keep)
		}
		if (!newTargetIsVillager && actionTargetIsVillager && newTargetCanAttack) {
			return true; // New is NOT a villager AND is miltary, attack it (change)
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


// Handles "retreat after shooting" feature for hunters (non-native feature !)
// Returns true if an action (movement) could be triggered
// Returns false if error or no movement triggered
bool HunterMoveBackAfterShooting(STRUCT_UNIT_ACTIVITY *unitActivity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notify) {
	// This is restricted to "AI improvements ON" configuration.
	if (!unitActivity || !unitActivity->IsCheckSumValid() || !notify) { return false; }
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	// This is restricted to medium/hard/hardest levels
	if (!settings || !settings->IsCheckSumValid() || (settings->rgeGameOptions.difficultyLevel > GAME_DIFFICULTY_LEVEL::GDL_MEDIUM)) { return false; }
	STRUCT_UNIT_BASE *unit = unitActivity->ptrUnit;
	if (!unit || !unit->IsCheckSumValidForAUnitClass()) { return false; }
	STRUCT_PLAYER *player = unit->ptrStructPlayer;
	if (!player || !player->IsCheckSumValid()) { return false; }
	if (!IsImproveAIEnabled(player->playerId)) { return false; } // This feature is an AI improvement. Do it only if config says so.
	if (!settings || !settings->IsCheckSumValid()) { return false; }
	if (settings->rgeGameOptions.difficultyLevel == GAME_DIFFICULTY_LEVEL::GDL_EASIEST) { return false; }
	if (player->isComputerControlled == 0) { return false; } // cf 0x4E6476
	if (unitActivity->targetUnitType != GLOBAL_UNIT_AI_TYPES::TribeAIGroupPredatorAnimal) { return false; } // This is only used for hunting aggressive animals.
	if ((unitActivity->currentTaskId != ACTIVITY_TASK_ID::CST_ATI_TASK_ATTACK) &&
		(unitActivity->currentTaskId != ACTIVITY_TASK_ID::CST_ATI_TASK_GATHER_ATTACK)) {
		return false;
	} // cf 0x4E64B2
	long int activityTargetUnitId = unitActivity->targetUnitId;
	STRUCT_UNIT_BASE *targetUnit = GetUnitStruct(activityTargetUnitId);
	if (targetUnit == NULL) { return false; } // No valid target: ignore notification

	STRUCT_UNIT_BASE *targetsTarget = AOE_STRUCTURES::GetAttackTarget(targetUnit);
	if ((targetsTarget != NULL) && (targetsTarget != unit)) { return false; } // Attacking someone else: I don't need to move away

	float range = AOE_METHODS::UNIT::GetMaxRange(unit);
	if (range < 1.f) { return false; } // note: this excludes non-hunter villagers.

	float contactDist = AOE_METHODS::UNIT::GetContactDistanceTo(unit, targetUnit);
	// dist is raw distance between the units (not counting unit radiuses). Original code takes radius into account. No need here for hunters (simplify).
	float dist = GetDistance(unit->positionX, unit->positionY, targetUnit->positionX, targetUnit->positionY);
	float allowedMovingRange = range - contactDist; // actor.maxRange - contactDistance
	if (allowedMovingRange < 1) { return false; } // cf 0x4E64E9. Do nothing if too far or already at max shooting range

	long int priority_unsure = unitActivity->currentOrderPriority;
	float activityMaxDistance = unitActivity->desiredTargetDistance; // for arg8
	float activityTargetPosX = unitActivity->targetPosX;
	float activityTargetPosY = unitActivity->targetPosY;
	float activityTargetPosZ = unitActivity->targetPosZ;
	int attackTaskId = UNIT_AI_ORDER::CST_ORDER_GATHER_ATTACK; // CST_ORDER_GATHER_ATTACK for military units...
	long int actorPlayerId = player->playerId;

	_asm {
		MOV ECX, unitActivity;
		PUSH priority_unsure;
		PUSH 1;
		PUSH 0;
		PUSH activityMaxDistance; // arg8
		PUSH activityTargetPosZ;
		PUSH activityTargetPosX;
		PUSH activityTargetPosY; // arg5
		PUSH -1; // target player id
		PUSH activityTargetUnitId;
		PUSH attackTaskId;
		PUSH actorPlayerId;
		MOV EDX, DWORD PTR DS : [ECX];
		CALL DS:[EDX + 0x10]; // activity.order
	}
	if (unitActivity->unitIDsThatAttackMe.usedElements > 0) {
		// A hunter wouldn't be hunting if being attacked (unless defending against a lion), so we may ignore "attacking" units here (otherwise, see 0x4E6547)
	}

	float diffX = targetUnit->positionX - unit->positionX;
	float diffY = targetUnit->positionY - unit->positionY;
	float relativeX = diffX / dist;
	float relativeY = diffY / dist;
	if ((unit == targetsTarget) && (settings->rgeGameOptions.difficultyLevel <= GAME_DIFFICULTY_LEVEL::GDL_HARD)) {
		if (targetUnit->unitDefinition && targetUnit->unitDefinition->DerivesFromFlag() && unit->unitDefinition->DerivesFromFlag() &&
			((STRUCT_UNITDEF_FLAG*)targetUnit->unitDefinition)->speed > ((STRUCT_UNITDEF_FLAG*)unit->unitDefinition)->speed) {
			// Animal is faster than me, or equivalent: move far enough (if other units are helping me, that will buy more time for them to shoot)
			allowedMovingRange += COMBAT_CONST::afterShootRetreatRangeForHunterFastAnimal; // Move further if "I" am animal's current target.
		} else {
			// Slow animal (alligator)
			allowedMovingRange += COMBAT_CONST::afterShootRetreatRangeForHunterSlowAnimal; // Move further if "I" am animal's current target.
		}
	}
	relativeX *= allowedMovingRange;
	relativeY *= allowedMovingRange;
	float newX = unit->positionX - relativeX;
	float newY = unit->positionY - relativeY;
	long int callResult;
	// Move away from animal after shooting...
	_asm {
		PUSH 1;
		PUSH 0;
		PUSH activityTargetPosZ;
		PUSH newX;
		PUSH newY;
		MOV ECX, unitActivity;
		MOV EDX, DS:[ECX];
		CALL DS:[EDX + 0xA0];
		MOV callResult, EAX;
	}
	return (callResult == 1);
}


// Triggered when a unit sees another unit around (cf EVENT_PLAYER_SEE_UNIT), even if actor unit is not idle
// Remark: feeding infAI list is handled in parent calls.
// This method is called only if IsImproveAIEnabled(...) is true.
// Note: the units that are detected via this event are limited (cf 0x413108)
// - unit.status <=2 (the temp results contains other units(?), but this is filtered in 0x41312B before sending EVENT_PLAYER_SEE_UNIT.
// - neutral and enemy units only (so, not gaia !)
// - unit.definition.unitAIType is one of the "important" AItypes defined in actorUnit.activity.importantAITypes
void OnSeeNearbyUnit(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *actorUnit, STRUCT_UNIT_BASE *seenUnit) {
	if (!actorUnit || !seenUnit || !player) { return; }
	if ((actorUnit->unitStatus != GAME_UNIT_STATUS::GUS_2_READY) || (seenUnit->unitStatus > GAME_UNIT_STATUS::GUS_2_READY)) { return; }
	STRUCT_PLAYER *otherPlayer = seenUnit->ptrStructPlayer;
	STRUCT_UNITDEF_BASE *seenUnitDef = seenUnit->unitDefinition;
	
	if (!player->isComputerControlled) { return; } // should not happen

	if (seenUnitDef->unitAIType == TribeAIGroupPriest) {
		// Priests have a specific behaviour...
		OnPriestSeeNearbyUnit(player, actorUnit, seenUnit);
		return;
	}

	// Not applicable to siege weapons (let siege attack buildings ?)
	if (seenUnitDef->unitAIType == TribeAIGroupSiegeWeapon) { return; }

	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	// Medium/Easy/easiest levels: do not use this improvement.
	if (!settings || !settings->IsCheckSumValid() || (settings->rgeGameOptions.difficultyLevel > GAME_DIFFICULTY_LEVEL::GDL_HARD)) { return; }

	// The goal here is to take into account 'seen' unit and possible change target if necessary
	bool isNeutralOrEnemy = (player->ptrDiplomacyStances[otherPlayer->playerId] != AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_STANCES::CST_PDS_ALLY);
	assert(isNeutralOrEnemy); // because ROR's EVENT_PLAYER_SEE_UNIT handler collect neutral/enemy units only
	bool seenUnitCanAttack = UnitDefCanAttack(seenUnitDef, false);
	bool isWeakBuilding = (seenUnitDef->unitAIType == GLOBAL_UNIT_AI_TYPES::TribeAIGroupBuilding) && (seenUnit->remainingHitPoints < 2); // So that my unit will try to destroy new buildings with 1 HP
	bool isImmobileLivingUnit = false;
	if (seenUnit->DerivesFromAttackable() && (seenUnitDef->unitAIType != TribeAIGroupBuilding)) { // Exclude buildings !
		STRUCT_UNIT_ATTACKABLE *seenUnitAttackable = (STRUCT_UNIT_ATTACKABLE *)seenUnit;
		isImmobileLivingUnit = (seenUnitAttackable->currentMovementSpeed == 0);
	}
	bool seenUnitIsImportant = seenUnitCanAttack || isWeakBuilding || isImmobileLivingUnit;
	STRUCT_ACTION_ATTACK *action = (STRUCT_ACTION_ATTACK*)GetUnitAction(actorUnit);
	if (action && !action->IsCheckSumValid()) { action = NULL; } // not an *attack* action: ignore

	STRUCT_UNITDEF_BASE *myUnitDef = actorUnit->unitDefinition;
	if (isNeutralOrEnemy && seenUnitIsImportant && UnitDefCanAttack(myUnitDef, false) && actorUnit->currentActivity && actorUnit->currentActivity->IsCheckSumValid()) {
		STRUCT_UNIT_ACTIVITY *activity = actorUnit->currentActivity;
		AOE_CONST_INTERNAL::UNIT_AI_ORDER nextOrder = UNIT_AI_ORDER::CST_ORDER_NONE;
		long int nextOrderTargetId = -1;
		if (activity->orderQueueUsedElemCount > 0) {
			nextOrder = activity->orderQueue[0].orderId; // Note: if "attack", we might update this pending order if we wish to change target
			nextOrderTargetId = activity->orderQueue[0].targetUnitId;
		}
		bool canInterrupt = (nextOrder == UNIT_AI_ORDER::CST_ORDER_NONE) || (nextOrder == UNIT_AI_ORDER::CST_ORDER_ATTACK);
		// TODO: found cases where task is -1 but currently attacking...
		if (canInterrupt && action && (action->actionTypeID == UNIT_ACTION_ID::CST_IAI_ATTACK)) {
			// If attacking a low-priority target (non-tower building) and military unit/villager is around, switch
			// If attacking a *civilian* that is out of reach AND running faster, allow switching
			bool currentTargetIsImportant = false;
			
			if (action && action->IsCheckSumValid() && (action->actionTypeID == action->GetExpectedInternalActionId())) {
				STRUCT_UNIT_BASE *currentTarget = action->targetUnit; // "this" unit's action current target
				STRUCT_UNIT_FLAG *currentTargetFlag = (STRUCT_UNIT_FLAG *)action->targetUnit;
				if (!currentTarget) {
					currentTarget = GetUnitStruct(action->targetUnitId);
				}
				if (currentTarget && currentTarget->unitDefinition) {
					currentTargetIsImportant = (currentTarget->unitDefinition->DAT_ID1 == CST_UNITID_WONDER) ||
						(currentTarget->unitDefinition->unitAIType == TribeAIGroupCivilian) ||
						UnitDefCanAttack(currentTarget->unitDefinition) ||
						(currentTarget->remainingHitPoints < 2);
					// Special: faster & fleeing & out of reach civilian (hardest only)
					if ((settings->rgeGameOptions.difficultyLevel == GAME_DIFFICULTY_LEVEL::GDL_HARDEST) &&
						(currentTarget->unitDefinition->unitAIType == TribeAIGroupCivilian) &&
						currentTargetFlag->DerivesFromFlag() &&
						(currentTargetFlag->currentMovementSpeed > 0) && // currently moving
						((action->actionStatus == ACTION_STATUS::CST_AS_04_GOTO) || (action->actionStatus == ACTION_STATUS::CST_AS_05_GOTO2))
						) {
						float targetSpeed = AOE_METHODS::UNIT::GetSpeed(currentTarget);
						float mySpeed = AOE_METHODS::UNIT::GetSpeed(activity->ptrUnit);
						if (mySpeed * COMBAT_CONST::minimumSpeedProportionToFollowFleeingVillager <= targetSpeed) {
							currentTargetIsImportant = false; // Faster out of reach villager: let him run if there is another target around
						}
					}
				}
				if (currentTarget == seenUnit) {
					//currentTargetIsImportant = false; // TO DEBUG: find out for which situation I needed that ??
					currentTargetIsImportant = true;
				}
			}
			// A last validation : is the unit reachable ? We check this last because it uses pathfinding
			if (!currentTargetIsImportant) {
				float actorMaxRange = AOE_METHODS::UNIT::GetMaxRange(activity->ptrUnit);
				// set currentTargetIsImportant to true if seen unit can't be reached (force ignore seen unit)
				currentTargetIsImportant = !AOE_METHODS::UNIT::CanMoveToTarget(activity->ptrUnit, seenUnit->unitInstanceId, actorMaxRange, false);
			}

			if (!currentTargetIsImportant) {
				if (activity->orderQueueSize == 0) {
					// This should not happen as order queue is generally initialized with non-empty array !
					if (activity->orderQueue != NULL) {
						AOEFree(activity->orderQueue);
						activity->orderQueue = NULL;
					}
					const long int allocatedElemCount = 2;
					activity->orderQueue = (STRUCT_UNIT_ACTIVITY_ORDER_EVENT*)AOEAllocZeroMemory(allocatedElemCount, sizeof(STRUCT_UNIT_ACTIVITY_ORDER_EVENT));
					activity->orderQueueSize = allocatedElemCount;
				}
				
				bool doNotUpdateOrder = false;
				if ((nextOrderTargetId >= 0) && // If another "see unit" event already set an "next order target", choose which one is better
					(nextOrderTargetId != seenUnit->unitInstanceId) // make sure we call ShouldChangeTarget for distinct units !
					) {
					// Hack to use should change target: set actitivy's target temporarily
					long int backupActivityTargetId = activity->targetUnitId;
					activity->targetUnitId = nextOrderTargetId;
					// If "should not change target", then do NOT update pending order
					doNotUpdateOrder = !ShouldChangeTarget(activity, seenUnit->unitInstanceId);
					activity->targetUnitId = backupActivityTargetId; // (hack) restore correct target id
				}

				// Insert/update our "attack" order (unless we have decided NOT to change target)
				if (!doNotUpdateOrder) {
					// Force refresh (that will occur afterwards, when all the "see units" events are handled... normally)
					activity->targetUnitId = seenUnit->unitInstanceId;
					activity->targetUnitType = seenUnitDef->unitAIType;
					activity->currentTaskId = ACTIVITY_TASK_ID::CST_ATI_NONE;
					activity->orderId = UNIT_AI_ORDER::CST_ORDER_NONE;

					if (activity->orderQueueUsedElemCount == 0) {
						activity->orderQueueUsedElemCount = 1;
					}
					activity->orderQueue[0].actorUnitId = actorUnit->unitInstanceId;
					activity->orderQueue[0].orderId = UNIT_AI_ORDER::CST_ORDER_ATTACK;
					activity->orderQueue[0].targetUnitId = seenUnit->unitInstanceId;
					activity->orderQueue[0].targetPlayerId = otherPlayer->playerId;
					activity->orderQueue[0].priority = -1; // ?
					activity->orderQueue[0].maxRange = myUnitDef->lineOfSight;
					activity->orderQueue[0].posX = seenUnit->positionX;
					activity->orderQueue[0].posY = seenUnit->positionY;
					activity->orderQueue[0].posZ = seenUnit->positionZ;
				}
			}
		}
	}
}


// Triggered when a priest sees another unit around (cf EVENT_PLAYER_SEE_UNIT), even if actor unit is not idle
void OnPriestSeeNearbyUnit(STRUCT_PLAYER *player, STRUCT_UNIT_BASE *actorUnit, STRUCT_UNIT_BASE *seenUnit) {
	// TODO: evasive manoeuvre if faith is <100%.. Complex (take into account all allies and enemies)
	// TODO: heal immobile allied nearby target ?
	// TODO: other unit being healed: do not move away (using same event ?)
}


// Global update on idle soldiers for specified player (tacAI)
void OnAIUpdateIdleSoldiers(STRUCT_TAC_AI *tacAI, long int startProcessingTime, long int allowedProcessingTime) {
	if (!tacAI || !tacAI->IsCheckSumValid() || !tacAI->ptrMainAI) { return; }
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	STRUCT_PLAYER *player = tacAI->ptrMainAI->player;
	assert(player && player->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return; }
	for (int i = 0; i < tacAI->landMilitaryUnits.usedElements; i++) {
		long int curId = tacAI->landMilitaryUnits.unitIdArray[i];
		STRUCT_UNIT_BASE *curUnit = global->GetUnitFromId(curId);
		if (!curUnit || !curUnit->IsCheckSumValidForAUnitClass()) { continue; }
		if (!curUnit->currentActivity) { continue; }
		
		// Run checks on current activity consistency
		long int targetUnitId = curUnit->currentActivity->targetUnitId;
		STRUCT_UNIT_BASE *targetUnit = global->GetUnitFromId(targetUnitId);
		STRUCT_PLAYER *targetPlayer = NULL;
		bool targetIsAllied = false;
		if (targetUnit) {
			targetPlayer = targetUnit->ptrStructPlayer;
			PLAYER_DIPLOMACY_VALUES targetDiplValue = player->diplomacyVSPlayers[targetPlayer->playerId];
			if (targetUnit && ((targetDiplValue == PLAYER_DIPLOMACY_VALUES::CST_PDV_ALLY) || (targetDiplValue == PLAYER_DIPLOMACY_VALUES::CST_PDV_SELF))) {
				targetIsAllied = true;
			}
		} else {
			curUnit->currentActivity->targetUnitId = -1;
		}
		
		bool orderExpectsEnemy = false;
		bool orderExpectsAlly = true;
		switch (curUnit->currentActivity->orderId) {
		case AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_ATTACK:
		case AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_CONVERT:
		case AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_UNKNOWN_2D7_AI_PLAY:
		case AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_UNKNOWN_2D9_POP_TARGET:
			orderExpectsEnemy = true;
			orderExpectsAlly = false;
			break;
		case AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_NONE:
		case AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_GATHER_ATTACK:
		case AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_GATHER_NOATTACK:
		case AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_HOLD_POSITION:
		case AOE_CONST_INTERNAL::UNIT_AI_ORDER::CST_ORDER_FOLLOW_OBJECT:
			// Case NOT to take care of
			orderExpectsAlly = false;
		}

		bool taskExpectsEnemy = false;
		bool taskExpectsAlly = true;
		switch (curUnit->currentActivity->currentTaskId) {
		case ACTIVITY_TASK_ID::CST_ATI_TASK_ATTACK:
		case ACTIVITY_TASK_ID::CST_ATI_TASK_CONVERT:
		case ACTIVITY_TASK_ID::CST_ATI_TASK_SEEK_AND_DESTROY:
		case ACTIVITY_TASK_ID::CST_ATI_TASK_EXPLORE_AND_DESTROY:
			taskExpectsEnemy = true;
			taskExpectsAlly = false;
			break;
		case ACTIVITY_TASK_ID::CST_ATI_NONE:
			curUnit->currentActivity->targetUnitId = -1;
			// no break here
		case ACTIVITY_TASK_ID::CST_ATI_TASK_GATHER_ATTACK:
		case ACTIVITY_TASK_ID::CST_ATI_TASK_GATHER_NOATTACK:
		case ACTIVITY_TASK_ID::CST_ATI_TASK_FOLLOW_OBJECT:
			// Case NOT to take care of
			taskExpectsAlly = false;
		}

		bool doStop = false;
		if (targetUnit) {
			if (orderExpectsEnemy && targetIsAllied) {
				doStop = true;
				//ROCKNROR::SYSTEM::StopExecution(std::wstring(_T("OnAIUpdateIdleSoldiers: detected an error for unit order ; order expects enemy / #") + std::to_wstring(curId)).c_str(), true, true);
			}
			if (taskExpectsEnemy && targetIsAllied) {
				doStop = true;
				//ROCKNROR::SYSTEM::StopExecution(std::wstring(_T("OnAIUpdateIdleSoldiers: detected an error for unit order ; task expects enemy / #") + std::to_wstring(curId)).c_str(), true, true);
			}
			if (orderExpectsAlly && !targetIsAllied) {
				doStop = true;
				//ROCKNROR::SYSTEM::StopExecution(std::wstring(_T("OnAIUpdateIdleSoldiers: detected an error for unit order ; order expects ally / #") + std::to_wstring(curId)).c_str(), true, true);
			}
			if (taskExpectsAlly && !targetIsAllied) {
				doStop = true;
				//ROCKNROR::SYSTEM::StopExecution(std::wstring(_T("OnAIUpdateIdleSoldiers: detected an error for unit order ; task expects ally / #") + std::to_wstring(curId)).c_str(), true, true);
			}
		}
		if (doStop) {
			// Stop invalid actions : this still happens on some occasions, including priests (target killed or converted by another priest?),
			// some remaining bugs/missing updates (given we can't update everything each time a unit dies/is converted/etc)
			GAME_COMMANDS::CreateCmd_Stop(curId);
		}
	}
	
}


}
}
