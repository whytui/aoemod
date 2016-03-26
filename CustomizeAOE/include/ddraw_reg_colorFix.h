#include <Windows.h>
#include <winreg.h>
#include <string> // std::wstring...
#include <tchar.h>
#include <AOE_const_global.h>
#include "FileSelector.h"
#include "mystrings.h"

#pragma once


static long int GetColorFixFileDDrawID(std::wstring EXE_fullPath) {
	char str[MAX_PATH];
	sprintf_s(str, MAX_PATH, "%ls", EXE_fullPath.c_str());
	FILE *file;
	int my_errno = fopen_s(&file, str, "r");
	if (my_errno) {
		return 0;
	}
	fseek(file, 0x3C, 0);
	long int offset;
	long int result;
	size_t res = fread_s(&offset, sizeof(long int), sizeof(long int), 1, file);
	if (res < 1) { // res = nb of elements (not size !)
		fclose(file); 
		return 0;
	}
	offset += 0x08;
	fseek(file, offset, 0);
	res = fread_s(&result, sizeof(long int), sizeof(long int), 1, file);
	fclose(file);
	return result;
}


// This allows installing registry fix for ddraw color bug in Windows7 or later versions
static bool FixRegistryForDDrawColorBug(std::wstring EXE_fullPath) {
	long int ddrawIDToUse = GetColorFixFileDDrawID(EXE_fullPath);
	if (ddrawIDToUse == 0) { return false; }

	WCHAR szNameBuffer[MAX_PATH + 1];
	DWORD dwNameBufferSize = sizeof(szNameBuffer);
	LONG resKey;
	LONG resKeyValue;
	std::wstring fileName = extractFileName(EXE_fullPath);
	std::wstring regKeyWithFileName = DDRAW_COMPATIBILITY_REG_PATH;
	regKeyWithFileName += _T("\\cror_");
	regKeyWithFileName += fileName;

	swprintf_s(szNameBuffer, _T("%s"), fileName.c_str());
	dwNameBufferSize = fileName.length() * sizeof(wchar_t);

	HKEY hkey_res;
	DWORD lpDisp_res;

	resKeyValue = ERROR_FILE_NOT_FOUND;
	// Do not use: not compatible with Windows XP
	//res = RegGetValue(HKEY_LOCAL_MACHINE, AOE_REG_PATH_INSTALLDIR_32, AOE_REG_KEY_INSTALLDIR, RRF_RT_ANY, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	// KEY_WOW64_64KEY allows to bypass the *$!# Wow6432Node: REALLY manage the key described in DDRAW_COMPATIBILITY_REG_PATH
	resKey = RegOpenKeyEx(HKEY_LOCAL_MACHINE, regKeyWithFileName.c_str(), 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hkey_res);
	if (resKey != ERROR_SUCCESS) {
		// Create missing key
		resKey = RegCreateKeyEx(HKEY_LOCAL_MACHINE, regKeyWithFileName.c_str(), 0, NULL /*lpClass*/, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS | KEY_WOW64_64KEY, NULL /*attr*/, &hkey_res, &lpDisp_res);
		// lpDisp_res: REG_CREATED_NEW_KEY=key was created. REG_OPENED_EXISTING_KEY=key already existed (unchanged)
	}

	if (resKey == ERROR_SUCCESS) {
		resKeyValue = RegSetValueEx(hkey_res, _T("Name2"), NULL, REG_SZ, (BYTE*)szNameBuffer, dwNameBufferSize);
		if (resKey == ERROR_SUCCESS)
			resKeyValue = RegSetValueEx(hkey_res, _T("ID2"), NULL, REG_BINARY, (BYTE*)&ddrawIDToUse, 4);
			if (resKey == ERROR_SUCCESS)
				resKeyValue = RegSetValueEx(hkey_res, _T("Flags2"), NULL, REG_BINARY, new byte[] {0, 8, 0, 0}, 4);
	}

	RegCloseKey(hkey_res);

	return (resKey == ERROR_SUCCESS);
}
