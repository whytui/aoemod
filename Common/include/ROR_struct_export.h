#pragma once

#include <AOE_const_functional.h>
#include <AOE_const_internal.h>
#include <ROR_structures.h>
#include <mystrings.h>
#include <assert.h>
#include <string>


/* This file does not provide methods to do any treatments, only tools for debugging.
*/

namespace AOE_STRUCTURES
{

	class ROR_STRUCT_EXPORTER {
	public:
		ROR_STRUCT_EXPORTER() {}

		// Default behaviour: provided object is the "real" object so we can directly access its members
		template<class T> std::string ExportStruct(T *obj, size_t forcedSize = 0) {
			std::string res = "";
			size_t myObjSize = forcedSize;
			if (myObjSize <= 0) {
				myObjSize = sizeof(T);
			}
			// 1) Prepare the object (get a valid copy if needed)
			T* tempObj = (T*)this->GetTempObject(obj, myObjSize);
			if (tempObj == NULL) {
				res = "(failed)";
			} else {
				// 2) If OK, call specialized writer
				res = this->ExportStruct_internal(tempObj, (unsigned long int) obj);
			}
			// 3) Clean temp object if needed
			this->CleanTempObject(tempObj);
			return res;
		}

		// Similar to ExportStruct, but obj refers to an array of "elemCount" objects (of type T)
		template<class T> std::string ExportArrayOfStruct(T *obj, int elemCount) {
			std::string res = "";
			if (elemCount < 1) { return ""; }
			size_t myObjSize = sizeof(T) * elemCount;
			// 1) Prepare the object (get a valid copy if needed)
			T* tempObj = (T*)this->GetTempObject(obj, myObjSize);
			if (tempObj == NULL) {
				res = "(failed)";
			} else {
				// 2) If OK, call specialized writer
				for (int i = 0; i < elemCount;  i++) {
					if (i > 0) { res += " "; }
					res += "{";
					res += this->ExportStruct_internal(tempObj + i, (unsigned long int) obj);
					res += "}";
				}
			}
			// 3) Clean temp object if needed
			this->CleanTempObject(tempObj);
			return res;
		}


		// This overload exports a copy of a ROR object whose links are hosted in ROR process
		// (pointers lead to ROR process addresses, not addresses that are valid in THIS context)
		template<class T> std::string ExportHostedStruct(T *obj, unsigned long int actualRORAddress) {
			return this->ExportStruct_internal(obj, actualRORAddress);
		}

		// Get a zero-terminated string (unknown size). This will read string byte by byte to avoid reading invalid memory.
		virtual std::string GetZeroTerminatedStringFromRORData(unsigned long int gameAddress, long int maxLength = 0) {
			// Base version: just read bytes 1 by one
			std::string res = "";
			if (maxLength <= 0) { maxLength = 0x80000; } // (huge) Limitation here
			long int readBytes = 0;
			char *curPos = (char*)gameAddress;
			while (readBytes < maxLength) {
				if (*curPos == 0) {
					return res;
				}
				res += *curPos;
				curPos++;
			}
			return res;
		}

	protected:
		// Get a (temporary - if remote) object that actually contains all relevant object data (accessible in THIS context/process)
		// Do not call directly. Call CleanTempObject once you're finished with object.
		virtual void *GetTempObject(void *obj, size_t objSize) {
			// In base implementation we directly use provided object
			return obj;
		}
		// Clean temporary object if necessary
		virtual void CleanTempObject(void *tempObj) {
			// Nothing to do here because GetTempValidObject returns the object itself (not a copy) - no object to free here.
		}

		// obj=structure to export as text. RORAddress is the original/true address of object in ROR process.
		std::string ExportStruct_internal(char *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_GAME_SETTINGS *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_GAME_GLOBAL *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_MP_COMMUNICATION *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_PLAYER *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_UNIT_BASE *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_UNIT_BIRD *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_UNIT_LIVING *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_UNIT_BUILDING *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_UNIT *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_UNITDEF_BASE *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_UNITDEF_TYPE50 *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_DEF_UNIT *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_ARMOR_OR_ATTACK *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_UNIT_GROUP_ELEM *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_ACTION_BASE *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_UNIT_ACTION_INFO *obj, unsigned long int RORAddress);
		std::string ExportStruct_internal(STRUCT_ACTION_LINK *obj, unsigned long int RORAddress);
	};


}

