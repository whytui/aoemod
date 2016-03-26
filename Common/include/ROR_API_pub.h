/*
 This file contains public declarations you need to compile custom DLLs that are compatible with ROR_API.dll

 To write your own compatible DLL, declare a method with following signature:

 extern "C" void __declspec(dllexport) DispatchToCustomCode(REG_BACKUP *REG_values);

 This method will be called each time that ROR_API will be called from RoR executable.
 Use the provided REG_BACKUP structure to get all context information you need to determine what you should do.

 Do NOT use recursion with this API (a custom DLL must not CALL this API or game code that calls this API).
 If a recursive call is detected, the API will return without doing any treatment (the first call will be managed normally).
*/

#pragma once

#include <AOE_offsets_10c.h>

// This structure describes the register context when ROR_API was called from game code.
typedef struct{
	long int return_address; // This is NOT the CALL instruction's address, but the next one's ;). Do not update directly. Use ChangeReturnAddress(...).
	long int EAX_val;
	long int EBX_val;
	long int ECX_val;
	long int EDX_val;
	long int EDI_val;
	long int ESI_val;
	long int EBP_val;
	long int ESP_val; // ESP value BEFORE the call to ROR_API (the value it had in game EXE code)
	// Set this flag to true when you made the necessary treatments to replace overwritten code in original EXE code.
	// So that other DLLs can know if it has already been done.
	// This is crucial to avoid doing twice original "replaced" code
	// Those treatments depend on how the call to ROR_API has been done (sometimes it uses unused code (NOPs), 
	//  sometimes it overwrites existing code, so mising code has to be done in a ROR_API plug-in instead (or the game will crash).
	// It can consists in : change some "REG_BACKUP" fileds value, make an asm call to a ROR method, or even change return address according to conditions. , etc
	// If you're not sure, don't set this.
	bool fixesForGameEXECompatibilityAreDone;
} REG_BACKUP;


// Procedure signature for custom DLL API entry points
typedef void(*PPROC)(REG_BACKUP*);
// Procedure name for custom DLL API entry points
static char API_PROC_NAME[] = "DispatchToCustomCode";


// Returns true if the executable code at provided address is a call to ROR_API
// Useful to check which ROR_API options have been enabled in empiresX.exe
// Warning: gameAddress is the (supposed) return address of ROR_API call, not the address of customized code
static bool IsRORAPICallEnabled(unsigned long int gameReturnAddress) {
	if ((gameReturnAddress < AOE_OFFSETS_10C::ADDR_EXE_MIN + 5) || (gameReturnAddress > AOE_OFFSETS_10C::ADDR_EXE_MAX)) {
		return false; // invalid address
	}
	// Now accessing gameAddress-pointed values is secure as we're sure it is in game "TEXT" memory zone.

	// Test "CALL" instruction (0xE8 in asm)
	if (*(char *)(gameReturnAddress - 5) != (char)0xE8) { return false; }
	// Use signed values (callOffset may be negative, it is a relative offset between calling address=EIP and called address).
	long int signedGameAddress = (long int)gameReturnAddress;
	long int callOffset = *(unsigned long *)(gameReturnAddress - 4); // We checked that gameAddress is in correct range, so it is a valid pointer value.
	return signedGameAddress + callOffset == AOE_OFFSETS_10C::ROR_API_GAME_PROC_ADDR;
}

// Modifies return address.
static void ChangeReturnAddress(REG_BACKUP *REG_values, unsigned long int newReturnAddress) {
	long int myESP = REG_values->ESP_val;
	myESP -= 4; // return address location in stack
	unsigned long int *addr = (unsigned long int *) myESP;
	*addr = newReturnAddress; // Change return address in (original code's) stack
}

// Useful to get a value from stack (sort of MOV ..., DS:[ESP+offset])
static unsigned long int GetIntValueFromRORStack(REG_BACKUP *REG_values, long int offset) {
	unsigned long int *addr = (unsigned long int *) (REG_values->ESP_val + offset);
	return *addr;
}
// Useful to get a value from stack (sort of MOV ..., DS:[ESP+offset])
static float GetFloatValueFromRORStack(REG_BACKUP *REG_values, long int offset) {
	float *addr = (float *) (REG_values->ESP_val + offset);
	return *addr;
}
