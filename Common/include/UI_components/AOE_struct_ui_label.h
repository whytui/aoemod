
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
	// Represents text alignment for labels (including combobox displayed text). Vertical & horizontal alignment are separate values.
	enum TextPanelAlignment : long int {
		ALIGN_CENTER = 0, // align center.
		ALIGN_LEFT = 1, // align left. Not relevant for vertical align
		ALIGN_RIGHT = 2, // align right. Not relevant for vertical align
		ALIGN_TOP = 3, // align top. Not relevant for horizontal align
		ALIGN_BOTTOM = 4 // align bottom. Not relevant for horizontal align
	};


	// Size = 0x10
	class STRUCT_UI_LIST_ITEM {
	public:
		char *itemText;
		unsigned long int *unknown_04;
		long int optionalId; // +0C. Identifier for the list element. This is NOT the same as index.
		STRUCT_UI_LIST_ITEM *next;
	};


#define CHECKSUM_UI_LABEL 0x00545A90
#define CHECKSUM_UI_INPUT_PANEL 0x00545304
#define CHECKSUM_UI_LISTBOX 0x005453F8
#define CHECKSUM_UI_SCROLL_LABEL 0x00546594 // scroll text panel

	// Size 0x188. Constructor=0x4685B0 (no arg)
	// "TextPanel"
	// +0xEC = label.setText???(text)
	class STRUCT_UI_LABEL : public STRUCT_ANY_UI {
	public:
		STRUCT_UI_LIST_ITEM *firstItem; // +F4. There can be several of them (multi line ?)
		short int itemCount; // +F8. Maybe lines/texts count ?
		short int numberDrawLines; // +FA
		short int firstVisibleEntryIndex; // +FC
		short int lastVisibleEntryIndex; // +FE
		short int selectedIndex; // +100. current line number
		short int unknown_102;
		unsigned long int unknown_104;
		unsigned long int unknown_108;
		unsigned long int unknown_10C;
		// 0x110
		TextPanelAlignment verticalAlign; // +110. Set in 0x4F7CF0.
		TextPanelAlignment horizontalAlign; // +114. Set in 0x4F7CF0.
		long int wordWrap; // +118. Set in 0x469050.
		long int textStyle; // default 2. Set in 0x469070.
		// 0x120
		unsigned long int textColor; // +120. RGB. 4th byte is always 0. Taken as char[4], x[0]=red, x[1]=green, x[2]=blue. 000000=black
		unsigned long int textShadowColor; // +124. RGB. 4th byte is always 0. Taken as char[4], x[0]=red, x[1]=green, x[2]=blue. 000000=black
		unsigned long int highlightColor1; // type ?
		unsigned long int highlightColor2; // +12C.
		// 0x130
		unsigned long int unknown_130;
		unsigned long int unknown_134;
		unsigned short int unknown_138;
		unsigned short int unknown_13A; // type ?
		unsigned long int unknown_13C;
		unsigned long int unknown_140; // +140 oneElemSizeY (or half?) ? updated automatically from ?? Do not modify directly
		unsigned long int unknown_144;
		unsigned short int unknown_148;
		unsigned short int unknown_14A;
		unsigned long int fillBack;
		char fillBackColor; // +150. init -1
		char unknown_151; // +151. is it used ?
		short int unknown_152; // is it used ?
		long int unknown_154; // Set in 0x469190
		char unknown_158;
		char unknown_159; // is it used ?
		short int unknown_15A; // is it used ?
		unsigned long int unknown_15C;
		long int secondColumnPos; // +160. Set in 0x46A740
		unsigned long int unknown_164;
		unsigned long int unknown_168;
		unsigned long int unknown_16C;
		// 0x170
		long int unknown_170;
		char unknown_174;
		char unknown_175;
		char unknown_176;
		char unknown_177;
		char unknown_178;
		char unknown_179;
		char unknown_17A; // unused ?
		char unknown_17B; // unused ?
		unsigned long int unknown_17C;
		// 0x180
		long int unknown_180; // margin ??
		long int spacerSize; // +184. Set in 0x4692A0

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_LABEL; }
	};
	static_assert(sizeof(STRUCT_UI_LABEL) == 0x188, "STRUCT_UI_LABEL size");


	// Constructor=0x463650. Size=0x1B8.
	// 0x464290=TInputPanel::set_text(text_in)
	class STRUCT_INPUT_PANEL : public STRUCT_UI_LABEL {
	public:
		unsigned long int unknown_188;
		long int caretBlinkTime; // +18C
		unsigned long int unknown_190;
		long int unknown_194;
		unsigned long int unknown_198;
		long int format; // +19C
		char *inputBuffer; // +1A0
		unsigned long int *unknown_1A4;
		unsigned long int unknown_1A8;
		short int unknown_1AC;
		short int unknown_1AE;
		long int unknown_1B0;
		short int unknown_1B4;
		short int unknown_1B6; // unused ?

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_INPUT_PANEL; }
	};
	static_assert(sizeof(STRUCT_INPUT_PANEL) == 0x1B8, "STRUCT_INPUT_PANEL size");


	// "ScrollTextPanel". Constructor=0x47CC40. Size=0x198
	// 0x47CD50=TScrollTextPanel::set_text(text)
	class STRUCT_SCROLL_LABEL : public STRUCT_UI_LABEL {
	public:
		unsigned long int unknown_188;
		unsigned long int unknown_18C;
		unsigned long int unknown_190;
		unsigned long int unknown_194;
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_SCROLL_LABEL; }
	};
	static_assert(sizeof(STRUCT_SCROLL_LABEL) == 0x198, "STRUCT_SCROLL_LABEL size");

}

