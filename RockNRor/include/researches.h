#pragma once

#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <assert.h>
#include <macroUtil.h>
#include <AOE_struct_game_global.h>
#include <ROR_global_variables.h>
#include <AOE_const_functional.h>
#include <AOE_const_language.h>
#include "traceMessage.h"
#include "language.h"
#include "RockNRorLocalization.h"
#include "TechnologyFilter.h"
#include "mainStructuresHandling.h"

using namespace AOE_STRUCTURES;
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


namespace AOE_METHODS {
	// Enable or disable a research for a player
	void AOE_enableResearch(AOE_STRUCTURES::STRUCT_PLAYER *player, long int research_id, bool enable);
}


namespace AOE_STRUCTURES {
namespace RESEARCH {
;

// Returns true if technology enables at least one unit (including upgrade a unit into another one).
bool DoesTechEnableSomeUnit(STRUCT_TECH_DEF *techDef);

// Returns true if technology disables at least one unit (generally, tech tree effects).
bool DoesTechDisableSomeUnit(STRUCT_TECH_DEF *techDef);

// Returns true if technology disables at least one research (generally, tech tree effects).
bool DoesTechDisableSomeResearch(STRUCT_TECH_DEF *techDef);

// Returns the first found armor class that is improved by a technology. Returns CST_AC_NONE(-1) if the tech does not improve any armor.
// Generally, even if it has many effects, a technology should improve only 1 armor type, so the result is deterministic as long as this hypothesis is correct.
// Warning: "default armor" is ignored here
AOE_CONST_FUNC::ATTACK_CLASS DoesTechImproveSomeArmor(STRUCT_TECH_DEF *techDef);

// Returns the first found attack class that is improved by a technology. Returns CST_AC_NONE(-1) if the tech does not improve any attack.
// Generally, even if it has many effects, a technology should improve only 1 attack type, so the result is deterministic as long as this hypothesis is correct.
AOE_CONST_FUNC::ATTACK_CLASS DoesTechImproveSomeAttack(STRUCT_TECH_DEF *techDef);

// Returns true for technologies that affect "Shared exploration" resource (#50).
// Can be use to identify writing tech (*not* hardcoded Id)
bool DoesTechRevealAlly(STRUCT_TECH_DEF *techDef);

// Returns true if technology improves priest in some way (includes medecine, fanatism or martydom, but not jihad !)
bool DoesTechImpactReligion(STRUCT_TECH_DEF *techDef);

// Returns true if technology affects villager speed. Wheel and Jihad should do ! (+some tech trees)
bool DoesTechAffectCivilianSpeed(STRUCT_TECH_DEF *techDef);

// Returns true if technology appears to be 'architecture' (improve buildings and walls HP, builder work rate)
bool HasArchitectureEffects(STRUCT_TECH_DEF *techDef);

// Returns true if technology improves lumberjacks AND improves range/LOS for almost all range units (=woodworking-like tech)
bool HasWoodWorkingAndImproveRangeEffects(STRUCT_TECH_DEF *techDef);

}
}


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

// Finds all non-disabled researches that are impossible to develop for given civilization's technology tree.
// Only writes log, does not fix anything.
int DetectDatImpossibleResearches(STRUCT_GAME_GLOBAL *global, short int civId);

// Returns true if technology has at least one effect on provided unit definition.
// Effect can be negative too.
// Use filter argument to exclude techs like martyrdom or negative effects
// /!\ Be careful with villager effects (eg. effects on lumberjack only VS effects on all villagers like jihad)
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

// Returns true if the technology has some negative side effect (slower projectile, less-efficient villagers, etc)
// Evaluates unit attribute changes, upgraded units (slower projectile, lower damage rate, etc)
// Does not take into account "disable unit" or "disable research" effects
// In standard game, this can be trireme/ballista tower (slower projectile), jihad
bool HasTechNegativeSideEffect(STRUCT_TECH_DEF *techDef);

// Returns the age research id of research's required age, from direct dependencies or recursively, if necessary.
// Returns -1 if there is no required age (always available)
short int FindResearchRequiredAge(STRUCT_PLAYER *player, short int researchId);

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
AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *FindBuildingDefThatEnablesResearch(STRUCT_PLAYER *player, short int researchId);

// Writes text representing available tech tree (available technologies that have not been researched yet)
std::string GetRemainingTechTreeText(AOE_STRUCTURES::STRUCT_PLAYER *player);


// Apply a technology for a player. To be used only when there is no research for given technology (like cheat techs).
// In most cases, please use ApplyResearchForPlayer instead.
bool ApplyTechnologyForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int tech_id);
bool ApplyTechnologyForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int tech_id);


// Apply a research for a player (if possible)
// Not allowed if research status is CST_RESEARCH_STATUS_DISABLED or CST_RESEARCH_STATUS_DONE_OR_INVALID
// Works as expected (= like if player actually clicked the corresponding button...) if research status is CST_RESEARCH_STATUS_AVAILABLE
// Works with side effects if research status is : CST_RESEARCH_STATUS_WAITING_REQUIREMENT : other researches might not become available,
// ... tech tree might be somewhat stuck
// Return true if successful
bool ApplyResearchForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id);
// Apply a research for a player (if possible)
// Not allowed if research status is CST_RESEARCH_STATUS_DISABLED or CST_RESEARCH_STATUS_DONE_OR_INVALID
// Works as expected (= like if player actually clicked the corresponding button...) if research status is CST_RESEARCH_STATUS_AVAILABLE
// Works with side effects if research status is : CST_RESEARCH_STATUS_WAITING_REQUIREMENT : other researches might not become available,
// ... tech tree might be somewhat stuck
// Return true if successful
bool ApplyResearchForPlayer(AOE_STRUCTURES::STRUCT_PLAYER *player, long int research_id);


// Returns true if a research has been researched for a given player
bool IsResearchResearched(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id);

// Returns current status of a research for given player.
AOE_CONST_FUNC::RESEARCH_STATUSES GetResearchStatus(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id);

