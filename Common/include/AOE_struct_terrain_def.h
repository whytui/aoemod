
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

	// Size 0x198
	// Describes terrain definition (corresponds to terrains from empires.dat)
	class STRUCT_TERRAIN_DEF {
	public:
		char enabled; // WRONG ? random ?
		char random; // TO CONFIRM
		char terrainName[13];
		char SLPName[13];
		long int SLP; // +0x1C
		// 0x20
		unsigned long int unknown_020;
		unsigned long int unknown_024;
		char color1;
		char color2_diamondMap; // +29 ?
		char color3;
		char cliffColor1;
		char cliffColor2; // +2C
		char passableTerrainId;
		char impassableTerrainId;
		char isAnimated; // unsure
		// 0x30
		short int animationFramesCount; // unsure
		short int pauseFramesCount; // unsure
		float interval; // unsure
		float pauseBetweenLoops; // unsure
		short int frame; // unsure
		short int drawFrame; // unsure
		// 0x40
		float animateLast; // unsure
		char frameChanged; // unsure
		char drawn; // unsure
		// 0x46
		short int elevationGraphics[19 * 3]; // 0x39*2 bytes (19 slots of 3 words). 0=frame, 1=Animation, 2=shape_index ?
		// 0xB8
		short int terrainIdToDraw; // Can be -1
		short int terrainDimension1;
		short int terrainDimension2;
		short int borders[0x20]; // 0xBE, 0x40 bytes
		// 0xFE
		short int terrainUnitId[30]; // 30=0x1E, total size=0x3C
		short int terrainUnitDensity[30]; // +0x13A
		char terrainUnitPriority[30]; // +0x176
		// +194
		short int usedTerrainUnitCount;
		short int unused_196;
	};
	static_assert(sizeof(STRUCT_TERRAIN_DEF) == 0x198, "STRUCT_TERRAIN_DEF size");
}

