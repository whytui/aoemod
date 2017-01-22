
#pragma once

#include <AOE_empires_dat.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{
	// External dependency
	class STRUCT_SLP_INFO;


#pragma pack(push, 1) // Prevent compiler from aligning on dwords (or other alignment). Very important here as there are some weird structures.

	// Size=0x06.
	class STRUCT_FRAME_ANIMATION_SHAPE_INFO {
	public:
		short int frameCount;
		short int animationCount;
		short int shapeIndex;
	};
	static_assert(sizeof(STRUCT_FRAME_ANIMATION_SHAPE_INFO) == 0x06, "STRUCT_FRAME_ANIMATION_SHAPE_INFO size");


	// Size 0x198
	// Describes terrain definition (corresponds to terrains from empires.dat)
	class STRUCT_TERRAIN_DEF {
	public:
		char enabled; // +00. Cf 0x446B0F.
		char random; // TO CONFIRM
		char terrainName[13];
		char SLPName[13]; // +0F.
		long int SLPID; // +0x1C. ID of SLP to load in DRS file (in theory, terrain.drs).
		STRUCT_SLP_INFO *ptrToSlpObject; // +20. Corresponds to SLPID.
		long int soundIdOrPointer; // +24. Can be -1 (none). After "LoadTerrainTypes", this contains the pointer to sound object.
		char color1; // +28
		char color2_diamondMap; // +29 ?
		char color3;
		char cliffColor1;
		char cliffColor2; // +2C
		char passableTerrainId;
		char impassableTerrainId;
		char isAnimated; // unsure
		short int animationFramesCount; // +30. unsure
		short int pauseFramesCount; // unsure
		float interval; // unsure
		float pauseBetweenLoops; // unsure
		short int frame; // unsure
		short int drawFrame; // unsure
		float animateLast; // +40. unsure
		char frameChanged; // +44. unsure
		char drawn; // +45. unsure
		STRUCT_FRAME_ANIMATION_SHAPE_INFO elevationGraphics[ELEVATION_GRAPHICS_TYPE::EGT_COUNT]; // +46. (19 slots of 3 words).
		// 0xB8
		short int terrainIdToDraw; // Can be -1
		short int terrainDimension1;
		short int terrainDimension2;
		short int borders[0x20]; // 0xBE, 0x40 bytes
		// 0xFE
		short int terrainUnitId[30]; // +0xFE. Count=30=0x1E, total size=0x3C
		short int terrainUnitDensity[30]; // +0x13A
		char terrainUnitPriority[30]; // +0x176
		// +194
		short int usedTerrainUnitCount;
		short int unused_196;
	};
	static_assert(sizeof(STRUCT_TERRAIN_DEF) == 0x198, "STRUCT_TERRAIN_DEF size");

#pragma pack(pop)

}

