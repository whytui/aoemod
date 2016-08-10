
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
	// 0x46A105 = addEntry(text, index) ? to confirm
	// 0x464E60 = listbox.GetEntryIndexForPosition(posX, posY) returns word
	// 0x4650C0 = listBox.highlightEntry???(enum1-7?, index, arg3) ?
#define CHECKSUM_UI_LISTBOX 0x005453F8
	class STRUCT_UI_LISTBOX : public STRUCT_ANY_UI {
	public:
		STRUCT_UI_LIST_ITEM *firstItem; // +F4. First element of list of items.
		short int itemCount; // +F8.
		short int unknown_0FA;
		short int firstVisibleEntryIndex; // +FC. Not necessarily first entry (if there is a scrollbar)
		short int lastVisibleEntryIndex; // +FE.
		short int selectedIndex; // +100.
		short int unknown_102; // +102
		char unknown_104[0x140 - 0x104];
		// +134 = long int seen 0xE
		// +138=word
		long int unknown_140; // +140 oneElemSizeY (or half?) ? updated automatically from ?? Do not modify directly
		char unknown_144[0x170 - 0x144];
		unsigned long int unknown_170; // dword
		char unknown_174[0x180 - 0x174];
		long int unknown_180; // +180. marginX ?
		long int unknown_184; // marginY ?
		unsigned long int unknown_188;
		unsigned long int unknown_18C;
		long int unknown_190; // default 50
		unsigned long int unknown_194;
		unsigned long int unknown_198;
		unsigned long int unknown_19C;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_LISTBOX; }

		// Get index in listbox (0 to n-1) of element that matches "optionalId"=elementId
		// Returns -1 if elementId was not found (AOE method's behaviour)
		long int GetIndexFromElementId(long int elementId) {
			unsigned long int addr = 0x46A570;
			long int res = -1;
			_asm {
				MOV ECX, this;
				PUSH elementId;
				CALL addr;
				MOV res, EAX;
			}
			return res;
		}
	};
	static_assert(sizeof(STRUCT_UI_LISTBOX) == 0x1A0, "STRUCT_UI_LISTBOX size");

}

