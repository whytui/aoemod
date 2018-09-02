
#pragma once

#include <assert.h>
#include <string>
#include <AOE_struct_game_settings.h>
#include "AOE_memory.h"
#include "AOE_commands.h"
#include "mainStructuresHandling.h"

/* This file contains AOE primitives about game commands handling
 * Game commands are async small commands that are sent to host player (if MP game)
 * so that game behaviour/actions/changes remain synchronous on all playing computer.
 * Using those 'game commands' are the only way to guarantee sync safety in MP games when running custom treatments on the game.
*/

using namespace AOE_STRUCTURES;

namespace AOE_METHODS {

	// commandStruct must have been allocated with a "AOE" alloc method like AOEAlloc(...)
	// It is freed by game code, don't use it / free it afterwards !
	// Returns false if failed
	bool AddCommandToGameCmdQueue(void *commandStruct, long int structSize);

}

namespace GAME_COMMANDS {
	// Create a "ROR" command struct (right-click). Returns false if failed.
	bool CreateCmd_RightClick(long int actorUnitId, long int targetUnitId, float posX, float posY);
	// Create a "ROR" command struct (right-click). Returns false if failed.
	// MP support is unfinished !
	// Note: there might be "non-commandable" units in the array, it's not a problem.
	// The game will check if the command makes sense individually for each unit
	bool CreateCmd_RightClick(AOE_STRUCTURES::STRUCT_UNIT_BASE **actorUnitsList, long int actorUnitsCount, long int targetUnitId, float posX, float posY);

	// Create a "ROR" command struct (stop). Returns false if failed.
	bool CreateCmd_Stop(long int actorUnitId);

	// Create a "ROR" command struct (build). Returns false if failed.
	// The building is created at construction step (status=0, HP=1)
	// ONLY for building (type 80)
	bool CreateCmd_Build(long int actorUnitId, short int DATID, float posX, float posY);

	// Create a "ROR" command struct (build multiple = walls-like). Returns false if failed.
	// The building is created at construction step (status=0, HP=1)
	// ONLY for building (type 80)
	bool CreateCmd_BuildMultiple(long int playerId, long int actorUnitId, short int DATID, float minPosX, float minPosY, float maxPosX, float maxPosY);

	// Create a "ROR" command struct (build). Returns false if failed.
	// The building is created at construction step (status=0, HP=1)
	// ONLY for building (type 80)
	bool CreateCmd_Build_withoutBuilder(long int playerId, short int DATID, float posX, float posY);

	// Create a "ROR" command struct (build). Returns false if failed. This creates a fully-built building
	// Important note: the unit WILL actually be created even if map position/terrain restriction/conflicting units are invalid
	// This also works to create non-building units
	bool CreateCmd_CompleteBuild(long int playerId, short int unitDefIdToCreate, float posX, float posY);

	// Takes top-left unit as leader (lol). Other units move according to the formation specified (base = leader position)
	// This just orders a movement action, the formation is not preserved afterwards, and may not work well if units are moving (especially leader)
	// Not sure this always works very well, some units may ignore the movement.
	bool CreateCmd_Formation(AOE_STRUCTURES::STRUCT_UNIT_COMMANDABLE **actorUnitsList, long int actorUnitsCount, INTERNAL_UNIT_FORMATION_ID formationType);

	// Creates a "ROR" command to pay a (tribute free !) tribute from a player to another one.
	// "fromPlayerId" player must have enough resources to pay or the command will have no effect.
	bool CreateCmd_GiveResource(long int fromPlayerId, long int toPlayerId, long int resourceType, float resourceAmount);

	// Create a "ROR" command struct (change diplomacy). Returns false if failed.
	bool CreateCmd_ChangeDiplomacy(short int playerId, short int targetPlayerId, PLAYER_DIPLOMACY_STANCES diplomacyStance);

	// Create a "ROR" command struct (set ally victory flag). Returns false if failed.
	bool CreateCmd_SetAllyVictory(short int playerId, bool enable);

	// Create a "ROR" command struct (give writing - set shared exploration). Returns false if failed.
	bool CreateCmd_SetSharedExploration(short int playerId);

	// Create a "ROR" command struct (change game speed). Returns false if failed.
	bool CreateCmd_ChangeGameSpeed(float newSpeed);

	// Create a "ROR" command struct (add a resource amount to a player). Returns false if failed.
	bool CreateCmd_AddResource(short int playerId, short int resourceId, float value);

	// Create a "ROR" command struct (set steroids mode ON/OFF). Returns false if failed.
	bool CreateCmd_SetSteroids(bool enable);

	// Create a "ROR" command struct (pay tribute). Returns false if failed.
	bool CreateCmd_PayTribute(long int actorPlayerId, long int targetPlayerId, AOE_CONST_FUNC::RESOURCE_TYPES resourceType, float amount, float tributeInefficiency);

	// Create a "ROR" command struct (attack position). Returns false if failed.
	bool CreateCmd_AttackPosition(long int actorUnitId, float posX, float posY);

	// Create a "ROR" command struct (explore). Returns false if failed.
	// Seems to do no more than "move to"
	bool CreateCmd_Explore(long int actorPlayerId, long int actorUnitId, float posX, float posY);

	// Create a "ROR" command struct (kill a unit). Returns false if failed.
	// WARNING: do not call this for units that don't derive from trainable (living units) because unit->kill() method wouldn't exist (you'll get a crash)
	bool CreateCmd_KillUnit(long int unitId);

}
