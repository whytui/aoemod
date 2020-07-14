#include <Windows.h>
#include <string> // std::wstring...
#include <tchar.h>
#include <mystrings.h>
#include <FileSelector.h>
#include <customResolution.h>
#include "basicFilesHandling.h"

#pragma once

// Manage file operations to install RockNRor (and ROR_API + wndmode.dll)
// shortMessage and logs are OUT parameters.
// Returns true if successful
bool installRockNRorFiles(std::wstring sourceDirectory, std::wstring targetExeFullPath, bool overwriteFiles,
	std::wstring &shortMessage, std::wstring &logs);

// Manage file operations to install a custom resolution
// logs is an OUT parameter.
// Returns true if successful
bool installResolutionFiles(std::wstring data_DRS_fullPath, std::wstring data2_DRS_fullPath, std::wstring targetExeFullPath,
	std::wstring &logs);

