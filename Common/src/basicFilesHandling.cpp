#include "basicFilesHandling.h"



// Returns true if the file m_fileName exists
bool CheckFileExistence(const std::wstring &fileName) {
	char str[MAX_PATH];
	sprintf_s(str, MAX_PATH, "%ls", fileName.c_str());
	return CheckFileExistence(str);
	/*FILE *fTest;
	int my_errno = fopen_s(&fTest, str, "r");
	if (!my_errno) {
		fclose(fTest);
		return true;
	}
	return false;*/
}
// Returns true if the file fileName exists
bool CheckFileExistence(const char *fileName) {
	FILE *fTest;
	int my_errno = fopen_s(&fTest, fileName, "r");
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


std::vector<std::wstring> GetFileListFromFolder(const std::wstring &folder)
{
	std::vector<std::wstring> names;
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

// Writes text to file. if append=true, append, otherwise overwrite existing file
// Returns true if successful
bool WriteToFile(const std::string &text, const std::string &filename, bool append) {
	FILE *file = NULL;
	char *openMode = append ? "ab" : "wb"; // open in binary mode to avoid modifying newline chars
	errno_t err = fopen_s(&file, filename.c_str(), openMode);
	if (err) {
		return false;
	}
	fwrite(text.c_str(), text.length(), sizeof(char), file);
	//fprintf_s(file, text.c_str());
	fclose(file);
	return true;
}

// Writes text to file. if append=true, append, otherwise overwrite existing file
// Might be slower than overload with std::string becaues of strlen.
// Returns true if successful
bool WriteToFile(const char *text, const char *filename, bool append) {
	FILE *file = NULL;
	char *openMode = append ? "ab" : "wb"; // open in binary mode to avoid modifying newline chars
	errno_t err = fopen_s(&file, filename, openMode);
	if (err) {
		return false;
	}
	fwrite(text, strlen(text), sizeof(char), file);
	fclose(file);
	return true;
}

