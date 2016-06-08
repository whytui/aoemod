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

using namespace ROR_STRUCTURES_10C;
using namespace AOE_CONST_FUNC;

// Technologies to ignore for automatic technology adding/detection, because they have drawbacks or unused by AI
const short int LST_TECHS_TO_IGNORE[] = { CST_TCH_JIHAD, CST_TCH_MARTYRDOM };


// Disable all impossible researches for all players.
// An impossible research is a research that is waiting for requirements, including ones that can never be satisfied.
// Example in original game: irrigation for persian, armored elephant for yamato, etc.
int DisableImpossibleResearches();

// Disable all impossible researches for a specific player.
// An impossible research is a research that is waiting for requirements, including ones that can never be satisfied.
// Example in original game: irrigation for persian, armored elephant for yamato, etc.
int DisablePlayerImpossibleResearches(STRUCT_PLAYER *player);

// Returns true if technology has at least one effect on provided unit definition.
bool DoesTechAffectUnit(STRUCT_TECH_DEF *techDef, STRUCT_UNITDEF_BASE *unitDef);

// Finds all (non disabled) researches that affect a unit (definition)
// If ignoreUndesirableTechs==true, techs from LST_TECHS_TO_IGNORE are ignored (jihad, etc)
std::vector<short int> FindResearchesThatAffectUnit(STRUCT_PLAYER *player, long int unitDefId, bool ignoreUndesirableTechs);


// Get a technology name from languagex.dll or language.dll.
// Technologies don't really have a name, we use matching research to find it. Works in many cases, not all.
std::string GetTechnologyLocalizedName(short int techId);
