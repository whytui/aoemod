CustomROR is a plugin for Rise of Rome. It requires ROR_API.

Installation
- Extract all files to a directory of your choice (example C:\temp\customROR\): ROR_API.dll, ROR_API.conf, CustomROR.dll, CustomROR.xml
- Get latest CustomizeAOE version from http://aoe.heavengames.com/dl-php/showfile.php?fileid=2464
- Run CustomizeAOE and click Menu>File>Install CustomROR.
- Select C:\temp\customROR\CustomROR.dll and C:\your_aoe_install_dir\EmpiresX.exe (the game file you want to patch).
  (note: you can copy-paste EmpiresX.exe to EmpiresX2.exe and patch this file to keep EmpiresX.exe unchanged)

Notes:
- Several options can be configured in customROR.xml, like conversion resistances.
- Custom civilizations can be configured in customROR_civs.xml.
=> Edit XML files with a text editor (with syntax coloration if possible, for example Notepad++)
- Use CustomizeAOE to change other options if you want. Ths installation process should already have patched CustomROR-required options (including ROR_API).
- CustomROR will also work if the EXE you patch is not named EmpiresX.exe (for example, Copy of EmpiresX.exe).

Features:
---------

* Triggers system (see separate documentation)
* Add up to 255 civilizations (total) + configure AI, names... (see customROR_civs.xml)
* Most AI improvements (see below) are NOT applied in easy/easiest difficulty levels.

Strategy (AI):
- Fix AI when loading saved games: some information is lost in standard game, especially if you save a game in the very first seconds.
- Fix stuck AI bug #1: missing unitId in computer players strategy for unfinished buildings.
- Fix stuck AI bug #2: strategy when an unfinished building is destroyed (computer player will no longer get stuck).
- Fix stuck AI bug #3: strategy when a building that is currently training a unit is destroyed (computer player will no longer get stuck).
- Fix stuck AI bug #4: when entering "panic mode", too much irrelevant units are added on top of strategy and AI gets stuck.
- Dramatically improve defence AI: train relevant units according to attacking units and accoding to player's available units.
- Automatically adapt AI strategy to maximum population
- Improved tactical AI updates:
 - Fix conversion & strategy: When enemy units are converted, strategy is updated to avoid training/building them twice.
 - Disable irrelevant limited-retrain units (like clubmen) when starting at iron age or post-iron.
 - Unblock "stuck buildings". It is necessary to wait a moment before doing this or the player will lose all his resources trying to build something that is "instant-killed" by the enemy.
 - Add some useful (available) researches if the player is rich, well developed (for example, wheel when it's not included in player's strategy).
 - TO DO - Disable useless building (governement center) if it is no longer useful (all underlying techs researched, iron age already researched).
 - TO DO - Improve farms management (lower their priority in AI list).
 - When switching player, AI is updated to take into account human-created units (avoid re-building/training duplicates).
 - Also detect dynamically-added units (houses, boats) in scenarios so that AI player does not create them twice.
 - Fix dynamically-added elements dependencies issues (trireme research before iron age...)
 - Set a maximum number of farms in deathmatch games. AI builds too much farms in deathmatch (SNMaxFarms).
 - Replace SNScalingFrequency unit from minutes to seconds (low values will automatically be fixed to prevent issues).
 - Wait for requirements before building a wonder: make sure to have extra resource (SNMinimumFood, etc), make sure previous strategy elements are done...

Town building (AI):
- Faster town building at game startup in deathmatch using appropriate SN numbers (SNRequiredFirstBuilding, SNInitialExplorationRequired).
- Improve some city plan calculations to prevent villagers from being stuck and improve access to TC (+granary/SP) for farmers/gatherers...
 - Always protect Town center (and granary/storage pit) immediate neighborhood from being built (may occur when using custom city plan configurations)
 - Improve granary location choice (closer from most of the target bushes)
 - Avoid construction of many consecutive buildings in a row that leave no way for living units
 - Fix a bug in "building history" that sometimes prevent AI from constructing again some buildings (when destroyed)

