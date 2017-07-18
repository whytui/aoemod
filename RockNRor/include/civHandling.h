#include <assert.h>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_global.h>
#include <AOE_struct_civilization_def.h>
#include "RockNRorConfig.h"
#include "RockNRorInfo.h"
#include "AOE_memory.h"
#include "traceMessage.h"
#include "mainStructuresHandling.h"
#include "unitDefHandling.h"
#include "techDefHandling.h"

#pragma once

/*
This file contains primitives to manipulate civilizations.
*/

namespace AOE_METHODS {
	namespace CIVILIZATION {

		// Call destructor on a STRUCT_CIVILIZATION_DEF object
		void CivilizationDestroy(STRUCT_CIVILIZATION_DEF *civDef);

	}
}


namespace AOE_STRUCTURES {
	
	// Re-alloc array of civilizations in Global structure to add some civs.
	// newCivCount must be > initial civ count and < 255
	// Requirement: Global structure must have been initialized and DAT file must have been read
	// Added civilization pointers will be NULL (this does not create any structure)
	// This updates global->civCount
	bool ReallocCivArray(long int newCivCount);


	// Create a new civilization definition object, copy of source object.
	// Returns NULL on failure
	STRUCT_CIVILIZATION_DEF *CreateCivCopy(STRUCT_CIVILIZATION_DEF *source);

}
