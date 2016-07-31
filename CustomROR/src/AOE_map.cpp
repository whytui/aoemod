#include "../include/AOE_map.h"



// Returns the map visibility mask for given location
// High word: bit mask per player for "explored"
// Low word: bit mask per player for "fog visibility"
// Lower bit is player 0 (gaia), 2nd=player1, etc
ROR_STRUCTURES_10C::STRUCT_MAP_VISIBILITY_INFO *GetMapVisibilityInfo(long int posX, long int posY) {
	unsigned long int **pMapInfo = (unsigned long int **)AOE_OFFSETS::ADDR_MAP_VISIBILITY_INFO;
	assert(pMapInfo != NULL);
	assert(posX >= 0);
	assert(posY >= 0);
	assert(posX < 256); // TO DO: get exact map size
	assert(posY < 256); // TO DO: get exact map size
	if ((pMapInfo == NULL) || (posX < 0) || (posY < 0) || (posX >= 256) || (posY >= 256)) { return 0; }
	unsigned long int *pMapInfoX = pMapInfo[posX];
	assert(pMapInfoX != NULL);
	if (pMapInfoX == NULL) { return 0; }
	ROR_STRUCTURES_10C::STRUCT_MAP_VISIBILITY_INFO *result = (ROR_STRUCTURES_10C::STRUCT_MAP_VISIBILITY_INFO*) &pMapInfoX[posY];
	return result;
}


bool IsFogVisibleForPlayer(long int playerId, long int posX, long int posY) {
	assert(playerId >= 0);
	assert(playerId < 9);
	assert(posX >= 0);
	assert(posY >= 0);
	assert(posX < 256); // TO DO: get exact map size
	assert(posY < 256); // TO DO: get exact map size
	if ((posX < 0) || (posY < 0) || (posX > 255) || (posY > 255) || (playerId < 0) || (playerId > 8)) { return false; }
	ROR_STRUCTURES_10C::STRUCT_MAP_VISIBILITY_INFO *v = (ROR_STRUCTURES_10C::STRUCT_MAP_VISIBILITY_INFO*)GetMapVisibilityInfo(posX, posY);
	return v->isFogVisibleForPlayer(playerId);
}

bool IsExploredForPlayer(long int playerId, long int posX, long int posY) {
	assert(playerId >= 0);
	assert(playerId < 9);
	assert(posX >= 0);
	assert(posY >= 0);
	assert(posX < 256); // TO DO: get exact map size
	assert(posY < 256); // TO DO: get exact map size
	if ((posX < 0) || (posY < 0) || (posX > 255) || (posY > 255) || (playerId < 0) || (playerId > 8)) { return false; }
	ROR_STRUCTURES_10C::STRUCT_MAP_VISIBILITY_INFO *v = (ROR_STRUCTURES_10C::STRUCT_MAP_VISIBILITY_INFO*)GetMapVisibilityInfo(posX, posY);
	return v->isExploredForPlayer(playerId);
}


// Returns a placement error number (ERROR_FOR_UNIT_CREATION) - 0 is OK for creating unit
AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION GetErrorForUnitCreationAtLocation(ROR_STRUCTURES_10C::STRUCT_PLAYER * player,
	ROR_STRUCTURES_10C::STRUCT_DEF_UNIT *unitDef, float posY, float posX, bool checkVisibility, bool checkHills,
	bool editorMode, bool checkAirModeAndHPBar, bool checkConflictingUnits) {
	long int iCheckVisibility = checkVisibility ? 1 : 0;
	long int iCheckHills = checkHills ? 1 : 0;
	long int iEditorMode = editorMode ? 1 : 0;
	long int iCheckAirModeAndHPBar = checkAirModeAndHPBar ? 1 : 0;
	long int iCheckConflictingUnits = checkConflictingUnits ? 1 : 0;
	AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION res;
	_asm {
		MOV ECX, unitDef
			PUSH iCheckConflictingUnits
			PUSH iCheckAirModeAndHPBar
			PUSH iEditorMode
			PUSH 1
			PUSH iCheckHills // arg6 = (byte) hillMode (for buildings, 0=no check)
			PUSH iCheckVisibility
			PUSH 0
			PUSH posX
			PUSH posY
			PUSH player
			MOV EAX, DS:[ECX]
			CALL[EAX + 0x20]
			MOVSX EAX, AL
			MOV res, EAX
	}
	return res;
}


