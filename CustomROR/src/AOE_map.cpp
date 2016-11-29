#include "../include/AOE_map.h"

namespace AOE_METHODS {

// pathFindingStruct can be 0x583BC8 or 0x6A1CC0. Don't know the specific roles yet :(
// updateUnitPathInfo = "do move". If false, this just checks if movement is possible.
// arg15 : seen 0x1B hardcoded
long int CalcPathForMove(STRUCT_UNKNOWN_MAP_DATA_F04C *pathFindingStruct,
	long int srcPosY, long int srcPosX, long int destPosY, long int destPosX,
	AOE_STRUCTURES::STRUCT_UNIT_BASE *ptrActorUnit, float maxRange, long int targetUnitId, long int updateUnitPathInfo,
	long int arg9, long int arg10, long int arg11, long int arg12,
	long int distance_unsure, long int targetPlayerId, long int unknown_unitClass) {
	const unsigned long int callAddr = 0x458930;
#ifdef _DEBUG
	// DEBUG: run some data quality checks
	assert(pathFindingStruct && pathFindingStruct->IsCheckSumValid());
	assert((updateUnitPathInfo == 0) || (updateUnitPathInfo == 1));
	if (ptrActorUnit) {
		assert(ptrActorUnit->IsCheckSumValidForAUnitClass());
	}
	assert(targetUnitId >= -1);
#endif
	long int result = 0;
	_asm {
		MOV ECX, pathFindingStruct;
		PUSH unknown_unitClass;
		PUSH targetPlayerId;
		PUSH distance_unsure;
		PUSH arg12;
		PUSH arg11;
		PUSH arg10;
		PUSH arg9;
		PUSH updateUnitPathInfo;
		PUSH targetUnitId;
		PUSH maxRange;
		PUSH ptrActorUnit;
		PUSH destPosX;
		PUSH destPosY;
		PUSH srcPosX;
		PUSH srcPosY;
		CALL callAddr;
		MOV result, EAX;
	}
	return result;
}


	
// Calls AOE's path finding method, using 0x583BC8 (not 0x6A1CC0)
// allArgs indices are 1-15 (do NOT use 0). Warning, allArgs[6] is a float, not an int.
// Arguments (1-15) are:
// srcPosY, srcPosX, destPosY, destPosX, ptrActorUnit, f_range?, targetUnitId, updateUnitPathInfo?(!OnlyCheck),
// arg9, arg10, arg11, arg12, arg13(int_distance?), arg14(unitGrp?), arg15
// See also AOE_calcPathForMove
long int CallFindPathForUnit(long int allArgs[15 + 1]) {
	const unsigned long int myecx = 0x583BC8;
	const unsigned long int callAddr = 0x458930;
	long int result;
	_asm {
		MOV ECX, myecx
		MOV EAX, allArgs
		ADD EAX, 0x3C
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		PUSH DS:[EAX]
		SUB EAX, 4
		MOV EAX, callAddr
		CALL EAX
		MOV result, EAX
	}
#ifdef _DEBUG
	bool doTrace = false; // change this to enable/disable debug on call result.
	if (doTrace) {
		char buffer[800];
		// fix float arg
		float *pf = (float*)(&allArgs[6]);
		double f = (double)*pf;
		std::string s5 = GetHexStringAddress(allArgs[5]);
		std::string s7 = GetHexStringAddress(allArgs[7]);
		sprintf_s(buffer, 800, "%ld %ld %ld %ld %s %7.1f %s %d %d %d %d %d %d %d %d ; res=%d", allArgs[1], allArgs[2], allArgs[3], allArgs[4],
			s5.c_str(), f, s7.c_str(),
			allArgs[8], allArgs[9], allArgs[10], allArgs[11], allArgs[12], allArgs[13], allArgs[14], allArgs[15], result);
		traceMessageHandler.WriteMessageNoNotification(buffer);
	}
#endif
	return result;
}


// Returns a placement error number (ERROR_FOR_UNIT_CREATION) - 0 is OK for creating unit
AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION GetErrorForUnitCreationAtLocation(AOE_STRUCTURES::STRUCT_PLAYER * player,
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef, float posY, float posX, bool checkVisibility, bool checkHills,
	bool editorMode, bool checkAirModeAndHPBar, bool checkConflictingUnits) {
	long int iCheckVisibility = checkVisibility ? 1 : 0;
	long int iCheckHills = checkHills ? 1 : 0;
	long int iEditorMode = editorMode ? 1 : 0;
	long int iCheckAirModeAndHPBar = checkAirModeAndHPBar ? 1 : 0;
	long int iCheckConflictingUnits = checkConflictingUnits ? 1 : 0;
	AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION res;
	_asm {
		MOV ECX, unitDef;
		PUSH iCheckConflictingUnits;
		PUSH iCheckAirModeAndHPBar;
		PUSH iEditorMode;
		PUSH 1;
		PUSH iCheckHills; // arg6 = (byte) hillMode (for buildings, 0=no check)
		PUSH iCheckVisibility;
		PUSH 0;
		PUSH posX;
		PUSH posY;
		PUSH player;
		MOV EAX, DS:[ECX];
		CALL[EAX + 0x20];
		MOVSX EAX, AL;
		MOV res, EAX;
	}
	return res;
}


// Updates map data to make nearby tiles appear correctly when altitude is not constant (after a manual modification)
// This does not change altitude values. Altitudes will be softened ONLY if consecutive tiles have maximum 1 altitude difference.
// This does NOT soften terrain borders, only altitudes !
void SoftenAltitudeDifferences(AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo,
	long int minPosX, long int minPosY, long int maxPosX, long int maxPosY) {
	if (!mapInfo || !mapInfo->IsCheckSumValid()) {
		return;
	}
	const unsigned long int addr = 0x447E40;
	_asm {
		PUSH 0;
		PUSH 0;
		PUSH 0;
		PUSH maxPosX;
		PUSH maxPosY;
		PUSH minPosX;
		PUSH minPosY;
		MOV ECX, mapInfo;
		CALL addr;
	}
}

// Updates map data to make nearby tiles appear correctly when terrain is not constant (after a manual modification)
// This does NOT soften altitude borders, only terrain !
void SoftenTerrainDifferences(AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo,
	long int minPosX, long int minPosY, long int maxPosX, long int maxPosY) {
	if (!mapInfo || !mapInfo->IsCheckSumValid()) {
		return;
	}
	const unsigned long int updateNearbyTerrainInfo = 0x4487F0;
	const unsigned long int refreshTerrainUnsure = 0x444980;
	minPosX--;
	minPosY--;
	if (minPosX < 0) { minPosX = 0; }
	if (minPosY < 0) { minPosY = 0; }
	maxPosX++;
	maxPosY++;
	if (minPosX >= mapInfo->mapArraySizeX) { minPosX = mapInfo->mapArraySizeX; }
	if (minPosY >= mapInfo->mapArraySizeY) { minPosY = mapInfo->mapArraySizeY; }
	for (int x = minPosX; x <= maxPosX; x++) {
		for (int y = minPosY; y <= maxPosY; y++) {
			_asm {
				PUSH x;
				PUSH y;
				MOV ECX, mapInfo;
				CALL updateNearbyTerrainInfo;
			}
		}
	}
	_asm {
		PUSH 0;
		PUSH maxPosX;
		PUSH maxPosY;
		PUSH minPosX;
		PUSH minPosY;
		MOV ECX, mapInfo;
		CALL refreshTerrainUnsure;
	}
}


// Collects info at mouse position : game position, underlying unit...
// WARNING: does not return all units
bool AOE_GetGameInfoUnderMouse(long int maxInteractionMode, long int mousePosX, long int mousePosY, AOE_STRUCTURES::STRUCT_TEMP_MAP_POSITION_INFO *posInfo) {
	STRUCT_UI_PLAYING_ZONE *gameZone = GetGameZone();
	if ((posInfo == NULL) || (!gameZone || !gameZone->IsCheckSumValid())) { return false; }
	const unsigned long int addr = 0x51A650;
	long int res = 0;
	const long int arg1 = AOE_CONST_INTERNAL::CST_MBA_RELEASE_CLICK;
	_asm {
		PUSH 0;
		PUSH 0;
		PUSH posInfo;
		PUSH mousePosY;
		PUSH mousePosX;
		PUSH maxInteractionMode;
		PUSH arg1;
		MOV ECX, gameZone;
		CALL addr;
		MOV res, EAX;
	}
	return (res > 0); // really not sure of this. Seen 0x33(found pos?), 0x34(found a unit?)... ?
}


// Get unit at (mouse) position, using AOE methods.
// Warning, this impacts the global variables in 0x7D1CF8
AOE_STRUCTURES::STRUCT_UNIT_BASE *GetUnitAtMousePosition(long int mousePosX, long int mousePosY, INTERACTION_MODES maxInteractionMode, bool allowTempUnits) {
	if ((mousePosX < 0) || (mousePosY < 0)) { return NULL; }
	AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *gameZone = GetGameZone();
	if (!gameZone || !gameZone->IsCheckSumValid()) { return NULL; }
	long int i_allowTempUnits = allowTempUnits ? 1 : 0;
	long int result;
	long int i_maxInteraction = (long int)maxInteractionMode;
	_asm {
		MOV ECX, gameZone;
		PUSH i_allowTempUnits;
		PUSH i_maxInteraction; // arg6 = Max interaction mode
		PUSH 0x0F; // ? Maybe related to fog visibility
		PUSH 40; // maxUnitCategory (level ?)
		PUSH 0; // minUnitCategory. 10 would be enough. 20 to disable selecting birds.
		PUSH mousePosY;
		PUSH mousePosX;
		MOV EDX, 0x005145A0;
		CALL EDX;
		MOV result, EAX;
	}
	if (!result) { return NULL; }
	long int *foundUnitId = (long int*)0x7D1CF8;
	if ((*foundUnitId < 0) && (!i_allowTempUnits)) { return NULL; }
	return GetUnitStruct(*foundUnitId);
}


} // AOE_METHODS


