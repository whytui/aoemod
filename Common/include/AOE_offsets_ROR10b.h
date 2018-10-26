
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

	/* The values below are specific to Rise of Rome 1.0 (patch 1.0b) */


	// Some useful raw addresses

	// Technical (EXE)
	static const unsigned long int ADDR_FILE_EXE_MIN = 0x400;
	static const unsigned long int ADDR_FILE_EXE_MAX = 0x13A5FF;
	static const unsigned long int ADDR_EXE_MIN = 0x401000;
	static const unsigned long int ADDR_EXE_MAX = 0x53BFFF;
	static const unsigned long int ADDR_THIS_CODE_MIN = 0x53C000;
	static const unsigned long int ADDR_THIS_CODE_MAX = 0x546C00;
	static const unsigned long int ADDR_RDATA_MIN = 0x547000;
	static const unsigned long int ADDR_RDATA_MAX = 0x556FFF;
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

	// Game objects/variables...
	//static const unsigned long int ADDR_VAR_GAME_GLOBAL_STRUCT = 0x6A6858; // Do not use it
	static const unsigned long int ADDR_VAR_GAME_SETTINGS_STRUCT = 0x585E88;
	static const unsigned long int ADDR_VAR_PANEL_SYSTEM = 0x00588138; // Base point for managing screens, popups = TPanelSystem
	static const unsigned long int ADDR_VAR_UNKNOWN_UI_MAIN_INFO = 0x588138; // some included struct
	static const unsigned long int ADDR_VAR_INTERFAC_DRS_NAME = 0x55FA5C;
	//static const unsigned long int ADDR_VAR_EMPIRES_DAT_PATH = ; // data2\empires.dat
	static const unsigned long int ADDR_VAR_HINST_LANGUAGE_DLL = 0x585DDC;
	static const unsigned long int ADDR_VAR_HINST_LANGUAGEX_DLL = 0x7C56A0;
	static const unsigned long int ADDR_DRS_LINK_FIRST_ELEM = 0x7C4B24; // type= STRUCT_DRS_FILE_LINK*
	static const unsigned long int ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS = 0x7C5508;
	static const unsigned long int ADDR_MP_COMM_STRUCT = 0x585DF8;
	static const unsigned long int ADDR_ELEMCOUNT_TEMP_NEARBY_UNITS_PER_DIPLVALUE = 0x7D7088; // array of 5 ints (elem count in ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE) indexed by PLAYER_DIPLOMACY_VALUES (0-4)
	static const unsigned long int ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE = 0x7D70A0; // array of 5 arrays of STRUCT_NEARBY_UNIT_INFO indexed by PLAYER_DIPLOMACY_VALUES (0-4)
	static const unsigned long int ADDR_VAR_STRUCT_VISIBLE_UNITS_HELPER = 0x7D70C0;
	static const unsigned long int ADDR_MAP_VISIBILITY_INFO = 0x7D70C8;
	static const unsigned long int ADDR_GAME_ACTIONS_ALLOWED = 0x55B734;
	static const long int *AOE_CURRENT_OFFSET_IN_FILE_DESERIALIZATION = (long int *)0x7C4B38; // Pointer to the variable that contains offset of current position in file
	static const unsigned long int *AOE_AITYPE_INTERACTION_MASK = (unsigned long int *)0; // TODO
	static const unsigned long int AOE_VAR_CURSORS_INFO = 0; // Pointer to structure about cursors
	static const unsigned long int *AOE_LNK_TIMEGETTIME = (unsigned long int *)0x7DA8CC;
	static const unsigned long int **ADDR_VAR_REGISTRY_OBJECT_DEFAULT_REG_PATH = (const unsigned long int **)0x585E10; // Contains a TRegistry object pointer.
	static const unsigned long int **ADDR_VAR_TCHAT_OBJECT = (const unsigned long int **)0x585DF0;
	static unsigned long int *AOE_VAR_F5_DEBUG_INFO_TYPE = (unsigned long int *)0; // Variable =0/1 indicating current type of "F5 debug info".
	static const unsigned long int *AOE_VAR_FOUND_UNIT_ID_UNDER_MOUSE = (const unsigned long int*)0;
	static const short int *AOE_VAR_FOUND_UNIT_AI_TYPE_UNDER_MOUSE = (const short int*)0;


	// Game executable interface procedure address (ROR_API call)
	static const unsigned long int ROR_API_GAME_PROC_ADDR = 0x419C43;
	static const unsigned long int AOE_ROR_API_FIRST_CALL_RETURN_ADDRESS = 0x419C5B;






	/***** Game methods Call addresses *****/
	static const unsigned long int EXEADDR_AddDrsFile = 0x46C190;
	static const unsigned long int EXEADDR_SlpInfo_ctor = 0x4A0B80;




};


