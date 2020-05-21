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
		PUSH 0; // arg2 = "nonUserPause"
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
	global->showScores = show ? 1 : 0;
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


// Save current game into a gmx savegame file. filenameNoPath is filename with extension, without path.
// Returns true if successful
// Does not display any specific UI, does not disable input, does not unpause, etc. Just save game !
bool SaveCurrentGame(const char *filenameNoPath) {
	// Refer to 0x501250=gameSettings.saveGameToFile(filename_nopath) to find whole treatment (including disable input...)
	if (!filenameNoPath) { return false; } // Null pointer
	if (filenameNoPath[0] == '\0') { return false; } // Empty string
	STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) {
		return false;
	}
	if (settings->currentUIStatus < GAME_SETTINGS_UI_STATUS::GSUS_PLAYING) {
		return false; // Statuses 4 5 6 7 are allowed cf 0x50125A.
	}
	STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) {
		return false;
	}
	long int asmResult = 0;
	_asm{
		MOV ECX, global;
		MOV EDX, [ECX];
		PUSH filenameNoPath;
		CALL [EDX + 0xD8];
		AND EAX, 0xFF; // Result is 1-byte. 1 on success.
		MOV asmResult, EAX;
	}
	return (asmResult != 0);
}


// Automatically save current game to a file (hardcoded filename). Can be used in case of error, for example.
bool AutoSaveCurrentGame() {
	const char saveFilename[] = "_autosave-RockNRor.gmx";
	bool res = AOE_METHODS::SaveCurrentGame(saveFilename);
	std::string msg = "Save game to ";
	msg += saveFilename;
	msg += ": ";
	msg += (res ? "success" : "failure");
	//traceMessageHandler.WriteMessageNoNotification(msg);
	return res;
}


// Restart current game, just like Restart button does
bool RestartGame() {
	AOE_STRUCTURES::STRUCT_GAME_SETTINGS *settings = GetGameSettingsPtr();
	if (!settings || !settings->IsCheckSumValid()) { return false; }
#ifdef GAMEVERSION_ROR10c
	const unsigned long int addrRestart = 0x00500FB0;
	_asm {
		MOV ECX, settings;
		CALL addrRestart;
	}
	return true;
#endif
	return false;
}


// Save current scenario into a scn or scx scenario file. filenameNoPath is filename with extension, without path.
// Returns true if successful
// If you call this from in-game, you won't have a crash, but the scenario file will be corrupted and opening it may crash.
bool SaveScenario(const char *filenameNoPath) {
	AOE_STRUCTURES::STRUCT_GAME_GLOBAL *global = GetGameGlobalStructPtr();
	if (!global || !global->IsCheckSumValid()) { return false; }
	//assert(global->gameRunStatus == 2); // not ingame
	char res = 0;
	_asm {
		MOV ECX, global;
		MOV EDX, [ECX];
		PUSH filenameNoPath;
		CALL[EDX + 0xE4];
		MOV res, AL;
	}
	return (res != 0);
}


// Get a string representing game time in a human readable format
std::string GetHumanTimeFromGameTime(unsigned long int gameTimeValue, bool withMilliSeconds) {
	std::string result = "";
	unsigned long int remaining = gameTimeValue / 1000; // Ignore milliseconds
	unsigned long int hours = remaining / 3600;
	remaining = remaining % 3600;
	unsigned long int minutes = remaining / 60;
	unsigned long int seconds = remaining % 60;
	char bufTime[32] = "";
	if (hours > 0) {
		if (withMilliSeconds) {
			sprintf_s(bufTime, "%02ld:%02ld:%02ld.%03ld", hours, minutes, seconds, gameTimeValue % 1000);
		} else {
			sprintf_s(bufTime, "%02ld:%02ld:%02ld", hours, minutes, seconds);
		}
		result = std::to_string(hours);
		result += ":";
	} else {
		if (withMilliSeconds) {
			sprintf_s(bufTime, "%02ld:%02ld.%03ld", minutes, seconds, gameTimeValue % 1000);
		} else {
			sprintf_s(bufTime, "%02ld:%02ld", minutes, seconds);
		}
	}
	return std::string(bufTime);
	result += std::to_string(minutes);
	result += ":";
	result += std::to_string(seconds);
	if (withMilliSeconds) {
		result += ".";
		result += std::to_string(gameTimeValue % 1000);
	}
	return result;
}

}



namespace ROCKNROR {
	namespace SYSTEM {

		// allowEscToContinue = if true, the user can press ESCAPE to resume program execution
		// tryConnectDebugger = if true, set a breakpoint (and resume execution) when a debugger is attached
		// In RELEASE mode, we can wait for debugger, but we can't break execution (user needs to set a breakpoint manually)
		// If in-game, it might be safer to call AOE_METHODS::SetGamePause(true) prior to this.
		void SaveGameAndStopExecution(const wchar_t *message, bool allowEscToContinue, bool tryConnectDebugger) {
			AOE_METHODS::AutoSaveCurrentGame();
			ROCKNROR::SYSTEM::StopExecution(message, allowEscToContinue, tryConnectDebugger);
		}

	}
}
