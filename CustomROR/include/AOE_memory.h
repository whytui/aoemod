
#pragma once

#include <assert.h>


/*
// Allocates memory using ROR's method. Please use this, not malloc or new(). (not sure but it seems ROR fails freeing otherwise ?)
void *AOEAlloc(long int sizeInBytes);
// Allocates memory using ROR's method. Please use this, not malloc or new(). (not sure but it seems ROR fails freeing otherwise ?)
void *AOEAllocZeroMemory(long int number, long int elemSizeInBytes);
// Free memory using ROR's method
void AOEFree(void *ptr);

// Calls AOE's class destructor and also sets the variable (ptrObj) to NULL.
// ASSERTS that the provided pointer is non-NULL (so don't call this with NULL)
void CallAOEDestructor(unsigned long int **ptrObj);
*/



// Allocates memory using ROR's method
static void *AOEAlloc(long int sizeInBytes) {
	void *p;
	_asm {
		MOV ESI, 0x52601D // alloc
		PUSH sizeInBytes
		CALL ESI
		MOV p, EAX
		ADD ESP, 4
	}
	return p;
}

// Allocates memory using ROR's method. Please use this, not malloc or new(). (not sure but it seems ROR fails freeing otherwise ?)
static void *AOEAllocZeroMemory(long int number, long int elemSizeInBytes) {
	void *p;
	_asm {
		MOV ESI, 0x52607B // alloc + zeromemory
		PUSH elemSizeInBytes
		PUSH number
		CALL ESI
		MOV p, EAX
		ADD ESP, 8
	}
}

// Free memory using ROR's method
static void AOEFree(void *p) {
	_asm {
		MOV EAX, 0x525FC2
		PUSH p
		CALL EAX // free
		ADD ESP, 4
	}
}


// Calls AOE's class destructor and also sets the variable (ptrObj) to NULL.
// Does NOT assert that pointer is not NULL, but returns (and does nothing) in that case.
/*void CallAOEDestructor_noassert(unsigned long int **ptrObj) {
if (!ptrObj || (*ptrObj == NULL)) { return; }
CallAOEDestructor(ptrObj);
}*/

// Calls AOE's class destructor and also sets the variable (ptrObj) to NULL.
// ASSERTS that the provided pointer is non-NULL (so don't call this with NULL itself or a NULL pointer)
static void CallAOEDestructor(unsigned long int **ptrObj) {
	assert(ptrObj != NULL);
	assert(*ptrObj != NULL);
	_asm {
		PUSH ptrObj
		MOV EAX, 0x419950
		CALL EAX
	}
}

