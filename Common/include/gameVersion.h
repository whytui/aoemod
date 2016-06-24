#pragma once

/*
This file allows to define with which game version CustomROR will be compatible.
*/

// For vanilla AOE (empires.exe)
#define GAMEVERSIONID_AOE10a 1 // First official release
#define GAMEVERSIONID_AOE10b 2 // Delivered with ROR extension
#define GAMEVERSIONID_AOE10c 3 // With patch 1.0c

// For ROR 1.0b version (the one delivered with ROR extension)
#define GAMEVERSIONID_ROR10b 10

// For ROR 1.0c version (patch 1.0c)
#define GAMEVERSIONID_ROR10c 11


// *** Choose game version to support ***
#define GAMEVERSION GAMEVERSIONID_ROR10b





// Label for supported game version + other useful macros
#if GAMEVERSION == GAMEVERSIONID_AOE10a
#define GAMEVERSION_TEXT "AOE 1.0a"
#define GAMEVERSION_AOE10a
#endif
#if GAMEVERSION == GAMEVERSIONID_AOE10b
#define GAMEVERSION_TEXT "AOE 1.0b"
#define GAMEVERSION_AOE10b
#endif
#if GAMEVERSION == GAMEVERSIONID_AOE10c
#define GAMEVERSION_TEXT "AOE 1.0c"
#define GAMEVERSION_AOE10c
#endif
#if GAMEVERSION == GAMEVERSIONID_ROR10b
#define GAMEVERSION_TEXT "ROR 1.0b"
#define GAMEVERSION_ROR10b
#endif
#if GAMEVERSION == GAMEVERSIONID_ROR10c
#define GAMEVERSION_TEXT "ROR 1.0c"
#define GAMEVERSION_ROR10c
#endif
