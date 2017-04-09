#pragma once

#include <string>
#include <locale>
#include <vector>

// Convert a wstring to "normal" string
static std::string narrow(const std::wstring& ws)
{
	if (ws.empty()) {
		return "";
	}
	std::vector<char> buffer(ws.size());
	std::locale loc("english");
	std::use_facet< std::ctype<wchar_t> >(loc).narrow(ws.data(), ws.data() + ws.size(), '?', &buffer[0]);

	return std::string(&buffer[0], buffer.size());
}

// Convert a string to a wide string
static std::wstring widen(const std::string& s)
{
	if (s.empty()) { return L""; }
	std::vector<wchar_t> buffer(s.size());
	std::locale loc("english");
	std::use_facet< std::ctype<wchar_t> >(loc).widen(s.data(), s.data() + s.size(), &buffer[0]);

	return std::wstring(&buffer[0], buffer.size());
}

// Converts supplied wstring to lowercase
static void towlower(std::wstring &s) {
	for (std::wstring::iterator it = s.begin(); it != s.end(); it++)
		*it = towlower(*it);
}


// Get a directory from a full path
static std::wstring extractDirectory(std::wstring fullPath) {
	if (fullPath.empty()) { return L""; }
	return fullPath.substr(0, fullPath.find_last_of(L"/\\"));
}

// Get file name from a full path
static std::wstring extractFileName(std::wstring fullPath) {
	if (fullPath.empty()) { return L""; }
	return fullPath.substr(fullPath.find_last_of(L"/\\") + 1);
}

// Get an hexadecimal address as a string of (2+)8 characters (capital letters, if any). Like 0x1234ABCD.
static std::string GetHexStringAddress(unsigned long int address) {
	char buf[15];
	sprintf_s(buf, "0x%08X", address);
	return std::string(buf);
}
// Get an hexadecimal address as a string of (2+)8 characters (capital letters, if any). Like 0x1234ABCD.
static std::string GetHexStringAddress(void *address) {
	return GetHexStringAddress((unsigned long int)address);
}

// Get an hexadecimal address as a string of (2+)"hexDigits" characters (capital letters, if any). Like 0x1234ABCD.
static std::string GetHexStringAddress(unsigned long int address, int hexDigits) {
	if (hexDigits > 8) { return std::string(""); }
	char buf[32];
	static char format[] = "0x%08X";
	format[4] = hexDigits + '0';
	sprintf_s(buf, format, address);
	return std::string(buf);
}

// Returns int value from str, or defaultValue if str is not a valid number
static int StrToInt(const char *str, int defaultValue) {
	if (str == NULL) {
		return defaultValue;
	}
	int val = atoi(str);
	if (val != 0) {
		return val;
	}
	const char *tmp = str;
	bool onlyZeros = true;
	while ((*tmp != 0) && onlyZeros) {
		if (*tmp != '0') { onlyZeros = false; }
		tmp++;
	}
	if (!onlyZeros) {
		return -1; // atoi returned 0 but the string is not "0" or "00..." (it means it failed).
	}
	return 0;
}
