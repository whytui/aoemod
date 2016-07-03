#pragma once
#include <assert.h>
#include <string.h>
#include <vector>
#include <set>
#include <memory>
#include <ROR_structures.h>
#include <AOE_const_functional.h>
#include "crCommon.h"
#include "researches.h"
#include "randomizer.h"

/*
* This file contains primitives to deal with (AI) strategy.
* Like crCommon, this file's methods do not depend on customROR configuration.
*/

using namespace AOE_CONST_FUNC;

const char CST_CUSTOMROR_FAKE_STRATELEM_MAXPOP_BEGIN[] = "CustomRORMaxPopulation";


// This class collects useful information for strategy generation
class StrategyGenerationInfo {
public:
	StrategyGenerationInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
		this->civId = -1;
		this->fishingShipsCount_alwaysRetrain = 0;
		this->fishingShipsCount_limitedRetrains = 0;
		this->maxPopulation = 50;
		this->tradeShipsCount = 0;
		this->transportShipsCount = 0;
		this->villagerCount_alwaysRetrain = 0;
		this->villagerCount_limitedRetrains = 0;
		this->SetPlayerInfo(player);
	}
	const ROR_STRUCTURES_10C::STRUCT_PLAYER *player;
	const ROR_STRUCTURES_10C::STRUCT_AI *ai; // automatically set from player (SetPlayerAndAIStructs)
	const ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI; // automatically set from player (SetPlayerAndAIStructs)
	short int civId;
	bool isWaterMap;
	long int maxPopulation;
	long int villagerCount_alwaysRetrain;
	long int villagerCount_limitedRetrains;
	long int fishingShipsCount_alwaysRetrain;
	long int fishingShipsCount_limitedRetrains;
	long int tradeShipsCount;
	long int transportShipsCount;
	//long int warShipsCount;
	//long int militaryUnitsCount;

private:
	void SetPlayerInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player) {
		this->ai = NULL;
		this->buildAI = NULL;
		this->player = player;
		if (!player || !player->IsCheckSumValid()) { return; }
		this->civId = player->civilizationId;
		this->ai = player->ptrAIStruct;
		if (!this->ai || !this->ai->IsCheckSumValid()) { return; }
		this->buildAI = &this->ai->structBuildAI;
	}
};


class PotentialUnitInfo {
public:
	PotentialUnitInfo() {
		this->unitDefId = -1;
		this->enabledByResearchId = -1;
		this->hasUnavailableUpgrade = false;
		this->hasCivBonus = false;
		this->hitPoints = 0;
		this->range = 0;
		this->isMelee = false;
		this->speed = 0;
		this->displayedAttack = 0;
		this->unitAIType = GLOBAL_UNIT_AI_TYPES::TribeAINone;
		this->unavailableRelatedResearchesCount = 0;
		this->availableRelatedResearchesCount = 0;
		this->ageResearchId = -1;
	}
	short int unitDefId;
	short int baseUnitDefId; // non-upgraded unit that can be upgraded to "this" one, if any.
	std::list<short int> upgradesUnitDefId; // List of all available unitDefId this unit can be upgraded to
	short int enabledByResearchId; // research ID that enables the unit, if any. -1 if no such research (unit is always available)
	bool hasUnavailableUpgrade; // Some unit upgrade are unavailable in my tech tree (which means I'm missing super unit, or at least full upgraded unit)
	long int unavailableRelatedResearchesCount;
	long int availableRelatedResearchesCount;
	short int ageResearchId; // ResearchId of the age (stone/tool/bronze/iron) where the unit can be enabled first. -1=always available
	float speed;
	float range;
	bool isMelee; // Might be simpler that manipulating range everywhere... We consider range<2 means melee (normally, range=0 for melee, but beware exceptions like fire galley)
	short int hitPoints;
	short int displayedAttack; // displayedAttack from unitDef is reliable (always matches default attack without bonuses, never 0 for units with an attack)
	AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType;
	char *unitName; // For debug
	bool hasCivBonus;
};


// Returns true if the research is present in tech tree but not researched yet (nor being researched)
bool IsResearchRelevantForStrategy(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, short int research_id);

// Returns 1st element position (>=0) if (at least) 1 matching element exists in strategy. -1=no such element
long int FindElementInStrategy(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, AOE_CONST_FUNC::TAIUnitClass elementType, short int DAT_ID);

