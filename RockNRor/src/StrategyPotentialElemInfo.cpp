#include "../include/StrategyBuilder.h"

using namespace STRATEGY;


PotentialUnitInfo::PotentialUnitInfo() {
	this->baseUnitDefLiving = NULL;
	this->upgradedUnitDefLiving = NULL;
	this->unitDefId = -1;
	this->baseUnitDefId = -1;
	this->enabledByResearchId = -1;
	this->strongestUpgradeUnitDefId = -1;
	this->hasUnavailableUpgrade = false;
	this->hasCivBonus = false;
	this->hasSpecificAttackBonus = false;
	this->hitPoints = 0;
	this->range = 0;
	this->isMelee = false;
	this->speedBase = 0;
	this->speedUpgraded = 0;
	this->displayedAttack = 0;
	this->unitAIType = GLOBAL_UNIT_AI_TYPES::TribeAINone;
	this->unavailableRelatedResearchesCount = 0;
	this->availableRelatedResearchesCount = 0;
	this->ageResearchId = -1;
	this->conversionResistance = 1; // Default value
	this->totalResourceCost = 0;
	this->costsGold = false;
	this->isSuperUnit = false;
	this->damageScore = 0;
	this->globalScore = 0;
	this->globalScoreEarlyAge = 0;
	for (int i = 0; i < 4; i++) { this->trainCosts[i] = 0; }
	this->weightedCost = 0;
	this->bonusForRareStrength = 0;
	this->bonusForUsedResourceTypes = 0;
	this->bonusForTechsSimilarity = 0;
	this->isSelected = false;
	this->isOptionalUnit = false;
	this->earlyAgeUnit = false;
	this->desiredCount = 0;
	this->addedCount = 0;
	this->scoreForEarlyAge = 0;
	this->firstStratElem = NULL;
}

PotentialResearchInfo::PotentialResearchInfo() {
	this->researchId = -1;
	this->researchDef = NULL;
	this->requiredAge = -1;
	this->techDef = NULL;
	this->markedForAdd = false;
	this->hasOptionalRequirements = false;
	this->directRequirementsAreSatisfied; // True when enough direct requirements are satisfied to "enable" the research.
	for (int i = 0; i < 4; i++) { missingRequiredResearches[i] = -1; }
	this->isInStrategy = false;
	this->mustBeAfterThisElem = NULL;
	this->mustBeBeforeThisElem = NULL;
	this->requiredAgeResearch = NULL;
	this->totalCosts = 0;
	this->forcePlaceForFirstImpactedUnit = false;
	this->forcePutAsEarlyAsPossible = false;
	this->forcePutAfterOtherResearches = false;
	this->actualStrategyElement = NULL;
}

PotentialBuildingInfo::PotentialBuildingInfo() {
	this->unitDef = NULL;
	this->unitDefId = -1;
	this->enabledByResearchId = -1;
	this->enabledInAge = -1; // ResearchId of age where the building becomes available
	this->addedInStrategyCount = 0;
	this->desiredCount = 0;
	this->highPriority = false;
	this->firstAddedInStrategy = NULL;
	this->unitsToBeTrained = 0;
}

