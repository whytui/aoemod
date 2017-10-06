
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

	// Size=0x490 - Constructor = 0x49A5F0
	// This is the "welcome" screen with 3 buttons (NOT menu in scenario editor screen)
	class STRUCT_UI_SCENARIO_EDITOR_MAIN_MENU : public STRUCT_UI_SCREEN_PANEL {
	public:
		STRUCT_ANY_UI *lblScenarioBuilder; // +478
		STRUCT_UI_BUTTON *btnCreateScenario;
		// 0x480
		STRUCT_UI_BUTTON *btnEditScenario;
		STRUCT_UI_BUTTON *btnCampaignEditor;
		STRUCT_UI_BUTTON *btnCancel;
		STRUCT_UI_BUTTON *btnUnknown_48C;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MAIN_MENU; }
	};
	static_assert(sizeof(STRUCT_UI_SCENARIO_EDITOR_MAIN_MENU) == 0x490, "STRUCT_UI_SCENARIO_EDITOR_MAIN_MENU size");

}

