
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

	// Size = 0x8B8. Constructor = 0x47C510. 68 64 54 00
	class STRUCT_UI_CREDITS_SCREEN : public STRUCT_ANY_UI {
	public:
		// unknown...
		bool IsCheckSumValid() { return this->checksum == 0x00546468; }
	};

}

