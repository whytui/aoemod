#include <tchar.h>
#include <string>
#include <gameVersion.h>

#pragma once


const static std::wstring AOEFileVersionLabel[AOE_VERSION_COUNT] = { _T("Invalid"), _T("AOE1.0a"), _T("AOE1.0b"), _T("AOE1.0c"),
_T("ROR1.0b"), _T("ROR1.0c"), _T("UPatch Beta"), _T("UPatch 1.1HD") };
#define GET_AOE_FILE_VERSION_LABEL(n) AOEFileVersionLabel[n]


const static wchar_t *AOE_EXE_NAME_NOEXT = _T("Empires");
const static wchar_t *ROR_EXE_NAME_NOEXT = _T("EmpiresX");
const static wchar_t *AOE_EXE_NAME = _T("Empires.exe");
const static wchar_t *ROR_EXE_NAME = _T("EmpiresX.exe");

// Registry keys
const static wchar_t *AOE_REG_PATH_INSTALLDIR_32 = _T("SOFTWARE\\Microsoft\\Games\\Age of Empires\\1.00");
const static wchar_t *AOE_REG_PATH_INSTALLDIR_64 = _T("SOFTWARE\\Wow6432Node\\Microsoft\\Games\\Age of Empires\\1.00");
const static wchar_t *AOE_REG_KEY_INSTALLDIR = _T("InstallationDirectory");
const static wchar_t *AOE_REG_KEY_SCREEN_SIZE = _T("Screen Size");
const static wchar_t *DDRAW_COMPATIBILITY_REG_PATH = _T("SOFTWARE\\Microsoft\\DirectDraw\\Compatibility");

// File sizes
const static long EXE_FILE_SIZE_AOE10A = 1622016; // 0.9.12.1215
const static long EXE_FILE_SIZE_AOE10B = 1622016; // 0.9.13.490 : same size, but different version !
const static long EXE_FILE_SIZE_AOE10C = 1470464;
const static long EXE_FILE_SIZE_10B = 1513984;
const static long EXE_FILE_SIZE_10C = 1503232;
const static long EXE_FILE_SIZE_UPATCH_BETA = 1511424;
const static long EXE_FILE_SIZE_UPATCH_1_1_HD = 1515520;
const static long EXE_FILE_SIZE_AOK0005030706 = 2077184;

// EXE Magic number
const static size_t EXE_MAGIC_NUMBER_SIZE = 2;
const static unsigned char EXE_MAGIC_NUMBER[EXE_MAGIC_NUMBER_SIZE] = { 0x4d, 0x5a };

// Binary sequence: EXE version
const static size_t EXE_VERSION_CHECK_AOE10A_OFFSET = 0x164008;
const static size_t EXE_VERSION_CHECK_AOE10A_SIZE = 13;
const static unsigned char EXE_VERSION_CHECK_AOE10A[EXE_VERSION_CHECK_AOE10A_SIZE] = { // ascii 00.09.12.1215.
	0x30, 0x30, 0x2E, 0x30, 0x39, 0x2E, 0x31, 0x32, 0x2E, 0x31, 0x32, 0x31, 0x35
};
const static size_t EXE_VERSION_CHECK_AOE10B_OFFSET = 0x1673F0;
const static size_t EXE_VERSION_CHECK_AOE10B_SIZE = 13;
const static unsigned char EXE_VERSION_CHECK_AOE10B[EXE_VERSION_CHECK_AOE10B_SIZE] = { // ascii 00.09.13.0409. Shouldn't it be 490 instead of 409 ?
	0x30, 0x30, 0x2E, 0x30, 0x39, 0x2E, 0x31, 0x33, 0x2E, 0x30, 0x34, 0x30, 0x39
};
const static size_t EXE_VERSION_CHECK_AOE10C_OFFSET = 0x15103C;
const static size_t EXE_VERSION_CHECK_AOE10C_SIZE = 13;
const static unsigned char EXE_VERSION_CHECK_AOE10C[EXE_VERSION_CHECK_AOE10C_SIZE] = { // ascii 00.09.13.0409 (same version number, but different offset, they forgot updating this?)
	0x30, 0x30, 0x2E, 0x30, 0x39, 0x2E, 0x31, 0x33, 0x2E, 0x30, 0x34, 0x30, 0x39
};

const static size_t EXE_VERSION_CHECK_10B_OFFSET = 0x1716e2;
const static size_t EXE_VERSION_CHECK_10B_SIZE = 18;
const static unsigned char EXE_VERSION_CHECK_10B[EXE_VERSION_CHECK_10B_SIZE] = { // ascii 4.2.0901
	0x30, 0x00, 0x34, 0x00, 0x2E, 0x00, 0x32, 0x00, 0x2E, 0x00, 0x30, 0x00, 0x39, 0x00, 0x30, 0x00, 0x31, 0x00
};
const static size_t EXE_VERSION_CHECK_10C_OFFSET = 0x16ece6;
const static size_t EXE_VERSION_CHECK_10C_SIZE = 18;
const static unsigned char EXE_VERSION_CHECK_10C[EXE_VERSION_CHECK_10C_SIZE] = { // ascii 1.6.1115
	0x30, 0x00, 0x31, 0x00, 0x2E, 0x00, 0x36, 0x00, 0x2E, 0x00, 0x31, 0x00, 0x31, 0x00, 0x31, 0x00, 0x35, 0x00
};

// AOK
const static size_t EXE_VERSION_CHECK_AOK0005030706_OFFSET = 0x1C4368;
const static size_t EXE_VERSION_CHECK_AOK0005030706_SIZE = 14;
const static unsigned char EXE_VERSION_CHECK_AOK0005030706[EXE_VERSION_CHECK_AOK0005030706_SIZE] = { // ascii 00.06..01.0717
	0x30, 0x30, 0x2E, 0x30, 0x36, 0x2E, 0x30, 0x31, 0x2E, 0x30, 0x37, 0x31, 0x37, 0x00
};
