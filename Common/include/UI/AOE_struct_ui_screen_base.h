
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
		char unknown_0F4[0x478 - 0x0F4];
	};
	static_assert(sizeof(STRUCT_UI_SCREEN_BASE) == 0x478, "STRUCT_UI_SCREEN_BASE size");
}

