
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

	// Information to display green highlight (blinking) under units, like after a right click.
	// Size=0x14. no constructor (alloc in 0x50F1F2)
	// See 0x50F970 = gamePlayZone.setGreenUnitBlinking(unitId, time_ms, arg3, arg4)
	// To allow more than 1 blinking at once, remove SUB DWORD PTR DS:[EAX+8],EBP in 0x50F9B0 (but some other clicks like move (red cross) will still stop the blinkings)
	// "View Panel"
	class STRUCT_UNIT_GREEN_BLINKING_INFO {
	public:
		long int isActive;
		long int unitId; // Warning: never set this to -1 or invalid unit ID !
		unsigned long int blinkingStartAbsoluteTime; // +08. TimGetTime value when blinking started
		unsigned long int blinkingTime; // +0C. Time amount in milliseconds the blinking should last.
		char mouseSelectionStatus; // +10. Corresponds to unit.mouseSelectionStatus. 1=selected, 2=right-click-target. Is it a Mask, not an enum ! Used to update unit After blinking stops ?
		char unused_11[3];
	};
	static_assert(sizeof(STRUCT_UNIT_GREEN_BLINKING_INFO) == 0x14, "STRUCT_UNIT_GREEN_BLINKING_INFO size");


#define CHECKSUM_UI_RGE_VIEW 0x0054A61C // ccor 0x50EFF0
#define CHECKSUM_UI_RGE_MAIN_VIEW 0x0054A840 // ccor 0x518690
#define CHECKSUM_UI_TRIBE_MAIN_VIEW 0x00546688 // ccor "inline", see 0x47D975, 0x48FE1A. Synonym for playing zone (or gameZone, etc)
#define CHECKSUM_UI_PLAYING_ZONE CHECKSUM_UI_TRIBE_MAIN_VIEW // NO specific constructor, set directly in mainGameUI.ccor and editorMainUI.ccor !

	// Size = 0x360 - Constructor 0x50EFF0 "RGE_view"
	// Only used for main game UI ? (in alpha, another child class was map view)
	class STRUCT_UI_RGE_VIEW : public STRUCT_ANY_UI {
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
		long int unknown_130_mousePosX; // +130. A relative mouse pos X.
		long int unknown_134_mousePosY; // +134. A relative mouse pos Y.
		float screenGamePosY; // +138.
		float screenGamePosX; // +13C.
		char unknown_140;
		char unknown_141[3];

		char unknown_144[0x334 - 0x144];
		// +168 : byte
		// +304, 318, 31C, 320: dwords related to mouse pos?
		// +324 = TDrawArea ? unsure

		unsigned long int unknown_334_ptr; // Ptr to obj size=0x70, ccor=516110. +5C=array size 4*((layer?)maxTypeId+1) of ptrs to obj. obj:+4=next,+8=unitId,+30/32/34/36=min/maxPosXY? +C,+20=ptrGraphicsShp?
		// Note: +4C=maxTypeId, typeIDs: 00=eyeCandy,10=birds,20=flag/deadfish/living/building/tree,30,40=?
		unsigned long int unknown_338_ptr; // Ptr to same kind of obj as +334
		unsigned long int unknown_33C;
		// 0x340
		long int unknown_340_unitId; // +340. Under mouse ?
		unsigned long int unknown_344;
		unsigned long int unknown_348;
		unsigned long int unknown_34C;
		unsigned long int *unknown_350; // +350. Something that handles displaying the red cross ? A list of those things ? (+0/+4=links?) See 0x516050. ElemSize=0x30.+8=slp +20=replaymode +24=lasttimegettime?
		STRUCT_UNIT_GREEN_BLINKING_INFO *greenUnitBlinkingInfoArray; // +354. Never NULL (alloc in ccor). Pointer to array of info for green unit blinking.
		long int numberOfBlinkingGreenUnitRectangles; // +358. Number of active "right-click" blinking unit indicators
		long int blinkingGreenUnitRectanglesArraySize; // +35C. Array size for +354 (greenUnitBlinkingInfoArray). Hardcoded to 8 in 0x50F1E8.

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_UI_PLAYING_ZONE) || 
			(this->checksum == CHECKSUM_UI_RGE_MAIN_VIEW) || 
			(this->checksum == CHECKSUM_UI_RGE_VIEW);
		}
	};
	static_assert(sizeof(STRUCT_UI_RGE_VIEW) == 0x360, "STRUCT_UI_RGE_VIEW size");


	// Size 0x380 - Constructor 0x518690. "TRIBE_Main_View" (same structure with 2 checksums)
	// Parent can be STRUCT_UI_IN_GAME_MAIN, or editor UI (Also used in editor)
	// 88 66 54 00 (inline constructor), parent = 40 A8 54 00 (ccor 0x518690)
	// 0x50F8D0 = gameZone.displayGreenBlinkingUnitReclangle()
	// 0x50F970 = gamePlayZone.setGreenUnitBlinking(unitId, time_ms, arg3, arg4)
	// +D0 = int RGE_View::do_paint(long,long,long,long,int) ??
	// +E4 = gameZone.canAlwaysBeSelected(pUnit) : 1 for dock (selectable even under fog)
	// +E8 = gameZone.getInfoAboutCommand?(pUnit, unitAIType) ? Hardcoded for towers.
	// +EC = gameZone.onMouseHoldMove(arg1, mousePosX,mousePosY...). left or right btn down + mouse is moving
	// +F0 = gameZone.OnMouseHoldMove(mousePosX, mousePosY) left or right btn down + mouse is moving
	// +F4 = gameZone.OnMouseEndDrag?(mousePosX, mousePosY) both left and right click ? ("mouseMoveAction")
	// +104 = gameZone.MouseRightDown(mousePosX, mousePosY, arg3, arg4). Excludes "drag end".
	// +110 = gameZone.createContiguousBuildings(minPosY, minPosX, maxPosY, maxPosX)
	class STRUCT_UI_PLAYING_ZONE : public STRUCT_UI_RGE_VIEW {
	public:
		unsigned long int unknown_360;
		unsigned long int unknown_364;
		unsigned long int unknown_368;
		long int unsure_gamePosYUnderMouse; // +36C. mouse pos or game pos (or temp variable ?)
		long int unsure_gamePosXUnderMouse; // +370. mouse pos or game pos (or temp variable ?)
		unsigned long int unknown_374;
		unsigned long int unknown_378;
		unsigned long int unknown_37C;

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_UI_PLAYING_ZONE) || (this->checksum == CHECKSUM_UI_RGE_MAIN_VIEW); }
	};
	static_assert(sizeof(STRUCT_UI_PLAYING_ZONE) == 0x380, "STRUCT_UI_PLAYING_ZONE size");


}

