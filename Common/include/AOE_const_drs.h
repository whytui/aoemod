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


	// Bottom friezes
	// 50734 to 50744 (AOE) + 53010, 53011, 53012 (ROR)
}

