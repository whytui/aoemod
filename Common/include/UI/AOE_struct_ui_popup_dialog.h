
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
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

	// Size = 0x4A4
	// Constructor=0x434550
	class STRUCT_UI_POPUP_DIALOG : public STRUCT_ANY_UI {
	public:
		// Note on isActive: For a dialog it is probably always 0. Parent UI object is active, not the dialog.
		char unknown_0F4[0x498 - 0x0F4];
		STRUCT_UI_BUTTON *btnYes;
		STRUCT_UI_BUTTON *btnNo;
		// 4A0
		STRUCT_UI_BUTTON *btnCancel;
		// End.
		bool IsCheckSumValid() { return this->checksum == 0x00543CEC; }
	};

}

