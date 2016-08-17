
#pragma once

#include <string.h>

// Get a text from languagex.dll or language.dll, using ROR method. See also GetLanguageDllText.
static void AOE_LoadStringLanguage(long int stringID, char *buffer, long int bufferSize) {
	_asm {
		PUSH bufferSize
		PUSH buffer
		PUSH stringID
		MOV EAX, 0x004FF530
		CALL EAX
	}
}

// Get a text from languagex.dll or language.dll. If not found, uses defaultText.
static void GetLanguageDllText(long int stringID, char *buffer, long int bufferSize, char *defaultText) {
	*buffer = 0;
	if (stringID < 100) { return; } // First valid string is 101
	AOE_LoadStringLanguage(stringID, buffer, bufferSize);
	if (*buffer == 0) {
		strcpy_s(buffer, bufferSize, defaultText);
	}
}

// Get a text from languagex.dll or language.dll. If not found, returns "".
static std::string GetLanguageDllText(long int stringID) {
	char buffer[100];
	buffer[0] = 0;
	GetLanguageDllText(stringID, buffer, sizeof(buffer), "");
	return std::string(buffer);
}
