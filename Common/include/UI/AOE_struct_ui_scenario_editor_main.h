
#pragma once

#include <UI\AOE_struct_ui_screen_base.h>
#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_button.h>
#include <UI_components\AOE_struct_ui_label.h>
#include <UI_components\AOE_struct_ui_combobox.h>
#include <UI\AOE_struct_ui_diamond_map.h>
#include <UI\AOE_struct_ui_playing_zone.h>
#include <UI\AOE_struct_ui_editor_unit_info_zone.h>
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

	// Size 0x948. Constructor 0x48FC40. Inherits from 9C 57 54 00, then screen_base
	// This is the parent UI object of in-game screen.
	class STRUCT_UI_SCENARIO_EDITOR_MAIN : public STRUCT_UI_SCREEN_BASE { // 60 73 54 00
	public:
		char unknown_478[0x4A8 - 0x478];
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
		STRUCT_UI_COMBOBOX *map_cbb_defaultTerrain; // +528. Combobox
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

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_SCENARIO_EDITOR_MAIN; }
	};
	static_assert(sizeof(STRUCT_UI_SCENARIO_EDITOR_MAIN) == 0x948, "STRUCT_UI_SCENARIO_EDITOR_MAIN size");


}

