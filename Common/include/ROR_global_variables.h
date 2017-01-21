#pragma once

#include <AOE_const_functional.h>
#include <AOE_const_internal.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_map_tile_values.h>
#include <AOE_struct_ui_main_info.h>
#include <AOE_offsets.h>
#include <UI_components\AOE_struct_any_ui.h>

using namespace AOE_OFFSETS;

namespace AOE_STRUCTURES
{
	static STRUCT_DRS_FILE_LINK **ROR_firstDrsFileLink = (STRUCT_DRS_FILE_LINK **)AOE_OFFSETS::ADDR_DRS_LINK_FIRST_ELEM;
	static STRUCT_GAME_SETTINGS **ROR_gameSettings = (STRUCT_GAME_SETTINGS**)ADDR_VAR_GAME_SETTINGS_STRUCT;
	static STRUCT_UI_PANEL_SYSTEM *ROR_pUIMainInfo = (STRUCT_UI_PANEL_SYSTEM *)ADDR_VAR_PANEL_SYSTEM;
	static STRUCT_MAP_TILE_VALUES* ROR_pTempMapBuildLikeInfos = (STRUCT_MAP_TILE_VALUES *)AOE_OFFSETS::ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS;
	static STRUCT_ANY_UI *ROR_pCurrentUIObject = (STRUCT_ANY_UI *)AOE_OFFSETS::ADDR_VAR_CURRENT_UI_OBJECT;
	static STRUCT_UNKNOWN_MAP_INFO_7D2058 **ROR_unknown_mapInfo_7D2058 = (STRUCT_UNKNOWN_MAP_INFO_7D2058 **)ADDR_STRUCT_UNKNOWN_MAP_INFO_7D2058;
	static STRUCT_TREGISTRY **ROR_pGlobalRegistry = (STRUCT_TREGISTRY**)ADDR_VAR_REGISTRY_OBJECT_DEFAULT_REG_PATH; // NOT the same object as game setting's
	static STRUCT_TCHAT **ROR_pTChat = (STRUCT_TCHAT**)ADDR_VAR_TCHAT_OBJECT;
	static STRUCT_MP_COMMUNICATION **ROR_pCommunicationHandler = (STRUCT_MP_COMMUNICATION**)ADDR_MP_COMM_STRUCT;
}


