
#pragma once

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// Size=4 (1 dword)
	class STRUCT_MAP_VISIBILITY_INFO {
	public:
		short int fogVisibilityMask;
		short int explorationVisibilityMask;
		// Returns true if a position is fog-visible to a player. This call is fast (bit-and operation, no underlying call)
		bool isFogVisibleForPlayer(long int playerId) {
			long int mask = 1 << playerId;
			return (this->fogVisibilityMask & mask) != 0;
		}
		// Returns true if a position has been explored by a player. This call is fast (bit-and operation, no underlying call)
		bool isExploredForPlayer(long int playerId) {
			long int mask = 1 << playerId;
			return (this->explorationVisibilityMask & mask) != 0;
		}
	};
	static_assert(sizeof(STRUCT_MAP_VISIBILITY_INFO) == 4, "STRUCT_MAP_VISIBILITY_INFO size");


	// Size 0x0C. Constructor = 0x517E30
	class STRUCT_MAP_VISIBILITY_LINK {
	public:
		long int mapSizeY;
		long int mapSizeX;
		// +08. Array size (number of dwords) = sizeX*sizeY (*4 to get bytes count)
		// Each DWORD represents a mask of fog visibility+exploration for all players
		// The 0x7D205C variable points to this array too.
		STRUCT_MAP_VISIBILITY_INFO *tilesVisibilityArray;

		// Returns a pointer to tile visibility info at specified position
		// Returns NULL if position is invalid
		STRUCT_MAP_VISIBILITY_INFO *GetPtrTileVisibility(long int posX, long int posY) const {
			assert(posX >= 0);
			assert(posY >= 0);
			assert(posX < this->mapSizeX);
			assert(posY < this->mapSizeY);
			if ((posX < 0) || (posY < 0) || (posX >= this->mapSizeX) || (posY >= this->mapSizeY)) { return NULL; }
			long int dwordOffset = (posX * this->mapSizeX + posY);
			return &this->tilesVisibilityArray[dwordOffset];
		}
	};
	static_assert(sizeof(STRUCT_MAP_VISIBILITY_LINK) == 0x0C, "STRUCT_PLAYER_FOG_VISIBILITY size");

}
