#include "../include/AOEPrimitives_global.h"

namespace AOE_METHODS {
;


// ROR's method to get a pseudo-random value
long int GetAndReCalcPseudoRandomValue() {
	long int result;
	_asm {
		MOV EDX, 0x52605D;
		CALL EDX; // Recalculate pseudo random
		MOV result, EAX;
	}
	return result;
}


// Pause/unpause the game
void SetGamePause(bool pauseOn) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (settings == NULL) { return; }
	if (settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) { return; } // Call should be robust enough, but we still check this
	long int argPause = pauseOn ? 1 : 0;
	_asm {
		PUSH 0; // arg2 - what is this ?
		PUSH argPause;
		MOV EAX, 0x0419A60;
		MOV ECX, settings;
		CALL EAX;
	}
}


// Show/Hide scores (F4 key)
void ShowScores(bool show) {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (settings == NULL) { return; }
	if ((settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) &&
		(settings->currentUIStatus != AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS::GSUS_GAME_OVER_BUT_STILL_IN_GAME)) { return; }
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = settings->ptrGlobalStruct;
	if (!global || !global->IsCheckSumValid()) { return; }

#ifdef GAMEVERSION_ROR10c
	long int argShow = show ? 1 : 0;
	STRUCT_UI_IN_GAME_MAIN *gameMainUi = settings->ptrGameUIStruct;
	if (gameMainUi && gameMainUi->IsCheckSumValid()) {
		_asm {
			PUSH argShow;
			MOV EAX, 0x484F90;
			MOV ECX, gameMainUi;
			CALL EAX;
		}
	}
#else
	global->showScores = argShow;
#endif
}


// Get a localized string using ROR method.
// Returns true on success.
bool ReadLanguageTextForCategory(INTERNAL_MAIN_CATEGORIES category, long int commandId, long int subParam, char *buffer, long int bufferSize) {
	if (!buffer || (bufferSize <= 0)) {
		return false;
	}
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) {
		return false;
	}
	long int result = 0;
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	const unsigned long int addr = 0x4FF580;
	_asm {
		PUSH bufferSize;
		PUSH buffer;
		PUSH subParam;
		PUSH commandId;
		PUSH category;
		MOV ECX, settings;
		CALL addr;
		MOV result, EAX;
	}
	return (result != 0);
}


// Call AOE's Notify event method. Warning, the parameters can have different types.
// Use the overload with pointers to make sure you don't have cast issues.
// arg3 = unitDefId (depends on event type...)
// arg4 = posY (depends on event type...)
// arg5 = posX (depends on event type...)
void CallGameSettingsNotifyEvent(long int eventId, long int playerId, long int variant_arg3, long int variant_arg4, long int variant_arg5){
	const unsigned long int gameSettingsNotifyEvent = 0x501980;
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return; }
	_asm {
		MOV ECX, settings;
		PUSH variant_arg5;
		PUSH variant_arg4;
		PUSH variant_arg3;
		PUSH playerId;
		PUSH eventId;
		MOV EAX, gameSettingsNotifyEvent;
		CALL EAX;
	}
}


}
