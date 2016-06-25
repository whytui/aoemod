#pragma once

/*
This file allows to define with which game version CustomROR will be compatible.
*/

// Note: game version IDs must remain consecutive, starting at 1.

#define GAMEVERSIONID_AOE10a 1 // First official release
#define GAMEVERSIONID_AOE10b 2 // Delivered with ROR extension
#define GAMEVERSIONID_AOE10c 3 // With patch 1.0c

// For ROR 1.0b version (the one delivered with ROR extension)
#define GAMEVERSIONID_ROR10b 4

// For ROR 1.0c version (patch 1.0c)
#define GAMEVERSIONID_ROR10c 5

// Other
#define GAMEVERSIONID_UPATCH_BETA 6
#define GAMEVERSIONID_UPATCH_1_1_HD 7

// List of supported (recognized) versions
// If updated, please update AOEFileVersionLabel in AOE_const_global.h too
enum AOE_FILE_VERSION {
	AOE_INVALID_FILE = 0,
	// For vanilla AOE (empires.exe)
	AOE_VERSION_AOE1_0A = GAMEVERSIONID_AOE10a, // AOE first release
	AOE_VERSION_AOE1_0B = GAMEVERSIONID_AOE10b, // AOE version delivered with ROR extension
	AOE_VERSION_AOE1_0C = GAMEVERSIONID_AOE10c, // AOE with patch 1.0c
	// ROR versions
	AOE_VERSION_1_0B = GAMEVERSIONID_ROR10b, // ROR first release
	AOE_VERSION_1_0C = GAMEVERSIONID_ROR10c, // ROR with patch 1.0c
	AOE_VERSION_UPATCH_BETA = GAMEVERSIONID_UPATCH_BETA, // ROR
	AOE_VERSION_UPATCH_1_1_HD = GAMEVERSIONID_UPATCH_1_1_HD, // ROR
	AOE_VERSION_COUNT
};






// ********************************************************************
// *** Choose game version to support (see numeric constants above) ***
#define GAMEVERSION GAMEVERSIONID_ROR10c
// ********************************************************************








// Returns the game version that corresponds to current build version
static const AOE_FILE_VERSION GetBuildVersion() {
	return (AOE_FILE_VERSION)GAMEVERSION;
}



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
