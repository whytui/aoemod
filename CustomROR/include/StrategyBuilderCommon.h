#pragma once
#include <assert.h>
#include <AOE_empires_dat.h>

using namespace AOE_CONST_FUNC;

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

	// Returns true if the two classes are equals or very similar
	// Infantry - phalanx - war elephant
	// Cavalry (includes camel) - chariot
	// All archers together
	bool AreSimilarClasses(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2);

	// Updates provided bools to true/false according to the 2 classes. true=the classes use comme techs for this domain.
	void ShareCommonTechs(GLOBAL_UNIT_AI_TYPES class1, GLOBAL_UNIT_AI_TYPES class2, bool &attack, bool &defense, bool &hitPoints);

}
