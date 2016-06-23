#pragma once

/*
This file allows to define with which game version ROR_API/CustomROR will be compatible.
*/


// For ROR 1.0b version
#define GAMEVERSIONID_ROR10b 2

// For ROR 1.0c version
#define GAMEVERSIONID_ROR10c 3


// *** Choose game version to support ***
#define GAMEVERSION GAMEVERSIONID_ROR10b





// Label for supported game version + other useful macros
#if GAMEVERSION == GAMEVERSIONID_ROR10b
#define GAMEVERSION_TEXT "ROR 1.0b"
#define GAMEVERSION_10b
#endif
#if GAMEVERSION == GAMEVERSIONID_ROR10c
#define GAMEVERSION_TEXT "ROR 1.0c"
#define GAMEVERSION_10c
#endif
