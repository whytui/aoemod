
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

	// Size = 0x1C4 // B8 56 54 00
	// Constructor = 0x466E30. Destructor = 0x466F80
#define CHECKSUM_UI_SCROLLBAR 0x005456B8
	class STRUCT_UI_SCROLLBAR : public STRUCT_ANY_UI {
	public:
		STRUCT_ANY_UI *objectToScroll; // +F4
		long int unknown_0F8;
		char unknown_FC[0x102 - 0xFC];
		long int unknown_120;
		long int unknown_124; // previous for +12C?
		long int unknown_128;
		long int unknown_12C;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_SCROLLBAR; }
	};


}

