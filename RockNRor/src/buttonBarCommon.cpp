
#include "../include/buttonBarCommon.h"

namespace BUTTONBAR {
;
namespace BUTTONBAR_CONST {
; // for indentation

// Returns the icon id relevant for provided UI command id, if found.
// Returns -1 if not found. (WARNING: check this case in caller, -1 is not an acceptable value)
long int GuessIconIdFromUICommandId(AOE_CONST_INTERNAL::INGAME_UI_COMMAND_ID UICmdId) {
	// Note : icon 6 in actions is a nice flag but is unused.
	switch (UICmdId) {
	case AOE_CONST_INTERNAL::CST_IUC_WORK:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_INVALID_NONE; // does not have an icon ?
	case AOE_CONST_INTERNAL::CST_IUC_MOVE:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_MOVE; // Just a supposition as it is not implemented ! (actually, from early versions then removed, probably)
	case AOE_CONST_INTERNAL::CST_IUC_BUILD:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_BUILD; // cf 482598, 483273
	case AOE_CONST_INTERNAL::CST_IUC_EXCHANGE:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_INVALID_NONE; // unused/not implemented?
	case AOE_CONST_INTERNAL::CST_IUC_STOP:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_HAND_STOP; // hand icon
	case AOE_CONST_INTERNAL::CST_IUC_CANCEL_SELECTION:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_CANCEL_SELECTION;
	case AOE_CONST_INTERNAL::CST_IUC_UNLOAD_TRANSPORT:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_UNLOAD_TRANSPORT;
	case AOE_CONST_INTERNAL::CST_IUC_REGROUP:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_REGROUP;
	case AOE_CONST_INTERNAL::CST_IUC_UNGROUP:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_UNGROUP;
	case AOE_CONST_INTERNAL::CST_IUC_FORMATION:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_CANCEL_OR_BACK:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_CANCEL_SELECTION; // Cf 482526.
	case AOE_CONST_INTERNAL::CST_IUC_NEXT_PAGE:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_NEXT_PAGE; // Cf 4824F9.
	case AOE_CONST_INTERNAL::CST_IUC_CHAT:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_DIPLOMACY:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_MENU:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_TRADE_WITH:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_CANCEL_BUILD:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_HAND_STOP; // cf 0x482C30
	case AOE_CONST_INTERNAL::CST_IUC_SHOW_HELP:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_HOLD_POSITION:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_HOLD_POSITION; // cf 4827AD
	case AOE_CONST_INTERNAL::CST_IUC_ATTACK_POSITION:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_ATTACK_POSITION; // cf 4827DD
	case AOE_CONST_INTERNAL::CST_IUC_SHOW_SCORES:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_TRADE_FOOD_FOR_GOLD:
		return 0x57; // cf 482AB7
	case AOE_CONST_INTERNAL::CST_IUC_TRADE_WOOD_FOR_GOLD:
		return 0x56;
	case AOE_CONST_INTERNAL::CST_IUC_TRADE_STONE_FOR_GOLD:
		return 0x58;
	case AOE_CONST_INTERNAL::CST_IUC_HEAL:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_HEAL; // cf 4828E9
	case AOE_CONST_INTERNAL::CST_IUC_CONVERT:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_CONVERT;
	case AOE_CONST_INTERNAL::CST_IUC_ATTACK:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_REPAIR:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_REPAIR;
	case AOE_CONST_INTERNAL::CST_IUC_ADD_TO_QUEUE:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_DROP_FROM_QUEUE:
		break;
	case AOE_CONST_INTERNAL::CST_IUC_DO_RESEARCH:
	case AOE_CONST_INTERNAL::CST_IUC_DO_TRAIN:
	case AOE_CONST_INTERNAL::CST_IUC_DO_BUILD:
		// Those ones do not have a specific icon id, it depends on unit/research...
		break;
	case AOE_CONST_INTERNAL::CST_IUC_CROR_DONT_ATTACK_VILLAGERS:
		return AOE_CONST_DRS::RockNRorIconId::CR_ICON_DONT_ATTACK_VILLAGERS;
	case AOE_CONST_INTERNAL::CST_IUC_CROR_DONT_ATTACK_BUILDINGS:
		return AOE_CONST_DRS::RockNRorIconId::CR_ICON_DONT_ATTACK_BUILDINGS;
	case AOE_CONST_INTERNAL::CST_IUC_CROR_NO_AUTO_ATTACK:
		return AOE_CONST_DRS::RockNRorIconId::CR_ICON_DONT_AUTO_ATTACK;
	case AOE_CONST_INTERNAL::CST_IUC_CROR_RESET_AUTO_ATTACK:
		return AOE_CONST_DRS::RockNRorIconId::CR_ICON_RESTORE_AUTO_ATTACK;
	case AOE_CONST_INTERNAL::CST_IUC_CROR_DEFEND:
		//return AOE_CONST_DRS::RockNRorIconId::CR_ICON_DEFEND_UNIT_OR_POSITION; // TODO
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_UNKNOWN_BLUE_FLAG;
	case AOE_CONST_INTERNAL::CST_IUC_CROR_DEFEND_STOP:
		return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_HAND_STOP;
	default:
		break;
	}
	return AOE_CONST_DRS::AoeButtonBarCommonCommandsIconId::BBC_ICON_INVALID_NONE; // unknown or not applicable
}


} // BUTTONBAR_CONST



// Returns true if the button is visible. Use this overload for performance if you already have STRUCT_UI_IN_GAME_MAIN pointer.
// Returns false if the button is hidden, or if an error occurs.
bool IsInGameUnitCommandButtonVisible(AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *gameMainUI, long int buttonIndex) {
	assert(gameMainUI && gameMainUI->IsCheckSumValid());
	if (!gameMainUI || !gameMainUI->IsCheckSumValid()) {
		return false;
	}
	if ((buttonIndex < 0) || (buttonIndex >= 12)) {
		assert(false); // Should never happen
		return false;
	}
	return (gameMainUI->unitCommandButtons[buttonIndex] != NULL) &&
		(gameMainUI->unitCommandButtons[buttonIndex]->visible != 0);
}
// Returns true if the button is visible
// Returns false if the button is hidden, or if an error occurs.
bool IsInGameUnitCommandButtonVisible(long int buttonIndex) {
	AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *inGameMain = (AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *) AOE_METHODS::GetScreenFromName(gameScreenName);
	if (!inGameMain || !inGameMain->IsCheckSumValid() || !inGameMain->visible) {
		return false;
	}
	if (inGameMain != (void*)AOE_METHODS::GetCurrentScreen()) {
		return false;
	}
	return IsInGameUnitCommandButtonVisible(inGameMain, buttonIndex);
}


// To be used with button IDs from unit defintion/researches to get a buttonIndex for game main UI structure (command buttons)
// WARNING: returns -1 if DATButtonId is 0 or negative (invalid)
// Valid results are 0-4 or 6-10.
// See also 0x483710 (only works for 2 pages)
long int GetButtonInternalIndexFromDatBtnId(char DATButtonId) {
	if (DATButtonId <= 0) { return -1; } // Invalid (including 0).
	long int tmp = ((DATButtonId - 1) % 10) + 1; // 1-5 is same as 11-15 / 21-25, etc. 6-10 = same as 16-20, etc
	if (tmp <= 5) {
		return tmp - 1; // Source 1-5 = index 0-4
	}
	if (tmp <= 10) {
		return tmp; // Source 6-10 = index 6-10
	}
	return -1; // Should never happen !
}


}
