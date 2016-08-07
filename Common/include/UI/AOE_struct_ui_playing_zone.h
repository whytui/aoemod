
#pragma once

#include <UI_components\AOE_struct_any_ui.h>
#include <AOE_struct_game_global.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{

#define CHECKSUM_UI_PLAYING_ZONE 0x00546688
	// Size 0x380
	// Constructor 0x518690
	// Parent can be STRUCT_UI_IN_GAME_MAIN, or editor UI?
	// Also used in editor !
	// 88 66 54 00, parent = 40 A8 54 00, 1C A6 54 00, then UI base
	class STRUCT_UI_PLAYING_ZONE : public STRUCT_ANY_UI {
	public:
		long int hWnd; // TO CONFIRM. +F4
		unsigned long unknown_0F8;
		STRUCT_GAME_GLOBAL *globalStruct; // +FC
		// 0x100
		STRUCT_PLAYER *controlledPlayer;
		STRUCT_GAME_MAP_INFO *globalGameMapInfo; // +104. To confirm
		short int unknown_108; // some value for Y
		short int unknown_10A;
		short int unknown_10C;
		short int unknown_10E; // some value for X
		unsigned long int unknown_110;
		short int unknown_114;
		short int unknown_116;
		short int unknown_118;
		short int unknown_11A;
		short int unknown_11C; //?
		short int unknown_11E; //?
		short int unknown_120;
		short int unknown_122_screenGamePosY_word; // +122. From player.screenPosY
		short int unknown_124_screenGamePosX_word; // +124. From player.screenPosX
		short int unknown_126;
		short int unknown_128;
		short int unknown_12A;
		short int unknown_12C;
		short int unknown_12E;
		char unknown_130[0x138 - 0x130];
		float screenGamePosY; // +138.
		float screenGamePosX; // +13C.
		char unknown_140;
		char unknown_141[3];

		char unknown_144[0x334 - 0x144];
		// +168 : byte
		// +304, 318, 31C, 320: dwords related to mouse pos?

		unsigned long int unknown_334_ptr; // Ptr to obj size=0x70, ccor=516110. +5C=array size 4*(maxTypeId+1) of ptrs to obj. obj:+4=next,+8=unitId,+30/32/34/36=min/maxPosXY? +C,+20=ptrGraphicsShp?
		// Note: +4C=maxTypeId, typeIDs: 00=eyeCandy,10=birds,20=flag/deadfish/living/building/tree,30,40=?
		unsigned long int unknown_338_ptr; // Ptr to same kind of obj as +334
		unsigned long int unknown_33C;
		// 0x340
		long int unknown_340_unitId; // Target ? Selected ? Under mouse ?
		unsigned long int unknown_344;
		unsigned long int unknown_348;
		unsigned long int unknown_34C;
		char unknown_350[0x36C - 0x350];
		long int unsure_gamePosYUnderMouse; // +36C. mouse pos or game pos (or temp variable ?)
		long int unsure_gamePosXUnderMouse; // +370. mouse pos or game pos (or temp variable ?)

		bool IsCheckSumValid() { return (this->checksum == CHECKSUM_UI_PLAYING_ZONE) || (this->checksum == 0x0054A840); }
	};


}

