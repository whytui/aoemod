
#pragma once

#include <AOE_const_functional.h>


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// Size=0xB0 - Constructor = ?
	// aiblditm.obj (BuildItem)
	class STRUCT_STRATEGY_ELEMENT {
	public:
		unsigned long int checksum; // 3C 2A 54 00
		long int unitDAT_ID; // Identifies the unit definition or research. "type" term found in 408891
		long int unitInstanceId; // gID term found in 408891.
		long int elemId; // Just an internal ID to identify this strategy element in player's strategy list. "uID" term found in 408891.
		// 0x10
		char unitName[0x40]; // 0x3F + \0 maximum
		// 0x50
		float unknown_050; // posY_unused ? For early versions ?
		float unknown_054; // posX_unused ? For early versions ?
		float unknown_058; // posZ_unused ? For early versions ?
		float sizeRadius1;
		// 0x60
		float sizeRadius2;
		float unknown_064;
		unsigned long int unknown68_skipCount; // Unsure. "skip" term found in 408891
		STRUCT_STRATEGY_ELEMENT *next;
		// 0x70
		STRUCT_STRATEGY_ELEMENT *previous;
		AOE_CONST_FUNC::TAIUnitClass elementType; // 4-bytes. 0=building, 1=research,2=unit,4=critical_tech. "cat" (category) term found in 408891
		unsigned long int unknown_078; // +78. number ?
		unsigned long int unknown_07C; // +7C. priority ?
		// 0x80
		long int inProgressCount; // Unit count that are currently being created for this element. Can only be 0 or 1.
		long int aliveCount; // +84. Unit count that are currently alive for this element. Can only be 0 or 1.
		long int buildAttempts; // +88. Used in ROR internal calculations. "bldAtt"
		long int actor; // +8C. a unitId, or value for setGatherPercentage
		// Identifies the unit itself. -1 if N/A. In standard game, it is only valued for "alive" units. 
		// With RockNRor, we also value it for in-progress buildings => in-construction building already has its ID, use it
		// With RockNRor, we also value it for in-progress living units => store actor's unitID. This way, if building is destroyed, we can cancel its strategy element (otherwise it remains stuck forever)
		// 0x90
		long int unknown_090; // terrainSet[3] ?
		long int unknown_094;
		long int unknown_098;
		long int unknown_09C; // +9C. placeOnElevation
		// 0xA0
		long int totalCount; // Total number of created units. "#blds"
		long int retrains; // Number of times the unit will be created. Then, strategy element is inactive (will never be done again). "cap" in game terminology ?
		unsigned long int unknown_0A8_skipCy; // "skipCy" term found in 408891
		char canSkip; // +AC. "permSkip" term found in 408891
		char unused_0AD;
		short int unused_0AE;

		bool IsCheckSumValid() { return this->checksum == 0x00542A3C; }
	};
	static_assert(sizeof(STRUCT_STRATEGY_ELEMENT) == 0xB0, "STRUCT_STRATEGY_ELEMENT size");


}
