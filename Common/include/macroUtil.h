#pragma once

// This file contains technical macros only


// Please do not use. Internal usage.
#define MACRO_TO_STRING_INTERNAL(s) #s

// Transforms a value into "value"
#define MACRO_TO_STRING(s) MACRO_TO_STRING_INTERNAL(s)

// A quoted version of __LINE__. Will return "123" instead of 123.
#define _LINE_ MACRO_TO_STRING(__LINE__)

// A macro that expands into "file:line" quoted string.
#define __FILE_LINE__ __FILE__ "(" _LINE_ ")"

// Please do not use. Internal usage.
#define ___PRAGMA_BLDMSG_INTERNAL(strMsg) message(__FILE_LINE__ ": " strMsg)

// To be used a #pragma TODO("message")
#define TODO(msg) ___PRAGMA_BLDMSG_INTERNAL("TODO - " msg)

// To be used a #pragma TODO("message")
#define WARNING(msg) ___PRAGMA_BLDMSG_INTERNAL("WARNING - " msg)

// Used in other macros. Remove parentheses to use a macro parameter as is: typically, pass {a, b, c} as a parameter.
#define NOPAREN(...) __VA_ARGS__



// Mod name, to avoid being duplicated everywhere
#define MOD_NAME "RockNRor"


