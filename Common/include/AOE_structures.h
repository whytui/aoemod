
#pragma once

//#include <math.h>
#include <AOE_const_functional.h>
#include <AOE_const_internal.h>
#include <AOE_empires_dat.h>
#include <AOE_structures_drs.h>
#include <AOE_struct_tech_def.h>
#include <AOE_struct_research_def.h>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_civilization_def.h>
#include <AOE_struct_terrain_def.h>
#include <AOE_struct_map_def.h>
#include <AOE_struct_map_generation.h>
#include <AOE_struct_map_visibility_info.h>
#include <AOE_struct_map_base_common.h>
#include <AOE_struct_map_tile_info.h>
#include <AOE_struct_map_global_objects.h>
#include <AOE_struct_game_map_info.h>
#include <AOE_struct_unit_activity.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include <AOE_struct_game_global.h>
#include <gameVersion.h>


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// UI components
	class STRUCT_UI_OBJ_UNKNOWN_INFO;
	class STRUCT_ANY_UI; // parent class
	class STRUCT_UI_LIST_ITEM;
	class STRUCT_UI_BUTTON;
	class STRUCT_UI_BUTTON_WITH_NUMBER;
	class STRUCT_UI_TEXTBOX;
	class STRUCT_UI_LISTBOX;
	class STRUCT_UI_COMBOBOX;
	class STRUCT_UI_SCROLLBAR;
	class STRUCT_UI_LABEL;
	// UI
	class STRUCT_UI_DIAMOND_MAP;
	class STRUCT_UI_IN_GAME_TEXT_ZONE;
	class STRUCT_UI_PLAYING_ZONE; // Also used in editor !
	class STRUCT_UI_UNIT_BUTTON_INFO;
	class STRUCT_UI_SCENARIO_EDITOR_MAIN; // "Scenario Editor Screen"
	class STRUCT_UI_SCENARIO_EDITOR_MENU;
	class STRUCT_UI_F11_POP_PANEL;
	class STRUCT_UI_UNIT_INFO_ZONE;
	class STRUCT_UI_EDITOR_UNIT_INFO_ZONE;
	class STRUCT_UI_IN_GAME_MAIN; // "GameScreen"
	class STRUCT_UI_IN_GAME_SUB1;
	class STRUCT_UI_IN_GAME_MENU;
	class STRUCT_UI_POPUP_DIALOG;
	class STRUCT_UI_GAME_SETTINGS;
	class STRUCT_UI_DIPLOMACY_DIALOG;
	class STRUCT_UI_IN_GAME_OPTIONS;
	class STRUCT_UI_LOAD_SAVED_GAME;
	class STRUCT_UI_SCENARIO_EDITOR_MAIN_MENU; // welcome screen for scenario editor (with 3 buttons)
	class STRUCT_UI_CAMPAIGN_EDITOR;
	class STRUCT_UI_DIAM_MAP; // diam_map in game comments
	class STRUCT_UI_JOIN_SCREEN;
	class STRUCT_UI_WELCOME_MAIN_SCREEN;





	// Size = 0xB0. Added (created) in 4FC5A0.
	// Represents population info at one precise given moment of the game. (used to draw timeline graphic)
	// Referenced by timelineUI+518 (last elem?)
	class STRUCT_TIMELINE_COLUMN_INFO {
	public:
		long int unknown_00;
		long int unknown_04;
		long int unknown_08;
		long int unknown_0C;
		// 0x10
		char unknown_10;
		char unknown_11;
		char unknown_12; // default = 3 ?
		char unknown_13;
		long int totalPopulationCount;
		long int unknown_18;
		long int unknown_1C;
		long int unknown_20;
		long int unknown_24;
		long int unknown_28;
		long int unknown_2C;
		long int unknown_30;
		long int unknown_34;
		long int unknown_38;
		char totalPopulationString[0x70 - 0x3C]; // like "24"
		// 0x70
		long int unknown_70;
		char timeString[0xA8 - 0x74]; // +74. Like "0:00"
		long int unknown_A8; // timeIndex ?
		STRUCT_TIMELINE_COLUMN_INFO *previous;
	};
	static_assert(sizeof(STRUCT_TIMELINE_COLUMN_INFO) == 0xB0, "STRUCT_TIMELINE_COLUMN_INFO size");




	class STRUCT_UI_OBJ_UNKNOWN_INFO {
	public:
		unsigned long int unknown_00; // A pointer
		long int hWnd;
		unsigned long int unknown_08;
		unsigned long int unknown_0C; // A pointer
		// 0x10
	};

	// A shortcut to be used with UI objects when exact type is unknown. Also used as parent class for UI objects.
	// Size=0xF4 for this class, but all child classes are larger !
	// Constructor: 004523F0 for "screen" objects, 00452580 for components
	// In general, you should not modify these members directly. You are supposed to use methods (few are implemented in this solution).
	// D8 4A 54 00 for this base (parent) class ??
	class STRUCT_ANY_UI {
	public:
		unsigned long int checksum; // Depends on UI object type
		unsigned long int unknown_004; // pointer ?
		unsigned long int unknown_008;
		unsigned long int unknown_00C_posX; // unsure
		// 0x10
		unsigned long int unknown_010_posY; // unsure
		long int sizeX;
		long int sizeY;
		char *screenName;
		// 0x20
		STRUCT_UI_OBJ_UNKNOWN_INFO *unknown_020; // pointer. +04=hWnd +13C=evtStatus?? +1F6=objectIndex,+F4+objectIndex*4=ButtonId +17C=hObject?
		long int minPosX; // +24. Min X position for this UI object. +24=a pRect object (4 position dwords)
		long int minPosY; // +28
		long int maxPosX; // +2C
		// 0x30
		long int maxPosY; // +30
		unsigned long int unknown_034;
		unsigned long int unknown_038; // init = 2
		STRUCT_ANY_UI *focusedComponent;
		// 0x40
		STRUCT_ANY_UI *ptrParentObject; // +40
		unsigned long int unknown_044;
		unsigned long int unknown_048;
		unsigned long int unknown_04C;
		// 0x50
		unsigned long int unknown_050;
		unsigned long int unknown_054; // ptr ?
		unsigned long int unknown_058; // ptr ?
		unsigned long int unknown_05C; // A ptr to a list: elem= {+0=STRUCT_ANY_UI*, +4=nextElem}. Used in 453360
		// 0x60
		unsigned long int unknown_060;
		unsigned long int unknown_064;
		unsigned long int unknown_068; // 1 when being clicked ???
		long int visible; // +6C. For a dialog it is 0 ? Do not edit directly.
		// 0x70
		unsigned long int unknown_070;
		unsigned long int editable; // TO CONFIRM !! seen for text components. What about others ? For button, it is text color when "active"?
		unsigned long int unknown_078;
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
		unsigned long int unknown_0D4;
		unsigned long int unknown_0D8;
		unsigned long int unknown_0DC;
		// 0xE0
		unsigned long int unknown_0E0;
		long int helpDllId; // +E4. Dll string Id for help (or tooltip?) ?
		long int winHelpDataDllId; // +E8. a DllID, used when clicking on help/(object)/details = data for winHelp call.
		char unknown_0EC;
		char unknown_0ED;
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
			if (!this->unknown_020) { return 0; }
			return this->unknown_020->hWnd;
		}
	};


	// Size 0x2B8. Also for checkboxes.
	// Can be created in 0x0456240
