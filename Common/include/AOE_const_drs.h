#pragma once

// This file contains IDs corresponding to DRS objects from AOE standard game

namespace AOE_CONST_DRS {

	enum AoeScreenTheme
	{
		RedTheme = 50002, // Weird looking but not so bad !
		SalmonWeirdTheme = 50003, // The background design is really weird, but font and UI components are nice (checkbox are special but ok)
		LightOrangeBrownTheme = 50004, // No background image (which can be useful), working well. Brown on top to light brown in the bottom (with yellow/orange touch). The white font is looking weird sometimes.
		FullBlackTheme = 50005, // Completely black background, very effective. All UI components are working well/well supported. A nice alternative.
		EgyptNeutralScreensLightOrangeGrandmasWallpaper = 50006, // An imitation of my grandma's home wallpaper. Used for egypt tileset in save/load dialogs
		GreekNeutralScreensGreyTheme = 50007, // A quite simple - but very nice - theme. Used for greek tileset in save/load dialogs
		BabylonNeutralScreensGreyBricks = 50008, // A slightly darker grey theme, with kind of bricks in the background. Nice too but we don't really see the checkboxes. Used for babylon tileset in save/load dialogs
		AsianNeutralScreensYellowOrangeTheme = 50009, // Checkboxes arent easy to see (but work). Used for asian tileset in save/load dialogs
		GreyDarkGreenTheme = 50010, // No background image here. A bit unusual color/theme, and checkboxes are totally crappy.
		DarkOrangeTheme = 50011, // No background image here. The color gives me headache. Very agressive. But works well.
		LightOrangeTheme = 50012, // No background image here. A bit unusual color/theme, but components and fonts look great here.
		RomanNeutralScreensGreyTheme = 50013, // No background image here. Used for roman tileset in save/load dialogs Similar to greek one (a bit darker maybe).

		// Background image themes
		WelcomeMenuTheme = 50051, // Warning: checkboxes are not correctly supported
		GameSettingsTheme = 50052,
		MpSettingsTheme = 50053,
		UnknownTheme54 = 50054, // Scenario editor menus ? Nice but background image went through the washing machine
		InGameOptionsTheme = 50055, // clear-sand colored ; warning, this may not support high resolutions unless you fix the DRS file
		UnknownTheme56 = 50056, // Black with unreadable font and unsupported checkboxes, to be avoided (many IDs are -1 in this "bina")
		VictoryAchievementTheme = 50057, // Using this for custom screens might look weird ;)
		LostAchievementTheme = 50058, // Using this for custom screens might look weird ;)
		CreditsScreenTheme = 50059, // Most components are unsupported, do not use this
		ScenarioInfoTheme = 50060, // Nice black theme with nice UI components, but the image spoils the thing a bit
		InGameAchievementTheme = 50061, // Using this for custom screens might look a bit weird. Still better than victory once though !
	};

