#pragma once

#include <stdio.h>
#include <Windows.h>
#include <AOE_offsets.h>


// Allow/disallow writing in .rdata section of AOE/ROR process.
// Returns true if successful
bool EnableWritingInRData(bool enable);


// Returns true if successful.
bool WriteInMyMemory(unsigned long int address, char *buffer, int size);
// Returns true if successful.
bool WriteInMyMemory(unsigned long int address, unsigned char *buffer, int size);

