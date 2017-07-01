#pragma once

#include <map>
#include <string>
#include <mystrings.h>
#include <AOE_struct_unit_activity.h>
#include "EXESelfEditor.h"
#include "crDebugOutputPrimitives.h"
#include "RORVirtualMethodHelper.h"
#include "GameEventHandler.h"
#include "RockNRorInfo.h" // to measure execution times
#include "interruption.h"


namespace ROCKNROR {
;
namespace VIRTUAL_METHOD_HOOKS {


	// Patches ROR process (.rdata memory) to connect RockNRor method (rnrorMethodName) to ROR class's method,
	// ROR class identified by "baseChecksum" and ROR class' method identified by "methodOffset" (should be a multiple of 4 !)
	// varPreviousAddress is a variable name that will be updated with previous (original) call address (unsigned long int)
#define INSTALL_VIRTUAL_METHOD_PATCH(baseChecksum, methodOffset, rnrorMethodName, varPreviousAddress) \
	assert(methodOffset % 4 == 0); \
	varPreviousAddress = *(unsigned long int*)(baseChecksum + methodOffset); \
	*(unsigned long int*)(baseChecksum + methodOffset) = (unsigned long)VIRTUAL_METHOD_HANDLER_NAME(rnrorMethodName);


	/*
	SECTION: declare specific hook methods. Always declare as "__stdcall".
	Hook methods must return void or a 4-bytes type (DWORD)
	*/


	// Return value is an unknown enum. 2=ok, processed. (unitAI: EDX+0xCC call).
	long int __stdcall ActivityProcessNotify(STRUCT_UNIT_ACTIVITY *activity, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT *notifyEvent, unsigned long int arg2);

	// Returns void. This hook handles player's notifications (EDX+0xE8 call).
	void __stdcall PlayerProcessNotify(STRUCT_PLAYER *player, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT notifyEvent);

	// unit.addPositionToTargetPosArray(pDword_posYXZ, arg2) for ALL unit classes (base+children). Method offset=+1BC.
	// Returns 1 on success, 0 on failure
	long int __stdcall UnitAddPositionToTargetPosArray(STRUCT_UNIT_BASE *unit, STRUCT_UNIT_TARGET_POS *targetPos, long int arg2);



	/*
	SECTION: declare initialization method that installs patches in ROR executable
	*/

	// Patches ROR process (.rdata section) to connect overloaded virtual methods
	// Don't forget to declare RockNRor entry point methods as "__stdcall"
	bool InstallVirtualMethodsPatches();

}
}
