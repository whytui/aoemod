
#pragma once

#include <AOE_empires_dat.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// External dependencies
	class STRUCT_UNIT_BASE;



#pragma pack(push, 1) // Prevent compiler from aligning on dwords (or other alignment)
	// Size = 1 byte. This is a shortcut to avoid duplicating bit operations everywhere
	class TERRAIN_BYTE {
	public:
		char terrainData;
		char GetTerrainId() const {
			return this->terrainData & 0x1F;
		}
		bool SetTerrainId(char value) { // To properly update terrain, there are more complex treatments to do than this
			if ((value < 0) || (value > 31)) { return false; }
			this->terrainData = (this->terrainData & 0xE0) | value; // preserve altitude
			return true;
		}
		char GetAltitude() const {
			return (this->terrainData & 0xE0) >> 5;
		}
		bool SetAltitude(char value) { // To update altitude, there are more complex treatments to do
			if ((value < 0) || (value > 7)) { return false; }
			this->terrainData = (this->terrainData & 0x1F) | (value << 5); // preserve terrainId
			return true;
		}
	};
	static_assert(sizeof(TERRAIN_BYTE) == 1, "TERRAIN_BYTE size");

	// Size = 1 byte. This is a shortcut to avoid duplicating bit operations everywhere
	// 4 bits (high) for shapeIndex. Seems to be bit-wise in some way ? Not sure it corresponds to actual shape index !
	// 4 bits (low) for border ID.
	class TERRAIN_BORDER_BYTE {
	public:
		char borderData; // Do not use directly.
		AOE_CONST_FUNC::GROUND_BORDERS GetBorderId() const {
			return (AOE_CONST_FUNC::GROUND_BORDERS)(this->borderData & 0x0F); // 4 low bits = border id
		}
		// Shape indexes can be 0-15, 0=none. They depend on borders.drs SLP shapes.
		char GetShapeIndex() const {
			return (this->borderData & 0xF0) >> 4; // 4 high bits = shape index
		}
		// Set border Id. Value must be 0-15.
		bool SetBorderId(char value) {
			if ((value < 0) || (value >= 16)) { return false; }
			this->borderData = (this->borderData & 0xF0) | value; // preserve shape index
			return true;
		}
		// Set shape index. Value must be 0-15. Not sure of the exact role of this value.
		bool SetShapeIndex(char value) {
			if ((value < 0) || (value >= 16)) { return false; }
			this->borderData = (this->borderData & 0x0F) | (value << 4); // preserve border Id
			return true;
		}
	};
	static_assert(sizeof(TERRAIN_BORDER_BYTE) == 1, "TERRAIN_BORDER_BYTE size");
#pragma pack(pop)

	// Size=8. Represents a unit list element
	class STRUCT_UNIT_BASE_LIST {
	public:
		STRUCT_UNIT_BASE *unit;
		STRUCT_UNIT_BASE_LIST *next; // NULL=end of list
	};
	static_assert(sizeof(STRUCT_UNIT_BASE_LIST) == 8, "STRUCT_UNIT_BASE_LIST size");


	// Size = 0x18. The main "actual" map info necessary to set correct terrain/altitude and transitions are terrainData, terrainBorderData and elevationGraphicsIndex
	class STRUCT_GAME_MAP_TILE_INFO {
	public:
		short int displayX; // +00. A screen X position where to display tile (independent from player's screen position). Units on this tile will be displayed accordingly.
		short int displayY; // +02. A screen Y position where to display tile (independent from player's screen position). Units on this tile will be displayed accordingly. Changing this value can generate fake elevation (display only).
		char elevationGraphicsIndex; // +4. To retrieve elevation graphics in terrain's table - 0=flat. HAS an effect on units altitude ! Please call mapInfo->RecomputeTileDisplayPos(...) after modifying this
		TERRAIN_BYTE terrainData; // +5: elevation/terrainID. 3 high bits (0x80/0x40/0x20) represent altitude 0-7. 5 low bits represent terrainId 0-31
		TERRAIN_BORDER_BYTE terrainBorderData; // +6. Border id & shape index (for terrain transitions)
		char unknown_07; // Updated in 444980 to 0xCC, but reset to 0x0F very frequently (quite instantly). Some status ? Depends on current player perspective.
		char unknown_08; // Some status ? Depends on current player perspective. Reset value=-1.
		char unknown_09; // Set =0x0F in 0x5179DE. Some status ? Depends on current player perspective.
		char tileHighlightLevel; // +0A. Tile brillance level (for tile selection in editor). Values in 0-0xB0. Default=0, 0x0F=underMouse(editor). Updated in 444980 with arg5 ?
		char unknown_0B; // +0B. Some status ? Depends on current player perspective.
		// A sub-structure starts here ?
		char unknown_0C;
		char unknown_0D;
		char unknown_0E;
		char unknown_0F;
		// +10. Array of units occupying this tile. The list ends with a NULL pointer. NULL if no unit on this tile.
		// Note: in the list, we find dopplegangers which correspond to enemy buildings under fog (represents the last explored view, maybe the real unit no longer exists)
		STRUCT_UNIT_BASE_LIST *unitsOnThisTile;
		short int unitsOnThisTileCount; // +14. Number of units in unitsOnThisTile array. unitsOnThisTile=NULL if unitsOnThisTileCount==0.
		short int unknown_16; // possibly unused ?
	};
	static_assert(sizeof(STRUCT_GAME_MAP_TILE_INFO) == 0x18, "STRUCT_GAME_MAP_TILE_INFO size");
}
