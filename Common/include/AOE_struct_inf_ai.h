
#pragma once

#include <AOE_const_internal.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_common_ai_object.h>
#include <AOE_struct_ai_unit_list_info.h>
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
	// Organized as a (big, initially 0x1F4 elements) array in infAI structure
	// Empty slots are re-used when adding an entry.
	// In standard games, entries are NEVER reset !!!! which leads to some very bad behavior (for example, to evaluate if panic mode is needed)
	class STRUCT_INF_AI_UNIT_LIST_ELEM {
	public:
		long int unitId; // -1 means free (empty) slot. Bug in 4C41BD ?
		short int unitDATID; // +04
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitClass; // +06 - short int
		unsigned char posY; // +08
		unsigned char posX; // +09
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
		long int YMapSize; // +104. Warning: this might NOT be initialized
		long int XMapSize; // +108. Warning: this might NOT be initialized
		unsigned long int unknown_10C;
		// 0x110
		long int unitElemListSize; // +110. Total allocated size of unitElemList (the array can contain some "-1")
		// Unit lists : see 0x4BDD10. In those 4 unit lists, elements are never cleaned/removed !
		STRUCT_INF_AI_UNIT_LIST_ELEM *unitElemList; // +114. Contains info about units that can be created by players + resources + artefacts
		STRUCT_AI_UNIT_LIST_INFO creatableAndGatherableUnits; // +118. units that can be created by players + resources (gazelle but not other animals) + artefacts
		STRUCT_AI_UNIT_LIST_INFO playerCreatableUnits; // +128. All player-creatable units (villager, military, buildings...). NOT resources.
		STRUCT_AI_UNIT_LIST_INFO artefactsAndFlags; // +138. Store the found flags/artefacts.
		STRUCT_AI_UNIT_LIST_INFO elementsToDefend; // +148. Can be TC, dock, relic, ruin, mines+bushes. Related to SNxxxDefendPriority
		STRUCT_MAP_TILE_VALUES mapExplorationInfo; // +158. Values = INFAI_TILE_EXPLORATION_STATUS
		// Remove unit from allMyUnits & buildingUnits: 0x4BF440.
		STRUCT_AI_UNIT_LIST_INFO allMyUnits; // +180. To confirm.
		STRUCT_AI_UNIT_LIST_INFO buildingUnits; // +190
		// 0x1A0
		char unknown_1A0[0x1F0 - 0x1A0];
		// 0x1F0
		unsigned long int unknown_1F0;
		long int buildHistoryArraySize; // +1F4. Allocated element count of +1F8 list. (default=0xC8=200)
		STRUCT_INF_AI_BUILDS_HISTORY *buildHistoryArray; // +1F8.
		long int unknown_1FC; // Element count in unknown_200_ptrSomeArray array ? Init=25
		// 0x200
		// Element size = 0x18. see 4C3719,4C3FF2. +0=dword=index(-1=bad?), +4=? +(5,6)=byte(minposYX?),(7,8)=byte(maxposYX?)
		// +9=playerIdAttacker(notme) +A=playerId_me? +C=word +E=byte +10=ptr? +14=dword,init=-1
		// Related to being attacked ? add in 4C3660.
		unsigned long int *unknown_200_ptrSomeArray;
		long int unknown_204;
		char unknown_208[0x308 - 0x208]; // +208. Unused, not even initialized ?
		char terrainZoneFullyExplored[0xFF]; // +308. Index=terrainZoneId. Get=4BC690 Set=4BC6E0. Zone fully explored? Not sure this works well as terrainZone IDs depend on terrain restrictions ! (not the same for all units)
		char unused_407; // +407.
		// +408: included array of 4*4 elems size=0xC (3 dwords?). Total size in bytes=0xC0
		unsigned long int unknown_408[4 * 4 * 3];
		unsigned long int *unknown_4C8; // +4C8. Pointer to struct size=0x10. Array of 4 dwords.
		unsigned long int *unknown_4CC; // +4CC. Pointer to struct size=0x10. Array of 4 dwords.
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

		bool IsCheckSumValid() const { return this->checksum == 0x00548B74; }

		AOE_CONST_INTERNAL::INFAI_TILE_EXPLORATION_STATUS GetExplorationStatus(long int posX, long int posY) const {
			return (AOE_CONST_INTERNAL::INFAI_TILE_EXPLORATION_STATUS)this->mapExplorationInfo.GetTileValue(posX, posX, 0xFF);
		}
	};
	static_assert(sizeof(STRUCT_INF_AI) == 0x10090, "STRUCT_INF_AI size");


}
