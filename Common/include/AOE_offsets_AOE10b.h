
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

	/* The values below are specific to Age of Empires 1.0b (patch 1.0b). NOT Rise of Rome ! */


	// Some useful raw addresses

	// Technical (EXE)
	static const unsigned long int ADDR_FILE_EXE_MIN = 0x400;
	static const unsigned long int ADDR_FILE_EXE_MAX = 0x1411FF;
	static const unsigned long int ADDR_EXE_MIN = 0x401000;
	static const unsigned long int ADDR_EXE_MAX = 0x541FFF;
	static const unsigned long int ADDR_THIS_CODE_MIN = 0x542000;
	static const unsigned long int ADDR_THIS_CODE_MAX = 0x54CFFF;
	static const unsigned long int ADDR_RDATA_MIN = 0x54D000;
	static const unsigned long int ADDR_RDATA_MAX = 0x55EFFF;
	static const unsigned long int ADDR_DATA_MIN = 0x55F000; // game global variables
	static const unsigned long int ADDR_DATA_MAX = 0x7E0FFF; // game global variables
	//0x7E1000 : imports (.idata) size 0x2000
	static const unsigned long int ADDR_THIS_DAT_MIN = 0x7E3000;
	static const unsigned long int ADDR_THIS_DAT_MAX = 0x7E3FFF;
	static const unsigned long int ADDR_INF32_DAT_MIN = 0x7E4000;
	static const unsigned long int ADDR_INF32_DAT_MAX = 0x7EEFFF;
	static const unsigned long int ADDR_RESOURCES_MIN = 0x7EF000;
	static const unsigned long int ADDR_RESOURCES_MAX = 0x7EFFFF;
	// 0x7F0000 : .reloc
	static const unsigned long int ADDR_CHECKSUM_MIN = 0x54D0B8;
	static const unsigned long int ADDR_CHECKSUM_MAX = 0x555D18;

	// Game objects/variables...
	static const unsigned long int ADDR_VAR_GAME_SETTINGS_STRUCT = 0x5681D8;
	static const unsigned long int ADDR_VAR_PANEL_SYSTEM = 0x56D5F8; // Base point for managing screens, popups = TPanelSystem
	static const unsigned long int ADDR_VAR_INTERFAC_DRS_NAME = 0x564FB4;
	//static const unsigned long int ADDR_VAR_EMPIRES_DAT_PATH = ; // data2\empires.dat
	static const unsigned long int ADDR_VAR_HINST_LANGUAGE_DLL = 0x7DDF84;
	static const unsigned long int ADDR_VAR_HINST_LANGUAGEX_DLL = 0; // does not exist in this version
	static const unsigned long int ADDR_DRS_LINK_FIRST_ELEM = 0x562760; // type= STRUCT_DRS_FILE_LINK*
	static const unsigned long int ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS = 0x580E30;
	static const unsigned long int ADDR_MP_COMM_STRUCT = 0x7DDFA4;
	static const unsigned long int ADDR_ELEMCOUNT_TEMP_NEARBY_UNITS_PER_DIPLVALUE = 0x580E80; // cf 0x4515E0. Array of 5 ints (elem count in ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE) indexed by PLAYER_DIPLOMACY_VALUES (0-4)
	static const unsigned long int ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE = 0x580E98; // array of 5 arrays of STRUCT_NEARBY_UNIT_INFO indexed by PLAYER_DIPLOMACY_VALUES (0-4)
	static const unsigned long int ADDR_VAR_STRUCT_VISIBLE_UNITS_HELPER = 0x565C18;
	static const unsigned long int ADDR_MAP_VISIBILITY_INFO = 0x56CFF8;
	static const unsigned long int ADDR_GAME_ACTIONS_ALLOWED = 0x5620A4;
	static const long int *AOE_CURRENT_OFFSET_IN_FILE_DESERIALIZATION = (long int *)0x56F514; // Pointer to the variable that contains offset of current position in file
	static const unsigned long int *AOE_AITYPE_INTERACTION_MASK = (unsigned long int *)0; //TODO
	static const unsigned long int AOE_VAR_CURSORS_INFO = 0; // Pointer to structure about cursors
	static const unsigned long int *AOE_LNK_TIMEGETTIME = 0;
	static const unsigned long int **ADDR_VAR_REGISTRY_OBJECT_DEFAULT_REG_PATH = (const unsigned long int **)0; // Contains a TRegistry object pointer.
	static const unsigned long int **ADDR_VAR_TCHAT_OBJECT = (const unsigned long int **)0;
	static unsigned long int *AOE_VAR_F5_DEBUG_INFO_TYPE = (unsigned long int *)0; // Variable =0/1 indicating current type of "F5 debug info".
	static const unsigned long int *AOE_VAR_FOUND_UNIT_ID_UNDER_MOUSE = (const unsigned long int*)0;
	static const short int *AOE_VAR_FOUND_UNIT_AI_TYPE_UNDER_MOUSE = (const short int*)0;
	

	// Game executable interface procedure address (ROR_API call)
	static const unsigned long int ROR_API_GAME_PROC_ADDR = 0x4E9003;
	static const unsigned long int AOE_ROR_API_FIRST_CALL_RETURN_ADDRESS = 0x4E901B; // to verify




	/***** Game methods Call addresses *****/
	static const unsigned long int EXEADDR_AddDrsFile = 0x45D020;
	static const unsigned long int EXEADDR_SlpInfo_ctor = 0x46B800;





};


