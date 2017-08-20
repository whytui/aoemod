
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

#define CHECKSUM_UI_CAMPAIGN_EDITOR 0x00545FB0

	// Size=0x4C4. Constructor=0x479360
	class STRUCT_UI_CAMPAIGN_EDITOR : public STRUCT_UI_SCREEN_PANEL {
	public:
		/*char unknown_0F4[0x47C - 0x0F4];*/
		unsigned long int unknown_478;
		STRUCT_ANY_UI *lblCampaignEditor; // +47C.
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
		unsigned long int unknown_4BC;
		unsigned long int unknown_4C0;

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_CAMPAIGN_EDITOR; }
	};
	static_assert(sizeof(STRUCT_UI_CAMPAIGN_EDITOR ) = 0x4C4, "STRUCT_UI_CAMPAIGN_EDITOR size");

}

