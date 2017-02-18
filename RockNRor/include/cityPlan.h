#pragma once

#include <assert.h>
#include <string>
#include <AOE_struct_main_ai.h>
#include <AOE_struct_units.h>
#include "RockNRorInfo.h"
#include "crDebugOutputPrimitives.h"
#include "BitmapExporter.h"
#include "AOEPrimitives_units.h"
#include "unitHandling.h"

namespace CITY_PLAN {

	// Computes (existing) building influence zone for farm placement map like values computation.
	// Updates existingBldInfluenceZone with the influence distance we want to use for provided building (positions near building will be preferred)
	// Updates skipThisBuilding to true if the building must be ignored for farm position computation. If true, existingBldInfluenceZone returned value should be ignored.
	void FixCityPlanFarmPlacement(AOE_STRUCTURES::STRUCT_UNIT_BASE *existingBuilding, long int &existingBldInfluenceZone, bool &skipThisBuilding);

	// Updates temp map like data for choosing a new building location according to existing buildings
	void ManageCityPlanOtherBuildingsImpact(AOE_STRUCTURES::STRUCT_INF_AI *infAI, AOE_STRUCTURES::STRUCT_STRATEGY_ELEMENT *stratElem, AOE_STRUCTURES::STRUCT_POSITION_INFO *TCPosition);
	
}

