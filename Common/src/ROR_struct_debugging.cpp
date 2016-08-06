#include "../include/AOE_struct_debugging.h"



// Concatenate macros/values (internal - do not use directly)
#define PPCAT_NX(A, B) A ## B
// Concatenate macros/values (use this one)
#define PPCAT(A, B) PPCAT_NX(A, B)

// inits a local variable for given class and sets result="className" if checksum matches.
#define GET_STRUCT_NAME_IF_MATCHING(x) x *PPCAT(obj_, x) = NULL; \
if (((x *)pData)->IsCheckSumValid()) { \
PPCAT(obj_, x) = (x *)pData; \
result = _STRINGIZE(x); \
}

// Returns sizeof(<class>) if checksum matches
#define GET_STRUCT_SIZE_IF_MATCHING(x) if (((x *)pData)->IsCheckSumValid()) { return sizeof(x); }



namespace AOE_STRUCTURES {

	long int GuessRORStructureSize(void *pData) {
		//if (((STRUCT_ACTION_ATTACK *)pData)->IsCheckSumValid()) { return "STRUCT_ACTION_ATTACK"; }
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_ACTION_ATTACK);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_ACTION_CONVERSION);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_ACTION_DISCOVERY_ARTEFACT);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_ACTION_LINK); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_ACTION_MAKE_OBJECT);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_ACTION_MOVE);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_AI);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_ANY_UI);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_ARMOR_OR_ATTACK); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_BUILD_AI);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_COMMAND_LINE_INFO); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_COMMON_AI_OBJECT);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_CON_AI);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_DEF_CIVILIZATION);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_DEF_UNIT);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_DEF_UNIT_COMMAND); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_DEF_UNIT_COMMAND_HEADER);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_DIPLOMACY_AI);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_EMOTIONAL_AI);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_GAME_COMMANDS_INFO);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_GAME_GLOBAL);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_GAME_MAP_INFO);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_GAME_MAP_TILE_INFO); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_GAME_SETTINGS);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_INF_AI);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_INF_AI_UNIT_LIST_ELEM); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAIN_MUSIC); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAIN_SOUND); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAPGEN_BASE_ZONE_INFO); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAPGEN_ELEVATION_DATA); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAPGEN_ELEVATION_INFO); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAPGEN_ELEVATION_LINK); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAPGEN_MAP_TYPE); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAPGEN_OBJECT_TO_PLACE); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAPGEN_PLAYER_BASE_INFO); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAPGEN_TERRAINS_INFO); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAP_GENERATION_INFO);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_MAP_VISIBILITY_INFO); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_PATH_FINDING_INTERMEDIATE_STEP); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_PER_TYPE_UNIT_LIST_ELEMENT); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_PER_TYPE_UNIT_LIST_LINK);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_PLANNED_RESOURCE_NEEDS);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_PLAYER);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_PLAYER_BUILDINGS_HEADER); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_PLAYER_MAP_INFO); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_PLAYER_RESEARCH_INFO); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_PLAYER_RESEARCH_STATUS); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_PLAYER_UNKNOWN_58_AND_6C); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_POSITION_INFO); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_RESEARCH_DEF); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_RESEARCH_DEF_INFO); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_RESOURCE_AI);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_SCENARIO_INFO);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_SCORE_ELEM); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_SCORE_HEADER);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_SOUND_DATA_ELEMENT); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_STRATEGY_AI);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_STRATEGY_ELEMENT);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_TAC_AI);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_TECH_DEF); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_TECH_DEF_EFFECT); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_TECH_DEF_INFO);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_TEMP_MAP_BUILD_LIKE_INFOS);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_TEMP_MAP_POSITION_INFO); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_TERRAIN_DEF); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_TRADE_AI);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_BUTTON);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_CAMPAIGN_EDITOR);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_COMBOBOX);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_DIAMOND_MAP);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_DIAM_MAP);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_DIPLOMACY_DIALOG);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_GAME_SETTINGS);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_IN_GAME_MAIN);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_IN_GAME_OPTIONS);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_IN_GAME_TEXT_ZONE);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_LABEL);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_LISTBOX);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_LIST_ITEM); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_LOAD_SAVED_GAME);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_PLAYING_ZONE);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_POPUP_DIALOG);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_SCENARIO_EDITOR_MAIN);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_SCENARIO_EDITOR_MAIN_MENU);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_SCROLLBAR);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_TEXTBOX);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UI_JOIN_SCREEN);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNITDEF_BASE);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNITDEF_BIRD);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNITDEF_BUILDING);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNITDEF_DEAD_FISH);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNITDEF_DOPPLEGANGER);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNITDEF_FLAG);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNITDEF_LIVING);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNITDEF_PROJECTILE);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNITDEF_TREE);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNITDEF_TYPE50);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_ACTION_INFO);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_ACTIVITY);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_ACTIVITY_TARGET_ELEM); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_BASE);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_BIRD);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_BUILDING);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_DEAD_FISH);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_DOPPLEGANGER);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_FLAG);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_GROUP_ELEM);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_LIVING);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_MOVEMENT_INFO); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_PROJECTILE);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_TARGET_POS); // No checksum
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_TREE);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNIT_TYPE50);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNKNOWN_GAME_TIME);
		GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNKNOWN_MAP_DATA_F04C);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNKNOWN_MAP_INFO_7D2058); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNKNOWN_MAP_INFO_7D2058_PLAYERELEM);
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_UNKNOWN_P580DA8); // No checksum
		//GET_STRUCT_SIZE_IF_MATCHING(STRUCT_VILLAGER_TASKS_ELEM); // No checksum
		return 0; // not found
	}

	char *GuessRORStructure(void *pData) {
		char *result = "";
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_ACTION_ATTACK);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_ACTION_CONVERSION);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_ACTION_DISCOVERY_ARTEFACT);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_ACTION_LINK); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_ACTION_MAKE_OBJECT);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_ACTION_MOVE);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_AI);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_ANY_UI);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_ARMOR_OR_ATTACK); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_BUILD_AI);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_COMMAND_LINE_INFO); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_COMMON_AI_OBJECT);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_CON_AI);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_DEF_CIVILIZATION);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_DEF_UNIT);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_DEF_UNIT_COMMAND); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_DEF_UNIT_COMMAND_HEADER);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_DIPLOMACY_AI);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_EMOTIONAL_AI);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_GAME_COMMANDS_INFO);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_GAME_GLOBAL);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_GAME_MAP_INFO);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_GAME_MAP_TILE_INFO); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_GAME_SETTINGS);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_INF_AI);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_INF_AI_UNIT_LIST_ELEM); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAIN_MUSIC); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAIN_SOUND); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAPGEN_BASE_ZONE_INFO); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAPGEN_ELEVATION_DATA); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAPGEN_ELEVATION_INFO); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAPGEN_ELEVATION_LINK); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAPGEN_MAP_TYPE); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAPGEN_OBJECT_TO_PLACE); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAPGEN_PLAYER_BASE_INFO); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAPGEN_TERRAINS_INFO); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAP_GENERATION_INFO);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_MAP_VISIBILITY_INFO); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_PATH_FINDING_INTERMEDIATE_STEP); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_PER_TYPE_UNIT_LIST_ELEMENT); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_PER_TYPE_UNIT_LIST_LINK);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_PLANNED_RESOURCE_NEEDS);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_PLAYER);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_PLAYER_BUILDINGS_HEADER); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_PLAYER_MAP_INFO); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_PLAYER_RESEARCH_INFO); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_PLAYER_RESEARCH_STATUS); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_PLAYER_UNKNOWN_58_AND_6C); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_POSITION_INFO); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_RESEARCH_DEF); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_RESEARCH_DEF_INFO); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_RESOURCE_AI);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_SCENARIO_INFO);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_SCORE_ELEM); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_SCORE_HEADER);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_SOUND_DATA_ELEMENT); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_STRATEGY_AI);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_STRATEGY_ELEMENT);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_TAC_AI);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_TECH_DEF); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_TECH_DEF_EFFECT); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_TECH_DEF_INFO);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_TEMP_MAP_BUILD_LIKE_INFOS);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_TEMP_MAP_POSITION_INFO); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_TERRAIN_DEF); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_TRADE_AI);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_BUTTON);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_CAMPAIGN_EDITOR);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_COMBOBOX);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_DIAMOND_MAP);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_DIAM_MAP);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_DIPLOMACY_DIALOG);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_GAME_SETTINGS);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_IN_GAME_MAIN);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_IN_GAME_OPTIONS);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_IN_GAME_TEXT_ZONE);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_LABEL);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_LISTBOX);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_LIST_ITEM); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_LOAD_SAVED_GAME);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_PLAYING_ZONE);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_POPUP_DIALOG);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_SCENARIO_EDITOR_MAIN);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_SCENARIO_EDITOR_MAIN_MENU);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_SCROLLBAR);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_TEXTBOX);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UI_JOIN_SCREEN);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNITDEF_BASE);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNITDEF_BIRD);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNITDEF_BUILDING);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNITDEF_DEAD_FISH);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNITDEF_DOPPLEGANGER);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNITDEF_FLAG);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNITDEF_LIVING);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNITDEF_PROJECTILE);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNITDEF_TREE);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNITDEF_TYPE50);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_ACTION_INFO);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_ACTIVITY);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_ACTIVITY_TARGET_ELEM); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_BASE);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_BIRD);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_BUILDING);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_DEAD_FISH);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_DOPPLEGANGER);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_FLAG);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_GROUP_ELEM);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_LIVING);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_MOVEMENT_INFO); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_PROJECTILE);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_TARGET_POS); // No checksum
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_TREE);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNIT_TYPE50);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNKNOWN_GAME_TIME);
		GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNKNOWN_MAP_DATA_F04C);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNKNOWN_MAP_INFO_7D2058); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNKNOWN_MAP_INFO_7D2058_PLAYERELEM);
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_UNKNOWN_P580DA8); // No checksum
		//GET_STRUCT_NAME_IF_MATCHING(STRUCT_VILLAGER_TASKS_ELEM); // No checksum

		if (*result == 0) {
			return result; // Nonsense... But useful for breakpoint (on next "return") if we want to stop only when object was found !
		}
		// Set a breakpoint here.
		// In local variables, see obj_<found_class_name>
		// Of course it is required that provided buffer (pData) contains the entire object, not only the checksum
		return result;
	}


	// Get the name of the structure contained at "address".
	// You can set a breakpoint in "GuessRORStructure" to get more run-time information.
	char *DebugStructAtAddress(HANDLE h, unsigned long int address) {
		char *res = "";
		static char outbuf[11];
		DWORD buf4;
		if (h == NULL) { return res; }
		SIZE_T readBytes;
		BOOL ok = ReadProcessMemory(h, (void*)address, &buf4, 4, &readBytes);
		if ((readBytes == 4) && (ok)) {
			sprintf_s(outbuf, "0x%08X", buf4);
			long int objsize = AOE_STRUCTURES::GuessRORStructureSize(&buf4);
			if (objsize <= 0) { return outbuf; }
			// Get the entire object (only useful for debugging with breakpoints)
			char *objBuffer = (char*)malloc(objsize);
			ok = ReadProcessMemory(h, (void*)address, objBuffer, objsize, &readBytes);
			if (ok && (readBytes == objsize)) {
				// Set a breakpoint at the end of GuessRORStructure to visualize object and its members.
				res = AOE_STRUCTURES::GuessRORStructure(objBuffer);
			}
			free(objBuffer);
		}
		if (res[0] == 0) {
			return outbuf;
		}
		return res;
	}


	RORDebugger::RORDebugger() {
		this->handleROR = NULL;
		this->RefreshMainGameStructs(); // will ZeroMemory...
	}

	bool RORDebugger::RefreshGameSettings() {
		if (!this->handleROR) {
			this->gameSettingsAddr = 0;
			ZeroMemory(&this->gameSettings, sizeof(this->gameSettings)); 
			return false;
		}
		if (!GetObjectFromRORData(this->handleROR, AOE_OFFSETS::ADDR_VAR_GAME_SETTINGS_STRUCT, &this->gameSettingsAddr)) {
			this->gameSettingsAddr = 0;
			return false;
		}
		if (!GetObjectFromRORData(this->handleROR, this->gameSettingsAddr, &this->gameSettings)) {
			this->gameSettingsAddr = 0;
			return false;
		}
		assert(this->gameSettings.IsCheckSumValid());
		return this->gameSettings.IsCheckSumValid();
	}

	bool RORDebugger::RefreshGameGlobal() {
		if (!this->handleROR || !this->gameSettings.IsCheckSumValid()) {
			ZeroMemory(&this->gameGlobal, sizeof(this->gameGlobal));
			return false;
		}
		if (!GetObjectFromRORData(this->handleROR, this->gameSettings.ptrGlobalStruct, &this->gameGlobal)) {
			return false;
		}
		assert(this->gameGlobal.IsCheckSumValid());
		return this->gameGlobal.IsCheckSumValid();
	}

	bool RORDebugger::RefreshGameMapInfo() {
		if (!this->handleROR || !this->gameGlobal.IsCheckSumValid()) {
			ZeroMemory(&this->gameMapInfo, sizeof(this->gameMapInfo));
			return false;
		}
		if (!GetObjectFromRORData(this->handleROR, this->gameGlobal.gameMapInfo, &this->gameMapInfo)) {
			return false;
		}
		assert(this->gameMapInfo.IsCheckSumValid());
		return this->gameMapInfo.IsCheckSumValid();
	}

	bool RORDebugger::RefreshMapVisibilityInfo() {
		if (!this->handleROR || !this->gameGlobal.IsCheckSumValid()) {
			ZeroMemory(&this->mapVisibilityInfoRows, sizeof(this->mapVisibilityInfoRows));
			return false;
		}
		if (!GetObjectFromRORData(this->handleROR, AOE_OFFSETS::ADDR_MAP_VISIBILITY_INFO, &this->mapVisibilityInfoRows)) {
			return false;
		}
		// Now get all data (columns)
		for (int i = 0; i < 255; i++) {
			AOE_STRUCTURES::STRUCT_MAP_VISIBILITY_INFO *v = this->mapVisibilityInfoRows[i];
			if (v != NULL) {
				if (!GetObjectFromRORData(this->handleROR, v, &this->mapVisibilityInfo[i])) {
					return false;
				}
			} else {
				ZeroMemory(this->mapVisibilityInfo[i], 255);
			}
		}
		return true;
	}

	bool RORDebugger::RefreshTempMapBuildLikeInfo() {
		if (!this->handleROR) {
			ZeroMemory(&this->tempMapBuildLikeInfo, sizeof(this->tempMapBuildLikeInfo));
			return false;
		}
		if (!GetObjectFromRORData(this->handleROR, AOE_OFFSETS::ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS, &this->tempMapBuildLikeInfo)) {
			return false;
		}
		return this->tempMapBuildLikeInfo.IsCheckSumValid();
	}

	bool RORDebugger::RefreshCurrentUIObjectBase() {
		if (!this->handleROR) {
			ZeroMemory(&this->currentUIStruct, sizeof(this->currentUIStruct));
			return false;
		}
		unsigned long int p = NULL;
		if (!GetObjectFromRORData(this->handleROR, AOE_OFFSETS::ADDR_VAR_CURRENT_UI_OBJECT, &p)) {
			return false;
		}
		if (!GetObjectFromRORData(this->handleROR, p, &this->currentUIStruct)) {
			return false;
		}
		return isAValidRORChecksum(this->currentUIStruct.checksum);
	}

	bool RORDebugger::RefreshPlayers() {
		if ((!this->handleROR) || (!this->gameGlobal.IsCheckSumValid())) {
			for (int i = 0; i < 9; i++) {
				ZeroMemory(&this->players[i], sizeof(this->players[i]));
				ZeroMemory(&this->playersAI[i], sizeof(this->playersAI[i]));
			}
			return false;
		}
		if ((this->gameGlobal.playerTotalCount < 1) || (this->gameGlobal.playerTotalCount > 9)) { return false; }
		if (!GetObjectFromRORData(this->handleROR, this->gameGlobal.ptrPlayerStructPtrTable, &this->playerAddresses)) {
			return false;
		}
		// Gaia
		if (!GetObjectFromRORData(this->handleROR, this->playerAddresses[0], &this->players[0], 0x85C)) {
			return false;
		}
		for (int i = 1; i < this->gameGlobal.playerTotalCount; i++) {
			// Non-gaia players have a smaller structure (0x84C instead of 0x85C)
			if (!GetObjectFromRORData(this->handleROR, this->playerAddresses[i], &this->players[i], 0x84C)) {
				return false;
			}
			if (this->players[i].ptrAIStruct != NULL) {
				if (!GetObjectFromRORData(this->handleROR, this->players[i].ptrAIStruct, &this->playersAI[i])) {
					return false;
				} else {
					ZeroMemory(&this->playersAI[i], sizeof(this->players[i].ptrAIStruct));
				}
			}
		}
		// Unused players
		for (int j = this->gameGlobal.playerTotalCount; j < 9; j++) {
			ZeroMemory(&this->players[j], sizeof(this->players[j]));
		}
		return true;
	}

	bool RORDebugger::RefreshScenarioInfo() {
		if (!this->handleROR) {
			ZeroMemory(&this->scenarioInfo, sizeof(this->scenarioInfo));
			return false;
		}
		if (!GetObjectFromRORData(this->handleROR, this->gameGlobal.scenarioInformation, &this->scenarioInfo)) {
			return false;
		}
		return this->scenarioInfo.IsCheckSumValid();;
	}

	bool RORDebugger::RefreshUnknown() {
		if (!this->handleROR) {
			ZeroMemory(&this->unknown_0x580DA8, sizeof(this->unknown_0x580DA8));
			ZeroMemory(&this->unknown_0x6A18C0, sizeof(this->unknown_0x6A18C0));
			return false;
		}
		// unknown array
		if (!GetObjectFromRORData(this->handleROR, 0x6A18C0, &this->unknown_0x6A18C0)) {
			//return false;
		}

		// Global MP-compatible info ?
		unsigned long int p = NULL;
		if (!GetObjectFromRORData(this->handleROR, 0x580DA8, &p)) {
			return false;
		}
		if (!GetObjectFromRORData(this->handleROR, p, &this->unknown_0x580DA8)) {
			return false;
		}
		return true;
	}

	bool RORDebugger::RefreshMainGameStructs() {
		return this->RefreshGameSettings() &&
			this->RefreshGameGlobal() &&
			this->RefreshGameMapInfo() &&
			this->RefreshMapVisibilityInfo() &&
			this->RefreshTempMapBuildLikeInfo() &&
			this->RefreshCurrentUIObjectBase() &&
			this->RefreshPlayers() &&
			this->RefreshScenarioInfo() &&
			this->RefreshUnknown()
			;
	}

	// Creates a temporary copy of desired unit (from its ID).
	// Returns NULL if not found / failed.
	// You MUST free the object when you no longer need it.
	AOE_STRUCTURES::STRUCT_UNIT_BASE *RORDebugger::GetUnitFromRORData(long int unitId, unsigned long int &out_RORAddress) {
		out_RORAddress = 0;
		if (!this->handleROR) {
			return NULL;
		}
		this->RefreshMainGameStructs();
		if (!this->gameGlobal.IsCheckSumValid()) { return NULL; }
		if (unitId >= 0) {
			unsigned long int pToRead = (unsigned long int) gameGlobal.ptrUnitPointersList;
			pToRead += sizeof(DWORD) * unitId;
			if (!GetObjectFromRORData(this->handleROR, pToRead, &out_RORAddress)) {
				return NULL;
			}
			if (out_RORAddress == NULL) { return NULL; }
			// Identify the exact type of unit
			AOE_STRUCTURES::STRUCT_UNIT_BASE fakeUnitBase;
			if (!GetObjectFromRORData(this->handleROR, out_RORAddress, &fakeUnitBase)) {
				return NULL;
			}
			int unitObjSize = GetUnitStructRealSize(&fakeUnitBase);
			if (unitObjSize <= 0) { return NULL; }
			// Now get the full object (we know its exact size)
			AOE_STRUCTURES::STRUCT_UNIT_BASE *resultUnit = (AOE_STRUCTURES::STRUCT_UNIT_BASE *)malloc(unitObjSize);
			if (!GetObjectFromRORData(this->handleROR, out_RORAddress, resultUnit)) {
				free(resultUnit);
				return NULL;
			}
			return resultUnit;
		}
		// TO DO: temp units
		assert(false && "TO DO : temp units ?");
		return NULL;
	}


	// Returns a unit's name (from its definition)
	std::string RORDebugger::GetUnitName(long int unitId) {
		unsigned long int addr;
		if (!this->handleROR) {
			return "";
		}
		this->RefreshMainGameStructs();
		if (!this->gameGlobal.IsCheckSumValid()) { return NULL; }
		AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = this->GetUnitFromRORData(unitId, addr);
		if (unit == NULL) { return ""; }
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE unitDef;

		unsigned long int pToRead = (unsigned long int) unit->ptrStructDefUnit;
		if (unit) { free(unit); } // No longer needed
		if (!GetObjectFromRORData(this->handleROR, pToRead, &unitDef)) {
			return NULL;
		}
		return GetZeroTerminatedString(this->handleROR, (unsigned long int)unitDef.ptrUnitName, 16);
	}


	// Return a string with list of <unitID>(<plId>.<name>)
	std::string RORDebugger::GetUnitNamesFromArrayOfID(unsigned long int RORUnitIdArrayAddress, long int elemCount) {
		if (!this->handleROR) {
			return NULL;
		}
		std::string res = "";
		long int *unitIdList = (long int*)malloc(sizeof(DWORD32) * elemCount);
		if (AOE_STRUCTURES::GetObjectFromRORData(this->handleROR, RORUnitIdArrayAddress, unitIdList, sizeof(DWORD32) * elemCount)) {
			for (int i = 0; i < elemCount; i++) {
				res += " ";
				res += std::to_string(unitIdList[i]);
				if (unitIdList[i] >= 0) {
					unsigned long int a = NULL;
					AOE_STRUCTURES::STRUCT_UNIT_BASE *unit = GetUnitFromRORData(unitIdList[i], a);
					AOE_STRUCTURES::STRUCT_PLAYER tmpPlayer;
					res += "(";
					if (unit && GetObjectFromRORData(this->handleROR, unit->ptrStructPlayer, &tmpPlayer)) {
						res += "p";
						res += std::to_string(tmpPlayer.playerId);
						res += ".";
					}
					res += this->GetUnitName(unitIdList[i]);
					res += ")";
					free(unit);
				}
			}
		}
		free(unitIdList);
		return res;
	}


	// Creates a temporary copy of desired unit definition (from its ID/player).
	// Returns NULL if not found / failed.
	// You MUST free the object when you no longer need it.
	AOE_STRUCTURES::STRUCT_UNITDEF_BASE *RORDebugger::GetUnitDefFromRORData(long int playerId, long int unitDefId,
		unsigned long int &out_RORAddress) {
		out_RORAddress = 0;
		if (!this->handleROR) {
			return NULL;
		}
		this->RefreshMainGameStructs();
		if (!this->gameGlobal.IsCheckSumValid()) { return NULL; }
		if ((playerId < 0) || (playerId > 8) || (unitDefId < 0)) { return NULL; }
		AOE_STRUCTURES::STRUCT_PLAYER *player = &this->players[playerId];
		if (unitDefId >= player->structDefUnitArraySize) { return NULL; } // important security to avoid reading out of table

		unsigned long int pToRead = (unsigned long int) player->ptrStructDefUnitTable;
		pToRead += sizeof(DWORD) * unitDefId; // Move to relevant position in table
		if (!GetObjectFromRORData(this->handleROR, pToRead, &out_RORAddress)) {
			return NULL;
		}
		if (out_RORAddress == NULL) { return NULL; }
		
		// Identify the exact type of unit definition
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE fakeUnitDefBase;
		if (!GetObjectFromRORData(this->handleROR, out_RORAddress, &fakeUnitDefBase)) {
			return NULL;
		}
		int unitDefObjSize = GetUnitDefStructRealSize(&fakeUnitDefBase);
		if (unitDefObjSize <= 0) { return NULL; }
		// Now get the full object (we know its exact size)
		AOE_STRUCTURES::STRUCT_UNITDEF_BASE *resultUnitDef = (AOE_STRUCTURES::STRUCT_UNITDEF_BASE *)malloc(unitDefObjSize);
		if (!GetObjectFromRORData(this->handleROR, out_RORAddress, resultUnitDef)) {
			free(resultUnitDef);
			return NULL;
		}
		return resultUnitDef;
	}


	// Export to text main structures information
	std::string RORDebugger::ExportMainStructInfo() {
		std::string res = "";
		if (!this->handleROR || !this->gameSettings.IsCheckSumValid()) {
			return res;
		}
		this->RefreshMainGameStructs();
		res += "Settings = " + GetHexStringAddress(this->gameSettingsAddr);
		res += " - Global = " + GetHexStringAddress(this->gameSettings.ptrGlobalStruct);
		if (!this->gameGlobal.IsCheckSumValid()) { return res; }
		res += " - ScenarioInfo = " + GetHexStringAddress(this->gameGlobal.scenarioInformation);
		res += "\nTotalPlayers=";
		res += std::to_string(this->gameGlobal.playerTotalCount);
		res += " - GlobalCommandsInfo=";
		res += GetHexStringAddress(this->gameGlobal.commandsInfo);
		res += " - GameMapInfo = ";
		res += GetHexStringAddress(this->gameGlobal.gameMapInfo);
		res += " pTiles = ";
		res += GetHexStringAddress(this->gameMapInfo.pTileInfoRows);
		res += "\nPlayers: gaia=";
		res += GetHexStringAddress(this->playerAddresses[0]);
		for (int i = 1; i < this->gameGlobal.playerTotalCount; i++) {
			if (i == 5) { res += "\np"; } else { res += " - p"; }
			res += std::to_string(i);
			res += "= ";
			res += GetHexStringAddress(this->playerAddresses[i]);
		}
		res += "\nAI";
		for (int i = 1; i < this->gameGlobal.playerTotalCount; i++) {
			if (i == 5) { res += "\np"; } else { res += " - p"; }
			res += std::to_string(i);
			res += "= ";
			res += GetHexStringAddress(this->players[i].ptrAIStruct);
		}

		return res;
	}

}

