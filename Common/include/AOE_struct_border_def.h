
#pragma once

#include <AOE_struct_terrain_def.h>

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


	// Size 0x5A0.
	// Describes border definition (corresponds to terrains from empires.dat).
	class STRUCT_BORDER_DEF {
	public:
		char enabled; // +00. Cf 0x446EDF.
		char random; // TO CONFIRM
		char borderName[13];
		char SLPName[13]; // +0F.
		long int SLPID; // +0x1C. ID of SLP to load in DRS file (in theory, terrain.drs).
		STRUCT_SLP_INFO *ptrToSlpObject; // +20. Corresponds to SLPID.
		long int soundIdOrPointer; // +24. Can be -1 (none). After "LoadTerrainTypes", this contains the pointer to sound object.
		char color1; // +28
		char color2_diamondMap; // +29
		char color3; // +2A
		char unknown_2B;
		unsigned long int unknown_2C;
		unsigned long int unknown_30;
		unsigned long int unknown_34;
		unsigned long int unknown_38;
		unsigned long int unknown_3C;
		short int unknown_40;
		STRUCT_FRAME_ANIMATION_SHAPE_INFO framesInfo[0xE4]; // +42. 228 elements {frames, animations, shape index}
		short int unknown_59A; // +59A
		short int underlayTerrainId; // +59C
		short int borderStyle; //+59E
	};
	static_assert(sizeof(STRUCT_BORDER_DEF) == 0x5A0, "STRUCT_BORDER_DEF size");

}

