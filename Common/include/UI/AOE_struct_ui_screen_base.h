
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

#define CHECKSUM_UI_SCREEN_BASE 0x00544BBC
#define CHECKSUM_UI_SCREEN_INTERMEDIATE 0x0054579C // Some mysterious intermediate (child) class with no own member ?
#define CHECKSUM_UI_IN_GAME_MAIN 0x0054679C
#define CHECKSUM_UI_SCENARIO_EDITOR_MAIN 0x00547360
#define CHECKSUM_UI_GAME_SETTINGS 0x00547810

	// Size=0x478. A base class for all screens and popups (not UI components). Directly derives from base UI class.
	// Constructor = 0x454430 (0x468150 for intermediate class), Init (own fields) 0x454B60.
	class STRUCT_UI_SCREEN_BASE : public STRUCT_ANY_UI {
	public:
		long int screenSizeX; // +F4. Default 640
		long int screenSizeY; // +F8. Default 480
		char dialogName[0x104]; // +FC. unknown size
		// 0x200
		long int slpIdForPopupBackground1_200;
		unsigned long int unknown_204;
		unsigned long int unknown_208;
		unsigned long int unknown_20C;
		char unknownName_mcursors[0x104]; // +210.
		long int unknown_slpId_314; // +314
		unsigned long int unknown_318;
		unsigned long int unknown_31C;
		char unknown_320;
		char unknown_321;
		char unknown_322;
		char unknown_323;
		char unknown_324;
		char unknown_325;
		char unknown_326[2]; // unused
		long int unknown_328;
		unsigned long int unknown_32C;
		long int unknown_330;
		unsigned long int unknown_334;
		unsigned long int unknown_338;
		unsigned long int unknown_33C;
		char dialogNameForPopup[0x104]; // +340. Dialog name for options popup (dlg6_0) + diplomacy, menu, etc
		long int themeSlpId; // +444. Determines the colors, background images, UI component themes to use in popups
		unsigned long int unknown_448; // pointer?
		char unknown_44C[0x478 - 0x44C];
		bool IsCheckSumValid() const {
			return (this->checksum == CHECKSUM_UI_SCREEN_BASE);
		}
		bool IsCheckSumValidForAChildClass() const {
			return (this->checksum == CHECKSUM_UI_SCREEN_BASE) || (this->checksum == CHECKSUM_UI_SCREEN_INTERMEDIATE) ||
				(this->checksum == CHECKSUM_UI_IN_GAME_MAIN) || (this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MAIN) || 
				(this->checksum == CHECKSUM_UI_GAME_SETTINGS);
		}
	};
	static_assert(sizeof(STRUCT_UI_SCREEN_BASE) == 0x478, "STRUCT_UI_SCREEN_BASE size");
}

