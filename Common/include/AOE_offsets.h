
#pragma once

#include <gameVersion.h>

// Read offsets from appropriate file
#ifdef GAMEVERSION_AOE10b
#include <AOE_offsets_AOE10b.h>
#endif
#ifdef GAMEVERSION_AOE10c
#include <AOE_offsets_AOE10c.h>
#endif
#ifdef GAMEVERSION_ROR10b
#include <AOE_offsets_ROR10b.h>
#endif
#ifdef GAMEVERSION_ROR10c
#include <AOE_offsets_ROR10c.h>
#endif

/* 
 * This file contains various offsets/values that are specific to each AOE/ROR executable
 * Please read README.txt first.
 * Reading/using information from this file implies sharing with the community
 * ALL your findings about AOE/ROR structures
 * Please share knowledge for better modding experience !
 */
namespace AOE_OFFSETS
{
	// Some useful raw addresses (other versions - not supported !)
#ifdef GAMEVERSION_PTRIBE
	static const unsigned long int ADDR_VAR_PANEL_SYSTEM = 0x00595B90; // Base point for managing screens, popups = TPanelSystem
#endif
#ifdef GAMEVERSION_AOE10a
	static const unsigned long int ADDR_FILE_EXE_MIN = 0x400;
#endif


	static const unsigned long int ADDR_VAR_ACTIVE_UI_STRUCT = ADDR_VAR_PANEL_SYSTEM + 0x0C; // part of a bigger struct, +C
	static const unsigned long int ADDR_VAR_CURRENT_UI_OBJECT = ADDR_VAR_PANEL_SYSTEM + 0x0C; // part of a bigger struct, +C


};


// Useful function to control checksums on unknown structs
static bool isAValidRORChecksum(unsigned long int checksum){
	// Not completely certain of the interval, but seems quite good.
	return (checksum >= AOE_OFFSETS::ADDR_CHECKSUM_MIN) && (checksum < AOE_OFFSETS::ADDR_CHECKSUM_MAX) &&
		((checksum & 0x3) == 0); // 2 lower bits are 0 means it's a multiple of 4. (checksums values are dword-aligned values).
}

// Returns 0 if offset is invalid.
static unsigned long int AOE_FileOffsetToExeAddr(unsigned long int fileOffset) {
	if (fileOffset < AOE_OFFSETS::ADDR_FILE_EXE_MIN) { return 0; }
#if defined(GAMEVERSION_AOE10b) || defined(GAMEVERSION_ROR10b)
	if (fileOffset < AOE_OFFSETS::ADDR_FILE_EXE_MAX) {
		return fileOffset + 0x400c00;
	}
	if (fileOffset < (AOE_OFFSETS::ADDR_THIS_CODE_MAX - 0x401a00)) {
		return fileOffset + 0x401a00;
	}
	if (fileOffset < (AOE_OFFSETS::ADDR_RDATA_MAX - 0x401E00)) {
		return fileOffset + 0x401E00;
	}
	if (fileOffset < (AOE_OFFSETS::ADDR_DATA_MAX - 0x402600)) {
		return fileOffset + 0x402600;
	}
#endif
#if defined(GAMEVERSION_AOE10c) || defined(GAMEVERSION_ROR10c)
	if (fileOffset < (AOE_OFFSETS::ADDR_DATA_MAX - 0x400000)) {
		return fileOffset + 0x400000;
	}
#endif
	return 0;
}

// Returns 0 if address is invalid.
static unsigned long int AOE_ExeAddrToFileOffset(unsigned long int ExeAddr) {
	// TODO: only supports text (not THIS_COD, .rdata, .data)
	if ((ExeAddr < AOE_OFFSETS::ADDR_EXE_MIN) || (ExeAddr > AOE_OFFSETS::ADDR_EXE_MAX)) {
		return 0;
	}
#if defined(GAMEVERSION_AOE10b) || defined(GAMEVERSION_ROR10b)
	return ExeAddr - 0x400C00;
#endif
#if defined(GAMEVERSION_AOE10c) || defined(GAMEVERSION_ROR10c)
	return ExeAddr - 0x400000;
#endif
	return 0;
}

