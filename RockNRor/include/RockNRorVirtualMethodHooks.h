#pragma once

#include <map>
#include <string>
#include <mystrings.h>
#include <AOE_struct_unit_activity.h>
#include <UI\AOE_struct_ui_welcome_main_screen.h>
#include <UI\AOE_struct_ui_screen_base.h>
#include "EXESelfEditor.h"
#include "crDebugOutputPrimitives.h"
#include "RORVirtualMethodHelper.h"
#include "GameEventHandler.h"
#include "RockNRorInfo.h" // to measure execution times
#include "interruption.h"
//#include "RockNRorMainInterface.h"
#include "RnrScreenBase.h"
#include "RockNRorSettingsScreen.h"


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

	// Return value = 1 on success, 0 if failed
	long int __stdcall ActivityMoveToMoveAwayFrom(STRUCT_UNIT_ACTIVITY *activity, float posY, float posX, unsigned long int arg3, unsigned long int arg4, long int force);

	// Returns void. This hook handles player's notifications (EDX+0xE8 call).
	void __stdcall PlayerProcessNotify(STRUCT_PLAYER *player, STRUCT_UNIT_ACTIVITY_NOTIFY_EVENT notifyEvent);

	// unit.addPositionToTargetPosArray(pDword_posYXZ, arg2) for ALL unit classes (base+children). Method offset=+1BC.
	// Returns 1 on success, 0 on failure
	long int __stdcall UnitAddPositionToTargetPosArray(STRUCT_UNIT_BASE *unit, STRUCT_UNIT_TARGET_POS *targetPos, long int arg2);

	// Transform a unit from its current "unit definition" to provided "unit definition" (EDX+0x54 call).
	// Typically used to switch tasks (villagers) : in such case, newUnitDef is player's unitDefinition for the target unitDefId (repair_man, etc)
	// Also used to assign a dedicated "unit definition" in conversion process: unit will have its own "unitDefinition" (with its own specs)
	// This is only allowed if unit.status <= 2 (ready) because BASE method fails if status>2 (unit->unitDefinition is NOT updated).
	void __stdcall UnitTransform(STRUCT_UNIT_BASE *unit, STRUCT_UNITDEF_BASE *newUnitDef);

	// OnKeyDown event handler for menus, excluding in-game and scenario editor menus
	// Returns 1 if event has been handled and must not be transferred to parent object.
	// Warning: repeatCount is a word (or 2 words?)
	long int __stdcall UIMenuOnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT);

	// OnKeyDown event handler for in-game and scenario editor screens, excluding dialogs
	// Returns 1 if event has been handled and must not be transferred to parent object.
	// Warning: repeatCount is a word (or 2 words?)
	long int __stdcall UIGameEditorOnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT);

	// OnKeyDown event handler for dialogs (excluding generic dialog class)
	// Returns 1 if event has been handled and must not be transferred to parent object.
	// Warning: repeatCount is a word (or 2 words?)
	long int __stdcall UIDialogOnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT);

	// OnKeyDown event handler for generic classes (easyPanel, dialogPanel)
	// Returns 1 if event has been handled and must not be transferred to parent object.
	// Warning: repeatCount is a word (or 2 words?)
	// Can be used for custom screens and ALSO for unhandled events from child classes
	long int __stdcall UIGenericOnKeyDown(STRUCT_ANY_UI *uiObj, long int keyCode, long int repeatCount, long int ALT, long int CTRL, long int SHIFT);



	/*
	SECTION: declare initialization method that installs patches in ROR executable
	*/

	// Patches ROR process (.rdata section) to connect overloaded virtual methods
	// Don't forget to declare RockNRor entry point methods as "__stdcall"
	bool InstallVirtualMethodsPatches();

}
}
