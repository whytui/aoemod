
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

	// Size = 0x4B8. Constructor = 0x486600. F8 69 54 00
	// OnKeyDown = 0x486C10
	class STRUCT_UI_WELCOME_MAIN_SCREEN : public STRUCT_UI_SCREEN_PANEL {
	public:
		// unknown...
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_WELCOME_MAIN_SCREEN; }
	};

}

