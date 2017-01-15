
#pragma once


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{


	// Size = 0x18
	class STRUCT_MAPGEN_TERRAINS_INFO {
	public:
		long int proportion;
		long int terrainId;
		long int numberOfClumps; // Nb of clusters ("occurrences") in the map
		long int spacingToOtherTerrains;
		// 0x10
		long int placementTerrainId;
		long int unknown_14; // AGE3: unknown6
	};
	static_assert(sizeof(STRUCT_MAPGEN_TERRAINS_INFO) == 0x18, "STRUCT_MAPGEN_TERRAINS_INFO size");


	// Size 0x2C
	// Describes Base zone information for map scripts: in AGE3, Maps tab, base zone section.
	class STRUCT_MAPGEN_BASE_ZONE_INFO {
	public:
		unsigned long int unknown_00;
		long int baseTerrainId;
		long int nonBaseTerrainSizeBetweenPlayers; // Size of non-base terrain between players. Like rivers, for example.
		long int unknown_0C; // AGE: unknown4
		// 0x10
		char minDistBetweenPlayers_1; // Usually 0, 1 for mediterranean.
		char minDistBetweenPlayers_2; // Min distance in tiles between player starting zones. Always 2?
		char minDistBetweenPlayers_3; // always 0 ?
		char minDistBetweenPlayers_4; // always 0 ?
		unsigned long int unknown_14; // AGE3 unknown6
		unsigned long int unknown_18; // AGE3 unknown7
		char unknown_1C_baseZoneRadius_1; // Always 100 ?
		char unknown_1D_baseZoneRadius_2; // Usually 1 or 2
		char unknown_1E_baseZoneRadius_3; // always 0 ?
		char unknown_1F_baseZoneRadius_4; // always 0 ?
		// 0x20
		long int playerStartAreaRadius;
		long int unknown_24_playerStartAreaStuff; // Related to start area radius
		long int unknown_28; // AGE3 unknown11
	};
	static_assert(sizeof(STRUCT_MAPGEN_BASE_ZONE_INFO) == 0x2C, "STRUCT_MAPGEN_BASE_ZONE_INFO size");


	// Size = 0x30. "ObjectPlacer"
	// Initial debug topic for this object: http://aoe.heavengames.com/cgi-bin/aoecgi/display.cgi?action=ct&f=17,6326,,30
	class STRUCT_MAPGEN_OBJECT_PLACER {
	public:
		long int unit_DAT_ID;
		unsigned long int placementTerrainId; // +04. (-1)="joker"?.
		char groupMode; // +08. 0: only 1 unit is placed (objectCountPerGroup is ignored). - or 2: All units of groups are placed
		char unknown_05_groupMode; // +09. 0 or 1 ???
		unsigned short int unknown_0A_unused; // +0A. TO Confirm it is unused...
		unsigned long int objectCountPerGroup; // +0C. Number of object to place for each group
		// 0x10
		unsigned long int fluctuation; // Max random variation in the number of objects in the group (can vary in + or -).
		long int groupsCount; // Number of groups to create. Each group can contain 1 or several objects.
		long int groupRadius; // +18.
		long int ownAtStart; // 0=No, -1=Yes. Can be "Yes" only if "set for all players" is ON.
		// 0x20
		unsigned long int setPlaceForAllPlayers; // 1=done once per player. -1, -2=done only once, does not depend on players count.
		long int minDistanceToPlayers;
		long int maxDistanceToPlayers;
		unsigned long int unknown_2C; // Always 1 ? "used" or "enabled" ? 
	};
	static_assert(sizeof(STRUCT_MAPGEN_OBJECT_PLACER) == 0x30, "STRUCT_MAPGEN_OBJECT_PLACER size");


	// Size 0x48
	// Data from empires.dat about map generation (map types)
	class STRUCT_MAPGEN_MAP_TYPE {
	public:
		long int scriptNumber; // NOT MAP_TYPE_INDEX ! Example: smallIslands have index MAP_TYPE_INDEX=0 and script 4.
		long int southWestBorder; // Distance from map edge	allocated for non-base zone
		long int northWestBorder; // Distance from map edge	allocated for non-base zone
		long int southEastBorder; // Distance from map edge	allocated for non-base zone
		// 0x10
		long int northEastBorder; // Distance from map edge	allocated for non-base zone
		char borderUsagePercentage; // % of border area occupied by non-base terrain
		char unused_15[3];
		long int waterShape; // Unclear effect
		char nonBaseDefaultTerrainId;
		char unused_1D[3];
		// 0x20
		long int baseZoneCoverPercentage; // Can be > 100
		unsigned long int unknown_24; // AGE3: unknown9
		unsigned long int baseZoneInfoElemCount; // Usually 1.
		STRUCT_MAPGEN_BASE_ZONE_INFO *baseZoneInfoArray;
		// 0x30
		long int terrainsInfoCount;
		STRUCT_MAPGEN_TERRAINS_INFO *pTerrainInfoArray; // +34. ElemSize=0x18
		long int objectsToPlaceOnMapInfoCount; // +38
		STRUCT_MAPGEN_OBJECT_PLACER *objectsToPlaceOnMapInfoArray;
		// 0x40
		unsigned long int unknownOtherInfoCount;
		STRUCT_MAPGEN_TERRAINS_INFO *unknownOtherInfoArray; // +44. ElemSize=0x18. According to AGE3, same structure as terrain info. Unused in standard empires.dat.
	};
	static_assert(sizeof(STRUCT_MAPGEN_MAP_TYPE) == 0x48, "STRUCT_MAPGEN_MAP_TYPE size");

}

