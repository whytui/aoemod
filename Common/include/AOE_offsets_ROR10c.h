
#pragma once

#include <gameVersion.h>

/* 
 * This file contains various offsets/values that are specific to each AOE/ROR executable
 * Please read README.txt first.
 * Reading/using information from this file implies sharing with the community
 * ALL your findings about AOE/ROR structures
 * Please share knowledge for better modding experience !
 */
namespace AOE_OFFSETS
{

	/* The values below are specific to Rise of Rome 1.0a (patch 1.0c) */


	// Some useful raw addresses

	// Technical (EXE)
	static const unsigned long int ADDR_FILE_EXE_MIN = 0x1000;
	//static const unsigned long int ADDR_FILE_EXE_MAX = ;
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

	// Game objects/variables...
	//static const unsigned long int ADDR_VAR_GAME_GLOBAL_STRUCT = 0x6A1808; // Do not use it
	static const unsigned long int ADDR_VAR_GAME_SETTINGS_STRUCT = 0x580E38;
	static const unsigned long int ADDR_VAR_PANEL_SYSTEM = 0x005830E8; // Base point for managing screens, popups = TPanelSystem
	static const unsigned long int ADDR_VAR_INTERFAC_DRS_NAME = 0x0055CA5C;
	static const unsigned long int ADDR_VAR_EMPIRES_DAT_PATH = 0x005577C8; // data2\empires.dat
	static const unsigned long int ADDR_VAR_HINST_LANGUAGE_DLL = 0x580D8C;
	static const unsigned long int ADDR_VAR_HINST_LANGUAGEX_DLL = 0x7C0634;
	static const unsigned long int ADDR_DRS_LINK_FIRST_ELEM = 0x7BFAC4; // type= STRUCT_DRS_FILE_LINK*
	static const unsigned long int ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS = 0x7C04A0;
	static const unsigned long int ADDR_MP_COMM_STRUCT = 0x580DA8;
	static const unsigned long int ADDR_ELEMCOUNT_TEMP_NEARBY_UNITS_PER_DIPLVALUE = 0x7D2024;
	static const unsigned long int ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE = 0x7D2038; // array of ptr to arrays of STRUCT_NEARBY_UNIT_INFO
	static const unsigned long int ADDR_STRUCT_UNKNOWN_MAP_INFO_7D2058 = 0x7D2058;
	static const unsigned long int ADDR_MAP_VISIBILITY_INFO = 0x7D205C;
	static const unsigned long int ADDR_GAME_ACTIONS_ALLOWED = 0x55873C;
	static const unsigned long int ADDR_GLOBAL_UNIT_POINTERS_ARRAY = 0x7D2054; // Variable = pointer to array of unit pointers (unit**). Also found in game global structure.
	static const long int *AOE_CURRENT_OFFSET_IN_FILE_DESERIALIZATION = (long int *)0x7BFAD8; // Pointer to the variable that contains offset of current position in file
	static const unsigned char **AOE_TEMP_SERIALIZATION_BUFFER = (const unsigned char **)0x7BFADC;
	static const unsigned char **AOE_CURRENT_POSITION_IN_FILE_DESERIALIZATION = (const unsigned char **)0x7BFAE8; // Pointer to current position in file content buffer
	static const long int *AOE_CURRENTLY_OPENED_FILE_SIZE = (long int *)0x7BFAEC;
	static const unsigned char *AOE_CURRENTLY_OPENED_FILE_BUFFER = (const unsigned char*)0x7BFAF0;
	static const unsigned long int *AOE_AITYPE_INTERACTION_MASK = (unsigned long int *)0x55BDC8; // Array of 40 mask values for unit interactions (for each AI unit type - "class" in AGE3)
	static const unsigned long int AOE_VAR_CURSORS_INFO = 0x582EDC; // Pointer to structure about cursors
	static const unsigned long int *AOE_LNK_TIMEGETTIME = (unsigned long int *)0x5423BC;
	static const unsigned long int **ADDR_VAR_REGISTRY_OBJECT_DEFAULT_REG_PATH = (const unsigned long int **)0x580DC0; // Contains a TRegistry object pointer.
	static const unsigned long int **ADDR_VAR_TCHAT_OBJECT = (const unsigned long int **)0x580DA0;
	static unsigned long int *AOE_VAR_F5_DEBUG_INFO_TYPE = (unsigned long int *)0x7C0648; // Variable =0/1 indicating current type of "F5 debug info".
	static const unsigned long int *UNKNOWN_ARRAY_6A18C0 = (unsigned long int *)0x6A18C0; // Size = 0x100 elements = 0x400 bytes



	// Game executable interface procedure address (ROR_API call)
	static const unsigned long int ROR_API_GAME_PROC_ADDR = 0x419574;
	static const unsigned long int AOE_ROR_API_FIRST_CALL_RETURN_ADDRESS = 0x443713;





	/***** Game methods Call addresses *****/
	static const unsigned long int EXEADDR_AddDrsFile = 0x46AFC0;
	static const unsigned long int EXEADDR_SlpInfo_ctor = 0x49F5F0;






};


