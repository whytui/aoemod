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

}

