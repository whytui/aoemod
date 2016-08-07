
#pragma once

#include <AOE_empires_dat.h>
#include <AOE_struct_unit_def.h>

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES
{


#define CHECKSUM_CIVILIZATION_DEF 0x00549A28
	// Size 0x2C
	class STRUCT_CIVILIZATION_DEF {
	public:
		unsigned long int checksum; // 28 9A 54 00
		char civilizationName[0x14];
		short int civUnitDefCount; // a civilization has its own unit definitions...
		short int unused_1A; // probably unused.
	private:
		STRUCT_DEF_UNIT **ptrUnitDefArray; // Warning: when reading empires.dat, values are temporarily 0/1 (isUnitEnabledForCiv). Array size is civUnitDefCount
	public:
		// 0x20
		short int civResourcesCount;
		short int unused_22; // probably unused.
		float *ptrResourceValues;  // List element count is civResourcesCount
		char graphicSetId;
		char alwaysOne;
		short int techTreeId;

		bool IsCheckSumValid() { return this->checksum == CHECKSUM_CIVILIZATION_DEF; }
		// Use this to safely get unit definitions
		STRUCT_DEF_UNIT *GetUnitDef(short int DAT_ID) const {
			if ((DAT_ID < 0) || (DAT_ID >= this->civUnitDefCount)) { return NULL; }
			if ((long int)(this->ptrUnitDefArray[DAT_ID]) == 1) { return NULL; }
			return this->ptrUnitDefArray[DAT_ID];
		}
		// Safely get a unit definition. Returns NULL if the unit does not exist.
		STRUCT_UNITDEF_BASE *GetUnitDefBase(short int DAT_ID) const {
			return (STRUCT_UNITDEF_BASE *)this->GetUnitDef(DAT_ID);
		}
		// Returns true if a unit (definition) exists for this civilization, false if not (also if DAT_ID is invalid)
		// The unit does not require to be "enabled", we just check its existence.
		bool DoesUnitExistForCivilization(short int DAT_ID) const {
			if ((DAT_ID < 0) || (DAT_ID >= this->civUnitDefCount)) { return false; }
			return (this->ptrUnitDefArray[DAT_ID] != NULL); // real pointer or "1" (temp value while reading empires.dat) will both return true
		}
		// Safely get an initial resource value for civilization. Returns -1 for invalid resourceIndex.
		float GetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES resourceIndex) const {
			if ((resourceIndex < 0) || (resourceIndex > AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_ALL_RELICS)) { return -1; }
			return this->ptrResourceValues[resourceIndex];
		}
		// Safely set an initial resource value for civilization
		// Returns true if sucessful.
		bool SetResourceValue(AOE_CONST_FUNC::RESOURCE_TYPES resourceIndex, float value) {
			if ((resourceIndex < 0) || (resourceIndex > AOE_CONST_FUNC::RESOURCE_TYPES::CST_RES_ORDER_ALL_RELICS)) { return false; }
			this->ptrResourceValues[resourceIndex] = value;
			return true;
		}
	};

	static_assert(sizeof(STRUCT_CIVILIZATION_DEF) == 0x2C, "STRUCT_CIVILIZATION_DEF size");

}

