
#pragma once

#include <AOE_const_internal.h>
#include <AOE_const_drs.h>


namespace BUTTONBAR {

	namespace BUTTONBAR_CONST {
		const unsigned long int MAIN_GAME_ZONE_COMMAND_BAR_RESET_PAGE = 0x482290;

		// Returns the icon id relevant for provided UI command id, if found.
		// Returns -1 if not found.
		long int GuessIconIdFromUICommandId(AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID UICmdId);

	}

}
