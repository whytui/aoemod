#include "../include/ROR_API.h"


// Technical methods.
// Do NOT MODIFY THIS SECTION.
// See methods below (under the ---- line).


// Intermediate method to secure the stack and fix local ESP variable value.
// ptrCaller1 = return address of our custom API method in RoR code : do not use
// stackEAX   = "original" EAX value that has been pushed (between the 2 calls) before using it for calling this API.
// ptrCaller2 = return address of the caller (real code, not the API).
void _DispatchToCustomCode_internal(char *ptrCaller1, long int stackEAX, char *ptrCaller2) {
	// Saving return address and fixing ESP could also be done directly in API method. Let's do it here (no need for _asm here).
	tmp_REG_values.return_address = (long int)ptrCaller2;
	tmp_REG_values.EAX_val = stackEAX;
	long int EAX_stack_address = (long int) tmp_REG_values.ESP_val + 4;
	tmp_REG_values.ESP_val += 0x0C; // Set ESP value to the value it had just before the API call.
	tmp_REG_values.fixesForGameEXECompatibilityAreDone = false;
	// Now all context information is saved in tmp_REG_values

	if (global_conf.currentCalls >= MAX_AUTHORIZED_RECURSIVE_CALLS) {
		// This is a recursive call ! This is tolerated but limited to REG_values[] size.
		// Until now we only modified tmp_REG_values, REG_values is still unchanged. We can simply return (but ROR behaviour is uncertain).
#ifdef _DEBUG
		MessageBoxA(0, "WARNING: Recursion exceeded authorized limit.", "ROR API", MB_ICONWARNING);
#endif
		return;
	}
	// No recursion: overwrite REG_values and continue
	REG_values[global_conf.currentCalls] = tmp_REG_values;
	currentREG_values[global_conf.currentCalls] = &REG_values[global_conf.currentCalls]; // Use currentREG_values instead of REG_values[...] to avoid index errors !
	global_conf.currentCalls++;
	try {
		DispatchToCustomCode();
		tmp_REG_values = *currentREG_values[global_conf.currentCalls - 1]; // tmp_REG_values is used for restoring context, not REG_values
	}
	catch (...) {
		global_conf.currentCalls--;
		throw;
	}
	global_conf.currentCalls--;
	// This (over)writes the EAX value that was pushed in the stack by RoR process.
	long int myEAX = currentREG_values[global_conf.currentCalls]->EAX_val;
	long int *p;
	p = (long int *)EAX_stack_address;
	*p = myEAX;
}


// extern "C" is required to get correct method naming. In C++, the exported symbol may not be correct (and not found during game execution)
extern "C"
{
	// This API is the entry point for all custom (DLL) treatments to be added in ROR.
	// Requirement : the stack MUST have 2 consecutive "return address" = 2 consecutive calls => example stack = (EBP)... XXX_ret / YYY_ret (ESP)
	// At any location of AOE code: (EIP=XXX) CALL <YYY> (YYY is the custom code inserted in AOE EXE, that serves as an API to our DLL)
	// (location: YYY) CALL <thisDLL>.GenericCall()
	// The address we need to extract and pass to the dispatch method is XXX_ret (XXX_ret = address of instruction after the CALL <YYY>)
	// With a direct JMP to DispatchToCustomCode(), the stack is unchanged, so return address will be OK (YYY_ret) and method's parameter will be ESP+4=XXX_ret : perfect :)
	// WARNING: DO NOT modify the arguments in _DispatchToCustomCode_internal !!!
	__declspec(dllexport) __declspec(naked) void GenericCall() {
		// Backup register values so their values are available to custom functions (to determine current element, etc)
		// These MOV do not modify any register (MOV [memAddress], register) nor stack
		// Uses tmp_REG_values and not REG_values to make sure this is not a recursive call before we overwrite REG_values
		_asm {mov[tmp_REG_values.EBX_val], EBX}
		_asm {mov[tmp_REG_values.ECX_val], ECX}
		_asm {mov[tmp_REG_values.EDX_val], EDX}
		_asm {mov[tmp_REG_values.EDI_val], EDI}
		_asm {mov[tmp_REG_values.ESI_val], ESI}
		_asm {mov[tmp_REG_values.EBP_val], EBP}
		_asm {mov[tmp_REG_values.ESP_val], ESP}
		_asm {call[_DispatchToCustomCode_internal]}; // This will manage EAX value (get / set new value) because it is stored in stack
		// Restore register values (not for stack registers !)
		// This way the caller will find his data unchanged :)
		_asm {mov EBX, [tmp_REG_values.EBX_val]}
		_asm {mov ECX, [tmp_REG_values.ECX_val]}
		_asm {mov EDX, [tmp_REG_values.EDX_val]}
		_asm {mov EDI, [tmp_REG_values.EDI_val]}
		_asm {mov ESI, [tmp_REG_values.ESI_val]}
		_asm {mov EBP, [tmp_REG_values.EBP_val]}
		// No instruction but RETN after we restore the registers. Preserve register values.
		_asm {retn}
	};
}



// ---------------------------------------------------------------------------------------------
// You can add/modify code under this separation :)
// ---------------------------------------------------------------------------------------------



// Run the proper treatment according to calling context information stored in currentREG_values struct.
// In particular, currentREG_values->return_address let you know what portion of code just called the API.
// WARNING: if you modify currentREG_values values, this will impact the original process. DO NOT do it unless you have a good reason.
void DispatchToCustomCode() {
	if ((REG_values[global_conf.currentCalls - 1].return_address < AOE_OFFSETS_10C::ADDR_EXE_MIN) ||
		(REG_values[global_conf.currentCalls - 1].return_address > AOE_OFFSETS_10C::ADDR_EXE_MAX)) {
		return; // invalid address. Bad calling information - bad call stack.
	}

	switch (currentREG_values[global_conf.currentCalls - 1]->return_address) {
	case AOE_OFFSETS_10C::AOE_ROR_API_FIRST_CALL_RETURN_ADDRESS:
		MakeInits();
		break;
	default:
		break;
	}

	for (int index = 0; index < global_conf.modulesCount; index++) {
		if (global_conf.hModules[index]) {
			PPROC dll_method;
			dll_method = (PPROC)GetProcAddress(global_conf.hModules[index], API_PROC_NAME);
			if (dll_method) {
				// Call DLL method, if found. Provide it the REG_values so it can read/write the context from original call.
				dll_method(currentREG_values[global_conf.currentCalls - 1]);
			}
		}
	}
}


// This method is called only once at game startup.
// Do init tasks here, like loading DLLs
void MakeInits() {
	global_conf.LoadModules();
}

