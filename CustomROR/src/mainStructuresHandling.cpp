#include "../include/mainStructuresHandling.h"

using namespace AOE_STRUCTURES;

// Returns a pointer to global game struct
// Warning: can sometimes return NULL when called very early (when the game has just been run)
STRUCT_GAME_GLOBAL* GetGameGlobalStructPtr() {
	STRUCT_GAME_SETTINGS *settings = (STRUCT_GAME_SETTINGS*)*ROR_gameSettings; // Otherwise, find it in game settings
	assert(settings != NULL);
	if (!settings) { return NULL; }
	return settings->ptrGlobalStruct;
}


// Warning: this can fail when called from constructors (ROR_gameSettings might not be set)
STRUCT_GAME_SETTINGS* GetGameSettingsPtr() {
	if (!ROR_gameSettings) { return NULL; }
	return (STRUCT_GAME_SETTINGS*)*ROR_gameSettings;
}


// Returns the UI main information object
AOE_STRUCTURES::STRUCT_UI_PANEL_SYSTEM *GetUIMainInfoStruct() {
	return ROR_pUIMainInfo;
}


// Returns a pointer to current active UI (?)
// ASSERTS that the pointer is non-NULL
STRUCT_ANY_UI *GetCurrentUIStruct() {
	STRUCT_ANY_UI **p = (STRUCT_ANY_UI **)AOE_OFFSETS::ADDR_VAR_ACTIVE_UI_STRUCT;
	assert(*p != NULL);
	return *p;
}


// Returns currently human-controlled player struct according to game settings struct
STRUCT_PLAYER *GetControlledPlayerStruct_Settings() {
	STRUCT_GAME_SETTINGS *gameSettings = GetGameSettingsPtr();
	STRUCT_PLAYER *player;
	_asm {
		MOV ECX, gameSettings;
		MOV EAX, 0x419B70;
		CALL EAX;
		MOV player, EAX;
	}
	return player;
}


// Returns player struct for given player id
// returns NULL if incorrect player id or if some structures are invalid/not found
STRUCT_PLAYER *GetPlayerStruct(long int playerId) {
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid() || global->playerTotalCount < 1) { return NULL; }
	if ((playerId < 0) || (playerId >= global->playerTotalCount) || (playerId >= 9)) {
		return NULL;
	}
	STRUCT_PLAYER **playersArray = global->GetPlayerStructPtrTable();
	if (!playersArray) {
		return NULL;
	}
	return playersArray[playerId];
}


// Useful to get structure from a unit id. May return NULL !
// Only works for creatable (unitId >= 0). This is just a choice to avoid writing same bugs as ROR
// (some functions use -1 as <No unit> but get an irrevant unit struct then because -1 is not tested before calling getUnitStruct(...))
AOE_STRUCTURES::STRUCT_UNIT_BASE *GetUnitStruct(long int unitId) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || (unitId < 0)) { return NULL; }
	AOE_STRUCTURES::STRUCT_UNIT_BASE *result;
	_asm {
		MOV ECX, global
			PUSH unitId
			MOV EAX, 0x005206D0
			CALL EAX
			MOV result, EAX
	}
	return result;
}


// Returns a unit definition if valid, NULL otherwise.
AOE_STRUCTURES::STRUCT_UNITDEF_BASE *GetUnitDefStruct(AOE_STRUCTURES::STRUCT_PLAYER *player, short int unitDefId) {
	if (!player || !player->IsCheckSumValid() || (unitDefId < 0)) { return NULL; }
	if (unitDefId >= player->structDefUnitArraySize) { return NULL; }
	return player->ptrStructDefUnitTable[unitDefId];
}


AOE_STRUCTURES::STRUCT_CURSOR_SLP_INFO *GetCursorInfo() {
	return *(AOE_STRUCTURES::STRUCT_CURSOR_SLP_INFO**)AOE_VAR_CURSORS_INFO;
}


bool IsMultiplayer() {
#pragma message("TODO: bugs in IsMultiplayer when running MP and then SP games")
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = (AOE_STRUCTURES::STRUCT_GAME_SETTINGS *)AOE_OFFSETS::ADDR_VAR_GAME_SETTINGS_STRUCT;
	if (settings == NULL) { return false; }
	return (settings->rgeGameOptions.isMultiPlayer != 0);
}


// Returns true if the game is currently running
bool IsGameRunning() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) {
		return false;
	}
	if (global->gameRunStatus != 0) {
		return false;
	}

	return (settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING);
}
