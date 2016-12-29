
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
	
	// Size 0x16C? - Constructor = 0x462050. Name=TEditPanel
	// Can be created in 0x0456A30
#define CHECKSUM_UI_TEXTBOX 0x00545220
	class STRUCT_UI_TEXTBOX : public STRUCT_ANY_UI { // 20 52 54 00
	public:
		long int hWnd; // +F4.
		short int maxTextSize; // +F8. =constructor_maxTextSize+1 (to include ending \0)
		short int unknown_0FA; // +FA
		char *pTypedText; // +FC.
		// 0x100
		unsigned long int textLength; // +100. Text length, not counting ending \0.
		char unknown_104[0x114 - 0x104];
		long int textColor_114; // +114
		long int textColor_118; // +118
		char unknown_11C[0x124 - 0x11C];
		long int backGroundColor; // +124
		long int inputType; // +128. 0=normal, 3=numberOnly, 7=multiline
		long int unknown_12C_hObject; // object handle?
		char unknown_130[0x144 - 0x130];
		long int flagsForDrawTextEx; // +144.

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_TEXTBOX; }

		// Returns the maximum text length a TextBox can support.
		static long int GetMaxSupportedBufferSize() {
			return 0x8000 - 2;
		}

		void SetText(const char *text) {
			unsigned long int addr = 0x4623E0;
			_asm {
				MOV ECX, this;
				PUSH text;
				CALL addr; // supports arg1=NULL.
			}
		}
	};

}

