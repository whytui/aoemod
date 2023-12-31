#pragma once
#include <assert.h>
#include <string.h>
#include <set>
#include <AOE_struct_unit_def.h>
#include <AOE_struct_strategy_element.h>
#include <AOE_struct_tech_def.h>
#include <AOE_struct_research_def.h>
#include <AOE_empires_dat.h>
#include "TTDetailedResearchDef.h"

using namespace AOE_CONST_FUNC;

namespace ROCKNROR {
namespace STRATEGY {
	
	// Represents various military categories used to help strategy building decisions
	enum MILITARY_CATEGORY {
		MC_NONE = -1, // ignore in computations
		MC_PRIEST = 0,
		MC_FAST_RANGED,
		MC_SIEGE,
		MC_MELEE,
		MC_TOWER,
		MC_COUNT
	};

	// Represents 1 unit ("definition") that can potentially be selected to be added to strategy.
	// Used for military units (not buildings, not villagers, not towers)
	class PotentialUnitInfo {
	public:
		PotentialUnitInfo();
		short int unitDefId; // DATID1 (base unit ID = without upgrades)
		string langName; // For debug
		const char *unitName; // For debug
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *baseUnitDefLiving; // (DATID1) Unit definition of base unit, without upgrade
		AOE_STRUCTURES::STRUCT_UNITDEF_TRAINABLE *upgradedUnitDefLiving; // (DATID2) Unit definition of best (available) upgraded unit (this is not base unit = could differ from unitDefId)
		std::list<short int> upgradesUnitDefId; // List of all available unitDefId this unit can be upgraded to
		short int strongestUpgradeUnitDefId; // Unit definition ID of best available upgraded unit
		short int enabledByResearchId; // research ID that enables the unit, if any. -1 if no such research (unit is always available)
		short int strongestUpgradeUnitEnabledByResearchId; // research ID that enables the BEST available upgraded unit, if any. -1 if no such research (unit is always available)
		bool hasUnavailableUpgrade; // Some unit upgrade are unavailable in my tech tree (which means I'm missing super unit, or at least full upgraded unit)
		std::set<TTDetailedResearchDef*> unavailableRelatedResearchDetail; // List of detailed info on related UNavailable researches
		std::set<TTDetailedResearchDef*> availableRelatedResearchDetail; // List of detailed info on related available researches
		short int ageResearchId; // ResearchId of the age (stone/tool/bronze/iron) where the unit can be enabled first. -1=always available
		std::set<short int> requiredResearchesForBaseUnit; // ALL required researches for same age, excluding "enabledByResearchId", for base unit. Includes researches initiated by required buildings !
		std::set<short int> requiredResearchesForStrongestUpgrade; // Required researches for same age, excluding "strongestUpgradeUnitEnabledByResearchId", for best upgraded unit. Includes researches initiated by required buildings !
		bool isBoat; // true for water units, false for land units
		bool isNonTowerMilitary; // true for military units (excluding towers)
		float speedBase;
		float speedUpgraded;
		float range; // Range for best upgrade
		bool isMelee; // Might be simpler that manipulating range everywhere... We consider range<2 means melee (normally, range=0 for melee, but beware exceptions like fire galley)
		short int hitPoints; // Hit points of the best upgraded unit
		short int displayedAttack; // displayedAttack from unitDef is reliable (always matches default attack without bonuses, never 0 for units with an attack). displayedAttack=number before the "+" (constant even after upgrade)
		AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitAIType;
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
		int globalScore; // A global score for unit using its strengths (including upgrades)
		int globalScoreEarlyAge; // A global score for unit using its strengths, without upgrades
		int bonusForRareStrength; // 0-100
		int bonusForUsedResourceTypes; // 0-100
		int bonusForTechsSimilarity; // 0-100
		bool isSelected; // True if we validate the fact that unit will be actually added to strategy. See also desiredCount.
		bool isOptionalUnit; // True for "retrains" units and/or units added specifically against a weakness (ex: chariot vs priest is strategy has elephants). Such units don't need full upgrades
		bool earlyAgeUnit; // If true, this unit is just for early ages. It is NOT a main unit (not even optional from main choice) and must have limited retrains.
		float desiredCount;
		float addedCount;
		float scoreForUnitCount; // Used in temporary treatments to compute number of units to add in strategy
		float scoreForEarlyAge; // A temporary score for early age unit choices
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *firstStratElem; // First strategy element that trains such a unit
		ROCKNROR::STRATEGY::TTDetailedUnitDef *unitDefDetailedInfo;
		

