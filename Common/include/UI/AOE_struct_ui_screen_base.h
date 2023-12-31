
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
#define CHECKSUM_UI_GAME_SETTINGS 0x00547810 // scenario options (under RM/DM/scenario setting screens)
#define CHECKSUM_UI_WELCOME_MAIN_SCREEN 0x005469F8
#define CHECKSUM_UI_SP_MENU_SCREEN 0x0054793C // ccor 0x49E040. "SPMenuScreen" (SP menu after welcome screen). Parent=screenPanel
#define CHECKSUM_UI_MP_SETUP_SCREEN 0x00546D7C // includes RM/DM (main) settings screens
#define CHECKSUM_UI_CAMPAIGN_EDITOR 0x00545FB0
#define CHECKSUM_UI_CAMPAIGN_SELECTION 0x005460DC // SP menu->campaign->choose a name-> choose a campaign screen. Parent=screenPanel.
#define CHECKSUM_UI_SELECT_SCENARIO_SCREEN 0x005476E4 // ccor 0x49B360. "Select scenario screen". Parent=screenPanel
#define CHECKSUM_UI_MP_STARTUP_SCREEN 0x00546C50 // ccor 0x487250. Parent=screenPanel. First MP screen (choose name and cnx type)
#define CHECKSUM_UI_JOIN_SCREEN 0x00546208 // ccor 0x47B440. Parent=screenPanel?
#define CHECKSUM_UI_SCENARIO_EDITOR_MAIN_MENU 0x0054748C // parent=screenPanel
#define CHECKSUM_UI_ACHIEVEMENTS_SCREEN 0x00545E84 // parent=screenPanel
#define CHECKSUM_UI_CREDITS_SCREEN 0x00546468 // parent=screenPanel
#define CHECKSUM_UI_LOAD_SAVED_GAME 0x005468CC // ccor 0x485FC90
#define CHECKSUM_UI_MAIN_ERROR_SCREEN 0x00546B24 // ccor 0x486FD0. Parent=screenPanel
#define CHECKSUM_UI_MP_WAIT_SCREEN 0x00546EA8 // ccor 0x48D3A0. Parent=screenPanel
#define CHECKSUM_UI_NAME_SELECTION 0x00546FD4 // ccor 0x48DEC0. Parent=screenPanel
#define CHECKSUM_UI_SAVE_AS_SCREEN 0x00547234 // ccor 0x48ECA0. "Save Game Screen" (to save scenario also). Parent=screenPanel
#define CHECKSUM_UI_SC_EDITOR_OPEN 0x005475B8 // ccor 0x49AAA0. "Scenario editor open". Parent=screenPanel
#define CHECKSUM_UI_STATUS_MESSAGE 0x00547A68 // ccor 0x49E7A0, 0x49E9A0. Parent=screenPanel
#define CHECKSUM_UI_MISSION_SCREEN 0x00547B94 // ccor 0x49EC00. Parent=screenPanel

#define CHECKSUM_UI_DIALOG_BASE 0x00544F1C // Base class for dialogs, parent=screenPanel
#define CHECKSUM_UI_TLISTDIALOG 0x00543A7C // parent=dialogPanel.
#define CHECKSUM_UI_RGE_DIALOG_LIST 0x00545B84 // parent=TListDialog.
#define CHECKSUM_UI_MESSAGE_DIALOG 0x00543CEC // parent=dialogPanel. Eg confirmation before restart game, before opening help...
#define CHECKSUM_UI_NAME_DIALOG 0x00547100 // ccor 0x48E800. Parent=dialogPanel (then easyPanel). "New name dialog".
#define CHECKSUM_UI_SCENARIO_EDITOR_MENU 0x00543E20 // "Scenario Menu Dialog"
#define CHECKSUM_UI_IN_GAME_MENU 0x00543BB8 // in-game menu, parent=dialogBase
#define CHECKSUM_UI_IN_GAME_OPTIONS 0x005436E0 // in-game options popup
#define CHECKSUM_UI_DIPLOMACY_DIALOG 0x00543814
#define CHECKSUM_UI_QUICK_CHAT_DIALOG 0x00544088 // in-game small quick-chat dialog
#define CHECKSUM_UI_CONTEXT_HELP_DIALOG 0x00543948 // after clicking on ? then on something interesting... Ingame, also in MP menus
#define CHECKSUM_UI_ABOUT_DIALOG 0x005435AC // Menu->about in game screen
#define CHECKSUM_UI_CREATE_MP_GAME_DIALOG 0x00546334
#define CHECKSUM_UI_MP_CHAT_DIALOG 0x00543F54 // Chat menu in MP games

