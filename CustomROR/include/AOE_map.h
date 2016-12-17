
#pragma once

#include <Windows.h> // POINT, GetCursorPos...
#include <assert.h>
#include <string>
#include <set>
#include <mystrings.h>
#include <AOE_offsets.h>
#include <AOE_const_functional.h>
#include <AOE_struct_game_settings.h>
#include "mainStructuresHandling.h"
#include "UI_utilities.h"
#include "traceMessage.h"

/* This file contains various primitives about map handling */


using namespace AOE_STRUCTURES;
using namespace AOE_CONST_FUNC;


extern AOE_STRUCTURES::STRUCT_UNIT_BASE *GetUnitStruct(long int unitId);


namespace AOE_METHODS {
	// pathFindingStruct can be 0x583BC8 or 0x6A1CC0. Don't know the specific roles yet :(
	// updateUnitPathInfo = "do move". If false, this just checks if movement is possible.
	long int CalcPathForMove(STRUCT_UNKNOWN_MAP_DATA_F04C *pathFindingStruct,
		long int srcPosY, long int srcPosX, long int destPosY, long int destPosX,
		AOE_STRUCTURES::STRUCT_UNIT_BASE *ptrActorUnit, float maxRange, long int targetUnitId, long int updateUnitPathInfo,
		long int arg9, long int arg10, long int arg11, long int arg12,
		long int distance_unsure, long int targetPlayerId, long int unknown_unitClass);


	// Calls AOE's path finding method, using 0x583BC8 (not 0x6A1CC0)
	// allArgs indices are 1-15 (do NOT use 0). Warning, allArgs[6] is a float, not an int.
	// Arguments (1-15) are:
	// srcPosY, srcPosX, destPosY, destPosX, ptrActorUnit, f_range?, targetUnitId, updateUnitPathInfo?(!OnlyCheck),
	// arg9, arg10, arg11, arg12, arg13(int_distance?), arg14(unitGrp?), arg15
	// See also AOE_calcPathForMove
	long int CallFindPathForUnit(long int allArgs[15 + 1]);


	// Returns a placement error number (ERROR_FOR_UNIT_CREATION) - 0 is OK for creating unit
	AOE_CONST_INTERNAL::ERROR_FOR_UNIT_CREATION GetErrorForUnitCreationAtLocation(AOE_STRUCTURES::STRUCT_PLAYER * player,
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *unitDef, float posY, float posX, bool checkVisibility, bool hillMode,
		bool editorMode, bool checkAirModeAndHPBar, bool checkConflictingUnits);


	// Updates map data to make nearby tiles appear correctly when altitude is not constant (after a manual modification)
	// This does not change altitude values. Altitudes will be softened ONLY if consecutive tiles have maximum 1 altitude difference.
	// This does NOT soften terrain borders, only altitudes !
	void SoftenAltitudeDifferences(AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo,
		long int minPosX, long int minPosY, long int maxPosX, long int maxPosY);

	// Updates map data to make nearby tiles appear correctly when terrain is not constant (after a manual modification)
	// This does NOT soften altitude borders, only terrain !
	void SoftenTerrainDifferences(AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo,
		long int minPosX, long int minPosY, long int maxPosX, long int maxPosY);


	// Collects info at mouse position : game position, underlying unit...
	bool GetGameInfoUnderMouse(long int maxInteractionMode, long int mousePosX, long int mousePosY, AOE_STRUCTURES::STRUCT_TEMP_MAP_POSITION_INFO *posInfo);


	// Get unit at (mouse) position, using AOE methods.
	// Warning, this impacts the global variables in 0x7D1CF8
	AOE_STRUCTURES::STRUCT_UNIT_BASE *GetUnitAtMousePosition(long int mousePosX, long int mousePosY, INTERACTION_MODES maxInteractionMode, bool allowTempUnits);

}


// Returns the map visibility mask for given location - old method
// High word: bit mask per player for "explored"
// Low word: bit mask per player for "fog visibility"
// Lower bit is player 0 (gaia), 2nd=player1, etc
AOE_STRUCTURES::STRUCT_MAP_VISIBILITY_INFO *GetMapVisibilityInfo(long int xPos, long int yPos);


// Returns true if a position is fog-visible for a player.
// This is quite fast: directly accesses (optimized) memory, no underlying calls.
// Warning: posX/posY values are not controlled regarding map size !
// Does not support shared exploration !
bool IsFogVisibleForPlayerWithoutSharedExploration(long int playerId, long int posX, long int posY);

// Returns true if a position has been explored by a player
// This is quite fast: directly accesses (optimized) memory, no underlying calls.
// Warning: posX/posY values are not controlled regarding map size !
// Does not support shared exploration !
bool IsExploredForPlayerWithoutSharedExploration(long int playerId, long int posX, long int posY);


// Returns true if a position is fog-visible for a player.
// This is quite fast: directly accesses (optimized) memory, no underlying calls.
// Warning: posX/posY values are not controlled regarding map size !
bool IsFogVisibleForMask(STRUCT_MAP_VISIBILITY_MASK fogVisibilityMask, long int posX, long int posY);

// Returns true if a position has been explored by a player
// This is quite fast: directly accesses (optimized) memory, no underlying calls.
// Warning: posX/posY values are not controlled regarding map size !
bool IsExploredForMask(STRUCT_MAP_VISIBILITY_MASK explorationVisibilityMask, long int posX, long int posY);


// Get current *mouse* position, relative to UIObj
// If UIObj is null, use current "global" screen relative positions.
AOE_STRUCTURES::STRUCT_POSITION_INFO GetMousePosition(AOE_STRUCTURES::STRUCT_ANY_UI *UIObj);

// Get current *mouse* position, in game zone, if found. Returns (-1, -1) if not found.
AOE_STRUCTURES::STRUCT_POSITION_INFO GetMousePosition();

// Get "game" coordinates under mouse position. Returns true if successful. Updates posX/posY.
// If position is not valid, posX/posY are set to -1.
bool GetGamePositionUnderMouse(float *posX, float *posY);

// Returns "game" coordinates under mouse position (rounded to int).
// If position is not valid, posX/posY are set to -1.
AOE_STRUCTURES::STRUCT_POSITION_INFO GetGameMousePositionInfo();


// Refresh and smoothen terrain+altitude in given zone. If all position arguments are -1, all map is treated.
void RefreshTerrainAfterManualChange(AOE_STRUCTURES::STRUCT_GAME_MAP_INFO *mapInfo,
	long int minPosX, long int minPosY, long int maxPosX, long int maxPosY);

