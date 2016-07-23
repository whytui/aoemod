#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>


// Returns true if the file fileName exists
bool CheckFileExistence(const char *fileName);
// Returns true if the file fileName exists
bool CheckFileExistence(const std::wstring &fileName);

bool DirectoryExists(const char* dirName);
bool DirectoryExists(const std::wstring &dirName);

std::vector<std::wstring> GetFileListFromFolder(const std::wstring &folder);
