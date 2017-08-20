
#pragma once

#include <UI\AOE_struct_ui_screen_base.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

#define CHECKSUM_UI_DIALOG_BASE 0x00544F1C
#define CHECKSUM_UI_TLISTDIALOG 0x00543A7C
#define CHECKSUM_UI_RGE_DIALOG_LIST 0x00545B84


	// Size=0x490. "DialogPanel". A base class for all dialog popups ("dialogs"), excluding "screens".
	// Constructor = 0x460730, Init (own fields) 0x.
	// 0x4607A0 = dialog.setup(renderArea, parentUI, hSize, vSize, dlgName?, resourceFileId, allowShadowArea)
	// 0x460890 = dialog.setup(renderArea, parentUI, hSize, vSize, color, allowShadowArea)
	class STRUCT_UI_DIALOG_BASE : public STRUCT_UI_EASY_PANEL {
	public:
		unsigned long int unknown_478;
		unsigned long int unknown_47C;
		long int unknown_480; // init=0xB
		long int unknown_484; // init=0xB
		long int unknown_488; // init=0xB
		unsigned long int unknown_48C;

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_DIALOG_BASE; }
	};
	static_assert(sizeof(STRUCT_UI_DIALOG_BASE) == 0x490, "STRUCT_UI_DIALOG_BASE size");


	// Size=.. Constructor=0x433790
	class STRUCT_UI_TLISTDIALOG : public STRUCT_UI_DIALOG_BASE {
	public:

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_TLISTDIALOG; }
	};
	//static_assert(sizeof(STRUCT_UI_TLISTDIALOG) == 0x, "STRUCT_UI_TLISTDIALOG size");


	// Size=... Constructor=0x46A890
	class STRUCT_UI_RGE_DIALOG_LIST : public STRUCT_UI_TLISTDIALOG {
	public:

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_RGE_DIALOG_LIST; }
	};
	//static_assert(sizeof(STRUCT_UI_RGE_DIALOG_LIST) == 0x, "STRUCT_UI_RGE_DIALOG_LIST size");


	// Note : some child dialogs:
	// diplomacy : 14 38 54 00, 0x431470
	// Menu Dialog : B8 38 54 00, 0x433C30
}

