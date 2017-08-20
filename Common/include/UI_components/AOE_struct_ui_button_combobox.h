
#pragma once

#include <UI_components\AOE_struct_ui_button.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{
	// External dependencies
	class STRUCT_UI_COMBOBOX;

	// Size 0x2BC. Derives from standard button. Button dedicated to a dropdown.
	// Costructor=0x0461F20
	class STRUCT_UI_DROPDOWN_BUTTON : public STRUCT_UI_BUTTON {
	public:
		
		STRUCT_UI_COMBOBOX *combobox; // +2B8. The combobox it is attached to
		bool IsCheckSumValid() const {
			return (this->checksum == CHECKSUM_UI_DROPDOWN_BUTTON);
		}
	};
	static_assert(sizeof(STRUCT_UI_DROPDOWN_BUTTON) == 0x2BC, "STRUCT_UI_DROPDOWN_BUTTON size");

}

