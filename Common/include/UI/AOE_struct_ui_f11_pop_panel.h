
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

	// "TribePanelPop". Size=0x164. Constructor=0x4FA150
#define CHECKSUM_UI_F11_POP_LABEL 0x00549F6C
	class STRUCT_UI_F11_POP_LABEL : public STRUCT_ANY_UI {
	public:
		long int unknown_0F4_font; // +F4.
		long int unknown_0F8;
		long int colorRGB; // +0xFC
		char unknown_100[0x10C - 0x100];
		STRUCT_PLAYER *player; // +10C
		long int currentPopulation; // +110
		long int currentMaxHostedPopulation; // +114. Maximum population currently supported by houses
		char unknown_118;
		char unknown_119[3]; // unused ?
		char unknown_11C[0x164 - 0x11C];
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_F11_POP_LABEL; }
	};
	static_assert(sizeof(STRUCT_UI_F11_POP_LABEL) == 0x164, "STRUCT_UI_F11_POP_LABEL size");

}

