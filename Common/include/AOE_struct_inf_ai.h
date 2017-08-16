
#pragma once

#include <AOE_const_internal.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_common_ai_object.h>
#include <AOE_struct_managed_array.h>
#include <AOE_struct_map_tile_values.h>


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {
	// External dependencies
	class STRUCT_AI;


	// Size = 0x24. Update_Add=0x4BD750/0x4BD7E0
	// Organized as a (big, initially 0x1F4 elements) array in infAI structure.
	// Represents (spotted) *creatable* units for me/other players + "units to defend" (artefacts, ruins, possibly mines, bushes, depending on SN numbers).
	// Empty slots are re-used when adding an entry.
	// In standard games, entries are NEVER reset !!!! which leads to some very bad behavior (for example, to evaluate if panic mode is needed)
	// See unitExtensions for optimizations/fixes.
	class STRUCT_INF_AI_DETAILED_UNIT_INFO {
	public:
		long int unitId; // -1 means free (empty) slot. Bug in 0x4C41BD, fixed by RockNRor (FixGetUnitStructInTargetSelectionLoop).
		short int unitDefId; // +04. Unit definition ID.
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitClass; // +06 - short int
		unsigned char posY; // +08. Last known Y position
		unsigned char posX; // +09. Last known X position
		unsigned char posZ; // +0A.
		char playerId; // 0x0B
		short int HP; // +0x0C. unit health points (last known)
		short int unused_0E;
		// 0x10
		long int attackAttempts; // +10. Number of time the unit has been selected as "main target".
		char unknown_14; // Default 0 ? Never seen this set to anything else ! Some unused information ?
		char unused_15;
		short int unused_16;
		float attack; // +18. Float, but computed from INTs (from unit definition)
		float reloadTime1; // +1C
		float maxRange; // +20
	};
	static_assert(sizeof(STRUCT_INF_AI_DETAILED_UNIT_INFO) == 0x24, "STRUCT_INF_AI_DETAILED_UNIT_INFO size");


	// Size = 0x08. Used in InfAI+1F8 array.
	// Elements are never reset when a building is destroyed ?
	// Exact role ? When trying to build a deposit bld, it is used to check there is not another one of same kind in same zone.
	// BUG ? status is not checked (and not up to date) when doing this !
	// Methods: 0x4C3300: infAI.addOrUpdateBldHistory(unitDefId, posY, posX, status)
	// 0x4C34C0: infAI.array1F8_setStatus(unitDefId, posY, posX, status)
	// 0x4C3550: infAI.getAddrOfUnusedElemIn1F8List(DAT_ID)
	// 0x4C3590: infAI.hasBuildingIn1F8(DAT_ID, pos, pos, allowedDistance). This is bugged in standard game because status always remains=1, even if building no longer exists! This may prevent some constructions from being started
	class STRUCT_INF_AI_BUILDS_HISTORY {
	public:
		long int unitDefId;
		AOE_CONST_INTERNAL::INFAI_BLD_HISTORY_STATUS status; // +4
		unsigned char posY; // +5
		unsigned char posX; // +6
		char unused;
	};
	static_assert(sizeof(STRUCT_INF_AI_BUILDS_HISTORY) == 0x8, "STRUCT_INF_AI_BUILDS_HISTORY size");

	// Represents information for 1 recorded attack event. This can be both me being attacked or me attacking an enemy
	// So one of attacker and target playerId is "me", but both can be (always only one, of course).
	// Element size = 0x18. see 4C3719,4C3FF2. 
	// 0x4C3660 = add entry
	class STRUCT_INF_AI_ATTACK_HISTORY {
	public:
		long int myIndex; // Index in InfaAI array ?
		char unknown_04;
		unsigned char minPosY; // +05
		unsigned char minPosX; // +06
		unsigned char maxPosY; // +07
		unsigned char maxPosX; // +08
		char attackerPlayerId; // +09. Records both my attacks and me being attacked.
		char targetPlayerId; // +0A. Records both my attacks and me being attacked.
		char unknown_0B;
		short int unknown_0C;
		char unknown_0E;
		char unknown_0F;
		long int attackGameTime;
		long int unknown_14;
	};
	static_assert(sizeof(STRUCT_INF_AI_ATTACK_HISTORY) == 0x18, "STRUCT_INF_AI_ATTACK_HISTORY size");


	// Size=0x14. Used in InfAI structure.
	class STRUCT_SPOTTED_RESOURCE_INFO {
	public:
		long int gatherableUnitId;
		char posY;
		char posX;
		char unknown_06; // default=0 ?
		char unknown_07;
		long int estimatedTotalTripWeight; // +8. round trip estimated (square) distance, including an arbitrary +10 value for detour (formula = distance*2 +10)
		char unknown_0C; // default=0 ?
		char unknown_0D; // default=0 ?
		char distanceToStorageBuilding; // +E. Actual direct distance between resource unit and its closest storage building.
		char resourceType; // +F. Always 0-3 (food wood stone gold).
		long int storageBuildingUnitID; // +10. UnitID of the closest storage building.
	};
	static_assert(sizeof(STRUCT_SPOTTED_RESOURCE_INFO) == 0x14, "STRUCT_SPOTTED_RESOURCE_INFO size");


#define CHECKSUM_INF_AI 0x00548B74

	// size 0x10090 - constructor=0x4BA1C0. Offset is AI+0xCEC
	// Checksums : 74 8B 54 00, parents=04 2C 54 00, 74 2B 54 00
	// 0x4C1AC0 = infAI.searchTradeTarget?(actorUnitId)
	// +0x00 = infAI.destructor(do_free)
	// +0x30 = infAI.update(arg1)
	class STRUCT_INF_AI {
	public:
		unsigned long int checksum;
		STRUCT_COMMON_AI_OBJECT commonAIObject; // size 0xEC - id=1005 for infAI.
		// 0xF0
		STRUCT_MANAGED_ARRAY unknown_0F0; // Unused ?? See 0x4DB4E2.
		// 0x100
		STRUCT_AI *ptrMainAI;
		long int YMapSize; // +104. Warning: this might NOT be initialized (-1).
		long int XMapSize; // +108. Warning: this might NOT be initialized (-1).
		unsigned long int unknown_10C;

		// Unit lists (from detailed... to elementsToDefend) : see 0x4BDD10. In those 4 unit lists, elements are never cleaned/removed !

		// +110. Total allocated size of detailedSpottedUnitInfoList (the array can contain -many- empty slots with "id=-1")
		long int detailedSpottedUnitInfoListSize;

		// +114. Contains detailed info about (spotted) units that have a military importance: typically creatable+artefacts + some others
		// Contains all playerCreatable units => 0x4BE140=infAI.isPlayerCreatable(unitClass) : based on a (long) list of AITypes (which is good)
		// Contains also "units to defend" cf 0x4C1730=infAI.AddUnitToDefend(unitStruct) which adds relic=6F, ruin=158,163, mines+bushes(using AIType)
		// => Warning: if SNxxxDefendPriority<=0, the unit is NOT added here (same as elementsToDefend list)
		// AddUpdate=0x4BD750 (param=pUnit). Units are never removed in standard game ?
		STRUCT_INF_AI_DETAILED_UNIT_INFO *detailedSpottedUnitInfoList;
		
		// +118. units that can be created by players + resources (gazelle but not other animals - in standard game) + artefacts.
		// Cf 0x4BE100=infAI.isArtefactOrTargetableResourceOrCreatable(unitClass)
		STRUCT_MANAGED_ARRAY artefactsCreatableGatherableUnits;

		// +128. All player-creatable units (villager, military, buildings...). NOT resources.
		// Cf 0x4BE140=infAI.isPlayerCreatable(unitClass) : based on a (long) list of AITypes (which is good)
		STRUCT_MANAGED_ARRAY playerCreatableUnits;

		// +138. Store the found flags/artefacts.
		// Does not store player-creatable nor gatherable resources.
		// cf 0x4BE200=isArtefactOrFlag(unitClass)
		STRUCT_MANAGED_ARRAY artefactsAndFlags;

		// +148. Stores (hardcoded IDs) TownCenter=6D, dock=2D, relic=6F, ruin=158,163, mines+bushes(using AIType)
		// Cf 0x4C1730=infAI.AddUnitToDefend(unitStruct)
		// Related to the corresponding SNxxxDefendPriority values (if 0 : not added in list)
		STRUCT_MANAGED_ARRAY elementsToDefend;

		STRUCT_INFLUENCE_MAP mapExplorationInfo; // +158. Values = INFAI_TILE_EXPLORATION_STATUS
		// Remove unit from allMyUnits & buildingUnits: 0x4BF440.
		STRUCT_MANAGED_ARRAY allMyUnits; // +180. To confirm.
		STRUCT_MANAGED_ARRAY buildingUnits; // +190
		// 0x1A0
		char unknown_1A0[0x1F0 - 0x1A0];
		// 0x1F0
		unsigned long int unknown_1F0;
		long int buildHistoryArraySize; // +1F4. Allocated element count of +1F8 list. (default=0xC8=200)
		STRUCT_INF_AI_BUILDS_HISTORY *buildHistoryArray; // +1F8.
		long int attacksHistoryAllocatedSize; // +1FC. Element count in attacksHistory array. Init=25
		// 0x200
		STRUCT_INF_AI_ATTACK_HISTORY *attacksHistory; // +200. Element size = 0x18
		long int unknown_204;
		char unknown_208[0x100]; // +208. Unused, not even initialized ?
		char terrainZoneFullyExplored[0xFF]; // +308. Index=terrainZoneId. Get=4BC690 Set=4BC6E0. Zone fully explored? Not sure this works well as terrainZone IDs depend on terrain restrictions ! (not the same for all units)
		char unused_407; // +407.
		// +408: included array of 4*4 elems size=0xC (3 dwords?). Total size in bytes=0xC0. Index corresponds to each quarter of the map ?? Represents number of explored tiles by map zone ?
		long int unknown_408[4 * 4 * 3];
		unsigned long int *unknown_4C8; // +4C8. Pointer to struct size=0x10. Array of 4 dwords.
		unsigned long int *unknown_4CC; // +4CC. Pointer to struct size=0x10. Array of 4 dwords.
		// 0x4D0 : spotted resource lists : insert in 0x4C49C0=infAI.addGatherableInUnitList(unit). Also update bestXxxUnitId/distance
		STRUCT_SPOTTED_RESOURCE_INFO *spottedGatherableUnitsByResourceTypeArrays[4]; // +4D0. Stores explored resources, their position and (closest) storage building. Eg 0x4BC7B0. Add=0x4C49C0.
		long int spottedGatherableUnitsCountByResourceType[4]; // +4E0. Number of elements in +4D0+i array ? (i=resource type). Related to +540, +550
		long int spottedGatherableUnitsByResourceTypeArraySizes[4]; // +4F0. 4 Array sizes (allocated sizes ?)
		// 0x500
		unsigned long int unknown_500[4];
		unsigned long int unknown_510[4];
		unsigned long int unknown_520[4];
		unsigned long int unknown_530[4];
		long int bestForageDropSiteDistance; // +540. Best distance from forage/farm to storage (granary or TC)
		long int bestWoodDropSiteDistance; // +544. Best distance from tree to storage (SP or TC)
		long int bestStoneDropSiteDistance; // +548. Best distance from stone mine to storage (SP or TC)
		long int bestGoldDropSiteDistance; // +54C. Best distance from gold mine to storage (SP or TC)
		long int bestForageUnitId; // +550. closest target unitId for food (farms & forage only, not hunting)
		long int bestWoodUnitId; // +554. closest target unitId for wood
		long int bestStoneUnitId; // +558. closest target unitId for stone
		long int bestGoldUnitId; // +55C. closest target unitId for gold
		long int foundForestTiles; // +560. Used at game start to know when storage pit construction can be triggered ?
		char unknown_564[0xFF64 - 0x564];
		long int humanPlayer_militaryUnitCountByType[AOE_CONST_INTERNAL::INTERNAL_MILITARY_UNIT_TYPES::CST_IMUT_COUNT]; // 0xFF64 - used only if TrackHumanHistory is ON
		char learnUHFileName[0x100]; // +0xFF90. Name of xxx.uh file. Not sure this is really used.
		// 0x10090: end

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_INF_AI; }

		// Get AI's exploration status of a specific tile
		AOE_CONST_INTERNAL::INFAI_TILE_EXPLORATION_STATUS GetExplorationStatus(long int posX, long int posY) const {
			return (AOE_CONST_INTERNAL::INFAI_TILE_EXPLORATION_STATUS)this->mapExplorationInfo.GetTileValue(posX, posX, 0xFF);
		}
	};
	static_assert(sizeof(STRUCT_INF_AI) == 0x10090, "STRUCT_INF_AI size");


}
