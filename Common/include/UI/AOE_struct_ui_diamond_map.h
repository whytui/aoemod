
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

	// Size=0x16C. Constructor=0x42C360(parent=RGE), 0x518010(intermediate, editor), 0x5085A0(child, in-game)
	// 0x42CD60 = diamMap.mainDraw() ? Main draw entry point ?
	// 0x42CF30 = diamMap.clearImage()
	// 0x42D0C0 = diamMap.drawAllTiles()
	// 0x42D130 = diamMap.drawExploredTiles()
	// 0x42D670 = diamMap.drawSelectedArea()
	// 0x42D710 = diamMap.drawObjects()
	// 0x42DC10 = diamMap.draw() (EDX+F4)
#define CHECKSUM_UI_RGE_DIAMOND_MAP 0x005434B4 // parent / ccor 0x42C360
#define CHECKSUM_UI_DIAM_MAP_VIEW_EDITOR 0x0054A73C // RGE_DiamondMapView. Intermediate class (editor)
#define CHECKSUM_UI_DIAM_MAP_VIEW 0x0054A3F4 // "Tribe_DiamondMapView". Child class (in-game). ccor=0x5085A0
	class STRUCT_UI_DIAMOND_MAP : public STRUCT_ANY_UI { // 3C A7 54 00 (editor). Also F4 A3 54 00(game) ?
	public:
		STRUCT_GAME_GLOBAL *global; // +F4
		STRUCT_PLAYER *controlledPlayer; // +F8
		STRUCT_GAME_MAP_INFO *gameMapInfo; // +FC
		// 0x100
		char *unknown_100[0x128 - 0x100];
		unsigned long int unknown_128; // array map coordinates per diammap "index" ?? ElemSize=6 (3 words) [+0]+[+2]=constant(6E)?
		unsigned long int unknown_12C; // pointer to diam map data? Alloc_size = mapSizeX_plus_Y_minus_1 * 0x24. +0=word(an index?)+2=byte=relevant,+4=word,+8=float,+C=dword,+10=sizeY-?-1,+12=word,+14=word,+18=float,+1C=word,+20=float
		// 0x130
		short int mapSizeY;
		short int mapSizeX; // +132
		unsigned long int unknown_134;
		short int unknown_138; // Number of "diam_map" indexes ? =elemCount for +128 ?
		short int mapSizeX_plus_Y_minus_1; // +13A. = mapSizeY+mapSizeX-1
		float unknown_pixelFactor; // +13C. = mapSizeX_plus_Y_minus_1 / unknown_138
		// 0x140
		char *unknown_140[0x168 - 0x140];
		unsigned long int unknown_168; // ptr to diamMap data ? (color to display...) ? +8=ptr to array[byte=index]=colorPerPixel

		bool IsCheckSumValid() const { return (this->checksum == CHECKSUM_UI_DIAM_MAP_VIEW_EDITOR) ||
			(this->checksum == CHECKSUM_UI_DIAM_MAP_VIEW) || 
			(this->checksum == CHECKSUM_UI_RGE_DIAMOND_MAP);
		}
	};

}

