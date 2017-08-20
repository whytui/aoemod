
#pragma once

#include <macroUtil.h>
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
	// 0x45EDC0 = button.setup(renderArea, parent, x, y, sizeX, sizeY, drawType, sound, notifyType, btnId)
	// 0x45FCD0 = button.onButtonDown(mouseBtnId, mouseposX, mouseposY, CTRL, SHIFT)
#define CHECKSUM_UI_BUTTON_PANEL 0x00544E30 // parent button class
#define CHECKSUM_UI_BUTTON_WITH_NUMBER 0x00549CAC
#define CHECKSUM_UI_DROPDOWN_BUTTON 0x00545134 // button for dropdown
	class STRUCT_UI_BUTTON : public STRUCT_ANY_UI {
	public: // 30 4E 54 00 (a child class is AC 9C 54 00(size=0x3D4), rarely used. 16 are created in game screen=unit-commands)

		enum BUTTON_TYPE : long int {
			BT_DEFAULT = 1, // in constructor ?
			BT_RADIO = 2, // cf 0x45EEAB
			BT_CHECKBOX = 3, // cf 0x45EF20
			BT_UNKNOWN_5 = 5
		};

		long int commandIDs[9]; // +F4. "id1" = a free value. Index="state". Various types (enums) (GAME_SCREEN_BUTTON_IDS, INGAME_UI_COMMAND_ID, etc)
		long int buttonInfoValue[9]; // +118. "id2" = a free value. Index="state". For example, a DATID. Free values.
		BUTTON_TYPE buttonType; // +13C.
		long int drawType; // +140. 3 for standard buttons, 2 or 6 for checkboxes/combobox-buttons ?
		long int notifyTypeId; // +144.
		unsigned long int* buttonSound; // +148. NULL if not playing a sound. "TDigital".
		unsigned long int* pictureByState[9]; // +14C. Picture for each "state".
		short int iconIdByState[9]; // +170. Default -1. icon ids.
		char unknown_182[2];
		unsigned long int unknown_184_pictureInfo;
		unsigned long int unknown_188_pictureInfo;
		unsigned long int unknown_18C_pictureInfo;
		unsigned long int unknown_190_pictureInfo;
		unsigned long int unknown_194;
		char* pText1ByState[9]; // +198
		char* pText2ByState[9]; // +1BC.
		long int textPosX; // +1E0.
		long int textPosY; // +1E4.
		unsigned long int font; // +1E8. type ?
		long int fontWidth;
		long int fontHeight; // +1F0
		short int numberOfStates; // +1F4. For checkbox (type 3), see 0x45EF20.
		short int currentState; // +1F6. value 0-8. To update value, see AOE_CheckBox_SetChecked. Index for commandIDs and buttonInfoValue. For button, =1 when has focus (text color is highlighted)
		long int unknown_1F8_isRadioButton; // +1F8. Set to 1 for drawTypes 3&5 in 0x45F2D1, when set as a radiobutton and grouped (?)
		STRUCT_ANY_UI **groupedObjects; // +1FC. For radioButtons
		short int groupedObjectCount; // +200. For radioButtons
		short int unknown_202;
		long int timeGetTimeValue_204; // +204
		unsigned long int textColor[9]; // +208. For text1? RGB. 4th byte is always 0. Taken as char[4], x[0]=red, x[1]=green, x[2]=blue. 000000=black
		unsigned long int textShadowColor[9]; // +22C. For text2? RGB. 4th byte is always 0. Taken as char[4], x[0]=red, x[1]=green, x[2]=blue. 000000=black
		long int unknown_250[9]; // +250. default 0xFFFF
		unsigned long int unknown_274[9];
		long int hotkey; // +298. Ascii code of hotkey: 65='A', etc.
		long int unknown_29C; // +29C. Related to hotkey ? Not sure.
		long int unknown_2A0_borderSize; // +2A0. border size ? To confirm
		char borderColorOutTopRight; // +2A4
		char borderColorMidtTopRight; // +2A5
		char borderColorInTopRight; // +2A6
		char borderColorInBottomLeft; // +2A7
		char borderColorMidBottomLeft; // +2A8
		char borderColorOutBottomLeft; // +2A9
		char unknown_2AA_unused; // +2AA : unused ?
		char unknown_2AB_unused; // +2AB : unused ?
		char unknown_2AD_unused[4]; // +2AC unsure
		long int readOnly; // +2B0. 0x45F3A0 to modify. "isDisabled"
		long int soundNumber; // +2B4. 4606D0 = setter ?

		bool IsChecked() const {
			return this->currentState != 0;
		}
		bool IsCheckSumValid() const {
			return (this->checksum == CHECKSUM_UI_BUTTON_PANEL);
		}
		bool IsCheckSumValidForAChildClass() const {
			return (this->checksum == CHECKSUM_UI_BUTTON_WITH_NUMBER) || (this->checksum == CHECKSUM_UI_BUTTON_PANEL) ||
				(this->checksum == CHECKSUM_UI_DROPDOWN_BUTTON);
		}
#pragma TODO("use button type instead ?")
		bool IsACheckBox() const { return this->drawType != 3; } // Absolutely unsure, but I've got nothing else at this point
	};
	static_assert(sizeof(STRUCT_UI_BUTTON) == 0x2B8, "STRUCT_UI_BUTTON size");

}

