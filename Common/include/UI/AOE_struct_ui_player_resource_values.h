
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <AOE_struct_player.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size=0x118. Constructor=0x4F7C00 = uiPlayerResValues.constructor(arg1, gameMainUi, font?, controlledPlayer)
	// This is a component (parent constructor 0x452580), not a screen.
#define CHECKSUM_UI_PLAYER_RESOURCE_VALUES 0x00549D98
	class STRUCT_UI_PLAYER_RESOURCE_VALUES : public STRUCT_ANY_UI {
	public:
		unsigned long int unknown_0F4; // +F4.
		STRUCT_PLAYER *controlledPlayer; // +F8
		short int resourcesCount; // +FC
		short int unknown_0FE; // Maybe unused ?
		short int *currentResourceValuesArray; // +100
		long int lastRefreshTimeGetTime; // +104
		long int unknown_108; // +108. Default=0x1F4
		unsigned long int unknown_font; // +10C
		long int unknown_110; // +110. Default = -1
		unsigned long int unknown_114;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_UI_PLAYER_RESOURCE_VALUES; }
	};
	static_assert(sizeof(STRUCT_UI_PLAYER_RESOURCE_VALUES) == 0x118, "STRUCT_UI_PLAYER_RESOURCE_VALUES size");

}

