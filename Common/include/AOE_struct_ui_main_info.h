
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

	// Structure included in 0x5830E8 address (ROR 1.0c) => see ADDR_VAR_UI_MAIN_INFO
	// Size > 0xAD0?. Constructor = 0x451990?
	class STRUCT_UI_MAIN_INFO {
	public:
		STRUCT_ANY_UI *uiObjectReceivingEvents; // +00. If non-NULL, this object will intercept all UI events (other components will be unreactive)
		STRUCT_ANY_UI *unknown_004;
		STRUCT_ANY_UI *unknown_008;
		STRUCT_ANY_UI *currentScreen; // +0C. Current screen. If a popup is opened, currentScreen still refers to its parent screen.
		unsigned long int unknown_010;
		long int openedScreensCount; // +14, unsure.
		STRUCT_ANY_UI *previousFocusedObject; // +18. unsure
		char unknown_01C[0x48 - 0x1C];
		STRUCT_ANY_UI *scenarioEditor; // +48. TO CONFIRM
	};

}

