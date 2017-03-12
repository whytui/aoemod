#pragma once

/*
This file provides API to allow connecting our methods to specific ROR classes virtual methods
An arbitrary example: overload ROR's STRUCT_UNIT_ACTIVITY_LION::ProcessNotify(notifyEvent, arg2) method with a RockNRor method whose signature is xxx(STRUCT_UNIT_ACTIVITY_LION*, notifyEvent, arg2)
Note: RockNRor method's return type/value will actually be returned to caller in ROR code.
*/

namespace ROCKNROR {

	// Generic API to overload ROR virtual methods.
	// Do not use directly (use the supplied macros).
	// This reads the RockNRor method to call (that overloads ROR's virtual method), removes it from parameters list in stack, and calls it.
	// arg1 = address of RockNRor method to call (removed from stack when it has been read)
	// arg2 = ROR method "this" object (ECX)
	// arg3+ = ROR method optional arguments. There can be 0 to n parameters.
	void CommonVirtualMethodAPI();
	

	// Here is an example of what the macros below do:
	// ExampleMethod is the "functional" method with the treatments we want to do. 
	// "ExampleMethod" parameters list must be the same as ROR's virtual method + "arg1" that corresponds to "this" object (ECX).
	// ExampleAPI is a technical method used to intercept ROR's virtual method call: this calls our method=>Example(...)
	/*void __stdcall ExampleMethod(AOE_STRUCTURES::STRUCT_UNIT_ACTIVITY *activity, unsigned long int arg1, unsigned long int arg2) {}
	__declspec(naked) void ExampleAPI() {
		_asm {
			MOV EAX, DS:[ESP]; // This should be return address
			MOV DS : [ESP], ECX; // Add (insert) arg1 = "this" (ECX) object from original call
			PUSH ExampleMethod; // First arg = target proc. This arg will be removed from calling parameters in CommonVirtualMethodAPI(...).
			PUSH EAX; // Re-write return address in stack (using next stack DWORD)
			JMP CommonVirtualMethodAPI;
		}
	}*/


	// Get the (RockNRor) "handler" method name to use as entry point for ROR's virtual method call.
#define VIRTUAL_METHOD_HANDLER_NAME(procName) _VMHAPI_##procName##_

	// Declare the virtual method handler to call procName instead of ROR's virtual method.
	// WARNING: procName MUST be declared using "stdcall" calling convention and must NOT be a member of a class.
	// Note: the method declared by this macro is named using VIRTUAL_METHOD_HANDLER_NAME macro.
	// Note: use this in .cpp file, not .h (to avoid link errors)
	// Do NOT use ";" in the macro as in _asm it comments out the rest of the line !
#define DECLARE_VIRTUAL_METHOD_HANDLER(procName) \
	__declspec(naked) void VIRTUAL_METHOD_HANDLER_NAME(procName)() { \
	_asm MOV EAX, DS:[ESP] _asm MOV DS:[ESP], ECX _asm PUSH procName _asm PUSH EAX _asm JMP CommonVirtualMethodAPI \
		}
	

}
