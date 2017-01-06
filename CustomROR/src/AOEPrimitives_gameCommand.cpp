#include "../include/AOEPrimitives_gameCommand.h"

namespace AOE_METHODS {
;

// commandStruct must have been allocated with a "AOE" alloc method like AOEAlloc(...)
// It is freed by game code, don't use it / free it afterwards !
// Returns false if failed
bool AddCommandToGameCmdQueue(void *commandStruct, long int structSize) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_GAME_COMMANDS_INFO *cmdInfo = global->commandsInfo;
	assert(cmdInfo && cmdInfo->IsCheckSumValid());
	if (!cmdInfo || !cmdInfo->IsCheckSumValid()) {
		return false;
	}
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	const unsigned long int addr = 0x42B6E0; // commandsInfo.addCommandToList(ptrCommand, structSize)
	_asm {
		PUSH structSize;
		PUSH commandStruct;
		MOV EAX, addr;
		MOV ECX, cmdInfo;
		CALL EAX; // returns void
	}
	return true;
}

}

namespace GAME_COMMANDS {

// Create a "ROR" command struct (right-click). Returns false if failed.
// targetUnitId is optional. If -1 or non valid, then unit will just move to posX/posY.
// This method is compatible with MP games (does NOT causes sync issue)
bool CreateCmd_RightClick(long int actorUnitId, long int targetUnitId, float posX, float posY) {
	AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE *unit = (STRUCT_UNIT_COMMANDABLE*)GetUnitStruct(actorUnitId);
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->DerivesFromCommandable()) {
		return false;
	}
	return CreateCmd_RightClick(&unit, 1, targetUnitId, posX, posY);
}

