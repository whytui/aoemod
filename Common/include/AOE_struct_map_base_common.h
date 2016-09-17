
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

	// Size = 0x0C. Created in 0x520F20
	class STRUCT_CHAINED_POS_INFO {
	public:
		long int posY;
		long int posX;
		STRUCT_CHAINED_POS_INFO *next; // can be NULL
	};

	// Size = 0x10
	class STRUCT_PATH_FINDING_INTERMEDIATE_STEP {
	public:
		float posY;
		float posX;
		float posZ; // Unsure
		char remainingSteps; // -1=initial position, 0 for final position, 1 for last before final position...
	};
	static_assert(sizeof(STRUCT_PATH_FINDING_INTERMEDIATE_STEP) == 0x10, "STRUCT_PATH_FINDING_INTERMEDIATE_STEP size");

	// Size = 0x3 ! (yes !)
	class STRUCT_UNIT_TARGET_POS {
	public:
		char posY;
		char posX;
		char posZ;
	};
	static_assert(sizeof(STRUCT_UNIT_TARGET_POS) == 3, "STRUCT_UNIT_TARGET_POS size");


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
