#include <assert.h>
#include <set>
#include <AOE_offsets.h>
#include <AOE_empires_dat.h>
#include <AOE_struct_unit_def.h>
#include "TTDetailedResearchDef.h"


#pragma once

using namespace AOE_STRUCTURES;

namespace ROCKNROR {
namespace STRATEGY {
;


// Detailed information about a unit class, valued from tech tree analysis
class TTDetailedUnitClass {
public:
	TTDetailedUnitClass() {
		this->Init(AOE_CONST_FUNC::TribeAINone);
	}

	AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitClassId;
	std::set<TTDetailedUnitDef*> allClassUnits; // All units for this class
	std::set<TTDetailedUnitDef*> allClassUnitsNoHero; // All units for this class excluding heroes
	std::set<TTDetailedUnitDef*> rootUnits; // Root units for this class (excluding unit upgrades like axeman), excluding heroes.
	bool includesSuperUnit; // True if unit class has at least one member who is a super unit
	int stoneAgeUnitCount; // Number of stone age units in this unit class, including unit upgrades
	int toolAgeUnitCount; // Number of tool age units in this unit class, including unit upgrades
	int bronzeAgeUnitCount; // Number of bronze age units in this unit class, including unit upgrades
	int ironAgeUnitCount; // Number of iron age units in this unit class, including unit upgrades
	long int trainLocationIfUnique; // Train location if all non-hero units from this class have the same one.

	// Returns the total number of units for this unit class
	int GetClassUnitCount() const { return this->allClassUnits.size(); }

	bool IsValidClass() const { return (this->unitClassId >= 0) && (this->unitClassId < AOE_CONST_FUNC::TribeAIGroupStandardCount); }

	// Reset underlying information to default values (and set unitClassId)
	void Init(AOE_CONST_FUNC::GLOBAL_UNIT_AI_TYPES unitClassId) {
		this->unitClassId = unitClassId;
		this->allClassUnits.clear();
		this->rootUnits.clear();
		this->includesSuperUnit = false;
		this->stoneAgeUnitCount = 0;
		this->toolAgeUnitCount = 0;
		this->bronzeAgeUnitCount = 0;
		this->ironAgeUnitCount = 0;
		this->trainLocationIfUnique = -1;
	}

	// Returns true if unit class is one of the "water" unit classes (including fish)
	bool IsWater() const {
		// Note: we could analyse units' terrain restrictions and guess it (hoping their are consistent within each class)
		return (this->unitClassId == AOE_CONST_FUNC::TribeAIGroupFishingBoat) ||
			(this->unitClassId == AOE_CONST_FUNC::TribeAIGroupTradeBoat) ||
			(this->unitClassId == AOE_CONST_FUNC::TribeAIGroupTransportBoat) ||
			(this->unitClassId == AOE_CONST_FUNC::TribeAIGroupWarBoat) ||
			(this->unitClassId == AOE_CONST_FUNC::TribeAIGroupSeaFish) ||
			(this->unitClassId == AOE_CONST_FUNC::TribeAIGroupShoreFish) ||
			(this->unitClassId == AOE_CONST_FUNC::TribeAIGroupUnknownFish);
	}


};


}
}
