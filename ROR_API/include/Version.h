#pragma once
#include <Windows.h>
#include "../../Common/include/gameVersion.h"
//#include <gameVersion.h> // does not work because it's used by customROR.rc !!

#define MACRO_TO_STRING_INTERNAL(s) #s
#define MACRO_TO_STRING(s) MACRO_TO_STRING_INTERNAL(s)
#define VERSION_MAJOR     1
#define VERSION_MINOR     0
#define VERSION_REVISION  0
#define VERSION_BUILD     7
#define VER_FILE_VERSION     VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define VER_FILE_VERSION_STR MACRO_TO_STRING(VERSION_MAJOR)        \
                             "." MACRO_TO_STRING(VERSION_MINOR)    \
                             "." MACRO_TO_STRING(VERSION_REVISION) \
                             "." MACRO_TO_STRING(VERSION_BUILD)
#define VER_COMMENTS      "This program is free to use. If you use this tool to develop your programs/mods, please clearly mention it and provide a link if possible."
#define VER_DESCRIPTION   "Plugin manager for Age of Empires: " \
	GAMEVERSION_TEXT
#define VER_PROGRAM_NAME  "ROR_API"
#define VER_COPYRIGHT     "Chab"

