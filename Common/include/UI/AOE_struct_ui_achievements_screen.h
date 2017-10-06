
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

	// Size = 0x5F8. Constructor = 0x477340
	class STRUCT_UI_ACHIEVEMENTS_SCREEN : public STRUCT_UI_SCREEN_PANEL {
	public:
		char unknownu_478[0x5F8 - 0x478];
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_ACHIEVEMENTS_SCREEN; }
	};
	static_assert(sizeof(STRUCT_UI_ACHIEVEMENTS_SCREEN) == 0x5F8, "STRUCT_UI_ACHIEVEMENTS_SCREEN size");

}

