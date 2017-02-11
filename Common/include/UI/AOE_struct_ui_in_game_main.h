
#pragma once

#include <UI\AOE_struct_ui_screen_base.h>
#include <UI_components\AOE_struct_any_ui.h>
#include <UI_components\AOE_struct_ui_button_with_number.h>
#include <UI_components\AOE_struct_ui_label.h>
#include <UI\AOE_struct_ui_playing_zone.h>
#include <UI\AOE_struct_ui_diamond_map.h>
#include <UI\AOE_struct_ui_f11_pop_panel.h>
#include <UI_components\AOE_struct_ui_gameColoredInfoLabel.h>
#include <AOE_structures_drs.h>
#include <AOE_struct_units.h>
#include <UI\AOE_struct_ui_player_resource_values.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{
	
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


	// Size 0x7C8 - Constructor = 0x47D440. parent object = 9C 57 54 00, common with scenario editor
	// This is the parent UI object of in-game screen.
	// Pointed by 005830F4
	class STRUCT_UI_IN_GAME_MAIN : public STRUCT_UI_SCREEN_BASE { // "Game Screen"
	public:
		STRUCT_SLP_INFO *gameTopAndBottomFriezes; // +478. Top and down friezes, depending on both resolution and tileset. gameX.shp (X=a/b or c)
		STRUCT_SLP_INFO *buttonBoardResolutionA; // +47C. btnbrda%d.shp. For lowest resolution.
		STRUCT_SLP_INFO *iconsForOtherButtons; // +480. btnoth%d.shp
		STRUCT_SLP_INFO *buttonBoardResolutionB; // +484. btnbrdb%d.shp. For intermediate resolution.
		STRUCT_SLP_INFO *buttonBoardResolutionC; // +488. btnbrdc%d.shp. For highest resolution.
		STRUCT_SLP_INFO *iconsForUnitCommands; // +48C. Pointer to SLP data... Cf SLP 50721
		STRUCT_SLP_INFO *iconsForResearches; // +490. Pointer to SLP data... Cf SLP 50729
		STRUCT_SLP_INFO *iconsForTrainUnits; // +494. Pointer to SLP data... Cf SLP 50730.
		STRUCT_SLP_INFO *iconsForBuildings[5]; // +498 + tileset*4. There is no free slot for more tilesets. SLPId= C610+i except tileset4=>CF0D.
		STRUCT_SLP_INFO *iconsNextCancelForCurrentResolution; // +4AC. Updated according to tileset/resolution. Used in 48250F. "cancel" icon(10), "next" (id?). SLP 50725-50728 or 53009. In low resolution, uses smaller icons to avoid conflict with map.
		short int nextPageIconId; // +4B0. Icon id for Next page button ?
		short int unknown_4B2_iconId; // +4B2. Icon id for unselect button ?
		short int unknown_4B4; // +4B4. Icon id ?
		short int unknown_4B6;
		STRUCT_UI_PLAYING_ZONE *gamePlayUIZone; // +4B8. 88 66 54 00.
		STRUCT_UI_DIAMOND_MAP *diamondMap; // +4BC. ptr to F4 A3 54 00.
		// 0x4C0
		STRUCT_UI_PLAYER_RESOURCE_VALUES *resourceValuesIndicator; // +4C0. The resource values (food/wood/stone/gold) on top. NOT the corresponding icons ?
		unsigned long int unknown_4C4;
		STRUCT_UI_BUTTON_WITH_NUMBER *unitCommandButtons[12]; // +4C8. 2 rows of 6 slpButtons for command buttons in bottom center zone.
		STRUCT_UI_BUTTON_WITH_NUMBER *btnChat; // +4F8. Also referred as commandButtons[0xC]. Not visible in SP games.
		STRUCT_UI_BUTTON_WITH_NUMBER *btnDiplomacy; // +4FC. Also referred as commandButtons[0xD]
		STRUCT_UI_BUTTON_WITH_NUMBER *btnMenu; // +500. Also referred as commandButtons[0xE]
		STRUCT_UI_BUTTON_WITH_NUMBER *btnHelp; // +504. Also referred as commandButtons[0xF]
		STRUCT_UI_BUTTON_WITH_NUMBER *btnShowScores; // +508. Also referred as commandButtons[0x10]
		char unknown_50C[0x514 - 0x50C];
		STRUCT_GAME_COLORED_INFO_LABEL *topLeftInfoLabel; // +514. The left part (game time + speed) of F11 infos.
		STRUCT_GAME_COLORED_INFO_LABEL *victoryTimersInfoLabels[10]; // +518. cf 0x47F753,0x4839EA
		STRUCT_UI_F11_POP_LABEL *populationInfoPanel; // 0x540. F11 "pop : x/y" zone ?
		STRUCT_UI_IN_GAME_TEXT_ZONE *ingameErrorTextZone[6]; // 0x544. 6 lines for orange bottom (left or centered) messages. Reverse order (index0=bottom)
		STRUCT_UI_IN_GAME_TEXT_ZONE *ingameChatTextZone[8]; // 0x55C ; check count !
		STRUCT_ANY_UI *unknown_57C; // 50 5D 54 00
		STRUCT_UI_LABEL *lblCurrentAge; // +580. The current age indication on top (centered).
		STRUCT_ANY_UI *unknown_584;
		STRUCT_UI_LABEL *lblF5debugInfo; // +588. The text zone displayed instead of resource indicators.
		char unknown_58C[0x5B4 - 0x58C];
		STRUCT_GAME_GLOBAL *gameGlobal; // +5B4.
		char unknown_5B8[0x5C8 - 0x5B8];
		STRUCT_UNIT_BASE *panelSelectedUnit; // 0x5C8. Pointer to unit selected in unit info zone.
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
		char unknown_5E8[0x714 - 0x5E8]; // includes unused fields ?
		long int tileset; // +714
		long int tilesetRelatedGraphicsSizeX; // +718. "this" (gamemainui) sizeX when tileset-dependent graphics were last loaded
		long int unknown_71C; // +71C
		unsigned long int unknown_720;
		long int currentChatTextLine; // +724
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

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_UI_IN_GAME_MAIN; }
	};
	static_assert(sizeof(STRUCT_UI_IN_GAME_MAIN) == 0x7C8, "STRUCT_UI_IN_GAME_MAIN size");

}

