
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

	// Size = 0x4B0. Constructor = 0x47B440. 08 62 54 00
	// "Join Screen" = MP search game screen.
	class STRUCT_UI_JOIN_SCREEN : public STRUCT_ANY_UI {
	public:
		// unknown...
		bool IsCheckSumValid() { return this->checksum == 0x00546208; }
	};

}