#define CHECKSUM_UI_BUTTON 0x00544E30
#define CHECKSUM_UI_BUTTON_WITH_NUMBER 0x00549CAC
	class STRUCT_UI_BUTTON : public STRUCT_ANY_UI {
	public: // 30 4E 54 00 (a child class is AC 9C 54 00(size=0x3D4), rarely used. 16 are created in game screen=unit-commands)
		long int commandIDs[9]; // +F4. Only index 0 is really used ?? Various types (enums) (GAME_SCREEN_BUTTON_IDS, INGAME_UI_COMMAND_ID, etc)
		long int buttonInfoValue[9]; // +118. For example, a DATID.
		unsigned long int unknown_13C;
		unsigned long int unknown_140;
		unsigned long int unknown_144;
		unsigned long int* unknown_148_wav; // +148: sound button.wav info? (always 0 except when playing sound ?)
		unsigned long int* unknown_14C[9]; // Pointers, but not UI structs ?
		short int unknown_170[9]; // +170. Default -1.
		char unknown_182[0x198 - 0x182];
		char* unknown_198[9];
		char* unknown_1BC[9];
		char unknown_1E0[0x1F6 - 0x1E0];
		short int checked; // +1F6. To update value, see AOE_CheckBox_SetChecked. Index for commandIDs and buttonInfoValue
		unsigned long int unknown_1F8;
		STRUCT_ANY_UI **unknown_1FC; // ptr array groupedObjects ? really unsure
		// 0x200
		short int unknown_200; // groupedObjectCount ? really unsure (like radiobuttons?)
		char unknown_202[0x208 - 0x202];
		unsigned long int unknown_208[9]; // +208. default 0x00FFFFFF
		unsigned long int unknown_22C[9];
		long int unknown_250[9]; // +250. default 0xFFFF
		unsigned long int unknown_274[9];
		long int hotkey; // +298. Ascii code of hotkey: 65='A', etc.
		long int unknown_29C; // +29C. Related to hotkey ? Not sure.
		unsigned long int unknown_2A0;
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

		bool IsChecked() { return this->checked != 0; }
		bool IsCheckSumValid() {
			return (this->checksum == CHECKSUM_UI_BUTTON_WITH_NUMBER) || (this->checksum == CHECKSUM_UI_BUTTON);
		}
	};
	static_assert(sizeof(STRUCT_UI_BUTTON) == 0x2B8, "STRUCT_UI_BUTTON size");

	// Size 0x3D4. Constructor=0x4F7420
	class STRUCT_UI_BUTTON_WITH_NUMBER : public STRUCT_UI_BUTTON {
	public:
		// Starts at +2B8
		long int showNumber; // +2B8. If true, numberToDisplay will be displayed. Set in 0x4F72B0. Call refresh after changing it.
		long int numberToDisplay; // +2BC. The number to show on button icon, like queued item count. Call refresh after changing it.
		// 0x2C0
		char unknown_2C0; // Values 0, 3 .. ?
		char unknown_2C1[3];
		unsigned long int *unknown_2C4;
		unsigned long int *unknown_2C8;
		unsigned long int unknown_2CC;
		char contextHelpText[0x100]; // +2D0.
		long int unknown_3D0; // see 483785
		bool IsCheckSumValid() {
			return (this->checksum == CHECKSUM_UI_BUTTON_WITH_NUMBER);
		}
	};
	static_assert(sizeof(STRUCT_UI_BUTTON_WITH_NUMBER) == 0x3D4, "STRUCT_UI_BUTTON_WITH_NUMBER size");


	// Size ?
	// Can be created in 0x0456A30
#define CHECKSUM_UI_TEXTBOX 0x00545220
	class STRUCT_UI_TEXTBOX : public STRUCT_ANY_UI { // 20 52 54 00
	public:
		long int hWnd;
		short int maxTextSize; // +F8. =constructor_maxTextSize+1
		short int unknown_0FA; // +FA
		char *pTypedText; // +FC.
		// 0x100
		char unknown_100[0x128 - 0x100];
		long int inputType; // 0=normal, 3=numberOnly, 7=multiline

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_TEXTBOX; }
	};


	// Size = 0x10
	class STRUCT_UI_LIST_ITEM {
	public:
		char *itemText;
		unsigned long int *unknown_04;
		long int optionalId;
		STRUCT_UI_LIST_ITEM *next;
	};

	// Size = 0x1A0  // F8 53 54 00
	// Constructor = 0x464A50 (also 00456F20?). Parent class is label (90 5A 54 00).
	// See also STRUCT_UI_COMBOBOX (that contains a listbox)
#define CHECKSUM_UI_LISTBOX 0x005453F8
	class STRUCT_UI_LISTBOX : public STRUCT_ANY_UI {
	public:
		STRUCT_UI_LIST_ITEM *firstItem; // +F4. First element of list of items.
		short int itemCount; // +F8
		short int unknown_0FA;
		short int unknown_0FC; // minVisibleIndex??
		short int unknown_0FE; // maxVisibleIndex??
		short int selectedIndex; // +100.
		short int unknown_102; // +102
		// +138=word

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_LISTBOX; }
	};

	// Size = 0x144 - 50 50 54 00. Direct parent is STRUCT_ANY_UI
	// Constructor = 0x460A00
#define CHECKSUM_UI_COMBOBOX 0x00545050
	class STRUCT_UI_COMBOBOX : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4;
		unsigned long int unknown_0F8;
		STRUCT_UI_LISTBOX *underlyingListBox;
		// 0x100
		unsigned long int unknown_100;
		unsigned long int unknown_104;
		unsigned short int unknown_108;
		unsigned short int unknown_10A;
		unsigned long int unknown_10C; // unknown type
		char unknown_110[0x130 - 0x110];
		// 0x130
		char unknown_130;
		char unknown_131;
		short int unknown_132; // unknown type
		unsigned long int unknown_134; // unknown type
		unsigned long int unknown_138; // unknown type
		unsigned long int unknown_13C;
		// 0x140
		unsigned long int unknown_140; // init=1

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_COMBOBOX; }
		long int GetValue() {
			if (!this->IsCheckSumValid()) { return -1; }
			unsigned long int tmp = this->checksum;
			_asm {
				MOV ECX, tmp
				MOV EAX, 0x461E50
				CALL EAX
				MOV tmp, EAX
			}
			return tmp;
		}
	};

	// Size = 0x1C4 // B8 56 54 00
	// Constructor = 0x466E30.