// Returns the map visibility mask for given location
// High word: bit mask per player for "explored"
// Low word: bit mask per player for "fog visibility"
// Lower bit is player 0 (gaia), 2nd=player1, etc
AOE_STRUCTURES::STRUCT_MAP_VISIBILITY_INFO *GetMapVisibilityInfo(long int posX, long int posY) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	assert(global && global->IsCheckSumValid());
	if (!global || !global->IsCheckSumValid()) { return NULL; }
	assert(global->gameMapInfo && global->gameMapInfo->IsCheckSumValid());
	if (!global->gameMapInfo || !global->gameMapInfo->IsCheckSumValid()) { return NULL; }
	AOE_STRUCTURES::STRUCT_MAP_VISIBILITY_LINK *visLink = global->gameMapInfo->mapVisibilityLink;
	if (!visLink) { return NULL; }
	assert(posX >= 0);
	assert(posY >= 0);
	assert(posX < visLink->mapSizeX);
	assert(posY < visLink->mapSizeY);
	if ((posX < 0) || (posY < 0) || (posX >= visLink->mapSizeX) || (posY >= visLink->mapSizeY)) { return NULL; }
	return visLink->GetPtrTileVisibility(posX, posY);
}


// Returns true if a position is fog-visible for a player.
// This is quite fast: directly accesses (optimized) memory, no underlying calls.
// Warning: posX/posY values are not controlled regarding map size !
bool IsFogVisibleForPlayer(long int playerId, long int posX, long int posY) {
	assert(playerId >= 0);
	assert(playerId < 9);
	assert(posX >= 0);
	assert(posY >= 0);
	assert(posX < 256); // TO DO: get exact map size
	assert(posY < 256); // TO DO: get exact map size
	if ((posX < 0) || (posY < 0) || (posX > 255) || (posY > 255) || (playerId < 0) || (playerId > 8)) { return false; }
	AOE_STRUCTURES::STRUCT_MAP_VISIBILITY_INFO *v = (AOE_STRUCTURES::STRUCT_MAP_VISIBILITY_INFO*)GetMapVisibilityInfo(posX, posY);
	return v->isFogVisibleForPlayer(playerId);
}

