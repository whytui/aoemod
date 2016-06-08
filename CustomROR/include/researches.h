#pragma once

#include <string>
#include <vector>
#include <set>
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
// Ages and tech trees are in ignore list too
const short int LST_TECHS_TO_IGNORE[] = { CST_TCH_STONE_AGE, CST_TCH_TOOL_AGE, CST_TCH_BRONZE_AGE, CST_TCH_IRON_AGE,
CST_TCH_TECH_TREE_ALL_TECHS, CST_TCH_TECH_TREE_ASSYRIAN, CST_TCH_TECH_TREE_BABYLONIAN, CST_TCH_TECH_TREE_CARTHAGE,
CST_TCH_TECH_TREE_CHOSON, CST_TCH_TECH_TREE_EGYPTIAN, CST_TCH_TECH_TREE_GREEK, CST_TCH_TECH_TREE_HITTITE,
CST_TCH_TECH_TREE_MACEDONIAN, CST_TCH_TECH_TREE_MINOAN, CST_TCH_TECH_TREE_PALMYRA, CST_TCH_TECH_TREE_PERSIAN,
CST_TCH_TECH_TREE_PHOENICIAN, CST_TCH_TECH_TREE_ROMAN, CST_TCH_TECH_TREE_SHANG, CST_TCH_TECH_TREE_SUMERIAN, CST_TCH_TECH_TREE_YAMATO,
CST_TCH_JIHAD, CST_TCH_MARTYRDOM };


// Get a technology name from languagex.dll or language.dll.
// Technologies don't really have a name, we use matching research to find it. Works in many cases, not all.
std::string GetTechnologyLocalizedName(short int techId);


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

// Returns an ordered list of research IDs that allow researching input researches (researchesList) with all dependencies
// Returned collection includes all dependencies, including "shadow" researches (no location).
// All research IDs in returned collection are available in tech tree. Impossible research IDs from researchesList are NOT in returned collection.
std::vector<short int> GetValidOrderedResearchesListWithDependencies(STRUCT_PLAYER *player, std::vector<short int> researchesList);