#define CHECKSUM_UI_SCROLLBAR 0x005456B8
	class STRUCT_UI_SCROLLBAR : public STRUCT_ANY_UI {
	public:
		STRUCT_ANY_UI *objectToScroll; // +F4

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_SCROLLBAR; }
	};


	// Size 0x188
	// Constructor=0x4685B0
#define CHECKSUM_UI_LABEL 0x00545A90
	class STRUCT_UI_LABEL : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4;
		unsigned short int unknown_0F8;
		unsigned short int unknown_0FA;
		unsigned short int unknown_0FC;
		unsigned short int unknown_0FE;
		// 0x100
		unsigned short int unknown_100;
		unsigned short int unknown_102;
		unsigned long int unknown_104;
		unsigned long int unknown_108;
		unsigned long int unknown_10C;
		// 0x110
		unsigned long int unknown_110;
		unsigned long int unknown_114;
		unsigned long int unknown_118;
		unsigned long int unknown_11C; // default 2 ?
		// 0x120
		unsigned long int unknown_120;
		unsigned long int unknown_124;
		unsigned long int unknown_128; // type ?
		unsigned long int unknown_12C;
		// 0x130
		unsigned long int unknown_130;
		unsigned long int unknown_134;
		unsigned short int unknown_138;
		unsigned short int unknown_13A; // type ?
		unsigned long int unknown_13C;
		// 0x140
		unsigned long int unknown_140;
		unsigned long int unknown_144;
		unsigned short int unknown_148;
		unsigned short int unknown_14A;
		unsigned long int unknown_14C;
		// 0x150
		char unknown_150; // init -1
		char unknown_151; // is it used ?
		short int unknown_152; // is it used ?
		unsigned long int unknown_154;
		char unknown_158;
		char unknown_159; // is it used ?
		short int unknown_15A; // is it used ?
		unsigned long int unknown_15C;
		// 0x160
		unsigned long int unknown_160;
		unsigned long int unknown_164;
		unsigned long int unknown_168;
		unsigned long int unknown_16C;
		// 0x170
		unsigned long int unknown_170;
		char unknown_174;
		char unknown_175;
		char unknown_176;
		char unknown_177;
		char unknown_178;
		char unknown_179;
		char unknown_17A; // unused ?
		char unknown_17B; // unused ?
		unsigned long int unknown_17C;
		// 0x180
		unsigned long int unknown_180;
		unsigned long int unknown_184;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_LABEL; }
	};


	// Size ?
/*#define CHECKSUM_UI_IN_GAME_SUB1 0x00546688
	class STRUCT_UI_IN_GAME_SUB1 : public STRUCT_ANY_UI { // TO DO - name to fix - role to determine
	public:
		char unknown_0F4[0x100 - 0x0F4];
		STRUCT_PLAYER *humanControlledPlayer;
		long int unknown_104_link_map_infos; // like global+28 ?
		// 340 = unitId (under mouse ?)

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_SUB1; }
	};
	*/

	// Size ?
#define CHECKSUM_UI_DIAMOND_MAP 0x0054A73C
	class STRUCT_UI_DIAMOND_MAP : public STRUCT_ANY_UI { // 3C A7 54 00 (editor). Also F4 A3 54 00(game) ?
	public:
		STRUCT_GAME_GLOBAL *global; // +F4
		STRUCT_PLAYER *controlledPlayer;
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +FC
		// 0x100
		char *unknown_100[0x128 - 0x100];
		unsigned long int unknown_128; // array map coordinates per diammap "index" ?? ElemSize=6 (3 words) [+0]+[+2]=constant(6E)?
		unsigned long int unknown_12C; // pointer to diam map data? Alloc_size = mapSizeX_plus_Y_minus_1 * 0x24. +0=word(an index?)+2=byte=relevant,+4=word,+8=float,+C=dword,+10=sizeY-?-1,+12=word,+14=word,+18=float,+1C=word,+20=float
		// 0x130
		short int mapSizeY;
		short int mapSizeX; // +132
		unsigned long int unknown_134;
		short int unknown_138; // Number of "diam_map" indexes ? =elemCount for +128 ?
		short int mapSizeX_plus_Y_minus_1; // +13A. = mapSizeY+mapSizeX-1
		float unknown_pixelFactor; // +13C. = mapSizeX_plus_Y_minus_1 / unknown_138
		// 0x140
		char *unknown_140[0x168 - 0x140];
		unsigned long int unknown_168; // ptr to diamMap data ? (color to display...) ? +8=ptr to array[byte=index]=colorPerPixel

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_DIAMOND_MAP; }
	};

	// Size 0x380. Constructor=0x465730
	// class for chat text lines (1 for each chat line), yellow/orange centered in-game error messages
	class STRUCT_UI_IN_GAME_TEXT_ZONE : public STRUCT_ANY_UI {
	public:
		//unsigned long int checksum; // EC 54 54 00
		// F8 : text?
		bool IsCheckSumValid() { return this->checksum == 0x005454EC; }
	};

	// Size 0x0x4BC. Constructor = 0x433C30 (create and show). B8 3B 54 00. Parent=1C 4F 54 00
#define CHECKSUM_UI_IN_GAME_MENU 0x00543BB8
	class STRUCT_UI_IN_GAME_MENU : public STRUCT_ANY_UI{
	public:
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_MENU; }
	};

	// Size = 0x4A4
	// Constructor=0x434550
	class STRUCT_UI_POPUP_DIALOG : public STRUCT_ANY_UI {
	public:
		// Note on isActive: For a dialog it is probably always 0. Parent UI object is active, not the dialog.
		char unknown_0F4[0x498 - 0x0F4];
		STRUCT_UI_BUTTON *btnYes;
		STRUCT_UI_BUTTON *btnNo;
		// 4A0
		STRUCT_UI_BUTTON *btnCancel;
		// End.
		bool IsCheckSumValid() { return this->checksum == 0x00543CEC; }
	};

