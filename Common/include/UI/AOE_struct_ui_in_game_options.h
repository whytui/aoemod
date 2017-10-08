
#pragma once

#include <UI\AOE_struct_ui_dialog_base.h>
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

	// Size=0x564. In-game Options popup. Parents 1C 4F 54 00, BC 4B 54 00(easyPanel), D8 A4 54 00 (any_ui = base class)
	// Constructor = 0x4300D0
	class STRUCT_UI_IN_GAME_OPTIONS : public STRUCT_UI_DIALOG_BASE {
	public:
		// 0x490
		unsigned long int unknown_490; // Init with constructor's arg1
		STRUCT_ANY_UI *lblTitle;
		STRUCT_ANY_UI *lblSpeed;
		STRUCT_ANY_UI *lblNormalSpeed;
		// 0x4A0
		STRUCT_ANY_UI *lblFastSpeed;
		STRUCT_ANY_UI *lblVeryFastSpeed;
		STRUCT_ANY_UI *edtNormalSpeed; // checkbox
		STRUCT_ANY_UI *edtFastSpeed;
		// 0x4B0
		STRUCT_ANY_UI *edtVeryFastSpeed;
		STRUCT_ANY_UI *lblMusicVolume;
		STRUCT_UI_BUTTON *btnMusicVolumeMore; // ?
		STRUCT_UI_BUTTON *btnMusicVolumeLess;
		// 0x4C0
		STRUCT_UI_BUTTON *btnMusicVolumeSelector; // scroller
		STRUCT_ANY_UI *lblSoundVolume;
		STRUCT_UI_BUTTON *btnSoundVolumeMore;
		STRUCT_UI_BUTTON *btnSoundVolumeLess;
		// 0x4D0
		STRUCT_UI_BUTTON *btnSoundVolumeSelector; // scroller
		STRUCT_ANY_UI *lblScrollSpeed;
		STRUCT_UI_BUTTON *btnScrollSpeedFaster;
		STRUCT_UI_BUTTON *btnScrollSpeedSlower;
		// 0x4E0
		STRUCT_UI_BUTTON *btnScrollSpeedSelector; // scroller
		STRUCT_ANY_UI *lblScreenSize;
		STRUCT_ANY_UI *lblScreenSize1; // 640*480
		STRUCT_ANY_UI *lblScreenSize2; // 800*600
		// 0x4F0
		STRUCT_ANY_UI *lblScreenSize3; // 1024*768
		STRUCT_ANY_UI *edtScreenSize1; // checkbox
		STRUCT_ANY_UI *edtScreenSize2;
		STRUCT_ANY_UI *edtScreenSize3;
		// 0x500
		STRUCT_ANY_UI *lblMouseInterface;
		STRUCT_ANY_UI *lblTwoButtons;
		STRUCT_ANY_UI *lblOneButton;
		STRUCT_ANY_UI *edtTwoButtons;
		// 0x510
		STRUCT_ANY_UI *edtOneButton;
		STRUCT_ANY_UI *lblContextHelp;
		STRUCT_ANY_UI *lblContextHelpOn;
		STRUCT_ANY_UI *lblContextHelpOff;
		// 0x520
		STRUCT_ANY_UI *edtContextHelpOn;
		STRUCT_ANY_UI *edtContextHelpOff;
		STRUCT_ANY_UI *lblPathFinding;
		STRUCT_ANY_UI *lblPathFindingDefault;
		// 0x530
		STRUCT_ANY_UI *lblPathFindingMedium;
		STRUCT_ANY_UI *lblPathFindingHigh;
		STRUCT_ANY_UI *edtPathFindingDefault;
		STRUCT_ANY_UI *edtPathFindingMedium;
		// 0x540
		STRUCT_ANY_UI *edtPathFindingHigh;
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnCancel;
		long int musicInitialVolume;
		// 0x550
		long int soundInitialVolume;
		unsigned long int unknown_554;
		unsigned long int unknown_558;
		long int musicVolumeLevelValue; // values 0-99 (99=lowest)
		// 0x560
		long int soundVolumeLevelValue; // values 0-99 (99=lowest)
		// 0x564: end
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_IN_GAME_OPTIONS; }
	};
	static_assert(sizeof(STRUCT_UI_IN_GAME_OPTIONS) == 0x564, "STRUCT_UI_IN_GAME_OPTIONS size");
}

