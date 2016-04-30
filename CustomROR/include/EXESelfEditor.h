#pragma once

#include <stdio.h>
#include <Windows.h>
#include <AOE_offsets_10c.h>


// Returns true if successful.
bool WriteInMyMemory(unsigned long int address, char *buffer, int size);
// Returns true if successful.
bool WriteInMyMemory(unsigned long int address, unsigned char *buffer, int size);