#define CHECKSUM_UI_PLAYING_ZONE 0x00546688
	// Size 0x380
	// Constructor 0x518690
	// Parent can be STRUCT_UI_IN_GAME_MAIN, or editor UI?
	// Also used in editor !
	// 88 66 54 00, parent = 40 A8 54 00, 1C A6 54 00, then UI base
	class STRUCT_UI_PLAYING_ZONE : public STRUCT_ANY_UI {
	public:
		long int hWnd; // TO CONFIRM. +F4
		unsigned long unknown_0F8;
		STRUCT_GAME_GLOBAL *globalStruct; // +FC
		// 0x100
		STRUCT_PLAYER *controlledPlayer;
		STRUCT_GAME_MAP_INFO *globalGameMapInfo; // +104. To confirm
		short int unknown_108; // some value for Y
		short int unknown_10A;
		short int unknown_10C;
		short int unknown_10E; // some value for X
		unsigned long int unknown_110;
		short int unknown_114;
		short int unknown_116;
		short int unknown_118;
		short int unknown_11A;
		short int unknown_11C; //?
		short int unknown_11E; //?
		short int unknown_120;
		short int unknown_122_screenGamePosY_word; // +122. From player.screenPosY
		short int unknown_124_screenGamePosX_word; // +124. From player.screenPosX
		short int unknown_126;
		short int unknown_128;
		short int unknown_12A;
		short int unknown_12C;
		short int unknown_12E;
		char unknown_130[0x138 - 0x130];
		float screenGamePosY; // +138.
		float screenGamePosX; // +13C.
		char unknown_140;
		char unknown_141[3];

		char unknown_144[0x334 - 0x144];
		// +168 : byte
		// +304, 318, 31C, 320: dwords related to mouse pos?

		unsigned long int unknown_334_ptr; // Ptr to obj size=0x70, ccor=516110. +5C=array size 4*(maxTypeId+1) of ptrs to obj. obj:+4=next,+8=unitId,+30/32/34/36=min/maxPosXY? +C,+20=ptrGraphicsShp?
		// Note: +4C=maxTypeId, typeIDs: 00=eyeCandy,10=birds,20=flag/deadfish/living/building/tree,30,40=?
		unsigned long int unknown_338_ptr; // Ptr to same kind of obj as +334
		unsigned long int unknown_33C;
		// 0x340
		long int unknown_340_unitId; // Target ? Selected ? Under mouse ?
		unsigned long int unknown_344;
		unsigned long int unknown_348;
		unsigned long int unknown_34C;
		char unknown_350[0x36C - 0x350];
		long int unsure_gamePosYUnderMouse; // +36C. mouse pos or game pos (or temp variable ?)
		long int unsure_gamePosXUnderMouse; // +370. mouse pos or game pos (or temp variable ?)

		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_UI_PLAYING_ZONE) || (this->checksum == 0x0054A840); }
	};


	// Size=0x28. No constructor (this is a temporary struct). See init in 4F0EEC for example.
	class STRUCT_UI_UNIT_BUTTON_INFO {
	public:
		char *name;
		short int DATID; // +4. Can be unitDefId, researchId
		short int iconId; // +6.
		short int costType1; // +8. A resourceId
		short int unused_0A;
		float costAmount1; // +C
		// 0x10
		short int costType2; // +10. A resourceId
		short int unused_12;
		float costAmount2; // +14
		short int costType3; // +18. A resourceId
		short int unused_1A;
		float costAmount3; // +1C
		// 0x20
		long int languageDllHelp; // +20. Id for language(x).dll
		long int languageDllHotkeyText; // +24. Id of hotkey for language(x).dll
	};
	static_assert(sizeof(STRUCT_UI_UNIT_BUTTON_INFO) == 0x28, "STRUCT_UI_UNIT_BUTTON_INFO size");


	// Size 0x948. Constructor 0x48FC40
	// This is the parent UI object of in-game screen.
