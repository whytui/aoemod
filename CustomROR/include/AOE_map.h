
#pragma once

#include <Windows.h> // POINT, GetCursorPos...
#include <assert.h>
#include <string>
#include <set>
#include <AOE_offsets.h>
#include <ROR_structures.h>
#include "UI_utilities.h"
#include "traceMessage.h"

/* This file contains various primitives about map handling */


using namespace ROR_STRUCTURES_10C;


// TODO: create an include file for such methods AOE_mainStructs or something like that
// Duplicated from crCommon (dirty !!)
extern ROR_STRUCTURES_10C::STRUCT_GAME_SETTINGS* GetGameSettingsPtr();
extern ROR_STRUCTURES_10C::STRUCT_GAME_GLOBAL* GetGameGlobalStructPtr();
extern ROR_STRUCTURES_10C::STRUCT_UNIT *GetUnitStruct(long int unitId);




// Returns the map visibility mask for given location - old method
// High word: bit mask per player for "explored"
// Low word: bit mask per player for "fog visibility"
// Lower bit is player 0 (gaia), 2nd=player1, etc
ROR_STRUCTURES_10C::STRUCT_MAP_VISIBILITY_INFO *GetMapVisibilityInfo(long int xPos, long int yPos);


bool IsFogVisibleForPlayer(long int playerId, long int posX, long int posY);
bool IsExploredForPlayer(long int playerId, long int posX, long int posY);


// Get current mouse position, relative to UIObj
// If UIObj is null, use current "global" screen relative positions.
ROR_STRUCTURES_10C::STRUCT_POSITION_INFO GetMousePosition(ROR_STRUCTURES_10C::STRUCT_ANY_UI *UIObj);

// Get "game" coordinates under mouse position. Returns true if successful. Updates posX/posY.
// If position is not valid, posX/posY are set to -1.
bool GetGamePositionUnderMouse(float *posX, float *posY);

// Returns "game" coordinates under mouse position (rounded to int).
// If position is not valid, posX/posY are set to -1.
ROR_STRUCTURES_10C::STRUCT_POSITION_INFO GetGameMousePositionInfo();

// Get unit at (mouse) position, using AOE methods.
// Warning, this impacts the global variables in 0x7D1CF8
ROR_STRUCTURES_10C::STRUCT_UNIT *GetUnitAtMousePosition(long int mousePosX, long int mousePosY, bool allowTempUnits);


// Updates map data to make nearby tiles appear correctly when altitude is not constant (after a manual modification)
// This does not change altitude values. Altitudes will be softened ONLY if consecutive tiles have maximum 1 altitude difference.
// This does NOT soften terrain borders, only altitudes !
void AOE_SoftenAltitudeDifferences(ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *mapInfo,
	long int minPosX, long int minPosY, long int maxPosX, long int maxPosY);

// Updates map data to make nearby tiles appear correctly when terrain is not constant (after a manual modification)
// This does NOT soften altitude borders, only terrain !
void AOE_SoftenTerrainDifferences(ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *mapInfo,
	long int minPosX, long int minPosY, long int maxPosX, long int maxPosY);

// Refresh and smoothen terrain+altitude in given zone. If all position arguments are -1, all map is treated.
void RefreshTerrainAfterManualChange(ROR_STRUCTURES_10C::STRUCT_GAME_MAP_INFO *mapInfo,
	long int minPosX, long int minPosY, long int maxPosX, long int maxPosY);

