#include "../include/EXESelfEditor.h"


// Allow/disallow writing in .rdata section of AOE/ROR process.
// Returns true if successful
bool EnableWritingInRData(bool enable) {
	static DWORD processId = 0;
	if (processId == 0) {
		processId = GetCurrentProcessId(); // run it only once: process ID won't change ! (static variable)
	}
	if (processId == 0) { return false; }
	HANDLE hndl = OpenProcess(PROCESS_VM_OPERATION, true, processId);
	DWORD oldPermissions;
	// Note: by default, .rdata section is PAGE_READONLY (0x2)
	// Note: we don't need to use PAGE_EXECUTE_READWRITE, we don't intend to execute code in .rdata !
	DWORD newPermissions = enable ? PAGE_READWRITE : PAGE_READONLY;
	// Note: VirtualProtectEx requires PROCESS_VM_OPERATION
	BOOL callResult = VirtualProtectEx(hndl, (void*)AOE_OFFSETS::ADDR_RDATA_MIN,
		AOE_OFFSETS::ADDR_RDATA_MAX + 1 - AOE_OFFSETS::ADDR_RDATA_MIN, newPermissions, &oldPermissions);
	CloseHandle(hndl);
	return (callResult != 0);
}


// Returns true if successful.
bool WriteInMyMemory(unsigned long int address, char *buffer, int size) {
	if (address == 0) { return false; }
	if (buffer == NULL) { return false; }
	if (size <= 0) { return false; }

	DWORD processId = GetCurrentProcessId();
	if (processId == 0) { return false; }
	//HANDLE hndl = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
	HANDLE hndl = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, processId);
	SIZE_T sizeWritten = 0;
	BOOL success = WriteProcessMemory(hndl, (LPVOID)address, buffer, size, &sizeWritten);
	DWORD e = GetLastError();
	CloseHandle(hndl);
	return (success != 0);
}

// Returns true if successful.
bool WriteInMyMemory(unsigned long int address, unsigned char *buffer, int size) {
	 return WriteInMyMemory(address, (char*)buffer, size);
}


