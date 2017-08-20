
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
	
#define CHECKSUM_UI_UNIT_INFO_ZONE_PARENT 0x00549E7C // parent, used in-game(?), ccor=0x4F83D0
#define CHECKSUM_UI_EDITOR_UNIT_INFO_ZONE 0x00547CC0 // ccor=0x4A42A0. "Scenario editor panel object"
	// "TribePanelObject". Size=0x214. Constructor=0x4F83D0
	// Methods: 0x4F9970=unitInfoZone.refreshIfNeeded(), 0x4F8740=UnitInfoZone.showInfos(self?, mainGameZone?)
	class STRUCT_UI_UNIT_INFO_ZONE : public STRUCT_ANY_UI { // actually inherits from panelBase
	public:
		unsigned long int unknown_0F4; // +F4 : font info. (=fontObj+0)
		unsigned long int unknown_0F8; // +F8 : font info. (=fontObj+4)
		unsigned long int unknown_0FC; // +FC : font info. (=fontObj+8)
		STRUCT_SLP_INFO *healthSlp; // +100. Health.shp, slpId=0xC639
		STRUCT_SLP_INFO *itemIconSlp; // +104. itemicon.shp, slpid=0xC62B
		STRUCT_SLP_INFO *iconsForTrainUnits; // +108. Check type (STRUCT_SLP_INFO ok, but is pointer ok ?)
		STRUCT_SLP_INFO **iconsForBuildings; // +10C. Pointer to array of building icons for each tileset. ArraySize=5 (5 tilesets). Points to sc. editor OR game main UI array.
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
		AOE_CONST_INTERNAL::GAME_UNIT_STATUS unitStatus; // +134.
		char unknown_135[3]; // unused?
		float unknown_138; // +138. trade goods (for dock) ??
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
		UNIT_ACTION_ID unknown_160; // +160. Specifies if unit is currently training a unit, researching a tech
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
		long int currentPopulation; // +1D4. To confirm
		long int currentResource5Value; // +1D8. 
		char currentUnitShortcutNumber; // +1DC
		char unknown_1DD[3];
		STRUCT_PLAYER *currentUnitPlayer; // +1E0.
		unsigned long int unknown_1E4;
		char currentUnitHasAnAction; // +1E8
		char unknown_1E9[3];
		STRUCT_SLP_INFO *unknown_1EC_SLP[9]; // +1EC.
		long int unknown_210; // +210

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_UI_UNIT_INFO_ZONE_PARENT) || (this->checksum == CHECKSUM_UI_EDITOR_UNIT_INFO_ZONE); }
	};
	static_assert(sizeof(STRUCT_UI_UNIT_INFO_ZONE) == 0x214, "STRUCT_UI_UNIT_INFO_ZONE size");


}

