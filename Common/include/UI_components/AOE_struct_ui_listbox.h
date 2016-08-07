
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

	// Size = 0x10
	class STRUCT_UI_LIST_ITEM {
	public:
		char *itemText;
		unsigned long int *unknown_04;
		long int optionalId;
		STRUCT_UI_LIST_ITEM *next;
	};

	// Size = 0x1A0  // F8 53 54 00
	// Constructor = 0x464A50 (also 00456F20?). Parent class is label (90 5A 54 00).
	// See also STRUCT_UI_COMBOBOX (that contains a listbox)
#define CHECKSUM_UI_LISTBOX 0x005453F8
	class STRUCT_UI_LISTBOX : public STRUCT_ANY_UI {
	public:
		STRUCT_UI_LIST_ITEM *firstItem; // +F4. First element of list of items.
		short int itemCount; // +F8
		short int unknown_0FA;
		short int unknown_0FC; // minVisibleIndex??
		short int unknown_0FE; // maxVisibleIndex??
		short int selectedIndex; // +100.
		short int unknown_102; // +102
		// +138=word

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_LISTBOX; }
	};


}

