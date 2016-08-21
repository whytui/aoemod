
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

	// Size 0x188
	// Constructor=0x4685B0
#define CHECKSUM_UI_LABEL 0x00545A90
	class STRUCT_UI_LABEL : public STRUCT_ANY_UI {
	public:
		char **labelTexts; // There can be several of them (multi line ?)
		unsigned short int unknown_0F8; // Maybe lines/texts count ?
		unsigned short int unknown_0FA;
		unsigned short int unknown_0FC;
		unsigned short int unknown_0FE;
		// 0x100
		unsigned short int unknown_100;
		unsigned short int unknown_102;
		unsigned long int unknown_104;
		unsigned long int unknown_108;
		unsigned long int unknown_10C;
		// 0x110
		unsigned long int unknown_110;
		unsigned long int unknown_114;
		unsigned long int unknown_118;
		unsigned long int unknown_11C; // default 2 ?
		// 0x120
		unsigned long int unknown_120;
		unsigned long int unknown_124;
		unsigned long int unknown_128; // type ?
		unsigned long int unknown_12C;
		// 0x130
		unsigned long int unknown_130;
		unsigned long int unknown_134;
		unsigned short int unknown_138;
		unsigned short int unknown_13A; // type ?
		unsigned long int unknown_13C;
		// 0x140
		unsigned long int unknown_140;
		unsigned long int unknown_144;
		unsigned short int unknown_148;
		unsigned short int unknown_14A;
		unsigned long int unknown_14C;
		// 0x150
		char unknown_150; // init -1
		char unknown_151; // is it used ?
		short int unknown_152; // is it used ?
		long int unknown_154; // Set in 0x469190
		char unknown_158;
		char unknown_159; // is it used ?
		short int unknown_15A; // is it used ?
		unsigned long int unknown_15C;
		// 0x160
		unsigned long int unknown_160;
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
		unsigned long int unknown_180;
		unsigned long int unknown_184;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_LABEL; }
	};


}