bool CreateCmd_RightClick(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE **actorUnitsList, long int actorUnitsCount, long int targetUnitId, float posX, float posY) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) {
		return false;
	}
	assert(actorUnitsCount > 0);
	if (actorUnitsCount <= 0) { return false; }
	const long int structSize = 0x1C + (4 * actorUnitsCount);
	AOE_STRUCTURES::COMMAND_RIGHT_CLICK *cmd = (AOE_STRUCTURES::COMMAND_RIGHT_CLICK *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_RIGHT_CLICK;
	assert(cmd->IsCmdIdValid());
	cmd->actorCount = (char)actorUnitsCount;
	for (int i = 0; i < actorUnitsCount; i++) {
		if (actorUnitsList[i]) {
			cmd->actorIdList[i] = actorUnitsList[i]->unitInstanceId;
		} else {
			cmd->actorIdList[i] = -1;
		}
	}
	cmd->targetUnitId = targetUnitId;
	cmd->posX = posX;
	cmd->posY = posY;
	cmd->humanPlayerId = 0; // See below, overwritten if MP game.
	cmd->targetVisibleInFog = 0; // default
	AOE_STRUCTURES::STRUCT_UNIT_BASE *targetUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(targetUnitId);
	if (targetUnit && targetUnit->IsCheckSumValidForAUnitClass()) {
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *targetDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *) targetUnit->unitDefinition;
		if (targetDef && targetDef->IsCheckSumValidForAUnitClass()) {
			cmd->targetVisibleInFog = targetDef->visibleInFog;
		}
	}
	if (IsMultiplayer()) {
		cmd->humanPlayerId = (char)global->humanPlayerId;
#pragma message("MP Not fully supported at this point")
		// Not handled at this point
		// See 0x42B7ED...
		cmd->visibilityMask->explorationVisibilityMask = 0; // TO DO
		cmd->visibilityMask->fogVisibilityMask = 0; // TO DO
		cmd->sharedExploration = 0; // TO DO
		cmd->unknown_13 = 0; // TO DO
		cmd->unknown_14 = 0; // TO DO
	}
	// Our command is ready. Call API to add it in queue.
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (stop). Returns false if failed.
bool CreateCmd_Stop(long int actorUnitId) {
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(actorUnitId);
	if (!unit || !unit->IsCheckSumValidForAUnitClass() || !unit->ptrStructPlayer || !unit->ptrStructPlayer->IsCheckSumValid()) {
		return false;
	}
	const long int structSize = 0x06; // 0x02 + 4*1 (1 actor unit)... Unusual, but correct !
	AOE_STRUCTURES::COMMAND_STOP_ACTION *cmd = (AOE_STRUCTURES::COMMAND_STOP_ACTION *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_STOP_ACTION;
	assert(cmd->IsCmdIdValid());
	cmd->actorCount = 1;
	cmd->unitId[0] = actorUnitId;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (build). Returns false if failed.
// The building is created at construction step (status=0, HP=1)
// ONLY for building (type 80)
bool CreateCmd_Build(long int actorUnitId, short int DATID, float posX, float posY) {
	assert(DATID >= 0);
	AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)GetUnitStruct(actorUnitId);
	if ((DATID < 0) || !unit || !unit->IsCheckSumValidForAUnitClass() || !unit->ptrStructPlayer || !unit->ptrStructPlayer->IsCheckSumValid()) {
		return false;
	}
	const long int structSize = 0x18; // 0x14 + 4*1 (1 actor unit)
	AOE_STRUCTURES::COMMAND_MAKE_OBJECT *cmd = (AOE_STRUCTURES::COMMAND_MAKE_OBJECT *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_BUILD;
	assert(cmd->IsCmdIdValid());
	cmd->builderIdList[0] = actorUnitId;
	cmd->builderCount = 1;
	cmd->playerId = (char)unit->ptrStructPlayer->playerId;
	cmd->unitDefIdToBuild = DATID;
	cmd->stratElemId = -1;
	cmd->posX = posX;
	cmd->posY = posY;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (build multiple = walls-like). Returns false if failed.
// The building is created at construction step (status=0, HP=1)
// ONLY for building (type 80)
// actorUnitId can be -1 (no builder)
bool CreateCmd_BuildMultiple(long int playerId, long int actorUnitId, short int DATID, float minPosX, float minPosY, float maxPosX, float maxPosY) {
	assert((DATID >= 0) && (playerId >= 0) && (playerId < 9));
	const long int structSize = 0x14; // 0x10 + 4*1 (1 actor unit)
	AOE_STRUCTURES::COMMAND_BUILD_MULTIPLE *cmd = (AOE_STRUCTURES::COMMAND_BUILD_MULTIPLE *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_BUILD_MULTIPLE;
	cmd->builderCount = 1;
	assert(cmd->IsCmdIdValid());
	assert(cmd->MySizeInBytes() == structSize);
	cmd->builderIdList[0] = actorUnitId;
	cmd->playerId = (char)playerId;
	cmd->unitDefIdToBuild = DATID;
	cmd->stratElemId = -1;
	cmd->minPosX = (char)minPosX;
	cmd->minPosY = (char)minPosY;
	cmd->maxPosX = (char)maxPosX;
	cmd->maxPosY = (char)maxPosY;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (build). Returns false if failed.
// The building is created at construction step (status=0, HP=1)
// ONLY for building (type 80)
bool CreateCmd_Build_withoutBuilder(long int playerId, short int DATID, float posX, float posY) {
	assert((playerId >= 0) && (playerId < 9));
	assert(DATID >= 0);
	if ((playerId < 0) || (playerId > 8) || (DATID < 0)) { return false; }
	const long int structSize = 0x18; // 0x14 + 4*1 (1 actor unit)
	AOE_STRUCTURES::COMMAND_MAKE_OBJECT *cmd = (AOE_STRUCTURES::COMMAND_MAKE_OBJECT *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_BUILD;
	assert(cmd->IsCmdIdValid());
	cmd->builderIdList[0] = -1;
	cmd->builderCount = 1;
	cmd->playerId = (char)playerId;
	cmd->unitDefIdToBuild = DATID;
	cmd->stratElemId = -1;
	cmd->posX = posX;
	cmd->posY = posY;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (build). Returns false if failed. This creates a fully-built building.
// Important note: the unit WILL actually be created even if map position/terrain restriction/conflicting units are invalid
// This also works to create non-building units
// WARNING: the game will crash if DATID is invalid for provided player
bool CreateCmd_CompleteBuild(long int playerId, short int unitDefIdToCreate, float posX, float posY) {
	assert((playerId >= 0) && (playerId <= 8));
	if ((playerId < 0) || (playerId > 8) || (unitDefIdToCreate < 0)) { return false; }
	const long int structSize = sizeof(COMMAND_CREATE);
	AOE_STRUCTURES::COMMAND_CREATE *cmd = (AOE_STRUCTURES::COMMAND_CREATE *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CREATE;
	assert(cmd->IsCmdIdValid());
	cmd->playerId = (short int)playerId;
	cmd->unitDefIdToCreate = unitDefIdToCreate;
	cmd->posX = posX;
	cmd->posY = posY;
	cmd->posZ = -1;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Takes top-left unit as leader (lol). Other units move according to the formation specified (base = leader position)
// This just orders a movement action, the formation is not preserved afterwards, and may not work well if units are moving (especially leader)
// Not sure this always works very well, some units may ignore the movement.
bool CreateCmd_Formation(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE **actorUnitsList, long int actorUnitsCount, INTERNAL_UNIT_FORMATION_ID formationType) {
	assert(actorUnitsCount > 0);
	if (actorUnitsCount <= 0) { return false; }
	const long int structSize = 0x08 + (4 * actorUnitsCount);
	AOE_STRUCTURES::COMMAND_FORMATION *cmd = (AOE_STRUCTURES::COMMAND_FORMATION *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_FORMATION;
	assert(cmd->IsCmdIdValid());
	cmd->unitCount = actorUnitsCount;
	cmd->formationType = formationType;
	for (int i = 0; i < actorUnitsCount; i++) {
		if (actorUnitsList[i]) {
			assert(actorUnitsList[i]->IsCheckSumValidForAUnitClass());
			cmd->unitIdArray[i] = actorUnitsList[i]->unitInstanceId;
		} else {
			cmd->unitIdArray[i] = -1;
		}
	}
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Creates a "ROR" command to pay a (tribute free !) tribute from a player to another one.
// "fromPlayerId" player must have enough resources to pay or the command will have no effect.
bool CreateCmd_GiveResource(long int fromPlayerId, long int toPlayerId, long int resourceType, float resourceAmount) {
	assert((fromPlayerId >= 0) && (fromPlayerId < 9) && (toPlayerId >= 0) && (toPlayerId < 9));
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_GIVE_RESOURCE);
	AOE_STRUCTURES::COMMAND_GIVE_RESOURCE *cmd = (AOE_STRUCTURES::COMMAND_GIVE_RESOURCE *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_GIVE_RESOURCE;
	assert(cmd->IsCmdIdValid());
	cmd->fromPlayerId = (char)fromPlayerId;
	cmd->toPlayerId = (char)toPlayerId;
	cmd->resourceId = (char)resourceType;
	cmd->resourceValue = resourceAmount;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (change diplomacy). Returns false if failed.
bool CreateCmd_ChangeDiplomacy(short int playerId, short int targetPlayerId, PLAYER_DIPLOMACY_STANCES diplomacyStance) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_CHANGE_DIPLOMACY);
	AOE_STRUCTURES::COMMAND_CHANGE_DIPLOMACY *cmd = (AOE_STRUCTURES::COMMAND_CHANGE_DIPLOMACY *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_CHANGE_DIPLOMACY;
	assert(cmd->IsCmdIdValid());
	cmd->actorPlayerId = playerId;
	cmd->targetPlayerId = targetPlayerId;
	cmd->diplomacyStance = (short int)diplomacyStance;
	cmd->floatDiplomacyStance = (float)diplomacyStance;
	cmd->unknown_6 = 0;
	cmd->unused_E = 0;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (set ally victory flag). Returns false if failed.
bool CreateCmd_SetAllyVictory(short int playerId, bool enable) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_SET_ALLY_VICTORY);
	AOE_STRUCTURES::COMMAND_SET_ALLY_VICTORY *cmd = (AOE_STRUCTURES::COMMAND_SET_ALLY_VICTORY *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_SET_ALLY_VICTORY;
	assert(cmd->IsCmdIdValid());
	cmd->actorPlayerId = playerId;
	cmd->allyVictoryFlag = (short int)enable;
	cmd->unknown_6 = 0;
	cmd->unused_8 = 0;
	cmd->unknown_C = 0;
	cmd->unused_E = 0;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (give writing - set shared exploration). Returns false if failed.
bool CreateCmd_SetSharedExploration(short int playerId) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_APPLY_WRITING);
	AOE_STRUCTURES::COMMAND_APPLY_WRITING *cmd = (AOE_STRUCTURES::COMMAND_APPLY_WRITING *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_APPLY_WRITING_TECH;
	assert(cmd->IsCmdIdValid());
	cmd->playerId = playerId;
	cmd->unused_4 = 0;
	cmd->unused_6 = 0;
	cmd->unused_8 = 0;
	cmd->unused_C = 0;
	cmd->unused_E = 0;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (change game speed). Returns false if failed.
bool CreateCmd_ChangeGameSpeed(float newSpeed) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_CHANGE_GAME_SPEED);
	AOE_STRUCTURES::COMMAND_CHANGE_GAME_SPEED *cmd = (AOE_STRUCTURES::COMMAND_CHANGE_GAME_SPEED *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_CHANGE_GAME_SPEED;
	assert(cmd->IsCmdIdValid());
	cmd->newSpeed = newSpeed;
	cmd->unused_2 = 0;
	cmd->unused_4 = 0;
	cmd->unused_6 = 0;
	cmd->unused_C = 0;
	cmd->unused_E = 0;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (add a resource amount to a player). Returns false if failed.
bool CreateCmd_AddResource(short int playerId, short int resourceId, float value) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_ADD_RESOURCE);
	AOE_STRUCTURES::COMMAND_ADD_RESOURCE *cmd = (AOE_STRUCTURES::COMMAND_ADD_RESOURCE *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_ADD_RESOURCE;
	assert(cmd->IsCmdIdValid());
	cmd->actorPlayerId = playerId;
	cmd->resourceId = resourceId;
	cmd->resourceAmount = value;
	cmd->unused_6 = 0;
	cmd->unused_C = 0;
	cmd->unused_E = 0;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (set ally victory flag). Returns false if failed.
bool CreateCmd_SetSteroids(bool enable) {
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_SET_STEROIDS_MODE);
	AOE_STRUCTURES::COMMAND_SET_STEROIDS_MODE *cmd = (AOE_STRUCTURES::COMMAND_SET_STEROIDS_MODE *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_CHANGE_SETTING;
	cmd->subTypeId = AOE_CONST_INTERNAL::INTERNAL_COMMAND67_SUBTYPE::CST_IC67_SET_STEROIDS_MODE;
	assert(cmd->IsCmdIdValid());
	cmd->steroidsModeFlag = (short int)enable;
	cmd->unused_4 = 0;
	cmd->unused_6 = 0;
	cmd->unused_8 = 0;
	cmd->unused_C = 0;
	cmd->unused_E = 0;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (pay tribute). Returns false if failed.
bool CreateCmd_PayTribute(long int actorPlayerId, long int targetPlayerId, AOE_CONST_FUNC::RESOURCE_TYPES resourceType, float amount, float tributeInefficiency) {
	assert((actorPlayerId >= 0) && (targetPlayerId >= 0));
	if ((actorPlayerId < 0) || (targetPlayerId < 0)) { return false; }
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_PAY_TRIBUTE);
	AOE_STRUCTURES::COMMAND_PAY_TRIBUTE *cmd = (AOE_STRUCTURES::COMMAND_PAY_TRIBUTE *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_PAY_TRIBUTE;
	assert(cmd->IsCmdIdValid());
	cmd->actorPlayerId = (char)actorPlayerId;
	cmd->targetPlayerId = (char)targetPlayerId;
	cmd->resourceId = (char)resourceType;
	cmd->amount = amount;
	cmd->tributeTaxProportion = tributeInefficiency;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (attack position). Returns false if failed.
bool CreateCmd_AttackPosition(long int actorUnitId, float posX, float posY) {
	assert(actorUnitId >= 0);
	if (actorUnitId < 0) { return false; }
	const long int structSize = 0x0C + 4; // Size for 1 unit in array
	AOE_STRUCTURES::COMMAND_ATTACK_POSITION *cmd = (AOE_STRUCTURES::COMMAND_ATTACK_POSITION *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_ATTACK_POSITION;
	cmd->actorCount = 1;
	assert(cmd->IsCmdIdValid());
	assert(cmd->MySizeInBytes() == structSize);
	cmd->actorIdList[0] = actorUnitId;
	cmd->posX = posX;
	cmd->posY = posY;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (explore). Returns false if failed.
// Seems to do no more than "move to"
bool CreateCmd_Explore(long int actorPlayerId, long int actorUnitId, float posX, float posY) {
	assert((actorUnitId >= 0) && (actorPlayerId >= 0));
	if ((actorUnitId < 0) || (actorPlayerId < 0)) { return false; }
	const long int structSize = 0x0C + 4; // Size for 1 unit in array
	AOE_STRUCTURES::COMMAND_EXPLORE *cmd = (AOE_STRUCTURES::COMMAND_EXPLORE *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_EXPLORE;
	cmd->actorCount = 1;
	assert(cmd->IsCmdIdValid());
	assert(cmd->MySizeInBytes() == structSize);
	cmd->actorIdList[0] = actorUnitId;
	cmd->posX = posX;
	cmd->posY = posY;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}


// Create a "ROR" command struct (kill a unit). Returns false if failed.
// WARNING: do not call this for units that don't derive from trainable (living units) because unit->kill() method wouldn't exist (you'll get a crash)
bool CreateCmd_KillUnit(long int unitId) {
	assert(unitId >= 0);
	if (unitId < 0) { return false; }
	const long int structSize = sizeof(AOE_STRUCTURES::COMMAND_KILL_UNIT);
	AOE_STRUCTURES::COMMAND_KILL_UNIT *cmd = (AOE_STRUCTURES::COMMAND_KILL_UNIT *)AOEAllocZeroMemory(1, structSize);
	cmd->cmdId = AOE_CONST_INTERNAL::INTERNAL_COMMAND_ID::CST_ICI_KILL_UNIT;
	assert(cmd->IsCmdIdValid());
	cmd->unitId = unitId;
	return AOE_METHODS::AddCommandToGameCmdQueue(cmd, structSize);
}

}
