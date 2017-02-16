
#include "FileSelector.h"
using namespace std;

FileSelector::FileSelector()
{
	m_isFileValid = false;
	m_fileName = _T("");
}

FileSelector::~FileSelector()
{
}


// To move in a better place
// Returns an empty string if dialog is canceled
std::wstring openfilename(WCHAR *filter, HWND owner, WCHAR *title) {
	OPENFILENAME *ofn = new OPENFILENAME();
	WCHAR fileName[MAX_PATH];
	ZeroMemory(fileName, MAX_PATH);
	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = owner;
	ofn->lpstrFilter = filter;
	ofn->lpstrFile = fileName;
	ofn->nMaxFile = MAX_PATH;
	ofn->Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn->lpstrDefExt = _T("");
	ofn->lpstrTitle = title;
	wstring fileNameStr;
	if (GetOpenFileName(ofn)) {
		fileNameStr = fileName;
	}
	delete ofn;
	return fileNameStr;
}


// Resets the class
// Returns true if the filename was correct before the reset
bool FileSelector::CloseFile(){
	m_fileName = _T("");
	bool oldValue = m_isFileValid;
	m_isFileValid = false;
	return m_isFileValid;
}

// Returns true if the filename was supplied and correct
bool FileSelector::AskFileName() {
	m_fileName = openfilename(_T("Executable files\0*.exe\0"), NULL, _T("Select ROR executable"));
	m_isFileValid = CheckFileExistence(m_fileName);
	return m_isFileValid;
}


// Returns true if the filename was supplied and correct
bool FileSelector::SetFileName(const std::wstring &fileName) {
	this->CloseFile();
	m_fileName = fileName;
	m_isFileValid = CheckFileExistence(m_fileName);
	return m_isFileValid;
}



// Returns true if the filename was found and correct
bool FileSelector::ReadRegistry()
{
	WCHAR szBuffer[MAX_PATH + 1];
	DWORD dwBufferSize = sizeof(szBuffer);
	LONG res;
	this->m_isFileValid = false;

	HKEY hKey;
	res = ERROR_FILE_NOT_FOUND;
	// Do not use: not compatible with Windows XP
	//res = RegGetValue(HKEY_LOCAL_MACHINE, AOE_REG_PATH_INSTALLDIR_32, AOE_REG_KEY_INSTALLDIR, RRF_RT_ANY, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, AOE_REG_PATH_INSTALLDIR_32, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
		res = RegQueryValueEx(hKey, AOE_REG_KEY_INSTALLDIR, NULL, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	}
	if (res != ERROR_SUCCESS) {
		if (hKey) { RegCloseKey(hKey); }
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, AOE_REG_PATH_INSTALLDIR_64, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
			res = RegQueryValueEx(hKey, AOE_REG_KEY_INSTALLDIR, NULL, NULL, (LPBYTE)szBuffer, &dwBufferSize);
		}
	}
	RegCloseKey(hKey);

	if (res == ERROR_SUCCESS) {
		szBuffer[dwBufferSize] = 0;
		m_fileName = szBuffer;
		m_fileName += _T("\\");
		m_fileName += ROR_EXE_NAME;
		m_isFileValid = CheckFileExistence(m_fileName);
		return m_isFileValid;
	}
	return false;
}


// Modifies registry to change AOE/ROR installation directory
bool FileSelector::SetInstallDirInRegistry(const std::wstring &newPath) {
	WCHAR szBuffer[MAX_PATH + 1];
	DWORD newPathLength = newPath.length();
	DWORD dwBufferSize = newPathLength * sizeof(wchar_t);
	LONG res;

	if (dwBufferSize > MAX_PATH) {
		return false;
	}

	std::wstring checkFilename = newPath;
	checkFilename += _T("\\");
	checkFilename += ROR_EXE_NAME;
	if (!CheckFileExistence(checkFilename)) {
		return false;
	}

	swprintf_s(szBuffer, newPath.c_str());

	HKEY hKey = NULL;
	res = ERROR_FILE_NOT_FOUND;
	// Do not use: not compatible with Windows XP
	//res = RegGetValue(HKEY_LOCAL_MACHINE, AOE_REG_PATH_INSTALLDIR_32, AOE_REG_KEY_INSTALLDIR, RRF_RT_ANY, NULL, (LPBYTE)szBuffer, &dwBufferSize);

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, AOE_REG_PATH_INSTALLDIR_32, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
		res = RegSetValueEx(hKey, AOE_REG_KEY_INSTALLDIR, NULL, REG_SZ, (LPBYTE)szBuffer, dwBufferSize + 1);
	}
	if (res != ERROR_SUCCESS) {
		if (hKey) { RegCloseKey(hKey); }
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, AOE_REG_PATH_INSTALLDIR_64, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
			res = RegSetValueEx(hKey, AOE_REG_KEY_INSTALLDIR, NULL, REG_SZ, (LPBYTE)szBuffer, dwBufferSize + 1);
		}
	}
	RegCloseKey(hKey);
	return true;
}

