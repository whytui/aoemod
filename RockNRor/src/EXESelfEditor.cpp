#include "../include/EXESelfEditor.h"


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
