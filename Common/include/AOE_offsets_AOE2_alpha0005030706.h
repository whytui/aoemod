
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

	/* The values below are specific to Age of Kings alpha 00.05.03.0706 */


	// Some useful raw addresses

	// Technical (EXE)
	static const unsigned long int ADDR_FILE_EXE_MIN = 0x400;
	static const unsigned long int ADDR_FILE_EXE_MAX = 0x19BBFF; // 0x19B6C0(virtualsize)+0x400(ptr rawdata)
	static const unsigned long int ADDR_EXE_MIN = 0x401000;
	static const unsigned long int ADDR_EXE_MAX = 0x59CFFF;
	static const unsigned long int ADDR_THIS_CODE_MIN = 0x59D000;
	static const unsigned long int ADDR_THIS_CODE_MAX = 0x59DFFF;
	static const unsigned long int ADDR_RDATA_MIN = 0x5A9000; // offset 0x1A7600 in file (shift 0x401A00)
	static const unsigned long int ADDR_RDATA_MAX = 0x5BDFFF;
	static const unsigned long int ADDR_DATA_MIN = 0x5BE000; // game global variables
	static const unsigned long int ADDR_DATA_MAX = 0x8CCFFF; // game global variables
	static const unsigned long int ADDR_THIS_DAT_MIN = 0x8CD000;
	static const unsigned long int ADDR_THIS_DAT_MAX = 0x8CDFFF;
	static const unsigned long int ADDR_INF32_DAT_MIN = 0x8CE000;
	static const unsigned long int ADDR_INF32_DAT_MAX = 0x8CEFFF;
	static const unsigned long int ADDR_RESOURCES_MIN = 0x8D9000;
	static const unsigned long int ADDR_RESOURCES_MAX = 0x8D9FFF;
	static const unsigned long int ADDR_CHECKSUM_MIN = 0x005B3A60; // completely unsure
	static const unsigned long int ADDR_CHECKSUM_MAX = 0x005B42CC; // completely unsure

	// Game objects/variables...
	static const unsigned long int ADDR_VAR_GAME_SETTINGS_STRUCT = 0x66F354;
	static const unsigned long int ADDR_VAR_PANEL_SYSTEM = 0x00672AC8; // TO CONFIRM // Base point for managing screens, popups = TPanelSystem
	static const unsigned long int ADDR_VAR_INTERFAC_DRS_NAME = 0; // TODO
	static const unsigned long int ADDR_VAR_EMPIRES_DAT_PATH = 0x56C148; // data2\empires.dat
	static const unsigned long int ADDR_VAR_HINST_LANGUAGE_DLL = 0; // TODO
	static const unsigned long int ADDR_VAR_HINST_LANGUAGEX_DLL = 0; // Not relevant
	static const unsigned long int ADDR_DRS_LINK_FIRST_ELEM = 0; // TODO // type= STRUCT_DRS_FILE_LINK*
	//static const unsigned long int ADDR_LOS_MAP = 0x7C0478; // Type?
	static const unsigned long int ADDR_STRUCT_TEMP_MAP_BUILD_LIKE_INFOS = 0; // TODO
	static const unsigned long int ADDR_MP_COMM_STRUCT = 0x66F2A0; // TO CONFIRM
	static const unsigned long int ADDR_ELEMCOUNT_TEMP_NEARBY_UNITS_PER_DIPLVALUE = 0; // TODO // array of 5 ints (elem count in ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE) indexed by PLAYER_DIPLOMACY_VALUES (0-4) "visibleUnitsListSize"
	static const unsigned long int ADDR_ARRAYS_TEMP_NEARBY_UNITS_PER_DIPLVALUE = 0; // TODO // array of 5 arrays of STRUCT_NEARBY_UNIT_INFO indexed by PLAYER_DIPLOMACY_VALUES (0-4)
	static const unsigned long int ADDR_VAR_STRUCT_VISIBLE_UNITS_HELPER = 0; // TODO
	static const unsigned long int ADDR_MAP_VISIBILITY_INFO = 0x8CAA10; // TO CONFIRM // "unified map offsets"
	static const unsigned long int ADDR_GAME_ACTIONS_ALLOWED = 0; // TODO
	//static const unsigned long int ADDR_GLOBAL_UNIT_POINTERS_ARRAY = 0x7D2054; // Variable = pointer to array of unit pointers (unit**). Also found in game global structure.
	//static const unsigned long int *AOE_AITYPE_INTERACTION_MASK = (unsigned long int *)0x55BDC8; // Array of 40 mask values for unit interactions (for each AI unit type - "class" in AGE3)
	static const unsigned long int AOE_VAR_CURSORS_INFO = 0; // TODO // Pointer to STRUCT_CURSOR_SLP_INFO = TMousePointer
	static const unsigned long int *AOE_LNK_TIMEGETTIME = (unsigned long int *)0x5A9414;
	static const unsigned long int **ADDR_VAR_REGISTRY_OBJECT_DEFAULT_REG_PATH = (const unsigned long int **)0; // TODO // Contains a TRegistry object pointer.
	static const unsigned long int **ADDR_VAR_TCHAT_OBJECT = (const unsigned long int **)0; // TODO
	static unsigned long int *AOE_VAR_F5_DEBUG_INFO_TYPE = (unsigned long int *)0; // TODO // Variable =0/1 indicating current type of "F5 debug info".
	//static const unsigned long int *UNKNOWN_ARRAY_6A18C0 = (unsigned long int *)0x6A18C0; // Size = 0x100 elements = 0x400 bytes
	static const unsigned long int *AOE_VAR_FOUND_UNIT_ID_UNDER_MOUSE = (const unsigned long int*)0; // TODO
	//static const short int *AOE_VAR_FOUND_UNIT_AI_TYPE_UNDER_MOUSE = (const short int*)0x7D1CFC;
	//static const unsigned long int ADDR_NUMBER_DIFFERENT_PLAYER_OBJECTS_VISIBLE = 0x561464;

	// Files.
	//static const unsigned long int ADDR_IO_ENABLE_COMPRESSION = ;
	//static const unsigned long int ADDR_IO_CURRENT_BUFFER = ;
	//static const unsigned long int ADDR_IO_FLAGS = ;
	static const long int *AOE_CURRENT_OFFSET_IN_FILE_DESERIALIZATION = (long int *)0; // Pointer to the variable that contains offset of current position in file. "Point"
	//static const unsigned char **AOE_TEMP_SERIALIZATION_BUFFER = (const unsigned char **)0; // "compression buffer"
	//static const unsigned long int ADDR_IO_BUFFERS = 0;
	//static const unsigned char **AOE_CURRENT_POSITION_IN_FILE_DESERIALIZATION = (const unsigned char **)0; // Pointer to current position in file content buffer. "Current"
	//static const long int *AOE_IO_CURRENTLY_OPENED_FILE_SIZE = (long int *)0;
	//static const unsigned char *AOE_CURRENTLY_OPENED_FILE_BUFFER = (const unsigned char*)0; // "File buffers" ?



	// Game executable interface procedure address (ROR_API call)
	static const unsigned long int ROR_API_GAME_PROC_ADDR = 0x59C6EA;
	// Return address of the very first call to ROR_API (which is used to for one-shot initializations)
	static const unsigned long int AOE_ROR_API_FIRST_CALL_RETURN_ADDRESS = 0x59C702;






	/***** Game methods Call addresses *****/
	static const unsigned long int EXEADDR_AddDrsFile = 0; // TODO
	static const unsigned long int EXEADDR_SlpInfo_ctor = 0; // TODO




};


