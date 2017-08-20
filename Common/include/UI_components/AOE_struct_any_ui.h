
#pragma once
#include <AOE_struct_color_def.h>

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


// TPanel checksum (base class)
#define CHECKSUM_ANY_UI 0x00544AD8

// Other classes to define
#define CHECKSUM_UI_LOAD_SAVED_GAME 0x005468CC // ccor 0x485FC90
#define CHECKSUM_UI_MAIN_ERROR_SCREEN 0x00546B24 // ccor 0x486FD0
#define CHECKSUM_UI_MP_STARTUP_SCREEN 0x00546C50 // ccor 0x487250
#define CHECKSUM_UI_MP_WAIT_SCREEN 0x00546EA8 // ccor 0x48DA30
#define CHECKSUM_UI_NAME_SELECTION 0x00546FD4 // ccor 0x48DEC0
#define CHECKSUM_UI_NAME_DIALOG 0x00547100 // ccor 0x48E800
#define CHECKSUM_UI_SAVE_AS_SCREEN 0x00547234 // ccor 0x48ECA0. "Save Game Screen" (to save scenario also)
#define CHECKSUM_UI_SC_EDITOR_OPEN 0x005475B8 // ccor 0x49AAA0. "Scenario editor open"
#define CHECKSUM_UI_SELECT_SCENARIO_SCREEN 0x005476E4 // ccor 0x49B360. "Select scenario screen"
#define CHECKSUM_UI_SP_MENU_SCREEN 0x0054793C // ccor 0x49E040. "SPMenuScreen"
#define CHECKSUM_UI_STATUS_MESSAGE 0x00547A68 // ccor 0x49E7A0, 0x49E9A0.
#define CHECKSUM_UI_MISSION_SCREEN 0x00547B94 // ccor 0x49EC00.


	// Parent class for UI objects (both screens and UI components) = TPanel
	// Size=0xF4 for this class, but all child classes are larger !
	// Constructor: 004523F0 for "screen" objects (arg1=name), 00452580 for components (no arg). Destructor=0x4526B0
	// 0x452750=Panel.setup(renderArea, parent, x, y, width, height, color)
	// In general, you should not modify these members directly. You are supposed to use methods (few are implemented in this solution).
	// D8 4A 54 00 for this base (parent) class
	// +00 = uiComponent.destructor(do_free)
	// +04 = long TPanel::setup(TDrawArea *,TPanel *,long,long,long,long,unsigned char)
	// +08 = void TPanel::set_rect(minX, minY, sizeX, sizeY)
	// +0C = void RGE_View::set_rect(long,long,long,long)
	// +10 = void TPanel::set_color(unsigned char)
	// +14 = void TPanel::set_active(bool)
	// +18 = void TPanel::set_positioning(enum TPanel::PositionMode,long,long,long,long,long,long,long,long,TPanel *,TPanel *,TPanel *,TPanel *)
	// +1C = void TPanel::set_fixed_position(hPos, vPos, hSize, vSize)
	// +20 = anyUI.setRedraw(redrawMode) = refresh
	// +24 = void TPanel::set_overlapped_redraw(TPanel *,TPanel *,enum TPanel::RedrawMode) ?
	// +28 = void TPanel::draw_setup(int) ?
	// +2C = void TPanel::draw_finish(void)
	// +30 = void TPanel::draw(void)
	// +34 = void TPanel::draw_rect(struct tagRECT *) ?
	// +B8 = OnEvent(ptrSender, evtStatus, objId, btnInfoValue) ?? really unsure
	// +C4 = uiComponent.setFocus(doFocus)
	// 0x451F20 = struct PanelNode * TPanelSystem::findPanelNode(char *panelName)
	class STRUCT_ANY_UI {
	public:

		enum OBJECT_TYPE : char {
			OT_PANEL_BASE = 0,
			OT_TEXT_PANEL = 1,
			OT_UNKNOWN_2 = 2, // 0x5455D4
			OT_BUTTON = 3,
			OT_LISTBOX = 4,
			OT_SCROLLBAR = 5,
			OT_DROPDOWN = 6,
			OT_UNKNOWN_7 = 7 // 00545304
		};

		unsigned long int checksum; // Depends on UI object type
		STRUCT_ANY_UI *previousPanel; // +04.
		STRUCT_ANY_UI *previousPopup; // +08. "Previous modal panel".
		long int posX; // +0x0C. Absolute posX
		// 0x10
		long int posY; // +0x10. Absolute posY
		long int sizeX; // +0x14. Width
		long int sizeY; // +0x18. Height
		char *screenName; // +0x1C. "PanelName"
		// 0x20
		STRUCT_UI_DRAW_AREA *renderArea; // pointer. +04=hWnd +13C=evtStatus?? +1F6=objectIndex,+F4+objectIndex*4=ButtonId +17C=hObject?
		long int minPosX; // +24. Min X position for this UI object. +24=a pRect object (4 position dwords) or "tagRECT".
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
		long int trueRenderRect_minPosX; // +8C. MinPosX ? similar to +24 ?
		// 0x90
		long int trueRenderRect_minPosY; // +90. minPosY ? similar to +28 ? Example 0x14 (size of top banner, with resources, current age, menu)
		long int trueRenderRect_maxPosX; // maxPosX ? similar to +2C
		long int trueRenderRect_maxPosY; // +98. maxPosY ? similar to +30
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
		long int unknown_0D8; // a flag ? Error_code ?
		unsigned long int unknown_0DC;
		// 0xE0
		unsigned long int unknown_0E0;
		long int helpDllId; // +E4. Dll string Id for help (or tooltip?) = help message.
		long int winHelpDataDllId; // +E8. a DllID, used when clicking on help/(object)/details = data for winHelp call.
		char unknown_0EC; // a "status" for event handling ? 1, 2... 1 for left click, 2 for right-click ??
		char unknown_0ED; // a "status" for event handling ? 1, 2...
		OBJECT_TYPE baseObjectType; // +EE. Set in constructors.
		char unknown_0EF;
		// 0xF0
		char unknown_0F0;
		char unknown_0F1; // is this used ?
		char unknown_0F2;
		char unknown_0F3;
		// 0xF4 and after: different according to 2 main child (intermediate) classes: UI components and UI screens ?
		// Screens: F4, F8 are dwords (some X/Y size/pos). classes Size = 0x478/0x490 ? See 454B60/460730
		// Components: F4 is 2 words...?

		long int GetHWnd() const {
			if (!this->renderArea) { return 0; }
			return this->renderArea->hWnd;
		}
	};
	static_assert(sizeof(STRUCT_ANY_UI) == 0xF4, "STRUCT_ANY_UI size");


#define CHECKSUM_UI_PICTURE_PANEL 0x005455D4
	// Constructor=0x466950. Size=0x11C
	class STRUCT_UI_PICTURE_PANEL : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4;
		unsigned long int unknown_0F8;
		unsigned long int unknown_0FC;
		long int resInfoId; // +100
		unsigned long int unknown_104;
		unsigned long int unknown_108;
		unsigned long int unknown_10C;
		unsigned long int unknown_110;
		unsigned long int *unknown_114; // +114: TPicture
		unsigned long int *picture; // +118
		
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_PICTURE_PANEL; }
	};
	static_assert(sizeof(STRUCT_UI_PICTURE_PANEL) == 0x11C, "STRUCT_UI_PICTURE_PANEL size");


#define CHECKSUM_UI_PANEL_TOOL_BOX 0x00545D50
	// "PanelToolBox". Constructor=0x473D70. Size=0x120.
	// Part of scenario editor ?
	class STRUCT_UI_PANEL_TOOL_BOX : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x120 - 0xF4];
		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_PANEL_TOOL_BOX; }
	};
	static_assert(sizeof(STRUCT_UI_PANEL_TOOL_BOX) == 0x120, "STRUCT_UI_PANEL_TOOL_BOX size");

}