// Returns true if a position has been explored by a player
// This is quite fast: directly accesses (optimized) memory, no underlying calls.
// Warning: posX/posY values are not controlled regarding map size !
bool IsExploredForPlayer(long int playerId, long int posX, long int posY) {
	assert(playerId >= 0);
	assert(playerId < 9);
	assert(posX >= 0);
	assert(posY >= 0);
	assert(posX < 256); // TO DO: get exact map size
	assert(posY < 256); // TO DO: get exact map size
	if ((posX < 0) || (posY < 0) || (posX > 255) || (posY > 255) || (playerId < 0) || (playerId > 8)) { return false; }
	AOE_STRUCTURES::STRUCT_MAP_VISIBILITY_INFO *v = (AOE_STRUCTURES::STRUCT_MAP_VISIBILITY_INFO*)GetMapVisibilityInfo(posX, posY);
	return v->isExploredForPlayer(playerId);
}



// If UIObj is null, use current "global" screen positions.
AOE_STRUCTURES::STRUCT_POSITION_INFO GetMousePosition(AOE_STRUCTURES::STRUCT_ANY_UI *UIObj) {
	AOE_STRUCTURES::STRUCT_POSITION_INFO result;
	result.posX = -1;
	result.posY = -1;
	AOE_STRUCTURES::STRUCT_ANY_UI *currentUI = UIObj;
	if (!currentUI) {
		currentUI = AOE_METHODS::GetCurrentScreen();
	}
	assert(currentUI);
	if (!currentUI) { return result; }
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient((HWND)currentUI->GetHWnd(), &pt); // Returns false if failed
	pt.x += currentUI->unknown_08C_minPosX;
	pt.y += currentUI->unknown_090_minPosY;
	result.posX = pt.x;
	result.posY = pt.y;
	return result;
}

