
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
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

	// Size ?
	class STRUCT_UI_CAMPAIGN_EDITOR : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x47C - 0x0F4];
		STRUCT_ANY_UI *lblCampaignEditor;
		// 0x480
		STRUCT_ANY_UI *lblCampaignFilename;
		STRUCT_ANY_UI *lblScenarios;
		STRUCT_ANY_UI *lblCampaignScenarios;
		unsigned long int unknown_48C;
		// 0x490
		unsigned long int unknown_490;
		unsigned long int unknown_494;
		unsigned long int unknown_498;
		STRUCT_ANY_UI *unknown_49C_defaultCampaignInputDialog;
		// 0x4A0
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnCancel;
		STRUCT_UI_BUTTON *btnSave;
		STRUCT_UI_BUTTON *btnAdd;
		// 0x4B0
		STRUCT_UI_BUTTON *btnRemove;
		STRUCT_UI_BUTTON *btnUp;
		STRUCT_UI_BUTTON *btnDown;

		//bool IsCheckSumValid() { return this->checksum == 0x0054xxxx; }
	};

}

