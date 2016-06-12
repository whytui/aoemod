
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
	if (GetOpenFileName(ofn))
		fileNameStr = fileName;
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
	m_isFileValid = false;
	//AOE_REG_KEY_INSTALLDIR;

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
		m_fileName += AOE_EXE_NAME;
		m_isFileValid = CheckFileExistence(m_fileName);
		return m_isFileValid;
	}
	return false;
}


// Returns true if the file m_fileName exists
//bool FileSelector::CheckFileExistence(std::wstring fileName) {
bool CheckFileExistence(const std::wstring &fileName) {
	char str[MAX_PATH];
	sprintf_s(str, MAX_PATH, "%ls", fileName.c_str());
	FILE *fTest;
	int my_errno = fopen_s(&fTest, str, "r");
	if (!my_errno) {
		fclose(fTest);
		return true;
	}
	return false;
}

bool DirectoryExists(const char* dirName) {
	DWORD attribs = GetFileAttributesA(dirName);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool DirectoryExists(const std::wstring &dirName) {
	DWORD attribs = GetFileAttributes(dirName.c_str());
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0;
}


vector<std::wstring> GetFileListFromFolder(const std::wstring &folder)
{
	vector<std::wstring> names;
	std::wstring searchPattern = std::wstring(folder + _T("\\*"));
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(searchPattern.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder (excluding . and ..)
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}
