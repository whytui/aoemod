
#pragma once

#include <assert.h>
#include <string>
#include <AOE_const_internal.h>
#include <AOE_struct_game_settings.h>
#include <UI\AOE_struct_ui_in_game_main.h>
#include "mainStructuresHandling.h"
#include "interruption.h"


/* This file contains AOE primitives about game commands handling
 * Game commands are async small commands that are sent to host player (if MP game)
 * so that game behaviour/actions/changes remain synchronous on all playing computer.
 * Using those 'game commands' are the only way to guarantee sync safety in MP games when running custom treatments on the game.
*/

using namespace AOE_STRUCTURES;
using namespace AOE_CONST_INTERNAL;

namespace AOE_METHODS {
;

// ROR's method to get a pseudo-random value
long int GetAndReCalcPseudoRandomValue();


// Pause/unpause the game
void SetGamePause(bool pauseOn);


// Show/Hide scores (F4 key)
void ShowScores(bool show);


// Get a localized string using ROR method.
// Returns true on success.
bool ReadLanguageTextForCategory(INTERNAL_MAIN_CATEGORIES category, long int commandId, long int subParam, char *buffer, long int bufferSize);


// Call AOE's Notify event method. Warning, the parameters can have different types.
// Use the overload with pointers to make sure you don't have cast issues.
// arg3 = unitDefId (depends on event type...)
// arg4 = posY (depends on event type...)
// arg5 = posX (depends on event type...)
void CallGameSettingsNotifyEvent(long int eventId, long int playerId, long int variant_arg3, long int variant_arg4, long int variant_arg5);


// Save current game into a gmx savegame file. filenameNoPath is filename with extension, without path.
// Returns true if successful
// Does not display any specific UI, does not disable input, does not unpause, etc. Just save game !
bool SaveCurrentGame(const char *filenameNoPath);


// Automatically save current game to a file (hardcoded filename). Can be used in case of error, for example.
bool AutoSaveCurrentGame();

}


namespace ROCKNROR {
	namespace SYSTEM {
		// allowEscToContinue = if true, the user can press ESCAPE to resume program execution
		// tryConnectDebugger = if true, set a breakpoint (and resume execution) when a debugger is attached
		// In RELEASE mode, we can wait for debugger, but we can't break execution (user needs to set a breakpoint manually)
		// If in-game, it might be safer to call AOE_METHODS::SetGamePause(true) prior to this.
		void SaveGameAndStopExecution(const wchar_t *message, bool allowEscToContinue, bool tryConnectDebugger);

	}
}
