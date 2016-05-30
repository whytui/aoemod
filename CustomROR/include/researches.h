#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <ROR_structures.h>
#include <ROR_global_variables.h>
#include <AOE_const_functional.h>
#include "traceMessage.h"
#include "language.h"

// Disable all impossible researches for all players.
// An impossible research is a research that is waiting for requirements, including ones that can never be satisfied.
// Example in original game: irrigation for persian, armored elephant for yamato, etc.
int DisableImpossibleResearches();

// Disable all impossible researches for a specific player.
// An impossible research is a research that is waiting for requirements, including ones that can never be satisfied.
// Example in original game: irrigation for persian, armored elephant for yamato, etc.
int DisablePlayerImpossibleResearches(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);
