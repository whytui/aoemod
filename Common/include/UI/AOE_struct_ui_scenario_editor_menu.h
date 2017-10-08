
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
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

	// Size 0x4B4 - Constructor = 0x4349A0 - 20 3E 54 00
	// "Scenario Menu Dialog"

	class STRUCT_UI_SCENARIO_EDITOR_MENU : public STRUCT_UI_DIALOG_BASE {
	public:
		// +490...
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MENU; }
	};

}