#define CHECKSUM_UI_SCENARIO_EDITOR_MAIN 0x00547360
	class STRUCT_UI_SCENARIO_EDITOR_MAIN : public STRUCT_ANY_UI { // 60 73 54 00
	public:
		long int hWnd; // TO CONFIRM
		char unknown_0F8[0x4A8 - 0x0F8];
		long int currentMapGenerationChoice; // +4A8. 1=empty, 2=random, 3=from seed
		long int unknown_04AC; // default 2
		unsigned long int unknown_04B0;
		unsigned long int unknown_04B4;
		long int selectedPlayerId; // +4B8. Default 1
		unsigned long int unknown_04BC;
		// 0x4C0
		char unknown_04C0; // default 0
		char unknown_04C1; // default 0
		short int unknown_04C2; // unused??
		unsigned long int unknown_04C4;
		STRUCT_UI_PLAYING_ZONE *gamePlayUIZone; // +4C8. 88 66 54 00.
		STRUCT_UI_DIAMOND_MAP *diamondMap; // +4CC. ptr to 3C A7 54 00.
		// 0x4D0
		unsigned long int unknown_04D0;
		STRUCT_ANY_UI *unknown_04D4; // *Really* UI base class (D8 4A 54 00)
		STRUCT_UI_BUTTON *btnTabs[10]; // +4D8. Map, terrain, players, units, dipl // vict1, vict2, options, messages, animation
		// 0x500
		STRUCT_UI_BUTTON *btnMenu; // +500. Top right.
		STRUCT_UI_BUTTON *btnHelp; // +504. In bottom-right corner.
		STRUCT_UI_BUTTON *map_btn_emptyMap; // +508. Checkbox
		STRUCT_UI_BUTTON *map_btn_random; // +50C. Checkbox
		STRUCT_UI_BUTTON *map_btn_fromSeed; // +510. Checkbox
		STRUCT_UI_LABEL *map_lbl_btnEmpty; // +514
		STRUCT_UI_LABEL *map_lbl_btnRandom; // +518
		STRUCT_UI_LABEL *map_lbl_btnSeed; // +51C
		STRUCT_UI_LABEL *map_lbl_map; // +520
		STRUCT_UI_LABEL *map_lbl_cbbDefaultTerrain; // +524
		STRUCT_ANY_UI *map_cbb_defaultTerrain; // +528. Combobox
		STRUCT_UI_LABEL *map_lbl_mapSize; // +52C
		STRUCT_UI_COMBOBOX *map_cbb_mapSize; // +530. Combobox
		STRUCT_UI_LABEL *map_lbl_mapType; // +534
		STRUCT_UI_COMBOBOX *map_cbb_mapType; // +538
		STRUCT_UI_LABEL *map_lbl_seed_whenEditable; // +53C
		STRUCT_ANY_UI *map_edt_seed; // +540
		STRUCT_UI_LABEL *map_lbl_seed_whenReadOnly; // +544
		STRUCT_UI_LABEL *map_edt_seed_whenReadOnly; // +548
		STRUCT_UI_BUTTON *map_btn_generateMap; // +54C
		STRUCT_UI_LABEL *map_lbl_bigGeneratingMapNotification; // +550. The big centered message while generating.
		STRUCT_UI_LABEL *trn_lbl_pencilType; // +554
		STRUCT_UI_BUTTON *trn_btn_pencilSmallest; // +558
		STRUCT_UI_BUTTON *trn_btn_pencilSmall; // +55C
		STRUCT_UI_BUTTON *trn_btn_pencilMedium; // +560
		STRUCT_UI_BUTTON *trn_btn_pencilLarge; // +564
		STRUCT_UI_BUTTON *trn_btn_pencilLargest; // +568
		STRUCT_UI_LABEL *trn_lbl_pencilSmallest; // +56C
		STRUCT_UI_LABEL *trn_lbl_pencilSmall; // +570
		STRUCT_UI_LABEL *trn_lbl_pencilMedium; // +574
		STRUCT_UI_LABEL *trn_lbl_pencilLarge; // +578
		STRUCT_UI_LABEL *trn_lbl_pencilLargest; // +57C
		STRUCT_UI_LABEL *unknown_580;
		STRUCT_UI_BUTTON *trn_btn_map; // +584. To edit terrain
		STRUCT_UI_BUTTON *trn_btn_elevation; // +588. To edit elevation
		STRUCT_UI_BUTTON *trn_btn_cliffs; // +58C. cliffs button
		STRUCT_UI_LABEL *trn_lbl_map; // +590. "Map" in terrain tab (actually, to edit terrain)
		STRUCT_UI_LABEL *trn_lbl_elevation;
		STRUCT_UI_LABEL *trn_lbl_cliffs;
		STRUCT_UI_LABEL *unknown_59C;
		STRUCT_UI_LISTBOX *trn_lst_terrainList; // +5A0. List of terrains
		STRUCT_ANY_UI *trn_scroll_terrainList; // scroll for trn_lst_terrainList ? B8 56 54 00
		STRUCT_UI_LISTBOX *trn_lst_elevationList; // +5A8
		STRUCT_ANY_UI *unknown_5AC; // 56b8
		STRUCT_UI_LABEL *pl_lbl_playerCount;
		STRUCT_UI_COMBOBOX *pl_cbb_selectedPlayer; // +5B4. In Players tab, current player selection.
		STRUCT_UI_COMBOBOX *pl_cbb_playerCount; // +5B8. Number of players combobox.
		STRUCT_ANY_UI *unknown_5BC[(0x640 - 0x5BC) / 4]; // TO DO
		unsigned long int unknown_640; // +640 healthSlp ?
		unsigned long int unknown_644; // +644 itemiconSLP ?
		STRUCT_SLP_INFO *iconsForUnits; // +648.
		STRUCT_SLP_INFO *iconsForBuildingsTileSet0; // +64C.
		STRUCT_SLP_INFO *iconsForBuildingsTileSet1; // +650.
		STRUCT_SLP_INFO *iconsForBuildingsTileSet2; // +654.
		STRUCT_SLP_INFO *iconsForBuildingsTileSet3; // +658.
		STRUCT_SLP_INFO *iconsForBuildingsTileSet4; // +65C. Not well supported ?
		STRUCT_UI_EDITOR_UNIT_INFO_ZONE *selectedUnitInfoZone; // +660.
		long int unitListElementCount; // +664
		STRUCT_UI_BUTTON *gv_btn_custom_exploration; // +668
		STRUCT_UI_BUTTON *gv_btn_custom_ruins; // +66C
		STRUCT_UI_BUTTON *gv_btn_custom_relics; // +670
		STRUCT_UI_BUTTON *gv_btn_custom_discoveries; // +674
		STRUCT_ANY_UI *unknown_678[(0x928 - 0x678) / 4]; // TO DO
		STRUCT_GAME_GLOBAL *global; // +928
		long int unknown_92C; // Default 200
		long int unknown_930_timeGetTimeValue; // +930. TimeGetTime result value
		long int unknown_934; // Default 1000
		long int unknown_938_timeGetTimeValue; // +938. TimeGetTime result value
		unsigned long int unknown_93C;
		unsigned long int unknown_940;
		unsigned long int unknown_944;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MAIN; }
	};
	static_assert(sizeof(STRUCT_UI_SCENARIO_EDITOR_MAIN) == 0x948, "STRUCT_UI_SCENARIO_EDITOR_MAIN size");


	// Size=0x164. Constructor=0x4FA150?
#define CHECKSUM_UI_F11_POP_PANEL 0x00549F6C
	class STRUCT_UI_F11_POP_PANEL : public STRUCT_ANY_UI {
	public:
		long int unknown_0F4_font; // +F4.
		long int unknown_0F8;
		long int colorRGB; // +0xFC
		char unknown_100[0x110 - 0x100];
		// 0x110
		long int currentPopulation; // +110
		long int currentMaxHostedPopulation; // +114. Maximum population currently supported by houses
		char unknown_118;
		char unknown_119[3]; // unused ?
		char unknown_11C[0x164 - 0x11C];
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_F11_POP_PANEL; }
	};
	static_assert(sizeof(STRUCT_UI_F11_POP_PANEL) == 0x164, "STRUCT_UI_F11_POP_PANEL size");


