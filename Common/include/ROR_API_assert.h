#pragma once
#include <string>
#include <assert.h>

// Do not call this directly. Please use ror_api_assert macro.
static void _ror_api_assert(REG_BACKUP *REG_values, wchar_t *expression, const wchar_t *file, unsigned int line) {
#ifdef _DEBUG
	std::wstring msg = expression;
	wchar_t values[512];
	long int *stack = (long int *)REG_values->ESP_val;

	swprintf_s(values, _T("\nPlease screenshot this. Ignore to continue.\n")
		_T("REG_v\nEAX=%08X\nEBX=%08X\nECX=%08X\nEDX=%08X\nESI=%08X\nEDI=%08X\nESP=%08X\nEBP=%08X\n")
		_T("ROR_Stack\n+00=%08X\n+04=%08X\n+08=%08X\n+0C=%08X"),
		REG_values->EAX_val,
		REG_values->EBX_val,
		REG_values->ECX_val,
		REG_values->EDX_val,
		REG_values->ESI_val,
		REG_values->EDI_val,
		REG_values->ESP_val,
		REG_values->EBP_val,
		*(stack),
		*(stack+1),
		*(stack+2),
		*(stack+3)
		);

	msg += values;
	_wassert(msg.c_str(), file, line);
#endif
}


// Call this macro to run an assertion. It will only have an effect in DEBUG compilation mode.
// If expression is false, the assert debug message will also display _REG_values register hexadecimal values (+file/line number).
// You can still use assert(...) if you don't have a _REG_values object.
#define ror_api_assert(_REG_values, _Expression) (void)( \
	(!!(_Expression)) || (_ror_api_assert(_REG_values, _CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__), 0) \
	)
