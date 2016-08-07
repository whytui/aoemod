
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_listbox.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size = 0x144 - 50 50 54 00. Direct parent is STRUCT_ANY_UI
	// Constructor = 0x460A00
#define CHECKSUM_UI_COMBOBOX 0x00545050
	class STRUCT_UI_COMBOBOX : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4;
		unsigned long int unknown_0F8;
		STRUCT_UI_LISTBOX *underlyingListBox;
		// 0x100
		unsigned long int unknown_100;
		unsigned long int unknown_104;
		unsigned short int unknown_108;
		unsigned short int unknown_10A;
		unsigned long int unknown_10C; // unknown type
		char unknown_110[0x130 - 0x110];
		// 0x130
		char unknown_130;
		char unknown_131;
		short int unknown_132; // unknown type
		unsigned long int unknown_134; // unknown type
		unsigned long int unknown_138; // unknown type
		unsigned long int unknown_13C;
		// 0x140
		unsigned long int unknown_140; // init=1

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_COMBOBOX; }
		long int GetValue() {
			if (!this->IsCheckSumValid()) { return -1; }
			unsigned long int tmp = this->checksum;
			_asm {
				MOV ECX, tmp
					MOV EAX, 0x461E50
					CALL EAX
					MOV tmp, EAX
			}
			return tmp;
		}
	};


}

