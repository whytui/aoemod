#pragma once
#include <gameVersion.h>
#include <AOE_offsets.h>
#include "ROR_API_pub.h"
#include "../include/ConfigManager.h"

// Number of ROR_API calls that can be done "recursively" (ROR_API may call an original method that calls ROR_API again...)
// Must be > 0
#define MAX_AUTHORIZED_RECURSIVE_CALLS 4

// Register values of the context from which the call comes
// WARNING: all changes in these values will impact the original process !
REG_BACKUP REG_values[MAX_AUTHORIZED_RECURSIVE_CALLS];
REG_BACKUP tmp_REG_values;
REG_BACKUP *currentREG_values[MAX_AUTHORIZED_RECURSIVE_CALLS];

void _DispatchToCustomCode_internal(char *ptrCaller1, long int stackEAX, char *ptrCaller2);
void MakeInits();
void DispatchToCustomCode();

static_assert(MAX_AUTHORIZED_RECURSIVE_CALLS > 0, "MAX_AUTHORIZED_RECURSIVE_CALLS must be positive");