// This fixes dynamically added houses, boats + boat techs, docks, setgatherpercentage
void FixAutoBuildStrategyElements(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

// Sets a research status to "alive" in strategy ; useful in some cases that are not managed automatically
void MarkResearchAsDoneInStrategy(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, short int DATID);


// Update strategy to add existing units so AI won't train/build it again (to call after enabling AI on a player)
void UpdateStrategyWithUnreferencedExistingUnits(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);
// Update strategy to add existing units so AI won't train/build it again. Check will only be run on matching strategy elements.
// This overload is a bit faster because it filters unit types.
// You can use DAT_ID=-1 and elemType=AIUCNone and stratElemId=-1 as jokers (to apply on all strategy elements)
// We consider it is a technical fix, so it is always done, even if AI improvement is disabled.
void UpdateStrategyWithUnreferencedExistingUnits(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, long int DAT_ID,
	TAIUnitClass elemType, long int stratElemId);

// Tries to add an existing unit to player's strategy (cf buildAI pointer)
// Returns true if a matching (not in progress/done) strategy element has been found and updated (set to done + associate with unitId)
// Note: unit may NOT belong to same player because it is used in successful conversion code, before unit is actually transfered to new player.
bool UpdateStrategyWithExistingUnit(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, ROR_STRUCTURES_10C::STRUCT_UNIT *unit);

// Calls RoR original code to add an element in strategy (AFTER the element in positionToInsert).
// Consecutive inserts after a same element are reversed (of course) !
// If you call this to add a research (type 1 or 4), you need to update the unit name afterwards because ROR method is bugged.
// DO NOT use it to insert researches, please use AddUnitInStrategy_before
bool AddUnitInStrategy(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, long int positionToInsert, long int retrains, long int actor, 
	AOE_CONST_FUNC::TAIUnitClass unitType, long int unitDATID, ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

// Custom method to insert a line in strategy. This tries to be a bit faster and avoids "unit" Name bug for researches
// Inserts before 'nextElem' strategy element. Consecutive inserts order is preserved if we insert before the same element.
// If you want to insert researches in strategy, use this overload.
bool AddUnitInStrategy_before(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *nextElem, 
	long int retrains, long int actor, AOE_CONST_FUNC::TAIUnitClass unitType, long int unitDATID, ROR_STRUCTURES_10C::STRUCT_PLAYER *player, const char *name);

// Use it to move a strategy element to any other location in strategy list.
// afterThisElem if the element after which we move elemToMove
bool MoveStrategyElement_after(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elemToMove,
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *afterThisElem);

// If elem_shouldBeAfter if NOT after elem_shouldBeBefore, then move elem_shouldBeAfter just after elem_shouldBeBefore.
// Note: only elem_shouldBeAfter (3rd argument) can be MOVED by this method.
// Use this to fix dependency issues, a correct call example is elem_shouldBeBefore=barracks, elem_shouldBeAfter=clubman
// Return true if an element has been moved, false otherwise
bool MoveStrategyElement_after_ifWrongOrder(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI,
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elem_shouldBeBefore,
	ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elem_shouldBeAfter);

// An overload that applies on the first matching element (for both "shouldBeBefore"/"shouldBeAfter"), starting from strategy beginning.
// Example: if used with house and man (there are several of each in strategy), first house will be used as shouldBeBefore, first man as shouldBeAfter
bool MoveStrategyElement_after_ifWrongOrder(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI,
	long int DATID_shouldBeBefore,
	TAIUnitClass elemType_shouldBeBefore,
	long int DATID_shouldBeAfter,
	TAIUnitClass elemType_shouldBeAfter);

// Remove (and free) a strategy element from strategy chained list
// Return true if memory was successfully freed.
bool RemoveStrategyElement(ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elem);

// Remove all strategy element IDs from unit actions/internal information.
// Used when strategy is reset, and sequence(elem count) reset to 0, so that obsolete IDs do not interact with new strategy elements.
void RemoveAllReferencesToStratElemIDs(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

// Remove all strategy elements from buildAI except fake first element
// See also 0x4091C0: this ROR method free all elements, set sequence(=count) to 0, set fake's next&previous to itself.
// This does NOT reset the flags that will allow re-triggering dynamic strategy elements insertion. You'll have to manage it manually.
void ClearStrategy(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

// Add units if maximum population is >50
void AdaptStrategyToMaxPopulation(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

// Add relevant researches to strategy for current strategy's military units
void AddUsefulMilitaryTechsToStrategy(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);

// Returns true if current strategy status allows triggering a wonder construction
bool IsStrategyCompleteForWonder(ROR_STRUCTURES_10C::STRUCT_AI *ai);

// Adds in strategy all available researches that improve provided unit
// Returns the number of inserted researches in strategy
// allUpgrades: if true, all related upgrades will be added. Otherwise, only requirements will be added.
// nextElement: if non-NULL, newly-added strategy elements will be inserted BEFORE nextElement. Otherwise, insertion point will be computed automatically.
int AddResearchesInStrategyForUnit(ROR_STRUCTURES_10C::STRUCT_AI *ai, short int unitDefId, bool allUpgrades, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *nextElement);

// Inserts one or many new strategy elements before nextElement.
// New strategy elements corresponds to resDef (not always a research: can be a building !) + requirements
// Returns the number of added elements just before nextElement.
int AddStrategyElementForResearch(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *nextElement, short int researchId);

// Adds a strategy element (building) only if there are none already.
// Does not perform adding for buildings that are automatically build in AI (regarding player's PERsonality)
// Returns true if an element was added.
bool AddStrategyElementForBuildingIfNotExisting(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *nextElement, 
	ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDefBuilding);

// Find a strategy element for a specific actor building (instance)id. (strategy element must be in progress)
// This is only possible thanks to custom treatments that set unitInstanceId to actor unitId while training/researching is in progress.
// Unicity is not guaranteed (first found is returned), however it is not supposed to happen much.
// actorUnitId must be >=0
// Returns NULL if not found.
ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *GetStrategyElementForActorBuilding(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, long int actorUnitId);

// Resets a strategy element status (in progress, active)
void ResetStrategyElementStatus(ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *elem);

// Returns true if the strategy element is the fake one added by customROR when it updated strategy to adapt to maximum population.
bool IsCustomRorPopulationBeginStratElem(ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *stratElem);

// Returns the fake strategy element added by customROR when it updated strategy to adapt to maximum population.
// Returns NULL if not found.
ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *GetCustomRorMaxPopulationBeginStratElem(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

// Dumps strategy to text format, for debugging.
std::string ExportStrategyToText(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

// Create a brand new dynamic strategy for player.
void CreateStrategyFromScratch(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);

// Fills unitInfos with all available military units from tech tree.
// Towers are ignored (not added to list). Boats are ignored on non-water maps.
// *** Make sure to delete all PotentialUnitInfo from list when you're finished with the list ***
void CollectPotentialUnitsInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player, std::list<PotentialUnitInfo> &unitInfos);

std::shared_ptr<StrategyGenerationInfo> GetStrategyGenerationInfo(ROR_STRUCTURES_10C::STRUCT_PLAYER *player);
