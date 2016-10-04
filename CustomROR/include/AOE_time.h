
#pragma once

#include <assert.h>
#include <AOE_offsets.h>

namespace AOE_METHODS {
	// Does a timeGetTime() call using AOE's linked method.
	static long int TimeGetTime() {
		assert(AOE_OFFSETS::AOE_LNK_TIMEGETTIME != NULL);
		if (AOE_OFFSETS::AOE_LNK_TIMEGETTIME == NULL) { return 0; } // For unsupported versions
		unsigned long int addr = *AOE_OFFSETS::AOE_LNK_TIMEGETTIME;
		long int result;
		_asm {
			CALL DS:[addr];
			MOV result, EAX
		}
		return result;
	}


}
