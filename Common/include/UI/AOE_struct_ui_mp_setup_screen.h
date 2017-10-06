
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_button.h>
#include <UI_components\AOE_struct_ui_combobox.h>
#include <UI_components\AOE_struct_ui_label.h>
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


	// Size = 0x898 . 7C 6D 54 00. Parent=screenPanel.
	// Main screen for game settings (MP or SP)
	// Constructor: 0x487F40
	// 0x48B640: mpSetupScreen.InitPlayers?(?)
	class STRUCT_UI_MP_SETUP_SCREEN : public STRUCT_UI_SCREEN_PANEL {
	public:
		STRUCT_UI_LABEL *unknown_478;
		STRUCT_UI_LABEL *unknown_47C;
		STRUCT_UI_LABEL *unknown_480;
		STRUCT_ANY_UI *unknown_484;
		STRUCT_UI_LABEL *unknown_488;
		STRUCT_UI_LABEL *unknown_48C;
		// Objects for players 1-8
		STRUCT_UI_LABEL *unknown_490[8];
		STRUCT_UI_LABEL *unknown_4B0[8];
		STRUCT_UI_LABEL *unknown_4D0[8];
		STRUCT_UI_LABEL *lblHasCd[8]; // +4F0. has CD ?
		STRUCT_UI_LABEL *unknown_510[8];
		STRUCT_UI_LABEL *unknown_530[8];
		STRUCT_UI_LABEL *unknown_550[8];
		STRUCT_UI_BUTTON *unknown_570[8];
		STRUCT_UI_BUTTON *unknown_590[8];
		STRUCT_UI_COMBOBOX *cbxPlayerTypes[8]; // +5B0. open/closed/computer. Read-only in SP game settings.
		STRUCT_UI_COMBOBOX *cbxPlayerCivilization[8]; // +5D0. Civilization choice for each player.
		STRUCT_UI_COMBOBOX *cbxUnknown_5F0[8]; // +5F0. team ?
		STRUCT_ANY_UI *unknown_610;
		STRUCT_UI_LABEL *unknown_614_chat;
		STRUCT_ANY_UI *unknown_618;
		STRUCT_ANY_UI *unknown_61C;
		STRUCT_ANY_UI *unknown_620;
		STRUCT_ANY_UI *unknown_624;
		STRUCT_ANY_UI *unknown_628;
		STRUCT_ANY_UI *unknown_62C[20];
		STRUCT_ANY_UI *unknown_67C[20];
		STRUCT_ANY_UI *unknown_6CC;
		STRUCT_ANY_UI *unknown_6D0;
		STRUCT_ANY_UI *unknown_6D4;
		STRUCT_ANY_UI *unknown_6D8;
		STRUCT_UI_LABEL *lblPlayerCount; // +6DC.
		STRUCT_UI_COMBOBOX *cbxPlayerCount; // +6E0. Init in 0x48D1D0
		STRUCT_ANY_UI *unknown_6E4;
		STRUCT_ANY_UI *unknown_6E8;
		STRUCT_ANY_UI *unknown_6EC;
		STRUCT_ANY_UI *unknown_6F0;
		STRUCT_ANY_UI *unknown_6F4;
		STRUCT_ANY_UI *unknown_6F8;
		STRUCT_ANY_UI *unknown_6FC;
		STRUCT_UI_BUTTON *unknown_700;
		STRUCT_ANY_UI *unknown_704;
		STRUCT_ANY_UI *unknown_708;
		STRUCT_ANY_UI *unknown_70C;
		STRUCT_ANY_UI *unknown_710;
		STRUCT_ANY_UI *unknown_714;
		STRUCT_ANY_UI *unknown_718;
		STRUCT_ANY_UI *unknown_71C;
		STRUCT_ANY_UI *unknown_720;
		STRUCT_ANY_UI *unknown_724;
		long int maxPlayerCount; // +728. 8 in RM/DM, can change in scenarios ?
		STRUCT_ANY_UI *unknown_72C;
		long int unknown_730;
		STRUCT_ANY_UI *unknown_734;
		STRUCT_ANY_UI *unknown_738;
		STRUCT_ANY_UI *unknown_73C;
		char unknown_740;
		char unknown_741[3];
		char unknown_744[0x844 - 0x744];
		STRUCT_ANY_UI *unknown_844;
		STRUCT_ANY_UI *unknown_848;
		STRUCT_ANY_UI *unknown_84C;
		STRUCT_ANY_UI *unknown_850;
		STRUCT_ANY_UI *unknown_854;
		STRUCT_ANY_UI *unknown_858;
		STRUCT_ANY_UI *unknown_85C;
		unsigned long int unknown_860;
		unsigned long int unknown_864;
		unsigned long int unknown_868[8];
		STRUCT_ANY_UI *unknown_888;
		STRUCT_ANY_UI *unknown_88C;
		unsigned long int unknown_890;
		unsigned long int unknown_894;

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_MP_SETUP_SCREEN; }
	};
	static_assert(sizeof(STRUCT_UI_MP_SETUP_SCREEN) == 0x898, "STRUCT_UI_MP_SETUP_SCREEN size");

}

