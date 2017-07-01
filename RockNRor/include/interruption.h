
#pragma once

#include <Windows.h>
#include <crtdbg.h> // _CrtDbgBreak()
#include <tchar.h> // _T()


/* This file handles interrupting ROR process, to show -really- modal popups, attach a debugger... */



namespace ROCKNROR {
;
namespace SYSTEM {

	// Show a modal messagebox that REALLY interrupts ROR thread.
	// buttons: buttons to include, e.g. MB_OK (default)
	// icon: optional flag to use a specific icon, default MB_ICONERROR.
	int ShowMessageModal(const wchar_t *title, const wchar_t *message, UINT buttons = MB_OK, UINT icon = MB_ICONERROR);


	// Force refresh of UI even if process is currently interrupted/busy
	void ForceRefreshWindow();


	// allowEscToContinue = if true, the user can press ESCAPE to resume program execution
	// tryConnectDebugger = if true, set a breakpoint (and resume execution) when a debugger is attached
	// In RELEASE mode, we can wait for debugger, but we can't break execution (user needs to set a breakpoint manually)
	void StopExecution(const wchar_t *message, bool allowEscToContinue, bool tryConnectDebugger);

}
}
