
#pragma once

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


	// Size = 8. Mostly used in temporary treatments
	class STRUCT_POSITION_INFO {
	public:
		long int posY;
		long int posX;
	};
	static_assert(sizeof(STRUCT_POSITION_INFO) == 8, "STRUCT_POSITION_INFO size");


	// Size=8. Used in temporary treatments, for example auto-attack unit when idle...
	class STRUCT_NEARBY_UNIT_INFO {
	public:
		long int unitId; // +0
		char distance; // +4. Unsure. Not always valued ?
		char playerId; // +5
		char posY; // +6
		char posX; // +7
	};
	static_assert(sizeof(STRUCT_NEARBY_UNIT_INFO) == 0x8, "STRUCT_NEARBY_UNIT_INFO size");


	// Used in path finding algorithm (included in path finding struct)
	class STRUCT_PATH_FINDING_UNKNOWN_POS_INFO {
	public:
		char posY;
		char posX;
		char unknown_2; // Maybe unused
		char unknown_3; // Maybe unused
		long int distance;
	};


	// Size 0x14 ?
	// This struct is used internally to get position info (when clicking, for example)
	// Often it is in stack, (SUB ESP, ...), not allocated
	class STRUCT_TEMP_MAP_POSITION_INFO {
	public:
		float posY;
		float posX;
		unsigned short int unknown_08;
		unsigned short int unknown_0A;
		STRUCT_UNIT_BASE *unit;
		unsigned long int unknown_10;
	};

}