// Get current mouse position, in game zone, if found. Returns (-1, -1) if not found.
AOE_STRUCTURES::STRUCT_POSITION_INFO GetMousePosition() {
	AOE_STRUCTURES::STRUCT_POSITION_INFO result;
	result.posX = -1;
	result.posY = -1;
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_ANY_UI *currentUI = AOE_METHODS::GetCurrentScreen();
	assert(currentUI);
	if (!settings || !currentUI) { return result; }
	AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *gameZone = AOE_METHODS::GetGameZone();
	if (!gameZone || !gameZone->IsCheckSumValid()) {
		return result;
	}
	result = GetMousePosition(gameZone);
	result.posX = result.posX - gameZone->unknown_08C_minPosX;
	result.posY = result.posY - gameZone->unknown_090_minPosY;
	return result;
}

// Get "game" coordinates under mouse position. Returns true if successful. Updates posX/posY.
// If position is not valid, posX/posY are set to -1.
bool GetGamePositionUnderMouse(float *posX, float *posY) {
	*posX = -1;
	*posY = -1;
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	AOE_STRUCTURES::STRUCT_ANY_UI *currentUI = AOE_METHODS::GetCurrentScreen();
	assert(currentUI);
	if (!settings || !currentUI) { return false; }
	AOE_STRUCTURES::STRUCT_UI_PLAYING_ZONE *gameZone = AOE_METHODS::GetGameZone();
	if (!gameZone || !gameZone->IsCheckSumValid()) { return false; }
	AOE_STRUCTURES::STRUCT_POSITION_INFO mousePos = GetMousePosition(gameZone);

	// Analog to 0x5145D1
	if (mousePos.posX < gameZone->unknown_08C_minPosX) { return false; }
	if (mousePos.posX > gameZone->unknown_094_maxPosX) { return false; }
	if (mousePos.posY < gameZone->unknown_090_minPosY) { return false; }
	if (mousePos.posY > gameZone->unknown_098_maxPosY) { return false; }
	long int relativeMousePosX = mousePos.posX - gameZone->unknown_08C_minPosX;
	long int relativeMousePosY = mousePos.posY - gameZone->unknown_090_minPosY;

	AOE_STRUCTURES::STRUCT_TEMP_MAP_POSITION_INFO gamePos;
	long int unknown_res = AOE_METHODS::AOE_GetGamePosFromMousePos(gameZone, &gamePos, relativeMousePosX, relativeMousePosY);
	if (unknown_res > 0) { // really not sure of this. Often 0x33 ?
		*posX = gamePos.posX;
		*posY = gamePos.posY;
	}

	return (unknown_res > 0);
}

// Returns "game" coordinates under mouse position (rounded to int).
// If position is not valid, posX/posY are set to -1.
AOE_STRUCTURES::STRUCT_POSITION_INFO GetGameMousePositionInfo() {
	AOE_STRUCTURES::STRUCT_POSITION_INFO result;
	float x, y;
	GetGamePositionUnderMouse(&x, &y);
	result.posX = (long int)x;
	result.posY = (long int)y;
	return result;
}


// Refresh and smoothen terrain+altitude in given zone. If all position arguments are -1, all map is treated.
void RefreshTerrainAfterManualChange(AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo,
	long int minPosX, long int minPosY, long int maxPosX, long int maxPosY) {
	if (!mapInfo || !mapInfo->IsCheckSumValid()) {
		return;
	}
	if ((minPosX == -1) && (minPosY == -1) && (maxPosX == -1) && (maxPosY == -1)) {
		minPosX = 0;
		minPosY = 0;
		maxPosX = mapInfo->mapArraySizeX - 1;
		maxPosY = mapInfo->mapArraySizeY - 1;
	}
	AOE_METHODS::SoftenAltitudeDifferences(mapInfo, minPosX, minPosY, maxPosX, maxPosY);
	AOE_METHODS::SoftenTerrainDifferences(mapInfo, minPosX, minPosY, maxPosX, maxPosY);
}
