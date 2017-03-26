
#pragma once

#include <AOE_struct_dat_sound.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

	// Size = 0x10
	class STRUCT_GRAPHIC_DELTA {
	public:
		short int graphicId;
		short int unknown_02;
		short int unknown_04;
		short int unknown_06;
		short int directionY; // +08
		short int directionX; // +0A
		short int displayAngle; // +0C
		short int unknown_0E;
	};
	static_assert(sizeof(STRUCT_GRAPHIC_DELTA) == 0x10, "STRUCT_GRAPHIC_DELTA size");


	// Size = 0x14. Alloc 4A4DD7
	class STRUCT_GRAPHIC_ATTACK_SOUND_INFO {
	public:
		STRUCT_DAT_SOUND *pSound[3];
		short int soundDelay[3]; // +0C.
		short int unused_12;
	};
	static_assert(sizeof(STRUCT_GRAPHIC_ATTACK_SOUND_INFO) == 0x14, "STRUCT_GRAPHIC_ATTACK_SOUND_INFO size");


	// Size = 0x78. Constructor = 0x4A4C40 = graphic.readFromFile(internalFileId, pSoundStruct, pMPInfo?)
	// "RGE_Sprite"
	class STRUCT_GRAPHICS {
	public:
		char slpName[0x0D];
		char unused_0D[3];
		long int slpId; // +10
		char unknown_14; // +14.?
		char unknown_15;
		short int unknown_16; // type?
		unsigned long int *unknown_18; // Backpointer Unit???
		unsigned long int unknown_1C;
		unsigned long int *unknown_20;
		unsigned long int *unknown_24;
		char unknown_28; // age3 "unknown2". Priority ?
		char layer; // +29
		short int playerColor; // +2A. Read as a WORD, but actually 2 (byte) values ?
		char transparentPickMode; // +2C. 0=can't select. 1=select on pixels. 2=select on box.
		char unknown_2D;
		short int coordinates[4]; // +2E/30/32/34.
		short int deltaCount; // +36
		STRUCT_GRAPHIC_DELTA *deltasArray; // +38.
		STRUCT_DAT_SOUND *sound; // +3C. Pointer to (one) sound object (cf soundId in empires.dat)
		char attackSoundUsed; // +40
		char unknown_41;
		short int unknown_42; // type?
		STRUCT_GRAPHIC_ATTACK_SOUND_INFO *attackSoundsArray; // +44. Number of elements is angleCount ! It is only set if attackSoundUsed is true (NULL otherwise)
		char graphicName[0x15]; // +48. Long name
		char unused_5D;
		short int framesPerAngle; // +5E.
		short int angleCount; // +60. Defines number of elements in attackSoundsArray (if attackSoundUsed=1). angleCount must be >=8 to allow valid movement. Otherwise, possible unit movement orientations will be limited !
		short int unknown_62;
		float speedMultiplier; // +64. This is used to make unit walk faster when descending altitude, etc
		float frameDuration; // +68
		float replayDelay; // +6C
		char sequenceType; // +70
		char unknown_71;
		short int graphicId; // +72. Graphic ID, the same as AGE3's graphics tab. This is NOT SlpId.
		char mirroringMode; // +74
		char unused_75[3]; // +75

		// +48 name 0x15
	};
	static_assert(sizeof(STRUCT_GRAPHICS) == 0x78, "STRUCT_GRAPHICS size");

}

