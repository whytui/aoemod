#include <Windows.h>
#include <winreg.h>
#include <string> // std::wstring...
#include <tchar.h>
#include <vector>
#include <AOE_const_global.h>
#include <basicFilesHandling.h>
//#include <Shlobj.h> // CSIDL_MYDOCUMENTS
#include "AOE_const_global.h"

#pragma once

/* This class is used to select an AOE executable file (provides a full path)
 * Allows to find the file from windows registry or manually
*/
class FileSelector
{
public:
	FileSelector();
	~FileSelector();

	bool CloseFile();
	bool IsFileValid() { return m_isFileValid; };
	// Returns _T("") if no (valid) file has been selected
	std::wstring getFileName() { return m_isFileValid ? m_fileName : _T(""); };
	bool ReadRegistry(); // Reads AOE installation EXE file
	bool AskFileName(); // The user selects a file
	bool SetFileName(const std::wstring &fileName);

	// Modifies registry to change AOE/ROR installation directory
	bool SetInstallDirInRegistry(const std::wstring &newPath);

protected:
	bool m_isFileValid;
	std::wstring m_fileName;
};

// Returns an empty string if dialog is canceled
std::wstring openfilename(WCHAR *filter = _T("All Files (*.*)\0*.*\0"), HWND owner = NULL, WCHAR *title = _T("Select file"));
