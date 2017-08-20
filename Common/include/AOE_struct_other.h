
#pragma once

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size = 0xB0. Added (created) in 4FC5A0.
	// Represents population info at one precise given moment of the game. (used to draw timeline graphic)
	// Referenced by timelineUI+518 (last elem?)
	class STRUCT_TIMELINE_COLUMN_INFO {
	public:
		long int unknown_00;
		long int unknown_04;
		long int unknown_08;
		long int unknown_0C;
		// 0x10
		char unknown_10;
		char unknown_11;
		char unknown_12; // default = 3 ?
		char unknown_13;
		long int totalPopulationCount;
		long int unknown_18;
		long int unknown_1C;
		long int unknown_20;
		long int unknown_24;
		long int unknown_28;
		long int unknown_2C;
		long int unknown_30;
		long int unknown_34;
		long int unknown_38;
		char totalPopulationString[0x70 - 0x3C]; // like "24"
		// 0x70
		long int unknown_70;
		char timeString[0xA8 - 0x74]; // +74. Like "0:00"
		long int unknown_A8; // timeIndex ?
		STRUCT_TIMELINE_COLUMN_INFO *previous;
	};
	static_assert(sizeof(STRUCT_TIMELINE_COLUMN_INFO) == 0xB0, "STRUCT_TIMELINE_COLUMN_INFO size");


	// Constructor=0x45AF10, 0x45AF60. Size=0x20. Used for diamond map (temporarily?)?
	class STRUCT_PICTURE {
	public:
		unsigned long int unknown_00;
		unsigned long int unknown_04;
		unsigned long int unknown_08;
		unsigned long int unknown_0C;
		unsigned long int unknown_10;
		long int alignedWidth; // +14
		unsigned long int unknown_18;
		unsigned long int unknown_1C;
	};
	static_assert(sizeof(STRUCT_PICTURE) == 0x20, "STRUCT_PICTURE size");

}

