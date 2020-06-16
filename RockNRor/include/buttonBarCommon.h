
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


		// Returns the cursor that corresponds to a given mouse action type (for custom ones)
		static GAME_CURSOR GetCursorForCustomActionType(MOUSE_ACTION_TYPES mouseActionType) {
			switch (mouseActionType) {
			case AOE_CONST_INTERNAL::CST_MAT_CR_PROTECT_UNIT_OR_ZONE:
				return GAME_CURSOR::GC_GROUP;
			case AOE_CONST_INTERNAL::CST_MAT_VILLAGER_SET_BUILDING_LOCATION:
				return GAME_CURSOR::GC_HAND;
			case AOE_CONST_INTERNAL::CST_MAT_NORMAL:
			case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_01:
			case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_02_SELECT_ZONE:
			case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_03_SELECT_ZONE:
			case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_04:
			case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_05:
			case AOE_CONST_INTERNAL::CST_MAT_VILLAGER_BUILD_MENU:
			case AOE_CONST_INTERNAL::CST_MAT_EDITOR_SET_UNIT_LOCATION:
			case AOE_CONST_INTERNAL::CST_MAT_SET_TERRAIN_TYPE:
			case AOE_CONST_INTERNAL::CST_MAT_SET_ALTITUDE:
			case AOE_CONST_INTERNAL::CST_MAT_EDITOR_DELETE_UNIT:
			case AOE_CONST_INTERNAL::CST_MAT_EDITOR_MOVE_UNIT:
			case AOE_CONST_INTERNAL::CST_MAT_EDITOR_SELECT_TARGET_UNIT:
			case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_11_SELECT_ZONE:
			case AOE_CONST_INTERNAL::CST_MAT_UNKNOWN_12_SELECT_UNIT_CANT_UNSELECT:
			case AOE_CONST_INTERNAL::CST_MAT_SET_CLIFFS:
			case AOE_CONST_INTERNAL::CST_MAT_EDITOR_CHANGE_ORIENTATION:
			case AOE_CONST_INTERNAL::CST_MAT_SET_WALLS:
			case AOE_CONST_INTERNAL::CST_MAT_CLICK_ZONE_TO_ATTACK:
			case AOE_CONST_INTERNAL::CST_MAT_SELECT_CONVERT_TARGET:
			case AOE_CONST_INTERNAL::CST_MAT_SELECT_HEAL_TARGET:
			case AOE_CONST_INTERNAL::CST_MAT_SELECT_UNKNOWN_67:
			case AOE_CONST_INTERNAL::CST_MAT_SELECT_REPAIR_TARGET:
			case AOE_CONST_INTERNAL::CST_MAT_SELECT_UNLOAD_TRANSPORT:
			default:
				return GAME_CURSOR::GC_NORMAL; // This is NOT correct for all types... This method is supposed to handle custom types, not standard ones.
			}
		}

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