#define CHECKSUM_UI_UNIT_INFO_ZONE 0x00549E7C
#define CHECKSUM_UI_IN_GAME_UNIT_INFO_ZONE 0x00547CC0
	// Size=0x214. Constructor=0x4F83D0 = unitInfoZone.constructor(arg1, mainGameUI, arg3, arg4, arg5, iconsForTrainUnit, iconsForBuildings, controlledPlayer)
	// Methods: 0x4F9970=unitInfoZone.refreshIfNeeded(), 0x4F8740=UnitInfoZone.showInfos(self?, mainGameZone?)
	class STRUCT_UI_UNIT_INFO_ZONE : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4; // +F4 : font info. (=fontObj+0)
		unsigned long int unknown_0F8; // +F8 : font info. (=fontObj+4)
		unsigned long int unknown_0FC; // +FC : font info. (=fontObj+8)
		STRUCT_SLP_INFO *healthSlp; // +100. Health.shp, slpId=0xC639
		STRUCT_SLP_INFO *itemIconSlp; // +104. itemicon.shp, slpid=0xC62B
		STRUCT_SLP_INFO *iconsForTrainUnits; // +108. Check type (STRUCT_SLP_INFO ok, but is pointer ok ?)
		STRUCT_SLP_INFO **iconsForBuildings; // +10C. ArraySize=5 (tilesets)? Check type (STRUCT_SLP_INFO ok, but is pointer ok ?)
		STRUCT_PLAYER *controlledPlayer; // +110.
		STRUCT_UNIT_BASE *currentUnit; // +114
		long int unknown_118;
		long int unknown_11C; // Init = 0x1F4.
		char unknown_120; // +120. 0,1, 2 (enum ?)
		char unknown_121[3]; // unused?
		STRUCT_UNITDEF_BASE *currentUnitDef; // +124. Unit definition of selected unit
		unsigned long int unknown_128;
		short int currentUnitResourceTypeId; // +12C
		short int unknown_12E;
		float currentUnitResourceAmount; // +130
		char unknown_134;
		char unknown_135[3]; // unused?
		char unknown_138[0x13C - 0x138];
		unsigned long int *unknown_13C; // +13C. Pointer to ? First garrisoned unit ?
		short int garrisonedUnitCount; // +140. To confirm
		short int unknown_142;
		unsigned long int unknown_144;
		unsigned long int unknown_148;
		float currentUnitHP; // +14C
		short int displayedMeleeArmorValue; // +150. Only used to know if refresh is needed, not used for actual display.
		short int displayedPierceArmorValue; // +152. Only used to know if refresh is needed, not used for actual display.
		short int displayedAttack; // +154.  Only used to know if refresh is needed, not used for actual display.
		short int unknown_156; // +156. unused ?
		long int maxRange; // +158. The TOTAL unit range. Only used to know if refresh is needed, not used for actual display.
		float reloadTime1; // +15C. 
		short int unknown_160;
		short int unknown_162;
		short int unknown_164;
		char unknown_166;
		char unknown_167; // unused ?
		char unknown_168[0x1C8 - 0x168];
		short int unknown_1C8;
		char unknown_1CA;
		char unknown_1CB; // unused ?
		long int unknown_1CC;
		float currentUnitLineOfSight; // +1D0
		long int unknown_1D4;
		long int unknown_1D8;
		char currentUnitShortcutNumber; // +1DC
		char unknown_1DD[3];
		STRUCT_PLAYER *currentUnitPlayer; // +1E0.
		unsigned long int unknown_1E4;
		char currentUnitHasAnAction; // +1E8
		char unknown_1E9[3];
		STRUCT_SLP_INFO *unknown_1EC_SLP[9]; // +1EC.
		long int unknown_210; // +210

		bool IsCheckSumValid() { return
			(this->checksum == CHECKSUM_UI_UNIT_INFO_ZONE) || (this->checksum == CHECKSUM_UI_IN_GAME_UNIT_INFO_ZONE);
		}
	};
	static_assert(sizeof(STRUCT_UI_UNIT_INFO_ZONE) == 0x214, "STRUCT_UI_UNIT_INFO_ZONE size");

	// Size 0x21C Constructor 0x4A42A0
	class STRUCT_UI_EDITOR_UNIT_INFO_ZONE : public STRUCT_UI_UNIT_INFO_ZONE {
		long int currentUnitDefId; // +214
		long int currentTileSet; // +218. 0-4.
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_UNIT_INFO_ZONE; }
	};
	static_assert(sizeof(STRUCT_UI_EDITOR_UNIT_INFO_ZONE) == 0x21C, "STRUCT_UI_EDITOR_UNIT_INFO_ZONE size");


	// Size 0x7C8
	// Constructor 0x47D440
	// This is the parent UI object of in-game screen.
	// Pointed by 005830F4
#define CHECKSUM_UI_IN_GAME_MAIN 0x0054679C
	class STRUCT_UI_IN_GAME_MAIN : public STRUCT_ANY_UI { // "Game Screen"
	public:
		long int hWnd; // TO CONFIRM
		char unknown_0F8[0x48C - 0x0F8];
		STRUCT_SLP_INFO *iconsForUnitCommands; // +48C. Pointer to SLP data... Cf SLP 50721
		STRUCT_SLP_INFO *iconsForResearches; // +490. Pointer to SLP data... Cf SLP 50729
		STRUCT_SLP_INFO *iconsForTrainUnits; // +494. Pointer to SLP data... Cf SLP 50730.
		STRUCT_SLP_INFO *iconsForBuildings[5]; // +498 + tileset*4. There is no free slot for more tilesets. SLPId= C610+i except tileset4=>CF0D.
		STRUCT_SLP_INFO *unknown_4AC_icons; // +4AC. Used in 48250F. Includes the "cancel" icon, id=10. SLP 50725 ?
		short int unknown_4B0_iconId; // +4B0. Icon id for Next page button ?
		short int unknown_4B2_iconId; // +4B2. Icon id for unselect button ?
		unsigned long int unknown_4B4;
		STRUCT_UI_PLAYING_ZONE *gamePlayUIZone; // +4B8. 88 66 54 00.
		STRUCT_UI_DIAMOND_MAP *diamondMap; // +4BC. ptr to F4 A3 54 00.
		// 0x4C0
		unsigned long int unknown_4C0; // Struct size=0x118, constructor=4F7C00. 98 9D 54 00
		unsigned long int unknown_4C4;
		STRUCT_UI_BUTTON_WITH_NUMBER *unitCommandButtons[12]; // +4C8. 2 rows of 6 slpButtons for command buttons in bottom center zone.
		STRUCT_UI_BUTTON_WITH_NUMBER *btnChat; // +4F8. Also referred as commandButtons[0xC]. Not visible in SP games.
		STRUCT_UI_BUTTON_WITH_NUMBER *btnDiplomacy; // +4FC. Also referred as commandButtons[0xD]
		STRUCT_UI_BUTTON_WITH_NUMBER *btnMenu; // +500. Also referred as commandButtons[0xE]
		STRUCT_UI_BUTTON_WITH_NUMBER *btnHelp; // +504. Also referred as commandButtons[0xF]
		STRUCT_UI_BUTTON_WITH_NUMBER *btnShowScores; // +508. Also referred as commandButtons[0x10]
		char unknown_50C[0x518 - 0x50C];
		STRUCT_ANY_UI *unknown_518[10]; // cf 47F753
		STRUCT_UI_F11_POP_PANEL *populationInfoPanel; // 0x540. F11 "pop : x/y" zone ?
		STRUCT_UI_IN_GAME_TEXT_ZONE *ingameErrorTextZone; // 0x544
		char unknown_548[0x55C - 0x548];
		STRUCT_UI_IN_GAME_TEXT_ZONE *ingameChatTextZone[8]; // 0x55C ; check count !
		STRUCT_ANY_UI *unknown_57C; // 50 5D 54 00
		char unknown_580[0x5B4 - 0x580];
		STRUCT_GAME_GLOBAL *gameGlobal; // +5B4.
		char unknown_5B8[0x5C8 - 0x5B8];
		STRUCT_UNIT *panelSelectedUnit; // 0x5C8. Pointer to unit selected in unit info zone.
		char unknown_5CC[0x5D0 - 0x5CC];
		STRUCT_UI_UNIT_BUTTON_INFO *tmpUnitBtnCommandsInfo_researches; // +5D0. Pointer to array[]. Only for researches ! Used in internal treatments when showing buttons.
		STRUCT_UI_UNIT_BUTTON_INFO *tmpUnitBtnCommandsInfo_units; // +5D4. Pointer to array[]. Only for train units ! Used in internal treatments when showing buttons.
		unsigned long int unknown_5D8;
		unsigned long int unknown_5DC;
		// 0x5E0
		short int panelDisplayedButtonCount; // Total number of displayed buttons in button bar (unit commands)
		short int panelButtonIdPageOffset; // +5E2. 0=first page, 10=second page. No other possibility in standard game.
		short int panelLastButtonId; // +5E4. Maximum button id for last page.
		short int unknown_5E6; // unused ?
		char unknown_5E8[0x724 - 0x5E8]; // includes unused fields ?
		long int currentChatTextLine;
		char unknown_728[0x734 - 0x728];
		long int unknown_debugText_strlen; // +734
		char unknown_debugText[0x34];
		char shortcutKeySelectTownCenter[4]; // +76C. = 'H' = select TC and center screen.
		char shortcutKeyunused_villagerWork[4]; // +770. = 0 (unused) = Work, for a villager.
		char shortcutKeyMove[4]; // +774. = 'M'. Key to tell a unit to move.
		char shortcutKeyUnused_attack[4]; // +778. = 0 (unused) = Attack a unit (click to select target)
		char shortcutKeyUngroupUnits[4]; // +77C. = 'D' = ungroup units
		char shortcutKeyA_780[4]; // +780. = 'A'. Unused ?
		char shortcutKeyN[4]; // +784. = 'N'
		char shortcutKeyC[4]; // +788. = 'C'
		char shortcutKeyP[4]; // +78C. = 'P'
		char shortcutKeyA_790[4]; // +790. = 'A'
		char shortcutKeyS[4]; // +794. = 'S'
		char shortcutKeyE[4]; // +798. = 'E'
		char shortcutKeyT[4]; // +79C. = 'T'
		char shortcutKeyI[4]; // +7A0. = 'I'
		char unknown_7A4[0x7C4 - 0x7A4];
		long int panelSelectedUnitId; // +7C4. Selected unit ID (in unit details zone)

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_MAIN; }
	};
	static_assert(sizeof(STRUCT_UI_IN_GAME_MAIN) == 0x7C8, "STRUCT_UI_IN_GAME_MAIN size");


	// Size = 0x560. 10 78 54 00. Parent=9C 57 54 00 then BC 4B 54 00.
	// The screen before starting game to choose map type, etc ("advanced" options like map size, etc).
	// Constructor:0x49BF40