#define CHECKSUM_UI_TIMELINE_PANEL 0x0054A134 // ccor 0x4FBAA0. Parent=easyPanel


	// Size=0x478. A base class for all screens and popups (not UI components). Directly derives from base UI class. "EasyPanel".
	// Events are not overriden in this class: refer to STRUCT_ANY_UI methods.
	// Constructor = 0x454430, 0x454B10. Init (own fields) 0x454B60. After ccor, you should call setup(...)
	// 2 "main" direct child classes are screenPanel and DialogPanel. Both are "intermediate" generic classes.
	// Other direct child classes are timelinePanel (using 0x454B10 ccor).
	// 0x4544A0 = easyPanel.setup(renderArea, parentUI, dlgName, slpidbackground, fullScreen, hPos, vPos, hSize, vSize, allowShadowArea). Sizes: leave 0 if fullscreen. slpidbackground can be -1 or AOE_CONST_DRS::AoeScreenTheme.
	// 0x455D10=easyPanel.addButton(parent, ptrForNewObject, textId1, textId2, hPos,vPos, hSize,vSize, font, soundNum, actionId)
	// EDX+F0=easyPanel.addButton(parent, pVarNewObj, text1, text2, hPos, vPos, hSize, vSize, font, soundNum, btnActionId)
	// 0x455FE0=INT TEasy_Panel::create_check_box(parent, ptrForNewObject,x,y,width,height,sound_num,action_id)
	// 0x456240=easyPanel.createRadioButton(parent, ptrForNewObject, hPos, vPos, xSize, ySize, soundNum, actionId)
	// 0x4564A0=INT TEasy_Panel::createLabel(parent, ptrForNewObject,text_id,x,y,width,height,font_num,horz_center,vert_center,word_wrap)
	// 0x456530=INT TEasy_Panel::createLabel(parent, ptrForNewObject,text,x,y,width,height,font_num,horz_center,vert_center,word_wrap)
	// 0x4566D0=INT TEasy_Panel::createLabel(parent, ptrForNewObject,text,line_num,x,y,width,height,font_num,horz_center,vert_center)
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
	// +B8 = easyPanel.OnObjectUserEvent(sender, evtStatus, objBtnCmdId, InfoValue)
	class STRUCT_UI_EASY_PANEL : public STRUCT_ANY_UI {
	public:
		long int screenSizeX; // +F4. Default 640. "IdealSize"
		long int screenSizeY; // +F8. Default 480. "IdealSize"
		char dialogName[0x104]; // +FC. unknown size. "infoFile"
		// 0x200
		long int slpIdForPopupBackground1_200; // +200. "Info Id"
		STRUCT_SLP_INFO *background; // +204.
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
		char unknown_45C[0x470 - 0x45C];
		long int allowShadowArea; // +470. Not sure what it really does. Seems unused ?
		unsigned long int unknown_474;

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_UI_EASY_PANEL); }
	protected:
		// Returns true if checksum is valid for a screen panel class (or one of its child classes)
		bool IsCheckSumValidForScreenPanel() const {
			return (this->checksum == CHECKSUM_UI_SCREEN_PANEL) ||
				// Main welcome screen
				(this->checksum == CHECKSUM_UI_WELCOME_MAIN_SCREEN) ||
				// SP menu and sub-menus
				(this->checksum == CHECKSUM_UI_SP_MENU_SCREEN) || // Main menu for single player
				(this->checksum == CHECKSUM_UI_MP_SETUP_SCREEN) || // Game configuration for RM/DM/scenario/MP
				(this->checksum == CHECKSUM_UI_GAME_SETTINGS) || // Advanced game settings for RM/DM/scenario/MP
				(this->checksum == CHECKSUM_UI_CAMPAIGN_SELECTION) || // Choose a campaign screen
				(this->checksum == CHECKSUM_UI_SELECT_SCENARIO_SCREEN) || // Choose a scenario screen
				(this->checksum == CHECKSUM_UI_LOAD_SAVED_GAME) ||
				(this->checksum == CHECKSUM_UI_CREDITS_SCREEN) ||
				(this->checksum == CHECKSUM_UI_NAME_SELECTION) || // Choose a player "account" for campaign
				// MP screens
				(this->checksum == CHECKSUM_UI_MP_STARTUP_SCREEN) || // First MP screen: choose name, connection type
				(this->checksum == CHECKSUM_UI_JOIN_SCREEN) || // Find or create a MP game
				// Scenario editor menus/screens
				(this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MAIN) || (this->checksum == CHECKSUM_UI_CAMPAIGN_EDITOR) ||
				(this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MAIN_MENU) || (this->checksum == CHECKSUM_UI_SC_EDITOR_OPEN) ||
				// In-game
				(this->checksum == CHECKSUM_UI_IN_GAME_MAIN) ||
				(this->checksum == CHECKSUM_UI_MISSION_SCREEN) || // In-game "scenario info" screen (scenario pre-game, also)
				(this->checksum == CHECKSUM_UI_SAVE_AS_SCREEN) ||
				(this->checksum == CHECKSUM_UI_ACHIEVEMENTS_SCREEN) || // Achievements screen, (also for timeline main screen as it is a dialog)
				// Other
				(this->checksum == CHECKSUM_UI_MP_WAIT_SCREEN) || 
				(this->checksum == CHECKSUM_UI_MAIN_ERROR_SCREEN) || // When is this used ?
				(this->checksum == CHECKSUM_UI_STATUS_MESSAGE) // When is this used ?
				;
		}
		// Returns true if checksum is valid for a dialog panel class (or one of its child classes)
		bool IsCheckSumValidForDialog() const {
			return (this->checksum == CHECKSUM_UI_DIALOG_BASE) ||
				(this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MENU) ||
				(this->checksum == CHECKSUM_UI_IN_GAME_MENU) ||
				(this->checksum == CHECKSUM_UI_IN_GAME_OPTIONS) ||
				(this->checksum == CHECKSUM_UI_DIPLOMACY_DIALOG) ||
				(this->checksum == CHECKSUM_UI_QUICK_CHAT_DIALOG) ||
				(this->checksum == CHECKSUM_UI_CONTEXT_HELP_DIALOG) ||
				(this->checksum == CHECKSUM_UI_ABOUT_DIALOG) ||
				(this->checksum == CHECKSUM_UI_CREATE_MP_GAME_DIALOG) ||
				(this->checksum == CHECKSUM_UI_MP_CHAT_DIALOG) ||
				(this->checksum == CHECKSUM_UI_MESSAGE_DIALOG) ||
				(this->checksum == CHECKSUM_UI_RGE_DIALOG_LIST) ||
				(this->checksum == CHECKSUM_UI_TLISTDIALOG) ||
				(this->checksum == CHECKSUM_UI_NAME_DIALOG);
		}
	public:
		// Returns true if checksum corresponds to a known Screen class, including EasyPanel common ancestor.
		bool IsCheckSumValidForAChildClass() const {
			return (this->checksum == CHECKSUM_UI_EASY_PANEL) || // Actually easyPanel
				this->IsCheckSumValidForScreenPanel() ||
				this->IsCheckSumValidForDialog() ||
				// Directly under easyPanel:
				(this->checksum == CHECKSUM_UI_TIMELINE_PANEL);
		}
	};
	static_assert(sizeof(STRUCT_UI_EASY_PANEL) == 0x478, "STRUCT_UI_EASY_PANEL size");


	// Size=0x478 Constructor=0x468150
	// Setup=0x468180
	// Type for screens whose characteristics come from text information from DRS file
	class STRUCT_UI_SCREEN_PANEL : public STRUCT_UI_EASY_PANEL {
	public:

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_UI_SCREEN_PANEL); }

		bool IsCheckSumValidForAChildClass() const {
			return this->IsCheckSumValidForScreenPanel();
		}
	};
	static_assert(sizeof(STRUCT_UI_SCREEN_PANEL) == 0x478, "STRUCT_UI_SCREEN_PANEL size");

}