	// Tileset-related
	const long int SLPID_TILESET_SCREEN_THEME_BASE_STD = 50006; // 0xC356. For tilesets 0-3
	const long int SLPID_TILESET_SCREEN_THEME_ROR = 53000; // 0xC356. For tileset 4 (ROR)
	const long int SLPID_TILESET_CHECKBOXES_BASE_STD = 50605; // 0xC5AD. For tilesets 0-3 (btn6_%d)
	const long int SLPID_TILESET_CHECKBOXES_ROR = 53004; // 0xCF0C. For tileset 4 (ROR)
	const long int SLPID_TILESET_BLD_ICONS_BASE_STD = 50704; // 0xC610. For tilesets 0-3
	const long int SLPID_TILESET_BLD_ICONS_ROR = 53005; // 0xCF0D. For tileset 4 (ROR)
	const long int SLPID_TILESET_BUTTON_BOARD_LOW_RESOLUTION_BASE_STD = 50713; // 0xC619. For tilesets 0-3. See 0x481A64. (btnbrda%d.shp)
	const long int SLPID_TILESET_BUTTON_BOARD_LOW_RESOLUTION_ROR = 53006; // 0xCF0E. For tileset 4 (ROR)
	const long int SLPID_TILESET_BUTTON_BOARD_MEDIUM_RESOLUTION_BASE_STD = 50717; // 0xC61D. For tilesets 0-3. (btnbrdb%d.shp)
	const long int SLPID_TILESET_BUTTON_BOARD_MEDIUM_RESOLUTION_ROR = 53007; // 0xCF0F. For tileset 4 (ROR)
	const long int SLPID_TILESET_BUTTON_BOARD_HIGH_RESOLUTION_BASE_STD = 50747; // 0xC63B. For tilesets 0-3. (btnbrdc%d.shp)
	const long int SLPID_TILESET_BUTTON_BOARD_HIGH_RESOLUTION_ROR = 53008; // 0xCF10. For tileset 4 (ROR)
	const long int SLPID_TILESET_COMMON_CMD_ICONS_BASE_STD = 50725; // C625. For tilesets 0-3. See 0x481B08 (btnoth%d.shp)
	const long int SLPID_TILESET_COMMON_CMD_ICONS_ROR = 53009; // 0xCF11. For tileset 4 (ROR)
	const long int SLPID_TILESET_GAME_FRIEZES_LOW_RESOLUTION_BASE_STD = 50733; // 0xC62D. For tilesets 0-3. gamea%d.shp
	const long int SLPID_TILESET_GAME_FRIEZES_LOW_RESOLUTION_ROR = 53010; // 0xCF12. For tileset 4 (ROR). gamea%d.shp
	const long int SLPID_TILESET_GAME_FRIEZES_MEDIUM_RESOLUTION_BASE_STD = 50737; // 0xC631. For tilesets 0-3. gameb%d.shp
	const long int SLPID_TILESET_GAME_FRIEZES_MEDIUM_RESOLUTION_ROR = 53011; // 0xCF13. For tileset 4 (ROR). gameb%d.shp
	const long int SLPID_TILESET_GAME_FRIEZES_HIGH_RESOLUTION_BASE_STD = 50741; // 0xC635. For tilesets 0-3. gamec%d.shp
	const long int SLPID_TILESET_GAME_FRIEZES_HIGH_RESOLUTION_ROR = 53012; // 0xCF14. For tileset 4 (ROR). gamec%d.shp

	// Icon IDs for button bar: common commands. For SLPID=50721
	enum AoeButtonBarCommonCommandsIconId : long int {
		BBC_ICON_INVALID_NONE = -1,
		BBC_ICON_REPAIR = 0,
		BBC_ICON_MOVE = 1, // Only used in early versions of AOE. A Foot with a red arrow
		BBC_ICON_BUILD = 2,
		BBC_ICON_HAND_STOP = 3,
		BBC_ICON_HOLD_POSITION = 4,
		BBC_ICON_UNLOAD_TRANSPORT = 5,
		BBC_ICON_UNKNOWN_BLUE_FLAG = 6, // Unused in standard game
		BBC_ICON_REGROUP = 7,
		BBC_ICON_UNGROUP = 8,
		BBC_ICON_UNKNOWN_09_BLACK = 9, // Black (empty)
		BBC_ICON_CANCEL_SELECTION = 10, // 0x0A. Red cross
		BBC_ICON_NEXT_PAGE = 11,
		BBC_ICON_ATTACK_POSITION = 12, // 0x0C
		BBC_ICON_HEAL = 13, // 0x0D
		BBC_ICON_CONVERT = 14, // 0x0E
		BBC_ICON_STANDARD_COUNT
		/*BBC_ICON_TRADE_WOOD_FOR_GOLD = 0x56,
		BBC_ICON_TRADE_FOOD_FOR_GOLD = 0x57,
		BBC_ICON_TRADE_STONE_FOR_GOLD = 0x58*/
	};

	// Icon IDs for unit info zone: For SLPID=50731
	enum AoeUnitInfoZoneIconId : long int {
		UIZ_ICON_WOOD = 0,
		UIZ_ICON_STONE = 1,
		UIZ_ICON_FOOD = 2,
		UIZ_ICON_GOLD = 3,
		UIZ_ICON_POPULATION = 4, // Looks like convert icon !
		UIZ_ICON_TRADE = 5,
		UIZ_ICON_RANGE = 6, // (arrow in a target)
		UIZ_ICON_SWORD = 7, // Attack
		UIZ_ICON_ARMOR = 8,
		UIZ_ICON_TIME = 9, // Used for priest faith
		UIZ_ICON_SHIELD = 10, // For range armor
		UIZ_ICON_STANDARD_COUNT
	};

