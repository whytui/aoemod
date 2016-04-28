#include "../include/EXESelfEditor.h"


void WriteInMyMemory(unsigned long int address, char *buffer, int size) {
	if (address == 0) { return; }
	if (buffer == NULL) { return; }
	if (size <= 0) { return; }

	long int test = 0;
	DWORD processId = GetCurrentProcessId();
	if (processId == 0) { return; }
	//HANDLE hndl = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
	HANDLE hndl = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, processId);
	SIZE_T sizeWritten;
	BOOL success = WriteProcessMemory(hndl, (LPVOID)address, buffer, size, &sizeWritten);
	DWORD e = GetLastError();
	CloseHandle(hndl);
}


void WriteInMyMemory(unsigned long int address, unsigned char *buffer, int size) {
	 WriteInMyMemory(address, (char*)buffer, size);
}
