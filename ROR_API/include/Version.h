#pragma once
#include <Windows.h>
#include "../../Common/include/macroUtil.h"
#include "../../Common/include/gameVersion.h"
//#include <gameVersion.h> // does not work because it's used by ROR_API.rc !!

#define VERSION_MAJOR     1
#define VERSION_MINOR     0
#define VERSION_REVISION  1
#define VERSION_BUILD     9
#define VER_FILE_VERSION     VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define VER_FILE_VERSION_STR MACRO_TO_STRING(VERSION_MAJOR)        \
                             "." MACRO_TO_STRING(VERSION_MINOR)    \
                             "." MACRO_TO_STRING(VERSION_REVISION) \
                             "." MACRO_TO_STRING(VERSION_BUILD)
#define VER_COMMENTS      "This program is free to use. If you use this tool to develop your programs/mods, please clearly mention it and provide a link if possible."
#define VER_DESCRIPTION   "Plugin manager for Age of Empires/Rise of Rome"
#define VER_PROGRAM_NAME  "ROR_API"
#define VER_COPYRIGHT     "AoeModder"

