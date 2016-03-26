
#pragma once

/* 
 * This file contains various offsets/values that are specific to 1.0c ROR executable
*/
namespace AOE_OFFSETS_10C
{
	// Some useful raw addresses
	static const long int ADDR_EXE_MIN = 0x401000;
	static const long int ADDR_EXE_MAX = 0x536FFF;
	static const long int ADDR_CHECKSUM_MIN = 0x005423F0;
	static const long int ADDR_CHECKSUM_MAX = 0x0054AA60;
	static const long int ADDR_VAR_GAME_GLOBAL_STRUCT = 0x6A1808;
	static const long int ADDR_VAR_GAME_SETTINGS_STRUCT = 0x580E38;
	static const long int ADDR_VAR_ACTIVE_UI_STRUCT = 0x005830F4;
	static const unsigned long int ADDR_VAR_CURRENT_UI_OBJECT = 0x5830F4; // redundant - to remove
	static const long int ADDR_VAR_INTERFAC_DRS_NAME = 0x0055CA5C;
	static const long int ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS = 0x7C04A0;
	static const long int ADDR_STRUCT_UNKNOWN_MAP_INFO_7D2058 = 0x7D2058;
	static const long int ADDR_MAP_VISIBILITY_INFO = 0x7D205C;
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
