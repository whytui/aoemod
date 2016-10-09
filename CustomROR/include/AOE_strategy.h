#pragma once
#include <assert.h>
#include <string.h>
#include <vector>
#include <set>
#include <memory>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_strategy_element.h>
#include <AOE_struct_build_ai.h>
#include <AOE_const_functional.h>
#include <AOE_struct_unit_actions.h>
#include "crCommon.h"
#include "researches.h"
#include "randomizer.h"
#include "unitDefHandling.h"
#include "AOEPrimitives_units.h"

/*
* This file contains primitives to deal with (AI) strategy.
* Like crCommon, this file's methods do not depend on customROR configuration.
*/

using namespace AOE_CONST_FUNC;

namespace STRATEGY {

const char CST_CUSTOMROR_FAKE_STRATELEM_MAXPOP_BEGIN[] = "CustomRORMaxPopulation";

// Returns the element between elem1 and elem2 that comes first in strategy
// Returns NULL if not found (elem1 & elem2 do not belong to buildAI's strategy)
AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *GetFirstElementOf(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI,
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *elem1, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *elem2);

// Returns true if the research is present in tech tree but not researched yet (nor being researched)
bool IsResearchRelevantForStrategy(AOE_STRUCTURES::STRUCT_PLAYER *player, short int research_id);

// Returns the first element's ID that matches criteria. -1=no such element
// WARNING: AIUCTech and AIUCCritical are 2 different filter values ! Searching for researches won't find tool age/bronze/etc !
long int FindElementInStrategy(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_CONST_FUNC::TAIUnitClass elementType, short int DAT_ID);

// Returns the first element's ID that matches criteria AFTER searchFrom (not included in search). Will stop if an empty or start item is encountered
// Returns NULL if no such element
// WARNING: AIUCTech and AIUCCritical are 2 different filter values ! Searching for researches won't find tool age/bronze/etc !
AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *FindFirstElementInStrategy(AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *searchFrom, AOE_CONST_FUNC::TAIUnitClass elementType, short int DAT_ID);

// This fixes dynamically added houses, boats + boat techs, docks, setgatherpercentage
void FixAutoBuildStrategyElements(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI);

// Sets a research status to "alive" in strategy ; useful in some cases that are not managed automatically
void MarkResearchAsDoneInStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, short int DATID);


// Update strategy to add existing units so AI won't train/build it again (to call after enabling AI on a player)
void UpdateStrategyWithUnreferencedExistingUnits(AOE_STRUCTURES::STRUCT_PLAYER *player);
// Update strategy to add existing units so AI won't train/build it again. Check will only be run on matching strategy elements.
// This overload is a bit faster because it filters unit types.
// You can use DAT_ID=-1 and elemType=AIUCNone as jokers (to apply on all strategy elements)
// We consider it is a technical fix, so it is always done, even if AI improvement is disabled.
void UpdateStrategyWithUnreferencedExistingUnits(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, long int DAT_ID, TAIUnitClass elemType);

// Tries to add an existing unit to player's strategy (cf buildAI pointer)
// Returns true if a matching (not in progress/done) strategy element has been found and updated (set to done + associate with unitId)
// Note: unit may NOT belong to same player because it is used in successful conversion code, before unit is actually transfered to new player.
bool UpdateStrategyWithExistingUnit(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, AOE_STRUCTURES::STRUCT_UNIT_BASE *unit);

// Calls RoR original code to add an element in strategy (AFTER the element in positionToInsert).
// Consecutive inserts after a same element are reversed (of course) !
// If you call this to add a research (type 1 or 4), you need to update the unit name afterwards because ROR method is bugged.
// DO NOT use it to insert researches, please use AddUnitInStrategy_before
bool AddUnitInStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, long int positionToInsert, long int retrains, long int actor, 
	AOE_CONST_FUNC::TAIUnitClass unitType, long int unitDATID, AOE_STRUCTURES::STRUCT_PLAYER *player);

// Custom method to insert a line in strategy. This tries to be a bit faster and avoids "unit" Name bug for researches
// Inserts before 'nextElem' strategy element. Consecutive inserts order is preserved if we insert before the same element.
// If you want to insert researches in strategy, use this overload.
bool AddUnitInStrategy_before(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *nextElem, 
	long int retrains, long int actor, AOE_CONST_FUNC::TAIUnitClass unitType, long int unitDATID, AOE_STRUCTURES::STRUCT_PLAYER *player, const char *name);

// Create several strategy elements between (after) intervalStart and and Nth element after intervalStart (N=intervalLength)
// The new elements are inserted with a "flat" repartition between start point and end point
// elemCountToInsert = number of strategy elements to create
// Returns number of actually inserted elements
int AddStrategyElements(AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *intervalStart, int intervalLength, int elemCountToInsert,
	long int retrains, long int actor, AOE_CONST_FUNC::TAIUnitClass unitType, long int unitDATID, AOE_STRUCTURES::STRUCT_PLAYER *player, const char *name);