// If UIObj is null, use current "global" screen positions.
ROR_STRUCTURES_10C::STRUCT_POSITION_INFO GetMousePosition(ROR_STRUCTURES_10C::STRUCT_ANY_UI *UIObj) {
	ROR_STRUCTURES_10C::STRUCT_POSITION_INFO result;
	result.posX = -1;
	result.posY = -1;
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *currentUI = UIObj;
	if (!currentUI) {
		currentUI = AOE_GetCurrentScreen();
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


// Get "game" coordinates under mouse position. Returns true if successful. Updates posX/posY.
// If position is not valid, posX/posY are set to -1.
bool GetGamePositionUnderMouse(float *posX, float *posY) {
	*posX = -1;
	*posY = -1;
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	assert(settings && settings->IsCheckSumValid());
	ROR_STRUCTURES_10C::STRUCT_ANY_UI *currentUI = AOE_GetCurrentScreen();
	assert(currentUI);
	if (!settings || !currentUI) { return false; }
	ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *gameMainUI = (ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *)currentUI;
	ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *scEditorMainUI = (ROR_STRUCTURES_10C::STRUCT_UI_SCENARIO_EDITOR_MAIN *)currentUI;
	ROR_STRUCTURES_10C::STRUCT_UI_PLAYING_ZONE *gameZone = NULL;
	if (gameMainUI->IsCheckSumValid()) {
		assert((settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) ||
			(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_GAME_OVER_BUT_STILL_IN_GAME));
		gameZone = gameMainUI->gamePlayUIZone;
	}
	if (scEditorMainUI->IsCheckSumValid()) {
		assert(gameZone == NULL);
		assert(settings->currentUIStatus == AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR);
		gameZone = scEditorMainUI->gamePlayUIZone;
	}
	if (!gameZone || !gameZone->IsCheckSumValid()) { return false; }
	if (!settings) { return false; }
	if ((settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) &&
		(settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) &&
		(settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_GAME_OVER_BUT_STILL_IN_GAME)) {
		return false;
	}
	ROR_STRUCTURES_10C::STRUCT_POSITION_INFO mousePos = GetMousePosition(gameZone);

	// Analog to 0x5145D1
	if (mousePos.posX < gameZone->unknown_08C_minPosX) { return false; }
	if (mousePos.posX > gameZone->unknown_094_maxPosX) { return false; }
	if (mousePos.posY < gameZone->unknown_090_minPosY) { return false; }
	if (mousePos.posY > gameZone->unknown_098_maxPosY) { return false; }
	long int relativeMousePosX = mousePos.posX - gameZone->unknown_08C_minPosX;
	long int relativeMousePosY = mousePos.posY - gameZone->unknown_090_minPosY;

	ROR_STRUCTURES_10C::STRUCT_TEMP_MAP_POSITION_INFO gamePos;
	long int unknown_res = AOE_GetGamePosFromMousePos(gameZone, &gamePos, relativeMousePosX, relativeMousePosY);
	if (unknown_res > 0) { // really not sure of this. Often 0x33 ?
		*posX = gamePos.posX;
		*posY = gamePos.posY;
	}

	return (unknown_res > 0);
}

// Returns "game" coordinates under mouse position (rounded to int).
// If position is not valid, posX/posY are set to -1.
ROR_STRUCTURES_10C::STRUCT_POSITION_INFO GetGameMousePositionInfo() {
	ROR_STRUCTURES_10C::STRUCT_POSITION_INFO result;
	float x, y;
	GetGamePositionUnderMouse(&x, &y);
	result.posX = (long int)x;
	result.posY = (long int)y;
	return result;
}


// Get unit at (mouse) position, using AOE methods.
// Warning, this impacts the global variables in 0x7D1CF8
ROR_STRUCTURES_10C::STRUCT_UNIT *GetUnitAtMousePosition(long int mousePosX, long int mousePosY, bool allowTempUnits) {
	if ((mousePosX < 0) || (mousePosY < 0)) { return NULL; }
	ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return NULL; }
	// Only allowed in game or editor
	if ((settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_IN_EDITOR) &&
		(settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) &&
		(settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_GAME_OVER_BUT_STILL_IN_GAME)
		) {
		return NULL;
	}
	ROR_STRUCTURES_10C::STRUCT_UI_IN_GAME_MAIN *gameMainUI = settings->ptrGameUIStruct;
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) { return NULL; }

	ROR_STRUCTURES_10C::STRUCT_UI_PLAYING_ZONE *gameZone = gameMainUI->gamePlayUIZone;
	if (!gameZone || !gameZone->IsCheckSumValid()) { return NULL; }
	long int i_allowTempUnits = allowTempUnits ? 1 : 0;
	long int result;
	_asm {
		MOV ECX, gameZone
			PUSH i_allowTempUnits
			PUSH 4 // Max interaction mode
			PUSH 0x0F // ?
			PUSH 40 // maxUnitCategory
			PUSH 0 // minUnitCategory. 10 would be enough. 20 to disable selecting birds.
			PUSH mousePosY
			PUSH mousePosX
			MOV EDX, 0x005145A0
			CALL EDX
			MOV result, EAX
	}
	if (!result) { return NULL; }
	long int *foundUnitId = (long int*)0x7D1CF8;
	if ((*foundUnitId < 0) && (!i_allowTempUnits)) { return NULL; }
	return GetUnitStruct(*foundUnitId);
}


// Updates map data to make nearby tiles appear correctly when altitude is not constant (after a manual modification)
// This does not change altitude values. Altitudes will be softened ONLY if consecutive tiles have maximum 1 altitude difference.
// This does NOT soften terrain borders, only altitudes !
void AOE_SoftenAltitudeDifferences(ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *mapInfo,
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
void AOE_SoftenTerrainDifferences(ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *mapInfo,
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

// Refresh and smoothen terrain+altitude in given zone. If all position arguments are -1, all map is treated.
void RefreshTerrainAfterManualChange(ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *mapInfo,
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
	AOE_SoftenAltitudeDifferences(mapInfo, minPosX, minPosY, maxPosX, maxPosY);
	AOE_SoftenTerrainDifferences(mapInfo, minPosX, minPosY, maxPosX, maxPosY);
}