#define CHECKSUM_UI_GAME_SETTINGS 0x00547810
	class STRUCT_UI_GAME_SETTINGS : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x478 - 0x0F4];
		STRUCT_ANY_UI *lblScenarioSettings;
		unsigned long int unknown_47C;
		// 0x480
		STRUCT_ANY_UI *lblMapSize;
		STRUCT_ANY_UI *edtMapSize; // drop down
		STRUCT_ANY_UI *lblMapType;
		STRUCT_ANY_UI *edtMapType;
		// 0x490
		STRUCT_ANY_UI *lblVictoryCondition;
		STRUCT_ANY_UI *edtVictoryCondition;
		STRUCT_ANY_UI *lblScContainsIndividualVictoryConditions;
		unsigned long int unknown_49C;
		// 0x4A0
		STRUCT_ANY_UI *lblScore;
		STRUCT_ANY_UI *edtScore;
		STRUCT_ANY_UI *lblTimeLimit;
		STRUCT_ANY_UI *edtTimeLimit;
		// 0x4B0
		STRUCT_ANY_UI *lblDifficultyLevel;
		STRUCT_ANY_UI *edtDifficultyLevel;
		STRUCT_ANY_UI *lblStartingAge;
		STRUCT_ANY_UI *edtStartingAge;
		// 0x4C0
		STRUCT_ANY_UI *lblResources;
		STRUCT_ANY_UI *edtResources;
		STRUCT_ANY_UI *lblPathFinding;
		STRUCT_ANY_UI *edtPathFinding; // checkbox
		// 0x4D0
		STRUCT_ANY_UI *lblPopLimit;
		STRUCT_ANY_UI *edtPopLimit;
		STRUCT_ANY_UI *lblUnknown_4D8;
		STRUCT_ANY_UI *edtUnknown_4DC;
		// 0x4E0
		long int scenarioListPosX;
		long int scenarioListPosY;
		long int scenarioListSizeX;
		long int scenarioListSizeY;
		// 0x4F0
		STRUCT_ANY_UI *edtScenarioList;
		unsigned long int unknown_4F4;
		unsigned long int unknown_4F8;
		unsigned long int unknown_4FC;
		// 0x500
		unsigned long int unknown_500;
		unsigned long int unknown_504;
		STRUCT_ANY_UI *lblScenarioInstructions;
		unsigned long int unknown_50C;
		// 0x510
		char unknown_510[0x524 - 0x510];
		STRUCT_ANY_UI *edtFixedPositions; // +524
		STRUCT_ANY_UI *edtRevealMap;
		STRUCT_ANY_UI *edtFullTechTree;
		// 0x530
		STRUCT_ANY_UI *edtEnableCheatMode;
		STRUCT_ANY_UI *lblFixedPositions;
		STRUCT_ANY_UI *lblRevealMap;
		STRUCT_ANY_UI *lblFullTechTree;
		// 0x540
		STRUCT_ANY_UI *lblEnableCheatMode;
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnCancel;
		STRUCT_UI_BUTTON *btnHelp; // "?" button
		// 0x550
		unsigned long int unknown_550;
		unsigned long int unknown_554;
		unsigned long int unknown_558;
		unsigned long int unknown_55C;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_GAME_SETTINGS; }
	};

	// In-game diplomacy menu
	// Size = 0x6B8
#define CHECKSUM_UI_DIPLOMACY_DIALOG 0x00543814
	class STRUCT_UI_DIPLOMACY_DIALOG : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x490 - 0x0F4];
		// 0x490
		STRUCT_ANY_UI *lblDiplomacy;
		STRUCT_ANY_UI *lblName;
		STRUCT_ANY_UI *lblCivilization;
		STRUCT_ANY_UI *lblAlly;
		// 0x4A0
		STRUCT_ANY_UI *lblNeutral;
		STRUCT_ANY_UI *lblEnemy;
		STRUCT_ANY_UI *lblPayTribute;
		STRUCT_ANY_UI *lblAlliedVictory;
		// 0x4B0
		char unknown_4B0[0x510 - 0x4B0];
		// 0x510
		STRUCT_ANY_UI *lblNeedMarketToPayTribute;
		char unknown_514[0x5F4 - 0x514];
		STRUCT_ANY_UI *edtAlliedVictory; // +5F4. Checkbox
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnClearTributes;
		// 0x600
		STRUCT_UI_BUTTON *btnCancel;
		unsigned long int unknown_604;
		char unknown_608[0x620 - 0x608];
		// 0x620
		STRUCT_ANY_UI *lblFoodTribute;
		STRUCT_ANY_UI *lblGoldTribute;
		STRUCT_ANY_UI *lblWoodTribute;
		STRUCT_ANY_UI *lblStoneTribute;
		// 0x630
		char unknown_630[0x6B8 - 0x630];

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_DIPLOMACY_DIALOG; }
	};


	// Size=0x564. In-game Options popup.
	// Constructor = 0x4300D0
