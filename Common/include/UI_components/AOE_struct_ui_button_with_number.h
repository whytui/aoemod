
#pragma once

#include <UI_components\AOE_struct_ui_button.h>
#include <AOE_structures_drs.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size 0x3D4. Constructor=0x4F7420
	class STRUCT_UI_BUTTON_WITH_NUMBER : public STRUCT_UI_BUTTON {
	public:
		// Starts at +2B8
		long int showNumber; // +2B8. If true, numberToDisplay will be displayed. Set in 0x4F72B0. Call refresh after changing it.
		long int numberToDisplay; // +2BC. The number to show on button icon, like queued item count. Call refresh after changing it.
		// 0x2C0
		char unknown_2C0; // Values 0, 3 .. ?
		char unknown_2C1[3];
		STRUCT_SLP_INFO *unknown_2C4_btnBoard;
		unsigned long int *unknown_2C8;
		unsigned long int unknown_2CC;
		char contextHelpText[0x100]; // +2D0.
		long int unknown_3D0; // see 483785
		bool IsCheckSumValid() {
			return (this->checksum == CHECKSUM_UI_BUTTON_WITH_NUMBER);
		}
	};
	static_assert(sizeof(STRUCT_UI_BUTTON_WITH_NUMBER) == 0x3D4, "STRUCT_UI_BUTTON_WITH_NUMBER size");

}

