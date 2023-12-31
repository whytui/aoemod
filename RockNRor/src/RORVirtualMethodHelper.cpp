#include "../include/RORVirtualMethodHelper.h"


namespace ROCKNROR {

	// Represents ESP register value when (last) ROR virtual method was handled by a hook.
	// Please only use this in Virtual method hooks !
	unsigned long int ESPValueOnLastVirtualMethodHandledCall = 0;

	// Generic API to overload ROR virtual methods.
	// Do not use directly (use the supplied macros).
	// This reads the RockNRor method to call (that overloads ROR's virtual method), removes it from parameters list in stack, and calls it.
	// arg1 = address of RockNRor method to call (removed from stack when it has been read). The method MUST be declared using "stdcall".
	// arg2 = ROR method "this" object (ECX)
	// arg3+ = ROR method optional arguments. There can be 0 to n parameters.
	__declspec(naked) void CommonVirtualMethodAPI() {
		_asm {
			POP EAX; // This should be return address
			POP EDX; // arg1 = (RockNRor) proc to call
			PUSH EAX; // Re-write return address. Arg1 has been removed from parameters list.
			MOV DS:[ESPValueOnLastVirtualMethodHandledCall], ESP; // Save ESP value in static variable
			JMP EDX; // jumps to correct (target) method. The stack is now correct for this method to find the expected arguments.
		}
	}


	// Get return address for virtual method hooks.
	// Please only use this from virtual method hooks !
	unsigned long int GetLastVirtualMethodCallReturnAddress() {
		unsigned long int *p = (unsigned long int *)ESPValueOnLastVirtualMethodHandledCall;
		if (p == nullptr) { return 0; }
		return *p;
	}

}
