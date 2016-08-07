
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_button.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size ?
	class STRUCT_UI_LOAD_SAVED_GAME : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x478 - 0x0F4];
		STRUCT_ANY_UI *lblSavedGames;
		STRUCT_ANY_UI *lblSelectSavedGame;
		// 0x480
		STRUCT_ANY_UI *edtSavedGameList;
		STRUCT_ANY_UI *unknown_484; // type a name for the game?
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnCancel;
		// 0x490
		STRUCT_UI_BUTTON *btnDelete;

		//bool IsCheckSumValid() { return this->checksum == 0x0054xxxx; }
	};

}

