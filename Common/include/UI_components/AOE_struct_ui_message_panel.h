
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
	
#define CHECKSUM_UI_MESSAGE_PANEL 0x005454EC

	// Message panel (=>ingame chat message...). Size=0x380. Constructor=0x465730
	// Parent=TPanel (directly)
	// old name=STRUCT_UI_IN_GAME_TEXT_ZONE. Class for chat text lines (1 for each chat line), yellow/orange centered in-game error messages
	class STRUCT_UI_MESSAGE_PANEL : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4;
		char unknown_0F8[0x304 - 0xF4]; // F8: text?
		long int font; // +304
		long int fontWidth;
		long int fontHeight; // +30C
		unsigned long int unknown_310;
		unsigned long int unknown_314;
		unsigned long int unknown_318;
		unsigned long int unknown_31C;
		long int horizontalJustification; // +320
		long int verticalJustification; // +324
		long int wordWrap; // +328

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_MESSAGE_PANEL; }
	};
	// static_assert(sizeof(STRUCT_UI_MESSAGE_PANEL) == 0x380, "STRUCT_UI_MESSAGE_PANEL size");

}

