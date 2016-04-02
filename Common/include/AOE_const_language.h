#include <tchar.h>
#include <string>

#pragma once

// This file contains DLL IDs for texts that are used in game o custom treatments

const static unsigned long int LANG_ID_OK = 0xFA1;
const static unsigned long int LANG_ID_CANCEL = 0xFA2;
const static unsigned long int LANG_ID_PLAYERX_BASE = 10340; // LANG_ID_PLAYERX_BASE + 1-8 = player1-player8
const static unsigned long int LANG_ID_PLAYER_NAME_BY_CIV_BASE = 0x1130; // LANG_ID_PLAYER_NAME_BY_CIV_BASE + 10*civ+(1->9) = a tribe name
const static unsigned long int LANG_ID_PERSONALITY = 10258;
const static unsigned long int LANG_ID_STRATEGY = 10259;
const static unsigned long int LANG_ID_GAME_OPTIONS = 9274; // 0x243A. For game menu options entry
const static unsigned long int LANG_ID_OPTIONS = 10017; // Just "options"
const static unsigned long int LANG_ID_AI_FILENAME_BASE = 26928; // 26928+i = filename for predefined stratgy #i. Total count 0x83=131. EXE uses index 158000+i, why ?
const static unsigned long int LANG_ID_AI_FILENAME_LAST_VALID = 27058; // Last valid AI filename
