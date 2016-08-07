
#pragma once

#include <math.h>
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
#include <AOE_struct_score.h>
#include <AOE_struct_unit_actions.h>
#include <AOE_struct_command_line_info.h>
#include <AOE_struct_sound.h>
#include <AOE_struct_mp_communication.h>
#include <AOE_struct_game_map_info.h>
#include <AOE_struct_unit_activity.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
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

	// Pre-declarations
	class STRUCT_HUMAN_TRAIN_QUEUE_INFO;
	class STRUCT_SPOTTED_RESOURCE_INFO;
	class STRUCT_UNKNOWN_GAME_TIME;
	class STRUCT_SCENARIO_INFO;
	class STRUCT_GAME_COMMANDS_INFO;
	class STRUCT_GAME_GLOBAL;
	class STRUCT_GAME_SETTINGS;
	class STRUCT_INF_AI_UNIT_LIST_ELEM;
	class STRUCT_TAC_AI_BUILD_TO_TRIGGER;
	class STRUCT_VILLAGER_TASKS_ELEM;
	class STRUCT_UNIT_GROUP_ELEM;

	// AI
	class STRUCT_AI;
	class STRUCT_COMMON_AI_OBJECT; // The common object included in Main decision AI, build AI, etc
	class STRUCT_BUILD_AI;
	class STRUCT_CON_AI;
	class STRUCT_DIPLOMACY_AI;
	class STRUCT_EMOTIONAL_AI;
	class STRUCT_INF_AI;
	class STRUCT_RESOURCE_AI;
	class STRUCT_STRATEGY_AI;
	class STRUCT_TAC_AI;
	class STRUCT_TRADE_AI;
	class STRUCT_STRATEGY_ELEMENT;
	class STRUCT_PLANNED_RESOURCE_NEEDS;
	class STRUCT_CON_AI_CONSTRUCTION;
	class STRUCT_INF_AI_BUILDS_HISTORY;
	class STRUCT_TAC_AI_TARGET_INFO;

	// Activity
	class STRUCT_UNIT_ACTIVITY_TARGET_ELEM;
	class STRUCT_UNIT_ACTIVITY_QUEUE;
	class STRUCT_UNIT_ACTIVITY;

	// MAP
	class STRUCT_TEMP_MAP_BUILD_LIKE_INFOS;

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


	// Size=0x14. Used in InfAI structure.
	class STRUCT_SPOTTED_RESOURCE_INFO {
	public:
		long int gatherableUnitId;
		char posY;
		char posX;
		char unknown_06;
		char unknown_07;
		long int estimatedTotalTripLength; // +8. round trip estimated distance, including an arbitrary +10 value for detour (formula = distance*2 +10)
		char unknown_0C;
		char unknown_0D;
		char distanceToStorageBuilding; // +E. Actual direct distance between resource unit and its closest storage building.
		char resourceType; // +F. Always 0-3 (food wood stone gold).
		long int storageBuildingUnitID; // +10. UnitID of the closest storage building.
	};
	static_assert(sizeof(STRUCT_SPOTTED_RESOURCE_INFO) == 0x14, "STRUCT_SPOTTED_RESOURCE_INFO size");


	static const unsigned char CST_MAP_BUILD_LIKE_DISABLED = (unsigned char)0xFF;
	static const unsigned char CST_MAP_BUILD_LIKE_MAX_VALUE = (unsigned char)CST_MAP_BUILD_LIKE_DISABLED - 1;
	// Included in 0x7C04A0 address.
	// Size= 0x28 ?
	// Describes a temp structure that stores information for currently-AI-managed player's build position priorities (for a given building to be constructed).
	// Note about coordinates: (posX,posY)=(i,j) is the tile where i<x<i+1 and i<y<i+1
	// So coordinates go from 0 to ("size"-1). ("grid" goes from 0 to "size").
	class STRUCT_TEMP_MAP_BUILD_LIKE_INFOS {
	public:
		unsigned long int checksum; // C8 43 54 00
		long int mapArraySizeY;
		long int mapArraySizeX;
		long int unknown_0C_toDecreaseToPosY;
		// 0x10
		long int unknown_10_toDecreaseToPosX;
		// mapLikeValuesMemoryZone[mapArraySizeX * (x) + (y)] should be the same as ptrRowsPtr[X][Y]. Strange conception.
		// Please use supplied methods if possible.
		unsigned char *mapLikeValuesMemoryZone; // Pointer to the beginning of map values memory zone. But the game accesses it via ptrRowsPtr (that gives pointers to each row (X))
		unsigned char **ptrRowsPtr; // The list of pointers to each row (X dimension) in mapLikeValues. ptrRowsPtr[X] is a pointer to the row X.
		long int unknown_1C;
		// 0x20
		long int unknown_matchCount;
		unsigned char maxLikeValue; // 0xFE generally (always ?). FF is a NO, 0->0xFE are "like values"
		char unknown_25[3];
		// to be continued... unknowns. Size ?

		bool IsCheckSumValid() { return this->checksum == 0x005443C8; }

		// Easy-to-use methods
		bool IsPositionValid(long int posX, long int posY) {
			return ((posX >= 0) && (posX < this->mapArraySizeX) &&
				(posY >= 0) && (posY < this->mapArraySizeY)
				);
		};
		bool FixPositionToGetInMapBounds(long int *posX, long int *posY) {
			if (*posX < 0) { *posX = 0; }
			if (*posY < 0) { *posY = 0; }
			if (*posX >= this->mapArraySizeX) { *posX = this->mapArraySizeX - 1; }
			if (*posY >= this->mapArraySizeX) { *posY = this->mapArraySizeX - 1; }
		}
		// Will not fail/crash if position is out of bounds
		unsigned char GetMapLikeValue(long int posX, long int posY) {
			return this->IsPositionValid(posX, posY) ? ptrRowsPtr[posX][posY] : CST_MAP_BUILD_LIKE_DISABLED;
		}
		// Set a tile like value
		// Will not fail/crash if position is out of bounds
		void SetMapLikeValue(long int posX, long int posY, long int value) {
			if (this->IsPositionValid(posX, posY)) { ptrRowsPtr[posX][posY] = (unsigned char)value; }
		}
		// Increments a tile like value with provided value (can be <0). Preserves disabled ones (0xFF)
		// Will not fail/crash if position is out of bounds
		void AddToMapLikeValue(long int posX, long int posY, long int relativeValue) {
			if (!this->IsPositionValid(posX, posY) || (ptrRowsPtr[posX][posY] == CST_MAP_BUILD_LIKE_DISABLED)) {
				return;
			}
			long int v = ptrRowsPtr[posX][posY]; // long int to avoid overflows
			v += relativeValue;
			if (v > CST_MAP_BUILD_LIKE_MAX_VALUE) { v = CST_MAP_BUILD_LIKE_MAX_VALUE; }
			if (v < 0) { v = 0; }
			ptrRowsPtr[posX][posY] = (unsigned char)v;
			
		}
		// Set a tile like value but preserves disabled ones (0xFF)
		// Will not fail/crash if position is out of bounds.
		void SetMapLikeValueIfNotFF(long int posX, long int posY, long int value) {
			if (this->IsPositionValid(posX, posY) && (ptrRowsPtr[posX][posY] != CST_MAP_BUILD_LIKE_DISABLED)) {
				ptrRowsPtr[posX][posY] = (unsigned char)value;
			}
		}

		// Returns true if 1 of the line-end is disabled.
		// On each side, if one of the last 2 tiles is disabled, then the result is true
		bool IsAnExtremityBlocked_yLine(long int posX, long int posY_low, long int posY_high) {
			// Checks on values
			if (posY_low > posY_high) { return true; }
			if (posX < 0) { posX = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX >= this->mapArraySizeX) { posX = mapArraySizeX - 1; }
			if (posY_low >= this->mapArraySizeY) { posY_low = mapArraySizeY - 1; }
			if (posY_high >= this->mapArraySizeY) { posY_high = mapArraySizeY - 1; }
			unsigned char *currentRow = this->ptrRowsPtr[posX];
			if (posY_low >= mapArraySizeY - 1) { return true; }
			if ((currentRow[posY_low] == CST_MAP_BUILD_LIKE_DISABLED) || (currentRow[posY_low + 1] == CST_MAP_BUILD_LIKE_DISABLED)) {
				return true;
			}
			if (posY_high <= 0) { return true; }
			if ((currentRow[posY_high] == CST_MAP_BUILD_LIKE_DISABLED) || (currentRow[posY_high - 1] == CST_MAP_BUILD_LIKE_DISABLED)) {
				return true;
			}
			return false;
		}

		// Returns true if 1 of the line-end is disabled.
		// On each side, if one of the last 2 tiles is disabled, then the result is true
		bool IsAnExtremityBlocked_xLine(long int posX_low, long int posX_high, long int posY) {
			// Checks on values
			if (posX_low > posX_high) { return true; }
			if (posY < 0) { posY = 0; }
			if (posX_low < 0) { posX_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY >= this->mapArraySizeY) { posY = mapArraySizeY - 1; }
			if (posX_low >= this->mapArraySizeX) { posX_low = mapArraySizeX - 1; }
			if (posX_high >= this->mapArraySizeX) { posX_high = mapArraySizeX - 1; }
			unsigned char *currentRow = this->ptrRowsPtr[posX_low];
			if (this->ptrRowsPtr[posX_low][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			if (posX_low >= mapArraySizeX - 1) { return true; }
			if (this->ptrRowsPtr[posX_low + 1][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			if (this->ptrRowsPtr[posX_high][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			if (posX_high <= 0) { return true; }
			if (this->ptrRowsPtr[posX_high - 1][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			return false;
		}

		bool IsRegionAllDisabled(long int posX_low, long int posY_low, long int posX_high, long int posY_high) {
			// Checks on values
			if ((posX_low > posX_high) || (posY_low > posY_high)) { return true; }
			if (posX_low < 0) { posX_low = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX_low >= this->mapArraySizeX) { posX_low = mapArraySizeX - 1; }
			if (posY_low >= this->mapArraySizeY) { posY_low = mapArraySizeY - 1; }
			if (posX_high >= this->mapArraySizeX) { posX_high = mapArraySizeX - 1; }
			if (posY_high >= this->mapArraySizeY) { posY_high = mapArraySizeY - 1; }
			for (long int i = posX_low; i <= posX_high; i++) {
				unsigned char *currentRow = this->ptrRowsPtr[i];
				for (long int j = posY_low; j <= posY_high; j++) {
					if (currentRow[j] != CST_MAP_BUILD_LIKE_DISABLED) {
						return false;
					}
				}
			}
			return true;
		}

		// Set tile like value for all tiles in a rectangle (borders included)
		void SetMapLikeValueRectangle(long int posX_low, long int posY_low, long int posX_high, long int posY_high, long int value, bool preserveDisabled = false) {
			// Checks on values
			if ((posX_low > posX_high) || (posY_low > posY_high)) { return; }
			if (posX_low < 0) { posX_low = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX_low >= this->mapArraySizeX) { posX_low = mapArraySizeX - 1; }
			if (posY_low >= this->mapArraySizeY) { posY_low = mapArraySizeY - 1; }
			if (posX_high >= this->mapArraySizeX) { posX_high = mapArraySizeX - 1; }
			if (posY_high >= this->mapArraySizeY) { posY_high = mapArraySizeY - 1; }
			// Treatments
			for (long int i = posX_low; i <= posX_high; i++) {
				unsigned char *currentRow = this->ptrRowsPtr[i];
				for (long int j = posY_low; j <= posY_high; j++) {
					if (!preserveDisabled || (currentRow[j] != CST_MAP_BUILD_LIKE_DISABLED)) {
						currentRow[j] = (unsigned char)value;
					}
				}
			}
		}

		// Add like value for all tiles in a rectangle (borders included). Value can be negative.
		// This does NOT modify disabled tiles (0xFF)
		void AddMapLikeValueRectangle(long int posX_low, long int posY_low, long int posX_high, long int posY_high, long int value) {
			// Checks on values
			if ((posX_low > posX_high) || (posY_low > posY_high)) { return; }
			if (posX_low < 0) { posX_low = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX_low >= this->mapArraySizeX) { posX_low = mapArraySizeX - 1; }
			if (posY_low >= this->mapArraySizeY) { posY_low = mapArraySizeY - 1; }
			if (posX_high >= this->mapArraySizeX) { posX_high = mapArraySizeX - 1; }
			if (posY_high >= this->mapArraySizeY) { posY_high = mapArraySizeY - 1; }
			// Treatments
			for (long int i = posX_low; i <= posX_high; i++) {
				unsigned char *currentRow = this->ptrRowsPtr[i];
				for (long int j = posY_low; j <= posY_high; j++) {
					long int tmpvalue = currentRow[j];
					if (tmpvalue != CST_MAP_BUILD_LIKE_DISABLED) {
						tmpvalue += value; // Note: value can be <0
						if (tmpvalue > CST_MAP_BUILD_LIKE_MAX_VALUE) { tmpvalue = CST_MAP_BUILD_LIKE_MAX_VALUE; }
						if (tmpvalue < 0) { tmpvalue = 0; }
						currentRow[j] = (unsigned char)tmpvalue;
					}
				}
			}
		}

		// For each of the 4 sides of a building:
		// If the side is blocked (all neighbour tiles are disabled, side excluding corners),
		// then disable opposite side neighbour tiles (including corners)
		void ProtectBuildingOppositeSides(int unitPosX, int unitPosY, float unitRadius_f, char valueToUse = AOE_STRUCTURES::CST_MAP_BUILD_LIKE_DISABLED) {
			int unitRadius = (int)unitRadius_f;
			int unitRadiusDecreaseForHighPositions = 1 -((int)(unitRadius * 2) % 2); // 0 for values like 1.5 or 2.5, 1 for round values
			// Get borders of the building (last tile part of the building) for simpler calculations
			int xLeftSide = unitPosX - unitRadius;
			int xRightSide = unitPosX + unitRadius - unitRadiusDecreaseForHighPositions;
			int yBottomSide = unitPosY - unitRadius;
			int yTopSide = unitPosY + unitRadius - unitRadiusDecreaseForHighPositions;
			int includeCornersInDetection = 1; // Set to 0 to ignore diagonal-corner tile, 1 to take it into account.
			//assert((includeCornersInDetection == 0) || (includeCornersInDetection == 1));

			// Is left blocked ?
			//bool isDisabled = this->IsRegionAllDisabled(xLeftSide - 1, yBottomSide, xLeftSide - 1, yTopSide);
			bool isDisabled = this->IsAnExtremityBlocked_yLine(xLeftSide - 1, yBottomSide - includeCornersInDetection, yTopSide + includeCornersInDetection);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xRightSide + 1, yBottomSide - 1, xRightSide + 1, yTopSide + 1, valueToUse, true);
			}
			// Is Right blocked ?
			isDisabled = this->IsAnExtremityBlocked_yLine(xRightSide + 1, yBottomSide - includeCornersInDetection, yTopSide + includeCornersInDetection);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xLeftSide - 1, yBottomSide - 1, xLeftSide - 1, yTopSide + 1, valueToUse, true);
			}
			// Is bottom blocked ?
			isDisabled = this->IsAnExtremityBlocked_xLine(xLeftSide - includeCornersInDetection, xRightSide + includeCornersInDetection, yBottomSide - 1);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xLeftSide - 1, yTopSide + 1, xRightSide + 1, yTopSide + 1, valueToUse, true);
			}
			// Is top blocked ?
			isDisabled = this->IsAnExtremityBlocked_xLine(xLeftSide - includeCornersInDetection, xRightSide + includeCornersInDetection, yTopSide + 1);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xLeftSide - 1, yBottomSide - 1, xRightSide + 1, yBottomSide - 1, valueToUse, true);
			}

			// If a corner tile if disabled, do we disable diagonal-opposed corner tile ?
		}
	};



	// Size = 0x514C. Constructor=0x0506900
	// Warning: all arrays[0x10] are indexed by playerId-1, which means tribeNames[0] is player1's name.
	class STRUCT_SCENARIO_INFO {
	public:
		unsigned long int checksum; // C8 A3 54 00 ; parent struct = 3C 5E 54 00
		STRUCT_UNKNOWN_GAME_TIME *timeInfo; // ptr to 2C 99 54 00
		STRUCT_GAME_GLOBAL *globalStruct;
		char unknown_000C_generalVictory_conquest; // Init = 1 (default)
		char tribeNames[0x10][0x100]; // +D. Index=(playerId-1)
		char unknown_100E; // unused?
		char unknown_100F; // unused?
		// 0x10
		char *scenarioFileName; // +1010
		char *scenarioInstructions; // +1014
		char *scenarioTips;
		char *scenarioVictoryText;
		// 0x1020
		char *scenarioDefeatedText;
		char *scenarioHistoryText;
		//char unknown_1028[0x12C8 - 0x1028]; // 475C55
		char fullPath_tmpBuildList[0xE0];
		char unknown_1108[0x12C8 - 0x1108];
		char *strategyFileNames[0x10]; // +12C8. Index=(playerId-1). Size = 0xE0.
		char *cityPlanFileNames[0x10]; // +1308. Index=(playerId-1). Size = 0xE0.
		char *personalityFileNames[0x10]; // +1348. Index=(playerId-1). Size = 0xE0.
		char *strategyFileContent[0x10]; // +1388. pointer to AI file content. See in 408948 (read from a temp file, not from THIS). Is size limited to ~32ko ? Not sure
		char *cityPlanFileContent[0x10]; // +13C8. Pointer to an array of bytes.
		char *personalityFileContent[0x10]; // +1408. Pointer to an array of bytes. Note: The "END" keyword actually stops reading the file.
		long int strategyFileSize[0x10]; // +1448.
		long int cityPlanFileSize[0x10]; // +1488.
		long int personalityFileSize[0x10]; // +14C8.
		char scenarioBeginningAnimationFileName[0xE0]; // +1508
		char scenarioVictoryAnimationFileName[0xE0]; // +15E8
		char scenarioFailureAnimationFileName[0xE0]; // +16C8
		char scenarioBitmapFileName[0xE0]; // +17A8
		float scenarioVersion; // +1888. Corresponds to .scn/.scx version, like 1.02, 1.03, 1.04, 1.05, 1.13, 1.15(=ROR 1.0a)
		long int player_unknown_188C[0x10]; // +188C. Often referred to as +18CC-40. Default = 4 ?
		long int playerIsHumanPlayable[0x10]; // +188C. 0 if computer player only (cannot be played as human)
		long int playerCivilizationId[0x10]; // +18CC. Often referred to as +18CC+40.
		long int playerIsActive[0x10]; // +194C. 1 if player is active. Often referred to as +18CC+80.
		long int *unknown_198C; // +198C. ptr to struct size=0x20, constructor 45AF60. TO DO.
		long int startingResources[0x10][4]; // +1990. WARNING: resource order = gold/wood/food/stone here !!!
		// 0x1A90 - probably a sub-structure of size=0x3160 (last 4 bytes are unused) for victory conditions settings.
		// Those generalVictory_* are only relevant if generalVictory_mainCondition==4 (custom)
		long int generalVictory_conquest; // 1 = Standard, Conquest, Score, Time Limit, Custom if Conquest is enabled. ; 0=others
		long int generalVictory_ruinsCount; // Number of ruins to collect to win the game (for all players)
		long int generalVictory_relicsCount; // Number of relics to collect to win the game (for all players)
		long int generalVictory_discoveriesCount; // Number of discoveries to find to win the game (for all players)
		// 0x1AA0
		long int generalVictory_explorationAmount; // % of exploration to reach to win the game (for all players)
		long int generalVictory_goldAmount; // Amount of gold to collect to win the game (for all players). Hidden in scenario editor (value is never modified by editor).
		char unknown_1AA8_individual_victory_conditions[0x10][0x2D0]; // Internal structure to define
		long int diplomacySettings[0x10][0x10]; // +47A8. Refers to PLAYER_DIPLOMACY_STANCES but as DWORDs. 0=allied,1=neutral,3=enemy.
		long int hasAlliedVictory[0x10]; // +4BA8. 1=option is checked for that player
		long int artefactsRequireAll; // +4BE8. 0="at least one", 1="all". Applies to relics, ruins, discoveries ?
		unsigned long int unknown_4BEC; // Unused, it seems.
		// 0x4BF0
		unsigned long int unknown_4BF0;
		long int disableTechnologyFlags[0x10][AOE_CONST_INTERNAL::SC_ED_DISABLE_TECHNOLOGY_INDICES::CST_DTI_COUNT]; // +4BF4. Total Size =0x500 bytes. Used in 0x507E50=scenarioInfo.applyDisabledResearches(player)
		long int enableLengthenCombatMode; // +50F4. A flag 0/1. If 1, lengthen combat mode is ON (most units get *3 HP, it is technology 0x64)
		long int unknown_50F8; // +50F8 ? Unused ?
		long int fullTechTree; // +50FC
		// 0x5100
		long int playersStartingAge[0x10]; // 1=Tool, 4=post iron. Warning, this does not correspond to CST_AGE_TOOL, etc. Index is playerId-1.
		// 0x5140
		AOE_CONST_INTERNAL::GENERAL_VICTORY_CONDITION generalVictory_mainCondition; // 0=Standard, 1=Conquest, 2=Score, 3=Time Limit, 4=Custom
		long int generalVictory_scoreAmount; // Default 900
		long int generalVictory_timeAmount; // Default 9000 "years"

		bool IsCheckSumValid() { return this->checksum == 0x0054A3C8; }
	};


	// Size = ? - 04 99 54 00
	class STRUCT_GAME_COMMANDS_INFO {
	public:
		unsigned long int checksum;
		STRUCT_GAME_GLOBAL *global;
		unsigned long int currentCommand; // unsure
		long int currentCommandSize; // unsure
		// 0x10
		long int timestamp; // Value from timeGetTime
		unsigned long int *unknown_14; // Ptr to struct similar to [580DA8]

		bool IsCheckSumValid() { return this->checksum == 0x00549904; }
	};


