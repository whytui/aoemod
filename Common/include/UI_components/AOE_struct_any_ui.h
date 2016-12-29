
#pragma once

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	class STRUCT_UI_DRAW_AREA {
	public:
		unsigned long int unknown_00; // A pointer
		long int hWnd;
		unsigned long int unknown_08;
		unsigned long int unknown_0C; // A pointer
		// 0x10
		unsigned long int unknown_10;
		unsigned long int unknown_14;
		long int screenSizeX;
		long int screenSizeY;
	};


#define CHECKSUM_ANY_UI 0x00544AD8
	// Parent class for UI objects (both screens and UI components) = TPanel
	// Size=0xF4 for this class, but all child classes are larger !
	// Constructor: 004523F0 for "screen" objects (arg1=name), 00452580 for components (no arg). Destructor=0x4526B0
	// In general, you should not modify these members directly. You are supposed to use methods (few are implemented in this solution).
	// D8 4A 54 00 for this base (parent) class
	// [EDX+C4] = uiComponent.setFocus(doFocus)
	// 0x451F20 = struct PanelNode * TPanelSystem::findPanelNode(char *panelName)
	class STRUCT_ANY_UI {
	public:
		unsigned long int checksum; // Depends on UI object type
		STRUCT_ANY_UI *previousPanel; // +04.
		STRUCT_ANY_UI *previousPopup; // +08. Really really unsure. Previous modal panel ?
		long int posX; // +0x0C. Absolute posX
		// 0x10
		long int posY; // +x010. Absolute posY
		long int sizeX;
		long int sizeY;
		char *screenName;
		// 0x20
		STRUCT_UI_DRAW_AREA *renderArea; // pointer. +04=hWnd +13C=evtStatus?? +1F6=objectIndex,+F4+objectIndex*4=ButtonId +17C=hObject?
		long int minPosX; // +24. Min X position for this UI object. +24=a pRect object (4 position dwords)
		long int minPosY; // +28
		long int maxPosX; // +2C
		// 0x30
		long int maxPosY; // +30
		unsigned long int unknown_034;
		unsigned long int unknown_038; // init = 2
		STRUCT_ANY_UI *focusedComponent; // +3C. Underlying (child) focused component. A parent AND its child can both have focus (?)
		// 0x40
		STRUCT_ANY_UI *ptrParentObject; // +40
		unsigned long int unknown_044;
		unsigned long int unknown_048;
		unsigned long int unknown_04C;
		// 0x50
		unsigned long int unknown_050;
		unsigned long int *unknown_054; // ptr struct size 0x0C. +0=backptr,
		unsigned long int *unknown_058; // ptr to some list ? +8=next
		unsigned long int unknown_05C; // A ptr to a list: elem= {+0=STRUCT_ANY_UI*, +4=nextElem}. Used in 453360
		// 0x60
		unsigned long int unknown_060;
		unsigned long int unknown_064;
		unsigned long int unknown_068; // 1 when being clicked ???
		long int visible; // +6C. For a dialog it is 0 ? Do not edit directly.
		// 0x70
		unsigned long int unknown_070;
		unsigned long int editable; // TO CONFIRM !! seen for text components. What about others ? For button, it is text color when "active"?
		unsigned long int isFocused; // +78. 1 if control is focused, 0 otherwise. See also +3C. This does not prevent another *child* component from having focus too.
		unsigned long int unknown_07C; // byte ?
		// 0x80
		unsigned long int unknown_080; // Related to "!readonly" ??? But it is NOT "enabled"
		unsigned long int unknown_084; // type ?
		unsigned long int unknown_088;
		long int unknown_08C_minPosX; // MinPosX ? similar to +24 ?
		// 0x90
		long int unknown_090_minPosY; // minPosY ? similar to +28 ? Example 0x14 (size of top banner, with resources, current age, menu)
		long int unknown_094_maxPosX; // maxPosX ? similar to +2C
		long int unknown_098_maxPosY; // maxPosY ? similar to +30
		unsigned long int unknown_09C; // similar to +24, +8C ?
		// 0xA0
		unsigned long int unknown_0A0; // similar to +28, +90 ?
		unsigned long int unknown_0A4;
		unsigned long int unknown_0A8;
		unsigned long int unknown_0AC; // similar to +14 ? (X pos or size ?)
		// 0xB0
		unsigned long int unknown_0B0; // similar to +14 ? (X pos or size ?)
		unsigned long int unknown_0B4; // similar to +18 ? (Y pos or size ?)
		unsigned long int unknown_0B8; // similar to +18 ? (Y pos or size ?)
		unsigned long int unknown_0BC; // init=0xFA ?
		// 0xC0
		unsigned long int unknown_0C0;
		unsigned long int unknown_0C4;
		unsigned long int unknown_0C8;
		unsigned long int unknown_0CC;
		// 0xD0
		unsigned long int unknown_0D0;
		long int timeGetTimeValue; // +D4
		long int unknown_0D8; // a flag ?
		unsigned long int unknown_0DC;
		// 0xE0
		unsigned long int unknown_0E0;
		long int helpDllId; // +E4. Dll string Id for help (or tooltip?) ?
		long int winHelpDataDllId; // +E8. a DllID, used when clicking on help/(object)/details = data for winHelp call.
		char unknown_0EC; // a "status" for event handling ? 1, 2... 1 for left click, 2 for right-click ??
		char unknown_0ED; // a "status" for event handling ? 1, 2...
		char unknown_0EE;
		char unknown_0EF;
		// 0xF0
		char unknown_0F0;
		char unknown_0F1; // is this used ?
		char unknown_0F2;
		char unknown_0F3;
		// 0xF4 and after: different according to 2 main child (intermediate) classes: UI components and UI screens ?
		// Screens: F4, F8 are dwords (some X/Y size/pos). classes Size = 0x478/0x490 ? See 454B60/460730
		// Components: F4 is 2 words...?

		long int GetHWnd() {
			if (!this->renderArea) { return 0; }
			return this->renderArea->hWnd;
		}
	};
	static_assert(sizeof(STRUCT_ANY_UI) == 0xF4, "STRUCT_ANY_UI size");

}

