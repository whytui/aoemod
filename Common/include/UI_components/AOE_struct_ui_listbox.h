
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
		long int optionalId; // +0C. Identifier for the list element. This is NOT the same as index.
		STRUCT_UI_LIST_ITEM *next;
	};

	// Size = 0x1A0  // F8 53 54 00
	// Constructor = 0x464A50 (also 00456F20?). Parent class is label (90 5A 54 00).
	// See also STRUCT_UI_COMBOBOX (that contains a listbox)
	// 0x46A105 = addEntry(text, index) ?
	// 0x46A570 = listbox.GetSelectedIndexFromListId(idInList)
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
		char unknown_104[0x140 - 0x104];
		// +138=word
		long int unknown_140; // +140 oneElemSizeY ? updated automatically from ?? Do not modify directly
		char unknown_144[0x180 - 0x144];
		long int unknown_180; // item count ?

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_LISTBOX; }
	};


}

