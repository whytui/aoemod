
#pragma once

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// Used in AI structures to keep lists of unit IDs. Size=0x10
	// All methods suppose that usedElemCount==arraySize (no empty slot)
	// 0x4C6880 = unitElemList.containsUnitId(pUnitId)
	// 0x4E3AF0 = unitElemList.Remove(unitId) 
	// 0x414FC0 = unitListInfo.reallocArray(newSize). Not supposed to be used to remove elements.
	// 0x4C68C0 = unitElemList.AddUnitId(unitId)
	class STRUCT_AI_UNIT_LIST_INFO {
	public:
		long int *unitIdArray; // Pointer to array of unitIDs. NULL when size==0
		long int usedElements; // +4. Number of elements in array that are actually used
		long int maxElementCount; // +8. Max or desired value ?
		long int arraySize; // +C. Number of allocated elements in array

		bool ContainsUnitId(long int unitId) {
			const unsigned long int addr = 0x4C6880;
			long int res;
			_asm {
				MOV ECX, this;
				PUSH unitId;
				CALL addr;
				MOV res, EAX;
			}
			return (res != 0);
		}
		bool Remove(long int unitId) {
			const unsigned long int addr = 0x4E3AF0;
			long int res;
			_asm {
				MOV ECX, this;
				PUSH unitId;
				CALL addr;
				MOV res, EAX;
			}
			return (res != 0);
		}
		bool Add(long int unitId) { // returns true if elem was added.
			const unsigned long int addr = 0x4C68C0;
			long int res;
			_asm {
				MOV ECX, this;
				PUSH unitId;
				CALL addr;
				MOV res, EAX;
			}
			return (res != 0);
		}
	};
	static_assert(sizeof(STRUCT_AI_UNIT_LIST_INFO) == 0x10, "STRUCT_LIST_OF_UNITID size");

}
