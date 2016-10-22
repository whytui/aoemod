
#pragma once

#include <UI\AOE_struct_ui_unit_info_zone.h>
#include <AOE_struct_unit_def.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{
	// Size 0x21C Constructor 0x4A42A0
	class STRUCT_UI_EDITOR_UNIT_INFO_ZONE : public STRUCT_UI_UNIT_INFO_ZONE {
	public:
		STRUCT_UNITDEF_BASE *currentUnitDef; // +214
		long int currentTileSet; // +218. 0-4. Useful for building icons only ?
		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_IN_GAME_UNIT_INFO_ZONE; }
	};
	static_assert(sizeof(STRUCT_UI_EDITOR_UNIT_INFO_ZONE) == 0x21C, "STRUCT_UI_EDITOR_UNIT_INFO_ZONE size");

}

