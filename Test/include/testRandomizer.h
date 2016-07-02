#include <assert.h>
#include <tchar.h>
#include "../../CustomROR/include/randomizer.h"

#pragma once


// Entry point to test customROR randomizer
void test_randomizer();

#pragma once
#include <string>
#include <assert.h>

// Do not call this directly. Please use ror_api_assert macro.
static void _assertShowValues(wchar_t *expression, const wchar_t *file, unsigned int line, long int expected, long int actual) {
#ifdef _DEBUG
	std::wstring msg = expression;
	wchar_t values[512];

	swprintf_s(values, _T("\nAssert failed - expected=%ld, actual=%ld"),
		expected, actual
		);

	msg += values;
	_wassert(msg.c_str(), file, line);
#endif
}


// Call this macro to run an assertion. It will only have an effect in DEBUG compilation mode.
// If expression is false, the assert debug message will also display _REG_values register hexadecimal values (+file/line number).
// You can still use assert(...) if you don't have a _REG_values object.
#define assertShowValues(_Expression, expected, actual) (void)( \
	(!!(_Expression)) || ( \
_assertShowValues(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__, expected, actual), 0 \
) \
	)

