
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

	// Size ?
	// Can be created in 0x0456A30
#define CHECKSUM_UI_TEXTBOX 0x00545220
	class STRUCT_UI_TEXTBOX : public STRUCT_ANY_UI { // 20 52 54 00
	public:
		long int hWnd;
		short int maxTextSize; // +F8. =constructor_maxTextSize+1
		short int unknown_0FA; // +FA
		char *pTypedText; // +FC.
		// 0x100
		char unknown_100[0x128 - 0x100];
		long int inputType; // 0=normal, 3=numberOnly, 7=multiline

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_TEXTBOX; }
	};

}

