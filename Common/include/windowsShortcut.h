
#include <windows.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shlobj.h> // To get destkop directory
#include "mystrings.h"


// Create a new shortcut file (shortcutFullPath) that points to targetFileFullPath.
// Warning: shortcutFullPath is wide-char-based, whereas shortcutFullPath is narrow-char-based.
// If the shortcur already exists, it is overwritten.
static HRESULT CreateWindowsShortcut(LPCWSTR targetFileFullPath, LPCSTR shortcutFullPath, LPCWSTR shortcutDescription)
{
	HRESULT hres;
	IShellLink* psl;

	// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
	// has already been called.
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
	if (SUCCEEDED(hres)) {
		IPersistFile* ppf;

		// Set the path to the shortcut target and add the description. 
		psl->SetPath(targetFileFullPath);
		psl->SetDescription(shortcutDescription);
		psl->SetWorkingDirectory(extractDirectory(targetFileFullPath).c_str()); // Set start folder (required to run empires/empiresX)

		// Query IShellLink for the IPersistFile interface, used for saving the 
		// shortcut in persistent storage. 
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

		if (SUCCEEDED(hres)) {
			WCHAR wsz[MAX_PATH];

			// Ensure that the string is Unicode. 
			MultiByteToWideChar(CP_ACP, 0, shortcutFullPath, -1, wsz, MAX_PATH);

			// Save the link by calling IPersistFile::Save. 
			hres = ppf->Save(wsz, TRUE);
			ppf->Release();
		}
		psl->Release();
	}
	return hres;
}


// Returns Full path to current user's desktop, or NULL if failed
static LPSTR getDesktopDirectory()
{
	static char path[MAX_PATH + 1];
	if (SHGetSpecialFolderPathA(HWND_DESKTOP, path, CSIDL_DESKTOP, FALSE)) {
		return path;
	} else {
		return NULL;
	}
}

