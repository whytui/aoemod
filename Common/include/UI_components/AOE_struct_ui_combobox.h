
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_listbox.h>
#include <UI_components\AOE_struct_ui_scrollbar.h>
#include <UI_components\AOE_struct_ui_label.h>
#include <UI_components\AOE_struct_ui_button_combobox.h>

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
	// 461DA0 : addEntry(text, index) ?
	// 461E70 : combobox.GetSelectedIndexFromListId(idInList) returns a list index
	// 461D50 : setSelectedRow?("id"?)
	// 0x4542C0 set hint dll string(dllid, -1) ?
#define CHECKSUM_UI_COMBOBOX 0x00545050
	class STRUCT_UI_COMBOBOX : public STRUCT_ANY_UI {
	public:
		STRUCT_UI_LABEL *displayedLabel; // +F4.
		STRUCT_UI_BUTTON_COMBOBOX *btnShowList; // +F8. 34 51 54 00
		STRUCT_UI_LISTBOX *underlyingListBox; // +FC.
		// 0x100
		STRUCT_UI_SCROLLBAR *scrollbar; // +100. Scrollbar to apply on listbox.
		unsigned long int unknown_104;
		short int selectedIndex; // +108. Or selected "id" ?
		unsigned short int unknown_10A;
		long int unknown_10C; // pos or size ?
		long int unknown_110; // pos or size ?
		long int unknown_114; // pos or size ?
		long int unknown_118; // pos or size ?
		char unknown_11C[0x130 - 0x11C];
		// 0x130
		char unknown_130;
		char unknown_131;
		short int unknown_132; // unknown type
		unsigned long int unknown_134; // unknown type
		char unknown_138;
		char unknown_139[3]; // unknown type
		long int unknown_13C_lbl_btn_relativePosOrder; // +13C. 0=text on left, 1=text on right
		// 0x140
		long int drawBorderAroundText; // init=1. Set in 0x461180 (do not update directly)

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_COMBOBOX; }
		long int GetSelectedIndex() {
			if (!this->IsCheckSumValid()) { return -1; }
			unsigned long int tmp = this->checksum;
			_asm {
				MOV ECX, tmp;
				MOV EAX, 0x461E50;
				CALL EAX;
				MOV tmp, EAX;
			}
			return tmp;
		}
		// Show text on right and button on the left if textOnRight=true, reverse otherwise.
		void ShowTextOnRight(bool textOnRight) {
			const unsigned long int addr = 0x461150;
			long int v = textOnRight ? 1 : 0;
			_asm {
				PUSH v;
				MOV ECX, this;
				CALL addr;
			}
		}
		// Draw a border around text (or hide the border if drawBorder=false)
		void SetBorderAroundText(bool drawBorder) {
			const unsigned long int addr = 0x461180;
			long int v = drawBorder ? 1 : 0;
			_asm {
				PUSH v;
				MOV ECX, this;
				CALL addr;
			}
		}
	};


}

