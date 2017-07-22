#include <assert.h>
#include <set>
#include "TTDetailedResearchDef.h"
#include "TechTreeAnalyzer.h"

#pragma once

namespace ROCKNROR {
namespace STRATEGY {
;

// Tech tree handler for a given player "instance", with a specific civilization
class PlayerTechTree
{
public:
	PlayerTechTree() {
		this->playerId = -1;
		this->civId = -1;
		this->techTreeId = -1;
	}
	~PlayerTechTree() {}

	long int playerId;
	long int civId;
	long int techTreeId;
};


}
}
