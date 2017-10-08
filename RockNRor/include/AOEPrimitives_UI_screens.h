
#pragma once
#include <assert.h>
#include <string>
#include <UI_components\AOE_struct_any_ui.h>
#include <UI\AOE_struct_ui_scenario_editor_main.h>
#include <UI\AOE_struct_ui_in_game_main.h>
#include <UI\AOE_struct_ui_dialog_base.h>
#include <AOE_const_internal.h>
#include <AOE_offsets.h>
#include <AOE_const_drs.h>
#include <AOE_struct_game_settings.h>
#include "mainStructuresHandling.h"
#include "interruption.h"
#include "AOE_memory.h"

#include <mystrings.h>

/*
* This file provides useful (raw) basic methods to deal with AOE UI screens.
* Most of them directly call ROR's methods and are a practical encapsulation to avoid using assembler in our code.
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/


namespace AOE_METHODS {
namespace UI_BASE {
;

using namespace AOE_STRUCTURES;

// Create a popup from game screen (from Options original model)
// Must be called when game screen is active (no other popup)
STRUCT_UI_EASY_PANEL *CreateGameScreenPopup(STRUCT_UI_EASY_PANEL *parent, long int hSize, long int vSize,
	long int backgroundSlpId);



// Return current screen, using 0x5830E8 structure info
STRUCT_UI_EASY_PANEL *GetCurrentScreen();


// Returns a pointer to a UI object that matches screenName.
// Can return NULL if no matching screen was found
STRUCT_UI_EASY_PANEL *GetScreenFromName(const char *screenName);


// Calls 0x451BE0
// Set makeModal=1 for dialogs, not for screens.
void SetCurrentPanel(const char *screenName, unsigned long int makeModal);

// (cf 0x451DF0)
// ROR code first (not always, sometimes after?) calls SetCurrentScreen on the parent or new screen and then CloseScreenAndDestroy.
bool CloseScreenAndDestroy(const char *screenName);


// Close screen+destroy it, and set main screen = destroyed screen's parent (if any)
// Do not use UIObj after calling this !
void CloseScreenFullTreatment(STRUCT_ANY_UI *UIObj);


enum ScreenType { EASY_PANEL, SCREEN_PANEL, DIALOG_PANEL };

// Open a new modal screen (popup or fullscreen)
// parent can be null: if so current screen is used
// pos/size arguments can be 0 if isFullscreen=true, otherwise sizes must be >0.
// backgroundTheme can be -1
// allowShadowArea : unknown effect. unused ?
// ScreenType: Dialog seems to only work well in game screen or scenario editor ?
STRUCT_UI_EASY_PANEL *CreateModalScreen(const char *screenName, STRUCT_UI_EASY_PANEL *parent, bool isFullscreen, long int hPos, long int vPos, long int hSize, long int vSize,
	bool allowShadowArea, AOE_CONST_DRS::AoeScreenTheme backgroundTheme, ScreenType screenType);

}
}