	// Icon IDs for unit info zone: For SLPID=50731
	enum AoeResourceTypesIconId : long int {
		RT_ICON_WOOD = 0,
		RT_ICON_STONE = 1,
		RT_ICON_FOOD = 2,
		RT_ICON_GOLD = 3,
	};

	enum AoeInGameFlagsIconId : long int {
		IGF_SHORTCUT_NUMBERS = 0,
		IGF_MOVETO_INTERMEDIATE_STEP_FLAG = 1,
		IGF_MOVETO_RED_CROSS = 2
	};

	// SLP 51000 = mouse cursors: see GAME_CURSOR in AOE_const_internal.

	// Icon IDs for buildings. For SLPID=50704-50707 and 53005 (cf tileset)
	enum AoeBuildingIconId : long int {
		BLD_ICON_ACADEMY = 0,
		BLD_ICON_RANGE_TOOL = 1,
		BLD_ICON_RANGE_BRONZE = 2,
		BLD_ICON_BARRACKS_TOOL = 3,
		BLD_ICON_BARRACKS_BRONZE = 4,
		BLD_ICON_BARRACKS_IRON = 5,
		BLD_ICON_DOCK_STONE = 6,
		BLD_ICON_DOCK_BRONZE = 7,
		BLD_ICON_DOCK_IRON = 8,
		BLD_ICON_SIEGE_WORKSHOP = 9,
		BLD_ICON_FARM = 10,
		BLD_ICON_GRANARY_STONE = 11,
		BLD_ICON_GRANARY_IRON = 12,
		BLD_ICON_GUARD_TOWER = 13,
		BLD_ICON_SENTRY_TOWER = 14,
		BLD_ICON_HOUSE_STONE = 15,
		BLD_ICON_HOUSE_TOOL = 16,
		BLD_ICON_HOUSE_BRONZE = 17,
		BLD_ICON_HOUSE_IRON = 18,
		BLD_ICON_GOVERNMENT_CENTER_BRONZE = 19,
		BLD_ICON_GOVERNMENT_CENTER_IRON = 20,
		BLD_ICON_MARKET_TOOL = 21,
		BLD_ICON_MARKET_BRONZE = 22,
		BLD_ICON_MARKET_IRON = 23,
		BLD_ICON_STABLE_IRON = 24,
		BLD_ICON_STABLE_TOOL = 25,
		BLD_ICON_WATCH_TOWER = 26,
		BLD_ICON_WONDER = 27,
		BLD_ICON_STORAGE_PIT_STONE = 28,
		BLD_ICON_STORAGE_PIT_BRONZE = 29,
		BLD_ICON_STORAGE_PIT_IRON = 30,
		BLD_ICON_TEMPLE_BRONZE = 31,
		BLD_ICON_TEMPLE_IRON = 32,
		BLD_ICON_TOWN_CENTER_STONE_1 = 33,
		BLD_ICON_TOWN_CENTER_STONE_2 = 34, // Unused ?
		BLD_ICON_TOWN_CENTER_TOOL = 35,
		BLD_ICON_TOWN_CENTER_BRONZE = 36,
		BLD_ICON_TOWN_CENTER_UNKNOWN = 37, // Unused
		BLD_ICON_TOWN_CENTER_IRON = 38,
		BLD_ICON_UNKNOWN_39 = 39,
		BLD_ICON_UNKNOWN_40 = 40,
		BLD_ICON_TRADE_WORKSHOP = 41,
		BLD_ICON_SMALL_WALL = 42,
		BLD_ICON_UNKNOWN_43 = 43,
		BLD_ICON_UNKNOWN_44 = 44,
		BLD_ICON_MEDIUM_WALL = 45,
		BLD_ICON_UNKNOWN_46 = 46,
		BLD_ICON_UNKNOWN_47 = 47,
		BLD_ICON_FORTIFICATION = 48,
		BLD_ICON_BALLISTA_TOWER = 49
	};
}

