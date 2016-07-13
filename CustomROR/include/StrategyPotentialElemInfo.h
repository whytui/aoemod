#pragma once
#include <assert.h>
#include <string.h>
#include <set>
#include <ROR_structures.h>
#include <AOE_empires_dat.h>
#include "StrategyBuilderCommon.h"

using namespace AOE_CONST_FUNC;

namespace STRATEGY {

	// Represents 1 unit ("definition") that can potentially be selected to be added to strategy.
	// Used for military units (not buildings, not villagers, not towers)
	class PotentialUnitInfo {
	public:
		PotentialUnitInfo();
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *baseUnitDefLiving; // (DATID1) Unit definition of base unit, without upgrade
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_LIVING *upgradedUnitDefLiving; // (DATID2) Unit definition of best upgraded unit (this is not base unit = could differ from unitDefId)
		short int unitDefId; // DATID1 (base unit ID = without upgrades)
		short int baseUnitDefId; // non-upgraded unit that can be upgraded to "this" one, if any. -1 in most cases. PLEASE use unitDefId instead.
		std::list<short int> upgradesUnitDefId; // List of all available unitDefId this unit can be upgraded to
		short int strongestUpgradeUnitDefId; // Unit definition ID of best available upgraded unit
		short int enabledByResearchId; // research ID that enables the unit, if any. -1 if no such research (unit is always available)
		bool hasUnavailableUpgrade; // Some unit upgrade are unavailable in my tech tree (which means I'm missing super unit, or at least full upgraded unit)
		long int unavailableRelatedResearchesCount;
		long int availableRelatedResearchesCount;
		short int ageResearchId; // ResearchId of the age (stone/tool/bronze/iron) where the unit can be enabled first. -1=always available
		std::set<short int> requiredResearchesForBaseUnit;
		bool isBoat; // true for water units, false for land units
		float speedBase;
		float speedUpgraded;
		float range;
		bool isMelee; // Might be simpler that manipulating range everywhere... We consider range<2 means melee (normally, range=0 for melee, but beware exceptions like fire galley)
		short int hitPoints;
		short int displayedAttack; // displayedAttack from unitDef is reliable (always matches default attack without bonuses, never 0 for units with an attack). displayedAttack=number before the "+" (constant even after upgrade)
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType;
		char *unitName; // For debug
		bool hasCivBonus;
		bool hasSpecificAttackBonus; // true if unit has some special specific bonus, like camel, slinger, chariot
		float conversionResistance;
		long int totalResourceCost;
		bool costsGold;
		int trainCosts[4]; // Cost for training 1 unit (only food/wood/stone/gold costs)
		int weightedCost; // A (subjective) weighted cost value, where rare / precious resources are more valued.
		bool isSuperUnit;
		int damageScore; // An index 1-4 indicating unit attack. 1 is weak, 4 is strong.
		// Scores
		int strengthVs[MC_COUNT];
		int weaknessVs[MC_COUNT]; // Negative values mean good resistance
		int globalScore;
		int bonusForRareStrength; // 0-100
		int bonusForUsedResourceTypes; // 0-100
		int bonusForTechsSimilarity; // 0-100
		bool isSelected;
		bool isOptionalUnit; // True for "retrains" units and/or units added specifically against a weakness (ex: chariot vs priest is strategy has elephants). Such units don't need full upgrades
		bool earlyAgeUnit; // If true, this unit is just for early ages. It is NOT a main unit (not even optional from main choice) and must have limited retrains.
		float desiredCount;
		float addedCount;
		float scoreForUnitCount; // Used in temporary treatments to compute number of units to add in strategy
		float scoreForEarlyAge; // A temporary score for early age unit choices
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *firstStratElem; // First strategy element that trains such a unit
	};

	class PotentialResearchInfo {
	public:
		PotentialResearchInfo();
		short int researchId;
		short int requiredAge; // The age research that is required (direct or indirect requirement). -1 means no age requirement.
		ROR_STRUCTURES_10C::STRUCT_RESEARCH_DEF *researchDef;
		ROR_STRUCTURES_10C::STRUCT_TECH_DEF *techDef;
		bool hasOptionalRequirements;
		bool directRequirementsAreSatisfied;
		short int missingRequiredResearches[4]; // -1 = empty slot. Other value = a required research ID that is not satisfied yet
		bool markedForAdd; // If true, the research has to be added to strategy (or HAS been added, see isInStrategy)
		std::set<short int> researchesThatMustBePutBeforeMe;
		std::set<short int> researchesThatMustBePutAfterMe;
		std::set<short int> unitsThatMustBePutBeforeMe;
		std::set<short int> unitsThatMustBePutAfterMe;
		std::set<short int> impactedUnitDefIds; // Units that have benefits from the research, including villagers, towers.
		int unitInstanceScoreForOptionalResearch; // internal value for optional researches computing
		bool forcePlaceForFirstImpactedUnit; // If true, research will be put close to the first impacted unit "train" elements. Default=false. If set, other constraints may be ignored.
		bool isInStrategy; // Set to true once it has been added to strategy
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *mustBeAfterThisElem; // Do not modify outside of ComputeStratElemPositionConstraints
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *mustBeBeforeThisElem; // Do not modify outside of ComputeStratElemPositionConstraints
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *requiredAgeResearch; // Age research strategy element of my required age.
		int totalCosts; // Sum of all raw costs (not weighted)

		// Updates this->mustBeBeforeThisElem and this->mustBeAfterThisElem according to known dependencies on other unit/researches
		// Previous values of mustBeBeforeThisElem  and mustBeAfterThisElem are reset (lost)
		void ComputeStratElemPositionConstraints(ROR_STRUCTURES_10C::STRUCT_BUILD_AI *buildAI);
	};

	class PotentialBuildingInfo {
	public:
		PotentialBuildingInfo();
		short int unitDefId;
		ROR_STRUCTURES_10C::STRUCT_UNITDEF_BUILDING *unitDef;
		short int enabledByResearchId;
		short int enabledInAge;
		int addedInStrategyCount;
		int desiredCount;
		ROR_STRUCTURES_10C::STRUCT_STRATEGY_ELEMENT *firstAddedInStrategy; // The first of such buildings that was put in strategy
		bool highPriority; // If true, start construction of (first) this building ASAP. E.g. market (to enable farming)
	};
	
}