#ifdef GAMEVERSION_AOE10b
#define CHECKSUM_GAME_GLOBAL1 0x00554A58
#define CHECKSUM_GAME_GLOBAL2 0x00553230
#endif
#ifdef GAMEVERSION_AOE10c
#define CHECKSUM_GAME_GLOBAL1 0x005432AC
#define CHECKSUM_GAME_GLOBAL2 0x005436FC
#endif
#ifdef GAMEVERSION_ROR10b // cf 0x50C250
#define CHECKSUM_GAME_GLOBAL1 0x0054F990
#define CHECKSUM_GAME_GLOBAL2 0x0054FE58
#endif
#ifdef GAMEVERSION_ROR10c
#define CHECKSUM_GAME_GLOBAL1 0x0054A4F8
#define CHECKSUM_GAME_GLOBAL2 0x0054A954
#endif
	// Size = 0x124 - F8 A4 54 00. Parent=54 A9 54 00
	// Constructor 0x509AE0 (parent 51B140)
	class STRUCT_GAME_GLOBAL {
	public:
		unsigned long int checksum;
		unsigned long int currentGameTime; // in milliseconds
		unsigned long int unknown_008;
		unsigned long int unknown_00C;
		unsigned long int unknown_010;
		unsigned long int timeGetTimeValue;
		float gameSpeed;
		char gamePaused; // confirm exact role, do not use it directly.
		char gameRunStatus; // 0 = Game in progress. 1 = Game Over. 2 = Not ingame. (Default). 3 = Generate Map (when starting game).
		short int unknown_01E;
		// +0x20
		unsigned long int unknown_020;
		unsigned long int unknown_024;
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +28. Map related data. (F4 99 54 00)
		short int soundsArrayElemCount;
		short int unknown_02E; // maybe 2 bytes, not 1 word
		// +0x30
		unsigned long int ptrSoundsArray;
		unsigned long int unknown_034;
		unsigned long int ptrGraphicsArray; // ptr to list of elements (size=0x48) starting by a unit name?
		short int playerTotalCount; // Player count including gaia = real player count + 1
		short int unknown_03E;
		// +0x40
		STRUCT_PLAYER **ptrPlayerStructPtrTable; // points to table of player struct addresses (pointers)
		short int civCount; // +44. Number of civilizations.
		short int unknown_046;
		STRUCT_CIVILIZATION_DEF **civilizationDefinitions; // +48. A pointer to the array of pointers to civ definitions. Array size is civCount
		STRUCT_TECH_DEF_INFO *technologiesInfo; // +4C. 20 99 54 00
		// +0x50
		unsigned short int terrainRestrictionCount; // Size for ptrTerrainRestrictionArray array
		unsigned short int nbOfTerrainPerTerrainRestriction; // +52. How many terrain for each terrain restriction (common for all)
		float **ptrTerrainRestrictionArray; // +54. Array of pointers to terrain restriction info (if an array of float=0/1). Example 426951. ptrTerrainRestrictionArray[terrainRestriction][terrainId]=float : 0=inaccessible, othervalue=accessible=damagefactor
		STRUCT_GAME_COMMANDS_INFO *commandsInfo; // +58. 04 99 54 00.
		STRUCT_SCENARIO_INFO *scenarioInformation; // +5C.
		// +0x60
		unsigned short int unknown_060; // size for +64 ?
		unsigned short int unknown_062;
		unsigned long int unknown_064; // .col list ?
		long int seqUnitId; // Sequence for unit instance IDs (for creatable only ?)
		unsigned long int unknown_06C_unitIdSeqForTempUnits; // Unsure
		// +0x70
		long int forcedNextUnitId; // Unsure. Forced when loading a scenario.
		long int forceUseSpecifiedUnitId; // +74. If set (temporarily), use "+70" as next unitInstanceId instead of reading sequence.
		unsigned long int unknown_078; // random value for seed ?
		short int humanPlayerId; // +7C. PlayerId that is human controlled (controlledPlayerId)
		short int unknown_07E;
		// +0x80
		unsigned long int unknown_080; // list of unit data??
		float timerInterval;
		STRUCT_UNIT **ptrUnitPointersList; // +88. pointer to Array of unit struct pointers
		unsigned long int unknown_08C;
		// +0x90
		long int unitPointersListSize; // Size of the ptrUnitPointersList Array
		unsigned long int unknown_094;
		unsigned long int unknown_098;
		long int unknown_094_ListSize; // Size of x+0x94 array
		// +0xA0
		char unknown_0A0[0x10]; // A0: related to aoe.ply ?
		// +0xB0
		unsigned long int unknown_0B0_playerId; // currently managed playerid?
		unsigned long int unknownPlayerVar[0x09]; // 9 variables for each player, including gaia
		// +0xD8
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type10; // +D8. For unit type = eye candy
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type20; // +DC. For unit type = flag
		// +0xE0
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type30; // +E0. For unit type = killable ?
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type40; // +E4. For unit type = birds
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type50; // +E8. For unit type = type50
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_type60; // +EC. For unit type = projectiles. 0x520BD0
		// +0xF0
		STRUCT_PER_TYPE_UNIT_LIST_LINK *unitList_doppleganger; // list of all dopplegangers ?
		unsigned long int unknown_0F4;
		long int tmp_allowedTimeForAITreatment; // +F8. For internal treatments (temporary, reset to 0 when finished). This is a time value allowed for AI treatments
		long int currentlyManagedAIPlayer; // +FC. A player id. "currentUpdateComputerPlayer"
		// +0x100
		unsigned long int unknown_100_difficultyLevel; // NOT always diff. level. See in game settings struct.
		STRUCT_RESEARCH_DEF_INFO *researchDefInfo; // +104.
		unsigned long int unknown_108;
		unsigned long int relicsCount;
		// +0x110
		unsigned long int ruinsCount;
		char unknown_114;
		char unknown_115[3]; // unused ?
		unsigned long int unknown_118;
		char WRONG_showScores; // 0x11C. WRONG ?
		char temporaryTargetPlayerId; // 11D. Used for AI players. "ConCP", CP=computerPlayer? Con=?concerned?
		short int unknown_11E;
		// 0x120
		long int unknown_120;		

		AOE_STRUCTURES::STRUCT_PLAYER ** GetPlayerStructPtrTable() const{
			return (AOE_STRUCTURES::STRUCT_PLAYER **) this->ptrPlayerStructPtrTable;
		}
		// Returns player structure corresponding to supplied ID. Returns NULL if not found
		AOE_STRUCTURES::STRUCT_PLAYER *GetPlayerStruct(int playerId) const {
			if (!this->IsCheckSumValid()) { return NULL; }
			if ((playerId < 0) || (playerId >= this->playerTotalCount) || (playerId >= 9)) { return NULL; }
			return this->ptrPlayerStructPtrTable[playerId];
		}
		// Get a technology definition, or NULL if not found
		AOE_STRUCTURES::STRUCT_TECH_DEF *GetTechDef(short int techDefId) const {
			if (!this->technologiesInfo || !this->technologiesInfo->IsCheckSumValid()) { return NULL; }
			return this->technologiesInfo->GetTechDef(techDefId);
		}
		// Returns the array of (float) values for demanded terrain restriction.
		// 0 = not accessible, >0 means accessible and is the value for armor damage multiplier
		// Returns NULL if restriction is invalid
		float *GetTerrainRestrictionValues(TERRAIN_RESTRICTION restriction) const {
			if ((restriction >= this->terrainRestrictionCount) || (restriction < 0)) { return NULL; }
			return this->ptrTerrainRestrictionArray[restriction];
		}
		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_GAME_GLOBAL1) || (this->checksum == CHECKSUM_GAME_GLOBAL2); }
	};


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


	// Size = 0x14
	// Constructor : 4EBBF0 (readFromFile)
	class STRUCT_UNKNOWN_GAME_TIME {
	public:
		unsigned long int checksum; // 2C 99 54 00
		STRUCT_GAME_GLOBAL *global;
		unsigned long int unknown_08;
		short int unknown_0C; // Count of... ? (+08 ptr ?)
		short int unknown_0E;
		// 0x10
		float gameTime;

		bool IsCheckSumValid() { return (this->checksum == 0x0054992C); }
	};


