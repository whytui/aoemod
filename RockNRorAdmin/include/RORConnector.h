#include "Windows.h"
#include <tchar.h>
#include "Version.h"

#pragma once

class RORConnector {
public:
	RORConnector();
	~RORConnector();

	// Returns true if successful
	// Please call DisconnectFromROR when finished using it (but destructor will do it anyway).
	bool ConnectToROR();
	void DisconnectFromROR();

	// Returns NULL if not connected to ROR
	HANDLE GetRORProcessHandle();

private:
	HANDLE hProcess;
	DWORD processId;
};
