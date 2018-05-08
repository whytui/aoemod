
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

	// Size = 0x144 - 50 50 54 00. Direct parent is STRUCT_ANY_UI. "DropDownPanel"
	// Constructor = 0x460A00. 
	// *** WARNING *** Comboboxes are not intended to be used in popups and don't work well in such case in *native* ROR code.
	// Known issues are (all pointer issues can provoke game crash):
	// - Clicking on the combobox button messes with current screen's parent screen's "focused object".
	// - Opening the combobox/clicking outside to close it/reopening it/choosing an option corrupts panelSystem.previousFocusedObject.
	// - Opening the combobox messes with panelSystem's keyboard owner.
	// - Using the combobox (using ESC key while dropdown is opened ?) messes with panelSystem's modalPanel pointer.
	// 0x461D50 = setSelectedIndex
	// 0x4542C0 = set hint dll string(dllid, -1) ?
	// 0x461DA0 = combobox.appendLine(text, index)
#define CHECKSUM_UI_COMBOBOX 0x00545050
	class STRUCT_UI_COMBOBOX : public STRUCT_ANY_UI {
	public:
		STRUCT_UI_LABEL *displayedLabel; // +F4.
		STRUCT_UI_DROPDOWN_BUTTON *btnShowList; // +F8. 34 51 54 00
		STRUCT_UI_LISTBOX *underlyingListBox; // +FC.
		STRUCT_UI_SCROLLBAR *scrollbar; // +100. Scrollbar to apply on listbox.
		long int dropDownMode; // +104. Status ? 1=normal, 0=list_shown ? unclear and unsure
		short int selectedIndex; // +108. Index in list of current selection (0 to n-1).
		unsigned short int unknown_10A;
		long int labelSizeX; // +10C
		long int labelSizeY; // +110
		long int unknown_114; // btnSizeX? or pos ? unsure
		long int unknown_118; // btnSizeY? or pos ? unsure
		long int unknown_11C; // +11C. Listbox sizeX ? Calculated when listbox is shown
		long int unknown_120; // +120. Listbox sizeY ? Calculated when listbox is shown
		char unknown_124[0x130 - 0x124];
		// 0x130
		char unknown_130;
		char unknown_131;
		short int unknown_132; // unknown type
		long int unknown_134; // 
		char unknown_138;
		char unknown_139[3]; // unknown type
		long int drawStyle; // +13C. 0=text on left, 1=text on right
		long int drawBorderAroundText; // +140. init=1. Set in 0x461180 (do not update directly)

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_COMBOBOX; }
		// Returns index (0 to n-1) of selected entry in the list.
		// Can differ from this->selectedIndex if the user navigated to another entry without clicking, and pressed ESC:
		// In this case, GetListSelectedIndex returns list's highlighted entry, which is NOT selected one at combobox level.
		long int GetListSelectedIndex() const {
			if (!this->IsCheckSumValid()) { return -1; }
			const unsigned long int addr = 0x461E50;
			long int tmp = -1;
			_asm {
				MOV ECX, this;
				CALL addr;
				MOV tmp, EAX;
			}
			return tmp;
		}
		// Set selected index. Argument is row index (not element "custom" ID)
		void SetSelectedIndex(long int index) {
			const unsigned long int addr = 0x461D50;
			_asm {
				MOV ECX, this;
				PUSH index;
				CALL addr;
			}
		}
		// Get index in combobox (0 to n-1) of element that matches "optionalId"=elementId
		// Returns -1 if elementId was not found (AOE method's behaviour)
		long int GetIndexFromElementId(long int elementId) const {
			unsigned long int addr = 0x461E70;
			long int res = -1;
			_asm {
				MOV ECX, this;
				PUSH elementId;
				CALL addr;
				MOV res, EAX;
			}
			return res;
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
		// Remove all entries from list
		void Clear() {
			_asm {
				MOV ECX, this;
				MOV EDX, 0x461F10;
				CALL EDX;
			}
		}
	};


}