#ifdef GAMEVERSION_AOE10b
#define CHECKSUM_GAME_SETTINGS1 0x005509D8
#define CHECKSUM_GAME_SETTINGS2 0x00553B78 // parent class
#endif
#ifdef GAMEVERSION_AOE10c
#define CHECKSUM_GAME_SETTINGS1 0x0054301C
#define CHECKSUM_GAME_SETTINGS2 0x0053C0A4 // parent class
#endif
#ifdef GAMEVERSION_ROR10b
#define CHECKSUM_GAME_SETTINGS1 0x0054F6E0
#define CHECKSUM_GAME_SETTINGS2 0x00547DE8 // parent class
#endif
#ifdef GAMEVERSION_ROR10c
#define CHECKSUM_GAME_SETTINGS1 0x0054A264
#define CHECKSUM_GAME_SETTINGS2 0x005430B4 // parent class
#endif
	// Size = 0x11A0 (AOE1.0b&c). Constructor 0x4F91C0 (empires.exe)
	// Size = 0x1254. Constructor 0x5004C0(1.0b), ?(1.0c)
	class STRUCT_GAME_SETTINGS {
	public:
		unsigned long int checksum;
		unsigned long int unknown_004_game_nfo_data;
		unsigned long int unknown_008_ptr_scenario_data;
		STRUCT_COMMAND_LINE_INFO *commandLineInfo;
		// 0x10
		char unknown_010[0x1C];
		unsigned long int scenarioMapSeed; // +2C. When supplied manually through scenario editor. -1 for RM / DM
		unsigned long int unknown_030;
		unsigned long int actualMapSeed; //+34.  THE actual map seed
		unsigned long int unknown_038;
		unsigned long int unknown_03C;
		// 0x40
		char unknown_040[0x1C];
		STRUCT_SLP_INFO **ptrInfosSLP; // +5C. Pointer to array slpInfo, size=3 ? index 0=shortcut numbers
		// 0x60
		STRUCT_MAIN_SOUND *pSoundStruct; // Size 69C, see 41894B. Generally =NULL, only set while being used.
		STRUCT_MAIN_MUSIC *pMusicStruct; // Size 3F8, see 418B14. Generally =NULL, only set while being used.
		long int commonSoundDataArrayElemCount; // +68. Default 0x11 (see 4FEF2D).
		unsigned long int ptrCommonSoundDataArray; // +6C.
		// 0x70
		char musicFormat; // 0=AudioCD, 1=MIDI
		char unknown_071;
		char unknown_072;
		char unknown_073;
		long int minMusicTrack;
		long int maxMusicTrack;
		long int currentMusicTrack;
		// 0x80
		char musicFilename[0x184 - 0x80]; // Chack this...
		long int musicIsLooping; // +184
		long int musicSeekPos; // Resume from minimized window
		STRUCT_MP_COMMUNICATION *MPCommunicationStruct; // +18C
		// 0x190
		char unknown_190[0x1AC - 0x190];
		unsigned long int unknown_1AC; // related to registry ?
		// 0x1B0
		AOE_CONST_INTERNAL::GAME_SETTINGS_UI_STATUS currentUIStatus; // 0=loading,2=inMenu,4=playing,7=editor
		AOE_CONST_INTERNAL::MOUSE_ACTION_TYPES mouseActionType; // +1B4. Set this during game, you'll be able to edit the map !! userSelected* fields
		long int unknown_1B8; // Values 1,4,6,... ? SubMouseActionType ? Used when mouseActionType=6 (villager build menu) or in editor. Seen 1=editor_moveUnit?, 3, 4(editor_putunit?). Unsure !
		long int unknown_1BC;
		// 0x1C0
		char unknown_1C0[0x10];
		// 0x1D0
		unsigned long int unknown_1D0;
		long int hCursor; // Cursor handle, default=10003
		unsigned long int unknown_1D8;
		unsigned long int unknown_1DC;
		// 0x1E0
		long int unknown_1E0;
		long int unknown_ptrFonts; // Ptr to array, elemSize=0x0C, elemIndex = fontId
		char gameDirFullPath[0x105]; // To confirm
		char languageDLLRelativePath[0x105]; // 2ED. "language.dll"
		char unknown_3F2; // unused ?
		char unknown_3F3; // unused ?
		STRUCT_GAME_GLOBAL *ptrGlobalStruct; // +3F4
		unsigned long int unknown_3F8;
		short int editorUserSelectedUnitDefId; // +3FC: the unitDefID that user selected in "units" tab.
		short int editorUserSelectedTerrainId; // Can be changed in editor/terrain tab
		// 0x400
		short int editorUserSelectedAltitude; // in editor/terrain tab
		short int editorTerrainPencilSize; // in editor/terrain tab
		char debugString_404[0x508 - 0x404]; // "Avg view time"...
		long int unknown_508;
		long int unknown_50C; // a counter
		char unknown_510[0x528 - 0x510];
		char unknown_528[0x8E0 - 0x528]; // An included array, elemSize=0x20; getter=417430
		// +4F0 : pointer ? +100=Affects DisableMapSizeSetting
		// 0x8E0
		long int screenSizeX; // To confirm + confirm type
		long int screenSizeY; // To confirm + confirm type
		unsigned long int unknown_8E8;
		unsigned long int unknown_8EC;
		unsigned long int unknown_8F0;
		unsigned long int unknown_8F4;
		float gameSpeed; // +8F8. TO confirm - a struct starts here ?
		char isScenario; // +8FC. 1 if the game is a scenario, 0 for random game or deathMatch. (offset 0x8FC)
		char scenarioFileName[3 + 0x7C]; // +8FD.
		// 0x900
		//char unknown_900[0x7C];
		char unknown_97C;
		char isSinglePlayer;
		char isMultiplayer; // 0x97E
		char mapSizeX;
		// 0x980
		char mapSizeY;
		char unknown_981; // Related to map size. Always 8 ? Max player # ?
		char enableCheatMode; // +982. Set in 41BAE0
		char unknown_983; // +983. path finding for MP ? 0-2 default/medium/high ?
		char unknown_984; // Something to do with screen width and height??
		char revealMap; // 0x985. Set in 41BB00
		char noFog; // 0x986. Set in 41BB10
		char unknown_987; // default 1 ? Set in 41BB20
		char playerCount; // +988.
		char unknown_989; // can set speed > 16 ?
		char playerGameVersion[0x08]; // 0-127 ; 0="", 1="1.0", 2="1.0a", etc (array offset in 0-7)
		char unknown_992;
		AOE_CONST_INTERNAL::GAME_DIFFICULTY_LEVEL difficultyLevel; // +993. 0=hardest, 1=hard, 2, 3, 4=easiest. Set in 41BBC0
		char teamNumbers[0x08]; // player 1-8 (array offset in 0-7). Values: 1=noTeam, >1=teamId.
		char unknown_99C;
		// 0x9A0
		long int isCampaign; // get in 41B720, set in 41BA50
		long int isSavedGame; // +9A4. set in 41BA60
		long int hasSteroidsCheatCode; // +9A8. set in 50CA2B, 4E90B6. Can be cancelled !!!
		long int has_CD; //see 4169D7
		// 0x9B0
		unsigned long int chosenPlayerIndex[9]; // index in 0-8 (playerId, including gaia)
		// to 9D4...
		unsigned long int unknown_9D4;
		unsigned long int unknown_9D8[9]; // ???
		// to 9FC...
		unsigned long int gameIsRunning; // You can simply set it to 0 to pause game.
		// 0xA00
		char unknown_A00[0x0C];
		float unknown_A0C;
		char difficultyLevelChoice; // +A10. Get in 41C190. Copied into +993
		char unknown_A11;
		char unknown_A12;
		char unknown_A13;
		char pathFindingChoice; // 0xA14 (path finding value = 0-2)
		char playerIsInactive[0x09]; // +A15. Index is a "playerIndex"(???Wrong?) (cf +9B0), not playerID. 1 for player that lost/resigned..., 0 if alive
		char unknown_A1E;
		char unknown_A1F;
		// 0xA20
		char unknown_A20[0xA34 - 0xA20];
		long int unknown_A34[9]; // Indexed by playerId. Value = an int ?
		char unknown_A58[0xA7C - 0xA58];
		unsigned long int mapSizeChoice; // +A7C
		// 0xA80
		AOE_CONST_FUNC::MAP_TYPE_INDEX mapTypeChoice; // 0=small islands, 1=large islands... See empires.dat
		unsigned long int unknownA84; // +A84. set in 505340. Default=1. Unused in SP RM ?
		unsigned long int unknownA88; // +A88. set in 505360. Default=1. Unused in SP RM ?
		unsigned long int victoryConditionChoice; // 0=standard, 1=conquest, 7=time limit, 8=score
		// 0xA90
		unsigned long int victoryConditionChoice_parameter; // or float ?
		unsigned long int unknownA94; // Bytes for chosen civs ?
		unsigned long int unknownA98;
		unsigned long int unknownA9C;
		// 0xAA0
		char unknown_AA0[0x20]; // dwords index=0-7. Limits player # ? 8 Players: [0,1,2,3,4,5,6,7] 4 Players: [0,1,2,3,0,1,2,3] 2 Players: [0,1,0,1,0,1,0,1]
		// 0XAC0
		unsigned long int unknownAC0;
		char chosenPlayerNum[8]; // Corresponds to the colored number in game settings screen
		char unknown_ACC;
		// 0xACD
		char playerNameCivDLLID_Offset[8]; // per player. 4400+x=language_dll_id of a civ (player) name.
		// 0xAD5
		char unknown_AD5;
		char unknown_AD6; // +AD6. Set in 505520.
		char unknown_AD7; // +AD7. Set in 505530.
		char fixedPositions; // +AD8. Set in 505540
		char allTechs; // +AD9. Set in 505550
		char unknown_ADA;
		char unknown_ADB;
		unsigned long int initialResourcesChoice; // +ADC. 0=default, 3=large. Set in 505560.
		// 0xAE0
		AOE_CONST_INTERNAL::GAME_INITIAL_AGES initialAgeChoice; // 0=default, 1=nomad, 2=stone, 3=tool 4=bronze, 5=iron, 6=post-iron. Set in 505570
		char unknown_AE4; // +AE4. Set in 505580
		char isDeathMatch; // +AE5. Set in 505590
		char maxPopulation; // +AE6. For MP games. Set in 5055A0, Get in 5054F0. [+A9E in aoe10b]
		char unknown_AE7[0x09];
		// 0xAF0
		char unknown_AF0[0xC20 - 0xAF0];
		// 0xC20
		unsigned long int unknown_C20;
		AOE_STRUCTURES::STRUCT_UI_IN_GAME_MAIN *ptrGameUIStruct; // +C24
		long int lastEventPositionsY[5]; // +C28. Used when pressing HOME key
		// 0xC3C
		long int lastEventPositionsX[5]; // +C3C. Used when pressing HOME key
		// 0xC50
		long int lastEventIndex; // +C50. -1=no "last event". Or a 0-4 value
		long int lastAccessedEventIndex; // +C54. -1=no "last event". Or a 0-4 value. To remember which index was used in last "home key" press.
		// 0xC58
		char unknown_C58[0x106C - 0xC58];
		char debugString[0x1190 - 0x106C]; // Unknown_ size
		// 0x1190
		char isInitMPScreen; // +1190. Set in 5055B0. ??? to confirm. is MP game ?
		char unknown_1191;
		char unknown_1192;
		char unknown_1193;
		char unknown_1194; // default -1 ?
		char unknown_1195;
		char unknown_1196;
		char unknown_1197;
		char civPlayerNameIsUsed[0x08*0x0A]; // +1198. 8=number of players, 0x0A=number of names (0=unused, 1-9=used)
		char unknown_11E8[0x1254 - 0x11E8];

		bool IsCheckSumValid() {
			return (this->checksum == CHECKSUM_GAME_SETTINGS1) || (this->checksum == CHECKSUM_GAME_SETTINGS2);
		}
	};
	//GAMEVERSION_AOE10c : size 0x11A0