Units behavior (AI)
- Improve units targeting / behavior in battle
 - AI units no longer hesitate between several targets (including 2-priest trick).
 - Fix stuck priests (enemy tower is in AI's town, unfinished enemy buildings, hesitations when attacked while converting, etc).
 - Improve/customize units choice/behaviour when an enemy tower is in AI's town. Dramatic AI improvement, especially for villagers.
 - Fix stuck unit groups that try to attack their own units ! (they do nothing because they can't attack their target)

UI and Shortcuts:
- Farms can be automatically rebuilt (see configuration + custom options menu in game)
- Add 10 shortcut numbers 11-20 using numpad (original game uses 1-10 with main keyboard numeric keys). Custom numbers are not displayed (like 0) due to SLP limitation.
- Improved game speed change with +/- buttons. Choose the speed change factor (default 1.25) in XML config or in-game customROR menu.
- Press F9 to call nearby idle military units (they will move to mouse position).
- Automatically assign shortcut numbers to spawned units (to configure in customROR.xml file).
- Select a building and right-click to a location so that trained units will automatically spawn there (right-click on building itself to cancel).
- A new button in game menu opens CustomROR options UI.
- Show "remaining" available technologies (from you civ's tech tree) from CustomROR options UI (tech tree button).
- Automatically show "F11" information at game startup.
- "F11" information zone displays enhanced population information (including number of vilagers)
- Display a chat message at game startup to say the plugin is active.
- Press CTRL-F1 to display trace/debug messages (useful for triggers...)
- Support more than 1 page for buttons (similar to villager build list)

Gameplay & configuration:
- Enhanced/configurable "Auto-attack nearby units" behaviour for military units (some kind of enhanced "hold position" feature). For example, you can prevent your catapults from attacking villagers !
- Use customROR.xml to configure (for RM/DM) initial resource values, initial resource bonus for AI
- Use customROR.xml to force certain PER numbers in DM and/or RM. Example: set SNAllowDiplomacyChangeOnAllyAttack to 0 to avoid unfortunate diplomacy changes.
- Enable/Disable (customROR.xml) neutral initial diplomacy stance: this prevents computer players to become all allied against human player.
- Enable/disable shared exploration (writing tech) at game start (to configure in customROR.xml file).
- Ability to customize number of relics/ruins in random games/deathmatch.
- Automatically computes AI's "dislike" values against each player. A player with all relics/ruins/wonder will have more chances to be taken as target.
- Change conversion resistances, like 1.25 for war elephants instead of 1 (customizable in config file).
- Correctly sets AI control at game startup when using the "ManageAI" feature (allows AI to control your "old" player when changing with gaia cheat code).
- Customize Map generation elevation "amount" (hardcoded in standard game)
- Disable dock in maps where AI does NOT build docks, for fair SP games. You can turn this ON/OFF in customROR.xml file.

Scenario Editor:
- All terrains are available in editor (putting non-standard terrain does not refresh immediatly, but it works).
- All units are visible in editor (can be turned off in CustomROR configuration file)
- In scenario editor, F10 opens the menu.
- Fixed scenario editor so that it uses correct building set for each player (if you change a civilization, you need to save and reload the scenario)
- Edit unit popup (can edit unit status): press F1 to switch to "select" mode, F2 to edit selected unit.
- Show a number on selected units to indicate units that have a custom status (changed with F2 popup).
- Enhanced scenario editor options (triggers, get AI/PER files, edit terrain, ...): press F3
- Generate maps with any custom dimensions (X can differ from Y), compatible with all map generation modes.
- Fix to preserve unit status when loading a scenario file + display fix.

Fixes:
- Fix timer bug on fast computers (game freezes in low game speeds).
- Fix bug with reveal map option in game settings (does not applies correctly to all players). Bug can be observed with FULL_computerPlayer1 or when switching players with gaia cheat code.
- Fix SNRequiredFirstBuilding handling (was not working correctly).
- Fix fire galley icon: move fire galley icon when trireme research is completed IF player has both fire galley and catapult trireme. Can now be trained in All techs games !
--> To use fire galley in "All tech" games, you need to fix empires.dat with AGE3: remove the disable effect from technology 210 ("All techs" tech tree).
- Fix Crash to desktop bug: due to 2 bugs with building conversion (building list not updated for both players).
- Fix converted buildings visibility (they were no longer visible through fog !)
- Fix civilian count bug (resource 37) that affects "civilian high" score value.
- Fix military units count bug (resource 40) that affects "largest army" score value.
- Fix invisible tree (393): automatically assigns correct tree graphics.
- Fix discoveries count bug (AI players get a wrong discovery findings count and may win the game too early).
- Fix artefacts capture bug: human player no longer capture artefacts faster than AI (occurs the first time an artefact is seen).
- Fix gaia player (0) alert sound.
- Fix many bugs with units behaviour, like AI units attacking an empty location on the map for no reason.
- Fix kill* game crash when used with an invalid player
- Fix random game crash when using civilizations without a tech tree.

Chat commands:
- Type "t" or "time" in chat box to display system time (as a chat message)
- Type "seed" to get map seed (as a chat message)
- Type "ai" or "AI" in chat box to fix AI control flags after using a "gaia*" cheat code.
- Type "allai" or "allAI" in chat box enable computer-control for ALL players
- Type "hi" to get Hello world, "about" for information about customROR.
- Type "timer stats" to get info related to game timer bug.

Other commands (in customROR options menu)
- Type "addXXX" to add any unit to any explored location (like in editor). For example, add83=villager, add276=wonder. Right-click to get back to normal game mode.
- Type "set seed=XXX" to modify game map seed (RM and DM). This will only have an impact if you restart the game.
- Type "set maptype=XXX" to set map type. This will only have an impact if you restart the game. Ex: set maptype=0 for small islands (refer to AGE3).
- Type "set control=X" to set the player you want to play. It will automatically enable/disable AI control for your old/new player.
- Type "set maxpop=XXX" to set a new maximum population value (0-255). Impact both current and new games. Not persistent if you exit Age of Empires.
- Debug: Type "strat" or "stratX" where X is a player id to view a player's strategy.

Troubleshooting:
- You may need to run your editor as admin to edit XML files according to the location you installed the game.
- Game may crash if customROR.xml content is invalid. Try to restart using provided customROR.xml example.
- CustomROR produces a log file named "CustomROR.log" in Age of Empires installation directory.
- If the game process does not have necessary rights, the log file might be located in C:\Users\<username>\AppData\Local\VirtualStore\Program Files (x86)\Jeux\Age of Empires
- Make sure the "ROR_API.conf" file contains the lines 'wndmode.dll' (if you use windowed mode) and 'customROR.dll' (without the quotes and without extra characters on the same line)
- Never declare more civilizations in customROR_civs.xml than there really exist in empires.dat. ROR_API will force civilization to Egyptian if you select a civilization that is not found in empires.dat.
- If you install CustomROR on a UPatch HD1.1 installation, you may need to empty the file gamex/up.ini to have windowed mode work correctly. Default file values from UPatch installation seems to be incorrect.
- If you install CustomROR on a directory game that already had CustomROR, config files will not be overwritten.
	To get latest features, you should really get CustomROR.xml content from the provided example (in ZIP archive file). Same for CustomROR_civs.xml.
