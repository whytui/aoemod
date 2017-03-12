#pragma once

#include <map>
#include <string>
#include <mystrings.h>
#include <AOE_struct_unit_activity.h>
#include "EXESelfEditor.h"
#include "crDebugOutputPrimitives.h"
#include "RORVirtualMethodHelper.h"
#include "RockNRorCommand.h"


namespace ROCKNROR {
;
namespace OVERLOADED_VIRTUAL_METHODS {

	// Patches ROR process (.rdata memory) to connect RockNRor method (rnrorMethodName) to ROR class's method,
	// ROR class identified by "baseChecksum" and ROR class' method identified by "methodOffset" (should be a multiple of 4 !)
	// varPreviousAddress is a variable name that will be updated with previous address (unsigned long int)
#define INSTALL_VIRTUAL_METHOD_PATCH(baseChecksum, methodOffset, rnrorMethodName, varPreviousAddress) \
	assert(methodOffset % 4 == 0); \
	varPreviousAddress = *(unsigned long int*)(baseChecksum + methodOffset); \
	*(unsigned long int*)(baseChecksum + methodOffset) = (unsigned long)VIRTUAL_METHOD_HANDLER_NAME(rnrorMethodName);



	long int __stdcall TESTFN1(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity, unsigned long int arg1, unsigned long int arg2);


	// Patches ROR process (.rdata section) to connect overloaded virtual methods
	bool InstallVirtualMethodsPatches();

}
}