#if defined(GAMEVERSION_ROR10b) || defined(GAMEVERSION_ROR10c)
	static_assert(sizeof(STRUCT_GAME_SETTINGS) == 0x1254, "STRUCT_GAME_SETTINGS size");
#endif

	// Size=0xB0 - Constructor = ?
	class STRUCT_STRATEGY_ELEMENT {
	public:
		unsigned long int checksum; // 3C 2A 54 00
		long int unitDAT_ID; // Identifies the unit definition or research. "type" term found in 408891
		long int unitInstanceId; // gID term found in 408891.
		long int elemId; // Just an internal ID to identify this strategy element in player's strategy list. "uID" term found in 408891.
		// 0x10
		char unitName[0x40]; // 0x3F + \0 maximum
		// 0x50
		float unknown_050; // posY_unused ?
		float unknown_054; // posX_unused ?
		float unknown_058;
		float sizeRadius1;
		// 0x60
		float sizeRadius2;
		unsigned long int unknown_064;
		unsigned long int unknown68_skipCount; // Unsure. "skip" term found in 408891
		STRUCT_STRATEGY_ELEMENT *next;
		// 0x70
		STRUCT_STRATEGY_ELEMENT *previous;
		AOE_CONST_FUNC::TAIUnitClass elementType; // 4-bytes. 0=building, 1=research,2=unit,4=critical_tech. "cat" (category) term found in 408891
		unsigned long int unknown_078;
		unsigned long int unknown_07C;
		// 0x80
		long int inProgressCount; // Unit count that are currently being created for this element. Can only be 0 or 1.
		long int aliveCount; // Unit count that are currently alive for this element. Can only be 0 or 1.
		long int buildAttempts; // Used in ROR internal calculations. "bldAtt"
		long int actor; // a unitId, or value for setGatherPercentage
		// Identifies the unit itself. -1 if N/A. In standard game, it is only valued for "alive" units. 
		// With customROR, we also value it for in-progress buildings => in-construction building already has its ID, use it
		// With customROR, we also value it for in-progress living units => store actor's unitID. This way, if building is destroyed, we can cancel its strategy element (otherwise it remains stuck forever)
		// 0x90
		unsigned long int unknown_090;
		unsigned long int unknown_094;
		unsigned long int unknown_098;
		unsigned long int unknown_09C;
		// 0xA0
		long int totalCount; // Total number of created units. "#blds"
		long int retrains; // Number of times the unit will be created. Then, strategy element is inactive (will never be done again). "cap" in game terminology ?
		unsigned long int unknown_0A8_skipCy; // "skipCy" term found in 408891
		char canSkip; // +AC. "permSkip" term found in 408891
		char unused_0AD;
		short int unused_0AE;

		bool IsCheckSumValid() { return this->checksum == 0x00542A3C; }
	};
	static_assert(sizeof(STRUCT_STRATEGY_ELEMENT) == 0xB0, "STRUCT_STRATEGY_ELEMENT size");


	// Included in TacAI (+E4C). Temporary info about building currently analyzed for being built.
	// Size=0x130
	class STRUCT_TAC_AI_BUILD_TO_TRIGGER {
	public:
		long int unitDefId; // DAT_ID of currently analyzed building
		float unknown_04; // SizeRadius1+SizeRadius2 ?
		char bldName_ToBuild[0x100]; // +8. Set (copied) in 0x4BE9C5.
		long int actorUnitId; // +108.
		unsigned long int minDistanceToTC; // +10C.
		unsigned long int maxDistanceToTC; // +110.
		long int infoIsValid; // +114. Flag 0/1. See 4BE350. 1 when information is valid (saved for a next AI treatment loop because it could not be completed last time)
		long int buildSitePosY; // +118.
		long int buildSitePosX; // +11C
		long int buildSiteScore; // +120. Score from "temp map like" values
		long int unknown_124; // minPosY ?????
		long int randomPlacementFactor; // To confirm
		STRUCT_STRATEGY_ELEMENT *strategyElement;
	};
	static_assert(sizeof(STRUCT_TAC_AI_BUILD_TO_TRIGGER) == 0x130, "STRUCT_TAC_AI_BUILD_TO_TRIGGER size");


	// Size = 0x18
	// For gatheres tasks (not builder/repairman/explorer?)
	class STRUCT_VILLAGER_TASKS_ELEM {
	public:
		long int unitId; // Villager unit id
		long int resourceType; // 0-3
		long int maxResourceAmount; // resourceAmount is compared to resourceAmount to known is gathering is "finished" (needs to deposit)
		long int resourceAmount; // +0C. Gathered amount at this point (compared to +08)
		// 0x10
		long int targetUnitId; // building to build, gazelle to hunt, etc
		unsigned long int timeGetTimeValue; // Last evaluation system time in milliseconds. Task changes are evaluated if > 5 seconds ?

		AOE_CONST_FUNC::RESOURCE_TYPES GetResourceType() { return (AOE_CONST_FUNC::RESOURCE_TYPES)this->resourceType; };
	};


	// Size = 0x70
	// This struct is included in TacAI
	class STRUCT_PLANNED_RESOURCE_NEEDS {
	public:
		unsigned long int checksum; // 54 2C 54 00
		unsigned long int unknown_04; // a dword
		unsigned long int unknown_08; // a dword
		long int resourceAmount[8]; // +0C. Only 4 are used, but there's room for 8 resource types. Index is resourceId.
		long int unknown_2C[8]; // Only 4 are used, but there's room for 8 resource types. Init to -1 (a resource type?)
		long int unknown_4C[8]; // Only 4 are used, but there's room for 8 resource types. Init to -1 (a resource type?)
		long int resourceTypesCount; // +0x6C ; is always 4

		bool IsCheckSumValid() { return this->checksum == 0x00542C54; }
	};
	static_assert(sizeof(STRUCT_PLANNED_RESOURCE_NEEDS) == 0x70, "STRUCT_PLANNED_RESOURCE_NEEDS size");

	// Size = 0x80. Included in ConAI
	// Some member names were copied from struct strategy element because it looks very similar. (could have same parent class)
	// But this has to be checked (maybe it's wrong)
	class STRUCT_CON_AI_CONSTRUCTION {
	public:
		unsigned long int checksum; // 94 2A 54 00. Parent = 38 2A 54 00 (same parent as stratElem !)
		long int unitDefId;
		unsigned long int unknown_08; // unit instance id ?
		unsigned long int unknown_0C; // counter ?
		// 0x10
		char unitName[0x40]; // 0x3F + \0 ?
		// 0x50
		float posY; // The position that will actually be used to build the new building
		float posX; // The position that will actually be used to build the new building
		unsigned long int unknown_58;
		unsigned long int unknown_5C;
		// 0x60
		float sizeRadius1; // TO VERIFY : +5C and 60 maybe
		float sizeRadius2; // TO VERIFY
		unsigned int unknown68_skipCount; // TO VERIFY // Unsure. "skip" term found in 408891
		STRUCT_CON_AI_CONSTRUCTION *next;
		// 0x70
		STRUCT_CON_AI_CONSTRUCTION *previous;
		unsigned long int unknown_74;
		unsigned long int unknown_78;
		unsigned long int unknown_7C;

		bool IsCheckSumValid() { return this->checksum == 0x00542A94; }
	};


	// Size = 0x08. Used in InfAI+1F8 array.
	// Elements are never reset when a building is destroyed ?
	// Exact role ? When trying to build a deposit bld, it is used to check there is not another one of same kind in same zone.
	// BUG ? status is not checked (and not up to date) when doing this !
	// Methods: 0x4C3300: infAI.addOrUpdateBldHistory(unitDefId, posY, posX, status)
	// 0x4C34C0: infAI.array1F8_setStatus(unitDefId, posY, posX, status)
	// 0x4C3550: infAI.getAddrOfUnusedElemIn1F8List(DAT_ID)
	// 0x4C3590: infAI.hasBuildingIn1F8(DAT_ID, pos, pos, allowedDistance). This is bugged in standard game because status always remains=1, even if building no longer exists! This may prevent some constructions to be started
	class STRUCT_INF_AI_BUILDS_HISTORY {
	public:
		long int unitDefId;
		AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS status; // +4
		unsigned char posY; // +5
		unsigned char posX; // +6
		char unused;
	};
	static_assert(sizeof(STRUCT_INF_AI_BUILDS_HISTORY) == 0x8, "STRUCT_INF_AI_BUILDS_HISTORY size");


	// Included in tacAI structure. Size=0x2C.
	class STRUCT_TAC_AI_TARGET_INFO {
	public:
		long int targetUnitId;
		float targetEvaluation; // +04. According to SNTargetEvaluationxxx numbers
		long int unknown_08;
		long int infAIUnitElemListIndex; // +C. Index of matching element in infAI.unitListElem.
		// 0x10
		long int unknown_10;
		long int unknown_14;
		char unknown_18; // default 0. is building ? ignore speed? ? Really unsure. See 4C0ABC
		char unused_19[3];
		long int unknown_1C;
		// 0x20
		long int unknown_20;
		long int unknown_24;
		long int unknown_28; // A flag, default 1 ?
	};
	static_assert(sizeof(STRUCT_TAC_AI_TARGET_INFO) == 0x2C, "STRUCT_TAC_AI_TARGET_INFO size");


	// Size = 0x330. Constructor()=0x4CC630
	// tacAI.createGroup(useSequence)=0x4E0400 ; tacAI.removeUnitGroup(unitGroupId)=0x4E04B0
	// Organized as a loop chained list. See TacAI (first elem is included in TacAI).
	class STRUCT_UNIT_GROUP_ELEM {
	public:
		unsigned long int checksum; // E0 8C 54 00
		STRUCT_UNIT_GROUP_ELEM *next;
		STRUCT_UNIT_GROUP_ELEM *previous;
		long int unitGroupId; // A unique ID
		// 0x10
		long int unknown_resetOrg; // +10. resetOrg ?
		AOE_CONST_INTERNAL::UNIT_GROUP_TYPES unitGroupType; // +14. internal id: 64,65,66,67,6A,6B,6C,6D(=artefacts?)
		long int taskSubTypeId;
		long int myUnitsIdArray[0x28]; // +1C. 40 elements, can be non-consecutive (ignore -1 values).
		// 0xBC
		long int myUnitsHPArray[0x28]; // +BC. 40 elements. Also used as myUnitsIdArray+0xA0.
		long int unitCount; // +15C. Must correspond to valid unitIds count in unitIdArray. Warning: 4CCC90 method requires LucTieuPhung's fix to avoid breaking this assertion.
		// 0x160
		long int unitMaximumCount; // Unsure
		long int commanderUnitId; // +164. group leader
		long int totalGroupHP; // +168. or "old" value ? Not always updated: can be quite wrong !
		long int unsure_previousUnitCount; // +16C. Set from +15Cs
		// 0x170
		float unknown_170_posY;
		float unknown_174_posX;
		float unknown_178_posZ;
		unsigned long int unknown_17C; // unknown type
		// 0x180
		AOE_CONST_INTERNAL::UNIT_GROUP_TASK_IDS currentTask; // TaskId, see 4CD339. Values in 0-0x15
		long int targetUnitId; // +184. including defended unit case ?
		long int targetDAT_ID;
		float targetPosY;
		// 0x190
		float targetPosX;
		float unknown_194;
		unsigned long int unknown_198; // seems int. DWORD?
		float posY; // not sure what this is exactly, NOT group "supposed" position ?
		// 0x1A0
		float posX;
		float posZ;
		long int unknown_1A8;
		float unknown_1AC_posY;
		// 0x1B0
		float unknown_1B0_posX;
		float unknown_1B4_posZ;
		unsigned long int unknown_1B8;
		unsigned long int unknown_1BC;
		// 0x1C0
		unsigned long int unknown_1C0;
		unsigned long int unknown_1C4;
		unsigned long int unknown_1C8;
		unsigned long int unknown_1CC;
		// 0x1D0
		long int attackPlayId; // attackId that is being played. -1=Non-Play-based attack
		char unknown_1D4;
		char unknown_1D5;
		short int unknown_1D6; // check type (2 bytes ?)
		// 1D8
		char unknown_1D8[0x2C8 - 0x1D8];
		char unknown_2C8; // A counter ? 0x4CD42F
		char unknown_2C9;
		char unknown_2CA; // ?
		char unknown_2CB; // ?
		long int targetUnitIdArrayUsedElemCount; // +2CC. Index of first UNused element in targetUnitIdArray (=> values 0-14). See 0x4CEBB6
		long int targetUnitIdArray[20]; // +2D0. Array of unitIDs. Total size 0x50 bytes (0x14*4)
		long int targetPlayerId; // +320. Related to targetUnitIdArray unit's playerId (last inserted? All the same player?)
		long int unknown_324; // a dword, but containing a 1-byte value. Bool, maybe.
		long int unknown_328_gameTime; // Last tasking time, consistent with global+4.
		unsigned long int unknown_gameTime_ms; // +0x32C. To confirm
		// End (0x330)
		bool IsCheckSumValid() { return this->checksum == 0x00548CE0; }
	};


	// The common object included in each AI section (with xxxAI text)
	// Size = 0xEC
	class STRUCT_COMMON_AI_OBJECT { // 00 2C 54 00 in 1.0c
	public:
		unsigned long int checksum;
		unsigned long int AITypeId; // 1000=MainDecisionAI, 1001=BuildAI, 1002=ConAI... Is this used somewhere ?
		char AIType_text[0x38]; // Exact size ?
		// 0x40
		unsigned long int unknown_040;
		unsigned long int unknown_044;
		long int playerId;
		char playerName[0x40]; // size=64
		unsigned long int unknown_08C; // (3 bytes + 1 unused?)
		// 0x90
		char unknown_080[0x10];
		// 0xA0
		char unknown_0A0[0x40];
		// 0xE0
		unsigned long unknown_0E0; // init = 5 ?
		unsigned long unknown_0E4; // maximum population ?
		unsigned long unknown_0E8;
		// 0xEC: end

		bool IsCheckSumValid() { return this->checksum == 0x00542C00; }
	};


	// Size 0x5C0 - Constructor 0x4B7650
	// Checksums: CC 8A 54 00 (normal), parents=40 2A 54 00
	class STRUCT_BUILD_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1001
		// 0xF0: 1st strategy element
		STRUCT_STRATEGY_ELEMENT fakeFirstStrategyElement; // The first strategy element (empty)
		// 0x1A0
		long int strategyElementCount; // = number of strategy elements (NOT counting fake first elem) = ID for the next element to add
		char strategyFileName[0x40]; // +1A4. Size: unsure
		char unknown_1E4[0x2A5 - 0x1E4];
		char lastBuildName[0x101]; // +2A5: text
		char currentBuildName[0x101]; // +3A6: text (desired build unit name?)
		char nextBuildName[0x101]; // +4A7: text (desired build unit name?)
		long int currentIDInStrategy; // +5A8. Corresponds to stratelem.elemId, not "index". ID 2 might NOT be 3rd element due to inserted elems.
		STRUCT_AI_UNIT_LIST_INFO autoInsertedBuildingDATIDs; // +5AC. Actually, IDs are DATID, not unit instance IDs. Automatically added buildings ?
		STRUCT_AI *mainAI; // +5BC. Set in 0x4B76E0.
		// 0x5C0: end
		bool IsCheckSumValid() const { return this->checksum == 0x00548ACC; }
	};
	static_assert(sizeof(STRUCT_BUILD_AI) == 0x5C0, "STRUCT_BUILD_AI Size");


	// size 0x318 - constructor 0x4B9660.
	// Checksums 18 8B 54 00, parents=98 2A 54 00
	// Construction AI
	class STRUCT_CON_AI {
	public:
		unsigned long int checksum; // 18 8B 54 00
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1002
		// 0xF0
		long int unknown_0F0; // Tested in 4B9A4D
		STRUCT_CON_AI_CONSTRUCTION tempFakeConstructionInfo1; // +F4. First element of a chained list. The constructions that still have to BE STARTED ?
		unsigned long int unknown_174;
		STRUCT_CON_AI_CONSTRUCTION tempFakeConstructionInfo2; // +178. First element of a chained list. Independant from tempConstructionInfo1 !
		char cityPlanName[0x100]; // +1F8. For scenario, selected city plan name. Size is unsure (maybe 101 + 3 unused bytes ?)
		unsigned long int unknown_2F8;
		float townCenter_posY;
		// 0x300
		float townCenter_posX;
		float townCenter_posZ; // unsure !
		long int mapSizeY; // +308
		long int mapSizeX; // +30C
		unsigned long int unknown_310; // unknown type
		unsigned long int ptrMainAI; // +314
		// 0x318: end
		bool IsCheckSumValid() { return this->checksum == 0x00548B18; }
	};
	static_assert(sizeof(STRUCT_CON_AI) == 0x318, "STRUCT_CON_AI size");


	// size 0x150 - constructor=0x40A780
	// Checksums : F4 2A 54 00, parents=04 2C 54 00
	class STRUCT_DIPLOMACY_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1003
		// 0xF0
		unsigned long int ptrMainAI; // +F0.
		// 0xF4
		unsigned long int dislikeTable[10]; // +F4. dislike values against all players. Why is there a "player9" value ?
		//unsigned long int unknown_118;
		// 0x11C
		unsigned long int likeTable[10]; // +11C. like values against all players. Why is there a "player9" value ?
		//unsigned long int unknown_140;
		// 0x144
		char isNeutral[9]; // 0 or 1. To confirm
		char unused_14D;
		short int unused;
		// 0x150: end
		bool IsCheckSumValid() { return this->checksum == 0x00542AF4; }
	};
	static_assert(sizeof(STRUCT_DIPLOMACY_AI) == 0x150, "STRUCT_DIPLOMACY_AI size");

	// size 0x1C0 - constructor=0x40AFC0. (mainAI+0xB2C)
	// Checksums : 34 2B 54 00, parents=04 2C 54 00
	class STRUCT_EMOTIONAL_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1004
		// 0xF0
		STRUCT_AI *mainAI;
		long int attitudeValues[6]; // +F4
		char attitudeNames[6][0x1E];
		bool IsCheckSumValid() { return this->checksum == 0x00542B34; }
	};
	static_assert(sizeof(STRUCT_EMOTIONAL_AI) == 0x1C0, "STRUCT_EMOTIONAL_AI size");


	// size 0x10090 - constructor=0x4BA1C0. Offset is AI+0xCEC
	// Checksums : 74 8B 54 00, parents=04 2C 54 00, 74 2B 54 00
	class STRUCT_INF_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1005
		// 0xF0
		STRUCT_AI_UNIT_LIST_INFO unknown_0F0; // Unused ?? See 0x4DB4E2.
		// 0x100
		STRUCT_AI *ptrMainAI;
		long int YMapSize;
		long int XMapSize;
		unsigned long int unknown_10C;
		// 0x110
		long int unitElemListSize; // +110. Total allocated size of unitElemList (the array can contain some "-1")
		// Unit lists : see 0x4BDD10. In those 4 unit lists, elements are never cleaned/removed !
		STRUCT_INF_AI_UNIT_LIST_ELEM *unitElemList; // +114. Contains info about units that can be created by players + resources + artefacts
		STRUCT_AI_UNIT_LIST_INFO creatableAndGatherableUnits; // +118. units that can be created by players + resources (gazelle but not other animals) + artefacts
		STRUCT_AI_UNIT_LIST_INFO playerCreatableUnits; // +128. All player-creatable units (villager, military, buildings...). NOT resources.
		STRUCT_AI_UNIT_LIST_INFO artefactsAndFlags; // +138. Store the found flags/artefacts.
		STRUCT_AI_UNIT_LIST_INFO elementsToDefend; // +148. Can be TC, dock, relic, ruin, mines+bushes. Related to SNxxxDefendPriority
		STRUCT_TEMP_MAP_BUILD_LIKE_INFOS buildMapLikeInfo; // +158. Size to check
		// Remove unit from allMyUnits & buildingUnits: 0x4BF440.
		STRUCT_AI_UNIT_LIST_INFO allMyUnits; // +180. To confirm.
		STRUCT_AI_UNIT_LIST_INFO buildingUnits; // +190
		// 0x1A0
		char unknown_1A0[0x1F0 - 0x1A0];
		// 0x1F0
		unsigned long int unknown_1F0;
		long int buildHistoryArraySize; // +1F4. Allocated element count of +1F8 list. (default=0xC8=200)
		STRUCT_INF_AI_BUILDS_HISTORY *buildHistoryArray; // +1F8.
		long int unknown_1FC; // Element count in unknown_200_ptrSomeArray array ?
		// 0x200
		// Element size = 0x18. see 4C3719,4C3FF2. +0=dword=index(-1=bad?), +4=? +(5,6)=byte(minposYX?),(7,8)=byte(maxposYX?)
		// +9=playerIdAttacker(notme) +A=playerId_me? +C=word +E=byte +10=ptr? +14=dword
		// Related to being attacked ? add in 4C3660.
		unsigned long int *unknown_200_ptrSomeArray;
		char unknown_204[0x40C - 0x204];
		char unknown_40C[0x4D0 - 0x40C]; // 40C + x*4 = counter? arraySize=?
		// 0x4D0
		STRUCT_SPOTTED_RESOURCE_INFO *spottedGatherableUnitsByResourceTypeArrays[4]; // +4D0. Stores explored resources, their position and (closest) storage building
		long int spottedGatherableUnitsCountByResourceType[4]; // +4E0. Number of elements in +4D0+i array ? (i=resource type). Related to +540, +550
		long int spottedGatherableUnitsByResourceTypeArraySizes[4]; // +4F0. 4 Array sizes (allocated sizes ?)
		// 0x500
		unsigned long int unknown_500[4];
		unsigned long int unknown_510[4];
		unsigned long int unknown_520[4];
		unsigned long int unknown_530[4];
		// 0x540
		long int bestForageDropSiteDistance; // +540. Best distance from forage/farm to storage (granary or TC)
		long int bestWoodDropSiteDistance; // +544. Best distance from tree to storage (SP or TC)
		long int bestStoneDropSiteDistance; // +548. Best distance from stone mine to storage (SP or TC)
		long int bestGoldDropSiteDistance; // +54C. Best distance from gold mine to storage (SP or TC)
		// 0x550
		long int bestForageUnitId; // closest target unitId for food (farms & forage only, not hunting)
		long int bestWoodUnitId; // +554. closest target unitId for wood
		long int bestStoneUnitId; // +558. closest target unitId for stone
		long int bestGoldUnitId; // +55C. closest target unitId for gold
		// 0x560
		long int foundForestTiles;
		char unknown_564[0xFF64 - 0x564];
		long int humanPlayer_militaryUnitCountByType[AOE_CONST_INTERNAL::INTERNAL_MILITARY_UNIT_TYPES::CST_IMUT_COUNT]; // 0xFF64 - used only if TrackHumanHistory is ON
		char learnUHFileName[0x100]; // +0xFF90. Name of xxx.uh file. Not sure this is really used.
		// 0x10090: end

		bool IsCheckSumValid() { return this->checksum == 0x00548B74; }
	};
	static_assert(sizeof(STRUCT_INF_AI) == 0x10090, "STRUCT_INF_AI size");


	// size 0xF8 - constructor=0x4C8B50
	class STRUCT_RESOURCE_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1007
		// 0xF0
		unsigned long int ptrMainAI;
		unsigned long int resourceTypeCount; // = 4
	};


	// size 0x5F8 - constructor=0x4CB030
	class STRUCT_STRATEGY_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1008
		// 0xF0
		STRUCT_AI *ptrMainAI;
		unsigned long int unknown_0F4;
		char tempPerFile[0x100]; // +F8
		char unknown_1F8[0x234 - 0x1F8];
		long int unknown_counter; // Updated each time strategy AI is run ? (in fact, always reset to 0)
		STRUCT_AI_UNIT_LIST_INFO unknown_238; // List of SN numbers (index) ? For debugging ?
		STRUCT_AI_UNIT_LIST_INFO unknown_248; // List of SN numbers (index) ? For debugging ?
		STRUCT_AI_UNIT_LIST_INFO unknown_258; // List of SN numbers (index) ? For debugging ?
		unsigned long int structPersonalityChecksum; // +268. 9C 8C 54 00
		long int SNNumber[0xE3]; // +26C. only used in init phases ?
		// 0x5F8: end
	};


	// Tactical AI. Lots of things are managed here !
	// size 0x100C - Constructor=0x4CEBE0. Corresponds to MainAI+0x1146C
	class STRUCT_TAC_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=?
		// 0xF0
		STRUCT_AI *ptrMainAI;
		STRUCT_AI_UNIT_LIST_INFO allVillagers; // +F4. Array can contain empty values (-1). Does NOT contain any boat ! 
		//long int *ptrAllVillagersUnitIdArray; // Array of unitIDs (all villagers). May contain empty values (-1) ?
		//long int myVillagerCount; // +F8. exact count of villagers in ptrAllVillagersArray
		//unsigned long int unknown_0FC;
		// 0x100
		//long int allVillagersArraySize; // Size for ptrAllVillagersArray. Can be higher than exact count (if empty spaces).
		STRUCT_AI_UNIT_LIST_INFO villagerExplorers; // +104.
		STRUCT_AI_UNIT_LIST_INFO militaryUnits; // +114. military unitIds that do NOT belong to a unitGroup. Does NOT contain boats, nor towers. Includes priests.
		STRUCT_AI_UNIT_LIST_INFO unknown_124; // NOT SURE it is a STRUCT_AI_UNIT_LIST_INFO
		STRUCT_AI_UNIT_LIST_INFO allBoats; // +134. Includes both fishing + military boats
		STRUCT_AI_UNIT_LIST_INFO warShips; // +144
		STRUCT_AI_UNIT_LIST_INFO fishingShips; // +154
		STRUCT_AI_UNIT_LIST_INFO tradeShips; // +164
		STRUCT_AI_UNIT_LIST_INFO transportShips; // +174
		STRUCT_AI_UNIT_LIST_INFO relics; // +184. Relics only ? What is sure is that is does NOT include ruins.
		long int SNNumber[0xE2]; // +194. In game, we use this one (not strategyAI's). Total size=0x388 (4*0xE2). index from 0 to E1. (missing 1 SN?)
		// 0x51C
		// Here: a list of small structures size=0x18 (0x32 elems?) = gatherers and their tasks (excluding explorers). builders/repairmen NOT in this array?
		AOE_STRUCTURES::STRUCT_VILLAGER_TASKS_ELEM gatherersTasks[0x32];
		unsigned long int villagerTasksRelevantElemCount; // +9CC. Number of RELEVANT element count in villagerTasks array (array size is fixed, it's included in this structure !)
		// 0x9D0
		long int nonExplorerVillagersCount; // +9D0. Counts all villagers BUT explorers
		AOE_STRUCTURES::STRUCT_UNIT_GROUP_ELEM fakeFirstUnitGroupElem; // +9D4. Organized as a circular list (each elem is a group) ? This one = "fake" elem (like in strategy)
		unsigned long int seqUnitGroupId; // +D04. Is = next group's ID.
		unsigned long int unitGroupsCount; // +D08. This does NOT count fakeFirstUnitGroupElem (so it CAN be 0).
		unsigned long int unknown_D0C; // a Value in milliseconds (cmp to global+4)
		// 0xD10
		unsigned long int unknown_D10;
		unsigned long int lastAttackResponseTime_ms; // +D14. Value in milliseconds. Used to compute +D24 ?
		unsigned long int unknown_D18;
		long int lastTacticalUpdateTime; // +D1C. Value in milliseconds
		// 0xD20
		unsigned long int buildFrequencyCounter; // incremented until it reaches SNBuildFrequency
		unsigned long int timeSinceLastAttackResponse_seconds; // To confirm. Compared to SNAttackResponseSeparationTime. See 4E0BC0.
		unsigned long int unknown_D28_ptrTargetInfo; //ptr to playerIdTarget? SEEMS TO BE WHAT DEFINES WHO TO ATTACK?
		unsigned long int unknown_D2C;
		// 0xD30
		unsigned long int unknown_D30;
		unsigned long int unknown_D34; // some counter ? Related to buildAI?
		unsigned long int unknown_D38;
		unsigned long int unknown_D3C;
		// 0xD40
		unsigned long int unknown_D40;
		unsigned long int unknown_D44;
		STRUCT_AI_UNIT_LIST_INFO IdleOrInRangeMilitaryUnits; // +D48. available military units (idle OR in range from TC?) for temp treatments ? See 4D8960
		STRUCT_AI_UNIT_LIST_INFO unknownUnits_D58; // TC + villagers ? + others? Builders ?
		unsigned long int gathererCount_actual[4]; // +D68. Index IS resource ID.
		long int gathererCount_desired[AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_BASIC_RESOURCE_COUNT]; // +D78. Villager count we want to assign to each type of resource (4). Index IS resource Type ID
		long int extraResourceTypeNeededByPriority[4]; // +D88 First=most needed. Use extraResourceAmount with this index to get resource amount. <0 amount = I must collect
		long int extraResourceAmount[4]; // +D98. Index is always the same (0=food, 1=food...) <0 amount = I need to collect some. "resDiff"
		STRUCT_PLANNED_RESOURCE_NEEDS plannedResourceNeeds; // +DA8, size=0x70. Related to //SNMostNeededResourceLookAhead (number of stratElem to take into account)
		// 0xE18
		long int attackEnabled; // What is exact meaning ? 1 if attack is currently being run ? Or starts with 0, once set to 1, AI can attack ?
		AOE_CONST_INTERNAL::AI_UPDATE_TYPES currentAIUpdateType; // +E1C. loop ID for TacAI treatments types.
		// 0xE20
		long int unknown_E20; // a dword, a flag - related to villagers/gathering ?
		char unknown_E24[0xE44 - 0xE24];
		long int myWonderBeingBuiltFlag; // +E44. true(1) or false(0) value.
		long int myWonderIsBuiltFlag; // +E48. true(1) or false(0) value.
		STRUCT_TAC_AI_BUILD_TO_TRIGGER constructionToTrigger; // E4C : an included temporary struct (=>F78 or F80?) about building currently analyzed for being built. +0=DATID to build. +108=actorUnitId?
		long int currentAIUpdateVillagerId; // Currently evaluated villager unitId (from allVillagers array). Set to -1 when I have no villager or out of loop ?
		long int unknown_F80; // signed dword
		long int unknown_F84; // internal ID ? 4D1CF7. Related to attacking
		long int storagePitAddedToStrategy; // +F88. 1 when a SP has been inserted into strategy.
		long int granaryAddedToStrategy; // +F8C. 1 when a granary has been inserted into strategy.
		// 0xF90
		long int unknown_F90_someTimeValue; // 4D2245,4D242A...
		long int lastPanicModeTime; // +F94. In seconds. To be compared with globalStruct+4
		long int unknown_F98_timeAboutTributes; // Related to SNCoopDemandTributeInterval
		unsigned long int unknown_F9C;
		// 0xFA0
		unsigned long int lastCoopSharAttackTime_ms; // A game time value in milliseconds.
		unsigned long int unknown_FA4; // flag ? about exploration ? Used in gatherer affectation methods
		unsigned long int unknown_FA8;
		STRUCT_TAC_AI_TARGET_INFO targetInfo; // +FAC.
		long int attacksByPlayerCount[9]; // +FD8. number of times this player attacked me ?
		long int lastUAms; // +FFC. "LastUAms" (UpdateAI). Time spent in last updateAI execution
		long int averageUpdateAITime_ms; // +1000. Updated each time unknown_1004_time_counter reaches 20 and is reset. "Average UAms" ?
		long int calcAverageUpdateAITime_counter; // Maximum value = 20 = 0x14
		long int lastAIUpdateTime_ms; // +1008. A timeGetTime value. (not a game time)

		bool IsCheckSumValid() { return this->checksum == 0x00548CF4; }
	};
	static_assert(sizeof(STRUCT_TAC_AI) == 0x100C, "STRUCT_TAC_AI size");

	// Size 0xF4 - constructor=0x40ED50
	class STRUCT_TRADE_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1010
		// 0xF0
		STRUCT_AI *ptrMainAI;
		bool IsCheckSumValid() { return this->checksum == 0x00542CD0; }
	};
	static_assert(sizeof(STRUCT_TRADE_AI) == 0xF4, "STRUCT_TRADE_AI size");

	// Size = 0x24. Update_Add=0x4BD750/0x4BD7E0
	// Organized as a (big, initially 0x1F4 elements) array in infAI structure
	// Empty slots are re-used when adding an entry.
	// In standard games, entries are NEVER reset !!!! which leads to some very bad behavior (for example, to evaluate if panic mode is needed)
	class STRUCT_INF_AI_UNIT_LIST_ELEM {
	public:
		long int unitId; // -1 means free (empty) slot. Bug in 4C41BD ?
		short int unitDATID; // +04
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitClass; // +06 - short int
		unsigned char posY; // +08
		unsigned char posX;
		unsigned char posZ;
		char playerId; // 0x0B
		//float HP; // +C. unit health points
		short int HP; // +C. unit health points
		short int unused_0E;
		// 0x10
		long int unknown_10; // an int used in target rating evaluation, multiplied with //SNTargetEvaluationAttackAttempts in 4C0852
		char unknown_14;
		char unused_15;
		short int unused_16;
		float attack;
		float reloadTime1;
		// 0x20
		float maxRange;
	};

	// The main AI object. Total size = 0x125C8. Constructor = 0x4C6960.
	// Checksums: F0 8B 54 00 (normal) + parents 04 2C 54 00, B0 2B 54 00
	class STRUCT_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT structMainDecisionAI; // size 0xEC
		// 0xF0
		AOE_STRUCTURES::STRUCT_PLAYER *ptrStructPlayer;

		STRUCT_AI_UNIT_LIST_INFO allMyUnits; // +F4. Can contain empty elements ? Remove in 40B950.
		STRUCT_BUILD_AI structBuildAI; // +0x104
		STRUCT_CON_AI structConAI; // +0x6C4 ; city plan
		STRUCT_DIPLOMACY_AI structDiplAI; // +0x9DC
		STRUCT_EMOTIONAL_AI structEmotionalAI; // +0xB2C
		STRUCT_INF_AI structInfAI; // +0xCEC
		STRUCT_RESOURCE_AI structResourceAI; // +0x10D7C
		STRUCT_STRATEGY_AI structStrategyAI; // +0x10E74
		STRUCT_TAC_AI structTacAI; // +0x1146C
		STRUCT_TRADE_AI structTradeAI; // +0x12478

		STRUCT_PLAYER *player; // +1256C. Pointer to player structure
		// +12570. =1 until game start AI init has been done. Includes log like values, add dynamic strategy, resource AI bonus, tribute info init
		// Bug: this information is NOT saved in savegame files (whereas hasDiplomacyInitBeenDone is). When loading a game, it is set to 0 even if init has never been run.
		long int needGameStartAIInit;
		unsigned long int unknown_12574;
		long int nextStrategyAIExecutionCounter; // +12578. Increased, when = 0x14=20 then reset and execute strategy AI.
		unsigned long int unknown_1257C; // A milliseconds time value ? Related to diplomacy management ?
		unsigned long int unknown_12580; // +12580. A milliseconds time value ?
		unsigned long int unknown_12584; // +12584. A milliseconds time value ? Related to tributes ??
		unsigned long int tributeChatFrequency; // +12588. Including randomness. Calculated from SNTributeChatFrequency and SNTributeChatRandomness
		unsigned long int unknown_1258C;
		// 0x12590
		unsigned long int unknown_12590;
		unsigned long int unknown_12594_tributeIsNOTPersistent; // REALLY unsure.
		unsigned long int unknown_12598;
		unsigned long int hasDiplomacyInitBeenDone; // +1259C. 0/1 flag. Waits 10 seconds (0x2710=10000ms) before this init. cf 4C783F.
		unsigned long int requiredTributeToChangeDiplomacy; // +125A0
		long int receivedTributeFromPlayer[9]; // +125A4. Total tribute received from a player (no distinction on resource type). See 4C83B5


		bool IsCheckSumValid() const { return this->checksum == 0x00548BF0; }
	};


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

