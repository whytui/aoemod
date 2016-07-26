#pragma once

#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <assert.h>
#include <ROR_structures.h>
#include <ROR_global_variables.h>
#include <AOE_const_functional.h>
#include <AOE_const_language.h>
#include "traceMessage.h"
#include "language.h"
#include "crLocalization.h"
#include "TechnologyFilter.h"

using namespace ROR_STRUCTURES_10C;
using namespace AOE_CONST_FUNC;

// Technologies to ignore for automatic technology adding/detection, because they have drawbacks or unused by AI
// Ages and tech trees are in ignore list too
const short int LST_TECHS_TO_IGNORE[] = { CST_TCH_STONE_AGE, CST_TCH_TOOL_AGE, CST_TCH_BRONZE_AGE, CST_TCH_IRON_AGE,
CST_TCH_TECH_TREE_ALL_TECHS, CST_TCH_TECH_TREE_ASSYRIAN, CST_TCH_TECH_TREE_BABYLONIAN, CST_TCH_TECH_TREE_CARTHAGE,
CST_TCH_TECH_TREE_CHOSON, CST_TCH_TECH_TREE_EGYPTIAN, CST_TCH_TECH_TREE_GREEK, CST_TCH_TECH_TREE_HITTITE,
CST_TCH_TECH_TREE_MACEDONIAN, CST_TCH_TECH_TREE_MINOAN, CST_TCH_TECH_TREE_PALMYRA, CST_TCH_TECH_TREE_PERSIAN,
CST_TCH_TECH_TREE_PHOENICIAN, CST_TCH_TECH_TREE_ROMAN, CST_TCH_TECH_TREE_SHANG, CST_TCH_TECH_TREE_SUMERIAN, CST_TCH_TECH_TREE_YAMATO,
CST_TCH_JIHAD, CST_TCH_MARTYRDOM,
CST_TCH_BALLISTA_TOWER // because researching ballista tower in RM is not such a good idea (bad against villagers)
};


// Get a technology name from languagex.dll or language.dll.
// Technologies don't really have a name, we use matching research to find it. Works in many cases, not all.
std::string GetTechnologyLocalizedName(short int techId);

// Get a research name from languagex.dll or language.dll. Or internal name if no localization was found.
std::string GetResearchLocalizedName(short int researchId);

// Returns true if the research has more required researches than "minimum required researches count" (all requirements are not mandatory)
bool ResearchHasOptionalRequirements(STRUCT_RESEARCH_DEF *resDef);

// Returns the researchId of an age (stone/tool/bronze/iron) if found in one of the 4 research's requirements. NOT recursive.
// Returns -1 if not found, a value in [CST_RSID_STONE_AGE, CST_RSID_IRON_AGE] if found.
short int GetAgeResearchFromDirectRequirement(STRUCT_RESEARCH_DEF *researchDef);

// Disable all impossible researches for all players.
// An impossible research is a research that is waiting for requirements, including ones that can never be satisfied.
// Example in original game: irrigation for persian, armored elephant for yamato, etc.
int DisableImpossibleResearches();

// Disable all impossible researches for a specific player.
// An impossible research is a research that is waiting for requirements, including ones that can never be satisfied.
// Example in original game: irrigation for persian, armored elephant for yamato, etc.
int DisablePlayerImpossibleResearches(STRUCT_PLAYER *player);

// Returns true if technology has at least one effect on provided unit definition.
// Effect can be negative too.
// Use filter argument to exclude techs like martyrdom or negative effects
bool DoesTechAffectUnit(STRUCT_TECH_DEF *techDef, STRUCT_UNITDEF_BASE *unitDef, const AOE_TECHNOLOGIES::TechnologyFilterBase *filter);

// Returns "destination" unit ID if technology upgrades provided unit definition ID into another unit
// Returns -1 if technology does NOT upgrade provided unit to another unit
short int GetNewUnitIdIfTechUpgradesUnit(STRUCT_TECH_DEF *techDef, short int unitDefId);

// Returns true if technology enables provided unit.
bool DoesTechEnableUnit(STRUCT_TECH_DEF *techDef, short int unitDefId);

// Returns true if technology disables provided unit (generally, tech tree effects).
bool DoesTechDisableUnit(STRUCT_TECH_DEF *techDef, short int unitDefId);

// Returns true if technology disables provided research (generally, tech tree effects).
bool DoesTechDisableResearch(STRUCT_TECH_DEF *techDef, short int researchId);

// Returns true if a unit (DATID) is disabled by player's tech tree
bool IsUnitDisabledInTechTree(short int playerId, short int unitDefId);

// Returns true if a research (research ID) is disabled by player's tech tree
bool IsResearchDisabledInTechTree(short int playerId, short int researchId);


// Returns the age research id of research's required age, from direct dependencies or recursively, if necessary.
// Returns -1 if there is no required age (always available)
short int FindResearchRequiredAge(STRUCT_PLAYER *player, short int researchId);


// Finds all (non disabled) researches that affect a unit (definition)
// If ignoreUndesirableTechs==true, techs from LST_TECHS_TO_IGNORE are ignored (jihad, etc)
std::vector<short int> FindResearchesThatAffectUnit(STRUCT_PLAYER *player, long int unitDefId, bool ignoreUndesirableTechs);

// Returns the first (non disabled) research ID found that enables a unit. Returns -1 if not found.
// startAtResearchId : -1=ignored (joker). If >=0, than the search will start at this index and ignore previous researches.
short int FindResearchThatEnableUnit(STRUCT_PLAYER *player, short int unitDefId, short int startAtResearchId);

// Returns an ordered list of research IDs that allow researching input researches (researchesList) with all dependencies
// Returned collection includes all dependencies, including "shadow" researches (no location).
// Warning: dependency with choices (optional requirements, e.g. research74="A01, B01, F01, I01 ->Tool Age" are returned
// ... BUT underlying requirements are NOT analyzed (as we can't choose which requirements should be used / ignored)
// The caller MUST handle such researches (tip: use ResearchHasOptionalRequirements(...) to identify them)
// All research IDs in returned collection are available in tech tree. Impossible research IDs from researchesList are NOT in returned collection.
std::vector<short int> GetValidOrderedResearchesListWithDependencies(STRUCT_PLAYER *player, std::vector<short int> researchesList);

// Returns a unit (building) definition that enables provided research ID.
// For example, for research 17, it will return temple's definition because temple's unitDef.initiatesResearch == 17.
// Returns NULL if not found.
// Warning: result is not unique. Eg. Both barracks (12,132) initiate research 62. This method tries to return the base unit ID (not upgraded one)
ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *FindBuildingDefThatEnablesResearch(STRUCT_PLAYER *player, short int researchId);

// Writes text representing available tech tree (available technologies that have not been researched yet)
std::string GetRemainingTechTreeText(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

