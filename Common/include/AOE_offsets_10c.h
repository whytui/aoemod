
#pragma once

/* 
 * This file contains various offsets/values that are specific to 1.0c ROR executable
*/
namespace AOE_OFFSETS_10C
{
	// Some useful raw addresses
	static const unsigned long int ADDR_EXE_MIN = 0x401000;
	static const unsigned long int ADDR_EXE_MAX = 0x536FFF;
	static const unsigned long int ADDR_THIS_CODE_MIN = 0x537000;
	static const unsigned long int ADDR_THIS_CODE_MAX = 0x541FFF;
	static const unsigned long int ADDR_RDATA_MIN = 0x542000;
	static const unsigned long int ADDR_RDATA_MAX = 0x553FFF;
	static const unsigned long int ADDR_DATA_MIN = 0x554000; // game global variables
	static const unsigned long int ADDR_DATA_MAX = 0x7D4FFF; // game global variables
	static const unsigned long int ADDR_THIS_DAT_MIN = 0x7D5000;
	static const unsigned long int ADDR_THIS_DAT_MAX = 0x7D5FFF;
	static const unsigned long int ADDR_INF32_DAT_MIN = 0x7D6000;
	static const unsigned long int ADDR_INF32_DAT_MAX = 0x7E0FFF;
	static const unsigned long int ADDR_RESOURCES_MIN = 0x7E1000;
	static const unsigned long int ADDR_RESOURCES_MAX = 0x7E1FFF;
	static const unsigned long int ADDR_CHECKSUM_MIN = 0x005423F0;
	static const unsigned long int ADDR_CHECKSUM_MAX = 0x0054AA60;
	static const unsigned long int ADDR_VAR_GAME_GLOBAL_STRUCT = 0x6A1808;
	static const unsigned long int ADDR_VAR_GAME_SETTINGS_STRUCT = 0x580E38;
	static const unsigned long int ADDR_VAR_ACTIVE_UI_STRUCT = 0x005830F4;
	static const unsigned long int ADDR_VAR_CURRENT_UI_OBJECT = 0x5830F4; // redundant - to remove
	static const unsigned long int ADDR_VAR_INTERFAC_DRS_NAME = 0x0055CA5C;
	static const unsigned long int ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS = 0x7C04A0;
	static const unsigned long int ADDR_STRUCT_UNKNOWN_MAP_INFO_7D2058 = 0x7D2058;
	static const unsigned long int ADDR_MAP_VISIBILITY_INFO = 0x7D205C;
	static const unsigned long int ADDR_MP_COMM_STRUCT = 0x580DA8;
	static const unsigned long int *UNKNOWN_ARRAY_6A18C0 = (unsigned long int *)0x6A18C0; // Size = 0x100 elements = 0x400 bytes

	// Game executable interface procedure address (ROR_API call)
	static const unsigned long int ROR_API_GAME_PROC_ADDR = 0x419574;
};


// Useful function to control checksums on unknown structs
static bool isAValidRORChecksum(unsigned long int checksum){
	// Not completely certain of the interval, but seems quite good.
	return (checksum >= AOE_OFFSETS_10C::ADDR_CHECKSUM_MIN) && (checksum < AOE_OFFSETS_10C::ADDR_CHECKSUM_MAX) &&
		((checksum & 0x3) == 0); // 2 lower bits are 0 means it's a multiple of 4. (checksums values are dword-aligned values).
}

// Returns 0 if offset is invalid.
static unsigned long int AOE_FileOffsetToExeAddr(unsigned long int fileOffset) {
	if ((fileOffset <= 0) || (fileOffset + 0x400000 > AOE_OFFSETS_10C::ADDR_DATA_MAX)) {
		return 0;
	}
	return fileOffset + 0x400000;
}

// Returns 0 if address is invalid.
static unsigned long int AOE_ExeAddrToFileOffset(unsigned long int ExeAddr) {
	if ((ExeAddr < AOE_OFFSETS_10C::ADDR_EXE_MIN) || (ExeAddr > AOE_OFFSETS_10C::ADDR_EXE_MAX)) {
		return 0;
	}
	return ExeAddr - 0x400000;
}

