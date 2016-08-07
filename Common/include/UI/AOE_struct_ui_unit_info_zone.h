
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <AOE_structures_drs.h>
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

		bool IsCheckSumValid() {
			return
				(this->checksum == CHECKSUM_UI_UNIT_INFO_ZONE) || (this->checksum == CHECKSUM_UI_IN_GAME_UNIT_INFO_ZONE);
		}
	};
	static_assert(sizeof(STRUCT_UI_UNIT_INFO_ZONE) == 0x214, "STRUCT_UI_UNIT_INFO_ZONE size");

}

