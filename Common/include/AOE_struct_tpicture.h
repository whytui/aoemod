
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

	// Size=0x20.
	// Constructor = 0x45AF10 (FileName, res_id, UseTrans, MapToPal, ZeroIsTrans)
	// Constructor = 0x45AF60 (infile, UseTrans)
	// 0x45B130 = TPicture.save(internalFileId)
	// 0x45B1B0 = TPicture.load(FileName,res_id,UseTrans,MapToPal,ZeroIsTrans)
	// Info about BMP contained:
	// - First 0x0E bytes are missing (starts at BMP file's offset 0xE)
	// - Header is only 0x28 bytes, corresponds to after bitsPerPixel/before compression (offset 0x1E in real BMP file)
	// - hardcoded 0x428 corresponds to 0x28 bytes of info + 0x400 for palette
	class STRUCT_TPICTURE {
	public:
		unsigned char *pDibDataNoFileHeader; // +00, result from DibOpenFile(...). Same as BMP content except first 0xE bytes ?

		// +04. Size is always 0x428 = BMP info size (0x28) + palette (0x400).
		// The 0x28 bytes header corresponds to "real" BMP data from offset 0xE to 0x36 (bitmap *file* header is missing)
		// The 0x400 next bytes are palette data
		// The header format is BITMAPINFOHEADER here.
		unsigned char *pHeaderAndPalette;

		unsigned char *pixelsData; // +0x8 : BMP data (excluding headers and palette). Size = ("alignedWidth")*(height)
		unsigned long int *unknown_0C; // +0C
		long int unknown_10; // +10. If "2", destructor will heapFree pointer '+00', otherwise it's DibFree of that pointer
		long int width; // +14. Image width
		long int height; // +18. Image height
		short int unknown_1C; // +1C, almost always 1 ? Set to -1 in a specific case see 0x452194 
		short int unused; // +1E

		// Get the width value to use for BMP format
		long int GetAlignedWidth() const {
			return (this->width + 3) & 0xFFFFFFFC; // remove 2 ending bits
		}
		// Returns buffer size of pixelsData
		long int GetPixelsDataSize() const {
			return this->GetAlignedWidth() * this->height;
		}
	};
	static_assert(sizeof(STRUCT_TPICTURE) == 0x20, "STRUCT_TPICTURE size");


}

