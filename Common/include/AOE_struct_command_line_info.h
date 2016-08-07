
#pragma once


/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	// This structure is contained in stack. Maybe (probably) some fields should never be used here !
	// Size = 0x143C. "constructor" = 4436F0.
	class STRUCT_COMMAND_LINE_INFO {
	public:
		char applicationName[0x1C];
		char *unknown_01C; // pointer to stack value, inside a string ?
		// 0x20
		char *unknown_020; // pointer to stack value, inside a string ?
		unsigned long int unknown_024;
		unsigned long int unknown_028; // a pointer
		unsigned long int stack_SE_handler; // a pointer to executable code
		// 0x30
		char unknown_030[0x1F8 - 0x30];
		char relativePath_empires_dat[0x14]; // +1F8: (included) string  data2/empires.dat, copied from ADDR_VAR_EMPIRES_DAT_PATH and used in 0x501211.
		unsigned long int unknown_20C; // a pointer ?
		// 0x210
		char unknown_210[0x408 - 0x210];
		long int CDCheckFailed; // +408.
		unsigned long int unknown_40C;
		// 0x410
		long int hInstance; // +410 ; GetModuleHandle(NULL)
		char unknown_414[0x518 - 0x414];
		char unknown_518[0x890 - 0x518]; // Command-line arguments string ?
		// 0x890
		long int noStartupFlag; // NOSTARTUP command line flag
		unsigned long int unknown_894;
		unsigned long int unknown_898;
		unsigned long int unknown_89C;
		long int systemMemoryFlag; // SYSTEMMEMORY command line flag
		long int noMusicFlag; // +8A4. NOMUSIC command line flag ?
		long int noSoundFlag; // +8A8. NOSOUND command line flag
		long int isAudioCD; // +8AC. No Autodetect (force). To confirm
		unsigned long int unknown_8B0; // MIDI related ?
		long int isMidiMusic; // +8B4. Force midi. To confirm
		unsigned long int unknown_8B8; // MIDI related ?
		unsigned long int unknown_8BC;
		unsigned long int unknown_8C0;
		unsigned long int unknown_8C4;
		long int scrollSpeed; // +8C8 ; From 10 to 200
		unsigned long int unknown_8CC;
		unsigned long int unknown_8D0; // Related to scroll speed ?
		unsigned long int unknown_8D4;
		unsigned long int unknown_8D8;
		unsigned long int mouseButtonsStyle; // +8DC. 1 or 2 buttons.
		long int screenSizeX; // +8E0
		long int screenSizeY; // +8E4
		// B12 : "savegame\"
		// C17 : "scenario\"
		// D1C : "campaign\"
		// E21 : "sound\"
		// 1130 : "avi\"
		char unknown_8E8[0x143C - 0x8E8];
	};
	static_assert(sizeof(STRUCT_COMMAND_LINE_INFO) == 0x143C, "STRUCT_COMMAND_LINE_INFO size");

}
