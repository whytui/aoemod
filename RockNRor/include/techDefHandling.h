#include <assert.h>
#include <AOE_offsets.h>
#include <ROR_global_variables.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_game_global.h>
#include <AOE_struct_tech_def.h>
#include "AOE_memory.h"
#include "traceMessage.h"
#include "mainStructuresHandling.h"

#pragma once

/*
This file contains primitives to manipulate technology definitions.
*/



namespace AOE_STRUCTURES {
	
	// Re-alloc array of technology definitions in Global structure to add some techs.
	// Requirement: Global structure must have been initialized and DAT file must have been read
	// Added technology definition pointers will be NULL (this does not create any structure)
	// This updates global->technologiesInfo->{ptrTechDefArray and technologyCount}
	bool ReallocTechDefArray(long int newTechDefCount);


	// Removes all effects for a technology definition
	bool CleanEffectsForTechDef(STRUCT_TECH_DEF *techDef);


}
