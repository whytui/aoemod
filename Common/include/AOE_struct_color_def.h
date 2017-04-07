
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

	// Size=0x128. Constructor=0x41CF00 = RGE_ColorTable.constructor(internalFileId)
	// "RGE_Color_Table" (bad naming, this is a table *element* ?)
	// Used in global struct, and also in UI structs
#define CHECKSUM_COLOR_DEF 0x0054320C
	class STRUCT_COLOR_DEF {
	public:
		unsigned long int checksum;
		char colorName[0x1E]; // (30 char)
		short int unusedDrsResource; // +22. Unsure ? Always 0.
		char unknown_type; // +24. 1 in most cases, 2 for "shadow" ?
		char paletteColorIndex; // +25. Index in palette for this color.
		short int colorId; // +26. color ID ? Index in array ?
		char unknown_28[0x100]; // +28. This is really an array of 0x100 bytes (palette or such thing ?). Used by "shadow" color (and paletteColorIndex=0).

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_COLOR_DEF); }
	};
	static_assert(sizeof(STRUCT_COLOR_DEF) == 0x128, "STRUCT_COLOR_DEF size");

}