// Use it to move a strategy element to any other location in strategy list.
// afterThisElem if the element after which we move elemToMove
bool MoveStrategyElement_after(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *elemToMove,
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *afterThisElem);

// If elem_shouldBeAfter if NOT after elem_shouldBeBefore, then move elem_shouldBeAfter just after elem_shouldBeBefore.
// Note: only elem_shouldBeAfter (3rd argument) can be MOVED by this method.
// Use this to fix dependency issues, a correct call example is elem_shouldBeBefore=barracks, elem_shouldBeAfter=clubman
// Return true if an element has been moved, false otherwise
bool MoveStrategyElement_after_ifWrongOrder(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI,
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *elem_shouldBeBefore,
	AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *elem_shouldBeAfter);

// An overload that applies on the first matching element (for both "shouldBeBefore"/"shouldBeAfter"), starting from strategy beginning.
// Example: if used with house and man (there are several of each in strategy), first house will be used as shouldBeBefore, first man as shouldBeAfter
bool MoveStrategyElement_after_ifWrongOrder(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI,
	long int DATID_shouldBeBefore,
	TAIUnitClass elemType_shouldBeBefore,
	long int DATID_shouldBeAfter,
	TAIUnitClass elemType_shouldBeAfter);

// Remove (and free) a strategy element from strategy chained list
// Return true if memory was successfully freed.
bool RemoveStrategyElement(AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *elem);

// Remove all strategy element IDs from unit actions/internal information.
// Used when strategy is reset, and sequence(elem count) reset to 0, so that obsolete IDs do not interact with new strategy elements.
void RemoveAllReferencesToStratElemIDs(AOE_STRUCTURES::STRUCT_PLAYER *player);

// Remove all strategy elements from buildAI except fake first element
// See also 0x4091C0: this ROR method free all elements, set sequence(=count) to 0, set fake's next&previous to itself.
// This does NOT reset the flags that will allow re-triggering dynamic strategy elements insertion. You'll have to manage it manually.
void ClearStrategy(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI);

// Add units if maximum population is >50
void AdaptStrategyToMaxPopulation(AOE_STRUCTURES::STRUCT_PLAYER *player);

// Add relevant researches to strategy for current strategy's military units
void AddUsefulMilitaryTechsToStrategy(AOE_STRUCTURES::STRUCT_PLAYER *player);

// Returns true if current strategy status allows triggering a wonder construction
bool IsStrategyCompleteForWonder(AOE_STRUCTURES::STRUCT_AI *ai);

// Adds in strategy all available researches that improve provided unit
// Returns the number of inserted researches in strategy
// allUpgrades: if true, all related upgrades will be added. Otherwise, only requirements will be added.
// nextElement: if non-NULL, newly-added strategy elements will be inserted BEFORE nextElement. Otherwise, insertion point will be computed automatically.
int AddResearchesInStrategyForUnit(AOE_STRUCTURES::STRUCT_AI *ai, short int unitDefId, bool allUpgrades, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *nextElement);

// Inserts one or many new strategy elements before nextElement.
// New strategy elements corresponds to resDef (not always a research: can be a building !) + requirements
// Returns the number of added elements just before nextElement.
int AddStrategyElementForResearch(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *nextElement, short int researchId);

// Adds a strategy element (building) only if there are none already.
// Does not perform adding for buildings that are automatically build in AI (regarding player's PERsonality)
// Returns true if an element was added.
bool AddStrategyElementForBuildingIfNotExisting(AOE_STRUCTURES::STRUCT_PLAYER *player, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *nextElement, 
	AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDefBuilding);

// Find a strategy element for a specific actor building (instance)id. (strategy element must be in progress)
// This is only possible thanks to custom treatments that set unitInstanceId to actor unitId while training/researching is in progress.
// Unicity is not guaranteed (first found is returned), however it is not supposed to happen much.
// actorUnitId must be >=0
// Returns NULL if not found.
AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *GetStrategyElementForActorBuilding(AOE_STRUCTURES::STRUCT_PLAYER *player, long int actorUnitId);

// Resets a strategy element status (in progress, active)
void ResetStrategyElementStatus(AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *elem);

// Returns true if the strategy element is the fake one added by customROR when it updated strategy to adapt to maximum population.
bool IsCustomRorPopulationBeginStratElem(AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem);

// Returns the fake strategy element added by customROR when it updated strategy to adapt to maximum population.
// Returns NULL if not found.
AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *GetCustomRorMaxPopulationBeginStratElem(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI);

// Dumps strategy to text format, for debugging.
std::string ExportStrategyToText(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI);


}
