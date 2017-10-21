#pragma once
#include <Windows.h>
#include "../../Common/include/macroUtil.h"

#define VERSION_MAJOR     3
#define VERSION_MINOR     0
#define VERSION_REVISION  0
#define VERSION_BUILD     1
#define VER_FILE_VERSION     VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define VER_FILE_VERSION_STR MACRO_TO_STRING(VERSION_MAJOR)        \
                             "." MACRO_TO_STRING(VERSION_MINOR)    \
                             "." MACRO_TO_STRING(VERSION_REVISION) \
                             "." MACRO_TO_STRING(VERSION_BUILD)
#define VER_COMMENTS      "This program is free to use. If you use this tool to develop your programs/mods, please clearly mention it and provide a link if possible."
#define VER_DESCRIPTION   "Tool to customize your AoE: Rise of Rome game."
#define VER_PROGRAM_NAME  "RockNRor admin"
#define VER_COPYRIGHT     "AOEModder"

