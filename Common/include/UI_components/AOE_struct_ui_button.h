
#pragma once

#include <UI_components\AOE_struct_any_ui.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size 0x2B8. Also for checkboxes.
	// Can be created in 0x0456240
	// 0x45FCD0 = OnButtonDown?(arg1, mousePosX, mousePosY, arg4, arg5)
#define CHECKSUM_UI_BUTTON 0x00544E30
#define CHECKSUM_UI_BUTTON_WITH_NUMBER 0x00549CAC
#define CHECKSUM_UI_BUTTON_COMBOBOX 0x00545134
	class STRUCT_UI_BUTTON : public STRUCT_ANY_UI {
	public: // 30 4E 54 00 (a child class is AC 9C 54 00(size=0x3D4), rarely used. 16 are created in game screen=unit-commands)
		long int commandIDs[9]; // +F4. Only index 0 is really used ?? Various types (enums) (GAME_SCREEN_BUTTON_IDS, INGAME_UI_COMMAND_ID, etc)
		long int buttonInfoValue[9]; // +118. For example, a DATID.
		unsigned long int unknown_13C;
		long int unknown_140; // 3 for standard buttons, 2 or 6 for checkboxes/combobox-buttons ?
		unsigned long int unknown_144;
		unsigned long int* unknown_148_wav; // +148: sound button.wav info? (always 0 except when playing sound ?)
		unsigned long int* unknown_14C[9]; // Pointers to icons data?
		short int unknown_170[9]; // +170. Default -1. icon ids ?
		char unknown_182[0x198 - 0x182];
		char* unknown_198[9];
		char* unknown_1BC[9];
		char unknown_1E0[0x1F4 - 0x1E0];
		short int unknown_1F4;
		short int checked; // +1F6. To update value, see AOE_CheckBox_SetChecked. Index for commandIDs and buttonInfoValue. For button, =1 when has focus (text color is highlighted)
		unsigned long int unknown_1F8;
		STRUCT_ANY_UI **unknown_1FC; // ptr array groupedObjects ? really unsure
		// 0x200
		short int unknown_200; // groupedObjectCount ? really unsure (like radiobuttons?)
		short int unknown_202;
		long int timeGetTimeValue_204; // +204
		unsigned long int unknown_208[9]; // +208. default 0x00FFFFFF
		unsigned long int unknown_22C[9];
		long int unknown_250[9]; // +250. default 0xFFFF
		unsigned long int unknown_274[9];
		long int hotkey; // +298. Ascii code of hotkey: 65='A', etc.
		long int unknown_29C; // +29C. Related to hotkey ? Not sure.
		long int unknown_2A0;
		char unknown_2A4;
		char unknown_2A5;
		char unknown_2A6;
		char unknown_2A7;
		char unknown_2A8;
		char unknown_2A9;
		char unknown_2AA_unused; // +2AA : unused ?
		char unknown_2AB_unused; // +2AB : unused ?
		char unknown_2AD_unused[4]; // +2AC unsure
		long int readOnly; // +2B0. 45F3A0 to modify ?
		long int unknown_2B4; // +2B4. 4606D0 = setter ?

		bool IsChecked() const {
			return this->checked != 0;
		}
		bool IsCheckSumValid() const {
			return (this->checksum == CHECKSUM_UI_BUTTON);
		}
		bool IsCheckSumValidForAChildClass() const {
			return (this->checksum == CHECKSUM_UI_BUTTON_WITH_NUMBER) || (this->checksum == CHECKSUM_UI_BUTTON) ||
				(this->checksum == CHECKSUM_UI_BUTTON_COMBOBOX);
		}
		bool IsACheckBox() const { return this->unknown_140 != 3; } // Absolutely unsure, but I've got nothing else at this point
	};
	static_assert(sizeof(STRUCT_UI_BUTTON) == 0x2B8, "STRUCT_UI_BUTTON size");

}

