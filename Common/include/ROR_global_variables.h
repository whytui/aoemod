#pragma once

#include <AOE_const_functional.h>
#include <AOE_const_internal.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_map_tile_values.h>
#include <AOE_struct_ui_panel_system.h>
#include <AOE_offsets.h>
#include <UI_components\AOE_struct_any_ui.h>

using namespace AOE_OFFSETS;

namespace AOE_STRUCTURES
{
	static STRUCT_DRS_FILE_LINK **ROR_firstDrsFileLink = (STRUCT_DRS_FILE_LINK **)AOE_OFFSETS::ADDR_DRS_LINK_FIRST_ELEM;
	static STRUCT_GAME_SETTINGS **ROR_gameSettings = (STRUCT_GAME_SETTINGS**)ADDR_VAR_GAME_SETTINGS_STRUCT;
	static STRUCT_UI_PANEL_SYSTEM *ROR_pUIMainInfo = (STRUCT_UI_PANEL_SYSTEM *)ADDR_VAR_PANEL_SYSTEM;
	static STRUCT_INFLUENCE_MAP* ROR_pTempMapBuildLikeInfos = (STRUCT_INFLUENCE_MAP *)AOE_OFFSETS::ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS;
	static long int *ROR_elemCountsInUnitTempSearchResults = (long int *)ADDR_ELEMCOUNT_TEMP_NEARBY_UNITS_PER_DIPLVALUE;
	static STRUCT_NEARBY_UNIT_INFO **ROR_unitTempSearchResults = (STRUCT_NEARBY_UNIT_INFO**)ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE;
	static STRUCT_VISIBLE_UNITS_HELPER **ROR_visibleUnitsHelper = (STRUCT_VISIBLE_UNITS_HELPER **)ADDR_VAR_STRUCT_VISIBLE_UNITS_HELPER;
	static STRUCT_TREGISTRY **ROR_pGlobalRegistry = (STRUCT_TREGISTRY**)ADDR_VAR_REGISTRY_OBJECT_DEFAULT_REG_PATH; // NOT the same object as game setting's
	static STRUCT_TCHAT **ROR_pTChat = (STRUCT_TCHAT**)ADDR_VAR_TCHAT_OBJECT;
	static STRUCT_MP_COMMUNICATION **ROR_pCommunicationHandler = (STRUCT_MP_COMMUNICATION**)ADDR_MP_COMM_STRUCT;

	// Get result count (for diplomacy value) in temporary arrays. For example, after calling 0x516C10=[7D2058].collectVisibleUnitsToTempResults(posY, posX, distance, playerId, ptrArrayOfUnitAITypes, AITypesCount, diplomacyTable)
	static long int GetElemCountInUnitListTempSearchResult(AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES diplValue) {
		if ((diplValue < 0) || (diplValue > AOE_CONST_INTERNAL::CST_PDV_ENEMY)) { return 0; }
		return ROR_elemCountsInUnitTempSearchResults[diplValue]; // Reads long int from 0x7D2024+dipl*4. "visibleUnitsListSize"
	}
	// Get result element, collected in temporary arrays. For example, after calling 0x516C10=[7D2058].collectVisibleUnitsToTempResults(posY, posX, distance, playerId, ptrArrayOfUnitAITypes, AITypesCount, diplomacyTable)
	// Returns NULL if no data or invalid arguments
	static STRUCT_NEARBY_UNIT_INFO *GetUnitListTempSearchResult(AOE_CONST_INTERNAL::PLAYER_DIPLOMACY_VALUES diplValue, long int elemIndex) {
		long int maxIndex = GetElemCountInUnitListTempSearchResult(diplValue);
		if ((elemIndex < 0) || (elemIndex >= maxIndex)) { return NULL; }
		return &ROR_unitTempSearchResults[diplValue][elemIndex]; // Base array by diplomacy is in 0x7D2038+dipl*4
	}
}


