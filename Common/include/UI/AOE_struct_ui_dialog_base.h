
#pragma once

#include <UI\AOE_struct_ui_screen_base.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size=0x490. A base class for all popups ("dialogs"), excluding "screens".
	// Constructor = 0x460730, Init (own fields) 0x.
#define CHECKSUM_UI_DIALOG_BASE 0x00544F1C
	class STRUCT_UI_DIALOG_BASE : public STRUCT_UI_EASY_PANEL {
	public:
		unsigned long int unknown_478;
		unsigned long int unknown_47C;
		long int unknown_480; // init=0xB
		long int unknown_484; // init=0xB
		long int unknown_488; // init=0xB
		unsigned long int unknown_48C;
	};
	static_assert(sizeof(STRUCT_UI_DIALOG_BASE) == 0x490, "STRUCT_UI_DIALOG_BASE size");


	// Note : some child dialogs:
	// diplomacy : 14 38 54 00, 0x431470
	// Menu Dialog : B8 38 54 00, 0x433C30
}

