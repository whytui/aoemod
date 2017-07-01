#include "../include/interruption.h"


/* This file handles interrupting ROR process, to show -really- modal popups, attach a debugger... */


namespace ROCKNROR {
;
namespace SYSTEM {

	// Show a modal messagebox that REALLY interrupts ROR thread.
	// buttons: buttons to include, e.g. MB_OK (default)
	// icon: optional flag to use a specific icon, default MB_ICONERROR.
	int ShowMessageModal(const wchar_t *title, const wchar_t *message, UINT buttons, UINT icon) {
		return MessageBox(NULL, message, title, buttons | icon | MB_TASKMODAL); // MB_TASKMODAL completely blocks the rest of the execution = we have a REAL modal popup
	}


	// Force refresh of UI even if process is currently interrupted/busy
	void ForceRefreshWindow() {
		HWND hWnd = ::GetActiveWindow();
		UpdateWindow(hWnd);
	}


	// allowEscToContinue = if true, the user can press ESCAPE to resume program execution
	// tryConnectDebugger = if true, set a breakpoint (and resume execution) when a debugger is attached
	// In RELEASE mode, we can wait for debugger, but we can't break execution (user needs to set a breakpoint manually)
	// If in-game, it might be safer to call AOE_METHODS::SetGamePause(true) prior to this.
	void StopExecution(const wchar_t *message, bool allowEscToContinue, bool tryConnectDebugger) {
		BOOL doBreak = tryConnectDebugger && ::IsDebuggerPresent();
		if (!doBreak) {
			// ForceRefreshWindow(); // Not necessary as MessageBox does the job
			ShowMessageModal(_T("Execution stopped"), message);
			bool goOn = true;
			while (goOn) {
				Sleep(200);
				short escState = GetAsyncKeyState(VK_ESCAPE);
				goOn = !allowEscToContinue || (escState == 0);
				if (tryConnectDebugger && ::IsDebuggerPresent()) { // will automatically stop sleeping when a debugger is attached
					doBreak = true;
					goOn = false;
				}
			}
		}
		if (doBreak) {
#ifdef _DEBUG
			// Crashes if not attached to debugger. This is better than DebugBreak() (which stop execution in some obscure asm code, not here)
			// This has no effect in Release mode ! you need to set a breakpoint manually.
			_CrtDbgBreak();
#endif
		}
	}

}
}