		// Returns localized name of base unit
		const char *GetUpgradedUnitName() {
			static char buffer[100];
			buffer[0] = 0;
			if (this->baseUnitDefLiving) {
				GetLanguageDllText(this->upgradedUnitDefLiving->languageDLLID_Name, buffer, sizeof(buffer),
					this->upgradedUnitDefLiving->ptrUnitName);
				return buffer;
			}
			return this->unitName;
		}
		// Returns localized name of base unit
		const char *GetBaseUnitName() {
			static char buffer[100];
			buffer[0] = 0;
			if (this->baseUnitDefLiving) {
				GetLanguageDllText(this->baseUnitDefLiving->languageDLLID_Name, buffer, sizeof(buffer),
					this->baseUnitDefLiving->ptrUnitName);
				return buffer;
			}
			return this->unitName;
		}
	};

	class PotentialResearchInfo {
	public:
		PotentialResearchInfo();
		short int researchId;
		string langName; // For debug
		short int requiredAge; // The age research ID that is required (direct or indirect requirement). -1 means no age requirement, 100=stone, etc.
		AOE_STRUCTURES::STRUCT_RESEARCH_DEF *researchDef;
		AOE_STRUCTURES::STRUCT_TECH_DEF *techDef;
		bool hasOptionalRequirements; // True if the research has a "choice" on its requirements (less needed than available), like ages
		bool directRequirementsAreSatisfied;
		short int missingRequiredResearches[4]; // -1 = empty slot. Other value = a required research ID that is not satisfied yet
		bool markedForAdd; // If true, the research has to be added to strategy (or HAS been added, see isInStrategy)
		std::set<short int> researchesThatMustBePutBeforeMe;
		std::set<short int> researchesThatMustBePutAfterMe;
		std::set<short int> unitsThatMustBePutBeforeMe;
		std::set<short int> unitsThatMustBePutAfterMe;
		std::set<short int> impactedUnitDefIds; // Units that have benefits from the research, including villagers, towers.
		int unitInstanceScoreForOptionalResearch; // internal value for optional researches computing
		bool forcePlaceForFirstImpactedUnit; // If true, research will be put near first involved unit. Default=false. If set, other constraints may be ignored. Used in combination with mustBeAfterThisElem.
		bool forcePutAsEarlyAsPossible; // If true, research will be put as early as possible in strategy (just after its requirements). Default=false. If set, other constraints may be ignored. Used in combination with mustBeAfterThisElem.
		bool forcePutAfterOtherResearches; // If true, research will be put after all researches that impact "my" impacted units
		bool isInStrategy; // Set to true once it has been added to strategy
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *mustBeAfterThisElem; // Do not modify outside of ComputeStratElemPositionConstraints
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *mustBeBeforeThisElem; // Do not modify outside of ComputeStratElemPositionConstraints
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *requiredAgeResearch; // Age research strategy element of my required age.
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *actualStrategyElement;
		int totalCosts; // Sum of all raw costs (not weighted)
		ROCKNROR::STRATEGY::TTDetailedResearchDef *detailedResInfo;

		// Updates this->mustBeBeforeThisElem and this->mustBeAfterThisElem according to known dependencies on other unit/researches
		// Previous values of mustBeBeforeThisElem  and mustBeAfterThisElem are reset (lost)
		void ComputeStratElemPositionConstraints(AOE_STRUCTURES::STRUCT_BUILD_AI *buildAI);
	};

	class PotentialBuildingInfo {
	public:
		PotentialBuildingInfo();
		short int unitDefId;
		string langName; // For debug
		AOE_STRUCTURES::STRUCT_UNITDEF_BUILDING *unitDef;
		bool markedForAdd;
		short int enabledByResearchId;
		short int enabledInAge;
		int addedInStrategyCount;
		int desiredCount;
		AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *firstAddedInStrategy; // The first of such buildings that was put in strategy
		bool highPriority; // If true, start construction of (first) this building ASAP. E.g. market (to enable farming)
		int unitsToBeTrained; // Number of (always retrainable) units that are trained in such buildings
		ROCKNROR::STRATEGY::TTDetailedBuildingDef *unitDefDetailedInfo;
		std::set<short int> requiredResearchesForBaseUnit; // ALL required researches for same age, excluding "enabledByResearchId", for base unit. Includes researches initiated by required buildings !
	};
	
}
}
