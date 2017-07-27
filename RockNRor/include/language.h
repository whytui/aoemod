
#pragma once

#include <string.h>

// Uses ROR method to get a text from languagex.dll or language.dll, using ROR method. See also GetLanguageDllText.
// As RockNRor overloads this, the stringID is searched in custom strings in priority, then in languagex/language.dll
static void AOE_LoadStringLanguage(long int stringID, char *buffer, long int bufferSize) {
	assert(GetBuildVersion() == AOE_FILE_VERSION::AOE_VERSION_ROR1_0C);
	_asm {
		PUSH bufferSize;
		PUSH buffer;
		PUSH stringID;
		MOV EAX, 0x004FF530;
		CALL EAX;
	}
}

// Get a text from Custom strings, then languagex.dll or language.dll. If not found, uses defaultText.
// Uses ROR's method, which itself is overloaded by RockNRor (which is why "custom" strings are used in priority)
static void GetLanguageDllText(long int stringID, char *buffer, long int bufferSize, char *defaultText) {
	*buffer = 0;
	if (stringID < 100) { return; } // First valid string is 101
	AOE_LoadStringLanguage(stringID, buffer, bufferSize);
	if (*buffer == 0) {
		strcpy_s(buffer, bufferSize, defaultText);
	}
}

// Get a text from Custom strings, then languagex.dll or language.dll. If not found, returns "".
// Uses ROR's method, which itself is overloaded by RockNRor (which is why "custom" strings are used in priority)
static std::string GetLanguageDllText(long int stringID) {
	char buffer[100];
	buffer[0] = 0;
	GetLanguageDllText(stringID, buffer, sizeof(buffer), "");
	return std::string(buffer);
}
