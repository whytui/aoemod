
#pragma once
#include <AOE_struct_map_base_common.h>


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {



	// Size = 0x34. Name="Path". Constructor=0x457FE0. (free=0x4580A0)
	// An interesting method: 0x4581B0=moveInfo.replaceByNewDestination(STRUCT_POSITION_INFO*)
	// About movement/path finding
	// 0x458350=FLOAT Path::length()
	class STRUCT_PATH {
	public:
		unsigned long int timestamp; // +00
		STRUCT_WAYPOINT startPosition; // +04
		STRUCT_WAYPOINT endPosition; // +14.
		STRUCT_WAYPOINT *intermediatePositions; // +24. "Waypoints".
		long int intermediatePositionsArraySize; // +28. "maxNumberOfWaypoints"
		long int intermediatePositionsUsedElementsCount; // +2C
		long int intermediatePositionsCurrentIndex; // +30. See 0x4583D0.
	};
	static_assert(sizeof(STRUCT_PATH) == 0x34, "STRUCT_PATH size");


}
