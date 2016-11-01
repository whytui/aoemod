
#pragma once

#include <AOE_const_internal.h>
#include <AOE_const_drs.h>
#include <UI\AOE_struct_ui_in_game_main.h>
#include "interface.h"
#include "UI_utilities.h"


namespace BUTTONBAR {

	namespace BUTTONBAR_CONST {
		const unsigned long int MAIN_GAME_ZONE_COMMAND_BAR_RESET_PAGE = 0x482290;

		// Returns the icon id relevant for provided UI command id, if found.
		// Returns -1 if not found.
		long int GuessIconIdFromUICommandId(AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID UICmdId);

	}


	// Returns true if the button is visible. Use this overload for performance if you already have STRUCT_UI_IN_GAME_MAIN pointer.
	// Returns false if the button is hidden, or if an error occurs.
	bool IsInGameUnitCommandButtonVisible(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, long int buttonIndex);
	// Returns true if the button is visible
	// Returns false if the button is hidden, or if an error occurs.
	bool IsInGameUnitCommandButtonVisible(long int buttonIndex);

	// To be used with button IDs from unit defintion/researches to get a buttonIndex for game main UI structure (command buttons)
	// WARNING: returns -1 if DATButtonId is -1 or invalid.
	long int GetButtonInternalIndexFromDatBtnId(char DATButtonId);


}
