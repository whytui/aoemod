#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#pragma once

#define CONF_FILE_NAME "ROR_API.conf"


// A simple list of strings (char *).
// Be careful,  x->string may be NULL (for example, 1st element of list)
class StringList {
public:
	StringList() { next = NULL;  string = NULL; };
	StringList(char *src_string, unsigned int string_size) {
		this->next = NULL;
		this->string = (char *)malloc((string_size * sizeof(char)) + 1);
		memcpy(this->string, src_string, string_size);
		this->string[string_size] = 0; // just in case \0 is missing in src buffer
	};
	~StringList() {
		if (this->string) { free(this->string); }
		this->string = NULL;
		if (this->next) { delete this->next; }
		this->next = NULL;
	};
	int AddAfter(char *src_string, unsigned int string_size) {
		if (this->next) { return 1; }
		this->next = new StringList(src_string, string_size);
		return 0;
	}
	StringList *next;
	char *string;
};


// A very minimalist class to manage configuration (read from a flat text file).
class ConfigManager {
public:
	ConfigManager();
	~ConfigManager();
	StringList *DLL_names_first;
	StringList *DLL_names_current;
	int modulesCount;
	HMODULE *hModules;
	int currentCalls; // Counter to handle recurive calls, and keep track of stacked "Reg values" independently.
	bool verboseDebug; // If true, show more debugging information through message boxes.

	void Init();
	int ReadConfigFromFile();
	void LoadModules();
};


// Global variable (need to be the same for DLL Entry point and calls)
__declspec(dllexport) extern ConfigManager global_conf;
