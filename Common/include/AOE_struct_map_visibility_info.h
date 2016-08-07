
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
		bool isFogVisibleForPlayer(long int playerId) {
			long int mask = 1 << playerId;
			return (this->fogVisibilityMask & mask) != 0;
		}
		bool isExploredForPlayer(long int playerId) {
			long int mask = 1 << playerId;
			return (this->explorationVisibilityMask & mask) != 0;
		}
	};
	static_assert(sizeof(STRUCT_MAP_VISIBILITY_INFO) == 4, "STRUCT_MAP_VISIBILITY_INFO size");

}
