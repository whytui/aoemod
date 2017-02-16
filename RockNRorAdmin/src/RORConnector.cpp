#include "../include/RORConnector.h"


RORConnector::RORConnector() {
	this->hProcess = NULL;
	this->processId = -1;
}


RORConnector::~RORConnector() {
	this->DisconnectFromROR();
}

// Returns true if successful
// Please call DisconnectFromROR when finished using it (but destructor will do it anyway).
bool RORConnector::ConnectToROR() {
	HWND hWndTmp;
	this->hProcess = NULL;
	this->processId = -1;
	// Find process handle
	hWndTmp = FindWindow(NULL, _T("Age of Empires: The Rise of Rome"));
	if (hWndTmp == NULL) {
		hWndTmp = FindWindow(NULL, _T("Age of Empires Expansion"));
		if (hWndTmp == NULL) {
			return false;
		}
	}
	// Search for process id
	DWORD threadId = GetWindowThreadProcessId(hWndTmp, &this->processId);
	if (threadId == 0) {
		this->processId = -1;
		return false;
	}
	// Found the process id: open it so we can manipulate its data
	this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, this->processId);
	if (this->hProcess == NULL) {
		this->processId = -1;
		return false;
	}
	return true;
}


void RORConnector::DisconnectFromROR() {
	if (this->hProcess) {
		CloseHandle(this->hProcess);
		this->hProcess = NULL;
		this->processId = -1;
	}
}

// Returns NULL if not connected to ROR
HANDLE RORConnector::GetRORProcessHandle() {
	return this->hProcess;
}

