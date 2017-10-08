
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

	// Size 0x0x4BC. Constructor = 0x433C30 (create and show). B8 3B 54 00. Parent=dialogbase (then easypanel)
	class STRUCT_UI_IN_GAME_MENU : public STRUCT_UI_DIALOG_BASE {
	public:
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_IN_GAME_MENU; }
	};


}

