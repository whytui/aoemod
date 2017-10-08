
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_button.h>
#include <UI_components\AOE_struct_ui_label.h>
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
	// In-game diplomacy menu (dialog)
	// Size = 0x6B8, ccor=0x431470
	class STRUCT_UI_DIPLOMACY_DIALOG : public STRUCT_UI_DIALOG_BASE {
	public:
		// 0x490
		STRUCT_ANY_UI *lblDiplomacy;
		STRUCT_ANY_UI *lblName;
		STRUCT_ANY_UI *lblCivilization;
		STRUCT_ANY_UI *lblAlly;
		// 0x4A0
		STRUCT_ANY_UI *lblNeutral;
		STRUCT_ANY_UI *lblEnemy;
		STRUCT_ANY_UI *lblPayTribute;
		STRUCT_ANY_UI *lblAlliedVictory;
		// 0x4B0
		char unknown_4B0[0x510 - 0x4B0];
		// 0x510
		STRUCT_ANY_UI *lblNeedMarketToPayTribute;
		char unknown_514[0x5F4 - 0x514];
		STRUCT_ANY_UI *edtAlliedVictory; // +5F4. Checkbox
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnClearTributes;
		// 0x600
		STRUCT_UI_BUTTON *btnCancel;
		unsigned long int unknown_604;
		char unknown_608[0x620 - 0x608];
		// 0x620
		STRUCT_ANY_UI *lblFoodTribute;
		STRUCT_ANY_UI *lblGoldTribute;
		STRUCT_ANY_UI *lblWoodTribute;
		STRUCT_ANY_UI *lblStoneTribute;
		// 0x630
		char unknown_630[0x6B8 - 0x630];

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_DIPLOMACY_DIALOG; }
	};
	static_assert(sizeof(STRUCT_UI_DIPLOMACY_DIALOG) == 0x6B8, "STRUCT_UI_DIPLOMACY_DIALOG size");

}

