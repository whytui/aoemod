
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

#define CHECKSUM_STRATEGY_ELEMENT 0x00542A3C

	// Size=0xB0 - Constructor = ?
	// aiblditm.obj (BuildItem)
	class STRUCT_STRATEGY_ELEMENT {
	public:
		unsigned long int checksum; // 3C 2A 54 00

		// +04. Identifies the unit definition or research ID. "type" term found in 0x408891
		// For SetGatherPercentage elements, this is the resource type.
		long int unitDAT_ID;

		// +08. gID term found in 0x408891.
		// Identifies the unit itself. -1 if N/A. In standard game, it is only valued for "alive" units. 
		// RockNRor sets this while "in progress" for buildings (because "construction" unit already has its definitive ID).
		// RockNRor sets this to ACTOR's unitInstanceId (a building !) for "in-progress units". If the "parent" building is destroyed, we can cancel this strategy element (otherwise it remains stuck forever)
		// This should be -1 for elements that are not alive nor in progress.
		long int unitInstanceId;

		// +0C. Just an internal ID to identify this strategy element in player's strategy list. "uID" term found in 408891.
		// This is unique but not ordered : uses a sequence, but some elements are inserted in strategy (not always appended at end of list)
		long int elemId;

		char unitName[0x40]; // +10. Size = 0x3F + \0 maximum. Informative role only.
		float unknown_050; // posY_unused ? For early versions ?
		float unknown_054; // posX_unused ? For early versions ?
		float unknown_058; // posZ_unused ? For early versions ?
		float sizeRadius1; // +5C
		float sizeRadius2; // +60
		float sizeRadiusZ; // +64
		unsigned long int unknown68_skipCount; // Unsure. "skip" term found in 408891
		STRUCT_STRATEGY_ELEMENT *next; // +6C
		STRUCT_STRATEGY_ELEMENT *previous; // +70
		AOE_CONST_FUNC::TAIUnitClass elementType; // 4-bytes. 0=building, 1=research,2=unit,4=critical_tech. "category" (cf 0x408891)
		unsigned long int unknown_078; // +78. number (unused?)
		unsigned long int unknown_07C; // +7C. priority (unused?)

		// +80. Unit count that are currently being created for this element. Can only be 0 or 1.
		// There are RockNRor fixes to handle this correctly.
		long int inProgressCount;

		// +84. Unit count that are currently alive for this element. Can only be 0 or 1.
		// There are RockNRor fixes to handle this correctly.
		long int aliveCount;

		long int buildAttempts; // +88. Used in ROR internal calculations. "bldAtt"
		long int actor; // +8C. a unitDefId (who/what should take the action), or value (0-100) for setGatherPercentage
		long int terrainSet; // +90. terrainSet
		long int unknown_094; // terrainOne for adjacency
		long int unknown_098; // terrainTwo for adjacency
		long int placeOnElevation; // +9C.
		long int totalCount; // +A0. Total number of created units. "#blds"
		
		// +A4. Number of times the unit will be created. Then, strategy element is inactive (will never be done again). "build cap" in game terminology.
		// -1 means infinite retrains (for units). Researches use -1 but it does not really matter.
		long int retrains;
		
		long int skipCycles; // +A8. "skipCycles" term found in 0x408891
		char permanentSkip; // +AC. "permSkip" term found in 0x408891
		char unused_0AD;
		short int unused_0AE;

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_STRATEGY_ELEMENT; }
	};
	static_assert(sizeof(STRUCT_STRATEGY_ELEMENT) == 0xB0, "STRUCT_STRATEGY_ELEMENT size");


}
