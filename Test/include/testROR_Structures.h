#include <assert.h>
#include <AOE_structures.h>
#include <AOE_struct_game_settings.h>
#include <AOE_struct_strategy_element.h>
#include <AOE_struct_main_ai.h>
#include <string>
#include <Windows.h>
#include "../include/testRandomizer.h"

#pragma once

// Entry point to test ROR structure offsets.
static bool test_ror_structures();

static bool test_unit_structs();

// Obsolete tech method.
static bool cmp_obj_with_member_ptr(void *objPtr, int offset, void *memberPtr);

// Do not call directly. Please use trs_assert macro.
static void _assert_member_offset(void *objPtr, int offset, void *memberPtr, wchar_t *objName, const wchar_t *file, unsigned int line);


// Use it to make an assertion on a structure member's position.
// To control that myclass.myfield's offset in 0x24, then call trs_assert(&obj_myclass, 0x24, &obj_myclass.myfield);
#define trs_assert(objPtr, offset, memberPtr) (void)( \
	(_assert_member_offset(objPtr, offset, memberPtr, _CRT_WIDE(#memberPtr), _CRT_WIDE(__FILE__), __LINE__), 0) \
)
