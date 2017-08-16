
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
	// External dependency
	class STRUCT_UNIT_BASE;


#define CHECKSUM_ACTIVE_SPRITE 0x00543030
	// Size=0x14. Constructor = 0x415A00, 0x415A40 (fromFile)
	class STRUCT_ACTIVE_SPRITE {
	public:
		unsigned long int checksum;
		void *sprite; // +04
		char unknown_08; // 1 for active sprite, 2 for active animated sprite ?
		char unknown_09[3];
		unsigned long int unknown_0C_offset; // +0C. Set in 0x415AC0
		unsigned long int unknown_10_offset;
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTIVE_SPRITE; }
	};
	static_assert(sizeof(STRUCT_ACTIVE_SPRITE) == 0x14, "STRUCT_ACTIVE_SPRITE size");


#define CHECKSUM_ACTIVE_ANIMATED_SPRITE 0x00543078
	// Size=0x28. Constructor = 0x415D80 (fromFile)
	class STRUCT_ACTIVE_ANIMATED_SPRITE : STRUCT_ACTIVE_SPRITE {
	public:
		short int frame; // +14. Current frame index (starts at 0) ?
		short int unknown_16; // unused ?
		unsigned long int unknown_18;
		unsigned long int unknown_1C;
		short int unknown_20; // presvious frame ???
		char unknown_22;
		char isLooped; // +23.
		char unknown_24;
		char unused[3];
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTIVE_ANIMATED_SPRITE; }
	};
	static_assert(sizeof(STRUCT_ACTIVE_ANIMATED_SPRITE) == 0x28, "STRUCT_ACTIVE_ANIMATED_SPRITE size");


	class STRUCT_ACTIVE_SPRITE_LINK {
	public:
		STRUCT_ACTIVE_SPRITE *sprite;
		char unknown_value; // +04. Seen 0x3C (60) hardcoded. A string length ?
		char unused[3];
		STRUCT_ACTIVE_SPRITE_LINK *previous; // NULL for the "first" element of the chained list.
	};
	static_assert(sizeof(STRUCT_ACTIVE_SPRITE_LINK) == 0x0C, "STRUCT_ACTIVE_SPRITE_LINK size");


#define CHECKSUM_ACTIVE_SPRITE_LIST 0x00543000
	// Size=0x0C. Constructor = 0x415370 ?
	class STRUCT_ACTIVE_SPRITE_LIST {
	public:
		unsigned long int checksum;
		STRUCT_ACTIVE_SPRITE_LINK *spritesLastElement; // +04. Chained list of sprite links.
		void *ptrUnit;
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_ACTIVE_SPRITE_LIST; }
	};
	static_assert(sizeof(STRUCT_ACTIVE_SPRITE_LIST) == 0x0C, "STRUCT_ACTIVE_SPRITE_LIST size");


}

