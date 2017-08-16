
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <AOE_structures_drs.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{


// TEasyPanel checksum (screens base class ?)
#define CHECKSUM_UI_EASY_PANEL 0x00544BBC
#define CHECKSUM_UI_SCREEN_PANEL 0x0054579C // Some mysterious intermediate (child) class with no own member (TScreenPanel)
#define CHECKSUM_UI_IN_GAME_MAIN 0x0054679C
#define CHECKSUM_UI_SCENARIO_EDITOR_MAIN 0x00547360
#define CHECKSUM_UI_GAME_SETTINGS 0x00547810
#define CHECKSUM_UI_SAVE_AS 0x00547234 // TODO. "Save Game Screen" (to save scenario also)
#define CHECKSUM_UI_LOAD 0x005475B8 // TODO.

	// Size=0x478. A base class for all screens and popups (not UI components). Directly derives from base UI class. "EasyPanel".
	// Constructor = 0x454430 (0x468150 for intermediate class), Init (own fields) 0x454B60.
	// 0x455D10=easyPanel.addButton(parent, ptrForNewObject, textId1, textId2, hPos,vPos, hSize,vSize, font, soundNum, actionId)
	// EDX+F0=easyPanel.addButton(parent, pVarNewObj, text1, text2, hPos, vPos, hSize, vSize, font, soundNum, btnActionId)
	// 0x455FE0=INT TEasy_Panel::create_check_box(parent, ptrForNewObject,x,y,width,height,sound_num,action_id)
	// 0x456240=easyPanel.createRadioButton(parent, ptrForNewObject, hPos, vPos, xSize, ySize, soundNum, actionId)
	// 0x4564A0=INT TEasy_Panel::create_text(parent, ptrForNewObject,text_id,x,y,width,height,font_num,horz_center,vert_center,word_wrap)
	// 0x456530=INT TEasy_Panel::create_text(parent, ptrForNewObject,text,x,y,width,height,font_num,horz_center,vert_center,word_wrap)
	// 0x4566D0=INT TEasy_Panel::create_text(parent, ptrForNewObject,text,line_num,x,y,width,height,font_num,horz_center,vert_center)
	// 0x456870=INT TEasy_Panel::create_input(parent, ptrForNewObject,value,max_len,format_type,x,y,width,height, fontId)
	// 0x456A30=easyPanel.createTextbox(parent, ptrForNewObject, value, max_len, format_type, x, y, width, height, fontId, enable_ime, turn_ime_on)
	// 0x456BF0=easyPanel.createDropdown(parent, ptrForNewObject, list_width, list_height, x, y, width, height, fontId)
	// 0x456F20=easyPanel.createListBox(parent, ptrForNewObject, x, y, width, height, font_num)
	// 0x457170=easyPanel.create_scrollbar(parent, ptrForNewObject, list_panel, list_size, x, y, width, height)
	// 0x4572E0=easyPanel.create_auto_scrollbar(scrollbar_panel, ptrForNewObject, width)
	// 0x457330=easyPanel.create_vert_slider(parent, ptrForNewObject, scale, one_step, page_step, x, y, width, height)
	// 0x4574A0=easyPanel.create_horz_slider(parent, ptrForNewObject, scale, one_step, page_step, x, y, width, height)
	// 0x457610=TEasy_Panel::position_panel(panel,x,y,width,height)
	// 0x457670=TEasy_Panel::set_focus(have_focus_in)
	// 0x457740=TEasy_Panel::popupOKDialog(text_id,panel_title,wid,hgt)
	// 0x457790=TEasy_Panel::popupOKDialog(text,panel_title,wid,hgt)
	// 0x457A30=TEasy_Panel::popupYesNoDialog(text_id,panel_title,wid,hgt)
	// 0x457A80=TEasy_Panel::popupYesNoDialog(text,panel_title,wid,hgt)
	// 0x457B40=TEasy_Panel::popupYesNoCancelDialog(text_id,panel_title,wid,hgt)
	// 0x457B90=TEasy_Panel::popupYesNoCancelDialog(text,panel_title,wid,hgt)
	class STRUCT_UI_EASY_PANEL : public STRUCT_ANY_UI {
	public:
		long int screenSizeX; // +F4. Default 640. "IdealSize"
		long int screenSizeY; // +F8. Default 480. "IdealSize"
		char dialogName[0x104]; // +FC. unknown size. "infoFile"
		// 0x200
		long int slpIdForPopupBackground1_200; // +200. "Info Id"
		unsigned long int background; // +204.
		unsigned long int background2; // +208.
		unsigned long int palette; // +20C
		char unknownName_mcursors[0x104]; // +210.
		long int unknown_slpId_314; // +314. cursor ?
		unsigned long int backgroundPos; // +318.
		long int useBevels; // +31C.
		char colorForBtnBorderOutTopRight; // +320. Bevel color 1
		char colorForBtnBorderMidTopRight; // +321. Bevel color 2
		char colorForBtnBorderInTopRight; // +322. Bevel color 3
		char colorForBtnBorderInBottomLeft; // +323. Bevel color 4
		char colorForBtnBorderMidBottomLeft; // +324. Bevel color 5
		char colorForBtnBorderOutBottomLeft; // +325. Bevel color 6
		char unknown_326[2]; // unused
		long int textColor1; // +328
		long int textColor2; // +32C
		long int focusColor1; // +330
		long int focusColor2; // +334
		long int stateColor1; // +338
		long int stateColor2; // +33C
		char dialogNameForPopup[0x104]; // +340. Dialog name for options popup (dlg6_0) + diplomacy, menu, etc. "popupInfoFile"
		long int themeSlpId; // +444. Determines the colors, background images, UI component themes to use in popups. "popupInfoId"
		STRUCT_SLP_INFO *buttonPics; // +448.
		STRUCT_COLOR_DEF *shadowColorTable; // +44C. TODO: check type !
		long int shadowAmount; // +450.
		unsigned long int unknown_454;
		long int enableIME; // +458
		char unknown_45C[0x478 - 0x45C];
		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_UI_EASY_PANEL); }
		bool IsCheckSumValidForAChildClass() const {
			return (this->checksum == CHECKSUM_UI_EASY_PANEL) || (this->checksum == CHECKSUM_UI_SCREEN_PANEL) ||
				(this->checksum == CHECKSUM_UI_IN_GAME_MAIN) || (this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MAIN) || 
				(this->checksum == CHECKSUM_UI_GAME_SETTINGS);
		}
	};
	static_assert(sizeof(STRUCT_UI_EASY_PANEL) == 0x478, "STRUCT_UI_EASY_PANEL size");
}

