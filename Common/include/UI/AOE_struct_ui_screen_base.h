
#pragma once

#include <UI_components\AOE_struct_any_ui.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size=0x478. A base class for all screens and popups (not UI components). Directly derives from base UI class.
	// Constructor = 0x454430, Init (own fields) 0x454B60.
#define CHECKSUM_UI_SCREEN_BASE 0x00544BBC
	class STRUCT_UI_SCREEN_BASE : public STRUCT_ANY_UI {
	public:
		long int unknown_0F4_sizeX;
		long int unknown_0F4_sizeY;
		char dialogName[0x10]; // +FC. unknown size
		char unknown_10C[0x180 - 0x10C];
		long int unknown_180; // +180. A value 0-4 ?
		char unknown_184[0x320 - 0x184];
		char unknown_320;
		char unknown_321;
		char unknown_322;
		char unknown_323;
		char unknown_324;
		char unknown_325;
		char unknown_326[2];
		long int unknown_328;
		unsigned long int unknown_32C;
		long int unknown_330;
		unsigned long int unknown_334;
		unsigned long int unknown_338;
		unsigned long int unknown_33C;
		char unknown_340_scrName[0x100]; // unknown size
		char unknown_440[4]; // Maybe part of unknown_340_scrName.
		unsigned long int *unknown_444;
		unsigned long int *unknown_448; // A pointer ?
		char unknown_44C[0x478 - 0x44C];
	};
	static_assert(sizeof(STRUCT_UI_SCREEN_BASE) == 0x478, "STRUCT_UI_SCREEN_BASE size");
}