#define CHECKSUM_UI_IN_GAME_OPTIONS 0x005436E0
	class STRUCT_UI_IN_GAME_OPTIONS : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x490 - 0x0F4];
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
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_OPTIONS; }
	};


	// Size ?
	class STRUCT_UI_LOAD_SAVED_GAME : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x478 - 0x0F4];
		STRUCT_ANY_UI *lblSavedGames;
		STRUCT_ANY_UI *lblSelectSavedGame;
		// 0x480
		STRUCT_ANY_UI *edtSavedGameList;
		STRUCT_ANY_UI *unknown_484; // type a name for the game?
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnCancel;
		// 0x490
		STRUCT_UI_BUTTON *btnDelete;

		//bool IsCheckSumValid() { return this->checksum == 0x0054xxxx; }
	};


	// Size - Constructor = 
	// This is the "welcome" screen with 3 buttons (NOT menu in scenario editor screen)
	class STRUCT_UI_SCENARIO_EDITOR_MAIN_MENU : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x478 - 0x0F4];
		STRUCT_ANY_UI *lblScenarioBuilder;
		STRUCT_UI_BUTTON *btnCreateScenario;
		// 0x480
		STRUCT_UI_BUTTON *btnEditScenario;
		STRUCT_UI_BUTTON *btnCampaignEditor;
		STRUCT_UI_BUTTON *btnCancel;
		STRUCT_UI_BUTTON *btnUnknown_48C;

		//bool IsCheckSumValid() { return this->checksum == ; }
	};


	// Size 0x4B4 - Constructor = 0x4349A0 - 20 3E 54 00
	// "Scenario Menu Dialog"
#define CHECKSUM_UI_SCENARIO_EDITOR_MENU 0x00543E20
	class STRUCT_UI_SCENARIO_EDITOR_MENU : public STRUCT_ANY_UI {
	public:
		// +F4...
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MENU; }
	};


	// Size ?
	class STRUCT_UI_CAMPAIGN_EDITOR : public STRUCT_ANY_UI {
	public:
		char unknown_0F4[0x47C - 0x0F4];
		STRUCT_ANY_UI *lblCampaignEditor;
		// 0x480
		STRUCT_ANY_UI *lblCampaignFilename;
		STRUCT_ANY_UI *lblScenarios;
		STRUCT_ANY_UI *lblCampaignScenarios;
		unsigned long int unknown_48C;
		// 0x490
		unsigned long int unknown_490;
		unsigned long int unknown_494;
		unsigned long int unknown_498;
		STRUCT_ANY_UI *unknown_49C_defaultCampaignInputDialog;
		// 0x4A0
		STRUCT_UI_BUTTON *btnOK;
		STRUCT_UI_BUTTON *btnCancel;
		STRUCT_UI_BUTTON *btnSave;
		STRUCT_UI_BUTTON *btnAdd;
		// 0x4B0
		STRUCT_UI_BUTTON *btnRemove;
		STRUCT_UI_BUTTON *btnUp;
		STRUCT_UI_BUTTON *btnDown;

		//bool IsCheckSumValid() { return this->checksum == 0x0054xxxx; }
	};


	class STRUCT_UI_DIAM_MAP : public STRUCT_ANY_UI { // F4 A3 54 00
	public:
		char unknown_0F4[0x130 - 0x0F4];
		// 0x130
		short int mapSizeX; // unsure
		short int mapSizeY; // unsure
		

		bool IsCheckSumValid() { return this->checksum == 0x0054A3F4; }
	};

	// Size = 0x4B0. Constructor = 0x47B440. 08 62 54 00
	// "Join Screen" = MP search game screen.
	class STRUCT_UI_JOIN_SCREEN : public STRUCT_ANY_UI {
	public:
		// unknown...
		bool IsCheckSumValid() { return this->checksum == 0x00546208; }
	};

	// Size = . Constructor = . F8 69 54 00
	// OnKeyPress = 0x486C10
	class STRUCT_UI_WELCOME_MAIN_SCREEN : public STRUCT_ANY_UI {
	public:
		// unknown...
		bool IsCheckSumValid() { return this->checksum == 0x005469F8; }
	};

	// Size = 0x8B8. Constructor = 0x47C510. 68 64 54 00
	class STRUCT_UI_CREDITS_SCREEN : public STRUCT_ANY_UI {
	public:
		// unknown...
		bool IsCheckSumValid() { return this->checksum == 0x00546468; }
	};


	static long int GetUnitStructRealSize(AOE_STRUCTURES::STRUCT_UNIT_BASE *unit) {
		if (unit->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE);
		if (((STRUCT_UNIT_BIRD*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_FLAG*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_FLAG);
		if (((STRUCT_UNIT_DEAD_FISH*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DEAD_FISH);
		if (((STRUCT_UNIT_DOPPLEGANGER*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DOPPLEGANGER);
		if (((STRUCT_UNIT_LIVING*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_LIVING);
		if (((STRUCT_UNIT_PROJECTILE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE);
		if (((STRUCT_UNIT_TREE*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_TYPE50*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_TYPE50);
		if (((STRUCT_UNIT_BUILDING*)unit)->IsTypeValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING);
		// unit->unitType is often erroneous ! If not found, try again using only checksum.
		if (unit->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BASE);
		if (((STRUCT_UNIT_BIRD*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_FLAG*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_FLAG);
		if (((STRUCT_UNIT_DEAD_FISH*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DEAD_FISH);
		if (((STRUCT_UNIT_DOPPLEGANGER*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_DOPPLEGANGER);
		if (((STRUCT_UNIT_LIVING*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_LIVING);
		if (((STRUCT_UNIT_PROJECTILE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_PROJECTILE);
		if (((STRUCT_UNIT_TREE*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BIRD);
		if (((STRUCT_UNIT_TYPE50*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_TYPE50);
		if (((STRUCT_UNIT_BUILDING*)unit)->IsCheckSumValid()) return sizeof(AOE_STRUCTURES::STRUCT_UNIT_BUILDING);
		return 0;
	}

}

