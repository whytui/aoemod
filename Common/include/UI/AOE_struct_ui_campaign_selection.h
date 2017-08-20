
#pragma once

#include <UI\AOE_struct_ui_screen_base.h>
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

#define CHECKSUM_UI_CAMPAIGN_SELECTION 0x005460DC

	// Size=0x4CC. Constructor=0x47A7C0
	// 0x47B140=TRIBE_Screen_Campaign_Selection::startGame(void)
	class STRUCT_UI_CAMPAIGN_SELECTION : public STRUCT_UI_SCREEN_PANEL {
	public:
		char unknown_478[0x4CC - 0x478];

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_CAMPAIGN_SELECTION; }
	};
	static_assert(sizeof(STRUCT_UI_CAMPAIGN_SELECTION) = 0x4CC, "STRUCT_UI_CAMPAIGN_SELECTION size");

}

