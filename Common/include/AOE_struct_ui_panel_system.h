
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <UI\AOE_struct_ui_screen_base.h>

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
	// Size > 0xAD4?. Constructor = 0x451990
	// Name=TPanelSystem
	// 0x451AE0 = panelSystem.getPanel(name)
	// 0x451B00 = panelSystem.addPanel(panel, makeCurrent, makeModal)
	// 0x451DF0 = panelSystem::destroyPanel(name) = close screen
	class STRUCT_UI_PANEL_SYSTEM {
	public:
		STRUCT_ANY_UI *mouseOwner; // +00. If non-NULL, this object will intercept all mouse events (other components will be unreactive)
		STRUCT_ANY_UI *keyboardOwner; // +04
		STRUCT_UI_EASY_PANEL *modalPanel; // +08. Always an easyPanel ? Always a dialogPanel ?
		STRUCT_UI_EASY_PANEL *currentScreen; // +0C. Current screen. If a popup is opened, currentScreen still refers to its parent screen. "currentPanel"
		unsigned long int unknown_010;
		long int openedScreensCount; // +14. "activePanels"
		STRUCT_ANY_UI *previousFocusedObject; // +18. unsure
		char unknown_01C[0x48 - 0x1C];
		STRUCT_ANY_UI *scenarioEditor; // +48. TO CONFIRM

		// +AC0 = IMEenabled
		// +AC4 = isIMEOn
		// +AC8 = inputEnabled
		// +AD0 = color_window before ?
		// +AD4 = color_windowtext before ?
	};

}

