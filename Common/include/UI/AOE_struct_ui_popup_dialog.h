
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_button.h>
#include <UI\AOE_struct_ui_dialog_base.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size = 0x4A4. Constructor=0x434550. "MessageDialog". Child from DialogPanel.
	class STRUCT_UI_MESSAGE_DIALOG : public STRUCT_UI_DIALOG_BASE {
	public:
		// Note on isActive: For a dialog it is probably always 0. Parent UI object is active, not the dialog.
		char unknown_490[0x498 - 0x490];
		STRUCT_UI_BUTTON *btnYes;
		STRUCT_UI_BUTTON *btnNo;
		// 4A0
		STRUCT_UI_BUTTON *btnCancel;
		// End.
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_MESSAGE_DIALOG; }
	};
	static_assert(sizeof(STRUCT_UI_MESSAGE_DIALOG) == 0x4A4, "STRUCT_UI_MESSAGE_DIALOG size");

}

