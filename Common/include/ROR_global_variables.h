#pragma once

#include <AOE_const_functional.h>
#include <AOE_const_internal.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_temp_map_build_like_infos.h>
#include <AOE_offsets.h>
#include <UI_components\AOE_struct_any_ui.h>

using namespace AOE_OFFSETS;

namespace AOE_STRUCTURES
{
	static STRUCT_DRS_FILE_LINK **ROR_firstDrsFileLink = (STRUCT_DRS_FILE_LINK **)AOE_OFFSETS::ADDR_DRS_LINK_FIRST_ELEM;
	static STRUCT_GAME_SETTINGS **ROR_gameSettings = (STRUCT_GAME_SETTINGS**)ADDR_VAR_GAME_SETTINGS_STRUCT;
	static STRUCT_TEMP_MAP_BUILD_LIKE_INFOS* ROR_pTempMapBuildLikeInfos = (STRUCT_TEMP_MAP_BUILD_LIKE_INFOS *)AOE_OFFSETS::ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS;
	static STRUCT_ANY_UI *ROR_pCurrentUIObject = (STRUCT_ANY_UI *)AOE_OFFSETS::ADDR_VAR_CURRENT_UI_OBJECT;
	static STRUCT_UNKNOWN_MAP_INFO_7D2058 *ROR_unknown_mapInfo_7D2058 = (STRUCT_UNKNOWN_MAP_INFO_7D2058 *)ADDR_STRUCT_UNKNOWN_MAP_INFO_7D2058;
}


