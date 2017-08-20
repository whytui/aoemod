
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

#define CHECKSUM_UI_SCROLLBAR 0x005456B8
#define CHECKSUM_UI_VERTICAL_SLIDER 0x005458C8
#define CHECKSUM_UI_HORIZONTAL_SLIDER 0x005459AC

	// Size = 0x1C4 // B8 56 54 00
	// Constructor = 0x466E30. Destructor = 0x466F80
	class STRUCT_UI_SCROLLBAR : public STRUCT_ANY_UI {
	public:
		STRUCT_ANY_UI *objectToScroll; // +F4
		long int unknown_0F8;
		char unknown_FC[0x120 - 0xFC];
		long int unknown_120;
		long int unknown_124; // previous for +12C?
		long int unknown_128;
		long int unknown_12C;
		unsigned long int unknown_130;
		unsigned long int unknown_134;
		unsigned long int unknown_138;
		unsigned long int unknown_13C;
		char unknown_140[0x1C4 - 0x140];

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_SCROLLBAR; }
	};
	static_assert(sizeof(STRUCT_UI_SCROLLBAR) == 0x1C4, "STRUCT_UI_SCROLLBAR size");


	// Size=0x1CC
	class STRUCT_UI_VERTICAL_SLIDER : public STRUCT_UI_SCROLLBAR {
	public:
		long int unknown_1C4;
		unsigned long int unknown_1C8;

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_VERTICAL_SLIDER; }
	};
	static_assert(sizeof(STRUCT_UI_VERTICAL_SLIDER) == 0x1CC, "STRUCT_UI_VERTICAL_SLIDER size");


	// Size=0x1CC
	class STRUCT_UI_HORIZONTAL_SLIDER : public STRUCT_UI_SCROLLBAR {
	public:
		long int unknown_1C4;
		unsigned long int unknown_1C8;

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_HORIZONTAL_SLIDER; }
	};
	static_assert(sizeof(STRUCT_UI_HORIZONTAL_SLIDER) == 0x1CC, "STRUCT_UI_HORIZONTAL_SLIDER size");


}

