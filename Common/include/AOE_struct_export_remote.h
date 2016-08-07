#pragma once

#include <AOE_const_functional.h>
#include <AOE_const_internal.h>
#include <AOE_struct_game_settings.h>
#include <Windows.h>
#include <WinUser.h>
#include <assert.h>
#include <string>
#include <AOE_struct_export.h>

/* This file does not provide methods to do any treatments, only tools for debugging.
*/

namespace AOE_STRUCTURES
{

	// Allocates and gets a copy of a ROR structure
	template<class c> c *CreateObjectFromRORData(HANDLE h, unsigned long int gameAddress) {
		size_t mysize = sizeof(c);
		c_inst = (c*)malloc(mysize);
		SIZE_T readBytes;
		BOOL res = ReadProcessMemory(h, (void*)gameAddress, (void*)c_inst, mysize, &readBytes);
		if (!res || (readBytes < mysize)) {
			free(c_inst);
			return NULL;
		}
		return c_inst;
	}

	// Get a copy of a ROR structure to a buffer that is already allocated (make sure it is large enough).
	// Can also be used to read standard types like float, long int, etc.
	// forcedSize (optional) is the size to read/copy from ROR memory. Ignored if <= 0. Be careful when using it !
	// Returns true if successful
	template<class cl> bool GetObjectFromRORData(HANDLE h, unsigned long int gameAddress, cl *buffer, long int forcedSize = 0) {
		size_t mysize = sizeof(cl);
		if (forcedSize > 0) {
			mysize = forcedSize;
		}
		SIZE_T readBytes;
		BOOL res = ReadProcessMemory(h, (void*)gameAddress, buffer, mysize, &readBytes);
		if (!res || (readBytes < mysize)) {
			return false;
		}
		return true;
	}
	template<class cl> bool GetObjectFromRORData(HANDLE h, void *gameAddress, cl *buffer, long int forcedSize = 0) {
		return GetObjectFromRORData(h, (unsigned long int) gameAddress, buffer, forcedSize);
	}
	// This trick prevents compilation failures with (void*) type for buffer arg.
	inline bool GetObjectFromRORData(HANDLE h, unsigned long int gameAddress, void *buffer, long int forcedSize = 0) {
		if (forcedSize <= 0) { return false; }
		return GetObjectFromRORData(h, gameAddress, (char*) buffer, forcedSize);
	}


	// Get a zero-terminated string (unknown size). This will read string byte by byte to avoid reading invalid memory.
	static std::string GetZeroTerminatedString(HANDLE h, unsigned long int address, long int maxLength) {
		std::string res = "";
		// In this version we need to read ROR's memory byte by byte (only 1 byte each time) to avoid going out of allocated memory
		// (we can't know string's length)
		if (h == NULL) { return res; }
		if (maxLength <= 0) { maxLength = 0x80000; } // Limitation here
		long int readBytes = 0;
		unsigned long int curAddress = address;
		char buffer;

		while (readBytes < maxLength) {
			if (!GetObjectFromRORData(h, curAddress, &buffer, 1)) {
				return res;
			}
			if (buffer == 0) {
				return res;
			}
			res += buffer;
			curAddress++;
		}
		return res;
	}



	// This child class can export remote objects (from a different process).
	// All linked objects are copied from ROR process and then exported (recursively)
	class AOE_STRUCT_EXPORTER_REMOTE : public AOE_STRUCTURES::AOE_STRUCT_EXPORTER {
	public:
		// Handle to ROR process. Set this before calling various methods.
		HANDLE handleROR;

		AOE_STRUCT_EXPORTER_REMOTE() { this->handleROR = NULL; }

		// Get a (temporary?) object that actually contains all relevant object data (accessible in THIS context/process)
		virtual void *GetTempObject(void *obj, size_t objSize) {
			if (this->handleROR == NULL) { return ""; }
			std::string res = "";
			void *tempObj = malloc(objSize);
#ifdef _DEBUG
			std::wstring s;
			wchar_t buf[30];
			swprintf_s(buf, _T("malloc: %08X\n"), (unsigned long int)tempObj);
			s = buf;
			OutputDebugString(s.c_str());
#endif
			if (GetObjectFromRORData(this->handleROR, obj, tempObj, objSize)) {
				return tempObj;
			} else {
				free(tempObj);
				return NULL;
			}
		}

		// Clean temporary object if necessary
		virtual void CleanTempObject(void *tempObj) {
			// In this implementation, GetTempValidObject allocated a temp object so we have to free it here.
			if (tempObj != NULL) { free(tempObj); }
#ifdef _DEBUG
			std::wstring s;
			wchar_t buf[30];
			swprintf_s(buf, _T("free: %08X\n"), (unsigned long int)tempObj);
			s = buf;
			OutputDebugString(s.c_str());
#endif
		}

		// Get a zero-terminated string (unknown size). Poor performance but more secure/clean
		virtual std::string GetZeroTerminatedStringFromRORData(unsigned long int gameAddress, long int maxLength = 0) {
			return GetZeroTerminatedString(this->handleROR, gameAddress, maxLength);
		}
	};


}

