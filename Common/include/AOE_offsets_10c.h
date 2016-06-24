
#pragma once

#include <gameVersion.h>

/* 
 * This file contains various offsets/values that are specific to 1.0c ROR executable
*/
namespace AOE_OFFSETS_10C
{
	// Some useful raw addresses
#ifdef GAMEVERSION_10b
	static const unsigned long int ADDR_FILE_EXE_MIN = 0x400;
	static const unsigned long int ADDR_FILE_EXE_MAX = 0x13A5FF;
	static const unsigned long int ADDR_EXE_MIN = 0x401000;
	static const unsigned long int ADDR_EXE_MAX = 0x53BFFF;
	static const unsigned long int ADDR_THIS_CODE_MIN = 0x53C000;
	static const unsigned long int ADDR_THIS_CODE_MAX = 0x546C00;
	static const unsigned long int ADDR_RDATA_MIN = 0x547000;
	static const unsigned long int ADDR_RDATA_MAX = 0x557FFF;
	static const unsigned long int ADDR_DATA_MIN = 0x557000; // game global variables
	static const unsigned long int ADDR_DATA_MAX = 0x7D9FFF; // game global variables
	static const unsigned long int ADDR_THIS_DAT_MIN = 0x7DC000;
	static const unsigned long int ADDR_THIS_DAT_MAX = 0x7DCFFF;
	static const unsigned long int ADDR_INF32_DAT_MIN = 0x7DD000;
	static const unsigned long int ADDR_INF32_DAT_MAX = 0x7E7FFF;
	static const unsigned long int ADDR_RESOURCES_MIN = 0x7E8000;
	static const unsigned long int ADDR_RESOURCES_MAX = 0x7E8FFF;
	static const unsigned long int ADDR_CHECKSUM_MIN = 0x00547000;
	static const unsigned long int ADDR_CHECKSUM_MAX = 0x0054FF60; // unsure
	static const unsigned long int ADDR_VAR_GAME_GLOBAL_STRUCT = 0x6A6858;
#endif
#ifdef GAMEVERSION_10c
	static const unsigned long int ADDR_EXE= 0x1000;
	//static const unsigned long int ADDR_EXE_CODE_MAX = ;
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
#endif
	static const unsigned long int ADDR_VAR_GAME_SETTINGS_STRUCT = 0x580E38;
	static const unsigned long int ADDR_VAR_ACTIVE_UI_STRUCT = 0x005830F4;
	static const unsigned long int ADDR_VAR_CURRENT_UI_OBJECT = 0x5830F4; // redundant - to remove
	static const unsigned long int ADDR_VAR_INTERFAC_DRS_NAME = 0x0055CA5C;
	static const unsigned long int ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS = 0x7C04A0;
	static const unsigned long int ADDR_ELEMCOUNT_TEMP_NEARBY_UNITS_PER_DIPLVALUE = 0x7D2024;
	static const unsigned long int ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE = 0x7D2038; // array of ptr to arrays of STRUCT_NEARBY_UNIT_INFO
	static const unsigned long int ADDR_STRUCT_UNKNOWN_MAP_INFO_7D2058 = 0x7D2058;
	static const unsigned long int ADDR_MAP_VISIBILITY_INFO = 0x7D205C;
	static const unsigned long int ADDR_MP_COMM_STRUCT = 0x580DA8;
	static const unsigned long int *UNKNOWN_ARRAY_6A18C0 = (unsigned long int *)0x6A18C0; // Size = 0x100 elements = 0x400 bytes
	static const unsigned long int ADDR_GAME_ACTIONS_ALLOWED = 0x55873C;
	static const unsigned long int ADDR_DRS_LINK_FIRST_ELEM = 0x7BFAC4; // type= STRUCT_DRS_FILE_LINK*

	// Game executable interface procedure address (ROR_API call)
#ifdef GAMEVERSION_10b
	static const unsigned long int ROR_API_GAME_PROC_ADDR = 0x419C43;
	static const unsigned long int AOE_ROR_API_FIRST_CALL_RETURN_ADDRESS = 0x419C5B;
#endif
#ifdef GAMEVERSION_10c
	static const unsigned long int ROR_API_GAME_PROC_ADDR = 0x419574;
	static const unsigned long int AOE_ROR_API_FIRST_CALL_RETURN_ADDRESS = 0x443713;
#endif

};


// Useful function to control checksums on unknown structs
static bool isAValidRORChecksum(unsigned long int checksum){
	// Not completely certain of the interval, but seems quite good.
	return (checksum >= AOE_OFFSETS_10C::ADDR_CHECKSUM_MIN) && (checksum < AOE_OFFSETS_10C::ADDR_CHECKSUM_MAX) &&
		((checksum & 0x3) == 0); // 2 lower bits are 0 means it's a multiple of 4. (checksums values are dword-aligned values).
}

// Returns 0 if offset is invalid.
static unsigned long int AOE_FileOffsetToExeAddr(unsigned long int fileOffset) {
	if (fileOffset < AOE_OFFSETS_10C::ADDR_FILE_EXE_MIN) { return 0; }
	unsigned long int result = fileOffset;
#ifdef GAMEVERSION_10b
	// TODO: test
	if (fileOffset < AOE_OFFSETS_10C::ADDR_FILE_EXE_MAX) {
		result += 0x400c00;
	}
	if (fileOffset < (AOE_OFFSETS_10C::ADDR_THIS_CODE_MAX - 0x401a00)) {
		result += 0x401a00;
	}
	if (fileOffset < (AOE_OFFSETS_10C::ADDR_RDATA_MAX - 0x401E00)) {
		result += 0x401E00;
	}
	if (fileOffset < (AOE_OFFSETS_10C::ADDR_DATA_MAX - 0x402600)) {
		result += 0x402600;
	}
#endif
#ifdef GAMEVERSION_10c
	if (fileOffset < (AOE_OFFSETS_10C::ADDR_DATA_MAX - 0x400000)) {
		result += 0x400000;
	}
#endif
	return result;
}

// Returns 0 if address is invalid.
static unsigned long int AOE_ExeAddrToFileOffset(unsigned long int ExeAddr) {
	// TODO: only supports text (not THIS_COD, .rdata, .data)
	if ((ExeAddr < AOE_OFFSETS_10C::ADDR_EXE_MIN) || (ExeAddr > AOE_OFFSETS_10C::ADDR_EXE_MAX)) {
		return 0;
	}
#ifdef GAMEVERSION_10b
	return ExeAddr - 0x400C00;
#endif
#ifdef GAMEVERSION_10c
	return ExeAddr - 0x400000;
#endif
	return 0;
}

